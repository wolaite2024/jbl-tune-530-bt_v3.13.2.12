/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      adc_manager.h
   * @brief     Api and related structures defined for multi-user for adc_driver.
   * @details   The adc manager module is useful for multi-user for one-shot adc.
   *            Please call adc_mgr_deinit when continuous mode adc is in use.
   * @author    Sandy
   * @date      2016.12.29
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2016 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/

#ifndef __ADC_MANAGER_H_
#define __ADC_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "rtl876x_adc.h"


/** @defgroup  87x3e_ADC_MANAGER ADC Manager
    * @brief   arbitor to access ADC. This module will overall manage charger
    *          and user ADC requests cocurrently.
    * @{
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup 87x3e_ADC_MANAGER_Exported_Types Adc Manager Exported Types
    * @{
    */

/**  @brief  The register callback for adc interrupt notify*/
typedef void (*adc_callback_function_t)(void *, uint32_t);

/** End of ADC_MANAGER_Exported_Types
    * @}
    */


/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup 87x3e_ADC_MANAGER_Exported_Macros Adc Manager Exported Macros
    * @{
    */

/**  @brief  The Maximum number of read back*/
#define ADC_MGR_BUF_MAX_LEN    16


/** End of ADC_MANAGER_Exported_Macros
    * @}
    */


/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup 87x3e_ADC_MANAGER_Exported_Functions Adc Manager Exported Functions
    * @{
    */

/**
    * @brief    Initiate ADC Manager module
    * @note     Don't call this function unless call adc_mgr_deinit before
    * @return   status
    * @retval   true  status successful
    * @retval   false status fail
    */
extern bool (*adc_mgr_init)(uint8_t req_num);

/**
  * @brief      Close ADC Manager module
  * @return     none
  */
void adc_mgr_deinit(void);

/**
  * @brief      Fill in adc_driver structure default value
  * @param      p_adc_init  the point to adc_driver structure
  * @return     none
  */
void adc_struct_init_req(ADC_InitTypeDef *p_adc_init);

/**
  * @brief      Request for a channel in adc_manager
  * @note       If return successful, this channel will be reserved before free
  * @param      p_adc_init      the configuration for adc_driver
  * @param      cb              register callback for adc interrupt notify
  * @param      adc_mgr_index   the pointer to the return index corresponding
  *                             to a channel
  * @return     status
  * @retval     true            status successful and adc_mgr_index is valid
  * @retval     false           status fail and adc_mgr_index is meaningless
  */
extern bool (*adc_mgr_register_req)(ADC_InitTypeDef *p_adc_init, adc_callback_function_t cb,
                                    uint8_t *adc_mgr_index);

/**
  * @brief      Free the index corresponding channel
  * @param      index   the number corresponding to a channel
  * @return     none
  */
extern void (*adc_mgr_free_chann)(uint8_t index);

/**
  * @brief      Set interrupt related parameters
  * @note       Make sure the index corresponding channel is valid
  * @param      index                the number corresponding to a channel
  * @param      enable_intr_bit_map  specifies the interrupt pending bit to enable.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_INT_ONE_SHOT_DONE: ADC one convert end interrupt
  *     @arg ADC_INT_FIFO_OVERFLOW: ADC FIFO overflow interrupt
  *     @arg ADC_INT_FIFO_RD_REQ: ADC read FIFO request interrupt
  *     @arg ADC_INT_FIFO_RD_ERR: ADC read FIFO error interrupt
  *     @arg ADC_INT_FIFO_TH: fifo larger than threshold
  * @return   status
  * @retval   true  status successful
  * @retval   false status fail

  */
bool adc_mgr_set_int_para(uint8_t index, uint32_t enable_intr_bit_map);

/**
  * @brief      Start up a one shot mode adc
  * @note       Please call adc_mgr_read_data_req when callback
  * @param      index   the number corresponding to a channel
  * @return     status
  * @retval     true    status successful
  * @retval     false   status fail
  */
extern bool (*adc_mgr_enable_req)(uint8_t index);

/**
  * @brief      Read one shot mode data from adc driver
  * @note       Please call adc_mgr_read_data_req when callback function is called
  * @param      index           the number corresponding to a channel
  * @param      output_data     the pointer to the output data buffer
  * @param      sched_bit_map   ADC schedule table bit map
  * @return     status
  * @retval     true    status successful
  * @retval     false   status fail
  */

bool adc_mgr_read_data_req(uint8_t index, uint16_t *output_data, uint16_t sched_bit_map);

/** @} */ /* End of group 87x3e_ADC_MANAGER_Exported_Functions */

/** @} */ /* End of group 87x3e_ADC_MANAGER */

#ifdef __cplusplus
}
#endif

#endif
