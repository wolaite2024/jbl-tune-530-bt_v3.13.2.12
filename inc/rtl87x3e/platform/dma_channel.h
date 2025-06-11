/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file
* @brief
* @details
* @author    elliot chen ->justin
* @date      2015-05-08  ->2019-07-19
* @version   v1.0                ->V1.1
* *********************************************************************************************************
*/

#ifndef __GDMA_CHANNEL_MANAGER_H
#define __GDMA_CHANNEL_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "errno.h"
#include <stdbool.h>
/** @addtogroup 87x3e_PLATFORM_GDMA GDMA
  * @brief GDMA driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup 87x3e_PLATFORM_GDMA_Exported_Types GDMA Exported Types
  * @{
  */
typedef struct
{
    uint32_t lr;
} GDMA_ChInfoTypeDef;

#define DMA_CHANNEL_DSP_RESERVED (BIT2|BIT3|BIT4|BIT5|BIT8)
#define DMA_CHANNEL_LOG_RESERVED (BIT7)
/** End of Group 87x3e_PLATFORM_GDMA_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup 87x3e_PLATFORM_GDMA_Exported_Constants GDMA Exported Constants
  * @{
  */


/** End of Group 87x3e_PLATFORM_GDMA_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup 87x3e_PLATFORM_GDMA_Exported_Functions GDMA Exported Functions
  * @{
  */
/**
  * @brief  request an unused DMA ch
  * @param  ch: requested channel number. if there's no unused channel, ch is set to 0xa5
  * @param  isr: requested channel's isr. if isr is NULL, ignore ram vector table update
  * @param  is_hp_ch: 1: request high performance channel, 0: normal DMA channel
  * @retval   true  status successful
  * @retval   false status fail
  */
extern bool (*GDMA_channel_request)(uint8_t *ch, void *isr, bool is_hp_ch);

/**
  * @brief  release an used DMA ch
  * @param  ch: released channel number.
  * @retval None
  */
void GDMA_channel_release(uint8_t ch);

/**
  * @brief  get assigned DMA channel mask
  * @retval mask of assigned DMA channel
  */
uint16_t GDMA_channel_get_active_mask(void);
/**
  * @brief  config mandatory dsp allocation dma channel
  * @param  dsp_dma_channel_mask: mask of assigned DSP DMA channel.
    * @note   rom default set dsp dma channel 2\3\4\5\8, use dma_dsp_channel_set will
              reconfig dsp fixed channel, unused dma channel will release.
  * @retval None
  */
void dma_dsp_channel_set(uint32_t dsp_dma_channel_mask);

/**
  * @brief  config mandatory mcu allocation dma channel
  * @param  fixed_channel_mask: mask of assigned DMA channel.
    * @note   could called repeatedly
  * @retval refs to errno.h
  */
int32_t dma_fixed_channel_set(uint32_t fixed_channel_mask);

#ifdef __cplusplus
}
#endif

#endif /*__GDMA_CHANNEL_MANAGER_H*/

/** @} */ /* End of group 87x3e_PLATFORM_GDMA_Exported_Functions */
/** @} */ /* End of group 87x3e_PLATFORM_GDMA */


/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

