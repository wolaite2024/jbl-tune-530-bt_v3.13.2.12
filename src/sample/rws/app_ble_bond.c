#if F_APP_BLE_BOND_SYNC_SUPPORT
#include <stdlib.h>
#include "app_ble_bond.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "gap_storage_le.h"
#include <trace.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>


#define MAX_CBACK_NUM   (10)
#define BLE_BOND_MTU    (APP_RELAY_MTU)


typedef enum
{
    SYNC_IDLE               = 0,
    SYNC_PRIM_SEND_INFO     = 1,
    SYNC_PRIM_RECV_INFO     = 2,
    SYNC_SEC_RECV_INFO      = 3,
    SYNC_SEC_SEND_INFO      = 4,
} SYNC_STATE;


typedef struct
{
    SYNC_STATE  state;
    uint32_t    total_bond_num;
    uint32_t    cur_bond_num;
} SYNC_MGR;


/**************************
Gap_storage_le must follow BB2
sync to PRO3
***************************/
struct
{
    SYNC_MGR                    sync_mgr;
} app_le_bond_mgr;


typedef struct
{
    uint32_t bond_num_per_pkt;
    uint32_t batch_num;
    uint32_t remains;
} BATCH_SEND_MGR;


typedef enum
{
    APP_LE_BOND_DELETE      = 0x00,
    APP_LE_BOND_ADD         = 0x01,
    APP_LE_BOND_MERGE       = 0x02,
    APP_LE_BOND_EVT_MAX     = 0x03,
} APP_LE_BOND_REMOTE_MSG;


typedef struct
{
    uint32_t bond_num;
    uint32_t total_bond_num;
    T_LE_DEV_INFO dev_infos[];
} APP_LE_BOND_REMOTE_INFO;

static void app_ble_bond_sync_mgr_default(void)
{
    SYNC_MGR *p_mgr = &app_le_bond_mgr.sync_mgr;

    p_mgr->state = SYNC_IDLE;
    p_mgr->total_bond_num = 0;
    p_mgr->cur_bond_num = 0;
}


void app_ble_bond_sync_recovery(void)
{
    app_ble_bond_sync_mgr_default();
}

#if F_APP_ERWS_SUPPORT
inline static void app_ble_bond_send_info(APP_LE_BOND_REMOTE_MSG  msg_id,
                                          APP_LE_BOND_REMOTE_INFO *p_info,
                                          uint32_t len)
{
    if (len > BLE_BOND_MTU)
    {
        APP_PRINT_ERROR1("app_ble_bond send_info: len %d is over MTU", len);

    }

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BLE_BOND, msg_id, (uint8_t *)p_info, len);
}
#endif

inline static void app_ble_bond_batch_send_mgr_init(BATCH_SEND_MGR *p_send_mgr,
                                                    uint32_t total_bond_num)
{
    p_send_mgr->bond_num_per_pkt = (BLE_BOND_MTU - offsetof(APP_LE_BOND_REMOTE_INFO,
                                                            dev_infos)) / sizeof(T_LE_DEV_INFO);

    p_send_mgr->batch_num = total_bond_num / p_send_mgr->bond_num_per_pkt;
    p_send_mgr->remains = total_bond_num % p_send_mgr->bond_num_per_pkt;
    if (p_send_mgr->remains != 0) {p_send_mgr->batch_num += 1; }

    APP_PRINT_TRACE4("app_ble_bond batch_send_mgr_init: total_bond_num %d, batch_num %d, remains %d, bond_num_per_pkt %d",
                     total_bond_num, p_send_mgr->batch_num, p_send_mgr->remains, p_send_mgr->bond_num_per_pkt);
}


inline static void app_ble_bond_batch_send(BATCH_SEND_MGR *p_send_mgr,
                                           APP_LE_BOND_REMOTE_INFO *p_info,
                                           uint8_t *bond_pri_table)
{
    if (p_send_mgr->bond_num_per_pkt == 0)
    {
        APP_PRINT_ERROR0("app_ble_bond batch_send: BLE_BOND_MTU too small to hold bond");
        return;
    }

    for (uint32_t batch_idx = 0; batch_idx < p_send_mgr->batch_num; batch_idx++)
    {
        uint32_t dev_info_num = 0;
        if ((batch_idx == p_send_mgr->batch_num - 1) && (p_send_mgr->remains != 0))
        {
            dev_info_num = p_send_mgr->remains;
        }
        else
        {
            dev_info_num = p_send_mgr->bond_num_per_pkt;
        }

        p_info->bond_num = dev_info_num;

        for (uint32_t i = 0; i < dev_info_num; i++)
        {
            T_LE_KEY_ENTRY *p_entry = NULL;
            uint32_t pri = p_send_mgr->bond_num_per_pkt * batch_idx + i;

            p_entry = le_find_key_entry_by_idx(bond_pri_table[pri]);
            le_get_dev_info(p_entry, &p_info->dev_infos[i]);
            APP_PRINT_TRACE5("app_ble_bond batch_send: batch_idx %d, i %d, bond pri %d, bond idx %d, bd_addr %s",
                             batch_idx, i,
                             pri,
                             bond_pri_table[pri],
                             TRACE_BDADDR(p_info->dev_infos[i].remote_bd.addr));
        }

#if F_APP_ERWS_SUPPORT
        app_ble_bond_send_info(APP_LE_BOND_MERGE, p_info,
                               sizeof(*p_info) + dev_info_num * sizeof(T_LE_DEV_INFO));
#endif
    }
}


static void app_ble_bond_send_all_info(void)
{
    BATCH_SEND_MGR send_mgr = {0};


    uint8_t local_bond_num = 0;
    uint8_t *bond_pri_table = NULL;

    local_bond_num = le_get_bond_dev_num();
    app_ble_bond_batch_send_mgr_init(&send_mgr, local_bond_num);


    APP_LE_BOND_REMOTE_INFO *p_info = NULL;
    p_info = calloc(1, sizeof(*p_info) + send_mgr.bond_num_per_pkt * sizeof(T_LE_DEV_INFO));
    if (p_info == NULL)
    {
        goto INFO_MALLOC_ERR;
    }
    p_info->total_bond_num = local_bond_num;

    if (p_info->total_bond_num == 0)
    {
        p_info->bond_num = 0;

#if F_APP_ERWS_SUPPORT
        APP_PRINT_TRACE0("app_ble_bond send_all_info: no le bond, but need to trigger the other bud");
        app_ble_bond_send_info(APP_LE_BOND_MERGE, p_info, sizeof(*p_info));
#endif
        goto BOND_TBL_MALLOC_ERR;
    }

    bond_pri_table = calloc(1, local_bond_num * sizeof(uint8_t));
    if (bond_pri_table == NULL)
    {
        goto BOND_TBL_MALLOC_ERR;
    }
    le_get_bond_priority(&local_bond_num, bond_pri_table);

    app_ble_bond_batch_send(&send_mgr, p_info, bond_pri_table);

    free(bond_pri_table);
BOND_TBL_MALLOC_ERR:
    free(p_info);
INFO_MALLOC_ERR:
    return;
}


#if F_APP_ERWS_SUPPORT
static void __app_ble_bond_parse(uint16_t msg_type, APP_LE_BOND_REMOTE_INFO *p_info, uint16_t len)
{
    switch (msg_type)
    {
    case APP_LE_BOND_DELETE:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            le_delete_dev_info(p_info->dev_infos->remote_bd);
        }
        break;

    case APP_LE_BOND_ADD:
        {
            APP_PRINT_TRACE1("__app_ble_bond_parse: add bd_addr %s",
                             TRACE_BDADDR(p_info->dev_infos[0].remote_bd.addr));
            le_set_dev_info(&p_info->dev_infos[0]);
        }
        break;

    case APP_LE_BOND_MERGE:
        {
            if (p_info->bond_num > le_get_max_le_paired_device_num())
            {
                return;
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (app_le_bond_mgr.sync_mgr.state == SYNC_IDLE)
                {
                    app_le_bond_mgr.sync_mgr.total_bond_num = p_info->total_bond_num;
                    app_le_bond_mgr.sync_mgr.cur_bond_num = 0;
                    app_le_bond_mgr.sync_mgr.state = SYNC_SEC_RECV_INFO;
                    APP_PRINT_TRACE1("__app_ble_bond_parse: total_bond_num %d",
                                     app_le_bond_mgr.sync_mgr.total_bond_num);
                }

                if (app_le_bond_mgr.sync_mgr.state == SYNC_SEC_RECV_INFO)
                {
                    for (uint32_t i = 0; i < p_info->bond_num; i++)
                    {
                        APP_PRINT_TRACE1("__app_ble_bond_parse: bd_addr %s",
                                         TRACE_BDADDR(p_info->dev_infos[i].remote_bd.addr));

                        le_set_dev_info(&p_info->dev_infos[i]);
                    }

                    app_le_bond_mgr.sync_mgr.cur_bond_num += p_info->bond_num;
                    APP_PRINT_TRACE1("__app_ble_bond_parse: cur_bond_num %d",
                                     app_le_bond_mgr.sync_mgr.cur_bond_num);

                    if (app_le_bond_mgr.sync_mgr.cur_bond_num == app_le_bond_mgr.sync_mgr.total_bond_num)
                    {
                        app_le_bond_mgr.sync_mgr.state = SYNC_SEC_SEND_INFO;
                    }
                }

                if (app_le_bond_mgr.sync_mgr.state == SYNC_SEC_SEND_INFO)
                {
                    app_ble_bond_send_all_info();
                    app_ble_bond_sync_mgr_default();
                    APP_PRINT_TRACE0("__app_ble_bond_parse: secondary sync finish");
                }
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                if (app_le_bond_mgr.sync_mgr.state == SYNC_IDLE)
                {
                    app_le_bond_mgr.sync_mgr.total_bond_num = p_info->total_bond_num;
                    app_le_bond_mgr.sync_mgr.cur_bond_num = 0;
                    app_le_bond_mgr.sync_mgr.state = SYNC_PRIM_RECV_INFO;

                    APP_PRINT_TRACE1("__app_ble_bond_parse: total_bond_num %d",
                                     app_le_bond_mgr.sync_mgr.total_bond_num);
                }

                if (app_le_bond_mgr.sync_mgr.state == SYNC_PRIM_RECV_INFO)
                {
                    for (uint32_t i = 0; i < p_info->bond_num; i++)
                    {
                        APP_PRINT_TRACE1("__app_ble_bond_parse: bd_addr %s",
                                         TRACE_BDADDR(p_info->dev_infos[i].remote_bd.addr));

                        le_set_dev_info(&p_info->dev_infos[i]);
                    }

                    app_le_bond_mgr.sync_mgr.cur_bond_num += p_info->bond_num;

                    APP_PRINT_TRACE1("__app_ble_bond_parse: cur_bond_num %d",
                                     app_le_bond_mgr.sync_mgr.cur_bond_num);

                    if (app_le_bond_mgr.sync_mgr.cur_bond_num == app_le_bond_mgr.sync_mgr.total_bond_num)
                    {
                        app_ble_bond_sync_mgr_default();
                        APP_PRINT_TRACE0("__app_ble_bond_parse: primary sync finish");
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif


void app_ble_bond_add_send_to_sec(uint8_t *bd_addr)
{
    T_LE_KEY_ENTRY *p_entry = NULL;
    APP_LE_BOND_REMOTE_INFO *p_info = NULL;

    p_info = malloc(sizeof(APP_LE_BOND_REMOTE_INFO) + sizeof(T_LE_DEV_INFO));

    if (p_info != NULL)
    {
        p_entry = le_find_key_entry(bd_addr, GAP_REMOTE_ADDR_LE_PUBLIC);
        le_get_dev_info(p_entry, &p_info->dev_infos[0]);
        p_info->bond_num = 1;

        APP_PRINT_TRACE1("app_ble_bond_add_send_to_sec: bd_addr %s",
                         TRACE_BDADDR(p_info->dev_infos[0].remote_bd.addr));

#if F_APP_ERWS_SUPPORT
        app_ble_bond_send_info(APP_LE_BOND_ADD, p_info, sizeof(*p_info) + sizeof(T_LE_DEV_INFO));
#endif

        free(p_info);
    }

}


void app_ble_bond_sync_all_info(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        //primary send all le bond info first.
        app_ble_bond_send_all_info();
    }
    else
    {
        //Do nothing
    }
}

#if F_APP_ERWS_SUPPORT
static void app_ble_bond_parse(APP_LE_BOND_REMOTE_MSG msg_type, uint8_t *buf, uint16_t len,
                               T_REMOTE_RELAY_STATUS status)
{
    APP_LE_BOND_REMOTE_INFO *p_info = NULL;
    uint8_t *aligned_buf = NULL;
    uint32_t actual_len = 0;

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        goto OTHER_ERR;
    }


    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        goto OTHER_ERR;
    }

    if (buf == NULL)
    {
        goto OTHER_ERR;
    }

    APP_PRINT_TRACE1("app_ble_bond_parse: msg_type %d", msg_type);

    if ((uint32_t)buf & 0b11)
    {

        aligned_buf = malloc(len);
        if (aligned_buf == NULL)
        {
            goto ALLOC_FAILED;
        }

        memcpy(aligned_buf, buf, len);
        p_info = (APP_LE_BOND_REMOTE_INFO *)aligned_buf;
    }
    else
    {
        p_info = (APP_LE_BOND_REMOTE_INFO *)buf;
    }

    actual_len = sizeof(*p_info) + p_info->bond_num * sizeof(p_info->dev_infos[0]);
    if (actual_len != len)
    {
        APP_PRINT_ERROR0("app_ble_bond_parse: len was wrong");
        goto LEN_ERR;
    }

    __app_ble_bond_parse(msg_type, p_info, len);


LEN_ERR:
    if (aligned_buf)
    {
        free(aligned_buf);
    }
ALLOC_FAILED:
OTHER_ERR:
    return;
}
#endif

void app_ble_bond_init(void)
{
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK) app_ble_bond_parse,
                             APP_MODULE_TYPE_BLE_BOND, APP_LE_BOND_EVT_MAX);
#endif

    app_ble_bond_sync_mgr_default();
}

#endif
