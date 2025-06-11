/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "rtl876x_tim.h"
#include "rtl876x_rcc.h"
#include "hw_tim_internal.h"
#include "section.h"

IRQn_Type hw_timer_get_irqn_from_id(T_HW_TIMER_ID id)
{
    if ((id >= HW_TIMER_ID_2) && (id <= HW_TIMER_ID_4))
    {
        return (IRQn_Type)(TIM2_IRQn + id - HW_TIMER_ID_2);
    }
    else if ((id > HW_TIMER_ID_4) && (id <= HW_TIMER_ID_7))
    {
        return (IRQn_Type)(TIM5_IRQn + id - HW_TIMER_ID_5);
    }
    else
    {
        return (IRQn_Type)0;
    }
}

VECTORn_Type hw_timer_get_vectorn_from_id(T_HW_TIMER_ID id)
{
    if ((id >= HW_TIMER_ID_2) && (id <= HW_TIMER_ID_4))
    {
        return (VECTORn_Type)(Timer2_VECTORn + id - HW_TIMER_ID_2);
    }
    else if ((id > HW_TIMER_ID_4) && (id <= HW_TIMER_ID_7))
    {
        return (VECTORn_Type)(Timer5_VECTORn + id - HW_TIMER_ID_5);
    }
    else
    {
        return (VECTORn_Type)0;
    }
}

RAM_TEXT_SECTION void hal_timer_handler(void)
{
    uint32_t ipsr = __get_IPSR();
    if ((ipsr >= Timer2_VECTORn) && (ipsr <= Timer4_VECTORn))
    {
        hw_timer_handler_imp((T_HW_TIMER_ID)(ipsr - Timer2_VECTORn + HW_TIMER_ID_2));
    }
    else if ((ipsr >= Timer5_VECTORn) && (ipsr <= Timer7_VECTORn))
    {
        hw_timer_handler_imp((T_HW_TIMER_ID)(ipsr - Timer5_VECTORn + HW_TIMER_ID_5));
    }
}

uint8_t pwm_src_div_mapping(T_PWM_CLOCK_SOURCE source)
{
    switch (source)
    {
    case PWM_CLOCK_1M:
        return TIM_CLOCK_DIVIDER_40;
    case PWM_CLOCK_5M:
        return TIM_CLOCK_DIVIDER_8;
    case PWM_CLOCK_10M:
        return TIM_CLOCK_DIVIDER_4;
    case PWM_CLOCK_20M:
        return TIM_CLOCK_DIVIDER_2;
    case PWM_CLOCK_40M:
        return  TIM_CLOCK_DIVIDER_1;
    case PWM_CLOCK_MAX:
        return TIM_CLOCK_DIVIDER_40;
    case PWM_CLOCK_PLL:
        /* TIM2-7 clock source switch to pll */
        RCC_TimSourceConfig(TIM2TO7_CLOCK_DIV_1, TIM_CLOCK_SOURCE_PLL, ENABLE);
        /* select pll2 as clock source */
        CLK_SOURCE_REG_0 |= BIT8;
        return TIM_CLOCK_DIVIDER_1;
    default:
        return TIM_CLOCK_DIVIDER_40;
    }
}

void hw_timer_rcc_clock_cmd(T_HW_TIMER_ID id)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
}
