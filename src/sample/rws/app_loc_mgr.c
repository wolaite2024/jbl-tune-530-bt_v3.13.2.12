/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "sysm.h"
#include "trace.h"
#include "app_adp_cmd.h"
#include "app_audio_policy.h"
#include "app_cfg.h"
#include "app_dlps.h"
#include "app_ipc.h"
#include "app_loc_mgr.h"
#include "app_main.h"
#include "app_mmi.h"
#include "app_relay.h"
#include "app_timer.h"
#if GFPS_FEATURE_SUPPORT
#include "app_gfps_battery.h"
#endif
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

typedef enum
{
    APP_TIMER_LOC_IN_EAR_FROM_CASE,
} T_APP_lOC_TIMER;

#if F_APP_BUD_LOCATION_SUPPORT

static uint8_t app_loc_mgr_timer_id = 0;
static uint8_t timer_idx_in_ear_from_case = 0;
static bool loc_in_ear_from_case = false;

static T_BUD_LOCATION app_loc_mgr_evt_to_location(T_BUD_LOCATION_EVENT evt, T_BUD_LOCATION pre_loc)
{
    T_BUD_LOCATION cur_loc = pre_loc;

    if (evt != EVENT_NON)
    {
        if (evt == EVENT_IN_CASE)
        {
            cur_loc = BUD_LOC_IN_CASE;
        }
        else if (evt == EVENT_OUT_CASE)
        {
            cur_loc = app_db.local_in_ear ? BUD_LOC_IN_EAR : BUD_LOC_IN_AIR;
        }
        else if (evt == EVENT_IN_EAR)
        {
            cur_loc = app_db.local_in_case ? BUD_LOC_IN_CASE : BUD_LOC_IN_EAR;
        }
        else if (evt == EVENT_OUT_EAR)
        {
            cur_loc = app_db.local_in_case ? BUD_LOC_IN_CASE : BUD_LOC_IN_AIR;
        }
    }

    APP_PRINT_TRACE5("app_loc_mgr_evt_to_location: pre %d, cur %d, evt %d local_in_case %d local_in_ear %d",
                     pre_loc, cur_loc, evt, app_db.local_in_case, app_db.local_in_ear);

    return cur_loc;
}

static bool app_loc_mgr_evt_adjust(T_BUD_LOCATION_EVENT evt_in,
                                   T_BUD_LOCATION_EVENT *evt_out, T_BUD_LOCATION_CAUSE_ACTION cause_action)
{
    bool need_sync = true;

    if ((app_db.local_in_case && (evt_in == EVENT_IN_EAR))
        || (app_db.local_in_case && (evt_in == EVENT_OUT_EAR))
        || (app_db.local_loc_pre == BUD_LOC_IN_AIR) && (evt_in == EVENT_OUT_EAR))
    {
        /*not report event:
        ear changed during bud is in case or
        out ear during bud is in are*/
        *evt_out = EVENT_NON;
        need_sync = false;
    }

    if (app_db.local_in_ear && (evt_in == EVENT_OUT_CASE))
    {
        loc_in_ear_from_case = true;
        /*send out case event,dalay to check whether is in ear*/
        app_start_timer(&timer_idx_in_ear_from_case, "in_ear_from_case",
                        app_loc_mgr_timer_id, APP_TIMER_LOC_IN_EAR_FROM_CASE, cause_action, false,
                        500);
    }

    if (evt_in != EVENT_OUT_CASE)
    {
        loc_in_ear_from_case = false;
        app_stop_timer(&timer_idx_in_ear_from_case);
    }

    APP_PRINT_INFO8("app_loc_mgr_evt_adjust: evt 0x%02x-->0x%02x local_in_case %d local_in_ear %d cause_action %d need_sync %d local_loc %d-->%d",
                    evt_in, *evt_out, app_db.local_in_case, app_db.local_in_ear, cause_action, need_sync,
                    app_db.local_loc_pre, app_db.local_loc);

    return need_sync;
}

void app_loc_mgr_state_machine(T_BUD_LOCATION_EVENT evt, bool from_remote,
                               T_BUD_LOCATION_CAUSE_ACTION cause_action)
{
    uint8_t msg[3] = {0};

    //  bud_loc_msg;
    if (evt == EVENT_NON)
    {
        return;
    }

    if (app_cfg_nv.is_dut_test_mode && evt == EVENT_IN_CASE)
    {
        app_mmi_handle_action(MMI_DEV_POWER_OFF);
        APP_PRINT_INFO0("app_loc_mgr_state_machine: exit DUT test mode");
        return;
    }

    if (from_remote)
    {
        if (evt == EVENT_CLOSE_CASE)
        {
            app_db.remote_case_closed = true;
        }
        else if (evt == EVENT_OPEN_CASE)
        {
            app_db.remote_case_closed = false;
        }
    }
    else
    {
        app_db.local_loc_pre = app_db.local_loc;
        app_db.local_loc = app_loc_mgr_evt_to_location(evt, (T_BUD_LOCATION)app_db.local_loc);

        if (evt == EVENT_CLOSE_CASE)
        {
            app_db.local_case_closed = true;
        }
        else if (evt == EVENT_OPEN_CASE)
        {
            app_db.local_case_closed = false;
        }

#if (F_APP_ERWS_SUPPORT == 1)
        if (app_loc_mgr_evt_adjust(evt, &evt, cause_action))
        {
            msg[0] = evt;
            msg[1] = cause_action;
            msg[2] = app_db.local_loc;
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LOC, BUD_MSG_LOC_EVT,
                                                (uint8_t *)&msg, (3 * sizeof(uint8_t)));
        }
        else
        {
            goto log_mgr_print;
        }
#else
        if (evt == EVENT_IN_CASE && app_cfg_const.enable_inbox_power_off)
        {
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
#endif

    }

    app_db.last_bud_loc_event = evt;

    msg[0] = from_remote;
    msg[1] = cause_action;
    app_ipc_publish(BUD_LOCATION_IPC_TOPIC, evt, &msg);

log_mgr_print:
    APP_PRINT_INFO5("app_loc_mgr_state_machine 0x%02x, from_remote %d, cause_action %d, local loc %d, remote loc %d",
                    evt, from_remote, cause_action, app_db.local_loc, app_db.remote_loc);
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_loc_mgr_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case BUD_MSG_LOC:
        {
            payload_len = sizeof(app_db.local_loc);
            msg_ptr = (uint8_t *)&app_db.local_loc;
        }
        break;

    default:
        break;
    }
    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_LOC, payload_len, msg_ptr, skip,
                              total);
}

static void app_loc_mgr_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        switch (msg_type)
        {
        case BUD_MSG_LOC_EVT:
            {
                T_BUD_LOCATION_EVENT evt = *((T_BUD_LOCATION_EVENT *)&buf[0]);
                T_BUD_LOCATION_CAUSE_ACTION cause_action = *((T_BUD_LOCATION_CAUSE_ACTION *)&buf[1]);

                app_db.remote_loc_pre = app_db.remote_loc;
                app_db.remote_loc = *((T_BUD_LOCATION *)&buf[2]);
                app_loc_mgr_state_machine(evt, 1, cause_action);
            }
            break;

        case BUD_MSG_LOC:
            {
                uint8_t msg = 1; //from remote

                app_db.remote_loc = *((T_BUD_LOCATION *)buf);

#if GFPS_FEATURE_SUPPORT
                if (extend_app_cfg_const.gfps_support)
                {
                    if (app_db.remote_loc == BUD_LOC_IN_CASE)
                    {
                        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_BUD_IN_CASE);
                    }
                    else if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc != BUD_LOC_IN_CASE))
                    {
                        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_BUDS_OUT_CASE);
                    }
                }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
                app_listening_mode_check_remote_loc(app_db.remote_loc);
#endif

                app_ipc_publish(BUD_LOCATION_IPC_TOPIC, EVENT_RMEOTE_SYNCED, &msg);
            }
            break;

        default:
            break;
        }
    }
}
#endif

static void app_loc_mgr_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            app_db.local_loc = app_loc_mgr_local_detect();
            app_db.local_loc_pre = app_db.local_loc;
            app_relay_async_single(APP_MODULE_TYPE_LOC, BUD_MSG_LOC);
#if F_APP_DURIAN_SUPPORT
            /*spk mute check by durian*/
#else
            //moved from app_adp.c
            if (app_db.local_in_case)
            {
                app_audio_spk_mute_unmute(true);
            }
            else
            {
                app_audio_spk_mute_unmute(false);
            }
#endif
        }
        break;

    default:
        break;
    }
}


static void app_loc_mgr_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
#if F_APP_ERWS_SUPPORT
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            app_relay_async_single(APP_MODULE_TYPE_LOC, BUD_MSG_LOC);

            if (app_cfg_const.enable_rtk_charging_box)
            {
                app_loc_mgr_state_machine(EVENT_OPEN_CASE, 0, CAUSE_ACTION_NON);
            }
        }
        break;
#endif
    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_loc_mgr_bt_cback 0x%04x", event_type);
    }
}


T_BUD_LOCATION app_loc_mgr_local_detect(void)
{
    T_BUD_LOCATION local_loc;

    app_db.local_in_case = app_device_is_in_the_box();

    if (app_db.local_in_case)
    {
        local_loc = BUD_LOC_IN_CASE;
    }
    else if (app_db.local_in_ear)
    {
        local_loc = BUD_LOC_IN_EAR;
    }
    else
    {
        local_loc = BUD_LOC_IN_AIR;
    }

    APP_PRINT_INFO3("app_loc_mgr_local_detect: local_in_case %d, local_in_ear %d, local_loc 0x%x",
                    app_db.local_in_case, app_db.local_in_ear, local_loc);

    return local_loc;
}

static void app_loc_mgr_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_loc_mgr_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_LOC_IN_EAR_FROM_CASE:
        {
            app_stop_timer(&timer_idx_in_ear_from_case);

            if (loc_in_ear_from_case)
            {
                loc_in_ear_from_case = false;
                if (app_db.local_in_ear)
                {
                    app_loc_mgr_state_machine(EVENT_IN_EAR, 0, (T_BUD_LOCATION_CAUSE_ACTION)param);
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_loc_mgr_init(void)
{
    app_db.local_loc = BUD_LOC_UNKNOWN;
    app_db.remote_loc = BUD_LOC_UNKNOWN;
    app_db.last_bud_loc_event = EVENT_NON;

    sys_mgr_cback_register(app_loc_mgr_dm_cback);
    bt_mgr_cback_register(app_loc_mgr_bt_cback);
    app_timer_reg_cb(app_loc_mgr_timeout_cb, &app_loc_mgr_timer_id);

#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_loc_mgr_relay_cback, app_loc_mgr_parse_cback,
                             APP_MODULE_TYPE_LOC, BUD_MSG_MAX);
#endif
}

#endif
