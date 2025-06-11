/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __APP_SENSOR_iqs773_873__
#define __APP_SENSOR_iqs773_873__

#include "section.h"
#include "app_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/

/**
    * @brief  PSensor iqs773 Detect GPIO interrupt will be handle in this function.
    *         Disable GPIO interrupt and send IO_GPIO_MSG_TYPE message to app task.
    *         Then enable GPIO interrupt.
    * @param  void
    * @return void
    */
ISR_TEXT_SECTION void app_psensor_iqs773_873_intr_cb(uint32_t param);

/**
    * @brief  check psensor iqs773 i2c event when receive io msg from interrupt
    * @param  void
    * @return void
    */
void app_psensor_iqs_key_handle(T_PSENSOR_I2C_EVENT event);
void app_psensor_iqs773_check_i2c_event(void);
void app_psensor_iqs873_check_i2c_event(void);
void app_sensor_iqs873_enable(void);
void app_sensor_iqs873_disable(void);

#ifdef __cplusplus
}
#endif
#endif


