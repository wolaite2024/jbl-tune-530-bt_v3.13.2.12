
#include "app_ble_rand_addr_mgr.h"
#include "app_cfg.h"
#include "gap_le.h"
#include "trace.h"
#include "os_queue.h"
#include "app_relay.h"
#include <string.h>
#include <stdlib.h>


typedef enum
{
    SYNC_STATIC_ADDR = 0,
    REMOTE_MSG_MAX = 1,
} RELAY_MSG;


typedef struct CB_ELEM CB_ELEM;


typedef struct CB_ELEM
{
    CB_ELEM *next;
    RANDOM_ADDR_MGR_CB cb;
} CB_ELEM;


struct
{
    T_OS_QUEUE cb_queue;
} addr_mgr
=
{
    .cb_queue = {0}
};


static void app_ble_rand_addr_go_through_cbs(RANDOM_ADDR_MGR_EVT_DATA *data)
{
    CB_ELEM *tmp = (CB_ELEM *)addr_mgr.cb_queue.p_first;

    for (; tmp != NULL; tmp = tmp->next)
    {
        tmp->cb(data);
    }
}

#if F_APP_ERWS_SUPPORT
static void app_ble_rand_addr_send_static_addr(uint8_t *random_addr)
{

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BLE_RAND_ADDR, SYNC_STATIC_ADDR, \
                                        random_addr, 6);


    APP_PRINT_TRACE1("app_ble_rand_addr_mgr send_static_addr: random_addr %s",
                     TRACE_BDADDR(random_addr));
}
#endif

void app_ble_rand_addr_handle_remote_conn_cmpl(void)
{
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    //first time to gen rws random addr
    if (memcmp(app_cfg_nv.le_rws_random_addr, (uint8_t[6]) {0}, 6) == 0)
    {
        memcpy(app_cfg_nv.le_rws_random_addr, app_cfg_nv.le_single_random_addr, 6);
        app_cfg_store(app_cfg_nv.le_rws_random_addr, 6);
        APP_PRINT_TRACE1("app_ble_rand_addr_handle_remote_conn_cmpl: set new rws random addr %s",
                         TRACE_BDADDR(app_cfg_nv.le_rws_random_addr));

        RANDOM_ADDR_MGR_EVT_DATA evt_data = {.evt = RANDOM_ADDR_TWS_ADDR_UPD, .upd_tws_addr.addr = {0}};
        memcpy(evt_data.upd_tws_addr.addr, app_cfg_nv.le_rws_random_addr, 6);
        app_ble_rand_addr_go_through_cbs(&evt_data);
#if F_APP_ERWS_SUPPORT
        app_ble_rand_addr_send_static_addr(app_cfg_nv.le_rws_random_addr);
#endif
    }
}


void app_ble_rand_addr_handle_role_decieded(void)
{
    if (app_cfg_nv.first_engaged == 0) //means that first_engage has not finished.
    {
        memset(app_cfg_nv.le_rws_random_addr, 0, 6);
        app_cfg_store(app_cfg_nv.le_rws_random_addr, 6);
        APP_PRINT_TRACE0("app_ble_rand_addr_handle_role_decieded: clear rws random addr");
    }
}


void app_ble_rand_addr_get(uint8_t random_addr[6])
{
    if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    && (memcmp(app_cfg_nv.le_rws_random_addr, (uint8_t[6]) {0}, 6) != 0))
    {
        memcpy(random_addr, app_cfg_nv.le_rws_random_addr, 6);
    }
    else
    {
        memcpy(random_addr, app_cfg_nv.le_single_random_addr, 6);
    }
}

#if F_APP_ERWS_SUPPORT
static void app_ble_rand_addr_set(uint8_t random_addr[6])
{
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        memcpy(app_cfg_nv.le_rws_random_addr, random_addr, 6);
        app_cfg_store(app_cfg_nv.le_rws_random_addr, 6);
    }
    else
    {
        memcpy(app_cfg_nv.le_single_random_addr, random_addr, 6);
        app_cfg_store(app_cfg_nv.le_single_random_addr, 6);
    }
}


static void app_ble_rand_addr_relay_parse(RELAY_MSG msg_type, uint8_t *buf, uint16_t len,
                                          T_REMOTE_RELAY_STATUS status)
{
    uint8_t rws_random_addr[6] = {0};

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    if (msg_type != SYNC_STATIC_ADDR)
    {
        return;
    }

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    memcpy(rws_random_addr, buf, 6);

    APP_PRINT_TRACE1("app_ble_rand_addr_mgr relay_parse: rws_random_addr %s",
                     TRACE_BDADDR(rws_random_addr));
    app_ble_rand_addr_set(rws_random_addr);

    RANDOM_ADDR_MGR_EVT_DATA evt_data = {.evt = RANDOM_ADDR_TWS_ADDR_RECVED, .recv_tws_addr.addr = {0}};
    memcpy(evt_data.recv_tws_addr.addr, app_cfg_nv.le_rws_random_addr, 6);
    app_ble_rand_addr_go_through_cbs(&evt_data);
}
#endif


void app_ble_rand_addr_cb_reg(RANDOM_ADDR_MGR_CB cb)
{
    CB_ELEM *p_ele = NULL;
    p_ele = calloc(1, sizeof(*p_ele));
    if (p_ele)
    {
        p_ele->cb = cb;
        os_queue_in(&addr_mgr.cb_queue, p_ele);
    }
}



void app_ble_rand_addr_init(void)
{
    if ((app_cfg_nv.le_single_random_addr[5] & 0xC0) != 0xC0)
    {
        le_gen_rand_addr(GAP_RAND_ADDR_STATIC, app_cfg_nv.le_single_random_addr);
        app_cfg_store(&app_cfg_nv.le_single_random_addr, 6);
        APP_PRINT_TRACE1("app_ble_rand_addr_init: le_single_random_addr %s",
                         TRACE_BDADDR(app_cfg_nv.le_single_random_addr));
    }

    os_queue_init(&addr_mgr.cb_queue);

#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_ble_rand_addr_relay_parse,
                             APP_MODULE_TYPE_BLE_RAND_ADDR, REMOTE_MSG_MAX);
#endif
}


