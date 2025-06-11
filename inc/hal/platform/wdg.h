/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    wdg.h
  * @brief   This file provides wdg api wrapper for bbpro compatibility..
  * @author  sandy_jiang
  * @date    2018-11-29
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/** @defgroup  HAL_WDG_API    WDT api
    * @brief This file introduces the watchdog Timer(WDG) APIs
    * @{
    */
/**
 *
 * @section WDG_API_Usage_Chapter How to use this module.
 *    There are two hardware behaviors based on the supported mode configuration after the timer times out or soft reset register is set:
 *  - Reset all: In this mode, a hardware reset signal will be triggered by WDT hardware, and the system,including DSP, MCU will reboot.
 *  - Interrupt mode: In this mode, an interrupt will be triggered by WDT hardware. This mode is usually
 *                    used for debugging. The MCU subsystem, MCU peripheral and DSP subsystem will be reset after dumping debug information .
 *  - sample code:
 * @code
 *
 *     //get WDG default mode
 *     uint8_t wdg_default_mode = wdg_get_default_mode();
 *
 *     //get WDG current mode
 *     uint8_t wdg_current_mode = wdg_get_mode();
 *
 *     //get WDG default timeout value
 *     uint8_t wdg_default_timeout_value_ms = wdg_get_default_timeout_ms();
 *
 *     //get WDG current timeout value
 *     uint8_t wdg_current_timeout_value_ms = wdg_get_default_timeout_ms();
 *
 *     //swtich WDG mode to interrupt mode
 *     uint8_t wdg_current_mode = wdg_get_mode();
 *     if(wdg_current_mode != INTERRUPT_CPU)
 *     {
 *          wdg_change_mode(INTERRUPT_CPU);
 *     }
 *     else
 *     {
 *          //error handle
 *     }
 *
 *     //swtich WDG mode to reset mode
 *     uint8_t wdg_current_mode = wdg_get_mode();
 *     if(wdg_current_mode != RESET_ALL)
 *     {
 *          wdg_change_mode(RESET_ALL);
 *     }
 *     else
 *     {
 *          //error handle
 *     }
 *
 *     //change WDG timeout value to 10000ms(10s)
 *     wdg_change_timeout_period(10000)
 *
 *     //disable WDG if user wants to disable the WDG
 *     wdg_disable();
 *
 *     //enable WDG if user disabled WDG before
 *     wdg_disable();
 *     //do something here
 *     wdg_ebable();
 *
 *     //kick WDG if user wants to reset WDG timeout value
 *      wdg_kick();
 * @endcode
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __WDG_H_
#define __WDG_H_

typedef enum _WDG_MODE
{
    INTERRUPT_CPU = 0,          /**< Interrupt CPU only */
    RESET_ALL_EXCEPT_AON = 1,   /**< Reset all except RTC and some AON register */
    RESET_CORE_DOMAIN = 2,      /**< Reset core domain */
    RESET_ALL = 3               /**< Reset all */
} T_WDG_MODE;

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup HAL_WDG_API_Exported_Functions WDG Api
  * @{
  */

/**
    * @brief  wdg default mode get
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  none
    * @return wdg type @ref T_WDG_TYPE
    */
T_WDG_MODE wdg_get_default_mode(void);

/**
    * @brief  wdg current mode get
        * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  none
    * @return wdg type @ref T_WDG_TYPE
    */
T_WDG_MODE wdg_get_mode(void);

/**
    * @brief  wdg current timeout value get
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  none
    * @return current timeout
    */
uint32_t wdg_get_timeout_ms(void);

/**
    * @brief  wdg default timeout value get
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  none
    * @return default timeout
    */
uint32_t wdg_get_default_timeout_ms(void);

/**
    * @brief  change wdg timeout period
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  timeout_ms: new timeout value
    * @return none
    */
void wdg_change_timeout_period(uint32_t timeout_ms);

/**
    * @brief  change wdg mode
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  wdg type @ref T_WDG_TYPE
    * @return none
    */
void wdg_change_mode(T_WDG_MODE mode);

/**
    * @brief  check if wdg is enable
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  none
    * @return true: enable
        * @return false: disable
    */
bool wdg_is_enable(void);

/**
    * @brief  set wdg timeout periods and mode
    * @param  none
    * @return true: enable
    * @return false: disable
    */
extern bool (*wdg_start)(uint32_t ms, T_WDG_MODE  wdg_mode);
#define WDG_Start(ms, wdg_mode) wdg_start(ms, wdg_mode);

/**
    * @brief  disable wdg
    * @param  none
    * @return none
    */
extern void (*wdg_disable)(void);
#define WDG_Disable() wdg_disable();

/**
    * @brief  kick wdg
    * @param  none
    * @return none
    */
extern void (*wdg_kick)(void);
#define WDG_Kick() wdg_kick();

/**
    * @brief  reset the mcu at specified mode
    * @param  mode: @ref T_WDG_TYPE
    * @return none
    */
extern void wdt_reset(T_WDG_MODE wdg_mode, const char *function, uint32_t line);
#define chip_reset(mode)    wdt_reset(mode,  __FUNCTION__, __LINE__)

#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_WDG_API_Exported_Functions */
/** @} */ /* End of group HAL_WDG_API */
#endif
