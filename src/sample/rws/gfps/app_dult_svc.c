/*
 * Copyright (c) 2024, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FINDER_SUPPORT
#include "stddef.h"
#include "app_dult_svc.h"
#include "gap_conn_le.h"
#include "trace.h"

// 15190001-12F4-C226-88ED-2AC5579F2A85
const uint8_t GATT_UUID_NON_OWNER_SRV[16] = {0x85, 0x2A, 0x9F, 0x57, 0xC5, 0x2A, 0xED, 0x88, 0x26, 0xC2, 0xF4, 0x12, 0x01, 0x00, 0x19, 0x15};

#define GATT_UUID_NON_OWNER_CHAR            0x0E, 0x68, 0x21, 0x74, 0x37, 0x48, \
    0x61, 0xBF, 0x92, 0xFB, 0x68, 0x1D, \
    0x01, 0x00, 0x0C, 0x8E               // 8E0C0001-1D68-FB92-BF61-48377421680E

static P_FUN_EXT_SERVER_GENERAL_CB  pfn_dult_cb = NULL;

static const T_ATTRIB_APPL dult_attr_tbl[] =
{
    /* <<Primary Service>>, .. 0 */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),        /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                           /* bValueLen     */
        (void *)GATT_UUID_NON_OWNER_SRV,            /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_INDICATE)                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT, /* flags */
        {                                           /* type_value */
            GATT_UUID_NON_OWNER_CHAR
        },
        0,                                          /* variable size */
        NULL,
        (GATT_PERM_WRITE | GATT_PERM_NOTIF_IND)     /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,
        {
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT),
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)
    },
};

T_APP_RESULT dult_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attr_index, T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t *p_data = p_value;

    T_DULT_CALLBACK_DATA callback_data;
    callback_data.conn_handle = conn_handle;
    callback_data.cid = cid;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    LE_STREAM_TO_UINT16(callback_data.msg_data.opcode, p_data);
    callback_data.msg_data.p_operands = p_data;
    callback_data.msg_data.operands_len = length - 2;

    if (attr_index == DULT_CHAR_NONE_OWNER_INDEX)
    {
        if (pfn_dult_cb)
        {
            pfn_dult_cb(service_id, (void *)&callback_data);
        }
    }
    else
    {
        APP_PRINT_ERROR1("dult_attr_write_cb: attr_index %d not found", attr_index);
        cause  = APP_RESULT_ATTR_NOT_FOUND;
    }

    return cause;
}

void dult_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id, uint16_t index,
                         uint16_t ccc_bits)
{
    bool handle = true;

    T_DULT_CALLBACK_DATA callback_data;
    callback_data.cid = cid;
    callback_data.conn_handle = conn_handle;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;

    switch (index)
    {
    case DULT_CHAR_NONE_OWNER_CCCD_INDEX:
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_INDICATE)
            {
                APP_PRINT_INFO0("dult_cccd_update_cb: enabled");
            }
            else
            {
                APP_PRINT_INFO0("dult_cccd_update_cb: disabled");
            }
        }
        break;

    default:
        {
            handle = false;
            APP_PRINT_ERROR1("dult_cccd_update_cb: attr not found, index %d", index);
        }
        break;
    }

    if (pfn_dult_cb && (handle == true))
    {
        pfn_dult_cb(service_id, (void *)&callback_data);
    }

    return;
}

/**
 * @brief Service callback function
*/
const T_FUN_GATT_EXT_SERVICE_CBS  dult_cbs =
{
    NULL,                       // read
    dult_attr_write_cb,         // write
    dult_cccd_update_cb,        // cccd update
};

/**
 * @brief Send notificaiton
*/
bool app_dult_send_indication(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint8_t *p_data, uint16_t data_len)
{
    APP_PRINT_INFO0("app_dult_send_indication");
    return gatt_svc_send_data(conn_handle, cid, service_id,
                              DULT_CHAR_NONE_OWNER_INDEX,
                              p_data, data_len, GATT_PDU_TYPE_INDICATION);
}


/**
 * @brief Send command response
*/
bool app_dult_report_cmd_rsp(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                             uint16_t opcode, T_DULT_CMD_RSP_STATE state)
{
    struct
    {
        uint16_t opcode;
        uint16_t opcode_rsp;
        T_DULT_CMD_RSP_STATE state;
    } __attribute__((packed)) rpt = {};
    rpt.opcode = DULT_OPCODE_CTRL_CMD_RSP;
    rpt.opcode_rsp = opcode;
    rpt.state = state;
    APP_PRINT_INFO2("app_dult_report_cmd_rsp: opcode 0x%x, rsp_state %d", opcode, state);
    return app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
}

/**
 * @brief Add DULT service
*/
T_SERVER_ID app_dult_add_service(void *p_func)
{
    T_SERVER_ID service_id;

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)dult_attr_tbl,
                              sizeof(dult_attr_tbl),
                              &dult_cbs, NULL))
    {
        APP_PRINT_ERROR0("dult_add_service:failed");
        service_id = 0xff;
    }
    pfn_dult_cb = (P_FUN_EXT_SERVER_GENERAL_CB)p_func;
    return service_id;
}
#endif
