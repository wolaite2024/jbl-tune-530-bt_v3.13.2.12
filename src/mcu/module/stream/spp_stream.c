/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#include <string.h>
#include "os_mem.h"
#include "trace.h"
#include "bt_types.h"
#include "bt_spp.h"
#include "vector.h"
#include "app_cfg.h"
#include "spp_stream.h"


typedef struct
{
    T_REMOTE_RELAY_HANDLE               relay_handle;
    VECTOR stream_vector;
} SPP_STEAM_MGR;



SPP_STEAM_MGR spp_stream_mgr = {.stream_vector = NULL, .relay_handle = NULL};


typedef struct
{
    uint8_t                 bd_addr[6];
    uint8_t                 remote_credits;
    uint32_t                mtu;
    uint8_t                 server_chann;
    bool                    is_connected;
} SINGLE_STREAM_REMOTE_INFO;



typedef struct
{
    uint32_t                    stream_num;
    SINGLE_STREAM_REMOTE_INFO   stream_infos[1];
} SPP_STREAM_REMOTE_INFO;



/**  @brief app ama remote msg*/
typedef enum
{
    STREAM_CREATED           = 0x00,
    STREAM_DELETED           = 0x01,
    STREAM_CONNECTED         = 0x02,
    STREAM_DISCONNECTED      = 0x03,
    STREAM_SYNC_ALL_INFO     = 0x04,
} REMOTE_MSG_TYPE;


static bool spp_stream_delete(SPP_STREAM spp_stream);



static bool register_spp_stream(SPP_STREAM spp_stream)
{
    bool ret = false;

    ret = vector_add(spp_stream_mgr.stream_vector, spp_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("register_spp_stream: register failed");
    }

    return ret;
}


static bool unregister_spp_stream(SPP_STREAM spp_stream)
{
    bool ret = false;

    ret = vector_remove(spp_stream_mgr.stream_vector, spp_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("unregister_spp_stream: unregister failed");
    }

    return ret;
}


static bool match_stream_by_server_chann(SPP_STREAM spp_stream, SPP_STREAM template_stream)
{
    if (spp_stream->server_chann != template_stream->server_chann)
    {
        return false;
    }

    if (memcmp(spp_stream->common.bd_addr, template_stream->common.bd_addr,
               sizeof(spp_stream->common.bd_addr)) != 0)
    {
        return false;
    }

    return true;
}


SPP_STREAM spp_stream_find_by_server_chann(uint32_t server_chann,
                                           uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    SPP_STREAM_STRUCT template_stream_struct = {0};
    SPP_STREAM stream = NULL;

    template_stream_struct.server_chann = server_chann;
    memcpy(template_stream_struct.common.bd_addr, bd_addr,
           sizeof(template_stream_struct.common.bd_addr));

    stream = vector_search(spp_stream_mgr.stream_vector,
                           (V_PREDICATE)match_stream_by_server_chann, &template_stream_struct);

    return stream;
}


static bool spp_stream_delete_not_exists_in_primary(SPP_STREAM spp_stream,
                                                    SPP_STREAM_REMOTE_INFO *p_info)
{
    uint32_t stream_num = p_info->stream_num;
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    uint32_t i = 0;

    for (i = 0; i < stream_num; i++)
    {
        p_single_info = &p_info->stream_infos[i];
        if (memcmp(spp_stream->common.bd_addr, p_single_info->bd_addr,
                   sizeof(spp_stream->common.bd_addr)) != 0)
        {
            continue;
        }

        if (spp_stream->server_chann != p_single_info->server_chann)
        {
            continue;
        }

        break;
    }

    if (i == stream_num)
    {
        APP_PRINT_TRACE0("spp_stream_delete_not_exists_in_primary: stream not in primary");
        spp_stream_delete(spp_stream);
    }

    return true;
}


static void sync_stream_in_primary(SPP_STREAM_REMOTE_INFO *p_info)
{
    SPP_STREAM spp_stream = NULL;
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    uint32_t stream_num = p_info->stream_num;
    T_SPP_STREAM_EVENT_PARAM spp_stream_param = {0};

    for (uint32_t i = 0; i < stream_num; i++)
    {
        p_single_info = &p_info->stream_infos[i];

        APP_PRINT_TRACE2("spp_stream_sync_to_primary: bd_addr %s, server_chann %d",
                         TRACE_BDADDR(p_single_info->bd_addr),
                         p_single_info->server_chann);

        spp_stream = spp_stream_find_by_server_chann(p_single_info->server_chann, p_single_info->bd_addr);
        if (spp_stream == NULL)
        {
            SPP_INIT_SETTINGS spp_init_settings = {0};
            spp_init_settings.bd_addr = p_single_info->bd_addr;
            spp_init_settings.server_chann = p_single_info->server_chann;
            spp_init_settings.stream_cb = NULL;
            spp_stream = spp_stream_create(&spp_init_settings);
        }

        if (spp_stream->common.is_connected != p_single_info->is_connected)
        {
            if (p_single_info->is_connected)
            {
                spp_stream_param.server_chann = p_single_info->server_chann;
                spp_stream_param.connect_param.mtu            = p_single_info->mtu;
                spp_stream_param.connect_param.remote_credits = p_single_info->remote_credits;
                memcpy(spp_stream_param.connect_param.bd_addr, p_single_info->bd_addr, 6);
                spp_stream_event_process(SPP_STREAM_CONNECT_EVENT, &spp_stream_param);
            }
            else
            {
                spp_stream_param.server_chann = p_single_info->server_chann;
                memcpy(spp_stream_param.disconnect_param.bd_addr, p_single_info->bd_addr, 6);
                spp_stream_event_process(SPP_STREAM_DISCONNECT_EVENT, &spp_stream_param);
            }
        }
    }
}


static void sync_stream_not_in_primary(SPP_STREAM_REMOTE_INFO *p_info)
{
    vector_mapping(spp_stream_mgr.stream_vector, (V_MAPPER) spp_stream_delete_not_exists_in_primary,
                   p_info);
}


static void spp_stream_sync_to_primary(REMOTE_MSG_TYPE msg_type, SPP_STREAM_REMOTE_INFO *p_info)
{
    T_SPP_STREAM_EVENT_PARAM spp_stream_param = {0};
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    uint32_t stream_num = p_info->stream_num;


    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("spp_stream_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case STREAM_CONNECTED:
        p_single_info = &p_info->stream_infos[0];
        spp_stream_param.server_chann = p_single_info->server_chann;
        spp_stream_param.connect_param.mtu            = p_single_info->mtu;
        spp_stream_param.connect_param.remote_credits = p_single_info->remote_credits;
        memcpy(spp_stream_param.connect_param.bd_addr, p_single_info->bd_addr, 6);
        spp_stream_event_process(SPP_STREAM_CONNECT_EVENT, &spp_stream_param);
        break;

    case STREAM_DISCONNECTED:
        p_single_info = &p_info->stream_infos[0];
        spp_stream_param.server_chann = p_single_info->server_chann;
        memcpy(spp_stream_param.disconnect_param.bd_addr, p_single_info->bd_addr, 6);
        spp_stream_event_process(SPP_STREAM_DISCONNECT_EVENT, &spp_stream_param);
        break;

    case STREAM_CREATED:
        {
            p_single_info = &p_info->stream_infos[0];
            SPP_INIT_SETTINGS spp_init_settings = {0};
            spp_init_settings.bd_addr = p_single_info->bd_addr;
            spp_init_settings.server_chann = p_single_info->server_chann;
            spp_init_settings.stream_cb = NULL;
            spp_stream_create(&spp_init_settings);
        }
        break;

    case STREAM_DELETED:
        {
            p_single_info = &p_info->stream_infos[0];
            SPP_STREAM spp_stream = NULL;
            spp_stream = spp_stream_find_by_server_chann(p_single_info->server_chann, p_single_info->bd_addr);
            spp_stream_delete(spp_stream);
        }
        break;

    case STREAM_SYNC_ALL_INFO:
        {
            APP_PRINT_TRACE1("spp_stream_sync_to_primary: stream_num %d", stream_num);
            sync_stream_in_primary(p_info);
            sync_stream_not_in_primary(p_info);
        }
        break;

    default:
        break;
    }
}


static void spp_stream_relay_cback(REMOTE_MSG_TYPE msg_type, T_REMOTE_RELAY_STATUS status,
                                   SPP_STREAM_REMOTE_INFO *p_info, uint16_t len)
{
    uint32_t info_target_len = offsetof(SPP_STREAM_REMOTE_INFO,
                                        stream_infos) + p_info->stream_num * sizeof(p_info->stream_infos);

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("spp_stream_relay_cback: invalid status %d", status);
        return;
    }

    if (len != info_target_len)
    {
        APP_PRINT_ERROR2("spp_stream_relay_cback: info_target_len %d, len %d", info_target_len, len);
        return;
    }

    APP_PRINT_TRACE1("spp_stream_relay_cback: msg_type %d", msg_type);

    spp_stream_sync_to_primary(msg_type, p_info);
}


static void spp_stream_send_info_to_sec(SPP_STREAM spp_stream, REMOTE_MSG_TYPE msg_type)
{
    SPP_STREAM_REMOTE_INFO remote_info = {0};

    if (spp_stream == NULL)
    {
        APP_PRINT_ERROR0("spp_stream_send_info_to_sec: spp_stream is NULL");
        return;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("spp_stream_send_info_to_sec: this dev should be primary");
        return;
    }

    APP_PRINT_TRACE2("spp_stream_send_info_to_sec: msg_type %d, server chann %d", msg_type,
                     spp_stream->server_chann);

    remote_info.stream_num = 1;
    memcpy(remote_info.stream_infos[0].bd_addr, spp_stream->common.bd_addr, 6);
    remote_info.stream_infos[0].remote_credits = spp_stream->common.remote_credits;
    remote_info.stream_infos[0].mtu = spp_stream->common.mtu;
    remote_info.stream_infos[0].server_chann = spp_stream->server_chann;

    remote_async_msg_relay(spp_stream_mgr.relay_handle, msg_type,
                           (uint8_t *)&remote_info, sizeof(remote_info), false);
}


static bool prepare_single_stream_remote_info(SPP_STREAM spp_stream, SPP_STREAM_REMOTE_INFO *p_info)
{
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    p_single_info = &p_info->stream_infos[p_info->stream_num];
    memcpy(p_single_info->bd_addr, spp_stream->common.bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->mtu = spp_stream->common.mtu;
    p_single_info->remote_credits = spp_stream->common.remote_credits;
    p_single_info->server_chann = spp_stream->server_chann;
    p_single_info->is_connected = spp_stream->common.is_connected;
    p_info->stream_num++;

    APP_PRINT_TRACE4("prepare_single_stream_remote_info: stream_num %d, bd_addr %s, server_chann %d, is_connected %d",
                     p_info->stream_num, TRACE_BDADDR(p_single_info->bd_addr), p_single_info->server_chann,
                     p_single_info->is_connected);

    return true;
}


void spp_stream_send_all_info_to_sec(void)
{
    uint32_t stream_num = 0;
    SPP_STREAM_REMOTE_INFO *p_info = NULL;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("spp_stream_send_all_info_to_sec: this dev should be primary");
        return;
    }

    stream_num = vector_depth(spp_stream_mgr.stream_vector);

    APP_PRINT_TRACE3("spp_stream_send_all_info_to_sec: stream_num %d, size %d, default size %d",
                     stream_num, offsetof(SPP_STREAM_REMOTE_INFO,
                                          stream_infos) + stream_num * sizeof(p_info->stream_infos[0]),
                     sizeof(*p_info));

    p_info = os_mem_alloc(RAM_TYPE_BUFFER_OFF, offsetof(SPP_STREAM_REMOTE_INFO, stream_infos)
                          + stream_num * sizeof(p_info->stream_infos[0]));
    if (p_info != NULL)
    {
        p_info->stream_num = 0;
        memset(p_info, 0, stream_num * sizeof(p_info->stream_infos[0]));
        vector_mapping(spp_stream_mgr.stream_vector, (V_MAPPER) prepare_single_stream_remote_info, p_info);

        if (p_info->stream_num != stream_num)
        {
            APP_PRINT_ERROR0("spp_stream_send_all_info_to_sec: device num wrong");
        }

        remote_async_msg_relay(spp_stream_mgr.relay_handle, STREAM_SYNC_ALL_INFO, (uint8_t *)p_info,
                               offsetof(SPP_STREAM_REMOTE_INFO,
                                        stream_infos) + p_info->stream_num * sizeof(p_info->stream_infos[0]), false);
        os_mem_free(p_info);
    }

}


static void spp_stream_connect_process(T_SPP_STREAM_EVENT_PARAM *event_param)
{
    SPP_STREAM spp_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    uint8_t *bd_addr       = event_param->connect_param.bd_addr;
    uint8_t remote_credits = event_param->connect_param.remote_credits;
    uint16_t mtu           = event_param->connect_param.mtu;
    uint8_t server_chann   = event_param->server_chann;

    APP_PRINT_TRACE1("spp_stream_connect_process: bd_addr %s", TRACE_BDADDR(bd_addr));

    spp_stream = spp_stream_find_by_server_chann(server_chann, bd_addr);

    if (spp_stream == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("spp_stream_connect_process: mtu %d, remote_credits %d", mtu,
                     remote_credits);

    if (__common_stream_connect(&spp_stream->common, mtu, remote_credits) == false)
    {
        return;
    }

    spp_stream_send_info_to_sec(spp_stream, STREAM_CONNECTED);
}


static void spp_stream_disconnect_process(T_SPP_STREAM_EVENT_PARAM *event_param)
{
    SPP_STREAM spp_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("spp_stream_disconnect_process: server_chann %d, cause 0x%04x",
                     event_param->server_chann, event_param->disconnect_param.cause);
    spp_stream = spp_stream_find_by_server_chann(event_param->server_chann,
                                                 event_param->disconnect_param.bd_addr);
    if (spp_stream == NULL)
    {
        return;
    }

    if (event_param->disconnect_param.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
    {
        APP_PRINT_WARN0("spp_stream_disconnect_process: disconnect by roleswap, should not inform app layer");
        return;
    }
    if (__common_stream_disconnect(&spp_stream->common) == false)
    {
        return;
    }

    spp_stream_send_info_to_sec(spp_stream, STREAM_DISCONNECTED);
}


static void spp_stream_receive_data_process(T_SPP_STREAM_EVENT_PARAM *event_param)
{
    SPP_STREAM spp_stream = NULL;


    if (event_param == NULL)
    {
        return;
    }
    uint8_t  *data       = event_param->receive_data_param.data;
//    uint8_t  credits     = event_param->receive_data_param.credits;
    uint16_t data_length = event_param->receive_data_param.length;
    uint8_t  *bd_addr    = event_param->receive_data_param.bd_addr;

    spp_stream = spp_stream_find_by_server_chann(event_param->server_chann, bd_addr);
    if (spp_stream == NULL)
    {
        return;
    }

    if (__common_stream_receive_data(&spp_stream->common, data, data_length) == false)
    {
        return;
    }
}


static void spp_stream_update_credit_process(T_SPP_STREAM_EVENT_PARAM *event_param)
{
    SPP_STREAM spp_stream = NULL;

    if (event_param == NULL)
    {
        return;
    }

    uint8_t *bd_addr = event_param ->update_credit_param.bd_addr;
    uint8_t remote_credits = event_param->update_credit_param.remote_credits;

    spp_stream = spp_stream_find_by_server_chann(event_param->server_chann, bd_addr);

    APP_PRINT_TRACE1("spp_stream_update_credit_process: remote_credits %d", remote_credits);

    if (spp_stream == NULL)
    {
        return;
    }

    if (__common_stream_update_credits(&spp_stream->common, remote_credits) == false)
    {
        return;
    }
}


void spp_stream_event_process(T_SPP_STREAM_EVENT event,
                              T_SPP_STREAM_EVENT_PARAM *event_param)
{
    if (event_param == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("spp_stream_event_process event %d, server channel %d", event,
                     event_param->server_chann);

    switch (event)
    {
    case SPP_STREAM_CONNECT_EVENT:
        {
            spp_stream_connect_process(event_param);
        }
        break;

    case SPP_STREAM_DISCONNECT_EVENT:
        {
            spp_stream_disconnect_process(event_param);
        }
        break;

    case SPP_STREAM_RECEIVE_DATA_EVENT:
        {
            spp_stream_receive_data_process(event_param);
        }
        break;

    case SPP_STREAM_UPDATE_CREDIT_EVENT:
        {
            spp_stream_update_credit_process(event_param);
        }
        break;

    default:
        break;
    }
}


static bool spp_stream_check_write_condition(SPP_STREAM spp_stream, uint32_t length)
{
    return __common_stream_check_write_condition(&spp_stream->common, length);
}


static bool spp_stream_write(SPP_STREAM spp_stream, uint8_t *data, uint32_t len)
{
    if (spp_stream_check_write_condition(spp_stream, len) == false)
    {
        return false;
    }

    //ranhui
    if (bt_spp_data_send(spp_stream->common.bd_addr, spp_stream->server_chann,
                         data, len, false) == true)
    {
        APP_PRINT_TRACE0("spp_stream_write success");
        spp_stream->common.remote_credits--;
        APP_PRINT_TRACE1("spp_stream_write: remain credit %d", spp_stream->common.remote_credits);
        return true;
    }
    else
    {
        APP_PRINT_TRACE0("spp_stream_write fail");
        spp_stream->common.write_block = true;
        return false;
    }
}


static bool spp_stream_read(SPP_STREAM spp_stream, uint8_t **p_buf, uint32_t *p_len)
{
    return __common_stream_read(&spp_stream->common, p_buf, p_len);
}


static uint32_t spp_stream_get_mtu(SPP_STREAM spp_stream)
{
    return __common_stream_get_mtu(&spp_stream->common);
}


static void spp_stream_get_remote_addr(SPP_STREAM spp_stream, uint8_t *addr)
{
    __common_stream_get_bd_addr(&spp_stream->common, addr);
}


static bool spp_stream_delete(SPP_STREAM spp_stream)
{
    if (spp_stream == NULL)
    {
        return false;
    }

    __common_stream_delete(&spp_stream->common);

    unregister_spp_stream(spp_stream);

    spp_stream_send_info_to_sec(spp_stream, STREAM_DELETED);

    os_mem_free(spp_stream);

    return true;
}


static void spp_stream_default(SPP_STREAM spp_stream)
{
    __common_stream_default(&spp_stream->common);
    spp_stream->common.bt_type = COMMON_STREAM_BT_BREDR;
    spp_stream->server_chann = 0;
}


bool spp_stream_init(uint32_t max_stream_num)
{
    APP_PRINT_TRACE0("spp_stream_init");

    if ((uint32_t) & ((SPP_STREAM)0)->common != 0)
    {
        DBG_DIRECT("spp_stream_init: common stream struct should be first member");
        __make_hardfault();
        return false;
    }

    spp_stream_mgr.stream_vector = vector_create(max_stream_num);

    if (spp_stream_mgr.stream_vector == NULL)
    {
        APP_PRINT_ERROR0("spp_stream_init: spp_vector create failed");
        return false;
    }

    spp_stream_mgr.relay_handle = remote_relay_register((P_REMOTE_RELAY_CBACK)spp_stream_relay_cback);
    if (spp_stream_mgr.relay_handle == NULL)
    {
        APP_PRINT_ERROR0("spp_stream_init: register relay failed");
        return false;
    }


    return true;
}


SPP_STREAM spp_stream_create(SPP_INIT_SETTINGS *p_settings)
{
    bool ret = false;

    SPP_STREAM spp_stream = NULL;

    spp_stream = spp_stream_find_by_server_chann(p_settings->server_chann, p_settings->bd_addr);

    if (spp_stream != NULL)
    {
        APP_PRINT_WARN0("spp_stream_create: spp_stream exists");
        if (spp_stream->common.stream_cb == NULL)
        {
            spp_stream->common.stream_cb = p_settings->stream_cb;
            APP_PRINT_TRACE0("spp_stream_create: update stream_cb");
        }
        return spp_stream;
    }

    spp_stream = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(SPP_STREAM_STRUCT));
    if (spp_stream == NULL)
    {
        APP_PRINT_ERROR0("spp_stream_create: spp_stream malloc failed");
        return NULL;
    }

    spp_stream_default(spp_stream);

    spp_stream->common.read = (COMMON_STREAM_READ) spp_stream_read;
    spp_stream->common.write = (COMMON_STREAM_WRITE)spp_stream_write;
    spp_stream->common.get_addr = (COMMON_STREAM_GET_ADDR)spp_stream_get_remote_addr;
    spp_stream->common.get_mtu = (COMMON_STREAM_GET_MTU)spp_stream_get_mtu;
    spp_stream->common.del = (COMMON_STREAM_DEL)spp_stream_delete;
    spp_stream->common.stream_cb = p_settings->stream_cb;

    spp_stream->server_chann = p_settings->server_chann;
    memcpy(spp_stream->common.bd_addr, p_settings->bd_addr, sizeof(spp_stream->common.bd_addr));

    uint8_t *bd_addr = spp_stream->common.bd_addr;

    APP_PRINT_TRACE2("spp_stream_create: server_chann %d, bd_addr %s",
                     spp_stream->server_chann, TRACE_BDADDR(bd_addr));

    ret = register_spp_stream(spp_stream);
    if (ret != true)
    {
        os_mem_free(spp_stream);
        spp_stream = NULL;
    }

    spp_stream_send_info_to_sec(spp_stream, STREAM_CREATED);
    return spp_stream;
}
