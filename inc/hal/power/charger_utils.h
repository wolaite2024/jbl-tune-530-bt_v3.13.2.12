/**
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _CHARGER_UTILS_H_
#define _CHARGER_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

/**  @brief rtk charger function return general error code*/
typedef enum
{
    CHARGER_UTILS_SUCCESS,
    CHARGER_UTILS_NOT_SUPPROTED,
    CHARGER_UTILS_NOT_ENABLED,
    CHARGER_UTILS_INVALID_PARAM,
} T_CHARGER_UTILS_ERROR;

typedef struct _charger_utils_config
{
    uint16_t pre_charge_current;
    uint16_t pre_charge_timeout;
    uint16_t fast_charge_current;
    uint16_t fast_charge_timeout;
    uint16_t full_voltage;
} T_CHARGER_UTILS_CONFIG;

/**
 * charger_utils.h
 *
 * \brief   Get charging voltage.
 *
* \param[out]   battery voltage, unit: mV
 *
 * \return          The status of getting voltage.
 * \retval  CHARGER_UTILS_SUCCESS           current charging info is getting successfully.
 * \retval  CHARGER_UTILS_NOT_SUPPROTED     charging info getting failed.
 * \retval  CHARGER_UTILS_NOT_ENABLED       charger is not enabled. could not get battery information from charger module
 *
 * \ingroup CHARGER_UTILS
 */
T_CHARGER_UTILS_ERROR charger_utils_get_batt_volt(uint16_t *volt);

/**
 * charger_utils.h
 *
 * \brief   Get charging current, unit: mA
 *
 * \param[out]   charging current, positive in charging mode, negative in discharging mode
 *
 * \return          The status of getting current.
 * \retval  CHARGER_UTILS_SUCCESS           current charging info is getting successfully.
 * \retval  CHARGER_UTILS_NOT_SUPPROTED     charging info getting failed.
 * \retval  CHARGER_UTILS_NOT_ENABLED       charger is not enabled. could not get battery information from charger module
 *
 * \ingroup CHARGER_UTILS
 */
T_CHARGER_UTILS_ERROR charger_utils_get_batt_curr(int16_t *current);

/**
 * \brief   Get charging temperature1 and temperature2.
 *
 * \param[out]   temperature1, unit: mV
 * \param[out]   temperature2, unit: mV
 *
 * \return          The status of getting temperature.
 * \retval  CHARGER_UTILS_SUCCESS           Temperature obtained successfully.
 * \retval  CHARGER_UTILS_NOT_SUPPROTED     Getting temperature is not supported.
 * \retval  CHARGER_UTILS_NOT_ENABLED       Charger is not enabled. Could not get temperature information from charger module
 *
 * \ingroup CHARGER_UTILS
 */
T_CHARGER_UTILS_ERROR charger_utils_get_batt_temp(uint16_t *temperature1, uint16_t *temperature2);

/**
 * charger_utils.h
 *
 * \brief   Get charging adapter voltage, unit: mV
 *
 * \param[out]   adapter voltage
 *
 * \return          The status of getting adapter voltage.
 * \retval  CHARGER_UTILS_SUCCESS           Adapter voltage obtained successfully.
 * \retval  CHARGER_UTILS_NOT_SUPPROTED     Getting adapter voltage is not supported.
 * \retval  CHARGER_UTILS_NOT_ENABLED       Charger is not enabled. Could not get adapter voltage information from charger module.
 *
 * \ingroup CHARGER_UTILS
 */
T_CHARGER_UTILS_ERROR charger_utils_get_adapter_volt(uint16_t *volt);

/**
 * charger_utils.h
 *
 * \brief   Enable or disable charger.
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 * \param[in]   enable    Enable or disable charger.
 *                        true: enable charger.
 *                        false: disable charger.
 * @return      None.
 */
void charger_utils_charger_auto_enable(bool enable);

/**
 * charger_utils.h
 *
 * \brief   Get charger thermistor detection enable status.
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 * \param[in]   void
 * @return      Charger thermistor detection enable state.
 * @retval      true    Charger thermistor detection is supported.
 * @retval      false   Charger thermistor detection is not supported.
 */
bool charger_utils_get_thermistor_enable_state(void);

/**
 * charger_utils.h
 *
 * \brief   Set charging current and full voltage, restart charger fsm if charger is running.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  p_charger_config     The configuration structure of charging current and full voltage.
 *              pre_charge_current:  Charge current of pre-charge state. Unit: mA. Range: 5 ~ 50 (mA)
 *              pre_charge_timeout:  The timeout time of pre-charge stage. Unit: minutes. Range: 1 ~ 65535(minutes)
 *              fast_charge_current: Charge current of fast-charge state. Unit: mA.
 *                                   For RTL87X3E, range: 20 ~ 400 (mA)
 *                                   For RTL87X3D, range of internal charger: 30 ~ 400 (mA), range of external BJT charger: 405 ~ 1000 (mA)
 *              fast_charge_timeout: The timeout time of fast-charge stage. Unit: minutes. Range: 3 ~ 65535(minutes)
 *              full_voltage:        Voltage Limit of Battery. Unit: mV. Range: 4000 ~ 4400(mV)
 *
 * \return          The status of setting charging current and full voltage.
 * \retval  CHARGER_UTILS_SUCCESS           The charging current and full voltage are set successfully.
 * \retval  CHARGER_UTILS_INVALID_PARAM     Invalid charging current and full voltage parameters.
 *
 * \ingroup CHARGER_UTILS_Exported_Functions
 */
T_CHARGER_UTILS_ERROR charger_utils_set_all_param(T_CHARGER_UTILS_CONFIG *p_charger_config);

/**
 * charger_utils.h
 *
 * \brief   Get charging current and full voltage configurations.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  p_charger_config     The configuration structure of charging current and full voltage.
 *              pre_charge_current:  Charge current of pre-charge state. Unit: mA. Range: 5 ~ 50 (mA)
 *              pre_charge_timeout:  The timeout time of pre-charge stage. Unit: minutes. Range: 1 ~ 65535(minutes)
 *              fast_charge_current: Charge current of fast-charge state. Unit: mA.
 *                                   For RTL87X3E, range: 20 ~ 400 (mA)
 *                                   For RTL87X3D, range of internal charger: 30 ~ 400 (mA), range of external BJT charger: 405 ~ 1000 (mA)
 *              fast_charge_timeout: The timeout time of fast-charge stage. Unit: minutes. Range: 3 ~ 65535(minutes)
 *              full_voltage:        Voltage Limit of Battery. Unit: mV. Range: 4000 ~ 4400(mV)
 *
 * \return          The status of getting charging current and full voltage configurations.
 * \retval  CHARGER_UTILS_SUCCESS           The charging current and full voltage configurations are obtained successfully.
 * \retval  CHARGER_UTILS_INVALID_PARAM     Invalid parameter.
 *
 * \ingroup CHARGER_UTILS_Exported_Functions
 */
T_CHARGER_UTILS_ERROR charger_utils_get_all_param(T_CHARGER_UTILS_CONFIG *p_charger_config);

T_CHARGER_UTILS_ERROR charger_utils_get_thermistor_pin(uint8_t *pin);
T_CHARGER_UTILS_ERROR charger_utils_thermistor_enable(uint8_t *enable);
T_CHARGER_UTILS_ERROR charger_utils_get_auto_enable(uint8_t *auto_enable);
T_CHARGER_UTILS_ERROR charger_utils_get_high_temp_warn_voltage(uint16_t *voltage);
T_CHARGER_UTILS_ERROR charger_utils_get_high_temp_error_voltage(uint16_t *voltage);
T_CHARGER_UTILS_ERROR charger_utils_get_low_temp_warn_voltage(uint16_t *voltage);
T_CHARGER_UTILS_ERROR charger_utils_get_low_temp_error_voltage(uint16_t *voltage);
T_CHARGER_UTILS_ERROR charger_utils_get_recharge_voltage(uint16_t *voltage);
T_CHARGER_UTILS_ERROR charger_utils_get_bat_temp_hysteresis_voltage(uint16_t *voltage);

#endif









