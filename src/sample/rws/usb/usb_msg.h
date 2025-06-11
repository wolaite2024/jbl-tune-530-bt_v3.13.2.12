#ifndef __USB_MSG_H__
#define __USB_MSG_H__
#include <stdint.h>
#include "app_io_msg.h"

#define USB_IPC_TOPIC   "usb"

typedef enum
{
    USB_MSG_GROUP_DEV,
    USB_MSG_GROUP_IF_AUDIO,
    USB_MSG_GROUP_IF_HID,
    USB_MSG_GROUP_MAX
} T_USB_MSG_GROUP;

typedef enum
{
    USB_IPC_EVT_PLUG = 0,
    USB_IPC_EVT_UNPLUG,
    USB_IPC_EVT_PWR_CHG,

    USB_IPC_EVT_AUDIO_DS_START = 0x10,
    USB_IPC_EVT_AUDIO_DS_STOP,
    USB_IPC_EVT_AUDIO_US_START,
    USB_IPC_EVT_AUDIO_US_STOP,

    USB_IPC_EVT_SUSPEND = 0x20,
    USB_IPC_EVT_RESUME,
} T_USB_IPC_EVT;

#define IO_MSG_TYPE_USB IO_MSG_TYPE_USB_UAC

#define USB_MSG_GROUP_MASK      0xFF00
#define USB_MSG(group,submsg) (((group) << 8) | ((submsg) & 0xFF))
#define USB_MSG_GROUP(msg)  (((msg) & USB_MSG_GROUP_MASK) >> 8)
#define USB_MSG_SUBTYPE(msg) ((msg) & (~USB_MSG_GROUP_MASK))

#endif
