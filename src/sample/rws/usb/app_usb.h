#ifndef __APP_USB_H__
#define __APP_USB_H__
#include <stdbool.h>
#include "app_io_msg.h"

/** @defgroup APP_USB APP USB
  * @brief app usb module.
  * @{
  */

/**
 * app_usb.h
 *
 * \brief   handle USB-related message.
 *
 * \ingroup APP_USB
 */
void app_usb_msg_handle(T_IO_MSG *msg);

/**
 * app_usb.h
 *
 * \brief   Judge if USB is connected
 *
 * \details usb_dm_start will start USB task. USB connection state depends on if USB task is already started
 *
 * \ingroup APP_USB
 */
bool app_usb_connected(void);

/**
 * app_usb.h
 *
 * \brief   app usb init
 *
 * \ingroup APP_USB
 */
void app_usb_init(void);

/** @}*/
/** End of APP_USB
*/

#endif
