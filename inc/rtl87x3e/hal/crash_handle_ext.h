/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    crash_handle_ext.h
* @brief   This file provides crash handle wrapper for sdk customer.
* @author
* @date
* @version v1.0
* *************************************************************************************
*/
#ifndef __CRASH_HANDLE_EXT_H__
#define __CRASH_HANDLE_EXT_H__
#include "stdint.h"

/** @defgroup  HAL_87x3e_CRASH_HANDLE_API    crash handle api
    * @brief crash handle api
    * @{
    */
/** @defgroup 87x3e_Crash_Handle_Exported_Functions Crash Hanlde Exported Functions
    * @brief
    * @{
    */

#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  set the crash dump flash space
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param crash_dump_flash_addr  specify the flash address to save the crash dump data
    * @param crash_dump_flash_size  specify the flash size to save the crash dump data
    * @return >= 0 succuss, < 0 refer to errno.h
    */
int32_t hal_crash_dump_set(uint32_t crash_dump_flash_addr, uint32_t crash_dump_flash_size);

#ifdef __cplusplus
}
#endif
/** @} */ /* End of group 87x3e_Crash_Handle_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_CRASH_HANDLE_API */
#endif //__CRASH_HANDLE_EXT_H__
