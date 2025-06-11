/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include "string.h"
#include "trace.h"
#include "os_mem.h"
#include "os_sched.h"
#include "app_timer.h"
#include "btm.h"
#include "audio.h"
#if F_APP_ERWS_SUPPORT
#include "remote.h"
#endif
#include "bt_bond.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "audio_probe.h"
#include "app_main.h"
#include "app_report.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "app_link_util.h"
#include "app_sdp.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_audio_policy.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "audio_volume.h"
#include "app_mmi.h"
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#include "app_relay.h"
#include "app_bond.h"
#include "app_line_in.h"
#include "app_auto_power_off.h"
#include "audio_track.h"
#include "app_multilink.h"
#include "app_cmd.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif
#endif
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif
#endif
#include "app_sensor.h"
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#include "app_harman_ble_ota.h"
#include "sidetone.h"
#endif

typedef enum
{
    APP_HFP_TIMER_AUTO_ANSWER_CALL  = 0x00,
    APP_HFP_TIMER_RING              = 0x01,
    APP_HFP_TIMER_MIC_MUTE_ALARM    = 0x02,
    APP_HFP_TIMER_NO_SERVICE        = 0x03,
    APP_HFP_TIMER_ADD_SCO           = 0x04,
    APP_HFP_TIMER_CANCEL_VOICE_DAIL = 0x05,
    APP_HFP_TIMER_SET_VOL_IS_CHANGED = 0x06,
    APP_HFP_TIMER_DELAY_ROLE_SWITCH = 0x07,
} T_APP_HFP_TIMER;

static uint8_t hfp_timer_id = 0;
static uint8_t timer_idx_hfp_auto_answer_call = 0;
uint8_t timer_idx_hfp_ring = 0;
static uint8_t timer_idx_mic_mute_alarm = 0;
static uint8_t timer_idx_hfp_no_service = 0;
static uint8_t timer_idx_hfp_add_sco = 0;
static uint8_t timer_idx_iphone_voice_dail = 0;
static uint8_t timer_idx_hfp_set_vol_is_changed = 0;
static uint8_t timer_handle_hfp_delay_role_switch = NULL;
#if F_APP_ERWS_SUPPORT
static uint32_t hfp_auto_answer_call_start_time;
#endif
static uint32_t hfp_auto_answer_call_interval;

static T_APP_CALL_STATUS app_call_status;
static uint8_t active_hf_idx = 0;
static bool hf_ring_active = false;

static bool app_hfp_vol_change_ongoing = false;
static T_BT_EVENT_PARAM_HFP_SPK_VOLUME_CHANGED hfp_spk_vol_set = {0};
static T_BT_EVENT_PARAM_HFP_SPK_VOLUME_CHANGED hfp_spk_vol_set_cur = {0};

uint8_t app_hfp_hfp_ring_timer_check(void)
{
    return timer_idx_hfp_ring;
}

void app_hfp_set_vol_change_ongoing(bool vol_change_ongoing)
{
    app_hfp_vol_change_ongoing = vol_change_ongoing;
}

void app_hfp_voice_nr_enable(void)
{
    uint8_t enable;

    enable = (app_hfp_get_call_status() >= APP_CALL_ACTIVE) ? 1 : 0;
    if (app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                           APP_REMOTE_MSG_VOICE_NR_SWITCH,
                                           &enable, 1, REMOTE_TIMER_HIGH_PRECISION, 0, false) == false)
    {
        audio_probe_dsp_ipc_send_call_status(enable);
    }
}

void app_hfp_inband_tone_mute_ctrl(void)
{
    uint8_t cmd_ptr[9];
    T_APP_BR_LINK *p_link;

    app_hfp_vol_change_ongoing = false;

    p_link = &(app_db.br_link[active_hf_idx]);
    if (p_link != NULL)
    {
        if (p_link->connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
        {
            if (p_link->call_status == APP_CALL_INCOMING)
            {
                p_link->voice_muted = true;
            }
            else
            {
                p_link->voice_muted = false;
            }

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                memcpy(&cmd_ptr[0], app_db.br_link[active_hf_idx].bd_addr, 6);
                cmd_ptr[6] = AUDIO_STREAM_TYPE_VOICE;
                cmd_ptr[7] = p_link->voice_muted;
                cmd_ptr[8] = app_hfp_vol_change_ongoing;

                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                   APP_REMOTE_MSG_INBAND_TONE_MUTE_SYNC,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
            else
#endif
            {
                if (!p_link->voice_muted)
                {
                    audio_track_volume_out_unmute(p_link->sco_track_handle);
                }
            }
        }
    }

    APP_PRINT_INFO1("app_hfp_inband_tone_mute_ctrl voice_muted: %d", app_db.voice_muted);
}

static void app_hfp_ring_alert(T_APP_BR_LINK *p_link)
{
    T_APP_AUDIO_TONE_TYPE tone_type = TONE_TYPE_MAX;

    tone_type = (T_APP_AUDIO_TONE_TYPE)app_hfp_get_call_in_tone_type(p_link);
#if F_APP_MUTILINK_VA_PREEMPTIVE
    if ((app_cfg_const.enable_multi_link) &&
        (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
    {
        if (app_cfg_const.enable_multi_sco_disc_resume)
        {
            app_multi_pause_inactive_a2dp_link_stream(p_link->id, true);
        }
        else
        {
            app_multi_pause_inactive_a2dp_link_stream(p_link->id, false);
        }
    }

    if ((app_cfg_const.enable_multi_link) &&
        (p_link->id != active_hf_idx))
    {
        if (app_cfg_const.enable_multi_outband_call_tone == 0)
        {
            hf_ring_active = false;
        }
    }
#endif

    if (hf_ring_active)
    {
        app_audio_tone_type_play(tone_type, false, true);

        if (app_cfg_const.timer_hfp_ring_period != 0)
        {
            app_start_timer(&timer_idx_hfp_ring, "hfp_ring",
                            hfp_timer_id, APP_HFP_TIMER_RING, (uint8_t)tone_type, true,
                            app_cfg_const.timer_hfp_ring_period * 1000);
        }
        else
        {
            hf_ring_active = false;
        }
    }

#if F_APP_HARMAN_FEATURE_SUPPORT
    if (timer_idx_hfp_ring != NULL)
    {
        app_harman_sco_status_notify();
    }
#endif
}

void app_hfp_always_playing_outband_tone(void)
{
    T_APP_BR_LINK *p_link;

    p_link = &(app_db.br_link[active_hf_idx]);

    if (p_link != NULL)
    {
        app_hfp_inband_tone_mute_ctrl();

        if (p_link->call_status == APP_CALL_INCOMING)
        {
            hf_ring_active = true;
            app_hfp_ring_alert(p_link);
        }

        APP_PRINT_INFO2("app_hfp_always_playing_outband_tone: active_hf_idx %d, call_status %d",
                        active_hf_idx,
                        p_link->call_status);
    }
}

void app_hfp_update_call_status(void)
{
    uint8_t i = 0;
    uint8_t inactive_hf_idx = 0;
    uint8_t exchange_active_inactive_index_fg = 0;
    uint8_t call_status_old;
    T_APP_CALL_STATUS active_hf_status = APP_CALL_IDLE;
    T_APP_CALL_STATUS inactive_hf_status = APP_CALL_IDLE;

    call_status_old = app_hfp_get_call_status();

    active_hf_status = (T_APP_CALL_STATUS)app_db.br_link[active_hf_idx].call_status;

    APP_PRINT_INFO3("app_hfp_update_call_status: call_status_old 0x%04x, active_hf_status 0x%04x, active_hf_idx 0x%04x",
                    call_status_old, active_hf_status, active_hf_idx);

    //find inactive device
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (i != active_hf_idx)
        {
            if ((app_db.br_link[i].connected_profile & HFP_PROFILE_MASK) ||
                (app_db.br_link[i].connected_profile & HSP_PROFILE_MASK)
#if F_APP_GAMING_DONGLE_SUPPORT
                || (app_link_check_dongle_link(app_db.br_link[i].bd_addr))
#endif
               )
            {
                inactive_hf_idx = i;
                inactive_hf_status = (T_APP_CALL_STATUS)app_db.br_link[inactive_hf_idx].call_status;
                break;
            }
        }
    }

    if (active_hf_status == APP_CALL_IDLE)
    {
        if (inactive_hf_status != APP_CALL_IDLE)
        {
            exchange_active_inactive_index_fg = 1;
            if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
            {
                if (app_link_get_sco_conn_num() &&
                    (inactive_hf_status == APP_CALL_INCOMING) &&
                    (app_db.br_link[inactive_hf_idx].sco_track_handle != NULL))
                {
                    audio_track_start(app_db.br_link[inactive_hf_idx].sco_track_handle);
                }
            }
        }
    }
#if F_APP_MUTILINK_VA_PREEMPTIVE
    else if (app_cfg_const.enable_multi_link)
    {
        if ((active_hf_status != APP_CALL_ACTIVE) &&
            (active_hf_status != APP_CALL_INCOMING) &&
            (active_hf_status != APP_CALL_ACTIVE_WITH_CALL_HELD) &&
            (active_hf_status != APP_CALL_ACTIVE_WITH_CALL_WAITING))
        {
            if ((inactive_hf_status == APP_CALL_ACTIVE) ||
                (inactive_hf_status == APP_CALL_INCOMING) ||
                (inactive_hf_status == APP_CALL_ACTIVE_WITH_CALL_HELD) ||
                (inactive_hf_status == APP_CALL_ACTIVE_WITH_CALL_WAITING))
            {
                exchange_active_inactive_index_fg = 1;
            }
            else if (inactive_hf_status == APP_VOICE_ACTIVATION_ONGOING)
            {
                if (active_hf_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    exchange_active_inactive_index_fg = 1;
                }
            }
        }

#if F_APP_CALL_HOLD_SUPPORT
        if (active_hf_status == APP_CALL_HELD)
        {
            if (inactive_hf_status != APP_CALL_IDLE &&
                inactive_hf_status != APP_CALL_HELD)
            {
                exchange_active_inactive_index_fg = 1;
            }
        }
#endif
    }
#endif
    else if ((active_hf_status != APP_CALL_ACTIVE) &&
             (active_hf_status != APP_CALL_ACTIVE_WITH_CALL_HELD) &&
             (active_hf_status != APP_CALL_ACTIVE_WITH_CALL_WAITING))
    {
        if ((inactive_hf_status == APP_CALL_ACTIVE) ||
            (inactive_hf_status == APP_CALL_ACTIVE_WITH_CALL_HELD) ||
            (inactive_hf_status == APP_CALL_ACTIVE_WITH_CALL_WAITING))
        {
            exchange_active_inactive_index_fg = 1;
        }
    }

    if (exchange_active_inactive_index_fg)
    {
        uint8_t i;
        //exchange index
        i = inactive_hf_idx;
        inactive_hf_idx = active_hf_idx;
        app_hfp_set_active_idx(app_db.br_link[i].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
        app_bond_set_priority(app_db.br_link[i].bd_addr);
#endif
        //exchange status
        i = inactive_hf_status;
        inactive_hf_status = active_hf_status;
        active_hf_status = (T_APP_CALL_STATUS)i;
    }

    //update app_call_status
    switch (active_hf_status)
    {
    case APP_CALL_INCOMING:
        if (inactive_hf_status >= APP_CALL_ACTIVE)
        {
            app_hfp_set_call_status(APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT);
        }
        else
        {
            app_hfp_set_call_status(APP_CALL_INCOMING);
        }
        break;

    case APP_CALL_ACTIVE:
        {
            if (inactive_hf_status >= APP_CALL_ACTIVE)
            {
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                if (app_db.br_link[inactive_hf_idx].sco_handle)
                {
                    app_multilink_sco_preemptive(inactive_hf_idx);
                }
#endif
                app_hfp_set_call_status(APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD);
            }
            else if (inactive_hf_status == APP_CALL_INCOMING)
            {
                app_hfp_set_call_status(APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT);
            }
            else
            {
                app_hfp_set_call_status(APP_CALL_ACTIVE);
            }
        }
        break;

    default:
        app_hfp_set_call_status(active_hf_status);
        break;
    }

    APP_PRINT_INFO5("app_hfp_update_call_status: call_status 0x%04x, active_hf_status 0x%04x, mac : %s,"
                    "inactive_hf_status 0x%04x, inactive_hf_idx 0x%04x",
                    app_hfp_get_call_status(), active_hf_status,
                    TRACE_BDADDR(app_db.br_link[active_hf_idx].bd_addr), inactive_hf_status,
                    inactive_hf_idx);

#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_harman_ble_ota_get_upgrate_status() && (app_hfp_get_call_status() != APP_CALL_IDLE))
    {
        app_harman_ota_exit(OTA_EXIT_REASON_CALL);
    }

    if ((app_hfp_sco_is_connected() == false && app_hfp_get_call_status() == BT_HFP_CALL_IDLE)
        && (app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == false))
    {
        app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, active_hf_idx);
    }
    else
    {
        app_harman_connect_idle_to_power_off(ACTIVE_NEED_STOP_COUNT, active_hf_idx);
    }
#endif

    // app_start_timer(&timer_handle_hfp_delay_role_switch, "hfp_delay_role_switch",
    //                 hfp_timer_id, APP_HFP_TIMER_DELAY_ROLE_SWITCH, false,
    //                 inactive_hf_idx, 3000);

#if F_APP_LEA_SUPPORT
    mtc_legacy_update_call_status(&call_status_old);
#endif

#if F_APP_MUTILINK_VA_PREEMPTIVE
#if F_APP_ERWS_SUPPORT
    if ((app_cfg_const.enable_multi_link) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        app_relay_async_single(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC);
    }
#endif
#endif

    if (app_cfg_const.enable_multi_sco_disc_resume)
    {
        if ((app_cfg_const.enable_multi_link) &&
            (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
        {
            APP_PRINT_TRACE4("call_idle_resume %d,%d,%d,%d", app_link_get_sco_conn_num(),
                             active_hf_status,
                             inactive_hf_status,
                             app_hfp_get_call_status());
            if ((app_link_get_sco_conn_num() == 0) &&
                (active_hf_status == APP_CALL_IDLE) &&
                inactive_hf_status == APP_CALL_IDLE)
            {
                app_multi_resume_a2dp_link_stream(active_hf_idx);
            }
        }
    }

    if (call_status_old != app_hfp_get_call_status())
    {
#if F_APP_ERWS_SUPPORT
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
            (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_CALL_STATUS);
        }
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
        if (!app_slide_switch_mic_mute_toggle_support())
#endif
        {
            if (app_hfp_get_call_status() == APP_CALL_IDLE)
            {
                if (app_audio_is_mic_mute())
                {
                    app_audio_set_mic_mute_status(0);
                    audio_volume_in_unmute(AUDIO_STREAM_TYPE_VOICE);
                }
            }
        }

        if ((call_status_old == APP_CALL_INCOMING) ||
            (call_status_old == APP_CALL_ACTIVE_WITH_CALL_WAITING) ||
            (call_status_old == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT))
        {
            T_APP_BR_LINK *p_link;
            T_APP_AUDIO_TONE_TYPE tone_type = TONE_TYPE_MAX;

            if (exchange_active_inactive_index_fg)
            {
                p_link = &(app_db.br_link[inactive_hf_idx]);
            }
            else
            {
                p_link = &(app_db.br_link[active_hf_idx]);
            }

            tone_type = (T_APP_AUDIO_TONE_TYPE)app_hfp_get_call_in_tone_type(p_link);
            app_audio_tone_type_cancel(tone_type, true);
        }

#if F_APP_LISTENING_MODE_SUPPORT
        app_listening_mode_call_status_update();
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
        app_multilink_customer_hfp_status_changed(call_status_old, app_db.br_link[active_hf_idx].bd_addr);
#else
#if F_APP_LINEIN_SUPPORT
        if (app_cfg_const.line_in_support)
        {
            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
                app_line_in_call_status_update(app_hfp_get_call_status() == APP_CALL_IDLE);
            }
        }
#endif
#endif

        if (hf_ring_active && (active_hf_status != APP_CALL_INCOMING))
        {
            hf_ring_active = false;
        }

        if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
        {
            app_hfp_always_playing_outband_tone();
        }
    }

    app_hfp_voice_nr_enable();
}

void app_hfp_set_no_service_timer(bool all_service_status)
{
    app_stop_timer(&timer_idx_hfp_no_service);

    if (all_service_status == false)
    {
        app_start_timer(&timer_idx_hfp_no_service, "hfp_no_service",
                        hfp_timer_id, APP_HFP_TIMER_NO_SERVICE, 0, true,
                        app_cfg_const.timer_no_service_warning * 1000);
    }
}

static void app_hfp_check_service_status()
{
    if (app_cfg_const.timer_no_service_warning > 0)
    {
        bool all_service_status = true;
        uint8_t i;

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE ||
            app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if ((app_db.br_link[i].connected_profile & HFP_PROFILE_MASK) &&
                    (app_link_check_b2s_link_by_id(i) == true))
                {
                    if (app_db.br_link[i].service_status == false)
                    {
                        all_service_status = false;
                        break;
                    }
                }
            }

            if ((all_service_status == false) &&
                (timer_idx_hfp_no_service == 0) &&
                ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) ||
                 (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)))
            {
                app_audio_tone_type_play(TONE_HF_NO_SERVICE, false, true);
            }

            if (((all_service_status == true) && (timer_idx_hfp_no_service != 0)) ||
                ((all_service_status == false) && (timer_idx_hfp_no_service == 0)))
            {
                app_hfp_set_no_service_timer(all_service_status);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_SYNC_SERVICE_STATUS,
                                                        (uint8_t *)&all_service_status, 1);
                }
            }
        }
    }
}

void app_hfp_batt_level_report(uint8_t *bd_addr)
{
    uint8_t batt_level = app_db.local_batt_level;
    bool need_report = false;

#if F_APP_ERWS_SUPPORT
    if ((app_cfg_const.enable_report_lower_battery_volume) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        if ((app_db.remote_batt_level < app_db.local_batt_level) &&
            (app_db.remote_batt_level != 0))
        {
            batt_level = app_db.remote_batt_level;
        }
    }
#endif

#if F_APP_DURIAN_SUPPORT
    T_APP_BR_LINK *p_link = app_link_find_b2s_link(bd_addr);
    if (p_link != NULL)
    {
        need_report = app_durian_adp_batt_need_report(p_link->id);
    }
#else
    if (app_db.hfp_report_batt)
    {
        if (app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE)
        {
            need_report = true;
        }
    }
#endif

    if (need_report)
    {
        bt_hfp_batt_level_report(bd_addr, batt_level);
    }
}

static void app_hfp_set_mic_mute(void)
{
    APP_PRINT_INFO2("app_hfp_set_mic_mute is_mic_mute %d idx %d", app_audio_is_mic_mute(),
                    active_hf_idx);

#if (F_APP_TEAMS_GLOBAL_MUTE_SUPPORT == 0)
    T_AUDIO_STREAM_TYPE stream_type = AUDIO_STREAM_TYPE_VOICE;

#if F_APP_GAMING_DONGLE_SUPPORT
    bool set_dongle_link = false;
    T_APP_BR_LINK *p_dongle_link = NULL;
    p_dongle_link = app_link_find_br_link(app_db.connected_dongle_addr);

    if ((p_dongle_link != NULL) && (active_hf_idx == p_dongle_link->id))
    {
        set_dongle_link = true;
    }
#endif
#endif

    if (app_audio_is_mic_mute())
    {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
        T_APP_BR_LINK *p_link = NULL;

        p_link = &(app_db.br_link[active_hf_idx]);
        if (p_link != NULL)
        {
            audio_track_volume_in_mute(p_link->sco_track_handle);
        }
        app_audio_tone_type_play(TONE_MIC_MUTE_ON, true, true);
#else
#if F_APP_GAMING_DONGLE_SUPPORT
        if (set_dongle_link)
        {
            app_dongle_volume_in_mute();
        }
        else
#endif
        {
            audio_volume_in_mute(stream_type);
        }
#endif
        if (app_cfg_const.timer_mic_mute_alarm != 0)
        {
            app_start_timer(&timer_idx_mic_mute_alarm, "mic_mute_alarm",
                            hfp_timer_id, APP_HFP_TIMER_MIC_MUTE_ALARM, 0, true,
                            app_cfg_const.timer_mic_mute_alarm * 1000);
        }
    }
    else
    {
        app_stop_timer(&timer_idx_mic_mute_alarm);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
        T_APP_BR_LINK *p_link = NULL;

        p_link = &(app_db.br_link[active_hf_idx]);
        if (p_link != NULL)
        {
            audio_track_volume_in_unmute(p_link->sco_track_handle);
        }
        app_audio_tone_type_play(TONE_MIC_MUTE_OFF, true, true);
#else
#if F_APP_GAMING_DONGLE_SUPPORT
        if (set_dongle_link)
        {
            app_dongle_volume_in_unmute();
        }
        else
#endif
        {
            audio_volume_in_unmute(stream_type);
        }
#endif
    }
}

#if F_APP_ERWS_SUPPORT
static void app_hfp_transfer_sec_auto_answer_call_timer(void)
{
    uint32_t left_ms = 0;
    uint32_t run_ms = 0;

    app_stop_timer(&timer_idx_hfp_auto_answer_call);

    run_ms = os_sys_time_get() - hfp_auto_answer_call_start_time;

    if (hfp_auto_answer_call_interval > run_ms)
    {
        left_ms = hfp_auto_answer_call_interval - run_ms;
        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HFP, APP_REMOTE_HFP_AUTO_ANSWER_CALL_TIMER,
                                           (uint8_t *) &left_ms, 4, REMOTE_TIMER_HIGH_PRECISION, 0, false);
    }
}
#endif

static void app_hfp_spk_vol_set(uint8_t *bd_addr, uint8_t volume)
{
    T_APP_BR_LINK *p_link;
    p_link = app_link_find_br_link(bd_addr);

    memcpy(hfp_spk_vol_set.bd_addr, bd_addr, 6);
    hfp_spk_vol_set.volume = volume;
    hfp_spk_vol_set_cur = hfp_spk_vol_set;

    if (p_link != NULL)
    {
        app_hfp_vol_change_ongoing = true;

        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
        {
            uint8_t pair_idx_mapping;

            app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);

            app_cfg_nv.voice_gain_level[pair_idx_mapping] = (volume *
                                                             app_dsp_cfg_vol.voice_out_volume_max +
                                                             0x0f / 2) / 0x0f;

            app_audio_vol_set(p_link->sco_track_handle, app_cfg_nv.voice_gain_level[pair_idx_mapping]);

            if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
                (app_hfp_get_call_status() == APP_CALL_INCOMING))
            {
                // Not to do unmute
            }
            else
            {
                app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
            }
        }
        else
        {
            uint8_t cmd_ptr[8];
            uint8_t pair_idx_mapping;

            app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
            app_cfg_nv.voice_gain_level[pair_idx_mapping] = (volume *
                                                             app_dsp_cfg_vol.voice_out_volume_max +
                                                             0x0f / 2) / 0x0f;
            memcpy(&cmd_ptr[0], p_link->bd_addr, 6);
            cmd_ptr[6] = AUDIO_STREAM_TYPE_VOICE;
            cmd_ptr[7] = app_cfg_nv.voice_gain_level[pair_idx_mapping];
            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_HFP_VOLUME_SYNC,
                                               cmd_ptr, 8, REMOTE_TIMER_HIGH_PRECISION, 0, false);
        }
    }
}

void app_hfp_set_vol_is_changed(void)
{
    if ((memcmp(hfp_spk_vol_set_cur.bd_addr, hfp_spk_vol_set.bd_addr, 6) != 0) ||
        (hfp_spk_vol_set_cur.volume != hfp_spk_vol_set.volume))
    {
        app_hfp_spk_vol_set(hfp_spk_vol_set_cur.bd_addr, hfp_spk_vol_set_cur.volume);
    }
    else
    {
        app_hfp_vol_change_ongoing = false;
    }
    APP_PRINT_INFO5("app_hfp_set_vol_is_changed: bd_addr %s %s,vol %d %d ongoing %d",
                    TRACE_BDADDR(hfp_spk_vol_set_cur.bd_addr), TRACE_BDADDR(hfp_spk_vol_set.bd_addr),
                    hfp_spk_vol_set_cur.volume, hfp_spk_vol_set.volume, app_hfp_vol_change_ongoing);
}

static void app_hfp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_IND:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_hfp_bt_cback: no acl link found");
                return;
            }
            bt_hfp_connect_cfm(p_link->bd_addr, true);
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                uint8_t link_number;
                uint8_t pair_idx_mapping;

                p_link->call_id_type_check = true;
                p_link->call_id_type_num = false;

                app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
                bt_hfp_speaker_gain_level_report(p_link->bd_addr, app_cfg_nv.voice_gain_level[pair_idx_mapping]);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                if (p_link->is_mute)
                {
                    bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x00);
                }
                else
                {
                    bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x0f);
                }
#else
                bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x0a);
#endif

                app_hfp_batt_level_report(p_link->bd_addr);

                link_number = app_link_get_b2s_link_num_with_profile(HFP_PROFILE_MASK | HSP_PROFILE_MASK);
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                app_teams_multilink_handle_voice_profile_connect(p_link->id);
#if F_APP_USB_AUDIO_SUPPORT
                if ((link_number == 1) && !app_usb_audio_is_active())
#else
                if (link_number == 1)
#endif
                {
                    app_teams_multilink_handle_first_voice_profile_connect((app_link_find_br_link(
                                                                                p_link->bd_addr))->id);
                }
#else
#if F_APP_HARMAN_FEATURE_SUPPORT
                uint8_t find_another_hfp_idx = MAX_BR_LINK_NUM;

                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        if (i != p_link->id)
                        {
                            find_another_hfp_idx = i;
                        }
                    }
                }
#endif
                if (link_number == 1)
                {
#if F_APP_GAMING_DONGLE_SUPPORT
                    if ((!app_cfg_const.enable_multi_link) || (!(app_db.remote_is_dongle &&
                                                                 app_hfp_get_active_idx() != p_link->id)))
                    {
                        app_hfp_set_active_idx(p_link->bd_addr);
                        app_bond_set_priority(p_link->bd_addr);
                    }
#else
                    app_hfp_set_active_idx(p_link->bd_addr);
                    app_bond_set_priority(p_link->bd_addr);
#endif
                }
#if F_APP_HARMAN_FEATURE_SUPPORT
                else if (find_another_hfp_idx != MAX_BR_LINK_NUM)
                {
                    APP_PRINT_TRACE4("BT_EVENT_HFP_CONN_CMPL: active_a2dp_idx: %d, %d, find_another_hfp_idx: %d, %d",
                                     p_link->id, app_db.br_link[p_link->id].streaming_fg,
                                     find_another_hfp_idx, app_db.br_link[find_another_hfp_idx].streaming_fg);
                    if (app_db.br_link[find_another_hfp_idx].streaming_fg ||
                        app_db.br_link[find_another_hfp_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        app_bond_set_priority(p_link->bd_addr);
                        app_bond_set_priority(app_db.br_link[find_another_hfp_idx].bd_addr);
                    }
                    else
                    {
                        app_bond_set_priority(app_db.br_link[find_another_hfp_idx].bd_addr);
                        app_bond_set_priority(p_link->bd_addr);
                    }
                }
#endif
#endif
                if (app_db.br_link[app_db.first_hf_index].app_hf_state == APP_HF_STATE_STANDBY)
                {
                    app_db.first_hf_index = p_link->id;
                }
                else
                {
                    app_db.last_hf_index = p_link->id;
                }
                p_link->app_hf_state = APP_HF_STATE_CONNECTED;
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_ACTIVATION:
        {
            if (param->hfp_voice_recognition_activation.result == BT_HFP_CMD_OK)
            {
                T_APP_BR_LINK *p_link;

                p_link = app_link_find_br_link(param->hfp_voice_recognition_activation.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->call_status == APP_CALL_IDLE)
                    {
                        p_link->call_status = APP_VOICE_ACTIVATION_ONGOING;
                    }

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    app_teams_multilink_handle_voice_single_event(p_link->id,
                                                                  (T_APP_CALL_STATUS)p_link->call_status);
#else
                    app_hfp_update_call_status();
#endif
#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
                    //Workaround: iOS may not create SCO and not issue +BVRA:0 when trigger siri repeatedly
                    //Add a protection timer to exit voice activation state
                    if (p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS)
                    {
                        app_start_timer(&timer_idx_iphone_voice_dail, "cancel_iphone_voice_dail",
                                        hfp_timer_id, APP_HFP_TIMER_CANCEL_VOICE_DAIL, false,
                                        p_link->id, 1000);
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_DEACTIVATION:
        {
            if (param->hfp_voice_recognition_deactivation.result == BT_HFP_CMD_OK)
            {
                T_APP_BR_LINK *p_link;

                p_link = app_link_find_br_link(param->hfp_voice_recognition_deactivation.bd_addr);
                if (p_link != NULL)
                {
                    if (p_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        p_link->call_status = APP_CALL_IDLE;
                    }

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    app_teams_multilink_handle_voice_single_event(p_link->id,
                                                                  (T_APP_CALL_STATUS)p_link->call_status);
#else
                    app_hfp_update_call_status();
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            T_APP_BR_LINK *p_link;
#if (C_APP_END_OUTGOING_CALL_PLAY_CALL_END_TONE == 0)
            uint8_t temp_idx = active_hf_idx;
#endif
            p_link = app_link_find_br_link(param->hfp_call_status.bd_addr);
            if (p_link != NULL)
            {
                switch (param->hfp_call_status.curr_status)
                {
                case BT_HFP_CALL_IDLE:
                    {
                        p_link->call_status = APP_CALL_IDLE;
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        p_link->call_hold_status = APP_HFP_CALL_HELD_STATUS_IDLE;
#endif
                    }
                    break;

                case BT_HFP_CALL_INCOMING:
                    {
                        p_link->call_status = APP_CALL_INCOMING;
                    }
                    break;

                case BT_HFP_CALL_OUTGOING:
                    {
                        p_link->call_status = APP_CALL_OUTGOING;
                    }
                    break;

                case BT_HFP_CALL_ACTIVE:
                    {
                        p_link->call_status = APP_CALL_ACTIVE;
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        p_link->call_hold_status = APP_HFP_CALL_HELD_STATUS_IDLE;
#endif
                    }
                    break;

                case BT_HFP_CALL_HELD:
                    {
#if F_APP_CALL_HOLD_SUPPORT
                        p_link->call_status = APP_CALL_HELD;
#endif

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        p_link->call_hold_status = APP_HFP_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL;
#endif
                    }
                    break;

                case BT_HFP_CALL_ACTIVE_WITH_CALL_WAITING:
                    {
                        p_link->call_status = APP_CALL_ACTIVE_WITH_CALL_WAITING;
                    }
                    break;

                case BT_HFP_CALL_ACTIVE_WITH_CALL_HELD:
                    {
                        p_link->call_status = APP_CALL_ACTIVE_WITH_CALL_HELD;
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        p_link->call_hold_status = APP_HFP_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL;
#endif
                    }
                    break;

                default:
                    break;
                }

                if ((app_cfg_const.enable_auto_answer_incoming_call == 1) &&
                    (p_link->call_status == APP_CALL_INCOMING))
                {
#if F_APP_ERWS_SUPPORT
                    hfp_auto_answer_call_start_time = os_sys_time_get();
#endif
                    hfp_auto_answer_call_interval = app_cfg_const.timer_hfp_auto_answer_call * 1000;
                    app_start_timer(&timer_idx_hfp_auto_answer_call, "auto_answer_call",
                                    hfp_timer_id, APP_HFP_TIMER_AUTO_ANSWER_CALL, p_link->id, false,
                                    hfp_auto_answer_call_interval);
                }

                if (p_link->call_status == APP_CALL_IDLE)
                {
                    p_link->call_id_type_check = true;
                    p_link->call_id_type_num = false;
                }

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                app_teams_multilink_handle_voice_single_event(p_link->id,
                                                              (T_APP_CALL_STATUS)p_link->call_status);
#else
                app_hfp_update_call_status();
#endif

#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
                //Workaround: iOS may not create SCO and not issue +BVRA:0 when trigger siri repeatedly
                //Add a protection timer to exit voice activation state
                if ((p_link->call_status == APP_VOICE_ACTIVATION_ONGOING) &&
                    (p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS))
                {
                    app_start_timer(&timer_idx_iphone_voice_dail, "cancel_iphone_voice_dail",
                                    hfp_timer_id, APP_HFP_TIMER_CANCEL_VOICE_DAIL, p_link->id, false,
                                    1000);
                }
#endif

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    if (param->hfp_call_status.prev_status == BT_HFP_CALL_INCOMING &&
                        param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
                    {
                        if (app_db.reject_call_by_key)
                        {
                            app_db.reject_call_by_key = false;
                            app_audio_tone_type_play(TONE_HF_CALL_REJECT, false, true);
                        }
                    }

                    if (p_link->id == temp_idx &&
                        param->hfp_call_status.prev_status == BT_HFP_CALL_ACTIVE &&
                        param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
                    {
                        app_audio_tone_type_play(TONE_HF_CALL_END, false, true);
                    }

                    if (p_link->id == active_hf_idx &&
                        param->hfp_call_status.prev_status != BT_HFP_CALL_ACTIVE &&
                        param->hfp_call_status.curr_status == BT_HFP_CALL_ACTIVE)
                    {
                        app_audio_tone_type_play(TONE_HF_CALL_ACTIVE, false, true);
                    }

                    if (param->hfp_call_status.prev_status != BT_HFP_CALL_OUTGOING &&
                        param->hfp_call_status.curr_status == BT_HFP_CALL_OUTGOING)
                    {
                        app_audio_tone_type_play(TONE_HF_OUTGOING_CALL, false, true);
                    }
                }
            }

            if (param->hfp_call_status.curr_status == BT_HFP_CALL_ACTIVE ||
                param->hfp_call_status.curr_status == BT_HFP_CALL_INCOMING ||
                param->hfp_call_status.curr_status == BT_HFP_CALL_OUTGOING)
            {
                if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
                {
                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_VOICE);
                }

                app_audio_set_bud_stream_state(BUD_STREAM_STATE_VOICE);
            }
            else if (param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
            {
                if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
                {
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_VOICE,
                                              app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                }

                app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
            }

            if (param->hfp_call_status.curr_status == BT_HFP_CALL_ACTIVE)
            {
#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
                if (LIGHT_SENSOR_ENABLED &&
                    ((app_db.local_loc == BUD_LOC_IN_EAR) || (app_db.remote_loc == BUD_LOC_IN_EAR)))
                {
                    app_stop_timer(&timer_idx_hfp_add_sco);
                    app_start_timer(&timer_idx_hfp_add_sco, "hfp_add_sco",
                                    hfp_timer_id, APP_HFP_TIMER_ADD_SCO, active_hf_idx, false,
                                    400);
                }
#endif
            }
        }
        break;

    case BT_EVENT_HFP_SERVICE_STATUS:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_service_status.bd_addr);

            if (p_link != NULL)
            {
                p_link->service_status = param->hfp_service_status.status;
                app_hfp_check_service_status();
            }
        }
        break;

    case BT_EVENT_HFP_CALL_WAITING_IND:
    case BT_EVENT_HFP_CALLER_ID_IND:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_APP_BR_LINK *br_link;
                T_APP_LE_LINK *le_link;
                char *number;
                uint16_t num_len;

                if (event_type == BT_EVENT_HFP_CALLER_ID_IND)
                {
                    br_link = app_link_find_br_link(param->hfp_caller_id_ind.bd_addr);
                    le_link = app_link_find_le_link_by_addr(param->hfp_caller_id_ind.bd_addr);
                    number = (char *)param->hfp_caller_id_ind.number;
                    num_len = strlen(param->hfp_caller_id_ind.number);
                }
                else
                {
                    br_link = app_link_find_br_link(param->hfp_call_waiting_ind.bd_addr);
                    le_link = app_link_find_le_link_by_addr(param->hfp_call_waiting_ind.bd_addr);
                    number = (char *)param->hfp_call_waiting_ind.number;
                    num_len = strlen(param->hfp_call_waiting_ind.number);
                }

                if (br_link != NULL)
                {
                    /* Sanity check if BR/EDR TTS session is ongoing */
                    if (br_link->cmd_set_enable == true &&
                        br_link->tts_info.tts_handle != NULL)
                    {
                        break;
                    }

                    /* Sanity check if BLE TTS session is ongoing */
                    if (le_link != NULL &&
                        le_link->cmd_set_enable == true &&
                        le_link->tts_info.tts_handle != NULL)
                    {
                        break;
                    }

                    if (br_link->call_id_type_check == true)
                    {
                        if (br_link->connected_profile & PBAP_PROFILE_MASK)
                        {
                            if (bt_pbap_vcard_listing_by_number_pull(br_link->bd_addr, number) == false)
                            {
                                br_link->call_id_type_check = false;
                                br_link->call_id_type_num = true;
                            }
                        }
                        else
                        {
                            br_link->call_id_type_check = false;
                            br_link->call_id_type_num = true;
                        }
                    }

                    if (br_link->call_id_type_check == false)
                    {
                        if (br_link->call_id_type_num == true)
                        {
                            uint8_t *data;
                            uint16_t len;
                            len = num_len + 3;
                            data = malloc(len);

                            if (data != NULL)
                            {
                                data[1] = CALLER_ID_TYPE_NUMBER;
                                data[2] = num_len;
                                memcpy(data + 3, number, num_len);

                                if (br_link->connected_profile & SPP_PROFILE_MASK)
                                {
                                    data[0] = br_link->id;
                                    app_report_event(CMD_PATH_SPP, EVENT_CALLER_ID, br_link->id, data, len);
                                }
                                else if (br_link->connected_profile & IAP_PROFILE_MASK)
                                {
                                    data[0] = br_link->id;
                                    app_report_event(CMD_PATH_IAP, EVENT_CALLER_ID, br_link->id, data, len);
                                }
                                else if (le_link != NULL)
                                {
                                    data[0] = le_link->id;
                                    app_report_event(CMD_PATH_LE, EVENT_CALLER_ID, le_link->id, data, len);
                                }

                                free(data);
                            }
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_RING_ALERT:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_ring_alert.bd_addr);
            if (p_link != NULL)
            {
                p_link->is_inband_ring = param->hfp_ring_alert.is_inband;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                T_APP_TEAMS_MULTILINK_APP_HIGH_PRIORITY_ARRAY_DATA *p_high_priority_app_data = NULL;
                p_high_priority_app_data = app_teams_multilink_find_app_high_priority_array_data_by_link_idx(
                                               p_link->id);
#endif

                if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call == false) &&
                    ((p_link->is_inband_ring == false) ||
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                     ((p_link->id != app_teams_multilink_get_active_voice_index()) && p_high_priority_app_data &&
                      !p_high_priority_app_data->incoming_call_state_get_active_link))) /* TODO check active sco link */
#else
                     (p_link->id != active_hf_idx))) /* TODO check active sco link */
#endif
                {
                    if (hf_ring_active == false && p_link->call_status == APP_CALL_INCOMING)
                    {
                        hf_ring_active = true;

                        app_hfp_ring_alert(p_link);
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_SPK_VOLUME_CHANGED:
        {
            if (!app_hfp_vol_change_ongoing ||
                (memcmp(hfp_spk_vol_set_cur.bd_addr, param->hfp_spk_volume_changed.bd_addr, 6) != 0))
            {
                app_hfp_spk_vol_set(param->hfp_spk_volume_changed.bd_addr, param->hfp_spk_volume_changed.volume);
            }
            else
            {
                hfp_spk_vol_set_cur = param->hfp_spk_volume_changed;
            }
        }
        break;

    case BT_EVENT_HFP_MIC_VOLUME_CHANGED:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->hfp_spk_volume_changed.bd_addr);
#if F_APP_TEAMS_FEATURE_SUPPORT
            if (p_link != NULL)
            {
                uint8_t teams_active_device_idx = active_hf_idx;

                if (teams_active_device_idx == 0xff)
                {
                    break;
                }

                if (param->hfp_mic_volume_changed.volume == 0)
                {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    p_link->is_mute = true;
#endif
                    if (p_link->id == teams_active_device_idx)
                    {
                        app_mmi_handle_action(MMI_DEV_MIC_MUTE);
                    }
                }
                else
                {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    p_link->is_mute = false;
#endif
                    if (p_link->id == teams_active_device_idx)
                    {
                        app_mmi_handle_action(MMI_DEV_MIC_UNMUTE);
                    }
                }
            }
#else
            if (p_link != NULL)
            {
                if (active_hf_idx == 0xff)
                {
                    break;
                }

                if (param->hfp_mic_volume_changed.volume == 0)
                {
                    if (p_link->id == active_hf_idx)
                    {
                        app_audio_set_mic_mute_status(1);
                        app_mmi_handle_action(MMI_DEV_MIC_MUTE);
                    }
                }
                else
                {
                    if (p_link->id == active_hf_idx)
                    {
                        app_audio_set_mic_mute_status(0);
                        app_mmi_handle_action(MMI_DEV_MIC_UNMUTE);
                    }
                }
            }
#endif
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_hfp_voice_nr_enable();
            }
        }
        break;
#endif

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (param->hfp_disconn_cmpl.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
                    //do nothing
                }
                else
                {
                    p_link->call_status = APP_CALL_IDLE;
                    p_link->app_hf_state = APP_HF_STATE_STANDBY;
                    if (app_db.first_hf_index == p_link->id)
                    {
                        app_db.first_hf_index = app_db.last_hf_index;
                    }

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    app_teams_multilink_handle_voice_profile_disconnect((app_link_find_br_link(
                                                                             param->hfp_disconn_cmpl.bd_addr))->id);
#else
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    uint8_t orig_hfp_idx = active_hf_idx;
#endif
                    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_db.br_link[i].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            app_hfp_set_active_idx(app_db.br_link[i].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                            app_bond_set_priority(app_db.br_link[i].bd_addr);
#endif
                            break;
                        }
                    }

                    if (app_hfp_get_call_status() != APP_CALL_IDLE)
                    {
                        app_hfp_update_call_status();
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (app_cfg_const.enable_multi_link &&
                            p_link->id == orig_hfp_idx &&
                            !app_multi_check_in_sniffing())
                        {
                            app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                        }
#endif
                    }
#endif
                }

                if ((param->hfp_disconn_cmpl.cause & ~HCI_ERR) != HCI_ERR_CONN_ROLESWAP)
                {
                    app_hfp_check_service_status();
                }
            }
        }
        break;

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
    case BT_EVENT_SCO_CONN_CMPL:
        {
            if (param->sco_conn_cmpl.cause == 0 && app_slide_switch_mic_mute_toggle_support())
            {
                app_hfp_mute_ctrl();
            }
        }
        break;
#endif

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_SCO_SNIFFING_CONN_CMPL:
        {
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            T_BT_EVENT_PARAM *param = event_buf;
            T_BT_ROLESWAP_STATUS event;
#if F_APP_GAMING_DONGLE_SUPPORT
            uint8_t app_idx = app_a2dp_get_active_idx();
#endif

            event = param->remote_roleswap_status.status;

            if (event == BT_ROLESWAP_STATUS_SUCCESS)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (app_audio_check_mic_mute_enable())
                    {
                        app_hfp_set_mic_mute();
                    }

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_db.dongle_is_enable_mic)
                    {
                        app_dongle_start_recording(app_db.br_link[app_idx].bd_addr);
                    }
#endif
                }
                else
                {
                    app_stop_timer(&timer_idx_mic_mute_alarm);
                    if (app_audio_check_mic_mute_enable())
                    {
#if F_APP_RWS_MULTI_SPK_SUPPORT
#else
                        audio_volume_in_mute(AUDIO_STREAM_TYPE_VOICE);
#endif
                    }

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_db.dongle_is_enable_mic)
                    {
                        app_transfer_queue_reset(CMD_PATH_SPP);
                        app_dongle_stop_recording(app_db.br_link[app_idx].bd_addr);
                    }
#endif
                    app_hfp_transfer_sec_auto_answer_call_timer();
                }
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
        APP_PRINT_INFO1("app_hfp_bt_cback: event_type 0x%04x", event_type);
    }
}

#if F_APP_ERWS_SUPPORT
static void app_hfp_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_hfp_parse_cback: msg %d status %d", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_HFP_SYNC_SCO_GAIN_WHEN_CALL_ACTIVE:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                uint8_t addr[6] = {0};
                uint8_t voice_gain = 0;
                memcpy(addr, buf, 6);
                voice_gain = buf[6];

                app_hfp_set_link_voice_gain(addr, voice_gain);
            }
        }
        break;

    case APP_REMOTE_HFP_AUTO_ANSWER_CALL_TIMER:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    T_APP_BR_LINK *p_link = NULL;
                    uint8_t app_idx;
                    uint32_t *left_ms = (uint32_t *)buf;

                    for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
                    {
                        if (app_link_check_b2s_link_by_id(app_idx))
                        {
                            p_link = &app_db.br_link[app_idx];
                            if (p_link->call_status == APP_CALL_INCOMING)
                            {
#if F_APP_ERWS_SUPPORT
                                hfp_auto_answer_call_start_time = os_sys_time_get();
#endif
                                hfp_auto_answer_call_interval = *left_ms;

                                app_start_timer(&timer_idx_hfp_auto_answer_call, "auto_answer_call",
                                                hfp_timer_id, APP_HFP_TIMER_AUTO_ANSWER_CALL, p_link->id, false,
                                                hfp_auto_answer_call_interval);
                                break;
                            }
                        }
                    }
                }
            }
        }

    default:
        break;
    }
}
#endif

T_APP_CALL_STATUS app_hfp_get_call_status(void)
{
    return app_call_status;
}

void app_hfp_set_call_status(T_APP_CALL_STATUS call_status)
{
    static T_APP_CALL_STATUS pre_call_status = APP_CALL_IDLE;
    uint8_t active_idx = active_hf_idx;
    uint8_t pair_idx_mapping = 0;
    bool get_pair_idx = app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr,
                                                      &pair_idx_mapping);

    APP_PRINT_TRACE2("app_hfp_set_call_status: %d -> %d", pre_call_status, call_status);

    app_call_status = call_status;

    if (call_status != pre_call_status)
    {
        app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_CALL_STATUS_CHAGNED);
    }

    if (app_cfg_const.fixed_inband_tone_gain && get_pair_idx == true)
    {
        uint8_t voice_gain = app_cfg_nv.voice_gain_level[pair_idx_mapping];

        if (pre_call_status == APP_CALL_OUTGOING ||
            pre_call_status == APP_CALL_INCOMING)
        {
            if (call_status == APP_CALL_ACTIVE)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
#if F_APP_ERWS_SUPPORT
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        uint8_t tmp[7] = {0};

                        memcpy(tmp, app_db.br_link[active_idx].bd_addr, 6);
                        tmp[6] = voice_gain;

                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HFP,
                                                           APP_REMOTE_HFP_SYNC_SCO_GAIN_WHEN_CALL_ACTIVE,
                                                           tmp, sizeof(tmp), REMOTE_TIMER_HIGH_PRECISION, 0, false);
                    }
                    else
#endif
                    {
                        app_hfp_set_link_voice_gain(app_db.br_link[active_idx].bd_addr, voice_gain);
                    }
                }
            }
        }
        else if (call_status == APP_CALL_OUTGOING ||
                 call_status == APP_CALL_INCOMING)
        {
            app_hfp_set_link_voice_gain(app_db.br_link[active_idx].bd_addr, app_cfg_const.inband_tone_gain_lv);
        }
    }

    pre_call_status = call_status;
}

uint8_t app_hfp_get_active_idx(void)
{
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    return app_teams_multilink_get_active_index();
#else
    return active_hf_idx;
#endif
}

void app_hfp_only_set_active_idx(uint8_t idx)
{
    active_hf_idx = idx;
}

bool app_hfp_sco_is_connected(void)
{
    return (app_db.br_link[active_hf_idx].sco_handle == 0) ? false : true;
}

bool app_hfp_set_link_voice_gain(uint8_t *addr, uint8_t gain)
{
    T_APP_BR_LINK *p_link = NULL;
    bool ret = false;

    p_link = app_link_find_br_link(addr);


    if (p_link && p_link->sco_track_handle)
    {
        app_audio_vol_set(p_link->sco_track_handle, gain);
        app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
        ret = true;
    }

    return ret;
}

bool app_hfp_set_active_idx(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link)
    {
        active_hf_idx = p_link->id;
#if F_APP_HARMAN_FEATURE_SUPPORT
        app_harman_aling_active_hfp_idx_set(active_hf_idx);
#endif
        if (p_link->sco_track_handle)
        {
            audio_track_start(p_link->sco_track_handle);
        }
        return bt_sco_link_switch(bd_addr);
    }
    return false;
}

void app_hfp_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_hfp_timeout_cb: timer_evt %d, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_HFP_TIMER_AUTO_ANSWER_CALL:
        {
            T_APP_BR_LINK *p_link;
            p_link = &(app_db.br_link[param]);
            app_stop_timer(&timer_idx_hfp_auto_answer_call);
            if (p_link->call_status == APP_CALL_INCOMING)
            {
                bt_hfp_call_answer_req(p_link->bd_addr);
            }
        }
        break;

    case APP_HFP_TIMER_RING:
        {
            T_APP_BR_LINK *link1 = NULL;
            T_APP_BR_LINK *link2 = NULL;
            bool           ring_trigger = false;
            uint8_t        i;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            T_APP_TEAMS_MULTILINK_APP_HIGH_PRIORITY_ARRAY_DATA *p_high_priority_app_data1 = NULL;
            T_APP_TEAMS_MULTILINK_APP_HIGH_PRIORITY_ARRAY_DATA *p_high_priority_app_data2 = NULL;
#endif
            T_APP_AUDIO_TONE_TYPE tone_type = (T_APP_AUDIO_TONE_TYPE) param;

            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_db.br_link[i].call_status == APP_CALL_INCOMING)
                {
                    link1 = &(app_db.br_link[i]);

                    i++;
                    break;
                }
            }

            for (; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_db.br_link[i].call_status == APP_CALL_INCOMING)
                {
                    link2 = &(app_db.br_link[i]);
                    break;
                }
            }

            if (link1 != NULL || link2 != NULL)
            {
                if (link1 != NULL)
                {
                    if (app_cfg_const.enable_multi_outband_call_tone)
                    {
                        if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) ||
                            (link1->is_inband_ring == false || link1->id != active_hf_idx)) /* TODO check active sco link */
                        {
                            ring_trigger = true;
                        }
                    }
                    else
                    {
                        if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) ||
                            ((link1->is_inband_ring == false) &&
                             (link1->id == active_hf_idx))) /* TODO check active sco link */
                        {
                            ring_trigger = true;
                        }
                    }
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    p_high_priority_app_data1 = app_teams_multilink_find_app_high_priority_array_data_by_link_idx(
                                                    link1->id);
                    if ((link1->id != app_teams_multilink_get_active_voice_index()) && p_high_priority_app_data1 &&
                        !p_high_priority_app_data1->incoming_call_state_get_active_link)
                    {
                        ring_trigger = true;
                    }
#endif
                }

                if (link2 != NULL)
                {
                    if (app_cfg_const.enable_multi_outband_call_tone)
                    {
                        if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) ||
                            (link2->is_inband_ring == false || link2->id != active_hf_idx)) /* TODO check active sco link */
                        {
                            ring_trigger = true;
                        }
                    }
                    else
                    {
                        if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) ||
                            ((link2->is_inband_ring == false) &&
                             (link2->id == active_hf_idx))) /* TODO check active sco link */
                        {
                            ring_trigger = true;
                        }
                    }
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    p_high_priority_app_data2 = app_teams_multilink_find_app_high_priority_array_data_by_link_idx(
                                                    link2->id);
                    if ((link2->id != app_teams_multilink_get_active_voice_index()) && p_high_priority_app_data2 &&
                        !p_high_priority_app_data2->incoming_call_state_get_active_link)
                    {
                        ring_trigger = true;
                    }
#endif
                }
            }

            if (ring_trigger)  // && (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY))
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                if (app_cfg_nv.language_status == 0)
                {
                    app_audio_tone_type_play(TONE_HARMAN_HF_CALL_IN, true, true);
                }
                else
#endif
                {
                    app_audio_tone_type_play(tone_type, true, true);
                }
            }
            else
            {
                app_stop_timer(&timer_idx_hfp_ring);
                hf_ring_active = false;
            }

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (timer_idx_hfp_ring == 0)
            {
                app_harman_sco_status_notify();
            }
#endif
        }
        break;

    case APP_HFP_TIMER_MIC_MUTE_ALARM:
        {
            T_APP_BR_LINK *p_link;

            p_link = &(app_db.br_link[active_hf_idx]);

            if (app_audio_is_mic_mute() &&
                ((p_link->call_status != APP_CALL_IDLE)
#if F_APP_GAMING_DONGLE_SUPPORT
                 || (app_db.dongle_is_enable_mic)
#endif
                ))
            {
                app_audio_tone_type_play(TONE_MIC_MUTE_ALARM, false, true);
            }
            else
            {
                app_stop_timer(&timer_idx_mic_mute_alarm);
            }
        }
        break;

    case APP_HFP_TIMER_NO_SERVICE:
        {
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
            {
                app_audio_tone_type_play(TONE_HF_NO_SERVICE, false, true);
            }
        }
        break;

    case APP_HFP_TIMER_ADD_SCO:
        {
            T_APP_BR_LINK *p_link;
            p_link = &(app_db.br_link[param]);

            app_stop_timer(&timer_idx_hfp_add_sco);
            if (!app_hfp_sco_is_connected())
            {
                //rebulid sco
                app_bt_sniffing_hfp_connect(p_link->bd_addr);
            }
        }
        break;

    case APP_HFP_TIMER_CANCEL_VOICE_DAIL:
        {
            T_APP_BR_LINK *p_link;
            p_link = &(app_db.br_link[active_hf_idx]);

            app_stop_timer(&timer_idx_iphone_voice_dail);
            if ((p_link->call_status == APP_VOICE_ACTIVATION_ONGOING) &&
                (p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                (app_hfp_sco_is_connected() == false))
            {
                bt_hfp_voice_recognition_disable_req(p_link->bd_addr);
            }
        }
        break;

    case APP_HFP_TIMER_DELAY_ROLE_SWITCH:
        {
            app_stop_timer(&timer_handle_hfp_delay_role_switch);
            app_bt_policy_roleswitch_when_sco(app_db.br_link[param].bd_addr);
        }
        break;

    case APP_HFP_TIMER_SET_VOL_IS_CHANGED:
        {
            app_stop_timer(&timer_idx_hfp_set_vol_is_changed);
            app_hfp_set_vol_is_changed();
        }
        break;

    default:
        break;
    }
}

static void app_hfp_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                T_APP_BR_LINK *p_link;

                p_link = app_link_find_br_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr);

                if ((p_link == NULL) || (p_link->sco_track_handle == NULL) ||
                    (param->track_state_changed.handle != p_link->sco_track_handle))
                {
                    break;
                }

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED ||
                    param->track_state_changed.state == AUDIO_TRACK_STATE_RESTARTED)
                {
                    if (app_hfp_get_call_status() >= APP_CALL_ACTIVE)
                    {
                        audio_probe_dsp_ipc_send_call_status(true);
                    }
                }
#if F_APP_HARMAN_FEATURE_SUPPORT
                //refine sidetone setting
                T_AUDIO_TRACK_STATE state;

                audio_track_state_get(p_link->sco_track_handle, &state);
                T_APP_BR_LINK *check_p_link;
                check_p_link = &app_db.br_link[app_hfp_get_active_idx()];

                APP_PRINT_TRACE7("[SD_CHECK] refine sidetone setting %d,%d,%d,%d, hfp_active=%d, call_status=%d,device_os=%d",
                                 app_cfg_nv.sidetone_switch,
                                 app_cfg_nv.sidetone_level,
                                 app_hfp_sco_is_connected(),
                                 param->track_state_changed.state,
                                 app_hfp_get_active_idx(),
                                 app_hfp_get_call_status(),
                                 check_p_link->remote_device_vendor_id);

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED) //sco
                {
                    if ((app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING)
                        //&&(check_p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS)
                       )
                    {
                        APP_PRINT_TRACE0("[SD_CHECK] VA not open sidetone2");
                    }
                    else
                    {
                        if (app_cfg_nv.sidetone_switch)
                        {
                            harman_sidetone_set_dsp();
                        }
                        else
                        {
                            if (p_link->sidetone_instance != NULL)
                            {
                                sidetone_disable(p_link->sidetone_instance);
                                sidetone_release(p_link->sidetone_instance);
                                p_link->sidetone_instance = NULL;
                            }
                        }
                    }
                }
#endif
            }
        }
        break;

    case AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_volume_out_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                app_stop_timer(&timer_idx_hfp_set_vol_is_changed);
                app_start_timer(&timer_idx_hfp_set_vol_is_changed, "hfp_set_vol_is_changed",
                                hfp_timer_id, APP_HFP_TIMER_SET_VOL_IS_CHANGED, 0, false,
                                1000);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_hfp_policy_cback: event_type 0x%04x", event_type);
    }
}

void app_hfp_init(void)
{
    if (app_cfg_const.supported_profile_mask & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
    {
        bt_hfp_init(app_cfg_const.hfp_link_number, RFC_HFP_CHANN_NUM,
                    RFC_HSP_CHANN_NUM, app_cfg_const.hfp_brsf_capability);
        audio_mgr_cback_register(app_hfp_audio_cback);
        bt_mgr_cback_register(app_hfp_bt_cback);

#if F_APP_ERWS_SUPPORT
        app_relay_cback_register(NULL, app_hfp_parse_cback,
                                 APP_MODULE_TYPE_HFP, APP_REMOTE_HFP_TOTAL);
#endif
        app_timer_reg_cb(app_hfp_timeout_cb, &hfp_timer_id);
    }
}

void app_hfp_mute_ctrl(void)
{
    uint8_t hf_active = 0;
    T_APP_BR_LINK *p_link;

    p_link = &(app_db.br_link[active_hf_idx]);

    if (p_link->app_hf_state == APP_HF_STATE_CONNECTED)
    {
        if (p_link->sco_handle != 0)
        {
            hf_active = 1;
        }
    }
    else
    {
        if (p_link->sco_handle)
        {
            hf_active = 1;
        }
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_db.dongle_is_enable_mic)
    {
        hf_active = 1;
    }
#endif

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (hf_active)
        {
            app_hfp_set_mic_mute();
            if (app_audio_is_mic_mute())
            {
#if F_APP_TEAMS_FEATURE_SUPPORT || F_APP_CONFERENCE_DONGLE_SUPPORT
                bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0);
#endif
            }
            else
            {
#if F_APP_TEAMS_FEATURE_SUPPORT || F_APP_CONFERENCE_DONGLE_SUPPORT
                bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x0f);
#endif
            }
        }
    }
}

void app_hfp_stop_ring_alert_timer(void)
{
    app_stop_timer(&timer_idx_hfp_ring);

#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_sco_status_notify();
#endif
}

uint8_t app_hfp_get_call_in_tone_type(T_APP_BR_LINK *p_link)
{
    T_APP_AUDIO_TONE_TYPE tone_type = TONE_TYPE_MAX;

#if F_APP_HARMAN_FEATURE_SUPPORT
    if ((app_cfg_nv.language_status == 0) &&
        (app_cfg_const.tone_harman_call_in != TONE_INVALID_INDEX))
    {
        tone_type = TONE_HARMAN_HF_CALL_IN;
    }
    else if ((app_cfg_nv.language_status == 1) && (app_cfg_const.tone_hf_call_in != TONE_INVALID_INDEX))
    {
        tone_type = TONE_HF_CALL_IN;
    }
#else
    if (app_cfg_const.tone_hf_call_in != TONE_INVALID_INDEX)
    {
        tone_type = TONE_HF_CALL_IN;
    }
#endif
    else if ((p_link == &(app_db.br_link[app_db.first_hf_index])) &&
             (app_cfg_const.tone_first_hf_call_in != TONE_INVALID_INDEX))
    {
        tone_type = TONE_FIRST_HF_CALL_IN;
    }
    else if ((p_link == &(app_db.br_link[app_db.last_hf_index])) &&
             (app_cfg_const.tone_last_hf_call_in != TONE_INVALID_INDEX))
    {
        tone_type = TONE_LAST_HF_CALL_IN;
    }

    APP_PRINT_TRACE1("app_hfp_get_call_in_tone_type: tone_type 0x%02x", tone_type);

    return tone_type;
}

void app_hfp_adjust_sco_window(uint8_t *bd_addr, T_APP_HFP_SCO_ADJUST_EVT evt)
{
    uint8_t active_hf_addr[6];
    uint8_t need_set = 0;

    if ((evt != APP_SCO_ADJUST_LINKBACK_END_EVT) && (bd_addr == NULL))
    {
        return;
    }

    if (app_hfp_sco_is_connected())
    {
        memcpy(active_hf_addr, app_db.br_link[active_hf_idx].bd_addr, 6);

        if (bd_addr == NULL)
        {
            // evt == APP_SCO_ADJUST_LINKBACK_END_EVT, linkback end
            need_set = 0;
        }
        else if ((app_bt_policy_get_state() == BP_STATE_LINKBACK) && (memcmp(active_hf_addr, bd_addr, 6)))
        {
            //bud is paging
            if (memcmp(active_hf_addr, bd_addr, 6))
            {
                need_set = 1;
            }
        }
        else if (app_bt_policy_src_conn_ind_check(bd_addr) && memcmp(active_hf_addr, bd_addr, 6))
        {
            //src is connecting bud
            need_set = 1;
        }
        else if ((evt == APP_SCO_ADJUST_SCO_CONN_IND_EVT) && (app_link_get_sco_conn_num() == 1) &&
                 memcmp(active_hf_addr, bd_addr, 6))
        {
            //sco conn ind from inactive hf link
            need_set = 1;
        }
        else
        {
            need_set = 0;
        }

        if (bd_addr != NULL)
        {
            APP_PRINT_TRACE4("app_hfp_adjust_sco_window bd_addr %s, active hf addr %s, evt 0x%04x, need_set %d",
                             TRACE_BDADDR(bd_addr), TRACE_BDADDR(active_hf_addr), evt, need_set);
        }

        bt_sco_link_retrans_window_set(active_hf_addr, need_set);
    }
}

bool app_hfp_sco_is_need(void)
{
    bool ret = false;

    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

    if ((call_status >= APP_CALL_OUTGOING) ||
        (call_status == APP_VOICE_ACTIVATION_ONGOING) ||
        (call_status == APP_CALL_INCOMING && app_db.br_link[active_hf_idx].is_inband_ring))
    {
        ret = true;
    }

    APP_PRINT_TRACE1("app_hfp_call_status_sco_is_need %d", ret);

    return ret;
}

void app_hfp_stop_delay_role_switch(void)
{
    app_stop_timer(&timer_handle_hfp_delay_role_switch);
}

#if F_APP_HFP_CMD_SUPPORT
void app_hfp_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                        uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    T_APP_BR_LINK *p_link = &app_db.br_link[active_hf_idx];

    switch (cmd_id)
    {
    case CMD_SEND_DTMF:
        {
            char number = (char)cmd_ptr[2];

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                if ((p_link->app_hf_state == APP_HF_STATE_CONNECTED) &&
                    (app_hfp_get_call_status() >= APP_CALL_ACTIVE))
                {
                    if (bt_hfp_dtmf_code_transmit(p_link->bd_addr, number) == false)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                uint8_t temp_buff[1];
                temp_buff[0] = cmd_ptr[2];

                app_report_event(cmd_path, EVENT_DTMF, app_idx, temp_buff, sizeof(temp_buff));
            }
        }
        break;

    case CMD_GET_SUBSCRIBER_NUM:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                if ((p_link->app_hf_state == APP_HF_STATE_CONNECTED))
                {
                    if (bt_hfp_subscriber_number_query(p_link->bd_addr) == false)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_OPERATOR:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                if ((p_link->app_hf_state == APP_HF_STATE_CONNECTED))
                {
                    if (bt_hfp_network_operator_name_query(p_link->bd_addr) == false)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    default:
        break;
    }
}
#endif

