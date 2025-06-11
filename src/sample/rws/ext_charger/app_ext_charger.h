/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_EXTERNAL_CHARGER_H_
#define _APP_EXTERNAL_CHARGER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "app_msg.h"
#include "app_charger.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_EXTERNAL_CHAGER
  * @brief
  * @{
  */
#if HARMAN_T135_SUPPORT
#define EXTERNAL_CHARGING_STATUS_DET_PIN      P1_1    /* charging status control */
#define EXTERNAL_CHARGING_ENABLE_CTL_PIN      P0_1    /* charger enable control */
#define EXTERNAL_CHARGING_CURRENT_CTL_PIN1    MIC3_N  /* charger speed SET1 */
#define EXTERNAL_CHARGING_CURRENT_CTL_PIN2    MIC3_P  /* charger speed SET2 */
#elif (HARMAN_RUN3_SUPPORT || HARMAN_PACE_SUPPORT || HARMAN_T530_SUPPORT)
#define EXTERNAL_CHARGING_STATUS_DET_PIN      P1_1    /* charging status control */
#define EXTERNAL_CHARGING_ENABLE_CTL_PIN      P1_0    /* charger enable control */
#define EXTERNAL_CHARGING_CURRENT_CTL_PIN1    P3_0  /* charger speed SET1 */
#define EXTERNAL_CHARGING_CURRENT_CTL_PIN2    P3_1  /* charger speed SET2 */
#else
#endif

#define CHARGING_NTC_CHECK_PERIOD                   500
#define CHARGING_VBAT_LOW_VOLTAGE_CHECK             3000 // mv
#define CHARGING_VBAT_HIGH_VOLTAGE_CHECK            4350 // mv
#define NO_CHARGING_VBAT_HIGH_VOLTAGE_CHECK         4000 // mv

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
#define CHARGING_VBAT_DETECT_TOTAL_TIME             5000 // 5S
#define CHARGER_VABT_MAX_ADC_DISCREPANCY_VALUE      35   // mv
#endif

typedef enum
{
    APP_EXT_CHARGER_EVENT_ADP_IN                 = 0x00,
    APP_EXT_CHARGER_EVENT_ADP_OUT                = 0x01,
    APP_EXT_CHARGER_EVENT_PERIOD_CHECK           = 0x02,
    APP_EXT_CHARGER_EVENT_CHARGING_FULL          = 0x03,
    APP_EXT_CHARGER_EVENT_ADC_UPDATE             = 0x04,
} APP_EXT_CHARGER_EVENT_TYPE;

typedef struct _CHARGER_CONFIG
{
    uint8_t inter_charger_auto_enable;
    uint8_t inter_discharger_auto_enable;
    uint8_t ext_charger_support;
    uint8_t ext_discharger_support;
    uint8_t thermistor_1_enable;
    uint8_t thermistor_1_adc_channel;

    uint16_t recharge_threshold;

    /* Thermistor (NTC) Parameters */
    uint16_t bat_high_temp_warn_voltage;        // Warn Region Voltage of Battery High Temperature
    uint16_t bat_low_temp_warn_voltage;         // Warn Region Voltage of Battery Low Temperature
    uint16_t bat_high_temp_error_voltage;       // Error Region Voltage of Battery High Temperature
    uint16_t bat_low_temp_error_voltage;        // Error Region Voltage of Battery Low Temperature
    uint16_t bat_temp_hysteresis_voltage;       // Hysteresis Voltage of Battery Temperature
    uint16_t bat_high_temp_warn_recovery_vol;
    uint16_t bat_low_temp_warn_recovery_vol;
    uint16_t bat_high_temp_error_recovery_vol;
    uint16_t bat_low_temp_error_recovery_vol;
} APP_EXT_CHARGER_CONFIG;

typedef enum
{
    CURRENT_SPEED_0_C     = 0,
    CURRENT_SPEED_0_5_C   = 1,
    CURRENT_SPEED_1_C     = 2,
    CURRENT_SPEED_1_5_C   = 3,
    CURRENT_SPEED_2_C     = 4,
    CURRENT_SPEED_2_5_C   = 5,
    CURRENT_SPEED_3_C     = 6,
} APP_EXT_CHARGER_CURRENT_SPEED;

typedef enum
{
    VBAT_DETECT_STATUS_ING          = 0x00,
    VBAT_DETECT_STATUS_SUCCESS      = 0x01,
    VBAT_DETECT_STATUS_FAIL         = 0x02,
} T_APP_HARMAN_VBAT_DETECT_STATUS;

typedef enum
{
    APP_EXT_CHARGER_NTC_NORMAL                  = 0x00,
    APP_EXT_CHARGER_NTC_WARNING                 = 0x01,
    APP_EXT_CHARGER_NTC_ERROR                   = 0x02,
} APP_EXT_CHARGER_NTC_STATE;

typedef struct
{
    bool enable;
    bool ignore_first_adc_sample; // switch charging current or switch charger state, need set to true
    uint8_t vbat_is_normal;
    uint8_t fully_charged_level;

    uint8_t enable_ctrl_pin;
    uint8_t status_det_pin;
    uint8_t current_ctrl_pin1;
    uint8_t current_ctrl_pin2;

    T_APP_CHARGER_STATE state;
    APP_EXT_CHARGER_CURRENT_SPEED current_speed;
    APP_EXT_CHARGER_NTC_STATE ntc_state;
} APP_EXT_CHARGER_MGR;

T_APP_CHARGER_STATE app_ext_charger_no_charge_handler(APP_EXT_CHARGER_EVENT_TYPE event,
                                                      uint8_t *param);
T_APP_CHARGER_STATE app_ext_charger_charging_handler(APP_EXT_CHARGER_EVENT_TYPE event,
                                                     uint8_t *param);
T_APP_CHARGER_STATE app_ext_charger_charging_finish_handler(APP_EXT_CHARGER_EVENT_TYPE
                                                            event, uint8_t *param);
T_APP_CHARGER_STATE app_ext_charger_charging_error_handler(APP_EXT_CHARGER_EVENT_TYPE
                                                           event, uint8_t *param);
void app_ext_charger_send_charger_state(T_APP_CHARGER_STATE charger_state);
void app_ext_charger_handle_io_msg(T_IO_MSG console_msg);
void app_ext_charger_ntc_adc_update(void);
void app_ext_charger_handle_enter_dlps(void);
void app_ext_charger_handle_exit_dlps(void);
bool app_ext_charger_is_pin_status_det_pin(uint8_t gpio_num);
void app_ext_charger_init(void);
bool app_ext_charger_check_support(void);
uint8_t app_ext_charger_check_status(void);
void app_ext_charger_ntc_check_timeout(void);
void app_ext_charger_vbat_is_normal_set(uint8_t enable);
uint8_t app_ext_charger_vbat_is_normal_get(void);

/** End of APP_EXTERNAL_CHAGER
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_EXTERNAL_CHARGER_H_ */
