/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_HARMAN_ADC_H_
#define _APP_HARMAN_ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_HARMAN_ADC_H
  * @brief
  * @{
  */

#define HARMAN_THERMISTOR_PIN                           P0_2
#define DISCHARGER_NTC_CHECK_PERIOD                     500

#if HARMAN_SECOND_NTC_DETECT_PROTECT
#define HARMAN_THERMISTOR_PIN2                          P0_3
#endif

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
#define DISCHARGER_VBAT_CHECK_TOTAL_TIME                1500 // 1.5s
#define DISCHARGER_VABT_MAX_ADC_DISCREPANCY_VALUE       80
#endif

void app_harman_adc_init(void);
void app_harman_adc_io_read(void);

void app_harman_adc_ntc_check_timer_start(uint32_t time);
void app_harman_adc_ntc_check_timer_stop(void);
bool app_harman_adc_ntc_check_timer_started(void);

uint16_t app_harman_adc_voltage_battery_get(void);
void app_harman_adc_update_cur_ntc_value(uint16_t *p_ntc_value);
void app_harman_charger_ntc_adc_msg(void);

bool app_harman_adc_adp_in_handle(void);
void app_harman_adc_adp_out_handle(void);

void app_harman_adc_msg_handle(void);

void app_harman_adc_vbat_check_times_clear(void);

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
bool app_harman_discharger_ntc_check_valid(void);
void app_harman_discharger_ntc_timer_start(void);
#endif

/** End of APP_HARMAN_ADC_H
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HARMAN_ADC_H_ */


