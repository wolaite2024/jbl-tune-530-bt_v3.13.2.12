#if F_APP_LEA_REALCAST_SUPPORT
#include <stdlib.h>
#include "trace.h"
#include "ble_audio_sync.h"
#include "app_a2dp.h"
#include "app_auto_power_off.h"
#include "app_bt_policy_api.h"
#include "app_cfg.h"
#include "app_cmd.h"
#include "app_hfp.h"
#include "app_link_util.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_mgr.h"
#include "app_lea_realcast.h"
#include "app_lea_vol_def.h"
#include "app_main.h"
#include "app_mmi.h"
#include "app_report.h"

#define REALCAST_COMMAND_ID  0x0E005D
#define REALCAST_VENDOR_ID   0x0000

static bool pa_report_notify_fg = false;
static uint8_t big_info_encryption = false;
static T_REALCAST_DEVICE_INFO realcast_device;

uint8_t app_lea_realcast_get_subgroup(void)
{
    return realcast_device.subgroup;
}

uint16_t app_lea_realcast_get_cmd_id(void)
{
    return realcast_device.cmd_id;
}

static uint8_t app_lea_realcast_get_cmd_path(void)
{
    return realcast_device.cmd_path;
}

static uint8_t app_lea_realcast_get_app_idx(void)
{
    return realcast_device.app_idx;
}

void app_lea_realcast_set_big_encryption(uint8_t state)
{
    big_info_encryption |= state;
}

void app_lea_realcast_stop_scan(void)
{
    if (app_lea_realcast_get_cmd_id() == CMD_LEA_SCAN_START)
    {
        if ((app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
            (app_lea_bca_state() == LEA_BCA_STATE_SCAN))
        {
            app_lea_bca_state_change(LEA_BCA_STATE_IDLE);
            app_lea_bca_scan_stop();
            app_lea_realcast_state_notify(CMD_LEA_SCAN_STOP, CMD_STATUS_SUCCESS, 0,
                                          PKT_TYPE_COMPLETE, NULL, 0);
        }
    }
}

void app_lea_realcast_state_notify(uint16_t cmd_id, uint8_t status, uint8_t state,
                                   uint8_t data_type, uint8_t *p_data, uint16_t data_len)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    bool handle = true;

    switch (cmd_id)
    {
    case CMD_LEA_SYNC_BIS:
        {
            uint8_t data[2];
            uint8_t *p_buf = data;

            LE_UINT8_TO_STREAM(p_buf, status);
            LE_UINT8_TO_STREAM(p_buf, state);
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_SYNC_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 2);
        }
        break;

    case CMD_LEA_SYNC_CANCEL:
        {
            uint8_t data[1];
            uint8_t *p_buf = data;

            LE_UINT8_TO_STREAM(p_buf, status);
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_SYNC_CANCEL_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 1);
        }
        break;

    case CMD_LEA_SCAN_START:
        {
            uint8_t *p_buf = NULL;
            uint8_t buf_len = data_len + 3;
            T_LE_EXT_ADV_REPORT_INFO *p_report = (T_LE_EXT_ADV_REPORT_INFO *)p_data;

            p_buf = malloc(buf_len);
            if (p_buf != NULL)
            {
                uint8_t *p_temp = p_buf;

                LE_UINT8_TO_STREAM(p_buf, status);
                LE_UINT8_TO_STREAM(p_buf, state);
                LE_UINT8_TO_STREAM(p_buf, p_report->data_status);
                LE_UINT16_TO_STREAM(p_buf, p_report->event_type);
                LE_UINT8_TO_STREAM(p_buf, p_report->addr_type);
                memcpy(p_buf, p_report->bd_addr, 6);
                p_buf += 6;
                LE_UINT8_TO_STREAM(p_buf, p_report->primary_phy);
                LE_UINT8_TO_STREAM(p_buf, p_report->secondary_phy);
                LE_UINT8_TO_STREAM(p_buf, p_report->adv_sid);
                LE_UINT8_TO_STREAM(p_buf, p_report->tx_power);
                LE_UINT8_TO_STREAM(p_buf, p_report->rssi);
                LE_UINT16_TO_STREAM(p_buf, p_report->peri_adv_interval);
                LE_UINT8_TO_STREAM(p_buf, p_report->direct_addr_type);
                memcpy(p_buf, p_report->direct_addr, 6);
                p_buf += 6;
                LE_UINT8_TO_STREAM(p_buf, p_report->data_len);
                memcpy(p_buf, p_report->p_data, p_report->data_len);

                app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_SCAN_NOTIFY,
                                 app_lea_realcast_get_app_idx(), p_temp, buf_len);
                APP_PRINT_TRACE1("app_lea_realcast_state_notify: EA notify %b", TRACE_BINARY(buf_len, p_temp));
                free(p_temp);
            }
            else
            {
                uint8_t data[2];

                p_buf = data;

                LE_UINT8_TO_STREAM(p_buf, CMD_STATUS_NO_RESOURCE);
                LE_UINT8_TO_STREAM(p_buf, state);

                app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_SCAN_NOTIFY,
                                 app_lea_realcast_get_app_idx(), data, 2);

            }
        }
        break;

    case CMD_LEA_SCAN_STOP:
        {
            uint8_t data[1];
            uint8_t *p_buf = data;

            if (realcast_device.cmd_id == CMD_LEA_SCAN_START)
            {
                realcast_device.cmd_id = CMD_LEA_SCAN_STOP;
            }

            LE_UINT8_TO_STREAM(p_buf, status);
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_SCAN_STOP_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 1);
        }
        break;

    case CMD_LEA_PA_START:
        {
            if (pa_report_notify_fg && big_info_encryption)
            {
                uint8_t *p_para = NULL;
                uint8_t para_len = 0;
                uint8_t *p_buf = NULL;
                uint8_t buf_len = data_len + 7 + REALCAST_DEVICE_LEN;

                p_buf = malloc(buf_len);
                if (p_buf != NULL)
                {
                    uint8_t *p_temp = p_buf;

                    LE_UINT8_TO_STREAM(p_buf, status);
                    LE_UINT8_TO_STREAM(p_buf, state);
                    LE_UINT8_TO_STREAM(p_buf, (big_info_encryption & REALCAST_ENCRYPTION_YES));
                    if (app_lea_realcast_get_data(REALCAST_DEVICE, &p_para, &para_len))
                    {
                        uint8_t len;

                        len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                        LE_UINT8_TO_STREAM(p_buf, len + 2);
                        LE_UINT8_TO_STREAM(p_buf, len + 1);
                        LE_UINT8_TO_STREAM(p_buf, REALCAST_DEVICE);
                        memcpy(p_buf, p_para, len);
                        p_buf += len;
                    }
                    LE_UINT8_TO_STREAM(p_buf, data_type);
                    memcpy(p_buf, p_data, data_len);

                    app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_PA_NOTIFY,
                                     app_lea_realcast_get_app_idx(), p_temp, buf_len);
                    APP_PRINT_TRACE1("app_lea_realcast_state_notify: PA notify %b", TRACE_BINARY(buf_len, p_temp));
                    free(p_temp);
                }
                else
                {
                    uint8_t data[2];

                    p_buf = data;

                    LE_UINT8_TO_STREAM(p_buf, CMD_STATUS_NO_RESOURCE);
                    LE_UINT8_TO_STREAM(p_buf, state);
                    app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_PA_NOTIFY,
                                     app_lea_realcast_get_app_idx(), data, 2);

                }
                pa_report_notify_fg = false;
            }
        }
        break;

    case CMD_LEA_PA_STOP:
        {
            uint8_t data[1];
            uint8_t *p_buf = data;

            if (realcast_device.cmd_id == CMD_LEA_PA_START)
            {
                realcast_device.cmd_id = CMD_LEA_PA_STOP;
            }

            LE_UINT8_TO_STREAM(p_buf, status);
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_PA_STOP_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 1);
        }
        break;

    case CMD_LEA_CTRL_VOL:
        {
            uint8_t *p_para = NULL;
            uint8_t para_len = 0;
            uint8_t data[10];
            uint8_t *p_buf = data;

            LE_UINT8_TO_STREAM(p_buf, state);
            LE_UINT8_TO_STREAM(p_buf, *p_data);
            if (app_lea_realcast_get_data(REALCAST_DEVICE, &p_para, &para_len))
            {
                uint8_t len;

                len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                LE_UINT8_TO_STREAM(p_buf, len + 1);
                LE_UINT8_TO_STREAM(p_buf, REALCAST_DEVICE);
                memcpy(p_buf, p_para, len);
            }
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_DEVICE_STATE_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 10);
        }
        break;

    case CMD_LEA_GET_DEVICE_STATE:
        {
            uint8_t *p_para = NULL;
            uint8_t para_len = 0;
            uint8_t data[10];
            uint8_t *p_buf = data;

            LE_UINT8_TO_STREAM(p_buf, state);
            LE_UINT8_TO_STREAM(p_buf, *p_data);
            if (app_lea_realcast_get_data(REALCAST_DEVICE, &p_para, &para_len))
            {
                uint8_t len;

                len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                LE_UINT8_TO_STREAM(p_buf, len + 1);
                LE_UINT8_TO_STREAM(p_buf, REALCAST_DEVICE);
                memcpy(p_buf, p_para, len);
            }
            app_report_event(app_lea_realcast_get_cmd_path(), EVENT_LEA_DEVICE_STATE_NOTIFY,
                             app_lea_realcast_get_app_idx(), data, 10);
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE3("app_lea_realcast_state_notify: cmd_id 0x%02X, status 0x%02X, state 0x%02X",
                         cmd_id,
                         status, state);
    }
}

bool app_lea_realcast_filter_service_data(T_REALCAST_SCAN_ADV_INFO adv_info,
                                          uint8_t *p_service_data, uint16_t *p_data_len)
{
    uint8_t *p_buffer = NULL;
    uint16_t pos = 0;

    while (pos < adv_info.report_data_len)
    {
        /* Length of the AD structure. */
        uint16_t length = adv_info.p_report_data[pos++];

        if (length <= 1)
        {
            return false;
        }

        if ((pos + length) <= adv_info.report_data_len)
        {
            uint8_t type;

            /* Copy the AD Data to buffer. */
            p_buffer = adv_info.p_report_data + pos + 1;
            /* AD Type, one octet. */
            type = adv_info.p_report_data[pos];
            APP_PRINT_TRACE2("app_lea_realcast_filter_service_data: len 0x%02X, type 0x%02X", length, type);

            switch (type)
            {
            case GAP_ADTYPE_MANUFACTURER_SPECIFIC:
                {
                    uint32_t cmd_id;
                    uint16_t vendor;

                    LE_STREAM_TO_UINT24(cmd_id, p_buffer);
                    LE_STREAM_TO_UINT16(vendor, p_buffer);
                    APP_PRINT_TRACE2("app_lea_realcast_filter_service_data: cmd_id 0x%02X, vendor 0x%02X", cmd_id,
                                     vendor);
                    if ((adv_info.filter == ADV_FILTER_REALCAST) && (cmd_id == REALCAST_COMMAND_ID) &&
                        (vendor == REALCAST_VENDOR_ID))
                    {
                        if (p_service_data != NULL)
                        {
                            memcpy(p_service_data, p_buffer, REALCAST_DEVICE_LEN);
                        }

                        *p_data_len = REALCAST_DEVICE_LEN;
                        APP_PRINT_TRACE1("app_lea_realcast_filter_service_data: GAP_ADTYPE_MANUFACTURER_SPECIFIC data %b",
                                         TRACE_BINARY(*p_data_len, p_service_data));
                        return true;
                    }
                }
                break;

            case GAP_ADTYPE_BROADCAST_NAME:
                {
                    if (adv_info.filter == ADV_FILTER_NAME)
                    {
                        if (p_service_data != NULL)
                        {
                            memcpy(p_service_data, p_buffer, length - 1);
                        }

                        *p_data_len = length - 1;
                        APP_PRINT_TRACE1("app_lea_realcast_filter_service_data: GAP_ADTYPE_BROADCAST_NAME data %b",
                                         TRACE_BINARY(*p_data_len, p_service_data));
                        return true;
                    }
                }
                break;

            default:
                break;
            }
        }

        pos += length;
    }
    return false;
}

bool app_lea_realcast_get_data(T_REALCAST_LTV_TYPE type_para, uint8_t **p_para, uint8_t *para_len)
{
    bool ret = true;

    switch (type_para)
    {
    case BROADCAST_NAME:
        {
            *para_len = sizeof(realcast_device.bs_name);
            *p_para = realcast_device.bs_name;
        }
        break;

    case BROADCAST_CODE:
        {
            *para_len = REALCAST_BS_CODE_LEN;
            *p_para = realcast_device.bs_code;
        }
        break;

    case REALCAST_DEVICE:
        {
            *para_len = REALCAST_DEVICE_LEN;
            *p_para = realcast_device.bd_addr;
        }
        break;

    default:
        ret = false;
        break;
    }
    return ret;
}

static bool app_lea_realcast_parse_data(T_REALCAST_LTV_TYPE type_para, uint8_t *p_data,
                                        uint8_t data_len, uint8_t **p_para, uint8_t *para_len)
{
    uint8_t pos = 0;

    while (pos < data_len)
    {
        uint8_t len = p_data[pos++];

        if (len <= 1)
        {
            return false;
        }

        if ((pos + len) <= data_len)
        {
            uint8_t type;

            type = p_data[pos];
            if (type == type_para)
            {
                *para_len = len - 1;
                *p_para = &p_data[pos + 1];
                return true;
            }
        }

        pos += len;
    }
    return false;
}

static void app_lea_realcast_dump_device_info(void)
{
    APP_PRINT_TRACE5("app_lea_realcast_dump_device_info: app_idx 0x%02X, subgroup 0x%02X, bs_name %b, bs_code %b, bd_addr %b",
                     realcast_device.app_idx, realcast_device.subgroup,
                     TRACE_BINARY(REALCAST_BS_NAME_LEN, realcast_device.bs_name),
                     TRACE_BINARY(REALCAST_BS_CODE_LEN, realcast_device.bs_code),
                     TRACE_BINARY(REALCAST_DEVICE_LEN, realcast_device.bd_addr));
}

static void app_lea_realcast_clear_device_info(void)
{
    memset(&realcast_device, 0x00, sizeof(T_REALCAST_DEVICE_INFO));
}

bool app_lea_realcast_relay_device_info(void)
{
    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_DEVICE_SYNC,
                                       (uint8_t *)(&realcast_device), sizeof(T_REALCAST_DEVICE_INFO),
                                       REMOTE_TIMER_HIGH_PRECISION, 0, true);
    return true;
}

static bool app_lea_realcast_update_device_info(T_REALCAST_DEVICE_INFO *device_info)
{
    memcpy(&realcast_device, device_info, sizeof(T_REALCAST_DEVICE_INFO));
    app_lea_realcast_dump_device_info();
    return true;
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_lea_realcast_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_REALCAST, 0, NULL, true, total);
}

static void app_lea_realcast_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                         T_REMOTE_RELAY_STATUS status)
{
    bool handle = true;

    switch (msg_type)
    {
    case REALCAST_REMOTE_DEVICE_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_lea_realcast_update_device_info((T_REALCAST_DEVICE_INFO *)buf);
                }
            }
        }
        break;

    case REALCAST_REMOTE_PA_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_LEA_BRS_INFO *p_src_info = (T_LEA_BRS_INFO *)buf;

                app_lea_bca_scan_start(app_cfg_const.scan_to * 1000);
                mtc_topology_dm(MTC_TOPO_EVENT_BIS_START);
                app_lea_bca_state_change(LEA_BCA_STATE_SCAN);
                app_lea_bca_scan_info(p_src_info);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE2("app_lea_realcast_parse_cback: msg_type 0x%02X, status 0x%02X", msg_type, status);
    }
}
#endif

void app_lea_realcast_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                 uint8_t app_idx, uint8_t *ack_pkt)
{
    bool handle = true;
    uint16_t cmd_id;

    if (app_bt_policy_get_call_status() != APP_CALL_IDLE)
    {
        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        return;
    }

    LE_STREAM_TO_UINT16(cmd_id, cmd_ptr);

    switch (cmd_id)
    {
    case CMD_LEA_SYNC_BIS:
        {
            if (app_lea_bca_state() == LEA_BCA_STATE_IDLE)
            {
                uint8_t len;
                uint16_t timeout;
                uint8_t *p_para = NULL;
                uint8_t para_len = 0;

                app_lea_realcast_clear_device_info();
                LE_STREAM_TO_UINT8(realcast_device.subgroup, cmd_ptr);
                LE_STREAM_TO_UINT16(timeout, cmd_ptr);
                if (app_lea_realcast_parse_data(BROADCAST_NAME, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_BS_NAME_LEN) ? REALCAST_BS_NAME_LEN : para_len;
                    memcpy(realcast_device.bs_name, p_para, len);
                }

                if (app_lea_realcast_parse_data(BROADCAST_CODE, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_BS_CODE_LEN) ? REALCAST_BS_CODE_LEN : para_len;
                    memcpy(realcast_device.bs_code, p_para, len);
                }

                if (app_lea_realcast_parse_data(REALCAST_DEVICE, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                    memcpy(realcast_device.bd_addr, p_para, len);
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;

                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_DEVICE_SYNC,
                                                       (uint8_t *)(&realcast_device), sizeof(T_REALCAST_DEVICE_INFO),
                                                       REMOTE_TIMER_HIGH_PRECISION, 0, true);

                    app_relay_sync_single(APP_MODULE_TYPE_MMI, MMI_BIG_START, REMOTE_TIMER_HIGH_PRECISION,
                                          0, false);
                }
                else
                {
                    app_lea_mgr_mmi_handle(MMI_BIG_START);
                }
                app_lea_realcast_state_notify(cmd_id, CMD_STATUS_SUCCESS, SYNC_STATE_SEARCHING, PKT_TYPE_COMPLETE,
                                              NULL, 0);
                APP_PRINT_TRACE6("app_lea_realcast_cmd_handle: LEA_BCA_STATE_IDLE cmd_id 0x%02X, subgroup 0x%02X, timeout 0x%02X, %b, %b, %b",
                                 cmd_id, realcast_device.subgroup, timeout,
                                 TRACE_BINARY(40, realcast_device.bs_name),
                                 TRACE_BINARY(16, realcast_device.bs_code),
                                 TRACE_BINARY(6, realcast_device.bd_addr));
            }
            else if ((app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
                     (app_lea_bca_state() == LEA_BCA_STATE_SCAN))
            {
                uint8_t len;
                uint16_t timeout;
                uint8_t *p_para = NULL;
                uint8_t para_len = 0;

                app_lea_realcast_clear_device_info();
                LE_STREAM_TO_UINT8(realcast_device.subgroup, cmd_ptr);
                LE_STREAM_TO_UINT16(timeout, cmd_ptr);
                if (app_lea_realcast_parse_data(BROADCAST_NAME, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_BS_NAME_LEN) ? REALCAST_BS_NAME_LEN : para_len;
                    memcpy(realcast_device.bs_name, p_para, len);
                }

                if (app_lea_realcast_parse_data(BROADCAST_CODE, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_BS_CODE_LEN) ? REALCAST_BS_CODE_LEN : para_len;
                    memcpy(realcast_device.bs_code, p_para, len);
                }

                if (app_lea_realcast_parse_data(REALCAST_DEVICE, cmd_ptr, cmd_len - 5, &p_para, &para_len))
                {
                    len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                    memcpy(realcast_device.bd_addr, p_para, len);
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;

                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_DEVICE_SYNC,
                                                       (uint8_t *)(&realcast_device), sizeof(T_REALCAST_DEVICE_INFO),
                                                       REMOTE_TIMER_HIGH_PRECISION, 0, true);
                }
                APP_PRINT_TRACE6("app_lea_realcast_cmd_handle: cmd_id 0x%02X, subgroup 0x%02X, timeout 0x%02X, %b, %b, %b",
                                 cmd_id, realcast_device.subgroup, timeout,
                                 TRACE_BINARY(40, realcast_device.bs_name),
                                 TRACE_BINARY(16, realcast_device.bs_code),
                                 TRACE_BINARY(6, realcast_device.bd_addr));
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_SYNC_CANCEL:
        {
            if (app_lea_bca_state() != LEA_BCA_STATE_IDLE)
            {
                uint8_t *p_para = NULL;
                uint8_t para_len = 0;

                if (app_lea_realcast_parse_data(REALCAST_DEVICE, cmd_ptr, cmd_len, &p_para, &para_len))
                {
                    uint8_t len;
                    uint8_t temp_realcast_device[REALCAST_DEVICE_LEN];

                    len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                    memcpy(temp_realcast_device, p_para, len);
                    APP_PRINT_TRACE2("app_lea_realcast_cmd_handle: realcast device %b, %b",
                                     TRACE_BINARY(len, realcast_device.bd_addr),
                                     TRACE_BINARY(len, temp_realcast_device));
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;

                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_sync_single(APP_MODULE_TYPE_MMI, MMI_BIG_STOP, REMOTE_TIMER_HIGH_PRECISION,
                                          0, false);

                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_DEVICE_SYNC,
                                                       (uint8_t *)(&realcast_device), sizeof(T_REALCAST_DEVICE_INFO),
                                                       REMOTE_TIMER_HIGH_PRECISION, 0, true);
                }
                else
                {
                    app_lea_mgr_mmi_handle(MMI_BIG_STOP);
                }
                app_lea_realcast_state_notify(cmd_id, CMD_STATUS_SUCCESS, 0, PKT_TYPE_COMPLETE, NULL, 0);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_SCAN_START:
        {
            if ((app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
                (app_lea_bca_state() == LEA_BCA_STATE_SCAN) ||
                (app_lea_bca_state() == LEA_BCA_STATE_STREAMING))
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t filter;
                uint16_t timeout;
                uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;
                LE_STREAM_TO_UINT8(filter, cmd_ptr);
                LE_STREAM_TO_UINT16(timeout, cmd_ptr);

                if (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    bt_avrcp_pause(app_db.br_link[active_a2dp_idx].bd_addr);
                }
                app_lea_bca_state_change(LEA_BCA_STATE_SCAN);
                app_lea_bca_scan_start(timeout * 1000);
                APP_PRINT_TRACE2("app_lea_realcast_cmd_handle: filter 0x%02X, timeout 0x%02X", filter, timeout);
            }
        }
        break;

    case CMD_LEA_SCAN_STOP:
        {
            if ((app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
                (app_lea_bca_state() == LEA_BCA_STATE_SCAN))
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;
                app_lea_bca_state_change(LEA_BCA_STATE_IDLE);
                app_lea_bca_scan_stop();
                app_lea_realcast_state_notify(cmd_id, CMD_STATUS_SUCCESS, 0, PKT_TYPE_COMPLETE, NULL, 0);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_PA_START:
        {
            if ((app_lea_bca_state() == LEA_BCA_STATE_IDLE) ||
                (app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
                (app_lea_bca_state() == LEA_BCA_STATE_SCAN))
            {
                uint8_t mode;
                uint16_t timeout;
                uint32_t broadcast_id;
                uint8_t adv_addr_type;
                uint8_t advertiser_sid;
                uint8_t *p_para = NULL;
                uint8_t para_len = 0;

                LE_STREAM_TO_UINT8(mode, cmd_ptr);
                LE_STREAM_TO_UINT16(timeout, cmd_ptr);
                LE_STREAM_TO_UINT24(broadcast_id, cmd_ptr);
                LE_STREAM_TO_UINT8(adv_addr_type, cmd_ptr);
                LE_STREAM_TO_UINT8(advertiser_sid, cmd_ptr);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if (app_lea_realcast_parse_data(REALCAST_DEVICE, cmd_ptr, cmd_len - 10, &p_para, &para_len))
                {
                    uint8_t len;
                    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

                    pa_report_notify_fg = true;
                    big_info_encryption = false;
                    len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                    memcpy(realcast_device.bd_addr, p_para, len);
                    realcast_device.app_idx = app_idx;
                    realcast_device.cmd_path = cmd_path;
                    realcast_device.cmd_id = cmd_id;

                    T_LEA_BRS_INFO src_info = {0};
                    src_info.adv_addr_type =  adv_addr_type;
                    src_info.advertiser_sid = advertiser_sid;
                    memcpy(src_info.broadcast_id, (uint8_t *)&broadcast_id, 3);
                    len = (para_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : para_len;
                    memcpy(src_info.adv_addr, p_para, len);

                    if (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        bt_avrcp_pause(app_db.br_link[active_a2dp_idx].bd_addr);
                    }

                    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_DEVICE_SYNC,
                                                           (uint8_t *)(&realcast_device), sizeof(T_REALCAST_DEVICE_INFO),
                                                           REMOTE_TIMER_HIGH_PRECISION, 0, true);

                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_PA_SYNC,
                                                           (uint8_t *)(&src_info), sizeof(T_LEA_BRS_INFO),
                                                           REMOTE_TIMER_HIGH_PRECISION, 0, true);
                    }
                    else
                    {
                        app_lea_bca_scan_start(app_cfg_const.scan_to * 1000);
                        mtc_topology_dm(MTC_TOPO_EVENT_BIS_START);
                        app_lea_bca_state_change(LEA_BCA_STATE_SCAN);
                        app_lea_bca_scan_info(&src_info);
                    }

                    APP_PRINT_TRACE5("app_lea_realcast_cmd_handle: mode 0x%02X, \
timeout 0x%02X, broadcast_id %b, adv_addr_type 0x%02X, advertiser_sid 0x%02X",
                                     mode, timeout, TRACE_BINARY(3, src_info.broadcast_id),
                                     adv_addr_type, advertiser_sid);
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_PA_STOP:
        {
            if ((app_lea_bca_state() == LEA_BCA_STATE_PRE_SCAN) ||
                (app_lea_bca_state() == LEA_BCA_STATE_SCAN))
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                realcast_device.app_idx = app_idx;
                realcast_device.cmd_path = cmd_path;
                realcast_device.cmd_id = cmd_id;

                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_sync_single(APP_MODULE_TYPE_MMI, MMI_BIG_STOP, REMOTE_TIMER_HIGH_PRECISION,
                                          0, false);
                }
                else
                {
                    app_lea_mgr_mmi_handle(MMI_BIG_STOP);
                }
                app_lea_realcast_state_notify(cmd_id, CMD_STATUS_SUCCESS, 0, PKT_TYPE_COMPLETE, NULL, 0);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_CTRL_VOL:
        {
            if (app_lea_bca_state() == LEA_BCA_STATE_STREAMING)
            {
                T_MMI_ACTION action;
                uint8_t type;

                LE_STREAM_TO_UINT8(type, cmd_ptr);
                if (type == VOL_UP)
                {
                    action = MMI_DEV_SPK_VOL_UP;
                }
                else if (type == VOL_DOWN)
                {
                    action = MMI_DEV_SPK_VOL_DOWN;
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                                          0, false);
                }
                else
                {
                    app_lea_mgr_mmi_handle(action);
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_LEA_GET_DEVICE_STATE:
        {
            uint8_t value;
            uint8_t type;

            LE_STREAM_TO_UINT8(type, cmd_ptr);
            if (type == DEVICE_STREAM_STATE)
            {
                value = (app_lea_bca_state() == LEA_BCA_STATE_STREAMING) ? true : false;
            }
            else if (type == VOL_VALUE_STATE)
            {
                value = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            app_lea_realcast_state_notify(cmd_id, CMD_STATUS_SUCCESS, type, PKT_TYPE_COMPLETE, &value, 1);
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_realcast_cmd_handle: cmd_id 0x%02X", cmd_id);
    }
}

void app_lea_realcast_init(void)
{
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_lea_realcast_relay_cback, app_lea_realcast_parse_cback,
                             APP_MODULE_TYPE_REALCAST, REALCAST_REMOTE_MSG_TOTAL);
#endif
}
#endif
