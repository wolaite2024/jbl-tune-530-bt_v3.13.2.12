/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_sleep_led.c
* @brief    This file provides all the Sleep LED firmware functions.
* @details
* @author   howie_wang
* @date     2016-04-18
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_sleep_led.h"


uint32_t SleepLed_GetControlRegister(SLEEP_LED_CHANNEL channel)
{
    uint32_t led_control_register;

    switch (channel)
    {
    case LED_CHANNEL_0:
        led_control_register = (uint32_t)(&(RTC_LED->S_LED_CH0_CR0));
        break;

    case LED_CHANNEL_1:
        led_control_register = (uint32_t)(&(RTC_LED->S_LED_CH1_CR0));
        break;

    case LED_CHANNEL_2:
        led_control_register = (uint32_t)(&(RTC_LED->S_LED_CH2_CR0));
        break;

    default:
        led_control_register = 0xFFFFFFFF;
        break;
    }

    return led_control_register;
}

/**
  * @brief  Fast write LED register
  * @param  offset: the offset of LED register .
  * @param  data:
  * @retval None
  */
void SleepLed_RegisterWrite(uint32_t offset, uint32_t data)
{

    *((volatile uint32_t *)0x400004f0) = data;
    *((volatile uint32_t *)0x400004f4) = offset;
    *((volatile uint32_t *)0x400004f8) = 1;
}

/**
  * @brief  Fast Read LED register,actually you don't need use this function
  * @param  offset: the offset of LED register .
  * @param  data:
  * @retval None
  */
uint32_t SleepLed_RegisterRead(uint32_t addr)
{
    return *((volatile uint32_t *)addr);
}

/**
  * @brief  Clear all the Sleep LED registers to their default reset
  *         values.
  * @param  None
  * @retval None
  */
void SleepLed_Reset(void)
{
    SleepLed_RegisterWrite(0x00, 0x0);
    SleepLed_RegisterWrite((uint32_t)(&(RTC_LED->S_LED_CR)),
                           0x00); // 0:Reset LED register & disable LED
    SleepLed_RegisterWrite((uint32_t)(&(RTC_LED->S_LED_CR)), 0x01); // 1:Release LED Reset
}

/**
  * @brief  Clear Sleep LED registers of specified channel.
  * @param  None
  * @retval None
  */
void SleepLed_DeInit(SLEEP_LED_CHANNEL channel)
{
    uint32_t reg_val = 0;
    uint32_t active_polarity;
    uint32_t led_control_register;

    /* Get channel CR */
    led_control_register = SleepLed_GetControlRegister(channel);
    active_polarity = SleepLed_RegisterRead(led_control_register) & BIT(29);

    //Clear LED period
    reg_val |= active_polarity;
    SleepLed_RegisterWrite(led_control_register, reg_val);
    SleepLed_RegisterWrite(led_control_register + 0x04, reg_val);

    //Clear LED phase
    for (uint8_t i = 0; i < 8; i++)
    {
        SleepLed_RegisterWrite(led_control_register + 0x08 + 0x04 * i, reg_val);
    }
}

/**
  * @brief  Initializes the Sleep LED Channelx according to the specified
  *         parameters in the Sleep LED_InitStruct.
  * @param  channel: can be LED_CHANNEL_0 to LED_CHANNEL_2 to select the Sleep LED Channel.
  * @param  SLEEP_LED_InitTypeDef: pointer to a SLEEP_LED_InitTypeDef structure that
  *         contains the configuration information for the specified Sleep LED Channel.
  * @retval None
  */
void SleepLed_Init(SLEEP_LED_CHANNEL channel, SLEEP_LED_InitTypeDef *LED_InitStruct)
{
    uint32_t reg_val;
    uint32_t active_polarity;
    uint32_t led_control_register;

    /* Check the parameters */
    assert_param(IS_LED_MODE(LED_InitStruct->mode));

    *((volatile uint32_t *)0x40000014) |= BIT(9);//no need run this every time

    /* Get channel CR */
    led_control_register = SleepLed_GetControlRegister(channel);
    active_polarity = SleepLed_RegisterRead(led_control_register) & BIT(29);

    /* config LED mode, blink or breathe */
    if (LED_InitStruct->mode == LED_BLINK_MODE)
    {
        //Set LED period
        reg_val = (LED_InitStruct->polarity << 30) | //Invert output polarity
                  (LED_InitStruct->prescale << 16) | //Timetick = 10ms
                  (LED_InitStruct->period_high[0] << 8) |
                  LED_InitStruct->period_low[0];
        reg_val |= active_polarity;
        SleepLed_RegisterWrite(led_control_register, reg_val);

        reg_val = (LED_InitStruct->period_high[2] << 24) |
                  (LED_InitStruct->period_low[2] << 16) |
                  (LED_InitStruct->period_high[1] << 8) |
                  LED_InitStruct->period_low[1];
        SleepLed_RegisterWrite(led_control_register + 0x04, reg_val);
    }
    else
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            reg_val = (LED_InitStruct->phase_uptate_rate[i] << 28) |
                      (LED_InitStruct->phase_phase_tick[i] << 16) |
                      (LED_InitStruct->phase_initial_duty[i] << 7) |
                      (LED_InitStruct->phase_increase_duty[i] << 6) |
                      LED_InitStruct->phase_duty_step[i];
            SleepLed_RegisterWrite(led_control_register + 0x08 + 0x04 * i, reg_val);
        }

        reg_val = BIT(31) | //Breath LED mode
                  (LED_InitStruct->polarity) << 30 |
                  (LED_InitStruct->prescale << 16);
        reg_val |= active_polarity;

        //Set breath LED
        SleepLed_RegisterWrite(led_control_register, reg_val);
    }
}

/**
  * @brief  Fills each LED_InitStruct member with its default value.
  * @param  SLEEP_LED_InitTypeDef : pointer to a LED_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void SleepLed_StructInit(SLEEP_LED_InitTypeDef *LED_InitStruct)
{
    /*-------------- Reset DMA init structure parameters values ------------------*/
    LED_InitStruct->mode                        = LED_BREATHE_MODE;
    LED_InitStruct->polarity                    = LED_OUTPUT_NORMAL;
    LED_InitStruct->prescale                    = LED_TIME_TICK_SCALE;

    for (uint8_t i = 0; i < 3; i++)
    {
        LED_InitStruct->period_high[i]          = 0;
        LED_InitStruct->period_low[i]           = 0;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        LED_InitStruct->phase_uptate_rate[i]    = 0;
        LED_InitStruct->phase_phase_tick[i]     = 0;
        LED_InitStruct->phase_initial_duty[i]   = 0;
        LED_InitStruct->phase_increase_duty[i]  = 0;
        LED_InitStruct->phase_duty_step[i]      = 0;
    }
}

/**
  * @brief  Enables or disables the specified LED Channelx.
  * @param  channel: specified LED Channelx. can specify 1 ~ 3 channels.
  * @param  NewState: new state of the LED Channelx.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SleepLed_Cmd(uint8_t channel, FunctionalState NewState)
{
    uint32_t led_control_register;
    uint32_t ctrl_value;

    led_control_register = (uint32_t)(&(RTC_LED->S_LED_CR));
    ctrl_value = SleepLed_RegisterRead(led_control_register);
    ctrl_value |= BIT(0); //S_LED_CR[0] set to 1: Release LED Reset

    if (NewState != DISABLE)
    {
        /* Enable the selected LED Channelx */
        if (channel & LED_CHANNEL_0)
        {
            ctrl_value |= BIT(31);
        }
        if (channel & LED_CHANNEL_1)
        {
            ctrl_value |= BIT(30);
        }
        if (channel & LED_CHANNEL_2)
        {
            ctrl_value |= BIT(29);
        }
    }
    else
    {
        /* Disable the selected LED Channelx */
        if (channel & LED_CHANNEL_0)
        {
            ctrl_value &= ~BIT(31);
        }
        if (channel & LED_CHANNEL_1)
        {
            ctrl_value &= ~BIT(30);
        }
        if (channel & LED_CHANNEL_2)
        {
            ctrl_value &= ~BIT(29);
        }
    }

    SleepLed_RegisterWrite(led_control_register, ctrl_value);
}

/**
  * @brief  Config idle state of the specified LED Channelx.
  * @param  channel: specified LED Channelx. can specify 1 ~ 3 channels.
  * @param  State: idle output value of Channelx.
  *   This parameter can be: LED_IDLE_HIGH or LED_IDLE_LOW.
  * @retval None
  */
void SleepLed_SetIdleMode(SLEEP_LED_CHANNEL channel, uint8_t state)
{
    uint32_t led_control_register;
    uint32_t ctrl_value;

    /* Get channel CR */
    led_control_register = SleepLed_GetControlRegister(channel);
    ctrl_value = SleepLed_RegisterRead(led_control_register);

    if (state != 0)
    {
        ctrl_value |= BIT(29);
    }
    else
    {
        ctrl_value &= ~BIT(29);
    }
    SleepLed_RegisterWrite(led_control_register, ctrl_value);
}

