/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#include "string.h"
#include "stdlib.h"
#include "trace.h"
#include "gap_le.h"
#include "remote.h"
#include "bt_bond.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "btm.h"
#include "eq_utils.h"
#include "app_main.h"
#include "app_roleswap.h"
#include "pm.h"
#include "app_ota.h"
#include "app_device.h"

typedef struct t_app_relay_cback_item
{
    struct t_app_relay_cback_item *p_next;
    P_APP_RELAY_CBACK              relay_cback;
    P_APP_PARSE_CBACK              parse_cback;
    uint16_t                       module_total_len;
    T_APP_MODULE_TYPE              module_type;
    uint8_t                        msg_type_max;
} T_APP_RELAY_CBACK_ITEM;

typedef struct t_app_relay_msg_item
{
    struct t_app_relay_msg_item    *p_next;
    P_APP_RELAY_CBACK               relay_cback;
    uint16_t                        len;
    T_APP_MODULE_TYPE               module_type;
    uint8_t                         msg_type;
    uint8_t                         msg_type_max;
} T_APP_RELAY_MSG_ITEM;

#if F_APP_ERWS_SUPPORT

T_APP_RELAY_CBACK_ITEM *app_relay_find_cb_by_module_type(T_APP_MODULE_TYPE module_type)
{
    T_APP_RELAY_CBACK_ITEM *p_item;

    p_item = (T_APP_RELAY_CBACK_ITEM *)app_db.relay_cback_list.p_first;

    if (module_type == APP_MODULE_TYPE_NONE)
    {
        return NULL;
    }

    while (p_item != NULL)
    {
        if (p_item->module_type == module_type)
        {
            return p_item;
        }

        p_item = p_item->p_next;
    }

    return NULL;
}

void app_relay_cback(uint16_t event, T_REMOTE_RELAY_STATUS status, void *buf, uint16_t len)
{
    APP_PRINT_TRACE2("app_relay_cback: event 0x%04x, status %d", event, status);

    switch (event)
    {
    case APP_REMOTE_MSG_ROLESWAP_STATE_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t event;
            uint8_t para;

            event = p_info[0];
            para = p_info[1];
            app_roleswap_state_machine(event, 1, para);
        }
        break;

    case APP_REMOTE_MSG_COMMON_RELAY:
        {
            uint16_t offset = 0;
            uint16_t msg_len = 0;
            uint8_t module_type;
            uint8_t msg_type;
            uint8_t *p_info = (uint8_t *)buf;

            T_APP_RELAY_CBACK_ITEM *p_item;
            uint8_t *p = p_info;

            while (offset < len)
            {
                LE_STREAM_TO_UINT16(msg_len, p);
                LE_STREAM_TO_UINT8(module_type, p);
                LE_STREAM_TO_UINT8(msg_type, p);
                APP_PRINT_TRACE5("app_relay_cback: msg_len %d, module_type 0x%x, msg_type 0x%x, len %d, offset %d,",
                                 msg_len, module_type,
                                 msg_type, len, offset);
                p_item = app_relay_find_cb_by_module_type((T_APP_MODULE_TYPE)module_type);
                if (p_item)
                {
                    if (msg_len > 4)
                    {
                        p_item->parse_cback(msg_type, p,  msg_len - 4, status);
                    }
                    else
                    {
                        p_item->parse_cback(msg_type, NULL,  0, status);
                    }


                }

                offset += msg_len;
                p = &p_info[offset];
            }
        }
        break;

    default:
        break;
    }
}

uint16_t app_relay_msg_pack(uint8_t *buf, uint8_t msg_type, T_APP_MODULE_TYPE module_type,
                            uint16_t payload_len, uint8_t *msg_ptr, bool skip, bool total)
{
    uint16_t msg_len;

    msg_len = payload_len + APP_RELAY_HEADER_LEN;
    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = module_type;
            buf[3] = msg_type;
            if (msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
    }
    return msg_len;
}

static uint16_t app_relay_get_msg_info(P_APP_RELAY_CBACK relay_cb, uint8_t *buf, uint8_t msg_type,
                                       uint8_t msg_max)
{
    if (relay_cb == NULL)
    {
        return 0;
    }

    uint16_t len = 0;
    uint16_t temp_len = 0;

    if (msg_type == 0xFF) // relay total msg
    {
        for (uint8_t i = 0; i < msg_max; i++)
        {
            temp_len = relay_cb(buf, i, true);
            len += temp_len;

            if (buf != NULL)
            {
                buf += temp_len;
            }
        }
    }
    else //relay one msg
    {
        len = relay_cb(buf, msg_type, false);
    }
    return len;
}

bool app_relay_cback_register(P_APP_RELAY_CBACK relay_cb, P_APP_PARSE_CBACK parse_cb,
                              T_APP_MODULE_TYPE module_type, uint8_t msg_max)
{
    T_APP_RELAY_CBACK_ITEM *p_item;

    p_item = (T_APP_RELAY_CBACK_ITEM *)app_db.relay_cback_list.p_first;
    while (p_item != NULL)
    {
        if ((p_item->relay_cback == relay_cb) && (p_item->parse_cback == parse_cb))
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = (T_APP_RELAY_CBACK_ITEM *)malloc(sizeof(T_APP_RELAY_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->relay_cback = relay_cb;
        p_item->parse_cback = parse_cb;
        p_item->module_type = module_type;
        p_item->msg_type_max = msg_max;
        os_queue_in(&app_db.relay_cback_list, p_item);
        return true;
    }

    return false;
}

bool app_relay_total_async(void)
{
    uint8_t *buf = NULL;
    uint16_t total_len = 0;

    T_APP_RELAY_CBACK_ITEM *p_item;

    p_item = (T_APP_RELAY_CBACK_ITEM *)app_db.relay_cback_list.p_first;

    while (p_item != NULL)
    {
        p_item->module_total_len = app_relay_get_msg_info(p_item->relay_cback, NULL, 0xFF,
                                                          p_item->msg_type_max);
        total_len += p_item->module_total_len;
        p_item = p_item->p_next;
    }

    APP_PRINT_TRACE1("app_relay_total_async: total_len %d", total_len);

    if (total_len > APP_RELAY_MTU)
    {
        buf = (uint8_t *)malloc(APP_RELAY_MTU);
    }
    else
    {
        buf = (uint8_t *)malloc(total_len);
    }

    if (buf == NULL)
    {
        return false;
    }

    p_item = (T_APP_RELAY_CBACK_ITEM *)app_db.relay_cback_list.p_first;

    uint8_t *temp_buf = buf;
    uint16_t temp_len = 0;
    uint16_t temp_len_next = 0;

    while (p_item != NULL)
    {
        app_relay_get_msg_info(p_item->relay_cback, temp_buf, 0xFF, p_item->msg_type_max);
        temp_buf += p_item->module_total_len;
        temp_len += p_item->module_total_len;

        if (p_item->p_next != NULL)
        {
            temp_len_next = temp_len + p_item->p_next->module_total_len;
            if (temp_len_next > APP_RELAY_MTU)
            {
                remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, temp_len, false);
                temp_buf = buf;
                temp_len = 0;
            }
        }
        p_item = p_item->p_next;
    }
    remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, temp_len, false);

    free(buf);
    return true;
}

bool app_relay_async_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                         uint8_t *payload_buf, uint16_t payload_len)
{
    bool ret = false;
    uint16_t msg_len = payload_len + APP_RELAY_HEADER_LEN;

    APP_PRINT_TRACE3("app_relay_async_single_with_raw_msg: module type = 0x%x, msg type = 0x%x,payload_len = %d",
                     module_type, msg_type, payload_len);
    if (msg_len > APP_RELAY_MTU)
    {
        return false;
    }

    uint8_t *buf = NULL;
    buf = (uint8_t *)malloc(msg_len);
    if (buf == NULL)
    {
        return false;
    }

    buf[0] = (uint8_t)(msg_len & 0xFF);
    buf[1] = (uint8_t)(msg_len >> 8);
    buf[2] = module_type;
    buf[3] = msg_type;
    if ((payload_buf != NULL) && (payload_len != 0))
    {
        memcpy(&buf[4], payload_buf, payload_len);
    }

    ret = remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, msg_len, false);
    free(buf);
    return ret;
}

bool app_relay_async_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type)
{
    uint8_t *buf = NULL;
    bool ret = false;

    T_APP_RELAY_CBACK_ITEM *p_cb;

    APP_PRINT_TRACE2("app_relay_async_single: module_type : 0x%x, msg_type : 0x%x", module_type,
                     msg_type);

    p_cb = app_relay_find_cb_by_module_type(module_type);
    if (p_cb == NULL)
    {
        return false;
    }

    uint16_t len = app_relay_get_msg_info(p_cb->relay_cback, NULL, msg_type, p_cb->msg_type_max);

    if ((len > APP_RELAY_MTU) || (len == 0))
    {
        return false;
    }

    buf = (uint8_t *)malloc(len);

    if (buf == NULL)
    {
        return false;
    }
    app_relay_get_msg_info(p_cb->relay_cback, buf, msg_type, p_cb->msg_type_max);

    ret = remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, len, false);
    free(buf);
    return ret;
}

bool app_relay_sync_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                        uint8_t *payload_buf, uint16_t payload_len,
                                        T_REMOTE_TIMER_TYPE timer_type,
                                        uint32_t timer_period, bool loopback)
{
    bool ret = false;
    uint16_t msg_len = payload_len + APP_RELAY_HEADER_LEN;

    APP_PRINT_TRACE3("app_relay_sync_single_with_raw_msg: module type = 0x%x, msg type = 0x%x, payload_len = %d",
                     module_type, msg_type, payload_len);
    if (msg_len > APP_RELAY_MTU)
    {
        return false;
    }

    uint8_t *buf = NULL;
    buf = (uint8_t *)malloc(msg_len);
    if (buf == NULL)
    {
        return false;
    }

    buf[0] = (uint8_t)(msg_len & 0xFF);
    buf[1] = (uint8_t)(msg_len >> 8);
    buf[2] = module_type;
    buf[3] = msg_type;
    if ((payload_buf != NULL) && (payload_len != 0))
    {
        memcpy(&buf[4], payload_buf, payload_len);
    }

    ret = remote_sync_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, msg_len,
                                timer_type, timer_period, loopback);
    free(buf);
    return ret;
}

bool app_relay_sync_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                           T_REMOTE_TIMER_TYPE timer_type,
                           uint32_t timer_period, bool loopback)
{
    uint8_t *buf = NULL;
    bool ret = false;

    T_APP_RELAY_CBACK_ITEM *p_cb;

    APP_PRINT_TRACE2("app_relay_sync_single: module_type : 0x%x, msg_type : 0x%x", module_type,
                     msg_type);

    p_cb = app_relay_find_cb_by_module_type(module_type);
    if (p_cb == NULL)
    {
        return false;
    }

    uint16_t len = app_relay_get_msg_info(p_cb->relay_cback, NULL, msg_type, p_cb->msg_type_max);

    if ((len > APP_RELAY_MTU) || (len == 0))
    {
        return false;
    }

    buf = (uint8_t *)malloc(len);
    if (buf == NULL)
    {
        return false;
    }
    app_relay_get_msg_info(p_cb->relay_cback, buf, msg_type, p_cb->msg_type_max);

    ret = remote_sync_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, len,
                                timer_type, timer_period, loopback);
    free(buf);
    return ret;
}

T_APP_RELAY_MSG_LIST_HANDLE app_relay_async_build(void)
{
    T_OS_QUEUE *relay_msg_async_list = malloc(sizeof(T_OS_QUEUE));
    if (relay_msg_async_list != NULL)
    {
        os_queue_init(relay_msg_async_list);
    }

    return (T_APP_RELAY_MSG_LIST_HANDLE)relay_msg_async_list;
}

void app_relay_async_add(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle, T_APP_MODULE_TYPE module_type,
                         uint8_t msg_type)
{
    if (msg_list_handle == NULL)
    {
        return;
    }

    T_APP_RELAY_CBACK_ITEM *p_cb;
    T_APP_RELAY_MSG_ITEM *p_msg;

    p_cb = app_relay_find_cb_by_module_type(module_type);
    if (p_cb == NULL)
    {
        return;
    }

    uint16_t len = app_relay_get_msg_info(p_cb->relay_cback, NULL, msg_type, p_cb->msg_type_max);

    APP_PRINT_TRACE3("app_relay_async_add: module_type : 0x%x, msg_type : 0x%x, len : %d", module_type,
                     msg_type, len);

    if ((len > APP_RELAY_MTU) || (len == 0))
    {
        return;
    }

    p_msg = (T_APP_RELAY_MSG_ITEM *)malloc(sizeof(T_APP_RELAY_MSG_ITEM));
    if (p_msg != NULL)
    {
        p_msg->relay_cback = p_cb->relay_cback;
        p_msg->module_type = module_type;
        p_msg->msg_type = msg_type;
        p_msg->len = len;
        p_msg->msg_type_max = p_cb->msg_type_max;
        os_queue_in((T_OS_QUEUE *)msg_list_handle, p_msg);
    }
}

void app_relay_async_send(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle)
{
    if (msg_list_handle == NULL)
    {
        return;
    }

    T_OS_QUEUE *relay_msg_async_list = (T_OS_QUEUE *)msg_list_handle;

    uint16_t total_len = 0;
    uint8_t *buf = NULL;
    T_APP_RELAY_MSG_ITEM *p_msg;

    p_msg = (T_APP_RELAY_MSG_ITEM *)relay_msg_async_list->p_first;
    while (p_msg != NULL)
    {
        total_len += p_msg->len;
        p_msg = p_msg->p_next;
    }

    APP_PRINT_TRACE1("app_relay_async_send: total_len : %d", total_len);

    if (total_len == 0)
    {
        free(msg_list_handle);
        return;
    }

    if (total_len > APP_RELAY_MTU)
    {
        buf = (uint8_t *)malloc(APP_RELAY_MTU);
    }
    else
    {
        buf = (uint8_t *)malloc(total_len);
    }

    if (buf == NULL)
    {
        p_msg = (T_APP_RELAY_MSG_ITEM *)relay_msg_async_list->p_first;
        while (p_msg != NULL)
        {
            T_APP_RELAY_MSG_ITEM *p_msg_next = (T_APP_RELAY_MSG_ITEM *)p_msg->p_next;
            os_queue_delete(relay_msg_async_list, p_msg);
            free(p_msg);
            p_msg = p_msg_next;
        }

        free(msg_list_handle);
        return;
    }

    uint8_t *temp_buf = buf;
    uint16_t temp_len = 0;
    uint16_t temp_len_next = 0;

    p_msg = (T_APP_RELAY_MSG_ITEM *)relay_msg_async_list->p_first;
    while (p_msg != NULL)
    {
        app_relay_get_msg_info(p_msg->relay_cback, temp_buf, p_msg->msg_type, p_msg->msg_type_max);
        temp_buf += p_msg->len;
        temp_len += p_msg->len;

        if (p_msg->p_next != NULL)
        {
            temp_len_next = temp_len + p_msg->p_next->len;
            if (temp_len_next > APP_RELAY_MTU)
            {
                remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, temp_len, false);
                temp_buf = buf;
                temp_len = 0;
            }
        }
        p_msg = p_msg->p_next;
    }
    remote_async_msg_relay(app_db.relay_handle, APP_REMOTE_MSG_COMMON_RELAY, buf, temp_len, false);

    free(buf);

    p_msg = (T_APP_RELAY_MSG_ITEM *)relay_msg_async_list->p_first;
    while (p_msg != NULL)
    {
        T_APP_RELAY_MSG_ITEM *p_msg_next = (T_APP_RELAY_MSG_ITEM *)p_msg->p_next;
        os_queue_delete(relay_msg_async_list, p_msg);
        free(p_msg);
        p_msg = p_msg_next;
    }

    free(msg_list_handle);
}

bool app_relay_init(void)
{
    app_db.relay_handle = remote_relay_register(app_relay_cback);
    if (app_db.relay_handle == NULL)
    {
        return false;
    }

    os_queue_init(&app_db.relay_cback_list);

    return true;
}

#else
uint16_t app_relay_msg_pack(uint8_t *buf, uint8_t msg_type, T_APP_MODULE_TYPE module_type,
                            uint16_t payload_len, uint8_t *msg_ptr, bool skip, bool total)
{
    return 0;
}

bool app_relay_cback_register(P_APP_RELAY_CBACK relay_cb, P_APP_PARSE_CBACK parse_cb,
                              T_APP_MODULE_TYPE module_type, uint8_t msg_max)
{
    return false;
}

bool app_relay_async_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                         uint8_t *payload_buf, uint16_t payload_len)
{
    return false;
}

bool app_relay_async_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type)
{
    return false;
}

bool app_relay_sync_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                        uint8_t *payload_buf, uint16_t payload_len,
                                        T_REMOTE_TIMER_TYPE timer_type,
                                        uint32_t timer_period, bool loopback)
{
    return false;
}

bool app_relay_sync_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                           T_REMOTE_TIMER_TYPE timer_type,
                           uint32_t timer_period, bool loopback)
{
    return false;
}


T_APP_RELAY_MSG_LIST_HANDLE app_relay_async_build(void)
{
    return NULL;
}

void app_relay_async_add(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle, T_APP_MODULE_TYPE module_type,
                         uint8_t msg_type)
{

}

void app_relay_async_send(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle)
{

}

bool app_relay_init(void)
{
    return false;
}
#endif
