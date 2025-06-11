#if F_APP_IAP_RTK_SUPPORT

/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "os_mem.h"
#include "os_queue.h"
#include "common_stream.h"
#include "iap_stream.h"
#include "app_cfg.h"
#include "app_cmd.h"
#include "app_main.h"
#include "app_iap_rtk.h"
#include "app_iap.h"
#include "app_flags.h"

typedef enum t_relay_msg_type
{
    RTK_IAP_CREATE = 0,
    RTK_IAP_DELETE = 1,
    RTK_IAP_CONNECTED = 2,
    RTK_IAP_DISCONNECTED = 3,
    SYNC_ALL       = 4,
    RTK_IAP_MSG_MAX,
} T_RELAY_MSG_TYPE;

typedef struct t_rtk_iap_info T_RTK_IAP_INFO;

struct t_rtk_iap_info
{
    T_RTK_IAP_INFO  *p_next;
    bool            session_connected;
    uint16_t        session_id;
    uint8_t         bd_addr[6];
};

typedef struct t_relay_info
{
    uint32_t num;
    T_RTK_IAP_INFO iap_infos[1];
} T_RELAY_INFO;

static struct
{
    T_OS_QUEUE queue;
} app_iap_rtk;


#define INVALID_SESSION_ID 0xFFFF

static T_RTK_IAP_INFO *app_iap_rtk_search_by_addr(uint8_t *addr)
{
    T_RTK_IAP_INFO *p_elem = (T_RTK_IAP_INFO *)app_iap_rtk.queue.p_first;

    for (; p_elem != NULL; p_elem = p_elem->p_next)
    {
        if (memcmp(p_elem->bd_addr, addr, 6) == 0)
        {
            return p_elem;
        }
    }

    return NULL;
}

static void app_iap_rtk_send_msg_to_sec(T_RELAY_INFO *p_relay_info, T_RELAY_MSG_TYPE msg_type)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    APP_PRINT_TRACE1("app_iap_rtk_send_msg_to_sec: msg_type %d", msg_type);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_RTK_IAP, msg_type,
                                        (uint8_t *)p_relay_info, sizeof(*p_relay_info));
}

static void app_iap_rtk_send_info_to_sec(T_RTK_IAP_INFO *p_iap_info, T_RELAY_MSG_TYPE msg_type)
{
    T_RELAY_INFO relay_info = {0};

    relay_info.num = 1;
    memcpy(&relay_info.iap_infos[0], p_iap_info, sizeof(relay_info.iap_infos[0]));
    app_iap_rtk_send_msg_to_sec(&relay_info, msg_type);
}

static void app_iap_rtk_ready_to_read_process(COMMON_STREAM stream)
{
    T_APP_BR_LINK *p_link;
    uint8_t bd_addr[6];
    uint8_t *p_data = NULL;
    uint32_t len = 0;
    uint16_t data_len;
    uint8_t app_idx;
    uint16_t total_len;
    uint8_t *data = NULL;

    common_stream_read(stream, &data, &len);

    p_data = data;
    data_len = len;

    common_stream_get_addr(stream, bd_addr);

    p_link = app_link_find_br_link(bd_addr);
    if (p_link == NULL)
    {
        APP_PRINT_ERROR0("app_iap_rtk_ready_to_read_process: no acl link found");
        free(data);
        return;
    }

    app_idx = p_link->id;

    if (app_cfg_const.enable_rtk_vendor_cmd)
    {
        if (app_db.br_link[app_idx].p_embedded_cmd == NULL)
        {
            uint16_t cmd_len;

            //ios will auto combine two cmd into one pkt
            while (data_len)
            {
                if (p_data[0] == CMD_SYNC_BYTE)
                {
                    cmd_len = (p_data[2] | (p_data[3] << 8)) + 4; //sync_byte, seqn, length
                    if (data_len >= cmd_len)
                    {
                        app_cmd_handler(&p_data[4], (cmd_len - 4), CMD_PATH_IAP, p_data[1], app_idx);
                        data_len -= cmd_len;
                        p_data += cmd_len;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    data_len--;
                    p_data++;
                }
            }

            if (data_len)
            {
                app_db.br_link[app_idx].p_embedded_cmd = malloc(data_len);
                memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_data, data_len);
                app_db.br_link[app_idx].embedded_cmd_len = data_len;
            }
        }
        else
        {
            uint8_t *p_temp;
            uint16_t cmd_len;

            p_temp = app_db.br_link[app_idx].p_embedded_cmd;
            total_len = app_db.br_link[app_idx].embedded_cmd_len + data_len;
            app_db.br_link[app_idx].p_embedded_cmd = malloc(total_len);
            memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_temp,
                   app_db.br_link[app_idx].embedded_cmd_len);
            free(p_temp);
            memcpy(app_db.br_link[app_idx].p_embedded_cmd +
                   app_db.br_link[app_idx].embedded_cmd_len,
                   p_data, data_len);
            app_db.br_link[app_idx].embedded_cmd_len = total_len;

            p_data = app_db.br_link[app_idx].p_embedded_cmd;
            data_len = total_len;
            p_temp = app_db.br_link[app_idx].p_embedded_cmd;
            app_db.br_link[app_idx].p_embedded_cmd = NULL;

            //ios will auto combine two cmd into one pkt
            while (data_len)
            {
                if (p_data[0] == CMD_SYNC_BYTE)
                {
                    cmd_len = (p_data[2] | (p_data[3] << 8)) + 4; //sync_byte, seqn, length
                    if (data_len >= cmd_len)
                    {
                        app_cmd_handler(&p_data[4], (cmd_len - 4), CMD_PATH_IAP, p_data[1], app_idx);
                        data_len -= cmd_len;
                        p_data += cmd_len;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    data_len--;
                    p_data++;
                }
            }

            if (data_len)
            {
                app_db.br_link[app_idx].p_embedded_cmd = malloc(data_len);
                memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_data, data_len);
            }
            app_db.br_link[app_idx].embedded_cmd_len = data_len;
            free(p_temp);
        }
    }
}

static void app_iap_rtk_stream_cb(COMMON_STREAM stream, COMMON_STREAM_EVENT event)
{
    APP_PRINT_TRACE1("app_iap_rtk_stream_cb: event %d", event);

    switch (event)
    {
    case COMMON_STREAM_CONNECTED_EVENT:
        {
            T_RTK_IAP_INFO *p_iap_info = NULL;
            IAP_STREAM iap_stream = (IAP_STREAM) stream;

            p_iap_info = app_iap_rtk_search_by_addr(stream->bd_addr);

            if (p_iap_info)
            {
                p_iap_info->session_connected = true;
                p_iap_info->session_id = iap_stream->eap_session_id;
                app_iap_rtk_send_info_to_sec(p_iap_info, RTK_IAP_CONNECTED);
            }
        }
        break;

    case COMMON_STREAM_DISCONNECTED_EVENT:
        {
            T_RTK_IAP_INFO *p_iap_info = NULL;

            p_iap_info = app_iap_rtk_search_by_addr(stream->bd_addr);
            if (p_iap_info)
            {
                p_iap_info->session_connected = false;
                p_iap_info->session_id = INVALID_SESSION_ID;
                app_iap_rtk_send_info_to_sec(p_iap_info, RTK_IAP_DISCONNECTED);
            }
        }
        break;

    case COMMON_STREAM_READY_TO_READ_EVENT:
        {
            app_iap_rtk_ready_to_read_process(stream);
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_iap_rtk_stream_cb: unknown event %d", event);
        }
        break;
    }
}

bool app_iap_rtk_delete(uint8_t *bd_addr)
{
    IAP_STREAM iap_rtk_stream = NULL;

    APP_PRINT_TRACE1("app_iap_rtk_delete: bd_addr %s", TRACE_BDADDR(bd_addr));

    iap_rtk_stream = iap_stream_find_by_ea_protocol_id(EA_PROTOCOL_ID_RTK, bd_addr);

    if (iap_rtk_stream == NULL)
    {
        APP_PRINT_ERROR0("app_iap_rtk_delete: iap_rtk_stream is NULL");
        return false;
    }

    common_stream_delete((COMMON_STREAM)iap_rtk_stream);

    T_RTK_IAP_INFO *p_elem = NULL;
    p_elem = app_iap_rtk_search_by_addr(bd_addr);

    if (p_elem != NULL)
    {
        app_iap_rtk_send_info_to_sec(p_elem, RTK_IAP_DELETE);
        os_queue_delete(&app_iap_rtk.queue, p_elem);
        free(p_elem);
    }

    return true;
}

bool app_iap_rtk_create(uint8_t *bd_addr)
{
    APP_PRINT_TRACE1("app_iap_rtk_create: %s", TRACE_BDADDR(bd_addr));

    IAP_INIT_SETTINGS iap_stream_settings =
    {
        .bd_addr            = bd_addr,
        .stream_cb          = app_iap_rtk_stream_cb,
        .ea_protocol_id     = EA_PROTOCOL_ID_RTK,
    };

    iap_stream_create(&iap_stream_settings);

    T_RTK_IAP_INFO *p_elem = NULL;

    p_elem = app_iap_rtk_search_by_addr(bd_addr);
    if (p_elem == NULL)
    {
        p_elem = calloc(1, sizeof(*p_elem));
        p_elem->p_next = NULL;
        p_elem->session_connected = false;
        p_elem->session_id = INVALID_SESSION_ID;
        memcpy(p_elem->bd_addr, bd_addr, sizeof(p_elem->bd_addr));
        os_queue_in(&app_iap_rtk.queue, p_elem);
    }

    app_iap_rtk_send_info_to_sec(p_elem, RTK_IAP_CREATE);

    return true;
}

static void app_iap_rtk_sync_all_delete(T_RELAY_INFO *p_info)
{
    T_RTK_IAP_INFO *p_elem = (T_RTK_IAP_INFO *)app_iap_rtk.queue.p_first;
    uint32_t info_num = p_info->num;

    for (; p_elem != NULL; p_elem = p_elem->p_next)
    {
        uint32_t i = 0;
        uint8_t *bd_addr = p_elem->bd_addr;

        APP_PRINT_TRACE1("app_iap_rtk_sync_all_delete: local bd_addr %s", TRACE_BDADDR(bd_addr));

        for (; i < info_num; i++)
        {
            if (memcmp(bd_addr, p_info->iap_infos[i].bd_addr, 6) == 0)
            {
                break;
            }
        }

        if (i == info_num)
        {
            app_iap_rtk_delete(bd_addr);
        }
    }
}

static void app_iap_rtk_sync_all_create(T_RELAY_INFO *p_info)
{
    T_RTK_IAP_INFO *p_elem = NULL;
    uint32_t info_num = p_info->num;

    for (uint32_t i = 0; i < info_num; i++)
    {
        uint8_t *bd_addr = p_info->iap_infos[i].bd_addr;

        APP_PRINT_TRACE1("app_iap_rtk_sync_all_create: remote bd_addr %s", TRACE_BDADDR(bd_addr));

        p_elem = app_iap_rtk_search_by_addr(bd_addr);
        if (p_elem == NULL)
        {
            app_iap_rtk_create(bd_addr);
            p_elem = app_iap_rtk_search_by_addr(bd_addr);
        }
        p_elem->session_connected = p_info->iap_infos[i].session_connected;
        p_elem->session_id = p_info->iap_infos[i].session_id;
    }
}

static void app_iap_rtk_sync_all(T_RELAY_INFO *p_info)
{
    app_iap_rtk_sync_all_delete(p_info);
    app_iap_rtk_sync_all_create(p_info);
}

static void app_iap_rtk_relay_parse(T_RELAY_MSG_TYPE msg_type, T_RELAY_INFO *p_info, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    APP_PRINT_TRACE1("app_iap_rtk_relay_parse: msg_type %d", msg_type);

    switch (msg_type)
    {
    case RTK_IAP_CREATE:
        app_iap_rtk_create(p_info->iap_infos[0].bd_addr);
        break;

    case RTK_IAP_DELETE:
        app_iap_rtk_delete(p_info->iap_infos[0].bd_addr);
        break;

    case RTK_IAP_CONNECTED:
    case RTK_IAP_DISCONNECTED:
        {
            T_RTK_IAP_INFO *p_iap_info = app_iap_rtk_search_by_addr(p_info->iap_infos[0].bd_addr);
            if (p_iap_info)
            {
                p_iap_info->session_connected = p_info->iap_infos[0].session_connected;
                p_iap_info->session_id = p_info->iap_infos[0].session_id;
            }
        }
        break;

    case SYNC_ALL:
        app_iap_rtk_sync_all(p_info);
        break;

    default:
        break;
    }
}

void app_iap_rtk_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    T_RELAY_INFO *p_info = calloc(1,
                                  sizeof(*p_info) + app_iap_rtk.queue.count * sizeof(p_info->iap_infos[0]));
    T_RTK_IAP_INFO *p_elem = (T_RTK_IAP_INFO *)app_iap_rtk.queue.p_first;

    for (uint32_t i = 0; p_elem != NULL; i++, p_elem = p_elem->p_next)
    {
        APP_PRINT_TRACE2("app_iap_rtk_handle_remote_conn_cmpl: i %d, bd_addr %s", i,
                         TRACE_BDADDR(p_elem->bd_addr));
        memcpy(&p_info->iap_infos[i], p_elem, sizeof(p_info->iap_infos[i]));
        p_info->num++;
    }

    app_iap_rtk_send_msg_to_sec(p_info, SYNC_ALL);
    free(p_info);
}

bool app_iap_rtk_launch(uint8_t *bd_addr, T_BT_IAP_APP_LAUNCH_METHOD method)
{
    T_RTK_IAP_INFO *p_iap_info = NULL;

    p_iap_info = app_iap_rtk_search_by_addr(bd_addr);
    if (p_iap_info)
    {
        if (p_iap_info->session_connected)
        {
            APP_PRINT_ERROR0("app_iap_rtk_launch: this rtk iap already connected");
            return false;
        }
    }

    char boundle_id[] = "com.realtek.EADemo2";
    bt_iap_app_launch(bd_addr, boundle_id, sizeof(boundle_id), method);

    return true;
}

bool app_iap_rtk_connected(uint8_t *bd_addr)
{
    T_RTK_IAP_INFO *p_iap_info = NULL;

    p_iap_info = app_iap_rtk_search_by_addr(bd_addr);
    if (p_iap_info)
    {
        return p_iap_info->session_connected;
    }

    return false;
}

bool app_iap_rtk_send(uint8_t *bd_addr, uint8_t *data, uint32_t len)
{
    T_RTK_IAP_INFO *p_iap_info = NULL;

    p_iap_info = app_iap_rtk_search_by_addr(bd_addr);
    if (p_iap_info)
    {
        return bt_iap_data_send(bd_addr,
                                p_iap_info->session_id, data, len);
    }

    return false;
}

void app_iap_rtk_init(void)
{
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_iap_rtk_relay_parse, APP_MODULE_TYPE_RTK_IAP,
                             RTK_IAP_MSG_MAX);

    os_queue_init(&app_iap_rtk.queue);
}
#endif

