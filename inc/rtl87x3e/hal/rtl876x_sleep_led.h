/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_sleep_led.h
* @brief
* @details
* @author    howie_wang
* @date      2016-04-18
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_SLEEP_LED_H_
#define _RTL876X_SLEEP_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"


#define LED_TIME_TICK_SCALE     320 //Unit: 1/32k sec, Range: 1~1023, Fix in 320, i.e. 10ms. This reg also effect PWM freq: 640->pwm freq become 50Hz
#define LED_BREATH_PHASE_NUM    8


/** @addtogroup HAL_87x3e_LED LED
  * @brief LED driver module
  * @{
  */

/** @defgroup LED_Exported_Types LED Exported Types
  * @{
  */

/**
 * @brief SleepLed initialize parameters
 *
 * SleepLed initialize parameters
 */
typedef struct
{
    uint8_t mode;               /*!< Specifies the LED working mode.
                                                    This parameter can be a value of @ref LED_mode */
    uint8_t polarity;           /*!< Specifies the LED polarity.
                                                    This parameter can be a value of @ref LED_Output_polarity */
    uint16_t prescale;          /*!< Specifies the LED prescale value.
                                                     This parameter can be a value of 0-1023 */
    uint8_t period_high[3];     /*!< Specifies the LED blink mode period high.
                                                     This parameter can be a value of 0-1023 */
    uint8_t period_low[3];      /*!< Specifies the  LED blink mode period low.
                                                       This parameter can be a value of 0-1023 */

    uint8_t phase_uptate_rate[8];  /*!< Specifies the update rate. duty_step interval = (uptate_rate + 1) * phase_tick.
                                                     This parameter can be a value of Range: 0~15. */
    // Range: 0~15. duty_step interval = (uptate_rate + 1) * phase_tick
    uint16_t phase_phase_tick[8];    /*!< Specifies Unit: 10ms, Range: 1~1023, 0: Bypass this phase */

    uint16_t phase_initial_duty[8]; /*!< Specifies LED_TIME_TICK_SCALE = 100%, i.e. 3.2-->1% 320-->100% */

    uint8_t phase_increase_duty[8]; /*!< 1: Increase duty, 0: Decrease duty. It must be 0 if duty step = 0 or HW become abnormal */

    uint8_t phase_duty_step[8];   /*!< Range: 0~LED_TIME_TICK_SCALE, i.e. 3.2-->1% 320-->100% */

} SLEEP_LED_InitTypeDef;

/** End of group LED_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup LED_Exported_Constants LED Exported Constants
  * @{
  */

/**
  * @brief LED channel enumeration
  */


typedef enum
{
    LED_CHANNEL_0 = 0x01,
    LED_CHANNEL_1 = 0x02,
    LED_CHANNEL_2 = 0x04
} SLEEP_LED_CHANNEL;


/** @defgroup LED_mode LED working mode
  * @{
  */

#define LED_BLINK_MODE              ((uint32_t)0x00000000)
#define LED_BREATHE_MODE            ((uint32_t)0x00000001)
#define IS_LED_MODE(MODE)           (((MODE) == LED_BLINK_MODE) || \
                                     ((MODE) == LED_BREATHE_MODE))

/** End of group LED_mode
  * @}
  */

/** @defgroup LED_Output_polarity LED output polarity
  * @{
  */

#define LED_OUTPUT_NORMAL           ((uint32_t)0x00000000)
#define LED_OUTPUT_INVERT           ((uint32_t)0x00000001)
#define IS_LED_OUTPUT_MODE(MODE)    (((MODE) == LED_OUTPUT_NORMAL) || \
                                     ((MODE) == LED_OUTPUT_INVERT))
/** End of group LED_Output_polarity
  * @}
  */

/** @defgroup LED_Idle_State  LED Idle State
  * @{
  */

#define LED_IDLE_HIGH               ((uint8_t)0x01)
#define LED_IDLE_LOW                ((uint8_t)0x00)
#define IS_LED_IDLE_MODE(MODE)    (((MODE) == LED_IDLE_HIGH) || \
                                   ((MODE) == LED_IDLE_LOW))
/** End of group LED_Output_polarity
  * @}
  */

/** End of group LED_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup LED_Exported_Functions LED Exported Functions
  * @{
  */

/**
  * @brief  Clear all the Sleep LED registers to their default reset
  *         values.
  * @param  None
  * @retval None
  */
void SleepLed_Reset(void);

/**
  * @brief  Clear Sleep LED registers of specified channel.
  * @param  None
  * @retval None
  */
void SleepLed_DeInit(SLEEP_LED_CHANNEL channel);

/**
  * @brief  Initializes the Sleep LED Channelx according to the specified
  *         parameters in the Sleep LED_InitStruct.
  * @param  channel: can be LED_CHANNEL_0 to LED_CHANNEL_2 to select the Sleep LED Channel.
  * @param  LED_InitStruct: pointer to a SLEEP_LED_InitTypeDef structure that
  *         contains the configuration information for the specified Sleep LED Channel.
  * @retval None
  */
void SleepLed_Init(SLEEP_LED_CHANNEL channel, SLEEP_LED_InitTypeDef *LED_InitStruct);

/**
  * @brief  Fills each LED_InitStruct member with its default value.
  * @param  LED_InitStruct : pointer to a LED_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void SleepLed_StructInit(SLEEP_LED_InitTypeDef *LED_InitStruct);

/**
  * @brief  Enables or disables the specified LED Channelx.
  * @param  channel: specified LED Channelx. can specify 1 ~ 3 channels.
  * @param  NewState: new state of the LED Channelx.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SleepLed_Cmd(uint8_t channel, FunctionalState NewState);

/**
  * @brief  Enables or disables the specified LED Channelx.
  * @param  channel: specified LED Channelx. can specify 1 ~ 3 channels.
  * @param  State: idle output value of Channelx.
  *   This parameter can be: LED_IDLE_HIGH or LED_IDLE_LOW.
  * @retval None
  */
void SleepLed_SetIdleMode(SLEEP_LED_CHANNEL channel, uint8_t State);

/**
  * @brief  Fast write LED register
  * @param  offset: the offset of LED register .
  * @param  data: data to write
  * @retval None
  */
void SleepLed_RegisterWrite(uint32_t offset, uint32_t data);

/**
  * @brief  Fast Read LED register,actually you don't need use this function
  * @param  addr: the address of LED register .
  * @retval None
  */
uint32_t SleepLed_RegisterRead(uint32_t addr);


/** @} */ /* End of group LED_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_LED */

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_SLEEP_LED_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

