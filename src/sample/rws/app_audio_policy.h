/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AUDIO_POLICY_H_
#define _APP_AUDIO_POLICY_H_

#include <stdint.h>
#include <stdbool.h>

#include "voice_prompt.h"
#include "audio_volume.h"
#include "audio_track.h"
#include "app_flags.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_AUDIO App Audio
  * @brief App Audio
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_AUDIO_Exported_Functions App Audio Functions
    * @{
    */

typedef enum
{
    AUDIO_VOL_CHANGE_NONE                  = 0x00,
    AUDIO_VOL_CHANGE_UP                    = 0x01,
    AUDIO_VOL_CHANGE_DOWN                  = 0x02,
    AUDIO_VOL_CHANGE_MAX                   = 0x03,
    AUDIO_VOL_CHANGE_MIN                   = 0x04,
    AUDIO_VOL_CHANGE_MUTE                  = 0x05,
    AUDIO_VOL_CHANGE_UNMUTE                = 0x06,
} T_AUDIO_VOL_CHANGE;

typedef struct
{
    uint8_t     bd_addr[6];
    uint8_t     playback_volume;
    uint8_t     voice_volume;
#if F_APP_IPHONE_ABS_VOL_HANDLE
    uint8_t     abs_volume;
#endif
} T_APP_AUDIO_VOLUME;

typedef enum
{
    AUDIO_COUPLE_SET                    = 0,
    AUDIO_SINGLE_SET                    = 1,
    AUDIO_SPECIFIC_SET                  = 2,
} T_AUDIO_CHANCEL_SET;

typedef enum
{
    AUDIO_MP_DUAL_MIC_SETTING_INVALID,
    AUDIO_MP_DUAL_MIC_SETTING_VALID,
    AUDIO_MP_DUAL_MIC_SETTING_ROLE_SWAP,

    AUDIO_MP_DUAL_MIC_SETTING_TOTAL
} T_AUDIO_MP_DUAL_MIC_SETTING;

/* NOTICE: Make sure T_APP_AUDIO_TONE_TYPE is align to app_cfg_const.tone_xxxx offset */
typedef enum
{
    TONE_POWER_ON                       = 0x00,
    TONE_POWER_OFF                      = 0x01,
    TONE_PAIRING                        = 0x02,
    TONE_PAIRING_COMPLETE               = 0x03,
    TONE_PAIRING_FAIL                   = 0x04,
    TONE_KEY_SHORT_PRESS                = 0x05,
    TONE_KEY_LONG_PRESS                 = 0x06,
    TONE_VOL_CHANGED                    = 0x07,
    TONE_KEY_HYBRID                     = 0x08,
    TONE_HF_NO_SERVICE                  = 0x09,
    TONE_HF_CALL_IN                     = 0x0A,
    TONE_FIRST_HF_CALL_IN               = 0x0B,
    TONE_LAST_HF_CALL_IN                = 0x0C,
    TONE_HF_CALL_ACTIVE                 = 0x0D,
    TONE_HF_CALL_END                    = 0x0E,
    TONE_HF_CALL_REJECT                 = 0x0F,

    TONE_HF_CALL_VOICE_DIAL             = 0x10,
    TONE_HF_CALL_REDIAL                 = 0x11,
    TONE_IN_EAR_DETECTION               = 0x12,
    TONE_GFPS_FINDME                    = 0x13,
    TONE_LINK_CONNECTED                 = 0x14,
    TONE_VOL_MAX                        = 0x15,
    TONE_VOL_MIN                        = 0x16,
    TONE_ALARM                          = 0x17,
    TONE_LINK_LOSS                      = 0x18,
    TONE_LINK_DISCONNECT                = 0x19,
    TONE_REMOTE_CONNECTED               = 0x1A,
    TONE_REMOTE_ROLE_PRIMARY            = 0x1B,
    TONE_REMOTE_ROLE_SECONDARY          = 0x1C,
    TONE_LIGHT_SENSOR_ON                = 0x1D,
    TONE_LIGHT_SENSOR_OFF               = 0x1E,
    TONE_EXIT_GAMING_MODE               = 0x1F,

    TONE_ENTER_AIRPLANE                 = 0x20,
    TONE_OTA_OVER_BLE_START             = 0x21,
    TONE_LE_PAIR_COMPLETE               = 0x22,
    TONE_PAIRING_TIMEOUT                = 0x23,
    TONE_MIC_MUTE_ALARM                 = 0x24,
    TONE_KEY_ULTRA_LONG_PRESS           = 0x25,
    TONE_DEV_MULTILINK_ON               = 0x26,
    TONE_REMOTE_DISCONNECT              = 0x27,
    TONE_FACTORY_RESET                  = 0x28,
    TONE_REMOTE_LOSS                    = 0x29,
    TONE_MIC_MUTE_ON                    = 0x2A,
    TONE_MIC_MUTE_OFF                   = 0x2B,
    TONE_LINK_LOSS2                     = 0x2C,
    TONE_LINK_DISCONNECT2               = 0x2D,
    TONE_BATTERY_PERCENTAGE_10          = 0x2E,
    TONE_BATTERY_PERCENTAGE_20          = 0x2F,

#if F_APP_HARMAN_FEATURE_SUPPORT
    TONE_HARMAN_POWER_ON                = 0x30,
    TONE_HARMAN_POWER_OFF               = 0x31,
    TONE_HARMAN_FACTORY_RESET           = 0x32,
    TONE_HARMAN_PAIRING                 = 0x33,
    TONE_HARMAN_HF_CALL_IN              = 0x34,
    TONE_HARMAN_LINK_CONNECTED          = 0x35,
    TONE_HARMAN_VOL_MAX_MIN             = 0x36,
    TONE_HARMAN_MIC_MUTE_ON             = 0x37,
#else
    TONE_BATTERY_PERCENTAGE_30          = 0x30,
    TONE_BATTERY_PERCENTAGE_40          = 0x31,
    TONE_BATTERY_PERCENTAGE_50          = 0x32,
    TONE_BATTERY_PERCENTAGE_60          = 0x33,
    TONE_BATTERY_PERCENTAGE_70          = 0x34,
    TONE_BATTERY_PERCENTAGE_80          = 0x35,
    TONE_BATTERY_PERCENTAGE_90          = 0x36,
    TONE_BATTERY_PERCENTAGE_100         = 0x37,
#endif

    TONE_AUDIO_PLAYING                  = 0x38,
    TONE_AUDIO_PAUSED                   = 0x39,
    TONE_AUDIO_FORWARD                  = 0x3A,
    TONE_AUDIO_BACKWARD                 = 0x3B,
    TONE_APT_ON                         = 0x3C,
    TONE_APT_OFF                        = 0x3D,
    TONE_ENTER_GAMING_MODE              = 0x3E,
    TONE_OUTPUT_INDICATION_1            = 0x3F,

    TONE_SWITCH_VP_LANGUAGE             = 0x40,
    TONE_DUT_TEST                       = 0x41,
    TONE_HF_TRANSFER_TO_PHONE           = 0x42,
    TONE_HF_OUTGOING_CALL               = 0x43,
    TONE_AUDIO_EFFECT_0                 = 0x44,
    TONE_AUDIO_EFFECT_1                 = 0x45,
    TONE_AUDIO_EFFECT_2                 = 0x46,
    TONE_SOUND_PRESS_CALIBRATION        = 0x47,
    TONE_EXIT_AIRPLANE                  = 0x48,
    TONE_ANC_APT_OFF                    = 0x49,

#if F_APP_HARMAN_FEATURE_SUPPORT
    TONE_HARMAN_MIC_MUTE_OFF            = 0x4A,
    TONE_HARMAN_BATTERY_PERCENTAGE_10   = 0x4B,
    TONE_HARMAN_BATTERY_PERCENTAGE_20   = 0x4C,
    TONE_HARMAN_GFPS_FIND_ME            = 0x4D,
    TONE_HARMAN_DULT_TONE               = 0x4E,
    TONE_HARMAN_DULT_VP                 = 0x4F,
#else
    TONE_ANC_SCENARIO_1                 = 0x4A,
    TONE_ANC_SCENARIO_2                 = 0x4B,
    TONE_ANC_SCENARIO_3                 = 0x4C,
    TONE_ANC_SCENARIO_4                 = 0x4D,
    TONE_ANC_SCENARIO_5                 = 0x4E,
    TONE_ENTER_DONGLE_MODE              = 0x4F,
#endif

    TONE_APT_VOL_0                      = 0x50,
    TONE_APT_VOL_1                      = 0x51,
    TONE_APT_VOL_2                      = 0x52,
    TONE_APT_VOLUME_UP                  = 0x53,
    TONE_APT_VOLUME_DOWN                = 0x54,
    TONE_AUDIO_EQ_0                     = 0x55,
    TONE_AUDIO_EQ_1                     = 0x56,
    TONE_AUDIO_EQ_2                     = 0x57,
    TONE_AUDIO_EQ_3                     = 0x58,
    TONE_AUDIO_EQ_4                     = 0x59,
    TONE_AUDIO_EQ_5                     = 0x5A,
    TONE_AUDIO_EQ_6                     = 0x5B,
    TONE_AUDIO_EQ_7                     = 0x5C,
    TONE_AUDIO_EQ_8                     = 0x5D,
    TONE_AUDIO_EQ_9                     = 0x5E,
    TONE_APT_VOL_MAX                    = 0x5F,

    TONE_APT_VOL_MIN                    = 0x60,
    TONE_APT_EQ_0                       = 0x61,
    TONE_APT_EQ_1                       = 0x62,
    TONE_APT_EQ_2                       = 0x63,
    TONE_APT_EQ_3                       = 0x64,
    TONE_APT_EQ_4                       = 0x65,
    TONE_APT_EQ_5                       = 0x66,
    TONE_APT_EQ_6                       = 0x67,
    TONE_APT_EQ_7                       = 0x68,
    TONE_APT_EQ_8                       = 0x69,
    TONE_APT_EQ_9                       = 0x6A,
    TONE_LLAPT_SCENARIO_1               = 0x6B,
    TONE_LLAPT_SCENARIO_2               = 0x6C,
    TONE_LLAPT_SCENARIO_3               = 0x6D,
    TONE_LLAPT_SCENARIO_4               = 0x6E,
    TONE_LLAPT_SCENARIO_5               = 0x6F,

    TONE_AMA_START_SPEECH               = 0x70,
    TONE_AMA_STOP_SPEECH                = 0x71,
    TONE_AMA_ERROR_INDICATION           = 0x72,

    TONE_APT_VOL_3                      = 0x73,
    TONE_APT_VOL_4                      = 0x74,
    TONE_APT_VOL_5                      = 0x75,
    TONE_APT_VOL_6                      = 0x76,
    TONE_APT_VOL_7                      = 0x77,
    TONE_APT_VOL_8                      = 0x78,
#if GFPS_LE_DEVICE_SUPPORT
    TONE_GFPS_PAIRING                   = 0x79,
#else
    TONE_APT_VOL_9                      = 0x79,
#endif
    TONE_BIS_START                      = 0x7A,
    TONE_BIS_STOP                       = 0x7B,

    TONE_RSV0                           = 0x7C,
    TONE_RSV1                           = 0x7D,
    TONE_RSV2                           = 0x7E,
    TONE_BIS_LOST                       = 0x7F,

    // APP config bin offset 0x3B0
    TONE_CIS_LOST                       = 0x80,
    TONE_CIS_CONNECTED                  = 0x81,
    TONE_GFPS_DULT_FIND                 = 0x82,

    TONE_TYPE_MAX                       = 0xA0, // Tone1 128bytes + Tone2 32bytes
} T_APP_AUDIO_TONE_TYPE;

#if F_APP_HARMAN_FEATURE_SUPPORT
#define   TONE_HARMAN_GFPS_ADV_STOP  TONE_HARMAN_MIC_MUTE_OFF
#endif

typedef struct
{
    uint8_t notification_vol_max_level;
    uint8_t notification_vol_min_level;
    uint8_t notification_vol_cur_level;
    bool    first_sync;
    bool    need_to_report;
    bool    need_to_sync;
} T_NOTIFICATION_VOL_RELAY_MSG;

typedef enum
{
    APP_REMOTE_MSG_EQ_DATA                                      = 0x00,
    APP_REMOTE_MSG_SYNC_ABS_VOLUME                              = 0x01,
    APP_REMOTE_MSG_HFP_VOLUME_SYNC                              = 0x02,
    APP_REMOTE_MSG_A2DP_VOLUME_SYNC                             = 0x03,
    APP_REMOTE_MSG_AUDIO_VOLUME_SYNC                            = 0x04,
    APP_REMOTE_MSG_AUDIO_VOLUME_RESET                           = 0x05,
    APP_REMOTE_MSG_INBAND_TONE_MUTE_SYNC                        = 0x06,
    APP_REMOTE_MSG_PLAY_MIN_MAX_VOL_TONE                        = 0x07,
    APP_REMOTE_MSG_AUDIO_CFG_PARAM_SYNC                         = 0x08,
    APP_REMOTE_MSG_ASK_TO_EXIT_GAMING_MODE                      = 0x09,
    APP_REMOTE_MSG_NOTIFICATION_VOL_SYNC                        = 0x0A,
    APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_CMD                   = 0x0B,
    APP_REMOTE_MSG_RELAY_NOTIFICATION_VOL_EVENT                 = 0x0C,
    APP_REMOTE_MSG_SYNC_IS_NOTIFICATION_VOL_ADJ_FROM_PHONE      = 0x0D,
    APP_REMOTE_MSG_SYNC_ABS_VOL_STATE                           = 0x0E,
    APP_REMOTE_MSG_VOICE_NR_SWITCH                              = 0x0F,

    APP_REMOTE_MSG_SYNC_TONE_STOP                               = 0x10,
    APP_REMOTE_MSG_GAMING_MODE_SET_ON                           = 0x11,
    APP_REMOTE_MSG_GAMING_MODE_SET_OFF                          = 0x12,
    APP_REMOTE_MSG_HFP_CALL_START                               = 0x15,
    APP_REMOTE_MSG_HFP_CALL_STOP                                = 0x16,
    APP_REMOTE_MSG_CALL_TRANSFER_STATE                          = 0x17,
    APP_REMOTE_MSG_AUDIO_CHANNEL_CHANGE                         = 0x18,
    APP_REMOTE_MSG_SYNC_PLAY_STATUS                             = 0x19,
    APP_REMOTE_MSG_SYNC_MIC_STATUS                              = 0x1A,
    APP_REMOTE_MSG_SYNC_PLAYBACK_MUTE_STATUS                    = 0x1B,
    APP_REMOTE_MSG_SYNC_VOICE_MUTE_STATUS                       = 0x1C,
    APP_REMOTE_MSG_SYNC_CALL_STATUS                             = 0x1D,
    APP_REMOTE_MSG_RSV                                          = 0x1E,
    APP_REMOTE_MSG_SYNC_VP_LANGUAGE                             = 0x1F,

    APP_REMOTE_MSG_SYNC_LOW_LATENCY                             = 0x20,
    APP_REMOTE_MSG_SYNC_BUD_STREAM_STATE                        = 0x21,
    APP_REMOTE_MSG_DEFAULT_CHANNEL                              = 0x22,
    APP_REMOTE_MSG_SYNC_REMOTE_IS_DONGLE                        = 0x23,
    APP_REMOTE_MSG_SYNC_GAMING_MODE_REQUEST                     = 0x24,
    APP_REMOTE_MSG_SYNC_DONGLE_IS_ENABLE_MIC                    = 0x25,
    APP_REMOTE_MSG_SYNC_DONGLE_IS_DISABLE_APT                   = 0x26,
    APP_REMOTE_MSG_SYNC_MP_DUAL_MIC_SETTING                     = 0x27,
    APP_REMOTE_MSG_SYNC_DEFAULT_EQ_INDEX                        = 0x28,
    APP_REMOTE_MSG_SYNC_LAST_CONN_INDEX                         = 0x29,
    APP_REMOTE_MSG_SYNC_DISALLOW_PLAY_GAMING_MODE_VP            = 0x2A,
    APP_REMOTE_MSG_RSV1                                         = 0x2B,
    APP_REMOTE_MSG_SYNC_SW_EQ_TYPE                              = 0x2C,
    APP_REMOTE_MSG_SYNC_GAMING_RECORD_EQ_INDEX                  = 0x2D,
    APP_REMOTE_MSG_SYNC_NORMAL_RECORD_EQ_INDEX                  = 0x2E,
    APP_REMOTE_MSG_SYNC_ANC_RECORD_EQ_INDEX                     = 0x2F,

    APP_REMOTE_MSG_SYNC_LOW_LATENCY_LEVEL                       = 0x30,
    APP_REMOTE_MSG_SYNC_APT_POWER_ON_DELAY_APPLY_TIME           = 0x31,
    APP_REMOTE_MSG_SYNC_VOL_IS_CHANGED_BY_KEY                   = 0x32,
    APP_REMOTE_MSG_SYNC_A2DP_PLAY_STATUS                        = 0x33,
    APP_REMOTE_MSG_SYNC_EITHER_BUD_HAS_VOL_CTRL_KEY             = 0x34,
    APP_REMOTE_MSG_SYNC_FORCE_JOIN_SET                          = 0x35,
    APP_REMOTE_MSG_SYNC_CONNECTED_TONE_NEED                     = 0x36,
    APP_REMOTE_MSG_SYNC_SUSPEND_A2DP_BY_OUT_EAR                 = 0x37,
    APP_REMOTE_MSG_SYNC_IN_EAR_RECOVER_A2DP                     = 0x38,
    APP_REMOTE_MSG_SYNC_USER_EQ                                 = 0x39,
    APP_REMOTE_MSG_SYNC_CMD_SET_VOLUME                          = 0x3A,
    APP_REMOTE_MSG_SYNC_ALLOWED_SOURCE                          = 0x3B,

    APP_AUDIO_REMOTE_MSG_TOTAL,
} T_APP_AUDIO_REMOTE_MSG;

/**  @brief  App define bud stream state */
typedef enum
{
    BUD_STREAM_STATE_IDLE = 0x00,
    BUD_STREAM_STATE_AUDIO = 0x01,
    BUD_STREAM_STATE_VOICE = 0x02
} T_APP_BUD_STREAM_STATE;

/**  @brief  App define audio judge result */
typedef enum
{
    APP_AUDIO_RESULT_NOTHING = 0x00,
    APP_AUDIO_RESULT_PAUSE = 0x01,
    APP_AUDIO_RESULT_STOP = 0x02,
    APP_AUDIO_RESULT_RESUME = 0x03,
    APP_AUDIO_RESULT_VOICE_TO_AG = 0x04,
    APP_AUDIO_RESULT_VOICE_TO_BUD = 0x05,
} T_APP_AUDIO_JUDGE_RESULT;

typedef enum
{
    APP_A2DP_STREAM_AVRCP_PLAY,
    APP_A2DP_STREAM_AVRCP_PAUSE,
    APP_A2DP_STREAM_A2DP_START,
    APP_A2DP_STREAM_A2DP_STOP,
} T_APP_A2DP_STREAM_EVENT;

typedef enum
{
    APP_AUDIO_CMD_MSG_SEND_DSP_CMD      = 0x00,
    APP_AUDIO_CMD_MSG_SEND_DSP_DATA     = 0x01,

    APP_AUDIO_CMD_MSG_TOTAL,
} T_APP_AUDIO_CMD_MSG;

#define AUDIO_MP_DUAL_MIC_L_PRI_ENABLE    0x01
#define AUDIO_MP_DUAL_MIC_L_SEC_ENABLE    0x02
#define AUDIO_MP_DUAL_MIC_R_PRI_ENABLE    0x04
#define AUDIO_MP_DUAL_MIC_R_SEC_ENABLE    0x08

/**
    * @brief  audio module init
    * @param  void
    * @return void
    */
void app_audio_init(void);

/**
    * @brief  audio set tts path by BR/EDR or BLE link.
    * @param  path: BR/EDR or BLE link
    * @return void
    */
void app_audio_set_tts_path(uint8_t path);

/**
    * @brief  update a2dp play status
    * @param  a2dp stream event
    * @return void
    */
void app_audio_a2dp_play_status_update(T_APP_A2DP_STREAM_EVENT event);

/**
    * @brief  set new avrcp play status
    * @param  new avrcp play status
    * @return void
    */
void app_audio_set_avrcp_status(uint8_t status);

/**
    * @brief  audio check mic mute or not
    * @param  void
    * @return true  mic mute.
    * @return false mic unmute.
    */
uint8_t app_audio_is_mic_mute(void);

/**
    * @brief  audio do mic switch
    * @param  mic switch mode
    *         0: for cycle switch;
    *         1: mic test + effect off, no swap
    *         2: mic test + effect off, swap
    *         3: normal mode
    * @return uint8_t mic switch mode
    */
uint8_t app_audio_mic_switch(uint8_t param);

/**
    * @brief  audio clear mic mute status
    * @param  void
    * @return void
    */
void app_audio_clear_mic_mute(void);

/**
    * @brief  audio check if mic mute is allowed
    * @param  void
    * @return bool enable mic mute
    */
bool app_audio_check_mic_mute_enable(void);

/**
    * @brief  play unmute tone status be set.
    * @param  play unmute tone enable or not;
    * @return void
    */
void app_audio_enable_play_mic_unmute_tone(bool enable);

/**
    * @brief  judge if play unmute tone.
    * @param  void
    * @return true means enable play mic unmute tone, otherwise not enable.
    */
bool app_audio_is_enable_play_mic_unmute_tone(void);

/**
    * @brief  play mute tone status be set.
    * @param  play mute tone enable or not;
    * @return void
    */
void app_audio_enable_play_mic_mute_tone(bool enable);

/**
    * @brief  judge if play mute tone.
    * @param  void
    * @return true means enable play mic mute tone, otherwise not enable.
    */
bool app_audio_is_enable_play_mic_mute_tone(void);

/**
    * @brief  flush all the tones that has not been played.
    * @param  relay need to relay(true) or not relay(false) for the flush
    * @return void
    */
void app_audio_tone_flush(bool relay);

/**
    * @brief  cancel specific tone in queue.
    *
    * @param  tone_type  The tone type of the queued tone.
    * @param  relay      Relay the tone canceling action to other remote identical devices.
    * @return the resault of canceling tone.
    */
bool app_audio_tone_type_cancel(T_APP_AUDIO_TONE_TYPE tone_type, bool relay);

/**
    * @brief  Play a tone.
    *
    * @param  tone_type  The type of the playing tone.
    * @param  flush     Clear preceding ringtone, VP, and TTS notifications in queue.
    * @param  relay     Relay the tone to other remote identical devices.
    * @return true if is tone was played successfully, otherwise return false.
    */
bool app_audio_tone_type_play(T_APP_AUDIO_TONE_TYPE tone_type, bool flush, bool relay);

/**
    * @brief  Stop a tone.
    *
    * @param  void
    * @return true if is tone was stoped successfully, otherwise return false.
    */
bool app_audio_tone_type_stop(void);

/**
    * @brief  sync stop a tone.
    *
    * @param  void
    * @return true if is tone sync stoped successfully, otherwise return false.
    */
bool app_audio_tone_stop_sync(T_APP_AUDIO_TONE_TYPE tone_type);

/**
    * @brief  set mic mute status.
    *
    * @param  status  audio set mic is mute or unmute.
    * @return void.
    */
void app_audio_set_mic_mute_status(uint8_t status);

/**
    * @brief  check if enable circular volume up or not.
    *
    * @param  void.
    * @return true  enable circular volume up.
    * @return false not enable circular volume up.
    */
bool app_audio_is_circular_volume_up(void);

/**
    * @brief  get latency value by audio track format and latency level
    *
    * @param  mode            Stream mode
    * @param  format_type     The type of the audio format
    * @param  latency_level   audio track latency level
    * @param  latency         audio track latency value
    * @return void
    */
void app_audio_get_latency_value_by_level(T_AUDIO_STREAM_MODE mode, T_AUDIO_FORMAT_TYPE format_type,
                                          uint8_t latency_level, uint16_t *latency);
/**
    * @brief  get last used latency value
    *
    * @param  latency         audio track latency value
    * @return void
    */
void app_audio_get_last_used_latency(uint16_t *latency);

/**
    * @brief  set latency value by audio track format and latency level
    *
    * @param  p_handle        audio track handle
    * @param  format_type     The type of the audio format
    * @param  latency_level   audio track latency level
    * @param  latency         audio track latency value
    * @param  latency_fixed   if the latency is fixed
    * @return void
    */
uint16_t app_audio_set_latency(T_AUDIO_TRACK_HANDLE p_handle, uint8_t latency_level,
                               bool latency_fixed);

uint16_t app_audio_update_audio_track_latency(T_AUDIO_TRACK_HANDLE p_handle, uint8_t latency_level);
void app_audio_report_low_latency_status(uint16_t latency_value);
void app_audio_update_latency_record(uint16_t latency_value);

/**
    * @brief  set max or min vol from phone flag.
    *
    * @param  flag  adjust max or min vol from phone or not.
    * @return void.
    */
void app_audio_set_max_min_vol_from_phone_flag(bool status);

/**
    * @brief  app audio action when roleswap
    * @param  action audio action
    * @return void
    */
void app_audio_action_when_roleswap(uint8_t action);

/* @brief  get bud_stream_state
*
* @param  void
* @return bud_stream_state
*/
T_APP_BUD_STREAM_STATE app_audio_get_bud_stream_state(void);

/* @brief  set bud_stream_state
*
* @param  bud_stream_state
* @return none
*/
void app_audio_set_bud_stream_state(T_APP_BUD_STREAM_STATE state);

/* @brief  set app_db.connected_tone_need and relay to sec if rws connected
*
* @param  need
* @return void
*/
void app_audio_set_connected_tone_need(bool need);

void app_audio_speaker_channel_set(T_AUDIO_CHANCEL_SET set_mode, uint8_t spk_channel);

T_AUDIO_MP_DUAL_MIC_SETTING app_audio_mp_dual_mic_setting_check(uint8_t *ptr);
void app_audio_mp_dual_mic_switch_action(void);
uint8_t app_audio_get_mp_dual_mic_setting(void);
void app_audio_set_mp_dual_mic_setting(uint8_t param);
void app_audio_vol_set(T_AUDIO_TRACK_HANDLE track_handle, uint8_t vol);

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
void app_audio_notification_vol_relay(uint8_t current_volume, bool first_sync, bool report_flag);
void app_audio_notification_vol_set_remote_data(T_NOTIFICATION_VOL_RELAY_MSG *rcv_msg);
void app_audio_notification_vol_cmd_handle(uint16_t volume_cmd, uint8_t *param_ptr,
                                           uint16_t param_len, uint8_t path, uint8_t app_idx);
#endif

void app_audio_in_ear_handle(void);
void app_audio_out_ear_handle(void);
uint8_t app_audio_get_tone_index(T_APP_AUDIO_TONE_TYPE tone_type);
bool app_audio_buffer_level_get(uint16_t *level);

void app_audio_spk_mute_unmute(bool need);
void app_audio_track_spk_unmute(T_AUDIO_STREAM_TYPE stream_type);
bool app_audio_mute_status_sync(uint8_t *bd_addr, T_AUDIO_STREAM_TYPE stream_type);
void app_audio_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt);
void app_audio_remote_join_set(bool enable);
bool app_audio_get_a2dp_active(void);
void app_audio_restart_track(void);

#if F_APP_COMMON_DONGLE_SUPPORT
void app_audio_update_dongle_flag(bool is_dongle);
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
uint8_t app_audio_a2dp_stream_resume(uint8_t *audio_cfg);
#endif

uint8_t app_audio_get_dongle_flag(void);
void app_audio_update_in_ear_recover_a2dp(bool flag);
void app_audio_update_detect_suspend_by_out_ear(bool flag);
void app_audio_action_when_bud_loc_changed(uint8_t action);
bool app_audio_call_transfer_check(void);

bool app_audio_get_seg_send_status(void);

void harman_push_max_tone_when_not_playing(T_AUDIO_STREAM_TYPE stream_type, uint8_t pre_volume,
                                           uint8_t cur_volume);

/** @} */ /* End of group APP_AUDIO_Exported_Functions */

/** End of APP_AUDIO
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUDIO_POLICY_H_ */
