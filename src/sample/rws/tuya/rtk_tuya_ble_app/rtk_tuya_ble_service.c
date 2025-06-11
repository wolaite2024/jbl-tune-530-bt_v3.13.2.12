/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TUYA_SUPPORT
#include <string.h>
#include <trace.h>
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
#include "gap_conn_le.h"
#endif
#include "rtk_tuya_ble_service.h"
#include <gap.h>

#define TUYA_SERVICE_CHAR_RX_WRITE_INDEX           0x02
#define TUYA_SERVICE_CHAR_TX_NOTIFY_INDEX          0x04
#define TUYA_SERVICE_CHAR_TX_NOTIFY_CCCD_INDEX     (TUYA_SERVICE_CHAR_TX_NOTIFY_INDEX + 1)
#define TUYA_SERVICE_CHAR_RX_READ_INDEX            0x07

#define TUYA_READ_VALUE_MAX_LENGTH                  244
static T_SERVER_ID app_tuya_service_id;

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
static P_FUN_EXT_SERVER_GENERAL_CB pfn_app_tuya_ble_service_cb = NULL;
#else
static P_FUN_SERVER_GENERAL_CB pfn_app_tuya_ble_service_cb = NULL;
#endif

static uint8_t tuya_read_value[TUYA_READ_VALUE_MAX_LENGTH] = {'d', 'd'};
static uint8_t tuya_read_value_len = 2;

static const T_ATTRIB_APPL app_tuya_ble_service_tbl[] =
{
    /* <<Primary Service>>, index = 0x00 */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_SERVICE_TUYA),      /* service UUID */
            HI_WORD(GATT_UUID_SERVICE_TUYA)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>> RX write index = 0x01 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP | GATT_CHAR_PROP_WRITE /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic Value>> RX write index = 0x02 */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,
        {
            GATT_UUID_CHAR_RX_WRITE
        },
        0,
        NULL,
        GATT_PERM_WRITE
    },
    /* <<Characteristic>>, TX notify index = 0x03*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic Value>> TX index = 0x04 */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                     /* flags */
        {                                         /* type_value */
            GATT_UUID_CHAR_TX_NOTIFY,
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND                              /* permissions */
    },
    /* client characteristic configuration index = 0x05*/
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
    /* <<Characteristic>> RX read index = 0x06 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic Value>> RX read index = 0x07 */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,
        {
            GATT_UUID_CHAR_RX_READ
        },
        0,
        NULL,
        GATT_PERM_READ
    }
};

bool app_tuya_ble_service_tx_send_notify(uint8_t conn_id, uint8_t *p_value,
                                         uint16_t length)
{
    if (p_value == NULL)
    {
        APP_PRINT_ERROR0("app_tuya_ble_service_tx_send_notify: NULL");
        return false;
    }

    APP_PRINT_INFO3("app_tuya_ble_service_tx_send_notify:conn_id %d, p_value 0x%x,length %d", conn_id,
                    p_value, length);
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    return server_ext_send_data(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT, app_tuya_service_id,
                                TUYA_SERVICE_CHAR_TX_NOTIFY_INDEX,
                                p_value,
                                length,
                                GATT_PDU_TYPE_ANY);
#else
    return server_send_data(conn_id, app_tuya_service_id, TUYA_SERVICE_CHAR_TX_NOTIFY_INDEX,
                            p_value,
                            length,
                            GATT_PDU_TYPE_ANY);
#endif
}

void app_tuya_ble_set_read_value(uint8_t *value, uint8_t length)
{
    if ((value != NULL) && (length <= TUYA_READ_VALUE_MAX_LENGTH))
    {
        memcpy(tuya_read_value, value, length);
        tuya_read_value_len = length;
    }
    else
    {
        APP_PRINT_ERROR0("app_tuya_ble_set_read_value: err");
    }
}

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
static T_APP_RESULT app_tuya_ble_service_attr_write_cb(uint16_t conn_handle, uint16_t cid,
                                                       T_SERVER_ID service_id,
                                                       uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                                       P_FUN_EXT_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_TUYA_SERVICE_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    APP_PRINT_INFO1("app_tuya_ble_service_attr_write_cb: write_type 0x%02x", write_type);
    *p_write_ind_post_proc = NULL;

    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.conn_id  = conn_id;
    callback_data.msg_data.write.write_type = write_type;
    callback_data.msg_data.write.len = length;
    callback_data.msg_data.write.p_value = p_value;

    if (p_value != NULL)
    {
        if (TUYA_SERVICE_CHAR_RX_WRITE_INDEX == attrib_index)
        {
            if (pfn_app_tuya_ble_service_cb && (cause == APP_RESULT_SUCCESS))
            {
                pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
            }
        }
        else
        {
            APP_PRINT_ERROR2("app_tuya_ble_service_attr_write_cb: attrib_index 0x%02x, length %d",
                             attrib_index,
                             length);
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    else
    {
        cause  = APP_RESULT_INVALID_VALUE_SIZE;
    }
    return cause;
}

static void app_tuya_ble_service_cccd_update_cb(uint16_t conn_handle, uint16_t cid,
                                                T_SERVER_ID service_id,
                                                uint16_t index,
                                                uint16_t cccbits)
{
    T_TUYA_SERVICE_CALLBACK_DATA callback_data;
    bool is_handled = true;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);

    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    APP_PRINT_INFO2("app_tuya_ble_service_cccd_update_cb: index %d, cccbits 0x%04x", index, cccbits);
    switch (index)
    {
    case TUYA_SERVICE_CHAR_TX_NOTIFY_CCCD_INDEX:
        {
            if (cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = TUYA_CHAR_TX_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = TUYA_CHAR_TX_NOTIFY_DISABLE;
            }

        }
        break;

    default:
        is_handled =  false;
        break;
    }

    if (pfn_app_tuya_ble_service_cb && (is_handled == true))
    {
        pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
    }
}

static T_APP_RESULT  app_tuya_ble_service_attr_read_cb(uint16_t conn_handle, uint16_t cid,
                                                       T_SERVER_ID service_id,
                                                       uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("simp_ble_service_attr_read_cb, Attr not found, index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case TUYA_SERVICE_CHAR_RX_READ_INDEX:
        {
            T_TUYA_SERVICE_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;

            callback_data.msg_data.read_value_index = TUYA_CHAR_RX_READ_VALUE;
            callback_data.conn_id = conn_id;

            if (pfn_app_tuya_ble_service_cb)
            {
                pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
            }

            *pp_value = tuya_read_value;
            *p_length = tuya_read_value_len;
        }
        break;
    }

    return (cause);
}


#else
static T_APP_RESULT app_tuya_ble_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                                       uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                                       P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_TUYA_SERVICE_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    APP_PRINT_INFO1("app_tuya_ble_service_attr_write_cb: write_type 0x%02x", write_type);
    *p_write_ind_post_proc = NULL;

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.conn_id  = conn_id;
    callback_data.msg_data.write.write_type = write_type;
    callback_data.msg_data.write.len = length;
    callback_data.msg_data.write.p_value = p_value;

    if (p_value != NULL)
    {
        if (TUYA_SERVICE_CHAR_RX_WRITE_INDEX == attrib_index)
        {
            if (pfn_app_tuya_ble_service_cb && (cause == APP_RESULT_SUCCESS))
            {
                pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
            }
        }
        else
        {
            APP_PRINT_ERROR2("app_tuya_ble_service_attr_write_cb: attrib_index 0x%02x, length %d",
                             attrib_index,
                             length);
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    else
    {
        cause  = APP_RESULT_INVALID_VALUE_SIZE;
    }
    return cause;
}

static void app_tuya_ble_service_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                                uint16_t index,
                                                uint16_t cccbits)
{
    T_TUYA_SERVICE_CALLBACK_DATA callback_data;
    bool is_handled = true;
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    APP_PRINT_INFO2("app_tuya_ble_service_cccd_update_cb: index %d, cccbits 0x%04x", index, cccbits);
    switch (index)
    {
    case TUYA_SERVICE_CHAR_TX_NOTIFY_CCCD_INDEX:
        {
            if (cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = TUYA_CHAR_TX_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = TUYA_CHAR_TX_NOTIFY_DISABLE;
            }

        }
        break;

    default:
        is_handled =  false;
        break;
    }

    if (pfn_app_tuya_ble_service_cb && (is_handled == true))
    {
        pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
    }
}

static T_APP_RESULT  app_tuya_ble_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                                       uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("simp_ble_service_attr_read_cb, Attr not found, index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case TUYA_SERVICE_CHAR_RX_READ_INDEX:
        {
            T_TUYA_SERVICE_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;

            callback_data.msg_data.read_value_index = TUYA_CHAR_RX_READ_VALUE;
            callback_data.conn_id = conn_id;

            if (pfn_app_tuya_ble_service_cb)
            {
                pfn_app_tuya_ble_service_cb(service_id, (void *)&callback_data);
            }

            *pp_value = tuya_read_value;
            *p_length = tuya_read_value_len;
        }
        break;
    }

    return (cause);
}
#endif

/**
 * @brief TUYA ble Service Callbacks.
*/
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
const T_FUN_GATT_EXT_SERVICE_CBS app_tuya_ble_service_cbs =
#else
const T_FUN_GATT_SERVICE_CBS app_tuya_ble_service_cbs =
#endif
{
    app_tuya_ble_service_attr_read_cb,  // Read callback function pointer
    app_tuya_ble_service_attr_write_cb, // Write callback function pointer
    app_tuya_ble_service_cccd_update_cb // CCCD update callback function pointer

};

/**
  * @brief Add TUYA BLE service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID app_tuya_ble_service_add_service(void *p_func)
{
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    if (false == server_ext_add_service(&app_tuya_service_id,
                                        (uint8_t *)app_tuya_ble_service_tbl,
                                        sizeof(app_tuya_ble_service_tbl),
                                        &app_tuya_ble_service_cbs))
    {
        APP_PRINT_ERROR0("app_tuya_ble_service_add_service: fail");
        app_tuya_service_id = 0xff;
        return app_tuya_service_id;
    }

    pfn_app_tuya_ble_service_cb = (P_FUN_EXT_SERVER_GENERAL_CB)p_func;
    return app_tuya_service_id;
#else
    if (false == server_add_service(&app_tuya_service_id,
                                    (uint8_t *)app_tuya_ble_service_tbl,
                                    sizeof(app_tuya_ble_service_tbl),
                                    app_tuya_ble_service_cbs))
    {
        APP_PRINT_ERROR0("app_tuya_ble_service_add_service: fail");
        app_tuya_service_id = 0xff;
        return app_tuya_service_id;
    }

    pfn_app_tuya_ble_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return app_tuya_service_id;
#endif
}
#endif
