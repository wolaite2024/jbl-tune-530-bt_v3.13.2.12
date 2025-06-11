/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_ADP_H_
#define _APP_ADP_H_

#include <stdbool.h>
#include "app_charger.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_ADP APP Adaptor
  * @brief APP Adaptor Command
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup AUDIO_SMART_ADP_COMMAND_Exported_Macros Audio smart adp command Macros
    * @{
    */

/******************************************************************************/

#define ADP_PAYLOAD_AVP_OPEN       0x55        // avp open
#define ADP_PAYLOAD_AVP_CLOSE      0x65        // avp close

typedef enum
{
    ADAPTOR_UNPLUG,
    ADAPTOR_PLUG
} T_ADAPTOR_STATUS;

/******************************************************************************/
/** End of AUDIO_SMART_ADP_COMMAND_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup AUDIO_SMART_ADP_COMMAND_Exported_Types Audio smart adp command Types
    * @{
    */

/** @} */ /* End of group AUDIO_SMART_ADP_COMMAND_Exported_Types */


/** @defgroup APP_ADP_Exported_Functions Audio smart adp command functions
    * @{
    */

/******************************************************************************/

/**
    * @brief  adp detect init
    * @param  void
    * @return void
    */
void app_adp_init(void);

/**
    * @brief  adp io plug out.
    * @param  void
    * @return void
    */
void app_adp_io_plug_out(void);

/**
    * @brief  set adp io wake up polarity.
    * @param  void
    * @return void
    */
void app_adp_io_wakeup_pol_set(void);

#if F_APP_LOCAL_PLAYBACK_SUPPORT
/**
    * @brief  adp usb start
    * @param  void
    * @return void
    */
void app_adp_usb_start_handle(void);
/**
    * @brief  adp usb stop
    * @param  void
    * @return void
    */
void app_adp_usb_stop_handle(void);
#endif

void app_adp_set_disallow_enable_charger(bool flag);

/**
    * @brief  smart box charger control
    * @param  enable whether enable charger
    * @return void
    */
void app_adp_smart_box_charger_control(bool enable);

/**
    * @brief  check whether adaptor is pluged or not.
    * @param  void
    * @return @ref T_ADAPTOR_STATUS.
    */
uint8_t app_adp_get_plug_state(void);

/**
    * @brief  check adaptor plug/unplug state
    * @param  void
    * @return none
    */
void app_adp_detect(void);
void app_adp_msg_handler(uint16_t type);
void app_adp_set_disable_charger_by_box_battery(bool flag);

/******************************************************************************/
/** @} */ /* End of group APP_ADP_Exported_Functions */

/** @} */ /* End of group APP_ADP */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ADP_DAT_H_ */
