#ifndef _USB_CH9_H_
#define _USB_CH9_H_
#include <stdint.h>

#define USB_DT_DEVICE                  0x01
#define USB_DT_CONFIG                  0x02
#define USB_DT_STRING                  0x03
#define USB_DT_INTERFACE               0x04
#define USB_DT_ENDPOINT                0x05
#define USB_DT_DEVICE_QUALIFIER        0x06
#define USB_DT_OTHER_SPEED_CONFIG      0x07
#define USB_DT_INTERFACE_POWER         0x08
#define USB_DT_OTG                     0x09
#define USB_DT_DEBUG                   0x0a
#define USB_DT_INTERFACE_ASSOCIATION   0x0b

#define EP_DIR_OUT                     0x00
#define EP_DIR_IN                      0x80

#define EP_XFER_CONTROL            0x00
#define EP_XFER_ISOC               0x01
#define EP_XFER_BULK               0x02
#define EP_XFER_INT                0x03

#define EP_SYNCTYPE                0x0c
#define EP_SYNC_NONE               (0 << 2)
#define EP_SYNC_ASYNC              (1 << 2)
#define EP_SYNC_ADAPTIVE           (2 << 2)
#define EP_SYNC_SYNC               (3 << 2)

#pragma pack(push,1)
typedef struct _usb_device_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} T_USB_DEVICE_DESC;

typedef struct _usb_config_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} T_USB_CONFIG_DESC;

typedef struct _usb_string_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t wData[1];       /* UTF-16LE encoded */
} T_USB_STRING_DESC;

typedef struct _usb_qualifier_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint8_t  bNumConfigurations;
    uint8_t  bRESERVED;
} T_USB_QUAL_DESC;

typedef struct _usb_interface_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
} T_USB_IF_DESC;

typedef struct _usb_endpoint_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
} T_USB_EP_DESC;

typedef struct _usb_interface_assoc_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bFirstInterface;
    uint8_t  bInterfaceCount;
    uint8_t  bFunctionClass;
    uint8_t  bFunctionSubClass;
    uint8_t  bFunctionProtocol;
    uint8_t  iFunction;
} T_USB_IA_DESC;
#pragma pack(pop)
#endif
