/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "os_sched.h"
#include "test_mode.h"
#include "app_timer.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hid.h"
#include "bt_bond.h"
#include "system_status_api.h"
#include "sysm.h"
#include "eq.h"
#if F_APP_APT_SUPPORT
#include "audio_passthrough.h"
#endif
#include "audio.h"
#include "audio_track.h"
#if F_APP_VAD_SUPPORT
#include "vad.h"
#endif
// #include "remote.h"
#include "platform_utils.h"
#include "eq_utils.h"
#include "app_mmi.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_cmd.h"
#include "app_multilink.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_key_process.h"
#include "app_led.h"
#include "app_audio_policy.h"
#include "app_link_util.h"
#include "app_ble_common_adv.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_dlps.h"
#include "wdg.h"
#include "pm.h"
#include "ringtone.h"

#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "app_playback.h"
#include "app_playback_trans.h"
#include "audio_fs.h"
#if TARGET_RTL8773CO
#include "app_listen_save.h"
#endif
#endif

#if F_APP_ADP_CMD_SUPPORT
#include "app_adp_cmd.h"
#endif

#if F_APP_OTA_TOOLING_SUPPORT
#include "app_ota_tooling.h"
#endif

#include "app_ble_gap.h"

#if BISTO_FEATURE_SUPPORT
#include "app_bisto_key.h"
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#include "app_eq.h"
#include "app_bt_sniffing.h"
#include "rtl876x_pinmux.h"
#include "app_relay.h"
#include "app_bond.h"
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#if AMA_FEATURE_SUPPORT
#include "ama_api.h"
#include "app_ama_accessory.h"
#include "app_ama_device.h"
#endif
#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_device.h"
#endif

#include "app_roleswap.h"
#include "app_roleswap_control.h"
#if F_APP_LINEIN_SUPPORT
#include "line_in.h"
#endif
#if F_APP_IO_OUTPUT_SUPPORT
#include "app_io_output.h"
#endif
#include "app_line_in.h"

#include "app_device.h"
#include "app_sniff_mode.h"
#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif
#include "eq.h"
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#if (F_APP_DUAL_AUDIO_EFFECT == 1)
#include "app_dual_audio_effect.h"
#endif
#include "app_sensor.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_cmd.h"
#include "app_asp_device.h"
#include "asp_device_api.h"
#include "asp_device_link.h"
#include "app_teams_bt_policy.h"
#include "app_teams_extend_led.h"
#include "app_teams_audio_policy.h"
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif
#include "single_tone.h"
#include "gap_vendor.h"
#include "app_auto_power_off.h"
#include "gap_ext_scan.h"

#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "app_xiaowei_device.h"
#include "app_xiaowei.h"
#endif

#if F_APP_IPHONE_ABS_VOL_HANDLE
#include "app_iphone_abs_vol_handle.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_spp.h"
#include "app_dongle_dual_mode.h"
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#include "app_dongle_common.h"
#endif
#endif

#if F_APP_IPHONE_ABS_VOL_HANDLE
#include "app_iphone_abs_vol_handle.h"
#endif

#include "hal_debug.h"

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_adv.h"
#include "app_lea_mgr.h"
#endif

#if F_APP_HAS_SUPPORT
#include "has_mgr.h"
#endif

#if F_APP_USB_HID_SUPPORT
#include "app_usb_mmi.h"
#endif

#if F_APP_HEARABLE_SUPPORT
#include "app_hearable.h"
#endif

#if F_APP_AMP_SUPPORT
#include "app_amp.h"
#endif

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include "app_sensor_mems.h"
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_SAIYAN_MODE
#include "app_data_capture.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "fmna_crypto.h"
#include "fmna_state_machine.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif

#if F_APP_USB_SUSPEND_SUPPORT
#include "app_usb_suspend.h"
#endif

#if GFPS_FINDER_SUPPORT
#include "app_gfps_finder.h"
#include "app_gfps_msg.h"
#include "bt_gfps.h"
#include "app_dult_device.h"
#include "app_gfps_finder_adv.h"
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
#include "app_adc.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#endif

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
#include "app_harman_adc.h"
#endif

#define A2DP_MUTE_WHEN_PAUSE_TIMER_MS  2000

static uint8_t app_mmi_timer_id = 0;
static uint8_t timer_idx_reboot_check = 0;
static uint8_t reboot_check_times = REBOOT_CHECK_MAX_TIMES;
static uint8_t app_gaming_freq_handle = 0;
#if F_APP_FINDMY_FEATURE_SUPPORT
static uint8_t timer_idx_put_sn_state = 0;
#endif

#if F_APP_MUTILINK_TRIGGER_HIGH_PRIORITY
uint8_t app_mmi_get_highest_priority_index(uint32_t profile_mask);
#endif

typedef enum
{
    APP_TIMER_REBOOT_CHECK,
#if F_APP_FINDMY_FEATURE_SUPPORT
    APP_TIMER_PUT_SN_STATE,
#endif
} T_APP_MMI_TIMER;

bool app_mmi_reboot_check_timer_started(void)
{
    return ((timer_idx_reboot_check != 0) ? true : false);
}

static void app_mmi_power_off(void)
{
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE &&
        (app_db.waiting_factory_reset ||
         /* nonsmart box in case to power off */
         (app_cfg_const.enable_rtk_charging_box == false && app_db.local_loc == BUD_LOC_IN_CASE)))
    {
        sys_mgr_power_off();
    }
    else
    {
        app_audio_tone_flush(false);
        app_audio_tone_type_stop();

#if F_APP_HARMAN_FEATURE_SUPPORT
        if (app_cfg_nv.language_status == 0)
        {
            if (!app_audio_tone_type_play(TONE_HARMAN_POWER_OFF, false, false))
            {
                sys_mgr_power_off();
            }
        }
        else
#endif
        {
            if (!app_audio_tone_type_play(TONE_POWER_OFF, false, false))
            {
                sys_mgr_power_off();
            }
        }

        app_led_timer_stop();
        app_led_change_mode(LED_MODE_POWER_OFF, true, false);
    }
}

static void app_mmi_spk_mute_set(uint8_t action)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();
    uint8_t pair_idx_mapping;
    T_APP_BR_LINK *p_link = NULL;

    APP_PRINT_INFO4("app_mmi_spk_mute_set: bud_role %d, call_status %d, idx %d, %d",
                    app_cfg_nv.bud_role, app_hfp_get_call_status(), active_a2dp_idx, active_hf_idx);

    if (action == MMI_DEV_SPK_MUTE)
    {
        if (app_hfp_get_call_status() != APP_CALL_IDLE)
        {
            app_bond_get_pair_idx_mapping(app_db.br_link[active_hf_idx].bd_addr, &pair_idx_mapping);

            p_link = app_link_find_br_link(app_db.br_link[active_hf_idx].bd_addr);
            if (p_link != NULL)
            {
                if (!p_link->voice_muted)
                {
                    p_link->voice_muted = true;
                    audio_track_volume_out_mute(p_link->sco_track_handle);
                }
            }
        }
        else
        {
            if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                p_link = &app_db.br_link[active_a2dp_idx];
                if (p_link != NULL)
                {
                    if (!p_link->playback_muted)
                    {
#if F_APP_IPHONE_ABS_VOL_HANDLE
                        if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                            (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                        {
                            app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
                            uint8_t cur_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];

                            if ((cur_volume == 0) && (app_cfg_nv.abs_vol[pair_idx_mapping] != 0))
                            {
                                // Current a2dp level is fake 0. It's need to set gain value of fake lv 0 when unmute.
                                p_link->set_a2dp_fake_lv0_gain = true;
                            }
                        }
#endif
                        p_link->playback_muted = true;
                        audio_track_volume_out_mute(p_link->a2dp_track_handle);
                    }
                }
            }
        }
    }
    else
    {
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
        {
            if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
                (app_hfp_get_call_status() == APP_CALL_INCOMING))
            {
                //do nothing
            }
            else
            {
                if (app_hfp_get_call_status() != APP_CALL_IDLE)
                {
                    p_link = app_link_find_br_link(app_db.br_link[active_hf_idx].bd_addr);
                    if (p_link != NULL)
                    {
                        app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
                    }
                }
                else
                {
                    p_link = app_link_find_br_link(app_db.br_link[active_a2dp_idx].bd_addr);
                    if (p_link != NULL)
                    {
                        if (p_link->playback_muted)
                        {
#if F_APP_IPHONE_ABS_VOL_HANDLE
                            if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                                (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                            {
                                if (p_link->set_a2dp_fake_lv0_gain)
                                {
                                    //It's need to setup gain value of fake level 0.
                                    app_audio_route_dac_gain_set(AUDIO_CATEGORY_AUDIO, 0, DAC_GAIN_DB_NEGATIVE_65_DB);
                                    p_link->set_a2dp_fake_lv0_gain = false;
                                }
                            }
#endif
                            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                        }
                    }
                }
            }
        }
        else
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                uint8_t cmd_ptr[9] = {0};
                uint8_t msg_type;
                uint8_t cur_volume = 0;

                if (app_hfp_get_call_status() != APP_CALL_IDLE)
                {
                    app_bond_get_pair_idx_mapping(app_db.br_link[active_hf_idx].bd_addr, &pair_idx_mapping);
                    cur_volume = app_cfg_nv.voice_gain_level[pair_idx_mapping];

                    memcpy(&cmd_ptr[0], app_db.br_link[active_hf_idx].bd_addr, 6);
                    cmd_ptr[6] = AUDIO_STREAM_TYPE_VOICE;
                    msg_type = APP_REMOTE_MSG_HFP_VOLUME_SYNC;
                }
                else
                {
                    app_bond_get_pair_idx_mapping(app_db.br_link[active_a2dp_idx].bd_addr, &pair_idx_mapping);
                    cur_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];

                    memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                    cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                    msg_type = APP_REMOTE_MSG_A2DP_VOLUME_SYNC;
                }
                cmd_ptr[7] = cur_volume;
                cmd_ptr[8] = 0;
                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, msg_type,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
#endif
        }
    }

}

void app_mmi_reboot_check_timer_start(uint32_t period_timer)
{
    app_start_timer(&timer_idx_reboot_check, "reboot_check",
                    app_mmi_timer_id, APP_TIMER_REBOOT_CHECK, 0, false,
                    period_timer);
}

static void app_mmi_reboot(void)
{
    if (app_db.waiting_factory_reset)
    {
        app_device_factory_reset();
        app_db.waiting_factory_reset = false;
    }

#if F_APP_OTA_TOOLING_SUPPORT
    if (app_db.ota_tooling_start)
    {
        app_ota_tooling_nv_store();

        /* ensure reboot after factory reset or power off */
        app_cfg_const.disable_power_off_wdt_reset = 0;
    }
#endif

    app_device_state_change(APP_DEVICE_STATE_OFF);

    if (app_cfg_const.disable_power_off_wdt_reset)
    {
        // power down mode
        app_dlps_enable(APP_DLPS_ENTER_CHECK_WAIT_RESET);
        app_dlps_power_off();
    }
    else
    {
        app_cfg_nv.is_app_reboot = 1;
        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

        os_delay(20);
        chip_reset(RESET_ALL_EXCEPT_AON);
    }
}

void app_mmi_modify_reboot_check_times(uint32_t times)
{
    if (reboot_check_times > times)
    {
        reboot_check_times = times;
    }
}

static void app_mmi_reboot_check(void)
{
    app_stop_timer(&timer_idx_reboot_check);

    if (reboot_check_times)
    {
        reboot_check_times--;
    }

    if (app_key_long_key_power_off_pressed_get() &&
        (app_db.power_off_cause == POWER_OFF_CAUSE_LONG_KEY ||
         app_db.power_off_cause == POWER_OFF_CAUSE_SHORT_KEY))
    {
        app_mmi_reboot_check_timer_start(100);
        return;
    }

    if (!reboot_check_times)
    {
        app_mmi_reboot();
    }
    else
    {
        if (ringtone_remaining_count_get()
            || voice_prompt_remaining_count_get()
            || (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            || app_link_get_b2s_link_num()
            || app_led_get_led_stop_check_state()
#if F_APP_AMP_SUPPORT
            || !app_amp_is_off_state()
#endif
           )
        {
            app_mmi_reboot_check_timer_start(100);
        }
        else
        {
            app_mmi_reboot();
        }
    }
}

static void app_mmi_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_REBOOT_CHECK:
        {

            app_mmi_reboot_check();

        }
        break;

#if F_APP_FINDMY_FEATURE_SUPPORT
    case APP_TIMER_PUT_SN_STATE:
        {
            app_stop_timer(&timer_idx_put_sn_state);
            serial_number_read_state = false;
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
uint16_t app_mmi_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    // uint16_t payload_len = 0;
    // uint8_t *msg_ptr = NULL;
    // bool skip = true;

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_MMI, 0, NULL, true, total);
}

static void app_mmi_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
        status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
        status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
        status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        uint8_t action = msg_type;

        if (action == MMI_DEV_POWER_OFF)
        {
            if (app_db.power_off_cause == POWER_OFF_CAUSE_UNKNOWN)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_RELAY;
            }
        }

        app_mmi_handle_action(action);
    }
}
#endif

void app_mmi_init(void)
{
    app_timer_reg_cb(app_mmi_timeout_cb, &app_mmi_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_mmi_relay_cback, app_mmi_parse_cback,
                             APP_MODULE_TYPE_MMI, MMI_TOTAL);
#endif
}

#if F_APP_HARMAN_FEATURE_SUPPORT
static uint8_t app_mmi_harman_get_active_hf_idx(void)
{
    uint8_t app_idx = 0;
    uint8_t record_active_index = 0xff;
    uint8_t i, hfhs_profile_link_number = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        APP_PRINT_INFO3("app_mmi_harman_get_active_hf_idx: DUMP_INDEX: %d, addr: %s, play_state: %d", i,
                        TRACE_BDADDR(app_db.br_link[i].bd_addr),
                        app_db.br_link[i].avrcp_play_status);

        if ((app_link_check_b2s_link_by_id(i)) &&
            (app_db.br_link[i].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK)))
        {
            hfhs_profile_link_number++;
            //if (i == app_a2dp_get_active_idx())
            if ((i == app_harman_aling_active_hfp_idx_get()) &&
                (app_harman_aling_active_hfp_idx_get() != 0xff))
            {
                record_active_index = i;
            }
        }
    }
    APP_PRINT_TRACE7("app_mmi_harman_get_active_hf_idx: %d,%d,%d,%d,%d,%d,%d",
                     app_db.last_hf_index,
                     app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status,
                     app_a2dp_get_active_idx(),
                     app_hfp_sco_is_connected(),
                     app_hfp_get_active_idx(),
                     hfhs_profile_link_number,
                     record_active_index);

    if ((hfhs_profile_link_number == 0) && (record_active_index == 0xff))
    {
        APP_PRINT_TRACE1("app_mmi_harman_get_active_hf_idx: INDEX NO SUPPORT HF1:%d", record_active_index);
    }
    else if (hfhs_profile_link_number == 1)
    {
        if (record_active_index == 0xff)
        {
            APP_PRINT_TRACE1("app_mmi_harman_get_active_hf_idx: VOICE_DIAL INDEX NO SUPPORT HF2:%d",
                             record_active_index);
        }
        else
        {
            app_idx = record_active_index;
        }
    }
    else if (hfhs_profile_link_number == 2)
    {
        app_idx = record_active_index;
    }

    APP_PRINT_TRACE1("app_mmi_harman_get_active_hf_idx: AU_MMI_INITIATE_VOICE_DIAL FINAL_INDEX:%d",
                     app_idx);

    return app_idx;
}
#endif

static void app_mmi_voice_recognition(uint8_t app_idx)
{
#if F_APP_DURIAN_SUPPORT
    app_durian_mmi_voice_recognition_enable(app_idx);
#else
    if (app_db.br_link[app_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
    {
        bt_hfp_voice_recognition_enable_req(app_db.br_link[app_idx].bd_addr);

        app_audio_tone_type_play(TONE_HF_CALL_VOICE_DIAL, false, true);
    }
    else
    {
        uint8_t bd_addr[6];

        if (app_bond_b2s_addr_get(1, bd_addr) == true)
        {
            app_bt_policy_default_connect(bd_addr,
                                          HSP_PROFILE_MASK | HFP_PROFILE_MASK, true);
        }
    }
#endif
}

static void app_mmi_volume_max_min_tone_play_status_sync(void)
{
    app_key_set_volume_status(false);
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_VOL_IS_CHANGED_BY_KEY);
    }
#endif
}

static void app_mmi_volume_up()
{
    uint8_t max_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx_mapping;
    T_AUDIO_STREAM_TYPE volume_type;
    int8_t ret = 0;

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_key_set_volume_status(false);
        return;
    }
#endif

    if ((app_hfp_get_call_status() == APP_CALL_IDLE) &&
        (app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING) &&
        (app_audio_get_bud_stream_state() != BUD_STREAM_STATE_AUDIO))
    {
        app_key_set_volume_status(false);

        if (app_cfg_const.disallow_adjust_volume_when_idle)
        {
            return;
        }
    }

#if F_APP_LOCAL_PLAYBACK_SUPPORT
    if (app_db.sd_playback_switch)
    {
        app_playback_volume_up();
        return ;
    }
#endif

#if F_APP_LINEIN_SUPPORT
    if (app_line_in_playing_state_get())
    {
        app_line_in_volume_up_handle();
        return;
    }
#endif

#if F_APP_TEAMS_BT_POLICY
    if ((app_hfp_get_call_status() != APP_CALL_IDLE) ||
        (app_link_get_sco_conn_num() != 0))
#else
#if (F_APP_GAMING_DONGLE_SUPPORT == 1)
    if ((app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING) &&
        (app_link_check_dongle_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr)))
    {
        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
        active_idx = app_hfp_get_active_idx();
        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 1;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_up;
        }
        curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
        max_volume = app_dsp_cfg_vol.playback_volume_max;
    }
    else if ((app_hfp_get_call_status() != BT_HFP_CALL_IDLE))
#else
    if ((app_hfp_get_call_status() != APP_CALL_IDLE))
#endif
#endif
    {
        volume_type = AUDIO_STREAM_TYPE_VOICE;
        active_idx = app_hfp_get_active_idx();

        if (app_db.br_link[active_idx].call_status == APP_CALL_INCOMING &&
            app_db.br_link[active_idx].is_inband_ring == false)
        {
            ret = 2;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_up;
        }

        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 3;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_up;
        }
        curr_volume = app_cfg_nv.voice_gain_level[pair_idx_mapping];
        max_volume = app_dsp_cfg_vol.voice_out_volume_max;
    }
    else
    {
        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
        if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
        {
            active_idx = app_teams_multilink_get_active_index();
        }
        else
        {
            active_idx = app_a2dp_get_active_idx();
        }
#else
        active_idx = app_a2dp_get_active_idx();
#endif
        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 4;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_up;
        }
        curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
        max_volume = app_dsp_cfg_vol.playback_volume_max;
    }

    if (curr_volume < max_volume)
    {
        curr_volume++;
    }
    else
    {
#if 0

        if (app_audio_is_circular_volume_up())
        {
            curr_volume = 0;
            app_db.is_circular_vol_up = true;
        }
        else
#endif

        {
            curr_volume = max_volume;
        }
    }

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        uint8_t cmd_ptr[9];
        memcpy(&cmd_ptr[0], app_db.br_link[active_idx].bd_addr, 6);
        cmd_ptr[6] = volume_type;
        cmd_ptr[7] = curr_volume;
        cmd_ptr[8] = 0;

        if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
        {
            uint8_t level;

#if F_APP_IPHONE_ABS_VOL_HANDLE
            T_APP_BR_LINK *p_link = NULL;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
            if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
            {
                p_link = app_link_find_br_link(app_db.br_link[app_teams_multilink_get_active_index()].bd_addr);
            }
            else
            {
                p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
            }
#else
            p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
#endif

            if (p_link != NULL)
            {
                if (((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                     (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true)))
                {
                    if (curr_volume == 1 && app_cfg_nv.abs_vol[pair_idx_mapping] == 0)
                    {
                        //up to fake level 0 ( 0 -> 1).
                        curr_volume = 0;

                        //need to update sync information.
                        cmd_ptr[7] = curr_volume;
                    }

                    level = app_cfg_nv.abs_vol[pair_idx_mapping] = iphone_abs_vol[curr_volume];

                    //if enable circular vol up, set all value to 0.
                    if (app_db.is_circular_vol_up && curr_volume == 0)
                    {
                        level = app_cfg_nv.abs_vol[pair_idx_mapping] = 0;
                    }

                    app_iphone_abs_vol_sync_abs_vol(app_db.br_link[active_idx].bd_addr,
                                                    app_cfg_nv.abs_vol[pair_idx_mapping]);
                }
                else
#endif
                {
                    level = (curr_volume * 0x7F + app_dsp_cfg_vol.playback_volume_max / 2) /
                            app_dsp_cfg_vol.playback_volume_max;
                }

                app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;

                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                bt_avrcp_volume_change_req(app_db.br_link[active_idx].bd_addr, level);
#if F_APP_IPHONE_ABS_VOL_HANDLE
            }
#endif
        }

        else if (volume_type == AUDIO_STREAM_TYPE_VOICE)
        {
            uint8_t level = (curr_volume * 0x0F + app_dsp_cfg_vol.voice_out_volume_max / 2) /
                            app_dsp_cfg_vol.voice_out_volume_max;

            app_cfg_nv.voice_gain_level[pair_idx_mapping] = curr_volume;

            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_HFP_VOLUME_SYNC,
                                               cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);

            bt_hfp_speaker_gain_level_report(app_db.br_link[active_idx].bd_addr, level);
        }
    }
    else
#endif
    {
        if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
        {
            uint8_t level;

#if F_APP_IPHONE_ABS_VOL_HANDLE
            T_APP_BR_LINK *p_link = NULL;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
            if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
            {
                p_link = app_link_find_br_link(app_db.br_link[app_teams_multilink_get_active_index()].bd_addr);
            }
            else
            {
                p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
            }
#else
            p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
#endif

            if (p_link != NULL)
            {
                if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                    (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                {
                    if (curr_volume == 1 && app_cfg_nv.abs_vol[pair_idx_mapping] == 0)
                    {
                        //up to fake level 0 ( 0 -> 1).
                        curr_volume = 0;
                    }

                    level = app_cfg_nv.abs_vol[pair_idx_mapping] = iphone_abs_vol[curr_volume];

                    //if enable circular vol up, set all value to 0.
                    if (app_db.is_circular_vol_up && curr_volume == 0)
                    {
                        level = app_cfg_nv.abs_vol[pair_idx_mapping] = 0;
                    }
                }
                else
#endif
                {
                    level = (curr_volume * 0x7F + app_dsp_cfg_vol.playback_volume_max / 2) /
                            app_dsp_cfg_vol.playback_volume_max;
                }

                if ((!app_cfg_const.enable_rtk_charging_box) || (app_db.local_in_case == false))
                {
#if F_APP_IPHONE_ABS_VOL_HANDLE
                    app_iphone_abs_vol_wrap_audio_track_volume_out_set(app_db.br_link[active_idx].a2dp_track_handle,
                                                                       curr_volume,
                                                                       app_cfg_nv.abs_vol[pair_idx_mapping]);
#else
                    audio_track_volume_out_set(app_db.br_link[active_idx].a2dp_track_handle, curr_volume);
#endif
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                }
#if F_APP_HARMAN_FEATURE_SUPPORT
                APP_PRINT_TRACE4("[SD_CHECK] bt_avrcp_volume_change_req curr_vol=%d,idx=%d,stream=%d,avrcp=%d",
                                 curr_volume,
                                 active_idx,
                                 app_db.br_link[active_idx].streaming_fg,
                                 app_db.br_link[active_idx].avrcp_play_status);

                if (app_db.br_link[active_idx].streaming_fg == false)
                {
                    harman_push_max_tone_when_not_playing(AUDIO_STREAM_TYPE_PLAYBACK,
                                                          app_cfg_nv.audio_gain_level[pair_idx_mapping],
                                                          curr_volume);
                }
#endif
#if HARMAN_OPEN_LR_FEATURE
                app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, curr_volume, __func__, __LINE__);
#endif
                app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                bt_avrcp_volume_change_req(app_db.br_link[active_idx].bd_addr, level);

#if F_APP_IPHONE_ABS_VOL_HANDLE
            }
#endif

        }
        else if (volume_type == AUDIO_STREAM_TYPE_VOICE)
        {
            uint8_t level = (curr_volume * 0x0F + app_dsp_cfg_vol.voice_out_volume_max /
                             2) / app_dsp_cfg_vol.voice_out_volume_max;
#if HARMAN_OPEN_LR_FEATURE
            app_harman_lr_balance_set(AUDIO_STREAM_TYPE_VOICE, curr_volume, __func__, __LINE__);
#endif
            if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
                (app_hfp_get_call_status() == APP_CALL_INCOMING))
            {
                app_key_set_volume_status(false);
                app_hfp_inband_tone_mute_ctrl();
            }
            else if ((!app_cfg_const.enable_rtk_charging_box) || (app_db.local_in_case == false))
            {
                audio_track_volume_out_set(app_db.br_link[active_idx].sco_track_handle, curr_volume);

                app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
            }

            app_cfg_nv.voice_gain_level[pair_idx_mapping] = curr_volume;
            bt_hfp_speaker_gain_level_report(app_db.br_link[active_idx].bd_addr, level);
        }
    }

fail_vol_up:
    APP_PRINT_ERROR1("app_mmi_volume_up adjust fail %d", -ret);
}

static void app_mmi_volume_down()
{
    bool already_vgs0 = false;
    uint8_t min_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx_mapping;
    T_AUDIO_STREAM_TYPE volume_type;
    int8_t ret = 0;

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_key_set_volume_status(false);
        return;
    }
#endif

    if ((app_hfp_get_call_status() == APP_CALL_IDLE) &&
        (app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING) &&
        (app_audio_get_bud_stream_state() != BUD_STREAM_STATE_AUDIO))
    {
        app_key_set_volume_status(false);

        if (app_cfg_const.disallow_adjust_volume_when_idle)
        {
            return;
        }
    }

#if F_APP_LOCAL_PLAYBACK_SUPPORT
    if (app_db.sd_playback_switch)
    {
        app_playback_volume_down();
        return ;
    }
#endif

#if F_APP_LINEIN_SUPPORT
    if (app_line_in_playing_state_get())
    {
        app_line_in_volume_down_handle();
        return;
    }
#endif

#if F_APP_TEAMS_BT_POLICY
    if ((app_hfp_get_call_status() != APP_CALL_IDLE) ||
        (app_link_get_sco_conn_num() != 0))
#else
#if (F_APP_GAMING_DONGLE_SUPPORT == 1)
    if ((app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING) &&
        (app_link_check_dongle_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr)))
    {
        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
        active_idx = app_hfp_get_active_idx();
        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 1;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_down;
        }
        curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
        min_volume = app_dsp_cfg_vol.playback_volume_min;
    }
    else if ((app_hfp_get_call_status() != BT_HFP_CALL_IDLE))
#else
    if ((app_hfp_get_call_status() != APP_CALL_IDLE))
#endif
#endif
    {
        volume_type = AUDIO_STREAM_TYPE_VOICE;
        active_idx = app_hfp_get_active_idx();

        if (app_db.br_link[active_idx].call_status == APP_CALL_INCOMING &&
            app_db.br_link[active_idx].is_inband_ring == false)
        {
            ret = 2;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_down;
        }

        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 3;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_down;
        }
        curr_volume = app_cfg_nv.voice_gain_level[pair_idx_mapping];
        min_volume = app_dsp_cfg_vol.voice_out_volume_min;
    }
    else
    {
        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
        if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
        {
            active_idx = app_teams_multilink_get_active_index();
        }
        else
        {
            active_idx = app_a2dp_get_active_idx();
        }
#else
        active_idx = app_a2dp_get_active_idx();
#endif
        if (app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping) == false)
        {
            ret = 4;
            app_mmi_volume_max_min_tone_play_status_sync();
            goto fail_vol_down;
        }
        curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
        min_volume = app_dsp_cfg_vol.playback_volume_min;
    }

    if (curr_volume > min_volume)
    {
        curr_volume--;
    }
    else
    {
        curr_volume = min_volume;

        /*if ios version is 13, send AT+VGS=0 repeatedly will make voice mute*/
        if (curr_volume == 0)
        {
            already_vgs0 = true;
        }
    }

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        uint8_t cmd_ptr[9];
        memcpy(&cmd_ptr[0], app_db.br_link[active_idx].bd_addr, 6);
        cmd_ptr[6] = volume_type;
        cmd_ptr[7] = curr_volume;
        cmd_ptr[8] = 0;

        if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
        {
            uint8_t level;

#if F_APP_IPHONE_ABS_VOL_HANDLE
            T_APP_BR_LINK *p_link = NULL;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
            if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
            {
                p_link = app_link_find_br_link(app_db.br_link[app_teams_multilink_get_active_index()].bd_addr);
            }
            else
            {
                p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
            }
#else
            p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
#endif

            if (p_link != NULL)
            {
                if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                    (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                {
                    level = app_cfg_nv.abs_vol[pair_idx_mapping] = iphone_abs_vol[curr_volume];

                    if ((app_cfg_nv.audio_gain_level[pair_idx_mapping] == 0) && (curr_volume == 0))
                    {
                        //down to real level 0 (0 -> 0).
                        level = app_cfg_nv.abs_vol[pair_idx_mapping] = 0;
                    }

                    app_iphone_abs_vol_sync_abs_vol(app_db.br_link[active_idx].bd_addr,
                                                    app_cfg_nv.abs_vol[pair_idx_mapping]);
                }
                else
#endif
                {
                    level = (curr_volume * 0x7F + app_dsp_cfg_vol.playback_volume_max / 2) /
                            app_dsp_cfg_vol.playback_volume_max;
                }

                app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;

                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);

                bt_avrcp_volume_change_req(app_db.br_link[active_idx].bd_addr, level);

#if F_APP_IPHONE_ABS_VOL_HANDLE
            }
#endif
        }
        else if (volume_type == AUDIO_STREAM_TYPE_VOICE)
        {
            uint8_t level = (curr_volume * 0x0F + app_dsp_cfg_vol.voice_out_volume_max / 2) /
                            app_dsp_cfg_vol.voice_out_volume_max;

            app_cfg_nv.voice_gain_level[pair_idx_mapping] = curr_volume;

            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_HFP_VOLUME_SYNC,
                                               cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);

            if (already_vgs0 == false)
            {
                bt_hfp_speaker_gain_level_report(app_db.br_link[active_idx].bd_addr, level);
            }
        }
    }
    else
#endif
    {
        if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
        {
            uint8_t level;

#if F_APP_IPHONE_ABS_VOL_HANDLE
            T_APP_BR_LINK *p_link = NULL;
#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
            if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
            {
                p_link = app_link_find_br_link(app_db.br_link[app_teams_multilink_get_active_index()].bd_addr);
            }
            else
            {
                p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
            }
#else
            p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
#endif

            if (p_link != NULL)
            {
                if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                    (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                {
                    level = app_cfg_nv.abs_vol[pair_idx_mapping] = iphone_abs_vol[curr_volume];

                    if ((app_cfg_nv.audio_gain_level[pair_idx_mapping] == 0) && (curr_volume == 0))
                    {
                        //down to real level 0 (0 -> 0).
                        level = app_cfg_nv.abs_vol[pair_idx_mapping] = 0;
                    }
                }
                else
#endif
                {
                    level = (curr_volume * 0x7F + app_dsp_cfg_vol.playback_volume_max / 2) /
                            app_dsp_cfg_vol.playback_volume_max;
                }

                if ((!app_cfg_const.enable_rtk_charging_box) || (app_db.local_in_case == false))
                {
#if F_APP_IPHONE_ABS_VOL_HANDLE
                    app_iphone_abs_vol_wrap_audio_track_volume_out_set(app_db.br_link[active_idx].a2dp_track_handle,
                                                                       curr_volume,
                                                                       app_cfg_nv.abs_vol[pair_idx_mapping]);
#else
                    audio_track_volume_out_set(app_db.br_link[active_idx].a2dp_track_handle, curr_volume);
#endif
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                }
#if HARMAN_OPEN_LR_FEATURE
                app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, curr_volume, __func__, __LINE__);
#endif
                bt_avrcp_volume_change_req(app_db.br_link[active_idx].bd_addr, level);
                app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;

#if F_APP_IPHONE_ABS_VOL_HANDLE
            }
#endif
        }
        else if (volume_type == AUDIO_STREAM_TYPE_VOICE)
        {
            uint8_t level = (curr_volume * 0x0F + app_dsp_cfg_vol.voice_out_volume_max /
                             2) / app_dsp_cfg_vol.voice_out_volume_max;
#if HARMAN_OPEN_LR_FEATURE
            app_harman_lr_balance_set(AUDIO_STREAM_TYPE_VOICE, curr_volume, __func__, __LINE__);
#endif
            if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
                (app_hfp_get_call_status() == APP_CALL_INCOMING))
            {
                app_key_set_volume_status(false);
                app_hfp_inband_tone_mute_ctrl();
            }
            else if ((!app_cfg_const.enable_rtk_charging_box) || (app_db.local_in_case == false))
            {
                audio_track_volume_out_set(app_db.br_link[active_idx].sco_track_handle, curr_volume);

                app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
            }

            if (already_vgs0 == false)
            {
                bt_hfp_speaker_gain_level_report(app_db.br_link[active_idx].bd_addr, level);
            }

            app_cfg_nv.voice_gain_level[pair_idx_mapping] = curr_volume;
        }
    }

fail_vol_down:
    APP_PRINT_ERROR1("app_mmi_volume_down adjust fail %d", -ret);
}

#if 0
void app_mmi_switch_gaming_mode()
{
    APP_PRINT_TRACE4("app_mmi_switch_gaming_mode: old status %d, b2b state %d, disallow tone %d dongle %d",
                     app_db.gaming_mode,
                     app_db.remote_session_state, app_db.disallow_play_gaming_mode_vp,
                     app_db.remote_is_dongle);
    T_AUDIO_STREAM_USAGE usage;
    T_AUDIO_STREAM_MODE mode;
    uint8_t vol;
    bool vol_muted;
    T_AUDIO_FORMAT_INFO format;
    T_APP_BR_LINK *p_link = NULL;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint16_t latency_value = app_cfg_nv.cmd_latency_in_ms;
#else
    uint16_t latency_value = app_cfg_nv.audio_latency;
#endif
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t inactive_a2dp_idx = app_a2dp_get_inactive_idx();

    if (app_db.gaming_mode)
    {
        app_db.gaming_mode = false;

#if F_APP_GAMING_DONGLE_SUPPORT
        if (app_db.remote_is_dongle)
        {
            uint32_t cpu_freq;
            pm_cpu_freq_clear(&app_gaming_freq_handle, &cpu_freq);
        }
#endif

        if (app_db.spk_eq_mode == GAMING_MODE)
        {
            app_cfg_nv.eq_idx_gaming_mode_record = app_cfg_nv.eq_idx;
        }

#if F_APP_LISTENING_MODE_SUPPORT
        if (app_cfg_const.disallow_listening_mode_on_when_gaming_mode)
        {
            app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_GAMING_MODE_END);
        }
#endif


        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
             app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
#if F_APP_GAMING_DONGLE_SUPPORT
            && (app_db.disallow_play_gaming_mode_vp == false)
#endif
           )
        {
            app_audio_tone_type_play(TONE_EXIT_GAMING_MODE, false, true);
        }
    }
    else
    {
        app_db.gaming_mode = true;

#if F_APP_GAMING_DONGLE_SUPPORT
        if (app_db.remote_is_dongle)
        {
            uint32_t cpu_freq;
            pm_cpu_freq_req(&app_gaming_freq_handle, 80, &cpu_freq);
        }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
        if (app_cfg_const.disallow_listening_mode_on_when_gaming_mode)
        {
            app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_GAMING_MODE);
        }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
        if (app_db.disallow_play_gaming_mode_vp == false)
#endif
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
                app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_audio_tone_type_play(TONE_ENTER_GAMING_MODE, false, true);
            }
            else if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                app_audio_tone_type_play(TONE_ENTER_GAMING_MODE, false, false);
            }
        }

        app_audio_get_last_used_latency(&latency_value);
    }

#if (F_APP_DUAL_AUDIO_EFFECT == 1)
    if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
        {
            app_dual_audio_single_effect();
        }
        else
        {
            app_dual_audio_effect_set(dual_audio_mode_sel);
        }
    }
    else
    {
        app_dual_audio_effect_set(dual_audio_mode_sel);
    }
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#else
    app_led_change_mode(LED_MODE_GAMING_MODE, true, false);
#endif
    app_bt_sniffing_set_low_latency();

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                p_link = &app_db.br_link[i];
                break;
            }
        }
    }
    else
    {
        if (app_link_check_b2s_link_by_id(active_a2dp_idx))
        {
            p_link = &app_db.br_link[active_a2dp_idx];
        }
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (p_link != NULL && app_db.remote_is_dongle)
    {
        if (app_db.gaming_mode)
        {
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
            {
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
            }
            else
            {
                app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
            }
        }
        else
        {
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
            {
                app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
            }
            else
            {
                app_sniff_mode_b2s_enable(p_link->bd_addr, SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
            }
        }

        app_bt_policy_qos_param_update(p_link->bd_addr, BP_TPOLL_DONGLE_GAMING);
    }
#endif

    if (p_link == NULL || p_link->a2dp_track_handle == NULL)
    {
        app_eq_change_audio_eq_mode(false);
    }

    if (p_link != NULL)
    {
        if (p_link->a2dp_track_handle != NULL)
        {
            APP_PRINT_INFO2("mmi set gaming mode: active link %s, mode %u", TRACE_BDADDR(p_link->bd_addr),
                            app_db.gaming_mode);
            audio_track_format_info_get(p_link->a2dp_track_handle, &format);
            audio_track_usage_get(p_link->a2dp_track_handle, &usage);
            audio_track_volume_out_is_muted(p_link->a2dp_track_handle, &vol_muted);
            audio_track_volume_out_get(p_link->a2dp_track_handle, &vol);

            if (p_link->eq_instance != NULL)
            {
                audio_track_effect_detach(p_link->a2dp_track_handle, p_link->eq_instance);
            }

            audio_track_release(p_link->a2dp_track_handle);

            if (app_db.gaming_mode)
            {
#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_link_check_dongle_link(p_link->bd_addr))
                {
                    mode = AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY;
                }
                else
#endif
                {
                    mode = AUDIO_STREAM_MODE_LOW_LATENCY;
                }
            }
            else
            {
                mode = AUDIO_STREAM_MODE_NORMAL;
            }

            p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                           mode,
                                                           AUDIO_STREAM_USAGE_SNOOP,
                                                           format,
                                                           vol,
                                                           0,
                                                           AUDIO_DEVICE_OUT_DEFAULT,
                                                           NULL,
                                                           NULL);

            if (vol_muted)
            {
                audio_track_volume_out_mute(p_link->a2dp_track_handle);
                p_link->playback_muted = true;
            }

            if (app_db.gaming_mode)
            {
#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_link_check_dongle_link(p_link->bd_addr))
                {
                    latency_value = app_dongle_get_gaming_latency();
                    app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);

                    bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
                    {
                        app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                    }
                    else
                    {
                        app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                    }
                }
                else
#endif
                {
                    latency_value = app_audio_set_latency(p_link->a2dp_track_handle,
                                                          app_cfg_nv.rws_low_latency_level_record,
                                                          GAMING_MODE_DYNAMIC_LATENCY_FIX);
                    bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
                }

                app_db.last_gaming_mode_audio_track_latency = latency_value;
            }
            else
            {
                app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_NORMAL, format.type, 0, &latency_value);
                audio_track_latency_set(p_link->a2dp_track_handle, latency_value,
                                        NORMAL_MODE_DYNAMIC_LATENCY_FIX);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }

            if (p_link->eq_instance != NULL)
            {
                /* when streaming, changing eq after audio track recreate */
                app_eq_change_audio_eq_mode(false);
                audio_track_effect_attach(p_link->a2dp_track_handle, p_link->eq_instance);
            }

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            audio_track_start(p_link->a2dp_track_handle);
#else
#if F_APP_LINEIN_SUPPORT
            if (app_cfg_const.line_in_support)
            {
                if (app_line_in_start_a2dp_check())
                {
                    audio_track_start(p_link->a2dp_track_handle);
                }
            }
            else
#endif
            {
                audio_track_start(p_link->a2dp_track_handle);
            }
#endif
        }
    }

    if ((app_cfg_const.enable_multi_link) &&
        (inactive_a2dp_idx != MAX_BR_LINK_NUM) &&
        (app_db.br_link[inactive_a2dp_idx].streaming_fg == true))
    {
        p_link = &app_db.br_link[inactive_a2dp_idx];
        if (p_link != NULL)
        {
            if (p_link->a2dp_track_handle != NULL)
            {
                APP_PRINT_TRACE2("mmi set multi gaming mode: inactive link %s, mode %u",
                                 TRACE_BDADDR(p_link->bd_addr),
                                 app_db.gaming_mode);
                audio_track_format_info_get(p_link->a2dp_track_handle, &format);
                audio_track_usage_get(p_link->a2dp_track_handle, &usage);
                audio_track_volume_out_is_muted(p_link->a2dp_track_handle, &vol_muted);
                audio_track_volume_out_get(p_link->a2dp_track_handle, &vol);

                if (p_link->eq_instance != NULL)
                {
                    audio_track_effect_detach(p_link->a2dp_track_handle, p_link->eq_instance);
                }

                audio_track_release(p_link->a2dp_track_handle);

                if (app_db.gaming_mode)
                {
#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(p_link->bd_addr))
                    {
                        mode = AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY;
                    }
                    else
#endif
                    {
                        mode = AUDIO_STREAM_MODE_LOW_LATENCY;
                    }
                }
                else
                {
                    mode = AUDIO_STREAM_MODE_NORMAL;
                }

                p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                               mode,
                                                               AUDIO_STREAM_USAGE_SNOOP,
                                                               format,
                                                               vol,
                                                               0,
                                                               AUDIO_DEVICE_OUT_DEFAULT,
                                                               NULL,
                                                               NULL);

                if (vol_muted)
                {
                    audio_track_volume_out_mute(p_link->a2dp_track_handle);
                    p_link->playback_muted = true;
                }

                if (app_db.gaming_mode)
                {
#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(p_link->bd_addr))
                    {
                        latency_value = app_dongle_get_gaming_latency();
                        app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);

                        bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
                        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
                        {
                            app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                        }
                        else
                        {
                            app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                        }
                    }
                    else
#endif
                    {
                        latency_value = app_audio_set_latency(p_link->a2dp_track_handle,
                                                              app_cfg_nv.rws_low_latency_level_record,
                                                              GAMING_MODE_DYNAMIC_LATENCY_FIX);
                        bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
                    }

                    app_db.last_gaming_mode_audio_track_latency = latency_value;
                }
                else
                {
                    app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_NORMAL, format.type, 0, &latency_value);
                    audio_track_latency_set(p_link->a2dp_track_handle, latency_value,
                                            NORMAL_MODE_DYNAMIC_LATENCY_FIX);
                    bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
                }

                if (p_link->eq_instance != NULL)
                {
                    /* when streaming, changing eq after audio track recreate */
                    app_eq_change_audio_eq_mode(false);
                    audio_track_effect_attach(p_link->a2dp_track_handle, p_link->eq_instance);
                }
            }
        }
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        app_audio_report_low_latency_status(latency_value);

        if (app_db.spk_eq_mode != LINE_IN_MODE)
        {
#if (F_APP_USER_EQ_SUPPORT == 1)
            if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
            {
                app_report_eq_idx(EQ_INDEX_REPORT_BY_UPDATE_EQ_INDEX);
            }
            else
#endif
            {
                app_report_eq_idx(EQ_INDEX_REPORT_BY_CHANGE_MODE);
            }
        }
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_db.disallow_play_gaming_mode_vp && app_db.gaming_mode_request_is_received)
    {
        app_db.disallow_play_gaming_mode_vp = false;
    }

    app_db.gaming_mode_request_is_received = false;

    app_bt_policy_msg_gaming_dongle_streaming();
#endif
}
#endif

#if 0
void app_mmi_execute_output_indication_action(uint8_t action)
{
    uint8_t *pinmux = &app_cfg_const.output_indication1_pinmux;
    uint8_t i = action - MMI_OUTPUT_INDICATION1_TOGGLE;

    if (app_cfg_const.output_indication_support)
    {
        if (*(pinmux + i) != 0xFF)
        {
            PAD_OUTPUT_VAL tmp_output_indication = PAD_OUT_HIGH;

            if (Pad_GetOutputCtrl(*(pinmux + i)) == PAD_AON_OUTPUT_HIGH)
            {
                tmp_output_indication = PAD_OUT_LOW;
            }
            else if (Pad_GetOutputCtrl(*(pinmux + i)) == PAD_AON_OUTPUT_LOW)
            {
                tmp_output_indication = PAD_OUT_HIGH;
            }

            if (action == MMI_OUTPUT_INDICATION3_TOGGLE)
            {
#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.output_ind3_link_mic_toggle)
                {
                    tmp_output_indication = PAD_OUT_HIGH;
                    if (app_cfg_const.led_on_when_mic_mute)
                    {
                        if ((app_cfg_const.enable_output_ind3_high_active == 1) && !app_audio_is_mic_mute()
                            || (app_cfg_const.enable_output_ind3_high_active == 0) &&  app_audio_is_mic_mute())
                        {
                            tmp_output_indication = PAD_OUT_LOW;
                        }
                    }
                    else //led on when mic unmute
                    {
                        if ((app_cfg_const.enable_output_ind3_high_active == 1) && app_audio_is_mic_mute()
                            || (app_cfg_const.enable_output_ind3_high_active == 0) && (!app_audio_is_mic_mute()))
                        {
                            tmp_output_indication = PAD_OUT_LOW;
                        }
                    }
                }
#endif
            }

            Pad_OutputControlValue(*(pinmux + i), tmp_output_indication);
        }
    }
}
#endif

static void app_mmi_check_factory_reset_behavior(uint8_t action)
{
    app_db.factory_reset_ignore_led_and_vp = false;

    switch (action)
    {
    case MMI_DEV_TOOLING_FACTORY_RESET:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_NORMAL;
        app_db.factory_reset_ignore_led_and_vp = true;
        break;

    case MMI_DEV_CFG_FACTORY_RESET:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_CFG;
        break;

    case MMI_DEV_FACTORY_RESET:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_NORMAL;
        if (app_db.ignore_led_mode)
        {
            app_db.factory_reset_ignore_led_and_vp = true;
        }
        break;

    case MMI_DEV_PHONE_RECORD_RESET:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_PHONE;
        if (app_db.ignore_led_mode)
        {
            app_db.factory_reset_ignore_led_and_vp = true;
        }
        break;

    case MMI_DEV_FACTORY_RESET_BY_SPP:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_NORMAL;
        break;

    case MMI_DEV_PHONE_RECORD_RESET_BY_SPP:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_PHONE;
        break;

    case MMI_DEV_RESET_TO_UNINITIAL_STATE:
        app_db.factory_reset_clear_mode = FACTORY_RESET_CLEAR_ALL;
        break;
    }
}

bool app_mmi_is_allow_factory_reset(void)
{
    bool allow_factory_reset = false;

    if (app_cfg_const.enable_factory_reset_whitout_limit == 1)
    {
        allow_factory_reset = true;
    }
    else if ((app_cfg_const.enable_factory_reset_when_in_the_box == 1) &&
             app_device_is_in_the_box())
    {
        /* power off, only allow factory reset MMI */
        allow_factory_reset = true;
    }
    else if ((app_cfg_const.enable_factory_reset_when_in_the_box == 0)
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
             && (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
#endif
             && (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        allow_factory_reset = true;
    }

    return allow_factory_reset;
}

#if F_APP_LEA_SUPPORT
static bool app_mmi_check_lea_action(uint8_t action)
{
    bool lea_only_mmi = true;
    mtc_topology_dm(MTC_TOPO_EVENT_MMI);

    switch (action)
    {
    case MMI_BIG_START:
    case MMI_BIG_STOP:
        {
            if (app_cfg_const.bis_mode == LEA_BROADCAST_SINK)
            {
                app_lea_mgr_mmi_handle(action);
            }
        }
        break;

#if F_APP_CCP_SUPPORT
    case MMI_HF_LAST_NUMBER_REDIAL:
        {
            if (app_lea_mgr_find_call_device_by_priority())
            {
                app_lea_mgr_mmi_handle(action);
            }
            else
            {
                lea_only_mmi = false;
            }
        }
        break;

    case MMI_HF_END_OUTGOING_CALL:
    case MMI_HF_ANSWER_CALL:
    case MMI_HF_END_ACTIVE_CALL:
    case MMI_HF_INITIATE_VOICE_DIAL:
    case MMI_HF_CANCEL_VOICE_DIAL:
    case MMI_HF_TRANSFER_CALL:
    case MMI_HF_REJECT_CALL:
    case MMI_HF_JOIN_TWO_CALLS:
    case MMI_HF_SWITCH_TO_SECOND_CALL:
    case MMI_HF_RELEASE_HELD_OR_WAITING_CALL:
    case MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL:
#endif
#if F_APP_MCP_SUPPORT
    case MMI_AV_PLAY_PAUSE:
    case MMI_AV_STOP:
    case MMI_AV_FWD:
    case MMI_AV_BWD:
    case MMI_AV_FASTFORWARD:
    case MMI_AV_REWIND:
    case MMI_AV_FASTFORWARD_STOP:
    case MMI_AV_REWIND_STOP:
#endif
    case MMI_DEV_SPK_VOL_UP:
    case MMI_DEV_SPK_VOL_DOWN:
    case MMI_DEV_SPK_MUTE:
    case MMI_DEV_SPK_UNMUTE:
    case MMI_DEV_MIC_MUTE_UNMUTE:
    case MMI_DEV_MIC_MUTE:
    case MMI_DEV_MIC_UNMUTE:
        {
            if (mtc_get_btmode())
            {
                app_lea_mgr_mmi_handle(action);
            }
            else
            {
                lea_only_mmi = false;
            }
        }
        break;

    default:
        {
            lea_only_mmi = false;
        }
        break;
    }

    return lea_only_mmi;
}
#endif

void app_mmi_hf_reject_call(void)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_TEAMS_FEATURE_SUPPORT
    app_asp_device_send_hook_button_status(1);
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    if (app_teams_multilink_get_voice_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT)
    {
        if (app_teams_multilink_get_second_hfp_priority_index() == 0xff)
        {
#if F_APP_USB_HID_SUPPORT
            app_usb_mmi_handle_action(action);
#endif
        }
        else
        {
            bt_hfp_call_terminate_req(
                app_db.br_link[app_teams_multilink_get_second_hfp_priority_index()].bd_addr);
        }
        return;
    }
#endif
#endif
    T_APP_BR_LINK *p_link;
    T_APP_AUDIO_TONE_TYPE tone_type = TONE_TYPE_MAX;

    p_link = &(app_db.br_link[active_hf_idx]);
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
#endif
    if (p_link != NULL)
    {
        app_hfp_stop_ring_alert_timer();

        tone_type = (T_APP_AUDIO_TONE_TYPE)app_hfp_get_call_in_tone_type(p_link);
        app_audio_tone_type_cancel(tone_type, true);
        if (bt_hfp_call_terminate_req(p_link->bd_addr))
        {
            app_db.reject_call_by_key = true;
        }
    }
}

void app_mmi_hf_cancel_voice_dial(void)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
#endif
#if F_APP_DURIAN_SUPPORT
    app_durian_mmi_voice_recognition_disable(active_hf_idx);
#else
    bt_hfp_voice_recognition_disable_req(app_db.br_link[active_hf_idx].bd_addr);
#endif
}

void app_mmi_hf_end_outgoing_call(void)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
#endif
    bt_hfp_call_terminate_req(app_db.br_link[active_hf_idx].bd_addr);
}

void app_mmi_hf_end_active_call(void)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
    app_asp_device_send_hook_button_status(1);
#endif
#if HARMAN_AUTO_ACCEPT_SECOND_CALL_SUPPORT
    if (app_db.br_link[app_hfp_get_active_idx()].call_status > APP_CALL_ACTIVE)
    {
        bt_hfp_release_active_call_accept_held_or_waiting_call_req(app_db.br_link[active_hf_idx].bd_addr);
    }
    else
#endif
    {
        bt_hfp_call_terminate_req(app_db.br_link[active_hf_idx].bd_addr);
    }
}

void app_mmi_handle_action(uint8_t action)
{
    APP_PRINT_INFO1("app_mmi_handle_action: action 0x%02x", action);

#if F_APP_HARMAN_FEATURE_SUPPORT
#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        //finder stop ring
        T_GFPS_MSG_RING_STATE ring_type = app_gfps_msg_get_ring_state();
        //dult stop ring
        T_DULT_RING_STATE dult_ring_state = app_dult_device_get_ring_state();

        if ((ring_type != GFPS_MSG_RING_STOP) || (dult_ring_state != DULT_RING_STATE_STOPPED))
        {
            APP_PRINT_INFO0("app_mmi_handle_action: keypress to stop gfps finder or dult ringtone");

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (ring_type != GFPS_MSG_RING_STOP)
                {
                    app_gfps_msg_handle_ring_event(GFPS_ALL_STOP);

                    uint8_t ring_state = GFPS_FINDER_RING_BUTTON_STOP;
                    app_gfps_finder_send_ring_rsp(ring_state);
                }

                if (dult_ring_state != DULT_RING_STATE_STOPPED)
                {
                    app_dult_device_sound_stop(0xFF, NULL);
                }
            }
            else
            {
                //maybe relay to primary bud and stop ring
            }

            return;
        }
    }
#endif

    if (action == MMI_DEV_FACTORY_RESET)
    {
        if ((app_hfp_sco_is_connected()) || app_hfp_hfp_ring_timer_check())
        {
            return;
        }
        else if (app_link_get_connected_src_num())
        {
            action = MMI_DEV_FACTORY_RESET_BY_SPP;
        }

        APP_PRINT_TRACE1("app_mmi_handle_action: refine action 0x%02x", action);
    }
#endif

#if F_APP_AUTO_POWER_TEST_LOG
    TEST_PRINT_INFO1("app_mmi_handle_action: action 0x%02x", action);
#endif

    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    if (action == MMI_AV_PLAY_PAUSE)
    {
        if ((app_link_get_lea_link_num() != 0) && (mtc_is_lea_cis_stream(NULL) == false) &&
            (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED ||
             app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED))
        {
            // BT and 2.4G both stream idle, 2.4G has high priority
            mtc_set_btmode(MULTI_PRO_BT_CIS);
        }
    }
#endif

#if F_APP_LEA_SUPPORT
    if (app_mmi_check_lea_action(action))
    {
        return;
    }
#endif

    switch (action)
    {
#if 0
    case MMI_DEV_GAMING_MODE_SWITCH:
        {
#if F_APP_LEA_SUPPORT
            app_lea_mgr_mmi_handle(MMI_DEV_GAMING_MODE_SWITCH);
#endif
            if (app_link_get_b2s_link_num() == 0)
            {
                APP_PRINT_TRACE0("app_mmi_handle_action b2s = 0 return ");
                return;
            }

#if F_APP_DONGLE_ALWAYS_GAMING_MODE
            if (app_db.gaming_mode && app_link_is_dongle_active_link())
            {
                // skip exit gaming mode, if active a2dp link is dongle
                return;
            }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
            if ((app_db.remote_is_dongle) && (app_db.gaming_mode_request_is_received == false))
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_dongle_gaming_mode_request(!app_db.gaming_mode);
                }

                return;
            }
#endif

            app_mmi_switch_gaming_mode();
        }
        break;
#endif

    case MMI_AV_PLAY_PAUSE:
        {
#if F_APP_LOCAL_PLAYBACK_SUPPORT
            if (app_db.sd_playback_switch == 1)
            {
                app_playback_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
                return;
            }
#endif

#if F_APP_TEAMS_MULTILINK_NOTIFICATION_FLAG
            if (JUDGE_EVENT_A2DP_START != app_teams_multilink_get_music_status())
            {
                active_a2dp_idx = app_teams_multilink_get_active_index();
            }
#endif
#if F_APP_MUTILINK_TRIGGER_HIGH_PRIORITY
            if (app_cfg_const.enable_multi_link)
            {
                if (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED ||
                    app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED)
                {
                    app_idx = app_mmi_get_highest_priority_index(A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK);
                    //When active a2dp index is idle, DUT will trigger highest priority SRC to play music
                }
            }
#endif
#if F_APP_HARMAN_FEATURE_SUPPORT
            if ((app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED ||
                 app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED) &&
                (app_harman_aling_active_hfp_idx_get() != 0xff))
            {
                active_a2dp_idx = app_harman_aling_active_hfp_idx_get();
            }
#endif
            //Spk2 do not handle a2dp/avrcp
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
#if F_APP_GAMING_DONGLE_SUPPORT && F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            if (app_multilink_customer_is_dongle_priority_higher() && app_link_get_b2s_link_num() > 1)
            {
                T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();

                if (p_dongle_link && !app_db.br_link[active_a2dp_idx].streaming_fg)
                {
                    // dongle exists and active a2dp idx not streaming, change_active_to_dongle
                    active_a2dp_idx = p_dongle_link->id;
                }
            }
#endif

            if ((app_db.br_link[active_a2dp_idx].connected_profile & A2DP_PROFILE_MASK) &&
                (app_db.br_link[active_a2dp_idx].connected_profile & AVRCP_PROFILE_MASK))
            {
                APP_PRINT_TRACE3("MMI_AV_PLAY_PAUSE: active_a2dp_idx %d, avrcp_play_status %d, bud_stream_state %d",
                                 active_a2dp_idx, app_db.br_link[active_a2dp_idx].avrcp_play_status,
                                 app_audio_get_bud_stream_state());

                if (app_db.br_link[active_a2dp_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    // Update play status after AVRCP play status event received
                    bt_avrcp_play(app_db.br_link[active_a2dp_idx].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
                    app_harman_is_record_a2dp_active_ever_set(true);
#endif
                    app_db.br_link[active_a2dp_idx].waiting_play_status_change  = true;

                    app_audio_tone_type_cancel(TONE_AUDIO_PAUSED, true);
                    app_audio_tone_type_play(TONE_AUDIO_PLAYING, true, true);
                }
                else
                {
                    bt_avrcp_pause(app_db.br_link[active_a2dp_idx].bd_addr);
                    app_db.br_link[active_a2dp_idx].waiting_paused_status_change = true;

                    app_audio_tone_type_cancel(TONE_AUDIO_PLAYING, true);
                    app_audio_tone_type_play(TONE_AUDIO_PAUSED, true, true);

                }
            }
            else
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                uint8_t bd_addr[6];

                if (app_bond_b2s_addr_get(1, bd_addr) == true)
                {
#if F_APP_GAMING_DONGLE_SUPPORT && (F_APP_ERWS_SUPPORT == 0)
                    if (app_cfg_const.enable_dongle_dual_mode)
                    {
                        app_dongle_dual_mode_linkback();
                    }
                    else
#endif
                    {
                        app_bt_policy_default_connect(bd_addr,
                                                      A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK, true);
                    }
                }
#endif
            }
        }
        break;

    case MMI_AV_STOP:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            bt_avrcp_stop(app_db.br_link[active_a2dp_idx].bd_addr);
        }
        break;

    case MMI_AV_FWD:
        {
#if F_APP_LOCAL_PLAYBACK_SUPPORT
            if (app_db.sd_playback_switch == 1)
            {
                app_playback_mmi_handle_action(MMI_SD_PLAYBACK_FWD);
                return;
            }
#endif
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
#if F_APP_DURIAN_SUPPORT
            app_durian_mmi_play_fwd();
#else
            if ((app_cfg_const.enable_av_fwd_bwd_only_when_playing == 0) ||
                (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
            {
                bt_avrcp_forward(app_db.br_link[active_a2dp_idx].bd_addr);
                app_audio_tone_type_play(TONE_AUDIO_FORWARD, false, true);
            }
#endif
        }
        break;

    case MMI_AV_BWD:
        {
#if F_APP_LOCAL_PLAYBACK_SUPPORT
            if (app_db.sd_playback_switch == 1)
            {
                app_playback_mmi_handle_action(MMI_SD_PLAYBACK_BWD);
                return;
            }
#endif
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
#if F_APP_DURIAN_SUPPORT
            app_durian_mmi_play_bwd();
#else
            if ((app_cfg_const.enable_av_fwd_bwd_only_when_playing == 0) ||
                (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
            {

                bt_avrcp_backward(app_db.br_link[active_a2dp_idx].bd_addr);
                app_audio_tone_type_play(TONE_AUDIO_BACKWARD, false, true);
            }
#endif
        }
        break;

#if 0
    case MMI_AV_FASTFORWARD:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }

            bt_avrcp_fast_forward_start(app_db.br_link[active_a2dp_idx].bd_addr);
        }
        break;

    case MMI_AV_REWIND:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }

            bt_avrcp_rewind_start(app_db.br_link[active_a2dp_idx].bd_addr);
        }
        break;

    case MMI_AV_FASTFORWARD_STOP:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }

            bt_avrcp_fast_forward_stop(app_db.br_link[active_a2dp_idx].bd_addr);
        }
        break;

    case MMI_AV_REWIND_STOP:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }

            bt_avrcp_rewind_stop(app_db.br_link[active_a2dp_idx].bd_addr);
        }
        break;
#endif

    case MMI_HF_ANSWER_CALL:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_asp_device_send_hook_button_status(0);
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            if (app_teams_multilink_get_voice_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT)
            {
                if (app_teams_multilink_get_second_hfp_priority_index() == 0xff)
                {
#if F_APP_USB_HID_SUPPORT
                    app_usb_mmi_handle_action(action);
#endif
                }
                else
                {
                    bt_hfp_call_answer_req(app_db.br_link[app_teams_multilink_get_second_hfp_priority_index()].bd_addr);
                }
                return;
            }
#endif
#endif
            T_APP_BR_LINK *p_link;
            T_APP_AUDIO_TONE_TYPE tone_type = TONE_TYPE_MAX;
            p_link = &(app_db.br_link[active_hf_idx]);
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            if (p_link != NULL)
            {
                app_hfp_stop_ring_alert_timer();

                tone_type = (T_APP_AUDIO_TONE_TYPE)app_hfp_get_call_in_tone_type(p_link);
                app_audio_tone_type_cancel(tone_type, true);
                bt_hfp_call_answer_req(p_link->bd_addr);
            }
        }
        break;

    case MMI_HF_REJECT_CALL:
        {
            app_mmi_hf_reject_call();
        }
        break;

    case MMI_HF_END_ACTIVE_CALL:
        {
            app_mmi_hf_end_active_call();
        }
        break;

    case MMI_HF_END_OUTGOING_CALL:
        {
            app_mmi_hf_end_outgoing_call();
        }
        break;

    case MMI_HF_TRANSFER_CALL:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            if ((app_hfp_get_call_status() == APP_CALL_INCOMING) &&
                !app_db.br_link[active_hf_idx].is_inband_ring)
            {
                //nothing
            }
            else
            {
                if (app_db.br_link[active_hf_idx].sco_handle)// to phone
                {
                    app_audio_tone_type_play(TONE_HF_TRANSFER_TO_PHONE, false, true);
                    bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
                }
                else //to bud
                {
                    app_audio_tone_type_play(TONE_HF_TRANSFER_TO_PHONE, false, true);
                    app_bt_sniffing_hfp_connect(app_db.br_link[active_hf_idx].bd_addr);
                }
            }
        }
        break;

    case MMI_HF_CANCEL_VOICE_DIAL:
        {
            app_mmi_hf_cancel_voice_dial();
        }
        break;

    case MMI_HF_LAST_NUMBER_REDIAL:
        {
            uint8_t app_idx = MAX_BR_LINK_NUM - 1;
            uint8_t link_number;
            link_number = app_link_get_b2s_link_num_with_profile(HFP_PROFILE_MASK | HSP_PROFILE_MASK);
            if (link_number > 1)
            {
                if (app_cfg_const.enable_last_num_redial_always_by_first_phone)
                {
                    app_idx = app_db.first_hf_index;
                }
                else if (app_cfg_const.enable_last_num_redial_always_by_last_phone)
                {
                    app_idx = app_db.last_hf_index;
                }
            }
            else if (link_number == 1)
            {
                app_idx = app_hfp_get_active_idx();
            }
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            if (app_db.br_link[app_idx].app_hf_state == APP_HF_STATE_CONNECTED)
            {
                if (app_hfp_get_call_status() == APP_CALL_IDLE)
                {
                    bt_hfp_dial_last_number_req(app_db.br_link[app_idx].bd_addr);
                    app_audio_tone_type_play(TONE_HF_CALL_REDIAL, false, true);
                }
            }
            else
            {
                uint8_t bd_addr[6];

                if (app_bond_b2s_addr_get(1, bd_addr) == true)
                {
                    app_bt_policy_default_connect(bd_addr,
                                                  HSP_PROFILE_MASK | HFP_PROFILE_MASK, true);
                }
            }
        }
        break;

    case MMI_HF_JOIN_TWO_CALLS:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY ||
                (app_db.br_link[app_hfp_get_active_idx()].call_status <= APP_CALL_ACTIVE))
            {
                return;
            }

            bt_hfp_join_two_calls_req(app_db.br_link[active_hf_idx].bd_addr);
        }
        break;

    case MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
                    bt_hfp_call_terminate_req(app_db.br_link[active_hf_idx].bd_addr);
                    bt_hfp_call_answer_req(app_db.br_link[inactive_hf_idx].bd_addr);
                    app_hfp_set_active_idx(app_db.br_link[inactive_hf_idx].bd_addr);
                    app_bond_set_priority(app_db.br_link[inactive_hf_idx].bd_addr);
                }

            }
            else if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
                    bt_hfp_call_terminate_req(app_db.br_link[active_hf_idx].bd_addr);
#if F_APP_CALL_HOLD_SUPPORT
                    if (app_db.br_link[inactive_hf_idx].call_status == APP_CALL_HELD)
                    {
                        bt_hfp_held_call_accept_req(app_db.br_link[inactive_hf_idx].bd_addr);
                    }
#endif
                    app_hfp_set_active_idx(app_db.br_link[inactive_hf_idx].bd_addr);
                    app_bond_set_priority(app_db.br_link[inactive_hf_idx].bd_addr);
                }
            }
            else
            {
                bt_hfp_release_active_call_accept_held_or_waiting_call_req(app_db.br_link[active_hf_idx].bd_addr);
            }
        }
        break;

    case MMI_HF_RELEASE_HELD_OR_WAITING_CALL:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif
            if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
                    bt_hfp_call_terminate_req(app_db.br_link[inactive_hf_idx].bd_addr);
                }
            }
            else if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
#if F_APP_CALL_HOLD_SUPPORT
                    if (app_db.br_link[inactive_hf_idx].call_status == APP_CALL_HELD)
                    {
                        bt_hfp_held_call_release_req(app_db.br_link[inactive_hf_idx].bd_addr);
                    }
                    else
#endif
                    {
                        bt_hfp_call_terminate_req(app_db.br_link[inactive_hf_idx].bd_addr);
                    }
                }
            }
            else
            {
                bt_hfp_release_held_or_waiting_call_req(app_db.br_link[active_hf_idx].bd_addr);
            }
        }
        break;

    case MMI_HF_SWITCH_TO_SECOND_CALL:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif

            if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
                    app_hfp_stop_delay_role_switch();
#if F_APP_CALL_HOLD_SUPPORT
                    bt_hfp_call_hold_req(app_db.br_link[active_hf_idx].bd_addr);
#endif

#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                    app_multi_active_hfp_transfer(inactive_hf_idx, true, false);
#endif
                    bt_hfp_call_answer_req(app_db.br_link[inactive_hf_idx].bd_addr);
                }
            }
            else if (app_hfp_get_call_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD)
            {
                uint8_t inactive_hf_idx;

                for (inactive_hf_idx = 0; inactive_hf_idx < MAX_BR_LINK_NUM; inactive_hf_idx++)
                {
                    if (inactive_hf_idx != active_hf_idx)
                    {
                        if (app_db.br_link[inactive_hf_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            break;
                        }
                    }
                }

                if (inactive_hf_idx < MAX_BR_LINK_NUM)
                {
                    app_hfp_stop_delay_role_switch();
#if F_APP_CALL_HOLD_SUPPORT
                    bt_hfp_call_hold_req(app_db.br_link[active_hf_idx].bd_addr);
                    bt_hfp_held_call_accept_req(app_db.br_link[inactive_hf_idx].bd_addr);
#endif
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                    app_multi_active_hfp_transfer(inactive_hf_idx, true, false);
#endif
                }
            }
            else
            {
                bt_hfp_hold_active_call_accept_held_or_waiting_call_req(app_db.br_link[active_hf_idx].bd_addr);
            }
        }
        break;

#if 0
    case MMI_HF_QUERY_CURRENT_CALL_LIST:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }

            bt_hfp_current_call_list_req(app_db.br_link[active_hf_idx].bd_addr);
        }
        break;
#endif

    case MMI_HF_INITIATE_VOICE_DIAL:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
#endif

            uint8_t app_idx = 0;
#if F_APP_MUTILINK_TRIGGER_HIGH_PRIORITY
            if (app_cfg_const.enable_multi_link)
            {
                app_idx = app_mmi_get_highest_priority_index(HFP_PROFILE_MASK);
            }
            else
#endif
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                app_idx = app_mmi_harman_get_active_hf_idx();
#else
                app_idx = app_hfp_get_active_idx();
#endif
            }

            app_mmi_voice_recognition(app_idx);
        }
        break;

    case MMI_DEV_POWER_ON:
        {
            if (app_db.device_state == APP_DEVICE_STATE_OFF_ING)
            {
                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) || app_link_get_b2s_link_num())
                {
                    app_bt_policy_disconnect_all_link();
                }

                app_mmi_modify_reboot_check_times(REBOOT_CHECK_POWER_ON_MAX_TIMES);
            }
            else if (app_db.device_state == APP_DEVICE_STATE_OFF)
            {
#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
                if (!app_harman_discharger_ntc_check_valid())
                {
                    app_db.need_delay_power_on = true;
                    return;
                }
#endif

#if HARMAN_OTA_COMPLETE_NONEED_POWERON
                APP_PRINT_TRACE1("MMI_DEV_POWER_ON: harman_ota_complete: %d", app_cfg_nv.harman_ota_complete);
                if (app_cfg_nv.harman_ota_complete)
                {
                    app_cfg_nv.harman_ota_complete = 0;
                    app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 4);

                    app_db.is_long_press_power_on_play_vp = true;
                }
#endif

                if (!app_db.is_long_press_power_on_play_vp && !mp_hci_test_mode_is_running())
                {
#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_cfg_const.enable_dongle_dual_mode && app_cfg_nv.is_dual_mode)
                    {
                        app_audio_tone_type_play(TONE_POWER_ON_BT_OR_BT_24G_MODE, false, false);
                    }
                    else
#endif
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        if (app_cfg_nv.language_status == 0)
                        {
                            app_audio_tone_type_play(TONE_HARMAN_POWER_ON, false, false);
                        }
                        else
#endif
                        {
                            app_audio_tone_type_play(TONE_POWER_ON, false, false);
                        }
                    }
                }

#if F_APP_USB_SUSPEND_SUPPORT
                if (app_db.is_usb_suspend)
                {
                    app_usb_suspend_disable();
                }
#endif

                app_db.is_long_press_power_on_play_vp = false;

                sys_mgr_power_on();
            }
        }
        break;

    case MMI_DEV_POWER_OFF:
        {
            APP_PRINT_TRACE3("app_mmi_handle_action: %d, 0x%x, %d", app_db.device_state,
                             app_db.power_off_cause, app_key_is_enter_pairing());
            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
                app_harman_find_my_buds_ring_stop();
#endif

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
                if (app_cfg_const.discharge_ntc_protection_thermistor_option)
                {
                    app_harman_adc_ntc_check_timer_stop();
                }
#endif

#if F_APP_LEA_SUPPORT
                app_lea_mgr_mmi_handle(MMI_DEV_POWER_OFF);
#endif

#if F_APP_SAIYAN_MODE
                app_data_capture_saiyan_mode_ctl(0, 0x0);
#endif
                app_device_state_change(APP_DEVICE_STATE_OFF_ING);

                app_dlps_disable(APP_DLPS_ENTER_CHECK_WAIT_RESET);

#if F_APP_ERWS_SUPPORT
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_relay_async_single(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_POWERING_OFF);
                }
#endif

                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && app_db.gaming_mode)
                {
                    app_bt_policy_stop_a2dp_and_sco();
                }

                app_stop_timer(&timer_idx_reboot_check);
                app_mmi_reboot_check_timer_start(500);
                app_timer_register_pm_excluded(&timer_idx_reboot_check);

                if (mp_hci_test_mode_is_running())
                {
                    mp_hci_test_mmi_handle_action(action);
                }
                else
                {
                    app_sniff_mode_disable_all();
                    app_mmi_power_off();
                }
#if F_APP_HARMAN_FEATURE_SUPPORT
                app_harman_is_record_a2dp_active_ever_set(false);
                app_harman_is_already_connect_one_set(false);
#endif
            }
        }
        break;

    case MMI_DEV_FORCE_ENTER_PAIRING_MODE:
    case MMI_DEV_ENTER_PAIRING_MODE:
        {
#if GFPS_FEATURE_SUPPORT
            if (extend_app_cfg_const.gfps_support)
            {
                app_gfps_force_enter_pairing_mode(GFPS_KEY_FORCE_ENTER_PAIR_MODE);
            }
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            if (app_teams_multilink_get_voice_status() != APP_CALL_IDLE)
#else
#if F_APP_HARMAN_FEATURE_SUPPORT
            if ((app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
#if HARMAN_NOT_ALLOW_ENTER_PAIR_MODE_ON_CALLING_SUPPORT
                || (app_hfp_get_call_status() != BT_HFP_CALL_IDLE)
#endif
               )
#else
            if (app_hfp_get_call_status() != BT_HFP_CALL_IDLE)
#endif
#endif
            {
                //Not allow to enter pairing mode when calling
                break;
            }
            else
            {
                if (app_db.device_state == APP_DEVICE_STATE_OFF)
                {
                    sys_mgr_power_on();
                }

#if F_APP_DURIAN_SUPPORT
                app_durian_mmi_adv_enter_pairing();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if (action == MMI_DEV_FORCE_ENTER_PAIRING_MODE &&
                    app_dongle_get_mode() == PAIRING_MODE_BT_OR_BT_24G)
                {
                    app_bt_policy_exit_pairing_mode();
                }
#endif

                if (action == MMI_DEV_FORCE_ENTER_PAIRING_MODE)
                {
                    app_bt_policy_enter_pairing_mode(true, true);
                }
                else
                {
                    app_bt_policy_enter_pairing_mode(false, true);
                }
            }
        }
        break;

#if 0
    case MMI_DEV_EXIT_PAIRING_MODE:
        {
            app_bt_policy_exit_pairing_mode();
#if F_APP_LEA_SUPPORT
            app_lea_adv_stop();
#endif

#if GFPS_FEATURE_SUPPORT
            if (extend_app_cfg_const.gfps_support)
            {
                app_gfps_force_enter_pairing_mode(GFPS_EXIT_PAIR_MODE);
            }
#endif
        }
        break;
#endif

    case MMI_DEV_MULTILINK_ON:
        {
            if (app_cfg_const.enable_multi_link == 0)
            {
                app_multi_switch_by_mmi(true);
                app_audio_tone_type_play(TONE_DEV_MULTILINK_ON, false, false);
#if F_APP_ADP_CMD_SUPPORT
                app_adp_cmd_factory_reset_link_dis(1000);
#endif
            }
        }
        break;

#if 0
    case MMI_DEV_LINK_LAST_DEVICE:
        {
            uint32_t bond_flag;
            uint32_t plan_profs;

            uint8_t bond_num = app_bond_b2s_num_get();
            if (app_bt_policy_get_state() == BP_STATE_STANDBY)
            {
                for (uint8_t i = 1; i <= bond_num; i++)
                {
                    if (app_bond_b2s_addr_get(i, app_db.bt_addr_disconnect))
                    {
                        bt_bond_flag_get(app_db.bt_addr_disconnect, &bond_flag);
                        if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                        {
                            plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);

                            app_bt_policy_default_connect(app_db.bt_addr_disconnect,  plan_profs, false);
                            break;
                        }
                    }
                }
            }
        }
        break;
#endif

    case MMI_DEV_TOOLING_FACTORY_RESET:
    case MMI_DEV_CFG_FACTORY_RESET:
    case MMI_DEV_FACTORY_RESET:
    case MMI_DEV_FACTORY_RESET_BY_SPP:
    case MMI_DEV_PHONE_RECORD_RESET:
    case MMI_DEV_PHONE_RECORD_RESET_BY_SPP:
    case MMI_DEV_RESET_TO_UNINITIAL_STATE:
        {
#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
            app_harman_find_my_buds_ring_stop();
#endif

            app_dlps_disable(APP_DLPS_ENTER_CHECK_WAIT_RESET);
            app_mmi_check_factory_reset_behavior(action);

#if F_APP_IO_OUTPUT_SUPPORT
            if (app_cfg_const.enable_external_mcu_reset)
            {
                app_io_output_ctrl_ext_mcu_pin(DISABLE);
            }
#endif

            if (!app_db.factory_reset_ignore_led_and_vp)
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                if (app_cfg_nv.language_status == 0)
                {
                    app_audio_tone_type_play(TONE_HARMAN_FACTORY_RESET, false, false);
                }
                else
#endif
                {
                    app_audio_tone_type_play(TONE_FACTORY_RESET, false, false);
                }
                /*
                    Stop led timer, and start factory reset LED immediately,
                    then enter power down mode as soon as possible.
                */
                app_led_timer_stop();
                if (app_led_check_set_mode_available(LED_MODE_FACTORY_RESET) == true)
                {
                    app_led_change_mode(LED_MODE_FACTORY_RESET, true, false);
                }
            }

            app_stop_timer(&timer_idx_reboot_check);
            app_mmi_reboot_check_timer_start(500);

            app_db.waiting_factory_reset = true;
            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
                app_device_state_change(APP_DEVICE_STATE_OFF_ING);
                app_sniff_mode_disable_all();
                sys_mgr_power_off();
            }
        }
        break;

    case MMI_MEMORY_DUMP:
        {
            hal_debug_memory_dump();
        }
        break;

    case MMI_DEV_SPK_VOL_UP:
        {
            app_key_set_volume_status(true);
            app_mmi_volume_up();
        }
        break;

    case MMI_DEV_SPK_VOL_DOWN:
        {
            if ((app_cfg_const.rws_circular_vol_up_when_solo_mode) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
            {
                APP_PRINT_TRACE0("rws_circular_vol_up_when_solo_mode");
                app_key_set_volume_status(true);
                app_mmi_volume_up();
                break;
            }
            app_key_set_volume_status(true);
            app_mmi_volume_down();
        }
        break;

    case MMI_DEV_SPK_MUTE:
    case MMI_DEV_SPK_UNMUTE:
        {
            app_mmi_spk_mute_set(action);
        }
        break;

    case MMI_DEV_MIC_MUTE_UNMUTE:
        {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
            uint8_t teams_active_device_idx = app_teams_multilink_get_active_index();
            if (!app_teams_audio_is_call_hold_without_active_call(teams_active_device_idx))
            {
                return;
            }
#endif
            if (app_audio_check_mic_mute_enable() == true)
            {
                if (app_audio_is_mic_mute())
                {
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        if (audio_volume_in_is_muted(AUDIO_STREAM_TYPE_VOICE))
                        {
                            app_audio_enable_play_mic_unmute_tone(true);
                        }
                    }
                    app_audio_set_mic_mute_status(0);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    app_teams_audio_set_bt_mute_status(teams_active_device_idx, false);
#endif
                }
                else
                {
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        if (!audio_volume_in_is_muted(AUDIO_STREAM_TYPE_VOICE))
                        {
                            app_audio_enable_play_mic_mute_tone(true);
                        }
                    }
                    app_audio_set_mic_mute_status(1);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    app_teams_audio_set_bt_mute_status(teams_active_device_idx, true);
#endif
                }

                app_hfp_mute_ctrl();
#if F_APP_USB_AUDIO_SUPPORT
                app_usb_audio_mute_ctrl();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.output_ind3_link_mic_toggle)
                {
                    app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION3_TOGGLE);
                }
#endif
            }
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT || F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
            else
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
                if (app_slide_switch_mic_mute_toggle_support())
#endif
                {
                    if (app_audio_is_mic_mute())
                    {
                        app_audio_set_mic_mute_status(false);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        app_teams_audio_set_bt_mute_status(active_hf_idx, false);
#endif
                        app_audio_tone_type_play(TONE_MIC_MUTE_OFF, true, true);
                    }
                    else
                    {
                        app_audio_set_mic_mute_status(true);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                        app_teams_audio_set_bt_mute_status(active_hf_idx, true);
#endif
                        app_audio_tone_type_play(TONE_MIC_MUTE_ON, true, true);
                    }
                }

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
            if (app_teams_audio_get_global_mute_status())
            {
                app_asp_device_send_mute_button_status(1);
            }
            else
            {
                app_asp_device_send_mute_button_status(0);
            }
#endif
#endif
        }
        break;

    case MMI_DEV_MIC_MUTE:
        {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
            uint8_t teams_active_device_idx = app_teams_multilink_get_active_index();
#endif
            if (app_audio_check_mic_mute_enable() == true)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    if (!audio_volume_in_is_muted(AUDIO_STREAM_TYPE_VOICE))
                    {
                        app_audio_enable_play_mic_mute_tone(true);
                    }
                }
                app_audio_set_mic_mute_status(1);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                app_teams_audio_set_bt_mute_status(teams_active_device_idx, true);
#endif
                app_hfp_mute_ctrl();
#if F_APP_USB_AUDIO_SUPPORT
                app_usb_audio_mute_ctrl();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.output_ind3_link_mic_toggle)
                {
                    app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION3_TOGGLE);
                }
#endif
            }
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT || F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
            else
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
                if (app_slide_switch_mic_mute_toggle_support())
#endif
                {
                    app_audio_set_mic_mute_status(1);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    app_teams_audio_set_bt_mute_status(teams_active_device_idx, true);
#endif
                    app_audio_tone_type_play(TONE_MIC_MUTE_ON, true, true);
                }
#endif
        }
        break;

    case MMI_DEV_MIC_UNMUTE:
        {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
            uint8_t teams_active_device_idx = app_teams_multilink_get_active_index();
#endif
            if (app_audio_check_mic_mute_enable() == true)
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                    (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    if (audio_volume_in_is_muted(AUDIO_STREAM_TYPE_VOICE))
                    {
                        app_audio_enable_play_mic_unmute_tone(true);
                    }
                }
                app_audio_set_mic_mute_status(0);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                app_teams_audio_set_bt_mute_status(teams_active_device_idx, false);
#endif
                app_hfp_mute_ctrl();
#if F_APP_USB_AUDIO_SUPPORT
                app_usb_audio_mute_ctrl();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.output_ind3_link_mic_toggle)
                {
                    app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION3_TOGGLE);
                }
#endif
            }
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT || F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
            else
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
                if (app_slide_switch_mic_mute_toggle_support())
#endif
                {
                    app_audio_set_mic_mute_status(0);
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                    app_teams_audio_set_bt_mute_status(teams_active_device_idx, false);
#endif
                    app_audio_tone_type_play(TONE_MIC_MUTE_OFF, true, true);
                }
#endif
        }
        break;

    case MMI_DEV_MIC_VOL_UP:
        {
            app_key_set_volume_status(true);
            //microphone_vol_change(1);
        }
        break;

    case MMI_DEV_MIC_VOL_DOWN:
        {
            app_key_set_volume_status(true);
            //microphone_vol_change(0);
        }
        break;

#if 0
    case MMI_DEV_MIC_SWITCH:
        {
            app_audio_mic_switch(0);
        }
        break;
#endif

    case MMI_AUDIO_EQ_SWITCH:
        {
            uint8_t eq_num = eq_utils_num_get(SPK_SW_EQ, app_db.spk_eq_mode);

            if (eq_num != 0)
            {
                app_cfg_nv.eq_idx++;

                if (app_cfg_nv.eq_idx >= eq_num)
                {
                    app_cfg_nv.eq_idx = 0;
                }

                app_eq_sync_idx_accord_eq_mode(app_db.spk_eq_mode, app_cfg_nv.eq_idx);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_eq_play_audio_eq_tone();
                }

#if (F_APP_USER_EQ_SUPPORT == 1)
                if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
                {
                    app_eq_index_set(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);
                    app_report_eq_idx(EQ_INDEX_REPORT_BY_UPDATE_EQ_INDEX);
                }
                else
#endif
                {
                    //not set default eq when audio connect,enable when set eq para from phone
                    if (!app_db.eq_ctrl_by_src
#if F_APP_LINEIN_SUPPORT
                        || (app_db.spk_eq_mode == LINE_IN_MODE)    /* line in eq not support adjust from phone */
#endif
                       )
                    {
                        app_eq_index_set(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);
                    }
                    else
                    {
                        app_report_eq_idx(EQ_INDEX_REPORT_BY_SWITCH_EQ_INDEX);
                    }
                }
            }
            else
            {
                APP_PRINT_ERROR0("app_mmi_handle_action: eq need to enable");
            }
        }
        break;

#if F_APP_APT_SUPPORT
    case MMI_AUDIO_APT:
        {
            if (!app_apt_open_condition_check())
            {
                //If APT is not allowed to be triggered, disable MMI
                break;
            }

            if (app_cfg_const.enable_direct_apt_on_off)
            {
                app_listening_direct_apt_on_off_handling();
                break;
            }

            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            if (app_apt_is_apt_on_state(*app_db.final_listening_state))
            {
                if (app_listening_is_allow_all_off_condition_check())
                {
                    app_listening_state_machine(EVENT_APT_OFF, true, true);
                }
            }
            else
            {
                if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
                {
                    app_listening_state_machine(EVENT_NORMAL_APT_ON, true, true);
                }
                else if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT)
                {
                    if (app_apt_is_llapt_on_state(app_db.last_llapt_on_state))
                    {
                        app_listening_state_machine(LLAPT_STATE_TO_EVENT(app_db.last_llapt_on_state), true, true);
                    }
                }
            }
        }
        break;

    case MMI_AUDIO_APT_VOL_UP:
        {
#if F_APP_HEARABLE_SUPPORT
            if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
            {
                app_ha_adjust_volume_level(true);
                break;
            }
#endif
            if ((app_apt_is_apt_on_state(app_db.current_listening_state))
#if F_APP_SUPPORT_ANC_APT_COEXIST
                || (app_listening_is_anc_apt_on_state(app_db.current_listening_state))
#endif
               )
            {
                bool is_need_tone_relay = true;

#if F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT
                if (app_cfg_nv.rws_disallow_sync_apt_volume)
                {
                    is_need_tone_relay = false;
                }
#endif
                if (app_cfg_nv.apt_volume_out_level < app_dsp_cfg_vol.apt_volume_out_max)
                {
                    app_cfg_nv.apt_volume_out_level++;
                }

                if (app_cfg_nv.apt_volume_out_level == app_dsp_cfg_vol.apt_volume_out_max)
                {
                    app_led_change_mode(LED_MODE_VOL_MAX_MIN, true, false);
                }
                else
                {
                    app_led_change_mode(LED_MODE_VOL_ADJUST, true, false);
                }

                if (app_cfg_const.enable_apt_circular_vol_up)
                {
                    app_cfg_nv.apt_volume_out_level = (app_cfg_nv.apt_volume_out_level <
                                                       app_dsp_cfg_vol.apt_volume_out_max) ? (app_cfg_nv.apt_volume_out_level + 1) : 0;
                }
                else
                {
                    if (app_cfg_nv.apt_volume_out_level >= app_dsp_cfg_vol.apt_volume_out_max)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                            app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
                            app_cfg_nv.rws_disallow_sync_apt_volume)
                        {
                            app_audio_tone_type_play(TONE_APT_VOL_MAX, false, is_need_tone_relay);
                        }
                    }
                }

#if APT_SUB_VOLUME_LEVEL_SUPPORT
                app_apt_main_volume_set(app_cfg_nv.apt_volume_out_level);
#else
                audio_passthrough_volume_out_set(app_cfg_nv.apt_volume_out_level);
#endif
                app_apt_volume_relay_report_handle();

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
                    app_cfg_nv.rws_disallow_sync_apt_volume)
                {
                    if ((app_cfg_const.enable_apt_circular_vol_up) &&
                        (app_cfg_nv.apt_volume_out_level == app_dsp_cfg_vol.apt_volume_out_max))
                    {
                        app_audio_tone_type_play(TONE_APT_VOL_MAX, false, is_need_tone_relay);
                    }
                    else
                    {
                        app_audio_tone_type_play(TONE_APT_VOLUME_UP, false, is_need_tone_relay);
                    }
                    app_apt_play_apt_volume_tone();
                }
            }
        }
        break;

    case MMI_AUDIO_APT_VOL_DOWN:
        {
#if F_APP_HEARABLE_SUPPORT
            if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
            {
                app_ha_adjust_volume_level(false);
                break;
            }
#endif
            if ((app_apt_is_apt_on_state(app_db.current_listening_state))
#if F_APP_SUPPORT_ANC_APT_COEXIST
                || (app_listening_is_anc_apt_on_state(app_db.current_listening_state))
#endif
               )
            {
                bool is_need_tone_relay = true;

#if F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT
                if (app_cfg_nv.rws_disallow_sync_apt_volume)
                {
                    is_need_tone_relay = false;
                }
#endif
                if (app_cfg_nv.apt_volume_out_level > app_dsp_cfg_vol.apt_volume_out_min)
                {
                    app_cfg_nv.apt_volume_out_level--;
                }

                if (app_cfg_nv.apt_volume_out_level == app_dsp_cfg_vol.apt_volume_out_min)
                {
                    app_led_change_mode(LED_MODE_VOL_MAX_MIN, true, false);
                }
                else
                {
                    app_led_change_mode(LED_MODE_VOL_ADJUST, true, false);
                }

                if (app_cfg_nv.apt_volume_out_level <= app_dsp_cfg_vol.apt_volume_out_min)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
                        app_cfg_nv.rws_disallow_sync_apt_volume)
                    {
                        app_audio_tone_type_play(TONE_APT_VOL_MIN, false, is_need_tone_relay);
                    }
                }

#if APT_SUB_VOLUME_LEVEL_SUPPORT
                app_apt_main_volume_set(app_cfg_nv.apt_volume_out_level);
#else
                audio_passthrough_volume_out_set(app_cfg_nv.apt_volume_out_level);
#endif
                app_apt_volume_relay_report_handle();

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
                    app_cfg_nv.rws_disallow_sync_apt_volume)
                {
                    app_audio_tone_type_play(TONE_APT_VOLUME_DOWN, false, is_need_tone_relay);
                    app_apt_play_apt_volume_tone();
                }
            }
        }
        break;

#if F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT
    case MMI_AUDIO_APT_DISALLOW_SYNC_VOLUME_TOGGLE:
        {
            app_cfg_nv.rws_disallow_sync_apt_volume ^= 1;
            app_cfg_store(&app_cfg_nv.offset_is_dut_test_mode, 4);

            // UI config apt volume sync
            if (!app_cfg_nv.rws_disallow_sync_apt_volume)
            {
                // set apt volume as medium value
#if APT_SUB_VOLUME_LEVEL_SUPPORT
                app_apt_sub_volume_set(APT_SUB_VOLUME_LEVEL_MAX / 2);
#else
                app_cfg_nv.apt_volume_out_level = APT_MAIN_VOLUME_LEVEL_MAX / 2;
                audio_passthrough_volume_out_set(app_cfg_nv.apt_volume_out_level);
#endif

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    //informate primary volume
                    app_apt_volume_relay(false, true);
                }
                else  // single or rws primary
                {
                    //don't report immediately, waiting for report of secondary
                    if (app_apt_volume_relay(false, false) == false)
                    {
                        app_apt_report(EVENT_APT_VOLUME_STATUS, NULL, 0);
                    }
                }
            }

            uint8_t buf[2];

            buf[0] = GET_STATUS_RWS_SYNC_APT_VOL;
            buf[1] = RWS_SYNC_APT_VOLUME;
            app_report_event_broadcast(EVENT_REPORT_STATUS, buf, 2);
        }
        break;
#endif
#endif

        /*
        #if BISTO_FEATURE_SUPPORT == 1
            case MMI_BISTO_FETCH_DOWN:
            case MMI_BISTO_FETCH_UP:
            case MMI_BISTO_FETCH_CLICK:
            case MMI_BISTO_PTT_DOWN:
            case MMI_BISTO_PTT_LONG_PRESS:
            case MMI_BISTO_PTT_UP:
            case MMI_BISTO_FETCH:
            case MMI_BISTO_PTT:
                app_bisto_btns_action((T_MMI_ACTION)action);
                break;
        #endif
        */

#if 0
    case MMI_DEV_NEXT_VOICE_PROMPT_LANGUAGE:
        {
            uint8_t buf[2];

            if (voice_prompt_language_roll() == true)
            {
                app_cfg_nv.voice_prompt_language = voice_prompt_language_get();

                app_cfg_store(&app_cfg_nv.voice_prompt_language, 1);

                buf[0] = (uint8_t)app_cfg_nv.voice_prompt_language;
                buf[1] = voice_prompt_supported_languages_get();

                app_report_event_broadcast(EVENT_LANGUAGE_REPORT, buf, 2);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_audio_tone_type_play(TONE_SWITCH_VP_LANGUAGE, false, true);
                }
            }
        }
        break;
#endif

#if F_APP_OTA_SUPPORT
    case MMI_OTA_OVER_BLE_START:
        {
            if (app_db.device_state != APP_DEVICE_STATE_OFF)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_ble_common_adv_start_rws(app_cfg_const.timer_ota_adv_timeout * 100);
                    app_audio_tone_type_play(TONE_OTA_OVER_BLE_START, false, true);
                }
            }
        }
        break;
#endif

#if F_APP_ANC_SUPPORT
    case MMI_ANC_ON_OFF:
        {
            if (!app_anc_open_condition_check())
            {
                //If ANC is not allowed to be triggered, disable MMI
                break;
            }

            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            if (app_anc_is_anc_on_state(*app_db.final_listening_state))
            {
                if (app_listening_is_allow_all_off_condition_check())
                {
                    app_listening_state_machine(EVENT_ANC_OFF, true, true);
                }
            }
            else
            {
                if (app_anc_is_anc_on_state(app_db.last_anc_on_state))
                {
                    app_listening_state_machine(ANC_STATE_TO_EVENT(app_db.last_anc_on_state), true, true);
                }
            }
        }
        break;
#endif

#if (F_APP_AIRPLANE_SUPPORT == 1)
    case MMI_AIRPLANE_MODE:
        {
            if (!app_airplane_mode_get())
            {
                // enter airplane mode
                app_airplane_mmi_handle();
            }
            else
            {
                // exit airplane mode need all link disconnected
                if (app_airplane_all_link_idle())
                {
                    app_airplane_mmi_handle();
                }
            }
        }
        break;
#endif

#if F_APP_HEARABLE_SUPPORT
    case MMI_HA_PROG_CYCLE:
        {
            if (app_cfg_const.enable_ha)
            {
                if (app_apt_is_normal_apt_started())
                {
                    app_ha_switch_hearable_prog();
#if F_APP_HAS_SUPPORT
                    has_update_active_preset_idx(app_ha_hearing_get_active_prog_id() + 1);
#endif
                }
            }
        }
        break;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
    case MMI_LISTENING_MODE_CYCLE:
        {
#if F_APP_DURIAN_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                return;
            }
            app_durian_sync_long_press();
#else
            if (app_cfg_const.disallow_trigger_listening_mode_again_time)
            {
                if (app_db.key_action_disallow_too_close == MMI_LISTENING_MODE_CYCLE)
                {
                    break;
                }
                else
                {
                    app_listening_set_disallow_trigger_again();
                    app_db.key_action_disallow_too_close = MMI_LISTENING_MODE_CYCLE;
                }
            }

            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            app_listening_state_machine(EVENT_LISTENING_MODE_CYCLE, true, true);
#endif
        }
        break;

    case MMI_ANC_CYCLE:
        {
            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            app_listening_state_machine(EVENT_ANC_CYCLE, true, true);
        }
        break;

    case MMI_LLAPT_CYCLE:
        {
            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            app_listening_state_machine(EVENT_LLAPT_CYCLE, true, true);
        }
        break;

    case MMI_DEFAULT_LISTENING_MODE_CYCLE:
        {
            if (app_cfg_const.disallow_trigger_listening_mode_again_time)
            {
                if (app_db.key_action_disallow_too_close == MMI_LISTENING_MODE_CYCLE)
                {
                    break;
                }
                else
                {
                    app_listening_set_disallow_trigger_again();
                    app_db.key_action_disallow_too_close = MMI_LISTENING_MODE_CYCLE;
                }
            }

            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            app_listening_state_machine(EVENT_DEFAULT_LISTENING_MODE_CYCLE, true, true);
        }
        break;

#if F_APP_SUPPORT_ANC_APT_COEXIST
    case MMI_ANC_APT_ON_OFF:
        {
            if ((!app_listening_anc_apt_open_condition_check()) ||
                (app_apt_get_apt_support_type() != APT_SUPPORT_TYPE_NORMAL_APT))
            {
                //If APT or ANC is not allowed to be triggered, disable MMI
                break;
            }

            if (app_listening_get_is_special_event_ongoing())
            {
                //MMI triggered, change final to current
                *app_db.final_listening_state = app_db.current_listening_state;
            }

            if (app_listening_is_anc_apt_on_state(*app_db.final_listening_state))
            {
                if (app_listening_is_allow_all_off_condition_check())
                {
                    app_listening_state_machine(EVENT_ANC_APT_OFF, true, true);
                }
            }
            else
            {
                if (app_listening_is_anc_apt_on_state(app_db.last_anc_apt_on_state))
                {
                    app_listening_state_machine(ANC_APT_STATE_TO_EVENT(app_db.last_anc_apt_on_state), true, true);
                }
            }
        }
        break;

    case MMI_SWITCH_APT_TYPE:
        {
            if (app_cfg_const.normal_apt_support && app_cfg_const.llapt_support)
            {
                if (app_apt_is_apt_on_state(*app_db.final_listening_state))
                {
                    if ((app_bt_policy_get_call_status() != APP_CALL_IDLE) &&
                        (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT))
                    {
                        break;
                    }

                    app_cfg_nv.apt_support_type ^= 1;
                    app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);

                    if (LIGHT_SENSOR_ENABLED &&
                        !app_cfg_const.listening_mode_does_not_depend_on_in_ear_status)
                    {
                        app_apt_setting_state_update();
                    }

                    app_listening_state_machine(EVENT_APT_OFF, false, false);

                    if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
                    {
                        app_listening_state_machine(EVENT_NORMAL_APT_ON, false, true);
                    }
                    else if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT)
                    {
                        app_listening_state_machine(LLAPT_STATE_TO_EVENT(app_db.last_llapt_on_state), false, true);
                    }

                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_apt_report(EVENT_ASSIGN_APT_TYPE, NULL, 0);
                    }
                }
            }
        }
        break;
#endif
#endif

#if (F_APP_SENSOR_SUPPORT == 1)
    case MMI_LIGHT_SENSOR_ON_OFF:
        {
            uint8_t new_state = app_cfg_nv.light_sensor_enable ? 0 : 1;
            app_sensor_control(new_state, true, false);

            app_report_event_broadcast(EVENT_EAR_DETECTION_STATUS, &new_state, 1);

#if F_APP_LISTENING_MODE_SUPPORT
            if (!app_cfg_nv.light_sensor_enable)
            {
                app_listening_special_event_bitmap_clear(APP_LISTENING_EVENT_OUT_EAR);

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
                {
                    app_listening_anc_apt_cfg_pointer_change(&app_cfg_nv.anc_both_setting);
                }
                else
                {
                    app_listening_anc_apt_cfg_pointer_change(&app_cfg_nv.anc_one_setting);
                }
                app_listening_state_machine(EVENT_APPLY_FINAL_STATE, false, false);
            }
#endif
        }
        break;
#endif

#if 0
    case MMI_START_ROLESWAP:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_MMI_TRIGGER_ROLESWAP);
            }
        }
        break;
#endif

#if F_APP_ERWS_SUPPORT
    case MMI_ENTER_SINGLE_MODE:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                break;
            }
            //le_scan_stop();
            //le_adv_stop();//fixme later

            for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
            {
                if (app_link_check_b2b_link_by_id(app_idx))
                {
                    app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, RDTP_PROFILE_MASK);
                    break;
                }
            }
            app_cfg_nv.bud_role = REMOTE_SESSION_ROLE_SINGLE;

            // remote_session_role_set(REMOTE_SESSION_ROLE_SINGLE);
        }
        break;
#endif

    case MMI_A2DP_FOCUS_TOGGLE:
        {
            if (app_cfg_const.enable_multi_link)
            {
                app_cfg_const.disable_multilink_preemptive = app_cfg_const.disable_multilink_preemptive ^ 1;
                APP_PRINT_TRACE1("MMI_A2DP_FOCUS_TOGGLE: %d", app_cfg_const.disable_multilink_preemptive);
            }
        }
        break;

#if F_APP_NFC_SUPPORT
    case MMI_DEV_NFC_DETECT:
        {
            if (app_db.device_state == APP_DEVICE_STATE_OFF)
            {
                app_dlps_enable_auto_poweroff_stop_wdg_timer();
                app_mmi_handle_action(MMI_DEV_POWER_ON);
            }
            else
            {
                if (app_cfg_const.enable_nfc_multi_link_switch)
                {
                    app_nfc_multi_link_switch_trigger();
                    app_bt_policy_enter_pairing_mode(false, false);
                }
            }
        }
        break;
#endif

    case MMI_ENTER_DUT_FROM_SPP:
        {
            app_device_enter_dut_mode();
        }
        break;

    case MMI_DUT_TEST_MODE:
        {
            if ((app_db.force_enter_dut_mode_when_acl_connected) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
            {
                app_bt_policy_enter_dut_test_mode();

                /* WROKAROUND: need to stop le ext scan before entering dut test mode otherwise it would test fail. */
                le_ext_scan_stop();
                ble_ext_adv_mgr_disable_all(APP_STOP_ADV_CAUSE_DUT_TEST);

                app_cfg_nv.is_dut_test_mode = 1;
                app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

                app_led_change_mode(LED_MODE_DUT_TEST_MODE, true, false);

                mp_hci_test_auto_enter_dut_init();
                mp_hci_test_set_mode(true);

                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_DUT_TEST_MODE);
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_DUT_MODE);

#if F_APP_DUT_MODE_AUTO_POWER_OFF
                if (app_cfg_const.enable_5_mins_auto_power_off_under_dut_test_mode)
                {
                    dut_test_start_auto_power_off_timer();
                }
#endif

                //Enable Device Under Test mode
                gap_vendor_cmd_req(0x1803, 0, NULL);
            }
            else if ((app_bt_policy_get_radio_mode() == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE) &&
                     (app_bt_policy_get_state() != BP_STATE_LINKBACK) &&
                     (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED) &&
                     (app_link_get_b2s_link_num() == 0)
                     && (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED) ||
                     (app_cfg_nv.trigger_dut_mode_from_power_off) ||
                     (app_db.ad2b_enter_dut_mode))
            {
                switch_into_single_tone_test_mode();
            }
        }
        break;

#if F_APP_APT_SUPPORT
    case MMI_AUDIO_APT_EQ_SWITCH:
        {
            if (app_apt_is_apt_on_state(app_db.current_listening_state)
#if F_APP_SUPPORT_ANC_APT_COEXIST
                || app_listening_is_anc_apt_on_state(app_db.current_listening_state)
#endif
               )
            {
                uint8_t eq_num;

                eq_num = eq_utils_num_get(MIC_SW_EQ, APT_MODE);
                if (eq_num != 0)
                {
                    T_APT_EQ_DATA_UPDATE_EVENT event = EQ_INDEX_REPORT_BY_SWITCH_APT_EQ_INDEX;
                    app_cfg_nv.apt_eq_idx++;

                    if (app_cfg_nv.apt_eq_idx >= eq_num)
                    {
                        app_cfg_nv.apt_eq_idx = 0;
                    }

                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_eq_play_apt_eq_tone();

#if (F_APP_USER_EQ_SUPPORT == 1)
                        if (app_eq_is_valid_user_eq_index(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx))
                        {
                            event = EQ_INDEX_REPORT_BY_UPDATE_APT_EQ_INDEX;
                        }
#endif
                        app_report_apt_eq_idx(event);
                    }

                    //not set default eq when audio connect,enable when set eq para from phone
                    if (!app_db.eq_ctrl_by_src || event == EQ_INDEX_REPORT_BY_UPDATE_APT_EQ_INDEX)
                    {
                        app_eq_index_set(MIC_SW_EQ, APT_MODE, app_cfg_nv.apt_eq_idx);
                    }
                }
                else
                {
                    APP_PRINT_ERROR0("app_mmi_handle_action: apt eq need to enable");
                }
            }
        }
        break;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
    case MMI_AUDIO_ANC_APT_ALL_OFF:
        {
#if F_APP_DURIAN_SUPPORT
            app_durian_anc_mode_switch(ANC_MODE_ALL_OFF);
#else

#if F_APP_ANC_SUPPORT
            if (app_anc_is_anc_on_state(*app_db.final_listening_state))
            {
                app_listening_state_machine(EVENT_ALL_OFF, true, true);
            }
            else
#endif
            {
#if F_APP_APT_SUPPORT
                if (app_apt_is_apt_on_state(*app_db.final_listening_state))
                {
                    app_listening_state_machine(EVENT_ALL_OFF, true, true);
                }
#endif
            }
#endif
        }
        break;
#endif

#if 0
    case MMI_TAKE_PICTURE:
        {
            uint8_t keyboard_vol_up[5] = {0x02, 0, 0, 0x80, 0};
            uint8_t keyboard_release[5] = {0x02, 0, 0, 0, 0};

            bt_hid_interrupt_data_send(app_db.br_link[active_a2dp_idx].bd_addr, BT_HID_REPORT_TYPE_INPUT,
                                       keyboard_vol_up, sizeof(keyboard_vol_up));
            bt_hid_interrupt_data_send(app_db.br_link[active_a2dp_idx].bd_addr, BT_HID_REPORT_TYPE_INPUT,
                                       keyboard_release, sizeof(keyboard_release));
        }
        break;

    case MMI_AUDIO_SWITCH_CHANNEL:
        {
            uint8_t channel = 0;

            /*L<->R; R/L->(L+R)/2; (L+R)/2 -> L/R*/
            if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
            {
                if (app_cfg_nv.spk_channel == CHANNEL_L)
                {
                    channel = CHANNEL_R;
                }
                else if (app_cfg_nv.spk_channel == CHANNEL_R)
                {
                    channel = CHANNEL_LR_HELF;
                }
                else if (app_cfg_nv.spk_channel == CHANNEL_LR_HELF)
                {
                    channel = CHANNEL_L;
                }
            }
            else if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
            {
                if (app_cfg_nv.spk_channel == CHANNEL_R)
                {
                    channel = CHANNEL_L;
                }
                else if (app_cfg_nv.spk_channel == CHANNEL_L)
                {
                    channel = CHANNEL_LR_HELF;
                }
                else if (app_cfg_nv.spk_channel == CHANNEL_LR_HELF)
                {
                    channel = CHANNEL_R;
                }
            }

            app_audio_speaker_channel_set(AUDIO_SPECIFIC_SET, channel);
        }
        break;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#if F_APP_ANC_SUPPORT
    case MMI_AUDIO_ANC_ON:
        {
#if F_APP_DURIAN_SUPPORT
            app_durian_anc_mode_switch(ANC_MODE_ANC_ON);
#else
            if (!app_anc_is_anc_on_state(*app_db.final_listening_state))
            {
                app_listening_state_machine(EVENT_ANC_ON_SCENARIO_1, true, true);
            }
#endif
        }
        break;
#endif

#if F_APP_APT_SUPPORT
    case MMI_AUDIO_APT_ON:
        {
#if F_APP_DURIAN_SUPPORT
            app_durian_anc_mode_switch(ANC_MODE_APT_ON);
#else
            if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
            {
                app_listening_state_machine(EVENT_NORMAL_APT_ON, true, true);
            }
            else if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT)
            {
                app_listening_state_machine(EVENT_LLAPT_ON_SCENARIO_1, true, true);
            }
#endif
        }
        break;
#endif
#endif

#if 0
    case MMI_OUTPUT_INDICATION1_TOGGLE:
        {
            app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION1_TOGGLE);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                app_audio_tone_type_play(TONE_OUTPUT_INDICATION_1, false, true);
            }
        }
        break;

    case MMI_OUTPUT_INDICATION2_TOGGLE:
        {
            app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION2_TOGGLE);
        }
        break;

    case MMI_OUTPUT_INDICATION3_TOGGLE:
        {
            app_mmi_execute_output_indication_action(MMI_OUTPUT_INDICATION3_TOGGLE);
        }
        break;
#endif

#if F_APP_VAD_SUPPORT
    case MMI_AUDIO_VAD_OPEN:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                vad_enable();
                app_cfg_const.enable_vad = 1;
            }
        }
        break;

    case MMI_AUDIO_VAD_CLOSE:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                vad_disable();
                app_cfg_const.enable_vad = 0;
            }
        }
        break;
#endif
#if (F_APP_DUAL_AUDIO_EFFECT == 1)
    case MMI_AUDIO_DUAL_EFFECT_SWITCH:
        {
            uint8_t cycle_num = app_dual_audio_get_cycle_num();
            bool need_switch = false;

            if (cycle_num > 1)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    need_switch = true;
                }
                else if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                         (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    need_switch = true;
                }
            }

            if (need_switch)
            {
                app_dual_audio_effect_switch();
            }
        }
        break;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
    case MMI_AUDIO_DUAL_EFFECT_SPACE_SWITCH:
        {
            if ((remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED &&
                 (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY ||
                  remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)) ||
                app_cfg_const.mems_support == 0)
            {
                break;
            }

            app_cfg_nv.dual_audio_space = (app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_OPEN) ?
                                          SPATIAL_AUDIO_CLOSE : SPATIAL_AUDIO_OPEN;

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }

            if (app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_CLOSE)
            {
                app_sensor_mems_stop_sensor();
            }
            else
            {
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO
                if (app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_SNIFFING)
#else
                if (app_db.br_link[active_a2dp_idx].streaming_fg)
#endif
                {
                    app_sensor_mems_start(active_a2dp_idx);
                }
            }
        }
        break;
#endif
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
    case MMI_MS_TEAMS_BUTTON_SHORT_PRESS:
        {
            uint8_t *bd_addr = NULL;
            uint8_t ret = 0;

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                bd_addr = asp_device_api_get_active_link_addr();
                if (bd_addr)
                {
                    ret = app_asp_device_send_teams_button_invoked_msg(bd_addr, ASP_DEVICE_SOURCE_ID_SHORT_PRESS);
                }
                else
                {
#if F_APP_TEAMS_BT_POLICY
                    bd_addr = app_teams_bt_policy_get_dongle_cod_info();
#endif
                    if (bd_addr && app_link_find_br_link(bd_addr))
                    {
                        ret = app_asp_device_send_teams_button_invoked_msg(bd_addr, ASP_DEVICE_SOURCE_ID_SHORT_PRESS);
                    }
                }
                APP_PRINT_INFO2("app_mmi_send_teams_button: bd_addr %s, ret %d, short press", TRACE_BDADDR(bd_addr),
                                ret);
                teams_extend_led_tone_control_when_press_teams_button();
            }
        }
        break;
    case MMI_MS_TEAMS_BUTTON_LONG_PRESS:
        {
            uint8_t *bd_addr = NULL;
            uint8_t ret = 0;

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                bd_addr = asp_device_api_get_active_link_addr();
                if (bd_addr)
                {
                    ret = app_asp_device_send_teams_button_invoked_msg(bd_addr, ASP_DEVICE_SOURCE_ID_LONG_PRESS);
                }
                else
                {
#if F_APP_TEAMS_BT_POLICY
                    bd_addr = app_teams_bt_policy_get_dongle_cod_info();
#endif
                    if (bd_addr && app_link_find_br_link(bd_addr))
                    {
                        ret = app_asp_device_send_teams_button_invoked_msg(bd_addr, ASP_DEVICE_SOURCE_ID_LONG_PRESS);
                    }
                }
                APP_PRINT_INFO2("app_mmi_send_teams_button: bd_addr %s, ret %d, long press", TRACE_BDADDR(bd_addr),
                                ret);
                teams_extend_led_tone_control_when_press_teams_button();
            }
        }
        break;
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
    case MMI_FINDMY_PUT_SERIAL_NUMBER_STATE:
        {
            if (m_fmna_state == FMNA_SM_SEPARATED)
            {
                if (!serial_number_read_state)
                {
                    serial_number_read_state = true;
                    app_start_timer(&timer_idx_put_sn_state, "put_sn_state", app_mmi_timer_id,
                                    APP_TIMER_PUT_SN_STATE, 0, false, 300 * 1000);
                }
                else
                {
                    app_stop_timer(&timer_idx_put_sn_state);
                    serial_number_read_state = false;
                }
            }
        }
#endif

#if AMA_FEATURE_SUPPORT
    case MMI_ALEXA_WAKEWORD:
        {
            app_ama_va_start();
        }
        break;
#endif

#if BISTO_FEATURE_SUPPORT
    case MMI_BISTO_PTT:
        app_bisto_key_btns_action(MMI_BISTO_PTT);
        break;

    case MMI_BISTO_FETCH:
        app_bisto_key_btns_action(MMI_BISTO_FETCH);
        break;
#endif


#if XM_XIAOAI_FEATURE_SUPPORT
    case MMI_XIAOAI_WAKEWORD:
        {
            APP_PRINT_INFO1("app_handle_mmi_message: MMI_XIAOAI_WAKEWORD bud_role %d", app_cfg_nv.bud_role);

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && extend_app_cfg_const.xiaoai_support)
            {
                app_xiaoai_device_va_start(app_db.br_link[active_a2dp_idx].bd_addr, true, false);
            }
        }
        break;
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    case MMI_XIAOWEI_START_RECORDING:
        {
            APP_PRINT_INFO1("app_handle_mmi_message: MMI_XIAOWEI_START_RECORDING bud_role %d",
                            app_cfg_nv.bud_role);
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && extend_app_cfg_const.xiaowei_support)
            {
                app_xiaowei_device_va_start(app_db.br_link[active_a2dp_idx].bd_addr, true, false);
            }
        }
        break;

    case MMI_XIAOWEI_STOP_RECORDING:
        {
            APP_PRINT_INFO1("app_handle_mmi_message: MMI_XIAOWEI_STOP_RECORDING bud_role %d",
                            app_cfg_nv.bud_role);

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && extend_app_cfg_const.xiaowei_support)
            {
                app_xiaowei_device_va_stop(app_db.br_link[active_a2dp_idx].bd_addr);
            }
        }
        break;
#endif

#if GFPS_FINDER_SUPPORT
    case MMI_GFPS_FINDER_STOP_RING:
        {
            if (extend_app_cfg_const.gfps_finder_support)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && extend_app_cfg_const.gfps_support)
                {
                    app_gfps_msg_handle_ring_event(GFPS_ALL_STOP);

                    uint8_t ring_state = GFPS_FINDER_RING_BUTTON_STOP;
                    app_gfps_finder_send_ring_rsp(ring_state);
                }
            }
        }
        break;

    case MMI_GFPS_FINDER_STOP_ADV:
        {
#if HARMAN_ADD_GFPS_FINDER_STOP_ADV_TONE
            app_audio_tone_type_play(TONE_HARMAN_GFPS_ADV_STOP, false, true);
#endif
            app_gfps_finder_adv_handle_mmi_action();
        }
        break;

    case MMI_DULT_ENTER_ID_READ_STATE:
        {
            if (extend_app_cfg_const.gfps_finder_support)
            {
                app_dult_id_read_state_enable();
            }
        }
        break;
#endif

#if GFPS_LE_DEVICE_SUPPORT
    case MMI_GFPS_LEA_ENTER_PAIR_MODE:
        {
            if (extend_app_cfg_const.gfps_le_device_support)
            {
                app_gfps_force_enter_pairing_mode(GFPS_KEY_FORCE_ENTER_PAIR_MODE);
                app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
                app_audio_tone_type_play(TONE_GFPS_PAIRING, false, false);
            }
        }
        break;

    case MMI_GFPS_LEA_EXIT_PAIR_MODE:
        {
            if (extend_app_cfg_const.gfps_le_device_support)
            {
                app_gfps_force_enter_pairing_mode(GFPS_EXIT_PAIR_MODE);
                app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
            }
        }
        break;
#endif

#if F_APP_BRIGHTNESS_SUPPORT
    case MMI_LLAPT_BRIGHTNESS_UP:
        {
            if (app_apt_brightness_llapt_scenario_support(app_db.current_listening_state))
            {
                if (app_cfg_nv.main_brightness_level < brightness_level_max)
                {
                    app_cfg_nv.main_brightness_level++;

                    app_apt_brightness_main_level_set(app_cfg_nv.main_brightness_level);
                    app_apt_brightness_relay_report_handle();
                }
            }
        }
        break;

    case MMI_LLAPT_BRIGHTNESS_DOWN:
        {
            if (app_apt_brightness_llapt_scenario_support(app_db.current_listening_state))
            {
                if (app_cfg_nv.main_brightness_level > brightness_level_min)
                {
                    app_cfg_nv.main_brightness_level--;

                    app_apt_brightness_main_level_set(app_cfg_nv.main_brightness_level);
                    app_apt_brightness_relay_report_handle();
                }
            }
        }
        break;
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
    case MMI_SD_PLAYBACK_SWITCH:
        {
            T_APP_BR_LINK *p_link = NULL;

            if (app_playback_trans_is_busy())
            {
                APP_PRINT_WARN0("app_playback_trans_is_busy switch playback mode fail !!!");
                return;
            }
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        p_link = &app_db.br_link[i];
                        break;
                    }
                }
            }
            else
            {
                if (app_link_check_b2s_link_by_id(active_a2dp_idx))
                {
                    p_link = &app_db.br_link[active_a2dp_idx];
                }
            }

            if (0 == app_db.sd_playback_switch)
            {
                if (p_link != NULL && p_link->a2dp_track_handle != NULL)
                {
                    T_AUDIO_TRACK_STATE state;
                    audio_track_state_get(p_link->a2dp_track_handle, &state);
                    if ((state == AUDIO_TRACK_STATE_STARTED) || \
                        (state == AUDIO_TRACK_STATE_PAUSED))
                    {
                        audio_track_stop(p_link->a2dp_track_handle);
                        app_db.wait_resume_a2dp_flag = true;
                    }
                }
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if ((app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)))
                    {
                        if ((app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                            && (app_db.br_link[i].streaming_fg == true))
                        {
                            bt_avrcp_pause(app_db.br_link[i].bd_addr);
                            app_db.br_link[i].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                        }
                    }
                }

                if (app_playback_mode_init() == 0)
                {
                    app_db.sd_playback_switch = 1;
                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_PLAYBACK_MODE);
                }
            }
            else
            {
                app_playback_mode_deinit();
                app_db.sd_playback_switch = 0;
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_PLAYBACK_MODE, app_cfg_const.timer_auto_power_off);
                if (app_db.wait_resume_a2dp_flag == true)
                {
                    app_db.wait_resume_a2dp_flag = false;
                    if (p_link != NULL && p_link->a2dp_track_handle != NULL)
                    {
                        audio_track_start(p_link->a2dp_track_handle);
                    }
                }
            }
            APP_PRINT_TRACE1("current sd_playback_switch:%d", app_db.sd_playback_switch);
        }
        break;

    case MMI_SD_PLAYPACK_PAUSE:
    case MMI_SD_PLAYBACK_FWD:
    case MMI_SD_PLAYBACK_BWD:
    case MMI_SD_PLAYBACK_FWD_PLAYLIST:
    case MMI_SD_PLAYBACK_BWD_PLAYLIST:
        if (app_db.sd_playback_switch == 1)
        {
            app_playback_mmi_handle_action(action);
        }
        else
        {
            APP_PRINT_ERROR1("sd_playback_switch %d is fail!", app_db.sd_playback_switch);
        }
        break;
#if F_APP_LOCAL_PLAYBACK_SUPPORT & TARGET_RTL8773CO
    case MMI_SAVE_WHILE_LISTENING_ON_OFF:
        {
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) || \
                (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED))
            {
                if (app_db.listen_save_mode == false)
                {
                    app_db.listen_save_mode = true;
                    app_listen_save_mode_enter();
                }
                else if (app_db.listen_save_mode == true)
                {
                    app_db.listen_save_mode = false;
                    app_listen_save_mode_quit();
                }
            }
        }
        break;
#endif
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    case MMI_DEV_DONGLE_DUAL_MODE_SWITCH:
        {
            app_dongle_dual_mode_switch(SWITCH_EVENT_SWITCH_MODE);
        }
        break;

#if F_APP_DONGLE_MULTI_PAIRING
    case MMI_DEV_DONGLE_SWITCH_PAIRING:
        {
            app_dongle_switch_pairing_mode();
        }
        break;
#endif
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    case MMI_24G_BT_AUDIO_SOURCE_SWITCH:
        {
            app_dongle_switch_allowed_source();
        }
        break;
#endif

    default:
        break;
    }
}

#if 0
bool app_mmi_is_local_execute(uint8_t action)
{
    bool ret = false;

    if (action == MMI_DEV_FACTORY_RESET ||
        (app_cfg_nv.rws_disallow_sync_apt_volume && action == MMI_AUDIO_APT_VOL_UP) ||
        (app_cfg_nv.rws_disallow_sync_apt_volume && action == MMI_AUDIO_APT_VOL_DOWN) ||
        (app_cfg_nv.rws_disallow_sync_llapt_brightness && action == MMI_LLAPT_BRIGHTNESS_UP) ||
        (app_cfg_nv.rws_disallow_sync_llapt_brightness && action == MMI_LLAPT_BRIGHTNESS_DOWN))
    {
        ret = true;
    }

    return ret;
}
#endif

#if F_APP_MUTILINK_TRIGGER_HIGH_PRIORITY
uint8_t app_mmi_get_highest_priority_index(uint32_t profile_mask)
{
    uint8_t app_idx = MAX_BR_LINK_NUM;
    uint8_t link_num = app_link_get_b2s_link_num_with_profile(profile_mask);
    uint8_t bd_addr[6];
    uint32_t bond_flag;
    uint8_t bond_num = bt_bond_num_get();
    for (uint8_t i = 1; i <= bond_num; i++)
    {
        if (bt_bond_addr_get(i, bd_addr) == false)
        {
            break;
        }
        bt_bond_flag_get(bd_addr, &bond_flag);
        for (uint8_t j = 0; j < MAX_BR_LINK_NUM; j++)
        {
            if ((memcmp(bd_addr, app_db.br_link[j].bd_addr, 6) == 0) &&
                (app_db.br_link[j].connected_profile & profile_mask))
            {
                app_idx = j;
                break;
            }
        }

        if (app_idx != MAX_BR_LINK_NUM)
        {
            break;
        }
    }
    return app_idx;
}
#endif
