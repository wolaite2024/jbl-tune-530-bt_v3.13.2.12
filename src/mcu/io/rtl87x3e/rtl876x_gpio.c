/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_gpio.c
* @brief    This file provides all the GPIO firmware functions.
* @details
* @author   elliot chen
* @date     2015-05-20
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
#include "string.h"

#define _IS_ASIC_

#ifdef _IS_ASIC_
#define GPIO_CLOCK_SOURCE         (40000000)
#define GPIO_CLOCK_SOURCE_KHZ     (40000)
#else
#define GPIO_CLOCK_SOURCE         (20000000)
#define GPIO_CLOCK_SOURCE_KHZ     (20000)
#endif

const uint8_t GPIO_mapping_tbl[TOTAL_PIN_NUM] =
{
    0,  1,  2,  3,  /*P0_0~P0_3*/
    4,  5,  6, 38,  /*P3_2 ~5*/
    7,  8,  9, 10,  /*P1_0~P1_3*/
    11, 12, 13, 14, /*P1_4~P1_7*/
    15, 16, 17, 18, /*P2_0~P2_3*/
    19, 20, 21, 22, /*P2_4~P2_7*/

    23, 24, 34, 35,         /*  P3_0 P3_1  AUX_R,  AUX_L  */
    27, 28, 25, 26,         /*  MIC1_P  ,MIC1_N , MIC2_P,  MIC2_N, */
    29, 30, 31, 32,         /*  MICBIAS,  LOUT_P ,LOUT_N, ROUT_P,  */
    33, 36, 37, 0xff,       /*  ROUT_N, MIC3_P,  MIC3_N ,  NULL   */

    39, 40, 41, 42,     /*P4_0~P4_3*/
    43, 44, 45, 46,     /*P4_4~P4_7*/

    47, 48, 49, 50,     /*P5_0~P5_3*/
    32, 33, 34, 35,     /*P5_4~P5_7*/

    25, 26, 27, 28,       /*P6_0~P6_3*/
    29, 30, 31, 0xff,   /*P6_4~P6_7*/

    51, 52, 53, 54,     /*P7_0~P7_3*/
    55, 56, 57, 0xff,   /*P7_4~P7_7*/

    58, 59, 60, 61,       /*P8_0~P8_3*/
    62, 63, 0xff, 0xff, /*P8_4~P8_7*/

    32, 33, 34, 35,         /*P9_0~P9_3*/
    36, 37, 0xff, 0xff, /*P9_4~P9_7*/

};

/**
  * @brief  Deinitializes the GPIO peripheral registers to their default reset values.
  * @param  None.
  * @retval None.
  */
void GPIOx_DeInit(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, DISABLE);
    }
    else
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, DISABLE);
    }
}

/**
  * @brief  Initializes the GPIO A and B peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIOx_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_PinBit));
    assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
    assert_param(IS_GPIOIT_LEVEL_TYPE(GPIO_InitStruct->GPIO_ITTrigger));
    assert_param(IS_GPIOIT_POLARITY_TYPE(GPIO_InitStruct->GPIO_ITPolarity));
    assert_param(IS_GPIOIT_DEBOUNCE_TYPE(GPIO_InitStruct->GPIO_ITDebounce));

    /* GPIOx configure */
    if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT)
    {
        GPIOx->DATADIR |= GPIO_InitStruct->GPIO_PinBit;

        if (GPIO_InitStruct->GPIO_ControlMode == GPIO_SOFTWARE_MODE)
        {
            /* Config GPIOx control software mode */
            GPIOx->DATASRC &= (~GPIO_InitStruct->GPIO_PinBit);
        }
        else
        {
            /* Config GPIOx hardware control mode */
            GPIOx->DATASRC |= (GPIO_InitStruct->GPIO_PinBit);
        }

    }
    else
    {
        /*Configure GPIO input mode */
        GPIOx->DATADIR = GPIOx->DATADIR & (~GPIO_InitStruct->GPIO_PinBit);

        if (GPIO_InitStruct->GPIO_ITCmd == ENABLE)
        {

            GPIOx->INTMASK = ~GPIO_Pin_All;

            /* configure GPIO interrupt trigger type */
            if (GPIO_InitStruct->GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL)
            {
                GPIOx->INTTYPE = GPIOx->INTTYPE & (~GPIO_InitStruct->GPIO_PinBit);

                /* Level-sensitive synchronization enable register */
                GPIOx->LSSYNC |= GPIO_InitStruct->GPIO_PinBit;
            }
            else if (GPIO_InitStruct->GPIO_ITTrigger == GPIO_INT_Trigger_EDGE)
            {
                GPIOx->INTTYPE = (GPIOx->INTTYPE & (~GPIO_InitStruct->GPIO_PinBit))
                                 | GPIO_InitStruct->GPIO_PinBit;
            }
            else
            {
                GPIOx->INTBOTHEDGE |= GPIO_InitStruct->GPIO_PinBit;
            }

            /* configure Interrupt polarity register */
            if (GPIO_InitStruct->GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_LOW)
            {
                GPIOx->INTPOLARITY = GPIOx->INTPOLARITY & (~GPIO_InitStruct->GPIO_PinBit);
            }
            else
            {
                GPIOx->INTPOLARITY = (GPIOx->INTPOLARITY & (~GPIO_InitStruct->GPIO_PinBit))
                                     | GPIO_InitStruct->GPIO_PinBit;
            }
            /* Configure Debounce enable register */
            if (GPIO_InitStruct->GPIO_ITDebounce == GPIO_INT_DEBOUNCE_DISABLE)
            {
                GPIOx->DEBOUNCE = GPIOx->DEBOUNCE & (~GPIO_InitStruct->GPIO_PinBit);
            }
            else
            {
                GPIOx->DEBOUNCE = (GPIOx->DEBOUNCE & (~GPIO_InitStruct->GPIO_PinBit))
                                  | GPIO_InitStruct->GPIO_PinBit;

#ifdef _IS_ASIC_

                /* Config debounce time , default debounce DIV is 14*/
                if (GPIOx == GPIOA)
                {
                    GPIO_DBCLK_DIV &= 0xffff0000;
                    GPIO_DBCLK_DIV |= (((0xd) << 8) | (1 << 12));
                    GPIO_DBCLK_DIV |= ((((GPIO_InitStruct->GPIO_DebounceTime) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                         (14)) - 1) & 0xff);
                }
                else if (GPIOx == GPIOB)
                {
                    GPIO_DBCLK_DIV &= 0x0000ffff;
                    GPIO_DBCLK_DIV |= (((0xd) << 24) | (1 << 28));
                    GPIO_DBCLK_DIV |= (((((GPIO_InitStruct->GPIO_DebounceTime) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                          (14)) - 1) << 16) & 0xff0000);
                }
#else

                if (GPIOx == GPIOA)
                {
                    /* Config debounce time , default debounce DIV is 13*/
                    GPIO_DBCLK_DIV &= 0xffff0000;
                    GPIO_DBCLK_DIV |= (((0x3) << 10) | (1 << 12));
                    GPIO_DBCLK_DIV |= ((((GPIO_InitStruct->GPIO_DebounceTime) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                         (13)) - 1) & 0xff);
                }
                else if (GPIOx == GPIOB)
                {
                    /* Config debounce time , default debounce DIV is 13*/
                    GPIO_DBCLK_DIV &= 0x0000ffff;
                    GPIO_DBCLK_DIV |= (((0x3) << 26) | (1 << 28));
                    GPIO_DBCLK_DIV |= (((((GPIO_InitStruct->GPIO_DebounceTime) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                          (13)) - 1) << 16) & 0xff0000);
                }
#endif
            }

            /* Configure Interrupt enable register */
            //GPIOx->INTEN |= GPIO_InitStruct->GPIO_PinBit;
        }
    }
}

/**
  * @brief    Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will
  *    be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct)
{
    memset(GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
    /* Reset GPIO init structure parameters values */
    GPIO_InitStruct->GPIO_PinBit  = GPIO_Pin_All;
    GPIO_InitStruct->GPIO_DebounceTime = 20;                 /* ms , can be 1~64 ms */
}

/**
  * @brief enable the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIOx_INTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, FunctionalState NewState)
{

    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected GPIO pin interrupts */
        GPIOx->INTEN |= GPIO_PinBit;
    }
    else
    {
        /* Disable the selected GPIO pin interrupts */
        GPIOx->INTEN &= ~GPIO_PinBit;
    }
}

/**
  * @brief clear the specified GPIO interrupt.
  * @param GPIOx choose GPIOA or GPIOB
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIOx_ClearINTPendingBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));

    GPIOx->INTCLR = GPIO_PinBit;
}


/**
  * @brief mask the specified GPIO interrupt.
  * @param GPIOx choose GPIOA or GPIOB
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIOx_MaskINTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        GPIOx->INTMASK |= GPIO_PinBit;
    }
    else
    {
        GPIOx->INTMASK &= ~(GPIO_PinBit);
    }
}

/**
  * @brief get the specified GPIO pin BIT.
  * @param  Pin_num: This parameter is from ADC_0 to P9_5, please refer to rtl876x.h .
  * @retval  GPIO pin  bit for GPIO initialization.
  */
uint32_t GPIO_GetPin(uint8_t Pin_num)
{
    /* Check the parameters */
    assert_param(IS_PIN_NUM(Pin_num));

    if (Pin_num >= TOTAL_PIN_NUM)
    {
        return 0xff;
    }

    uint8_t GPIO_num = GPIO_mapping_tbl[Pin_num];

    if ((GPIO_num >= 32) && (GPIO_num < 64))
    {
        GPIO_num = GPIO_num - 32;
    }
    else if (GPIO_num >= 64)
    {
        return 0xFF;
    }
    return BIT(GPIO_num);
}

/**
  * @brief get the specified GPIO pin number.
  * @param  Pin_num: This parameter is from ADC_0 to P9_5, please refer to rtl876x.h
  * @retval  GPIO pin number.
  */
uint8_t GPIO_GetNum(uint8_t Pin_num)
{
    /* Check the parameters */
    assert_param(IS_PIN_NUM(Pin_num));

    if (Pin_num >= TOTAL_PIN_NUM)
    {
        return 0xff;
    }

    uint8_t GPIO_num = GPIO_mapping_tbl[Pin_num];
    if (GPIO_num < 64)
    {
        return GPIO_num;
    }
    else
    {
        return 0xFF;
    }
}
#if 0  //remove the code for save size
/**
  * @brief Enable the debance clk of GPIOx.
  * @param GPIOx choose GPIOA or GPIOB
  * @param Pin_num: This parameter is from ADC_0 to H4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval  none.
  */
void GPIOx_DBClkCmd(GPIO_TypeDef *GPIOx, FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        if (GPIOx == GPIOA)
        {
            GPIO_DBCLK_DIV |= BIT12;
        }
        else if (GPIOx == GPIOB)
        {
            GPIO_DBCLK_DIV |= BIT28;
        }
    }
    else
    {
        if (GPIOx == GPIOA)
        {
            GPIO_DBCLK_DIV &= ~BIT12;
        }
        else if (GPIOx == GPIOB)
        {
            GPIO_DBCLK_DIV &= ~BIT28;
        }
    }
}
#endif

void GPIOx_IntPolaritySet(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, GPIOIT_PolarityType int_type)
{
    GPIOx->INTMASK |= GPIO_PinBit;

    if (GPIO_INT_POLARITY_ACTIVE_HIGH == int_type)
    {
        GPIOx->INTPOLARITY |= GPIO_PinBit;
    }
    else if (GPIO_INT_POLARITY_ACTIVE_LOW == int_type)
    {
        GPIOx->INTPOLARITY &= ~GPIO_PinBit;
    }

    GPIOx->INTCLR = GPIO_PinBit;
    GPIOx->INTMASK &= ~GPIO_PinBit;
}


/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

