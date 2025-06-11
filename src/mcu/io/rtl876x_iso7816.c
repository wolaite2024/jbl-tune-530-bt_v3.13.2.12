/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_iso7816.c
* @brief    This file provides all the SmartCard firmware functions.
* @details
* @author   yuan
* @date     2020-07-13
* @version  v1.0.0
*********************************************************************************************************
*/

#include "rtl876x_rcc.h"
#include "rtl876x_iso7816.h"
#include "platform_utils.h"

uint32_t ISO7816_Clock = 5000000;

void delay_us_internal(uint32_t us)
{
    for (uint32_t i = us; i > 0; i--)
    {
        for (uint32_t j = 0; j < 2; j++);
    }
}

//#define sc_delay_us(us) {platform_delay_us(us);}
#define iso7816_delay_us(us) {delay_us_internal(us);}
/**
 * \brief   This function aims to provide delay based on cycle count Which should be count/Freq (s),
 *          given unit of Freq is Hz. Yet the actual delay function takes input parameters with unit us.
 *          We should have called simc_delay_us(count * 10^6 / Freq). However, there would be possible
 *          integer overflow of (count * 10^6). Thus we call simc_delay_us((count * 10^3) / (Freq / 10^3)) instead.
 */
void ISO7816_DelayCycle(uint32_t cycle)
{
    do
    {
        if ((cycle * 1000) < (ISO7816_Clock / 1000))
            iso7816_delay_us(1)
            else
                iso7816_delay_us((cycle * 1000) / (ISO7816_Clock / 1000))
            }
    while (0);
}

/**
 * \brief   Deinitialize the SmartCard peripheral registers to their default reset values(turn off ISO7816 clock).
 * \param   None.
 * \return  None.
 */
void ISO7816_DeInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK, DISABLE);
}

/**
  * @brief Initializes the ISO7816 peripheral according to the specified
  *   parameters in the ISO7816_InitStruct
  * @param  ISO7816: selected ISO7816 peripheral.
  * @param  ISO7816_InitStruct: pointer to a ISO7816_InitTypeDef structure that
  *   contains the configuration information for the specified ISO7816 peripheral
  * @retval None
  */
void ISO7816_Init(ISO7816_InitTypeDef *ISO7816_InitStruct)
{
    assert_param(IS_ISO7816_PERIPH(ISO7816));
//    assert_param(IS_ISO7816_WORD_LENGTH(ISO7816_InitStruct->ISO7816_WordLen));
//    assert_param(IS_ISO7816_STOPBITS(ISO7816_InitStruct->ISO7816_StopBits));
//    assert_param(IS_ISO7816_PARITY(ISO7816_InitStruct->ISO7816_Parity));
//    assert_param(IS_ISO7816_RX_FIFO_TRIGGER_LEVEL(ISO7816_InitStruct->ISO7816_RxThdLevel));
//    assert_param(IS_ISO7816_IDLE_TIME(ISO7816_InitStruct->ISO7816_IdleTime));
//    assert_param(IS_ISO7816_DMA_CFG(ISO7816_InitStruct->ISO7816_DmaEn));
//    assert_param(IS_ISO7816_AUTO_FLOW_CTRL(ISO7816_InitStruct->ISO7816_HardwareFlowControl));

    //Enable ISO7816.
    ISO7816->LP_CLKG |= ISO7816_ENABLE;

    //Disable all interrupt.
    ISO7816->IER = 0x00;
    //Read to clear interrupt status reg.
    ISO7816->ISR;

    //Set default Fd Dd.
    uint16_t prescaler = ISO7816_InitStruct->ISO7816_IntegerF / ISO7816_InitStruct->ISO7816_IntegerD;
    ISO7816->GCR = prescaler - 1;

    //Set ISO7816 ctrl, Clear Rx&Tx FIFO.
    ISO7816->CR |= ISO7816_InitStruct->ISO7816_IOMode | ISO7816_RX_FIFO_CLEAR_MSK |
                   ISO7816_TX_FIFO_CLEAR_MSK;
    ISO7816->CR &= ISO7816_RX_FIFO_CLEAR_CLR;
    ISO7816->CR &= ISO7816_TX_FIFO_CLEAR_CLR;

    ISO7816->RCR |= ISO7816_InitStruct->ISO7816_IOStateSample;

//    ISO7816->CR |= ISO7816_INT_RX_FIFO_OVERFLOW | ISO7816_INT_RX_FIFO_FULL | ISO7816_INT_RX_FIFO_ALMOST_FULL | ISO7816_INT_TX_NAK_THD;
    ISO7816->CR |= ISO7816_INT_RX_FIFO_NOT_EMPTY | ISO7816_INT_TX_FIFO_EMPTY | ISO7816_INT_GP_COUNTER;
//    if(ISO7816_InitStruct->ISO7816_ProtocolType == 0)
//    {
//        ISO7816->CR |= ISO7816_InitStruct->ISO7816_NACKCmd;
//    }
//    ISO7816->CR |= ISO7816_InitStruct->ISO7816_ProtocolType | ISO7816_InitStruct->ISO7816_ClockStopPolar | ISO7816_InitStruct->ISO7816_CodingConvention;

//    ISO7816->TCR = ISO7816_InitStruct->ISO7816_ExtraGuardTime&0xFF;

//    ISO7816->RCR = ISO7816_InitStruct->ISO7816_IOStateSample | (ISO7816_InitStruct->ISO7816_WaitingTime&0xFFFE);

//    ISO7816->THR = ISO7816_InitStruct->ISO7816_RxAFWaterLevel | ISO7816_InitStruct->ISO7816_RxRetryCountLevel | ISO7816_InitStruct->ISO7816_TxRetryCountLevel;

    //Reset pin init state is H.
    ISO7816->CR &= ISO7816_RST_CONTACT_Set;//RST = 0.

//    platform_delay_us(5 * 1000);
//    extern void SYSTEMTICK_DelayUs(uint32_t DelayUsTime);
//    SYSTEMTICK_DelayUs(5 * 1000);
    delay_us_internal(5 * 1000);
}

/**
  * @brief  Fills each ISO7816_InitStruct member with its default value.
  * @param  ISO7816_InitStruct: pointer to an ISO7816_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ISO7816_StructInit(ISO7816_InitTypeDef *ISO7816_InitStruct)
{
    //9600 default
    ISO7816_InitStruct->ISO7816_IntegerF              = 372;
    ISO7816_InitStruct->ISO7816_IntegerD              = 1;
    ISO7816_InitStruct->ISO7816_NACKCmd               = 0;
    ISO7816_InitStruct->ISO7816_ProtocolType          = 0;
    ISO7816_InitStruct->ISO7816_ClockStopPolar        = ISO7816_CLOCK_STOP_POLAR_LOW;
    ISO7816_InitStruct->ISO7816_IOMode                = ISO7816_IO_MODE_TX;
    ISO7816_InitStruct->ISO7816_CodingConvention      = ISO7816_CODING_CONVENTION_DIRECT;
    ISO7816_InitStruct->ISO7816_ExtraGuardTime        = 0xFF;//255
    ISO7816_InitStruct->ISO7816_WaitingTime           = 0x2580;//9600
    ISO7816_InitStruct->ISO7816_IOStateSample         = ISO7816_IO_STATE_SAMPLE_ENABLE;
    ISO7816_InitStruct->ISO7816_RxAFWaterLevel        = 0x0E;
    ISO7816_InitStruct->ISO7816_RxRetryCountLevel     = 0x0F;// no limit
    ISO7816_InitStruct->ISO7816_TxRetryCountLevel     = 0x03;
    return;
}

/**
 * @brief  Enables or disables the specified ISO7816 interrupts.
 * @param[in] ISO7816_INT: specifies the interrupt source to check.
 *     This parameter can be one of the following values:
 *     @arg ISO7816_INT_RESET_TIMING_VIOLATION: Reset timing violation.
 *     @arg ISO7816_INT_TX_NAK_THD             TNAKTH exceeded.
 *     @arg ISO7816_INT_RX_WAIT_TIMEOUT        Character wait timeout.
 *     @arg ISO7816_INT_GP_COUNTER             General purpose counter hit.
 *     @arg ISO7816_INT_TX_FIFO_EMPTY          TXFIFO empty.
 *     @arg ISO7816_INT_TX_FIFO_NOT_FULL       TXFIFO not full, triggered when TX FIFO changes from full to non-full.
 *     @arg ISO7816_INT_TX_FIFO_OVERFLOW       TXFIFO overflow.
 *     @arg ISO7816_INT_RX_FIFO_NOT_EMPTY      RXFIFO not empty, triggered when RX FIFO changes from empty to non-empty.
 *     @arg ISO7816_INT_RX_FIFO_ALMOST_FULL    RXFIFO almost full
 *     @arg ISO7816_INT_RX_FIFO_FULL           RXFIFO full.
 *     @arg ISO7816_INT_RX_FIFO_OVERFLOW       RXFIFO overflow.
 *     @arg ISO7816_INT_RX_FIFO_UNDERFLOW      RXFIFO underflow.
 *     @arg ISO7816_INT_TX_DONE                Transmission completed, triggered when TX FIFO is empty and the last bit has been sent out.
 * @param  NewState: new state of the specified ISO7816 interrupts.
 *     This parameter can be: ENABLE or DISABLE.
 * @return None
 */
void ISO7816_INTConfig(uint32_t ISO7816_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_INT(ISO7816_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        ISO7816->IER |= ISO7816_INT & 0x1FFF;
    }
    else
    {
        ISO7816->IER &= ~(ISO7816_INT & 0x1FFF);
    }
}

/**
  * @brief  Send data to tx FIFO.
  * @param  ISO7816: selected ISO7816 peripheral.
  * @param  inBuf: buffer to be written to Tx FIFO.
  * @param  num: number of data to be written.
  * @retval None
  */
void ISO7816_SendData(const uint8_t *inBuf, uint16_t len)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_PERIPH(ISO7816));

    ISO7816->CR |= ISO7816_IO_MODE_MSK;

    while (len--)
    {
        ISO7816->TX_FIFO = *inBuf++;
    }
}

/**
  * @brief  Receive data from rx FIFO.
  * @param  ISO7816: selected ISO7816 peripheral.
  * @param[out]  outBuf: buffer to save data read from ISO7816 FIFO.
  * @param  count: number of data to be read.
  * @return None
  */
void ISO7816_ReceiveData(uint8_t *outBuf, uint16_t len)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_PERIPH(ISO7816));

    while (len--)
    {
        *outBuf++ = (uint8_t)ISO7816->RX_FIFO;
    }
}

void ISO7816_CardColdReset(void)
{
    uint32_t value = 0;

    value = ISO7816_VCC_CONTACT_Set | ISO7816_CLK_CONTACT_Set | ISO7816_CR_ENABLE;

    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;//RST = 0.
    ISO7816->CR |= value;
    ISO7816->RCR |= BIT(16);
}

void ISO7816_CardWarmReset(void)
{
    ISO7816->CR &= ISO7816_RST_CONTACT_Reset;//RST = 0.

    // Waits at least 40,000/f before RST goes high.
    // RST should not change during ATR dialog
    ISO7816_DelayCycle(40000);

    ISO7816->CR |= ISO7816_VCC_CONTACT_Set;
    ISO7816->CR |= ISO7816_CLK_CONTACT_Set;
    ISO7816->CR |= ISO7816_RST_CONTACT_Set;

    ISO7816_DelayCycle(40000);
}
/**
 * \brief   Stop the clock output to the pATRBuf.
 * \param   NewState: New state of the physically connected pATRBuf.
 *          This parameter can be: ENABLE or DISABLE.
 * \return  None.
 */
void ISO7816_CardClockStop(void)
{
    while ((ISO7816_GetFlagStatus(ISO7816_FLAG_TX_IDLE) == RESET)
           && (ISO7816_GetFlagStatus(ISO7816_FLAG_TX_FIFO_EMPTY) == RESET)
           && (ISO7816_GetFlagStatus(ISO7816_FLAG_RX_FIFO_NOT_EMPTY) != RESET));

    ISO7816->CR &= ISO7816_CLK_CONTACT_Reset;
    ISO7816_DelayCycle(1860);//delay >=1860*clk
}

void ISO7816_CardClockRestart(void)
{
    ISO7816->CR |= ISO7816_CLK_CONTACT_Set;
    ISO7816_DelayCycle(700);//delay >=700*clk
}

/**
 * \brief   Deactive a physically connected pATRBuf.
 * \param   NewState: New state of the physically connected pATRBuf.
 *          This parameter can be: ENABLE or DISABLE.
 * \return  None.
 */
void ISO7816_CardDeactive(void)
{
    uint32_t value = ISO7816_RESET_ENABLE_MSK | ISO7816_VCC_ENABLE_MSK | ISO7816_CLOCK_ENABLE_MSK |
                     ISO7816_CONTROLLER_ENBALE_MSK;
    ISO7816->CR &= ~value;
}

/**
 * @brief   Check whether the specified ISO7816 interrupt source is set or not.
            Software reads to clear all interrupts.
 * @param[in] ISO7816_INT: specifies the interrupt source to check.
 *     This parameter can be one of the following values:
 *     @arg ISO7816_FLAG_TX_FIFO_EMPTY                 ((uint16_t)(1 << 0))
 *     @arg ISO7816_FLAG_TX_FIFO_NOT_FULL              ((uint16_t)(1 << 1))
 *     @arg ISO7816_FLAG_TX_IDLE                       ((uint16_t)(1 << 2))
 *     @arg ISO7816_FLAG_RX_FIFO_NOT_EMPTY             ((uint16_t)(1 << 4))
 *     @arg ISO7816_FLAG_RX_FIFO_ALMOST_FULL           ((uint16_t)(1 << 5))
 *     @arg ISO7816_FLAG_RX_FIFO_FULL                  ((uint16_t)(1 << 6))
 *     @arg ISO7816_FLAG_RX_AD_TIMING_VIOLATION        ((uint16_t)(1 << 8))
 *     @arg ISO7816_FLAG_RX_CF_TIMING_VIOLATION        ((uint16_t)(1 << 9))
 *     @arg ISO7816_FLAG_TX_NAK_CNT_THD                ((uint16_t)(1 << 10))
 *     @arg ISO7816_FLAG_RX_NAK_CNT_THD                ((uint16_t)(1 << 11))
 *     @arg ISO7816_FLAG_TX_FIFO_OVERFLOW              ((uint16_t)(1 << 12))
 *     @arg ISO7816_FLAG_RX_FIFO_OVERFLOW              ((uint16_t)(1 << 13))
 *     @arg ISO7816_FLAG_RX_FIFO_UNDERFLOW             ((uint16_t)(1 << 14))
 *     @arg ISO7816_FLAG_RX_PARITY_ERR                 ((uint16_t)(1 << 15))
 * @retval The new state of ISO7816_FLAG (SET or RESET).
 */
FlagStatus ISO7816_GetFlagStatus(uint32_t ISO7816_FLAG)
{
    /* Check the parameters */
    assert_param(IS_ISO7816_FLAG(ISO7816_FLAG));

    FlagStatus flagstatus = RESET;
    uint32_t flag_status_bits = 0;

    if (ISO7816_FLAG & 0x07)
    {
        flag_status_bits = ISO7816->TSR & 0x07;
    }
    else if (ISO7816_FLAG & 0x70)
    {
        flag_status_bits = ISO7816->RSR & 0x70;
    }
    else if (ISO7816_FLAG & 0xFF00)
    {
        uint32_t temp = ISO7816->ESR & 0xFF;
        flag_status_bits = (temp << 8) & 0xFF00;
    }

    if (flag_status_bits & ISO7816_FLAG)
    {
        flagstatus = SET;
    }

    return flagstatus;
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor *****END OF FILE****/
