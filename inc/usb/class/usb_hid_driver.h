/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file usb_hid_driver.h
 * @version 1.0
 * @brief Upper application can used the definitions&APIs to realize HID function instances.
 *        The driver support multiple hid functions, such as consumer control, mouse, keyboard and so on.
 *
 * @note:
 */
#ifndef __USB_HID_DRIVER_H__
#define __USB_HID_DRIVER_H__
#include <stdbool.h>
#include <stdint.h>
#include "usb_pipe.h"
/**
 * @addtogroup USB_HID_DRIVER
 * @{
 * The module mainly supply components to realize USB HID Class. \n
 * This driver support multiple interfaces and endpoints, and also support multiple function, \n
 * such as consumer control, mouse, keyboard and so on.
 * @}
  */

/**
 * @addtogroup USB_HID_DRIVER
 * @{
 * @section USB_HID_DRIVER_USAGE How to realize USB HID interface.
 * Firstly, allocate a function instance by \ref usb_hid_driver_inst_alloc.
 * @par Example
 * @code
 *      void *demo_instance = usb_hid_driver_inst_alloc();
 * @endcode
 *
 * Then, realize hid interfaces as follows:
 *  - \b Hid \b Interfaces. \n
 *    - step1: realize a repot descriptor.
 *    - step2: realize descriptors array.
 *    - step3: realize pipe operations.
 * @par Example  -- USB HID
 * @code
 *      const char demo_report_descs[] =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_INTERFACE_DESC demo_std_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_HID_CS_IF_DESC demo_cs_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_int_in_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_int_in_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *      void *demo_if_descs_fs[] =
 *      {
 *          (void *) &demo_std_if_desc,
 *          (void *) &demo_cs_if_desc,
 *          (void *) &demo_int_in_ep_desc_fs,
 *          NULL,
 *      };
 *      void *demo_if_descs_hs[] =
 *      {
 *          ...
 *          NULL,
 *      };
 *
 *      void *demo_data_pipe_open(uint8_t ep_addr, T_USB_HID_DRIVER_ATTR attr, uint8_t pending_req_num,
 *                                  USB_CDC_DATA_PIPE_CB cb)
 *      {
 *          ...
 *          return usb_hid_driver_data_pipe_open(ep_addr, attr, pending_req_num, cb);
 *      }
 *      bool demo_data_pipe_send(void *handle, void *buf, uint32_t len)
 *      {
 *          usb_hid_driver_data_pipe_send(handle, buf, len);
 *          return true;
 *      }
 *      int demo_data_pipe_close(void *handle)
 *      {
 *          return usb_hid_driver_data_pipe_close(handle);
 *      }
 *      usb_hid_driver_if_desc_register(demo_instance, (void *)demo_if_descs_hs, (void *)demo_if_descs_fs, (void *)demo_report_descs);
 *      T_USB_HID_DRIVER_CB demo_cbs =
 *      {
 *          .get_report = demo_get_report,
 *          .set_report = demo_set_report,
 *      }
 *      usb_hid_driver_cbs_register(demo_instance, &demo_cbs);
 * @endcode
 *
 * Lastly, call \ref usb_hid_driver_init to initialize usb hid driver.
 */
/** @}*/

/**
 * @addtogroup USB_HID_DRIVER
 * @{
 * @section Definitions
 */
/**
 * usb_hid_driver.h
 *
 * \brief   HID pipe attribute zero length packet
 *
 * \details If ZLP attribute is set, hid will send zero length packet if data length
 *          is equal to ep maxpacket size
 *
 */
#define HID_PIPE_ATTR_ZLP   0x0001


/**
 * usb_hid_driver.h
 *
 * \brief   congestion control
 *
 * \details If HID_DRIVER_CONGESTION_CTRL_DROP_CUR is set, current data to send
 *          will be dropped, else the first data in the queue will be dropped.
 *
 * \note    Only effective for in endpoint.
 *
 */
#define HID_DRIVER_CONGESTION_CTRL_DROP_CUR     USB_PIPE_CONGESTION_CTRL_DROP_CUR
#define HID_DRIVER_CONGESTION_CTRL_DROP_FIRST   USB_PIPE_CONGESTION_CTRL_DROP_FIRST

/*  HID Report structure (from audio spec)
    **********************************************
    Byte0:  Report ID   0x01    (fixed)
    Byte1:  Volume Up           BIT0
            Volume Down         BIT1
            Play                BIT2
            Scan Next           BIT3
            Scan Previous       BIT4
            Stop                BIT5
            Fast Forward        BIT6
            Rewind              BIT7

    Byte2:  Redial              BIT0
            Hook Switch         BIT1
            Mic Mute            BIT2
            Reserved            BIT3~BIT7

    Byte3:  Reserved
    ***********************************************
*/

/**
 * usb_hid_driver.h
 *
 * \brief   USB HID pipe attr
 *
 */
typedef T_USB_PIPE_ATTR T_USB_HID_DRIVER_ATTR;

/**
 * usb_hid_driver.h
 *
 * \brief   HID report value
 *
 */
typedef struct _hid_driver_request_val
{
    uint8_t     id;
    uint8_t     type;
} T_HID_DRIVER_REPORT_REQ_VAL;

/**
 * usb_hid_driver.h
 *
 * \brief   USB HID callback
 *
 */
typedef int32_t (*INT_IN_FUNC)(T_HID_DRIVER_REPORT_REQ_VAL, void *, uint16_t *);
typedef int32_t (*INT_OUT_FUNC)(T_HID_DRIVER_REPORT_REQ_VAL, void *, uint16_t);
typedef USB_PIPE_CB USB_HID_DRIVER_CB;
/**
 * usb_hid_driver.h
 *
 * \brief   HID driver callback to support cfu
 *          \ref get_report: this api will be called when receiving HID GET_REPORT request
 *          \ref set_report: this api will be called when receiving HID SET_REPORT request
 *
 */
typedef struct _usb_hid_driver_cbs
{
    INT_IN_FUNC    get_report;
    INT_OUT_FUNC   set_report;
} T_USB_HID_DRIVER_CBS;

/**
 * @brief alloc hid function instance, which has the standalone interface
 * @return void*  hid function instance
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
void *usb_hid_driver_inst_alloc(void);

/**
 * @brief free hid function instance, alloacted by \ref usb_hid_driver_inst_alloc
 * @param inst, instance alloacted by \ref usb_hid_driver_inst_alloc
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_inst_free(void *inst);

/**
 * usb_hid_driver.h
 *
 * \brief   HID interface descriptor register
 *
 * \param[in]  inst hid instance returned in \ref usb_hid_driver_inst_alloc
 * \param[in]  hs_desc HID interface descriptor of high speed
 * \param[in]  fs_desc HID interface descriptor of full speed
 * \param[in]  report_desc HID report descriptor, \ref T_REPORT_DESC
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_if_desc_register(void *inst, void *hs_desc, void *fs_desc, void *report_desc);

/**
 * usb_hid_driver.h
 *
 * \brief   HID interface descriptor unregister
 *
 * \param inst hid instance returned in \ref usb_hid_driver_inst_alloc
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_if_desc_unregister(void *inst);

/**
 * usb_hid_driver.h
 *
 * \brief   open hid data pipe
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr HID pipe attribute of \ref T_USB_HID_DRIVER_ATTR
 * \param[in]  pending_req_num supported pending request number
 * \param[in]  cb app callback of \ref USB_HID_DRIVER_CB, which will be called after data is sent over
 *
 * \return hid handle
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
void *usb_hid_driver_data_pipe_open(uint8_t ep_addr, T_USB_HID_DRIVER_ATTR attr,
                                    uint8_t pending_req_num, USB_HID_DRIVER_CB cb);

/**
 * usb_hid_driver.h
 *
 * \brief   close hid data pipe
 *
 * \param[in]  handle return value of \ref usb_hid_driver_data_pipe_open
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_hid_driver_data_pipe_close(void *handle);

/**
 * usb_hid_driver.h
 *
 * \brief   hid pipe send data
 *
 * \param[in]  handle return value of \ref usb_hid_driver_data_pipe_open
 * \param[in]  buf data will be sent
 * \param[in]  len length of data
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_data_pipe_send(void *handle, void *buf, uint32_t len);

/**
 * usb_hid_driver.h
 *
 * \brief   register hid driver callbacks to process custom SET_REPORT/GET_REPORT request
 *
 * \param[in]  inst hid instance returned in \ref usb_hid_driver_inst_alloc
 * \param[in]  cbs \ref T_HID_DRIVER_CBS
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_cbs_register(void *inst, T_USB_HID_DRIVER_CBS *cbs);

/**
 * usb_hid_driver.h
 *
 * \brief   unregister hid driver callbacks
 *
 * \param[in]  inst hid instance returned in \ref usb_hid_driver_inst_alloc
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_hid_driver_cbs_unregister(void *inst);

/**
 * usb_hid_driver.h
 *
 * \brief   usb remote wakeup
 *
 * \return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_hid_driver_remote_wakeup(void);

/**
 * usb_hid_driver.h
 *
 * \brief   initalize USB HID interfaces.
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_HID_DRIVER_USAGE
 */
int usb_hid_driver_init(void);

/**
 * usb_hid_driver.h
 *
 * \brief   deinit USB HID interfaces.
 *
 * \return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_hid_driver_deinit(void);

/** @}*/
#endif
