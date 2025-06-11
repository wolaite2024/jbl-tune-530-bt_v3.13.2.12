#include "stdint.h"

#define HID_DEBUG_INTERFACE_SUPPORT 1

#define REPORT_ID_ASP_FEATURE   0x9A
#define REPORT_ID_ASP_INPUT     0x9B

#define USAGE_ID_ASP_FEATURE    0x03
#define USAGE_ID_ASP_INPUT      0x04

void app_cfu_usb_hid_send_debug_data(uint8_t type, uint8_t *data, uint8_t length);
void app_cfu_usb_hid_send_report(uint8_t channel, uint8_t report_id, uint8_t *data, uint8_t length);
