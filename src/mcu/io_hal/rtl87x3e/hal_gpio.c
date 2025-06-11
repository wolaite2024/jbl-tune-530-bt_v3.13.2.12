/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hal_gpio.c
* @brief    This file provides all the gpio hal functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/
#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "section.h"
#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "hal_gpio.h"

#define GPIOB_SUPPORT

#define TOTAL_HAL_GPIO_NUM                     (64)
#define TOTAL_HAL_GPIO_PORT_NUM                (32)

#define TOTAL_HAL_PIN_NUM                      TOTAL_PIN_NUM

//#define PRINT_GPIO_LOGS
#ifdef PRINT_GPIO_LOGS
#define GPIO_PRINT_INFO(fmt, ...)                DBG_DIRECT(fmt, __VA_ARGS__)
#else
#define GPIO_PRINT_INFO(...)
#endif

#define IS_GPIO_INVALID(gpio_num)                   (gpio_num == 0xff)
#define PIN_FLAG_TYPE_BIT                           (0x03)
#define PIN_GET_GPIO_TYPE(pin_index)                ((T_GPIO_TYPE)(hal_gpio_sw_context.pin_flags[pin_index] & PIN_FLAG_TYPE_BIT))

typedef struct t_gpio_state
{
    uint8_t pin_flags[TOTAL_HAL_PIN_NUM];
    uint32_t debounce_time;
} T_GPIO_SW_CONTEXT;

T_GPIO_SW_CONTEXT hal_gpio_sw_context;

ISR_TEXT_SECTION
GPIO_TypeDef *get_gpio(uint8_t gpio_num)
{
    if (gpio_num < 32)
    {
        return GPIOA;
    }
#ifdef GPIOB_SUPPORT
    else if (gpio_num < 64)
    {
        return GPIOB;
    }
#endif
#if GPIOC_SUPPORT
    else if (gpio_num < 96)
    {
        return GPIOC;
    }
#endif
    else
    {
        return NULL;
    }
}

T_GPIO_STATUS hal_gpio_set_pull_value(uint8_t pin_index, T_GPIO_PULL_VALUE pull_value)
{
    Pad_PullEnableValue_Dir(pin_index, 1, (PAD_Pull_Mode)pull_value);
    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_init_pin(uint8_t pin_index, T_GPIO_TYPE type, T_GPIO_DIRECTION direction,
                                T_GPIO_PULL_VALUE pull_value)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint8_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *GPIOx = get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    hal_gpio_sw_context.pin_flags[pin_index] = (hal_gpio_sw_context.pin_flags[pin_index] &
                                                (~PIN_FLAG_TYPE_BIT)) | type;

    switch (type)
    {
    case GPIO_TYPE_CORE:
        Pinmux_Config(pin_index, DWGPIO);
        Pad_ControlSelectValue(pin_index, PAD_PINMUX_MODE);
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        break;

    case GPIO_TYPE_AON:
        Pad_ControlSelectValue(pin_index, PAD_SW_MODE);
        Pad_OutputEnableValue(pin_index, direction);
        break;

    case GPIO_TYPE_AUTO:
        Pinmux_Config(pin_index, DWGPIO);
        Pad_ControlSelectValue(pin_index, PAD_PINMUX_MODE);
        Pad_OutputEnableValue(pin_index, direction);
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        break;

    default:
        break;
    }

    Pad_PullEnableValue_Dir(pin_index, 1, (PAD_Pull_Mode)pull_value);
    Pad_PowerOrShutDownValue(pin_index, 1);

    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_change_direction(uint8_t pin_index, T_GPIO_DIRECTION direction)
{
    T_GPIO_TYPE type = PIN_GET_GPIO_TYPE(pin_index);
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint8_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *GPIOx = get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    switch (type)
    {
    case GPIO_TYPE_CORE:
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        break;

    case GPIO_TYPE_AON:
        Pad_OutputEnableValue(pin_index, direction);
        break;

    case GPIO_TYPE_AUTO:
        Pad_OutputEnableValue(pin_index, direction);
        GPIOx_ModeSet(GPIOx, gpio_pin, (GPIOMode_TypeDef)direction);
        break;

    default:
        break;
    }

    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_set_level(uint8_t pin_index, T_GPIO_LEVEL level)
{
    T_GPIO_TYPE type = PIN_GET_GPIO_TYPE(pin_index);
    uint8_t gpio_num = GPIO_GetNum(pin_index);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    if (type ==  GPIO_TYPE_AON)
    {
        Pad_OutputControlValue(pin_index, level);
    }
    else
    {
        GPIOx_WriteBit(get_gpio(gpio_num), GPIO_GetPin(pin_index), (BitAction)level);
    }

    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_LEVEL hal_gpio_get_input_level(uint8_t pin_index)
{
    T_GPIO_TYPE type = PIN_GET_GPIO_TYPE(pin_index);
    uint8_t gpio_num = GPIO_GetNum(pin_index);

    if (IS_GPIO_INVALID(gpio_num) || (type == GPIO_TYPE_AON))
    {
        return GPIO_LEVEL_UNKNOWN;
    }

    return (T_GPIO_LEVEL)GPIOx_ReadInputDataBit(get_gpio(gpio_num), GPIO_GetPin(pin_index));
}

static T_GPIO_STATUS gpio_set_up_irq_active(uint8_t pin_index, T_GPIO_IRQ_MODE mode,
                                            T_GPIO_IRQ_POLARITY polarity,
                                            bool debounce_enable)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_PinBit  = gpio_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd = ENABLE;
    GPIO_InitStruct.GPIO_ITTrigger = (GPIO_IRQ_EDGE == mode) ? GPIO_INT_Trigger_EDGE :
                                     GPIO_INT_Trigger_LEVEL;
    GPIO_InitStruct.GPIO_ITPolarity = (polarity == GPIO_IRQ_ACTIVE_HIGH) ?
                                      GPIO_INT_POLARITY_ACTIVE_HIGH : GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct.GPIO_ITDebounce = debounce_enable ? GPIO_INT_DEBOUNCE_ENABLE :
                                      GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_InitStruct.GPIO_DebounceTime = hal_gpio_sw_context.debounce_time;

    GPIOx_Init(p_gpio, &GPIO_InitStruct);

    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_set_up_irq(uint8_t pin_index, T_GPIO_IRQ_MODE mode,
                                  T_GPIO_IRQ_POLARITY polarity,
                                  bool debounce_enable)
{
    return gpio_set_up_irq_active(pin_index, mode, polarity, debounce_enable);
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_irq_change_polarity(uint8_t pin_index, T_GPIO_IRQ_POLARITY polarity)
{
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    uint32_t gpio_pin = GPIO_GetPin(pin_index);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    GPIOx_IntPolaritySet(get_gpio(gpio_num), gpio_pin, (GPIOIT_PolarityType)polarity);
    return GPIO_STATUS_OK;
}

ISR_TEXT_SECTION
T_GPIO_STATUS hal_gpio_irq_enable(uint8_t pin_index)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    GPIOx_MaskINTConfig(p_gpio, gpio_pin, (FunctionalState)DISABLE);
    GPIOx_INTConfig(p_gpio, gpio_pin, (FunctionalState)ENABLE);
    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_irq_disable(uint8_t pin_index)
{
    uint32_t gpio_pin = GPIO_GetPin(pin_index);
    uint32_t gpio_num = GPIO_GetNum(pin_index);
    GPIO_TypeDef *p_gpio = get_gpio(gpio_num);

    if (IS_GPIO_INVALID(gpio_num))
    {
        return GPIO_STATUS_ERROR_PIN;
    }

    GPIOx_MaskINTConfig(p_gpio, gpio_pin, (FunctionalState)ENABLE);
    GPIOx_INTConfig(p_gpio, gpio_pin, (FunctionalState)DISABLE);
    return GPIO_STATUS_OK;
}

T_GPIO_STATUS hal_gpio_set_debounce_time(uint8_t ms)
{
    hal_gpio_sw_context.debounce_time = ms;

    return GPIO_STATUS_OK;
}

void hal_gpio_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
#ifdef GPIOB_SUPPORT
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
#endif
#ifdef GPIOC_SUPPORT
    RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, ENABLE);
#endif
}

void hal_gpio_deinit(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, DISABLE);
#ifdef GPIOB_SUPPORT
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, DISABLE);
#endif
#ifdef GPIOC_SUPPORT
    RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, DISABLE);
#endif
}

