/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hal_gpio.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup 87x3e_HAL_GPIO_H_   _HAL_GPIO_H_
  * @brief HAL GPIO device driver module
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
typedef enum
{
    GPIO_STATUS_ERROR              = -3,        /**< The GPIO function failed to execute.*/
    GPIO_STATUS_ERROR_PIN          = -2,        /**< Invalid input pin number. */
    GPIO_STATUS_INVALID_PARAMETER  = -1,        /**< Invalid input parameter. */
    GPIO_STATUS_OK                 = 0,         /**< The GPIO function executed successfully. */
} T_GPIO_STATUS;

typedef enum
{
    GPIO_TYPE_AUTO      = 0,    /**< The GPIO function switch automatically, use core GPIO in active mode,
                                    use aon GPIO in low power mode.*/
    GPIO_TYPE_CORE      = 1,    /**< The GPIO function in core domain, only works when active mode.
                                    Support all the features. */
    GPIO_TYPE_AON       = 2,    /**< The GPIO function in aon domain, works in dlps/power down,
                                    much slower than CORE mode,
                                    not support read level and debounce.*/
} T_GPIO_TYPE;

typedef enum
{
    GPIO_DIR_INPUT     = 0,
    GPIO_DIR_OUTPUT    = 1,
} T_GPIO_DIRECTION;

typedef enum
{
    GPIO_LEVEL_LOW     = 0,
    GPIO_LEVEL_HIGH    = 1,
    GPIO_LEVEL_UNKNOWN = 2,
} T_GPIO_LEVEL;

typedef enum
{
    GPIO_IRQ_EDGE,
    GPIO_IRQ_LEVEL,
} T_GPIO_IRQ_MODE;

typedef enum
{
    GPIO_IRQ_ACTIVE_LOW,
    GPIO_IRQ_ACTIVE_HIGH,
} T_GPIO_IRQ_POLARITY;

typedef enum
{
    GPIO_PULL_DOWN,   // band   PAD_PULL_DOWN,
    GPIO_PULL_UP,     //        PAD_PULL_UP,
    GPIO_PULL_NONE    //        PAD_PULL_NONE
} T_GPIO_PULL_VALUE;

typedef void (*P_GPIO_CBACK)(uint32_t context);

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * hal_gpio.h
 *
 * \brief   Initialize a pin to gpio mode.
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part.
 *
 * \param[in]   type        The TYPE of GPIO mode to be used.
 *
 * \param[in]   direction   The direction GPIO is set to, could be output mode or input mode.
 *
 * \param[in]   pull_value   The GPIO pull value is set to , could be pull high low or none.
 *
 * \return                   The status of the gpio pin initialization.
 * \retval GPIO_STATUS_OK        The GPIO pin was initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to initialized due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_init_pin(TEST_PIN, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT ,GPIO_PULL_UP );
 *     hal_gpio_init_pin(TEST_PIN_2, GPIO_TYPE_CORE, GPIO_DIR_INPUT ,GPIO_PULL_UP);
 * }
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_init_pin(uint8_t pin_index, T_GPIO_TYPE type, T_GPIO_DIRECTION direction,
                                T_GPIO_PULL_VALUE pull_value);

/**
 * hal_gpio.h
 *
 * \brief   De-Initialize the pin to shutdown.
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part.
 *
 *
 * \return                   The status of the gpio pin initialization.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_deinit(TEST_PIN);
 *     hal_gpio_deinit(TEST_PIN_2);
 * }
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS gpio_deinit_pin(uint8_t pin_index);

/**
 * hal_gpio.h
 *
 * \brief   Change gpio direction
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part.
 *
 * \param[in]  direction    The direction GPIO is set to, could be output mode or input mode.
 *
 * \return                       The status of the gpio changing direction.
 * \retval GPIO_STATUS_OK        The GPIO pin changes direction successfully.
 * \retval GPIO_STATUS_ERROR_PIN The GPIO pin was failed to change direction due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_change_direction(TEST_PIN, GPIO_DIR_OUTPUT);
 *     hal_gpio_change_direction(TEST_PIN_2, GPIO_DIR_INPUT);
 * }
 * \endcode
 *
 * \ingroup  HAL_GPIO_Exported_Functions
 */
T_GPIO_STATUS hal_gpio_change_direction(uint8_t pin_index, T_GPIO_DIRECTION direction);

/**
 * hal_gpio.h
 *
 * \brief   Set the pin pull value.
 *
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part.
 *
 * \param[in]  pull_value        The value for the specific pin to pull, up down or none
 *
 *
 * \return                   none.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_set_pull_value(TEST_PIN_2, GPIO_PULL_UP);
 * }
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_set_pull_value(uint8_t pin_index, T_GPIO_PULL_VALUE pull_value);
/**
 * hal_gpio.h
 *
 * \brief   Set the pin output level.
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part.
 *
 * \param[in]  level        The level for the specific pin to output
 *
 *
 * \return                   The status of the gpio pin initialization.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_init_pin(TEST_PIN, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT);
 *     gpio_set_level(TEST_PIN_2, GPIO_LEVEL_HIGH);
 * }
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_set_level(uint8_t pin_index, T_GPIO_LEVEL level);

/**
 * hal_gpio.h
 *
 * \brief   Get the pin current level.
 *
 * \param[in]  pin_index    The pin index, please refer to rtl876x.h "Pin_Number" part. *
 *
 * \return                   The current pin level.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     hal_gpio_init_pin(TEST_PIN, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT);
 *     hal_gpio_set_level(TEST_PIN_2, GPIO_LEVEL_HIGH);
 * }
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_LEVEL hal_gpio_get_input_level(uint8_t pin_index);

/**
 * hal_gpio.h
 *
 * \brief   Get the pin current level.
 *
 * \param[in]  pin_index          The pin index, please refer to rtl876x.h "Pin_Number" part. *
 * \param[in]  mode               The interrupt mode to be set to.
 * \param[in]  polarity           The polarity for the interrupt to be set to.
 * \param[in]  debounce_enable    Enable or Disable the pin hardware debounce feature.
 * \param[in]  callback           The callback to be called when the specific interrupt happened.
 * \param[in]  context            The user data when callback is called.
 *
 * \return                   The current pin level.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_set_up_irq(uint8_t pin_index, T_GPIO_IRQ_MODE mode,
                                  T_GPIO_IRQ_POLARITY polarity,
                                  bool debounce_enable);

/**
 * hal_gpio.h
 *
 * \brief   Enable the interrupt of the pin.
 *
 * \param[in]  pin_index          The pin index, please refer to rtl876x.h "Pin_Number" part. *
 *
 * \return                   The status of enable interrupt.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_irq_enable(uint8_t pin_index);


/**
 * hal_gpio.h
 *
 * \brief   Disable the interrupt of the pin.
 *
 * \param[in]  pin_index          The pin index, please refer to rtl876x.h "Pin_Number" part. *
 *
 * \return                   The status of disable interrupt.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_irq_disable(uint8_t pin_index);

/**
 * hal_gpio.h
 *
 * \brief   Set the hardware debounce time. the interrupt could be triggered after 1 to 2 of debounce time.
 *
 * \param[in]  ms          The pin index, please refer to rtl876x.h "Pin_Number" part. *
 *
 * \return                   The status of setting debounce time.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_set_debounce_time(uint8_t ms);

/**
 * hal_gpio.h
 *
 * \brief   Change the interrupt polarity of GPIO current mode.
 *
 * \param[in]  pin_index          The pin index, please refer to rtl876x.h "Pin_Number" part. *
 *
 * \return                   The status of change polarity.
 * \retval GPIO_STATUS_OK        The GPIO pin was de-initialized successfully.
 * \retval GPIO_STATUS_ERROR_PIN     The GPIO pin was failed to de-initialized due to invalid pin number.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
T_GPIO_STATUS hal_gpio_irq_change_polarity(uint8_t pin_index, T_GPIO_IRQ_POLARITY polarity);


/**
 * hal_gpio.h
 *
 * \brief   Init the hal gpio module, gpio clock would be enabled, and hal gpio function could be used after this function called.
 *
 * \param   None
 * \return  None.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
void hal_gpio_init(void);

/**
 * hal_gpio.h
 *
 * \brief   De-Init the hal gpio module, gpio clock would be enabled, and hal gpio function could not be used after this function called.
 *
 * \param   None
 * \return  None.
 *
 * \endcode
 *
 * \ingroup  GPIO
 */
void hal_gpio_deinit(void);


#ifdef __cplusplus
}
#endif

#endif

/** @} */ /* End of group 87x3e_HAL_GPIO_H_ */

/******************* (C) COPYRIGHT 2021 Realtek Semiconductor *****END OF FILE****/
