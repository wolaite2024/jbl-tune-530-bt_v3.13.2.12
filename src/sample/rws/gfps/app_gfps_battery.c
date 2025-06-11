#if GFPS_FEATURE_SUPPORT
#include "app_gfps_battery.h"
#include "app_main.h"
#include "app_gfps_msg.h"
#include "app_gfps.h"
#include "app_loc_mgr.h"
#include "stdint.h"
#include "string.h"
#include "trace.h"
#include "bt_gfps.h"
#include "app_cfg.h"
#include "app_ipc.h"

//app_report_rws_bud_info()
extern T_GFPS_DB gfps_db;
extern uint8_t gfps_adv_len;
extern uint8_t gfps_adv_data[GAP_MAX_LEGACY_ADV_LEN];

void app_gfps_battery_set_info(T_GFPS_BATTERY_INFO *p_battery_info)
{
    T_GFPS_BATTERY_INFO *p_info = &(gfps_db.gfps_battery_info);
    if ((memcmp(p_info, p_battery_info, sizeof(T_GFPS_BATTERY_INFO)) != 0) &&
        (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        //only primary bud update battery info
        memcpy(p_info, p_battery_info, sizeof(T_GFPS_BATTERY_INFO));
        gfps_set_battery_info(p_info);

        if (gfps_db.gfps_curr_action == GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE)
        {
            if (gfps_gen_adv_data(NOT_DISCOVERABLE_MODE, gfps_adv_data, &gfps_adv_len, true))
            {
                ble_ext_adv_mgr_set_adv_data(gfps_db.gfps_adv_handle, gfps_adv_len, gfps_adv_data);
            }
            APP_PRINT_INFO0("app_gfps_battery_set_info: updated battery by LE,popup show ui");
        }
        else if (gfps_db.gfps_curr_action == GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI)
        {
            if (gfps_gen_adv_data(NOT_DISCOVERABLE_MODE, gfps_adv_data, &gfps_adv_len, false))
            {
                ble_ext_adv_mgr_set_adv_data(gfps_db.gfps_adv_handle, gfps_adv_len, gfps_adv_data);
            }
            APP_PRINT_INFO0("app_gfps_battery_set_info: updated battery by LE,popup hide ui");
        }
    }
#if F_APP_ERWS_SUPPORT
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        gfps_set_battery_info(NULL);
    }
#endif
}

void app_gfps_battery_rfc_update(uint8_t *battery)
{
    uint8_t i;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            (app_db.br_link[i].connected_profile & GFPS_PROFILE_MASK))
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[i];
            bt_gfps_updated_battery(0xFF, 0, p_link->bd_addr, p_link->gfps_rfc_chann, battery,
                                    GFPS_BATTERY_LEN);
            APP_PRINT_INFO3("app_gfps_battery_rfc_update: left 0x%x, right 0x%x, case 0x%x", battery[0],
                            battery[1], battery[2]);
        }
    }
}

void app_gfps_battery_le_l2cap_update(uint8_t *battery)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        T_APP_LE_LINK *le_link = &app_db.le_link[i];

        if ((le_link->used == true) && (le_link->gfps_link.gfps_conn_id != 0xFF) &&
            (le_link->gfps_link.gfps_conn_state == GAP_CONN_STATE_CONNECTED))
        {
            bt_gfps_updated_battery(le_link->gfps_link.gfps_conn_id, le_link->gfps_link.gfps_msg_cid,
                                    NULL, 0, battery, GFPS_BATTERY_LEN);

            APP_PRINT_INFO4("app_gfps_battery_le_l2cap_update: conn_id %d, left 0x%x, right 0x%x, case 0x%x",
                            le_link->gfps_link.gfps_conn_id, battery[0], battery[1], battery[2]);
        }
    }
}

T_GFPS_BATTERY_INFO app_gfps_battery_get_info()
{
    return gfps_db.gfps_battery_info;
}

void app_gfps_battery_info_report(T_GFPS_BATTERY_REPORT_EVENT event)
{
    T_GFPS_BATTERY_INFO battery_info;
    battery_info = app_gfps_battery_get_info();
    APP_PRINT_INFO1("app_gfps_battery_info_report: event %d", event);

    uint8_t local_batt_level = app_db.local_batt_level;
    app_db.local_charger_state = app_charger_get_charge_state();
    bool local_charger_state = (app_db.local_charger_state == APP_CHARGER_STATE_CHARGING) ? true :
                               false;

    uint8_t remote_batt_level = app_db.remote_batt_level;
    bool remote_charger_state = (app_db.remote_charger_state == APP_CHARGER_STATE_CHARGING) ? true :
                                false;

    uint8_t case_battery = app_db.case_battery;

    switch (event)
    {
    case GFPS_BATTERY_REPORT_b2b_DISCONNECT:
    case GFPS_BATTERY_REPORT_ENGAGE_FAIL:
        {
            remote_batt_level = 0x7F;
            remote_charger_state = false;
            if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
            {
                case_battery = 0x7F;
            }
        }
        break;

    case GFPS_BATTERY_REPORT_BUDS_OUT_CASE:
        {
            case_battery = 0x7F;
        }
        break;

    default:
        break;
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        remote_batt_level = 0x7F;
        remote_charger_state = false;
        case_battery = 0x7F;
    }

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        remote_batt_level = 0x7F;
        remote_charger_state = false;
        if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
        {
            case_battery = 0x7F;
        }
    }

    if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc != BUD_LOC_IN_CASE))
    {
        case_battery = 0x7F;
    }

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
    {
        if (extend_app_cfg_const.gfps_left_ear_batetry_support)
        {
            battery_info.battery_left_charging = local_charger_state;
            battery_info.battery_left_level = local_batt_level;
        }

        if (extend_app_cfg_const.gfps_right_ear_batetry_support)
        {
            battery_info.battery_right_charging = remote_charger_state;
            battery_info.battery_right_level = remote_batt_level;
        }
    }
    else
    {
        if (extend_app_cfg_const.gfps_left_ear_batetry_support)
        {
            battery_info.battery_left_charging = remote_charger_state;
            battery_info.battery_left_level = remote_batt_level;
        }

        if (extend_app_cfg_const.gfps_right_ear_batetry_support)
        {
            battery_info.battery_right_charging = local_charger_state;
            battery_info.battery_right_level = local_batt_level;
        }
    }

    if (extend_app_cfg_const.gfps_case_battery_support)
    {
        battery_info.battery_case_charging = ((case_battery >> 7) == 0) ? 1 : 0;
        battery_info.battery_case_level = case_battery & 0x7F;

        if (case_battery == 0x7F)
        {
            battery_info.battery_case_charging = 0;
        }
    }

    if ((battery_info.battery_enable) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_gfps_battery_set_info(&battery_info);/*report battery by adv*/

        uint8_t battery[3];
        battery[0] = (battery_info.battery_left_charging << 7) | (battery_info.battery_left_level & 0x7F);
        battery[1] = (battery_info.battery_right_charging << 7) | (battery_info.battery_right_level & 0x7F);
        battery[2] = (battery_info.battery_case_charging << 7) | (battery_info.battery_case_level & 0x7F);

        if (app_cfg_const.supported_profile_mask & GFPS_PROFILE_MASK)
        {
            app_gfps_battery_rfc_update(battery);/*report by rfcomm*/
        }
#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            app_gfps_battery_le_l2cap_update(battery);//report by le l2cap channel
        }
#endif
    }
}

/**
 * @brief  show_ui = true: battery show ui
 *         show_ui = false: battery hide ui
 */
void app_gfps_battery_show_ui(bool show_ui)
{
    APP_PRINT_INFO1("app_gfps_battery_show_ui: %d", show_ui);
    T_GFPS_BATTERY_INFO battery_info;
    battery_info = app_gfps_battery_get_info();
    battery_info.battery_show_ui = show_ui;
    app_gfps_battery_set_info(&battery_info);
}

#if F_APP_BUD_LOCATION_SUPPORT
static void app_gfps_battery_bud_loc_event_cback(uint32_t event, void *msg)
{
    bool from_remote = *((bool *)msg);

    switch (event)
    {
    case EVENT_OUT_CASE:
        {
            if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc != BUD_LOC_IN_CASE))
            {
                app_gfps_battery_info_report(GFPS_BATTERY_REPORT_BUDS_OUT_CASE);
            }
        }
        break;

    case EVENT_IN_CASE:
        {
            app_gfps_battery_info_report(GFPS_BATTERY_REPORT_BUD_IN_CASE);
        }
        break;

    default:
        break;
    }

    APP_PRINT_TRACE2("app_gfps_battery_bud_loc_event_cback: event_type 0x%02x, from_remote %d",
                     event, from_remote);
}
#endif

void app_gfps_battery_info_init()
{
    T_GFPS_BATTERY_INFO battery_info;
    battery_info = app_gfps_battery_get_info();

    battery_info.battery_enable = extend_app_cfg_const.gfps_battery_info_enable;
    battery_info.battery_remain_time_enable = extend_app_cfg_const.gfps_battery_remain_time_enable;
    battery_info.battery_show_ui = extend_app_cfg_const.gfps_battery_show_ui;

    battery_info.battery_left_charging = 0;
    battery_info.battery_left_level = 0x7F;
    battery_info.battery_right_charging = 0;
    battery_info.battery_right_level = 0x7F;
    battery_info.battery_case_charging = 0;
    battery_info.battery_case_level = 0x7F;

    app_gfps_battery_set_info(&battery_info);

#if F_APP_BUD_LOCATION_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_gfps_battery_bud_loc_event_cback);
    }
#endif
}
#endif
