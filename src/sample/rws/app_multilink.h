/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_MULTILINK_H_
#define _APP_MULTILINK_H_

#include <stdbool.h>
#include <stdint.h>
#include "app_roleswap.h"
#include "app_relay.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_MULTILINK App Multilink
  * @brief App Multilink
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_MULTILINK_Exported_Macros App Multilink Macros
   * @{
   */
#define MULTILINK_SRC_CONNECTED         2
#define TIMER_TO_DISCONN_ACL            800
/** End of APP_MULTILINK_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_MULTILINK_Exported_Types App Multilink Types
    * @{
    */
/** @brief Multilink handle connect, disconnect, related player status event */
typedef enum
{
    JUDGE_EVENT_A2DP_CONNECTED,
    JUDGE_EVENT_MEDIAPLAYER_PLAYING,
    JUDGE_EVENT_MEDIAPLAYER_PAUSED,
    JUDGE_EVENT_A2DP_START,
    JUDGE_EVENT_A2DP_DISC,
    JUDGE_EVENT_DSP_SILENT,
    JUDGE_EVENT_A2DP_STOP,
    JUDGE_EVENT_SNIFFING_STOP,

    JUDGE_EVENT_TOTAL
} T_APP_JUDGE_A2DP_EVENT;

typedef enum
{
    APP_REMOTE_MSG_PROFILE_CONNECTED,
    APP_REMOTE_MSG_PHONE_CONNECTED,
    APP_REMOTE_MSG_MUTE_AUDIO_WHEN_MULTI_CALL_NOT_IDLE,
    APP_REMOTE_MSG_UNMUTE_AUDIO_WHEN_MULTI_CALL_IDLE,
    APP_REMOTE_MSG_RESUME_BT_ADDRESS,
    APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC,
    APP_REMOTE_MSG_CONNECTED_TONE_NEED,

    APP_REMOTE_MSG_MULTILINK_TOTAL
} T_MULTILINK_REMOTE_MSG;

/** @brief Multilink update active a2dp link, delay role switch timer */

/** End of APP_MULTILINK_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_MULTILINK_Exported_Functions App Multilink Functions
    * @{
    */
/**
    * @brief  multilink module init
    * @param  void
    * @return void
    */
void app_multi_init(void);

/**
    * @brief  set active a2dp link index
    * @param  bd_addr
    * @return bool
    */
bool app_multi_a2dp_active_link_set(uint8_t *bd_addr);

/**
    * @brief  check if a2dp play status needs to update
    * @param  event judge a2dp active link event
    * @return void
    */
void app_multi_update_a2dp_play_status(T_APP_JUDGE_A2DP_EVENT event);

/**
    * @brief  judge one device a2dp link as active link and avrcp can control it.
    *         config BT qos when multilink connect or disconnect.
    * @param  app_idx BT link index
    * @param  event judge a2dp active link event
    * @return void
    */
void app_multi_judge_active_a2dp_idx_and_qos(uint8_t app_idx, T_APP_JUDGE_A2DP_EVENT event);

void app_multi_disconnect_inactive_link(uint8_t app_idx, uint8_t input_link_type);
void app_multi_reconnect_inactive_link(void);
void app_multi_roleswap_disconnect_inactive_link(void);

bool app_multi_active_link_check(uint8_t *bd_addr);

void app_multi_stream_avrcp_set(uint8_t idx);

void app_multi_handle_sniffing_link_disconnect_event(uint8_t id);

bool app_multi_pause_inactive_a2dp_link_stream(uint8_t keep_active_a2dp_idx, uint8_t resume_fg);

void app_multi_resume_a2dp_link_stream(uint8_t app_idx);

void app_bond_set_priority(uint8_t *bd_addr);
bool app_multi_check_in_sniffing(void);

void app_multi_switch_by_mmi(bool is_on_by_mmi);
bool app_multi_is_on_by_mmi(void);

void app_multilink_sco_preemptive(uint8_t inactive_idx);
uint8_t app_multi_find_inacitve(uint8_t idx);
uint8_t app_multi_get_acl_connect_num(void);
void app_multi_stop_acl_disconn_timer(void);
void app_multi_active_hfp_transfer(uint8_t idx, bool disc, bool force);

void app_multi_set_active_idx(uint8_t idx);
uint8_t app_multi_get_active_idx(void);

bool app_multi_sass_bitmap(uint8_t app_idx, uint8_t profile, bool a2dp_check);
bool app_multi_get_stream_only(uint8_t idx);
bool app_multi_check_pend(void);
bool app_multi_checking_silence_pkts(void);
uint8_t app_multi_get_pending_id(void);
void app_multi_stop_silence_check_timer(void);
void app_multi_set_stream_only(uint8_t idx, bool streaming);
void app_multi_voice_ongoing_preemptive(uint8_t disable_idx, uint8_t idx);
/** @} */ /* End of group APP_MULTILINK_Exported_Functions */

/** End of APP_MULTILINK
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_MULTILINK_H_ */
