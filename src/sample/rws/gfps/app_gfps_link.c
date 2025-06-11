/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "os_mem.h"
#include "os_queue.h"
#include "app_gfps.h"
#include "app_main.h"
#include "app_ble_gap.h"
#include "app_link_util.h"
#include "app_gfps_link.h"

#if GFPS_FEATURE_SUPPORT
static T_OS_QUEUE gfps_link_priority_q;

typedef struct t_gfps_link_priority_elem
{
    struct t_gfps_link_priority_elem *p_next;
    uint8_t conn_id;
} T_GFPS_LINK_PRIORITY_ELEM;

void app_gfps_link_priority_queue_init()
{
    os_queue_init(&gfps_link_priority_q);
}

void app_gfps_link_print_link_priority()
{
    if (gfps_link_priority_q.count == 0)
    {
        return;
    }

    for (uint8_t i = 0; i < gfps_link_priority_q.count; i++)
    {
        T_GFPS_LINK_PRIORITY_ELEM *op = os_queue_peek(&gfps_link_priority_q, i);
        APP_PRINT_INFO2("app_gfps_print_link_priority: conn_id %d, priority %d", op->conn_id, i);
    }
}

void app_gfps_link_add_link_into_priority_queue(uint8_t conn_id)
{
    APP_PRINT_INFO1("app_gfps_link_add_link_into_priority_queue: conn_id %d", conn_id);

    T_GFPS_LINK_PRIORITY_ELEM *p_elem = os_mem_zalloc(RAM_TYPE_DATA_ON,
                                                      sizeof(T_GFPS_LINK_PRIORITY_ELEM));

    if (p_elem != NULL)
    {
        p_elem->conn_id = conn_id;
        os_queue_in(&gfps_link_priority_q, p_elem);
    }

    app_gfps_link_print_link_priority();
}

uint8_t app_gfps_link_delete_oldest_link_from_priority_queue(void)
{
    uint8_t conn_id = 0xFF;

    if (gfps_link_priority_q.count != 0)
    {
        T_GFPS_LINK_PRIORITY_ELEM *p_elem = os_queue_out(&gfps_link_priority_q);

        if (p_elem != NULL)
        {
            conn_id = p_elem->conn_id;
            os_mem_free(p_elem);
            p_elem = NULL;
        }
    }

    app_gfps_link_print_link_priority();

    return conn_id;
}

void app_gfps_link_delete_link_from_priority_queue(uint8_t conn_id)
{
    APP_PRINT_INFO1("app_gfps_delete_link_from_priority_queue: conn_id %d", conn_id);

    for (uint8_t i = 0; i < gfps_link_priority_q.count; i++)
    {
        T_GFPS_LINK_PRIORITY_ELEM *p_elem;

        p_elem = os_queue_peek(&gfps_link_priority_q, i);

        if (p_elem != NULL && p_elem->conn_id == conn_id)
        {
            os_queue_delete(&gfps_link_priority_q, p_elem);
            os_mem_free(p_elem);
            p_elem = NULL;
        }
    }

    app_gfps_link_print_link_priority();
}

uint8_t app_gfps_link_find_gfps_link(void)
{
    uint8_t link_num = 0;

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        T_APP_LE_LINK *le_link = &app_db.le_link[i];

        if ((le_link->used == true) && (le_link->gfps_link.gfps_conn_id != 0xFF) &&
            (le_link->gfps_link.gfps_conn_state == GAP_CONN_STATE_CONNECTED))
        {
            link_num++;
        }
    }

    APP_PRINT_INFO1("app_gfps_link_find_gfps_link: link_num %d", link_num);
    return link_num;
}

void app_gfps_link_disconnect_all_gfps_link(void)
{
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        T_APP_LE_LINK *le_link = &app_db.le_link[i];

        if ((le_link->used == true) && (le_link->gfps_link.gfps_conn_id != 0xFF) &&
            (le_link->gfps_link.gfps_conn_state != GAP_CONN_STATE_DISCONNECTED))
        {
            APP_PRINT_INFO1("app_gfps_link_disconnect_all_gfps_link: gfps_conn_id %d",
                            le_link->gfps_link.gfps_conn_id);
            app_ble_gap_disconnect(le_link, LE_LOCAL_DISC_CAUSE_GFPS_STOP);
            app_gfps_link_delete_link_from_priority_queue(le_link->gfps_link.gfps_conn_id);
        }
    }
}
#endif
