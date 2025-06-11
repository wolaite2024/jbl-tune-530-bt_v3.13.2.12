/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file usb_cdc_driver.h
 * @version 1.0
 * @brief Upper application can used the definitions&APIs to realize CDC function instances.
 *        The driver support support virtual port com to communicate with computer.
 *
 * @note:
 */
#ifndef __USB_CDC_DRIVER_H__
#define __USB_CDC_DRIVER_H__
#include <stdbool.h>
#include <stdint.h>
#include "usb_pipe.h"
/**
 * @addtogroup USB_CDC_DRIVER
 * @{
 * The module mainly supply components to realize USB CDC Class. \n
 * This driver support virtual port com to communicate with computer.
 * @}
  */

/**
 * @addtogroup USB_CDC_DRIVER
 * @{
 * @section USB_CDC_DRIVER_USAGE How to realize USB CDC interface.
 * Firstly, allocate two function instances by \ref usb_cdc_driver_inst_alloc.
 * @par Example
 * @code
 *      void *demo_instance0 = usb_cdc_driver_inst_alloc();
 *      void *demo_instance1 = usb_cdc_driver_inst_alloc();
 * @endcode
 *
 * Then, realize cdc interfaces as follows:
 *  - \b CDC \b Interfaces. \n
 *    - step1: realize descriptors array.
 *    - step2: place cdc requests in queue to ensure that each request can be processed.
 * @par Example  -- USB CDC
 * @code
 *
 *      T_USB_INTERFACE_DESC demo_std_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_HEADER_FUNC_DESC demo_header_desc =
 *      {
 *          ...init...
 *      };
 *      T_CM_FUNC_DESC demo_cm_func_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_ACM_FUNC_DESC demo_acm_func_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_UNION_FUNC_DESC demo_union_desc =
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
 *      T_USB_INTERFACE_DESC demo_std_data_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_in_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_in_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_out_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_out_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      void *demo_if0_descs_fs[] =
 *      {
 *          (void *) &demo_std_if_desc,
 *          (void *) &demo_header_desc,
 *          (void *) &demo_cm_func_desc,
 *          (void *) &demo_acm_func_desc,
 *          (void *) &demo_union_desc,
 *          (void *) &demo_int_in_ep_desc_fs,
 *          NULL,
 *      };
 *      void *demo_if0_descs_hs[] =
 *      {
 *          ...
 *          NULL,
 *      };
 *
 *      void *demo_if1_descs_fs[] =
 *      {
 *          (void *) &demo_std_data_if_desc,
 *          (void *) &demo_bulk_in_ep_desc_fs,
 *          (void *) &demo_bulk_out_ep_desc_fs,
 *          NULL,
 *      };
 *      void *demo_if1_descs_hs[] =
 *      {
 *          ...
 *          NULL,
 *      };
 *
 *      void *demo_data_pipe_open(uint8_t ep_addr, T_USB_CDC_DRIVER_ATTR attr, uint8_t pending_req_num)
 *      {
 *          ...
 *          return usb_cdc_driver_data_pipe_open(ep_addr, attr, pending_req_num);
 *      }
 *      bool demo_data_pipe_send(void *handle, void *buf, uint32_t len, void(*cb)())
 *      {
 *          ...
 *          usb_cdc_driver_data_pipe_send();
 *          return true;
 *      }
 *      int demo_data_pipe_close(void *handle)
 *      {
 *          return usb_cdc_driver_data_pipe_close(handle);
 *      }
 *
 *      usb_cdc_driver_if_desc_register(demo_instance0, (void *)demo_if0_descs_hs,
 *                                      (void *)demo_if0_descs_fs);
 *      usb_cdc_driver_if_desc_register(demo_instance1, (void *)demo_if1_descs_hs,
 *                                      (void *)demo_if1_descs_fs);
 * @endcode
 *
 * Lastly, call \ref usb_cdc_driver_init to initialize usb cdc driver.
 */
/** @}*/

/**
 * @addtogroup USB_CDC_DRIVER
 * @{
 * @section Definitions
 */

/**
 * usb_cdc_driver.h
 *
 * \brief   congestion control
 *
 * \details If CDC_DRIVER_CONGESTION_CTRL_DROP_CUR is set, current data to send
 *          will be dropped, else the first data in the queue will be dropped.
 *
 * \note    Only effective for in endpoint.
 *
 */
#define CDC_DRIVER_CONGESTION_CTRL_DROP_CUR     USB_PIPE_CONGESTION_CTRL_DROP_CUR
#define CDC_DRIVER_CONGESTION_CTRL_DROP_FIRST   USB_PIPE_CONGESTION_CTRL_DROP_FIRST

/**
 * usb_cdc_driver.h
 *
 * \brief   USB CDC pipe attr
 *
 */
typedef T_USB_PIPE_ATTR T_USB_CDC_DRIVER_ATTR;

/**
 * usb_cdc_driver.h
 *
 * \brief   USB CDC callback
 *
 */
typedef USB_PIPE_CB USB_CDC_DRIVER_CB;

/**
 * @brief alloc cdc function instance, which has the standalone interface
 *
 * @return void*  cdc function instance
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void *usb_cdc_driver_inst_alloc(void);

/**
 * @brief free cdc function instance, alloacted by \ref usb_cdc_driver_inst_alloc
 *
 * @param inst, instance alloacted by \ref usb_cdc_driver_inst_alloc
 * @return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_inst_free(void *inst);

/**
 * usb_cdc_driver.h
 *
 * \brief   CDC interface descriptor register
 *
 * \param[in]  inst cdc instance returned in \ref usb_cdc_driver_inst_alloc
 * \param[in]  hs_desc CDC interface descriptor of high speed
 * \param[in]  fs_desc CDC interface descriptor of full speed
 *
 * \return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_if_desc_register(void *inst, void *hs_desc, void *fs_desc);

/**
 * usb_cdc_driver.h
 *
 * \brief   CDC interface descriptor unregister
 *
 * \param inst cdc instance returned in \ref usb_cdc_driver_inst_alloc
 *
 * \return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_if_desc_unregister(void *inst);

/**
 * usb_cdc_driver.h
 *
 * \brief   open cdc data pipe
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr CDC pipe attribute of \ref T_USB_CDC_DRIVER_ATTR
 * \param[in]  pending_req_num supported pending request number
 * \param[in]  cb app callback of \ref USB_CDC_DRIVER_CB, which will be called after data is sent over
 *
 * \return cdc handle
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void *usb_cdc_driver_data_pipe_open(uint8_t ep_addr, T_USB_CDC_DRIVER_ATTR attr,
                                    uint8_t pending_req_num, USB_CDC_DRIVER_CB cb);

/**
 * usb_cdc_driver.h
 *
 * \brief   close cdc data pipe
 *
 * \param[in]  handle return value of \ref usb_cdc_driver_data_pipe_open
 *
 * \return int result, refer to "errno.h"
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_data_pipe_close(void *handle);

/**
 * usb_cdc_driver.h
 *
 * \brief   cdc pipe send data
 *
 * \param[in]  handle return value of \ref usb_cdc_driver_data_pipe_open
 * \param[in]  buf data will be sent
 * \param[in]  len length of data
 *
 * \return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_data_pipe_send(void *handle, void *buf, uint32_t len);

/**
 * usb_cdc_driver.h
 *
 * \brief   initalize USB CDC interfaces.
 *
 * \return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE
 *
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_init(void);

/**
 * usb_cdc_driver.h
 *
 * \brief   deinit USB CDC interfaces.
 *
 * \return int result, refer to "errno.h"
 * \note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_cdc_driver_deinit(void);

/** @}*/
#endif
