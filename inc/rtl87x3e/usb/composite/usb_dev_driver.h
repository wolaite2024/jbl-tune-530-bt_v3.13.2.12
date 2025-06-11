#ifndef __USB_DEV_DRIVER_H__
#define __USB_DEV_DRIVER_H__
#include <stdint.h>
#include <stdbool.h>


/** @defgroup 87x3e_USB_DEV_Driver USB Dev Driver
  * @brief USB Core driver for app custom application
  * @{
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
 * usb_dev_driver.h
 *
 * \brief   USB device descriptor register.
 *
 * \param[in] desc device descriptor
 *
 */
void usb_dev_driver_dev_desc_register(void *desc);

/**
 * usb_dev_driver.h
 *
 * \brief   USB string descriptor register.
 *
 * \param[in] string string descriptor
 *
 */
void usb_dev_driver_string_desc_register(void *string);

/**
 * usb_dev_driver.h
 *
 * \brief   USB configuration descriptor register.
 *
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 *
 * \param[in] string configuration descriptor
 *
 */
void usb_dev_driver_cfg_desc_register(void *desc);

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
 *
 */
void usb_dev_driver_string_desc_unregister(void);

/**
 * usb_dev_driver.h
 *
 * \brief   USB configuration descriptor unregister.
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 */
void usb_dev_driver_cfg_desc_unregister(void);

/** @}*/
/** End of 87x3e_USB_DEV_Driver
*/

#endif
