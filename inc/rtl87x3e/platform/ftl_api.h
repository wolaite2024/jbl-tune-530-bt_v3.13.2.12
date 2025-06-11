/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      ftl_api.h
   * @brief     ftl header file for rom project
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _FTL_API_H_
#define _FTL_API_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include "stdint.h"


#ifdef  __cplusplus
extern  "C" {
#endif

/*============================================================================*
  *                                   Types
  *============================================================================*/

/*============================================================================*
  *                                Functions
  *============================================================================*/
/**
    * @brief    Save specified value to specified ftl offset
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to store
    * @param    size   size to store
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
extern uint32_t(*ftl_save_to_storage)(void *pdata_tmp, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified ftl offset parameter to specified buffer
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to load
    * @param    size   size to load
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
extern uint32_t(*ftl_load_from_storage)(void *pdata_tmp, uint16_t offset, uint16_t size);

extern void (*ftl_garbage_collect_check_in_idle)(void);

extern void (*ftl_garbage_collect_in_flash_task)(void);

extern uint32_t (*ftl_module_init)(uint32_t u32PageStartAddr, uint8_t pagenum, uint32_t value_size,
                                   uint32_t start_offse);


#ifdef  __cplusplus
}
#endif

#endif // _FTL_API_H_
