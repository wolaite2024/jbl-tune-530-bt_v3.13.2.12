/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      led_demo.h
* @brief     Modulize LED driver for more simple led implementation
* @details   Led module is to group led_driver to provide more simple and easy usage for LED module.
* @author    brenda_li
* @date      2016-12-15
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __LED_DEMO_H_
#define __LED_DEMO_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup MODULE_LED  LED
  * @brief modulization for sleep led module for simple application usage.
  * @{
  */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup LED_MODULE_Exported_Types Led Module Exported Types
  * @{
  */

typedef enum
{
    LED_ACTIVE_POLARITY_HIGH    = 0x00, /**< Setting led to high active */
    LED_ACTIVE_POLARITY_LOW     = 0x01, /**< Setting led to low active  */
} T_LED_ACTIVE_POLARITY;

/**
 * @brief LED channels definition.
*/
typedef enum
{
    LED_CH_0 = 0x01,
    LED_CH_1 = 0x02,
    LED_CH_2 = 0x04,
    LED_CH_3 = 0x08,
    LED_CH_4 = 0x10,
    LED_CH_5 = 0x20
} T_LED_CHANNEL;

/**
 * @brief LED modes types defination that each LED could display.
 *
 * Here defines the mostly used LED display types.
*/
typedef enum
{
    /* _____________________ Always OFF */
    LED_TYPE_KEEP_OFF,

    /*   _________________              */
    /* _|                 |_ Always ON  */
    LED_TYPE_KEEP_ON,

    /*   __    __    __    _            */
    /* _|  |__|  |__|  |__|  ON/OFF repeat */
    LED_TYPE_ON_OFF,

    /*      __    __    __              */
    /* ____|  |__|  |__|  |_ OFF/ON repeat */
    LED_TYPE_OFF_ON,

    LED_TYPE_BREATH,

    /* Bypass LED config */
    LED_TYPE_BYPASS = 0xFF
} T_LED_TYPE;

/**
 * @brief LED table structure.
 *
 * Here defines LED display parameters.
*/
typedef struct
{
    T_LED_TYPE type;        //!< led type @ref T_LED_TYPE
    uint8_t on_time;        //!< on time Unit: 10ms
    uint8_t off_time;       //!< off time Unit: 10ms
    uint8_t blink_count;    //!< Range: 0~4
    uint8_t blink_interval; //!< Unit: 100ms
} T_LED_TABLE;

/** @} */ /* End of group LED_MODULE_Exported_Types */

void led_demo_init(void);

void led_demo_blink(void);

void led_demo_breath_init(void);

void led_demo_breath_timer_handler(void);
/** @} */ /* End of group LED_MODULE_Exported_Functions */

/** @} */ /* End of group MODULE_LED */

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* __LED_H_ */

