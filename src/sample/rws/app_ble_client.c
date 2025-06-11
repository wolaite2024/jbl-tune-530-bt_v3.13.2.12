/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "string.h"
#include "trace.h"
#include "app_ble_client.h"
#include "ancs_client.h"
#include "stdlib.h"
#include "app_main.h"
#include "app_report.h"
#include "app_cmd.h"
#include "gaps_gatt_client.h"
#include "app_cfg.h"
#include "app_ble_device.h"
#if BISTO_FEATURE_SUPPORT
#include "app_bisto_ble.h"
#endif

#include <profile_client.h>
#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_ble.h"
#endif

#if 0
static T_APP_RESULT ancs_client_cb(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_ANCS_CB_DATA *p_cb_data = (T_ANCS_CB_DATA *)p_data;
    T_APP_LE_LINK *p_link;

    p_link = app_link_find_le_link_by_conn_id(conn_id);
    if (p_link != NULL)
    {
        switch (p_cb_data->cb_type)
        {
        case ANCS_CLIENT_CB_TYPE_DISC_STATE:
            {
                APP_PRINT_TRACE1("ancs_client_cb: ANCS_CLIENT_CB_TYPE_DISC_STATE disc_state %d",
                                 p_cb_data->cb_content.disc_state);
                switch (p_cb_data->cb_content.disc_state)
                {
                case DISC_ANCS_DONE:
                    /* Discovery Simple BLE service procedure successfully done. */
                    ancs_set_data_source_notify(conn_id, true);
                    break;

                case DISC_ANCS_FAILED:
                    /* Discovery Request failed. */
                    break;

                default:
                    break;
                }
            }
            break;

        case ANCS_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
            {
                uint8_t     *p_data;
                uint8_t     *tx_ptr;
                uint16_t    pkt_len;
                uint16_t    total_len;
                uint16_t    data_len;
                uint8_t     pkt_type;

                pkt_type = PKT_TYPE_SINGLE;
                p_data = p_cb_data->cb_content.notify_data.p_value;
                total_len = data_len = p_cb_data->cb_content.notify_data.value_size;
                while (data_len)
                {
                    if (data_len > (app_db.external_mcu_mtu - 12))
                    {
                        pkt_len = app_db.external_mcu_mtu - 12;
                        if (pkt_type == PKT_TYPE_SINGLE)
                        {
                            pkt_type = PKT_TYPE_START;
                        }
                        else
                        {
                            pkt_type = PKT_TYPE_CONT;
                        }
                    }
                    else
                    {
                        pkt_len = data_len;
                        if (pkt_type != PKT_TYPE_SINGLE)
                        {
                            pkt_type = PKT_TYPE_END;
                        }
                    }

                    tx_ptr = malloc((pkt_len + 7));
                    if (tx_ptr != NULL)
                    {
                        tx_ptr[0] = p_link->id;
                        tx_ptr[1] = p_cb_data->cb_content.notify_data.type;;
                        tx_ptr[2] = pkt_type;
                        tx_ptr[3] = (uint8_t)total_len;
                        tx_ptr[4] = (uint8_t)(total_len >> 8);
                        tx_ptr[5 = (uint8_t)pkt_len;
                                 tx_ptr[6] = (uint8_t)(pkt_len >> 8);
                                 memcpy(&tx_ptr[7], p_data, pkt_len);

                                 app_report_event(CMD_PATH_UART, EVENT_ANCS_NOTIFICATION, 0, tx_ptr, pkt_len + 7);

                                 free(tx_ptr);
                      }

                      p_data += pkt_len;
                      data_len -= pkt_len;
                  }
              }
              break;

          case ANCS_CLIENT_CB_TYPE_WRITE_RESULT:
              {
                  APP_PRINT_TRACE2("ancs_client_cb: ANCS_CLIENT_CB_TYPE_WRITE_RESULT cause 0x%04x, type 0x%02x",
                                 p_cb_data->cb_content.write_result.cause, p_cb_data->cb_content.write_result.type);

                switch (p_cb_data->cb_content.write_result.type)
                {
                case ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_ENABLE:
                    {
                        uint8_t event_buff[2];

                        event_buff[0] = p_link->id;
                        event_buff[1] = 0x00;//successful
                        app_report_event(CMD_PATH_UART, EVENT_ANCS_REGISTER_COMPLETE, 0, &event_buff[0], 2);
                    }
                    break;

                case ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_DISABLE:
                    break;

                case ANCS_WRITE_DATA_SOURCE_NOTIFY_ENABLE:
                    ancs_set_notification_source_notify(conn_id, true);
                    break;
                case ANCS_WRITE_DATA_SOURCE_NOTIFY_DISABLE:
                    break;

                case ANCS_WRITE_CONTROL_POINT:
                    {
                        uint8_t event_buff[3];

                        event_buff[0] = p_link->id;
                        event_buff[1] = (uint8_t)(p_cb_data->cb_content.write_result.cause);
                        event_buff[2] = (uint8_t)(p_cb_data->cb_content.write_result.cause >> 8);
                        app_report_event(CMD_PATH_UART, EVENT_ANCS_ACTION_RESULT, 0, &event_buff[0], 3);
                    }
                    break;

                default:
                    break;
                }
            }
            break;

        case ANCS_CLIENT_CB_TYPE_DISCONNECT_INFO:
            APP_PRINT_INFO1("ancs_client_cb: ANCS_CLIENT_CB_TYPE_DISCONNECT_INFO conn_id %d", conn_id);
            break;

        default:
            break;
        }
    }
    return result;
}
#endif

T_APP_RESULT app_gaps_gatt_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
{
    if (type == GATT_MSG_GAPS_CLIENT_DIS_DONE)
    {
        T_GAPS_CLIENT_DIS_DONE *p_dis = (T_GAPS_CLIENT_DIS_DONE *)p_data;
        APP_PRINT_INFO3("GATT_MSG_GAPS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, load_from_ftl %d",
                        conn_handle, p_dis->is_found, p_dis->load_from_ftl);
        APP_PRINT_INFO4("GATT_MSG_GAPS_CLIENT_DIS_DONE: device_name_write_sup %d, appearance_write_sup %d, central_address_resolution_exist %d, rpa_only_exist %d",
                        p_dis->device_name_write_support, p_dis->appearance_write_support,
                        p_dis->central_address_resolution_exist, p_dis->resolvable_private_addr_only_exist);

        gaps_client_read_char(conn_handle, GATT_UUID_CHAR_DEVICE_NAME);
    }
    else if (type == GATT_MSG_GAPS_CLIENT_READ_RESULT)
    {
        T_GAPS_CLIENT_READ_RESULT *p_read = (T_GAPS_CLIENT_READ_RESULT *)p_data;
        APP_PRINT_INFO3("GATT_MSG_GAPS_CLIENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, char_uuid 0x%x",
                        conn_handle, p_read->cause, p_read->char_uuid);

        if (p_read->cause == APP_RESULT_SUCCESS)
        {
            switch (p_read->char_uuid)
            {
            case GATT_UUID_CHAR_DEVICE_NAME:
                {
                    APP_PRINT_INFO2("GATT_MSG_GAPS_CLIENT_READ_RESULT: size %d, device_name %s",
                                    p_read->value_len,
                                    TRACE_STRING(p_read->data.p_device_name));

#if F_APP_HARMAN_FEATURE_SUPPORT
                    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(conn_handle);

                    // p_link = app_link_find_b2s_link((uint8_t *)&param->remote_name_rsp.bd_addr);

#if HARMAN_REQ_REMOTE_DEVICE_NAME_TIME

                    T_APP_LE_LINK *p_le_link;

                    // p_le_link = app_link_find_le_link_by_conn_id(app_ble_common_adv_get_conn_id());
                    // if ((app_link_get_b2s_link_num()) &&
                    //     ((app_ble_common_adv_get_conn_id() == 0xFF) ||
                    //         ((p_le_link != NULL) && (p_le_link->state != LE_LINK_STATE_CONNECTED))))
                    {
                        // app_harman_req_remote_device_name_timer_start(1 << 4 | p_link->id);
                    }
#endif
                    if ((p_link == NULL) || (p_link->auth_cmpl == false))
                    {
                        APP_PRINT_TRACE0("GATT_UUID_CHAR_DEVICE_NAME break");
                        break;
                    }

                    uint16_t new_device_name_crc = 0;
                    uint32_t remote_name_len = p_read->value_len;

                    APP_PRINT_TRACE2("GATT_UUID_CHAR_DEVICE_NAME: remote_device_name: %s, name_len: %d",
                                     TRACE_STRING(p_read->data.p_device_name), remote_name_len);

                    if (remote_name_len >= 31)
                    {
                        remote_name_len = 31;
                    }

                    new_device_name_crc = harman_crc16_ibm(p_read->data.p_device_name, remote_name_len,
                                                           p_link->bd_addr);
                    if (memcmp(&p_link->device_name_crc, (uint8_t *)&new_device_name_crc, 2))
                    {
                        APP_PRINT_TRACE2("GATT_UUID_CHAR_DEVICE_NAME: new_device_name_crc: 0x%x, device_name_crc: %b",
                                         new_device_name_crc, TRACE_BINARY(2, p_link->device_name_crc));
                        memcpy(&p_link->device_name_crc, (uint8_t *)&new_device_name_crc, 2);

                        app_harman_remote_device_name_crc_set(p_link->conn_id, NULL, true);

                        if (app_cfg_const.rtk_app_adv_support)
                        {
                            app_ble_rtk_adv_start();
                        }

                        app_harman_le_common_adv_update();
                    }
#endif
                }
                break;

            case GATT_UUID_CHAR_APPEARANCE:
                {
                    APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: device_appearance 0x%x",
                                    p_read->data.device_appearance);
                }
                break;

            case GATT_UUID_CHAR_PER_PREF_CONN_PARAM:
                {
                    APP_PRINT_INFO0("GATT_MSG_GAPS_CLIENT_READ_RESULT: prefer conn parameter");
                    APP_PRINT_INFO4("conn interval min 0x%x, conn interval max 0x%x, slave latecy 0x%x, sup timeout 0x%x",
                                    p_read->data.prefer_conn_param.conn_interval_min,
                                    p_read->data.prefer_conn_param.conn_interval_max,
                                    p_read->data.prefer_conn_param.slave_latency,
                                    p_read->data.prefer_conn_param.supervision_timeout);
                }
                break;

            case GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION:
                {
                    APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: central addr resolution 0x%x",
                                    p_read->data.central_addr_resolution);
                }
                break;

            case GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY:
                {
                    APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: central addr resolution 0x%x",
                                    p_read->data.rpa_only);
                }
                break;

            default:
                break;
            }
        }
    }
    else if (type == GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT)
    {
        T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT *p_write = (T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT *)p_data;

        APP_PRINT_INFO2("GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT: conn_handle 0x%x, cause 0x%x",
                        conn_handle, p_write->cause);
    }
    else if (type == GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT)
    {
        T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT *p_write = (T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT *)p_data;

        APP_PRINT_INFO2("GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT: conn_handle 0x%x, cause 0x%x",
                        conn_handle, p_write->cause);
    }

    return APP_RESULT_SUCCESS;
}


void app_ble_client_init(void)
{
    volatile uint8_t client_num = 0;

#if BISTO_FEATURE_SUPPORT
    client_num = client_num + BISTO_GATT_CLIENT_NUM;
#endif

#if (F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT)
    client_num += 1;
#endif
    //ANCS
    if (client_num)
    {
#if (F_APP_LEA_SUPPORT == 0)
        client_init(client_num);
#endif
    }
    else
    {
        APP_PRINT_INFO0("app_ble_client_init: num is 0");
    }
    //ancs_add_client(ancs_client_cb, MAX_BLE_LINK_NUM);
    gaps_client_init(app_gaps_gatt_client_callback);
}
