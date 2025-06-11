/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gaps_gatt_client.c
  * @brief   GAPS GATT client source file.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v0.1
  ******************************************************************************
  */

/** Add Includes here **/
#include <trace.h>
#include <string.h>
#include <stdlib.h>
#include "gaps_gatt_client.h"
#include "gatt.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Callback used to send data to app from GAPS client layer. */
static P_FUN_GAPS_CLIENT_APP_CB gaps_client_cb = NULL;

bool gaps_client_read_char(uint16_t conn_handle, uint16_t char_uuid)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid16;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GAP;
    char_uuid16.is_uuid16 = true;
    char_uuid16.instance_id = 0;
    char_uuid16.p.uuid16 = char_uuid;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid16, &handle))
    {
        cause = gatt_client_read(conn_handle, handle, NULL);
    }

    return (cause == GAP_CAUSE_SUCCESS) ? true : false;
}

bool gaps_client_write_device_name(uint16_t conn_handle, uint16_t name_len, uint8_t *p_name)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid16;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GAP;
    char_uuid16.is_uuid16 = true;
    char_uuid16.instance_id = 0;
    char_uuid16.p.uuid16 = GATT_UUID_CHAR_DEVICE_NAME;
    uint16_t properties = 0;

    if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid16, &properties))
    {
        if (properties & GATT_CHAR_PROP_WRITE)
        {
            if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid16, &handle))
            {
                cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, name_len, p_name, NULL);
            }
        }
        else
        {
            PROTOCOL_PRINT_ERROR0("gaps_client_write_device_name: write not support");
        }
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool gaps_client_write_appearance(uint16_t conn_handle, uint16_t appearance)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid16;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_GAP;
    char_uuid16.is_uuid16 = true;
    char_uuid16.instance_id = 0;
    char_uuid16.p.uuid16 = GATT_UUID_CHAR_APPEARANCE;
    uint16_t properties = 0;

    if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid16, &properties))
    {
        if (properties & GATT_CHAR_PROP_WRITE)
        {
            if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid16, &handle))
            {
                cause = gatt_client_write(conn_handle, GATT_WRITE_TYPE_REQ, handle, sizeof(uint16_t),
                                          (uint8_t *)&appearance, NULL);
            }
        }
        else
        {
            PROTOCOL_PRINT_ERROR0("gaps_client_write_appearance: write not support");
        }
    }

    if (cause == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

T_APP_RESULT gaps_gatt_client_cbs(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            PROTOCOL_PRINT_INFO2("[GAPS] GATT_CLIENT_EVENT_DIS_DONE: conn_handle 0x%x, is_found %d",
                                 conn_handle,
                                 p_client_cb_data->dis_done.is_found);
            T_GAPS_CLIENT_DIS_DONE dis_done = {0};

            dis_done.is_found = p_client_cb_data->dis_done.is_found;
            dis_done.load_from_ftl = p_client_cb_data->dis_done.load_from_ftl;

            if (dis_done.is_found)
            {
                T_ATTR_UUID srv_uuid;
                T_ATTR_UUID char_uuid;
                uint16_t handle = 0;
                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_GAP;
                char_uuid.is_uuid16 = true;
                char_uuid.instance_id = 0;
                char_uuid.p.uuid16 = GATT_UUID_CHAR_DEVICE_NAME;
                uint16_t properties = 0;

                if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &properties))
                {
                    if (properties & GATT_CHAR_PROP_WRITE)
                    {
                        dis_done.device_name_write_support = true;
                    }
                }

                char_uuid.p.uuid16 = GATT_UUID_CHAR_APPEARANCE;
                if (gatt_client_get_char_prop(conn_handle, &srv_uuid, &char_uuid, &properties))
                {
                    if (properties & GATT_CHAR_PROP_WRITE)
                    {
                        dis_done.appearance_write_support = true;
                    }
                }

                char_uuid.p.uuid16 = GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION;
                if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
                {
                    dis_done.central_address_resolution_exist = true;
                }

                char_uuid.p.uuid16 = GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY;
                if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
                {
                    dis_done.resolvable_private_addr_only_exist = true;
                }
            }

            if (gaps_client_cb)
            {
                result = (*gaps_client_cb)(conn_handle, GATT_MSG_GAPS_CLIENT_DIS_DONE, &dis_done);
            }
        }
        break;

    case GATT_CLIENT_EVENT_READ_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[GAPS] GATT_CLIENT_EVENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, uuid 0x%x",
                                 conn_handle, p_client_cb_data->read_result.cause,
                                 p_client_cb_data->read_result.char_uuid.p.uuid16);
            T_GAPS_CLIENT_READ_RESULT read_result = {0};
            read_result.cause = p_client_cb_data->read_result.cause;
            read_result.char_uuid = p_client_cb_data->read_result.char_uuid.p.uuid16;

            if (read_result.cause == ATT_SUCCESS)
            {
                read_result.value_len = p_client_cb_data->read_result.value_size;

                switch (read_result.char_uuid)
                {
                case GATT_UUID_CHAR_DEVICE_NAME:
                    {
                        read_result.data.p_device_name = p_client_cb_data->read_result.p_value;
                    }
                    break;

                case GATT_UUID_CHAR_APPEARANCE:
                    {
                        if (read_result.value_len != sizeof(uint16_t))
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            read_result.cause = ATT_ERR_INVALID_VALUE_SIZE;
                        }

                        memcpy(&read_result.data.device_appearance, p_client_cb_data->read_result.p_value, 2);
                    }
                    break;

                case GATT_UUID_CHAR_PER_PREF_CONN_PARAM:
                    {
                        if (read_result.value_len != 8)
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            read_result.cause = ATT_ERR_INVALID_VALUE_SIZE;
                        }
                        memcpy(&read_result.data.prefer_conn_param, p_client_cb_data->read_result.p_value, 8);
                    }
                    break;

                case GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION:
                    {
                        if (read_result.value_len != sizeof(uint8_t))
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            read_result.cause = ATT_ERR_INVALID_VALUE_SIZE;
                        }
                        read_result.data.central_addr_resolution = p_client_cb_data->read_result.p_value[0];
                    }
                    break;

                case GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY:
                    {
                        if (read_result.value_len != sizeof(uint8_t))
                        {
                            result = APP_RESULT_INVALID_VALUE_SIZE;
                            read_result.cause = ATT_ERR_INVALID_VALUE_SIZE;
                        }
                        read_result.data.rpa_only = p_client_cb_data->read_result.p_value[0];
                    }
                    break;

                default:
                    break;
                }
            }

            if (gaps_client_cb)
            {
                result = (* gaps_client_cb)(conn_handle, GATT_MSG_GAPS_CLIENT_READ_RESULT,
                                            &read_result);
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[GAPS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, uuid 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16);

            if (p_client_cb_data->write_result.char_uuid.p.uuid16 == GATT_UUID_CHAR_DEVICE_NAME)
            {
                T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT write_result = {0};
                write_result.cause = p_client_cb_data->write_result.cause;

                if (gaps_client_cb)
                {
                    result = (*gaps_client_cb)(conn_handle, GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT,
                                               &write_result);
                }
            }
            else if (p_client_cb_data->write_result.char_uuid.p.uuid16 == GATT_UUID_CHAR_APPEARANCE)
            {
                T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT write_result = {0};
                write_result.cause = p_client_cb_data->write_result.cause;

                if (gaps_client_cb)
                {
                    result = (*gaps_client_cb)(conn_handle, GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT,
                                               &write_result);
                }
            }
            else
            {
                result = APP_RESULT_ATTR_NOT_FOUND;
            }
        }
        break;

    default:
        break;
    }

    return result;
}

bool gaps_client_init(P_FUN_GAPS_CLIENT_APP_CB app_cb)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_GAP;

    if (gatt_client_spec_register(&srv_uuid, gaps_gatt_client_cbs) == GAP_CAUSE_SUCCESS)
    {
        /* register callback for profile to inform application that some events happened. */
        gaps_client_cb = app_cb;
        return true;
    }

    return false;
}

