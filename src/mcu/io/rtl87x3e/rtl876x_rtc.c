/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_rtc.c
* @brief    This file provides all the RTC firmware functions.
* @details
* @author   elliot_chen
* @date     2016-12-2
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rtc.h"

/* Internal defines ------------------------------------------------------------*/
#define REG_RTC_FAST_WRITE_BASE_ADDR    (0x40000100UL)
#define REG_RTC_FAST_WDATA              (0x400001f0UL)
#define REG_RTC_FAST_ADDR               (0x400001f4UL)
#define REG_RTC_WR_STROBE               (0x400001f8UL)

/**
  * @brief  Fast write RTC register.
  * @param  offset: the offset of RTC register.
  * @param  data: data which write to register.
  * @retval None
  */
void RTC_WriteReg(uint32_t offset, uint32_t data)
{
    /* Write data */
    *((volatile uint32_t *)REG_RTC_FAST_WDATA) = data;
    /* Write RTC register address. Only offset */
    *((volatile uint32_t *)REG_RTC_FAST_ADDR) = offset - REG_RTC_FAST_WRITE_BASE_ADDR;
    *((volatile uint32_t *)REG_RTC_WR_STROBE) = 1;
}

void RTC_WriteReg_Safe(uint32_t offset, uint32_t data)
{
    /* Store register value of REG_RTC_FAST_WDATA and REG_RTC_FAST_ADDR */
    uint32_t reg_rtc_fast_wdata_ori = *((volatile uint32_t *)REG_RTC_FAST_WDATA);
    uint32_t reg_rtc_fast_addr_ori = *((volatile uint32_t *)REG_RTC_FAST_ADDR);

    /* Write data */
    *((volatile uint32_t *)REG_RTC_FAST_WDATA) = data;
    /* Write RTC register address. Only offset */
    *((volatile uint32_t *)REG_RTC_FAST_ADDR) = offset - REG_RTC_FAST_WRITE_BASE_ADDR;
    *((volatile uint32_t *)REG_RTC_WR_STROBE) = 1;

    /* Restore register value of REG_RTC_FAST_WDATA and REG_RTC_FAST_ADDR */
    *((volatile uint32_t *)REG_RTC_FAST_WDATA) = reg_rtc_fast_wdata_ori;
    *((volatile uint32_t *)REG_RTC_FAST_ADDR) = reg_rtc_fast_addr_ori;
}

/**
  * @brief  Reset RTC.
  * @param  None.
  * @retval None
  */
void RTC_DeInit(void)
{
    /* Stop RTC counter */
    RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0 & RTC_START_CLR));

    /* Clear all RTC interrupt */
    RTC_WriteReg_Safe((uint32_t)(&(RTC->INT_MASK)), RTC_ALL_INT_CLR_SET);
    __NOP();
    __NOP();
    RTC_WriteReg_Safe((uint32_t)(&(RTC->INT_MASK)), RTC_ALL_INT_CLR_RESET);

    /* Clear prescale register */
    RTC_WriteReg_Safe((uint32_t)(&(RTC->PRESCALER)), 0);
    /* Clear all comparator register */
    RTC_WriteReg_Safe((uint32_t)(&(RTC->COMP0)), 0);
    RTC_WriteReg_Safe((uint32_t)(&(RTC->COMP1)), 0);
    RTC_WriteReg_Safe((uint32_t)(&(RTC->COMP2)), 0);
    RTC_WriteReg_Safe((uint32_t)(&(RTC->COMP3)), 0);

    /* Reset prescale counter and counter */
    RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), RTC_DIV_COUNTER_RST_EN | RTC_COUNTER_RST_EN);
    __NOP();
    __NOP();
    RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), 0x0);
}



/**
  * @brief  Set RTC prescaler value.
  * @param  PrescaleValue: the prescaler value to be set.should be no more than 12 bits!
  * @retval None
  */
void RTC_SetPrescaler(uint32_t value)
{
    RTC_WriteReg_Safe((uint32_t)(&(RTC->PRESCALER)), value);
}

/**
  * @brief  Set RTC comparator value.
  * @param  index: the comparator number reference to COMPX_INDEX_t .
  * @param  COMPValue: the comparator value to be set.
  * @retval None
  */
void RTC_SetComp(uint8_t index, uint32_t value)
{
    RTC_WriteReg_Safe((uint32_t)(&(RTC->COMP0) + index), value);
}

/**
  * @brief  Start or stop RTC peripheral.
  * @param  NewState: new state of RTC peripheral.
  *   This parameter can be the following values:
  *     @arg ENABLE: Start RTC.
  *     @arg DISABLE: Stop RTC.
  * @retval None
  */
void RTC_RunCmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Start RTC */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), RTC->CR0 | RTC_START_EN);
        RTC_CpuNVICEnable(ENABLE);

    }
    else
    {
        /* Stop RTC */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), RTC->CR0 & RTC_START_CLR);
        RTC_CpuNVICEnable(DISABLE);
    }
}

/**
  * @brief  Enable interrupt signal to CPU NVIC.
  * @param NewState: new state of the specified PF_RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_CpuNVICEnable(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RTC_CONFIG_INT(RTC_INT_NVIC));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Unmask the selected RTC interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_NV_IE_EN_GLB);
    }
    else
    {
        /* Mask the selected RTC interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~RTC_NV_IE_EN_GLB));
    }
}
/**
  * @brief  RTC related interrupts  enable.
  * @param  RTC_INT: specifies the RTC interrupt  to be enabled or disabled.
  *   RTC_INT arge can reference to RTC_INT_t .
  *     @arg RTC_INT_TICK
  *     @arg RTC_INT_OVF
  *     @arg RTC_PRECMP_INT
  *     .........
  * @param NewState: new state of the specified PF_RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_INTEnableConfig(uint32_t RTC_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RTC_CONFIG_INT(RTC_INT_NVIC));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Unmask the selected RTC interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_INT);
    }
    else
    {
        /* Mask the selected RTC interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~RTC_INT));
    }
}

/**
  * @brief  mask bit remove from the bbpro2 ,for compatible with bbpro and bblite overflow keep this alive.
  * @param  RTC_INT: specifies the RTC interrupt source to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_MaskINTConfig(uint32_t RTC_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RTC_CONFIG_INT(RTC_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != ENABLE)
    {
        /* mask remove from the bbpro2 compatible with bbpro and bblite overflow */
        if (RTC_INT == RTC_INT_OVF)
        {
            RTC_CompINTConfig(RTC_INT, ENABLE);
        }
    }
    else
    {
        /* mask remove from the bbpro2 compatible with bbpro and bblite overflow */
        if (RTC_INT == RTC_INT_OVF)
        {
            RTC_CompINTConfig(RTC_INT, DISABLE);
        }
    }
}
/**
  * @brief  Enable or disable the specified RTC interrupts for comb.
  * @param  RTC_INT: specifies the RTC interrupt source
  *         RTC_INT arge can reference to RTC_INT_t .
  *     @arg RTC_CMPxGT_INT: compare GT x interrupt x =0~3
  *     @arg RTC_CMPx_NV_INT (bbpro RTC_INT_CMPx): compare  x interrupt x =0~3
  *     @arg RTC_CMPx_WK_INT : RTC_CMPx wakeup  Enable x =0~3
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_CompINTConfig(uint32_t RTC_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RTC_COMP_INT(RTC_INT_IE));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Enable the selected RTC comparator interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_INT);
        RTC_CpuNVICEnable(ENABLE);
    }
    else
    {
        /* Disable the selected RTC comparator interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~RTC_INT));
    }
}

/**
  * @brief  Enable or disable RTC tick interrupts.
  * @param  NewState: new state of RTC tick interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_TickINTConfig(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Enable RTC tick interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), RTC->CR0 | RTC_INT_TICK);
        RTC_CpuNVICEnable(ENABLE);
    }
    else
    {
        /* Disable RTC tick interrupt */
        RTC_WriteReg_Safe((uint32_t)(&(RTC->CR0)), RTC->CR0 & (~RTC_INT_TICK));
    }
}

/**
  * @brief  Checks whether the specified RTC interrupt is set or not.
   * @param  RTC_INT: specifies the RTC interrupt source .
   *   RTC_INT arge can reference to RTC_INT_t .
   *     @arg RTC_INT_TICK
   *     @arg RTC_INT_OVF
   *     @arg RTC_PRECMP_INT
   *     .........
   * @retval The new state of RTC_INT (SET or RESET).
  */
ITStatus RTC_GetINTStatus(uint32_t RTC_INT)
{
    ITStatus bit_status = RESET;

    /* Check the parameters */
    assert_param(IS_RTC_CONFIG_INT(RTC_INT));

    if ((RTC->INT_SR & (RTC_INT >> 8)) != (uint32_t)RESET)
    {
        bit_status = SET;
    }

    /* Return the RTC_INT status */
    return  bit_status;
}

/**
  * @brief  Enable or disable system wake up of RTC .
  * @param  NewState: new state of the wake up function.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_SystemWakeupConfig(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        /* Enable system wake up */
        RTC_WriteReg_Safe(RTC_SLEEP_CR, (*(volatile uint32_t *)(RTC_SLEEP_CR)) | RTC_WAKEUP_EN);
    }
    else
    {
        /* Disable system wake up */
        RTC_WriteReg_Safe(RTC_SLEEP_CR, (*(volatile uint32_t *)(RTC_SLEEP_CR)) & (~RTC_WAKEUP_EN));
    }
}

/**
  * @brief  Clear interrupt status of RTC .
   * @param  RTC_INT: specifies the RTC interrupt source .
   *   RTC_INT arge can reference to RTC_INT_t .
   *     @arg RTC_INT_TICK
   *     @arg RTC_INT_OVF
   *     @arg RTC_PRECMP_INT
   *      .........
  * @retval None
  */
void RTC_ClearINTStatus(uint32_t RTC_INT)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), (RTC_INT >> 8));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

