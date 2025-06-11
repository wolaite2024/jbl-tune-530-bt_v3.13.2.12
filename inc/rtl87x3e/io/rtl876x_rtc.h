/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rtc.h
* @brief
* @details
* @author    justin
* @date      2018-12-7
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_RTC_H_
#define _RTL876X_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"


/* define ------------------------------------------------------------------*/
typedef enum _COMPX_INDEX
{
    COMP0_INDEX = 0,            /*0x110*/
    COMP1_INDEX,                /*0x114*/
    COMP2_INDEX,                /*0x118*/
    COMP3_INDEX,                /*0x11C*/
    COMP0GT_INDEX,               /*0x120*/
    COMP1GT_INDEX,               /*0x124*/
    COMP2GT_INDEX,               /*0x128*/
    COMP3GT_INDEX,               /*0x12C*/
    BACKUP0,                     /*0x130*/
    BACKUP1,
    PRESCALER_COMP_INDEX,        /*0x138*/
} COMPX_INDEX_t;

typedef enum _INT_STATUS_POS
{
    RTC_TICK_CLR = 0,
    RTC_CNT_OV_CLR,
    RTC_PRECMP_CLR,
    RTC_PRECMP_CMP3_CLR,
    RTC_CMP0GT_CLR,           /*4*/
    RTC_CMP1GT_CLR,
    RTC_CMP2GT_CLR,
    RTC_CMP3GT_CLR,
    RTC_CMP0_NV_CLR,        /*8*/
    RTC_CMP1_NV_CLR,
    RTC_CMP2_NV_CLR,
    RTC_CMP3_NV_CLR,
    RTC_CMP0_WK_CLR,         /*12*/
    RTC_CMP1_WK_CLR,
    RTC_CMP2_WK_CLR,
    RTC_CMP3_WK_CLR,
    RTC_PF_CMP0_GT_CLR,      /*16*/
    RTC_PF_CMP1_GT_CLR,
    RTC_PF_CMP0_NV_CLR,
    RTC_PF_CMP1_NV_CLR,
    RTC_PF_CMP0_WK_CLR,     /*20*/
    RTC_PF_CMP1_WK_CLR,

} INT_STATUS_POS;


typedef enum _RTC_INT
{
    RTC_INT_TICK        = BIT(8),
    RTC_INT_OVF         = BIT(9),
    RTC_PRECMP_INT      = BIT(10),
    RTC_PRECMP_CMP3_INT = BIT(11),

    RTC_CMP0GT_INT      = BIT(12),
    RTC_CMP1GT_INT      = BIT(13),
    RTC_CMP2GT_INT      = BIT(14),
    RTC_CMP3GT_INT      = BIT(15),
    RTC_CMP0_NV_INT  = BIT(16),
    RTC_INT_CMP0     = BIT(16),        /*compatible  bbpro*/

    RTC_CMP1_NV_INT  = BIT(17),
    RTC_INT_CMP1     = BIT(17),        /*compatible  bbpro*/

    RTC_CMP2_NV_INT  = BIT(18),
    RTC_INT_CMP2     = BIT(18),       /*compatible  bbpro*/

    RTC_CMP3_NV_INT  = BIT(19),
    RTC_INT_CMP3     = BIT(19),       /*compatible  bbpro*/

    RTC_CMP0_WK_INT     = BIT(20),
    RTC_CMP1_WK_INT     = BIT(21),
    RTC_CMP2_WK_INT     = BIT(22),
    RTC_CMP3_WK_INT     = BIT(23),

} RTC_INT_t;
/* Register: CR0 bit29~31 */
#define RTC_START_EN          BIT(0)
#define RTC_START_CLR         (~(RTC_START_EN))
/*compatible  bbpro*/
#define RTC_START_Msk                     RTC_START_EN

#define RTC_COUNTER_RST_EN  BIT(1)
/*compatible  bbpro*/
#define RTC_COUNTER_RST_Msk          RTC_COUNTER_RST_EN

#define RTC_DIV_COUNTER_RST_EN    BIT(2)
/*compatible  bbpro*/
#define RTC_DIV_COUNTER_RST_Msk          RTC_DIV_COUNTER_RST_EN

#define RTC_WAKEUP_EN      BIT(29)
#define RTC_NV_IE_EN_GLB   BIT(30)
#define RTC_REST_EN        BIT(31)


/* Peripheral: RTC */
/* Description: Real time counter register defines */

/* Register: sleep control global register0 */
#define RTC_SLEEP_CR                    (0x40000100UL)

/* Clear all interrupt */
#define RTC_ALL_INT_CLR_SET             0x3fffff

#define RTC_ALL_INT_CLR_RESET           0xffc00000

/** @addtogroup 87x3e_RTC RTC
  * @brief RTC driver module
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup 87x3e_RTC_Exported_Constants RTC Exported Constants
  * @{
  */

/** @defgroup 87x3e_RTC_interrupts_definition RTC Interrupts Definition
  * @{
  */

#define IS_RTC_CONFIG_INT(INT) (((INT) == RTC_INT_TICK) || \
                                ((INT) == RTC_CNT_OV_INT_EN) || \
                                ((INT) == RTC_PRECMP_INT_EN) || \
                                ((INT) == RTC_PRECMPCP3_INT_EN) || \
                                ((INT) == RTC_CMP0_NV_INT_EN) || \
                                ((INT) == RTC_CMP1_NV_INT_EN) || \
                                ((INT) == RTC_CMP2_NV_INT_EN) || \
                                ((INT) == RTC_CMP3_NV_INT_EN) || \
                                ((INT) == RTC_CMP0GT_INT_EN) || \
                                ((INT) == RTC_CMP1GT_INT_EN) || \
                                ((INT) == RTC_CMP2GT_INT_EN) || \
                                ((INT) == RTC_CMP3GT_INT_EN))

#define IS_RTC_COMP_INT(INT) ( ((INT) == RTC_CMP0_NV_INT_EN) || \
                               ((INT) == RTC_CMP1_NV_INT_EN) || \
                               ((INT) == RTC_CMP2_NV_INT_EN) || \
                               ((INT) == RTC_CMP3_NV_INT_EN))

/** End of group 87x3e_RTC_interrupts_definition
  * @}
  */

/** @defgroup 87x3e_RTC_sleep_mode_clock_definition RTC sleep mode clock Definition
  * @{
  */
#define RTC_EXTERNAL_CLK            EXTERNAL_CLK
#define RTC_INTERNAL_CLK            INTERNAL_CLK
#define IS_RTC_SLEEP_CLK(CLK) ( ((CLK) == EXTERNAL_CLK) || \
                                ((CLK) == RTC_INT_CMP3))

/** End of group 87x3e_RTC_sleep_mode_clock_definition
  * @}
  */

/** End of group 87x3e_RTC_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup 87x3e_RTC_Exported_Functions RTC Exported Functions
  * @{
  */

/**
  * @brief  Reset RTC.
  * @param  None.
  * @retval None
  */
void RTC_DeInit(void);

/**
  * @brief  Set RTC prescaler value.
  * @param  value: the prescaler value to be set.should be no more than 12 bits!
  * @retval None
  */
void RTC_SetPrescaler(uint32_t value);

/**
  * @brief  Set RTC comparator value.
  * @param  index: the comparator number reference to COMPX_INDEX_t .
  * @param  COMPValue: the comparator value to be set.
  * @retval None
  */
void RTC_SetComp(uint8_t index, uint32_t value);

/**
  * @brief  Start or stop RTC peripheral.
  * @param  NewState: new state of RTC peripheral.
  *   This parameter can be the following values:
  *     @arg ENABLE: Start RTC.
  *     @arg DISABLE: Stop RTC.
  * @retval None
  */
void RTC_RunCmd(FunctionalState NewState);


/**
  * @brief  Enable interrupt signal to CPU NVIC.
  * @param NewState: new state of the specified PF_RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_CpuNVICEnable(FunctionalState NewState);
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
void RTC_INTEnableConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
  * @brief  mask bit remove from the bbpro2 ,for compatible with bbpro and bblite overflow keep this alive.
  * @param  RTC_INT: specifies the RTC interrupt source to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define RTC_EN_INTConfig     RTC_MaskINTConfig
void RTC_MaskINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
  * @brief  Enable or disable the specified RTC interrupts for comp.
  * @param  RTC_INT: specifies the RTC interrupt source
  *         RTC_INT arge can reference to RTC_INT_t .
  *     @arg RTC_CMPxGT_INT: compare GT x interrupt x =0~3
  *     @arg RTC_CMPx_NV_INT (bbpro RTC_INT_CMPx): compare  x interrupt x =0~3
  *     @arg RTC_CMPx_WK_INT : RTC_CMPx wakeup  Enable x =0~3
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_CompINTConfig(uint32_t RTC_INT, FunctionalState NewState);

/**
  * @brief  Enable or disable RTC tick interrupts.
  * @param  NewState: new state of RTC tick interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_TickINTConfig(FunctionalState NewState);

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
ITStatus RTC_GetINTStatus(uint32_t RTC_INT);

/**
  * @brief  Enable or disable system wake up of RTC .
  * @param  NewState: new state of the wake up function.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_SystemWakeupConfig(FunctionalState NewState);

/**
  * @brief  Fast write RTC register.
  * @param  offset: the offset of RTC register.
  * @param  data: data which write to register.
  * @retval None
  */
void RTC_WriteReg(uint32_t offset, uint32_t data);
void RTC_WriteReg_Safe(uint32_t offset, uint32_t data);


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
void RTC_ClearINTStatus(uint32_t RTC_INT);
/**
  * @brief  Reset prescaler counter value of RTC.
  * @param  None
  * @retval the counter value.
  */
static __forceinline void RTC_ResetPrescalerCounter(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_DIV_COUNTER_RST_EN);
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~(RTC_DIV_COUNTER_RST_EN)));
}

/**
  * @brief  Get counter value of RTC.
  * @param  None
  * @retval the counter value.
  */
static __forceinline uint32_t RTC_GetCounter(void)
{
    return RTC->CNT;
}

/**
  * @brief  Reset counter value of RTC.
  * @param  None
  * @retval the counter value.
  */
static __forceinline void RTC_ResetCounter(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) | RTC_COUNTER_RST_Msk);
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->CR0)), (RTC->CR0) & (~(RTC_COUNTER_RST_EN)));
}

/**
  * @brief  Get RTC comparator value.
  * @param  index: value use typ of COMPX_INDEX_t.
  * @retval the comparator value.
  */
static __forceinline uint32_t RTC_GetComp(uint8_t index)
{
    return *((volatile uint32_t *)(&(RTC->COMP0) + index));
}

/**
  * @brief  Clear interrupt of the select comparator of RTC.
  * @param  index: the comparator value.
  * @retval None
  */
static __forceinline void RTC_ClearCompINT(uint8_t index)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CMP0_NV_CLR + index));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);

}
/**
  * @brief  Clear wakeup interrupt of the select comparator of RTC.
  * @param  index: the comparator value.
  * @retval None
  */
static __forceinline void RTC_ClearCompWkINT(uint8_t index)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CMP0_WK_CLR + index));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
}

/**
  * @brief  Clear overflow interrupt of RTC.
  * @param  None
  * @retval None.
  */
static __forceinline void RTC_ClearOverFlowINT(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), BIT(RTC_CNT_OV_CLR));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
}

/**
  * @brief  Clear tick interrupt of RTC.
  * @param  None
  * @retval None.
  */
static __forceinline void RTC_ClearTickINT(void)
{
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)),  BIT(RTC_TICK_CLR));
    __NOP();
    __NOP();
    RTC_WriteReg((uint32_t)(&(RTC->INT_MASK)), 0);
}

/**
  * @brief  Select sleep clock source .
  * @param  clock: sleep clock source.
  *   This parameter can be the following values:
  *     @arg RTC_EXTERNAL_CLK: from BTAON (xtal or ring osc)
  *     @arg RTC_INTERNAL_CLK: from 32k sdm(internal 32K after calibration)
  * @retval None
  */
static __forceinline void RTC_SleepModeClkConfig(uint32_t clock)
{
    /* Check the parameters */
    assert_param(IS_RTC_SLEEP_CLK(clock));

    // RTC_WriteReg(RTC_SLEEP_CR, *(volatile uint32_t *)(RTC_SLEEP_CR) & clock);
}

/** @} */ /* End of group 87x3e_RTC_Exported_Functions */
/** @} */ /* End of group 87x3e_RTC */

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_RTC_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

