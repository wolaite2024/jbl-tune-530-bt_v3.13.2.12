#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "app_timer.h"
#include "ble_ext_adv.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_adp.h"
#include "app_adp_cmd.h"
#include "app_rtk_fast_pair_adv.h"
#include "app_bt_policy_api.h"
#include "app_bond.h"
#include "app_adv_stop_cause.h"
#include "app_loc_mgr.h"
#include "app_ipc.h"

#if F_APP_ERWS_SUPPORT
#include "app_relay.h"
#endif

#define ADV_INTERVAL    (0x640)  //0x30 ~ 0x140

#define RTK_FAST_PAIR_ADV_STOP_TIMER      30

typedef enum
{
    APP_TIMER_RTK_FAST_PAIR_ADV,
} T_APP_RTK_FAST_PAIR_TIMER;

typedef enum
{
    SYNC_RTK_FAST_PAIR_ADV_COUNT,

    RTK_FAST_PAIR_REMOTE_MSG_MAX
} RTK_FAST_PAIR_REMOTE_MSG;

static uint8_t rtk_fast_pair_handle = 0xff;
static uint8_t rtk_fast_pair_timer_id = 0;
static uint8_t timer_idx_rtk_fast_pair_adv = 0;

static void app_rtk_fast_pair_adv_stop(void);

static uint8_t rtk_fast_pair_adv_data[] =
{
    0x14,       /* length */
    0xFF,       /* type = Manufacturer Specific Data */
    0x5D, 0x00, /* company id */
    0x00,       /* packet number */

    0x00,       /* bit7: left bud charging or not, bit6~bit0: left bud bat bol */
    0x00,       /* bit7: right bud charging or not, bit6~bit0: right bud bat bol */
    0x00,       /* bit7: box charging or not, bit6~bit0: box bat bol */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 6 byte spk1 MAC */
    0x00, 0x00, 0x00,   /* reserved for phone MAC LSP */
    0x00,       /* state */

    0x00,       /* CRC */
    0x53, 0x57, /* UUID */
};

static void app_rtk_fast_pair_timer_cback(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_RTK_FAST_PAIR_ADV:
        {
            app_rtk_fast_pair_adv_stop();
        }
        break;

    default:
        break;
    }
}

static void app_rtk_fast_pair_get_battery_state(uint8_t *p_data)
{
    uint8_t batt_state[3] = {0};
    bool local_charging, remote_charging, case_charging;

    local_charging = (app_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
    remote_charging = (app_db.remote_loc == BUD_LOC_IN_CASE) ? true : false;
    case_charging = ((app_db.case_battery >> 7) == 0) ? true : false;

    //byte 5~6: left and right charging and battery state
    //bit7: charging state. 0: no charging; 1:charging.
    //bit6~0: battery level.
    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
    {
        if (local_charging)
        {
            batt_state[0] |= BIT(7);
        }
        batt_state[0] |= app_db.local_batt_level;

        if (remote_charging)
        {
            batt_state[1] |= BIT(7);
        }
        batt_state[1] |= app_db.remote_batt_level;
    }
    else
    {
        if (remote_charging)
        {
            batt_state[0] |= BIT(7);
        }
        batt_state[0] |= app_db.remote_batt_level;

        if (local_charging)
        {
            batt_state[1] |= BIT(7);
        }
        batt_state[1] |= app_db.local_batt_level;
    }

    //byte 7: charger box charging and battery state
    //bit7: charging state. 0: no charging; 1:charging.
    //bit6~0: battery level.
    if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
    {
        if (case_charging)
        {
            batt_state[2] |= BIT(7);
        }

        // set box bat volume to 0 when two buds are out of case.
        if (local_charging || remote_charging)
        {
            batt_state[2] |= app_db.case_battery & 0x7f;
        }
    }

    memcpy(p_data, &batt_state, 3);
}

static uint8_t app_rtk_fast_pair_get_dev_state(void)
{
    uint8_t dev_state = 0;
    bool local_charging, remote_charging;

    local_charging = (app_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
    remote_charging = (app_db.remote_loc == BUD_LOC_IN_CASE) ? true : false;

    //bit7: left location state.
    //bit6: right location state.
    //0: in case; 1:out case.
    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
    {
        if (!local_charging)
        {
            dev_state |= BIT(7);
        }

        if (!remote_charging)
        {
            dev_state |= BIT(6);
        }
    }
    else
    {
        if (!remote_charging)
        {
            dev_state |= BIT(7);
        }

        if (!local_charging)
        {
            dev_state |= BIT(6);
        }
    }

    //bit5~4:role.
    //00:stereo; 01:prim; 10:sec.
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        dev_state |= BIT(5);
    }
    else
    {
        dev_state |= BIT(4);
    }

    //bit3-2:linkback_state.
    //00:no linkback/linkback fail; 01:doing linkback process; 10:linkback success.
    if (app_link_get_b2s_link_num() != 0)
    {
        dev_state |= BIT(3);
    }
    else if (app_bt_policy_get_state() == BP_STATE_LINKBACK)
    {
        dev_state |= BIT(2);
    }

    //bit1: phone pairing record.
    //0:no phone pairing record; 1:have phone pairing record
    T_APP_LINK_RECORD link_record;
    if (1 == app_bond_get_b2s_link_record(&link_record, 1))
    {
        dev_state |= BIT(1);
    }

    //bit0: reserved.

    return dev_state;
}

static void app_rtk_fast_pair_set_adv_para(uint8_t *p_data)
{
    /* ======  Set Byte2~3 Company Id  ======= */
    p_data[2] = app_cfg_const.company_id[0];
    p_data[3] = app_cfg_const.company_id[1];

    /* ======  Set Byte4 Packet Number  ======= */
    uint16_t count_tmp = app_cfg_nv.adv_count;

    app_cfg_nv.adv_count = (++count_tmp) & 0xFF;
    p_data[4] = app_cfg_nv.adv_count;
#if F_APP_ERWS_SUPPORT
    app_relay_async_single(APP_MODULE_TYPE_RTK_FAST_PAIR, SYNC_RTK_FAST_PAIR_ADV_COUNT);
#endif

    /* ======  Set Byte5~7 Battery State  ======= */
    app_rtk_fast_pair_get_battery_state(&p_data[5]);

    /* ======  Set Byte8-13 Primary Addr  ======= */
    for (int i = 0; i < 6; i++)
    {
        if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
        {
            p_data[i + 8] = app_db.factory_addr[5 - i];
        }
        else
        {
            p_data[i + 8] = app_cfg_nv.bud_local_addr[5 - i];
        }
    }

    /* ======  Byte14-16 reserved for phone adddress LSP  ======= */

    /* ======  Set Byte17 Device State  ======= */
    p_data[17] = app_rtk_fast_pair_get_dev_state();

    /* ======  Set Byte18 CRC  ======= */
    uint16_t crc = 0;
    for (int i = 5; i <= 17; i++)
    {
        crc += p_data[i];
    }
    p_data[18] = (crc & 0xFF);

    /* ======  Set Byte18 UUID  ======= */
    p_data[19] = app_cfg_const.uuid[1];
    p_data[20] = app_cfg_const.uuid[0];
}

static void app_rtk_fast_pair_adv_start(uint8_t adv_type)
{
    APP_PRINT_TRACE2("app_rtk_fast_pair_adv_start: type %x, device_state: %d", adv_type,
                     app_db.device_state);

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        app_rtk_fast_pair_set_adv_para(rtk_fast_pair_adv_data);
        ble_ext_adv_mgr_change_adv_interval(rtk_fast_pair_handle, ADV_INTERVAL);
        ble_ext_adv_mgr_set_adv_data(rtk_fast_pair_handle, sizeof(rtk_fast_pair_adv_data),
                                     (uint8_t *)&rtk_fast_pair_adv_data);

        if (ble_ext_adv_mgr_get_adv_state(rtk_fast_pair_handle) == BLE_EXT_ADV_MGR_ADV_DISABLED)
        {
            ble_ext_adv_mgr_enable(rtk_fast_pair_handle, 0);
        }

        app_start_timer(&timer_idx_rtk_fast_pair_adv, "rtk_fast_pair_adv",
                        rtk_fast_pair_timer_id, APP_TIMER_RTK_FAST_PAIR_ADV, 0, false,
                        RTK_FAST_PAIR_ADV_STOP_TIMER * 1000);
    }
}

static void app_rtk_fast_pair_adv_stop(void)
{
    APP_PRINT_TRACE0("app_rtk_fast_pair_adv_stop");

    app_stop_timer(&timer_idx_rtk_fast_pair_adv);
    ble_ext_adv_mgr_disable(rtk_fast_pair_handle, APP_STOP_ADV_CAUSE_RTK_FAST_PAIR);
}

void app_rtk_fast_pair_handle_power_on(void)
{
    app_rtk_fast_pair_adv_start(RTK_FAST_PAIR_ADV_TYPE_POWER_ON);
}

void app_rtk_fast_pair_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_rtk_fast_pair_adv_start(RTK_FAST_PAIR_ADV_TYPE_ENGAGEMENT);
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_rtk_fast_pair_adv_stop();
    }
}

void app_rtk_fast_pair_handle_in_out_case(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_rtk_fast_pair_adv_start(RTK_FAST_PAIR_ADV_TYPE_IN_OUT_CASE);
    }
}

void app_rtk_fast_pair_handle_batt_change(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (ble_ext_adv_mgr_get_adv_state(rtk_fast_pair_handle) == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            app_rtk_fast_pair_adv_start(RTK_FAST_PAIR_ADV_TYPE_BATT_CHANGE);
        }
    }
}

#if F_APP_ERWS_SUPPORT
uint16_t app_rtk_fast_pair_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    APP_PRINT_INFO1("app_rtk_fast_pair_relay_cback: msg_type %d", msg_type);

    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case SYNC_RTK_FAST_PAIR_ADV_COUNT:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.adv_count;
        }
        break;

    default:
        break;

    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_RTK_FAST_PAIR, payload_len, msg_ptr, skip,
                              total);
}

static void app_rtk_fast_pair_parse(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_rtk_fast_pair_parse: msg = 0x%x, status = %x", msg_type, status);

    switch (msg_type)
    {
    case SYNC_RTK_FAST_PAIR_ADV_COUNT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_cfg_nv.adv_count = p_info[0];
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

static void app_rtk_fast_pair_bud_loc_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case EVENT_OUT_CASE:
    case EVENT_IN_CASE:
        {
            app_rtk_fast_pair_handle_in_out_case();
        }
        break;

    default:
        break;
    }
}
void app_rtk_fast_pair_adv_init(void)
{
    if (app_cfg_const.enable_rtk_fast_pair_adv)
    {
        T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
            LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
        uint16_t adv_interval_min = 0x640;
        uint16_t adv_interval_max = 0x640;
        T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
        T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
        T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

        ble_ext_adv_mgr_init_adv_params(&rtk_fast_pair_handle, adv_event_prop, adv_interval_min,
                                        adv_interval_max, own_address_type, peer_address_type, peer_address,
                                        filter_policy, 0, NULL,
                                        0, NULL, NULL);

        app_timer_reg_cb(app_rtk_fast_pair_timer_cback, &rtk_fast_pair_timer_id);

#if F_APP_ERWS_SUPPORT
        app_relay_cback_register(app_rtk_fast_pair_relay_cback, app_rtk_fast_pair_parse,
                                 APP_MODULE_TYPE_RTK_FAST_PAIR, RTK_FAST_PAIR_REMOTE_MSG_MAX);
#endif

        app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_rtk_fast_pair_bud_loc_event_cback);
    }
}
#endif

