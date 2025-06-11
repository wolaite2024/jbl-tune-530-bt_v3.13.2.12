#ifndef __HID_DESC_H__
#define __HID_DESC_H__
#include <stdint.h>

#define USB_CLASS_CODE_HID                  0x03

#define DESC_TYPE_HID                       0x21
#define DESC_TYPE_REPORT                    0x22

#define HID_REPORT_ID_AUDIO_CONTROL         0x01
#define HID_REPORT_DESC_CONSUMER_CONTROL  \
    0x05, 0x0C,                         /* Usage Page (Consumer)            */ \
          0x09, 0x01,                         /* Usage (Consumer Control)         */ \
          0xA1, 0x01,                         /* Collection (Application)         */ \
          0x85, HID_REPORT_ID_AUDIO_CONTROL,  /* Report ID (1)                    */ \
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
          0x09, 0xB5,                         /* Usage (Scan Next Track)          */ \
          0x81, 0x02,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0x09, 0xB6,                         /* Usage (Scan Previous Track)      */ \
          0x81, 0x02,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0x95, 0x03,                         /* Report Count (3)                 */ \
          0x81, 0x01,                         /* Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)*/ \
          0xC0,                               /* END_COLLECTION                    */

#define HID_REPORT_DESCS        HID_REPORT_DESC_CONSUMER_CONTROL

#endif
