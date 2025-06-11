#ifndef __USB_PIPE_H__
#define __USB_PIPE_H__
#include <stdint.h>
#include "usb_spec20.h"

/**
 * @addtogroup USB_PIPE
 * @{
 * The module mainly supply common pipe operations for interrupt and bulk endpoint. \n
 * This driver support data pipe open, send and receive. \n
 * @}
  */

/**
 * @addtogroup USB_PIPE
 * @{
 * @section USB_PIPE_USAGE How to use USB pipe.
 * Firstly, open data pipe by \ref usb_pipe_open.
 * @par Example
 * @code
 *      // open in ep
 *      T_USB_HID_ATTR in_attr =
 *      {
 *          .zlp = 1,
 *          .high_throughput = 0,
 *          .congestion_ctrl = USB_PIPE_CONGESTION_CTRL_DROP_CUR,
 *          .rsv = 0,
 *          .mtu = HID_MAX_TRANSMISSION_UNIT
 *      };
 *      void *demo_in_handle = usb_pipe_open(in_desc, in_ep_addr, in_attr, in_pending_req_num, in_cb);
 *      // open out ep, if you want to usb out ep, you must open out ep in initailization
 *      // phase before usb enumeration, because alt_set funciton will call \ref usb_pipe_recv.
 *      T_USB_HID_ATTR out_attr =
 *      {
 *          .zlp = 0,
 *          .high_throughput = 0,
 *          .congestion_ctrl = USB_PIPE_CONGESTION_CTRL_DROP_CUR,
 *          .rsv = 0,
 *          .mtu = HID_MAX_TRANSMISSION_UNIT
 *      };
 *      void *demo_out_handle = usb_pipe_open(out_desc, in_ep_addr, out_attr, out_pending_req_num, out_cb);
 * @endcode
 *
 * Then, send data by \ref usb_pipe_send.
 * @par Example
 * @code
 *      usb_hid_data_pipe_send(demo_in_handle, data, length);
 * @endcode
 */
/** @}*/

/**
 * @addtogroup USB_PIPE
 * @{
 * @section Definitions
 */

/**
 * usb_pipe.h
 *
 * \brief   congestion control
 *
 * \details If CONGESTION_CTRL_DROP_CUR is set, current data to send will be dropped,
 *          else the first data in the queue will be dropped.
 *
 * \note    Only effective for in endpoint.
 *
 */
#define USB_PIPE_CONGESTION_CTRL_DROP_CUR     (0)
#define USB_PIPE_CONGESTION_CTRL_DROP_FIRST   (1)

/**
 * usb_pipe.h
 *
 * \brief   USB pipe attr
 *          \ref zlp: zero length packet
 *          \ref high_throughput: if it is set to 1, it can be be executed in interrupt, else it excute in task.
 *          \ref congestion_ctrl: if it is set to 0, drop current data, else drop the first data in the queue.
 *          \ref rsv: reserved
 *          \ref mtu: maximum transfer unit
 *
 *  \note   congestion_ctrl only effective for in endpoint.
 * \ingroup USB_PIPE
 */
typedef struct _usb_pipe_attr
{
    uint16_t zlp: 1;
    uint16_t high_throughput: 1;
    uint16_t congestion_ctrl: 2;
    uint16_t rsv: 12;
    uint16_t mtu;
} T_USB_PIPE_ATTR;

typedef uint32_t (*USB_PIPE_CB)(void *handle, void *buf, uint32_t len, int status);

/**
 * usb_pipe.h
 *
 * \brief   open data pipe
 *
 * \param[in]  desc ep descriptor
 * \param[in]  ep_addr ep address
 * \param[in]  attr pipe attribute of \ref T_USB_PIPE_ATTR
 * \param[in]  pending_req_num supported pending request number
 * \param[in]  cb app callback of \ref USB_PIPE_CB, which will be called after data is sent over
 *
 * \return handle
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
void *usb_pipe_open(T_USB_ENDPOINT_DESC **desc, uint8_t ep_addr, T_USB_PIPE_ATTR attr,
                    uint8_t pending_req_num, USB_PIPE_CB cb);

/**
 * usb_pipe.h
 *
 * \brief   close data pipe
 *
 * \param[in]  handle return value of \ref usb_pipe_open
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
int usb_pipe_close(void *handle);

/**
 * usb_pipe.h
 *
 * \brief   pipe send data
 *
 * \details data is sent serially, which means data is not sent actually until previous data transmission is complete.
 *
 * \param[in]  handle return value of \ref usb_pipe_open
 * \param[in]  buf data will be sent
 * \param[in]  len length of data
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
int usb_pipe_send(void *handle, void *buf, uint32_t len);

/**
 * usb_pipe.h
 *
 * \brief   pipe receive data
 *
 * \param[in]  handle return value of \ref usb_pipe_open
 *
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
int usb_pipe_recv(void *handle);

/**
 * \brief stall USB pipe, then all data transfer through the pipe will be STALLED
 *
 * \param[in] handle return value of \ref usb_pipe_open
 * \param[in] err errno leads to stall pipe
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
int usb_pipe_stall_set(void *handle, int err);

/**
 * \brief clear pipe stall status set by \ref usb_pipe_stall_set
 *
 * \param[in] handle return value of \ref usb_pipe_open
 * \return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
int usb_pipe_stall_clear(void *handle);

/**
 * \brief get usb pipe attribute
 *
 * \param[in] handle return value of \ref usb_pipe_open
 * \return T_USB_PIPE_ATTR* pipe attribute
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * \ingroup USB_PIPE
 */
T_USB_PIPE_ATTR *usb_pipe_attr_get(void *handle);

/** @}*/
/** End of USB_PIPE
*/

#endif
