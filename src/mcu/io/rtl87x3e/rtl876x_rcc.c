
/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_rcc.c
* @brief    This file provides all the IO clock firmware functions..
* @details
* @author   tifnan_ge
* @date     2015-05-16
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "platform_utils.h"

#define DELAY_US(n) do{ for (uint32_t i = 0;i < 100 * n;i ++); }while(0)
/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMERA
  *     @arg APBPeriph_TIMERB
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_LCD
  *     @arg APBPeriph_SPI2W
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_CODEC
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_TIMERA_CLOCK
  *     @arg APBPeriph_TIMERB_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_LCD_CLOCK
  *     @arg APBPeriph_SPI2W_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t apbRegOff = ((APBPeriph & (0x03 << 26)) >> 26);
    uint32_t clkRegOff = ((APBPeriph_Clock & (0x03 << 29)) >> 29);

    /*Open clock gating first*/
    if (NewState == ENABLE)
    {
        if ((APBPeriph_Clock == APBPeriph_ADC_CLOCK) || (APBPeriph_Clock == APBPeriph_KEYSCAN_CLOCK))
        {

            /*Open 10M clock source*/
            SYSBLKCTRL->u_20C.REG_0x20C |= BIT26;
            SYSBLKCTRL->u_20C.REG_0x20C |= BIT28;
        }
        else if ((APBPeriph_Clock == APBPeriph_QDEC_CLOCK) || (APBPeriph_Clock == APBPeriph_SPI2W_CLOCK))
        {
            /*Open 20M clock source*/
            SYSBLKCTRL->u_20C.REG_0x20C |= BIT26;
            SYSBLKCTRL->u_20C.REG_0x20C |= BIT27;
        }
        else if (APBPeriph_Clock == APBPeriph_TIMER_CLOCK)
        {
            /* Enable TIM1~2 TIM9~TIM12fixed 40M tim15*/
            CLK_SOURCE_REG_2 |= BIT9;
            /* enable TIM3~8 ICG*/
            CLK_SOURCE_REG_0 |= 0xB0;
            /* enable TIM3~8 ICG*/
            CLK_SOURCE_REG_0 |= BIT4;
        }
    }
    /* Special register handle */
    if (NewState == ENABLE)
    {
        if ((APBPeriph_Clock == APBPeriph_I2S0_CLOCK) || ((APBPeriph_Clock == APBPeriph_I2S1_CLOCK)) ||
            (APBPeriph_Clock == APBPeriph_CODEC_CLOCK))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN |= APBPeriph | APBPeriph_Clock;
            return;
        }
        if (APBPeriph == APBPeriph_CAP)
        {
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_1M_SRC_EN = 1;
            SYSBLKCTRL->u_234.BITS_234.BIT_CKE_CAP = 1;
            return;
        }
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            RCC_PeriClockConfig(APBPeriph_Clock, ENABLE);
            return;
        }
    }
    else
    {
        if ((APBPeriph_Clock == APBPeriph_I2S0_CLOCK) || ((APBPeriph_Clock == APBPeriph_I2S1_CLOCK)) ||
            (APBPeriph_Clock == APBPeriph_CODEC_CLOCK))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN &= ~(APBPeriph | APBPeriph_Clock);
            return;
        }
        if (APBPeriph == APBPeriph_CAP)
        {
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_1M_SRC_EN = 0;
            SYSBLKCTRL->u_234.BITS_234.BIT_CKE_CAP = 0;
            return;
        }
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            RCC_PeriClockConfig(APBPeriph_Clock, DISABLE);
            return;
        }
    }

    /* clear flag */
    APBPeriph &= (~(0x03 << 26));
    APBPeriph_Clock &= (~(0x03 << 29));
    uint8_t sleep_clk_cfg = (APBPeriph_Clock & BIT10) ? 0 : 1 ;
    APBPeriph_Clock &= ~BIT10;
    if (NewState == ENABLE)
    {
        //enable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.SOC_FUNC_EN)) + apbRegOff) |= (uint32_t)((
                                                                                        uint32_t)1 << APBPeriph);
        //enable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((
                                                                                    uint32_t)1 << APBPeriph_Clock);
        //enable peripheral clock in sleep mode
        if (sleep_clk_cfg)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((uint32_t)1 << (APBPeriph_Clock + 1));}
    }
    else
    {
        /*delay 2us or aux adc potential hang*/
        platform_delay_us(2);
        //disable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.SOC_FUNC_EN)) + apbRegOff) &= ~((uint32_t)1 << APBPeriph);
        //disable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((
                                                                                      uint32_t)1 << APBPeriph_Clock);
        //disable peripheral clock in sleep mode
        if (sleep_clk_cfg)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((uint32_t)1 << (APBPeriph_Clock + 1));}
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  ClockGate: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg CLOCK_GATE_5M
  *     @arg CLOCK_GATE_20M
  *     @arg CLOCK_GATE_10M
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ClockGateCmd(uint32_t ClockGate, FunctionalState NewState)
{
    assert_param(IS_CLOCK_GATE(ClockGate));

    if (NewState == ENABLE)
    {
        /* Open 40M clock source first */
        SYSBLKCTRL->u_20C.REG_0x20C |= BIT20;
        SYSBLKCTRL->u_20C.REG_0x20C |= BIT21;
        SYSBLKCTRL->u_20C.REG_0x20C |= BIT22;
        SYSBLKCTRL->u_20C.REG_0x20C |= BIT26;
        SYSBLKCTRL->u_20C.REG_0x20C |= ClockGate;
    }
    else
    {
        SYSBLKCTRL->u_20C.REG_0x20C &= ~ClockGate;
    }

    return;
}

/**
  * @brief  Clock source selected.
  * @param  clocklevel: Timer clock divider.
  *      this parameter can be one of the following values:
  *     @arg TIM2TO7_CLOCK_DIV_1
  *     @arg TIM2TO7_CLOCK_DIV_2
  *     @arg TIM2TO7_CLOCK_DIV_3
  *     @arg TIM2TO7_CLOCK_DIV_4
  *     @arg TIM2TO7_CLOCK_DIV_6
  *     @arg TIM2TO7_CLOCK_DIV_8
  *     @arg TIM2TO7_CLOCK_DIV_16
  * @param  clocksource: Timer clock Source.
  *      this parameter can be one of the following values:
  *     @arg TIM_CLOCK_SOURCE_SYSTEM_CLOCK
  *     @arg TIM_CLOCK_SOURCE_40MHZ
  *     @arg TIM_CLOCK_SOURCE_PLL
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_TimSourceConfig(uint16_t clocklevel, uint16_t clocksource, FunctionalState NewState)
{
    assert_param(IS_TIM_CLOCK_SOURCE(clocksource));
    assert_param(TIM2TO7_TIM_DIV(clocklevel));

    /*Open TIM Clock source*/
    if (NewState == ENABLE)
    {
        /* See if change clock source */
        if (clocksource != (CLK_SOURCE_REG_0 & (0x7 << 5)))
        {
            /*Choose TIM clock source*/
            if (clocksource == TIM_CLOCK_SOURCE_40MHZ)
            {
                CLK_SOURCE_REG_0 &= ~BIT6;
                CLK_SOURCE_REG_0 |= BIT5;
                CLK_SOURCE_REG_0 |= BIT7;
            }
            else if (clocksource == TIM_CLOCK_SOURCE_SYSTEM_CLOCK)
            {
                CLK_SOURCE_REG_0 &= ~(BIT6 | BIT5);
                CLK_SOURCE_REG_0 |= BIT7;
            }
            else if (clocksource == TIM_CLOCK_SOURCE_PLL)
            {
                CLK_SOURCE_REG_0 |= BIT6;
                CLK_SOURCE_REG_0 |= BIT7;
            }
        }

        /* if tim clock divider changes */
        if (!((clocklevel == (CLK_SOURCE_REG_0 & (0x7))) || (clocklevel == (CLK_SOURCE_REG_0 & BIT3))))
        {
            /*close the TIMER clock*/
            CLK_SOURCE_REG_0 &= ~BIT4;
            DELAY_US(2);

            /* Disable divider */
            CLK_SOURCE_REG_0 &= ~BIT3;

            /* No divider */
            if (clocklevel != TIM2TO7_CLOCK_DIV_1)
            {
                /* Set divide value */
                CLK_SOURCE_REG_0 &= ~(0x07);
                CLK_SOURCE_REG_0 |= clocklevel;
                /*enable divide*/
                CLK_SOURCE_REG_0 |= BIT3;
            }

            DELAY_US(2);

            /* enable ICG*/
            CLK_SOURCE_REG_0 |= BIT4;
        }
    }
    else
    {
        /* close divider */
        CLK_SOURCE_REG_0 &= ~BIT3;

        DELAY_US(2);

        /* close TIM clock source */
        CLK_SOURCE_REG_0 &= ~BIT4;
        CLK_SOURCE_REG_0 &= ~BIT7;
    }

    return;
}

/**
  * @brief  SPI clock divider config.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @param  ClockDiv: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg SPI_CLOCK_DIV_1
  *     @arg SPI_CLOCK_DIV_2
  *     @arg SPI_CLOCK_DIV_4
  *     @arg SPI_CLOCK_DIV_8
  * @arg SPI_CLOCK_DIV_16
  * @retval None
  */
void RCC_SPIClkDivConfig(SPI_TypeDef *SPIx, uint16_t ClockDiv)
{
    assert_param(IS_SPI_DIV(ClockDiv));

    /* Config I2C clock divider */
    if (SPIx == SPI0)
    {
        /* disable clock first */
        SYSBLKCTRL->u_234.BITS_234.BIT_SOC_ACTCK_SPI0_EN = 0;
        DELAY_US(1);

        CLK_SOURCE_REG_1 &= ~(0x03 << 19);
        CLK_SOURCE_REG_1 |= (ClockDiv << 19);

        DELAY_US(1);
        SYSBLKCTRL->u_234.BITS_234.BIT_SOC_ACTCK_SPI0_EN = 1;
    }
    return;
}

/**
  * @brief  I2C clock divider config.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  ClockDiv: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg I2C_CLOCK_DIV_1
  *     @arg I2C_CLOCK_DIV_2
  *     @arg I2C_CLOCK_DIV_4
  *     @arg I2C_CLOCK_DIV_8
  * @retval None
  */
void RCC_I2CClkDivConfig(I2C_TypeDef *I2Cx, uint16_t ClockDiv)
{
    assert_param(IS_I2C_DIV(ClockDiv));

    /* Config I2C clock divider */
    if (I2Cx == I2C0)
    {
        /* disable clock first */
        SYSBLKCTRL->u_238.BITS_238.BIT_SOC_ACTCK_I2C0_EN = 0;
        DELAY_US(1);

        CLK_SOURCE_REG_1 &= ~(0x03 << 15);
        CLK_SOURCE_REG_1 |= (ClockDiv << 15);

        DELAY_US(1);
        SYSBLKCTRL->u_238.BITS_238.BIT_SOC_ACTCK_I2C0_EN = 1;
    }

    return;
}

/**
  * @brief  UART clock divider config.
  * @param  UARTx: selected UART peripheral.
  * @param  ClockDiv: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg UART_CLOCK_DIV_1
  *     @arg UART_CLOCK_DIV_2
  *     @arg UART_CLOCK_DIV_4
  *     @arg UART_CLOCK_DIV_16
  * @retval None
  */
void RCC_UARTClkDivConfig(UART_TypeDef *UARTx, uint16_t ClockDiv)
{
    assert_param(IS_UART_DIV(ClockDiv));

    /* Config UART clock divider */
    if (UARTx == UART0)
    {
        /* disable clock first */
        SYSBLKCTRL->u_234.BITS_234.BIT_SOC_ACTCK_UART0_EN = 0;
        DELAY_US(1);

        CLK_SOURCE_REG_1 &= ~(0x03 << 9);
        CLK_SOURCE_REG_1 |= (ClockDiv << 9);

        DELAY_US(1);
        SYSBLKCTRL->u_234.BITS_234.BIT_SOC_ACTCK_UART0_EN = 1;
    }
    else if (UARTx == UART1)
    {
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_UART1_EN = 0;
        DELAY_US(1);

        CLK_SOURCE_REG_1 &= ~(0x03 << 11);
        CLK_SOURCE_REG_1 |= (ClockDiv << 11);

        DELAY_US(1);
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_UART1_EN = 1;
    }
    else if (UARTx == UART2)
    {
        /* disable clock first */
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_UART2_EN = 0;
        DELAY_US(1);

        CLK_SOURCE_REG_1 &= ~(0x03 << 13);
        CLK_SOURCE_REG_1 |= (ClockDiv << 13);

        DELAY_US(1);
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_UART2_EN = 1;
    }

    return;
}


/**
  * @brief  TIM clock divider config.
  * @param  TIMx: selected TIM peripheral.
  * @param  ClockDiv: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg TIM_CLOCK_DIV_1
  *     @arg TIM_CLOCK_DIV_2
  *     @arg TIM_CLOCK_DIV_4
  *     @arg TIM_CLOCK_DIV_8
  *     @arg TIM_CLOCK_DIV_FIX_1MHZ
  * @retval None
  */

const char timerDivBit[8] =
{
    16, 19, 22, 25,
    28, 0, 3, 6,

};
void RCC_TIMClkDivConfig(TIM_TypeDef *TIMx, uint16_t ClockDiv)
{
    assert_param(IS_UART_DIV(ClockDiv));
    uint8_t pos;
    uint32_t tempreg;
    tempreg = (uint32_t)TIMx;
    /* enable 5/10/20/40MHZ clock source */
    SYSTEM_CLK_CTRL |= (0x0F << 26);
    /* enable divider for  GTIMER0~7*/
    CLK_SOURCE_REG_2 |= (0x07 << 11);
    /* Config TIM clock divider */
    /* disable timer first */
    TIM_Cmd(TIMx, DISABLE);
    pos = (tempreg - ((uint32_t)TIM0)) / 0x14;
    if (pos < 8)
    {
        if (TIMx >= TIM0 && TIMx <= TIM4)
        {
            /*TIM0~4 CLK_SOURCE_REG_2*/
            CLK_SOURCE_REG_2 &= ~(0x07 << timerDivBit[pos]);
            CLK_SOURCE_REG_2 |= (ClockDiv << timerDivBit[pos]);
        }
        else if (TIMx >= TIM5 && TIMx <= TIM7) /*TIM5~4 CLK_SOURCE_REG_1*/
        {
            CLK_SOURCE_REG_1 &= ~(0x07 << timerDivBit[pos]);
            CLK_SOURCE_REG_1 |= (ClockDiv << timerDivBit[pos]);
        }
    }
    TIM_Cmd(TIMx, ENABLE);
}


/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_TIMERA_CLOCK
  *     @arg APBPeriph_TIMERB_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_SPI2W_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriClockConfig(uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t clkRegOff = ((APBPeriph_Clock & (0x03 << 29)) >> 29);

    /* Special register handle */
    if (NewState == ENABLE)
    {
        if ((APBPeriph_Clock == APBPeriph_I2S0_CLOCK) || ((APBPeriph_Clock == APBPeriph_I2S1_CLOCK)) ||
            (APBPeriph_Clock == APBPeriph_CODEC_CLOCK))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN |= APBPeriph_Clock;
            return;
        }

    }
    else
    {
        if ((APBPeriph_Clock == APBPeriph_I2S0_CLOCK) || ((APBPeriph_Clock == APBPeriph_I2S1_CLOCK)) ||
            (APBPeriph_Clock == APBPeriph_CODEC_CLOCK))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN &= ~(APBPeriph_Clock);
            return;
        }

    }

    APBPeriph_Clock &= (~(0x03 << 29));
    uint8_t sleep_clk_cfg = (APBPeriph_Clock & BIT10) ? 0 : 1 ;
    APBPeriph_Clock &= ~BIT10;
    if (NewState == ENABLE)
    {
        //enable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((
                                                                                    uint32_t)1 << APBPeriph_Clock);
        //enable peripheral clock in sleep mode
        if (sleep_clk_cfg)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((uint32_t)1 << (APBPeriph_Clock + 1));}
    }
    else
    {
        //disable peripheral clock
        platform_delay_us(2);
        *((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((
                                                                                      uint32_t)1 << APBPeriph_Clock);
        //disable peripheral clock in sleep mode
        if (sleep_clk_cfg)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((uint32_t)1 << (APBPeriph_Clock + 1));}
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_LCD
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_GPIO
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_CODEC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriFunctionConfig(uint32_t APBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t apbRegOff = ((APBPeriph & (0x03 << 26)) >> 26);
    if (APBPeriph == APBPeriph_UART1_HCI)
    {
        return;
    }
    /* Special register handle */
    if (NewState == ENABLE)
    {
        if ((APBPeriph == APBPeriph_I2S0) || ((APBPeriph == APBPeriph_I2S1)) ||
            (APBPeriph == APBPeriph_CODEC))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN |= APBPeriph;
            return;
        }
    }
    else
    {
        if ((APBPeriph == APBPeriph_I2S0) || ((APBPeriph == APBPeriph_I2S1)) ||
            (APBPeriph == APBPeriph_CODEC))
        {
            SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN &= ~(APBPeriph);
            return;
        }
    }

    /* clear flag */
    APBPeriph &= (~(0x03 << 26));

    if (NewState == ENABLE)
    {
        //enable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.SOC_FUNC_EN)) + apbRegOff) |= (uint32_t)((
                                                                                        uint32_t)1 << APBPeriph);
    }
    else
    {
        /*delay 2us or aux adc potential hang*/
        platform_delay_us(2);
        //disable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.SOC_FUNC_EN)) + apbRegOff) &= ~((uint32_t)1 << APBPeriph);
    }

    return;
}




