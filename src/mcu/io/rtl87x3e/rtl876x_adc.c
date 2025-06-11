/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_adc.c
* @brief    This file provides all the ADC firmware functions.
* @details
* @author   renee
* @date     2016-11-05
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "rtl876x_aon_reg.h"
#include "indirect_access.h"


void ADC_SISet(void)
{
    AON_FAST_REG0X_AUXADC_TYPE adc_si = {.d16 = btaon_fast_read_safe(AON_FAST_REG0X_AUXADC)};
    adc_si.AUXADC_SEL_VREF = 2;
    adc_si.AUXADC_VCM_SEL = 1;
    adc_si.AUXADC_EN_LN = 1;
    adc_si.AUXADC_EN_LNA = 1;
    btaon_fast_write_safe(AON_FAST_REG0X_AUXADC, adc_si.d16);
}
/**
  * @brief Initializes the ADC peripheral according to the specified
  *   parameters in the ADC_InitStruct
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_InitStruct: pointer to a ADCInitTypeDef structure that
  *   contains the configuration information for the specified ADC peripheral
  * @retval None
  */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct)
{
    uint8_t index = 0;
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_LATCH_MODE(ADC_InitStruct->dataLatchEdge));
    assert_param(IS_ADC_CLOCK(ADC_InitStruct->adcClock));
    assert_param(IS_ADC_POWER_MODE(ADC_InitStruct->pwrmode));
    assert_param(IS_ADC_RG2X_0_DELAY_TIME(ADC_InitStruct->adcRG2X0Dly));
    assert_param(IS_ADC_RG0X_1_DELAY_TIME(ADC_InitStruct->adcRG0X1Dly));
    assert_param(IS_ADC_RG0X_0_DELAY_TIME(ADC_InitStruct->adcRG0X0Dly));
    assert_param(IS_ADC_BURST_SIZE_CONFIG(ADC_InitStruct->adcBurstSize));
    assert_param(IS_ADC_FIFO_THRESHOLD(ADC_InitStruct->adcFifoThd));

    ADC_SISet();
    /*Disable all interrupt.*/
    ADCx->INTCR &= (~0x1f);

    /* Set power mode first */
    ADCx->PWRDLY = (ADC_InitStruct->pwrmode | (ADC_InitStruct->adc_fifo_stop_write) \
                    | (ADC_InitStruct->datalatchDly << 6)) | ADC_InitStruct->adcPowerAlwaysOnCmd;//| BIT15
    if (ADC_InitStruct->pwrmode == ADC_POWER_AUTO)
    {
        ADCx->PWRDLY |= (ADC_InitStruct->adcRG2X0Dly \
                         | ADC_InitStruct->adcRG0X1Dly \
                         | ADC_InitStruct->adcRG0X0Dly);
    }
    /* Disable schedule table */
    ADCx->SCHCR &= (~0xffff);

    /* Set schedule table */
    for (index = 0; index < 8; index++)
    {
        *(__IO uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + index) = (ADC_InitStruct->schIndex[index * 2] |
                                                                    (ADC_InitStruct->schIndex[index * 2 + 1] << 16));
    }
    ADCx->SCHCR = ADC_InitStruct->bitmap;

    /* Set ADC mode */
    ADCx->CR = ((ADC_InitStruct->dataLatchEdge)
                | (ADC_InitStruct->adcFifoThd << 20)
                | (ADC_InitStruct->adcBurstSize << 14)
                | (ADC_InitStruct->adcFifoOverWritEn)
                | (ADC_InitStruct->dataWriteToFifo << 27));
    /* adc data and clock config */
    if ((ADC_InitStruct->adcClock & 0x3FFF) < 5)
    {
        /* adc sample period should larger than 4(100ns)*/
        ADC_InitStruct->adcClock = 5;
    }
    ADCx->DATCLK = (ADC_InitStruct->dataMinusEn
                    | (ADC_InitStruct->dataAligned)
                    | (ADC_InitStruct->timerTriggerEn << 29)
                    | ((ADC_InitStruct->dataMinusOffset) << 16));
    ADCx->ANACTL |= (0x03 << 10);
    ADCx->REG_5C_CLK = ((ADC_InitStruct->adcClock & 0x3fff)
                        | (ADC_InitStruct->adcConvertTimePeriod << 30));
    /*clear adc fifo*/
    ADCx->CR |= BIT26;
    /*clear all interrupt*/
    ADCx->INTCR |= (0x1f << 8);
    return;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
    ADC_InitStruct->adcConvertTimePeriod = ADC_CONVERT_TIME_500NS;
    ADC_InitStruct->adcClock                = ADC_CLK_78_125K;      /* ( n+ 1) cycle of 10M Hz */
    ADC_InitStruct->dataLatchEdge     = ADC_Latch_Data_Positive;
    ADC_InitStruct->adcFifoOverWritEn = ADC_FIFO_OVER_WRITE_ENABLE;
    ADC_InitStruct->timerTriggerEn    = DISABLE;
    ADC_InitStruct->dataWriteToFifo   = DISABLE;
    ADC_InitStruct->dataAligned       = ADC_DATA_LSB;
    ADC_InitStruct->dataMinusEn       = ADC_DATA_MINUS_DIS;
    ADC_InitStruct->dataMinusOffset   = 0;
    for (uint8_t i = 0; i < 16; i++)
    {
        ADC_InitStruct->schIndex[i]         = 0;
    }
    ADC_InitStruct->bitmap              = 0x0;
    ADC_InitStruct->adcFifoThd        = 0x06;
    ADC_InitStruct->adcBurstSize      = 0x1;

    ADC_InitStruct->adc_fifo_stop_write = 0;

    /*Reserved parameter, please do not change values*/
    ADC_InitStruct->pwrmode           = ADC_POWER_AUTO;
    ADC_InitStruct->datalatchDly      = 0x1;
    ADC_InitStruct->adcRG2X0Dly       = ADC_RG2X_0_DELAY_40_US;
    ADC_InitStruct->adcRG0X1Dly       = ADC_RG0X_1_DELAY_40_US;
    ADC_InitStruct->adcRG0X0Dly       = ADC_RG0X_0_DELAY_30_US;
    ADC_InitStruct->adcPowerAlwaysOnCmd = ADC_POWER_ALWAYS_ON_DISABLE;

    return;
}

/**
  * @brief  Deinitializes the ADC peripheral registers to their default reset values(turn off ADC clock).
  * @param  ADCx: selected ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef *ADCx)
{
    assert_param(IS_ADC_PERIPH(ADCx));

    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, (FunctionalState)DISABLE);

    return;
}

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_INT_FIFO_RD_REQ :FIFO read request
  *     @arg ADC_INT_FIFO_RD_ERR :FIFO read error
  *     @arg ADC_INT_FIFO_TH :ADC FIFO size>thd
  *     @arg ADC_INT_FIFO_FULL :ADC FIFO overflow
  *     @arg ADC_INT_ONE_SHOT_DONE :ADC one shot mode done
  * @param  NewState: new state of the specified ADC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_INTConfig(ADC_TypeDef *ADCx, uint32_t ADC_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_ADC_IT(ADC_IT));

    if (newState != DISABLE)
    {
        /* Enable the selected ADC interrupts */
        ADCx->INTCR |= ADC_IT;
    }
    else
    {
        /* Disable the selected ADC interrupts */
        ADCx->INTCR &= (uint32_t)~ADC_IT;
    }

}

/**
  * @brief  read ADC data according to specific channel.
  * @param  ADCx: selected ADC peripheral.
  * @param  ScheduleIndex: can be 0 to 15
  * @retval The 12-bit converted ADC data.
  */
uint16_t ADC_Read(ADC_TypeDef *ADCx, uint8_t ScheduleIndex)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(ScheduleIndex < 16);

    if (ScheduleIndex & BIT(0))
    {
        return ((*(uint32_t *)((uint32_t *)(&ADCx->SCHD0) + (ScheduleIndex >> 1))) >> 16);
    }
    else
    {
        return (*(uint32_t *)((uint32_t *)(&ADCx->SCHD0) + (ScheduleIndex >> 1)));
    }
}

/**
  * @brief  Enables or disables the ADC peripheral.
  * @param  ADCx: selected ADC peripheral.
  * @param  adcMode: adc mode select.
        This parameter can be one of the following values:
  *     @arg ADC_One_Shot_Mode: one shot mode.
  *     @arg ADC_Auto_Sensor_Mode: compare mode.
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef *ADCx, uint8_t adcMode, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_ADC_MODE(adcMode));

    if (NewState == ENABLE)
    {
        /* In case manual mode */
        if (ADCx->PWRDLY & ADC_POWER_MANNUAL)
        {
            ADCx->PWRDLY  |= 0x3C00;
            ADC_delayUS(80);
            ADCx->PWRDLY  |= (BIT14 | BIT15);
            ADC_delayUS(320);
            ADCx->PWRDLY  |= (BIT16 | BIT17);
            ADC_delayUS(240);
            ADCx->PWRDLY  |= BIT18;
        }

        /* Reset ADC mode first */
        ADCx->CR &= ~0x03;
        /* Enable ADC */
        ADCx->CR |= adcMode;

    }
    else
    {
        ADCx->CR &= ~0x03;
    }
    return;
}

/**
  * @brief  Config ADC schedule table.
  * @param  ADCx: selected ADC peripheral.
  * @param  Index: Schedule table index.
  * @param  adcMode: ADC mode.
  *      this parameter can be one of the following values:
  *     @arg EXT_SINGLE_ENDED(index)
  *     @arg EXT_DIFFERENTIAL(index)
  *     @arg VREF_AT_CHANNEL7(index)
  *     @arg INTERNAL_VBAT_MODE
  *     @arg INTERNAL_VADPIN_MODE
  *     @arg INTERNAL_VDDCORE_MODE
  *     @arg INTERNAL_VCORE_MODE
  *     @arg INTERNAL_ICHARGE_MODE
  *     @arg INTERNAL_IDISCHG_MODE
  * @return none.
  */
void ADC_SchTableConfig(ADC_TypeDef *ADCx, uint16_t Index,
                        uint8_t adcMode)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_SCHEDULE_INDEX_CONFIG(adcMode));

    if (Index & BIT0)
    {
        *(uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + (Index >> 1)) |= (adcMode << 16);
    }
    else
    {
        *(uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + (Index >> 1)) |= adcMode;
    }

    return;
}

/**
  * @brief  Data from ADC FIFO.
  * @param  ADCx: selected ADC peripheral.
  * @param[out]  outBuf: buffer to save data read from ADC FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void ADC_GetFifoData(ADC_TypeDef *ADCx, uint16_t *outBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));

    while (count--)
    {
        *outBuf++ = (uint16_t)ADCx->FIFO;
    }

    return;
}

/**
  * @brief  Config ADC bypass resistor.Attention!!!Channels using bypass mode cannot over 0.9V!!!!
  * @param  channelNum: external channel number, can be 0~7.
  * @param  NewState: ENABLE or DISABLE.
  * @retval None
  */
void ADC_HighBypassCmd(uint8_t channelNum, FunctionalState NewState)
{
    uint16_t value16;
    if (NewState != DISABLE)
    {
        value16 = btaon_fast_read_safe(AON_FAST_REG1X_AUXADC);
        value16 |= BIT(channelNum);
        btaon_fast_write_safe(AON_FAST_REG1X_AUXADC, value16);
    }
    else
    {
        value16 = btaon_fast_read_safe(AON_FAST_REG1X_AUXADC);
        value16 &= ~BIT(channelNum);
        btaon_fast_write_safe(AON_FAST_REG1X_AUXADC, value16);
    }
}

void ADC_HwEvgEn(ADC_TypeDef *ADCx, FunctionalState Newstate)
{

    if (Newstate)
    {
        ADCx->PWRDLY |= ADC_DATA_AVG_EN_BIT;
    }
    else
    {
        ADCx->PWRDLY &= ~ADC_DATA_AVG_EN_BIT;
    }
}

void ADC_HwEvgSel(ADC_TypeDef *ADCx, T_ADC_DTAT_AVG_SEL data_sel_by)
{
    ADCx->PWRDLY &= ~ADC_DATA_AVG_SEL_BIT;
    ADCx->PWRDLY |= ((uint32_t)data_sel_by << ADC_DATA_AVG_SEL_Pos);
}

