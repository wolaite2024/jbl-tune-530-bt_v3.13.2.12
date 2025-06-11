#if F_APP_USB_AUDIO_SUPPORT|F_APP_USB_HID_SUPPORT
#include <stddef.h>
#include "usb_dev_driver.h"

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#define IAD_SUPPORT     1
#else
#define IAD_SUPPORT     0
#endif
typedef enum
{
    STRING_ID_UNDEFINED,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    STRING_ID_SERIALNUM,
} T_STRING_ID;


static T_USB_DEVICE_DESC usb_dev_desc =
{
    .bLength                = sizeof(T_USB_DEVICE_DESC),
    .bDescriptorType        = USB_DESC_TYPE_DEVICE,

    .bcdUSB                 = 0x0200,
#if (IAD_SUPPORT == 1)
    .bDeviceClass           = 0xEF,
    .bDeviceSubClass        = 0x02,
    .bDeviceProtocol        = 0x01,
#else
    .bDeviceClass           = 0x00,
    .bDeviceSubClass        = 0x00,
    .bDeviceProtocol        = 0x00,
#endif
    .bMaxPacketSize0        = 0x40,
    .idVendor               = 0x0bda,
    .idProduct              = 0x8773,
    .bcdDevice              = 0x0200,
    .iManufacturer          = STRING_ID_MANUFACTURER,
    .iProduct               = STRING_ID_PRODUCT,
    .iSerialNumber          = STRING_ID_SERIALNUM,
    .bNumConfigurations     = 1,
};

static T_STRING dev_strings[] =
{
    [0] =
    {
        .id     = STRING_ID_MANUFACTURER,
        .s = "RealTek",
    },

    [1] =
    {
        .id     = STRING_ID_PRODUCT,
        .s = "USB Audio Device",
    },

    [2] =
    {
        .id     = STRING_ID_SERIALNUM,
        .s = "0123456789A",
    },

    [3] =
    {
        .id     = STRING_ID_UNDEFINED,
        .s = NULL,
    },
};

T_STRING_TAB dev_stringtab =
{
    .language = 0x0409,
    .strings = dev_strings,
};

const T_STRING_TAB *const dev_stringtabs[] =
{
    [0] = &dev_stringtab,
    [1] = NULL,
};

static T_USB_CONFIG_DESC usb_cfg_desc =
{
    .bLength = sizeof(T_USB_CONFIG_DESC),
    .bDescriptorType = USB_DESC_TYPE_CONFIG,
    .wTotalLength = 0xFFFF, //wTotalLength will be recomputed in usb lib according total interface descriptors
    .bNumInterfaces = 3, //bNumInterfaces will be recomputed in usb lib according total interface num
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x32
};

void usb_dev_init(void)
{
    usb_dev_driver_dev_desc_register((void *)&usb_dev_desc);
    usb_dev_driver_cfg_desc_register((void *)&usb_cfg_desc);
    usb_dev_driver_string_desc_register((void *)dev_stringtabs);
}
#endif
