/**
****************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file      ftl.h
* @brief     flash transport layer is used as abstraction layer for user application to save read/write
*            parameters in flash.
* @note      ftl is dedicate block in flash, only used for save read/write value, and offset here is
*            logical offset which is defined for whole ftl section.If value is only for one time read,
*            refer to fs_load_app_data_8 or other APIs in flash_device.h
* @author    Brenda_li
* @date      2016-12-27
* @version   v1.0
* **************************************************************************************************
*/

#ifndef _FTL_H_
#define _FTL_H_

#include <stdint.h>
#include <stdbool.h>
#include "errno.h"
#include "storage.h"

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus


/** @defgroup  HAL_FTL    Flash Transport Layer
    * @brief simple implementation of file system for flash
    * @{
    */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup FTL_Exported_Macros Flash Transport Layer Exported Macros
    * @brief
    * @{
    */
#define FTL_PARTITION_NAME "FTL"

#define FTL_POOL_ENABLE 1

#define FTL_IN_HAL

/** @defgroup FTL_IO_CTL_CODE Flash Transport Layer ioctrl code
 * @{
 */

/**
  * @}
  */


/** End of FTL_Exported_Macros
    * @}
    */
/*============================================================================*
  *                                   Types
  *============================================================================*/


/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FTL_Exported_Functions Flash Transport Layer Exported Functions
    * @brief
    * @{
    */
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

/**
    * @brief    Init ftl
    * @param    info  ftl info
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    */
int32_t ftl_init(const T_STORAGE_PARTITION_INFO *info);

/**
    * @brief    Init a ext ftl module
    * @param    module_name  specify ftl ext name, the first 4byte can't be the same
    * @param    malloc_size  ftl module logical size
    * @param    block_len    minimum access unit. Must be an integral multiple of 4 and cannot exceed 128
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     Up to 6 modules can be applied
    */
int32_t ftl_init_module(char *module_name, uint16_t malloc_size, uint8_t block_len);

/**
    * @brief    Set V1 ftl module info
    * @param    module_name  V1 ext FTL name
    * @param    u32PageStartAddr  V1 FTL ext start address
    * @param    pagenum  V1 ext FTL pagenum
    * @param    value_size  V1 ext FTL value size
    * @param    offset_min  V1 ext FTL min offset
    * @return   status
    * @retval   0 status successful
    * @retval   otherwise fail
    */
void ftl_v1_module_info(char *module_name, uint32_t u32PageStartAddr, uint8_t pagenum,
                        uint32_t value_size, uint16_t offset_min);

/**
    * @brief    Save specified value to ftl module
    * @param    module_name  specify ftl module name
    * @param    pdata specify data buffer
    * @param    offset FTL offset to store
    * @param    size   size to store, need to be an integer multiple of block_len in ftl_init_module
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    */
int32_t ftl_save_to_module(char *module_name, void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified ftl module offset parameter to specified buffer
    * @param    module_name  specify ftl module name
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to load, need to be an integer multiple of block_len in ftl_init_module
    * @param    size   size to load
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    */
int32_t ftl_load_from_module(char *module_name, void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Init ftl cache and set ftl cache enable
    * @param    cache_size  ftl cache size
    * @return   status
    * @retval   true: ftl cache init successfully
    * @retval   otherwise fail
    */
bool ftl_cache_init(uint16_t cache_size);

/**
    * @brief    Set rom ftl module size
    * @param    rom_size  ftl module logic size
    * @return   status
    * @retval   0 status successful
    * @retval   otherwise fail
    */
int32_t ftl_set_rom_mudule_size(uint16_t rom_size);

/** @} */ /* End of group FTL_Exported_Functions */

/** @} */ /* End of group HAL_FTL */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_H_
