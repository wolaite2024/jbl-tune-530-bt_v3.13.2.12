/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
/** @defgroup  HAL_87x3e_Storage_Access    storage access
    * @brief storage access wrapper api
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup HAL_87x3e_STORAGE_Exported_Macros Storage Access Exported Macros
    * @brief
    * @{
    */
/**
 * storage.h
 *
 * \brief   Storage access permission type definitions.
 *
 * \ingroup STORAGE
 */
#define STORAGE_PERMISSION_EXECUTE  0x00000001
#define STORAGE_PERMISSION_READ     0x00000002
#define STORAGE_PERMISSION_WRITE    0x00000004

/** End of HAL_87x3e_STORAGE_Exported_Macros
    * @}
    */

/** @defgroup HAL_87x3e_T_STORAGE_MEDIA_TYPE T_STORAGE_MEDIA_TYPE
 * @{
 */
/**
 * storage.h
 *
 * \brief   Storage media type definitions.
 *
 * \ingroup STORAGE
 */
typedef enum t_storage_media_type
{
    STORAGE_MEDIA_TYPE_ROM      =  0x01,
    STORAGE_MEDIA_TYPE_RAM      =  0x02,
    STORAGE_MEDIA_TYPE_NOR      =  0x03,
    STORAGE_MEDIA_TYPE_NAND     =  0x04,
    STORAGE_MEDIA_TYPE_PSRAM    =  0x05,
    STORAGE_MEDIA_TYPE_EMMC     =  0x06,
} T_STORAGE_MEDIA_TYPE;
/** End of HAL_87x3e_T_STORAGE_MEDIA_TYPE
  * @}
  */

/** @defgroup HAL_87x3e_T_STORAGE_CONTENT_TYPE T_STORAGE_CONTENT_TYPE
* @{
*/
/**
 * storage.h
 *
 * \brief   Storage content type definitions.
 *
 * \ingroup STORAGE
 */
typedef enum  t_storage_content_type
{
    STORAGE_CONTENT_TYPE_TEXT     = 0,
    STORAGE_CONTENT_TYPE_RO_DATA  = 1,
    STORAGE_CONTENT_TYPE_RW_DATA  = 2,
} T_STORAGE_CONTENT_TYPE;
/** End of HAL_87x3e_T_STORAGE_CONTENT_TYPE
  * @}
  */

/** @defgroup HAL_87x3e_T_STORAGE_ASYNC_CBACK T_STORAGE_ASYNC_CBACK
 * @{
 */
/**
 * storage.h
 *
 * \brief Storage asynchronous callback function prototype.
 *
 * \ingroup STORAGE
 */
typedef void (*T_STORAGE_ASYNC_CBACK)(void *context);
/** End of HAL_87x3e_T_STORAGE_ASYNC_CBACK
  * @}
  */

/** @defgroup HAL_87x3e_T_STORAGE_PARTITION_INFO T_STORAGE_PARTITION_INFO
* @{
*/
/**
 * storage.h
 *
 * \brief Storage partition information definition.
 *
 * \ingroup STORAGE
 */
typedef struct t_storage_partition_info
{
    const char                  *name;
    size_t                      address;
    size_t                      size;
    /*permition, STORAGE_PERMISSION_EXECUTE\STORAGE_PERMISSION_READ\STORAGE_PERMISSION_WRITE.*/
    size_t                      perm;
    /*media type , \ref T_STORAGE_MEDIA_TYPE.*/
    T_STORAGE_MEDIA_TYPE        media_type;
    /*content_type , \ref T_STORAGE_CONTENT_TYPE.*/
    T_STORAGE_CONTENT_TYPE      content_type;
    /*init, initial partition function.*/
    int (*init)(const struct t_storage_partition_info *);
    /*deinit, deinitial partition function.*/
    int (*deinit)(const struct t_storage_partition_info *);
    /*read, read from the partiton,  synchronous function.*/
    int (*read)(const struct t_storage_partition_info *, size_t offset, size_t len, void *buf);
    /*async_read, read from the partiton , asynchronous function, callback function provided read resule.*/
    int (*async_read)(const struct t_storage_partition_info *, size_t offset, size_t len, void *buf,
                      T_STORAGE_ASYNC_CBACK callback, void *context);
    /*write, write to the partiton , synchronous function.*/
    int (*write)(const struct t_storage_partition_info *, size_t offset, size_t len, void *buf);
    /*async_write, write to the partiton , asynchronous function, callback function provided write resule.*/
    int (*async_write)(const struct t_storage_partition_info *, size_t offset, size_t len, void *buf,
                       T_STORAGE_ASYNC_CBACK callback, void *context);
    /*erase, erase the partiton , synchronous function.*/
    int (*erase)(const struct t_storage_partition_info *, size_t offset, size_t len);
    /*async_erase, erase the partiton , asynchronous function, callback function provided erase resule.*/
    int (*async_erase)(const struct t_storage_partition_info *, size_t offset, size_t len,
                       T_STORAGE_ASYNC_CBACK callback, void *context);
} T_STORAGE_PARTITION_INFO;
/** End of HAL_87x3e_T_STORAGE_PARTITION_INFO
  * @}
  */

/** @defgroup HAL_87x3e_Storage_Access_Exported_Functions Storage Access Exported Functions
   * @brief
   * @{
   */

/**
 * storage.h
 *
 * \brief  Register storage partition table.
 *
 * \param[in] table Storage partition table \ref T_STORAGE_PARTITION_INFO.
 * \param[in] size  Storage partition table size.
 *
 * \return refer to errno.h.
 *
 * \ingroup STORAGE
 */
int storage_partition_init(const T_STORAGE_PARTITION_INFO *table, size_t size);

/**
 * storage.h
 *
 * \brief  Deinitialize storage partition table.
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 *
 * \param[in] table Storage partition table \ref T_STORAGE_PARTITION_INFO.
 *
 * \return refer to errno.h.
 *
 * \ingroup STORAGE
 */
int storage_partition_deinit(const T_STORAGE_PARTITION_INFO *info);

/**
 * storage.h
 *
 * \brief  Get the specific storage partition information.
 *
 * \param[in] name Storage partition name.
 *
 * \return   the info refers to T_STORAGE_PARTITION_INFO.
 *
 * \ingroup STORAGE
 */
const T_STORAGE_PARTITION_INFO *storage_partition_get(const char *name);


/**
 * storage.h
 *
 * \brief  Storage partition read operation.
 *
 * \param[in] name     Storage partition name.
 * \param[in] offset   Storage read from offset of the partition
 * \param[in] len      Storage read bytes length
 * \param[in] buf      Storage read buffer
 * \param[in] callback   NULL for synchronous operation and callback fucntion for asynchronous
 * \param[in] context    input paramter for callback funciton
 *
 * \return   refer to errno.h.
 *
 * \ingroup STORAGE
 */
int storage_read(const char *name, size_t offset, size_t len, void *buf,
                 T_STORAGE_ASYNC_CBACK callback, void *context);


/**
 * storage.h
 *
 * \brief  Storage partition write operation.
 *
 * \param[in] name    Storage partition name.
 * \param[in] offset  Storage write offset of the partition
 * \param[in] len     Storage write bytes length
 * \param[in] buf     Storage write buffer
 * \param[in] callback   NULL for synchronous operation and callback fucntion for asynchronous
 * \param[in] context    input paramter for callback funciton
 *
 * \return   refer to errno.h.
 *
 * \ingroup STORAGE
 */
int storage_write(const char *name, size_t offset, size_t len, void *buf,
                  T_STORAGE_ASYNC_CBACK callback, void *context);


/**
 * storage.h
 *
 * \brief  Storage partition erase operation.
 *
 * \param[in] name    Storage partition name.
 * \param[in] offset  Storage erase offset of the partition
 * \param[in] len     Storage erase bytes length
 * \param[in] callback   NULL for synchronous operation and callback fucntion for asynchronous
 * \param[in] context    input parameter for callback funciton
 *
 * \return   refer to errno.h.
 *
 * \ingroup STORAGE
 */
int storage_erase(const char *name, size_t offset, size_t len,
                  T_STORAGE_ASYNC_CBACK callback, void *context);


#ifdef __cplusplus
}
#endif /* __cplusplus */
/** @} */ /* End of group HAL_87x3e_Storage_Access_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_Storage_Access */
#endif /* _STORAGE_H_ */
