#ifndef __HID_H__
#define __HID_H__

/*
 * HID brequest field value
 */
#define HID_REQ_GET_REPORT              0x01
#define HID_REQ_GET_IDLE                0x02
#define HID_REQ_GET_PROTOCOL            0x03
#define HID_REQ_SET_REPORT              0x09
#define HID_REQ_SET_IDLE                0x0A
#define HID_REQ_SET_PROTOCOL            0x0B

#define HID_DT_HID                      (USB_TYPE_CLASS | 0x01)
#define HID_DT_REPORT                   (USB_TYPE_CLASS | 0x02)

#pragma pack(push,1)
typedef struct _hid_class_descriptor
{
    uint8_t  bDescriptorType;
    uint16_t wDescriptorLength;
} T_HID_CLASS_DESC;

typedef struct _hid_cs_if_desc
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;

    T_HID_CLASS_DESC desc[1];
} T_HID_CS_IF_DESC;
#pragma pack(pop)


#endif
