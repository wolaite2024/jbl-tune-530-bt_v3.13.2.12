/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     task_log_api.h
* @brief    This file provides log task api declaration.
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _TASK_LOG_API_H_
#define _TASK_LOG_API_H_
#include <stdint.h>
#include "mem_types.h"
/** @defgroup  HAL_87x3e_Task_Log    save log to flash
    * @brief save log to flash api
    * @{
    */
/**
 * @section LOG2FLASH_API_Usage_Chapter How to use this module.
 *
 * prepare log_task_init parameter value:
 * - Assign the log partition space in the flash layout aligned by 4Kbytes as log_part_addr and log_part_size. \n
 * - select the ram type to alloc the ring buffer for log2flash as rb_ram_type. \n
 * - check the avaliable ram size for ring buffer on the selected ram type as log_rb_size. \n
 * - check whether there is dma channel available as use_dma. If use dma for flash write operation, \n
 *   available dma channel will be requested and freed dynamically.
 *
 * before calling log_task_init api:
 * - If the log partition space is under flash block pretection region, Please call fmc_flash_nor_set_bp_lv to
 *   unlock flash bp level.
 * - If the log partition address is more than 16Mbytes, please call fmc_flash_set_4_byte_address_mode to enable
 *   4-byte address mode for flash driver.
 *
 * sample code:
 * @code
 *    //use inactive bank space as log partition space
 *
 *   uint32_t log_temp_addr = 0x02056000;
 *   uint32_t log_temp_length = 0x001AA000;
 *
 *   //alloc ring buffer 4000 bytes on data ram, and use flash write dma
 *   int init_ret = log_task_init(log_temp_addr, log_temp_length, 4000, OS_MEM_TYPE_DATA, true);
 *   if(init_ret != 0)
 *   {
 *      APP_PRINT_ERROR1("log_task_init return %d",init_ret);
 *   }
 *
 *   // unlock flash bp level before calling log_task_init if the log partition space
 *   // is under flash block protection region
 *   fmc_flash_nor_set_bp_lv(log_temp_addr, 0);
 *
 *   // enable 4-byte address mode for flash before calling log_task_init if the log
 *   // partition address is larger than 16Mbytes.
 *   fmc_flash_set_4_byte_address_mode(FMC_FLASH_NOR_IDX0, true);
 *
 *   // If need to disable log2flash function, call enable_log_to_flash with parameter "false"
 *   enable_log_to_flash(false);
 * @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup HAL_87x3e_Task_Log_Exported_Macros Task Log Exported Macros
    * @brief
    * @{
    */

#define LOG_PARTITION_NAME "LOGGING_PART"
#define LOG_TASK_PRIORITY       (1)
#define LOG_TASK_STACK_MAX_SIZE (1024)

/*adjust the log buffer size according to the available heap size*/
#define LOG_BUFFER_SIZE        (1024*30)
/** End of HAL_87x3e_Task_Log_Exported_Macros
    * @}
    */
/** @defgroup HAL_87x3e_TASK_LOG_Exported_Functions Task Log Exported Functions
    * @brief
    * @{
    */
/**
    * @brief  set up log2flash task
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @note   specify the log partition space on the flash through log_part_addr
    *         and log_part_size, specify the log2flash cache ring buffer size
    *         through the log_rb_size, LOG_BUFFER_SIZE is the reference value.
    *         Please keep the log partition space aligned by 4Kbytes. If the log
    *         partition space is under the flash block protection region, please
    *         unlock the flash bp level before using it as log partition. If the
    *         log partition region is assigned at the address more than 16 Mbytes,
    *         please consider to use 4-byte address mode for the flash driver,
    *         refer to the api fmc_flash_set_4_byte_address_mode.
    * @param  log_part_addr log partition address on the flash
    * @param  log_part_size log partition size on the flash
    * @param  log_rb_size   log2flash cache ring buffer size on the heap
    * @param  rb_ram_type   ref RAM_TYPE definition, RAM_TYPE_DSPSHARE, RAM_TYPE_ITCM1
    *                       or other invalid ram type.
    * @param  use_dma true: use flash dma write. false: use flash user write.
    * @return the result of log2flash task init and create
    * @retval 0      success
    * @retval != 0   error code
    */
int log_task_init(size_t log_part_addr, size_t log_part_size, size_t log_rb_size,
                  RAM_TYPE rb_ram_type,  bool use_dma);
/**
    * @brief  Enable or disable log to flash
    * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
    * @note  When user call log_task_int api to init the log to flash function, the log
    *        to flash status is enabled by default. If user wants to disable the log to
    *        flash operation, use enable_log_to_flash(false) to stop writting log data to
    *        flash, but the log task will still run without writting flash.
    * @warning This api is supported in RTL87x3E and RTL87x3D.
    *          It is NOT supported in RTL87x3G.
    * @param  enable_status true: enable log to flash operation
    *                       false: disable log to flash operation
    * @return void
    */
void enable_log_to_flash(bool enable_status);


/**
    * @brief  Get status of log to flash operation
    * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
    * @note  When user call log_task_int api to init the log to flash function, the log
    *        to flash status is enabled by default. If user wants to known the current status
    *        of log to flash operation, use log_to_flash_status_get to get the status.
    * @warning This api is supported in RTL87x3E and RTL87x3D.
    *          It is NOT supported in RTL87x3G.
    * @param  void
    * @return the status of log to flash operation
    * @retval true: log to flash operation is enabled
    * @retval false: log to flash operation is disabled
    */
bool log_to_flash_status_get(void);
/** @} */ /* End of group HAL_87x3e_TASK_LOG_Exported_Functions */
#ifdef __cplusplus
}
#endif

/** @} */ /* End of group HAL_87x3e_Task_Log */
#endif
