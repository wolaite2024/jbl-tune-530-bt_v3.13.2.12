/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     transmit_service.c
* @brief    Transmit service source file.
* @details  Interfaces to access transmit service.
* @author   jane
* @date     2015-5-12
* @version  v0.1
*********************************************************************************************************
*/
#include <stdint.h>
#include "gatt.h"
#include <string.h>
#include "trace.h"
#include "transmit_service.h"
#include "gap.h"
#include "gap_conn_le.h"
#include "app_flags.h"
#include "app_harman_report.h"

/** @brief  Transmit Service related UUIDs. */
#define TRANSMIT_SVC_CHAR_UUID_RX_CHAR                  0x01,0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65
#define TRANSMIT_SVC_CHAR_UUID_TX_CHAR                  0x02,0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65
#define TRANSMIT_SVC_CHAR_UUID_RW_AUX_CHAR              0x03,0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65
#define TRANSMIT_SVC_CHAR_UUID_RW_UPLINK_DATA_CHAR      0x04,0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65
#define TRANSMIT_SVC_CHAR_UUID_RW_DOWNLINK_DATA_CHAR    0x05,0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static P_FUN_SERVER_GENERAL_CB transmit_srv_cb = NULL;

static uint8_t battery_level = 0;
static bool transmit_read_battery_level_pending = false;
const uint8_t HID_READ_IN_USER_SESCR[] = "TI Data Path TX Data";
const uint8_t HID_WRITE_IN_USER_SESCR[] = "TI Data Path RX Data";
const uint8_t GATT_UUID128_TRANSMIT_SRV[16] = { 0x00, 0x00, 0x6D, 0x6F, 0x63, 0x2E, 0x74, 0x6E, 0x69, 0x6F, 0x70, 0x6C, 0x65, 0x63, 0x78, 0x65};
static uint8_t transmit_srv_id = 0xff;

/**< @brief  profile/service definition.
 *   Harman Lifestyle General Device Control Protocol
*/
static const T_ATTRIB_APPL TRANSMIT_SRV_TABLE[] =
{
    /*----------------- Transmit Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                            /* bValueLen     */
        (void *)GATT_UUID128_TRANSMIT_SRV,         /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<Characteristic>>, RX_CHAR */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY)

            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* transmit tx*/
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* flags */
        {                                           /* type_value */
            TRANSMIT_SVC_CHAR_UUID_RX_CHAR
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND | GATT_PERM_READ, /* permissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL | ATTRIB_FLAG_CCCD_NO_FILTER,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
#if HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
#else
        GATT_PERM_READ | GATT_PERM_WRITE         /* permissions */
#endif
    },

    /* HID VOICE 0xFE input report user descriptor 25 */
    // {
    //     ATTRIB_FLAG_VOID, /* wFlags */
    //     { /* bTypeValue */
    //         LO_WORD(GATT_UUID_CHAR_USER_DESCR),
    //         HI_WORD(GATT_UUID_CHAR_USER_DESCR),
    //     },
    //     (sizeof(HID_READ_IN_USER_SESCR) - 1), /* bValueLen */
    //     (void *)HID_READ_IN_USER_SESCR,
    //     (GATT_PERM_READ) /* wPermissions */
    // },

    /* <<Characteristic>>, TX_CHAR */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
#ifdef TRANS_SRV_TRX_NEED_RSP
            GATT_CHAR_PROP_INDICATE
#else
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP)
#endif
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* transmit rx*/
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* flags */
        {                                           /* type_value */
            TRANSMIT_SVC_CHAR_UUID_TX_CHAR
        },
        0,                                          /* bValueLen */
        NULL,
#if HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
        GATT_PERM_WRITE_ENCRYPTED_REQ /* permissions */
#else
        GATT_PERM_WRITE /* permissions */
#endif
    },

    /* <<Characteristic>>, RW_AUX_CHAR */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP | GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* transmit trx*/
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* flags */
        {                                           /* type_value */
            TRANSMIT_SVC_CHAR_UUID_RW_AUX_CHAR
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND | GATT_PERM_READ,       /* permissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
#if HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
#else
        GATT_PERM_READ | GATT_PERM_WRITE         /* permissions */
#endif
    },

    /* HID VOICE 0xFE input report user descriptor 25 */
    // {
    //     ATTRIB_FLAG_VOID, /* wFlags */
    //     { /* bTypeValue */
    //         LO_WORD(GATT_UUID_CHAR_USER_DESCR),
    //         HI_WORD(GATT_UUID_CHAR_USER_DESCR),
    //     },
    //     (sizeof(HID_WRITE_IN_USER_SESCR) - 1), /* bValueLen */
    //     (void *)HID_WRITE_IN_USER_SESCR,
    //     (GATT_PERM_READ) /* wPermissions */
    // },
};

const static uint16_t TRANSMIT_SRV_TABLE_SIZE = sizeof(TRANSMIT_SRV_TABLE);

bool transmit_set_parameter(T_TRANSMIT_PARAM_TYPE param_type, uint8_t length, uint8_t *p_value)
{
    bool ret = true;

    switch (param_type)
    {
    default:
        {
            ret = false;
            PROFILE_PRINT_ERROR1("transmit_set_parameter: unknown param_type 0x%02x", param_type);
        }
        break;

    case TRANSMIT_PARAM_BATTERY_LEVEL:
        {
            APP_PRINT_INFO2("transmit_set_parameter: TRANSMIT_PARAM_BATTERY_LEVEL len %d, data %b", length,
                            TRACE_BINARY(length, p_value));
            if (length != sizeof(uint8_t))
            {
                ret = false;
            }
            else
            {
                battery_level = p_value[0];
            }
        }
        break;
    }

    return ret;
}

/**
  * @brief transmit data.
  *
  * @param[in] conn_id   Connection ID.
  * @param[in] service_id   Service ID.
  * @param[in] len   Length of value to be sendt.
  * @param[in] p_value Pointer of value to be sent.
  * @return transmit result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool transmit_srv_tx_data(uint8_t conn_id, uint16_t len, uint8_t *p_value)
{
    APP_PRINT_INFO3("transmit_srv_tx_data: conn_id %d len %d, data %b", conn_id,
                    len,
                    TRACE_BINARY(len, p_value));
    return server_send_data(conn_id, transmit_srv_id, TRANSMIT_SVC_TX_DATA_INDEX, p_value,
                            len, GATT_PDU_TYPE_ANY);
}

bool transmit_battery_level_value_read_confirm(uint8_t conn_id, uint8_t service_id,
                                               uint8_t battery_level)
{
    APP_PRINT_INFO3("transmit_battery_level_value_read_confirm: conn_id %d service_id %d, battery_level %d",
                    conn_id,
                    service_id,
                    battery_level);
    if (transmit_read_battery_level_pending == true)
    {
        transmit_read_battery_level_pending = false;
        return server_attr_read_confirm(conn_id, service_id, TRANSMIT_SVC_RX_DATA_INDEX,
                                        &battery_level, sizeof(battery_level), APP_RESULT_SUCCESS);
    }
    else
    {
        return false;
    }
}

void transmit_write_post_callback(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_idx,
                                  uint16_t length, uint8_t *p_value)
{
    APP_PRINT_INFO4("transmit_write_post_callback: conn_id %d, service_id %d, attrib_index 0x%x, length %d",
                    conn_id, service_id, attrib_idx, length);

    switch (attrib_idx)
    {
    case TRANSMIT_SVC_RX_DATA_INDEX:
        {
            //transmit_srv_tx_data
            APP_PRINT_TRACE0("transmit_write_post_callback: wait TRANSMIT_SVC_TX_DATA_INDEX");
            app_harman_wait_ota_report(conn_id);
        }
        break;

    default:
        {

        }
        break;
    }
}

T_APP_RESULT transmit_srv_write_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_idx,
                                   T_WRITE_TYPE write_type, uint16_t len,
                                   uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_post_proc)
{
    APP_PRINT_INFO3("transmit_srv_write_cb: attrib_idx %d len %d, data %b", attrib_idx,
                    len,
                    TRACE_BINARY(len, p_value));
    T_TRANSMIT_SRV_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    *p_write_post_proc = transmit_write_post_callback;
    switch (attrib_idx)
    {
    case TRANSMIT_SVC_RX_DATA_INDEX:
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.conn_id  = conn_id;
            callback_data.attr_index = attrib_idx;
            callback_data.msg_data.rx_data.len = len;
            callback_data.msg_data.rx_data.p_value = p_value;

            if (transmit_srv_cb)
            {
                transmit_srv_cb(service_id, (void *)&callback_data);
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("transmit_srv_write_cb: attrib_idx %d not found", attrib_idx);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }
    return cause;
}

T_APP_RESULT transmit_srv_read_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                                  uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    *p_length = 0;

    PROFILE_PRINT_INFO2("transmit_attr_read_cb: attrib_index %d offset %d", attrib_index, offset);

    switch (attrib_index)
    {
    default:
        {
            PROFILE_PRINT_ERROR0("transmit_attr_read_cb: unknown attrib_index");
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;

    case TRANSMIT_SVC_TX_DATA_INDEX:
        {
            T_TRANSMIT_SRV_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.conn_id = conn_id;
            callback_data.msg_data.read_value_index = TRANSMIT_READ_BATTERY_LEVEL;
            cause = transmit_srv_cb(service_id, (void *)&callback_data);
            if (cause == APP_RESULT_PENDING)
            {
                transmit_read_battery_level_pending = true;
            }

            *pp_value = &battery_level;
            *p_length = sizeof(battery_level);
        }
        break;
    }
    return (cause);
}

void transmit_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_idx,
                             uint16_t cccd_bits)
{
    T_TRANSMIT_SRV_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    callback_data.conn_id = conn_id;

    APP_PRINT_INFO2("transmit_cccd_update_cb: attrib_idx %d, cccd_bits 0x%04x", attrib_idx, cccd_bits);
    switch (attrib_idx)
    {
    case TRANSMIT_SVC_TX_DATA_CCCD_INDEX:
        {
#ifdef TRANS_SRV_TRX_NEED_RSP
            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_INDICATE)
#else
            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
#endif
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_value = TRANSMIT_SVC_TX_DATA_CCCD_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_value = TRANSMIT_SVC_TX_DATA_CCCD_DISABLE;
            }

            callback_data.attr_index = TRANSMIT_SVC_TX_DATA_CCCD_INDEX;

            if (transmit_srv_cb)
            {
                transmit_srv_cb(service_id, (void *)&callback_data);
            }
            break;
        }
    default:
        break;
    }
    return;
}

/**
 * @brief Transmit Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS TRANSMIT_SRV_CBS =
{
    transmit_srv_read_cb,  // Read callback function pointer
    transmit_srv_write_cb, // Write callback function pointer
    transmit_cccd_update_cb  // CCCD update callback function pointer
};

/**
  * @brief Add transmit service to the BLE stack database.
  *
  * @param[in]   p_func_cb  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID transmit_srv_add(void *p_func_cb)
{
    if (false == server_add_service(&transmit_srv_id,
                                    (uint8_t *)TRANSMIT_SRV_TABLE,
                                    TRANSMIT_SRV_TABLE_SIZE,
                                    TRANSMIT_SRV_CBS))
    {
        APP_PRINT_ERROR1("transmit_srv_add: srv_id %d", transmit_srv_id);
        transmit_srv_id = 0xff;
    }
    transmit_srv_cb = (P_FUN_SERVER_GENERAL_CB)p_func_cb;
    return transmit_srv_id;
}
