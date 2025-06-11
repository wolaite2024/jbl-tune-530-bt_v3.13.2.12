
#ifndef _USB_SPEC20_
#define _USB_SPEC20_

#include <stdint.h>


/*
 * The USB records contain some unaligned little-endian word
 * components.  The U[SG]ETW macros take care of both the alignment
 * and endian problem and should always be used to access non-byte
 * values.
 */
#define UGETW(w) (w)


/*-------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)

//enum usb_device_speed { USB_SPEED_UNKNOWN = 0,  USB_SPEED_LOW, USB_SPEED_FULL,  USB_SPEED_HIGH};

/*=====================================================*/

#define UE_ADDR     0x0f
#define UE_GET_ADDR(a)  ((a) & UE_ADDR)
#define USB_CLASS_AUDIO         1
#define USB_CLASS_COMM          2
#define USB_CLASS_HID           3
#define USB_CLASS_MSC           8
#define USB_CLASS_CDC_DATA      0x0a
#define USB_CLASS_WIRELESS_CONTROLLER   0xe0
#define USB_CLASS_VENDOR_SPEC    0xff



// USB spec. Table 9-2
#define UE_GET_DIR(a)   ((a) & 0x80)
#define USB_DIR_MASK    0x80
/* directions : D7 */
#define USB_DIR_MASK        0x80
#define USB_DIR_OUT         0               /* Host to Device */
#define USB_DIR_IN          0x80        /* Device to Host */
/* Type : D6..5*/
#define UT_GET_TYPE(a) ((a) & 0x60)
#define USB_TYPE_MASK           (0x03 << 5)
#define USB_TYPE_STANDARD       (0x00 << 5)
#define USB_TYPE_CLASS          (0x01 << 5)
#define USB_TYPE_VENDOR         (0x02 << 5)
#define USB_TYPE_RESERVED       (0x03 << 5)
/* Recipients D 4..0 */
#define UT_GET_RECIPIENT(a) ((a) & 0x1f)
#define USB_RECIP_MASK          0x1f
#define USB_RECIP_DEVICE      0x00
#define USB_RECIP_INTERFACE   0x01
#define USB_RECIP_ENDPOINT    0x02

// Spec Table 9-4
/* Standard requests */
#define USB_REQ_CODE_GET_STATUS          0x00
#define USB_REQ_CODE_CLEAR_FEATURE       0x01
#define USB_REQ_CODE_SET_FEATURE         0x03
#define USB_REQ_CODE_SET_ADDRESS         0x05
#define USB_REQ_CODE_GET_DESCRIPTOR      0x06
#define USB_REQ_CODE_SET_DESCRIPTOR      0x07
#define USB_REQ_CODE_GET_CONFIGURATION   0x08
#define USB_REQ_CODE_SET_CONFIGURATION   0x09
#define USB_REQ_CODE_GET_INTERFACE       0x0A
#define USB_REQ_CODE_SET_INTERFACE       0x0B
#define USB_REQ_CODE_SYNCH_FRAME         0x0C

// Table 9-5 and IAD ECN */
/* Descriptor types */
#define USB_DESC_TYPE_DEVICE        0x01
#define USB_DESC_TYPE_CONFIG        0x02
#define USB_DESC_TYPE_STRING        0x03
#define USB_DESC_TYPE_INTERFACE     0x04
#define USB_DESC_TYPE_ENDPOINT      0x05
#define USB_DESC_TYPE_DEVICE_QUALIFIER     0x06
#define USB_DESC_TYPE_OTHER_SPEED_CONFIG   0x07
#define USB_DESC_TYPE_IAD   0x0B

#define USB_DESCTYPE_CLASS_INTERFACE     ((USB_TYPE_CLASS)|(USB_DESC_TYPE_INTERFACE))

/* Table 9-6 */
/* Feature numbers */
#define UF_ENDPOINT_HALT    0
#define UF_DEVICE_REMOTE_WAKEUP 1
#define UF_TEST_MODE        2

#define UF_TEST_MODE_SELECTOR_TEST_J            (0x01)
#define UF_TEST_MODE_SELECTOR_TEST_K            (0x02)
#define UF_TEST_MODE_SELECTOR_TEST_SE0_NAK      (0x03)
#define UF_TEST_MODE_SELECTOR_TEST_PACKET       (0x04)
#define UF_TEST_MODE_SELECTOR_TEST_FORCE_ENABLE (0x05)

// USB spec. Table 9-10
/* from config descriptor bmAttributes */
#define USB_ATTRIBUTE_ALWAYS_ONE     (1 << 7)    /* must be set */
#define USB_ATTRIBUTE_SELFPOWERED    (1 << 6)    /* self-powered */
#define USB_ATTRIBUTE_REMOTEWAKEUP   (1 << 5)    /* support remote wakeup */
#define USB_ATTRIBUTE_BATTERY           (1 << 4)    /* battery powered */

// USB spec. Table 9-13
/* Endpoint bmAttributes */
#define USB_EP_TYPE_MASK 0x03 /* in bmAttributes */
#define USB_EP_TYPE_CONTROL  0
#define USB_EP_TYPE_ISOC     1
#define USB_EP_TYPE_BULK     2
#define USB_EP_TYPE_INT      3

#define USB_EP_SYNCTYPE                0x0c
#define USB_EP_SYNC_NONE               (0 << 2)
#define USB_EP_SYNC_ASYNC              (1 << 2)
#define USB_EP_SYNC_ADAPTIVE           (2 << 2)
#define USB_EP_SYNC_SYNC               (3 << 2)

#define USB_EP_USAGE_MSK               (0b11 << 4)
#define USB_EP_USAGE_DATA              (0 << 4)
#define USB_EP_USAGE_FEEDBACK          (1 << 4)
#define USB_EP_USAGE_IMPLICIT_FEEDBACK (2 << 4)

typedef struct _usb_device_request
{
    uint8_t       bmRequestType;
    uint8_t       bRequest;
    uint16_t       wValue;
    uint16_t       wIndex;
    uint16_t       wLength;
} T_USB_DEVICE_REQUEST;

/* All standard descriptors have these 2 fields at the beginning */
// class.c, config_rom. composite_rom, usb_gadget, composite.h
typedef struct usb_descriptor_header
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
} T_USB_DESC_HDR;

typedef struct _usb_endpoint_descriptor
{
    uint8_t       bLength;
    uint8_t       bDescriptorType;
    uint8_t       bEndpointAddress;
    uint8_t       bmAttributes;
    uint16_t      wMaxPacketSize;
    uint8_t       bInterval;
} T_USB_ENDPOINT_DESC;

/*-------------------------------------------------------------------------*/
/* USB_DT_DEVICE: Device descriptor */
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

/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters.
 */

/*-------------------------------------------------------------------------*/
/* Interface descriptor */
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
} T_USB_INTERFACE_DESC;

// for USB 2.0
/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
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
} T_USB_QUALIFIER_DESC;

/*-------------------------------------------------------------------------*/
// IAD
/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
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
} T_USB_INTERFACE_ASSOC_DESC;

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
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wLANGID[];
} T_USB_STRING_DESC;


#pragma pack(pop)
#endif /*#if !defined(_USB_SPEC20_RTK_) */
