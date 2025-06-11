#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "app_main.h"
#include "app_harman_parser.h"
#include "app_harman_vendor_cmd.h"
#include "app_harman_report.h"
#include "app_harman_ble_ota.h"
#include "app_report.h"
#include "app_bt_policy_api.h"
#include "os_mem.h"
#include "bt_bond.h"
#include "audio.h"
#include "app_bt_policy_int.h"
#include "app_multilink.h"
#include "app_bond.h"
#include "bt_a2dp.h"
#include "app_mmi.h"
#include "app_ble_gap.h"
#include "app_cfg.h"
#include "app_ble_common_adv.h"
#include "transmit_service.h"
#include "app_transfer.h"
#include "app_ble_gap.h"
#include "gap_conn_le.h"

uint8_t *ota_buf;
uint16_t ota_len;
uint16_t ota_event_id = 0x00;
uint8_t *eq_buf;
uint16_t eq_len;

void app_harman_report_le_event(T_APP_LE_LINK *p_link, uint16_t event_id, uint8_t *data,
                                uint16_t len)
{
    if (p_link->state == LE_LINK_STATE_CONNECTED)
    {
        if (event_id == CMD_HARMAN_OTA_NOTIFICATION)
        {
            ota_len = len;
            ota_event_id = CMD_HARMAN_OTA_NOTIFICATION;

            ota_buf = malloc(ota_len + 8);
            if (ota_buf == NULL)
            {
                return;
            }

            ota_buf[0] = (uint8_t)(HARMAN_CMD_DIRECT_SEND & 0xFF);
            ota_buf[1] = (uint8_t)((HARMAN_CMD_DIRECT_SEND >> 8) & 0xFF);
            ota_buf[2] = (uint8_t)(event_id & 0xFF);
            ota_buf[3] = (uint8_t)((event_id >> 8) & 0xFF);
            ota_buf[4] = 1; // packet_count, min value is 1
            ota_buf[5] = 0; // packet_index
            ota_buf[6] = (uint8_t)(ota_len);
            ota_buf[7] = (uint8_t)((ota_len) >> 8);

            if (ota_len)
            {
                memcpy(&ota_buf[8], data, ota_len);
            }
            bool ota_notify_direct = app_harman_ble_ota_get_notify_direct();

            APP_PRINT_INFO3("app_report_le_ota_event_wait: ota_notify_direct: %d, ota_len: %d, %b",
                            ota_notify_direct, (ota_len + 8), TRACE_BINARY((ota_len + 8), ota_buf));
            if (ota_notify_direct)
            {
                if (app_transfer_push_data_queue(CMD_PATH_LE, ota_buf, (ota_len + 8), p_link->id) == false)
                {
                    free(ota_buf);
                }
                ota_event_id = 0x00;
                ota_notify_direct = false;
            }
        }
        else if ((event_id == CMD_HARMAN_DEVICE_INFO_GET) ||
                 (event_id == CMD_HARMAN_DEVICE_INFO_DEVICE_NOTIFY) ||
                 (event_id == CMD_HARMAN_DEVICE_INFO_SET))
        {
            uint8_t *buf;

            ota_event_id = 0x00;
            buf = malloc(len + 8);
            if (buf == NULL)
            {
                return;
            }
            buf[0] = (uint8_t)(HARMAN_CMD_DIRECT_SEND & 0xFF);
            buf[1] = (uint8_t)((HARMAN_CMD_DIRECT_SEND >> 8) & 0xFF);
            buf[2] = (uint8_t)(event_id & 0xFF);
            buf[3] = (uint8_t)((event_id >> 8) & 0xFF);
            buf[4] = 1; // packet_count, min value is 1
            buf[5] = 0; // packet_index
            buf[6] = (uint8_t)(len);
            buf[7] = (uint8_t)((len) >> 8);
            if (len)
            {
                memcpy(&buf[8], data, len);
            }
            APP_PRINT_INFO2("app_report_le_event: len: %d, %b", (len + 8), TRACE_BINARY((len + 8), buf));

            if (app_transfer_push_data_queue(CMD_PATH_LE, buf, (len + 8), p_link->id) == false)
            {
                free(buf);
            }
        }
    }
}

void app_harman_wait_ota_report(uint8_t conn_id)
{
    if ((ota_event_id == CMD_HARMAN_OTA_PACKET) ||
        (ota_event_id == CMD_HARMAN_OTA_START) ||
        (ota_event_id == CMD_HARMAN_OTA_STOP) ||
        (ota_event_id == CMD_HARMAN_OTA_NOTIFICATION))
    {
        APP_PRINT_TRACE3("app_report_le_ota_event: conn_id: %d, evenota_event_id: 0x%x, %b",
                         conn_id, ota_event_id, TRACE_BINARY((ota_len + 8), ota_buf));
        if (app_transfer_push_data_queue(CMD_PATH_LE, ota_buf, (ota_len + 8), conn_id) == false)
        {
            free(ota_buf);
        }
    }
    ota_event_id = 0x00;
}
#endif
