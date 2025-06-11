/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "app_cfg.h"
#include "bt_types.h"
#include "bt_sdp.h"
#include "btm.h"
#include "gap_br.h"
#include "app_sdp.h"
#include "app_main.h"

#if BISTO_FEATURE_SUPPORT
#include "bisto_bt_uuid.h"
#endif

#if AMA_FEATURE_SUPPORT
#include "app_ama_transport.h"
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_transport.h"
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "app_xiaowei_transport.h"
#endif

#if DMA_FEATURE_SUPPORT
#include "app_dma_rfc.h"
#endif

#include "app_avrcp.h"

#include "app_report.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define REPORT_ID_USB_UTILITY_1 (42)
#define REPORT_ID_USB_UTILITY_3 (44)
#define REPORT_ID_USB_UTILITY_4 (45)

#define REPORT_ID_USB_UTILITY_5 (0x24)
#define REPORT_ID_USB_UTILITY_6 (0x27)

#define REPORT_ID_USB_UTILITY_7 (0x39)

#if F_APP_AVRCP_CMD_SUPPORT
#define AVRCP_BROWSING_FEATURE_SUPPORT 1
#endif

static const uint8_t did_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4D,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PNP_INFORMATION >> 8),
    (uint8_t)(UUID_PNP_INFORMATION),

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PNP_INFORMATION >> 8),
    (uint8_t)UUID_PNP_INFORMATION,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.3
    0x03,

    //attribute SDP_ATTR_DIP_SPECIFICATION_ID
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_SPECIFICATION_ID >> 8),
    (uint8_t)SDP_ATTR_DIP_SPECIFICATION_ID,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,
    0x03,

    //attribute SDP_ATTR_DIP_VENDOR_ID
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_VENDOR_ID >> 8),
    (uint8_t)SDP_ATTR_DIP_VENDOR_ID,
    SDP_UNSIGNED_TWO_BYTE,
    //0x00,//0x005D : RealTek
    //0x5D,
    (uint8_t)(RTK_COMPANY_ID >> 8),
    (uint8_t)RTK_COMPANY_ID,

    //attribute SDP_ATTR_DIP_PRODUCT_ID
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_PRODUCT_ID >> 8),
    (uint8_t)SDP_ATTR_DIP_PRODUCT_ID,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PRODUCT_ID >> 8),
    (uint8_t)PRODUCT_ID,

    //attribute SDP_ATTR_DIP_PRODUCT_VERSION
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_PRODUCT_VERSION >> 8),
    (uint8_t)SDP_ATTR_DIP_PRODUCT_VERSION,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PRODUCT_VERSION >> 8),
    (uint8_t)PRODUCT_VERSION,

    //attribute SDP_ATTR_DIP_PRIMARY_RECORD
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_PRIMARY_RECORD >> 8),
    (uint8_t)SDP_ATTR_DIP_PRIMARY_RECORD,
    SDP_BOOL_ONE_BYTE,
    true,

    //attribute SDP_ATTR_DIP_VENDOR_ID_SOURCE
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_DIP_VENDOR_ID_SOURCE >> 8),
    (uint8_t)SDP_ATTR_DIP_VENDOR_ID_SOURCE,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(VENDOR_ID_SOURCE >> 8),
    (uint8_t)VENDOR_ID_SOURCE,
};

#if F_APP_HID_SUPPORT
#if F_APP_HID_MOUSE_SUPPORT
static const uint8_t hid_mouse_sdp_record[] =
{
    //Total length
    SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    0x01,
    0x00,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_CONTROL >> 8),
    (uint8_t)(PSM_HID_CONTROL),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)(SDP_LANG_ENGLISH),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)(SDP_CHARACTER_UTF8),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)(SDP_BASE_LANG_OFFSET),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0101 >> 8),
    (uint8_t)(0x0101),

    //Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_ADD_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0F,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_INTERRUPT >> 8),
    (uint8_t)(PSM_HID_INTERRUPT),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', ' ', 'H', 'I', 'D',

    //Attribute SDP_ATTR_SRV_DESC
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x10,
    '(', '3', ')', ' ', 'B', 'u', 't', 't', 'o', 'n', ' ', 'M', 'o', 'u', 's', 'e',

    //Attribute SDP_ATTR_PROVIDER_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x07,
    'R', 'e', 'a', 'l', 't', 'e', 'k',

    //Attribute HIDParserVersion
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0201 >> 8),
    (uint8_t)(0x0201),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0111 >> 8),
    (uint8_t)(0x0111),

    //Attribute HIDDeviceSubclass
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0202 >> 8),
    (uint8_t)(0x0202),
    SDP_UNSIGNED_ONE_BYTE,
    0x80,

    //Attribute HIDCountryCode
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0203 >> 8),
    (uint8_t)(0x0203),
    SDP_UNSIGNED_ONE_BYTE,
    0x21,

    //Attribute HIDVirtualCable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0204 >> 8),
    (uint8_t)(0x0204),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDReconnectInitiate
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0205 >> 8),
    (uint8_t)(0x0205),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDDescriptorList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0206 >> 8),
    (uint8_t)(0x0206),
    SDP_DATA_ELEM_SEQ_HDR,
    //0x38,
    0x46,
    SDP_DATA_ELEM_SEQ_HDR,
    //0x36,
    0x44,
    SDP_UNSIGNED_ONE_BYTE,
    0x22,                   /* Type = Report Descriptor */
    SDP_STRING_HDR,
    //0x32,
    0x40,
    0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x02,             /* USAGE (Mouse) */
    0xA1, 0x01,             /* COLLECTION (Application) */
    0x85, 0x02,             /* REPORT_ID         (2)           */
    0x09, 0x01,             /*   USAGE (Pointer) */
    0xA1, 0x00,             /*   COLLECTION (Physical) */
    0x05, 0x09,             /*     USAGE_PAGE (Button) */
    0x19, 0x01,             /*     USAGE_MINIMUM (Button 1) */
    0x29, 0x03,             /*     USAGE_MAXIMUM (Button 3) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x95, 0x03,             /*     REPORT_COUNT (3) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x05,             /*     REPORT_SIZE (5) */
    0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */
    0x05, 0x01,             /*     USAGE_PAGE (Generic Desktop) */
    0x09, 0x30,             /*     USAGE (X) */
    0x09, 0x31,             /*     USAGE (Y) */
    0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x95, 0x02,             /*     REPORT_COUNT (2) */
    0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
    0x09, 0x38,             /*     USAGE (Wheel) */
    0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
    0xC0,                   /*   END_COLLECTION */
    0xC0,                   /* END_COLLECTION */

    //Attribute HIDLANGIDBaseList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0207 >> 8),
    (uint8_t)(0x0207),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0409 >> 8),
    (uint8_t)(0x0409),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),

    //Attribute HIDBatteryPower
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0209 >> 8),
    (uint8_t)(0x0209),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDRemoteWake
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020A >> 8),
    (uint8_t)(0x020A),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDNormallyConnectable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020D >> 8),
    (uint8_t)(0x020D),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDBootDevice
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020E >> 8),
    (uint8_t)(0x020E),
    SDP_BOOL_ONE_BYTE,
    0x01,
};
#endif

#if F_APP_HID_KEYBOARD_SUPPORT
static const uint8_t hid_keyboard_sdp_record[] =
{
    //Total length
    SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    0x01,
    0x53,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_CONTROL >> 8),
    (uint8_t)(PSM_HID_CONTROL),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)(SDP_LANG_ENGLISH),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)(SDP_CHARACTER_UTF8),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)(SDP_BASE_LANG_OFFSET),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0101 >> 8),
    (uint8_t)(0x0101),

    //Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_ADD_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0F,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_INTERRUPT >> 8),
    (uint8_t)(PSM_HID_INTERRUPT),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', ' ', 'H', 'I', 'D',

    //Attribute SDP_ATTR_SRV_DESC
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x10,
    '(', '3', ')', ' ', 'B', 'u', 't', 't', 'o', 'n', ' ', 'M', 'o', 'u', 's', 'e',

    //Attribute SDP_ATTR_PROVIDER_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x07,
    'R', 'e', 'a', 'l', 't', 'e', 'k',

    //Attribute HIDParserVersion
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0201 >> 8),
    (uint8_t)(0x0201),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0111 >> 8),
    (uint8_t)(0x0111),

    //Attribute HIDDeviceSubclass
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0202 >> 8),
    (uint8_t)(0x0202),
    SDP_UNSIGNED_ONE_BYTE,
    0x40,

    //Attribute HIDCountryCode
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0203 >> 8),
    (uint8_t)(0x0203),
    SDP_UNSIGNED_ONE_BYTE,
    0x21,

    //Attribute HIDVirtualCable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0204 >> 8),
    (uint8_t)(0x0204),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDReconnectInitiate
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0205 >> 8),
    (uint8_t)(0x0205),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDDescriptorList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0206 >> 8),
    (uint8_t)(0x0206),
    SDP_DATA_ELEM_SEQ_HDR,
    //0x47,
    0x99,
    SDP_DATA_ELEM_SEQ_HDR,
    //0x45,
    0x97,
    SDP_UNSIGNED_ONE_BYTE,
    0x22,                   /* Type = Report Descriptor */
    SDP_STRING_HDR,
    0x93,
    0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x06,             /* USAGE (Keyboard) */
    0xA1, 0x01,             /* COLLECTION (Application) */
    0x85, 0x01,             /*     REPORT_ID         (1)           */
    0x95, 0x08,             /*     REPORT_COUNT (8) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x05, 0x07,             /*     USAGE (Keyboard/Keypad) */
    0x19, 0xe0,             /*     USAGE_MINIMUM (Keyboard Left Control) */
    0x29, 0xe7,             /*     USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */

    0x95, 0x05,             /*     REPORT_COUNT (5) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x05, 0x08,             /*     USAGE_PAGE (LEDs) */
    0x19, 0x01,             /*     USAGE_MINIMUM (Num Lock) */
    0x29, 0x05,             /*     USAGE_MAXIMUM (Kana) */
    0x91, 0x02,             /*     OUTPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x03,             /*     REPORT_SIZE (3) */
    0x91, 0x03,             /*     OUTPUT (Cnst,Var,Abs) */

    0x95, 0x06,             /*     REPORT_COUNT (6) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0xff,             /*     LOGICAL_MAXIMUM (255) */
    0x05, 0x07,             /*     USAGE_PAGE (Keyboard/Keypad) */
    0x19, 0x00,             /*     USAGE_MINIMUM (Keyboard Return (ENTER)) */
    0x29, 0xff,             /*     USAGE_MAXIMUM (Keyboard ESCAPE) */
    0x81, 0x00,             /*     INPUT (Data,Arr,Abs) */
    0xC0,                   /* END_COLLECTION */

    0x05, 0x0c,             /* USAGE_PAGE (Consumer) */
    0x09, 0x01,             /* USAGE (Consumer Control) */
    0xA1, 0x01,             /* COLLECTION (Application) */
    0x85, 0x02,             /*     REPORT_ID         (2)           */
    0x95, 0x18,             /*     REPORT_COUNT (24) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x0a, 0x23, 0x02,       /*     USAGE (AL Home) */
    0x0a, 0x21, 0x02,       /*     USAGE (AC Search) */
    0x0a, 0x8a, 0x01,       /*     USAGE (AL Email Reader) */
    0x0a, 0x96, 0x01,       /*     USAGE (AL Internet Browser) */
    0x0a, 0x9e, 0x01,       /*     USAGE (AL Terminal Lock/Screensaver) */
    0x0a, 0x01, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x02, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x05, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x06, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x07, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x08, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x0a, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x0b, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0xae, 0x01,       /*     USAGE (AL Keyboard Layboard) */
    0x0a, 0xb1, 0x01,       /*     USAGE (AL Screen Saver) */
    0x09, 0x40,             /*     USAGE (Menu) */
    0x09, 0x30,             /*     USAGE (Power) */
    0x09, 0xb7,             /*     USAGE (Stop) */
    0x09, 0xb6,             /*     USAGE (Scan Previous Track) */
    0x09, 0xcd,             /*     USAGE (Play/Pause) */
    0x09, 0xb5,             /*     USAGE (Scan Next Track) */
    0x09, 0xe2,             /*     USAGE (Mute) */
    0x09, 0xea,             /*     USAGE (Volume Down) */
    0x09, 0xe9,             /*     USAGE (Volume Up) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0xC0,                   /* END_COLLECTION */

    //Attribute HIDLANGIDBaseList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0207 >> 8),
    (uint8_t)(0x0207),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0409 >> 8),
    (uint8_t)(0x0409),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),

    //Attribute HIDBatteryPower
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0209 >> 8),
    (uint8_t)(0x0209),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDRemoteWake
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020A >> 8),
    (uint8_t)(0x020A),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDNormallyConnectable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020D >> 8),
    (uint8_t)(0x020D),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDBootDevice
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020E >> 8),
    (uint8_t)(0x020E),
    SDP_BOOL_ONE_BYTE,
    0x01,
};
#endif


#if F_APP_TEAMS_FEATURE_SUPPORT
#if F_APP_TEAMS_HID_SUPPORT
static const uint8_t hid_sdp_record[] =
{
    //Total length
    // SDP_DATA_ELEM_SEQ_HDR,
    // 0xED,
    // SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    // 0x01,
    // 0x01,
    SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    0x01,
    0x36,//0x28,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_CONTROL >> 8),
    (uint8_t)(PSM_HID_CONTROL),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)(SDP_LANG_ENGLISH),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)(SDP_CHARACTER_UTF8),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)(SDP_BASE_LANG_OFFSET),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0101 >> 8),
    (uint8_t)(0x0101),

    //Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_ADD_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0F,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_INTERRUPT >> 8),
    (uint8_t)(PSM_HID_INTERRUPT),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', ' ', 'H', 'I', 'D',

    //Attribute SDP_ATTR_SRV_DESC
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x10,
    '(', '3', ')', ' ', 'B', 'u', 't', 't', 'o', 'n', ' ', 'M', 'o', 'u', 's', 'e',

    //Attribute SDP_ATTR_PROVIDER_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x07,
    'R', 'e', 'a', 'l', 't', 'e', 'k',

    //Attribute HIDParserVersion
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0201 >> 8),
    (uint8_t)(0x0201),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0111 >> 8),
    (uint8_t)(0x0111),

    //Attribute HIDDeviceSubclass
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0202 >> 8),
    (uint8_t)(0x0202),
    SDP_UNSIGNED_ONE_BYTE,
    0x80,

    //Attribute HIDCountryCode
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0203 >> 8),
    (uint8_t)(0x0203),
    SDP_UNSIGNED_ONE_BYTE,
    0x21,

    //Attribute HIDVirtualCable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0204 >> 8),
    (uint8_t)(0x0204),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDReconnectInitiate
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0205 >> 8),
    (uint8_t)(0x0205),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDDescriptorList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0206 >> 8),
    (uint8_t)(0x0206),

    SDP_DATA_ELEM_SEQ_HDR,
    0x81,//0x4c,
    SDP_DATA_ELEM_SEQ_HDR,
    0x7f,//0x4A,
    SDP_UNSIGNED_ONE_BYTE,
    0x22,                   /* Type = Report Descriptor */
    SDP_STRING_HDR,
    0x7b,//0x46,
    0x06, 0x0b, 0xff,        /* USAGE_PAGE       Vendor          */
    0x0a, 0x04, 0x01,        /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,              /* COLLECTION        (Application)  */
    0x15, 0x00,              /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,              /* LOGICAL_MAXIMUM   (255)          */
    0x75, 0x08,              /* REPORT_SIZE       (8)            */
    0x95, 0x3c,              /* REPORT_COUNT      (60)           */
    0x85, REPORT_ID_USB_UTILITY_1,              /* REPORT_ID         (42)           */
    0x09, 0x60,              /* USAGE             (0x60)         */
    0x82, 0x02, 0x01,        /* INPUT             (Data,Var,Abs) */
    0x09, 0x61,              /* USAGE             (0x61)         */
    0x92, 0x02, 0x01,        /* OUTPUT            (Data,Var,Abs) */
    0x09, 0x62,              /* USAGE             (0x62)         */
    0xb2, 0x02, 0x01,        /* FEATURE           (Data,Var,Abs) */
    /* 32-bit data */
    0x17, 0x00, 0x00, 0x00, 0x80,   /* LOGICAL_MINIMUM                 */
    0x27, 0xff, 0xff, 0xff, 0x7e,   /* LOGICAL_MAXIMUM                 */
    0x75, 0x20,                     /* REPORT_SIZE      (32)           */
    0x95, 0x04,                     /* REPORT_COUNT     (4)            */
    0x85, REPORT_ID_USB_UTILITY_3,                     /* REPORT_ID        (44)           */
    0x19, 0x66,                     /* USAGE_MINIMUM    (0x66)         */
    0x29, 0x69,                     /* USAGE_MAXIMUM    (0x69)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x85, REPORT_ID_USB_UTILITY_4,                     /* REPORT_ID        (45)           */
    0x19, 0x8a,                     /* USAGE_MINIMUM    (0x8a)         */
    0x29, 0x8d,                     /* USAGE_MAXIMUM    (0x8d)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x19, 0x8e,                     /* USAGE_MINIMUM    (0x8e)         */
    0x29, 0x91,                     /* USAGE_MAXIMUM    (0x91)         */
    0x91, 0x02,                     /* OUTPUT           (Data,Var,Abs) */
    0xc0,                            /* END_COLLECTION                  */

    // SDP_DATA_ELEM_SEQ_HDR,
    // 0x27,
    // SDP_DATA_ELEM_SEQ_HDR,
    // 0x25,
    // SDP_UNSIGNED_ONE_BYTE,
    // 0x22,                   /* Type = Report Descriptor */
    // SDP_STRING_HDR,
    // 0x21,

    0x06, 0x07, 0xff,               /* USAGE_PAGE       Vendor          */
    0x0a, 0x22, 0x02,               /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,                     /* COLLECTION        (Application)  */
    0x85, REPORT_ID_USB_UTILITY_5,  /* REPORT_ID         (24)           */
    0x75, 0x08,                     /* REPORT_SIZE      (8)           */
    0x95, 0x3e,                     /* REPORT_COUNT     (62)            */
    0x15, 0x00,                     /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,                     /* LOGICAL_MAXIMUM   (255)          */
    0x09, 0x01,                     /* USAGE             (0x01)         */
    0xb2, 0x02, 0x01,               /* FEATURE           (Data,Var,Abs) */

    0x85, REPORT_ID_USB_UTILITY_6,  /* REPORT_ID         (27)           */
    0x95, 0x3e,                     /* REPORT_COUNT     (62)            */
    0x09, 0x02,                     /* USAGE             (0x02)         */
    0x82, 0x02, 0x01,               /* INPUT             (Data,Var,Abs) */
    0xc0,                            /* END_COLLECTION                  */

    0x06, 0x00, 0xff,               /* USAGE_PAGE       Vendor          */
    0x0a, 0x39, 0xff,               /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,                     /* COLLECTION        (Application)  */
    0x85, REPORT_ID_USB_UTILITY_7,  /* REPORT_ID         (0x39)         */
    0x75, 0x08,                     /* REPORT_SIZE       (8)            */
    0x95, 0x07,                     /* REPORT_COUNT      (7)            */
    0x09, 0x04,                     /* USAGE             (0x04)         */
    0x82, 0x02, 0x01,               /* INPUT           (Data,Var,Abs)   */

    0xc0,                            /* END_COLLECTION                  */

    //Attribute HIDLANGIDBaseList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0207 >> 8),
    (uint8_t)(0x0207),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0409 >> 8),
    (uint8_t)(0x0409),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),

    //Attribute HIDBatteryPower
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0209 >> 8),
    (uint8_t)(0x0209),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDRemoteWake
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020A >> 8),
    (uint8_t)(0x020A),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDBootDevice
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020E >> 8),
    (uint8_t)(0x020E),
    SDP_BOOL_ONE_BYTE,
    0x01,
};
#endif
#endif

#if F_APP_CFU_HID_SUPPORT
static const uint8_t hid_cfu_sdp_record[] =
{
    //Total length
    // SDP_DATA_ELEM_SEQ_HDR,
    // 0xED,
    // SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    // 0x01,
    // 0x01,
    SDP_DATA_ELEM_SEQ_HDR_2BYTE,
    0x01,
    0x01,//0x28,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_CONTROL >> 8),
    (uint8_t)(PSM_HID_CONTROL),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)(SDP_LANG_ENGLISH),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)(SDP_CHARACTER_UTF8),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)(SDP_BASE_LANG_OFFSET),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
    (uint8_t)(UUID_HUMAN_INTERFACE_DEVICE_SERVICE),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0101 >> 8),
    (uint8_t)(0x0101),

    //Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_ADD_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0F,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0D,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_HID_INTERRUPT >> 8),
    (uint8_t)(PSM_HID_INTERRUPT),
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_HIDP >> 8),
    (uint8_t)(UUID_HIDP),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', ' ', 'H', 'I', 'D',

    //Attribute SDP_ATTR_SRV_DESC
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_DESC + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x10,
    '(', '3', ')', ' ', 'B', 'u', 't', 't', 'o', 'n', ' ', 'M', 'o', 'u', 's', 'e',

    //Attribute SDP_ATTR_PROVIDER_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x07,
    'R', 'e', 'a', 'l', 't', 'e', 'k',

    //Attribute HIDParserVersion
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0201 >> 8),
    (uint8_t)(0x0201),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0111 >> 8),
    (uint8_t)(0x0111),

    //Attribute HIDDeviceSubclass
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0202 >> 8),
    (uint8_t)(0x0202),
    SDP_UNSIGNED_ONE_BYTE,
    0x80,

    //Attribute HIDCountryCode
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0203 >> 8),
    (uint8_t)(0x0203),
    SDP_UNSIGNED_ONE_BYTE,
    0x21,

    //Attribute HIDVirtualCable
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0204 >> 8),
    (uint8_t)(0x0204),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDReconnectInitiate
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0205 >> 8),
    (uint8_t)(0x0205),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDDescriptorList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0206 >> 8),
    (uint8_t)(0x0206),

    SDP_DATA_ELEM_SEQ_HDR,
    0x4c,//0x4c,
    SDP_DATA_ELEM_SEQ_HDR,
    0x4a,//0x4A,
    SDP_UNSIGNED_ONE_BYTE,
    0x22,                   /* Type = Report Descriptor */
    SDP_STRING_HDR,
    0x46,//0x46,
    0x06, 0x0b, 0xff,        /* USAGE_PAGE       Vendor          */
    0x0a, 0x04, 0x01,        /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,              /* COLLECTION        (Application)  */
    0x15, 0x00,              /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,              /* LOGICAL_MAXIMUM   (255)          */
    0x75, 0x08,              /* REPORT_SIZE       (8)            */
    0x95, 0x3c,              /* REPORT_COUNT      (60)           */
    0x85, REPORT_ID_USB_UTILITY_1,              /* REPORT_ID         (42)           */
    0x09, 0x60,              /* USAGE             (0x60)         */
    0x82, 0x02, 0x01,        /* INPUT             (Data,Var,Abs) */
    0x09, 0x61,              /* USAGE             (0x61)         */
    0x92, 0x02, 0x01,        /* OUTPUT            (Data,Var,Abs) */
    0x09, 0x62,              /* USAGE             (0x62)         */
    0xb2, 0x02, 0x01,        /* FEATURE           (Data,Var,Abs) */
    /* 32-bit data */
    0x17, 0x00, 0x00, 0x00, 0x80,   /* LOGICAL_MINIMUM                 */
    0x27, 0xff, 0xff, 0xff, 0x7e,   /* LOGICAL_MAXIMUM                 */
    0x75, 0x20,                     /* REPORT_SIZE      (32)           */
    0x95, 0x04,                     /* REPORT_COUNT     (4)            */
    0x85, REPORT_ID_USB_UTILITY_3,                     /* REPORT_ID        (44)           */
    0x19, 0x66,                     /* USAGE_MINIMUM    (0x66)         */
    0x29, 0x69,                     /* USAGE_MAXIMUM    (0x69)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x85, REPORT_ID_USB_UTILITY_4,                     /* REPORT_ID        (45)           */
    0x19, 0x8a,                     /* USAGE_MINIMUM    (0x8a)         */
    0x29, 0x8d,                     /* USAGE_MAXIMUM    (0x8d)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x19, 0x8e,                     /* USAGE_MINIMUM    (0x8e)         */
    0x29, 0x91,                     /* USAGE_MAXIMUM    (0x91)         */
    0x91, 0x02,                     /* OUTPUT           (Data,Var,Abs) */
    0xc0,                            /* END_COLLECTION                  */

    0xc0,                            /* END_COLLECTION                  */

    //Attribute HIDLANGIDBaseList
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0207 >> 8),
    (uint8_t)(0x0207),
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0409 >> 8),
    (uint8_t)(0x0409),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0100 >> 8),
    (uint8_t)(0x0100),

    //Attribute HIDBatteryPower
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0209 >> 8),
    (uint8_t)(0x0209),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDRemoteWake
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020A >> 8),
    (uint8_t)(0x020A),
    SDP_BOOL_ONE_BYTE,
    0x01,

    //Attribute HIDBootDevice
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x020E >> 8),
    (uint8_t)(0x020E),
    SDP_BOOL_ONE_BYTE,
    0x01,
};
#endif
#endif

#if AVRCP_BROWSING_FEATURE_SUPPORT
static const uint8_t avrcp_ct_sdp_record[] =
{
    //Total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x52,//0x3B,//0x49,//0x62,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Attribute length: 6 bytes
    //Service Class #0: A/V Remote Control
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL),
    //Service Class #1: A/V Remote Control Controller
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL_CONTROLLER >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL_CONTROLLER),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x10, //Attribute length: 12 bytes
    //Protocol #0: L2CAP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 3 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    //Parameter #0 for Protocol #0: PSM = AVCTP
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_AVCTP >> 8),
    (uint8_t)PSM_AVCTP,
    //Protocol #1: AVCTP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 5 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AVCTP >> 8),
    (uint8_t)(UUID_AVCTP),
    //Parameter #0 for Protocol #1: 0x0104 (v1.4)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0104 >> 8),
    (uint8_t)(0x0104),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08, //Attribute length: 8 bytes
    //Profile #0: A/V Remote Control
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 6 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL),
    //Parameter #0 for Profile #0: 0x0106 (v1.6)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0106 >> 8),
    (uint8_t)(0x0106),

    //Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_ADD_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x12,
    SDP_DATA_ELEM_SEQ_HDR,
    0x10, //Attribute length: 12 bytes
    //Protocol #0: L2CAP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 3 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    //Parameter #0 for Protocol #0: PSM = AVCTP_Browsing
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_AVCTP_BROWSING >> 8),
    (uint8_t)PSM_AVCTP_BROWSING,
    //Protocol #1: AVCTP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 5 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AVCTP >> 8),
    (uint8_t)(UUID_AVCTP),
    //Parameter #0 for Protocol #1: 0x0104 (v1.4)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0104 >> 8),
    (uint8_t)(0x0104),

    //Attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FEATURES) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0241 >> 8), //Category 1 Player / Recorder
    (uint8_t)(0x0241),      //Support Browsing & Cover Art

    //Attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP
    /*
        //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //Attribute SDP_ATTR_PROVIDER_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x07, //Attribute length: 7 bytes
        0x52, 0x65, 0x61, 0x6C, 0x54, 0x65, 0x6B, //RealTek

        //Attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x08, //Attribute length: 8 bytes
        0x41, 0x56, 0x52, 0x43, 0x50, 0x20, 0x43, 0x54, //AVRCP CT
    */
};
#else
static const uint8_t avrcp_ct_sdp_record[] =
{
    //Total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x3B,//0x49,//0x62,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Attribute length: 6 bytes
    //Service Class #0: A/V Remote Control
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL),
    //Service Class #1: A/V Remote Control Controller
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL_CONTROLLER >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL_CONTROLLER),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x10, //Attribute length: 12 bytes
    //Protocol #0: L2CAP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 3 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    //Parameter #0 for Protocol #0: PSM = AVCTP
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_AVCTP >> 8),
    (uint8_t)PSM_AVCTP,
    //Protocol #1: AVCTP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 5 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AVCTP >> 8),
    (uint8_t)(UUID_AVCTP),
    //Parameter #0 for Protocol #1: 0x0104 (v1.4)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0104 >> 8),
    (uint8_t)(0x0104),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08, //Attribute length: 8 bytes
    //Profile #0: A/V Remote Control
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 6 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL),
    //Parameter #0 for Profile #0: 0x0106 (v1.6)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0106 >> 8),
    (uint8_t)(0x0106),

    //Attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FEATURES) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0001 >> 8), //Category 1 Player / Recorder
    (uint8_t)(0x0001),

    //Attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP
    /*
        //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //Attribute SDP_ATTR_PROVIDER_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x07, //Attribute length: 7 bytes
        0x52, 0x65, 0x61, 0x6C, 0x54, 0x65, 0x6B, //RealTek

        //Attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x08, //Attribute length: 8 bytes
        0x41, 0x56, 0x52, 0x43, 0x50, 0x20, 0x43, 0x54, //AVRCP CT
    */
};
#endif

static const uint8_t avrcp_tg_sdp_record[] =
{
    //Total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x38,//0x46,//0x5F,

    //Attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03, //Attribute length: 6 bytes
    //Service Class #0: A/V Remote Control Target
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL_TARGET >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL_TARGET),

    //Attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x10, //Attribute length: 12 bytes
    //Protocol #0: L2CAP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 3 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    //Parameter #0 for Protocol #0: PSM = AVCTP
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_AVCTP >> 8),
    (uint8_t)PSM_AVCTP,
    //Protocol #1: AVCTP
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 5 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AVCTP >> 8),
    (uint8_t)(UUID_AVCTP),
    //Parameter #0 for Protocol #1: 0x0104 (v1.4)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0104 >> 8),
    (uint8_t)(0x0104),

    //Attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08, //Attribute length: 8 bytes
    //Profile #0: A/V Remote Control
    SDP_DATA_ELEM_SEQ_HDR,
    0x06, //Element length: 6 bytes
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AV_REMOTE_CONTROL >> 8),
    (uint8_t)(UUID_AV_REMOTE_CONTROL),
    //Parameter #0 for Profile #0: 0x0106 (v1.6)
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0106 >> 8),
    (uint8_t)(0x0106),

    //Attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FEATURES) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0002 >> 8), //Category 2 Amplifier
    (uint8_t)(0x0002),

    //Attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP
    /*
        //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //Attribute SDP_ATTR_PROVIDER_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_PROVIDER_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x07, //Attribute length: 7 bytes
        0x52, 0x65, 0x61, 0x6C, 0x54, 0x65, 0x6B, //RealTek

        //Attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x08, //Attribute length: 8 bytes
        0x41, 0x56, 0x52, 0x43, 0x50, 0x20, 0x54, 0x47, //AVRCP TG
    */
};

static const uint8_t hsp_sdp_record[] =
{
    //total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x39,//0x58,

    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x09,                                   //6bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HEADSET >> 8),       //0x1108
    (uint8_t)(UUID_HEADSET),
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HEADSET_HS >> 8),    //0x1131
    (uint8_t)(UUID_HEADSET_HS),
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_GENERIC_AUDIO >> 8),  //0x1203
    (uint8_t)(UUID_GENERIC_AUDIO),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x0C,                                   //12 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x03,                               ///3 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_L2CAP >> 8),     //0x0100
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,                               //5 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_RFCOMM >> 8),    //0x0003
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,           //0x08
    RFC_HSP_CHANN_NUM,      //0x01

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x08,                                   //8 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x06,                               //6 bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HEADSET >> 8),   //0x1108
    (uint8_t)(UUID_HEADSET),
    SDP_UNSIGNED_TWO_BYTE,               //0x09
    (uint8_t)(0x0102 >> 8),         //version number default hs1.2
    (uint8_t)(0x0102),

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,
    /*
        //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,                             //0x25 text string
        0x0C,                               //12 bytes
        0x48, 0x65, 0x61, 0x64, 0x73, 0x65, 0x74, 0x20, 0x75, 0x6e,
        0x69, 0x74, //"Headset unit"
    */
    //attribute SDP_ATTR_RMT_AUDIO_VOL_CTRL
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_RMT_AUDIO_VOL_CTRL >> 8),
    (uint8_t)SDP_ATTR_RMT_AUDIO_VOL_CTRL,
    SDP_BOOL_ONE_BYTE,                       //0x28
    0x01                                    //True
};

static const uint8_t hfp_sdp_record[] =
{
    //total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x4B,//0x37,//0x59,

    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x06,                                   //6bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HANDSFREE >> 8), //0x111E
    (uint8_t)(UUID_HANDSFREE),
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_GENERIC_AUDIO >> 8),  //0x1203
    (uint8_t)(UUID_GENERIC_AUDIO),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x0C,                                   //12bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x03,                               //3bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_L2CAP >> 8),     //0x0100
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x05,                               //5bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_RFCOMM >> 8),   //0x0003
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,           //0x08
    RFC_HFP_CHANN_NUM,  //0x02

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,
    /*
        //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,                             //0x25 text string
        0x0F,                                   //15 bytes
        0x48, 0x61, 0x6e, 0x64, 0x73, 0x2d, 0x66, 0x72, 0x65, 0x65,
        0x20, 0x75, 0x6e, 0x69, 0x74, //"Hands-free unit"
    */

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x08,                                   //8 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x06,                               //6 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_HANDSFREE >> 8), //0x111E
    (uint8_t)(UUID_HANDSFREE),
    SDP_UNSIGNED_TWO_BYTE,           //0x09
    (uint8_t)(0x0107 >> 8),     //version number default hf1.7
    (uint8_t)(0x0107),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0F,
    'H', 'a', 'n', 'd', 's', '-', 'F', 'r', 'e', 'e', ' ', 'u', 'n', 'i', 't',

    //attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FEATURES) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x003F >> 8),
    (uint8_t)(0x003F)
};

static const uint8_t a2dp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x39,//0x55,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AUDIO_SINK >> 8),
    (uint8_t)(UUID_AUDIO_SINK),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x10,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(PSM_AVDTP >> 8),
    (uint8_t)(PSM_AVDTP),
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_AVDTP >> 8),
    (uint8_t)(UUID_AVDTP),
    SDP_UNSIGNED_TWO_BYTE,
    0x01,
    0x03,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP),
    /*
        //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,
    */
    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_ADVANCED_AUDIO_DISTRIBUTION >> 8),
    (uint8_t)(UUID_ADVANCED_AUDIO_DISTRIBUTION),
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.4
    0x04,

    //attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES >> 8),
    (uint8_t)SDP_ATTR_SUPPORTED_FEATURES,
    SDP_UNSIGNED_TWO_BYTE,
    0x00,
    0x03
    /*
        //attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,
        0x09,
        0x61, 0x32, 0x64, 0x70, 0x5f, 0x73, 0x69, 0x6e, 0x6b //a2dp_sink
    */
};

#if F_APP_HFP_AG_SUPPORT
static const uint8_t hsp_ag_sdp_record[] =
{
    //total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x29,//41bytes,

    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x06,                                   //6bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HEADSET_AUDIO_GATEWAY >> 8),       //0x1112
    (uint8_t)(UUID_HEADSET_AUDIO_GATEWAY),
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_GENERIC_AUDIO >> 8),  //0x1203
    (uint8_t)(UUID_GENERIC_AUDIO),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x0C,                                   //12 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x03,                               ///3 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_L2CAP >> 8),     //0x0100
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,                               //5 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_RFCOMM >> 8),    //0x0003
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,           //0x08
    RFC_HSP_AG_CHANN_NUM,      //0x12

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x08,                                   //8 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x06,                               //6 bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HEADSET_AUDIO_GATEWAY >> 8),   //0x1112
    (uint8_t)(UUID_HEADSET_AUDIO_GATEWAY),
    SDP_UNSIGNED_TWO_BYTE,               //0x09
    (uint8_t)(0x0102 >> 8),         //version number default hs1.2
    (uint8_t)(0x0102)
    /*
        //attribute SDP_ATTR_BROWSE_GROUP_LIST
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
        (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x03,
        SDP_UUID16_HDR,
        (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
        (uint8_t)UUID_PUBLIC_BROWSE_GROUP,
    */
    /*
        //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,                             //0x25 text string
        0x0C,                               //12 bytes
        0x48, 0x65, 0x61, 0x64, 0x73, 0x65, 0x74, 0x20, 0x75, 0x6e,
        0x69, 0x74, //"Headset unit"
    */                                //True
};

static const uint8_t hfp_ag_sdp_record[] =
{
    //total length
    SDP_DATA_ELEM_SEQ_HDR,
    0x34,//52 bytes belowed.

    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x06,                                   //6bytes
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_HANDSFREE_AUDIO_GATEWAY >> 8), //0x111F
    (uint8_t)(UUID_HANDSFREE_AUDIO_GATEWAY),
    SDP_UUID16_HDR,                     //0x19
    (uint8_t)(UUID_GENERIC_AUDIO >> 8),  //0x1203
    (uint8_t)(UUID_GENERIC_AUDIO),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x0C,                                   //12bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x03,                               //3bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_L2CAP >> 8),     //0x0100
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x05,                               //5bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_RFCOMM >> 8),   //0x0003
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,           //0x08
    RFC_HFP_AG_CHANN_NUM,  //0x11

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,          //0x35
    0x08,                                   //8 bytes
    SDP_DATA_ELEM_SEQ_HDR,      //0x35
    0x06,                               //6 bytes
    SDP_UUID16_HDR,                 //0x19
    (uint8_t)(UUID_HANDSFREE_AUDIO_GATEWAY >> 8), //0x111F
    (uint8_t)(UUID_HANDSFREE_AUDIO_GATEWAY),
    SDP_UNSIGNED_TWO_BYTE,           //0x09
    (uint8_t)(0x0108 >> 8),     //version number default hf1.8
    (uint8_t)(0x0108),
    /*
        //attribute SDP_ATTR_BROWSE_GROUP_LIST
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
        (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x03,
        SDP_UUID16_HDR,
        (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
        (uint8_t)UUID_PUBLIC_BROWSE_GROUP,
    */
    /*
        //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
        (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
        SDP_DATA_ELEM_SEQ_HDR,
        0x09,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_LANG_ENGLISH >> 8),
        (uint8_t)SDP_LANG_ENGLISH,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
        (uint8_t)SDP_CHARACTER_UTF8,
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
        (uint8_t)SDP_BASE_LANG_OFFSET,

        //attribute SDP_ATTR_SRV_NAME
        SDP_UNSIGNED_TWO_BYTE,
        (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
        (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
        SDP_STRING_HDR,                             //0x25 text string
        0x0F,                                   //15 bytes
        0x48, 0x61, 0x6e, 0x64, 0x73, 0x2d, 0x66, 0x72, 0x65, 0x65,
        0x20, 0x75, 0x6e, 0x69, 0x74, //"Hands-free unit"
    */
    //attribute SDP_ATTR_EXT_NETWORK
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_EXT_NETWORK) >> 8),
    (uint8_t)(SDP_ATTR_EXT_NETWORK),
    SDP_UNSIGNED_ONE_BYTE,
    (uint8_t)(0x01),

    //attribute SDP_ATTR_SUPPORTED_FEATURES
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FEATURES) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FEATURES),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(0x0029 >> 8),
    (uint8_t)(0x0029)
};
#endif

const uint8_t spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4C,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_SERIAL_PORT >> 8),
    (uint8_t)(UUID_SERIAL_PORT),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_SERIAL_PORT >> 8),
    (uint8_t)UUID_SERIAL_PORT,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.2
    0x02,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
};

const uint8_t rtk_vendor_spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4E,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x6a, 0x24, 0xee, 0xab, 0x4b, 0x65, 0x46, 0x93, 0x98, 0x6b, 0x3c, 0x26, 0xc3, 0x52, 0x26, 0x4f,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_RTK_VENDOR_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'r', 't', 'k', '_', 'v', 'n', 'd', '_', 's', 'p', 'p'
};

#if F_APP_SPECIFIC_UUID_SUPPORT
//This is a NON-CONST table, customer will difine its' own UUID via UI tool
static uint8_t spp_specific_uuid_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4F,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_RTK_VENDOR_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0C,
    's', 'p', 'e', 'c', 'i', 'f', 'i', 'c', '_', 's', 'p', 'p'
};
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
const uint8_t xiaowei_spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4B,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,//85dbf2f9-73e3-43f5-a129-971b91c72f1e
    //0x1e, 0x2f, 0xc7, 0x91, 0x1b, 0x97, 0x29, 0xa1, 0xf5, 0x43, 0xe3, 0x73, 0xf9, 0xf2, 0xdb, 0x85,
    0x85, 0xdb, 0xf2, 0xf9, 0x73, 0xe3, 0x43, 0xf5, 0xa1, 0x29, 0x97, 0x1b, 0x91, 0xc7, 0x2f, 0x1e,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_XIAOWEI_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x09,
    'x', 'i', 'a', 'o', 'w', 'e', 'i', '_', '_'
};
#endif

static const uint8_t iap_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4F,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x00, 0x00, 0x00, 0x00, 0xde, 0xca, 0xfa, 0xde, 0xde, 0xca, 0xde, 0xaf, 0xde, 0xca, 0xca, 0xff,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_IAP_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0C,
    0x57, 0x69, 0x72, 0x65, 0x6c, 0x65, 0x73, 0x73, 0x20, 0x69, 0x41, 0x50 //"Wireless iAP"
};

#if F_APP_BT_PROFILE_MAP_SUPPORT
const uint8_t map_mce_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x49,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_MSG_NOTIFICATION_SERVER >> 8),
    (uint8_t)(UUID_MSG_NOTIFICATION_SERVER),

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,
    RFC_MAP_MNS_CHANN_NUM,  //channel number
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_OBEX >> 8),
    (uint8_t)(UUID_OBEX),

    //Attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', '-', 'M', 'N', 'S',

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_MSG_ACCESS_PROFILE >> 8),
    (uint8_t)UUID_MSG_ACCESS_PROFILE,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,
    0x04,   //version 1.4

    //attribute SDP_ATTR_L2C_PSM
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_L2C_PSM) >> 8),
    (uint8_t)(SDP_ATTR_L2C_PSM),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(L2CAP_MAP_MNS_PSM >> 8),
    (uint8_t)(L2CAP_MAP_MNS_PSM),

    //attribute SDP_ATTR_MAP_SUPPERTED_FEATS
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_MAP_SUPPERTED_FEATS) >> 8),
    (uint8_t)(SDP_ATTR_MAP_SUPPERTED_FEATS),
    SDP_UNSIGNED_FOUR_BYTE,
    (uint8_t)(0x0000024F >> 24),
    (uint8_t)(0x0000024F >> 16),
    (uint8_t)(0x0000024F >> 8),
    (uint8_t)(0x0000024F)
};
#endif

const uint8_t pbap_pce_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x36,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PBAP_PCE >> 8),
    (uint8_t)(UUID_PBAP_PCE),

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PBAP >> 8),
    (uint8_t)UUID_PBAP,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.2
    0x02,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x14,
    0x50, 0x68, 0x6f, 0x6e, 0x65, 0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x41, 0x63, 0x63, 0x65, 0x73, 0x73, 0x20, 0x50, 0x43, 0x45 //"Phonebook Access PCE"
};


#if GFPS_FEATURE_SUPPORT
static const uint8_t gfps_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4F,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0xdf, 0x21, 0xfe, 0x2c, 0x25, 0x15, 0x4f, 0xdb, 0x88, 0x86, 0xf1, 0x2c, 0x4d, 0x67, 0x92, 0x7c,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_GFPS_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x09,
    0x67, 0x66, 0x70, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74 //"gfps test"
};
#endif

#if DMA_FEATURE_SUPPORT
static const uint8_t dma_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    sizeof((uint8_t []) {DMA_CLS_ID_LIST}) + sizeof((uint8_t []) {DMA_PROTO_DESC_LIST}) + sizeof((uint8_t []) {DMA_BROWSE_DESC_LIST})
    + sizeof((uint8_t []) {DMA_LANG_BASE_LIST}) + sizeof((uint8_t []) {DMA_SERV_NAME}),
    DMA_CLS_ID_LIST,
    DMA_PROTO_DESC_LIST,
    DMA_BROWSE_DESC_LIST,
    DMA_LANG_BASE_LIST,
    DMA_SERV_NAME,
};
#endif


#if AMA_FEATURE_SUPPORT
const uint8_t ama_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4E,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x93, 0x1C, 0x7E, 0x8A, 0x54, 0x0F, 0x46, 0x86, 0xB7, 0x98, 0xE8, 0xDF, 0x0A, 0x2A, 0xD9, 0xF7,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_AMA_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
};
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
const uint8_t dongle_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4E,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x5A, 0x01, 0x00, 0x00,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_DONGLE_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
};
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
static const uint8_t xm_xiaoai_spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4F,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x85, 0x84, 0xd0, 0x18, 0x10,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_XIAOAI_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x09,
    0x67, 0x66, 0x70, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74 //"gfps test"
};
#endif

#if BISTO_FEATURE_SUPPORT

const uint8_t bisto_control_spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4E,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    BISTO_BT_CONTROL_SDP_UUID,
    //22


    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_BISTO_CTRL_CHANN_NUM,
    //17

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,
    //8

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,
    //14

//    //attribute SDP_ATTR_PROFILE_DESC_LIST
//    SDP_UNSIGNED_TWO_BYTE,
//    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
//    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
//    SDP_DATA_ELEM_SEQ_HDR,
//    0x08,
//    SDP_DATA_ELEM_SEQ_HDR,
//    0x06,
//    SDP_UUID16_HDR,
//    (uint8_t)(UUID_SERIAL_PORT >> 8),
//    (uint8_t)UUID_SERIAL_PORT,
//    SDP_UNSIGNED_TWO_BYTE,
//    0x01,//version 1.2
//    0x02,
//    //13

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
};


const uint8_t bisto_audio_spp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x4E,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    BISTO_BT_AUDIO_SDP_UUID,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_SPP_BISTO_AU_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

//    //attribute SDP_ATTR_PROFILE_DESC_LIST
//    SDP_UNSIGNED_TWO_BYTE,
//    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
//    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
//    SDP_DATA_ELEM_SEQ_HDR,
//    0x08,
//    SDP_DATA_ELEM_SEQ_HDR,
//    0x06,
//    SDP_UUID16_HDR,
//    (uint8_t)(UUID_SERIAL_PORT >> 8),
//    (uint8_t)UUID_SERIAL_PORT,
//    SDP_UNSIGNED_TWO_BYTE,
//    0x01,//version 1.2
//    0x02,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
};
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
static const uint8_t ms_hs_asp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x58,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x54, 0x3C, 0xEA, 0x22, 0xAD, 0x9C, 0x42, 0x54, 0xA4, 0x7E, 0x5E, 0x1A, 0xE2, 0x5A, 0x40, 0xBD,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_HS_ASP_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_SERIAL_PORT >> 8),
    (uint8_t)UUID_SERIAL_PORT,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.0
    0x00,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x09,
    'T', 'E', 'A', 'M', 'S', ' ', 'A', 'S', 'P' //"TEAMS ASP"
};

static const uint8_t ms_hs_hid_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x5C,
    //attribute SDP_ATTR_SRV_CLASS_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8E, 0x93, 0xD2, 0x17, 0x3C, 0x5A, 0x01, 0x00, 0x00,

    //attribute SDP_ATTR_PROTO_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x0c,
    SDP_DATA_ELEM_SEQ_HDR,
    03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)UUID_L2CAP,
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)UUID_RFCOMM,
    SDP_UNSIGNED_ONE_BYTE,
    RFC_HS_HID_CHANN_NUM,

    //attribute SDP_ATTR_BROWSE_GROUP_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x09,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_LANG_ENGLISH >> 8),
    (uint8_t)SDP_LANG_ENGLISH,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),
    (uint8_t)SDP_CHARACTER_UTF8,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),
    (uint8_t)SDP_BASE_LANG_OFFSET,

    //attribute SDP_ATTR_PROFILE_DESC_LIST
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_SERIAL_PORT >> 8),
    (uint8_t)UUID_SERIAL_PORT,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,//version 1.0
    0x00,

    //attribute SDP_ATTR_SRV_NAME
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0D,
    'T', 'E', 'A', 'M', 'S', ' ', 'C', 'O', 'N', 'T', 'R', 'O', 'L' //"TEAMS CONTROL"
};

static const uint8_t ms_device_info_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x45,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_UUID128_HDR,
    0x40, 0xbf, 0xe9, 0x57, 0x1d, 0x78, 0x49, 0x6a, 0x8b, 0x50, 0x96, 0xd5, 0xe7, 0xc9, 0x64, 0x5f,

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP,

    SDP_UNSIGNED_TWO_BYTE,
    0x03, 0x01,           //attr id
    SDP_DATA_ELEM_SEQ_HDR,
    0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    SDP_UNSIGNED_TWO_BYTE,
    0x0F, 0x01,           //attr id
    SDP_DATA_ELEM_SEQ_HDR,
    0x02,
    0x12, 0x28,

    SDP_UNSIGNED_TWO_BYTE,
    0x0F, 0x02,           //attr id
    SDP_DATA_ELEM_SEQ_HDR,
    0x02,
    0x04, 0x05
};

#endif

#if F_APP_DURIAN_SUPPORT
const uint8_t *durian_aap_record = NULL;
const uint8_t *durian_sdp_record = NULL;
const uint8_t *durian_did_record = NULL;
#endif

void app_sdp_init(void)
{
    bool iap_sdp_record_added = false;

#if F_APP_DURIAN_SUPPORT
    if (durian_aap_record == NULL || durian_sdp_record == NULL || durian_did_record == NULL)
    {
        bt_sdp_record_add((void *)did_sdp_record);
    }
    else
    {
        bt_sdp_record_add((void *)durian_aap_record);
        bt_sdp_record_add((void *)durian_sdp_record);
        bt_sdp_record_add((void *)durian_did_record);
    }
#else
    bt_sdp_record_add((void *)did_sdp_record);
#endif

    if (app_cfg_const.supported_profile_mask & A2DP_PROFILE_MASK)
    {
        bt_sdp_record_add((void *)a2dp_sdp_record);
    }
    if (app_cfg_const.supported_profile_mask & AVRCP_PROFILE_MASK)
    {
        uint8_t ct_features;
        uint8_t tg_features;

        app_avrcp_get_supported_features(&ct_features, &tg_features);

        if (ct_features != 0)
        {
            bt_sdp_record_add((void *)avrcp_ct_sdp_record);
        }

        if (tg_features != 0)
        {
            bt_sdp_record_add((void *)avrcp_tg_sdp_record);
        }
    }
    if (app_cfg_const.supported_profile_mask & HFP_PROFILE_MASK)
    {
        bt_sdp_record_add((void *)hfp_sdp_record);
#if F_APP_HFP_AG_SUPPORT
        bt_sdp_record_add((void *)hfp_ag_sdp_record);
#endif
    }
    if (app_cfg_const.supported_profile_mask & HSP_PROFILE_MASK)
    {

        bt_sdp_record_add((void *)hsp_sdp_record);
#if F_APP_HFP_AG_SUPPORT
        bt_sdp_record_add((void *)hsp_ag_sdp_record);
#endif
    }

    if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
    {
#if XM_XIAOAI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaoai_support && app_xiaoai_over_spp_supported())
        {
            bt_sdp_record_add((void *)xm_xiaoai_spp_sdp_record);
        }
        else
        {
            bt_sdp_record_add((void *)spp_sdp_record);
        }
#else
        bt_sdp_record_add((void *)spp_sdp_record);
#endif

#if F_APP_SPECIFIC_UUID_SUPPORT
        if (app_cfg_const.enable_specific_service_uuid)
        {
            memcpy(&spp_specific_uuid_sdp_record[8], app_cfg_const.specific_service_uuid, 16);
            bt_sdp_record_add((void *)spp_specific_uuid_sdp_record);
        }
        else
        {
            bt_sdp_record_add((void *)rtk_vendor_spp_sdp_record);
        }
#else
        bt_sdp_record_add((void *)rtk_vendor_spp_sdp_record);
#endif
    }

    if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
    {
        iap_sdp_record_added = true;
        bt_sdp_record_add((void *)iap_sdp_record);
    }

    if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
    {
        bt_sdp_record_add((void *)pbap_pce_sdp_record);
    }

#if F_APP_BT_PROFILE_MAP_SUPPORT
    if (app_cfg_const.supported_profile_mask & MAP_PROFILE_MASK)
    {
        bt_sdp_record_add((void *)map_mce_sdp_record);
    }
#endif

#if F_APP_HID_SUPPORT

    if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
    {
#if F_APP_HID_MOUSE_SUPPORT
        bt_sdp_record_add((void *)hid_mouse_sdp_record);
#endif
#if F_APP_HID_KEYBOARD_SUPPORT
        bt_sdp_record_add((void *)hid_keyboard_sdp_record);
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#if F_APP_TEAMS_HID_SUPPORT
        bt_sdp_record_add((void *)hid_sdp_record);
#endif
#endif
#if F_APP_CFU_HID_SUPPORT
        bt_sdp_record_add((void *)hid_cfu_sdp_record);
#endif
    }
#endif

    if (iap_sdp_record_added == false)
    {
#if AMA_FEATURE_SUPPORT
        if (extend_app_cfg_const.ama_support && app_ama_transport_supported(AMA_IAP_STREAM))
        {
            bt_sdp_record_add((void *)iap_sdp_record);
        }
#endif
    }

#if GFPS_FEATURE_SUPPORT
    if (app_cfg_const.supported_profile_mask & GFPS_PROFILE_MASK)
    {
        bt_sdp_record_add((void *)gfps_sdp_record);
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support && app_ama_transport_supported(AMA_SPP_STREAM))
    {
        bt_sdp_record_add((void *)ama_sdp_record);
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support && app_xiaowei_over_spp_supported())
    {
        bt_sdp_record_add((void *)xiaowei_spp_sdp_record);
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        bt_sdp_record_add((void *)bisto_audio_spp_sdp_record);
        bt_sdp_record_add((void *)bisto_control_spp_sdp_record);
    }
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
    bt_sdp_record_add((void *)ms_hs_asp_sdp_record);
    bt_sdp_record_add((void *)ms_hs_hid_sdp_record);
    bt_sdp_record_add((void *)ms_device_info_sdp_record);
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    bt_sdp_record_add((void *)dongle_sdp_record);
#endif

#if DMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.dma_support)
    {
        bt_sdp_record_add((void *)dma_sdp_record);
    }
#endif
}

