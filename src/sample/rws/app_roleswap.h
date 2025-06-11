/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_ROLESWAP_H_
#define _APP_ROLESWAP_H_

#include <stdint.h>
#include <stdbool.h>
#include "remote.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_ROLESWAP App Roleswap
  * @brief Roleswap implementation for sample project
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_ROLESWAP_Exported_Macros App Roleswap Macros
   * @{
   */
#define A2DP_DISALLOW_DO_LOW_BATTERY_ROLESWAP   0
#define SCO_DISALLOW_DO_LOW_BATTERY_ROLESWAP    1

/** End of APP_ROLESWAP_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_ROLESWAP_Exported_Types App Roleswap Types
    * @{
    */

/**  @brief Bud Battery */
typedef enum
{
    BUD_BATT_MAX = 100,
    BUD_BATT_HIGH  = 70,
    BUD_BATT_LOW  = 30,
    BUD_BATT_SHUTDOWN  = 5,
    BUD_BATT_MIN = 0,
    BUD_BATT_DIFF_THRESHOLD = 10,
    BUD_BATT_PRI_ROLESWAP_THRESHOLD = 50,
    BUD_BATT_BOTH_ROLESWAP_THRESHOLD = 20,
} T_BUD_BATTERY_THRESHOLD;

/**  @brief  App define roleswap event */
typedef enum
{
    ROLESWAP_EVENT_OTHER_GROUP = 0x00,
    ROLESWAP_EVENT_EAR_ROLESWAP = 0x01,
    ROLESWAP_EVENT_POWEROFF = 0x02,
    ROLESWAP_EVENT_LINK_MONITOR = 0x03,

    ROLESWAP_EVENT_CHARGERBOX_GROUP = 0x04,
    ROLESWAP_EVENT_OTHER_SYNC_GROUP = 0x08,
    /*0xE0~0xFF is reseverd,don't use it*/
    ROLESWAP_EVENT_MAX,
} T_ROLESWAP_EVENT;

/**  @brief  App define bud mute state */
typedef enum
{
    BUD_NOT_MUTED   = 0x00,
    BUD_AUDIO_MUTED = 0x01,
    BUD_VOICE_MUTED = 0x10,
    BUD_AUDIO_VOICE_MUTED = 0x11
} T_APP_BUD_MUTE_STATE;

typedef enum
{
    APP_RS_TRIGGER_CHECK_EVENT_NONE,
    APP_RS_TRIGGER_CHECK_EVENT_INBOX,
    APP_RS_TRIGGER_CHECK_EVENT_BATTERY,
    APP_RS_TRIGGER_CHECK_EVENT_PENDING,
    APP_RS_TRIGGER_CHECK_EVENT_EAR,
    APP_RS_TRIGGER_CHECK_EVENT_POWEROFF,
    APP_RS_TRIGGER_CHECK_EVENT_LINK_MONITOR,
} T_RS_TRIGGER_CHECK_EVENT;

/** End of APP_ROLESWAP_Exported_Types
    * @}
    */
#if F_APP_ERWS_SUPPORT
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_ROLESWAP_Exported_Functions App Roleswap Functions
    * @{
    */
/* @brief  app receive roleswap event, devide group and deliver into sub state machine
*
* @param  event roleswap event
* @param  frome_remote event source
* @param  para parameter if needed
* @return none
*/
void app_roleswap_state_machine(uint8_t event, bool from_remote, uint8_t para);

/* @brief  primary request battery level from secondary
*
* @param  void
* @return none
*/
void app_roleswap_req_battery_level(void);

/* @brief  roleswap module init
*
* @param  void
* @return none
*/
void app_roleswap_init(void);

/* @brief  send event to remote
*
* @param  event event type
* @param  param parameter to be send
* @param  para_len length of param
* @return none
*/
void app_roleswap_event_info_remote(uint8_t event, uint8_t *param, uint8_t para_len);

/**
    * @brief  handle sync power off.
    * @param  void
    * @return void
    */
void app_roleswap_sync_poweroff(void);

/**
    * @brief  handle poweroff for sync / unsync(handle roleswap then power off if it needs).
    * @param  is_batt
    * @return void
    */
void app_roleswap_poweroff_handle(bool is_bat_0_to_power_off);

/**
    * @brief  call transfer check.
    * @param  void
    * @return void
    */
bool app_roleswap_call_transfer_check(uint8_t event);

/** @} */ /* End of group APP_ROLESWAP_Exported_Functions */

/** End of APP_ROLESWAP
* @}
*/

#else
#define app_roleswap_state_machine(event, from_remote, para)    while(0){}
#define app_roleswap_req_battery_level()    while(0){}
#define app_roleswap_check()    ROLESWAP_DISALLOW
#define app_roleswap_process_after_check(result, event) while(0){}
#define app_roleswap_event_info_remote(event, param, para_len)  while(0){}
#define app_roleswap_get_trigger_event()   APP_ROLESWAP_TRIGGER_EVENT_NONE

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ROLESWAP_H_ */
