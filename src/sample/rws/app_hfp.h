/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_HFP_H_
#define _APP_HFP_H_

#include "bt_hfp.h"
#include "app_link_util.h"
#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_HFP App Hfp
  * @brief this file handle hfp profile related process
  * @{
  */

typedef enum
{
    APP_REMOTE_HFP_SYNC_SCO_GAIN_WHEN_CALL_ACTIVE,
    APP_REMOTE_HFP_AUTO_ANSWER_CALL_TIMER,

    APP_REMOTE_HFP_TOTAL,
} T_APP_HFP_REMOTE_MSG;

/**  @brief  caller id type
  */
typedef enum
{
    CALLER_ID_NUMBER = 0x00,
    CALLER_ID_NAME = 0x01
} T_CALLER_ID_TYPE;

typedef enum
{
    APP_SCO_ADJUST_ACL_CONN_IND_EVT = 0x00,
    APP_SCO_ADJUST_ACL_CONN_END_EVT = 0x01,
    APP_SCO_ADJUST_LINKBACK_EVT = 0x02,
    APP_SCO_ADJUST_LINKBACK_END_EVT = 0x03,
    APP_SCO_ADJUST_SCO_CONN_IND_EVT = 0x04,
    APP_SCO_ADJUST_SCO_CONN_CMPL_EVT = 0x05,

    APP_SCO_ADJUST_EVT_TOTAL,
} T_APP_HFP_SCO_ADJUST_EVT;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_HFP_Exported_Functions App Hfp Functions
    * @{
    */
/**
    * @brief  hfp module init.
    * @param  void
    * @return void
    */
void app_hfp_init(void);

/**
    * @brief  get current call status.
    * @param  void
    * @return @ref T_APP_HFP_CALL_STATUS
    */
T_APP_CALL_STATUS app_hfp_get_call_status(void);

/**
    * @brief  set current call status.
    * @param  T_APP_HFP_CALL_STATUS
    * @return void
    */
void app_hfp_set_call_status(T_APP_CALL_STATUS call_status);

/**
    * @brief  set no service timer.
    * @param  all service status
    * @return void
    */
void app_hfp_set_no_service_timer(bool all_service_status);

/**
    * @brief  get active hfp connection br link id.
    * @param  void
    * @return link id
    */
uint8_t app_hfp_get_active_idx(void);

/**
    * @brief  set voice gain by link .
    * @param  bd_addr active link bt device address
    * @param  gain : voice gain
    * @return true if sco gain is set
    */
bool app_hfp_set_link_voice_gain(uint8_t *addr, uint8_t gain);

/**
    * @brief  set active hfp connection br link .
    * @param  bd_addr active link bt device address
    * @return link id
    */
bool app_hfp_set_active_idx(uint8_t *bd_addr);

/**
    * @brief  get sco connect status.
    * @param  void
    * @return true for sco connected
    */
bool app_hfp_sco_is_connected(void);

/**
    * @brief  control hfp mic mute.
    * @param  void
    * @return void
    */
void app_hfp_mute_ctrl(void);

/**
    * @brief  Send a command to report current battery level.
    * @param  bd_addr Remote BT address.
    * @return void
    */
void app_hfp_batt_level_report(uint8_t *bd_addr);

/**
    * @brief  inband tone mute ctrl.
    * @param  void
    * @return void
    */
void app_hfp_inband_tone_mute_ctrl(void);

/**
    * @brief  always playing outband tone.
    * @param  void
    * @return void
    */
void app_hfp_always_playing_outband_tone(void);

/**
    * @brief  update call status.
    * @param  void
    * @return void
    */
void app_hfp_update_call_status(void);

/**
    * @brief  stop ring alert timer.
    * @param  void
    * @return void
    */
void app_hfp_stop_ring_alert_timer(void);

/**
    * @brief  get call in tone type.
    * @param  p_link
    * @return call in tone type
    */
uint8_t app_hfp_get_call_in_tone_type(T_APP_BR_LINK *p_link);

/**
    * @brief  set active hfp index.
    * @param  uint8_t index
    * @return void
    */
void app_hfp_only_set_active_idx(uint8_t idx);

/**
    * @brief  hfp related command handler
    * @param  cmd_ptr: hfp command
    * @param  cmd_len: length of command
    * @param  cmd_path: command path
    * @param  app_idx: link index
    * @param  ack_pkt: ack status
    * @return void
    */
void app_hfp_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                        uint8_t *ack_pkt);

/**
    * @brief  hfp dynamic adjust sco window
    * @param  bd_addr: address of the device bud is paging or connects acl/sco with bud
    * @param  evt: trigger event of adjusting sco window
    * @return void
    */
void app_hfp_adjust_sco_window(uint8_t *bd_addr, T_APP_HFP_SCO_ADJUST_EVT evt);

/**
    * @brief  hfp judge whether transfer call is needed at the call status
    * @param  void
    * @return bool
    */
bool app_hfp_sco_is_need(void);

void app_hfp_set_vol_is_changed(void);

/*
    * @brief  stop delay role switch timer.
    * @param  void
    * @return void
    */
void app_hfp_stop_delay_role_switch(void);

/*
    * @brief  vol change ongoing set.
    * @param  vol_change_ongoing
    * @return void
    */
void app_hfp_set_vol_change_ongoing(bool vol_change_ongoing);

uint8_t app_hfp_hfp_ring_timer_check(void);

/** @} */ /* End of group APP_HFP_Exported_Functions */
/** End of APP_HFP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HFP_H_ */
