/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "os_sched.h"
#include "gap_br.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_report.h"
#include "app_ipc.h"

#if (F_APP_ERWS_SUPPORT == 1)
#define AVP_IN_EAR_INTERVAL_MS            1000
#endif

typedef enum
{
    APP_TIMER_BUD_CHANGE_DEBOUNCE,
    APP_TIMER_LOC_REPORT,
} T_APP_AVP_TIMER;

static uint8_t app_durian_loc_timer_id = 0;

static uint8_t timer_idx_bud_change_debounce = 0;
static uint8_t timer_idx_loc_report = 0;

#define BUD_CHANGE_DEBOUNCE_MS        200
#define LOC_REPORT_MS                 1500

#if (F_APP_ERWS_SUPPORT == 1)
static uint32_t first_in_ear_time;
#endif

static const T_BT_AVP_BUD_LOCATION  bud_loc_map[4] = {BT_AVP_BUD_UNKNOWN, BT_AVP_BUD_IN_CASE, BT_AVP_BUD_OUT_OF_CASE, BT_AVP_BUD_IN_EAR};/*refer to T_BUD_LOCATION*/

void app_durian_loc_in_ear_detect(uint8_t ear_detect_en)
{
    APP_PRINT_INFO1("app_avp_in_ear_detection: ear_detect_en %d", ear_detect_en);

    durian_db.ear_detect_en = ear_detect_en;
    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_EAR_DETECT_ENABLE);

    app_start_timer(&timer_idx_loc_report, "loc_report",
                    app_durian_loc_timer_id, APP_TIMER_LOC_REPORT, 0, false,
                    LOC_REPORT_MS);
    app_durian_audio_spk_check();
}

static void app_durian_loc_in_ear_play(uint8_t cause_action, uint8_t local_pre, uint8_t remote_pre)
{
    bool low_batt_play = false, tone_need_play = false;

    bool local_is_low = app_charger_local_battery_status_is_low();
    bool remote_is_low = app_charger_remote_battery_status_is_low();
    uint8_t b2s_num = app_link_get_b2s_link_num();
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

    if (cause_action == 0)
    {
        goto ear_action_no_need_play;
    }

    if ((b2s_num == 0) && (!app_cfg_const.play_in_ear_tone_regardless_of_phone_connection))
    {
        goto ear_action_no_need_play;
    }

    if (durian_db.ear_detect_en == 0)
    {
        goto ear_action_no_need_play;
    }

    if (call_status >= APP_CALL_INCOMING)
    {
        goto ear_action_no_need_play;
    }

    if ((durian_db.local_loc != BUD_LOC_IN_EAR) && (durian_db.remote_loc != BUD_LOC_IN_EAR))
    {
        goto ear_action_no_need_play;
    }

    if ((durian_db.local_loc == BUD_LOC_IN_EAR) && (durian_db.remote_loc == BUD_LOC_IN_EAR))
    {
        uint32_t in_ear_differ_time = os_sys_time_get() - first_in_ear_time;
        if ((app_cfg_const.play_in_ear_tone_when_any_bud_in_ear) &&
            (in_ear_differ_time > AVP_IN_EAR_INTERVAL_MS))
        {
            tone_need_play = true;
            if (local_is_low)
            {
                low_batt_play = true;
            }
        }
    }
    else
    {
        if (((durian_db.local_loc == BUD_LOC_IN_EAR) && (local_pre != BUD_LOC_IN_EAR)) ||
            ((durian_db.remote_loc == BUD_LOC_IN_EAR) &&
             (remote_pre != BUD_LOC_IN_EAR)))//only one bud is in ear with action,play in ear or low batt tone
        {
            tone_need_play = true;
            if (local_is_low)
            {
                low_batt_play = true;
            }
            first_in_ear_time = os_sys_time_get();
        }
    }

    if (tone_need_play)
    {
        if (low_batt_play)
        {
            app_audio_tone_type_play(TONE_BATTERY_PERCENTAGE_10, false, true);
        }
        else
        {
            app_audio_tone_type_play(TONE_IN_EAR_DETECTION, false, true);
        }
    }

ear_action_no_need_play:
    APP_PRINT_INFO8("app_durian_loc_in_ear_play: b2s_num %d ear_detect_en %d cause_action %d call_status %d,local_is_low %d remote_is_low %d tone_need_play %d low_batt_play %d ",
                    b2s_num, durian_db.ear_detect_en, cause_action, call_status, local_is_low, remote_is_low,
                    tone_need_play, low_batt_play);
}

void app_durian_loc_report(uint8_t local, uint8_t remote)
{
    T_BT_AVP_BUD_LOCATION pri_ear_loc, sec_ear_loc;
    uint8_t bud_loc_buff[2];

    remote = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ?
             remote : BUD_LOC_UNKNOWN;

#if DURIAN_AIRMAX
    pri_ear_loc = bud_loc_map[local];
    sec_ear_loc = pri_ear_loc;
#else
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        pri_ear_loc = bud_loc_map[local];
        sec_ear_loc = bud_loc_map[remote];
    }
    else
    {
        sec_ear_loc = bud_loc_map[local];
        pri_ear_loc = bud_loc_map[remote];
    }
#endif

    APP_PRINT_INFO4("app_durian_loc_report: local =%d, remote =%d, pri_ear_loc %d sec_ear_loc %d",
                    local, remote, pri_ear_loc,
                    sec_ear_loc);
    bud_loc_buff[0] = pri_ear_loc;
    bud_loc_buff[1] = sec_ear_loc;
    //report bud to non-apple mode and android
    app_durian_sync_report_event_broadcast(EVENT_AVP_BUD_LOCAL, bud_loc_buff, 2);

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & AVP_PROFILE_MASK)
        {
            APP_PRINT_INFO0("app_durian_loc_report: reported");


            bt_avp_bud_location_report(app_db.br_link[i].bd_addr, pri_ear_loc, sec_ear_loc,
                                       (app_cfg_const.bud_side == DEVICE_ROLE_LEFT) ? false : true);
        }
    }
}

static void app_durian_loc_change_debounce(void)
{
    app_start_timer(&timer_idx_bud_change_debounce, "bud_change_debounce",
                    app_durian_loc_timer_id, APP_TIMER_BUD_CHANGE_DEBOUNCE, 0, false,
                    BUD_CHANGE_DEBOUNCE_MS);
}

#if (F_APP_ERWS_SUPPORT == 1)
void app_durian_loc_changed(int8_t event, bool from_remote)
{
    static T_BUD_LOCATION local_loc_pre = BUD_LOC_UNKNOWN;
    static T_BUD_LOCATION remote_loc_pre = BUD_LOC_UNKNOWN;
    bool link_back = false;

    app_db.last_bud_loc_event = event;

    if (app_cfg_const.sensor_support)
    {
        durian_db.local_loc = (app_db.local_loc != BUD_LOC_UNKNOWN) ? app_db.local_loc :
                              durian_db.local_loc;
        durian_db.remote_loc = (app_db.remote_loc != BUD_LOC_UNKNOWN) ? app_db.remote_loc :
                               durian_db.remote_loc;
    }
    else
    {
        if (app_db.local_loc != BUD_LOC_UNKNOWN)
        {
            durian_db.local_loc = (app_db.local_loc == BUD_LOC_IN_CASE) ? BUD_LOC_IN_CASE : BUD_LOC_IN_EAR;
        }

        if (app_db.remote_loc != BUD_LOC_UNKNOWN)
        {
            durian_db.remote_loc = (app_db.remote_loc == BUD_LOC_IN_CASE) ? BUD_LOC_IN_CASE : BUD_LOC_IN_EAR;
        }
    }

    if (((local_loc_pre != BUD_LOC_IN_CASE) && (durian_db.local_loc == BUD_LOC_IN_CASE)) ||
        ((local_loc_pre == BUD_LOC_IN_CASE) && (durian_db.local_loc != BUD_LOC_IN_CASE)))
    {
        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_IN_BOX, app_cfg_const.timer_auto_power_off);
    }

    app_durian_loc_change_debounce();

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        goto bud_changed_update_pre;
    }

    app_durian_loc_in_ear_play(1, local_loc_pre, remote_loc_pre);

    if ((durian_db.remote_loc != BUD_LOC_IN_EAR) && (durian_db.local_loc != BUD_LOC_IN_EAR))
    {
        //both out ear
        durian_db.both_in_ear_src_lost = false;
    }

    if ((local_loc_pre != BUD_LOC_IN_EAR) && (durian_db.local_loc == BUD_LOC_IN_EAR))
    {
        //local in ear
        if (durian_db.both_in_ear_src_lost)
        {
            durian_db.local_loc_changed = true;
            if (durian_db.remote_loc_changed)
            {
                link_back = true;
            }
        }
        else
        {
            link_back = true;
        }
    }

    if ((remote_loc_pre != BUD_LOC_IN_EAR) && (durian_db.remote_loc == BUD_LOC_IN_EAR))
    {
        //remote in ear
        if (durian_db.both_in_ear_src_lost)
        {
            durian_db.remote_loc_changed = true;
            if (durian_db.local_loc_changed)
            {
                link_back = true;
            }
        }
        else
        {
            link_back = true;
        }
    }

    if ((local_loc_pre != BUD_LOC_IN_AIR) && (durian_db.local_loc == BUD_LOC_IN_AIR))
    {
        //local out ear
        if (durian_db.both_in_ear_src_lost)
        {
            if (durian_db.remote_loc_changed)
            {
                link_back = true;
            }
        }
    }

    if ((remote_loc_pre != BUD_LOC_IN_AIR) && (durian_db.remote_loc == BUD_LOC_IN_AIR))
    {
        //remote out ear
        if (durian_db.both_in_ear_src_lost)
        {
            if (durian_db.local_loc_changed)
            {
                link_back = true;
            }
        }
    }

    if (((local_loc_pre == BUD_LOC_IN_CASE) && (durian_db.local_loc != BUD_LOC_IN_CASE)) ||
        ((remote_loc_pre == BUD_LOC_IN_CASE) && (durian_db.remote_loc != BUD_LOC_IN_CASE)))
    {
        link_back = true;
    }

    if (link_back)
    {
        app_durian_link_back();
    }

    if (durian_db.ear_detect_en != 0)
    {
        if ((local_loc_pre != durian_db.local_loc) || (remote_loc_pre != durian_db.remote_loc))
        {
            app_durian_loc_report(durian_db.local_loc, durian_db.remote_loc);
        }

    }

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        goto bud_changed_update_pre;
    }

    if (!durian_db.remote_loc_received)
    {
        goto bud_changed_update_pre;
    }

    {
        bool need_report = false;
        if ((durian_db.remote_loc == BUD_LOC_IN_CASE) &&
            (durian_db.local_loc == BUD_LOC_IN_CASE))//both in box
        {
            if (from_remote && (event == EVENT_OPEN_CASE) && (remote_loc_pre == BUD_LOC_UNKNOWN))
            {
                app_durian_adv_purpose_set_open_case();
            }
            else
            {
                /*inbox adv trig only for inbox action*/
                if ((((local_loc_pre == BUD_LOC_IN_AIR) || (local_loc_pre == BUD_LOC_IN_EAR)) &&
                     (durian_db.local_loc == BUD_LOC_IN_CASE)) ||
                    (((remote_loc_pre == BUD_LOC_IN_AIR) || (remote_loc_pre == BUD_LOC_IN_EAR)) &&
                     (durian_db.remote_loc == BUD_LOC_IN_CASE)))
                {
                    app_durian_adv_in_box();
                }
                need_report = true;
            }
        }
        else
        {
            app_durian_adv_out_box();
            need_report = true;
        }

        if (app_cfg_const.disallow_avp_advertising != 0)
        {
            need_report = true;
        }

        if (need_report)
        {
            app_durian_adp_batt_report();
        }
    }

bud_changed_update_pre:
    APP_PRINT_INFO8("app_durian_loc_changed: local_loc %d->%d remote_loc %d->%d local_loc avp %d remote_loc avp %d b2b_is_conn %d remote_loc_received %d",
                    local_loc_pre, app_db.local_loc, remote_loc_pre, app_db.remote_loc, durian_db.local_loc,
                    durian_db.remote_loc, app_db.remote_session_state, durian_db.remote_loc_received);
    local_loc_pre = (T_BUD_LOCATION)durian_db.local_loc;
    remote_loc_pre = (T_BUD_LOCATION)durian_db.remote_loc;
}
#else
static void app_durian_loc_changed_single(void)
{
    static T_BUD_LOCATION local_loc_pre = BUD_LOC_UNKNOWN;

    if (app_cfg_const.sensor_support)
    {
        durian_db.local_loc = (app_db.local_loc != BUD_LOC_UNKNOWN) ? app_db.local_loc :
                              durian_db.local_loc;
    }
    else
    {
        if (app_db.local_loc != BUD_LOC_UNKNOWN)
        {
            durian_db.local_loc = (app_db.local_loc == BUD_LOC_IN_CASE) ? BUD_LOC_IN_CASE : BUD_LOC_IN_EAR;
        }
    }

    if (((local_loc_pre != BUD_LOC_IN_CASE) && (durian_db.local_loc == BUD_LOC_IN_CASE)) ||
        ((local_loc_pre == BUD_LOC_IN_CASE) && (durian_db.local_loc != BUD_LOC_IN_CASE)))
    {
        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_IN_BOX, app_cfg_const.timer_auto_power_off);
    }

#if DURIAN_AIRMAX
    if ((local_loc_pre == BUD_LOC_IN_CASE) && (durian_db.local_loc != BUD_LOC_IN_CASE))
    {
#if F_APP_LISTENING_MODE_SUPPORT
        app_listening_rtk_out_case();
#endif
    }

    if ((local_loc_pre != BUD_LOC_IN_CASE) && (durian_db.local_loc == BUD_LOC_IN_CASE))
    {
#if F_APP_LISTENING_MODE_SUPPORT
        app_listening_rtk_in_case();
#endif
    }
#endif

    app_durian_loc_change_debounce();

    if (durian_db.ear_detect_en != 0)
    {
        if (local_loc_pre != durian_db.local_loc)
        {
            app_durian_loc_report(durian_db.local_loc, (durian_db.remote_loc));
        }
    }
    APP_PRINT_INFO3("app_durian_loc_changed_single: local_loc %d local_loc_pre %d local_loc avp %d",
                    app_db.local_loc, local_loc_pre, durian_db.local_loc);
    local_loc_pre = (T_BUD_LOCATION)durian_db.local_loc;
}
#endif

static void app_durian_loc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_durian_loc_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_BUD_CHANGE_DEBOUNCE:
        {
            app_stop_timer(&timer_idx_bud_change_debounce);
#if DURIAN_AIRMAX
            app_durian_anc_switch_check();
#else
            if (durian_db.remote_loc_received)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_durian_audio_spk_check();
                }
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_durian_anc_switch_check();
                }
#endif
            }
#endif
        }
        break;

    case APP_TIMER_LOC_REPORT:
        {
            app_stop_timer(&timer_idx_loc_report);
            app_durian_loc_report(durian_db.local_loc, durian_db.remote_loc);
        }
        break;

    default:
        break;
    }
}

bool app_durian_loc_get_action(uint8_t *action, uint8_t event)
{
    bool action_is_get = true;
    uint8_t app_idx = 0;
    *action = APP_AUDIO_RESULT_NOTHING;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return action_is_get;
    }

    if (app_audio_get_bud_stream_state() == BUD_STREAM_STATE_VOICE)
    {
        app_idx = app_hfp_get_active_idx();
    }
    else
    {
        app_idx = app_a2dp_get_active_idx();
    }

    if (((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) == 0) &&
        (app_cfg_const.enable_auto_a2dp_sco_control_for_android != 0))
    {
        if (((event == EVENT_IN_EAR) || (event == EVENT_OUT_EAR) ||
             (event == EVENT_IN_CASE)) &&
            (durian_db.ear_detect_en == 0) && (durian_db.fast_pair_connected != 0))
        {
            *action = APP_AUDIO_RESULT_NOTHING;
        }
        else
        {
            action_is_get = false;
        }
    }

    return action_is_get;
}

static void app_durian_loc_event_cback(uint32_t event, void *msg)
{
    bool from_remote = *((bool *)msg);

    if (from_remote)
    {
        durian_db.remote_loc_received = true;
    }

    switch (event)
    {
    case EVENT_CLOSE_CASE:
        {
            if (from_remote)
            {
                app_durian_adv_close_case(true);
            }
        }
        break;

    default:
        break;
    }

    app_durian_loc_changed(event, from_remote);

    APP_PRINT_TRACE2("app_durian_loc_event_cback: event_type 0x%02x, from_remote %d",
                     event, from_remote);
}

void app_durian_loc_init(void)
{
    app_timer_reg_cb(app_durian_loc_timeout_cb, &app_durian_loc_timer_id);
    app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_durian_loc_event_cback);
}
#endif
