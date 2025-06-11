/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_MMI_H_
#define _APP_MMI_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_MMI App MMI
  * @brief App MMI
  * @{
  */

/**  @brief  action ID parameter for @ref app_mmi_handle_action */
typedef enum
{
    MMI_NULL = 0x00,

    //HFP MMI
    MMI_HF_END_OUTGOING_CALL = 0x02,
    MMI_HF_ANSWER_CALL = 0x03,
    MMI_HF_REJECT_CALL = 0x04,
    MMI_HF_END_ACTIVE_CALL = 0x05,

    MMI_HF_INITIATE_VOICE_DIAL = 0x09,
    MMI_HF_CANCEL_VOICE_DIAL = 0x0A,
    MMI_HF_LAST_NUMBER_REDIAL = 0x0B,
    MMI_HF_SWITCH_TO_SECOND_CALL = 0x0C,
    MMI_HF_TRANSFER_CALL = 0x0D,
    MMI_HF_QUERY_CURRENT_CALL_LIST = 0x0E,
    MMI_HF_JOIN_TWO_CALLS = 0x0F,
    MMI_HF_RELEASE_HELD_OR_WAITING_CALL = 0x10,
    MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL = 0x11,

    //AV MMI
    MMI_AV_PLAY_PAUSE = 0x32,
    MMI_AV_STOP = 0x33,
    MMI_AV_FWD = 0x34,
    MMI_AV_BWD = 0x35,
    MMI_AV_FASTFORWARD = 0x36,
    MMI_AV_REWIND = 0x37,
    MMI_AV_FASTFORWARD_STOP = 0x3B,
    MMI_AV_REWIND_STOP = 0x3C,
    MMI_AV_PAUSE = 0xF8,
    MMI_AV_RESUME = 0xF9,

    //DEV MMI
    MMI_DEV_MIC_MUTE_UNMUTE = 0x06, //From av or hf mmi,keep it's original value
    MMI_DEV_MIC_MUTE = 0x07,
    MMI_DEV_MIC_UNMUTE = 0x08,
    MMI_DEV_MIC_VOL_UP = 0x15,
    MMI_DEV_MIC_VOL_DOWN = 0x16,
    MMI_DEV_MIC_SWITCH = 0x6A,
    MMI_DEV_SPK_VOL_UP = 0x30,
    MMI_DEV_SPK_VOL_DOWN = 0x31,
    MMI_DEV_SPK_MUTE = 0x3D,
    MMI_DEV_SPK_UNMUTE = 0x3E,

    MMI_DEV_ENTER_PAIRING_MODE = 0x51,
    MMI_DEV_EXIT_PAIRING_MODE = 0x52,
    MMI_DEV_LINK_LAST_DEVICE = 0x53,

    MMI_DEV_POWER_ON = 0x54,
    MMI_DEV_TOOLING_FACTORY_RESET = 0x55,
    MMI_DEV_POWER_OFF = 0x56,
    MMI_DEV_CFG_FACTORY_RESET = 0x57,
    MMI_DEV_FACTORY_RESET = 0x58,
    MMI_DEV_DISCONNECT_ALL_LINK = 0x59,
    MMI_DEV_FACTORY_RESET_BY_SPP = 0x5a,
    MMI_DEV_PHONE_RECORD_RESET_BY_SPP = 0x5b,
    MMI_ENTER_DUT_FROM_SPP = 0x5c,

    MMI_AUDIO_EQ_NEXT = 0x60,
    MMI_AUDIO_EQ_PREVIOUS = 0x61,
    MMI_DEV_NFC_DETECT = 0x62,
    MMI_AUDIO_APT = 0x65,
    MMI_DEV_NEXT_VOICE_PROMPT_LANGUAGE = 0x66,

    MMI_OUTPUT_INDICATION1_TOGGLE = 0x67,
    MMI_OUTPUT_INDICATION2_TOGGLE = 0x68,
    MMI_OUTPUT_INDICATION3_TOGGLE = 0x69,

    MMI_AUDIO_EQ_SWITCH = 0x6B,
    MMI_DEV_FORCE_ENTER_PAIRING_MODE = 0x6C,
    MMI_START_ROLESWAP = 0x6E,
    MMI_DEV_RESET_TO_UNINITIAL_STATE = 0x6F,
    MMI_TAKE_PICTURE = 0x70,
    MMI_AUDIO_SWITCH_CHANNEL = 0x74,

    //must follow public version function defination
    /*
    #if BISTO_FEATURE_SUPPORT == 1
        MMI_BISTO_FETCH = 0x75,
        MMI_BISTO_PTT = 0x76,
    #endif
    */

    MMI_DEV_PHONE_RECORD_RESET = 0x76,
    MMI_MEMORY_DUMP  = 0x77,

    MMI_DEV_GAMING_MODE_SWITCH = 0x80,

    MMI_AUDIO_ANC_ON = 0x81,
    MMI_AUDIO_APT_ON = 0x82,
    MMI_AUDIO_ANC_APT_ALL_OFF = 0x83,

    MMI_AUDIO_VAD_OPEN = 0x87,
    MMI_AUDIO_VAD_CLOSE = 0x88,

    MMI_AUDIO_DUAL_EFFECT_SWITCH = 0x89,
    MMI_AUDIO_DUAL_EFFECT_SPACE_SWITCH = 0x8A,

    MMI_AUDIO_APT_EQ_SWITCH = 0x91,
    MMI_AUDIO_APT_VOL_UP = 0x92,
    MMI_AUDIO_APT_VOL_DOWN = 0x93,
    MMI_AUDIO_APT_VOICE_FOCUS = 0x94,
    MMI_AUDIO_APT_DISALLOW_SYNC_VOLUME_TOGGLE = 0x95,
    MMI_DEV_MULTILINK_ON = 0x96,
    MMI_DEV_DONGLE_DUAL_MODE_SWITCH = 0x97,
    MMI_24G_BT_AUDIO_SOURCE_SWITCH = 0x98,
    MMI_DEV_DONGLE_SWITCH_PAIRING = 0x99,

    MMI_OTA_OVER_BLE_START = 0xA0,
    MMI_DUT_TEST_MODE = 0xA1,
    MMI_DEV_KEY_REMAP_LOCK_TOGGLE = 0xA2,
#if F_APP_TEAMS_FEATURE_SUPPORT
    MMI_MS_TEAMS_BUTTON_SHORT_PRESS = 0xA9,
    MMI_MS_TEAMS_BUTTON_LONG_PRESS = 0xAA,
#endif
#if F_APP_FINDMY_FEATURE_SUPPORT
    MMI_FINDMY_PUT_SERIAL_NUMBER_STATE = 0xAB,
#endif
    MMI_ALEXA_WAKEWORD = 0xB0,
    MMI_XIAOAI_WAKEWORD = 0xB2,

    MMI_BISTO_FETCH = 0xB3,
    MMI_BISTO_PTT   = 0xB4,

    MMI_XIAOWEI_START_RECORDING = 0xB5,
    MMI_XIAOWEI_STOP_RECORDING = 0xB6,
    MMI_GFPS_FINDER_STOP_RING   = 0xB8,
    MMI_GFPS_LEA_ENTER_PAIR_MODE = 0xB9,
    MMI_GFPS_LEA_EXIT_PAIR_MODE  = 0xBA,
    MMI_DULT_ENTER_ID_READ_STATE = 0xBB,
    MMI_GFPS_FINDER_STOP_ADV     = 0xBC,

    MMI_SD_PLAYBACK_SWITCH              = 0xC0,
    MMI_SD_PLAYPACK_PAUSE               = 0xC1,
    MMI_SD_PLAYBACK_FWD                 = 0xC2,
    MMI_SD_PLAYBACK_BWD                 = 0xC3,
    MMI_SD_PLAYBACK_FWD_PLAYLIST        = 0xC4,
    MMI_SD_PLAYBACK_BWD_PLAYLIST        = 0xC5,
    MMI_SAVE_WHILE_LISTENING_ON_OFF     = 0xC6,

    MMI_ANC_ON_OFF = 0xD0,
    MMI_LISTENING_MODE_CYCLE = 0xD1,
    MMI_ANC_CYCLE = 0xD2,
    MMI_LIGHT_SENSOR_ON_OFF = 0xD3,
    MMI_AIRPLANE_MODE = 0xD4,
    MMI_LLAPT_BRIGHTNESS_UP = 0xD5,
    MMI_LLAPT_BRIGHTNESS_DOWN = 0xD6,
    MMI_LLAPT_CYCLE = 0xD7,
    MMI_HA_PROG_CYCLE = 0xD8,
    MMI_DEFAULT_LISTENING_MODE_CYCLE = 0xD9,
    MMI_ANC_APT_ON_OFF = 0xDA,
    MMI_SWITCH_APT_TYPE = 0xDB,

    MMI_BIG_START   = 0xE0,
    MMI_BIG_STOP    = 0xE1,

    MMI_ENTER_SINGLE_MODE = 0xF1,
    MMI_A2DP_FOCUS_TOGGLE = 0xFA,

    MMI_TOTAL
} T_MMI_ACTION;

#define REBOOT_CHECK_MAX_TIMES                      56
#define REBOOT_CHECK_OTA_TOOLING_MAX_TIMES          1
#define REBOOT_CHECK_POWER_ON_MAX_TIMES             3

/**
    * @brief  App need to process key message are handle in this function.
    *         User trigger the specific key process, then bbpro would send le advertising data.
    * @param  action key event
    * @return void
    */
void app_mmi_handle_action(uint8_t action);

/**
    * @brief  mmi switch  gaming mode
    * @param  void
    * @return void
    */
void app_mmi_switch_gaming_mode(void);

void app_mmi_init(void);

/**
    * @brief  In rws condition, MMI need to sync or not.
    * @param  action key event
    * @return void
    */
bool app_mmi_is_local_execute(uint8_t action);

/**
    * @brief  reboot check timer started or not.
    * @param  void
    * @return timer started or not
    */
bool app_mmi_reboot_check_timer_started(void);

/**
    * @brief  modify reboot check timers.
    * @param  times reboot check times
    * @return void
    */
void app_mmi_modify_reboot_check_times(uint32_t times);

/**
    * @brief  check allow do factory reset or not.
    * @param  void
    * @return true: allow; flase: disallow
    */
bool app_mmi_is_allow_factory_reset(void);

/**
    * @brief start reboot check timer.
    * @param  period_timer
    * @return void
    */
void app_mmi_reboot_check_timer_start(uint32_t period_timer);

/**
    * @brief reject call.
    * @param  void
    * @return void
    */
void app_mmi_hf_reject_call(void);

/**
    * @brief cancel voice dial.
    * @param  void
    * @return void
    */
void app_mmi_hf_cancel_voice_dial(void);

/**
    * @brief end outgoing call.
    * @param  void
    * @return void
    */
void app_mmi_hf_end_outgoing_call(void);

/**
    * @brief end active call.
    * @param  void
    * @return void
    */
void app_mmi_hf_end_active_call(void);


/** End of APP_MMI
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_MMI_H_ */
