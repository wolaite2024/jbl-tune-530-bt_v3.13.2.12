/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#include "vector.h"
#include "app_cfg.h"
#include "iap_stream.h"
#include "trace.h"
#include "os_mem.h"
#include <string.h>
#include <bt_iap.h>
#include <stddef.h>
#include <bt_types.h>

typedef struct
{
    T_REMOTE_RELAY_HANDLE               relay_handle;
    VECTOR stream_vector;
} IAP_STEAM_MGR;


IAP_STEAM_MGR iap_stream_mgr = {.stream_vector = NULL, .relay_handle = NULL};


/**  @brief app ama remote msg*/
typedef enum
{
    STREAM_CREATED           = 0x00,
    STREAM_DELETED           = 0x01,
    STREAM_CONNECTED         = 0x02,
    STREAM_DISCONNECTED      = 0x03,
    STREAM_SYNC_ALL_INFO     = 0x04,
} REMOTE_MSG_TYPE;


typedef struct
{
    bool                    is_connected;
    uint8_t                 bd_addr[6];
    uint8_t                 remote_credits;
    uint32_t                mtu;
    uint8_t                 ea_protocol_id;
    uint16_t                eap_session_id;
} SINGLE_STREAM_REMOTE_INFO;


typedef struct
{
    bool                        disc_all;
    uint32_t                    stream_num;
    SINGLE_STREAM_REMOTE_INFO   stream_infos[1];
} IAP_STREAM_REMOTE_INFO;



static bool iap_stream_delete(IAP_STREAM iap_stream);



static bool iap_stream_register(IAP_STREAM iap_stream)
{
    bool ret = false;

    ret = vector_add(iap_stream_mgr.stream_vector, iap_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("register_iap_stream: register failed");
    }

    return ret;
}


static bool match_stream_by_session_id(IAP_STREAM iap_stream, IAP_STREAM template_stream)
{
    if (iap_stream->eap_session_id != template_stream->eap_session_id)
    {
        return false;
    }

    if (memcmp(iap_stream->common.bd_addr, template_stream->common.bd_addr,
               sizeof(iap_stream->common.bd_addr)) != 0)
    {
        return false;
    }

    return true;
}


static bool match_stream_by_protocol_id(IAP_STREAM iap_stream, IAP_STREAM template_stream)
{
    if (iap_stream->ea_protocol_id != template_stream->ea_protocol_id)
    {
        return false;
    }

    if (memcmp(iap_stream->common.bd_addr, template_stream->common.bd_addr,
               sizeof(iap_stream->common.bd_addr)) != 0)
    {
        return false;
    }

    return true;
}


static bool iap_stream_unregister(IAP_STREAM iap_stream)
{
    bool ret = false;

    ret = vector_remove(iap_stream_mgr.stream_vector, iap_stream);
    if (ret == false)
    {
        APP_PRINT_ERROR0("unregister_iap_stream: unregister failed");
    }

    return ret;
}


static IAP_STREAM iap_stream_find_by_ea_session_id(uint16_t eap_session_id,
                                                   uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    IAP_STREAM iap_stream = NULL;

    IAP_STREAM_STRUCT iap_stream_struct = {0};
    iap_stream_struct.eap_session_id = eap_session_id;
    memcpy(iap_stream_struct.common.bd_addr, bd_addr, sizeof(iap_stream_struct.common.bd_addr));

    iap_stream = vector_search(iap_stream_mgr.stream_vector, (V_PREDICATE)match_stream_by_session_id,
                               &iap_stream_struct);

    return iap_stream;
}


IAP_STREAM iap_stream_find_by_ea_protocol_id(uint8_t ea_protocol_id,
                                             uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH])
{
    IAP_STREAM iap_stream;
    IAP_STREAM_STRUCT iap_stream_struct = {0};

    iap_stream_struct.ea_protocol_id = ea_protocol_id;
    memcpy(iap_stream_struct.common.bd_addr, bd_addr, sizeof(iap_stream_struct.common.bd_addr));

    iap_stream = vector_search(iap_stream_mgr.stream_vector, (V_PREDICATE)match_stream_by_protocol_id,
                               &iap_stream_struct);

    return iap_stream;
}


static bool iap_stream_delete_not_exists_in_primary(IAP_STREAM iap_stream,
                                                    IAP_STREAM_REMOTE_INFO *p_info)
{
    uint32_t stream_num = p_info->stream_num;
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    uint32_t i = 0;

    for (i = 0; i < stream_num; i++)
    {
        p_single_info = &p_info->stream_infos[i];
        if (memcmp(iap_stream->common.bd_addr, p_single_info->bd_addr,
                   sizeof(iap_stream->common.bd_addr)) != 0)
        {
            continue;
        }

        if (iap_stream->ea_protocol_id != p_single_info->ea_protocol_id)
        {
            continue;
        }

        break;
    }

    if (i == stream_num)
    {
        iap_stream_delete(iap_stream);
    }

    return true;
}


static void sync_stream_in_primary(IAP_STREAM_REMOTE_INFO *p_info)
{
    IAP_STREAM iap_stream = NULL;
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;
    uint32_t stream_num = p_info->stream_num;
    T_IAP_STREAM_EVENT_PARAM iap_stream_param = {0};

    for (uint32_t i = 0; i < stream_num; i++)
    {
        p_single_info = &p_info->stream_infos[i];
        iap_stream = iap_stream_find_by_ea_protocol_id(p_single_info->ea_protocol_id,
                                                       p_single_info->bd_addr);
        if (iap_stream == NULL)
        {
            IAP_INIT_SETTINGS iap_init_settings = {0};
            iap_init_settings.bd_addr = p_single_info->bd_addr;
            iap_init_settings.ea_protocol_id = p_single_info->ea_protocol_id;
            iap_init_settings.stream_cb = NULL;
            iap_stream = iap_stream_create(&iap_init_settings);
        }

        if (iap_stream->common.is_connected != p_single_info->is_connected)
        {
            if (p_single_info->is_connected)
            {
                iap_stream_param.eap_session_id = p_single_info->eap_session_id;
                memcpy(iap_stream_param.bd_addr, p_single_info->bd_addr, sizeof(iap_stream_param.bd_addr));
                iap_stream_param.connect_param.ea_protocol_id = p_single_info->ea_protocol_id;
                iap_stream_param.connect_param.mtu = p_single_info->mtu;
                iap_stream_param.connect_param.remote_credits = p_single_info->remote_credits;
                iap_stream_event_process(IAP_STREAM_CONNECT_EVENT, &iap_stream_param);
            }
            else
            {
                memcpy(iap_stream_param.bd_addr, p_single_info->bd_addr, sizeof(iap_stream_param.bd_addr));
                iap_stream_param.eap_session_id = p_single_info->eap_session_id;
                iap_stream_param.disconnect_param.disc_all = false;
                iap_stream_event_process(IAP_STREAM_DISCONNECT_EVENT, &iap_stream_param);
            }
        }
    }
}


static void sync_stream_not_in_primary(IAP_STREAM_REMOTE_INFO *p_info)
{
    vector_mapping(iap_stream_mgr.stream_vector, (V_MAPPER) iap_stream_delete_not_exists_in_primary,
                   p_info);
}


static void iap_stream_sync_to_primary(REMOTE_MSG_TYPE msg_type, IAP_STREAM_REMOTE_INFO *p_info)
{
    T_IAP_STREAM_EVENT_PARAM iap_stream_param = {0};

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_ERROR0("iap_stream_sync_to_primary: this dev should be secondary");
        return;
    }

    switch (msg_type)
    {
    case STREAM_CONNECTED:
        iap_stream_param.eap_session_id = p_info->stream_infos[0].eap_session_id;
        memcpy(iap_stream_param.bd_addr, p_info->stream_infos[0].bd_addr, sizeof(iap_stream_param.bd_addr));
        iap_stream_param.connect_param.ea_protocol_id = p_info->stream_infos[0].ea_protocol_id;
        iap_stream_param.connect_param.mtu = p_info->stream_infos[0].mtu;
        iap_stream_param.connect_param.remote_credits = p_info->stream_infos[0].remote_credits;
        iap_stream_event_process(IAP_STREAM_CONNECT_EVENT, &iap_stream_param);
        break;

    case STREAM_DISCONNECTED:
        memcpy(iap_stream_param.bd_addr, p_info->stream_infos[0].bd_addr, sizeof(iap_stream_param.bd_addr));
        iap_stream_param.eap_session_id = p_info->stream_infos[0].eap_session_id;
        iap_stream_param.disconnect_param.disc_all = p_info->disc_all;
        iap_stream_event_process(IAP_STREAM_DISCONNECT_EVENT, &iap_stream_param);
        break;

    case STREAM_CREATED:
        {
            IAP_INIT_SETTINGS iap_init_settings = {0};
            iap_init_settings.bd_addr = p_info->stream_infos[0].bd_addr;
            iap_init_settings.ea_protocol_id = p_info->stream_infos[0].ea_protocol_id;
            iap_init_settings.stream_cb = NULL;
            iap_stream_create(&iap_init_settings);
        }
        break;

    case STREAM_DELETED:
        {
            IAP_STREAM iap_stream = NULL;
            iap_stream = iap_stream_find_by_ea_protocol_id(p_info->stream_infos[0].ea_protocol_id,
                                                           p_info->stream_infos[0].bd_addr);
            iap_stream_delete(iap_stream);
        }
        break;

    case STREAM_SYNC_ALL_INFO:
        {
            APP_PRINT_TRACE1("iap_stream_sync_to_primary: stream_num %d", p_info->stream_num);
            sync_stream_in_primary(p_info);
            sync_stream_not_in_primary(p_info);
        }
        break;

    default:
        break;
    }
}


static void iap_stream_relay_cback(REMOTE_MSG_TYPE msg_type, T_REMOTE_RELAY_STATUS status,
                                   IAP_STREAM_REMOTE_INFO *p_info, uint16_t len)
{
    uint32_t info_target_len = offsetof(IAP_STREAM_REMOTE_INFO,
                                        stream_infos) + p_info->stream_num * sizeof(p_info->stream_infos[0]);

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        APP_PRINT_ERROR1("iap_stream_relay_cback: invalid status %d", status);
        return;
    }

    if (len != info_target_len)
    {
        APP_PRINT_ERROR2("iap_stream_relay_cback: info_target_len %d, len %d", info_target_len, len);
        return;
    }

    APP_PRINT_TRACE1("iap_stream_relay_cback: msg_type %d", msg_type);

    iap_stream_sync_to_primary(msg_type, p_info);
}


static void iap_stream_send_info_to_sec(IAP_STREAM iap_stream, REMOTE_MSG_TYPE msg_type,
                                        bool disc_all)
{
    IAP_STREAM_REMOTE_INFO remote_info = {0};

    if (iap_stream == NULL)
    {
        APP_PRINT_ERROR0("iap_stream_send_info_to_sec: iap_stream is NULL");
        return;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("iap_stream_send_info_to_sec: this dev should be primary");
        return;
    }

    APP_PRINT_TRACE3("iap_stream_send_info_to_sec: msg_type %d, ea_protocol_id %d , eap_session_id %d",
                     msg_type, iap_stream->ea_protocol_id, iap_stream->eap_session_id);

    remote_info.stream_num = 1;
    memcpy(remote_info.stream_infos[0].bd_addr, iap_stream->common.bd_addr,
           sizeof(remote_info.stream_infos[0].bd_addr));
    remote_info.stream_infos[0].ea_protocol_id = iap_stream->ea_protocol_id;
    remote_info.stream_infos[0].eap_session_id = iap_stream->eap_session_id;
    remote_info.stream_infos[0].mtu = iap_stream->common.mtu;
    remote_info.stream_infos[0].remote_credits = iap_stream->common.remote_credits;
    remote_info.disc_all = disc_all;

    remote_async_msg_relay(iap_stream_mgr.relay_handle, msg_type,
                           (uint8_t *)&remote_info, sizeof(remote_info), false);
}



static bool prepare_single_stream_remote_info(IAP_STREAM iap_stream, IAP_STREAM_REMOTE_INFO *p_info)
{
    SINGLE_STREAM_REMOTE_INFO *p_single_info = NULL;

    p_single_info = &p_info->stream_infos[p_info->stream_num];
    memcpy(p_single_info->bd_addr, iap_stream->common.bd_addr, sizeof(p_single_info->bd_addr));
    p_single_info->eap_session_id = iap_stream->eap_session_id;
    p_single_info->ea_protocol_id = iap_stream->ea_protocol_id;
    p_single_info->is_connected = iap_stream->common.is_connected;
    p_single_info->mtu = iap_stream->common.mtu;
    p_single_info->remote_credits = iap_stream->common.remote_credits;
    p_info->stream_num++;

    return true;
}



void iap_stream_send_all_info_to_sec(void)
{
    uint32_t stream_num = 0;
    IAP_STREAM_REMOTE_INFO *p_info = NULL;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("iap_stream_send_all_info_to_sec: this dev should be primary");
        return;
    }

    stream_num = vector_depth(iap_stream_mgr.stream_vector);
    p_info = os_mem_alloc(RAM_TYPE_BUFFER_OFF, offsetof(IAP_STREAM_REMOTE_INFO, stream_infos)
                          + stream_num * sizeof(p_info->stream_infos[0]));
    if (p_info != NULL)
    {
        p_info->stream_num = 0;
        memset(p_info, 0, stream_num * sizeof(p_info->stream_infos[0]));
        vector_mapping(iap_stream_mgr.stream_vector, (V_MAPPER) prepare_single_stream_remote_info, p_info);
        if (p_info->stream_num != stream_num)
        {
            APP_PRINT_ERROR0("iap_stream_send_all_info_to_sec: device num wrong");
        }

        remote_async_msg_relay(iap_stream_mgr.relay_handle, STREAM_SYNC_ALL_INFO, (uint8_t *)p_info,
                               offsetof(IAP_STREAM_REMOTE_INFO,
                                        stream_infos) + p_info->stream_num * sizeof(p_info->stream_infos[0]), false);

        os_mem_free(p_info);
    }
}



static void iap_stream_connect_process(T_IAP_STREAM_EVENT_PARAM *event_param)
{
    IAP_STREAM iap_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    APP_PRINT_TRACE1("iap_stream_connect_process: ea_protocol_id %d",
                     event_param->connect_param.ea_protocol_id);

    uint8_t *bd_addr       = event_param->bd_addr;
    uint8_t remote_credits = event_param->connect_param.remote_credits;
    uint16_t mtu           = event_param->connect_param.mtu;
    uint8_t ea_protocol_id = event_param->connect_param.ea_protocol_id;
    uint8_t eap_session_id = event_param->eap_session_id;

    iap_stream = iap_stream_find_by_ea_protocol_id(ea_protocol_id, bd_addr);

    if (iap_stream == NULL)
    {
        APP_PRINT_ERROR1("iap_stream_connect_process: iap_stream is NULL ea_protocol_id %d",
                         event_param->connect_param.ea_protocol_id);
        return;
    }

    iap_stream->eap_session_id = eap_session_id;

    APP_PRINT_TRACE3("iap_stream_connect_process: max data size %d, is_connected %d, stream_cb %p",
                     mtu,
                     iap_stream->common.is_connected, iap_stream->common.stream_cb);

    if (__common_stream_connect(&iap_stream->common, mtu, remote_credits) == false)
    {
        return;
    }

    iap_stream_send_info_to_sec(iap_stream, STREAM_CONNECTED, false);
}

static void iap_stream_disconnect(IAP_STREAM iap_stream)
{
    if (iap_stream == NULL)
    {
        return;
    }

    __common_stream_disconnect(&iap_stream->common);
}


static bool disconnect_for_mapping(IAP_STREAM iap_stream, void *p)
{
    if (iap_stream == NULL)
    {
        return false;
    }

    iap_stream_disconnect(iap_stream);

    return true;
}



static void disconnect_streams(void)
{
    vector_mapping(iap_stream_mgr.stream_vector, (V_MAPPER)disconnect_for_mapping, NULL);
}


static void iap_stream_disconnect_process(T_IAP_STREAM_EVENT_PARAM *event_param)
{
    IAP_STREAM iap_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    uint8_t *bd_addr        = event_param->bd_addr;
    uint8_t eap_session_id  = event_param->eap_session_id;

    if (event_param->disconnect_param.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
    {
        APP_PRINT_WARN0("iap_stream_disconnect_process: disconnect by roleswap, should not inform app layer");
        return;
    }

    if (event_param->disconnect_param.disc_all)
    {
        disconnect_streams();
    }
    else
    {
        iap_stream = iap_stream_find_by_ea_session_id(eap_session_id, bd_addr);
        iap_stream_disconnect(iap_stream);
    }

    iap_stream_send_info_to_sec(iap_stream, STREAM_DISCONNECTED,
                                event_param->disconnect_param.disc_all);
}


static void iap_stream_receive_data_process(T_IAP_STREAM_EVENT_PARAM *event_param)
{
    IAP_STREAM iap_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    uint8_t  *data          = event_param->receive_data_param.data;
    //uint8_t  credits      = event_param->receive_data_param.credits;
    uint16_t data_length    = event_param->receive_data_param.length;
    uint8_t  *bd_addr       = event_param->bd_addr;
    uint8_t eap_session_id  = event_param->eap_session_id;

    iap_stream = iap_stream_find_by_ea_session_id(eap_session_id, bd_addr);

    if (iap_stream == NULL)
    {
        APP_PRINT_ERROR1("iap_stream_receive_data_process: iap_stream is NULL", eap_session_id);
        return;
    }

    APP_PRINT_TRACE2("iap_stream_receive_data_process: ea_protocol_id %d, length %d",
                     iap_stream->ea_protocol_id, data_length);

    if (__common_stream_receive_data(&iap_stream->common, data, data_length) == false)
    {
        return;
    }
}


static void iap_stream_update_credit_process(T_IAP_STREAM_EVENT_PARAM *event_param)
{
    IAP_STREAM iap_stream = NULL;
    if (event_param == NULL)
    {
        return;
    }

    uint8_t *bd_addr        = event_param->bd_addr;
    uint8_t remote_credits  = event_param->update_credit_param.remote_credits;
    uint8_t eap_session_id  = event_param->eap_session_id;

    iap_stream = iap_stream_find_by_ea_session_id(eap_session_id, bd_addr);

    if (iap_stream == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("iap_stream_update_credit_process: ea_protocol_id %d, remote_credits %d",
                     iap_stream->ea_protocol_id, remote_credits);

    if (__common_stream_update_credits(&iap_stream->common, remote_credits) == false)
    {
        return;
    }
}


void iap_stream_event_process(T_IAP_STREAM_EVENT event,
                              T_IAP_STREAM_EVENT_PARAM *event_param)
{
    if (event_param == NULL)
    {
        return;
    }

    APP_PRINT_TRACE2("iap_stream_event_process: event %d, eap_session_id %d", event,
                     event_param->eap_session_id);

    switch (event)
    {
    case IAP_STREAM_CONNECT_EVENT:
        {
            iap_stream_connect_process(event_param);
        }
        break;

    case IAP_STREAM_DISCONNECT_EVENT:
        {
            iap_stream_disconnect_process(event_param);
        }
        break;

    case IAP_STREAM_RECEIVE_DATA_EVENT:
        {
            iap_stream_receive_data_process(event_param);
        }
        break;

    case IAP_STREAM_UPDATE_CREDIT_EVENT:
        {
            iap_stream_update_credit_process(event_param);
        }
        break;

    default:
        break;
    }
}

static bool iap_stream_check_write_condition(IAP_STREAM iap_stream, uint32_t length)
{
    if (true != iap_stream->common.is_connected)
    {
        APP_PRINT_TRACE0("iap_stream_check_write_condition fail, iap stream not connected");
        return false;
    }

    if (length > iap_stream->common.mtu)
    {
        APP_PRINT_TRACE0("iap_stream_check_write_condition fail, data length is longer than mtu");
        return false;
    }

#if 0
    if (stream->remote_credits == 0)
    {
        APP_PRINT_TRACE0("iap_stream_check_write_condition fail, have no credits");
        stream->write_block = true;
        return false;
    }
#endif
    return true;
}


static bool iap_stream_write(IAP_STREAM iap_stream, uint8_t *data, uint32_t len)
{

    if (iap_stream_check_write_condition(iap_stream, len) == true)
    {
        //ranhui
        if (bt_iap_data_send(iap_stream->common.bd_addr, iap_stream->eap_session_id,
                             data, len) == true)
        {
            APP_PRINT_TRACE0("iap_stream_write success");
            iap_stream->common.remote_credits--;
            return true;
        }
        else
        {
            APP_PRINT_TRACE0("iap_stream_write fail");
            iap_stream->common.write_block = true;
            return false;
        }
    }

    return false;
}

static bool iap_stream_read(IAP_STREAM iap_stream, uint8_t **p_buf, uint32_t *p_len)
{
    return __common_stream_read(&iap_stream->common, p_buf, p_len);
}


static uint32_t iap_stream_get_mtu(IAP_STREAM iap_stream)
{
    return __common_stream_get_mtu(&iap_stream->common);
}


static void iap_stream_get_remote_addr(IAP_STREAM iap_stream, uint8_t *addr)
{
    __common_stream_get_bd_addr(&iap_stream->common, addr);
}


static bool iap_stream_delete(IAP_STREAM iap_stream)
{
    if (iap_stream == NULL)
    {
        return false;
    }

    __common_stream_delete(&iap_stream->common);

    iap_stream_unregister(iap_stream);

    iap_stream_send_info_to_sec(iap_stream, STREAM_DELETED, false);

    os_mem_free(iap_stream);

    return true;
}


static void iap_stream_default(IAP_STREAM iap_stream)
{
    __common_stream_default(&iap_stream->common);
    iap_stream->common.bt_type = COMMON_STREAM_BT_BREDR;

    iap_stream->ea_protocol_id = 0;
    iap_stream->eap_session_id = 0;
}


bool iap_stream_init(uint32_t max_stream_num)
{
    APP_PRINT_TRACE0("iap_stream_init");

    if ((uint32_t) & ((IAP_STREAM)0)->common != 0)
    {
        DBG_DIRECT("iap_stream_create: common stream struct should be first member");
        __make_hardfault();
        return false;
    }

    iap_stream_mgr.stream_vector = vector_create(max_stream_num);

    if (iap_stream_mgr.stream_vector == NULL)
    {
        APP_PRINT_ERROR0("iap_stream_init: iap_vector create failed");
        return false;
    }

    iap_stream_mgr.relay_handle = remote_relay_register((P_REMOTE_RELAY_CBACK)iap_stream_relay_cback);
    if (iap_stream_mgr.relay_handle == NULL)
    {
        APP_PRINT_ERROR0("iap_stream_init: register relay failed");
        return false;
    }

    return true;
}


IAP_STREAM iap_stream_create(IAP_INIT_SETTINGS *p_settings)
{
    bool ret = false;
    uint8_t *bd_addr = p_settings->bd_addr;
    IAP_STREAM iap_stream = NULL;

    APP_PRINT_TRACE2("iap_stream_create: bd_addr %s, ea_protocol_id %d",
                     TRACE_BDADDR(bd_addr), p_settings->ea_protocol_id);

    iap_stream = iap_stream_find_by_ea_protocol_id(p_settings->ea_protocol_id, bd_addr);
    if (iap_stream != NULL)
    {
        APP_PRINT_TRACE0("iap_stream_create: iap_stream exists");
        if (iap_stream->common.stream_cb == NULL)
        {
            iap_stream->common.stream_cb = p_settings->stream_cb;
            APP_PRINT_TRACE0("iap_stream_create: update stream_cb");
        }
        return iap_stream;
    }

    iap_stream = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(IAP_STREAM_STRUCT));
    if (iap_stream == NULL)
    {
        APP_PRINT_ERROR0("iap_stream_create: iap_stream failed");
        return NULL;
    }

    iap_stream_default(iap_stream);


    iap_stream->common.read = (COMMON_STREAM_READ)iap_stream_read;
    iap_stream->common.write = (COMMON_STREAM_WRITE) iap_stream_write;
    iap_stream->common.get_addr = (COMMON_STREAM_GET_ADDR) iap_stream_get_remote_addr;
    iap_stream->common.get_mtu = (COMMON_STREAM_GET_MTU) iap_stream_get_mtu;
    iap_stream->common.del = (COMMON_STREAM_DEL)iap_stream_delete;
    iap_stream->common.stream_cb = p_settings->stream_cb;

    iap_stream->ea_protocol_id = p_settings->ea_protocol_id;

    memcpy(iap_stream->common.bd_addr, p_settings->bd_addr, sizeof(iap_stream->common.bd_addr));

    ret = iap_stream_register(iap_stream);
    if (ret != true)
    {
        os_mem_free(iap_stream);
        iap_stream = NULL;
    }

    iap_stream_send_info_to_sec(iap_stream, STREAM_CREATED, false);

    return iap_stream;
}
