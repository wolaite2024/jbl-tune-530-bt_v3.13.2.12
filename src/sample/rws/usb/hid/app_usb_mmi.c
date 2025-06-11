#if F_APP_USB_HID_SUPPORT
#include <stdint.h>
#include <stddef.h>
#include "trace.h"
#include "usb_hid_desc.h"
#include "usb_hid.h"
#include "errno.h"
#include "app_mmi.h"

#define HID_MAX_TRANSMISSION_UNIT                   0x02
#define HID_MAX_PENDING_REQ_NUM                     0x0A

#define BIT_POS_VOL_UP        0
#define BIT_POS_VOL_DOWN      1
#define BIT_POS_PLAY_PAUSE    2
#define BIT_POS_NEXT_TRACK    3
#define BIT_POS_PREV_TRACK    4

static void *hid_mmi_handle = NULL;

int32_t app_usb_mmi_handle_action(uint8_t action)
{
    uint8_t bit_pos = 0;
    uint8_t report[2] = {HID_REPORT_ID_AUDIO_CONTROL, 0};
    int32_t ret = ESUCCESS;

    APP_PRINT_INFO1("app_hid_ac_handle_mmi, action:0x%x", action);
    switch (action)
    {
    case MMI_DEV_SPK_VOL_UP:
        {
            bit_pos = BIT_POS_VOL_UP;
        }
        break;

    case MMI_DEV_SPK_VOL_DOWN:
        {
            bit_pos = BIT_POS_VOL_DOWN;
        }
        break;

    case MMI_AV_PLAY_PAUSE:
        {
            bit_pos = BIT_POS_PLAY_PAUSE;
        }
        break;

    case MMI_AV_FWD:
        {
            bit_pos = BIT_POS_NEXT_TRACK;
        }
        break;

    case MMI_AV_BWD:
        {
            bit_pos = BIT_POS_PREV_TRACK;
        }
        break;

    default:
        {
            ret = -ESRCH;
            return ret;
        }
    }

    if (hid_mmi_handle == NULL)
    {
        T_USB_HID_ATTR attr =
        {
            .zlp = 1,
            .high_throughput = 0,
            .congestion_ctrl = HID_CONGESTION_CTRL_DROP_CUR,
            .rsv = 0,
            .mtu = HID_MAX_TRANSMISSION_UNIT
        };
        hid_mmi_handle = usb_hid_data_pipe_open(HID_INT_IN_EP_1, attr, HID_MAX_PENDING_REQ_NUM,
                                                NULL);
    }

    report[1] = 1 << bit_pos;
    usb_hid_data_pipe_send(hid_mmi_handle, report, 2);

    report[1] = 0;
    usb_hid_data_pipe_send(hid_mmi_handle, report, 2);

    return ret;
}

void app_usb_mmi_init(void)
{

}
#endif

