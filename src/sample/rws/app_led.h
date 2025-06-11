/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_LED_H_
#define _APP_LED_H_

#include "led_module.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_LED App Led
  * @brief app led module implementation for audio sample project
  * @{
  */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_LED_Exported_Types App Led Types
    * @{
    */
/**  @brief  Define led different mode */
typedef enum t_led_mode
{
    /**<Repeat LED Mode*/
    LED_MODE_DONGLE_MODE,                   //0x00
    LED_MODE_RSV2,                          //0x01

    LED_MODE_LINK_BACK,                     //0x02
    LED_MODE_STANDBY,                       //0x03
    LED_MODE_PAIRING,                       //0x04
    LED_MODE_CONNECTED_SINGLE_LINK,         //0x05
    LED_MODE_CONNECTED_MULTI_LINK,          //0x06

    LED_MODE_INCOMING_CALL,                 //0x07
    LED_MODE_TALKING,                       //0x08
    LED_MODE_MIC_MUTE,                      //0x09
    LED_MODE_AUDIO_PLAYING,                 //0x0A
    LED_MODE_APT_ON,                        //0x0B

    LED_MODE_RWS_CREATION,                  //0x0C  // LED_MODE_LINE_IN also use this
    LED_MODE_OUTGOING_CALL,                 //0x0D
    LED_MODE_BUD_FIRST_ENGAGE_CREATION,     //0x0E
    LED_MODE_ANC_ON,                        //0x0F
    LED_MODE_BUD_ROLE_PRIMARY,              //0x10
    LED_MODE_BUD_ROLE_SECONDARY,            //0x11

    LED_MODE_CHARGING,                      //0x12
    LED_MODE_CHARGING_COMPLETE,             //0x13
    LED_MODE_CHARGING_ERROR,                //0x14

    LED_MODE_DUT_TEST_MODE,                 //0x15

    /**<Non-Repeat LED Mode */
    LED_MODE_GAMING_MODE,                   //0x16
    LED_MODE_POWER_ON,                      //0x17
    LED_MODE_POWER_OFF,                     //0x18
    LED_MODE_PAIRING_COMPLETE,              //0x19
    LED_MODE_FACTORY_RESET,                 //0x1A
    LED_MODE_LOW_BATTERY,                   //0x1B
    LED_MODE_RSV3,                          //0x1C
    LED_MODE_VOL_ADJUST,                    //0x1D
    LED_MODE_VOL_MAX_MIN,                   //0x1E
    LED_MODE_ENTER_PCBA_SHIPPING_MODE,      //0x1F

    /* do not use, means the start point of LED modes from RAM table */
    LED_MODE_RAM_TABLE_START,               //0x20

    /**<Repeat LED Mode, define in RAM */
    LED_MODE_ALL_BLINKING,                  //0x21
    LED_MODE_ALL_OFF,                       //0x22
    LED_MODE_ALL_ON,                        //0x23

    LED_MODE_TOTAL                          //0x24
} T_LED_MODE;

#define LED_INDEX_0             0
#define LED_INDEX_1             1
#define LED_INDEX_2             2
#define LED_INDEX_ANY           3

/** End of APP_LED_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LED_Exported_Functions App Led Functions
    * @{
    */
/**
    * @brief  app LED module init.
    * @param  void
    * @return void
    */
void app_led_init(void);

/**
    * @brief  A decision-making function set before app_led_set_mode.
    *         To make the final decision of LED.
    *         If led change mode is not equal to current led mode, app will set led mode again.
    * @param  change_mode: LED mode which is expected to change to.
    * @param  forced_sec_change: Indicated if LED mode is forced to change to change_mode. Non-repeat modes would set to true.
    *                            If set to false, secondary bud may not change LED mode if its current LED mode is an indepedent mode.
    * @param  relay_to_sec: Indicated if this LED mode needs to sync with secondary bud. Only valid for primary bud.
    * @return change result
    */
bool app_led_change_mode(T_LED_MODE change_mode, bool forced_sec_change, bool relay_to_sec);

/**
    * @brief  Check current led mode. When app task receive any message, app need to call this function.
    *         Set led change mode according to app state and call status.
    *         If led change mode is not equal to current led mode, app will set led mode again.
    * @param  void
    * @return void
    */
void app_led_check_repeat_mode(void);

/**
    * @brief  Check current charging led mode. When app task receive any message, app need to call this function.
    *         Set charging led change mode according to charger state.
    * @param  from_non_repeat_mode now led mode is repeat or not
    * @return void
    */
void app_led_check_charging_mode(bool from_non_repeat_mode);

/**
    * @brief  Check current led are all in off state or not
    *
    * @param  void
    * @return check result
    */
bool app_led_is_all_keep_off(void);

/**
    * @brief  Check if app_led_set_mode can be called. If false, mode will be pending and set after
    *         currenct led blink finished.
    *
    * @param  LED mode
    * @return check result
    */
bool app_led_check_set_mode_available(T_LED_MODE mode);

/**
    * @brief  get led_stop_check state
    *
    * @param  void
    * @return led_stop_check state
    */
uint8_t app_led_get_led_stop_check_state(void);

/**
    * @brief  Stop all of the LED timer when power off function was called
    *
    * @param  void
    * @return NULL
    */
void app_led_timer_stop(void);

/**
    * @brief  Set designated led on
    *
    * @param  uint8_t
    * @return NULL
    */
void app_led_set_designate_led_on(uint8_t led_number);

/**
    * @brief  set designated led off
    *
    * @param  uint8_t
    * @return NULL
    */
void app_led_set_designate_led_off(uint8_t led_number);

/**
    * @brief  check if led idx is used as specify led
    *
    * @param  uint8_t
    * @return NULL
    */
bool app_led_is_specify_led_function(uint8_t led_idx);

/**
    * @brief  disable non repeat mode
    *
    * @param  void
    * @return NULL
    */
void app_led_disable_non_repeat_mode(void);

/**
    * @brief  check if specify led need to update
    *
    * @param  void
    * @return NULL
    */
void app_led_check_specify_function_mode(void);

/**
    * @brief  reset pad config
    *
    * @param  void
    * @return NULL
    */
void app_led_reset_pad_config(void);

/** @} */ /* End of group APP_LED_Exported_Functions */

/** End of APP_LED
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_LED_H_ */
