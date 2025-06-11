#ifndef _CDC_H_
#define _CDC_H_
#include <stdint.h>
#include <stdbool.h>
#include "usb_cdc_driver.h"

/** @defgroup USB_CDC USB CDC
  * @brief USB cdc data pipe usage
  * @{
  */
#define     CDC_CONGESTION_CTRL_DROP_CUR    CDC_DRIVER_CONGESTION_CTRL_DROP_CUR
#define     CDC_CONGESTION_CTRL_DROP_FIRST  CDC_DRIVER_CONGESTION_CTRL_DROP_FIRST

#define     CDC_BULK_IN_EP                  0x82
#define     CDC_BULK_OUT_EP                 0x02

typedef uint32_t (*USB_CDC_DATA_PIPE_CB)(void *handle, void *buf, uint32_t len, int status);

/**
 * usb_cdc.h
 *
 * \brief   USB CDC pipe attr
 *          \ref zlp: zero length packet
 *          \ref high_throughput: if it is set to 1, it can be be executed in interrupt, else it excute in task.
 *          \ref rsv: reserved
 *          \ref mtu: maximum transfer unit
 *
 * \ingroup USB_CDC
 */
typedef struct _usb_cdc_attr
{
    uint16_t zlp: 1;
    uint16_t high_throughput: 1;
    uint16_t congestion_ctrl: 2;
    uint16_t rsv: 12;
    uint16_t mtu;
} T_USB_CDC_ATTR;

/**
 * usb_cdc.h
 *
 * \brief   open cdc data pipe
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr CDC pipe attribute of \ref T_USB_CDC_ATTR
 * \param[in]  pending_req_num supported pending request number
 * \param[in] cb callback of \ref USB_CDC_DATA_PIPE_CB, which will be called after data is sent over
 *
 * \return handle
 *
 * \ingroup USB_CDC
 */
void *usb_cdc_data_pipe_open(uint8_t ep_addr, T_USB_CDC_ATTR attr, uint8_t pending_req_num,
                             USB_CDC_DATA_PIPE_CB cb);

/**
 * usb_cdc.h
 *
 * \brief   cdc pipe send data
 *
 * \details data is sent serially, which means data is not sent actually until previous data transmission is complete.
 *
 * \param[in]  handle return value of \ref usb_cdc_data_pipe_open
 * \param[in]  buf data will be sent
 * \param[in]  len length of data
 *
 * \return true data will be sent now or after all previous transmissions are complete, false will never be sent
 *
 * \ingroup USB_CDC
 */
bool usb_cdc_data_pipe_send(void *handle, void *buf, uint32_t len);

/**
 * usb_cdc.h
 *
 * \brief   close cdc data pipe
 *
 * \param[in]  handle return value of \ref usb_cdc_data_pipe_open
 *
 * \return int result, refer to "errno.h"
 *
 * \ingroup USB_CDC
 */
int usb_cdc_data_pipe_close(void *handle);

/**
 * usb_cdc.h
 *
 * \brief   usb cdc init
 *
 * \ingroup USB_CDC
 */
void usb_cdc_init(void);

/** @}*/
/** End of USB_CDC
*/

#endif
