/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "ringtone.h"
#include "gap_conn_le.h"
#include "stdlib.h"
#include "dfu_api.h"
#if F_APP_OTA_SUPPORT
#include "ota_service.h"
#endif
#include "transmit_svc_dongle.h"
#include "transmit_service.h"
#include "app_ble_service.h"
#include "app_ble_common_adv.h"
#include "app_cmd.h"
#include "app_main.h"
#include "app_transfer.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "bas.h"
#include "dis.h"
#include "ble_stream.h"
#include "bt_gatt_svc.h"
#if GFPS_FEATURE_SUPPORT
#include "gfps.h"
#endif
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
#include "profile_server_ext.h"
#else
#include "profile_server.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_behaviors.h"
#include "app_harman_parser.h"
#include "app_harman_vendor_cmd.h"
#endif

#define MAX_BLE_SRV_NUM 16

static T_APP_RESULT app_ble_service_general_srv_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    uint8_t conn_id;

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    T_SERVER_EXT_APP_CB_DATA *p_para = (T_SERVER_EXT_APP_CB_DATA *)p_data;
    le_get_conn_id_by_handle(p_para->event_data.send_data_result.conn_handle, &conn_id);
#else
    T_SERVER_APP_CB_DATA *p_para = (T_SERVER_APP_CB_DATA *)p_data;
    conn_id = p_para->event_data.send_data_result.conn_id;
#endif
    APP_PRINT_INFO1("app_ble_service_general_srv_cb: conn_id %d", conn_id);

    switch (p_para->eventId)
    {
    case PROFILE_EVT_SRV_REG_COMPLETE:
        break;

    case PROFILE_EVT_SEND_DATA_COMPLETE:
        /*APP_PRINT_TRACE2("PROFILE_EVT_SEND_DATA_COMPLETE: cause 0x%x, attrib_idx %d",
                         p_para->event_data.send_data_result.cause,
                         p_para->event_data.send_data_result.attrib_idx);*/
#if F_APP_HARMAN_FEATURE_SUPPORT
        if (p_para->event_data.send_data_result.attrib_idx == TRANSMIT_SVC_RX_DATA_INDEX)
#else
        if (p_para->event_data.send_data_result.attrib_idx == TRANSMIT_SVC_TX_DATA_INDEX)
#endif
        {
            app_transfer_pop_data_queue(CMD_PATH_LE, true);
        }

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
        if (!gatt_svc_handle_profile_data_cmpl(p_para->event_data.send_data_result.conn_handle,
                                               p_para->event_data.send_data_result.cid,
                                               p_para->event_data.send_data_result.service_id,
                                               p_para->event_data.send_data_result.attrib_idx,
                                               p_para->event_data.send_data_result.credits,
                                               p_para->event_data.send_data_result.cause))
        {
            APP_PRINT_ERROR0("gatt_svc_handle_profile_data_cmpl failed");
        }
#else
        {
            uint16_t conn_handle = le_get_conn_handle(conn_id);
            if (!gatt_svc_handle_profile_data_cmpl(conn_handle, L2C_FIXED_CID_ATT,
                                                   p_para->event_data.send_data_result.service_id,
                                                   p_para->event_data.send_data_result.attrib_idx,
                                                   p_para->event_data.send_data_result.credits,
                                                   p_para->event_data.send_data_result.cause))
            {
                APP_PRINT_ERROR0("gatt_svc_handle_profile_data_cmpl failed");
            }
        }
#endif
        break;

    default:
        break;
    }
    return app_result;
}

static T_APP_RESULT app_ble_service_transmit_srv_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    T_APP_LE_LINK *p_link;
    T_TRANSMIT_SRV_CALLBACK_DATA *p_callback = (T_TRANSMIT_SRV_CALLBACK_DATA *)p_data;

    APP_PRINT_INFO2("app_ble_service_transmit_srv_cb: conn_id %d, msg_type %d", p_callback->conn_id,
                    p_callback->msg_type);
    p_link = app_link_find_le_link_by_conn_id(p_callback->conn_id);
    if (p_link != NULL)
    {
        if (p_callback->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
        {
            app_harman_parser_process(HARMAN_PARSER_WRITE_CMD, p_data);
        }
        else if (p_callback->msg_type == SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION)
        {
            if (p_callback->attr_index == TRANSMIT_SVC_TX_DATA_CCCD_INDEX)
            {
                app_ble_common_adv_set_conn_id(p_callback->conn_id);

                if (p_callback->msg_data.notification_indification_value == TRANSMIT_SVC_TX_DATA_CCCD_ENABLE)
                {
                    p_link->transmit_srv_tx_enable_fg |= TX_ENABLE_CCCD_BIT;
                    APP_PRINT_INFO0("app_ble_service_transmit_srv_cb: TRANSMIT_SVC_TX_DATA_CCCD_ENABLE");
                }
                else if (p_callback->msg_data.notification_indification_value == TRANSMIT_SVC_TX_DATA_CCCD_DISABLE)
                {
                    p_link->transmit_srv_tx_enable_fg &= ~TX_ENABLE_CCCD_BIT;
                    APP_PRINT_INFO0("app_ble_service_transmit_srv_cb: TRANSMIT_SVC_TX_DATA_CCCD_DISABLE");
                }
            }

#if HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
            APP_PRINT_INFO2("app_ble_service_transmit_srv_cb: transmit_srv_tx_enable_fg %d, encryption_status %d",
                            p_link->transmit_srv_tx_enable_fg, p_link->encryption_status);
            if ((p_link->transmit_srv_tx_enable_fg & TX_ENABLE_CCCD_BIT) &&
                (p_link->encryption_status == LE_LINK_ENCRYPTIONED))
            {
                app_harman_notify_device_info_timer_start(HARMAN_NOTIFY_DEVICE_INFO_TIME);
            }
#endif

        }
        else if (p_callback->msg_type == SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE)
        {
            /* Update battery level for bas using */
            transmit_set_parameter(TRANSMIT_PARAM_BATTERY_LEVEL, 1, &app_db.local_batt_level);
            APP_PRINT_INFO1("app_ble_service_transmit_srv_cb: local_batt_level %d",
                            app_db.local_batt_level);
        }
    }

    return app_result;
}

#if F_APP_OTA_SUPPORT
static T_APP_RESULT app_ble_service_ota_srv_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    T_OTA_CALLBACK_DATA *p_ota_cb_data = (T_OTA_CALLBACK_DATA *)p_data;
    APP_PRINT_INFO2("app_ble_service_ota_srv_cb: service_id %d, msg_type %d",
                    service_id, p_ota_cb_data->msg_type);
    switch (p_ota_cb_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        break;
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        if (OTA_WRITE_CHAR_VAL == p_ota_cb_data->msg_data.write.opcode &&
            OTA_VALUE_ENTER == p_ota_cb_data->msg_data.write.value)
        {
            /* Check battery level first */
            if (app_db.local_batt_level >= 30)
            {
                T_APP_LE_LINK *p_link;
                p_link = app_link_find_le_link_by_conn_id(p_ota_cb_data->conn_id);
                /* Battery level is greater than or equal to 30 percent */
                if (p_link != NULL)
                {
                    app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_SWITCH_TO_OTA);
                }
                APP_PRINT_INFO1("app_ble_service_ota_srv_cb: Preparing switch into OTA mode conn_id %d",
                                p_ota_cb_data->conn_id);
            }
            else
            {
                /* Battery level is less than 30 percent */
                APP_PRINT_WARN1("app_ble_service_ota_srv_cb: Battery level is not enough to support OTA, local_batt_level %d",
                                app_db.local_batt_level);
            }
        }
        break;

    default:

        break;
    }

    return app_result;
}
#endif

static T_APP_RESULT app_ble_service_bas_srv_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    T_BAS_CALLBACK_DATA *p_bas_cb_data = (T_BAS_CALLBACK_DATA *)p_data;
    switch (p_bas_cb_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_bas_cb_data->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_ENABLE)
            {
                APP_PRINT_INFO0("app_ble_service_bas_srv_cb: BAS_NOTIFY_BATTERY_LEVEL_ENABLE");
            }
            else if (p_bas_cb_data->msg_data.notification_indification_index ==
                     BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
            {
                APP_PRINT_INFO0("app_ble_service_bas_srv_cb: BAS_NOTIFY_BATTERY_LEVEL_DISABLE");
            }
#if F_APP_LEA_SUPPORT && F_APP_GATT_SERVER_EXT_API_SUPPORT
            T_APP_LE_LINK *p_link;

            p_link = app_link_find_le_link_by_conn_handle(p_bas_cb_data->conn_handle);
            if (p_link != NULL)
            {
                p_link->bas_report_batt = p_bas_cb_data->msg_data.notification_indification_index;
            }
#endif
        }
        break;

    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            /* Update battery level for bas using */
            bas_set_parameter(BAS_PARAM_BATTERY_LEVEL, 1, &app_db.local_batt_level);
            APP_PRINT_INFO1("app_ble_service_bas_srv_cb: local_batt_level %d", app_db.local_batt_level);
        }
        break;

    default:
        break;
    }

    return app_result;
}

static T_APP_RESULT app_ble_service_dis_srv_cb(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    T_DIS_CALLBACK_DATA *p_dis_cb_data = (T_DIS_CALLBACK_DATA *)p_data;
    switch (p_dis_cb_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_FIRMWARE_REV_INDEX)
            {
#if GFPS_FEATURE_SUPPORT
                if (extend_app_cfg_const.gfps_support)
                {
                    dis_set_parameter(DIS_PARAM_FIRMWARE_REVISION,
                                      sizeof(extend_app_cfg_const.gfps_version),
                                      (void *)extend_app_cfg_const.gfps_version);
                }
                else
#endif
                {
                    const uint8_t DISFirmwareRev[] = "1.0.0";
                    dis_set_parameter(DIS_PARAM_FIRMWARE_REVISION,
                                      sizeof(DISFirmwareRev),
                                      (void *)DISFirmwareRev);
                }
            }
            else if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
            {

            }
        }
        break;

    default:
        break;
    }

    return app_result;
}

void app_ble_service_init(void)
{
    /** NOTES: 4 includes transimit service, ota service, bas, dis service.
     *  transimit service dongle is added when use LEA.
     *  if more ble service are added, you need to modify this value.
     * */

#if F_APP_LEA_SUPPORT
    uint8_t server_num = 5;
#else
    uint8_t server_num = 4;
#endif

#if BISTO_FEATURE_SUPPORT
    server_num += BISTO_GATT_SERVICE_NUM;
#endif

#if GFPS_FEATURE_SUPPORT
    server_num++;
#if GFPS_FINDER_SUPPORT
    server_num++;
#endif
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        server_num++;
    }
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    server_num += 3; //ASCS, PACS and CAS
#endif

#if F_APP_TMAP_BMR_SUPPORT
    server_num++; // BASS
#endif

#if F_APP_VCS_SUPPORT
    server_num++;
#endif

#if F_APP_MICS_SUPPORT
    server_num++;
#endif

#if F_APP_CSIS_SUPPORT
    server_num++;
#endif

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    server_cfg_use_ext_api(true);
    APP_PRINT_INFO0("app_ble_service_init: server_cfg_use_ext_api true");
    server_ext_register_app_cb(app_ble_service_general_srv_cb);
#else
    server_register_app_cb(app_ble_service_general_srv_cb);
#endif

    server_init(server_num);

#if F_APP_LEA_SUPPORT
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    gatt_svc_init(GATT_SVC_USE_EXT_SERVER, 0);
#else
    gatt_svc_init(GATT_SVC_USE_NORMAL_SERVER, 0);
#endif
#endif

    transmit_srv_add(app_ble_service_transmit_srv_cb);

#if F_APP_OTA_SUPPORT
    if (app_cfg_const.rtk_app_adv_support)
    {
        ota_add_service(app_ble_service_ota_srv_cb);
    }
#endif

#if F_APP_BAS_DIS_SUPPORT
    lea_bas_id = bas_add_service(app_ble_service_bas_srv_cb);
    dis_add_service(app_ble_service_dis_srv_cb);//Add for GFPS
#endif

}

