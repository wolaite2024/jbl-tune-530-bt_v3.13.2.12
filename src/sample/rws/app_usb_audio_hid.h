/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_USB_AUDIO_HID_H_
#define _APP_USB_AUDIO_HID_H_
#include "stdint.h"
#include "app_msg.h"
#include "app_hid_descriptor.h"


#define USB_LINK_INDEX      0xFF
#define USB_AUDIO_REPORT_TYPE_HID           0x01

#define REPORT_ID_USB_COMMAND_SET           REPORT_ID_APP_CMD_FEATURE
#define REPORT_ID_USB_COMMAND_GET           REPORT_ID_APP_CMD_INPUT

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

#define USB_HID_MSG_TYPE_HID_GET_REPORT             0xE0
#define USB_HID_MSG_TYPE_HID_SET_REPORT             0xF0
#define USB_HID_MSG_TYPE_HID_SUSPEND_RESUME         0xF1
#define USB_HID_MSG_TYPE_ASP                        REPORT_ID_ASP_FEATURE
#define USB_HID_MSG_TYPE_HID_IN_REQUEST             0xF2
#define USB_HID_MSG_TYPE_HID_IN_COMPLETE            0xF3

#define BUTTON_VOL_PLAY_CTRL_HID_DESC_ATTRIB_2  \
    0x05, 0x0C,                         /* Usage Page (Consumer)            */ \
          0x09, 0x01,                         /* Usage (Consumer Control)         */ \
          0xA1, 0x01,                         /* Collection (Application)         */ \
          0x85, 0x01,                         /* Report ID (1)                    */ \
          0x15, 0x00,                         /* Logical Minimum (0)              */ \
          0x25, 0x01,                         /* Logical Maximum (1)              */ \
          0x75, 0x01,                         /* Report Size (1)                  */ \
          0x95, 0x01,                         /* Report Count (1)                 */ \
          0x09, 0xE9,                         /* Usage (Volume Increment)         */ \
          0x81, 0x02,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0x09, 0xEA,                         /* Usage (Volume Decrement)         */ \
          0x81, 0x02,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0x09, 0xCD,                         /* Usage (Play/Pause)               */ \
          0x81, 0x02,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0x95, 0x05,                         /* Report Count (5)                 */ \
          0x81, 0x01,                         /* Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0xC0                               /* END_COLLECTION                   */

/**  @brief  action ID parameter for @ref app_usb_mmi_handle_action */
typedef enum
{
    MMI_USB_NULL                        = 0x00,

    MMI_USB_AUDIO_VOL_UP                = 0x30,
    MMI_USB_AUDIO_VOL_DOWN              = 0x31,
    MMI_USB_AUDIO_PLAY_PAUSE            = 0x32,
    MMI_USB_AUDIO_STOP                  = 0x33,
    MMI_USB_AUDIO_FWD                   = 0x34,
    MMI_USB_AUDIO_BWD                   = 0x35,
    MMI_USB_AUDIO_FASTFORWARD           = 0x36,
    MMI_USB_AUDIO_REWIND                = 0x37,
    MMI_USB_AUDIO_FASTFORWARD_STOP      = 0x3B,
    MMI_USB_AUDIO_REWIND_STOP           = 0x3C,

    MMI_USB_MIC_VOL_UP                  = 0x15,
    MMI_USB_MIC_VOL_DOWN                = 0x16,
    MMI_USB_TEAMS_ANSWER_CALL           = 0x03,
    MMI_USB_TEAMS_REJECT_CALL           = 0x04,
    MMI_USB_TEAMS_END_ACTIVE_CALL       = 0x05,
    MMI_USB_TEAMS_CANCEL_VOICE_DIAL     = 0x0A,
    MMI_USB_TEAMS_SWITCH_TO_SECOND_CALL = 0x0C,
    MMI_USB_TEAMS_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL = 0x11,
    MMI_USB_TEAMS_MIC_MUTE_UNMUTE       = 0X06,

    MMI_USB_TEAMS_MIC_MUTE              = 0x07,
    MMI_USB_TEAMS_MIC_UNMUTE            = 0x08,
    MMI_USB_ENTER_PAIRING_MODE          = 0x6C,
    MMI_USB_TEAMS_BUTTON_SHORT_PRESS    = 0xA9,
    MMI_USB_TEAMS_BUTTON_LONG_PRESS     = 0xAA,
    MMI_USB_TEAMS_HOLD_CALL             = 0xAB,

    MMI_USB_TOTAL
} T_MMI_USB_ACTION;

typedef enum
{
    HID_IF_KEYBOARD,
    HID_IF_CONSUMER,
    HID_IF_MOUSE,
    HID_IF_HIDSYSCTRL,
    HID_IF_TELEPHONY,
    HID_IF_ASP,
    HID_IF_GATT,
    HID_IF_GAIA,
    HID_IF_CTRL,
    HID_IF_TEST
} SEND_TYPE;

typedef union
{
    uint8_t data[2];
    struct
    {
        uint8_t reportId;
        uint8_t offHook     : 1;
        uint8_t mute        : 1;
        uint8_t redial      : 1;
        uint8_t flash       : 1;
        uint8_t drop        : 1;
        uint8_t button1     : 1;
        uint8_t reserved    : 2;
    } report;
} TELEPHONY_HID_INPUT_REPORT;

typedef union
{
    uint8_t data[2];
    struct
    {
        uint8_t reportId;
        uint8_t offHook     : 1;
        uint8_t mute        : 1;
        uint8_t ring        : 1;
        uint8_t mic         : 1;
        uint8_t online      : 1;
        uint8_t hold        : 1;
        uint8_t reserved    : 2;
    } report;
} TELEPHONY_HID_OUTPUT_REPORT;

typedef enum t_usb_teams_call_status
{
    USB_TEAMS_CALL_IDLE                     = 0x00,
    USB_TEAMS_INCOMING_CALL_ONGOING         = 0x02,
    USB_TEAMS_OUTGOING_CALL_ONGOING         = 0x03,
    USB_TEAMS_CALL_ACTIVE                   = 0x04,
    USB_TEAMS_CALL_ACTIVE_WITH_CALL_WAITING = 0x05,
    USB_TEAMS_CALL_ACTIVE_WITH_CALL_HOLD    = 0x06,
} T_USB_TEAMS_CALL_STATUS;

T_USB_TEAMS_CALL_STATUS app_usb_teams_get_call_status(void);
void app_usb_hid_handle_msg(T_IO_MSG *msg);
void app_hid_interrupt_in(uint8_t *data, uint8_t data_size);
void app_usb_hid_init(void);
bool app_usb_audio_link_is_active(void);
void app_usb_audio_set_global_mute_status(bool mute_status);
bool app_usb_audio_get_global_mute_status(void);
void app_usb_audio_sync_mute_status_handle(void);
#endif
