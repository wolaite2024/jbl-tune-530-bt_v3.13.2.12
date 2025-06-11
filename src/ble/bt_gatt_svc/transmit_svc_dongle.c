/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     transmit_svc_dongle.c
* @brief    Transmit service for dongle source file.
* @details  Interfaces to access transmit service.
* @author   wade
* @date     2023-9-4
* @version  v0.1
*********************************************************************************************************
*/
#if F_APP_TRANSMIT_SRV_DONGLE_SUPPORT
#include <stdint.h>
#include "gatt.h"
#include <string.h>
#include "trace.h"
#include "transmit_svc_dongle.h"
#include "gap.h"
#include "gap_conn_le.h"

/** @brief  transmit service related UUIDs. */
#define TRANSMIT_SVC_DONGLE_CHAR_UUID_RX_DATA         0xFD07
#define TRANSMIT_SVC_DONGLE_CHAR_UUID_TX_DATA         0xFD08
#define TRANSMIT_SVC_DONGLE_CHAR_UUID_DEVICE_INFO     0xFD09

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static P_FUN_EXT_SERVER_GENERAL_CB transmit_srv_dongle_cb = NULL;

const uint8_t GATT_UUID128_TRANSMIT_SRV_DONGLE[16] = { 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFD, 0x06, 0x00, 0x00};
static T_SERVER_ID transmit_srv_dongle_id = 0xff;

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL transmit_svc_dongle_table[] =
{
    /*----------------- transmit service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                            /* bValueLen     */
        (void *)GATT_UUID128_TRANSMIT_SRV_DONGLE,         /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
//#ifdef TRANS_SRV_TRX_NEED_RSP
//            GATT_CHAR_PROP_WRITE
//#else
//            GATT_CHAR_PROP_WRITE_NO_RSP
//#endif
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP)

            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* transmit rx*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_RX_DATA),
            HI_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_RX_DATA)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE/* permissions */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
#ifdef TRANS_SRV_TRX_NEED_RSP
            GATT_CHAR_PROP_INDICATE
#else
            GATT_CHAR_PROP_NOTIFY
#endif
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* transmit tx*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_TX_DATA),
            HI_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_TX_DATA)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND_ENCRYPTED_REQ/* permissions */
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
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP |
#ifdef TRANS_SRV_TRX_NEED_RSP
             GATT_CHAR_PROP_INDICATE
#else
             GATT_CHAR_PROP_NOTIFY
#endif
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /* Device Info*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_DEVICE_INFO),
            HI_WORD(TRANSMIT_SVC_DONGLE_CHAR_UUID_DEVICE_INFO)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE | GATT_PERM_NOTIF_IND) /* permissions */
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
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    }
};

const static uint16_t transmit_svc_dongle_table_size = sizeof(transmit_svc_dongle_table);

/**
  * @brief transmit data.
  *
  * @param[in] conn_handle  connection handle.
  * @param[in] len length of report data.
  * @param[in] p_value report value pointer.
  * @return transmit result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool transmit_srv_dongle_tx_data(uint8_t conn_handle, uint16_t len, uint8_t *p_value)
{
    return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, transmit_srv_dongle_id,
                              TRANSMIT_SVC_DONGLE_TX_DATA_INDEX, p_value,
                              len, GATT_PDU_TYPE_NOTIFICATION);
}

void transmit_srv_dongle_write_post_callback(uint16_t conn_handle, uint16_t cid,
                                             T_SERVER_ID service_id,
                                             uint16_t attrib_idx,
                                             uint16_t length, uint8_t *p_value)
{
    APP_PRINT_INFO4("transmit_srv_dongle_write_post_callback: conn_handle %d, service_id %d, attrib_idx 0x%x, length %d",
                    conn_handle, service_id, attrib_idx, length);

    switch (attrib_idx)
    {
    case TRANSMIT_SVC_DONGLE_RX_DATA_INDEX:
        {
            //transmit_srv_dongle_tx_data
        }
        break;

    default:
        {

        }
        break;
    }
}

T_APP_RESULT transmit_srv_dongle_write_cb(uint16_t conn_handle, uint16_t cid,
                                          T_SERVER_ID service_id,
                                          uint16_t attrib_idx, T_WRITE_TYPE write_type, uint16_t len,
                                          uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_TRANSMIT_SRV_DONGLE_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    *p_write_post_proc = transmit_srv_dongle_write_post_callback;

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.conn_handle  = conn_handle;
    callback_data.cid = cid;
    callback_data.attr_index = attrib_idx;

    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    callback_data.conn_id = conn_id;

    APP_PRINT_INFO2("transmit_srv_dongle_write_cb: attrib_idx %d, conn_handle 0x%04x", attrib_idx,
                    conn_handle);

    switch (attrib_idx)
    {
    case TRANSMIT_SVC_DONGLE_RX_DATA_INDEX:
        {
            /* Notify Application. */
            callback_data.msg_data.rx_data.len = len;
            callback_data.msg_data.rx_data.p_value = p_value;
        }
        break;

    case TRANSMIT_SVC_DONGLE_DEVICE_INFO_INDEX:
        {
            if (len == sizeof(T_TRANSMIT_SRV_DONGLE_DEVICE_INFO))
            {
                memcpy(&callback_data.msg_data.device_info, p_value, len);
            }
            else
            {
                cause  = APP_RESULT_INVALID_VALUE_SIZE;
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("transmit_srv_dongle_write_cb: attrib_idx %d not found", attrib_idx);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }

    if (cause == APP_RESULT_SUCCESS && transmit_srv_dongle_cb)
    {
        transmit_srv_dongle_cb(service_id, (void *)&callback_data);
    }

    return cause;
}

void transmit_srv_dongle_cccd_update_cb(uint16_t conn_handle,  uint16_t cid, T_SERVER_ID service_id,
                                        uint16_t attrib_idx,
                                        uint16_t cccd_bits)
{
    T_TRANSMIT_SRV_DONGLE_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    callback_data.conn_handle = conn_handle;
    callback_data.cid = cid;

    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    callback_data.conn_id = conn_id;

    APP_PRINT_INFO2("transmit_srv_dongle_cccd_update_cb: attrib_idx %d, cccd_bits 0x%04x", attrib_idx,
                    cccd_bits);
    switch (attrib_idx)
    {
    case TRANSMIT_SVC_DONGLE_TX_DATA_CCCD_INDEX:
        {
#ifdef TRANS_SRV_TRX_NEED_RSP
            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_INDICATE)
#else
            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
#endif
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_value = TRANSMIT_SVC_DONGLE_TX_DATA_CCCD_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_value = TRANSMIT_SVC_DONGLE_TX_DATA_CCCD_DISABLE;
            }

            callback_data.attr_index = TRANSMIT_SVC_DONGLE_TX_DATA_CCCD_INDEX;

            if (transmit_srv_dongle_cb)
            {
                transmit_srv_dongle_cb(service_id, (void *)&callback_data);
            }
            break;
        }
    default:
        break;
    }
    return;
}

/**
 * @brief transmit service Callbacks.
*/
const T_FUN_GATT_EXT_SERVICE_CBS transmit_svc_dongle_cbs =
{
    NULL,  // Read callback function pointer
    transmit_srv_dongle_write_cb, // Write callback function pointer
    transmit_srv_dongle_cccd_update_cb  // CCCD update callback function pointer
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
T_SERVER_ID transmit_srv_dongle_add(void *p_func_cb)
{
    if (false == gatt_svc_add(&transmit_srv_dongle_id,
                              (uint8_t *)transmit_svc_dongle_table,
                              transmit_svc_dongle_table_size,
                              &transmit_svc_dongle_cbs, NULL))
    {
        PROFILE_PRINT_ERROR1("ias_add_service: transmit_srv_dongle_id %d", transmit_srv_dongle_id);
        transmit_srv_dongle_id = 0xff;
    }

    transmit_srv_dongle_cb = (P_FUN_EXT_SERVER_GENERAL_CB)p_func_cb;
    return transmit_srv_dongle_id;
}

#endif
