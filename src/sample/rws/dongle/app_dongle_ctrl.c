#if F_APP_GAMING_DONGLE_SUPPORT

/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file      app_dongle_ctrl.c
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
  * @attention
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "bt_bond.h"
#include "bt_spp.h"
#include "app_main.h"
#include "app_link_util.h"
#include "app_dongle_ctrl.h"
#include "app_timer.h"
#include "app_roleswap_control.h"
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#include "app_sniff_mode.h"
#include "app_dongle_data_ctrl.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/

#define MAX_DONGLE_CTRL_FIFO_LEN    15
#define MAX_CTRL_DATA_LEN           100

/*
    buf[0] = DONGLE_SPP_START_BIT;
    buf[1] = ((DONGLE_SPP_CMD & 0x0F) | (((payload_len >> 8) & 0x0F) << 4));
    buf[2] = (payload_len & 0xFF);
    buf[3] = cmd;
    memcpy(buf + 4, data, len);
    buf[len + 4] = DONGLE_SPP_STOP_BIT;

    byte 0~3 and last byte
*/
#define CMD_FORMAT_LEN          5

typedef enum
{
    APP_TIMER_SEND_DONLGE_CTRL_DATA,
    APP_TIMER_SEND_ACK_VIA_SPP_VOICE,
    APP_TIMER_SEND_SPP_EXIT_SNIFF,
} T_APP_DONGLE_CTRL_TIMER;


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         global variable
 *============================================================================*/

bool dongle_ctrl_data_send = false;
uint8_t dbg_seq = 0;
uint8_t ack_seq = 0;
uint8_t current_sending_seq = 0;

/*============================================================================*
 *                         local variable
 *============================================================================*/

static T_APP_CTRL_QUEUE *dongle_ctrl_fifo_head;
static uint8_t dongle_ctrl_fifo_num = 0;
static uint8_t dongle_ctrl_data_cnt = 1; /* 1~255 */
static uint8_t app_dongle_ctrl_timer_id = 0;
static uint8_t timer_idx_send_dongle_ctrl_data = 0;
static uint8_t timer_idx_send_ack_via_spp_voice = 0;
static uint8_t timer_idx_send_spp_exit_sniff = 0;
static uint8_t last_rcv_pkt_seq;
static uint8_t retry_cnt = 0;

/*============================================================================*
 *                         Functions
 *============================================================================*/

static bool dongle_ctrl_fifo_enqueue(uint8_t *data, uint16_t size)
{
    bool ret = false;

    if (dongle_ctrl_fifo_num >= MAX_DONGLE_CTRL_FIFO_LEN)
    {
        APP_PRINT_ERROR1("dongle_ctrl_fifo_enqueue: buf full : %d", dongle_ctrl_fifo_num);
        return ret;
    }

    T_APP_CTRL_QUEUE *new_data = NULL;

    new_data = malloc(sizeof(T_APP_CTRL_QUEUE) + CMD_FORMAT_LEN);

    if (new_data != NULL)
    {
        new_data->data = malloc(size);

        if (new_data->data != NULL)
        {
            memcpy(new_data->data, data, size);
            new_data->header.size = size;

            new_data->header.is_ack = 0;
            new_data->header.seq = dongle_ctrl_data_cnt;

            dongle_ctrl_data_cnt = (dongle_ctrl_data_cnt == 255) ? 1 : (dongle_ctrl_data_cnt + 1);
        }

        new_data->next = NULL;

        if (dongle_ctrl_fifo_num == 0)
        {
            dongle_ctrl_fifo_head = new_data;
        }
        else
        {
            new_data->next = dongle_ctrl_fifo_head;
            dongle_ctrl_fifo_head = new_data;
        }

        dongle_ctrl_fifo_num++;
        ret = true;
    }
    return ret;

}

static bool dongle_ctrl_fifo_dequeue(void)
{
    if (dongle_ctrl_fifo_num <= 0)
    {
        APP_PRINT_ERROR0("dongle_ctrl_fifo_dequeue: fifo empty");
        return false;
    }

    T_APP_CTRL_QUEUE *curr = dongle_ctrl_fifo_head;
    T_APP_CTRL_QUEUE *next = dongle_ctrl_fifo_head->next;

    if (curr->next == NULL)
    {
        free(curr->data);
        free(curr);

        dongle_ctrl_fifo_head = NULL;
    }
    else
    {
        while (next->next != NULL)
        {
            next = next->next;
            curr = curr->next;
        }

        free(next->data);
        free(next);

        curr->next = NULL;
    }

    dongle_ctrl_fifo_num--;

    return true;
}

T_APP_CTRL_QUEUE *get_first_dongle_ctrl_fifo_data(void)
{
    if (dongle_ctrl_fifo_head == NULL)
    {
        return NULL;
    }

    T_APP_CTRL_QUEUE *tmp = dongle_ctrl_fifo_head;

    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }

    return tmp;
}

static bool send_dongle_ctrl_data(uint8_t *data, uint16_t size)
{
    bool ret = false;
    uint16_t total_size = 0;
    uint8_t *buf = NULL;

#if F_APP_LEA_SUPPORT
    T_APP_LE_LINK *p_lea_link = app_dongle_get_le_audio_link();

    if (p_lea_link == NULL)
    {
        APP_PRINT_ERROR0("send_dongle_ctrl_data_by_le_link: dongle LE link is not exist");
        goto exit;
    }
#else
    T_APP_BR_LINK *p_link = app_dongle_get_connected_dongle_link();
    bool dongle_connected = false;

    if (p_link && (p_link->connected_profile & SPP_PROFILE_MASK))
    {
        dongle_connected = true;
    }

    if (dongle_connected == false)
    {
        APP_PRINT_ERROR0("send_dongle_ctrl_data_by_spp_cmd: dongle connection not exist");
        goto exit;
    }
#endif

    total_size = size + CMD_FORMAT_LEN;

    buf = malloc(total_size);

    if (buf != NULL)
    {
        ret = app_dongle_send_cmd(DONGLE_CMD_CTRL_RAW_DATA, data, size);

        if (ret == true)
        {
            dongle_ctrl_data_send = true;
        }

        free(buf);
    }

exit:

    return ret;
}

static void dongle_ctrl_pkt_send_timer_start(void)
{
    app_start_timer(&timer_idx_send_dongle_ctrl_data, "send_dongle_ctrl_data",
                    app_dongle_ctrl_timer_id, APP_TIMER_SEND_DONLGE_CTRL_DATA, 0, false,
                    250);
}

static void dongle_ctrl_pkt_send_timer_stop(void)
{
    dongle_ctrl_data_send = false;

    app_stop_timer(&timer_idx_send_dongle_ctrl_data);
}

static void send_dongle_ctrl_data_from_fifo(void)
{
    T_APP_CTRL_QUEUE *first = get_first_dongle_ctrl_fifo_data();

    if (first != NULL && dongle_ctrl_data_send == false)
    {
#if (F_APP_LEA_SUPPORT == 0)
        if (app_db.dongle_is_enable_mic && retry_cnt < 2)
        {
            APP_PRINT_TRACE0("send ctrl data via voice spp");
        }
        else
#endif
        {
            uint16_t total_size = sizeof(T_APP_CTRL_HEADER) + first->header.size;
            uint8_t *buf = malloc(total_size);

            if (buf != NULL)
            {
                T_APP_CTRL_HEADER header;

                memcpy(&header, &first->header, sizeof(T_APP_CTRL_HEADER));
                header.chk_seq = dbg_seq;
                dbg_seq = (dbg_seq == 127) ? 0 : dbg_seq + 1;

                memcpy(buf, &header, sizeof(T_APP_CTRL_HEADER));
                memcpy(buf + sizeof(T_APP_CTRL_HEADER), first->data, first->header.size);

                if (send_dongle_ctrl_data(buf, total_size) == true)
                {
                    current_sending_seq = first->header.seq;
                }

                free(buf);
            }
        }

        dongle_ctrl_pkt_send_timer_start();
    }
}

static void dequeue_and_send_next_ctrl_pkt(void)
{
    dongle_ctrl_fifo_dequeue();

    bool still_has_data_in_fifo = get_first_dongle_ctrl_fifo_data() != NULL ? true : false;

    if (still_has_data_in_fifo)
    {
        send_dongle_ctrl_data_from_fifo();
    }
}

static void send_ack(uint8_t seq)
{
    T_APP_CTRL_HEADER ack_remote;

    ack_remote.seq = seq;
    ack_remote.is_ack = 1;
    ack_remote.size = 0;
    ack_remote.chk_seq = 0;

    APP_PRINT_TRACE1("send_ack: seq %d", seq);

    app_dongle_send_cmd(DONGLE_CMD_CTRL_RAW_DATA, (uint8_t *)&ack_remote, sizeof(ack_remote));
}

static void app_dongle_ctrl_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_SEND_DONLGE_CTRL_DATA:
        {
            dongle_ctrl_pkt_send_timer_stop();

            retry_cnt++;

            /* re-send due to ack not received; also restart timer */
            send_dongle_ctrl_data_from_fifo();
        }
        break;

    case APP_TIMER_SEND_ACK_VIA_SPP_VOICE:
        {
            app_stop_timer(&timer_idx_send_ack_via_spp_voice);

            send_ack(ack_seq);
        }
        break;

    case APP_TIMER_SEND_SPP_EXIT_SNIFF:
        {
            uint8_t addr[6] = {0};

            app_stop_timer(&timer_idx_send_spp_exit_sniff);

            if (app_dongle_get_connected_dongle_addr(addr))
            {
                app_sniff_mode_b2s_enable(addr, SNIFF_DISABLE_MASK_SPP_CTRL_PKT);
            }
        }
        break;

    default:
        break;
    }
}

static void app_dongle_handle_ctrl_pkt(uint8_t *data, uint16_t size)
{
    APP_PRINT_TRACE1("app_dongle_handle_ctrl_pkt: %b", size > 10 ? TRACE_BINARY(10,
                                                                                data) : TRACE_BINARY(size, data));
}

uint16_t app_dongle_get_append_ctrl_data_len(void)
{
    T_APP_CTRL_QUEUE *first = get_first_dongle_ctrl_fifo_data();
    uint16_t len = 0;

    if (first != NULL)
    {
        bool has_data_to_send = (dongle_ctrl_data_send == false &&
                                 app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE);

        uint16_t input_size = sizeof(T_APP_CTRL_HEADER) + first->header.size;

        if (has_data_to_send)
        {
            len = (input_size + CMD_FORMAT_LEN);
        }
    }
    else if (timer_idx_send_ack_via_spp_voice)
    {
        len = (sizeof(T_APP_CTRL_HEADER) + CMD_FORMAT_LEN);
    }

    return len;
}

void app_dongle_append_ctrl_data_to_voice_spp(uint8_t *start)
{
    T_APP_CTRL_QUEUE *first = get_first_dongle_ctrl_fifo_data();
    bool has_data_to_send = (first != NULL && dongle_ctrl_data_send == false &&
                             app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE);
    uint8_t cause = 0;

    if (has_data_to_send)
    {
        uint16_t data_size = sizeof(T_APP_CTRL_HEADER) + first->header.size;

        cause = 1;

        start[0] = DONGLE_FORMAT_START_BIT;
        start[1] = DONGLE_TYPE_CMD;
        start[2] = 1 + data_size;
        start[3] = DONGLE_CMD_CTRL_RAW_DATA;

        T_APP_CTRL_HEADER header;
        memcpy(&header, &first->header, sizeof(T_APP_CTRL_HEADER));
        header.chk_seq = dbg_seq;
        dbg_seq = (dbg_seq == 127) ? 0 : dbg_seq + 1;

        memcpy(start + 4, &header, sizeof(T_APP_CTRL_HEADER));
        memcpy(start + 4 + sizeof(T_APP_CTRL_HEADER), first->data, first->header.size);

        start[4 + data_size] = DONGLE_FORMAT_STOP_BIT;

        current_sending_seq = first->header.seq;
        dongle_ctrl_data_send = true;
    }
    else if (timer_idx_send_ack_via_spp_voice)
    {
        T_APP_CTRL_HEADER ack_remote;

        cause = 2;

        ack_remote.seq = ack_seq;
        ack_remote.is_ack = 1;
        ack_remote.size = 0;
        ack_remote.chk_seq = 0;

        start[0] = DONGLE_FORMAT_START_BIT;
        start[1] = DONGLE_TYPE_CMD;
        start[2] = 1 + sizeof(T_APP_CTRL_HEADER);
        start[3] = DONGLE_CMD_CTRL_RAW_DATA;
        memcpy(start + 4, &ack_remote, sizeof(ack_remote));
        start[4 + sizeof(ack_remote)] = DONGLE_FORMAT_STOP_BIT;

        app_dongle_stop_send_ack_timer();
    }

    if (cause != 0)
    {
        APP_PRINT_TRACE1("app_dongle_append_ctrl_data_to_voice_spp: cause %d", cause);
    }
}

void app_dongle_dispatch_ctrl_pkt(uint8_t *a2dp_payload, uint16_t *len)
{
    uint8_t *ctrl_pkt = a2dp_payload + STEREO_GAMING_PKT_SIZE;
    uint8_t ctrl_len = *len - STEREO_GAMING_PKT_SIZE;

    app_dongle_rcv_ctrl_data(ctrl_pkt, ctrl_len);

    /* update len */
    *len -= ctrl_len;
}

bool app_dongle_send_ctrl_data(uint8_t *data, uint16_t size)
{
    uint8_t cause = 0;
    uint8_t ret = true;
#if F_APP_LEA_SUPPORT
    T_APP_LE_LINK *p_lea_link = NULL;
#else
    T_APP_BR_LINK *p_link = NULL;
    bool dongle_connected = false;
#endif

    if (size > MAX_CTRL_DATA_LEN)
    {
        cause = 1;
        goto exit;
    }

#if F_APP_LEA_SUPPORT
    p_lea_link = app_dongle_get_le_audio_link();

    if (p_lea_link == NULL)
    {
        /*LE dongle not connected */
        cause = 2;
        goto exit;
    }
#else
    p_link = app_dongle_get_connected_dongle_link();

    if (p_link && p_link->connected_profile & SPP_PROFILE_MASK)
    {
        dongle_connected = true;
    }

    if (dongle_connected == false)
    {
        /* dongle not connected */
        cause = 2;
        goto exit;
    }

    if (p_link->acl_link_in_sniffmode_flg)
    {
        app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_SPP_CTRL_PKT);
        app_start_timer(&timer_idx_send_spp_exit_sniff, "send_spp_exit_sniff",
                        app_dongle_ctrl_timer_id, APP_TIMER_SEND_SPP_EXIT_SNIFF, 0, false,
                        3000);
    }
#endif

    if (dongle_ctrl_fifo_enqueue(data, size) == false)
    {
        cause = 3;
        goto exit;
    }

    send_dongle_ctrl_data_from_fifo();

exit:

    if (cause != 0)
    {
        ret = false;
    }

    APP_PRINT_TRACE2("app_dongle_send_ctrl_data: ret %d cause %d", ret, cause);

    return ret;
}

void app_dongle_stop_send_ack_timer(void)
{
    app_stop_timer(&timer_idx_send_ack_via_spp_voice);
}

void app_dongle_rcv_ctrl_data(uint8_t *data, uint16_t size)
{
    uint8_t addr[6] = {0};

    T_APP_CTRL_HEADER *header = (T_APP_CTRL_HEADER *)data;

#if F_APP_LEA_SUPPORT
    T_APP_LE_LINK *p_lea_link = app_dongle_get_le_audio_link();
#endif

    if (header->is_ack == 0
#if F_APP_LEA_SUPPORT
        && (p_lea_link != NULL)
#else
        && app_dongle_get_connected_dongle_addr(addr)
#endif
       )
    {
        T_APP_CTRL_HEADER *header = (T_APP_CTRL_HEADER *)data;

        if (header->seq != last_rcv_pkt_seq)
        {
            uint8_t *payload = (uint8_t *)data + sizeof(T_APP_CTRL_HEADER);

            app_dongle_handle_ctrl_pkt(payload, header->size);
            last_rcv_pkt_seq = header->seq;
        }

#if (F_APP_LEA_SUPPORT == 0)
        if (app_db.dongle_is_enable_mic)
        {
            /* send ack via spp voice */
            ack_seq = header->seq;

            if (timer_idx_send_ack_via_spp_voice == 0)
            {
                app_start_timer(&timer_idx_send_ack_via_spp_voice, "send_ack_via_spp_voice",
                                app_dongle_ctrl_timer_id, APP_TIMER_SEND_ACK_VIA_SPP_VOICE, 0, false,
                                100);
            }
        }
        else
#endif
        {
            send_ack(header->seq);
        }
    }
    else
    {
        if (dongle_ctrl_data_send && header->seq == current_sending_seq)
        {
            retry_cnt = 0;

            dongle_ctrl_pkt_send_timer_stop();

            dequeue_and_send_next_ctrl_pkt();
        }
    }
}

void app_dongle_ctrl_init(void)
{
    app_timer_reg_cb(app_dongle_ctrl_timeout_cb, &app_dongle_ctrl_timer_id);
}

#endif
