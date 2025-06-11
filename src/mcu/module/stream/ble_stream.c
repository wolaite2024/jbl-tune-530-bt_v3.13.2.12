/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#include "vector.h"
#include "app_cfg.h"
#include "ble_stream.h"
//#include "ring_buffer.h"
#include "profile_server.h"
#include "gap_conn_le.h"
#include "gap.h"
#include "profile_server.h"
#include "os_mem.h"
#include "trace.h"
#include <string.h>
#include <stddef.h>


/* Receiving buffer size, it can be modified as needed */
#define BLE_STREAM_RX_BUFFER_SIZE                 500
/* Invalid id in BLE stream register table, do not modify */
#define INVAILAD_BLE_STREAM_TABLE_ID              MAX_BLE_STREAM_NUM
/* ATT header length, do not modify*/
#define ATT_HEADER_LENGTH                         3
/* Invalid BLE service id, do not modify */
#define INVAILAD_BLE_SERVICE_ID                   0xFF



#define ANY_ATTR_IDX    (0xff)


typedef struct
{
    T_REMOTE_RELAY_HANDLE               relay_handle;
    VECTOR stream_vector;
} BLE_STREAM_MGR;


BLE_STREAM_MGR ble_stream_mgr = {.stream_vector = NULL, .relay_handle = NULL};


typedef struct
{
    uint8_t                 bd_addr[BLUETOOTH_ADDR_LENGTH];
    uint8_t                 service_id;
    uint8_t                 conn_id;
    uint32_t                read_attr_idx;
    uint32_t                write_attr_idx;
    T_GATT_PDU_TYPE         read_pdu_type;
    T_GATT_PDU_TYPE         write_pdu_type;
} BLE_STREAM_REMOTE_INFO;


typedef enum
{
    STREAM_CREATED          = 0x00,
    STREAM_DELETED          = 0x01,
    STREAM_CONNECTED        = 0x02,
    STREAM_DISCONNECTED     = 0x03,
    STREAM_COPY_PUB_ADDR    = 0x04,
} REMOTE_MSG_TYPE;


static bool ble_stream_delete(BLE_STREAM ble_stream);


static bool match_ble_stream_roughly(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    uint32_t read_attr_idx = template_stream->read_attr_idx;
    uint32_t write_attr_idx = template_stream->write_attr_idx;

    if (ble_stream->service_id != template_stream->service_id)
    {
        return false;
    }

    if (read_attr_idx != ANY_ATTR_IDX)
    {
        if (read_attr_idx != ble_stream->read_attr_idx)
        {
            APP_PRINT_ERROR1("match_ble_stream_roughly: not match read_attr_idx %d", read_attr_idx);
            return false;
        }
    }

    if (write_attr_idx != ANY_ATTR_IDX)
    {
        if (write_attr_idx != ble_stream->write_attr_idx)
        {
            APP_PRINT_ERROR1("match_ble_stream_roughly: not match write_attr_idx %d", write_attr_idx);
            return false;
        }
    }

    return true;
}


static bool match_ble_stream_by_id(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    if (match_ble_stream_roughly(ble_stream, template_stream) == false)
    {
        return false;
    }

    if (ble_stream->conn_id != template_stream->conn_id)
    {
        APP_PRINT_ERROR2("match_ble_stream_by_id: conn_id %d, target conn_id %d", ble_stream->conn_id,
                         template_stream->conn_id);
        return false;
    }

    return true;
}


BLE_STREAM ble_stream_search_by_id(uint32_t service_id, uint8_t conn_id,
                                   uint8_t read_attr_idx, uint8_t write_attr_idx)
{
    BLE_STREAM ble_stream = NULL;
    BLE_STREAM_STRUCT ble_stream_struct = {0};
    ble_stream_struct.conn_id = conn_id;
    ble_stream_struct.service_id = service_id;
    ble_stream_struct.read_attr_idx = read_attr_idx;
    ble_stream_struct.write_attr_idx = write_attr_idx;

    APP_PRINT_TRACE4("ble_stream_search_by_id: conn_id %d, service_id %d, read_attr_idx %d, write_attr_idx %d",
                     conn_id, service_id, read_attr_idx, write_attr_idx);

    ble_stream = vector_search(ble_stream_mgr.stream_vector, (V_PREDICATE)match_ble_stream_by_id,
                               &ble_stream_struct);

    return ble_stream;
}


static bool match_ble_stream_by_addr(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    if (match_ble_stream_roughly(ble_stream, template_stream) == false)
    {
        return false;
    }

    if (memcmp(ble_stream->common.bd_addr, template_stream->common.bd_addr,
               sizeof(ble_stream->common.bd_addr)) != 0)
    {
        APP_PRINT_ERROR2("match_ble_stream_by_id: template bd_addr %s, target bd_addr %s",
                         TRACE_BDADDR(template_stream->common.bd_addr), TRACE_BDADDR(ble_stream->common.bd_addr));
        return false;
    }

    return true;
}


BLE_STREAM ble_stream_search_by_addr(uint32_t service_id, uint8_t *bd_addr,
                                     uint8_t read_attr_idx, uint8_t write_attr_idx)
{
    BLE_STREAM ble_stream = NULL;
    BLE_STREAM_STRUCT ble_stream_struct = {0};
    memcpy(ble_stream_struct.common.bd_addr, bd_addr, sizeof(ble_stream_struct.common.bd_addr));
    ble_stream_struct.service_id = service_id;
    ble_stream_struct.read_attr_idx = read_attr_idx;
    ble_stream_struct.write_attr_idx = write_attr_idx;

    APP_PRINT_TRACE3("ble_stream_search_by_addr: bd_addr %s, read_attr_idx %d, write_attr_idx %d",
                     TRACE_BDADDR(bd_addr), read_attr_idx, write_attr_idx);

    ble_stream = vector_search(ble_stream_mgr.stream_vector, (V_PREDICATE) match_ble_stream_by_addr,
                               &ble_stream_struct);

    return ble_stream;
}


static bool register_ble_stream(BLE_STREAM ble_stream)
{
    bool ret = false;

    ret = vector_add(ble_stream_mgr.stream_vector, ble_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("register_ble_stream: register failed");
    }

    return ret;
}


static bool unregister_ble_stream(BLE_STREAM ble_stream)
{
    bool ret = false;

    ret = vector_remove(ble_stream_mgr.stream_vector, ble_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("unregister_ble_stream: unregister failed");
    }

    return ret;
}


static bool set_mtu(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    if (ble_stream == NULL)
    {
        return false;
    }

    if (ble_stream->conn_id != template_stream->conn_id)
    {
        return false;
    }

    ble_stream->common.mtu = template_stream->common.mtu;

    return true;
}




static void ble_stream_sync_to_primary(REMOTE_MSG_TYPE msg_type, BLE_STREAM_REMOTE_INFO *p_info)
{
    T_BLE_STREAM_EVENT_PARAM ble_stream_param = {0};
    T_GAP_REMOTE_ADDR_TYPE type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t conn_id = 0xff;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("ble_stream_sync_to_primary: this dev should be secondary");
        return;
    }

    APP_PRINT_TRACE2("ble_stream_sync_to_primary: msg_type %d, service_id %d", msg_type,
                     p_info->service_id);

    switch (msg_type)
    {
    case STREAM_CONNECTED:
        le_get_conn_id(p_info->bd_addr, type, &conn_id);
        ble_stream_param.write_cccd_param.conn_id = conn_id;
        ble_stream_param.write_cccd_param.service_id = p_info->service_id;
        ble_stream_event_process(BLE_STREAM_WRITE_CCCD_EVENT, &ble_stream_param);
        break;

    case STREAM_DISCONNECTED:
        le_get_conn_id(p_info->bd_addr, type, &conn_id);
        ble_stream_param.write_cccd_param.conn_id = conn_id;
        ble_stream_param.disconnect_param.service_id = p_info->service_id;
        ble_stream_event_process(BLE_STREAM_DISCONNECT_EVENT, &ble_stream_param);
        break;

    case STREAM_CREATED:
        {
            BLE_INIT_SETTINGS ble_init_settings = {0};
            ble_init_settings.conn_id = p_info->conn_id;
            ble_init_settings.read_attr_idx = p_info->read_attr_idx;
            ble_init_settings.read_pdu_type = p_info->read_pdu_type;
            ble_init_settings.service_id = p_info->service_id;
            ble_init_settings.write_attr_idx = p_info->write_attr_idx;
            ble_init_settings.write_pdu_type = p_info->write_pdu_type;
            ble_stream_create(&ble_init_settings);
        }
        break;

    case STREAM_DELETED:
        {
            BLE_STREAM ble_stream = NULL;
            ble_stream = ble_stream_search_by_id(p_info->service_id, p_info->conn_id,
                                                 p_info->read_attr_idx, p_info->write_attr_idx);
            ble_stream_delete(ble_stream);
        }
        break;

    case STREAM_COPY_PUB_ADDR:
        {
            ble_stream_copy_pub_addr(p_info->conn_id, p_info->bd_addr);
        }
        break;

    default:
        break;
    }
}


static void ble_stream_relay_cback(REMOTE_MSG_TYPE msg_type, T_REMOTE_RELAY_STATUS status,
                                   BLE_STREAM_REMOTE_INFO *p_info, uint16_t len)
{
    if (len != sizeof(*p_info))
    {
        APP_PRINT_ERROR0("ble_stream_relay_cback: struct len was wrong");
        return;
    }

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("ble_stream_relay_cback: invalid status %d", status);
        return;
    }

    APP_PRINT_TRACE0("ble_stream_relay_cback");

    ble_stream_sync_to_primary(msg_type, p_info);
}


static void ble_stream_send_info_to_sec(BLE_STREAM ble_stream, REMOTE_MSG_TYPE msg_type)
{
    BLE_STREAM_REMOTE_INFO remote_info = {0};

    if (ble_stream == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_send_info_to_sec: ble_stream is NULL");
        return;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("ble_stream_send_info_to_sec: this dev should be secondary");
        return;
    }

    APP_PRINT_TRACE2("ble_stream_send_info_to_sec: msg_type %d, service_id %d", msg_type,
                     ble_stream->service_id);

    memcpy(remote_info.bd_addr, ble_stream->common.bd_addr, sizeof(remote_info.bd_addr));
    remote_info.service_id = ble_stream->service_id;
    remote_info.conn_id = ble_stream->conn_id;
    remote_info.read_pdu_type = ble_stream->read_pdu_type;
    remote_info.read_attr_idx = ble_stream->read_attr_idx;
    remote_info.write_attr_idx = ble_stream->write_attr_idx;
    remote_info.write_pdu_type = ble_stream->write_pdu_type;

    remote_async_msg_relay(ble_stream_mgr.relay_handle, msg_type,
                           (uint8_t *)&remote_info, sizeof(remote_info), false);
}


static bool disconnect_streams_by_id(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    if (ble_stream == NULL)
    {
        return false;
    }

    if (ble_stream->service_id != template_stream->service_id)
    {
        return false;
    }

    if (ble_stream->conn_id != template_stream->conn_id)
    {
        return false;
    }

    __common_stream_disconnect(&ble_stream->common);

    ble_stream_send_info_to_sec(ble_stream, STREAM_DISCONNECTED);

    return true;
}


static void ble_stream_write_cccd_process(T_BLE_STREAM_EVENT_PARAM *event_param)
{
    T_GAP_REMOTE_ADDR_TYPE type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]  = {0};

    uint8_t conn_id     = event_param->write_cccd_param.conn_id;
    uint8_t service_id  = event_param->write_cccd_param.service_id;

    uint16_t mtu = 0;
    uint8_t remote_credits = 0;

    le_get_conn_addr(conn_id, bd_addr, (uint8_t *)&type);

    BLE_STREAM ble_stream;

    APP_PRINT_TRACE4("ble_stream_write_cccd_process: conn_id %d, service_id %d, bd_addr %s, type %d",
                     conn_id, service_id, TRACE_BDADDR(bd_addr), type);

    ble_stream = ble_stream_search_by_id(service_id, conn_id, ANY_ATTR_IDX, ANY_ATTR_IDX);

    if (ble_stream == NULL)
    {
        return;
    }

    le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, (void *)&mtu,
                      ble_stream->conn_id);

    mtu = mtu - ATT_HEADER_LENGTH;

    le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, (void *)&remote_credits);

    ble_stream->conn_id        = conn_id;

    __common_stream_connect(&ble_stream->common, mtu, remote_credits);

    ble_stream_send_info_to_sec(ble_stream, STREAM_CONNECTED);
}


static void ble_stream_disconnect_process(T_BLE_STREAM_EVENT_PARAM *event_param)
{
    uint8_t conn_id = event_param->disconnect_param.conn_id;
    uint8_t service_id = event_param->disconnect_param.service_id;
    BLE_STREAM_STRUCT ble_stream_struct = {0};
    ble_stream_struct.conn_id = conn_id;
    ble_stream_struct.service_id = service_id;

    //disconnect all streams for service_id and conn_id. stream may diff for different attr_idx.
    vector_mapping(ble_stream_mgr.stream_vector, (V_MAPPER)disconnect_streams_by_id,
                   &ble_stream_struct);
}


static void ble_stream_receive_data_process(T_BLE_STREAM_EVENT_PARAM *event_param)
{
    BLE_STREAM ble_stream = NULL;
    T_GAP_REMOTE_ADDR_TYPE    type = GAP_REMOTE_ADDR_LE_PUBLIC;

    uint8_t conn_id         = event_param->receive_data_param.conn_id;
    uint8_t *data           = event_param->receive_data_param.data;
    uint16_t data_length    = event_param->receive_data_param.length;
    uint8_t service_id      = event_param->receive_data_param.service_id;
    uint8_t index           = event_param->receive_data_param.index;

    APP_PRINT_TRACE3("ble_stream_receive_data_process: conn_id %d, type %d, data_length %d",
                     conn_id, type, data_length);

    ble_stream = ble_stream_search_by_id(service_id, conn_id, index, ANY_ATTR_IDX);
    if (ble_stream == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_receive_data_process: cannot found ble_stream");
        return;
    }

    __common_stream_receive_data(&ble_stream->common, data, data_length);
}

static void ble_stream_update_credit_process(T_BLE_STREAM_EVENT_PARAM *event_param)
{
    uint8_t conn_id = event_param->update_credit_param.conn_id;
    uint8_t service_id = event_param->update_credit_param.service_id;
    uint8_t remote_credits    = event_param->update_credit_param.credits;

    APP_PRINT_TRACE3("ble_stream_update_credit_process: conn_id %d, service_id %d, remote_credits %d",
                     conn_id, service_id, remote_credits);

    BLE_STREAM ble_stream = ble_stream_search_by_id(service_id, conn_id, ANY_ATTR_IDX, ANY_ATTR_IDX);

    if (ble_stream == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_update_credit_process: ble_stream is NULL");
        return;
    }

    if (__common_stream_update_credits(&ble_stream->common, remote_credits) == false)
    {
        return;
    }
}


static void ble_stream_update_mtu_process(T_BLE_STREAM_EVENT_PARAM *event_param)
{
    uint8_t     conn_id = event_param->update_mut_param.conn_id;
    uint16_t    mtu    = event_param->update_mut_param.mtu;

    BLE_STREAM_STRUCT ble_stream_struct = {0};
    ble_stream_struct.conn_id = conn_id;
    ble_stream_struct.common.mtu = mtu;

    vector_mapping(ble_stream_mgr.stream_vector, (V_MAPPER)set_mtu, &ble_stream_struct);
}


void ble_stream_event_process(T_BLE_STREAM_EVENT event,
                              T_BLE_STREAM_EVENT_PARAM *event_param)
{
    if (event_param == NULL)
    {
        return;
    }

    APP_PRINT_TRACE1("ble_stream_event_process event is %d", event);

    switch (event)
    {
    case BLE_STREAM_WRITE_CCCD_EVENT:
        {
            ble_stream_write_cccd_process(event_param);
        }
        break;

    case BLE_STREAM_DISCONNECT_EVENT:
        {
            ble_stream_disconnect_process(event_param);
        }
        break;

    case BLE_STREAM_RECEIVE_DATA_EVENT:
        {
            ble_stream_receive_data_process(event_param);
        }
        break;

    case BLE_STREAM_UPDATE_CREDIT_EVENT:
        {
            ble_stream_update_credit_process(event_param);
        }
        break;

    case BLE_STREAM_UPDATE_MTU_EVENT:
        {
            ble_stream_update_mtu_process(event_param);
        }
        break;

    default:
        break;
    }
}



static bool ble_stream_check_write_condition(BLE_STREAM ble_stream, uint32_t length)
{
    return __common_stream_check_write_condition(&ble_stream->common, length);
}


bool ble_stream_write(BLE_STREAM ble_stream, uint8_t *data, uint32_t len)
{
    if (ble_stream_check_write_condition(ble_stream, len) == false)
    {
        return false;
    }

    if (server_send_data(ble_stream->conn_id, ble_stream->service_id,
                         ble_stream->write_attr_idx, data,
                         len, ble_stream->write_pdu_type))
    {
        APP_PRINT_TRACE0("ble_stream_write success");
        ble_stream->common.remote_credits--;
        return true;
    }
    else
    {
        APP_PRINT_TRACE0("ble_stream_write fail");
        ble_stream->common.write_block = true;
        return false;
    }
}


static bool ble_stream_read(BLE_STREAM ble_stream, uint8_t **p_buf, uint32_t *p_len)
{
    return __common_stream_read(&ble_stream->common, p_buf, p_len);
}


static void ble_stream_get_addr(BLE_STREAM ble_stream, uint8_t *bd_addr)
{
    __common_stream_get_bd_addr(&ble_stream->common, bd_addr);
}


static uint32_t ble_stream_get_mtu(BLE_STREAM ble_stream)
{
    APP_PRINT_TRACE0("ble_stream_get_mtu");
    return __common_stream_get_mtu(&ble_stream->common);
}


static bool ble_stream_copy_single_pub_addr(BLE_STREAM ble_stream, BLE_STREAM template_stream)
{
    if (ble_stream->conn_id != template_stream->conn_id)
    {
        return false;
    }

    memcpy(ble_stream->common.bd_addr, template_stream->common.bd_addr,
           sizeof(ble_stream->common.bd_addr));
    APP_PRINT_TRACE0("ble_stream_copy_single_pub_addr: copy successfully");

    return true;
}


bool ble_stream_copy_pub_addr(uint8_t conn_id, uint8_t *pub_addr)
{
    BLE_STREAM_STRUCT ble_stream_struct = {0};

    APP_PRINT_TRACE2("ble_stream_copy_pub_addr: conn_id %d, pub_addr %s", conn_id,
                     TRACE_BDADDR(pub_addr));

    ble_stream_struct.conn_id = conn_id;
    memcpy(ble_stream_struct.common.bd_addr, pub_addr, sizeof(ble_stream_struct.common.bd_addr));

    vector_mapping(ble_stream_mgr.stream_vector, (V_MAPPER)ble_stream_copy_single_pub_addr,
                   &ble_stream_struct);

    ble_stream_send_info_to_sec(&ble_stream_struct, STREAM_COPY_PUB_ADDR);

    return true;
}


static bool ble_stream_delete(BLE_STREAM ble_stream)
{
    APP_PRINT_TRACE0("ble_stream_delete");
    if (ble_stream == NULL)
    {
        return false;
    }

    __common_stream_delete(&ble_stream->common);

    unregister_ble_stream(ble_stream);

    ble_stream_send_info_to_sec(ble_stream, STREAM_DELETED);

    os_mem_free(ble_stream);
    return true;
}


static void ble_stream_default(BLE_STREAM ble_stream)
{
    __common_stream_default(&ble_stream->common);
    ble_stream->common.bt_type = COMMON_STREAM_BT_LE;

    ble_stream->conn_id = 0xff;
    ble_stream->service_id = 0;
    ble_stream->authenticated = false;
    ble_stream->read_attr_idx = 0xff;
    ble_stream->write_attr_idx = 0xff;
    ble_stream->read_pdu_type = GATT_PDU_TYPE_ANY;
    ble_stream->write_pdu_type = GATT_PDU_TYPE_ANY;
}


bool ble_stream_init(uint32_t max_stream_num)
{
    APP_PRINT_TRACE0("ble_stream_init");

    if ((uint32_t) & ((BLE_STREAM)0)->common != 0)
    {
        DBG_DIRECT("ble_stream_create: common stream struct should be first member");
        __make_hardfault();
        return false;
    }

    ble_stream_mgr.stream_vector = vector_create(max_stream_num);

    if (ble_stream_mgr.stream_vector == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_init: stream_vector create failed");
        return false;
    }

    ble_stream_mgr.relay_handle = remote_relay_register((P_REMOTE_RELAY_CBACK)ble_stream_relay_cback);
    if (ble_stream_mgr.relay_handle == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_init: register relay failed");
        return false;
    }

    return true;
}

BLE_STREAM ble_stream_create(BLE_INIT_SETTINGS *p_settings)
{
    bool ret = false;
    BLE_STREAM ble_stream = NULL;

    APP_PRINT_TRACE4("ble_stream_create: service_id %d, conn_id %d, read_idx %d, write_idx %d",
                     p_settings->service_id, p_settings->conn_id, p_settings->read_attr_idx,
                     p_settings->write_attr_idx);

    ble_stream = ble_stream_search_by_id(p_settings->service_id, p_settings->conn_id,
                                         p_settings->read_attr_idx, p_settings->write_attr_idx);
    if (ble_stream != NULL)
    {
        return ble_stream;
    }

    ble_stream = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(BLE_STREAM_STRUCT));
    if (ble_stream == NULL)
    {
        APP_PRINT_ERROR0("ble_stream_create: stream alloc ble_stream_instance fail");
        return NULL;
    }

    ble_stream_default(ble_stream);

    ble_stream->common.read = (COMMON_STREAM_READ) ble_stream_read;
    ble_stream->common.write = (COMMON_STREAM_WRITE) ble_stream_write;
    ble_stream->common.get_addr = (COMMON_STREAM_GET_ADDR) ble_stream_get_addr;
    ble_stream->common.get_mtu = (COMMON_STREAM_GET_MTU) ble_stream_get_mtu;
    ble_stream->common.del = (COMMON_STREAM_DEL)ble_stream_delete;
    ble_stream->common.stream_cb = p_settings->stream_cb;

    ble_stream->read_attr_idx = p_settings->read_attr_idx;
    ble_stream->read_pdu_type = p_settings->read_pdu_type;
    ble_stream->write_attr_idx = p_settings->write_attr_idx;
    ble_stream->write_pdu_type = p_settings->write_pdu_type;

    ble_stream->conn_id = p_settings->conn_id;
    ble_stream->service_id = p_settings->service_id;

    ret = register_ble_stream(ble_stream);
    if (ret != true)
    {
        os_mem_free(ble_stream);
        ble_stream = NULL;
    }

    ble_stream_send_info_to_sec(ble_stream, STREAM_CREATED);

    return ble_stream;
}
