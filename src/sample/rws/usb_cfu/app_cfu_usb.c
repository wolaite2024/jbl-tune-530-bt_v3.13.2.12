/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CFU_SUPPORT

#include <string.h>
#include <bt_types.h>
#include "trace.h"

#include "os_mem.h"
#include "rtl876x.h"


#include "app_cfu_usb.h"
#include "app_cfu.h"
#include "app_common_cfu.h"

#if HID_DEBUG_INTERFACE_SUPPORT
#define REPORT_ID_HID_DEBUG_INTERFACE_IN    REPORT_ID_CFU_FEATURE
#define REPORT_ID_HID_DEBUG_INTERFACE_OUT   0x91
void app_cfu_usb_hid_handle_debug_data(uint8_t *data, uint8_t length);
#endif

#define APP_USB_HID_MAX_IN_SIZE 64

uint8_t app_usb_hid_interrupt_in_buff[APP_USB_HID_MAX_IN_SIZE];
#define ATTRIBUTE_LENGTH            144+39+52+44
uint16_t addin_report_desc_attrib_length = ATTRIBUTE_LENGTH;
char addin_report_desc_attrib[ATTRIBUTE_LENGTH] =
{
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB5,        //   Usage (Scan Next Track)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB6,        //   Usage (Scan Previous Track)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB7,        //   Usage (Stop)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB3,        //   Usage (Fast Forward)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xB4,        //   Usage (Rewind)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0B,        //   Usage Page (Telephony)
    0x09, 0x24,        //   Usage (Redial)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x20,        //   Usage (Hook Switch)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x2F,        //   Usage (Phone Mute)
    0x81, 0x06,        //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x05,        //   Report Count (5)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection

    // 67 bytes

    0x06, 0x0B, 0xFF,  // Usage Page (Vendor Defined 0xFF0B)
    0x0A, 0x01, 0x01,  // Usage (0x0101)
    0xA1, 0x01,        // Collection (Application)
    // 8-bit data
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (-1)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3c,        //   Report Count (60)
    0x85, REPORT_ID_CFU_FEATURE,        //   Report ID (REPORT_ID_CFU_FEATURE)
    0x09, 0x60,        //   Usage (0x60)
    0x82, 0x02, 0x01,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Buffered Bytes)
    0x09, USAGE_ID_CFU_PAYLOAD_OUTPUT,        //   Usage (0x61)
    0x92, 0x02, 0x01,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x09, USAGE_ID_CFU_FEATURE,        //   Usage (0x62)
    0xB2, 0x02, 0x01,  //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x85, REPORT_ID_CFU_FEATURE_EX,        //   Report ID (REPORT_ID_CFU_FEATURE_EX)
    0x09, USAGE_ID_CFU_FEATURE_EX,        //   Usage (0x65)
    0xB2, 0x02, 0x01,  //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    // 32-bit data
    0x17, 0x00, 0x00, 0x00, 0x80,  //   Logical Minimum (-2147483649)
    0x27, 0xFF, 0xFF, 0xFF, 0x7F,  //   Logical Maximum (2147483646)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x04,        //   Report Count (4)
    0x85, REPORT_ID_CFU_PAYLOAD_INPUT,        //   Report ID (REPORT_ID_CFU_OFFER_INPUT)
    0x19, USAGE_ID_CFU_PAYLOAD_INPUT_MIN,        //   Usage Minimum (0x66)
    0x29, USAGE_ID_CFU_PAYLOAD_INPUT_MAX,        //   Usage Maximum (0x69)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, REPORT_ID_CFU_OFFER_INPUT,        //   Report ID (45)
    0x19, USAGE_ID_CFU_OFFER_INPUT_MIN,        //   Usage Minimum (0x8A)
    0x29, USAGE_ID_CFU_OFFER_INPUT_MAX,        //   Usage Maximum (0x8D)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, USAGE_ID_CFU_OFFER_OUTPUT_MIN,        //   Usage Minimum (0x8E)
    0x29, USAGE_ID_CFU_OFFER_OUTPUT_MAX,        //   Usage Maximum (0x91)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    // 77 bytes

    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x11,                    //   REPORT_ID (17)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (10)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x15, 0xff,                    //   LOGICAL_MINIMUM (-1)
    0x19, 0x01,                    //   USAGE_MINIMUM (Keyboard ErrorRollOver)
    0x29, 0xa4,                    //   USAGE_MAXIMUM (Keyboard ExSel)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                           // END_COLLECTION

    //39
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x05,                    //     USAGE_MAXIMUM (Button 5)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x85, 0x12,                    //     REPORT_ID (18)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0,                           // END_COLLECTION

    //52

    0x06, 0x99, 0xFF,  // Usage Page (Vendor Defined 0xFF99)
    0x09, 0x01,        // Usage (0x01)
    0xA1, 0x01,        // Collection (Application)
    0x09, USAGE_ID_ASP_FEATURE,        //   Usage (0x03)
    0x85, REPORT_ID_ASP_FEATURE,        //   Report ID (-102)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3E,        //   Report Count (62)
    0xB2, 0x02, 0x01,  //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x09, USAGE_ID_ASP_INPUT,        //   Usage (0x04)
    0x85, REPORT_ID_ASP_INPUT,        //   Report ID (-101)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x0F,        //   Report Count (15)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection

    // 44 bytes

};

uint8_t usb_string_desc_id_addin = 33;
char *usb_string_desc_str_addin = "UCQ00011000101000";

void app_cfu_usb_hid_send_report(uint8_t channel, uint8_t report_id, uint8_t *data, uint8_t length)
{
    int i;
    if (channel == 0)
    {
        app_usb_hid_interrupt_in_buff[0] = 0x11;
    }
    else
    {
        app_usb_hid_interrupt_in_buff[0] = 0x12;
    }
    for (i = 0; i < APP_USB_HID_MAX_IN_SIZE - 1 && i < length; i++)
    {
        app_usb_hid_interrupt_in_buff[i + 1] = data[i];
    }
    USB_PRINT_TRACE3("USB HID in channel %x, length %x, %b", channel, i + 1, TRACE_BINARY(i + 1,
                     app_usb_hid_interrupt_in_buff));
    addin_hid_interrupt_in(app_usb_hid_interrupt_in_buff,
                           sizeof(app_usb_hid_interrupt_in_buff) / sizeof(app_usb_hid_interrupt_in_buff[0]));
}
#define hid_debug 0
#if hid_debug
#include "app_timer.h"

typedef enum
{
    APP_TIMER_USB_HID,
} T_APP_CFU_USB_TIMER;

static uint8_t app_usb_hid_timer_id = 0;
static uint8_t timer_idx_usb_hid = 0;

void app_usb_hid_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    USB_PRINT_TRACE1("app_usb_hid_timeout_cb param %x", param);
    app_stop_timer(&timer_idx_usb_hid);
    uint8_t mouse_data[4] = {0, 11, 11};
    app_cfu_usb_hid_send_report(1, 0, mouse_data, 4);
    uint8_t keyboard_data[12] = {0, 0x0a};
    app_cfu_usb_hid_send_report(0, 0, keyboard_data, 10);
    void os_delay(uint32_t ms);
    os_delay(10);
    keyboard_data[1] = 0;
    app_usb_hid_send_report(0, 0, keyboard_data, 11);
    app_start_timer(&timer_idx_usb_hid, "usb_hid",
                    app_usb_hid_timer_id, APP_TIMER_USB_HID, timer_chann + 1, false,
                    1000);
}
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
void app_cfu_usb_teams_handle_set_report(uint8_t *data, uint16_t length)
{
    uint8_t cfu_rsp[17] = {0};

    length -= 1;

    switch (data[0])
    {
    case REPORT_ID_CFU_OFFER_OUTPUT:
        {
            FWUPDATE_OFFER_RESPONSE offerRsp = {0};

            app_teams_cfu_save_report_info(APP_HID_REPORT_SOURCE_USB, APP_HID_REPORT_ACTION_NONE,
                                           0);

            if (app_teams_cfu_offer_parse(&data[1], length, &offerRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }

            cfu_rsp[0] = REPORT_ID_CFU_OFFER_INPUT;
            memcpy(&cfu_rsp[1], (uint8_t *)&offerRsp, sizeof(FWUPDATE_OFFER_RESPONSE));
            break;
        }
    case REPORT_ID_CFU_PAYLOAD_OUTPUT:
        {
            FWUPDATE_CONTENT_RESPONSE contentRsp = {0};

            if (app_teams_cfu_content_parse(&data[1], length, &contentRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }

            cfu_rsp[0] = REPORT_ID_CFU_PAYLOAD_INPUT;
            memcpy(&cfu_rsp[1], (uint8_t *)&contentRsp, sizeof(FWUPDATE_CONTENT_RESPONSE));
            break;
        }
    default:
        return;
    }

    addin_hid_interrupt_in(cfu_rsp, sizeof(cfu_rsp));
}
#endif

void app_cfu_usb_addin_hid_handle_set_report(uint8_t *data, uint16_t length)
{
    switch (data[0])
    {
    case REPORT_ID_CFU_TEST:
    case REPORT_ID_CFU_OFFER_OUTPUT:
    case REPORT_ID_CFU_PAYLOAD_OUTPUT:
#if F_APP_TEAMS_FEATURE_SUPPORT
        app_cfu_usb_teams_handle_set_report(data, length);
#else
        app_cfu_handle_set_report(data, length);
#endif
        break;
#if HID_DEBUG_INTERFACE_SUPPORT
    case REPORT_ID_HID_DEBUG_INTERFACE_OUT:
        app_cfu_usb_hid_handle_debug_data(data, length);
        break;
#endif
    default:
        USB_PRINT_ERROR3("app_usb_hid_set_report unknow report ID %x, length 0x%x, %b", data[0], length,
                         TRACE_BINARY(length, data));
        break;
    }
}

bool app_cfu_usb_addin_hid_handle_get_report(uint8_t reportID, uint8_t *data, uint16_t *length)
{

#if hid_debug
    app_timer_reg_cb(app_usb_hid_timeout_cb, &app_usb_hid_timer_id);
    app_start_timer(&timer_idx_usb_hid, "usb_hid",
                    app_usb_hid_timer_id, APP_TIMER_USB_HID, 0, false,
                    5000);
#endif
    switch (reportID)
    {
    case REPORT_ID_CFU_FEATURE:
    case REPORT_ID_CFU_FEATURE_EX:
#if F_APP_TEAMS_FEATURE_SUPPORT
        data[0] = REPORT_ID_CFU_FEATURE;
        app_teams_cfu_get_version_parse((FWUPDATE_VER_RESPONSE *)&data[1]);
        *length =  sizeof(FWUPDATE_VER_RESPONSE) + 1;
#else
        app_cfu_handle_get_report(data, length);
#endif
        break;
    case REPORT_ID_ASP_FEATURE:
        USB_PRINT_ERROR1("app_usb_hid_get_report ASP report ID %x", reportID);
        break;
    default:
        USB_PRINT_ERROR1("app_usb_hid_get_report unknow report ID %x", reportID);
        break;
    }

    return true;
}

#if HID_DEBUG_INTERFACE_SUPPORT
#define HID_DEBUG_INTERFACE_MAX_PKG_SIZE 58
void app_cfu_usb_hid_send_debug_data(uint8_t type, uint8_t *data, uint8_t length)
{
    int i;
    memset(app_usb_hid_interrupt_in_buff, 0, APP_USB_HID_MAX_IN_SIZE);
    app_usb_hid_interrupt_in_buff[0] = REPORT_ID_HID_DEBUG_INTERFACE_IN;
    app_usb_hid_interrupt_in_buff[1] = type;
    app_usb_hid_interrupt_in_buff[2] = (length < HID_DEBUG_INTERFACE_MAX_PKG_SIZE) ? length :
                                       HID_DEBUG_INTERFACE_MAX_PKG_SIZE;
    for (i = 0; i < app_usb_hid_interrupt_in_buff[2]; i++)
    {
        app_usb_hid_interrupt_in_buff[i + 3] = data[i];
    }
    addin_hid_interrupt_in(app_usb_hid_interrupt_in_buff, HID_DEBUG_INTERFACE_MAX_PKG_SIZE + 3);
    USB_PRINT_TRACE1("app_cfu_usb_hid_send_debug_data %b",
                     TRACE_BINARY(app_usb_hid_interrupt_in_buff[2] + 2, app_usb_hid_interrupt_in_buff));
}

void app_cfu_usb_hid_handle_debug_data(uint8_t *data, uint8_t length)
{
    USB_PRINT_TRACE1("app_cfu_usb_hid_handle_debug_data %b", TRACE_BINARY(length, data));
    switch (data[1])
    {
    case 1:
        {
            app_cfu_usb_hid_send_debug_data(data[2], &data[4], data[3]);
        }
    }

}
#endif

#endif
