#ifndef __USB_DEV_DRIVER_H__
#define __USB_DEV_DRIVER_H__
#include <stdint.h>
#include <stdbool.h>
#include "usb_spec20.h"

/** @defgroup 87x3d_USB_DEV_Driver USB Dev Driver
  * @brief app usb module.
  * @{
  */

/**
 * @addtogroup 87x3d_USB_DEV_Driver
 * @{
 *
 * @section USB_dev_driver_Usage_Chapter_Vendor How to use vendor callback.
 * @details How to use vendor callback, it will notify to app.
 * @par Example
 * @code
 *      #include "usb_dev_driver.h"

 *      int demo_get_vendor(uint16_t request_cmd, void *buf, uint16_t len)
 *      {
 *          //Process memcpy data to buf
 *      }
 *
 *      int demo_set_vendor(uint16_t request_cmd, void *buf, uint16_t len)
 *      {
 *          //Process set vendor
 *      }
 *
 *      T_USB_DEV_VENDOR_CBS demo_cbs =
 *      {
 *          .get = demo_get_vendor,
 *          .set = demo_set_vendor,
 *      };
 *      usb_dev_driver_vendor_cbs_register(&demo_cbs);

 *@endcode
 * @}
 */

/**
 * usb_dev_driver.h
 *
 * \brief   usb device-related strings.
 *
 * \param id string id defined in device descriptor
 * \param s  actual string matched with \ref id
 *
 */
typedef struct _string
{
    uint8_t          id;
    const char      *s;
} T_STRING;

/**
 * usb_dev_driver.h
 *
 * \brief   usb device-related string table.
 *
 * \param language language id used by strings
 * \param strings  string set \ref T_STRING
 *
 */
typedef struct _string_tab
{
    uint16_t language;
    T_STRING *strings;
} T_STRING_TAB;

/**
 *
 * @brief   USB dev vendor callback
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
typedef struct _usb_dev_vendor_cbs
{
    INT_VENDOR_FUNC   get;
    INT_VENDOR_FUNC   set;
} T_USB_DEV_VENDOR_CBS;

/**
 * usb_dev_driver.h
 *
 * \brief   USB device descriptor register.
 *
 * \param[in] desc device descriptor
 *
 */
void usb_dev_driver_dev_desc_register(T_USB_DEVICE_DESC *desc);

/**
 * usb_dev_driver.h
 *
 * \brief   USB string descriptor register.
 *
 * \param[in] string_tbl string descriptor table
 *
 */
void usb_dev_driver_string_desc_register(T_STRING_TAB *string_tbl[]);

/**
 * usb_dev_driver.h
 *
 * \brief   USB configuration descriptor register.
 *
 * \param[in] desc configuration descriptor
 *
 */
void usb_dev_driver_cfg_desc_register(T_USB_CONFIG_DESC *desc);


/**
 * usb_dev_driver.h
 *
 * \brief   USB device descriptor unregister.
 *
 */
void usb_dev_driver_dev_desc_unregister(void);

/**
 * usb_dev_driver.h
 *
 * \brief   USB string descriptor unregister.
 * \param[in] string_tbl string descriptor table
 *
 */
void usb_dev_driver_string_desc_unregister(T_STRING_TAB *string_tbl[]);

/**
 * usb_dev_driver.h
 *
 * \brief   USB configuration descriptor unregister.
 * \param[in] desc configuration descriptor
 *
 */
void usb_dev_driver_cfg_desc_unregister(T_USB_CONFIG_DESC *desc);

/**
 * usb_dev_driver.h
 *
 * @brief   USB device driver vendor callbacks register.
 *
 * \param[in] USB device driver vendor callbacks, ref to \ref T_USB_DEV_VENDOR_CBS
 * @par Example
 * Please refer to \ref USB_dev_driver_Usage_Chapter_Vendor
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */

void usb_dev_driver_vendor_cbs_register(T_USB_DEV_VENDOR_CBS *cbs);

/**
 * usb_dev_driver.h
 *
 * @brief   USB device driver vendor callbacks unregister.
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */

void usb_dev_driver_vendor_cbs_unregister(void);

/**
 * usb_dev_driver.h
 *
 * @brief   USB device driver control transfer buffer len initialization.
 *
 * \param[in] buf_len control transfer buffer len
 *
 * \note The default control transfer buffer length is 512. If need a larger buffer,
 *          call this API to increase the buffer size during the initialization phase.
 *          This api must be called before \ref usb_dev_driver_dev_desc_register.
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void usb_dev_driver_ctrl_xfer_buf_init(uint16_t buf_len);

/** @}*/
/** End of 87x3d_USB_DEV_Driver
*/
#endif
