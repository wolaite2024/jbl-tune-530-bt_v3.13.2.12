/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_tim.h
* @brief
* @details
* @author    elliot chen
* @date      2015-05-15
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __RTL876X_TIM_H
#define __RTL876X_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include <stdbool.h>

#define TIMER0_LOAD_COUNT2 *((volatile uint32_t *)0x400250b0UL)
#define TIMER1_LOAD_COUNT2 *((volatile uint32_t *)0x400250b4UL)

#define TIMER2_LOAD_COUNT2 *((volatile uint32_t *)0x400250b8UL)
#define TIMER3_LOAD_COUNT2 *((volatile uint32_t *)0x400250bCUL)
#define TIMER4_LOAD_COUNT2 *((volatile uint32_t *)0x400250C0UL)
#define TIMER5_LOAD_COUNT2 *((volatile uint32_t *)0x400250c4UL)
#define TIMER6_LOAD_COUNT2 *((volatile uint32_t *)0x400250c8UL)
#define TIMER7_LOAD_COUNT2 *((volatile uint32_t *)0x400250ccUL)

/* TIM Private Defines */
#define TIMER_PWM2_CR *((volatile uint32_t *)0x40000364UL)
#define TIMER_PWM3_CR *((volatile uint32_t *)0x40000368UL)

/** @addtogroup 87x3e_TIM TIM
  * @brief TIM driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup 87x3e_TIM_Exported_Types TIM Exported Types
  * @{
  */

/**
  * @brief  TIM Init structure definition
  */
typedef struct
{
    uint16_t TIM_ClockSrc;          /*!< <b>Deprecated</b> use RCC instead*/
    uint16_t TIM_DIV;               /*!< Clock fix at 40Mhz,add this parameter div the clock*/

    uint16_t TIM_SOURCE_DIV;        /*!< Specifies the clock source div.
                                            This parameter can be a value of @ref TIM_Clock_Divider*/
    uint16_t TIM_SOURCE_DIV_En;     /*!< timer source clock div enable*/
    uint16_t TIM_Mode;              /*!< Specifies the counter mode.
                                        This parameter can be a value of @ref TIM_Mode */

    uint16_t TIM_PWM_En;            /*!< Specifies the PWM mode.
                                        This parameter can be a value of DISABLE or ENABLE */
    uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                         Auto-Reload Register at the next update event.
                                        This parameter must range from 0x0000 to 0xFFFFFFFF.  */
    uint32_t TIM_PWM_High_Count;    /*!< Specifies the PWM High Count.
                                        This parameter must range from 0x0000 to 0xFFFFFFFF. */
    uint32_t TIM_PWM_Low_Count;     /*!< Specifies the PWM Low Count.
                                        This parameter must range from 0x0000 to 0xFFFFFFFF. */
    uint32_t TIM_EventMode;         /*!< Specifies the TIM event mode */

    uint32_t TIM_EventIndex;        /*!< Specifies the TIM event index */

    uint32_t TIM_EventDuration;     /*!< Specifies the TIM event duration*/

    uint8_t  ClockDepend;           /*!< Specifies TIM Source depend.
                                        timer3 depend timer2 ,timer5 depend timer4 timer7 depend timer6
                                        This parameter can be a value of ENABLE or DISABLE */

    uint32_t PWM_Deazone_Size;      /*!<Size of deadzone time, DeadzoneTime=deadzonesize/32000 or 32768
                                        This parameter must range from 1 to 0xffffffff */
    uint16_t PWMDeadZone_En;        /*!<PWM Deadzone enable, pwm0_pn: timer2, pwm1_pn:timer3
                                        This parameter can be a value of ENABLE or DISABLE */
    uint16_t PWM_Stop_State_P;      /*!< Specifies the PWM P stop state( voltage level).
                                        This parameter can be a value of @ref PWMDeadZone_Stop_state */
    uint16_t PWM_Stop_State_N;      /*!< Specifies the PWM N stop state( voltage level).
                                        This parameter can be a value of @ref PWMDeadZone_Stop_state */
} TIM_TimeBaseInitTypeDef;

/** End of group 87x3e_TIM_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup 87x3e_TIM_Exported_constants TIM Exported Constants
  * @{
  */

#define IS_TIM_ALL_PERIPH(PERIPH) (((PERIPH) == TIM0) || \
                                   ((PERIPH) == TIM1) || \
                                   ((PERIPH) == TIM2) || \
                                   ((PERIPH) == TIM3) || \
                                   ((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5) || \
                                   ((PERIPH) == TIM6) || \
                                   ((PERIPH) == TIM7))


/** @defgroup 87x3e_TIM_Clock_Divider TIM clock divider
  * @{
  */


#define TIM_CLOCK_DIVIDER_1                           ((uint16_t)0x0)
#define TIM_CLOCK_DIVIDER_2                           ((uint16_t)0x4)
#define TIM_CLOCK_DIVIDER_4                           ((uint16_t)0x5)
#define TIM_CLOCK_DIVIDER_8                           ((uint16_t)0x6)
#define TIM_CLOCK_DIVIDER_40                          ((uint16_t)0x7)
#define IS_TIM_SOURCE_DIVIDER(DIV)              (((DIV) == TIM_CLOCK_DIVIDER_1) || \
                                                 ((DIV) == TIM_CLOCK_DIVIDER_2) || \
                                                 ((DIV) == TIM_CLOCK_DIVIDER_4) || \
                                                 ((DIV) == TIM_CLOCK_DIVIDER_8) ||\
                                                 ((DIV) == TIM_CLOCK_DIVIDER_40))

/** End of group 87x3e_TIM_Exported_constants
  * @}
  */

/** @defgroup 87x3e_TIM_Mode TIM Mode
  * @{
  */

#define TIM_Mode_FreeRun                    ((uint16_t)0x0000)
#define TIM_Mode_UserDefine                 ((uint16_t)0x0001)
#define IS_TIM_MODE(mode) (((mode) == TIM_Mode_FreeRun) || \
                           ((mode) == TIM_Mode_UserDefine))
/** End of group 87x3e_TIM_Mode
  * @}
  */

/** @defgroup 87x3e_TIM_PWM_En TIM PWM Mode Enable
  * @{
  */

#define PWM_ENABLE                        ((uint16_t)0x1)
#define PWM_DISABLE                       ((uint16_t)0x0)
#define IS_TIM_PWM_En(mode) (((mode) == PWM_ENABLE) || \
                             ((mode) == PWM_DISABLE))
/** End of group 87x3e_TIM_PWM_En
  * @}
  */

/** @defgroup 87x3e_TIM_Event_Duration TIM Event Duration
  * @{
  */

#define TIM_EventDuration_32us                    ((uint16_t)0x0000)
#define TIM_EventDuration_64us                    ((uint16_t)0x0001)
#define TIM_EventDuration_128us                   ((uint16_t)0x0002)
#define TIM_EventDuration_256us                   ((uint16_t)0x0003)

#define IS_TIM_Event_DURATION(duration) (((duration) == TIM_EventDuration_32us) || \
                                         ((duration) == TIM_EventDuration_64us) || \
                                         ((duration) == TIM_EventDuration_128us) || \
                                         ((duration) == TIM_EventDuration_256us))
/** End of group 87x3e_TIM_Event_Duration
  * @}
  */

/** @defgroup 87x3e_PWMDeadZone_En PWM DeadZone enable
* @{
*/

#define DEADZONE_ENABLE                        ((uint16_t)0x1)
#define DEADZONE_DISABLE                       ((uint16_t)0x0)
#define IS_TIM_PWM_DeadZone_En(mode) (((mode) == DEADZONE_ENABLE) || \
                                      ((mode) == DEADZONE_DISABLE))
/** End of group 87x3e_PWMDeadZone_En
  * @}
  */

/** @defgroup 87x3e_PWMDeadZone_Stop_state PWM DeadZone stop state
* @{
*/

#define PWM_STOP_AT_HIGH                        ((uint16_t)0x1)
#define PWM_STOP_AT_LOW                         ((uint16_t)0x0)

/** End of group 87x3e_PWMDeadZone_Stop_state
  * @}
  */

/** @defgroup 87x3e_PWMDeadZone_Perioheral PWMDeadZone
* @{
*/


#define IS_PWM_ALL_PERIPH(PERIPH) (((PERIPH) == PWM0_PN) || \
                                   ((PERIPH) == PWM1_PN))

//#define TIM_DeInit() RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, DISABLE);
/** End of group 87x3e_PWMDeadZone_Perioheral
  * @}
  */

/** End of group 87x3e_TIM_Exported_constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup 87x3e_TIM_Exported_Functions TIM Exported Functions
  * @{
  */

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be 0 to 7 to select the TIM peripheral.
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);

/**
  * @brief  Fills each TIM_InitStruct member with its default value.
  * @param  TIM_TimeBaseInitStruct : pointer to a TIM_TimeBaseInitTypeDef structure which will be initialized.
  * @retval None
  */
void TIM_StructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);

/**
  * @brief  change TIM period value.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  period: period value to be changed.
  * @retval The new state of success or not  (SET or RESET).
  */
void TIM_ChangePeriod(TIM_TypeDef *TIMx, uint32_t period);

/**
  * @brief  Enables or disables the specified TIMx interrupt.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_INTConfig(TIM_TypeDef *TIMx, FunctionalState NewState);

/**
  * @brief  Change PWM freq and duty according high_cnt and low_cnt
  * @param  TIMx: where x can be 2 to 3 to select the TIMx peripheral.
  * @param  high_count:
    *   This parameter can be:
    * @param  low_count:
  *   This parameter can be:
  * @retval None
  */
void TIM_PWMChangeFreqAndDuty(TIM_TypeDef *TIMx, uint32_t high_count, uint32_t low_count);


/**
  * @brief  Get TIMx current value when timer is running.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The counter value.
  */
__forceinline uint32_t TIM_GetCurrentValue(TIM_TypeDef *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    return TIMx->CurrentValue;
}

/**
  * @brief  Get TIMx elapsed value when timer is running.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The elapsed counter value.
  */
__forceinline uint32_t TIM_GetElapsedValue(TIM_TypeDef *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    return (TIMx->LoadCount - TIMx->CurrentValue);
}

/**
  * @brief  Checks whether the TIM has been enabled or disabled.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The state of the TIM(ENABLE or DISABLE).
  */
__forceinline FunctionalState TIM_GetStatus(TIM_TypeDef *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    if (TIMx->ControlReg & BIT(0))
    {
        return ENABLE;
    }
    else
    {
        return DISABLE;
    }
}

/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The new state of the TIM_IT(SET or RESET).
  */
__forceinline ITStatus TIM_GetINTStatus(TIM_TypeDef *TIMx)
{
    ITStatus bitstatus = RESET;
    uint16_t itstatus = (uint16_t)TIMx->IntStatus;

    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    if (itstatus != (uint16_t)RESET)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Cclear the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval none
  */
__forceinline void TIM_ClearINT(TIM_TypeDef *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    /* Clear the IT */
    TIMx->EOI;
}

/**
* @brief  PWM_deadzone mode mergence stop.
* @param  PWMx: PWM0_PN or PWM1_PN
* @retval none
*/
__forceinline void PWM_Deadzone_EMStop(PWM_TypeDef *PWMx)
{
    /* Check the parameters */
    assert_param(IS_PWM_ALL_PERIPH(PWMx));

    PWMx->CR |= BIT(8);
}

/**
  * @brief  Change PWM clock source,function and clock enable.
    * @param  TIM2:pwm0.  TIM3:pwm1.
    * @param  clock_source  0: 32K. 1: 5Mhz.
    * @param  NewState  enable or disable PWM_PN function and clock.
    *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_PWMClockConfig(TIM_TypeDef *TIMx, bool clock_source, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_TIM_H*/

/** @} */ /* End of group 87x3e_TIM_Exported_Functions */
/** @} */ /* End of group 87x3e_TIM */


/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

