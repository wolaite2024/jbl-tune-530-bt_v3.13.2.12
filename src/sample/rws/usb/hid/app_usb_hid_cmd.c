#if F_APP_USB_HID_SUPPORT
#include <stdint.h>
#include "trace.h"
#include "usb_hid_desc.h"
#include "usb_hid.h"
#include "btm.h"
#include "app_bt_policy_api.h"
#include "gap_vendor.h"

#define BT_MP_HCI_CMD_PASSTHROUGH_OPCODE            0x1014
#define HCI_VENDOR_MP_OPCODE                        0xFCEB

int8_t app_usb_hid_cmd_handle_get_report(T_HID_DRIVER_REPORT_REQ_VAL req_value, uint8_t *data,
                                         uint16_t *length)
{
    /* not implement yet */
    return 0;
}

int8_t app_usb_hid_cmd_handle_set_report(T_HID_DRIVER_REPORT_REQ_VAL req_value, uint8_t *data,
                                         uint16_t length)
{
    APP_PRINT_TRACE1("app_usb_hid_cmd_handle_set_report: data %b", TRACE_BINARY(length, data));

    uint16_t opcode;
    uint8_t *pp = data;
    uint16_t cmd_len;

    pp++;
    LE_STREAM_TO_UINT16(cmd_len, pp);
    LE_STREAM_TO_UINT16(opcode, pp);

    switch (opcode)
    {
    case BT_MP_HCI_CMD_PASSTHROUGH_OPCODE:
        {
            uint16_t hci_opcode;
            uint8_t cmd_pl_len = cmd_len - 4;
            BE_STREAM_TO_UINT16(hci_opcode, pp);

            APP_PRINT_INFO3("HCI MP CMD START: hci_opcode %x, data %b, len %d", hci_opcode,
                            TRACE_BINARY(cmd_pl_len, pp), cmd_pl_len);
            //usb cmd formate: big-endian and little-endian about byte order.
            if (hci_opcode == HCI_VENDOR_MP_OPCODE)
            {
                app_bt_policy_disconnect_all_link();
                bt_device_mode_set(BT_DEVICE_MODE_IDLE);
                gap_vendor_cmd_req(HCI_VENDOR_MP_OPCODE, cmd_pl_len, pp);
            }
        }
        break;
    }
    return 0;
}

void app_usb_hid_cmd_init(void)
{
    T_HID_CBS cbs = {.get_report = (INT_IN_FUNC)app_usb_hid_cmd_handle_get_report, .set_report = (INT_OUT_FUNC)app_usb_hid_cmd_handle_set_report};
    usb_hid_ual_register(cbs);
}
#endif

