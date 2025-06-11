/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_gpio_int.h
* @brief     This file provides all the gpio hal interrupt functions.
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _HAL_GPIO_INT_
#define _HAL_GPIO_INT_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup 87x3e_GPIO_INT_H_
  * @brief HAL GPIO device driver module
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

typedef void (*P_GPIO_CBACK)(uint32_t context);

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * hal_gpio_int.h
 *
 * \brief   Register the GPIO interrupt callback for the specific pin.
 *
 * \param[in]  pin_index     The gpio number
 * \param[in]  callback      The callback to be called when the specific interrupt happened.
 * \param[in]  context       The user data when callback is called.
 *
 * \return                   The status of register interrupt functions.
 * \retval true              The callback is registered successfully.
 * \retval false             The callback is failed to register due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  87x3e_GPIO_INT_H_
 */
bool hal_gpio_register_isr_callback(uint8_t pin_index, P_GPIO_CBACK callback, uint32_t context);

/**
 * hal_gpio_int.h
 *
 * \brief   Get the registered callback for the specific pin.
 *
 * \param[in]  pin_index     The gpio number
 * \param[in]  callback      The pointer for the callback to get.
 * \param[in]  context       The pointer for the context to get.
 *
 * \return                   The status of register interrupt functions.
 * \retval true              The callback is registered successfully.
 * \retval false             The callback is failed to register due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  87x3e_GPIO_INT_H_
 */
bool hal_gpio_get_isr_callback(uint8_t pin_index, P_GPIO_CBACK *p_callback, uint32_t *p_context);

/**
 * hal_gpio_int.h
 *
 * \brief   Initialise the hal gpio interrupt module.
 *
 * \param[in]  None
 *
 * \return None
 * \endcode
 *
 * \ingroup  87x3e_GPIO_INT_H_
 */
void hal_gpio_int_init(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */ /* End of group 87x3d_GPIO_INT_H_ */

/******************* (C) COPYRIGHT 2021 Realtek Semiconductor *****END OF FILE****/
