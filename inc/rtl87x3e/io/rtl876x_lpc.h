/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_lpc.h
* @brief
* @details
* @author    elliot chen
* @date      2016-11-29
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __RTL876X_LPC_H
#define __RTL876X_LPC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_rtc.h"

/* Defines -------------------------------------------------------------------*/

/* Peripheral: LPC */
/* Description: Low Power Comparator register defines */

/* Register: LPC_CR0 */
/* Description: Control Register 0 */

/* LPC_CR0[15] :LPC_Threshold. Specified the threshold value of comparator voltage */
#define LPC_THRESHOLD_Pos (15UL)
#define LPC_THRESHOLD_Msk (0xfUL << LPC_THRESHOLD_Pos)

/* LPC_CR0[12] :LPC_Channel. Specified the input pin */
#define LPC_CH_NUM_Pos (12UL)
#define LPC_CH_NUM_Msk (0x7UL << LPC_CH_NUM_Pos)

/* LPC_CR0[11] :LPC_Edge. Specified the cmp output edge */
#define LPC_POSEDGE_Pos (11UL)
#define LPC_POSEDGE_Msk (0x1UL << LPC_POSEDGE_Pos)

/* LPC_CR0[9] :LPC_SRC_INT_EN. 1: Enable lpcomp out sync signal to CPU interrupt  */
#define LPC_SRC_INT_EN_Pos (9UL)
#define LPC_SRC_INT_EN_Msk (0x1UL << LPC_SRC_INT_EN_Pos)

/* LPC_CR0[8] :LPC_EVENT_EN. 1: Enable or disable LPCOMP event */
#define LPC_EVENT_EN_Pos (8UL)
#define LPC_EVENT_EN_Msk (0x1UL << LPC_EVENT_EN_Pos)

/* LPC_CR0[3] : LPC_POWER. 1: Enable power of low power cmp */
#define LPC_POWER_EN_Pos (10UL)
#define LPC_POWER_EN_Msk (0x1UL << LPC_POWER_EN_Pos)

/* LPC_CR0[2] :LPC_FLAG_CLEAR. 1: Clear Event Status of LPCOMP */
#define LPC_FLAG_CLEAR_Pos (2UL)
#define LPC_FLAG_CLEAR_Msk (0x1UL << LPC_FLAG_CLEAR_Pos)

/* LPC_CR0[1] :LPC_COUNTER_START. 1: Start LPCOMP counter. */
#define LPC_COUNTER_START_Pos (1UL)
#define LPC_COUNTER_START_Msk (0x1UL << LPC_COUNTER_START_Pos)

/* LPC_CR0[0] :LPC_COUNTER_RESET. 1: Reset LPCOMP Counter */
#define LPC_COUNTER_RESET_Pos (0UL)
#define LPC_COUNTER_RESET_Msk (0x1UL << LPC_COUNTER_RESET_Pos)

/* Register: LPC_SR */
/* Description: Status Register */

/* LPC_SR[0] :LPC_FLAG. Event Status of LPCOMP */
#define LPC_FLAG_Pos (0UL) /*!< Position of */
#define LPC_TFLAG_Msk (0xfUL << LPC_FLAG_Pos) /*!< Bit mask of  */


/** @addtogroup 87x3e_LPC LPC
  * @brief Low Power Comparator driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup 87x3e_LPC_Exported_Types LPC Exported Types
  * @{
  */

/**
  * @brief  LPC Init structure definition
  */
typedef struct
{
    uint16_t LPC_Channel;          /*!< Specifies the input pin.
                                                    This parameter can be a value of ADC_0 to ADC_7 */

    uint32_t LPC_Edge;             /*!< Specifies the comparator output edge */

    uint32_t LPC_Threshold;        /*!< Specifies the threshold value of comparator voltage. */

} LPC_InitTypeDef;

/** End of group 87x3e_LPC_Exported_Types
      * @}
      */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup 87x3e_LPC_Exported_constants LPC Exported Constants
  * @{
  */

/** @defgroup 87x3e_LPC_Threshold LPC threshold
  * @{
  */
#define LPC_200_mV                 ((uint32_t)(0x0000 << LPC_THRESHOLD_Pos))
#define LPC_400_mV                 ((uint32_t)(0x0001 << LPC_THRESHOLD_Pos))
#define LPC_600_mV                 ((uint32_t)(0x0002 << LPC_THRESHOLD_Pos))
#define LPC_800_mV                 ((uint32_t)(0x0003 << LPC_THRESHOLD_Pos))
#define LPC_1000_mV                ((uint32_t)(0x0004 << LPC_THRESHOLD_Pos))
#define LPC_1200_mV                ((uint32_t)(0x0005 << LPC_THRESHOLD_Pos))
#define LPC_1400_mV                ((uint32_t)(0x0006 << LPC_THRESHOLD_Pos))
#define LPC_1600_mV                ((uint32_t)(0x0007 << LPC_THRESHOLD_Pos))
#define LPC_1800_mV                ((uint32_t)(0x0008 << LPC_THRESHOLD_Pos))
#define LPC_2000_mV                ((uint32_t)(0x0009 << LPC_THRESHOLD_Pos))
#define LPC_2200_mV                ((uint32_t)(0x000a << LPC_THRESHOLD_Pos))
#define LPC_2400_mV                ((uint32_t)(0x000b << LPC_THRESHOLD_Pos))
#define LPC_2600_mV                ((uint32_t)(0x000c << LPC_THRESHOLD_Pos))
#define LPC_2800_mV                ((uint32_t)(0x000d << LPC_THRESHOLD_Pos))
#define LPC_3000_mV                ((uint32_t)(0x000e << LPC_THRESHOLD_Pos))
#define LPC_3200_mV                ((uint32_t)(0x000f << LPC_THRESHOLD_Pos))

#define IS_LPC_THRESHOLD(THRESHOLD) (((THRESHOLD) == LPC_200_mV) || \
                                     ((THRESHOLD) == LPC_400_mV) || \
                                     ((THRESHOLD) == LPC_600_mV) || \
                                     ((THRESHOLD) == LPC_800_mV) || \
                                     ((THRESHOLD) == LPC_1000_mV) || \
                                     ((THRESHOLD) == LPC_1200_mV) || \
                                     ((THRESHOLD) == LPC_1400_mV) || \
                                     ((THRESHOLD) == LPC_1600_mV) || \
                                     ((THRESHOLD) == LPC_1800_mV) || \
                                     ((THRESHOLD) == LPC_2000_mV) || \
                                     ((THRESHOLD) == LPC_2200_mV) || \
                                     ((THRESHOLD) == LPC_2400_mV) || \
                                     ((THRESHOLD) == LPC_2800_mV) || \
                                     ((THRESHOLD) == LPC_3000_mV) || \
                                     ((THRESHOLD) == LPC_3200_mV))

/** End of group 87x3e_LPC_Threshold
      * @}
      */

/** @defgroup 87x3e_LPC_Channel LPC channel
  * @{
  */
#define IS_LPC_CHANNEL(CHANNEL) ((CHANNEL) <= 0x07)

/** End of group 87x3e_LPC_Channel
      * @}
      */

/** @defgroup 87x3e_LPC_Edge LPC output edge
  * @{
  */
#define LPC_Vin_Below_Vth               ((uint32_t)(0x0000UL << LPC_POSEDGE_Pos))
#define LPC_Vin_Over_Vth                ((uint32_t)(0x0001UL << LPC_POSEDGE_Pos))

#define IS_LPC_EDGE(EDGE) (((EDGE) == LPC_Vin_Below_Vth) || \
                           ((EDGE) == LPC_Vin_Over_Vth))

/** End of group 87x3e_LPC_Edge
      * @}
      */

/** @defgroup 87x3e_LPC_interrupts_definition LPC Interrupts Definition
      * @{
      */
#define LPC_INT_VOLTAGE_COMP                (LPC_SRC_INT_EN_Msk)
#define LPC_INT_COUNT_COMP                  (LPC_EVENT_EN_Msk)

#define IS_LPC_CONFIG_INT(INT) (((INT) == LPC_INT_VOLTAGE_COMP) || \
                                ((INT) == LPC_INT_COUNT_COMP))
#define IS_LPC_CLEAR_INT(INT) ((INT) == LPC_INT_COUNT_COMP)

/** End of group 87x3e_LPC_interrupts_definition
      * @}
      */


/** End of group 87x3e_LPC_Exported_constants
      * @}
      */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup 87x3e_LPC_Exported_Functions LPC Exported Functions
  * @{
  */

/**
  * @brief  Initializes LPC peripheral according to
  *    the specified parameters in the LPC_InitStruct.
  * @param  LPC_InitStruct: pointer to a LPC_InitTypeDef
  *    structure that contains the configuration information for the
  *    specified LPC peripheral.
  * @retval None
  */
void LPC_Init(LPC_InitTypeDef *LPC_InitStruct);

/**
  * @brief  Enables or disables LPC peripheral.
  * @param  NewState: new state of LPC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_Cmd(FunctionalState NewState);

/**
  * @brief Start or stop the LPC counter.
  * @param  NewState: new state of the LPC counter.
  *   This parameter can be one of the following values:
  *     @arg ENABLE: Start LPCOMP counter.
  *     @arg DISABLE: Stop LPCOMP counter.
  * @retval None
  */
void LPC_CounterCmd(FunctionalState NewState);

/**
  * @brief Reset the LPC counter.
  * @retval None
  */
void LPC_CounterReset(void);

/**
  * @brief  Fills each LPC_InitStruct member with its default value.
  * @param  LPC_InitStruct : pointer to a LPC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void LPC_StructInit(LPC_InitTypeDef *LPC_InitStruct);

/**
  * @brief  Configure LPCOMP counter's comparator value.
  * @param  data: LPCOMP counter's comparator value which can be 0 to 0xfff.
  * @retval None.
  */
void LPC_WriteComparator(uint32_t data);

/**
  * @brief  read LPCOMP comparator value.
  * @param none.
  * @retval LPCOMP comparator value.
  */
uint16_t LPC_ReadComparator(void);

/**
  * @brief  Read LPC counter value.
  * @retval LPCOMP counter value.
  */
uint16_t LPC_ReadCounter(void);

/**
  * @brief  Enables or disables the specified LPC interrupts.
  * @param  LPC_INT: specifies the LPC interrupt source to be enabled or disabled.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_VOLTAGE_COMP: voltage detection interrupt.If Vin<Vth, cause this interrupt.
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @param  NewState: new state of the specified LPC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_INTConfig(uint32_t LPC_INT, FunctionalState NewState);

/**
  * @brief  Clear the specified LPC interrupt.
  * @param  LPC_INT: specifies the LPC interrupt to clear.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval None.
  */
void LPC_ClearINTPendingBit(uint32_t LPC_INT);

/**
  * @brief  Checks whether the specified LPC interrupt is set or not.
  * @param  LPC_INT: specifies the LPC interrupt to check.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval The new state of SPI_IT (SET or RESET).
  */
ITStatus LPC_GetINTStatus(uint32_t LPC_INT);

#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_LPC_H*/

/** @} */ /* End of group 87x3e_LPC_Exported_Functions */
/** @} */ /* End of group 87x3e_LPC */


/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

