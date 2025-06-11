/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _GPIO_ON_OFF_H_
#define _GPIO_ON_OFF_H_

#include "rtl876x_gpio.h"
#include "app_msg.h"
#include "app_in_out_box.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup  APP_GPIO_ON_OFF App GPIO On Off
    * @brief GPIO POWER ONOFF module implementation for app sample project
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup GPIO_ON_OFF_Exported_Macros App Gpio Power OnOff Macros
    * @{
    */

/**CHARGERBOX DETECT timer unit, unit : ms*/
#define CHARGERBOX_DETECT_TIMER                  500
#define SMART_CHARGERBOX_DETECT_IN_TIMER_40MS    30
#define SMART_CHARGERBOX_DETECT_IN_TIMER_20MS    10
#define SMART_CHARGERBOX_DETECT_OUT_TIMER        500

/** End of GPIO_ON_OFF_PROCESS_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup GPIO_ON_OFF_PROCESS_Exported_Types App Gpio Power OnOff Types
    * @{
    */


/** End of GPIO_ON_OFF_PROCESS_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup GPIO_ON_OFF_PROCESS_Exported_Functions App Gpio Power OnOff Functions
    * @{
    */

/**
    * @brief  gpio detect driver initial.
    *         Include APB peripheral clock config, GPIO parameter config and
    *         GPIO interrupt mark config. Enable GPIO interrupt.
    *         register gpio detect timer callback queue,device manager callback.
    * @param  void
    * @return void
    */
void app_gpio_on_off_driver_init(uint8_t pin_num);


/**
    * @brief  check gpio detect debounce timer.
    *
    * @param  void
    * @return timer started if true.
    */
bool app_gpio_on_off_debounce_timer_started(void);

/**
    * @brief  Chargerbox detect GPIO interrupt will be handle in this function.
    *         First disable app enter dlps mode and read current Chargerbox detect GPIO input data bit.
    *         Disable Chargerbox detect GPIO interrupt and send IO_GPIO_MSG_TYPE message to app task.
    *         Then enable Chargerbox detect GPIO interrupt.
    * @param  void
    * @return void
    */
void app_gpio_on_off_intr_cb(uint32_t param);

/**
    * @brief  get device location.
    * @param  void
    * @return device location
    */
T_CASE_LOCATION_STATUS app_gpio_on_off_get_location(void);
/**
    * @brief  Handle power onoff gpio detect io msg
    * @param  io_driver_msg_recv The T_IO_MSG from peripherals or BT stack state machine
    * @return void
    */
void app_gpio_on_off_handle_msg(T_IO_MSG *io_driver_msg_recv);

/**
    * @brief  enter dlps handle
    * @param  none
    * @return void
    */
void  app_gpio_on_off_enter_dlps(void);

/**
    * @brief  exit dlps handle
    * @param  none
    * @return void
    */
void app_gpio_on_off_exit_dlps(void);
/** @} */ /* End of group GPIO_ON_OFF_PROCESS_Exported_Functions */
/** @} */ /* End of group APP_GPIO_ON_OFF */


#ifdef __cplusplus
}
#endif
#endif
