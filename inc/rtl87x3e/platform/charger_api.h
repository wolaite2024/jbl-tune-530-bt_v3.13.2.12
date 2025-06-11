/**
************************************************************************************************************
*            Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file      charger_api.h
* @brief     Charger api head file.
* @author    Kellan Ho
* @date      2016-12-30
* @version   v1.0
*************************************************************************************************************
*/

#ifndef __CHARGER_API_H
#define __CHARGER_API_H
#include <stdint.h>


/** @defgroup 87x3e_CHARGER_API Charger API Sets
  * @brief charger module API sets.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup 87x3e_CHARGER_API_Exported_Types Charger API Sets Exported Types
  * @{
  */

/**  @brief  charger states*/
typedef enum
{
    STATE_CHARGER_START = 0,
    STATE_CHARGER_PRE_CHARGE,
    STATE_CHARGER_FAST_CHARGE,
    STATE_CHARGER_FINISH,
    STATE_CHARGER_ERROR,
    STATE_CHARGER_END,
} T_CHARGER_STATE;

/**  @brief  charger error code*/
typedef enum
{
    CHARGER_NO_ERROR = 0,
    CHARGER_ERROR_OPEN,
    CHARGER_ERROR_SHORT,
    CHARGER_ERROR_PRE_CHARGE_TIMEOUT,
    CHARGER_ERROR_FAST_CHARGE_TIMEOUT,
    CHARGER_ERROR_CURRENT,
    CHARGER_ERROR_VOLTAGE,
    CHARGER_ERROR_ADC_INTERRUPT,
    CHARGER_ERROR_THERMISTOR_1,
    CHARGER_ERROR_THERMISTOR_2,
    CHARGER_ERROR_FW_OTP
} T_CHARGER_ERROR_CODE;

/**  @brief  charger state change callback function prototype */
typedef void (*CHARGER_STATE_CALLBACK)(T_CHARGER_STATE);

/**  @brief  charge state change callback function prototype */
typedef void (*STATE_OF_CHARGE_CALLBACK)(uint8_t);

/** End of CHARGER_API_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup 87x3e_CHARGER_API_EXPORT_Functions Charger API Sets Exported Functions
 *
 * @{
 */

/**
    * @brief  register the user-defined callback function, which will be called by charger module when charge state changes
    * @param  callback_func    callback function to be registered
    * @return none
    */
extern void (*charger_api_reg_state_of_charge_callback)(STATE_OF_CHARGE_CALLBACK callback_func);

/**
    * @brief  unregister the user-defined STATE_OF_CHARGE_CALLBACK callback function
    * @param  void
    * @return none
    */
extern void (*charger_api_unreg_state_of_charge_callback)(void);

/* register the user-defined callback function, which called by charger moudle when Charger Module State changes */
/**
    * @brief  register the user-defined callback function, which will be called by charger moudle when charger module state changes
    * @param  callback_func    callback function to be registered
    * @return none
    */
extern void (*charger_api_reg_charger_state_callback)(CHARGER_STATE_CALLBACK callback_func);

/**
    * @brief  unregister the user-defined CHARGER_STATE_CALLBACK callback function
    * @param  void
    * @return none
    */
extern void (*charger_api_unreg_charger_state_callback)(void);

/**
    * @brief  return state of charge
    * @param  void
    * @return charge state
    */
extern uint8_t (*charger_api_get_state_of_charge)(void);

/**
    * @brief  return charger module state
    * @param  void
    * @return charger state @ref T_CHARGER_STATE
    */
extern T_CHARGER_STATE(*charger_api_get_charger_state)(void);

/**
    * @brief  return error code of charger module
    * @param  void
    * @return charger module error code @ref T_CHARGER_ERROR_CODE
    */
extern T_CHARGER_ERROR_CODE(*charger_api_get_error_code)(void);

/**
    * @brief  return maximum current of adapter
    * @param  void
    * @return maximum current of adapter
    */
extern uint16_t (*charger_api_get_adapter_current)(void);

/**
    * @brief  return full charge current (1C)
    * @param  void
    * @return full charge current
    */
extern uint16_t (*charger_api_get_full_current)(void);

/**
    * @brief  set maximum current of adapter
    * @note   will cause soft reset of charger module
    * @param  adapter_current   adapter current to te set
    * @return none
    */
extern void (*charger_api_set_adapter_current)(uint16_t);

/**
    * @brief  set full charge current (1C)
    * @note   will cause soft reset of charger module
    * @param  full_current  full current to te set
    * @return none
    */
extern void (*charger_api_set_full_current)(uint16_t);

/**
    * @brief  enable charger module manually
    * @note   will cause soft reset of charger module and force discharger module to be disable if discharger module is running
    * @param  void
    * @return none
    */
extern void (*charger_api_enable_charger)(void);

/**
    * @brief  disable charger module manually
    * @param  void
    * @return none
    */
extern void (*charger_api_disable_charger)(void);

/**
    * @brief  enable discharger module manually
    * @note   will cause soft reset of discharger module and force charger module to be disable if charger module is running
    * @param  void
    * @return none
    */
extern void (*charger_api_enable_discharger)(void);

/**
    * @brief  disable discharger module manually
    * @param  void
    * @return none
    */
extern void (*charger_api_disable_discharger)(void);

/** @} */ /* End of group 87x3e_CHARGER_API_EXPORT_Functions */
/** @} */ /* End of group 87x3e_CHARGER_API */


#endif  /* __CHARGER_API_H */

