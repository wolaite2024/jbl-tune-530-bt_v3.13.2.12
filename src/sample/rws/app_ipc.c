/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "os_queue.h"
#include "os_mem.h"
#include "trace.h"
#include "app_ipc.h"

#define TOPIC_CHANN_NUM     8

typedef struct t_app_ipc_topic_item
{
    struct t_app_ipc_topic_item *p_next;
    P_APP_IPC_CBACK              cback;
} T_APP_IPC_TOPIC_ITEM;

typedef struct
{
    const char *topic;
    T_OS_QUEUE subscriber_list;
    bool used;
} T_APP_IPC_TOPIC_CHANN;

typedef struct
{
    T_APP_IPC_TOPIC_CHANN topic_chann[TOPIC_CHANN_NUM];
} T_APP_IPC;

static T_APP_IPC app_ipc;

bool app_ipc_init(void)
{
    memset(&app_ipc, 0, sizeof(app_ipc));

    return true;
}

bool app_ipc_publish(const char *topic, uint32_t event, void *msg)
{
    T_APP_IPC_TOPIC_ITEM *item;
    uint8_t i;

    for (i = 0; i < TOPIC_CHANN_NUM; i++)
    {
        if (app_ipc.topic_chann[i].used == true &&
            !strcmp(app_ipc.topic_chann[i].topic, topic))
        {
            item = os_queue_peek(&app_ipc.topic_chann[i].subscriber_list, 0);
            while (item != NULL)
            {
                if (item->cback != NULL)
                {
                    item->cback(event, msg);
                }

                item = item->p_next;
            }

            break;
        }
    }

    return true;
}

T_APP_IPC_HANDLE app_ipc_subscribe(const char *topic, P_APP_IPC_CBACK cback)
{
    T_APP_IPC_TOPIC_ITEM *item;
    uint8_t i;

    for (i = 0; i < TOPIC_CHANN_NUM; i++)
    {
        if (app_ipc.topic_chann[i].used == true &&
            !strcmp(app_ipc.topic_chann[i].topic, topic))
        {
            item = os_mem_alloc2(sizeof(T_APP_IPC_TOPIC_ITEM));
            if (item != NULL)
            {
                item->cback = cback;
                os_queue_in(&app_ipc.topic_chann[i].subscriber_list, item);
                return (T_APP_IPC_HANDLE)item;
            }
            else
            {
                return NULL;
            }
        }
    }

    for (i = 0; i < TOPIC_CHANN_NUM; i++)
    {
        if (app_ipc.topic_chann[i].used == false)
        {
            item = os_mem_alloc2(sizeof(T_APP_IPC_TOPIC_ITEM));
            if (item != NULL)
            {
                app_ipc.topic_chann[i].used = true;
                app_ipc.topic_chann[i].topic = topic;

                item->cback = cback;
                os_queue_in(&app_ipc.topic_chann[i].subscriber_list, item);
                return (T_APP_IPC_HANDLE)item;
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}

bool app_ipc_unsubscribe(T_APP_IPC_HANDLE handle)
{
    T_APP_IPC_TOPIC_ITEM *item;
    uint8_t i;

    item = (T_APP_IPC_TOPIC_ITEM *)handle;

    for (i = 0; i < TOPIC_CHANN_NUM; i++)
    {
        if (app_ipc.topic_chann[i].used == true)
        {
            if (os_queue_delete(&app_ipc.topic_chann[i].subscriber_list, item) == true)
            {
                os_mem_free(item);

                if (app_ipc.topic_chann[i].subscriber_list.count == 0)
                {
                    app_ipc.topic_chann[i].used = false;
                    app_ipc.topic_chann[i].topic = NULL;
                }

                return true;
            }
        }
    }

    return false;
}
