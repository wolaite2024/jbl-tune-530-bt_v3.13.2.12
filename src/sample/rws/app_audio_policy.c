/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "os_mem.h"
#include "trace.h"
#include "sysm.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "bt_bond.h"
#include "bt_a2dp.h"
#include "audio.h"
#include "audio_volume.h"
#include "audio_probe.h"
#include "ringtone.h"
#include "voice_prompt.h"
#include "eq.h"
#include "eq_utils.h"
#include "app_timer.h"
#include "app_dsp_cfg.h"
#include "app_tts.h"
#include "app_audio_policy.h"
#include "app_main.h"
#include "app_report.h"
#if F_APP_VENDOR_CMD_SUPPORT
#include "app_cmd.h"
#endif
#include "app_multilink.h"
#include "app_cfg.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_avrcp.h"
#include "app_bond.h"
#include "app_led.h"
#include "app_relay.h"
#include "app_key_process.h"
#include "app_eq.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif
#include "app_auto_power_off.h"
#include "app_device.h"
#include "app_loc_mgr.h"
#include "app_ipc.h"
#include "app_sensor.h"
#include "app_test.h"
#include "app_mmi.h"
#include "app_ble_gap.h"
#include "app_bud_loc.h"
#include "app_audio_route.h"
#include "app_link_util.h"
#include "app_nrec.h"

#if F_APP_SIDETONE_SUPPORT
#include "app_sidetone.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_unicast_audio.h"
#endif

#if F_APP_APT_SUPPORT
#include "audio_passthrough.h"
#include "app_audio_passthrough.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#include "line_in.h"
#endif

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif

#if F_APP_QOL_MONITOR_SUPPORT
#include "app_qol.h"
#endif

#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif

#if F_APP_IPHONE_ABS_VOL_HANDLE
#include "app_iphone_abs_vol_handle.h"
#endif


#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_RWS_MULTI_SPK_SUPPORT
#include "msbc_sync.h"
#endif


#if F_APP_CCP_SUPPORT
#include "app_lea_ccp.h"
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_1
#include "app_spp_capture_dsp_data.h"
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_2
#include "app_data_capture.h"
#endif


#if F_APP_LEA_SUPPORT
#include "multitopology_ctrl.h"
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#include "app_harman_eq.h"
#endif

#if F_APP_LEA_SUPPORT
#include "multitopology_ctrl.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

typedef enum
{
    APP_TIMER_LONG_PRESS_REPEAT = 0x00,
    APP_TIMER_NOTIFICATION_VOL_ADJ_WAIT_SEC_RSP = 0x01,
    APP_TIMER_NOTIFICATION_VOL_GET_WAIT_SEC_RSP = 0x02,
    APP_TIMER_IN_EAR_RESTORE_A2DP = 0x03,
    APP_TIMER_WAITING_PAUSED_STATUS = 0x04,
    APP_TIMER_WAITING_PLAY_STATUS = 0x05,
} T_APP_AUDIO_POLICY_TIMER;

typedef enum
{
    VOL_REPEAT_MAX = 0x00,
    VOL_REPEAT_MIN = 0x01,
} T_VOL_REPEAT;

typedef enum
{
    DSP_DATA_START,
    DSP_DATA_SEND,
    DSP_DATA_FINISH,
} T_DSP_DATA_OPCODE;

typedef enum
{
    DSP_DATA_TYPE_HA_VENDOR,
} T_DSP_DATA_TYPE;

typedef struct
{
    bool sending_status;
    uint8_t type;
    uint8_t *dsp_data;
    uint16_t len;
    uint16_t offset;
} T_SEG_SEND_DB;

typedef enum
{
    DSP_TOOL_OPCODE_BRIGHTNESS = 0x0000,
    DSP_TOOL_OPCODE_HW_EQ      = 0x0001,
    DSP_TOOL_OPCODE_GAIN       = 0x0002,
    DSP_TOOL_OPCODE_SW_EQ      = 0x0003,
} T_CMD_DSP_TOOL_OPCODE;

#if F_APP_TTS_SUPPORT
static uint8_t tts_path;
#endif

static uint8_t is_mic_mute;
static uint8_t mic_switch_mode;
static bool is_max_min_vol_from_phone = false;
static bool enable_play_max_min_tone_when_adjust_vol_from_phone = false;
static uint8_t mp_dual_mic_setting;
static bool force_join = false;
static T_APP_BUD_STREAM_STATE bud_stream_state = BUD_STREAM_STATE_IDLE;
static T_SEG_SEND_DB seg_send_db;
static bool enable_play_mic_unmute_tone = false;
static bool enable_play_mic_mute_tone = false;

static uint8_t audio_policy_timer_id = 0;
static uint8_t timer_idx_long_press_repeat = 0;
static uint8_t timer_idx_in_ear_restore_a2dp = 0;
static uint8_t timer_idx_waiting_paused_status = 0;
static uint8_t timer_idx_waiting_play_status = 0;

static const uint8_t codec_low_latency_table[9][LOW_LATENCY_LEVEL_MAX] =
{
    /* LOW_LATENCY_LEVEL1: 0;   LOW_LATENCY_LEVEL2: 1;  LOW_LATENCY_LEVEL3: 2;  LOW_LATENCY_LEVEL4: 3;  LOW_LATENCY_LEVEL5: 4. */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_PCM: 0 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_CVSD: 1 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_MSBC: 2 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_SBC: 3 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_AAC: 4 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_OPUS: 5 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_FLAC: 6 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_MP3: 7 */
    {40,                        60,                     80,                     100,                    120},      /* AUDIO_FORMAT_TYPE_LC3: 8 */
};

//for CMD_AUDIO_DSP_CTRL_SEND
#define CFG_H2D_DAC_GAIN                0x0C
#define CFG_H2D_VOICE_ADC_POST_GAIN     0x5D
#define CFG_H2D_APT_DAC_GAIN            0x4C

#define APP_BT_AUDIO_A2DP_COUNT         7
#define MAX_MIN_VOL_REPEAT_INTERVAL     1500
#define A2DP_TRACK_PAUSE_INTERVAL       3000

#define SEG_SEND_HEADER_LEN             8
#define SEG_SEND_MAX_PAYLOAD_LEN        120
#define SEG_SEND_MAX_LEN                (SEG_SEND_HEADER_LEN + SEG_SEND_MAX_PAYLOAD_LEN)

#define WAITING_PLAY_STATUS_TIMEOUT     1000//ms
#define WAITING_PAUSED_STATUS_TIMEOUT   5000//ms

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
static uint8_t active_vol_cmd_path;
static uint8_t active_vol_app_idx;
static uint8_t vol_cmd_status_mask = 0;

#define VOL_CMD_STATUS_SUCCESS          0x00
#define VOL_CMD_STATUS_FAILED_L         0x01
#define VOL_CMD_STATUS_FAILED_R         0x02
#define INVALID_NOTIFICATION_VOL_LEVEL  0xFF

#define VOLUME_ADJ_WAIT_SEC_INTERVAL    3000

// vp/ringtone volume should be the same level, here we use vp volume
#define L_CH_NOTIFICATION_VOL         CHECK_IS_LCH ? app_cfg_nv.voice_prompt_volume_out_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_voice_prompt_volume_out_level : INVALID_NOTIFICATION_VOL_LEVEL
#define R_CH_NOTIFICATION_VOL         CHECK_IS_RCH ? app_cfg_nv.voice_prompt_volume_out_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_voice_prompt_volume_out_level : INVALID_NOTIFICATION_VOL_LEVEL
#define L_CH_NOTIFICATION_VOL_MIN     CHECK_IS_LCH ? app_db.local_notification_vol_min_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_notification_vol_min_level : INVALID_NOTIFICATION_VOL_LEVEL
#define R_CH_NOTIFICATION_VOL_MIN     CHECK_IS_RCH ? app_db.local_notification_vol_min_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_notification_vol_min_level : INVALID_NOTIFICATION_VOL_LEVEL
#define L_CH_NOTIFICATION_VOL_MAX     CHECK_IS_LCH ? app_db.local_notification_vol_max_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_notification_vol_max_level : INVALID_NOTIFICATION_VOL_LEVEL
#define R_CH_NOTIFICATION_VOL_MAX     CHECK_IS_RCH ? app_db.local_notification_vol_max_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)\
    ? app_db.remote_notification_vol_max_level : INVALID_NOTIFICATION_VOL_LEVEL

static uint8_t timer_idx_notification_vol_adj_wait_sec_rsp = 0;
static uint8_t timer_idx_notification_vol_get_wait_sec_rsp = 0;
#endif

#if F_APP_LEA_SUPPORT
T_MTC_RESULT app_audio_mtc_if_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf);
#endif

#if (F_APP_VENDOR_CMD_SUPPORT | F_APP_ERWS_SUPPORT)
static void app_audio_cmd_set_volume_report_status(uint8_t cmd_path, uint8_t app_idx)
{
    uint8_t temp_buff[3];
    T_AUDIO_STREAM_TYPE volume_type;

    if (app_hfp_get_call_status() != BT_HFP_CALL_IDLE)
    {
        volume_type = AUDIO_STREAM_TYPE_VOICE;
    }
    else
    {
        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
    }

    temp_buff[0] = GET_STATUS_VOLUME;
    temp_buff[1] = audio_volume_out_get(volume_type);
    temp_buff[2] = audio_volume_out_max_get(volume_type);

    app_report_event(cmd_path, EVENT_REPORT_STATUS, app_idx, temp_buff, sizeof(temp_buff));
}
#endif

static void app_audio_judge_resume_a2dp_param(void)
{
    if (app_db.down_count >= APP_BT_AUDIO_A2DP_COUNT && app_db.recover_param == true)
    {
        app_db.down_count = 0;
        app_db.recover_param = false;
        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ROLESWAP_DOWNSTREAM_CMPL);
    }
    else if (app_db.recover_param == true)
    {
        app_db.down_count++;
    }
}

static void app_audio_policy_vol_max_tone_play(void)
{
#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_play(TONE_HARMAN_VOL_MAX_MIN, false, true);
    }
    else
#endif
    {
        app_audio_tone_type_play(TONE_VOL_MAX, false, true);
    }
}

static void app_audio_policy_mic_mute_on_tone_play(void)
{
#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_play(TONE_HARMAN_MIC_MUTE_ON, false, true);
    }
    else
#endif
    {
        app_audio_tone_type_play(TONE_MIC_MUTE_ON, false, true);
    }
}

static void app_audio_policy_mic_mute_off_tone_play(void)
{
#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_play(TONE_HARMAN_MIC_MUTE_OFF, false, true);
    }
    else
#endif
    {
        app_audio_tone_type_play(TONE_MIC_MUTE_OFF, false, true);
    }
}

static void app_audio_handle_vol_change(T_AUDIO_VOL_CHANGE vol_status)
{
    if ((app_key_is_set_volume() == true) ||
        (enable_play_max_min_tone_when_adjust_vol_from_phone))
    {
        app_key_set_volume_status(false);
        enable_play_max_min_tone_when_adjust_vol_from_phone = false;

        if (vol_status == AUDIO_VOL_CHANGE_MAX)
        {
            app_led_change_mode(LED_MODE_VOL_MAX_MIN, true, true);

#if 0
            if (app_audio_is_circular_volume_up())
            {
                app_db.play_min_max_vol_tone = true;
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                     app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    && app_db.play_min_max_vol_tone)
                {
                    app_audio_policy_vol_max_tone_play();
                }
            }
            else
#endif
                if (app_cfg_const.only_play_min_max_vol_once)
                {
                    if (app_db.play_min_max_vol_tone == true)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                            app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            app_audio_policy_vol_max_tone_play();
                        }
                        /* only play volume Max/Min tone once when adjust volume via long press repeat */
                        app_db.play_min_max_vol_tone = false;
                    }
                }
                else
                {
                    app_db.play_min_max_vol_tone = true;
                    if (timer_idx_long_press_repeat == 0)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                            app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            app_audio_policy_vol_max_tone_play();
                        }
                        app_db.play_min_max_vol_tone = false;

                        app_start_timer(&timer_idx_long_press_repeat, "long_press_repeat",
                                        audio_policy_timer_id, APP_TIMER_LONG_PRESS_REPEAT, VOL_REPEAT_MAX, true,
                                        MAX_MIN_VOL_REPEAT_INTERVAL);
                    }
                }
        }
        else if (vol_status == AUDIO_VOL_CHANGE_MIN)
        {
            app_led_change_mode(LED_MODE_VOL_MAX_MIN, true, true);

            if (app_db.is_circular_vol_up == true)
            {
                app_db.is_circular_vol_up = false;
            }
            else if (app_cfg_const.only_play_min_max_vol_once)
            {
                if (app_db.play_min_max_vol_tone == true)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_audio_tone_type_play(TONE_VOL_MIN, false, true);
                    }
                    /* only play volume Max/Min tone once when adjust volume via long press repeat */
                    app_db.play_min_max_vol_tone = false;
                }
            }
            else
            {
                app_db.play_min_max_vol_tone = true;
                if (timer_idx_long_press_repeat == 0)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_audio_tone_type_play(TONE_VOL_MIN, false, true);
                    }
                    app_db.play_min_max_vol_tone = false;
                    app_start_timer(&timer_idx_long_press_repeat, "long_press_repeat",
                                    audio_policy_timer_id, APP_TIMER_LONG_PRESS_REPEAT, VOL_REPEAT_MIN, true,
                                    MAX_MIN_VOL_REPEAT_INTERVAL);
                }
            }
        }
        else if (vol_status == AUDIO_VOL_CHANGE_DOWN ||
                 vol_status == AUDIO_VOL_CHANGE_UP)
        {
            app_led_change_mode(LED_MODE_VOL_ADJUST, true, true);

            if (app_cfg_const.disallow_sync_play_vol_changed_tone_when_vol_adjust)
            {
                if (app_key_is_sync_play_vol_changed_tone_disallow())
                {
                    app_key_set_sync_play_vol_changed_tone_disallow(false);
                    app_audio_tone_type_play(TONE_VOL_CHANGED, false, false);
                }
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                    app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_audio_tone_type_play(TONE_VOL_CHANGED, false, true);
                }
            }
        }
    }
}

void app_audio_set_connected_tone_need(bool need)
{
    app_db.connected_tone_need = need;

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_CONNECTED_TONE_NEED);
    }
#endif
}

#if 0
bool app_audio_is_circular_volume_up(void)
{
    if ((app_cfg_const.enable_circular_vol_up == 1) ||
        ((app_cfg_const.rws_circular_vol_up_when_solo_mode) &&
         (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)))
    {
        return true;
    }
    return false;
}
#endif

T_APP_BUD_STREAM_STATE app_audio_get_bud_stream_state(void)
{
    return bud_stream_state;
}

void app_audio_set_bud_stream_state(T_APP_BUD_STREAM_STATE state)
{
    APP_PRINT_TRACE2("app_audio_set_bud_stream_state: state %d roleswap_status %d", state,
                     app_roleswap_ctrl_get_status());

    if (bud_stream_state != state &&
        /* ignore bud stream state change due to roleswap profile disc */
        app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE)
    {
        bud_stream_state = state;

#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED
            && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_BUD_STREAM_STATE);
        }
#endif
    }
}


#if 0
bool app_audio_call_transfer_check(void)
{
    bool ret = false;
    bool sco_connected = app_hfp_sco_is_connected();
    bool sco_is_need = app_hfp_sco_is_need();
    T_BUD_LOCATION_EVENT last_bud_loc_event = (T_BUD_LOCATION_EVENT)app_db.last_bud_loc_event;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY && sco_is_need)
    {
        if ((app_cfg_const.enable_rtk_charging_box == true) && (app_cfg_const.sensor_support == true))
        {
            if (sco_connected)
            {
                if (last_bud_loc_event == EVENT_OUT_EAR &&
                    app_cfg_const.disallow_auto_transfer_to_phone_when_out_of_ear == false)
                {
                    if (app_db.local_loc != BUD_LOC_IN_EAR && app_db.remote_loc != BUD_LOC_IN_EAR)
                    {
                        ret = true;
                    }
                }
                else if (last_bud_loc_event == EVENT_IN_CASE)
                {
                    if (app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE)
                    {
                        ret = true;
                    }
                }
            }
            else
            {
                if (last_bud_loc_event == EVENT_IN_EAR || last_bud_loc_event == EVENT_OUT_CASE)
                {
                    ret = true;
                }
            }
        }
        else if ((app_cfg_const.enable_rtk_charging_box == true) && (app_cfg_const.sensor_support == false))
        {
            if (sco_connected)
            {
                if (last_bud_loc_event == EVENT_IN_CASE)
                {
                    if (app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE)
                    {
                        ret = true;
                    }
                }
            }
            else
            {
                if (last_bud_loc_event  == EVENT_OUT_CASE)
                {
                    ret = true;
                }
            }
        }
        else if ((app_cfg_const.enable_rtk_charging_box == false) && (app_cfg_const.sensor_support == true))
        {
            if (sco_connected)
            {
                if (last_bud_loc_event == EVENT_OUT_EAR &&
                    app_cfg_const.disallow_auto_transfer_to_phone_when_out_of_ear == false)
                {
                    if (app_db.local_loc != BUD_LOC_IN_EAR && app_db.remote_loc != BUD_LOC_IN_EAR)
                    {
                        ret = true;
                    }
                }
            }
            else
            {
                if (last_bud_loc_event  == EVENT_IN_EAR)
                {
                    ret = true;
                }
            }
        }
    }


    APP_PRINT_TRACE1("app_audio_call_transfer_check %d", ret);

    return ret;
}
#endif

void app_audio_set_avrcp_status(uint8_t status)
{
    APP_PRINT_TRACE4("app_audio_set_avrcp_status: bud_role %d avrcp_play_status old %d new %d a2dp_play_status %d",
                     app_cfg_nv.bud_role, app_db.avrcp_play_status, status, app_db.a2dp_play_status);

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (status == BT_AVRCP_PLAY_STATUS_PLAYING)
        {
            app_audio_a2dp_play_status_update(APP_A2DP_STREAM_AVRCP_PLAY);

        }
        else
        {
            app_audio_a2dp_play_status_update(APP_A2DP_STREAM_AVRCP_PAUSE);
        }
    }

    app_db.avrcp_play_status = status;

#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }
#endif
}

void app_audio_get_latency_value_by_level(T_AUDIO_STREAM_MODE mode, T_AUDIO_FORMAT_TYPE format_type,
                                          uint8_t latency_level, uint16_t *latency)
{
    if (mode == AUDIO_STREAM_MODE_LOW_LATENCY)
    {
        *latency = codec_low_latency_table[format_type][latency_level];;
    }
    else if (mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
    {
        *latency = (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE ? ULTRA_LOW_LATENCY_MS :
                    TWS_ULTRA_LOW_LATENCY_MS);
    }
    else
    {
        if (format_type == AUDIO_FORMAT_TYPE_LDAC)
        {
            *latency = 130;
        }
        else
        {
            *latency = app_cfg_nv.audio_latency;
        }
    }
}

void app_audio_get_last_used_latency(uint16_t *latency)
{
    if (app_db.last_gaming_mode_audio_track_latency == 0)
    {
        uint16_t latency_value;

        // choose AUDIO_FORMAT_TYPE_AAC as default format type
        app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, AUDIO_FORMAT_TYPE_AAC,
                                             app_cfg_nv.rws_low_latency_level_record, &latency_value);
        app_db.last_gaming_mode_audio_track_latency = latency_value;
    }

    *latency = app_db.last_gaming_mode_audio_track_latency;
}

#if 0
uint16_t app_audio_set_latency(T_AUDIO_TRACK_HANDLE p_handle, uint8_t latency_level,
                               bool latency_fixed)
{
    uint16_t latency;
    T_AUDIO_TRACK_HANDLE *p_a2dp_handle = (T_AUDIO_TRACK_HANDLE *)p_handle;
    T_AUDIO_FORMAT_INFO format_info;

    audio_track_format_info_get(p_a2dp_handle, &format_info);
    app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, format_info.type, latency_level,
                                         &latency);
    audio_track_latency_set(p_a2dp_handle, latency, GAMING_MODE_DYNAMIC_LATENCY_FIX);

    return latency;
}

uint16_t app_audio_update_audio_track_latency(T_AUDIO_TRACK_HANDLE p_handle, uint8_t latency_level)
{
    uint16_t latency_value;
    T_AUDIO_FORMAT_INFO format;
    T_AUDIO_TRACK_HANDLE *p_a2dp_handle = (T_AUDIO_TRACK_HANDLE *)p_handle;

    app_audio_get_last_used_latency(&latency_value);

    if (app_cfg_nv.rws_low_latency_level_record != latency_level) // need to update
    {
        if (audio_track_format_info_get(p_a2dp_handle, &format))
        {
            latency_value = app_audio_set_latency(p_a2dp_handle, latency_level,
                                                  GAMING_MODE_DYNAMIC_LATENCY_FIX);
        }
        else // a2dp handle is not active
        {
            // choose AUDIO_FORMAT_TYPE_AAC as default format type
            app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, AUDIO_FORMAT_TYPE_AAC,
                                                 latency_level, &latency_value);
        }

        app_db.last_gaming_mode_audio_track_latency = latency_value;
        app_cfg_nv.rws_low_latency_level_record = latency_level;
        app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_LOW_LATENCY_LEVEL);
        }
    }
    else
    {
        APP_PRINT_TRACE1("app_audio_update_audio_track_latency: latency level %d, no need to update",
                         app_cfg_nv.rws_low_latency_level_record);
    }

    return latency_value;
}

void app_audio_report_low_latency_status(uint16_t latency_value)
{
    uint8_t buf[5];
    buf[0] = app_db.gaming_mode;
    buf[1] = (uint8_t)(latency_value);
    buf[2] = (uint8_t)(latency_value >> 8);
    buf[3] = LOW_LATENCY_LEVEL_MAX;
    buf[4] = app_cfg_nv.rws_low_latency_level_record;

    app_report_event_broadcast(EVENT_LOW_LATENCY_STATUS, buf, sizeof(buf));
}

void app_audio_update_latency_record(uint16_t latency_value)
{
    app_db.last_gaming_mode_audio_track_latency = latency_value;

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        app_audio_report_low_latency_status(latency_value);
    }
}
#endif

#if F_APP_TTS_SUPPORT
static void app_audio_tts_report(uint8_t id, uint8_t request)
{
    uint8_t buf = request;
    app_report_event(tts_path, EVENT_TTS, id, &buf, 1);
}

static void app_audio_tts_stop(T_TTS_INFO *tts_info)
{
    tts_info->tts_handle = NULL;
    tts_info->tts_frame_len = 0;
    tts_info->tts_data_offset = 0;
    tts_info->tts_state = TTS_SESSION_CLOSE;
    tts_info->tts_seq = 0 /*TTS_INIT_SEQ*/;
    if (tts_info->tts_frame_buf != NULL)
    {
        free(tts_info->tts_frame_buf);
        tts_info->tts_frame_buf = NULL;
    }
}

static void app_audio_tts_report_handler(uint16_t event_type, T_AUDIO_EVENT_PARAM *param)
{
    uint8_t      report_data;
    T_TTS_HANDLE tts_handle;

    if (event_type == AUDIO_EVENT_TTS_STARTED)
    {
        report_data = TTS_SESSION_SEND_ENCODE_DATA;
        tts_handle = param->tts_started.handle;
    }
    else if (event_type == AUDIO_EVENT_TTS_PAUSED)
    {
        report_data = TTS_SESSION_PAUSE_REQ;
        tts_handle = param->tts_paused.handle;
    }
    else if (event_type == AUDIO_EVENT_TTS_RESUMED)
    {
        report_data = TTS_SESSION_RESUME_REQ;
        tts_handle = param->tts_resumed.handle;
    }
    else // invalid event
    {
        return;
    }

    if ((tts_path == CMD_PATH_SPP) || (tts_path == CMD_PATH_IAP))
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link_by_tts_handle(tts_handle);

        if (p_link != NULL)
        {
            app_audio_tts_report(p_link->id, report_data);
        }
    }
    else if (tts_path == CMD_PATH_LE)
    {
        T_APP_LE_LINK *p_link = app_link_find_le_link_by_tts_handle(tts_handle);

        if (p_link != NULL)
        {
            app_audio_tts_report(p_link->id, report_data);
        }
    }
}

void app_audio_set_tts_path(uint8_t path)
{
    tts_path = path;
}
#endif

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
static bool app_audio_notification_check_vol_is_valid(uint8_t vol)
{
    if ((vol < voice_prompt_volume_min_get()) ||
        (vol > voice_prompt_volume_max_get()) ||
        (vol < ringtone_volume_min_get()) ||
        (vol > ringtone_volume_max_get()))
    {
        return false;
    }
    return true;
}

void app_audio_notification_vol_relay(uint8_t current_volume, bool first_sync, bool report_flag)
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        return;
    }

    APP_PRINT_TRACE5("app_audio_notification_vol_relay: local vol %d min %d max %d sync %d report %d",
                     current_volume,
                     app_db.local_notification_vol_min_level, app_db.local_notification_vol_max_level,
                     app_cfg_nv.rws_sync_notification_vol, report_flag);

    T_NOTIFICATION_VOL_RELAY_MSG send_msg;

    send_msg.notification_vol_cur_level = current_volume;
    send_msg.notification_vol_min_level = app_db.local_notification_vol_min_level;
    send_msg.notification_vol_max_level = app_db.local_notification_vol_max_level;
    send_msg.first_sync                 = first_sync;
    send_msg.need_to_report             = report_flag;
    send_msg.need_to_sync               = app_cfg_nv.rws_sync_notification_vol;

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                        APP_REMOTE_MSG_NOTIFICATION_VOL_SYNC,
                                        (uint8_t *)&send_msg, sizeof(T_NOTIFICATION_VOL_RELAY_MSG));
}

void app_audio_notification_vol_set_remote_data(T_NOTIFICATION_VOL_RELAY_MSG *rcv_msg)
{
    app_db.remote_voice_prompt_volume_out_level = rcv_msg->notification_vol_cur_level;
    app_db.remote_ringtone_volume_out_level = rcv_msg->notification_vol_cur_level;
    app_db.remote_notification_vol_min_level = rcv_msg->notification_vol_min_level;
    app_db.remote_notification_vol_max_level = rcv_msg->notification_vol_max_level;

    APP_PRINT_TRACE6("app_audio_notification_vol_set_remote_data: local vol %d min %d max %d remote vol %d min %d max %d",
                     app_cfg_nv.voice_prompt_volume_out_level,
                     app_db.local_notification_vol_min_level,
                     app_db.local_notification_vol_max_level,
                     app_db.remote_voice_prompt_volume_out_level,
                     app_db.remote_notification_vol_min_level,
                     app_db.remote_notification_vol_max_level);
}

static void app_audio_notification_vol_report_info(void)
{
    uint8_t event_data[7];

    // here we use vp volume level
    LE_UINT8_TO_ARRAY(&event_data[0], L_CH_NOTIFICATION_VOL_MIN);
    LE_UINT8_TO_ARRAY(&event_data[1], L_CH_NOTIFICATION_VOL_MAX);
    LE_UINT8_TO_ARRAY(&event_data[2], L_CH_NOTIFICATION_VOL);
    LE_UINT8_TO_ARRAY(&event_data[3], R_CH_NOTIFICATION_VOL_MIN);
    LE_UINT8_TO_ARRAY(&event_data[4], R_CH_NOTIFICATION_VOL_MAX);
    LE_UINT8_TO_ARRAY(&event_data[5], R_CH_NOTIFICATION_VOL);
    LE_UINT8_TO_ARRAY(&event_data[6], app_cfg_nv.rws_sync_notification_vol);

    app_report_event_broadcast(EVENT_NOTIFICATION_VOL_STATUS, event_data, sizeof(event_data));
}

static void app_audio_notification_vol_report_status(uint8_t report_status, uint8_t cmd_path,
                                                     uint8_t app_idx)
{
    uint8_t report_data[8];

    LE_UINT8_TO_ARRAY(&report_data[0], report_status);
    LE_UINT8_TO_ARRAY(&report_data[1], L_CH_NOTIFICATION_VOL_MIN);
    LE_UINT8_TO_ARRAY(&report_data[2], L_CH_NOTIFICATION_VOL_MAX);
    LE_UINT8_TO_ARRAY(&report_data[3], L_CH_NOTIFICATION_VOL);
    LE_UINT8_TO_ARRAY(&report_data[4], R_CH_NOTIFICATION_VOL_MIN);
    LE_UINT8_TO_ARRAY(&report_data[5], R_CH_NOTIFICATION_VOL_MAX);
    LE_UINT8_TO_ARRAY(&report_data[6], R_CH_NOTIFICATION_VOL);
    LE_UINT8_TO_ARRAY(&report_data[7], app_cfg_nv.rws_sync_notification_vol);

    app_report_event(cmd_path, EVENT_NOTIFICATION_VOL_LEVEL_SET, app_idx, &report_data[0],
                     sizeof(report_data));
}

static void app_audio_notification_vol_update_local(uint8_t local_volume)
{
    if ((app_cfg_nv.voice_prompt_volume_out_level != local_volume) ||
        (app_cfg_nv.ringtone_volume_out_level != local_volume))
    {
        APP_PRINT_TRACE1("app_audio_notification_vol_update_local: local_volume %d", local_volume);

        // vp and tone volume should be the same level
        app_cfg_nv.ringtone_volume_out_level = local_volume;
        app_cfg_nv.voice_prompt_volume_out_level = local_volume;
        ringtone_volume_set(local_volume);
        voice_prompt_volume_set(local_volume);
        app_cfg_store(&app_cfg_nv.offset_notification_vol, 4);
    }
}

void app_audio_notification_vol_cmd_handle(uint16_t volume_cmd, uint8_t *param_ptr,
                                           uint16_t param_len, uint8_t path, uint8_t app_idx)
{
    APP_PRINT_TRACE4("app_audio_notification_vol_cmd_handle: volume_cmd 0x%04X, param_len %d, param_ptr 0x%02X 0x%02X",
                     volume_cmd, param_len, param_ptr[0], param_ptr[1]);

    uint8_t vol_set_failed = CHECK_IS_LCH ? VOL_CMD_STATUS_FAILED_L : VOL_CMD_STATUS_FAILED_R;

    switch (volume_cmd)
    {
    case CMD_SET_NOTIFICATION_VOL_LEVEL:
        {
            uint8_t l_volume = param_ptr[0];
            uint8_t r_volume = param_ptr[1];
            uint8_t local_volume;
            uint8_t valid_fg;
            uint8_t report_status;

            local_volume = CHECK_IS_LCH ? l_volume : r_volume;
            valid_fg = app_audio_notification_check_vol_is_valid(local_volume);

            if (param_len > 2) // new flow, including rws_sync_notification_vol state
            {
                uint8_t rws_sync = param_ptr[2];

                APP_PRINT_TRACE2("app_audio_notification_vol_cmd_handle: sync 0x%02X (current state %d)",
                                 param_ptr[2], app_cfg_nv.rws_sync_notification_vol);

                if ((rws_sync != INVALID_VALUE) && (app_cfg_nv.rws_sync_notification_vol != rws_sync))
                {
                    app_cfg_nv.rws_sync_notification_vol = rws_sync;
                    app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 4);
                }
            }

            if (valid_fg)
            {
                // update volume
                app_audio_notification_vol_update_local(local_volume);
                app_audio_notification_vol_relay(local_volume, false, false);
            }

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                // RWS primary
                active_vol_cmd_path = path;
                active_vol_app_idx = app_idx;
                vol_cmd_status_mask = valid_fg ? VOL_CMD_STATUS_SUCCESS : vol_set_failed;

                if (app_cmd_relay_command_set(volume_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_AUDIO_POLICY,
                                              APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_CMD, false))
                {
                    app_start_timer(&timer_idx_notification_vol_adj_wait_sec_rsp, "notification_vol_adj_wait_sec_rsp",
                                    audio_policy_timer_id, APP_TIMER_NOTIFICATION_VOL_ADJ_WAIT_SEC_RSP, 0, false,
                                    VOLUME_ADJ_WAIT_SEC_INTERVAL);
                }
                else
                {
                    // failed to relay command to spk2
                    report_status = CHECK_IS_LCH ? VOL_CMD_STATUS_FAILED_R : VOL_CMD_STATUS_FAILED_L;
                    app_audio_notification_vol_report_status(report_status, path, app_idx);
                    vol_cmd_status_mask = 0;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                // RWS secondary
                report_status = valid_fg ? VOL_CMD_STATUS_SUCCESS : vol_set_failed;
                app_cmd_relay_event(EVENT_NOTIFICATION_VOL_LEVEL_SET, &report_status, sizeof(report_status),
                                    APP_MODULE_TYPE_AUDIO_POLICY,
                                    APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_EVENT);
            }
            else
            {
                // single primary
                vol_cmd_status_mask = valid_fg ? VOL_CMD_STATUS_SUCCESS : vol_set_failed;
                app_audio_notification_vol_report_status(vol_cmd_status_mask, path, app_idx);
                vol_cmd_status_mask = 0;
            }
        }
        break;

    case CMD_GET_NOTIFICATION_VOL_LEVEL:
        {
            // vp/ringtone volume should be the same level
            if (app_cfg_nv.ringtone_volume_out_level != app_cfg_nv.voice_prompt_volume_out_level)
            {
                app_cfg_nv.ringtone_volume_out_level = app_cfg_nv.voice_prompt_volume_out_level;
                app_cfg_store(&app_cfg_nv.offset_notification_vol, 4);
            }

            if (!app_cfg_nv.is_notification_vol_set_from_phone)
            {
                app_cfg_nv.is_notification_vol_set_from_phone = 1;
                app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);
            }

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                // RWS primary
                T_NOTIFICATION_VOL_RELAY_MSG *send_msg = malloc(sizeof(T_NOTIFICATION_VOL_RELAY_MSG));

                if (send_msg == NULL)
                {
                    app_audio_notification_vol_report_info();
                    break;
                }

                send_msg->notification_vol_cur_level = app_cfg_nv.voice_prompt_volume_out_level;
                send_msg->notification_vol_min_level = app_db.local_notification_vol_min_level;
                send_msg->notification_vol_max_level = app_db.local_notification_vol_max_level;
                send_msg->first_sync = false;
                send_msg->need_to_report = false;
                send_msg->need_to_sync = false;

                if (app_cmd_relay_command_set(volume_cmd, (uint8_t *)send_msg, sizeof(T_NOTIFICATION_VOL_RELAY_MSG),
                                              APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_CMD, false))
                {
                    app_start_timer(&timer_idx_notification_vol_get_wait_sec_rsp, "notification_vol_get_wait_sec_rsp",
                                    audio_policy_timer_id, APP_TIMER_NOTIFICATION_VOL_GET_WAIT_SEC_RSP, 0, false,
                                    VOLUME_ADJ_WAIT_SEC_INTERVAL);
                }
                else
                {
                    // failed to relay command to spk2
                    app_audio_notification_vol_report_info();
                }

                free(send_msg);
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                // RWS secondary
                T_NOTIFICATION_VOL_RELAY_MSG *rcv_msg = (T_NOTIFICATION_VOL_RELAY_MSG *) &param_ptr[0];

                app_audio_notification_vol_set_remote_data(rcv_msg);

                T_NOTIFICATION_VOL_RELAY_MSG *send_msg = malloc(sizeof(T_NOTIFICATION_VOL_RELAY_MSG));

                if (send_msg == NULL)
                {
                    break;
                }

                send_msg->notification_vol_cur_level = app_cfg_nv.voice_prompt_volume_out_level;
                send_msg->notification_vol_min_level = app_db.local_notification_vol_min_level;
                send_msg->notification_vol_max_level = app_db.local_notification_vol_max_level;

                app_cmd_relay_event(EVENT_NOTIFICATION_VOL_STATUS, (uint8_t *)send_msg,
                                    sizeof(T_NOTIFICATION_VOL_RELAY_MSG),
                                    APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_EVENT);
                free(send_msg);
            }
            else
            {
                // single primary
                app_audio_notification_vol_report_info();
            }
        }
        break;

    default:
        break;
    }
}
#endif

static void app_audio_policy_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_APP_BR_LINK *p_link;
            T_AUDIO_STREAM_TYPE stream_type;

#if F_APP_LEA_SUPPORT
            if (mtc_get_btmode() != MULTI_PRO_BT_BREDR)
            {
                break;
            }
#endif

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                break;
            }

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                app_harman_set_stream_track_started(param->track_state_changed.state);
            }
            else
            {
                app_harman_set_stream_track_started(AUDIO_TRACK_STATE_RELEASED);
            }
#endif

            switch (param->track_state_changed.state)
            {
            case AUDIO_TRACK_STATE_STARTED:
            case AUDIO_TRACK_STATE_RESTARTED:
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                        {
#if F_APP_HARMAN_FEATURE_SUPPORT
                            app_harman_set_eq_boost_gain();
#endif
                            p_link = &app_db.br_link[active_a2dp_idx];

#if F_APP_SAIYAN_MODE
                            app_data_capture_send_gain();
#endif

#if F_APP_IPHONE_ABS_VOL_HANDLE
                            if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                                (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                            {
                                uint8_t abs_vol;
                                uint8_t pair_idx_mapping;

                                APP_PRINT_TRACE2("app_audio_policy_cback: %s (%d)",
                                                 TRACE_BDADDR(p_link->bd_addr), p_link->id);

                                if (app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping) == false)
                                {
                                    return;
                                }

                                //It's need to update abs vol & gain lv when audio track started.
                                abs_vol = app_cfg_nv.abs_vol[pair_idx_mapping];

                                app_cfg_nv.audio_gain_level[pair_idx_mapping] = app_iphone_abs_vol_lv_handle(abs_vol);

                                app_audio_vol_set(param->track_state_changed.handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
                                app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                            }
#endif
#if F_APP_MUTILINK_VA_PREEMPTIVE
                            if (app_cfg_const.enable_multi_link && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                                app_bond_set_priority(p_link->bd_addr);
#endif
                            }
#endif
                        }
#if F_APP_QOL_MONITOR_SUPPORT
                        else if (param->track_state_changed.state == AUDIO_TRACK_STATE_RESTARTED)
                        {
                            int8_t rssi = 0;

                            app_qol_get_aggregate_rssi(false, &rssi);

                            if (param->track_state_changed.cause == AUDIO_TRACK_CAUSE_BUFFER_EMPTY ||
                                param->track_state_changed.cause == AUDIO_TRACK_CAUSE_JOIN_PACKET_LOST)
                            {
                                APP_PRINT_TRACE1("app_audio_policy_cback: rssi %d", rssi);
                                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                                {
                                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                                    {
                                        app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_DSP_DECODE_EMPTY);

                                        if ((rssi < RSSI_MINIMUM_THRESHOLD_TO_RECEIVE_PACKET_BAD) && (!app_db.sec_going_away))
                                        {
                                            app_db.sec_going_away = true;
                                            app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_SEC_GOING_AWAY);
                                        }
                                    }
                                    else
                                    {
                                        app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_PRI_DECODE_EMPTY);
                                    }
                                }
                            }
                        }
#endif
                    }

                    if (stream_type == AUDIO_STREAM_TYPE_VOICE)
                    {
                        p_link = &app_db.br_link[active_hf_idx];

                        if (p_link != NULL)
                        {
                            if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                            {
                                if (app_dsp_cfg_sidetone.hw_enable)
                                {
                                    audio_volume_in_unmute(stream_type);
                                }

                                p_link->nrec_instance = app_nrec_attach(p_link->sco_track_handle, true);
                                p_link->sidetone_instance = app_sidetone_attach(p_link->sco_track_handle, app_dsp_cfg_sidetone);
                            }

#if F_APP_MUTILINK_VA_PREEMPTIVE
                            if (app_cfg_const.enable_multi_link && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                                app_bond_set_priority(p_link->bd_addr);
#endif
                            }
#endif
                            p_link->duplicate_fst_sco_data = true;
#if F_APP_ERWS_SUPPORT
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                            {

#if F_APP_RWS_MULTI_SPK_SUPPORT

#else
#if F_APP_LEA_SUPPORT
                                if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
#endif
                                {
                                    audio_volume_in_mute(AUDIO_STREAM_TYPE_VOICE);
                                }
#endif
                            }
#endif
                        }
                    }
                }
                break;

            case AUDIO_TRACK_STATE_STOPPED:
            case AUDIO_TRACK_STATE_PAUSED:
                {
                    if (stream_type == AUDIO_STREAM_TYPE_VOICE)
                    {
                        p_link = &app_db.br_link[active_hf_idx];

                        if (p_link != NULL)
                        {
                            app_nrec_detach(p_link->sco_track_handle, p_link->nrec_instance);
                            app_sidetone_detach(p_link->sco_track_handle, p_link->sidetone_instance);
                        }

                        p_link = &app_db.br_link[active_a2dp_idx];

                        if (p_link->streaming_fg == true)
                        {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
                            uint8_t active_idx = app_multilink_customer_get_active_music_link_index();

                            app_multilink_customer_music_event_handle(active_idx, JUDGE_EVENT_A2DP_START);
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
                }
                break;
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {
#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
            if ((app_spp_capture_data_state() & SPP_CAPTURE_DATA_ENTER_SCO_MODE_MASK) == 0)
#elif (F_APP_SPP_CAPTURE_DSP_DATA_2 == 1)
            if (((app_data_capture_get_state() & DATA_CAPTURE_DATA_START_SCO_MODE) == 0) &&
                ((app_data_capture_get_state() & DATA_CAPTURE_DATA_SAIYAN_EXECUTING) == 0))
#endif
            {
// #if F_APP_RWS_MULTI_SPK_SUPPORT == 0
//                 if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
// #endif
                {
                    T_APP_BR_LINK *p_link;

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    if (app_teams_multilink_get_voice_status())
                    {
                        active_hf_idx = app_teams_multilink_get_active_voice_index();
                    }
                    else
                    {
                        active_hf_idx = app_teams_multilink_get_active_record_index();
                    }
#endif
                    p_link = app_link_find_br_link(app_db.br_link[active_hf_idx].bd_addr);
                    if (p_link == NULL)
                    {
                        break;
                    }

                    if (param->track_data_ind.handle != p_link->sco_track_handle)
                    {
                        break;
                    }

                    // APP_PRINT_TRACE1("app_audio_policy_cback: data ind len %u", param->track_data_ind.len);
                    uint32_t timestamp;
                    uint16_t seq_num;
                    uint8_t frame_num;
                    T_AUDIO_STREAM_STATUS status;
                    uint16_t read_len;
                    uint8_t *buf;

                    buf = malloc(param->track_data_ind.len);

                    if (buf == NULL)
                    {
                        return;
                    }

                    if (audio_track_read(param->track_data_ind.handle,
                                         &timestamp,
                                         &seq_num,
                                         &status,
                                         &frame_num,
                                         buf,
                                         param->track_data_ind.len,
                                         &read_len) == true)
                    {
#if F_APP_RWS_MULTI_SPK_SUPPORT
                        // if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                        // {
                        //     APP_PRINT_TRACE1("app_audio_policy_cback: buf %b", TRACE_BINARY(57, buf));
                        // }
                        // APP_PRINT_TRACE1("app_audio_policy_cback: seq %d", seq_num);
                        msbc_data_proc((MSBC_FRAME *)buf, seq_num);
#else
                        if (p_link->duplicate_fst_sco_data)
                        {
                            p_link->duplicate_fst_sco_data = false;
                            bt_sco_data_send(p_link->bd_addr, seq_num - 1, buf, read_len);
                        }
                        bt_sco_data_send(p_link->bd_addr, seq_num, buf, read_len);
#endif
                    }


                    free(buf);
                }
            }
        }
        break;

    case AUDIO_EVENT_VOLUME_IN_MUTED:
        if ((param->volume_in_muted.type == AUDIO_STREAM_TYPE_VOICE)
#if F_APP_LEA_SUPPORT
            || ((param->volume_in_muted.type == AUDIO_STREAM_TYPE_RECORD) &&
                (app_db.local_loc != BUD_LOC_IN_CASE))
#endif
#if F_APP_GAMING_DONGLE_SUPPORT
            || ((param->volume_in_muted.type == AUDIO_STREAM_TYPE_RECORD) &&
                (app_db.dongle_is_enable_mic) && (app_db.local_loc != BUD_LOC_IN_CASE))
#endif
           )
        {
#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
            {
                if (app_audio_is_enable_play_mic_mute_tone())
                {
                    app_audio_policy_mic_mute_on_tone_play();
                }
                app_audio_enable_play_mic_mute_tone(false);
            }
#endif
        }
        break;

    case AUDIO_EVENT_VOLUME_IN_UNMUTED:
        if ((param->volume_in_muted.type == AUDIO_STREAM_TYPE_VOICE)
#if F_APP_GAMING_DONGLE_SUPPORT || F_APP_LEA_SUPPORT
            || (param->volume_in_muted.type == AUDIO_STREAM_TYPE_RECORD)
#endif
           )
        {
#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
            if (app_audio_is_enable_play_mic_unmute_tone())
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                    (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    app_audio_policy_mic_mute_off_tone_play();
                }
                app_audio_enable_play_mic_unmute_tone(false);
            }
#endif
        }
        break;

    case AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;
            uint8_t             pre_volume;
            uint8_t             cur_volume;
            T_AUDIO_VOL_CHANGE  vol_status;
            bool                is_phone_real_level_0;

            if (audio_track_stream_type_get(param->track_volume_out_changed.handle, &stream_type) == false)
            {
                break;
            }

            pre_volume = param->track_volume_out_changed.prev_volume;
            cur_volume = param->track_volume_out_changed.curr_volume;
            vol_status = AUDIO_VOL_CHANGE_NONE;
            is_phone_real_level_0 = false;

            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
#if F_APP_LEA_SUPPORT
                if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
#endif
                {
                    app_avrcp_set_vol_is_changed();
                }

                if (app_cfg_const.enable_rtk_charging_box)
                {
                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                        (app_db.local_loc == BUD_LOC_IN_CASE) && (app_db.remote_loc != BUD_LOC_IN_CASE))
                    {
                        if ((app_key_is_set_volume() == true) ||
                            (enable_play_max_min_tone_when_adjust_vol_from_phone))
                        {
                            uint8_t pair_idx_mapping;

                            if (app_bond_get_pair_idx_mapping(app_db.br_link[active_a2dp_idx].bd_addr,
                                                              &pair_idx_mapping) == true)
                            {
                                cur_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
                            }
                        }
                    }
                }

                APP_PRINT_TRACE2("[SD_CHECK]:AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED %d,%d", pre_volume, cur_volume);
#if F_APP_HARMAN_FEATURE_SUPPORT
                if ((cur_volume == app_dsp_cfg_vol.playback_volume_max) &&
                    (pre_volume == app_dsp_cfg_vol.playback_volume_max))
#else
                if (cur_volume == app_dsp_cfg_vol.playback_volume_max)
#endif
                {
                    vol_status = AUDIO_VOL_CHANGE_MAX;
                }
                else if (cur_volume == app_dsp_cfg_vol.playback_volume_min)
                {
#if F_APP_IPHONE_ABS_VOL_HANDLE
                    T_APP_BR_LINK *p_link;

                    p_link = &app_db.br_link[active_a2dp_idx];
                    if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
                        (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
                    {
                        uint8_t pair_idx_mapping;

                        app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);

                        //Real level 0.
                        if (app_cfg_nv.abs_vol[pair_idx_mapping] == 0)
                        {
                            vol_status = AUDIO_VOL_CHANGE_MIN;
                            is_phone_real_level_0 = true;
                        }
                        else
                        {
                            /* for case:
                              1. real lv 0 -> fake lv 0
                              2. lv1 -> fake lv 0
                            */
                            vol_status = AUDIO_VOL_CHANGE_DOWN;
                        }
                    }
                    else
#endif
                    {
                        vol_status = AUDIO_VOL_CHANGE_MIN;
                    }
                }
                else
                {
                    if (cur_volume > pre_volume)
                    {
                        vol_status = AUDIO_VOL_CHANGE_UP;
                    }
                    else if (cur_volume < pre_volume)
                    {
                        vol_status = AUDIO_VOL_CHANGE_DOWN;
                    }
                }

                //if enable play_max_min_tone_when_adjust_vol_from_phone, just handle first vol change to max or min operation
                if (is_max_min_vol_from_phone == true)
                {
                    enable_play_max_min_tone_when_adjust_vol_from_phone = false;

                    if (
#if F_APP_HARMAN_FEATURE_SUPPORT
                        ((app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                         (cur_volume == app_dsp_cfg_vol.playback_volume_max) &&
                         (cur_volume == pre_volume + 1))
#else
                        ((vol_status == AUDIO_VOL_CHANGE_MAX) && (cur_volume == pre_volume + 1))
#endif
                        || ((vol_status == AUDIO_VOL_CHANGE_MIN) && (cur_volume == pre_volume - 1))
                        || ((vol_status == AUDIO_VOL_CHANGE_MIN) && is_phone_real_level_0))
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        if (vol_status != AUDIO_VOL_CHANGE_MIN)
                        {
                            APP_PRINT_TRACE1("[SD_CHECK]:change_vol_from_phone++ check2 =%d", vol_status);
                            vol_status = AUDIO_VOL_CHANGE_MAX;
                        }
#endif
#if HARMAN_ADJUST_MAX_MIN_VOLUME_FROM_PHONE_SUPPORT
                        enable_play_max_min_tone_when_adjust_vol_from_phone = true;
#else
                        enable_play_max_min_tone_when_adjust_vol_from_phone = false;
#endif
                        is_max_min_vol_from_phone = false;
                    }
                }
            }
            else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                if (cur_volume == app_dsp_cfg_vol.voice_out_volume_max)
                {
                    vol_status = AUDIO_VOL_CHANGE_MAX;
                }
                else if (cur_volume == app_dsp_cfg_vol.voice_out_volume_min)
                {
                    vol_status = AUDIO_VOL_CHANGE_MIN;
                }
                else
                {
                    if (cur_volume > pre_volume)
                    {
                        vol_status = AUDIO_VOL_CHANGE_UP;
                    }
                    else if (cur_volume < pre_volume)
                    {
                        vol_status = AUDIO_VOL_CHANGE_DOWN;
                    }
                }
            }

            app_audio_handle_vol_change(vol_status);
        }
        break;

#if F_APP_TTS_SUPPORT
    case AUDIO_EVENT_TTS_ALERTED:
        {
        }
        break;

    case AUDIO_EVENT_TTS_STOPPED:
        {
            if ((tts_path == CMD_PATH_SPP) || (tts_path == CMD_PATH_IAP))
            {
                T_APP_BR_LINK *p_link = app_link_find_br_link_by_tts_handle(param->tts_stopped.handle);

                if (p_link != NULL)
                {
                    app_audio_tts_stop(&p_link->tts_info);
                }
            }
            else if (tts_path == CMD_PATH_LE)
            {
                T_APP_LE_LINK *p_link = app_link_find_le_link_by_tts_handle(param->tts_stopped.handle);

                if (p_link != NULL)
                {
                    app_audio_tts_stop(&p_link->tts_info);
                }
            }

            tts_destroy(param->tts_stopped.handle);
        }
        break;

    case AUDIO_EVENT_TTS_STARTED:
    case AUDIO_EVENT_TTS_PAUSED:
    case AUDIO_EVENT_TTS_RESUMED:
        {
            app_audio_tts_report_handler(event_type, param);
        }
        break;
#endif

    case AUDIO_EVENT_VOICE_PROMPT_STARTED:
        {
            app_db.tone_vp_status.state = APP_TONE_VP_STARTED;
#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_cpu_clk_improve();
#endif
            app_db.tone_vp_status.index = param->voice_prompt_started.index + VOICE_PROMPT_INDEX;

#if HARMAN_SUPPORT_WATER_EJECTION
            if ((param->voice_prompt_started.index + VOICE_PROMPT_INDEX) == app_cfg_const.tone_apt_eq_1)
            {
                voice_prompt_volume_set(voice_prompt_volume_max_get());
            }
#endif

            if (
#if F_APP_HARMAN_FEATURE_SUPPORT
                (((app_cfg_nv.language_status == 0) &&
                  (app_db.tone_vp_status.index == app_cfg_const.tone_harman_power_off))
                 || ((app_cfg_nv.language_status == 1) &&
                     (app_db.tone_vp_status.index == app_cfg_const.tone_power_off)))
#else
                (app_db.tone_vp_status.index == app_cfg_const.tone_power_off)
#endif
                && (app_db.device_state == APP_DEVICE_STATE_OFF_ING))
            {
                sys_mgr_power_off();
            }
        }
        break;

    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:
        {
            app_db.tone_vp_status.state = APP_TONE_VP_STOP;
#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_cpu_clk_improve();
#endif
            app_db.tone_vp_status.index = param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX;

#if F_APP_AIRPLANE_SUPPORT
            if (app_db.tone_vp_status.index == app_cfg_const.tone_enter_airplane)
            {
                app_airplane_enter_airplane_mode_handle();
            }
#endif

            app_db.tone_vp_status.index = TONE_INVALID_INDEX;

#if F_APP_HARMAN_FEATURE_SUPPORT
            //APP_PRINT_TRACE4("[SD_CHECK]:AUDIO_EVENT_VOICE_PROMPT_STOPPED %d,%d,%d,%d",
            //                 app_db.tone_vp_status.state,
            //                 (param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX),
            //                 app_cfg_const.tone_vol_max,
            //                 app_cfg_const.tone_vol_min);

            // only filter vol max & vol min tone/VP
            if ((param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX) == app_cfg_const.tone_vol_max)
            {
                app_audio_tone_type_cancel(TONE_VOL_MAX, false);
            }

            if ((param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX) ==
                app_cfg_const.tone_harman_vol_max)
            {
                app_audio_tone_type_cancel(TONE_HARMAN_VOL_MAX_MIN, false);
            }

            if ((param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX) == app_cfg_const.tone_vol_min)
            {
                app_audio_tone_type_cancel(TONE_VOL_MIN, false);
            }
#endif

#if HARMAN_SUPPORT_WATER_EJECTION
            if ((param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX) == app_cfg_const.tone_apt_eq_1)
            {
                voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_default);
            }
#endif
        }
        break;

    case AUDIO_EVENT_RINGTONE_STARTED:
        {
            app_db.tone_vp_status.state = APP_TONE_VP_STARTED;
#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_cpu_clk_improve();
#endif
            app_db.tone_vp_status.index = param->ringtone_started.index;

            if (
#if F_APP_HARMAN_FEATURE_SUPPORT
                (((app_cfg_nv.language_status == 0) &&
                  (app_db.tone_vp_status.index == app_cfg_const.tone_harman_power_off))
                 || ((app_cfg_nv.language_status == 1) &&
                     (app_db.tone_vp_status.index == app_cfg_const.tone_power_off)))
#else
                (app_db.tone_vp_status.index == app_cfg_const.tone_power_off)
#endif
                && (app_db.device_state == APP_DEVICE_STATE_OFF_ING))
            {
                sys_mgr_power_off();
            }
        }
        break;

    case AUDIO_EVENT_RINGTONE_STOPPED:
        {
            app_db.tone_vp_status.state = APP_TONE_VP_STOP;
#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_cpu_clk_improve();
#endif
            app_db.tone_vp_status.index = param->ringtone_stopped.index;

#if F_APP_AIRPLANE_SUPPORT
            if (app_db.tone_vp_status.index == app_cfg_const.tone_enter_airplane)
            {
                app_airplane_enter_airplane_mode_handle();
            }
#endif

            app_db.tone_vp_status.index = TONE_INVALID_INDEX;
        }
        break;

    case AUDIO_EVENT_BUFFER_STATE_PALYING:
        {
#if F_APP_QOL_MONITOR_SUPPORT
            T_APP_BR_LINK *p_link = NULL;
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

                if (p_link != NULL && p_link->a2dp_track_handle)
                {
                    app_qol_link_monitor(p_link->bd_addr, true);
                }
            }
#endif
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_audio_policy_cback: event_type 0x%04x", event_type);
    }
}

static bool app_audio_set_voice_gain_when_sco_conn_cmpl(void *sco_track_handle,
                                                        uint8_t voice_gain_level,
                                                        uint8_t voice_muted)
{
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

    if (sco_track_handle == NULL)
    {
        return false;
    }

    if (app_cfg_const.fixed_inband_tone_gain &&
        (call_status == APP_CALL_OUTGOING || call_status == APP_CALL_INCOMING))
    {
        voice_gain_level = app_cfg_const.inband_tone_gain_lv;
    }

    APP_PRINT_TRACE4("app_audio_set_voice_gain_when_sco_conn_cmpl: call_status %d level %d(%d) voice_muted %d",
                     call_status, voice_gain_level, app_cfg_const.inband_tone_gain_lv, voice_muted);

    if (voice_muted)
    {
        audio_track_volume_out_mute(sco_track_handle);
    }
    else
    {
        app_audio_vol_set(sco_track_handle, voice_gain_level);
    }

    if (app_audio_is_mic_mute())
    {
        app_mmi_handle_action(MMI_DEV_MIC_MUTE);
    }
    else
    {
        app_mmi_handle_action(MMI_DEV_MIC_UNMUTE);
    }

    return true;
}

static void app_audio_save_a2dp_config(uint8_t *a2dp_cfg, T_AUDIO_FORMAT_INFO *format_info)
{
    T_BT_EVENT_PARAM_A2DP_CONFIG_CMPL *cfg = (T_BT_EVENT_PARAM_A2DP_CONFIG_CMPL *)a2dp_cfg;
    T_APP_BR_LINK *p_link = app_link_find_br_link(cfg->bd_addr);
    if (p_link == NULL)
    {
        return;
    }

    p_link->a2dp_codec_type = cfg->codec_type;

    if (cfg->codec_type == BT_A2DP_CODEC_TYPE_SBC)
    {
        format_info->type = AUDIO_FORMAT_TYPE_SBC;
        switch (cfg->codec_info.sbc.sampling_frequency)
        {
        case BT_A2DP_SBC_SAMPLING_FREQUENCY_16KHZ:
            format_info->attr.sbc.sample_rate = 16000;
            break;
        case BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ:
            format_info->attr.sbc.sample_rate = 32000;
            break;
        case BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ:
            format_info->attr.sbc.sample_rate = 44100;
            break;
        case BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ:
            format_info->attr.sbc.sample_rate = 48000;
            break;
        }

        switch (cfg->codec_info.sbc.channel_mode)
        {
        case BT_A2DP_SBC_CHANNEL_MODE_MONO:
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
            break;
        case BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL:
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_DUAL;
            break;
        case BT_A2DP_SBC_CHANNEL_MODE_STEREO:
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_STEREO;
            break;
        case BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
            format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO;
            break;
        }
        switch (cfg->codec_info.sbc.block_length)
        {
        case BT_A2DP_SBC_BLOCK_LENGTH_4:
            format_info->attr.sbc.block_length = 4;
            break;
        case BT_A2DP_SBC_BLOCK_LENGTH_8:
            format_info->attr.sbc.block_length = 8;
            break;
        case BT_A2DP_SBC_BLOCK_LENGTH_12:
            format_info->attr.sbc.block_length = 12;
            break;
        case BT_A2DP_SBC_BLOCK_LENGTH_16:
            format_info->attr.sbc.block_length = 16;
            break;
        }
        switch (cfg->codec_info.sbc.subbands)
        {
        case BT_A2DP_SBC_SUBBANDS_4:
            format_info->attr.sbc.subband_num = 4;
            break;
        case BT_A2DP_SBC_SUBBANDS_8:
            format_info->attr.sbc.subband_num = 8;
            break;
        }
        switch (cfg->codec_info.sbc.allocation_method)
        {
        case BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS:
            format_info->attr.sbc.allocation_method = 0;
            break;
        case BT_A2DP_SBC_ALLOCATION_METHOD_SNR:
            format_info->attr.sbc.allocation_method = 1;
            break;
        }

        p_link->a2dp_codec_info.sbc.sampling_frequency = format_info->attr.sbc.sample_rate;
        p_link->a2dp_codec_info.sbc.channel_mode = format_info->attr.sbc.chann_mode;
        p_link->a2dp_codec_info.sbc.block_length = format_info->attr.sbc.block_length;
        p_link->a2dp_codec_info.sbc.subbands = format_info->attr.sbc.subband_num;
        p_link->a2dp_codec_info.sbc.allocation_method = format_info->attr.sbc.allocation_method;
    }
    else if (cfg->codec_type == BT_A2DP_CODEC_TYPE_AAC)
    {
        format_info->type = AUDIO_FORMAT_TYPE_AAC;
        switch (cfg->codec_info.aac.sampling_frequency)
        {
        case BT_A2DP_AAC_SAMPLING_FREQUENCY_8KHZ:
            format_info->attr.aac.sample_rate = 8000;
            break;
        case BT_A2DP_AAC_SAMPLING_FREQUENCY_16KHZ:
            format_info->attr.aac.sample_rate = 16000;
            break;

        case BT_A2DP_AAC_SAMPLING_FREQUENCY_44_1KHZ:
            format_info->attr.aac.sample_rate = 44100;
            break;

        case BT_A2DP_AAC_SAMPLING_FREQUENCY_48KHZ:
            format_info->attr.aac.sample_rate = 48000;
            break;

        case BT_A2DP_AAC_SAMPLING_FREQUENCY_96KHZ:
            format_info->attr.aac.sample_rate = 96000;
            break;
        default:
            break;
        }

        switch (cfg->codec_info.aac.channel_number)
        {
        case BT_A2DP_AAC_CHANNEL_NUMBER_1:
            format_info->attr.aac.chann_num = 1;
            break;

        case BT_A2DP_AAC_CHANNEL_NUMBER_2:
            format_info->attr.aac.chann_num = 2;
            break;

        default:
            break;
        }
        format_info->attr.aac.transport_format = AUDIO_AAC_TRANSPORT_FORMAT_LATM;

        p_link->a2dp_codec_info.aac.sampling_frequency = format_info->attr.aac.sample_rate;
        p_link->a2dp_codec_info.aac.channel_number = format_info->attr.aac.chann_num;
    }
#if (F_APP_A2DP_CODEC_LDAC_SUPPORT == 1)
    else if (cfg->codec_type == BT_A2DP_CODEC_TYPE_LDAC)
    {
        format_info->type = AUDIO_FORMAT_TYPE_LDAC;
        switch (cfg->codec_info.ldac.sampling_frequency)
        {
        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_44_1KHZ:
            format_info->attr.ldac.sample_rate = 44100;
            break;

        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_48KHZ:
            format_info->attr.ldac.sample_rate = 48000;
            break;

        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_88_2KHZ:
            format_info->attr.ldac.sample_rate = 88200;
            break;

        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_96KHZ:
            format_info->attr.ldac.sample_rate = 96000;
            break;

        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_176_4KHZ:
            format_info->attr.ldac.sample_rate = 176400;
            break;

        case BT_A2DP_LDAC_SAMPLING_FREQUENCY_192KHZ:
            format_info->attr.ldac.sample_rate = 192000;
            break;

        default:
            break;
        }

        switch (cfg->codec_info.ldac.channel_mode)
        {
        case BT_A2DP_LDAC_CHANNEL_MODE_MONO:
            format_info->attr.ldac.chann_mode = AUDIO_LDAC_CHANNEL_MODE_MONO;
            break;

        case BT_A2DP_LDAC_CHANNEL_MODE_DUAL:
            format_info->attr.ldac.chann_mode = AUDIO_LDAC_CHANNEL_MODE_DUAL;
            break;

        case BT_A2DP_LDAC_CHANNEL_MODE_STEREO:
            format_info->attr.ldac.chann_mode = AUDIO_LDAC_CHANNEL_MODE_STEREO;
            break;

        default:
            break;
        }

        p_link->a2dp_codec_info.ldac.sampling_frequency = format_info->attr.ldac.sample_rate;
        p_link->a2dp_codec_info.ldac.channel_mode = format_info->attr.ldac.chann_mode;
    }
#endif
#if (F_APP_A2DP_CODEC_LC3_SUPPORT == 1)
    else if (cfg->codec_type == BT_A2DP_CODEC_TYPE_LC3)
    {
        format_info->type = AUDIO_FORMAT_TYPE_LC3;
        switch (cfg->codec_info.lc3.sampling_frequency)
        {
        case BT_A2DP_LC3_SAMPLING_FREQUENCY_8KHZ:
            format_info->attr.lc3.sample_rate = 8000;
            break;

        case BT_A2DP_LC3_SAMPLING_FREQUENCY_16KHZ:
            format_info->attr.lc3.sample_rate = 16000;
            break;

        case BT_A2DP_LC3_SAMPLING_FREQUENCY_24KHZ:
            format_info->attr.lc3.sample_rate = 24000;
            break;

        case BT_A2DP_LC3_SAMPLING_FREQUENCY_32KHZ:
            format_info->attr.lc3.sample_rate = 32000;
            break;

        case BT_A2DP_LC3_SAMPLING_FREQUENCY_44_1KHZ:
            format_info->attr.lc3.sample_rate = 44100;
            break;

        case BT_A2DP_LC3_SAMPLING_FREQUENCY_48KHZ:
            format_info->attr.lc3.sample_rate = 48000;
            break;

        default:
            break;
        }

        switch (cfg->codec_info.lc3.channel_number)
        {
        case BT_A2DP_LC3_CHANNEL_NUM_1:
            format_info->attr.lc3.chann_location = AUDIO_CHANNEL_LOCATION_MONO;
            break;

        case BT_A2DP_LC3_CHANNEL_NUM_2:
            format_info->attr.lc3.chann_location = AUDIO_CHANNEL_LOCATION_FL |
                                                   AUDIO_CHANNEL_LOCATION_FR;
            break;

        default:
            break;
        }

        switch (cfg->codec_info.lc3.frame_duration)
        {
        case BT_A2DP_LC3_FRAME_DURATION_7_5MS:
            format_info->attr.lc3.frame_duration = AUDIO_LC3_FRAME_DURATION_7_5_MS;
            break;

        case BT_A2DP_LC3_FRAME_DURATION_10MS:
            format_info->attr.lc3.frame_duration = AUDIO_LC3_FRAME_DURATION_10_MS;
            break;

        default:
            break;
        }

        format_info->attr.lc3.frame_length = cfg->codec_info.lc3.frame_length;

        p_link->a2dp_codec_info.lc3.sampling_frequency = format_info->attr.lc3.sample_rate;
        p_link->a2dp_codec_info.lc3.chann_location = format_info->attr.lc3.chann_location;
        p_link->a2dp_codec_info.lc3.frame_duration = format_info->attr.lc3.frame_duration;
        p_link->a2dp_codec_info.lc3.frame_length = format_info->attr.lc3.frame_length;
    }
#endif
    else
    {
        return;
    }
}

static void app_audio_a2dp_track_release(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (p_link->a2dp_track_handle != NULL)
        {
            audio_track_release(p_link->a2dp_track_handle);
            p_link->a2dp_track_handle = NULL;
        }

        if (p_link->eq_instance != NULL)
        {
            eq_release(p_link->eq_instance);
            p_link->eq_instance = NULL;
        }
        p_link->playback_muted = 0;
    }
}

static void app_audio_sco_conn_cmpl_handle(uint8_t *bd_addr, uint8_t air_mode, uint8_t rx_pkt_len)
{
    uint8_t pair_idx_mapping;
    T_AUDIO_FORMAT_INFO format_info = {};
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);

    if (p_link == NULL)
    {
        return;
    }

    p_link->sco_seq_num = 0;

    app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping);

    if (air_mode == 3) /**< Air mode transparent data. */
    {
        format_info.type = AUDIO_FORMAT_TYPE_MSBC;
        format_info.attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
        format_info.attr.msbc.sample_rate = 16000;
        format_info.attr.msbc.bitpool = 26;
        format_info.attr.msbc.allocation_method = 0;
        format_info.attr.msbc.subband_num = 8;
        format_info.attr.msbc.block_length = 15;
    }
    else if (air_mode == 2) /**< Air mode CVSD. */
    {
        format_info.type = AUDIO_FORMAT_TYPE_CVSD;
        format_info.attr.cvsd.chann_num = AUDIO_SBC_CHANNEL_MODE_MONO;
        format_info.attr.cvsd.sample_rate = 8000;
        if (rx_pkt_len == 30)
        {
            format_info.attr.cvsd.frame_duration = AUDIO_CVSD_FRAME_DURATION_3_75_MS;
        }
        else
        {
            format_info.attr.cvsd.frame_duration = AUDIO_CVSD_FRAME_DURATION_7_5_MS;
        }
    }
    else
    {
        return;
    }

#if F_APP_LEA_SUPPORT
    bool need_return = false;
    if (mtc_if_fm_ap_handle(AP_TO_MTC_SCO_CMPL, bd_addr, &need_return) == MTC_RESULT_SUCCESS)
    {
        if (need_return)
        {
            return;
        }
    }
#endif

    if (p_link->sco_track_handle != NULL)
    {
        audio_track_release(p_link->sco_track_handle);
        p_link->sco_track_handle = NULL;
    }

#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (p_link->voice_spk_eq_instance != NULL)
    {
        eq_release(p_link->voice_spk_eq_instance);
        p_link->voice_spk_eq_instance = NULL;
    }

    if (p_link->voice_mic_eq_instance != NULL)
    {
        eq_release(p_link->voice_mic_eq_instance);
        p_link->voice_mic_eq_instance = NULL;
    }
#endif

    p_link->sco_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                  AUDIO_STREAM_MODE_NORMAL,
                                                  AUDIO_STREAM_USAGE_SNOOP,
                                                  format_info,
                                                  app_cfg_nv.voice_gain_level[pair_idx_mapping],
                                                  app_dsp_cfg_vol.voice_volume_in_default,
                                                  AUDIO_DEVICE_OUT_DEFAULT | AUDIO_DEVICE_IN_DEFAULT,
                                                  NULL,
                                                  NULL);
    if (p_link->sco_track_handle == NULL)
    {
        return;
    }

    if (app_link_get_b2s_link_num() > 1)
    {
        audio_track_latency_set(p_link->sco_track_handle, 150, false);
    }
    else
    {
#if F_APP_TEAMS_FEATURE_SUPPORT
        audio_track_latency_set(p_link->sco_track_handle, 60, false);
#else
        audio_track_latency_set(p_link->sco_track_handle, 100, false);
#endif
    }

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    if (p_link->is_mute)
    {
        audio_track_volume_in_mute(p_link->sco_track_handle);
        bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0);
    }
    else
    {
        T_APP_CALL_STATUS call_status = app_hfp_get_call_status();
        if (call_status == APP_VOICE_ACTIVATION_ONGOING)
        {
            bt_hfp_microphone_gain_level_report(p_link->bd_addr, 15);
        }
    }
#endif

    app_audio_set_voice_gain_when_sco_conn_cmpl(p_link->sco_track_handle,
                                                app_cfg_nv.voice_gain_level[pair_idx_mapping],
                                                p_link->voice_muted);
#if F_APP_VOICE_SPK_EQ_SUPPORT
    app_eq_change_audio_eq_mode(true);

    app_eq_idx_check_accord_mode();
    p_link->voice_spk_eq_instance = app_eq_create(EQ_CONTENT_TYPE_VOICE, EQ_STREAM_TYPE_VOICE,
                                                  SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx);

    if (p_link->voice_spk_eq_instance != NULL)
    {
        eq_enable(p_link->voice_spk_eq_instance);
        audio_track_effect_attach(p_link->sco_track_handle, p_link->voice_spk_eq_instance);
    }

    p_link->voice_mic_eq_instance = app_eq_create(EQ_CONTENT_TYPE_VOICE, EQ_STREAM_TYPE_VOICE,
                                                  MIC_SW_EQ, VOICE_MIC_MODE, app_cfg_nv.eq_idx);

    if (p_link->voice_mic_eq_instance != NULL)
    {
        eq_enable(p_link->voice_mic_eq_instance);
        audio_track_effect_attach(p_link->sco_track_handle, p_link->voice_mic_eq_instance);
    }
#endif

#if F_APP_ERWS_SUPPORT
    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
    {
        audio_track_start(p_link->sco_track_handle);
#if F_APP_MUTILINK_VA_PREEMPTIVE
        memcpy(app_db.active_hfp_addr, p_link->bd_addr, 6);
#endif
        p_link->streaming_fg = false;
    }
    else
#endif
    {
        p_link->nrec_instance = app_nrec_attach(p_link->sco_track_handle, true);
#if F_APP_MUTILINK_VA_PREEMPTIVE
        if (app_cfg_const.enable_multi_link)
        {
            uint8_t active_hf_idx = app_hfp_get_active_idx();
            APP_PRINT_TRACE3("app_audio_sco_conn_cmpl_handle: Cheat call %d %d %d",
                             app_hfp_get_call_status(),
                             active_hf_idx,
                             p_link->id);
            if (app_hfp_get_call_status() != APP_CALL_IDLE && (active_hf_idx != p_link->id))
            {
                return;
            }
        }
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        uint8_t teams_active_sco_idx = app_teams_multilink_get_active_voice_index();
        uint8_t call_status = p_link->call_status;
        uint8_t low_priority_app_active_index = 0;
        uint8_t record_active_index = 0;

        if (call_status)
        {
            if (teams_active_sco_idx == p_link->id)
            {
                app_teams_multilink_voice_track_start(teams_active_sco_idx);
            }
            else
            {
                APP_PRINT_TRACE2("not the active sco: active %d, current %d", teams_active_sco_idx, p_link->id);
            }

        }
        else
        {
            record_active_index = app_teams_multilink_get_active_record_index();
            low_priority_app_active_index = app_teams_multilink_get_active_low_priority_app_index();
            APP_PRINT_TRACE4("sco cmpl from record: active low app idx %d, active record index %d, link id %d, low start check %d",
                             low_priority_app_active_index, record_active_index, p_link->id,
                             app_teams_multilink_check_low_priority_app_start_condition());
            if (app_teams_multilink_check_low_priority_app_start_condition() &&
                (p_link->id == record_active_index) && (record_active_index == low_priority_app_active_index))
            {
                app_teams_multilink_record_track_start(p_link->id);
            }
        }
#else
        p_link = &app_db.br_link[app_hfp_get_active_idx()];

        //HFP status may not be update by source, but SCO is created
        if (app_link_get_sco_conn_num() == 1)
        {
            app_hfp_set_active_idx(bd_addr);
        }
        else if (memcmp(bd_addr, p_link->bd_addr, 6) == 0)
        {
            audio_track_start(p_link->sco_track_handle);
        }
        else
        {
            APP_PRINT_TRACE2("app_audio_sco_conn_cmpl_handle: active link %s, current link %s",
                             TRACE_BDADDR(p_link->bd_addr),
                             TRACE_BDADDR(bd_addr));
        }
#endif

        if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_VOICE);
        }
    }
}

static void app_audio_a2dp_stream_start_handle(uint8_t *bd_addr, uint8_t *audio_cfg)
{
    T_APP_BR_LINK *p_link;
    T_BT_EVENT_PARAM_A2DP_CONFIG_CMPL *cfg;
    uint8_t pair_idx_mapping;
    T_AUDIO_FORMAT_INFO format_info = {};
    T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_NORMAL;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint16_t latency_value = app_cfg_nv.cmd_latency_in_ms;
#else
    uint16_t latency_value = app_cfg_nv.audio_latency;
#endif
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    p_link = app_link_find_br_link(bd_addr);
    if (p_link == NULL)
    {
        return;
    }

    app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping);
    cfg = (T_BT_EVENT_PARAM_A2DP_CONFIG_CMPL *)audio_cfg;
    app_audio_save_a2dp_config((uint8_t *)cfg, &format_info);

    if (app_db.gaming_mode)
    {
#if F_APP_GAMING_DONGLE_SUPPORT
        if (app_link_check_dongle_link(bd_addr))
        {
            mode = AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY;
        }
        else
#endif
        {
            mode = AUDIO_STREAM_MODE_LOW_LATENCY;
        }

        app_audio_get_last_used_latency(&latency_value);
    }
    else
    {
        mode = AUDIO_STREAM_MODE_NORMAL;
        app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_NORMAL, format_info.type, 0, &latency_value);
    }

    if (p_link->a2dp_track_handle)
    {
        audio_track_release(p_link->a2dp_track_handle);
    }

    if (p_link->eq_instance != NULL)
    {
        eq_release(p_link->eq_instance);
        p_link->eq_instance = NULL;
    }

    {
        T_AUDIO_STREAM_USAGE stream;
#if F_APP_ERWS_SUPPORT
        if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
        {
            stream = AUDIO_STREAM_USAGE_SNOOP;
        }
        else
#endif
        {
#if F_APP_ERWS_SUPPORT
            if ((p_link->acl_link_role == BT_LINK_ROLE_MASTER) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
            {
                stream = AUDIO_STREAM_USAGE_LOCAL;
                app_db.src_roleswitch_fail_no_sniffing = true;
            }
            else
#endif
            {
                stream = AUDIO_STREAM_USAGE_SNOOP;
            }

        }

        p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                       mode,
                                                       stream,
                                                       format_info,
                                                       app_cfg_nv.audio_gain_level[pair_idx_mapping],
                                                       0,
                                                       AUDIO_DEVICE_OUT_DEFAULT,
                                                       NULL,
                                                       NULL);
    }

    if (p_link->a2dp_track_handle != NULL)
    {
#if 0
        if (app_db.gaming_mode)
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_link_check_dongle_link(p_link->bd_addr))
            {
                latency_value = app_dongle_get_gaming_latency();
                app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);

                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }
            else
#endif
            {
                latency_value = app_audio_set_latency(p_link->a2dp_track_handle,
                                                      app_cfg_nv.rws_low_latency_level_record,
                                                      GAMING_MODE_DYNAMIC_LATENCY_FIX);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }

            app_audio_update_latency_record(latency_value);
        }
        else
#endif
        {
#if F_APP_HARMAN_FEATURE_SUPPORT
            latency_value = app_cfg_nv.cmd_latency_in_ms;
#endif
            audio_track_latency_set(p_link->a2dp_track_handle, latency_value,
                                    NORMAL_MODE_DYNAMIC_LATENCY_FIX);
            bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
        }

        if (p_link->playback_muted)
        {
            audio_track_volume_out_mute(p_link->a2dp_track_handle);
        }
        else
        {
            app_audio_vol_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
        }

        APP_PRINT_INFO2("[SD_CHECK] app_audio_a2dp_stream_start_handle2 %d,%d", app_db.spk_eq_mode,
                        app_cfg_nv.eq_idx);

        app_eq_idx_check_accord_mode();
        p_link->eq_instance = app_eq_create(EQ_CONTENT_TYPE_AUDIO, EQ_STREAM_TYPE_AUDIO, SPK_SW_EQ,
                                            app_db.spk_eq_mode, app_cfg_nv.eq_idx);
        p_link->audio_eq_enabled = false;

        if (p_link->eq_instance != NULL)
        {
            //not set default eq when audio connect,enable when set eq para from SRC
            if (!app_db.eq_ctrl_by_src)
            {
                app_eq_audio_eq_enable(&p_link->eq_instance, &p_link->audio_eq_enabled);
            }
#if (F_APP_USER_EQ_SUPPORT == 1)
            else if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
            {
                app_eq_audio_eq_enable(&p_link->eq_instance, &p_link->audio_eq_enabled);
            }
#endif

            audio_track_effect_attach(p_link->a2dp_track_handle, p_link->eq_instance);
        }

#if F_APP_ERWS_SUPPORT
        if (remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
        {
#if F_APP_LOCAL_PLAYBACK_SUPPORT
            if (app_db.sd_playback_switch == 1)
            {
                app_db.wait_resume_a2dp_flag = true;
                return;
            }
#endif
            audio_track_start(p_link->a2dp_track_handle);
        }
        else
#endif
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            if (app_teams_multilink_find_sco_conn_num() ||
                multilink_usb_idx == app_teams_multilink_get_active_low_priority_app_index() ||
                memcmp(app_db.br_link[app_teams_multilink_get_active_low_priority_app_index()].bd_addr,
                       p_link->bd_addr,
                       6) != 0)
#else
            if (memcmp(app_db.br_link[active_a2dp_idx].bd_addr, p_link->bd_addr, 6) != 0)
#endif
            {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                APP_PRINT_TRACE2("app_audio_a2dp_stream_start_handle: not match active a2dp index %d, a2dp index %d",
                                 app_teams_multilink_get_active_music_index(), p_link->id);
#else
                APP_PRINT_TRACE1("app_audio_a2dp_stream_start_handle: not match active a2dp index %d",
                                 active_a2dp_idx);
#endif
            }

#if F_APP_LOCAL_PLAYBACK_SUPPORT
            else if (app_db.sd_playback_switch == 1)
            {
                app_db.wait_resume_a2dp_flag = true;
            }
#endif
            else
            {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
                app_multilink_customer_music_event_handle(active_a2dp_idx, JUDGE_EVENT_A2DP_START);
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

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
#if (F_APP_USER_EQ_SUPPORT == 1)
                if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
                {
                    app_report_eq_idx(EQ_INDEX_REPORT_BY_GET_UNSAVED_EQ);
                }
                else
#endif
                {
                    app_report_eq_idx(EQ_INDEX_REPORT_BY_PLAYING);
                }
            }
        }
    }
}

void app_audio_a2dp_play_status_update(T_APP_A2DP_STREAM_EVENT event)
{
//    bool need_to_sync = false;

    if (app_db.a2dp_play_status == false)
    {
        if (event == APP_A2DP_STREAM_AVRCP_PLAY || event == APP_A2DP_STREAM_A2DP_START)
        {
            app_db.a2dp_play_status = true;
//            need_to_sync = true;
        }
    }
    else
    {
        if (event == APP_A2DP_STREAM_AVRCP_PAUSE || event == APP_A2DP_STREAM_A2DP_STOP)
        {
            app_db.a2dp_play_status = false;
//            need_to_sync = true;
        }
    }

#if F_APP_ERWS_SUPPORT
    if (need_to_sync)
    {
        if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
            (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_A2DP_PLAY_STATUS);
        }
    }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    app_bt_policy_msg_gaming_dongle_streaming();
#endif

    APP_PRINT_TRACE2("app_audio_a2dp_play_status_update: event %d status %d", event,
                     app_db.a2dp_play_status);
}

static void app_audio_check_sbc_frame_num(T_APP_BR_LINK *p_link, T_BT_EVENT_PARAM *param)
{
    if (p_link->a2dp_codec_type == BT_A2DP_CODEC_TYPE_SBC)
    {
        uint8_t channel_mode = p_link->a2dp_codec_info.sbc.channel_mode;
        uint8_t subbands = p_link->a2dp_codec_info.sbc.subbands;
        uint8_t channels = (channel_mode == AUDIO_SBC_CHANNEL_MODE_MONO) ? 1 : 2;
        uint8_t block_length = p_link->a2dp_codec_info.sbc.block_length;
        uint8_t bitpool = param->a2dp_stream_data_ind.payload[2];
        uint8_t frame_len = 0;

        if (channel_mode == AUDIO_SBC_CHANNEL_MODE_MONO || channel_mode == AUDIO_SBC_CHANNEL_MODE_DUAL)
        {
            frame_len = 4 + (4 * subbands * channels) / 8 + (block_length * channels * bitpool + 4) / 8;
        }
        else if (channel_mode == AUDIO_SBC_CHANNEL_MODE_STEREO ||
                 channel_mode == AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO)
        {
            uint8_t join = (channel_mode == AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO) ? 1 : 0;

            frame_len = 4 + (4 * subbands * channels) / 8 + (join * subbands + block_length * bitpool + 4) / 8;
        }

        if (frame_len != 0)
        {
            if (abs(frame_len * param->a2dp_stream_data_ind.frame_num - param->a2dp_stream_data_ind.len) >=
                frame_len / 2)
            {
                param->a2dp_stream_data_ind.frame_num = param->a2dp_stream_data_ind.len / frame_len;
            }
        }
    }
}

static void app_audio_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();


    switch (event_type)
    {
    case BT_EVENT_SCO_CONN_CMPL:
        {
            int8_t temp_tx_power = app_cfg_const.bud_to_phone_sco_tx_power_control;

            if (temp_tx_power > 7)
            {
                temp_tx_power -= 16;
            }

            bt_link_tx_power_set(param->sco_conn_cmpl.bd_addr, temp_tx_power);

            if (param->sco_conn_cmpl.cause != 0)
            {
                break;
            }
            app_audio_sco_conn_cmpl_handle(param->sco_conn_cmpl.bd_addr, param->sco_conn_cmpl.air_mode,
                                           param->sco_conn_cmpl.rx_pkt_len);
        }
        break;

    case BT_EVENT_SCO_DATA_IND:
        {
            uint16_t written_len;
            T_APP_BR_LINK *p_link;
            T_APP_BR_LINK *p_active_link = &app_db.br_link[active_hf_idx];
            T_AUDIO_STREAM_STATUS  status;

            handle = false;

            p_link = app_link_find_br_link(param->sco_data_ind.bd_addr);
            if (p_link == NULL)
            {
                break;
            }

#if F_APP_ERWS_SUPPORT
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
#endif
            {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                uint8_t active_record_idx = app_teams_multilink_get_active_record_index();
                uint8_t call_status = p_link->call_status;
                if ((call_status && memcmp(p_active_link->bd_addr, p_link->bd_addr, 6) != 0) ||
                    (!call_status && active_record_idx != p_link->id))
                {
                    APP_PRINT_TRACE3("BT_EVENT_SCO_DATA_IND: link status is not active %s %s %d.",
                                     TRACE_BDADDR(p_active_link->bd_addr), TRACE_BDADDR(p_link->bd_addr), active_record_idx);
                    break;
                }
#else
                if (memcmp(p_active_link->bd_addr, p_link->bd_addr, 6) != 0)
                {
                    break;
                }
#endif
            }
#if F_APP_ERWS_SUPPORT
#if F_APP_MUTILINK_VA_PREEMPTIVE
            else if (app_cfg_const.enable_multi_link &&
                     remote_session_role_get() == REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (memcmp(p_link->bd_addr, app_db.active_hfp_addr, 6) != 0)
                {
                    break;
                }
            }
#endif
#endif
            p_link->sco_seq_num++;
            if (param->sco_data_ind.status == BT_SCO_PKT_STATUS_OK)
            {
                status = AUDIO_STREAM_STATUS_CORRECT;
            }
            else
            {
                status = AUDIO_STREAM_STATUS_LOST;
            }

            audio_track_write(p_link->sco_track_handle, param->sco_data_ind.bt_clk,
                              p_link->sco_seq_num,
                              status,
                              1,
                              param->sco_data_ind.p_data,
                              param->sco_data_ind.length,
                              &written_len);
        }
        break;

    case BT_EVENT_A2DP_CONFIG_CMPL:
        {
            T_AUDIO_FORMAT_INFO format_info = {};
            app_audio_save_a2dp_config((uint8_t *)&param->a2dp_config_cmpl, &format_info);
        }
        break;

#if 0
    case BT_EVENT_A2DP_SNIFFING_CONN_CMPL:
        {
            T_AUDIO_FORMAT_INFO format_info = {};
            app_audio_save_a2dp_config((uint8_t *)&param->a2dp_sniffing_conn_cmpl, &format_info);
        }
        break;
#endif

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
#if F_APP_BUD_LOCATION_SUPPORT
            if (LIGHT_SENSOR_ENABLED)
            {
                if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    app_audio_update_detect_suspend_by_out_ear(false);
                }
            }
#endif

#if F_APP_LEA_SUPPORT
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            if (mtc_is_lea_cis_stream(NULL) || app_db.remote_cis_link_is_streaming)
            {
                // don't handle a2dp stream start when cis is streaming
                break;
            }
#else
            if (mtc_get_btmode())
            {
                if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
                {
                    app_bt_policy_qos_param_update(app_db.br_link[active_a2dp_idx].bd_addr,
                                                   BP_TPOLL_LEA_SUSPEND_A2DP_EVENT);
                }
                break;
            }
#endif
#endif
#if GFPS_SASS_LEA_SUPPORT
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
            if (p_link)
            {
                //run preem if the sass switch policy is different with original
                uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_EDR_LINK,
                                                                MULTILINK_SASS_A2DP_PREEM, false);

                if (sass_preempt >> 8)
                {
                    break;
                }
            }
#endif
            app_db.playback_start_time = sys_timestamp_get();
            app_audio_a2dp_stream_start_handle(param->a2dp_stream_start_ind.bd_addr,
                                               (uint8_t *)&param->a2dp_stream_start_ind);


        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
            uint16_t written_len;
            T_AUDIO_STREAM_MODE  mode;
            T_AUDIO_TRACK_STATE state;
            T_APP_BR_LINK *p_link;

            handle = false;

            p_link = app_link_find_br_link(param->a2dp_stream_data_ind.bd_addr);
            if (p_link == NULL)
            {
                break;
            }

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            if (mtc_is_lea_cis_stream(NULL))
            {
                // ignore a2dp stream data when cis is streaming
                break;
            }
#endif

#if (F_APP_DUAL_AUDIO_EFFECT == 1)
            if (app_dual_audio_get_a2dp_ignore())
            {
                break;
            }
#endif

#if F_APP_ANC_SUPPORT
            if (app_anc_ramp_tool_is_busy() || app_anc_tool_burn_gain_is_busy())
            {
                break;
            }
#endif

#if F_APP_SAIYAN_EQ_FITTING
            if (app_eq_fitting_is_busy())
            {
                break;
            }
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
            if (app_db.sd_playback_switch == 1)
            {
                app_db.wait_resume_a2dp_flag = true;
                break;
            }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
            uint8_t addr[6] = {0};

            if (app_dongle_get_connected_dongle_addr(addr))
            {
#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
                if (dongle_ctrl_data.disallow_dongle_a2dp)
                {
                    break;
                }
#endif

                if (app_db.gaming_mode &&
                    (param->a2dp_stream_data_ind.len > STEREO_GAMING_PKT_SIZE) &&
                    (remote_session_role_get() == REMOTE_SESSION_ROLE_SINGLE) &&
                    (memcmp(addr, param->a2dp_stream_data_ind.bd_addr, 6) == 0))
                {
                    app_dongle_dispatch_ctrl_pkt(param->a2dp_stream_data_ind.payload, &param->a2dp_stream_data_ind.len);
                }
            }
#endif

#if F_APP_OTA_SUPPORT
            if (app_ota_dfu_is_busy()
#if F_APP_TUYA_SUPPORT
                || tuya_ota_is_busy()
#endif
               )
            {
                //If ota is going on, a2dp data won't be written to audio track.
                break;
            }
#endif
            app_audio_check_sbc_frame_num(p_link, param);

#if F_APP_MUTILINK_VA_PREEMPTIVE
            if (app_cfg_const.enable_multi_link)
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                //silent check
                app_harman_silent_check(p_link->id,
                                        audio_track_data_in_is_silent(p_link->a2dp_track_handle, (param->a2dp_stream_data_ind.len + 21)));
#endif
                APP_PRINT_TRACE5("app_audio_bt_cback: multilink dbg %d %d %d %d, %d",
                                 app_cfg_nv.bud_role,
                                 app_hfp_get_call_status(),
                                 p_link->id,
                                 active_hf_idx,
                                 param->a2dp_stream_data_ind.len);

                if (((p_link->id == active_hf_idx) &&
                     (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                      app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)) ||
                    ((memcmp(p_link->bd_addr, app_db.active_hfp_addr, 6) == 0) &&
                     (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)) ||
                    (app_hfp_get_call_status() == APP_CALL_IDLE))
                {
                    app_audio_judge_resume_a2dp_param();
                    audio_track_write(p_link->a2dp_track_handle, param->a2dp_stream_data_ind.bt_clock,
                                      param->a2dp_stream_data_ind.seq_num,
                                      AUDIO_STREAM_STATUS_CORRECT,
                                      param->a2dp_stream_data_ind.frame_num,
                                      param->a2dp_stream_data_ind.payload,
                                      param->a2dp_stream_data_ind.len,
                                      &written_len);
                }
            }
            else
#endif
            {
                app_audio_judge_resume_a2dp_param();
                audio_track_write(p_link->a2dp_track_handle, param->a2dp_stream_data_ind.bt_clock,
                                  param->a2dp_stream_data_ind.seq_num,
                                  AUDIO_STREAM_STATUS_CORRECT,
                                  param->a2dp_stream_data_ind.frame_num,
                                  param->a2dp_stream_data_ind.payload,
                                  param->a2dp_stream_data_ind.len,
                                  &written_len);
            }
            app_bt_policy_set_first_connect_sync_default_vol_flag(false);
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            app_audio_a2dp_track_release(param->a2dp_stream_stop.bd_addr);
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            app_audio_a2dp_track_release(param->a2dp_stream_close.bd_addr);
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
#if F_APP_TTS_SUPPORT
            if (param->hfp_call_status.curr_status == BT_HFP_CALL_ACTIVE ||
                param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
            {
                if ((tts_path == CMD_PATH_SPP) || (tts_path == CMD_PATH_IAP))
                {
                    T_APP_BR_LINK *p_link = app_link_find_br_link(param->hfp_call_status.bd_addr);

                    if ((p_link != NULL) && (p_link->tts_info.tts_handle != NULL))
                    {
                        app_audio_tts_report(p_link->id, TTS_SESSION_ABORT_REQ);
                        tts_stop(p_link->tts_info.tts_handle);
                    }
                }
                else if (tts_path == CMD_PATH_LE)
                {
                    T_APP_LE_LINK *p_link = app_link_find_le_link_by_addr(param->hfp_call_status.bd_addr);

                    if ((p_link != NULL) && (p_link->tts_info.tts_handle != NULL))
                    {
                        app_audio_tts_report(p_link->id, TTS_SESSION_ABORT_REQ);
                        tts_stop(p_link->tts_info.tts_handle);
                    }
                }

            }
#endif
        }
        break;

    case BT_EVENT_AVRCP_TRACK_CHANGED:
    case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET:
        {

        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            if (memcmp(app_db.br_link[active_a2dp_idx].bd_addr,
                       param->avrcp_play_status_changed.bd_addr, 6) == 0) //Only handle active a2dp index AVRCP status
            {
                T_APP_BR_LINK *p_link = app_link_find_b2s_link(param->avrcp_play_status_changed.bd_addr);

                if (p_link == NULL)
                {
                    break;
                }

                if (app_db.avrcp_play_status != param->avrcp_play_status_changed.play_status
#if F_APP_GAMING_DONGLE_SUPPORT
                    && !(event_type == BT_EVENT_AVRCP_PLAY_STATUS_RSP &&
                         app_link_check_dongle_link(param->avrcp_play_status_changed.bd_addr))
#endif
                   )
                {
                    app_audio_set_avrcp_status(param->avrcp_play_status_changed.play_status);
                    app_avrcp_sync_status();
                }

#if F_APP_BUD_LOCATION_SUPPORT
                if (event_type == BT_EVENT_AVRCP_PLAY_STATUS_CHANGED)
                {
                    app_audio_update_in_ear_recover_a2dp(false);

                    if ((param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_STOPPED) ||
                        (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PAUSED))
                    {
                        if (LIGHT_SENSOR_ENABLED)
                        {
                            if (app_db.detect_suspend_by_out_ear == true)
                            {
                                //handle below
                            }
                            else if (app_db.in_ear_recover_a2dp == true)
                            {
                                //skip clearing app_db.in_ear_recover_a2dp if customer's
                                //test instrument send acrcp play status PAUSED twice due to sending flow wrong
                            }
                            else if (app_cfg_const.in_ear_auto_playing)
                            {
                                app_audio_update_in_ear_recover_a2dp(true);
                            }
                        }

                        if (p_link->waiting_paused_status_change == true)
                        {
                            p_link->waiting_paused_status_change = false;

                            app_stop_timer(&timer_idx_waiting_paused_status);

                            if (LIGHT_SENSOR_ENABLED && (app_db.detect_suspend_by_out_ear == true))
                            {
                                app_audio_update_detect_suspend_by_out_ear(false);
                                app_audio_update_in_ear_recover_a2dp(true);
                                if (app_db.last_bud_loc_event == EVENT_IN_EAR)
                                {
                                    uint8_t action = app_audio_in_ear_action(0, CAUSE_ACTION_PLAY_STATUS_CHANGE);
                                    app_audio_action_when_bud_loc_changed(action);
                                }
                            }
                        }
                    }
                    else
                    {
                        //playing status
                        if (LIGHT_SENSOR_ENABLED)
                        {
                            app_audio_update_detect_suspend_by_out_ear(false);
                            app_audio_update_in_ear_recover_a2dp(false);
                        }

                        if (p_link->waiting_play_status_change == true)
                        {
                            app_stop_timer(&timer_idx_waiting_play_status);
                            p_link->waiting_play_status_change = false;

                            if (LIGHT_SENSOR_ENABLED && (app_db.last_bud_loc_event == EVENT_OUT_EAR)
                                && (app_db.resume_a2dp_by_in_ear == true))
                            {
                                uint8_t action = app_audio_out_ear_action(0);
                                app_audio_action_when_bud_loc_changed(action);
                            }
                        }

                        app_db.resume_a2dp_by_in_ear = false;
                    }
                }
#endif
            }
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {
#if F_APP_ERWS_SUPPORT
            if (!app_link_check_b2b_link(param->acl_conn_success.bd_addr) &&
                remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED)
#endif
            {
#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_LINEIN_SUPPORT
                if (!app_line_in_playing_state_get())
#endif
                {
                    app_dual_audio_lr_info(MODE_INFO_L_R_DIV2, false, app_cfg_const.solo_speaker_channel);
                }

                if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
                {
                    app_dual_audio_single_effect();
                }
#else
                app_audio_speaker_channel_set(AUDIO_SINGLE_SET, 0);
#endif
            }
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_ACL_ROLE_SLAVE:
        {
            T_APP_BR_LINK *p_link;
            T_AUDIO_STREAM_USAGE usage;
            T_AUDIO_STREAM_MODE mode;
            uint8_t vol;
            T_AUDIO_FORMAT_INFO format;

            if (!app_link_check_b2b_link(param->acl_role_slave.bd_addr) &&
                (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                app_db.src_roleswitch_fail_no_sniffing)
            {
                app_db.src_roleswitch_fail_no_sniffing = false;

                p_link = app_link_find_br_link(param->acl_role_slave.bd_addr);
                if ((p_link != NULL) && (p_link->a2dp_track_handle != NULL))
                {
                    if (audio_track_mode_get(p_link->a2dp_track_handle, &mode) == true)
                    {
                        audio_track_format_info_get(p_link->a2dp_track_handle, &format);
                        audio_track_volume_out_get(p_link->a2dp_track_handle, &vol);
                        audio_track_usage_get(p_link->a2dp_track_handle, &usage);

                        if (usage == AUDIO_STREAM_USAGE_LOCAL)
                        {
                            audio_track_release(p_link->a2dp_track_handle);
                            p_link->a2dp_track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                                           mode,
                                                                           AUDIO_STREAM_USAGE_SNOOP,
                                                                           format,
                                                                           vol,
                                                                           0,
                                                                           AUDIO_DEVICE_OUT_DEFAULT,
                                                                           NULL,
                                                                           NULL);
                            audio_track_start(p_link->a2dp_track_handle);
                        }
                    }
                }
            }
        }
        break;
#endif

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            app_bt_sniffing_process(param->a2dp_conn_cmpl.bd_addr);

            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                p_link->playback_muted = 0;
            }
        }
        break;


    case BT_EVENT_HFP_CONN_CMPL:
        {
            app_bt_sniffing_process(param->hfp_conn_cmpl.bd_addr);
        }
        break;

    case BT_EVENT_HFP_SUPPORTED_FEATURES_IND:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->hfp_supported_features_ind.bd_addr);
            if (p_link != NULL)
            {
                p_link->remote_hfp_brsf_capability = param->hfp_supported_features_ind.ag_bitmap;

                if (param->hfp_supported_features_ind.ag_bitmap & BT_HFP_HF_REMOTE_CAPABILITY_INBAND_RINGING)
                {
                    p_link->is_inband_ring = true;
                }

                APP_PRINT_INFO1("app_audio_bt_cback: BT_EVENT_HFP_SUPPORTED_FEATURES_IND, remote_hfp_brsf_capability 0x%04x",
                                p_link->remote_hfp_brsf_capability);
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            app_audio_a2dp_track_release(param->a2dp_disconn_cmpl.bd_addr);
        }
        break;


    case BT_EVENT_SCO_DISCONNECTED:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->sco_disconnected.bd_addr);

            bt_link_tx_power_set(param->sco_disconnected.bd_addr, 0);

            if (p_link != NULL)
            {
                if (p_link->sco_track_handle != NULL)
                {
                    audio_track_release(p_link->sco_track_handle);
                    p_link->sco_track_handle = NULL;
                }

                app_nrec_detach(p_link->sco_track_handle, p_link->nrec_instance);
                app_sidetone_detach(p_link->sco_track_handle, p_link->sidetone_instance);

#if F_APP_VOICE_SPK_EQ_SUPPORT
                if (p_link->voice_spk_eq_instance != NULL)
                {
                    eq_release(p_link->voice_spk_eq_instance);
                    p_link->voice_spk_eq_instance = NULL;
                }

                if (p_link->voice_mic_eq_instance != NULL)
                {
                    eq_release(p_link->voice_mic_eq_instance);
                    p_link->voice_mic_eq_instance = NULL;
                }
#endif

                p_link->voice_muted = 0;
            }

#if F_APP_VOICE_SPK_EQ_SUPPORT
            app_eq_change_audio_eq_mode(true);
#endif

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#else
                p_link = &app_db.br_link[active_a2dp_idx];
                if (p_link == NULL || p_link->a2dp_track_handle == NULL)
                {
                    break;
                }

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
        break;

#if F_APP_ERWS_SUPPORT
#if F_APP_QOL_MONITOR_SUPPORT
    case BT_EVENT_ACL_SNIFFING_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                if (app_db.sec_going_away)
                {
                    int8_t b2b_rssi = 0;
                    app_qol_get_aggregate_rssi(true, &b2b_rssi);

                    if (b2b_rssi < RSSI_MINIMUM_THRESHOLD_TO_RECEIVE_PACKET_BAD)
                    {
                        bt_sniffing_set_a2dp_dup_num(true, 0, true);
                    }
                    else
                    {
                        app_db.sec_going_away = false;
                        app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_SEC_GOING_NEAR);
                        app_audio_remote_join_set(false);
                    }
                }
            }
        }
        break;
#endif

    case BT_EVENT_ACL_SNIFFING_DISCONN_CMPL:
        {
        }
        break;

    case BT_EVENT_SCO_SNIFFING_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_APP_BR_LINK *p_link = app_link_find_br_link(param->sco_sniffing_conn_cmpl.bd_addr);

                if (p_link == NULL)
                {
                    p_link = app_link_alloc_br_link(param->sco_sniffing_conn_cmpl.bd_addr);
                }

                if (p_link != NULL)
                {
                    app_hfp_set_active_idx(p_link->bd_addr);
                    p_link->sco_handle = param->sco_sniffing_conn_cmpl.handle;
                }
            }

            app_audio_sco_conn_cmpl_handle(param->sco_sniffing_conn_cmpl.bd_addr,
                                           param->sco_sniffing_conn_cmpl.air_mode,
                                           param->sco_sniffing_conn_cmpl.rx_pkt_len);
        }
        break;

    case BT_EVENT_SCO_SNIFFING_DISCONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->sco_sniffing_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->sco_track_handle != NULL)
                {
                    audio_track_release(p_link->sco_track_handle);
                    p_link->sco_track_handle = NULL;
                }

                app_nrec_detach(p_link->sco_track_handle, p_link->nrec_instance);
                app_sidetone_detach(p_link->sco_track_handle, p_link->sidetone_instance);

#if F_APP_VOICE_SPK_EQ_SUPPORT
                if (p_link->voice_spk_eq_instance != NULL)
                {
                    eq_release(p_link->voice_spk_eq_instance);
                    p_link->voice_spk_eq_instance = NULL;
                }

                if (p_link->voice_mic_eq_instance != NULL)
                {
                    eq_release(p_link->voice_mic_eq_instance);
                    p_link->voice_mic_eq_instance = NULL;
                }
#endif

                p_link->voice_muted = 0;
                p_link->sco_handle = 0;  // SCO link disconnected
            }

            // volume_in can be unmute only after audio track released,
            // otherwise it will cause the dsp calculation to diverge
            audio_volume_in_unmute(AUDIO_STREAM_TYPE_VOICE);

#if F_APP_VOICE_SPK_EQ_SUPPORT
            app_eq_change_audio_eq_mode(true);
#endif

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
                if (!app_slide_switch_mic_mute_toggle_support())
#endif
                {
                    app_audio_set_mic_mute_status(0);
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link = NULL;
            app_bt_policy_sync_b2s_connected();
#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_LINEIN_SUPPORT
            if (!app_line_in_playing_state_get())
#endif
            {
                if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
                {
                    app_dual_audio_lr_info(MODE_INFO_L, false, app_cfg_const.couple_speaker_channel);
                }
                else
                {
                    app_dual_audio_lr_info(MODE_INFO_R, false, app_cfg_const.couple_speaker_channel);
                }
            }
#else
            app_audio_speaker_channel_set(AUDIO_COUPLE_SET, 0);
#endif

            /* this info is exchanged between pri and sec */
            uint8_t either_bud_has_vol_ctrl_key = app_cfg_nv.either_bud_has_vol_ctrl_key;
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                APP_REMOTE_MSG_SYNC_EITHER_BUD_HAS_VOL_CTRL_KEY,
                                                (uint8_t *)&either_bud_has_vol_ctrl_key, 1);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
                if (app_cfg_nv.is_notification_vol_set_from_phone)
                {
                    app_audio_notification_vol_relay(app_cfg_nv.voice_prompt_volume_out_level, true, false);
                }
#endif
                app_relay_total_async();

                /* note: this must be placed at the last for fixed inband ringtone purpose
                         (prevent voice gain is changed by the subsequent relay message) */
                app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_CALL_STATUS);

                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        p_link = &app_db.br_link[i];
                        if (p_link != NULL)
                        {
                            app_audio_mute_status_sync(p_link->bd_addr, AUDIO_STREAM_TYPE_PLAYBACK);
                            app_audio_mute_status_sync(p_link->bd_addr, AUDIO_STREAM_TYPE_VOICE);
                        }
                    }
                }
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_DEFAULT_CHANNEL);
            }
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
#if (F_APP_DUAL_AUDIO_EFFECT == 1)
            app_dual_audio_lr_info(MODE_INFO_L_R_DIV2, false, app_cfg_const.solo_speaker_channel);
            app_dual_audio_single_effect();
            app_audio_restart_track();
#else
            app_audio_speaker_channel_set(AUDIO_SINGLE_SET, 0);
#endif
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            T_AUDIO_STREAM_MODE  mode;
            T_APP_BR_LINK *p_link = NULL;

            if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                if (app_db.local_loc == BUD_LOC_IN_CASE)
                {
#if F_APP_DURIAN_SUPPORT
                    /*not mute spk for durian*/
#else
                    app_audio_spk_mute_unmute(true);
#endif
                }

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_BUD_COUPLING);
                }
                else
                {
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BUD_COUPLING, app_cfg_const.timer_auto_power_off);
                    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_SOURCE_LINK);

                    if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
                    {
                        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK,
                                                  app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                        if (!app_db.a2dp_play_status)
                        {
                            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_AUDIO,
                                                      app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                        }
                        if (app_hfp_get_call_status() == APP_CALL_IDLE)
                        {
                            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_VOICE,
                                                      app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                        }
                    }

                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (app_link_get_b2s_link_num() > 0)
                        {
                            if (app_cfg_const.disable_low_bat_role_swap == 0)
                            {
                                app_roleswap_req_battery_level();
                            }
                        }
                    }

                    if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
                    {
                        app_hfp_always_playing_outband_tone();
                    }
                }

                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        p_link = &app_db.br_link[i];

                        if (app_db.br_link[i].connected_profile & (SPP_PROFILE_MASK | IAP_PROFILE_MASK))
                        {
                            app_db.br_link[i].cmd_set_enable = true;
                        }

                        if ((i < MAX_BLE_LINK_NUM) && (app_db.le_link[i].state == LE_LINK_STATE_CONNECTED))
                        {
                            app_db.le_link[i].cmd_set_enable = true;
                        }

                        app_report_rws_bud_side();
                        break;
                    }
                }

                if (p_link == NULL)
                {
                    break;
                }

                if (audio_track_mode_get(p_link->a2dp_track_handle, &mode) == true)
                {
                    if (mode == AUDIO_STREAM_MODE_LOW_LATENCY || mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        p_link->playback_muted = false;
                    }
                }

                if (p_link->sco_track_handle != NULL)
                {
                    app_nrec_roleswap(p_link->nrec_instance, false);
                }
            }
            else if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_HFP_INFO)
            {
                if (param->remote_roleswap_status.u.hfp.call_status == BT_HFP_CALL_ACTIVE)
                {
                    if ((mp_dual_mic_setting >> 4) == AUDIO_MP_DUAL_MIC_SETTING_ROLE_SWAP)
                    {
                        app_audio_mp_dual_mic_switch_action();
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_STARTED:
        {
            if (param->a2dp_sniffing_started.cause == HCI_SUCCESS)
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
#if (F_APP_USER_EQ_SUPPORT == 1)
                    if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
                    {
                        app_report_eq_idx(EQ_INDEX_REPORT_BY_GET_UNSAVED_EQ);
                    }
                    else
#endif
                    {
                        app_report_eq_idx(EQ_INDEX_REPORT_BY_PLAYING);
                    }
                }
                else if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
                         (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    T_APP_BR_LINK *p_link = app_link_find_br_link(param->a2dp_sniffing_started.bd_addr);

                    if (p_link != NULL)
                    {
                        p_link->streaming_fg = true;
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_STOPPED:
        {
            T_AUDIO_STREAM_MODE  mode;
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_sniffing_stopped.bd_addr);
            if (p_link == NULL)
            {
                break;
            }
            if (param->a2dp_sniffing_stopped.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
            {
                if (audio_track_mode_get(p_link->a2dp_track_handle, &mode) == true)
                {
                    if (mode == AUDIO_STREAM_MODE_LOW_LATENCY || mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                        {
                            audio_track_pause(p_link->a2dp_track_handle);
                        }
                        else
                        {
                            audio_track_volume_out_mute(p_link->a2dp_track_handle);
                            p_link->playback_muted = true;
                        }
                    }
                }
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    p_link = app_link_find_br_link(param->a2dp_sniffing_stopped.bd_addr);
                    if (p_link != NULL)
                    {
                        if (p_link->a2dp_track_handle != NULL)
                        {
                            audio_track_release(p_link->a2dp_track_handle);
                            p_link->a2dp_track_handle = NULL;
                        }

                        if (p_link->eq_instance != NULL)
                        {
                            eq_release(p_link->eq_instance);
                            p_link->eq_instance = NULL;
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_START_IND:
        {

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            if (mtc_is_lea_cis_stream(NULL))
            {
                // don't handle a2dp stream start when cis is streaming
                break;
            }
#endif
            app_audio_a2dp_stream_start_handle(param->a2dp_sniffing_start_ind.bd_addr,
                                               (uint8_t *)&param->a2dp_sniffing_start_ind);
        }
        break;

#endif

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_audio_bt_cback: event_type 0x%04x", event_type);
    }
}
#if F_APP_LEA_SUPPORT
static void app_audio_policy_media_eq_enable(T_APP_BR_LINK *p_link)
{
    if (mtc_get_btmode() == MULTI_PRO_BT_BREDR && p_link != NULL)
    {
        app_eq_audio_eq_enable(&p_link->eq_instance, &p_link->audio_eq_enabled);
    }
    else if (mtc_get_btmode() != MULTI_PRO_BT_BREDR && app_lea_uca_get_eq_instance() != NULL)
    {
        app_eq_audio_eq_enable(app_lea_uca_p_eq_instance(), app_lea_uca_get_eq_abled());
    }
}
#endif
static void app_audio_policy_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_audio_policy_timeout_cb: timer_evt %d, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_LONG_PRESS_REPEAT:
        {
            if (app_db.play_min_max_vol_tone == true)
            {
                if (param == VOL_REPEAT_MAX) // vol_max
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_audio_policy_vol_max_tone_play();
                    }
                }
                else if (param == VOL_REPEAT_MIN) // vol_min
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_audio_tone_type_play(TONE_VOL_MIN, false, true);
                    }
                }
                app_db.play_min_max_vol_tone = false;
            }
            else
            {
                app_stop_timer(&timer_idx_long_press_repeat);
            }
        }
        break;

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    case APP_TIMER_NOTIFICATION_VOL_ADJ_WAIT_SEC_RSP:
        {
            app_stop_timer(&timer_idx_notification_vol_adj_wait_sec_rsp);

            uint8_t report_status;

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                // report spk2 failed
                report_status = CHECK_IS_LCH ? VOL_CMD_STATUS_FAILED_R : VOL_CMD_STATUS_FAILED_L;
            }
            else
            {
                // spk2 not exist
                report_status = VOL_CMD_STATUS_SUCCESS;
            }

            vol_cmd_status_mask |= report_status;
            app_audio_notification_vol_report_status(vol_cmd_status_mask, active_vol_cmd_path,
                                                     active_vol_app_idx);
            vol_cmd_status_mask = 0;
        }
        break;

    case APP_TIMER_NOTIFICATION_VOL_GET_WAIT_SEC_RSP:
        {
            app_stop_timer(&timer_idx_notification_vol_get_wait_sec_rsp);
            app_audio_notification_vol_report_info();
        }
        break;
#endif

#if F_APP_BUD_LOCATION_SUPPORT
    case APP_TIMER_IN_EAR_RESTORE_A2DP:
        {
            app_audio_update_detect_suspend_by_out_ear(false);
            app_audio_update_in_ear_recover_a2dp(false);
        }
        break;

    case APP_TIMER_WAITING_PAUSED_STATUS:
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[param];

            if (p_link != NULL)
            {
                p_link->waiting_paused_status_change = false;
            }
            app_stop_timer(&timer_idx_waiting_paused_status);

            if (app_db.last_bud_loc_event == EVENT_IN_EAR)
            {
                if (app_db.detect_suspend_by_out_ear)
                {
                    app_audio_update_detect_suspend_by_out_ear(false);
                    app_audio_update_in_ear_recover_a2dp(true);
                    uint8_t action = app_audio_in_ear_action(0, CAUSE_ACTION_PLAY_STATUS_CHANGE);
                    app_audio_action_when_bud_loc_changed(action);
                }
                else if (app_db.in_ear_recover_a2dp)
                {
                    uint8_t action = app_audio_in_ear_action(0, CAUSE_ACTION_PLAY_STATUS_CHANGE);
                    app_audio_action_when_bud_loc_changed(action);
                }
            }
        }
        break;

    case APP_TIMER_WAITING_PLAY_STATUS:
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[param];

            if (p_link != NULL)
            {
                p_link->waiting_play_status_change = false;
            }
            app_stop_timer(&timer_idx_waiting_play_status);

            if (app_db.last_bud_loc_event == EVENT_OUT_EAR)
            {
                uint8_t action = app_audio_out_ear_action(0);
                app_audio_action_when_bud_loc_changed(action);
            }
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_audio_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    uint16_t ret = 0;

    uint8_t is_mic_mute;
    T_APP_CALL_STATUS call_status;

    uint8_t mp_dual_mic_setting_temp;
    uint8_t last_src_conn_idx;
    bool is_key_volume_set = false;
    T_APP_AUDIO_VOLUME *vol_buf = NULL;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_PLAY_MIN_MAX_VOL_TONE:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.play_min_max_vol_tone;
        }
        break;

    case APP_REMOTE_MSG_GAMING_MODE_SET_ON:
        {
            payload_len = 6;
            msg_ptr = (uint8_t *)app_db.br_link[app_a2dp_get_active_idx()].bd_addr;
        }
        break;

    case APP_REMOTE_MSG_AUDIO_CHANNEL_CHANGE:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_cfg_nv.spk_channel;
        }
        break;

    case APP_REMOTE_MSG_SYNC_PLAY_STATUS:
        {
            payload_len = 1;
            msg_ptr = &app_db.avrcp_play_status;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_MIC_STATUS:
        {
            payload_len = 1;
            is_mic_mute = app_audio_is_mic_mute();
            msg_ptr = &is_mic_mute;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_CALL_STATUS:
        {
            payload_len = 1;
            call_status = app_hfp_get_call_status();
            msg_ptr = (uint8_t *)&call_status;
            //skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_VP_LANGUAGE:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.voice_prompt_language;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_LOW_LATENCY:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.gaming_mode;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_BUD_STREAM_STATE:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&bud_stream_state;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_DEFAULT_CHANNEL:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_const.couple_speaker_channel;
        }
        break;

#if F_APP_COMMON_DONGLE_SUPPORT
    case APP_REMOTE_MSG_SYNC_REMOTE_IS_DONGLE:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.remote_is_dongle;
            skip = false;
        }
        break;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    case APP_REMOTE_MSG_SYNC_GAMING_MODE_REQUEST:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.gaming_mode_request_is_received;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_DONGLE_IS_ENABLE_MIC:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.dongle_is_enable_mic;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_DONGLE_IS_DISABLE_APT:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.dongle_is_disable_apt;
            skip = false;
        }
        break;
#endif

    case APP_REMOTE_MSG_SYNC_MP_DUAL_MIC_SETTING:
        {
            payload_len = 1;
            mp_dual_mic_setting_temp = app_audio_get_mp_dual_mic_setting();
            msg_ptr = &mp_dual_mic_setting_temp;
        }
        break;

    case APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.eq_idx;
        }
        break;

    case APP_REMOTE_MSG_SYNC_LAST_CONN_INDEX:
        {
            payload_len = 1;
            last_src_conn_idx = app_bt_policy_b2s_connected_get_last_conn_index();
            msg_ptr = (uint8_t *)&last_src_conn_idx;
        }
        break;

    case APP_REMOTE_MSG_SYNC_LOW_LATENCY_LEVEL:
#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    case APP_REMOTE_MSG_SYNC_IS_NOTIFICATION_VOL_ADJ_FROM_PHONE:
#endif
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_cfg_nv.offset_listening_mode_cycle;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_DISALLOW_PLAY_GAMING_MODE_VP:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.disallow_play_gaming_mode_vp;
        }
        break;

    case APP_REMOTE_MSG_SYNC_GAMING_RECORD_EQ_INDEX:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.eq_idx_gaming_mode_record;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_NORMAL_RECORD_EQ_INDEX:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.eq_idx_normal_mode_record;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_ANC_RECORD_EQ_INDEX:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.eq_idx_anc_mode_record;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_SW_EQ_TYPE:
        {
            payload_len = 1;
            msg_ptr = &app_db.sw_eq_type;
            skip = false;
        }
        break;

#if F_APP_POWER_ON_DELAY_APPLY_APT_SUPPORT
    case APP_REMOTE_MSG_SYNC_APT_POWER_ON_DELAY_APPLY_TIME:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_cfg_nv.offset_xtal_k_result;
            skip = false;
        }
        break;
#endif

    case APP_REMOTE_MSG_SYNC_VOL_IS_CHANGED_BY_KEY:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&is_key_volume_set;
        }
        break;

    case APP_REMOTE_MSG_SYNC_A2DP_PLAY_STATUS:
        {
            payload_len = 1;
            msg_ptr = &app_db.a2dp_play_status;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_FORCE_JOIN_SET:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&force_join;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_CONNECTED_TONE_NEED:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.connected_tone_need;
        }
        break;

    case  APP_REMOTE_MSG_SYNC_IN_EAR_RECOVER_A2DP:
        {
            payload_len = 1;
            msg_ptr = &app_db.in_ear_recover_a2dp;
        }
        break;

    case APP_REMOTE_MSG_SYNC_SUSPEND_A2DP_BY_OUT_EAR:
        {
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_db.detect_suspend_by_out_ear;
        }
        break;

    case APP_REMOTE_MSG_AUDIO_VOLUME_SYNC:
        {
            uint8_t bond_num;
            uint8_t i;
            uint8_t num = 0;
            uint8_t bd_addr[6];
            uint8_t pair_idx_mapping;

            bond_num = app_bond_b2s_num_get();
            vol_buf = malloc(bond_num * sizeof(T_APP_AUDIO_VOLUME));

            if (vol_buf != NULL)
            {
                for (i = 1; i <= bond_num; i++)
                {
                    if (app_bond_b2s_addr_get(i, bd_addr) == true)
                    {
                        app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping);
                        vol_buf[num].playback_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
                        vol_buf[num].voice_volume = app_cfg_nv.voice_gain_level[pair_idx_mapping];
#if F_APP_IPHONE_ABS_VOL_HANDLE
                        vol_buf[num].abs_volume = app_cfg_nv.abs_vol[pair_idx_mapping];
#endif
                        memcpy(vol_buf[num].bd_addr, bd_addr, 6);
                    }
                    else
                    {
                        vol_buf[num].playback_volume = app_dsp_cfg_vol.playback_volume_default;
                        vol_buf[num].voice_volume = app_dsp_cfg_vol.voice_out_volume_default;
#if F_APP_IPHONE_ABS_VOL_HANDLE
                        vol_buf[num].abs_volume = 0;
#endif
                        memset(vol_buf[num].bd_addr, 0, 6);
                    }

                    num++;
                }
                payload_len = bond_num * sizeof(T_APP_AUDIO_VOLUME);
                msg_ptr = (uint8_t *)vol_buf;
            }
            skip = false;
        }
        break;

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    case APP_REMOTE_MSG_SYNC_ALLOWED_SOURCE:
        {
            if (app_cfg_const.enable_24g_bt_audio_source_switch)
            {
                payload_len = 1;
                msg_ptr = (uint8_t *)&app_cfg_nv.offset_listening_mode_cycle;
                skip = false;
            }
        }
        break;
#endif

    default:
        break;
    }

    ret = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_AUDIO_POLICY, payload_len, msg_ptr, skip,
                             total);

    if (vol_buf != NULL)
    {
        free(vol_buf);
    }

    return ret;
}

static void app_audio_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                  T_REMOTE_RELAY_STATUS status)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    APP_PRINT_TRACE2("app_audio_parse_cback: msg = 0x%x, status = %x", msg_type, status);

    switch (msg_type)
    {
#if F_APP_IPHONE_ABS_VOL_HANDLE
    case APP_REMOTE_MSG_SYNC_ABS_VOLUME:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t addr[6];
                uint8_t pair_idx_mapping;

                memcpy(addr, &p_info[0], 6);

                if (app_bond_get_pair_idx_mapping(addr, &pair_idx_mapping) == false)
                {
                    return;
                }

                app_cfg_nv.abs_vol[pair_idx_mapping] = p_info[6];
                app_cfg_store(&app_cfg_nv.abs_vol[0], 8);
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_HFP_VOLUME_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t addr[6];
                uint8_t curr_volume;
                uint8_t pair_idx_mapping;
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(addr, &p_info[0], 6);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                curr_volume = p_info[7];

                APP_PRINT_TRACE4("app_audio_parse_cback: hfp vol sync, vol %d, stream_type %d, local_in_case %d, voice_muted %d",
                                 curr_volume, stream_type, app_db.local_in_case, app_db.voice_muted);

                if (app_bond_get_pair_idx_mapping(addr, &pair_idx_mapping) == false)
                {
                    app_key_set_volume_status(false);
                    return;
                }

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_cfg_nv.voice_gain_level[pair_idx_mapping] = curr_volume;
                }

                if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
                    (app_hfp_get_call_status() == APP_CALL_INCOMING))
                {
                    app_key_set_volume_status(false);
                    app_hfp_inband_tone_mute_ctrl();
                    return;
                }

                p_link = app_link_find_br_link(addr);
                if (p_link != NULL)
                {
                    app_audio_vol_set(p_link->sco_track_handle, curr_volume);
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_A2DP_VOLUME_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t addr[6];
                uint8_t curr_volume;
                uint8_t pair_idx_mapping;
                uint8_t ignore_store;
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(addr, &p_info[0], 6);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                curr_volume = p_info[7];
                ignore_store = p_info[8];

                APP_PRINT_TRACE5("app_audio_parse_cback: a2dp vol sync, vol %d, stream_type %d, local_in_case %d, ignore_store %d, playback_muted %d",
                                 curr_volume, stream_type, app_db.local_in_case, ignore_store, app_db.playback_muted);

                if (app_bond_get_pair_idx_mapping(addr, &pair_idx_mapping) == false)
                {
                    app_key_set_volume_status(false);
                    return;
                }

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY && !ignore_store)
                {
                    app_cfg_nv.audio_gain_level[pair_idx_mapping] = curr_volume;
                }

                p_link = app_link_find_br_link(addr);

                if (p_link != NULL)
                {
                    app_audio_vol_set(p_link->a2dp_track_handle, curr_volume);
                    app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_AUDIO_VOLUME_SYNC:
        {
            uint8_t i;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_APP_AUDIO_VOLUME *para = (T_APP_AUDIO_VOLUME *)buf;
                uint8_t bond_num;
                uint8_t num = 0;
                uint8_t pair_idx_mapping;
                T_APP_BR_LINK *p_link;

                bond_num = app_bond_b2s_num_get();

                for (i = 1; i <= bond_num; i++)
                {
                    if (app_bond_get_pair_idx_mapping(para[num].bd_addr, &pair_idx_mapping))
                    {
                        app_cfg_nv.audio_gain_level[pair_idx_mapping] = para[num].playback_volume;
                        app_cfg_nv.voice_gain_level[pair_idx_mapping] = para[num].voice_volume;

                        p_link = app_link_find_br_link(para[num].bd_addr);
                        if (p_link != NULL)
                        {
                            app_audio_vol_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
                            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);

                            app_audio_vol_set(p_link->sco_track_handle, app_cfg_nv.voice_gain_level[pair_idx_mapping]);
                            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_VOICE);
                        }

#if F_APP_IPHONE_ABS_VOL_HANDLE
                        app_cfg_nv.abs_vol[pair_idx_mapping] = para[num].abs_volume;
                        app_cfg_store(&app_cfg_nv.abs_vol[0], 8);
#endif
                        num++;
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_AUDIO_CFG_PARAM_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_bt_sniffing_sec_set_audio_cfg_param((T_APP_BT_SNIFFING_PARAM *)buf);
            }
        }
        break;

    case APP_REMOTE_MSG_INBAND_TONE_MUTE_SYNC:
        if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
            status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
            status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
            status == REMOTE_RELAY_STATUS_SEND_FAILED)
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t bd_addr[6];
            uint8_t pair_idx;
            T_AUDIO_STREAM_TYPE stream_type;
            T_APP_BR_LINK *p_link = NULL;

            memcpy(bd_addr, &p_info[0], 6);
            stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];

            if (bt_bond_index_get(bd_addr, &pair_idx) == false)
            {
                APP_PRINT_ERROR0("app_audio_parse_cback: inband tone mute sync fail, cannot find pair index");
                return;
            }

            app_hfp_set_vol_change_ongoing(p_info[8]);

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                p_link->voice_muted = p_info[7];

                APP_PRINT_TRACE3("app_audio_parse_cback: inband tone mute sync, voice_muted %d, stream_type %d, local_in_case %d",
                                 p_link->voice_muted, stream_type, app_db.local_in_case);

                if (stream_type == AUDIO_STREAM_TYPE_VOICE)
                {
                    if (p_link->voice_muted)
                    {
                        audio_track_volume_out_mute(p_link->sco_track_handle);
                    }
                    else
                    {
                        audio_track_volume_out_unmute(p_link->sco_track_handle);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_VOICE_NR_SWITCH:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                bool *enable = (bool *)buf;
                audio_probe_dsp_ipc_send_call_status(*enable);
            }
        }
        break;

    case APP_REMOTE_MSG_EQ_DATA:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t eq_mode = buf[0];
                uint8_t *p_info = (uint8_t *)&buf[1];
                uint16_t data_len = len - 1; // buf[0] is eq_mode

                T_EQ_ENABLE_INFO enable_info;

                app_eq_enable_info_get(eq_mode, &enable_info);

                if (app_db.sw_eq_type == SPK_SW_EQ)
                {
                    app_eq_param_set(eq_mode, enable_info.idx, &p_info[0], data_len);
                    app_eq_audio_eq_enable(&enable_info.instance, &enable_info.is_enable);
                }
                else
                {
                    app_eq_param_set(eq_mode, app_cfg_nv.apt_eq_idx, &p_info[0], data_len);
                    eq_enable(app_db.apt_eq_instance);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_PLAY_MIN_MAX_VOL_TONE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_db.play_min_max_vol_tone = *((uint8_t *)buf);
            }
        }
        break;

#if (F_APP_USER_EQ_SUPPORT == 1)
    case APP_REMOTE_MSG_SYNC_USER_EQ:
        {
            uint8_t action = buf[0];
            T_EQ_TYPE eq_type = (T_EQ_TYPE) buf[1];
            uint8_t eq_index = buf[2];

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
                status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED)
            {
                switch (action)
                {
                case EQ_SYNC_USER_EQ:
                    {
                        uint16_t offset = (buf[3] | (buf[4] << 8));
                        eq_utils_save_eq_to_ftl(offset, &buf[5], len - 5);
                    }
                    break;

                case EQ_SYNC_USER_EQ_WHEN_B2B_CONNECTED:
                    {
                        uint16_t offset = (buf[3] | (buf[4] << 8));
                        app_eq_process_sync_user_eq_when_b2b_connected(eq_type, eq_index, offset, &buf[5], len - 5);
                    }
                    break;

                case EQ_SYNC_RESET_EQ:
                    {
                        uint8_t eq_mode = buf[3];
                        app_eq_reset_user_eq(eq_type, eq_mode, eq_index);
                    }
                    break;

#if (F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT == 1)
                case EQ_SYNC_GET_SECONDARY_EQ_INFO:
                    {
                        uint8_t eq_mode = buf[3];
                        uint8_t cmd_path = buf[4];
                        uint8_t app_idx = buf[5];

                        app_eq_report_sec_eq_to_pri(eq_type, eq_mode, eq_index, cmd_path, app_idx);
                    }
                    break;

                case EQ_SYNC_REPORT_SECONDARY_EQ_INFO:
                    {
                        uint8_t cmd_path = buf[3];
                        uint8_t app_idx = buf[4];
                        uint8_t eq_len = (buf[5] | buf[6] << 8);

                        app_eq_report_sec_eq_to_src(cmd_path, app_idx, eq_len, &buf[7]);
                    }
                    break;
#endif

                default:
                    break;
                }
            }
            else if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
            {
                if (action == EQ_SYNC_USER_EQ_WHEN_B2B_CONNECTED)
                {
                    uint16_t offset = (buf[3] | (buf[4] << 8));
                    app_eq_continue_sync_user_eq_when_connected(false, eq_type, eq_index, offset, len - 5);
                }
            }
        }
        break;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    case APP_REMOTE_MSG_ASK_TO_EXIT_GAMING_MODE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t cmd = *(uint8_t *)buf;

                if (cmd)
                {
                    if (app_db.gaming_mode)
                    {
                        app_mmi_switch_gaming_mode();
                    }
                }
            }
        }
        break;
#endif

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    case APP_REMOTE_MSG_NOTIFICATION_VOL_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_NOTIFICATION_VOL_RELAY_MSG *rcv_msg = (T_NOTIFICATION_VOL_RELAY_MSG *)buf;

                app_audio_notification_vol_set_remote_data(rcv_msg);

                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) && rcv_msg->need_to_report)
                {
                    app_audio_notification_vol_report_info();
                }
                else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    if (rcv_msg->need_to_sync != app_cfg_nv.rws_sync_notification_vol)
                    {
                        app_cfg_nv.rws_sync_notification_vol = rcv_msg->need_to_sync;
                        app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 4);
                    }

                    if (rcv_msg->need_to_sync &&
                        app_audio_notification_check_vol_is_valid(rcv_msg->notification_vol_cur_level))
                    {
                        // set local notification volume level as the same as primary bud
                        app_audio_notification_vol_update_local(rcv_msg->notification_vol_cur_level);
                    }

                    if (rcv_msg->first_sync)
                    {
                        // inform primary bud to report phone
                        app_audio_notification_vol_relay(app_cfg_nv.voice_prompt_volume_out_level, false, true);
                    }

                    if (!app_cfg_nv.is_notification_vol_set_from_phone)
                    {
                        app_cfg_nv.is_notification_vol_set_from_phone = 1;
                        app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_CMD:
        {
            /* bypass_cmd
             * byte [0,1] : cmd_id   *
             * byte [2,3] : cmd_len  *
             * byte [4]   : cmd_path *
             * byte [5-N] : cmd      */

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint16_t cmd_id = (p_info[0] | p_info[1] << 8);
                uint16_t vol_cmd_len = (p_info[2] | p_info[3] << 8);

                app_audio_notification_vol_cmd_handle(cmd_id, &p_info[5], vol_cmd_len, p_info[4], p_info[5]);
            }
        }
        break;

    case APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_EVENT:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                /* report
                 * byte [0,1] : event_id    *
                 * byte [2,3] : report_len  *
                 * byte [4-N] : report      */

                uint8_t *p_info = (uint8_t *)buf;
                uint16_t event_id = (p_info[0] | p_info[1] << 8);

                if (event_id == EVENT_NOTIFICATION_VOL_LEVEL_SET)
                {
                    app_stop_timer(&timer_idx_notification_vol_adj_wait_sec_rsp);

                    uint8_t report_status = p_info[4] | vol_cmd_status_mask;
                    app_audio_notification_vol_report_status(report_status, active_vol_cmd_path, active_vol_app_idx);
                    vol_cmd_status_mask = 0;
                }
                else if (event_id == EVENT_NOTIFICATION_VOL_STATUS)
                {
                    app_stop_timer(&timer_idx_notification_vol_get_wait_sec_rsp);

                    T_NOTIFICATION_VOL_RELAY_MSG *rcv_msg = (T_NOTIFICATION_VOL_RELAY_MSG *) &p_info[4];

                    app_audio_notification_vol_set_remote_data(rcv_msg);
                    app_audio_notification_vol_report_info();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_IS_NOTIFICATION_VOL_ADJ_FROM_PHONE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    uint8_t new_fg = (p_info[0] & 0x40) >> 6;

                    if (app_cfg_nv.is_notification_vol_set_from_phone != new_fg)
                    {
                        app_cfg_nv.is_notification_vol_set_from_phone = new_fg;
                        app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);
                    }
                }
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_SYNC_PLAY_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;

                APP_PRINT_TRACE1("app_audio_parse_cback: Receive play sync status, para = %x", p_info[0]);
                app_audio_set_avrcp_status(p_info[0]);
#if F_APP_LISTENING_MODE_SUPPORT
                app_listening_judge_a2dp_event(APPLY_LISTENING_MODE_AVRCP_PLAY_STATUS_CHANGE);
#endif
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_ABS_VOL_STATE:
        if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
            status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
            status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
            status == REMOTE_RELAY_STATUS_SEND_FAILED)
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t bd_addr[6];
            uint8_t pair_idx_mapping;
            T_APP_BR_LINK *p_link;
            T_APP_ABS_VOL_STATE abs_vol_state = ABS_VOL_NOT_SUPPORTED;

            memcpy(bd_addr, &p_info[0], 6);
            abs_vol_state = (T_APP_ABS_VOL_STATE)p_info[6];

            if (abs_vol_state == ABS_VOL_SUPPORTED)
            {
                app_avrcp_stop_abs_vol_check_timer();
            }

            if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping) == false)
            {
                APP_PRINT_ERROR0("app_audio_parse_cback: abs vol state sync fail, cannot find pair index");
                return;
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_cfg_nv.audio_gain_level[pair_idx_mapping] = p_info[7];
            }

            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                p_link->abs_vol_state = abs_vol_state;
                app_audio_vol_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
                app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_TONE_STOP:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_APP_AUDIO_TONE_TYPE *tone_type = (T_APP_AUDIO_TONE_TYPE *)buf;

                if (app_audio_get_tone_index(*tone_type) == app_db.tone_vp_status.index)
                {
                    app_audio_tone_type_stop();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_GAMING_MODE_SET_ON:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) || (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];

                memcpy(bd_addr, &p_info[0], 6);

                app_bt_sniffing_set_nack_flush_param(bd_addr);
            }
        }
        break;

    case APP_REMOTE_MSG_GAMING_MODE_SET_OFF:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) || (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT))
            {
                bt_sniffing_set_a2dp_dup_num(false, 0, false);
            }
        }
        break;

    case APP_REMOTE_MSG_AUDIO_CHANNEL_CHANGE:
        {

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.remote_spk_channel = p_info[0];
                    app_report_rws_bud_info();
                }
            }

        }
        break;

    case APP_REMOTE_MSG_SYNC_MIC_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_audio_set_mic_mute_status(p_info[0]);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_PLAYBACK_MUTE_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    uint8_t bd_addr[6];
                    uint8_t pair_idx_mapping;
                    T_APP_BR_LINK *p_link;

                    memcpy(bd_addr, &p_info[0], 6);

                    if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping) == false)
                    {
                        APP_PRINT_ERROR0("app_audio_parse_cback: playback mute status sync fail, cannot find pair index");
                        return;
                    }

                    p_link = app_link_find_br_link(bd_addr);

                    if (p_link != NULL)
                    {
                        if (app_cfg_const.enable_rtk_charging_box)
                        {
                            //One bud connected to phone in box and playing,
                            //the other bud into the box at first, then out of the box power on and engaged,
                            //the bud outside box is silent, so need to do unmute action
                            if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
                            {
                                p_link->playback_muted = false;
                            }
                            else
                            {
                                p_link->playback_muted = p_info[6];
                            }
                        }
                        else
                        {
                            p_link->playback_muted = p_info[6];
                        }

                        APP_PRINT_INFO1("app_audio_parse_cback: playback_muted %d", p_link->playback_muted);

                        if (p_link->playback_muted)
                        {
                            audio_track_volume_out_mute(p_link->a2dp_track_handle);
                        }
                        else
                        {
                            audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        }
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_VOICE_MUTE_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    uint8_t bd_addr[6];
                    uint8_t pair_idx_mapping;
                    T_APP_BR_LINK *p_link;

                    memcpy(bd_addr, &p_info[0], 6);

                    if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping) == false)
                    {
                        APP_PRINT_ERROR0("app_audio_parse_cback: voice mute status sync fail, cannot find pair index");
                        return;
                    }

                    p_link = app_link_find_br_link(bd_addr);

                    if (p_link != NULL)
                    {
                        if (app_cfg_const.enable_rtk_charging_box)
                        {
                            //One bud connected to phone in box and call active,
                            //the other bud into the box at first, then out of the box power on and engaged,
                            //the bud outside box is silent, so need to do unmute action
                            if ((app_db.local_loc != BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
                            {
                                p_link->voice_muted = false;
                            }
                            else
                            {
                                p_link->voice_muted = p_info[6];
                            }
                        }
                        else
                        {
                            p_link->voice_muted = p_info[6];
                        }

                        APP_PRINT_INFO1("app_audio_parse_cback: voice_muted %d", p_link->voice_muted);

                        if (p_link->voice_muted)
                        {
                            audio_track_volume_out_mute(p_link->sco_track_handle);
                        }
                        else
                        {
                            audio_track_volume_out_unmute(p_link->sco_track_handle);
                        }
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_CALL_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;

                app_hfp_set_call_status((T_APP_CALL_STATUS)p_info[0]);

#if F_APP_LISTENING_MODE_SUPPORT
                app_listening_mode_call_status_update();
#endif
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_VP_LANGUAGE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    if (voice_prompt_supported_languages_get() & BIT(p_info[0]))
                    {
                        bool need_to_save_to_flash = false;

                        if (p_info[0] != app_cfg_nv.voice_prompt_language)
                        {
                            need_to_save_to_flash = true;
                        }

                        app_cfg_nv.voice_prompt_language = p_info[0];
                        voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language);

                        if (need_to_save_to_flash)
                        {
                            app_cfg_store(&app_cfg_nv.voice_prompt_language, 1);
                        }
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_LOW_LATENCY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;

                    if (app_db.gaming_mode != p_info[0])
                    {
                        app_mmi_switch_gaming_mode();
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_BUD_STREAM_STATE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                T_APP_BUD_STREAM_STATE relay_bud_stream_state = (T_APP_BUD_STREAM_STATE)p_info[0];
                app_audio_set_bud_stream_state(relay_bud_stream_state);
            }
        }
        break;

    case APP_REMOTE_MSG_DEFAULT_CHANNEL:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.remote_default_channel = p_info[0];
                }
            }
        }
        break;

#if F_APP_COMMON_DONGLE_SUPPORT
    case APP_REMOTE_MSG_SYNC_REMOTE_IS_DONGLE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.remote_is_dongle = p_info[0];
                }
            }
        }
        break;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    case APP_REMOTE_MSG_SYNC_GAMING_MODE_REQUEST:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.gaming_mode_request_is_received = p_info[0];
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_DONGLE_IS_ENABLE_MIC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.dongle_is_enable_mic = p_info[0];
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_DONGLE_IS_DISABLE_APT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.dongle_is_disable_apt = p_info[0];
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_DISALLOW_PLAY_GAMING_MODE_VP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.disallow_play_gaming_mode_vp = p_info[0];
                }
            }
        }
        break;
#endif
    case APP_REMOTE_MSG_SYNC_MP_DUAL_MIC_SETTING:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_audio_set_mp_dual_mic_setting(p_info[0]);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD
                || status == REMOTE_RELAY_STATUS_SYNC_EXPIRED
                || status == REMOTE_RELAY_STATUS_SYNC_TOUT
                || status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                uint8_t *p_info = (uint8_t *)buf;
                T_APP_BR_LINK *p_link = &app_db.br_link[active_a2dp_idx];
                app_eq_index_set(SPK_SW_EQ, app_db.spk_eq_mode, p_info[0]);
#if F_APP_LEA_SUPPORT
                app_audio_policy_media_eq_enable(p_link);
#else
                if (p_link)
                {
                    app_eq_audio_eq_enable(&p_link->eq_instance, &p_link->audio_eq_enabled);
                }
#endif
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_LAST_CONN_INDEX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_bt_policy_b2s_connected_set_last_conn_index(p_info[0]);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_GAMING_RECORD_EQ_INDEX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_cfg_nv.eq_idx_gaming_mode_record = p_info[0];
                    app_eq_idx_check_accord_mode();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_NORMAL_RECORD_EQ_INDEX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_cfg_nv.eq_idx_normal_mode_record = p_info[0];
                    app_eq_idx_check_accord_mode();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_ANC_RECORD_EQ_INDEX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_cfg_nv.eq_idx_anc_mode_record = p_info[0];
                    app_eq_idx_check_accord_mode();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_LOW_LATENCY_LEVEL:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    T_APP_BR_LINK *p_link = NULL;
                    T_AUDIO_FORMAT_INFO format;
                    uint16_t latency = 0;
                    uint8_t *p_info = (uint8_t *)buf;
                    uint8_t latency_level = (p_info[0] & 0x1C) >> 2;

                    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        p_link = &app_db.br_link[i];

                        if (p_link && p_link->a2dp_track_handle)
                        {
                            T_AUDIO_STREAM_MODE  track_mode;
                            if (audio_track_mode_get(p_link->a2dp_track_handle, &track_mode) == true)
                            {
                                if (track_mode == AUDIO_STREAM_MODE_LOW_LATENCY ||
                                    track_mode == AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY)
                                {
                                    audio_track_format_info_get(p_link->a2dp_track_handle, &format);
                                    app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, format.type, latency_level,
                                                                         &latency);
#if F_APP_GAMING_DONGLE_SUPPORT
                                    if (app_db.remote_is_dongle)
                                    {
                                        uint16_t latency_value = app_dongle_get_gaming_latency();
                                        app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);
                                    }
                                    else
#endif
                                    {
                                        audio_track_latency_set(p_link->a2dp_track_handle, latency, GAMING_MODE_DYNAMIC_LATENCY_FIX);
                                    }
                                }
                            }
                            app_db.last_gaming_mode_audio_track_latency = latency;
                            break;
                        }
                    }

                    app_cfg_nv.rws_low_latency_level_record = (p_info[0] & 0x1C) >> 2;
                    app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_SW_EQ_TYPE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_db.sw_eq_type = p_info[0];
                }
            }
        }
        break;

#if F_APP_POWER_ON_DELAY_APPLY_APT_SUPPORT
    case APP_REMOTE_MSG_SYNC_APT_POWER_ON_DELAY_APPLY_TIME:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    uint8_t delay_time = (p_info[0] & 0xF8) >> 3;

                    /* valid value: 0 ~ 31s */
                    if ((delay_time < 32) &&
                        (delay_time != app_cfg_nv.time_delay_to_open_apt_when_power_on))
                    {
                        app_cfg_nv.time_delay_to_open_apt_when_power_on = delay_time;
                        app_cfg_store(&app_cfg_nv.offset_xtal_k_result, 1);
                    }
                }
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_SYNC_VOL_IS_CHANGED_BY_KEY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    app_key_set_volume_status((bool)p_info[0]);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_A2DP_PLAY_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_db.a2dp_play_status = p_info[0];
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_EITHER_BUD_HAS_VOL_CTRL_KEY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t has_vol_ctrl_key = *(uint8_t *)buf;

                if (has_vol_ctrl_key)
                {
                    if (app_cfg_nv.either_bud_has_vol_ctrl_key == false)
                    {
                        app_cfg_nv.either_bud_has_vol_ctrl_key = true;
                        app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_FORCE_JOIN_SET:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                force_join = (bool)p_info[0];
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_CONNECTED_TONE_NEED:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_db.connected_tone_need = (bool)p_info[0];
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_SUSPEND_A2DP_BY_OUT_EAR:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_db.detect_suspend_by_out_ear = (bool)p_info[0];
                APP_PRINT_INFO1("APP_REMOTE_MSG_SYNC_SUSPEND_A2DP_BY_OUT_EAR: flag %d",
                                app_db.detect_suspend_by_out_ear);
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_IN_EAR_RECOVER_A2DP:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_audio_update_in_ear_recover_a2dp(p_info[0]);
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_CMD_SET_VOLUME:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t curr_volume;
                T_AUDIO_STREAM_TYPE stream_type;

                stream_type = (T_AUDIO_STREAM_TYPE)p_info[0];
                curr_volume = p_info[1];

                audio_volume_out_set(stream_type, curr_volume);
                app_audio_cmd_set_volume_report_status(p_info[2], p_info[3]);
            }
        }
        break;

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    case APP_REMOTE_MSG_SYNC_ALLOWED_SOURCE:
        {
            if (!app_cfg_const.enable_24g_bt_audio_source_switch)
            {
                break;
            }

            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t tmp = (p_info[0] & 0x80) >> 7;
#if F_APP_LEA_SUPPORT
                uint8_t le_audio_num = app_link_get_lea_link_num();
#endif

                APP_PRINT_TRACE2("Secondary update allowed source: %d, %d", tmp, app_cfg_nv.allowed_source);

                if (tmp != app_cfg_nv.allowed_source)
                {
                    app_cfg_nv.allowed_source = tmp;

                    if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
                    {
#if F_APP_LEA_SUPPORT
                        if (le_audio_num > 0)
                        {
                            app_dongle_source_switch_to_bt_le_audio_handle();
                        }
#endif
                    }
                    else
                    {
#if F_APP_LEA_SUPPORT
                        if (le_audio_num > 0)
                        {
                            app_dongle_source_switch_to_dongle_le_audio_handle();
                        }
#endif
                    }
                }
            }
        }
        break;
#endif

    default:
        break;
    }
#if F_APP_LEA_SUPPORT
    mtc_audio_policy_cback(msg_type, buf, len, status);
#endif
}
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#else
bool app_audio_get_seg_send_status(void)
{
    return seg_send_db.sending_status;
}

static void app_audio_seg_send_init(uint8_t *buf, uint16_t len, uint8_t type)
{
    seg_send_db.sending_status = true;
    seg_send_db.type = type;
    seg_send_db.dsp_data = malloc(len);
    seg_send_db.len = len;
    seg_send_db.offset = 0;
    memcpy(seg_send_db.dsp_data, buf, len);
}

static void app_audio_seg_send_start(void)
{
    uint8_t cmd_buf[SEG_SEND_HEADER_LEN] = {0};

    cmd_buf[0] = AUDIO_PROBE_SEG_SEND;
    cmd_buf[2] = 1;
    cmd_buf[4] = DSP_DATA_START;
    cmd_buf[5] = seg_send_db.type;
    cmd_buf[6] = seg_send_db.len;
    cmd_buf[7] = seg_send_db.len >> 8;

    audio_probe_dsp_send(cmd_buf, SEG_SEND_HEADER_LEN);
}

static void app_audio_seg_send_data(uint8_t *buf, uint16_t seg_len)
{
    uint8_t cmd_buf[SEG_SEND_MAX_LEN] = {0};

    cmd_buf[0] = AUDIO_PROBE_SEG_SEND;
    cmd_buf[2] = (4 + seg_len + 3) >> 2;
    cmd_buf[4] = DSP_DATA_SEND;
    cmd_buf[5] = seg_send_db.type;
    cmd_buf[6] = seg_len;
    cmd_buf[7] = seg_len >> 8;

    memcpy_s(cmd_buf + SEG_SEND_HEADER_LEN, (SEG_SEND_MAX_LEN - SEG_SEND_HEADER_LEN), buf, seg_len);

    audio_probe_dsp_send(cmd_buf, (cmd_buf[2] + 1) << 2);
}

static void app_audio_seg_send_finish(void)
{
    uint8_t cmd_buf[SEG_SEND_HEADER_LEN] = {0};

    cmd_buf[0] = AUDIO_PROBE_SEG_SEND;
    cmd_buf[2] = 1;
    cmd_buf[4] = DSP_DATA_FINISH;
    cmd_buf[5] = seg_send_db.type;

    audio_probe_dsp_send(cmd_buf, SEG_SEND_HEADER_LEN);
}
#endif

#if (F_APP_VENDOR_CMD_SUPPORT & 0)
void app_audio_cmd_cback(uint8_t msg_type, uint8_t *buf, uint16_t len)
{
    switch (msg_type)
    {
    case APP_AUDIO_CMD_MSG_SEND_DSP_CMD:
        {
            audio_probe_dsp_send(buf, len);
        }
        break;

    case APP_AUDIO_CMD_MSG_SEND_DSP_DATA:
        {
            app_audio_seg_send_init(buf, len, DSP_DATA_TYPE_HA_VENDOR);
            app_audio_seg_send_start();
        }
        break;

    default:
        break;
    }

    return;
}
#endif

#if F_APP_BUD_LOCATION_SUPPORT
static uint8_t app_audio_out_ear_action(uint8_t cause_action)
{
    uint8_t action = APP_AUDIO_RESULT_NOTHING;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (app_hfp_sco_is_need())
        {
            if (!app_cfg_const.disallow_auto_transfer_to_phone_when_out_of_ear)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (app_db.local_loc != BUD_LOC_IN_EAR && app_db.remote_loc != BUD_LOC_IN_EAR)
                    {
                        action = APP_AUDIO_RESULT_VOICE_TO_AG;
                    }
                }
                else if (app_db.local_loc != BUD_LOC_IN_EAR)
                {
                    action = APP_AUDIO_RESULT_VOICE_TO_AG;
                }
            }
        }
        else if ((app_audio_get_bud_stream_state() == BUD_STREAM_STATE_IDLE) ||
                 (app_audio_get_bud_stream_state() == BUD_STREAM_STATE_AUDIO))
        {
            if ((app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                (app_db.detect_suspend_by_out_ear == false))
            {
                if (app_db.br_link[app_a2dp_get_active_idx()].waiting_play_status_change == false)
                {
                    action = APP_AUDIO_RESULT_PAUSE;
                }
                else
                {
                    app_start_timer(&timer_idx_waiting_play_status, "waiting_playing_status",
                                    audio_policy_timer_id, APP_TIMER_WAITING_PLAY_STATUS, app_a2dp_get_active_idx(), false,
                                    WAITING_PLAY_STATUS_TIMEOUT);
                    action = APP_AUDIO_RESULT_NOTHING;
                }
            }

            if (cause_action == CAUSE_ACTION_NON)
            {
                action = APP_AUDIO_RESULT_NOTHING;
            }
        }
    }
    return action;
}

static uint8_t app_audio_in_ear_action(bool from_remote, uint8_t cause_action)
{
    uint8_t action = APP_AUDIO_RESULT_NOTHING;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (app_hfp_sco_is_need())
        {
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                if ((from_remote && (app_db.local_loc != BUD_LOC_IN_EAR)) || (!from_remote &&
                                                                              (app_db.remote_loc != BUD_LOC_IN_EAR)))
                {
                    action = APP_AUDIO_RESULT_VOICE_TO_BUD;
                }
            }
            else
            {
                action = APP_AUDIO_RESULT_VOICE_TO_BUD;
            }
        }
        else if ((app_audio_get_bud_stream_state() == BUD_STREAM_STATE_IDLE) ||
                 (app_audio_get_bud_stream_state() == BUD_STREAM_STATE_AUDIO))
        {
            if ((app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED) ||
                (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED))
            {
                uint8_t idx = app_a2dp_get_active_idx();

                if (app_db.br_link[idx].waiting_paused_status_change == true)
                {
                    //play status changed msg hasn't come, waiting
                    action = APP_AUDIO_RESULT_NOTHING;
                    app_start_timer(&timer_idx_waiting_paused_status, "waiting_paused_status",
                                    audio_policy_timer_id, APP_TIMER_WAITING_PAUSED_STATUS, idx, false,
                                    WAITING_PAUSED_STATUS_TIMEOUT);
                }
                else if (app_db.in_ear_recover_a2dp)
                {
                    action = APP_AUDIO_RESULT_RESUME;
                    app_audio_update_in_ear_recover_a2dp(false);
                    app_db.resume_a2dp_by_in_ear = true;
                }
            }

            if (cause_action == CAUSE_ACTION_NON)
            {
                action = APP_AUDIO_RESULT_NOTHING;
            }
        }
    }
    return action;
}

static uint8_t app_audio_out_case_action(void)
{
    uint8_t action = APP_AUDIO_RESULT_NOTHING;

    if (app_cfg_const.enable_rtk_charging_box)
    {
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            if (app_hfp_sco_is_need())
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (!(app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE))
                    {
                        action = APP_AUDIO_RESULT_VOICE_TO_BUD;
                    }
                }
                else
                {
                    action = APP_AUDIO_RESULT_VOICE_TO_BUD;
                }
            }
        }
    }

    return action;
}

static uint8_t app_audio_in_case_action(void)
{
    uint8_t action = APP_AUDIO_RESULT_NOTHING;

    if (app_cfg_const.enable_rtk_charging_box)
    {
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            if ((app_db.local_loc == BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
            {

                if (app_hfp_sco_is_need())
                {
                    action = APP_AUDIO_RESULT_VOICE_TO_AG;
                }
                else if ((app_audio_get_bud_stream_state() == BUD_STREAM_STATE_IDLE) ||
                         (app_audio_get_bud_stream_state() == BUD_STREAM_STATE_AUDIO))
                {
                    if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        action = APP_AUDIO_RESULT_STOP;
                    }
                }
            }
        }
    }

    return action;
}

void app_audio_action_when_bud_loc_changed(uint8_t action)
{
    uint8_t play_status;
    play_status = app_db.avrcp_play_status;

    APP_PRINT_TRACE7("app_audio_action_when_bud_loc_changed: action 0x%x, bud_role %d, play_status 0x%x, roleswap_status 0x%02x, detect_suspend %d, in_ear_recover %d resume_by_in_ear %d",
                     action, app_cfg_nv.bud_role, play_status, app_roleswap_ctrl_get_status(),
                     app_db.detect_suspend_by_out_ear, app_db.in_ear_recover_a2dp, app_db.resume_a2dp_by_in_ear);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        || (app_roleswap_ctrl_get_status() != APP_ROLESWAP_STATUS_IDLE))
    {
        return;
    }

    switch (action)
    {
    case APP_AUDIO_RESULT_PAUSE:
        {
            if (play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
                app_audio_update_detect_suspend_by_out_ear(true);
            }
        }
        break;

    case APP_AUDIO_RESULT_STOP:
        {
            if (play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
            }
        }
        break;

    case APP_AUDIO_RESULT_RESUME:
        {
            if (play_status == BT_AVRCP_PLAY_STATUS_PAUSED || play_status == BT_AVRCP_PLAY_STATUS_STOPPED)
            {
                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
            }
        }
        break;

    case APP_AUDIO_RESULT_VOICE_TO_AG:
        {
            if (app_hfp_sco_is_connected())
            {
                app_mmi_handle_action(MMI_HF_TRANSFER_CALL);
            }
        }
        break;

    case APP_AUDIO_RESULT_VOICE_TO_BUD:
        {
            if (!app_hfp_sco_is_connected())
            {
                app_mmi_handle_action(MMI_HF_TRANSFER_CALL);
            }
        }
        break;

    case APP_AUDIO_RESULT_NOTHING:
        {
        }
        break;

    default:
        {
        }
        break;
    }
}

void app_audio_update_detect_suspend_by_out_ear(bool flag)
{

    app_db.detect_suspend_by_out_ear = flag;
    APP_PRINT_INFO1("app_audio_update_detect_suspend_by_out_ear: flag %d", flag);
#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_SUSPEND_A2DP_BY_OUT_EAR);
    }
#endif
}

void app_audio_update_in_ear_recover_a2dp(bool flag)
{
    if (app_cfg_const.restore_a2dp_when_bud_in_ear_in_15s)
    {
        if (flag)
        {
            app_start_timer(&timer_idx_in_ear_restore_a2dp, "in_ear_restore_a2dp",
                            audio_policy_timer_id, APP_TIMER_IN_EAR_RESTORE_A2DP, 0, false,
                            15000);
        }
        else
        {
            app_stop_timer(&timer_idx_in_ear_restore_a2dp);
        }
    }

    app_db.in_ear_recover_a2dp = flag;

#if F_APP_ERWS_SUPPORT
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_IN_EAR_RECOVER_A2DP);
    }
#endif
}

static void app_audio_bud_loc_event_cback(uint32_t event, void *msg)
{
    bool handle = true;
    uint8_t action = 0;
    uint8_t *p_info = msg;
    bool from_remote = p_info[0];

#if F_APP_LEA_SUPPORT
    if (mtc_get_btmode())
    {
        return;
    }
#endif

#if (F_APP_SENSOR_SUPPORT == 1)
    uint8_t cause_action = p_info[1];
    //clear action flag after sync
    app_bud_loc_cause_action_flag_set(false);
#endif

    /*loc changed action and spk mute/unmte check is done by durian*/
#if F_APP_DURIAN_SUPPORT
    if (app_durian_loc_get_action(&action, event))
    {
        handle = true;
        goto bud_loc_action_exe;
    }
#else
    if (app_db.local_loc == BUD_LOC_IN_CASE)
    {
        app_audio_spk_mute_unmute(true);
    }
    else
    {
        app_audio_spk_mute_unmute(false);
    }
#endif

    switch (event)
    {
#if (F_APP_SENSOR_SUPPORT == 1)
    case EVENT_OUT_EAR:
        {
            action = app_audio_out_ear_action(cause_action);
        }
        break;

    case EVENT_IN_EAR:
        {
            if ((!from_remote) && app_sensor_is_play_in_ear_tone())
            {
                if (app_cfg_const.play_in_ear_tone_regardless_of_phone_connection ||
                    (app_link_get_b2s_link_num() > 0))
                {
                    app_audio_tone_type_play(TONE_IN_EAR_DETECTION, false, false);
                }
            }

            action = app_audio_in_ear_action(from_remote, cause_action);
        }
        break;
#endif

    case EVENT_OUT_CASE:
        {
            action = app_audio_out_case_action();
        }
        break;

    case EVENT_IN_CASE:
        {
            action = app_audio_in_case_action();
        }
        break;

    default:
        handle = false;
        break;
    }

#if F_APP_DURIAN_SUPPORT
bud_loc_action_exe:
#endif
    if (handle == true)
    {
        APP_PRINT_TRACE2("app_audio_bud_loc_event_cback: event_type 0x%02x, action 0x%04x", event,
                         action);
        app_audio_action_when_bud_loc_changed(action);
    }
}
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
void app_audio_probe_cback(T_AUDIO_PROBE_EVENT event, void *buf)
{
    APP_PRINT_INFO1("app_audio_probe_cback: event 0x%04x", event);

    switch (event)
    {
    case PROBE_SCENARIO_STATE:
        {
            uint8_t app_idx;
            uint32_t *event_buff = (uint32_t *)buf;

            for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
            {
                if (app_db.br_link[app_idx].connected_profile & SPP_PROFILE_MASK)
                {
                    app_report_event(CMD_PATH_SPP, EVENT_AUDIO_DSP_CTRL_INFO, app_idx, (uint8_t *)event_buff, 4);
                }
                else if (app_db.br_link[app_idx].connected_profile & IAP_PROFILE_MASK)
                {
                    app_report_event(CMD_PATH_IAP, EVENT_AUDIO_DSP_CTRL_INFO, app_idx, (uint8_t *)event_buff, 4);
                }
            }
        }
        break;

#if 0
    case PROBE_SEG_SEND_REQ_DATA:
        {
            uint16_t seg_len = seg_send_db.len - seg_send_db.offset;

            if (seg_len > SEG_SEND_MAX_PAYLOAD_LEN)
            {
                seg_len = SEG_SEND_MAX_PAYLOAD_LEN;
            }

            if (seg_len == 0)
            {
                app_audio_seg_send_finish();
                free(seg_send_db.dsp_data);
                seg_send_db.sending_status = false;
            }
            else
            {
                app_audio_seg_send_data(seg_send_db.dsp_data + seg_send_db.offset, seg_len);
                seg_send_db.offset += seg_len;
            }
        }
        break;

    case PROBE_SEG_SEND_ERROR:
        {
            free(seg_send_db.dsp_data);
            seg_send_db.sending_status = false;
        }
        break;
#endif

    default:
        break;
    }
}
#endif

void app_audio_init(void)
{
    audio_mgr_cback_register(app_audio_policy_cback);
    bt_mgr_cback_register(app_audio_bt_cback);
    app_timer_reg_cb(app_audio_policy_timeout_cb, &audio_policy_timer_id);

#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_audio_relay_cback, app_audio_parse_cback,
                             APP_MODULE_TYPE_AUDIO_POLICY, APP_AUDIO_REMOTE_MSG_TOTAL);
#endif

#if F_APP_BUD_LOCATION_SUPPORT
    app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_audio_bud_loc_event_cback);
#endif

#if (F_APP_VENDOR_CMD_SUPPORT & 0)
    app_cmd_cback_register(app_audio_cmd_cback, APP_CMD_MODULE_TYPE_AUDIO_POLICY);
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
    audio_probe_dsp_cback_register(app_audio_probe_cback);
#endif

    app_audio_route_gain_init();
    app_eq_init();

#if F_APP_SAIYAN_EQ_FITTING
    app_eq_fitting_init();
#endif

    voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language);

    audio_volume_out_max_set(AUDIO_STREAM_TYPE_PLAYBACK, app_dsp_cfg_vol.playback_volume_max);
    audio_volume_out_max_set(AUDIO_STREAM_TYPE_VOICE, app_dsp_cfg_vol.voice_out_volume_max);
    audio_volume_in_max_set(AUDIO_STREAM_TYPE_VOICE, app_dsp_cfg_vol.voice_volume_in_max);
    audio_volume_in_max_set(AUDIO_STREAM_TYPE_RECORD, app_dsp_cfg_vol.record_volume_max);
    audio_volume_out_min_set(AUDIO_STREAM_TYPE_PLAYBACK, app_dsp_cfg_vol.playback_volume_min);
    audio_volume_out_min_set(AUDIO_STREAM_TYPE_VOICE, app_dsp_cfg_vol.voice_out_volume_min);
    audio_volume_in_min_set(AUDIO_STREAM_TYPE_VOICE, app_dsp_cfg_vol.voice_volume_in_min);
    audio_volume_in_min_set(AUDIO_STREAM_TYPE_RECORD, app_dsp_cfg_vol.record_volume_min);

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    if (!app_cfg_nv.is_notification_vol_set_from_phone)
#endif
    {
        app_cfg_nv.voice_prompt_volume_out_level = app_dsp_cfg_vol.voice_prompt_volume_default;
        app_cfg_nv.ringtone_volume_out_level = app_dsp_cfg_vol.ringtone_volume_default;
    }

#if HARMAN_OPEN_LR_FEATURE
    harman_set_vp_ringtone_balance(__func__, __LINE__);
#endif

    voice_prompt_volume_max_set(app_dsp_cfg_vol.voice_prompt_volume_max);
    voice_prompt_volume_min_set(app_dsp_cfg_vol.voice_prompt_volume_min);
    voice_prompt_volume_set(app_cfg_nv.voice_prompt_volume_out_level);

    ringtone_volume_max_set(app_dsp_cfg_vol.ringtone_volume_max);
    ringtone_volume_min_set(app_dsp_cfg_vol.ringtone_volume_min);
    ringtone_volume_set(app_cfg_nv.ringtone_volume_out_level);

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    app_db.local_notification_vol_min_level = (voice_prompt_volume_min_get() >
                                               ringtone_volume_min_get())
                                              ? voice_prompt_volume_min_get() : ringtone_volume_min_get();
    app_db.local_notification_vol_max_level = (voice_prompt_volume_max_get() <
                                               ringtone_volume_max_get())
                                              ? voice_prompt_volume_max_get() : ringtone_volume_max_get();
    app_db.remote_notification_vol_min_level = app_db.local_notification_vol_min_level;
    app_db.remote_notification_vol_max_level = app_db.local_notification_vol_max_level;
#endif

#if F_APP_LEA_SUPPORT
    mtc_cback_register(app_audio_mtc_if_handle);
#endif
}

uint8_t app_audio_is_mic_mute(void)
{
    return is_mic_mute;
}

void app_audio_set_mic_mute_status(uint8_t status)
{
    is_mic_mute = status;

#if HARMAN_SUPPORT_MIC_STATUS_RECORD
    // record mic on/off seq and status
    uint8_t seq = app_cfg_nv.mic_switch_seq % 5;
    app_cfg_nv.mic_status_seq[seq] = app_cfg_nv.mic_switch_seq + 1;
    app_cfg_nv.mic_status[seq] = status;
    app_cfg_nv.mic_switch_seq++;

    app_cfg_store(&app_cfg_nv.mic_switch_seq, 12);

    APP_PRINT_INFO2("app_audio_set_mic_mute_status: seq %d, status %d",
                    app_cfg_nv.mic_status_seq[seq], app_cfg_nv.mic_status[seq]);

    APP_PRINT_INFO5("mic on/off record: seq %d, %d, %d,%d ,%d",
                    app_cfg_nv.mic_status_seq[0], app_cfg_nv.mic_status_seq[1],
                    app_cfg_nv.mic_status_seq[2], app_cfg_nv.mic_status_seq[3],
                    app_cfg_nv.mic_status_seq[4]);

    APP_PRINT_INFO5("mic on/off record: status %d, %d, %d,%d ,%d",
                    app_cfg_nv.mic_status[0], app_cfg_nv.mic_status[1],
                    app_cfg_nv.mic_status[2], app_cfg_nv.mic_status[3],
                    app_cfg_nv.mic_status[4]);
#endif
}

#if F_APP_HARMAN_FEATURE_SUPPORT
uint8_t app_audio_mic_switch(uint8_t param)
{
    uint8_t mic_switch_cmd[8];
    memset(mic_switch_cmd, 0, 8);

    if (param)
    {
        mic_switch_mode = param;
    }
    else
    {
        mic_switch_mode++;
    }

    mic_switch_cmd[0] = AUDIO_PROBE_TEST_MODE;
    mic_switch_cmd[2] = 0x1;

    switch (mic_switch_mode)
    {
    case 1:
        {
            mic_switch_cmd[4] = 0x3; // mic test + effect off
            mic_switch_cmd[6] = 0x1; // mic0 test
        }
        break;

    case 2:
        {
            mic_switch_cmd[4] = 0x3; // mic test + effect off
            mic_switch_cmd[6] = 0x2; // mic1 test
        }
        break;

    case 3:
        {
            // normal mode
            mic_switch_mode = 0;
        }
        break;

    default:
        break;
    }

    audio_probe_dsp_send(mic_switch_cmd, 8);

    return mic_switch_mode;
}

T_AUDIO_MP_DUAL_MIC_SETTING app_audio_mp_dual_mic_setting_check(uint8_t *ptr)
{
    T_AUDIO_MP_DUAL_MIC_SETTING result = AUDIO_MP_DUAL_MIC_SETTING_INVALID;
    uint8_t mp_dual_mic_l_pri_enable = ptr[0];
    uint8_t mp_dual_mic_l_sec_enable = ptr[1];
    uint8_t mp_dual_mic_r_pri_enable = ptr[2];
    uint8_t mp_dual_mic_r_sec_enable = ptr[3];

    /*  mp_dual_mic_setting bitmap
        bit0: mp_dual_mic_l_pri_enable, 0: disable; 1:enable.
        bit1: mp_dual_mic_l_sec_enable
        bit2: mp_dual_mic_r_pri_enable
        bit3: mp_dual_mic_r_sec_enable
        bit4~7: mp_dual_mic_setting, refer T_AUDIO_MP_DUAL_MIC_SETTING
    */
    mp_dual_mic_setting = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (ptr[i] == 1)
        {
            mp_dual_mic_setting |= BIT(i);
        }
    }

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        result = AUDIO_MP_DUAL_MIC_SETTING_VALID;
        mp_dual_mic_setting |= (result << 4);
    }
    else
    {
        if ((mp_dual_mic_l_pri_enable || mp_dual_mic_l_sec_enable) &&
            !(mp_dual_mic_r_pri_enable || mp_dual_mic_r_sec_enable))
        {
            if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
            {
                result = AUDIO_MP_DUAL_MIC_SETTING_VALID;
            }
            else
            {
                result = AUDIO_MP_DUAL_MIC_SETTING_ROLE_SWAP;
            }
        }
        else if ((mp_dual_mic_r_pri_enable | mp_dual_mic_r_sec_enable) &&
                 !(mp_dual_mic_l_pri_enable | mp_dual_mic_l_sec_enable))
        {
            if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
            {
                result = AUDIO_MP_DUAL_MIC_SETTING_VALID;
            }
            else
            {
                result = AUDIO_MP_DUAL_MIC_SETTING_ROLE_SWAP;
            }
        }
        else
        {
            //this condition not support
            result = AUDIO_MP_DUAL_MIC_SETTING_INVALID;
        }

        mp_dual_mic_setting |= (result << 4);
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_MP_DUAL_MIC_SETTING);
    }

    APP_PRINT_TRACE2("app_audio_mp_dual_mic_setting_check %x, result = %x", mp_dual_mic_setting,
                     result);

    return result;
}

void app_audio_mp_dual_mic_switch_action(void)
{
    uint8_t mic_switch_param;

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
    {
        if ((mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_R_PRI_ENABLE) &&
            (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_R_SEC_ENABLE))
        {
            //normal
            mic_switch_param = 3;
        }
        else if (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_R_PRI_ENABLE)
        {
            //non swap
            mic_switch_param = 1;
        }
        else if (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_R_SEC_ENABLE)
        {
            //swap
            mic_switch_param = 2;
        }
        else
        {
            //all mute
            mic_switch_param = 0;
        }
    }
    else if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
    {
        if ((mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_L_PRI_ENABLE) &&
            (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_L_SEC_ENABLE))
        {
            //normal
            mic_switch_param = 3;
        }
        else if (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_L_PRI_ENABLE)
        {
            //non swap
            mic_switch_param = 1;
        }
        else if (mp_dual_mic_setting & AUDIO_MP_DUAL_MIC_L_SEC_ENABLE)
        {
            //swap
            mic_switch_param = 2;
        }
        else
        {
            //all mute
            mic_switch_param = 0;
        }
    }

    APP_PRINT_TRACE2("app_audio_mp_dual_mic_switch_action setting = %x, mic_switch_param = %x",
                     mp_dual_mic_setting, mic_switch_param);

    if (mic_switch_param)
    {
        app_mmi_handle_action(MMI_DEV_MIC_UNMUTE);
        app_audio_mic_switch(mic_switch_param);
    }
    else
    {
        app_mmi_handle_action(MMI_DEV_MIC_MUTE);
    }

    mp_dual_mic_setting = 0;

#if F_APP_TEST_SUPPORT
    uint8_t success_param = 0;
    uint8_t active_idx = app_hfp_get_active_idx();

    app_test_report_event(app_db.br_link[active_idx].bd_addr, EVENT_DUAL_MIC_MP_VERIFY, &success_param,
                          1);
#endif

}

uint8_t app_audio_get_mp_dual_mic_setting(void)
{
    return mp_dual_mic_setting;
}

void app_audio_set_mp_dual_mic_setting(uint8_t param)
{
    mp_dual_mic_setting = param;
}
#endif

bool app_audio_check_mic_mute_enable(void)
{
    uint8_t i;
    bool enable_mic_mute = false;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].sco_handle != 0)
        {
            enable_mic_mute = true;
            break;
        }
        else
        {
            enable_mic_mute = false;
        }
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_db.dongle_is_enable_mic)
    {
        enable_mic_mute = true;
    }
#endif

#if F_APP_USB_AUDIO_SUPPORT
    if (app_db.usb_is_enable_mic)
    {
        enable_mic_mute = true;
    }
#endif

    return enable_mic_mute;
}

void app_audio_enable_play_mic_unmute_tone(bool enable)
{
    APP_PRINT_INFO1("app_audio_enable_play_mic_unmute_tone %d", enable);
    enable_play_mic_unmute_tone = enable;
}

bool app_audio_is_enable_play_mic_unmute_tone(void)
{
    return enable_play_mic_unmute_tone;
}

void app_audio_tone_flush(bool relay)
{
    ringtone_flush(relay);
    voice_prompt_flush(relay);
}

#if F_APP_LEA_SUPPORT
void app_audio_enable_play_mic_mute_tone(bool enable)
{
    APP_PRINT_INFO1("app_audio_enable_play_mic_mute_tone %d", enable);
    enable_play_mic_mute_tone = enable;
}

bool app_audio_is_enable_play_mic_mute_tone(void)
{
    return enable_play_mic_mute_tone;
}
#endif

bool app_audio_tone_type_cancel(T_APP_AUDIO_TONE_TYPE tone_type, bool relay)
{
    bool ret = false;
    uint8_t tone_index = TONE_INVALID_INDEX;

    tone_index = app_audio_get_tone_index(tone_type);

    if (tone_index < VOICE_PROMPT_INDEX)
    {
        ret = ringtone_cancel(tone_index, relay);
    }
    else if (tone_index < TONE_INVALID_INDEX)
    {
        ret = voice_prompt_cancel(tone_index - VOICE_PROMPT_INDEX, relay);
    }

    APP_PRINT_TRACE4("app_audio_tone_type_cancel: type 0x%02x, index 0x%02x, realy %d, ret %d",
                     tone_type,
                     tone_index, relay, ret);

    return ret;
}

static int8_t app_audio_tone_play_check(T_APP_AUDIO_TONE_TYPE tone_type, uint8_t tone_index)
{
    int8_t ret = 0;

#if F_APP_OTA_SUPPORT
    bool is_ota_mode = app_ota_mode_check();
    if (is_ota_mode)
    {
        // Notification is deinit when OTA due to ram size issue,
        // It is forbidden to play tone during OTA processing
        ret = 1;
        goto exit;
    }
#endif

    if (app_db.device_state == APP_DEVICE_STATE_OFF_ING)
    {
#if F_APP_HARMAN_FEATURE_SUPPORT
        if (((app_cfg_nv.language_status == 0) &&
             ((tone_type != TONE_HARMAN_POWER_OFF) && (tone_type != TONE_HARMAN_FACTORY_RESET)))
            || ((app_cfg_nv.language_status == 1) &&
                ((tone_type != TONE_POWER_OFF) && (tone_type != TONE_FACTORY_RESET))))
#else
        if ((tone_type != TONE_POWER_OFF) && (tone_type != TONE_FACTORY_RESET))
#endif
        {
            ret = 2;
            goto exit;
        }
    }

    if ((app_db.tone_vp_status.state == APP_TONE_VP_STARTED)
        && (app_db.tone_vp_status.index == tone_index)
        && (tone_type == TONE_KEY_SHORT_PRESS))
    {
        ret = 3;
        goto exit;
    }

exit:

    return -ret;
}

uint8_t app_audio_get_tone_index(T_APP_AUDIO_TONE_TYPE tone_type)
{
    uint8_t tone_index = TONE_INVALID_INDEX;
    uint8_t *tone_index_ptr = NULL;

    if (tone_type < TONE_CIS_LOST)
    {
        // T_APP_CFG_CONST Tone1: 128 bytes
        tone_index_ptr = &app_cfg_const.tone_power_on;
        tone_index = *(tone_index_ptr + (tone_type - TONE_POWER_ON));
    }
    else if (tone_type < TONE_CIS_LOST + 32)
    {
        // T_APP_CFG_CONST Tone2: 32 bytes
        tone_index_ptr = &app_cfg_const.tone_cis_loss;
        tone_index = *(tone_index_ptr + (tone_type - TONE_CIS_LOST));
    }

    return tone_index;
}

/* NOTICE: Make sure T_APP_AUDIO_TONE_TYPE is align to app_cfg_const.tone_xxxx offset */
bool app_audio_tone_type_play(T_APP_AUDIO_TONE_TYPE tone_type, bool flush, bool relay)
{
    bool ret = false;
    uint8_t tone_index = TONE_INVALID_INDEX;
    int8_t check_result = 0;

#if HARMAN_OPEN_LR_FEATURE
    harman_set_vp_ringtone_balance(__func__, __LINE__);
#endif

#if F_APP_SAIYAN_MODE
    if ((data_capture_saiyan.saiyan_enable) && (tone_type != TONE_POWER_ON))
    {
        APP_PRINT_TRACE1("app_audio_tone_type_play: disallow 0x%x", tone_type);
        return true;
    }
#endif

#if F_APP_DONGLE_MULTI_PAIRING
    if (app_cfg_const.enable_dongle_multi_pairing)
    {
        if (tone_type == TONE_PAIRING && app_cfg_nv.is_bt_pairing == false)
        {
            tone_type = TONE_PAIRING_2_4G;
        }
    }
#endif

    tone_index = app_audio_get_tone_index(tone_type);
    check_result = app_audio_tone_play_check(tone_type, tone_index);

    APP_PRINT_INFO6("app_audio_tone_type_play: tone_type 0x%02x, tone_index 0x%02x, state=%d, index=0x%02x, flush=%d, check_result = %d",
                    tone_type,
                    tone_index,
                    app_db.tone_vp_status.state,
                    app_db.tone_vp_status.index,
                    flush,
                    check_result);

    if (check_result != 0)
    {
        return ret;
    }

#if F_APP_TEAMS_FEATURE_SUPPORT
    if (app_db.device_state == APP_DEVICE_STATE_OFF)
    {
        if ((tone_type == TONE_MIC_MUTE_ON) || (tone_type == TONE_MIC_MUTE_OFF))
        {
            return ret;
        }
    }
#endif

    if (tone_index < VOICE_PROMPT_INDEX)
    {
        if (flush)
        {
            ringtone_cancel(tone_index, true);
        }
        ret = ringtone_play(tone_index, relay);
    }
    else if (tone_index < TONE_INVALID_INDEX)
    {
        if (flush)
        {
            voice_prompt_cancel(tone_index - VOICE_PROMPT_INDEX, true);
        }
        ret = voice_prompt_play(tone_index - VOICE_PROMPT_INDEX,
                                (T_VOICE_PROMPT_LANGUAGE_ID)app_cfg_nv.voice_prompt_language,
                                relay);
    }

    return ret;
}

bool app_audio_tone_type_stop(void)
{
    bool ret = false;

    APP_PRINT_TRACE1("app_audio_tone_type_stop: tone_index 0x%02x", app_db.tone_vp_status.index);

    if (app_db.tone_vp_status.index < VOICE_PROMPT_INDEX)
    {
        ret = ringtone_stop();
    }
    else if (app_db.tone_vp_status.index < TONE_INVALID_INDEX)
    {
        ret = voice_prompt_stop();
    }

    return ret;
}

#if F_APP_ERWS_SUPPORT
bool app_audio_tone_stop_sync(T_APP_AUDIO_TONE_TYPE tone_type)
{
    bool ret = false;
    uint8_t tone_index = TONE_INVALID_INDEX;
    int8_t check_result = 0;

    tone_index = app_audio_get_tone_index(tone_type);
    check_result = app_audio_tone_play_check(tone_type, tone_index);

    APP_PRINT_TRACE4("app_audio_tone_stop_sync: tone_type 0x%02x, tone_index 0x%02x, index 0x%02x, check_result %d",
                     tone_type, tone_index, app_db.tone_vp_status.index, check_result);

    if (tone_index != app_db.tone_vp_status.index ||
        check_result != 0)
    {
        return ret;
    }

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                           APP_REMOTE_MSG_SYNC_TONE_STOP,
                                           (uint8_t *)&tone_type, 1, REMOTE_TIMER_HIGH_PRECISION, 0, false);
    }

    return ret;
}
#endif

void app_audio_speaker_channel_set(T_AUDIO_CHANCEL_SET set_mode, uint8_t spk_channel)
{
    uint8_t channel;

    APP_PRINT_TRACE3("app_audio_speaker_channel_set: set_mode %d, spk_channel %d, specific spk_channel %d",
                     set_mode, app_cfg_nv.spk_channel, spk_channel);

    switch (set_mode)
    {
    case AUDIO_COUPLE_SET:
        {
            channel = (app_cfg_nv.spk_channel == 0) ? app_cfg_const.couple_speaker_channel :
                      app_cfg_nv.spk_channel;
        }
        break;

    case AUDIO_SINGLE_SET:
        {
            channel = app_cfg_const.solo_speaker_channel;
        }
        break;

    case AUDIO_SPECIFIC_SET:
        {
            channel = spk_channel;
        }
        break;

    default:
        channel = 0;
        break;
    }

#if F_APP_ERWS_SUPPORT
    if ((set_mode != AUDIO_SINGLE_SET) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        app_cfg_nv.spk_channel = channel;
        app_cfg_store(&app_cfg_nv.spk_channel, 1);

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_AUDIO_CHANNEL_CHANGE);
        }
    }
#endif

    audio_volume_out_channel_set((T_AUDIO_VOLUME_CHANNEL_MASK)channel);
}

void app_audio_set_max_min_vol_from_phone_flag(bool status)
{
    is_max_min_vol_from_phone = status;
}

void app_audio_vol_set(T_AUDIO_TRACK_HANDLE track_handle, uint8_t vol)
{
    APP_PRINT_INFO4("app_audio_vol_set: local_in_case %d, voice_muted %d, playback_muted %d, vol %d",
                    app_db.local_in_case, app_db.voice_muted, app_db.playback_muted, vol);

    if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
        (app_hfp_get_call_status() == APP_CALL_INCOMING))
    {
        app_hfp_inband_tone_mute_ctrl();
    }
    else
    {
#if F_APP_IPHONE_ABS_VOL_HANDLE
        T_AUDIO_STREAM_TYPE volume_type;

        if (audio_track_stream_type_get(track_handle, &volume_type) == false)
        {
            app_key_set_volume_status(false);
            return;
        }

        if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
        {
            uint8_t pair_idx_mapping;
            uint8_t active_idx = app_a2dp_get_active_idx();

            app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping);

            app_iphone_abs_vol_wrap_audio_track_volume_out_set(track_handle, vol,
                                                               app_cfg_nv.abs_vol[pair_idx_mapping]);
        }
        else
        {
#if HARMAN_OPEN_LR_FEATURE
            app_harman_lr_balance_set(AUDIO_STREAM_TYPE_VOICE, vol, __func__, __LINE__);
#endif
            audio_track_volume_out_set(track_handle, vol);
        }
#else
        audio_track_volume_out_set(track_handle, vol);
#endif
    }
}

#if 0
bool app_audio_buffer_level_get(uint16_t *level)
{
    T_APP_BR_LINK *p_link = NULL;
    bool ret = false;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        p_link = &app_db.br_link[app_a2dp_get_active_idx()];

        if (p_link && p_link->a2dp_track_handle)
        {
            audio_track_buffer_level_get(p_link->a2dp_track_handle, level);
            ret = true;
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            p_link = &app_db.br_link[i];

            if (p_link && p_link->a2dp_track_handle)
            {
                audio_track_buffer_level_get(p_link->a2dp_track_handle, level);
                ret = true;
                break;
            }
        }
    }

    return ret;
}
#endif

void app_audio_spk_mute_unmute(bool need)
{
    if (need)
    {
        app_db.playback_muted = true;
        app_db.voice_muted = true;

        audio_volume_out_mute(AUDIO_STREAM_TYPE_PLAYBACK);

        audio_volume_out_mute(AUDIO_STREAM_TYPE_VOICE);
    }
    else
    {
        app_db.playback_muted = false;
        app_db.voice_muted = false;

        if ((app_cfg_const.always_play_hf_incoming_tone_when_incoming_call) &&
            (app_hfp_get_call_status() == APP_CALL_INCOMING))
        {
            app_hfp_inband_tone_mute_ctrl();
        }
        else
        {
#if F_APP_IPHONE_ABS_VOL_HANDLE
            T_APP_BR_LINK *p_link;
            uint8_t pair_idx_mapping, cur_volume;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    p_link = &app_db.br_link[i];

                    if (p_link)
                    {
                        if (p_link->a2dp_track_handle)
                        {
                            app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
                            cur_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];
                            app_iphone_abs_vol_wrap_audio_track_volume_out_set(p_link->a2dp_track_handle, cur_volume,
                                                                               app_cfg_nv.abs_vol[pair_idx_mapping]);
                        }
                    }
                }
            }
#endif
            audio_volume_out_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
            audio_volume_out_unmute(AUDIO_STREAM_TYPE_VOICE);
        }
    }
}

void app_audio_track_spk_unmute(T_AUDIO_STREAM_TYPE stream_type)
{
    bool need_unmute = true;
    uint8_t active_idx;
    T_APP_BR_LINK *p_link = NULL;

    APP_PRINT_INFO4("app_audio_vol_unmute: local_in_case %d, remote_loc %d, voice_muted %d, playback_muted %d",
                    app_db.local_in_case, app_db.remote_loc, app_db.voice_muted, app_db.playback_muted);

#if F_APP_DURIAN_SUPPORT
    need_unmute = app_durian_audio_spk_unmute_check();
#else
    if (app_db.local_in_case)
    {
        need_unmute = false;
    }
#endif

    if (!need_unmute)
    {
        return;
    }

    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        active_idx = app_a2dp_get_active_idx();
        p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
        if (p_link != NULL)
        {
            if (p_link->playback_muted)
            {
                p_link->playback_muted = false;
                audio_track_volume_out_unmute(p_link->a2dp_track_handle);
            }
        }
    }
    else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
    {
        active_idx = app_hfp_get_active_idx();
        p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
        if (p_link != NULL)
        {
            if (p_link->voice_muted)
            {
                p_link->voice_muted = false;
                audio_track_volume_out_unmute(p_link->sco_track_handle);
            }
        }
    }
}

#if F_APP_ERWS_SUPPORT
bool app_audio_mute_status_sync(uint8_t *bd_addr, T_AUDIO_STREAM_TYPE stream_type)
{
    uint8_t cmd_ptr[7] = {0};
    uint8_t pair_idx_mapping;
    bool ret = false;
    T_APP_BR_LINK *p_link;

    APP_PRINT_INFO2("app_audio_mute_status_sync: bd_addr %s, stream_type %d", TRACE_BDADDR(bd_addr),
                    stream_type);

    if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping) == false)
    {
        return ret;
    }

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
            (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
        {
            memcpy(&cmd_ptr[0], bd_addr, 6);
            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                cmd_ptr[6] = p_link->playback_muted;
                ret = app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                         APP_REMOTE_MSG_SYNC_PLAYBACK_MUTE_STATUS,
                                                         cmd_ptr, 7, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
            else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                cmd_ptr[6] = p_link->voice_muted;
                ret = app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                         APP_REMOTE_MSG_SYNC_VOICE_MUTE_STATUS,
                                                         cmd_ptr, 7, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
        }
    }

    return ret;
}

void app_audio_remote_join_set(bool enable)
{
    if (enable != force_join)
    {
        force_join = enable;
        audio_remote_join_set(true, enable);
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_FORCE_JOIN_SET);
    }
}
#endif

#if (F_APP_VENDOR_CMD_SUPPORT)
void app_audio_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (cmd_id)
    {
    case CMD_SET_VP_VOLUME:
        {
#if HARMAN_OPEN_LR_FEATURE
            harman_set_vp_ringtone_balance(__func__, __LINE__);
#endif
            voice_prompt_volume_set(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AUDIO_DSP_CTRL_SEND:
        {
            uint8_t *buf;
            T_AUDIO_STREAM_TYPE stream_type;
            T_DSP_TOOL_GAIN_LEVEL_DATA *gain_level_data;
            bool send_cmd_flag = true;

            buf = malloc(cmd_len - 2);
            if (buf == NULL)
            {
                return;
            }

            memcpy(buf, &cmd_ptr[2], (cmd_len - 2));

            gain_level_data = (T_DSP_TOOL_GAIN_LEVEL_DATA *)buf;

#if F_APP_SPP_CAPTURE_DSP_DATA_1
            send_cmd_flag = app_spp_capture_audio_dsp_ctrl_send_handler(&cmd_ptr[0], cmd_len, cmd_path,
                                                                        app_idx, &ack_pkt[0], send_cmd_flag);
#endif
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (gain_level_data->cmd_id == CFG_H2D_DAC_GAIN ||
                gain_level_data->cmd_id == CFG_H2D_VOICE_ADC_POST_GAIN ||
                gain_level_data->cmd_id == CFG_H2D_APT_DAC_GAIN)
            {
                switch (gain_level_data->category)
                {
                case AUDIO_CATEGORY_TONE:
                case AUDIO_CATEGORY_VP:
                case AUDIO_CATEGORY_AUDIO:
                    {
                        stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
                    }
                    break;

                case AUDIO_CATEGORY_APT:
                case AUDIO_CATEGORY_LLAPT:
                case AUDIO_CATEGORY_ANC:
                case AUDIO_CATEGORY_ANALOG:
                case AUDIO_CATEGORY_VOICE:
                    {
                        stream_type = AUDIO_STREAM_TYPE_VOICE;
                    }
                    break;
#if 0
                case AUDIO_CATEGORY_VAD:
#endif
                case AUDIO_CATEGORY_RECORD:
                    {
                        stream_type = AUDIO_STREAM_TYPE_RECORD;
                    }
                    break;

                default:
                    {
                        stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
                    }
                    break;
                }

                if (gain_level_data->cmd_id == CFG_H2D_DAC_GAIN)
                {
                    app_audio_route_dac_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
                                                 gain_level_data->gain);
                    audio_volume_out_set(stream_type, gain_level_data->level);
                }
                else if (gain_level_data->cmd_id == CFG_H2D_APT_DAC_GAIN)
                {
                    app_audio_route_dac_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
                                                 gain_level_data->gain);
                    app_cfg_nv.apt_volume_out_level = gain_level_data->level;
#if F_APP_APT_SUPPORT
                    audio_passthrough_volume_out_set(gain_level_data->level);
#endif
                }
                else
                {
                    app_audio_route_adc_gain_set((T_AUDIO_CATEGORY)gain_level_data->category, gain_level_data->level,
                                                 gain_level_data->gain);
                    if (((T_AUDIO_CATEGORY)gain_level_data->category != AUDIO_CATEGORY_APT) &&
                        ((T_AUDIO_CATEGORY)gain_level_data->category != AUDIO_CATEGORY_LLAPT))
                    {
                        audio_volume_in_set(stream_type, gain_level_data->level);
                    }
#if F_APP_APT_SUPPORT
                    else
                    {
                        audio_passthrough_volume_in_set(gain_level_data->level);
                    }
#endif
                }
            }

            if (send_cmd_flag)
            {
                audio_probe_dsp_send(buf, cmd_len - 2);
            }
            free(buf);
        }
        break;

    case CMD_SET_VOLUME:
        {
            uint8_t max_volume = 0;
            uint8_t min_volume = 0;
            uint8_t set_volume = cmd_ptr[2];

            T_AUDIO_STREAM_TYPE volume_type;

            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                volume_type = AUDIO_STREAM_TYPE_VOICE;
            }
            else
            {
                volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
            }

            max_volume = audio_volume_out_max_get(volume_type);
            min_volume = audio_volume_out_min_get(volume_type);

            if ((set_volume <= max_volume) && (set_volume >= min_volume))
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    audio_volume_out_set(volume_type, set_volume);
                    app_audio_cmd_set_volume_report_status(cmd_path, app_idx);
                }
                else
                {
#if F_APP_ERWS_SUPPORT
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        uint8_t temp_cmd_ptr[4];

                        temp_cmd_ptr[0] = volume_type;
                        temp_cmd_ptr[1] = set_volume;
                        temp_cmd_ptr[2] = cmd_path;
                        temp_cmd_ptr[3] = app_idx;
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_CMD_SET_VOLUME,
                                                           temp_cmd_ptr, 4, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                    }
#endif
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

#if F_APP_APT_SUPPORT
    case CMD_SET_APT_VOLUME_OUT_LEVEL:
        {
            uint8_t event_data = cmd_ptr[2];

            if ((event_data < app_dsp_cfg_vol.apt_volume_out_min) ||
                (event_data > app_dsp_cfg_vol.apt_volume_out_max))
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                app_cfg_nv.apt_volume_out_level = event_data;

#if APT_SUB_VOLUME_LEVEL_SUPPORT
                app_apt_main_volume_set(event_data);
#else
                audio_passthrough_volume_out_set(event_data);
#endif
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_APT, APP_REMOTE_MSG_APT_VOLUME_OUT_LEVEL,
                                                    &event_data, sizeof(uint8_t));
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_APT_VOLUME_OUT_LEVEL:
        {
            uint8_t event_data = 0;

            event_data = app_cfg_nv.apt_volume_out_level;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_APT_VOLUME_OUT_LEVEL, app_idx, &event_data, sizeof(uint8_t));
        }
        break;
#endif

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    case CMD_SET_NOTIFICATION_VOL_LEVEL:
    case CMD_GET_NOTIFICATION_VOL_LEVEL:
        {
            uint16_t param_len = cmd_len - COMMAND_ID_LENGTH;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_audio_notification_vol_cmd_handle(cmd_id, &cmd_ptr[2], param_len, cmd_path, app_idx);
        }
        break;
#endif

    case CMD_DSP_TOOL_FUNCTION_ADJUSTMENT:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            uint8_t event_data[3];
            uint16_t function_type;

            LE_ARRAY_TO_UINT16(function_type, &cmd_ptr[2]);

            memcpy(event_data, &cmd_ptr[2], 2);
            event_data[2] = CMD_SET_STATUS_COMPLETE;

            switch (function_type)
            {
            case DSP_TOOL_OPCODE_BRIGHTNESS:
                {
#if F_APP_APT_SUPPORT
                    uint16_t linear_vlaue;
                    float alpha;

                    LE_ARRAY_TO_UINT16(linear_vlaue, &cmd_ptr[4]);
                    alpha = (float)linear_vlaue * 0.0025f;

                    audio_passthrough_brightness_set(alpha);
#endif
                }
                break;

            case DSP_TOOL_OPCODE_HW_EQ:
                {
                    uint8_t eq_type = cmd_ptr[4];
                    uint8_t eq_channel = cmd_ptr[5];
                    uint16_t eq_len;

                    LE_ARRAY_TO_UINT16(eq_len, &cmd_ptr[10]);

                    audio_probe_codec_hw_eq_set(eq_type, eq_channel, &cmd_ptr[12], eq_len);
                }
                break;


            case DSP_TOOL_OPCODE_SW_EQ:
                {
                    uint8_t direction = cmd_ptr[4];
                    uint8_t stream_type = cmd_ptr[6];
                    uint8_t eq_index = cmd_ptr[7];
                    uint16_t eq_len = 0;
                    uint8_t eq_mode;
                    uint8_t eq_num;
                    T_EQ_ENABLE_INFO enable_info;
                    T_AUDIO_EFFECT_INSTANCE eq_instance;

                    app_db.sw_eq_type = cmd_ptr[5];
                    LE_ARRAY_TO_UINT16(eq_len, &cmd_ptr[8]);
                    eq_mode = app_eq_mode_get(app_db.sw_eq_type, stream_type);

                    eq_num = eq_utils_num_get((T_EQ_TYPE)app_db.sw_eq_type, eq_mode);
                    app_eq_enable_info_get(eq_mode, &enable_info);
                    eq_instance = enable_info.instance;

                    if (eq_num == 0 && eq_instance == NULL)
                    {
                        app_eq_enable_effect(eq_mode, eq_len);
                    }

                    if (!app_eq_cmd_operate(eq_mode, direction, false, eq_index, eq_len, &cmd_ptr[10]))
                    {
                        event_data[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                break;

            default:
                {
                    event_data[2] = CMD_SET_STATUS_UNKNOW_CMD;
                }
                break;
            }

            app_report_event(cmd_path, EVENT_DSP_TOOL_FUNCTION_ADJUSTMENT, app_idx, event_data,
                             sizeof(event_data));
        }
        break;

#if F_APP_SIDETONE_SUPPORT
    case CMD_SET_SIDETONE:
        {
            T_APP_DSP_CFG_SIDETONE sidetone_info;
            T_APP_BR_LINK *p_link = NULL;

            sidetone_info.hw_enable = cmd_ptr[2];
            sidetone_info.gain = (cmd_ptr[3] | cmd_ptr[4] << 8);
            sidetone_info.hpf_level = cmd_ptr[5];

            if (app_link_get_sco_conn_num() != 0)
            {
                p_link = &app_db.br_link[active_hf_idx];

                app_sidetone_detach(p_link->sco_track_handle, &p_link->sidetone_instance);

                p_link->sidetone_instance = app_sidetone_attach(p_link->sco_track_handle, sidetone_info);
            }
#if F_APP_CCP_SUPPORT
            else if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
            {
#if F_APP_SIDETONE_SUPPORT
                app_lea_uca_set_sidetone_instance(sidetone_info);
#endif
            }
#endif
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_SCENARIO_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    case CMD_MIC_SWITCH:
        {
            uint8_t param = app_audio_mic_switch(cmd_ptr[2]);

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
            {
                app_report_event(cmd_path, EVENT_MIC_SWITCH, app_idx, &param, 1);
            }
        }
        break;

    case CMD_DSP_TEST_MODE:
        {
            uint8_t dsp_cmd_len;
            uint8_t *dsp_cmd_buf;
            uint8_t dsp_param_len;

            dsp_cmd_len = cmd_len + 2;
            dsp_cmd_len = ((dsp_cmd_len + 3) >> 2) << 2; //transfer to 4-byte align
            dsp_cmd_buf = calloc(1, dsp_cmd_len);
            dsp_param_len = cmd_len - 2;

            if (dsp_cmd_buf == NULL)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                return;
            }
            else
            {
                dsp_cmd_buf[0] = AUDIO_PROBE_TEST_MODE;
                dsp_cmd_buf[2] = ((dsp_param_len + 3) >> 2); //word length
                memcpy(&dsp_cmd_buf[4], &cmd_ptr[2], dsp_param_len);

                audio_probe_dsp_send(dsp_cmd_buf, dsp_cmd_len);
                free(dsp_cmd_buf);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_DUAL_MIC_MP_VERIFY:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            uint8_t error_code;

            if (app_audio_get_bud_stream_state() == BUD_STREAM_STATE_VOICE)
            {
                T_AUDIO_MP_DUAL_MIC_SETTING mp_dual_mic_setting;
                mp_dual_mic_setting = app_audio_mp_dual_mic_setting_check(&cmd_ptr[2]);

                if (mp_dual_mic_setting == AUDIO_MP_DUAL_MIC_SETTING_VALID)
                {
                    app_audio_mp_dual_mic_switch_action();
                    break;
                }
                else if (mp_dual_mic_setting == AUDIO_MP_DUAL_MIC_SETTING_ROLE_SWAP)
                {
                    app_mmi_handle_action(MMI_START_ROLESWAP);
                    break;
                }
                else
                {
                    error_code = 1;  //invalid mic setting
                }
            }
            else
            {
                error_code = 2;  //wrong dsp state
            }

            app_report_event(cmd_path, EVENT_DUAL_MIC_MP_VERIFY, app_idx, &error_code, 1);
        }
        break;

    case CMD_SOUND_PRESS_CALIBRATION:
        {
            uint8_t event_param_val = 0x00;

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (app_cfg_const.tone_sound_press_calibration == 0xFF)
            {
                event_param_val = 0x01;
            }
            app_report_event(cmd_path, EVENT_SOUND_PRESS_CALIBRATION, app_idx, &event_param_val,
                             sizeof(uint8_t));

            app_audio_tone_type_play(TONE_SOUND_PRESS_CALIBRATION, false, false);
        }
        break;

#if 0
    case CMD_GET_LOW_LATENCY_MODE_STATUS:
        {
#if F_APP_HARMAN_FEATURE_SUPPORT
            uint16_t latency_value = app_cfg_nv.cmd_latency_in_ms;
#else
            uint16_t latency_value = app_cfg_nv.audio_latency; // default value
#endif
            T_APP_BR_LINK *p_link = NULL;

            if (app_db.gaming_mode)
            {
                if (app_link_check_b2s_link_by_id(active_a2dp_idx))
                {
                    p_link = &app_db.br_link[active_a2dp_idx];
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if ((p_link != NULL) && (p_link->a2dp_track_handle != NULL))
                {
                    audio_track_latency_get(p_link->a2dp_track_handle, &latency_value);
                }
                else
                {
                    // audio track is null, use last used latency value
                    app_audio_get_last_used_latency(&latency_value);
                }
            }

            app_audio_report_low_latency_status(latency_value);
        }
        break;

    case CMD_SET_LOW_LATENCY_LEVEL:
        {
            uint8_t event_data[3];
            uint8_t latency_level = cmd_ptr[2];
#if F_APP_HARMAN_FEATURE_SUPPORT
            uint16_t latency_value = app_cfg_nv.cmd_latency_in_ms;
#else
            uint16_t latency_value = app_cfg_nv.audio_latency; // default value
#endif
            T_APP_BR_LINK *p_link = NULL;
            bool is_dongle_active = false;

            if (app_link_check_b2s_link_by_id(active_a2dp_idx))
            {
                p_link = &app_db.br_link[active_a2dp_idx];
            }

#if F_APP_GAMING_DONGLE_SUPPORT
            if (p_link && app_link_check_dongle_link(p_link->bd_addr))
            {
                is_dongle_active = true;
            }
#endif

            if ((!app_db.gaming_mode) || (latency_level >= LOW_LATENCY_LEVEL_MAX) || (p_link == NULL) ||
                is_dongle_active)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            latency_value = app_audio_update_audio_track_latency(p_link->a2dp_track_handle, latency_level);

            event_data[0] = app_cfg_nv.rws_low_latency_level_record;
            event_data[1] = (uint8_t)(latency_value);
            event_data[2] = (uint8_t)(latency_value >> 8);

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_LOW_LATENCY_LEVEL_SET, app_idx, event_data, sizeof(event_data));
        }
        break;
#endif

    default:
        break;
    }
}
#endif

#if F_APP_LEA_SUPPORT
T_MTC_RESULT app_audio_mtc_if_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT app_result = MTC_RESULT_SUCCESS;

    APP_PRINT_INFO2("app_audio_mtc_if_handle: msg %x, %d", msg, app_cfg_nv.bud_role);

    switch (msg)
    {
    case AP_FM_MTC_RESUME_SCO:
        {
            uint8_t *bd_addr = (uint8_t *)inbuf;
            T_APP_BR_LINK *p_link;
            //APP_PRINT_TRACE1("AP_FM_MTC_RESUME_SCO: bd_addr %s",
            //                 TRACE_BDADDR(bd_addr));

        } break;

    case AP_FM_MTC_RESUME_A2DP:
        {
            T_BT_EVENT_PARAM_A2DP_STREAM_START_IND *a2dp_para = (T_BT_EVENT_PARAM_A2DP_STREAM_START_IND *)inbuf;
            app_audio_a2dp_stream_start_handle(a2dp_para->bd_addr, (uint8_t *)a2dp_para);
        }
        break;

    case AP_FM_MTC_STOP_A2DP:
        {
            uint8_t *bd_addr = (uint8_t *)inbuf;
#if F_APP_QOL_MONITOR_SUPPORT
            app_qol_link_monitor(app_cfg_nv.bud_peer_addr, false);
            app_qol_link_monitor(bd_addr, false);
#endif
#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_bt_sniffing_stop(bd_addr, BT_SNIFFING_TYPE_A2DP);
            }
#endif
            // app_audio_a2dp_track_release(bd_addr);
        }
        break;

    default:
        break;
    }
    return app_result;
}
#endif

#if F_APP_DUAL_AUDIO_EFFECT
static T_APP_BR_LINK *app_audio_get_a2dp_active_link(void)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

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
    return p_link;
}

bool app_audio_get_a2dp_active(void)
{
    T_APP_BR_LINK *p_link = NULL;

    p_link = app_audio_get_a2dp_active_link();
    if (p_link != NULL && p_link->a2dp_track_handle != NULL)
    {
        T_AUDIO_TRACK_STATE state = AUDIO_TRACK_STATE_RELEASED;

        audio_track_state_get(p_link->a2dp_track_handle, &state);

        if (state == AUDIO_TRACK_STATE_STARTED)
        {
            return true;
        }
    }

    return false;
}

void app_audio_restart_track(void)
{
    T_APP_BR_LINK *p_link = NULL;
    T_AUDIO_STREAM_USAGE usage;
    T_AUDIO_STREAM_MODE mode;
    uint8_t vol;
    T_AUDIO_FORMAT_INFO format;
    bool vol_muted;
    uint16_t latency_value = app_cfg_nv.audio_latency;

    p_link = app_audio_get_a2dp_active_link();
    if (p_link != NULL && p_link->a2dp_track_handle != NULL)
    {
        APP_PRINT_TRACE2("dual_audio_effect_restart_track: active link %s, mode %u",
                         TRACE_BDADDR(p_link->bd_addr),
                         app_db.gaming_mode);
        audio_track_format_info_get(p_link->a2dp_track_handle, &format);
        audio_track_usage_get(p_link->a2dp_track_handle, &usage);
        audio_track_volume_out_is_muted(p_link->a2dp_track_handle, &vol_muted);
        audio_track_volume_out_get(p_link->a2dp_track_handle, &vol);

        audio_track_release(p_link->a2dp_track_handle);
        if (app_db.gaming_mode)
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_db.remote_is_dongle)
            {
                mode = AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY;
            }
            else
#endif
            {
                mode = AUDIO_STREAM_MODE_LOW_LATENCY;
            }

            app_audio_get_last_used_latency(&latency_value);
        }
        else
        {
            mode = AUDIO_STREAM_MODE_NORMAL;
        }

        p_link->a2dp_track_handle =  audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
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
        }

        if (app_db.gaming_mode)
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_db.remote_is_dongle)
            {
                latency_value = app_dongle_get_gaming_latency();
                app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);

                app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }
            else
#endif
            {
                latency_value = app_audio_set_latency(p_link->a2dp_track_handle,
                                                      app_cfg_nv.rws_low_latency_level_record,
                                                      GAMING_MODE_DYNAMIC_LATENCY_FIX);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }

            app_audio_update_latency_record(latency_value);
        }
        else
        {
            app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_NORMAL, format.type, 0, &latency_value);
            audio_track_latency_set(p_link->a2dp_track_handle, latency_value,
                                    NORMAL_MODE_DYNAMIC_LATENCY_FIX);
            bt_a2dp_stream_delay_report_request(p_link->bd_addr, app_cfg_nv.audio_latency);
        }
        audio_track_start(p_link->a2dp_track_handle);
    }
}

uint8_t app_audio_get_dongle_flag(void)
{
    return app_db.remote_is_dongle;
}
#endif

#if F_APP_COMMON_DONGLE_SUPPORT
void app_audio_update_dongle_flag(bool is_dongle)
{
    app_db.remote_is_dongle = is_dongle;

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_REMOTE_IS_DONGLE);
    }
#endif
}
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
uint8_t app_audio_a2dp_stream_resume(uint8_t *audio_cfg)
{
    uint8_t ret = 0;
    T_APP_BR_LINK *p_link = NULL;
    T_BT_EVENT_PARAM_A2DP_STREAM_START_IND *cfg = (T_BT_EVENT_PARAM_A2DP_STREAM_START_IND *)audio_cfg;
    uint8_t null_addr[6] = {0};

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        if (app_link_find_br_link(p_link->bd_addr) == NULL)
        {
            ret = 1;
            goto EXIT;
        }

        if (p_link->bt_sniffing_state < APP_BT_SNIFFING_STATE_READY)
        {
            app_bt_sniffing_process(p_link->bd_addr);
        }

        app_bt_sniffing_start(p_link->bd_addr, BT_SNIFFING_TYPE_A2DP);
    }

    if (!memcmp(cfg->bd_addr, null_addr, 6))
    {
        ret = 2;
        goto EXIT;
    }

    app_audio_a2dp_stream_start_handle(cfg->bd_addr, (uint8_t *)cfg);

EXIT:
    APP_PRINT_TRACE1("app_audio_a2dp_stream_resume: ret %d", ret);
    return ret;
}
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
void harman_push_max_tone_when_not_playing(T_AUDIO_STREAM_TYPE stream_type, uint8_t pre_volume,
                                           uint8_t cur_volume)
{
    T_AUDIO_VOL_CHANGE   vol_status;
    vol_status = AUDIO_VOL_CHANGE_NONE;

    APP_PRINT_TRACE5("[SD_CHECK] harman_push_max_tone_when_not_playing key_press=%d, phone_press=%d, stream_type=%d,pre_vol=%d,cur_vol=%d",
                     app_key_is_set_volume(),
                     is_max_min_vol_from_phone,
                     stream_type,
                     pre_volume,
                     cur_volume);

    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        if ((cur_volume == app_dsp_cfg_vol.playback_volume_max) &&
            (pre_volume == app_dsp_cfg_vol.playback_volume_max))
        {
            vol_status = AUDIO_VOL_CHANGE_MAX;
        }
    }
    if (vol_status == AUDIO_VOL_CHANGE_MAX)
    {
        app_key_set_volume_status(true);
        APP_PRINT_TRACE0("[SD_CHECK] harman_push_max_tone_when_not_playing change_vol++");
        app_audio_handle_vol_change(vol_status);
    }
}
#endif

void app_audio_remote_join_set(bool enable)
{
    if (enable != force_join)
    {
        force_join = enable;
        audio_remote_join_set(true, enable);
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_FORCE_JOIN_SET);
    }
}
