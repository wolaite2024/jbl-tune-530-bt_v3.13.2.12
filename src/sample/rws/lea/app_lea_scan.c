#include "string.h"
#include "trace.h"
#include "ble_audio_def.h"
#include "ble_scan.h"
#include "pbp_def.h"
#include "app_lea_mgr.h"
#include "app_lea_scan.h"
#include "app_lea_broadcast_audio.h"
#include "app_bt_policy_api.h"
#include "app_cfg.h"

#if F_APP_LEA_REALCAST_SUPPORT
#include "app_cmd.h"
#include "app_lea_realcast.h"
#endif

#if F_APP_TMAP_BMR_SUPPORT
typedef struct
{
    uint8_t report_data_len;
    uint8_t *p_report_data;
    uint16_t uuid;
} T_APP_LEA_SCAN_ADV_INFO;

static BLE_SCAN_HDL app_lea_scan_handle = NULL;

static bool app_lea_scan_filter_service_data(T_APP_LEA_SCAN_ADV_INFO adv_info,
                                             uint8_t **p_service_data,
                                             uint16_t *p_data_len)
{
    uint8_t *p_buffer = NULL;
    uint16_t pos = 0;
    bool found = false;

    while (pos < adv_info.report_data_len)
    {
        /* Length of the AD structure. */
        uint16_t length = adv_info.p_report_data[pos++];
        uint8_t type;

        if (length <= 1)
        {
            return false;
        }

        if ((pos + length) <= adv_info.report_data_len)
        {
            /* Copy the AD Data to buffer. */
            p_buffer = adv_info.p_report_data + pos + 1;
            /* AD Type, one octet. */
            type = adv_info.p_report_data[pos];

            switch (type)
            {
            case GAP_ADTYPE_SERVICE_DATA:
            case GAP_ADTYPE_16BIT_MORE: //just for pts
                {
                    uint16_t srv_uuid;

                    LE_STREAM_TO_UINT16(srv_uuid, p_buffer);

                    if (srv_uuid == adv_info.uuid)
                    {
                        *p_service_data = p_buffer;
                        *p_data_len = length - 3;
                        found = true;
                    }
                }
                break;

            default:
                break;
            }
        }
        else
        {
            APP_PRINT_TRACE0("app_lea_scan_filter_service_data format error");
            return false;
        }

        pos += length;
    }
    return found;
}

static bool app_lea_scan_report(T_LE_EXT_ADV_REPORT_INFO *p_report)
{
    int8_t error_code;
    uint8_t *p_service_data = NULL;
    uint16_t service_data_len = 0;
    uint8_t *p_pba_data = NULL;
    uint16_t pba_data_len = 0;
    T_LEA_BRS_INFO src_info = {0};
    T_APP_LEA_SCAN_ADV_INFO adv_info = {0};

    adv_info.report_data_len = p_report->data_len;
    adv_info.p_report_data = p_report->p_data;
    adv_info.uuid = BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID;
    if (!app_lea_scan_filter_service_data(adv_info, &p_service_data, &service_data_len))
    {
        error_code = 1;
        goto error;
    }

    APP_PRINT_TRACE2("app_lea_scan_report: state 0x%02X, bmr state 0x%02X",
                     app_bt_policy_get_state(), app_lea_bca_state());

#if F_APP_LEA_REALCAST_SUPPORT
    if ((app_lea_realcast_get_cmd_id() == CMD_LEA_SYNC_BIS) ||
        (app_lea_realcast_get_cmd_id() == CMD_LEA_SCAN_START))
    {
        if (app_lea_realcast_get_cmd_id() == CMD_LEA_SCAN_START)
        {
            app_lea_realcast_state_notify(CMD_LEA_SCAN_START, CMD_STATUS_SUCCESS, SCAN_STATE_SEARCHING,
                                          PKT_TYPE_COMPLETE,
                                          (uint8_t *)p_report, 24 + p_report->data_len);
            return true;
        }
        else if (app_lea_realcast_get_cmd_id() == CMD_LEA_SYNC_BIS)
        {
            bool device_match = false;
            bool name_match = false;
            uint8_t len = 0;
            uint16_t data_len = 0;
            uint8_t *p_para = NULL;
            uint8_t para_len = 0;
            T_REALCAST_SCAN_ADV_INFO adv_info;

            adv_info.report_data_len = p_report->data_len;
            adv_info.p_report_data = p_report->p_data;
            if (app_lea_realcast_get_data(REALCAST_DEVICE, &p_para, &para_len))
            {
                uint8_t p_device[REALCAST_DEVICE_LEN] = {0};

                adv_info.filter = ADV_FILTER_REALCAST;
                if (app_lea_realcast_filter_service_data(adv_info, p_device, &data_len))
                {
                    len = (data_len >= REALCAST_DEVICE_LEN) ? REALCAST_DEVICE_LEN : data_len;
                    if (memcmp(p_device, p_para, len) == 0)
                    {
                        device_match = true;
                    }
                }
            }

            if (app_lea_realcast_get_data(BROADCAST_NAME, &p_para, &para_len))
            {
                uint8_t p_name[REALCAST_BS_NAME_LEN] = {0};

                adv_info.filter = ADV_FILTER_NAME;
                if (app_lea_realcast_filter_service_data(adv_info, p_name, &data_len))
                {
                    len = (data_len >= REALCAST_BS_NAME_LEN) ? REALCAST_BS_NAME_LEN : data_len;
                    if (memcmp(p_name, p_para, data_len) == 0)
                    {
                        name_match = true;
                    }
                }
            }

            APP_PRINT_TRACE2("app_lea_scan_report: device_match 0x%02X, name_match 0x%02X", device_match,
                             name_match);

            if (name_match == false)
            {
                error_code = 2;
                goto error;
            }
            else
            {
                goto bmr_scan;
            }
        }
    }
#endif

#if F_APP_PBP_SUPPORT
    adv_info.uuid = PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID;
    if (app_lea_scan_filter_service_data(adv_info, &p_pba_data, &pba_data_len))
    {
        if (pba_data_len)
        {
            if (*p_pba_data & PUBIC_BROADCAST_BIT_ENCRYPTED)
            {
                if (memcmp(p_report->bd_addr, app_cfg_nv.lea_srcaddr, GAP_BD_ADDR_LEN) ||
                    (p_report->addr_type != app_cfg_nv.lea_srctype) ||
                    !app_cfg_nv.lea_encryp || !app_cfg_nv.lea_valid)
                {
                    APP_PRINT_TRACE1("app_lea_scan_report encrypted w/o code %s",
                                     TRACE_BDADDR(app_cfg_nv.lea_srcaddr));
                    error_code = 3;
                    goto error;
                }
            }
        }
    }
#endif

bmr_scan:
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) ||
        (app_bt_policy_get_state() != BP_STATE_FIRST_ENGAGE &&
         app_bt_policy_get_state() != BP_STATE_ENGAGE &&
         app_lea_bca_state() != LEA_BCA_STATE_IDLE))
    {
        src_info.adv_addr_type =  p_report->addr_type;
        src_info.advertiser_sid = p_report->adv_sid;
        memcpy(src_info.adv_addr, p_report->bd_addr, GAP_BD_ADDR_LEN);
        memcpy(src_info.broadcast_id, p_service_data, service_data_len);
        app_lea_bca_scan_info(&src_info);
    }
    return true;

error:
    APP_PRINT_ERROR1("app_lea_scan_report: error %d", -error_code);
    return false;
}

static void app_lea_scan_cb(BLE_SCAN_EVT state, BLE_SCAN_EVT_DATA *data)
{
    APP_PRINT_TRACE1("app_lea_scan_cb: state %d", state);

    switch (state)
    {
    case BLE_SCAN_REPORT:
        app_lea_scan_report(data->report);
        break;

    default:
        break;
    }
}

void app_lea_scan_start(void)
{
    APP_PRINT_INFO0("app_lea_scan_start");
    BLE_SCAN_PARAM param;

    /* Initialize extended scan parameters */
    param.own_addr_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    param.phys = GAP_EXT_SCAN_PHYS_1M_BIT;
    param.ext_filter_policy = GAP_SCAN_FILTER_ANY_RPA;
    param.ext_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

    param.scan_param_1m.scan_type = GAP_SCAN_MODE_PASSIVE;
    param.scan_param_1m.scan_interval = 0x140;
    param.scan_param_1m.scan_window = 0xD0;

    param.scan_param_coded.scan_type = GAP_SCAN_MODE_PASSIVE;
    param.scan_param_coded.scan_interval = 0x0050;
    param.scan_param_coded.scan_window = 0x0025;


    //le_ext_scan_report_filter(true, GAP_EXT_ADV_REPORT_BIT_CONNECTABLE_ADV, false);

    /* Enable extended scan */
    ble_scan_start(&app_lea_scan_handle, app_lea_scan_cb, &param, NULL);
}

void app_lea_scan_stop()
{
    APP_PRINT_INFO0("app_lea_scan_stop"); //For HRP test
    ble_scan_stop(&app_lea_scan_handle);
}
#endif
