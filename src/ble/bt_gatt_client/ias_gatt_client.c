/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    ias_client.c
  * @brief   Ias BLE client source file.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  ******************************************************************************
  */

/** Add Includes here **/
#include <trace.h>
#include <string.h>
#include <ias_gatt_client.h>
#include <os_mem.h>

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

/**<  Callback used to send data to app from ias client layer. */
static P_FUN_CLIENT_GENERAL_APP_CB ias_client_cb = NULL;

/**
  * @brief  Used by application, to write data of V2 write Characteristic.
  * @param[in]  conn_id connection ID.
  * @param[in]  length  write data length
  * @param[in]  p_value point the value to write
  * @param[in]  type    write type.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ias_client_write_char(uint16_t conn_handle, uint16_t length, uint8_t *p_value)
{
    T_ATTR_UUID srv_uuid;
    T_ATTR_UUID char_uuid;
    T_GAP_CAUSE cause = GAP_CAUSE_INVALID_PARAM;
    uint16_t handle = 0;
    T_GATT_WRITE_TYPE type = GATT_WRITE_TYPE_CMD;

    srv_uuid.is_uuid16 = true;
    srv_uuid.instance_id = 0;
    srv_uuid.p.uuid16 = GATT_UUID_IMMEDIATE_ALERT_SERVICE;
    char_uuid.is_uuid16 = true;
    char_uuid.instance_id = 0;
    char_uuid.p.uuid16 = GATT_UUID_CHAR_ALERT_LEVEL;

    if (gatt_client_find_char_handle(conn_handle, &srv_uuid, &char_uuid, &handle))
    {
        cause = gatt_client_write(conn_handle, type, handle, length, p_value, NULL);
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

/**
 * @brief Ias BLE Client Callbacks.
*/
T_APP_RESULT ias_client_cbs(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;
    T_IAS_CLT_CB_DATA cb_data;
    memset(&cb_data, 0, sizeof(T_IAS_CLT_CB_DATA));

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            PROTOCOL_PRINT_INFO1("[IAS] GATT_CLIENT_EVENT_DIS_DONE: is_found %d",
                                 p_client_cb_data->dis_done.is_found);
            cb_data.cb_type = IAS_CLIENT_CB_TYPE_DISC_STATE;

            if (p_client_cb_data->dis_done.is_found)
            {
                cb_data.cb_content.disc_state = DISC_IAS_DONE;
            }
            else
            {
                cb_data.cb_content.disc_state = DISC_IAS_FAILED;
            }
        }
        break;

    case GATT_CLIENT_EVENT_WRITE_RESULT:
        {
            PROTOCOL_PRINT_INFO3("[IAS] GATT_CLIENT_EVENT_WRITE_RESULT: conn_handle 0x%x, cause 0x%x, uuid 0x%x",
                                 conn_handle, p_client_cb_data->write_result.cause,
                                 p_client_cb_data->write_result.char_uuid.p.uuid16);
            if (p_client_cb_data->write_result.char_type == GATT_CHAR_VALUE &&
                p_client_cb_data->write_result.char_uuid.p.uuid16 == GATT_UUID_CHAR_ALERT_LEVEL)
            {
                cb_data.cb_type = IAS_CLIENT_CB_TYPE_WRITE_RESULT;
                cb_data.cb_content.write_result.type = IAS_WRITE_ALERT;
                cb_data.cb_content.write_result.cause = p_client_cb_data->write_result.cause;
            }
        }
        break;

    default:
        break;
    }

    if (ias_client_cb)
    {
        (*ias_client_cb)(conn_handle, GATT_UUID_IMMEDIATE_ALERT_SERVICE, &cb_data);
    }

    return result;
}

/**
  * @brief      Add ias service client to application.
  * @param[in]  app_cb pointer of app callback function to handle specific client module data.
  * @param[in]  link_num initialize link num.
  * @return Client ID of the specific client module.
  * @retval 0xff failed.
  * @retval other success.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {
        ias_client_id = ias_ble_add_client(app_client_callback, APP_MAX_LINKS);
    }
  * \endcode
  */
bool ias_add_client(P_FUN_CLIENT_GENERAL_APP_CB app_cb)
{
    T_ATTR_UUID srv_uuid = {0};
    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_IMMEDIATE_ALERT_SERVICE;

    if (gatt_client_spec_register(&srv_uuid, ias_client_cbs) == GAP_CAUSE_SUCCESS)
    {
        /* register callback for profile to inform application that some events happened. */
        ias_client_cb = app_cb;
        return true;
    }

    return false;
}

