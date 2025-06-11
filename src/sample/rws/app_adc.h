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
#ifndef _AUDIO_ADC_API_H_
#define _AUDIO_ADC_API_H_

#include "app_charger.h"

/** @defgroup  APP_AUDIO_ADC_API App Audio Adc Api
    * @brief AUDIO ADC module implementation for audio sample project
    * @{
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup AUDIO_ADC_Exported_Types Audio ADC Types
    * @{
    */


/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup AUDIO_ADC_Exported_Functions Audio ADC Functions
    * @{
    */
void app_adc_init(void);
bool app_adc_enable_read_adc_io(uint8_t target_io_pin);
void app_adc_interrupt_handler(void *pvPara, uint32_t int_status);
void app_adc_set_cmd_info(uint8_t cmd_path, uint8_t app_idx);

bool app_adc_ntc_voltage_valid_check(void);
void app_adc_ntc_voltage_read_start(void);
void app_adc_ntc_voltage_read_stop(void);
void app_adc_ntc_voltage_power_off_check(void);
/** @} */ /* End of group AUDIO_ADC_Exported_Functions */

/** @} */ /* End of group AUDIO_ADC_API */

#endif
