/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file usb_composite_driver.h
 * @version 1.0
 * @brief
 *
 * @note:
 */
#ifndef __USB_COMPOSITE_DEV_H__
#define __USB_COMPOSITE_DEV_H__

#include <stdint.h>
#include "os_queue.h"
#include "hal_usb.h"
#include "usb_spec20.h"
#include "usb_utils.h"
/**
 * @addtogroup USB_Composite
 * @{
 * USB composite device consists of mutiple configurations, which then each consist multiple interfaces. \n
 * This module offers fundamental components to realize USB composite device which include multiple USB classes, such as USB Audio & HID.
 * @}
 */
/**
 * @addtogroup USB_Composite
 * @{
 *
 * @section USB_Composite_Usage_Chapter_Device How to init USB device & configuration
 * @par Example
 * @code
 *      T_USB_DEVICE_DESC demo_usb_dev_desc =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_CONFIG_DESC demo_usb_cfg_desc =
 *      {
 *          ...init...
 *      };
 *
 *      usb_composite_dev_init(&demo_usb_dev_desc);
 *      usb_composite_dev_cfg_add(&demo_usb_cfg_desc);
 *
 *      //if string index of demo_usb_dev_desc/demo_usb_cfg_desc is not zero
 *      usb_composite_dev_string_add(language, string index, string);
 * @endcode
 *
 * @section USB_Composite_Usage_Chapter_Interface How to realize USB interfaces
 * @details The core interface information is defined in \ref T_USB_INTERFACE. It is mainly to initialize \ref T_USB_INTERFACE
 *          to realize an interface.
 * @par Example
 * @code
 *      #include "usb_spec20.h"
 *      T_USB_INTERFACE_DESC std_if_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      (Class interface descriptor type) class_if_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC std_endpoint_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      (Class endpoint descriptor type)  class_endpoint_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_INTERFACE_DESC std_if_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      (Class interface descriptor type) class_if_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_ENDPOINT_DESC std_endpoint_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      (Class endpoint descriptor type)  class_endpoint_desc_fs =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_DESC_HDR* demo_if_descs_hs[] =
 *      {
 *          (T_USB_DESC_HDR*)&std_if_desc_hs,
 *          (T_USB_DESC_HDR*)&class_if_desc_hs,
 *          (T_USB_DESC_HDR*)&std_endpoint_desc_hs,
 *          (T_USB_DESC_HDR*)&class_endpoint_desc_hs,
 *          NULL,
 *      };
 *
 *      T_USB_DESC_HDR* demo_if_descs_fs[] =
 *      {
 *          (T_USB_DESC_HDR*)&std_if_desc_fs,
 *          (T_USB_DESC_HDR*)&class_if_desc_fs,
 *          (T_USB_DESC_HDR*)&std_endpoint_desc_fs,
 *          (T_USB_DESC_HDR*)&class_endpoint_desc_fs,
 *          NULL,
 *      };
 *
 *      int demo_if_ctrl_request_proc(T_USB_INTERFACE *interface, T_USB_DEVICE_REQUEST *ctrl_request, T_HAL_USB_REQUEST_BLOCK *ctrl_urb)
 *      {
 *          //Process class specific control request
 *      }
 *
 *      int demo_if_alt_get(T_USB_INTERFACE *interface, T_HAL_USB_REQUEST_BLOCK *ctrl_urb)
 *      {
 *          //Process GET_ALT request
 *      }
 *
 *      int demo_if_alt_set(T_USB_INTERFACE *interface, T_HAL_USB_REQUEST_BLOCK *ctrl_urb, uint8_t alt)
 *      {
 *          //Process SET_ALT request
 *      }
 *
 *      int demo_if_create(T_USB_INTERFACE *interface)
 *      {
 *          //Init interface related resource, such as endpoints
 *      }
 *
 *      int demo_if_release(T_USB_INTERFACE *interface)
 *      {
 *          //Deinit interface related resource initialized in \ref demo_if_create
 *      }
 *
 *      int demo_if_suspend(T_USB_INTERFACE *interface)
 *      {
 *          //Process suspend
 *      }
 *
 *      int demo_if_resume(T_USB_INTERFACE *interface)
 *      {
 *          //Process resume
 *      }
 *
 *      T_USB_INTERFACE usb_if =
 *      {
 *          .if_num = 0,
 *          .descs_fs = demo_if_descs_fs,
 *          .descs_hs = demo_if_descs_hs,
 *          .ctrl_request_proc = demo_if_ctrl_request_proc,
 *          .alt_get = demo_if_alt_get,
 *          .alt_set = demo_if_alt_set,
 *          .suspend = demo_if_suspend,
 *          .resume = demo_if_resume,
 *          .create = demo_if_create,
 *          .release = demo_if_release,
 *      };
 *      usb_composite_dev_interface_add(&usb_if, cfg val);
 *@endcode
 * @}
 */

/**
 * @addtogroup USB_Composite
 * @{
 * @section USB_Composite_Definitions Definitions
*/
/**
 * @brief USB Composite device APIs & definitions
 *
 * @brief USB endpoint structure, it's the item of list \ref eps in \ref T_USB_INTERFACE
 * @param p_next point to next endpoint of list \ref eps in \ref T_USB_INTERFACE
 * @param addr endpoint address
 * @param desc endpoint descriptor
 * @param ep_handle endpoint handle
 *
 * @ingroup USB_COMPOSITE
 */
typedef struct _usb_ep
{
    struct _usb_ep *p_next;
    uint8_t addr;
    T_USB_ENDPOINT_DESC *desc;
    void *ep_handle;
    void *priv;
} T_USB_EP;

/**
 * @brief Core structure to realize interface
 * @param if_num interface number, NOTE: the value may be changed in \ref usb_composite_dev_interface_add
 * @param descs_fs full speed interface descriptors
 * @param descs_hs high speed interface descriptors
 * @param eps endpoint list belongs to the interface
 * @param ctrl_request_proc callback to process class specific requests
 * @param alt_get callback to process GET_ALT request
 * @param alt_set callback to process SET_ALT request
 * @param create callback to initialize interface, that will be called in \ref usb_composite_dev_interface_add. \n
 *               And \ref if_num in the input parameter interface may be changed, the interface MUST change bInterfaceNumber \n
 *              in the interface descriptor to the actual interface number.
 * @param release callback to release interface related resource
 * @param priv private data
 *
 * @ingroup USB_COMPOSITE
 */
typedef struct _usb_interface
{
    uint8_t if_num;
    // uint8_t cur_alt;
    struct usb_descriptor_header **descs_fs;
    struct usb_descriptor_header **descs_hs;
    T_USB_UTILS_LIST eps;

    int (*ctrl_request_proc)(struct _usb_interface *interface, T_USB_DEVICE_REQUEST *ctrl_request,
                             T_HAL_USB_REQUEST_BLOCK *ctrl_urb);
    int (*alt_get)(struct _usb_interface *interface, T_HAL_USB_REQUEST_BLOCK *ctrl_urb);
    int (*alt_set)(struct _usb_interface *interface, T_HAL_USB_REQUEST_BLOCK *ctrl_urb, uint8_t alt);
    int (*suspend)(struct _usb_interface *interface);
    int (*resume)(struct _usb_interface *interface);
    int (*create)(struct _usb_interface *interface);
    int (*release)(struct _usb_interface *interface);

    void *priv;

} T_USB_INTERFACE;

/**
 *
 * @brief   USB composite dev callback
 *
 */
typedef int (*INT_VENDOR_FUNC)(uint16_t request_cmd, void *buf, uint16_t len);

/**
 *
 * @brief   USB composite dev callback to support cfu
 *          \ref get_vendor: this api will be called when receiving composite get vendor request
 *          \ref set_vendor: this api will be called when receiving composite set vendor request
 *
 */
typedef struct _usb_composite_vendor_cbs
{
    INT_VENDOR_FUNC   get;
    INT_VENDOR_FUNC   set;
} T_USB_COMPOSITE_VENDOR_CBS;

/**
 *
 * @brief   register composite device vendor callbacks to process custom SET_VENDOR/GET_VENDOR request
 *
 * @param  cbs \ref T_USB_COMPOSITE_VENDOR_CBS
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_composite_dev_vendor_cbs_register(T_USB_COMPOSITE_VENDOR_CBS *cbs);

/**
 *
 * @brief   unregister composite device vendor callbacks
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_composite_dev_vendor_cbs_unregister(void);

/**
 * @brief Get ep0 maxpacket size
 *
 * @return uint8_t Get ep0 maxpacket size
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
uint8_t usb_composite_dev_ep0_mps_get(void);

/**
 * @brief Add string to USB composite device
 *
 * @param language language of target string
 * @param id id of  target string, it's the value of string index in the according descriptor
 * @param s string
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to \ref USB_Composite_Usage_Chapter_Device
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_string_add(uint16_t language, uint8_t id, const char *s);

/**
 * @brief Remove string from USB composite device
 *
 * @param language language of target string
 * @param id id of  target string, it's the value of string index in the according descriptor
 * @param s string
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_string_remove(uint16_t language, uint8_t id, const char *s);

/**
 * @brief Add configuration to USB composite device
 *
 * @param cfg_desc configuration descriptor
 * @return int  result, refer to "errno.h"
 * @par Example
 * Please refer to \ref USB_Composite_Usage_Chapter_Device
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_cfg_add(T_USB_CONFIG_DESC *cfg_desc);

/**
 * @brief Remove configuration from USB composite device
 *
 * @param cfg_desc configuration descriptor
 * @return int  result, refer to "errno.h"
 * @par Example
 * Please refer to \ref USB_Composite_Usage_Chapter_Device
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_cfg_remove(T_USB_CONFIG_DESC *cfg_desc);

/**
 * @brief Add interface to target configuration
 *
 * @param interface refer to \ref USB_Composite_Usage_Chapter_Interface
 * @param cfg_val configuration value the interface belongs to
 * @return int  result, refer to "errno.h"
 * @par Example
 * Please refer to \ref USB_Composite_Usage_Chapter_Interface
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_interface_add(T_USB_INTERFACE *interface, uint8_t cfg_val);

/**
 * @brief Remove interface from target configuration
 *
 * @param interface refer to \ref USB_Composite_Usage_Chapter_Interface
 * @param cfg_val configuration value the interface belongs to
 * @return int  result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_interface_remove(T_USB_INTERFACE *interface, uint8_t cfg_val);

/**
 * @brief get enum speed, it's the actual running speed after speed enum
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @return T_HAL_USB_SPEED
 */
T_HAL_USB_SPEED usb_composite_dev_enum_speed_get(void);

/**
 * @brief Remote wakeup
 *
 * @return int  result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_remote_wakeup(void);

/**
 * @brief init control transfer buffer len
 *
 * @param buf_len control transfer buffer len
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_ctrl_xfer_buf_init(uint16_t buf_len);

/**
 * @brief init USB composite device
 *
 * @param dev_desc device descriptor
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to \ref USB_Composite_Usage_Chapter_Device
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_init(T_USB_DEVICE_DESC *dev_desc);

/**
 * @brief deinit USB composite device
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @ingroup USB_COMPOSITE
 */
int usb_composite_dev_deinit(void);

/** @}*/
#endif
