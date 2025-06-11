/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file usb_ms_driver.h
 * @version 1.0
 * @brief usb mass storage driver--only support BOT (Bulk-Only Transport)
 *
 * @note:
 */
#ifndef __USB_MS_DRIVER_H__
#define __USB_MS_DRIVER_H__
#include <stddef.h>
#include <stdbool.h>
#include "usb_spec20.h"
#include "usb_msc_bot.h"

/**
 * @addtogroup USB_MS_DRIVER
 * @{
 * The module mainly supply components to realize USB Mass Storage Class. \n
 * This driver only support Bulk-Only Transport, and donot concern which command set specification \n
 * & storage medium are used
 * @}
  */

/**
 * @addtogroup USB_MS_DRIVER
 * @{
 * @section USB_MSC_DRIVER_USAGE How to realize USB Mass Storage interface.
 * Step1 Realize usb msc interface descriptors array.
 *
 * @par Example
 * @code
 *      #include "usb_spec20.h"
 *
 *      T_USB_INTERFACE_DESC demo_ms_if_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC demo_ms_bo_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC demo_ms_bi_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_INTERFACE_DESC demo_ms_if_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC demo_ms_bo_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC demo_ms_bi_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_DESC_HDR *demo_ms_descs_fs[] =
 *      {
 *          (T_USB_DESC_HDR*)&demo_ms_if_desc_fs,
 *          (T_USB_DESC_HDR*)&demo_ms_bo_ep_desc_fs,
 *          (T_USB_DESC_HDR*)&demo_ms_bi_ep_desc_fs,
 *          NULL
 *      };
 *
 *      T_USB_DESC_HDR *demo_ms_descs_hs[] =
 *      {
 *          (T_USB_DESC_HDR*)&demo_ms_if_desc_fs,
 *          (T_USB_DESC_HDR*)&demo_ms_bo_ep_desc_fs,
 *          (T_USB_DESC_HDR*)&demo_ms_bi_ep_desc_fs,
 *          NULL
 *      };
 *
 *      usb_ms_driver_if_desc_register(demo_ms_descs_fs, demo_ms_descs_hs);
 *
 * Step 2 Register disk driver.
 *
 * @par Example
 * @code
 *      int demo_usb_ms_disk_format(void)
 *      {
 *          ...
 *      }
 *
 *      int demo_usb_ms_disk_read(uint32_t lba, uint32_t blk_num, uint8_t *data)
 *      {
 *          ...
 *      }
 *
 *      int demo_usb_ms_disk_write(uint32_t lba, uint32_t blk_num, uint8_t *data)
 *      {
 *          ...
 *      }
 *
 *      bool demo_usb_ms_disk_is_ready(void)
 *      {
 *          ...
 *      }
 *
 *      int demo_usb_ms_disk_remove(void)
 *      {
 *          ...
 *      }
 *
 *      int usb_ms_disk_capacity_get(uint32_t *max_lba, uint32_t *blk_len)
 *      {
 *          ...
 *      }
 *
 *      T_DISK_DRIVER demo_usb_ms_disk_driver =
 *      {
 *          .type = 0,
 *          .format = demo_usb_ms_disk_format,
 *          .read = demo_usb_ms_disk_read,
 *          .write = demo_usb_ms_disk_write,
 *          .is_ready = demo_usb_ms_disk_is_ready,
 *          .remove = demo_usb_ms_disk_remove,
 *          .capacity_get = usb_ms_disk_capacity_get
 *      };
 *
 *  `   usb_ms_driver_disk_register((T_DISK_DRIVER *)&demo_usb_ms_disk_driver);
 * @endcode
 *
 * Step 3 Register callbacks to process command set specification.
 *
 * @par Example
 * @code
 *      int demo_usb_ms_cmd_proc(T_DISK_DRIVER *disk, T_CBW *cbw)
 *      {
 *          //process command set such as SCSI command
 *          ...
 *      }
 *
 *      int demo_usb_ms_data_proc(T_DISK_DRIVER *disk, uint8_t *data, uint32_t len)
 *      {
 *          //process data stage
 *          ...
 *      }
 *
 *      T_RX_CB demo_usb_ms_rx_cb =
 *      {
 *          .cmd = demo_usb_ms_cmd_proc,
 *          .data = demo_usb_ms_data_proc
 *      };
 *      usb_ms_driver_data_pipe_ind_cbs_register(&demo_usb_ms_rx_cb);
 * @endcode
 * @}
  */

/**
 * @addtogroup USB_MS_DRIVER
 * @{
 * @section Definitions
 */

/**
 * @brief disk driver
 *
 * @param type  disk type
 * @param format callback to format disk
 * @param read callback to read data from disk. \n
 *             parameter: \ref lba logical block address to read  \n
 *                        \ref blk_num block number to read from lba  \n
 *                        \ref data data that be read   \n
 *             return value indicate if read successfully, refer to "errno.h"
 * @param write callback to write data to disk. \n
 *             parameter: \ref lba logical block address to write  \n
 *                        \ref blk_num block number to write from lba  \n
 *                        \ref data data to write   \n
 *             return value indicate if write successfully, refer to "errno.h"
 * @param is_ready callback to judge if disk is ready. \n
 * @param remove callback to remove disk.
 * @param capacity_get callback to get disk capacity. \n
 *             parameter: \ref max_lba maximum logical block address. \n
 *                        \ref blk_len length per block
 *             return value indicate if get capacity successfully, refer to "errno.h"
 */
typedef struct _t_disk_driver
{
    uint8_t type;

    int (*format)(void);
    int (*read)(uint32_t lba, uint32_t blk_num, uint8_t *data);
    int (*write)(uint32_t lba, uint32_t blk_num, uint8_t *data);
    bool (*is_ready)(void);
    int (*remove)(void);
    int (*capacity_get)(uint32_t *max_lba, uint32_t *blk_len);

    void *(*buffer_alloc)(uint32_t blk_num);
    int (*buffer_free)(void *buf);
} T_DISK_DRIVER;

/**
 * @brief rx callback to process rx data
 *
 * @param cmd callback to process CBW stage of BOT \n
 *            parameter: \ref disk disk driver defined as \ref T_DISK_DRIVER. \n
 *                       \ref cbw CBW to process. \n
 *            return value indicate if cbw processed successfully, refer to "errno.h"
 * @param data callback to process data stage of BOT
 *            parameter: \ref disk disk driver defined as \ref T_DISK_DRIVER. \n
 *                       \ref data data to process. \n
 *                       \ref len length of data. \n
 *            return value indicate if cbw processed successfully, refer to "errno.h"
 */
typedef struct _t_rx_cb
{
    int (*cmd)(T_DISK_DRIVER *disk, T_CBW *cbw);
    int (*data)(T_DISK_DRIVER *disk, uint8_t *data, uint32_t len);
} T_RX_CB;

/**
 * @brief definition of callback which will be called when data tx is complete
 *
 * @param disk disk driver defined as \ref T_DISK_DRIVER.
 * @param actual actual length of data that has been sent
 * @param status indicate if data has been sent successfully
 * @return refer to "errno.h"
 *
 */
typedef int (*USB_MS_DRIVER_TX_COMPLETE)(T_DISK_DRIVER *disk, uint32_t actual, int status);

/**
 * @brief send data by bulk in endpoint
 *
 * @param data data to send
 * @param len length of data to send
 * @param complete refer to \ref USB_MS_DRIVER_TX_COMPLETE
 * @return int  refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_MSC_DRIVER_USAGE
 */
int usb_ms_driver_data_pipe_send(void *data, uint32_t len, USB_MS_DRIVER_TX_COMPLETE complete);

/**
 * @brief register callbacks to process data received
 *
 * @param cbs refer to \ref T_RX_CB
 * @return int refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_MSC_DRIVER_USAGE
 */
int usb_ms_driver_data_pipe_ind_cbs_register(T_RX_CB *cbs);

/**
 * @brief register disk driver
 *
 * @param disk refer to \ref T_DISK_DRIVER
 * @return int refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_MSC_DRIVER_USAGE
 */
int usb_ms_driver_disk_register(T_DISK_DRIVER *disk);

/**
 * usb_ms_driver.h
 *
 * \brief   Mass storage interface descriptor register
 *
 * \param[in]  fs_desc Mass storage interface descriptor of full speed
 * \param[in]  hs_desc Mass storage interface descriptor of high speed
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_MSC_DRIVER_USAGE
 */
int usb_ms_driver_if_desc_register(T_USB_DESC_HDR **descs_fs,  T_USB_DESC_HDR **descs_hs);

/**
 * @brief init usb mass storage driver
 *
 * @return int refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_MSC_DRIVER_USAGE
 */
int usb_ms_driver_init(void);
/** @}*/
#endif
