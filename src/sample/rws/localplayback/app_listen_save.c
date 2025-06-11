/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_LOCAL_PLAYBACK_SUPPORT & TARGET_RTL8773CO
#include <string.h>
#include "stdlib.h"
#include "trace.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_bond.h"
#include "bt_a2dp.h"
#include "app_cfg.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_relay.h"
#include "app_listen_save.h"
#include "audio_fs.h"
#include "ff.h"
#include "app_mmi.h"
#include "save_listen_tag.h"
#include "app_playback.h"

typedef enum
{
    APP_TIMER_SAVE_END,
} T_APP_LISTEN_SAVE_TIMER;
static uint8_t timer_idx_save_end = 0;
static uint8_t listen_save_timer_id = 0;

static uint8_t listen_saveing_state = LISTEN_SAVE_IDLE;
T_AUDIO_FS_HANDLE listen_save_fs_handle = NULL;
LISTEN_SAVE_FUNCTION_STRUCT listen_save_db;
LISTEN_SAVE_FILE_STRUCT *p_listen_save_file = NULL;
static T_LISTEN_SAVE_PRI_PKT_INFO *p_sync_pkt = NULL;
static T_LISTEN_SAVE_WAIT_PKT_INFO *p_wait_pkt = NULL;

static uint8_t app_listen_save_write_file(uint8_t *buf, uint16_t len);
static void app_listen_save_pri_sync_pkt_info(void *buf);
static uint8_t app_listen_save_local_save_write(void *buf);
static uint8_t app_listen_save_sec_sync_pkt_info(void *buf);
static uint8_t app_listen_save_relay_create_handle(void);
static void app_listen_save_repack(uint8_t file_type, T_LISTEN_SAVE_SYNC_INFO pri_pkt);
static int app_listen_save_cacu_diff_u16(uint16_t x, uint16_t y);
static void app_listen_save_wait_pkt_queue_move_one_pkt(void);

static void app_listen_save_codec_info_print(uint16_t a2dp_codec_type,
                                             LISTEN_SAVE_CODEC_INFO codec_info)
{
    if (a2dp_codec_type == BT_A2DP_CODEC_TYPE_SBC)
    {
        listen_save_db.file_type = AUDIO_FORMAT_TYPE_SBC;
        APP_PRINT_TRACE7("app_listen_save sbc sampling_frequency %d, channel_mode %d, block_length %d, "\
                         "subbands %d,allo %d, min_bit %d, max_bit %d",
                         codec_info.sbc.sampling_frequency,
                         codec_info.sbc.channel_mode,
                         codec_info.sbc.block_length,
                         codec_info.sbc.subbands,
                         codec_info.sbc.allocation_method,
                         codec_info.sbc.min_bitpool,
                         codec_info.sbc.max_bitpool);
    }
    else if (a2dp_codec_type == BT_A2DP_CODEC_TYPE_AAC)
    {
        listen_save_db.file_type = AUDIO_FORMAT_TYPE_AAC;
        APP_PRINT_TRACE6("app_listen_save aac sampling_frequency %d, channel_number %d, bit_rate %d "\
                         "object_type %d, vbr_supported %d, alloc %d",
                         codec_info.aac.sampling_frequency,
                         codec_info.aac.channel_number,
                         codec_info.aac.bit_rate,
                         codec_info.aac.object_type,
                         codec_info.aac.vbr_supported,
                         codec_info.aac.allocation_method);
    }
}

static void app_listen_save_relay_async_msg(uint8_t msg_type, uint8_t *payload_buf, uint16_t len)
{
    bool ret = false;
    ret = app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LISTEN_SAVE, msg_type, payload_buf, len);
    if (ret == false)
    {
        APP_PRINT_ERROR0("app_listen_save_relay_async_msg: fail!!!");
    }
}
static void app_listen_save_relay_sync_msg(uint8_t msg_type, uint8_t *payload_buf, uint16_t len,
                                           T_REMOTE_TIMER_TYPE timer_type,
                                           uint32_t timer_period, bool loopback)
{
    bool ret = false;
    ret = app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_LISTEN_SAVE, msg_type, payload_buf, len,
                                             timer_type, timer_period, loopback);
    if (ret == false)
    {
        APP_PRINT_ERROR0("app_listen_save_relay_sync_msg: fail!!!");
    }
}

static void app_listen_save_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_AVRCP_ELEM_ATTR:
        {
            if (param->avrcp_elem_attr.state == 0)
            {
                p_link = app_link_find_br_link(param->avrcp_elem_attr.bd_addr);
                if (p_link != NULL)
                {
                    if (p_listen_save_file != NULL)
                    {
                        free(p_listen_save_file);
                    }
                    p_listen_save_file = malloc(param->avrcp_elem_attr.attr->length + sizeof(
                                                    LISTEN_SAVE_FILE_STRUCT));
                    if (p_listen_save_file == NULL)
                    {
                        return;
                    }
                    p_listen_save_file->character_id = param->avrcp_elem_attr.attr->character_set_id;
                    p_listen_save_file->name_len = param->avrcp_elem_attr.attr->length;
                    memcpy(((uint8_t *)p_listen_save_file + sizeof(LISTEN_SAVE_FILE_STRUCT) - 1),
                           param->avrcp_elem_attr.attr->p_buf, p_listen_save_file->name_len);
                    uint8_t file_info_len = p_listen_save_file->name_len + sizeof(LISTEN_SAVE_FILE_STRUCT) - 1;
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_listen_save_relay_async_msg(LISTEN_SAVE_REMOTE_MSG_FILE_INFO_SYNC,
                                                        (uint8_t *)p_listen_save_file, file_info_len);
                    }
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_GET_PLAY_STATUS_RSP:
        {
            p_link = app_link_find_br_link(param->avrcp_get_play_status_rsp.bd_addr);
            if (p_link != NULL)
            {
//                APP_PRINT_TRACE4("AVRCP_GET_PLAY_STATUS_RSP state %d play_status %d, length_ms %d, position_ms %d",
//                                 param->avrcp_get_play_status_rsp.state, \
//                                 param->avrcp_get_play_status_rsp.play_status, \
//                                 param->avrcp_get_play_status_rsp.length_ms,
//                                 param->avrcp_get_play_status_rsp.position_ms);
            }
        }
        break;

    case BT_EVENT_AVRCP_TRACK_CHANGED:
        {

        }
        break;

    case BT_EVENT_A2DP_CONFIG_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_config_cmpl.bd_addr);
            if (p_link != NULL)
            {
                T_BT_EVENT_PARAM_A2DP_CONFIG_CMPL *cfg = &param->a2dp_config_cmpl;
                p_link->a2dp_codec_type = param->a2dp_config_cmpl.codec_type;

                LISTEN_SAVE_CODEC_INFO codec_info;
                memset((uint8_t *)&codec_info, 0, sizeof(LISTEN_SAVE_CODEC_INFO));
                memcpy((uint8_t *)&codec_info, (uint8_t *)&cfg->codec_info, sizeof(LISTEN_SAVE_CODEC_INFO));
                app_listen_save_codec_info_print(p_link->a2dp_codec_type, codec_info);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
            if (p_link != NULL)
            {
                T_BT_EVENT_PARAM_A2DP_STREAM_START_IND *cfg = &param->a2dp_stream_start_ind;
                p_link->a2dp_codec_type = param->a2dp_stream_start_ind.codec_type;

                LISTEN_SAVE_CODEC_INFO codec_info;
                memset((uint8_t *)&codec_info, 0, sizeof(LISTEN_SAVE_CODEC_INFO));
                memcpy((uint8_t *)&codec_info, (uint8_t *)&cfg->codec_info, sizeof(LISTEN_SAVE_CODEC_INFO));
                app_listen_save_codec_info_print(p_link->a2dp_codec_type, codec_info);
            }
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_sniffing_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                T_BT_EVENT_PARAM_A2DP_SNIFFING_CONN_CMPL *cfg = &param->a2dp_sniffing_conn_cmpl;
                p_link->a2dp_codec_type = param->a2dp_sniffing_conn_cmpl.codec_type;

                LISTEN_SAVE_CODEC_INFO codec_info;
                memset((uint8_t *)&codec_info, 0, sizeof(LISTEN_SAVE_CODEC_INFO));
                memcpy((uint8_t *)&codec_info, (uint8_t *)&cfg->codec_info, sizeof(LISTEN_SAVE_CODEC_INFO));
                app_listen_save_codec_info_print(p_link->a2dp_codec_type, codec_info);
            }
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_STARTED:
        {
            p_link = app_link_find_br_link(param->a2dp_sniffing_started.bd_addr);
            if (p_link != NULL)
            {
                T_BT_EVENT_PARAM_A2DP_SNIFFING_STARTED *cfg = &param->a2dp_sniffing_started;
                p_link->a2dp_codec_type = param->a2dp_sniffing_started.codec_type;

                LISTEN_SAVE_CODEC_INFO codec_info;
                memset((uint8_t *)&codec_info, 0, sizeof(LISTEN_SAVE_CODEC_INFO));
                memcpy((uint8_t *)&codec_info, (uint8_t *)&cfg->codec_info, sizeof(LISTEN_SAVE_CODEC_INFO));
                app_listen_save_codec_info_print(p_link->a2dp_codec_type, codec_info);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_stream_data_ind.bd_addr);
            if (p_link == NULL)
            {
                break;
            }
            if ((listen_saveing_state == LISTEN_SAVE_IDLE) || (listen_saveing_state == LISTEN_SAVE_END))
            {
                handle = false;
                break;
            }
//                APP_PRINT_INFO6(" end mem, data on %d, data off %d, buf on %d, buf off %d, comm on %d, comm off %d",
//                                os_mem_peek(RAM_TYPE_DATA_ON), os_mem_peek(RAM_TYPE_DATA_OFF),
//                                os_mem_peek(RAM_TYPE_BUFFER_ON), os_mem_peek(RAM_TYPE_BUFFER_OFF),
//                                os_mem_peek(RAM_TYPE_COMMON_ON), os_mem_peek(RAM_TYPE_COMMON_OFF));

            APP_PRINT_TRACE3("listen_saveing_state:%d, seq_num %d, len %d",
                             listen_saveing_state,
                             param->a2dp_stream_data_ind.seq_num,
                             param->a2dp_stream_data_ind.len);
            listen_save_db.cur_seq_num = param->a2dp_stream_data_ind.seq_num;
            if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (listen_saveing_state == LISTEN_SAVE_DOING)
                    {
                        app_listen_save_pri_sync_pkt_info(param);
                        app_listen_save_local_save_write(param);
                    }
                }
                else if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) && \
                         ((listen_saveing_state == LISTEN_SAVE_DOING) || (listen_saveing_state == LISTEN_SAVE_DONE)))
                {
                    app_listen_save_sec_sync_pkt_info(param);
                }
            }
            else if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
                     (listen_saveing_state == LISTEN_SAVE_DOING))
            {
                app_listen_save_local_save_write(param);
                if (p_sync_pkt->end_flag == 1)
                {
                    listen_saveing_state = LISTEN_SAVE_DONE;
                    app_start_timer(&timer_idx_save_end, "save_end",
                                    listen_save_timer_id, APP_TIMER_SAVE_END, 0, false,
                                    30);
                }
            }
            if (listen_saveing_state == LISTEN_SAVE_START)
            {
                listen_saveing_state = LISTEN_SAVE_DOING;
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                    (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    uint8_t attr = BT_AVRCP_ELEM_ATTR_TITLE;
                    bt_avrcp_get_element_attr_req(param->a2dp_stream_data_ind.bd_addr, 1, &attr);
                    bt_avrcp_get_play_status_req(param->a2dp_stream_data_ind.bd_addr); // get songs time
                }
            }
        }
        break;
#if 0
    case BT_EVENT_A2DP_STREAM_OPEN:
        {
        }
        break;
    case BT_EVENT_A2DP_STREAM_START_RSP:
        {
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
        }
        break;
#endif
    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_listen_save_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_listen_save_mode_enter(void)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    APP_PRINT_TRACE0("app_listen_save_mode_enter");
    listen_save_db.pri_flag = false;
    listen_save_db.sec_flag = false;

    app_sd_card_power_down_disable(APP_SD_POWER_DOWN_ENTER_CHECK_LISTEN_SAVE);
    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            uint8_t buf = 0;
            app_listen_save_relay_async_msg(LISTEN_SAVE_REMOTE_MSG_CREATE_FILE_SYNC, (uint8_t *)&buf,
                                            sizeof(buf));
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        res = app_listen_save_relay_create_handle();
        if (res == LISTEN_SAVE_SUCCESS)
        {
            listen_saveing_state = LISTEN_SAVE_START;
        }
    }
}

void app_listen_save_mode_quit(void)
{
    APP_PRINT_TRACE0("app_listen_save_mode_quit");
    p_sync_pkt->end_flag = 1;
    app_sd_card_power_down_enable(APP_SD_POWER_DOWN_ENTER_CHECK_LISTEN_SAVE);
}

static void app_listen_save_pri_sync_pkt_info(void *buf)
{
    T_BT_EVENT_PARAM *param = buf;
    p_sync_pkt->pkt_info[p_sync_pkt->pkt_num].frame_num = param->a2dp_stream_data_ind.frame_num;
    p_sync_pkt->pkt_info[p_sync_pkt->pkt_num].len = param->a2dp_stream_data_ind.len;
    p_sync_pkt->pkt_info[p_sync_pkt->pkt_num].seq_num = param->a2dp_stream_data_ind.seq_num;
    p_sync_pkt->pkt_num++;
    if ((p_sync_pkt->pkt_num == SYNC_PKT_NUM) || (p_sync_pkt->end_flag == 1))
    {
        app_listen_save_relay_async_msg(LISTEN_SAVE_REMOTE_MSG_FRAME_INFO_SYNC,
                                        (uint8_t *)p_sync_pkt, sizeof(T_LISTEN_SAVE_PRI_PKT_INFO));
        p_sync_pkt->pkt_num = 0;
        if (p_sync_pkt->end_flag == 1)
        {
            listen_saveing_state = LISTEN_SAVE_DONE;
            app_start_timer(&timer_idx_save_end, "save_end",
                            listen_save_timer_id, APP_TIMER_SAVE_END, 0, false,
                            60);
        }
    }
}

static uint8_t app_listen_save_local_save_write(void *buf)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    T_BT_EVENT_PARAM *param = buf;
    // save to buf
    RTK_PKT_HEADER pkt_header;
    memset((uint8_t *)&pkt_header, 0, sizeof(RTK_PKT_HEADER));
    pkt_header.syncword = RTK_PKT_HEAD_SYNCWORD;
    pkt_header.pktFrameNum = param->a2dp_stream_data_ind.frame_num;
    pkt_header.pktIsInValid = 0;
    pkt_header.pktLength = param->a2dp_stream_data_ind.len;
    memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, (uint8_t *)&pkt_header,
           sizeof(RTK_PKT_HEADER));
    listen_save_db.buf_used_size += sizeof(RTK_PKT_HEADER);
    memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size,
           param->a2dp_stream_data_ind.payload,
           param->a2dp_stream_data_ind.len);
    listen_save_db.buf_used_size += param->a2dp_stream_data_ind.len;
    listen_save_db.toal_frame_num++;
    APP_PRINT_TRACE1("app_listen_save_local_save_write buf_used_size 0x%x",
                     listen_save_db.buf_used_size);
    if ((listen_save_db.buf_used_size > LISTEN_SAVE_WRITE_LEVEL) ||
        (listen_saveing_state == LISTEN_SAVE_DONE))
    {
        res = app_listen_save_write_file(listen_save_db.p_temp_buf, listen_save_db.buf_used_size);
    }
    return res;
}

static uint8_t app_listen_save_sec_sync_pkt_info(void *buf)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    T_BT_EVENT_PARAM *param = buf;
    uint16_t x_seq = p_wait_pkt->pkt_info[0].seq_num;
    uint16_t y_seq = listen_save_db.cur_seq_num;
    if ((p_wait_pkt->pkt_num != 0) && (app_listen_save_cacu_diff_u16(x_seq, y_seq) < 0))
    {
        do
        {
            app_listen_save_repack(listen_save_db.file_type, p_wait_pkt->pkt_info[0]);
            app_listen_save_wait_pkt_queue_move_one_pkt();
            x_seq = p_wait_pkt->pkt_info[0].seq_num;
        }
        while (p_wait_pkt->pkt_num && (app_listen_save_cacu_diff_u16(x_seq, y_seq) < 0));
    }
    if ((p_wait_pkt->pkt_num != 0) && (p_wait_pkt->pkt_info[0].seq_num == listen_save_db.cur_seq_num))
    {
        APP_PRINT_TRACE1("app_listen_save_sec_sync_pkt_info wait_p ktnum %d", p_wait_pkt->pkt_num);
        app_listen_save_local_save_write(param);
        app_listen_save_wait_pkt_queue_move_one_pkt();
    }
    else
    {
        // put to media buffer pool
        T_LISTEN_SAVE_SYNC_INFO pkt_header;
        pkt_header.frame_num = param->a2dp_stream_data_ind.frame_num;
        pkt_header.len = param->a2dp_stream_data_ind.len;
        pkt_header.seq_num = param->a2dp_stream_data_ind.seq_num;
        listen_save_buffer_write(param->a2dp_stream_data_ind.payload,
                                 param->a2dp_stream_data_ind.len, &pkt_header);
    }

    return res;
}

static uint8_t app_listen_save_write_file(uint8_t *buf, uint16_t len)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    uint32_t wrd_len;
    uint32_t write_result = FR_OK;
    APP_PRINT_TRACE2("app_listen_save_write_file: file_total_len:0x%x, len 0x%x ",
                     listen_save_db.file_total_len, len);
    audio_fs_set_file_offset(listen_save_fs_handle, listen_save_db.file_total_len);
    write_result = audio_fs_write(listen_save_fs_handle, buf, len, &wrd_len);
    if (write_result == FR_OK)
    {
        listen_save_db.file_total_len += len;
        listen_save_db.buf_used_size = 0;
        memset(listen_save_db.p_temp_buf, 0, LISTEN_SAVE_BUF_SIZE);
        res = LISTEN_SAVE_SUCCESS;
    }
    else
    {
        APP_PRINT_ERROR0("app_listen_save_write_file FAIL");
        res = LISTEN_SAVE_WRITE_FILE_ERROR;
    }
    return res;
}

static uint8_t app_listen_save_write_file_header(void)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    LISTEN_SAVE_FILE_HEADER_STRUCT file_header;
    uint8_t offset;
    uint32_t wrd_len;
    uint32_t write_result = FR_OK;

    memcpy((uint8_t *)&file_header, (uint8_t *)g_file_header, sizeof(g_file_header));
    offset = sizeof(g_file_header);
    memcpy((uint8_t *)&file_header + offset, g_attr0_header, sizeof(g_attr0_header));
    offset = sizeof(g_file_header) + sizeof(g_attr0_header);
    uint8_t sbc_attr0_data[SBC_ATTR0_LEN] = {SBC_ATTR0_CONTENT};
    uint8_t latm_attr0_data[LATM_ATTR0_LEN] = {LATM_ATTR0_CONTENT};
    if (listen_save_db.file_type == AUDIO_FORMAT_TYPE_SBC)
    {
        memcpy((uint8_t *)&file_header + offset, sbc_attr0_data, sizeof(sbc_attr0_data));
    }
    else if (listen_save_db.file_type == AUDIO_FORMAT_TYPE_AAC)
    {
        memcpy((uint8_t *)&file_header + offset, latm_attr0_data, sizeof(latm_attr0_data));
    }
    offset = sizeof(g_file_header) + sizeof(g_attr0_header) + sizeof(sbc_attr0_data);
    memcpy((uint8_t *)&file_header + offset, g_attr1_header, sizeof(g_attr1_header));
    file_header.role = app_cfg_nv.bud_role;
    file_header.file_total_len = listen_save_db.file_total_len;
    file_header.toal_frame_num = listen_save_db.toal_frame_num;
    APP_PRINT_TRACE2("app_listen_save_write_file_header file_total_len 0x%x, toal_frame_num: 0x%x",
                     file_header.file_total_len, file_header.toal_frame_num);
    audio_fs_set_file_offset(listen_save_fs_handle, 0);
    write_result = audio_fs_write(listen_save_fs_handle, (uint8_t *)&file_header,
                                  sizeof(LISTEN_SAVE_FILE_HEADER_STRUCT), &wrd_len);
    if (write_result != FR_OK)
    {
        res = LISTEN_SAVE_WRITE_FILE_ERROR;
    }
    return res;
}

static uint8_t app_listen_save_relay_create_handle(void)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    if (listen_save_fs_handle != NULL)
    {
        res = audio_fs_close(listen_save_fs_handle);
        if (res != 0)
        {
            APP_PRINT_ERROR0("listen_save_db.relay_handle Close fail");
        }
        listen_save_fs_handle = NULL;
        audio_fs_spp_delete_file((uint8_t *)TEMP_FILE_NAME_STRING, sizeof(TEMP_FILE_NAME_STRING));
    }

    listen_save_fs_handle = audio_fs_open((uint8_t *)TEMP_FILE_NAME_STRING,
                                          sizeof(TEMP_FILE_NAME_STRING),
                                          FA_CREATE_ALWAYS | FA_WRITE);
    if (listen_save_fs_handle == NULL)
    {
        res = LISTEN_SAVE_CREATE_ERROR;
        APP_PRINT_ERROR1("app_listen_save_relay_create_handle creat FAIL,res:0x%x", res);
        return res;
    }

    listen_save_db.buf_used_size = 0;
    memset(listen_save_db.p_temp_buf, 0, LISTEN_SAVE_BUF_SIZE);
    memset((uint8_t *)p_sync_pkt, 0, sizeof(T_LISTEN_SAVE_PRI_PKT_INFO));
    memset((uint8_t *)p_wait_pkt, 0, sizeof(T_LISTEN_SAVE_WAIT_PKT_INFO));
    listen_save_db.file_total_len = sizeof(LISTEN_SAVE_FILE_HEADER_STRUCT);
    listen_save_db.toal_frame_num = 0;
    res = app_listen_save_write_file_header();

    return res;
}

static void app_listen_save_relay_sec_create_result_handle(uint8_t *buf, uint16_t len)
{

    APP_PRINT_TRACE1("app_listen_save_relay_sec_create_result_handle, result:%d", *((uint8_t *)buf));
    if (*((uint8_t *)buf + 0) == LISTEN_SAVE_SUCCESS)
    {
        listen_save_db.sec_flag = true;
        if ((listen_save_db.pri_flag == true) && (listen_save_db.sec_flag == true))
        {
            APP_PRINT_TRACE0("app_listen_save_relay_sec_create_result_handle TRUE&TRUE");
            uint8_t buf = 0;
            app_listen_save_relay_sync_msg(LISTEN_SAVE_REMOTE_MSG_SAVING_SYNC, (uint8_t *)&buf, sizeof(buf),
                                           REMOTE_TIMER_HIGH_PRECISION, 40, false);
            listen_save_db.pri_flag = false;
            listen_save_db.sec_flag = false;
        }
    }
    else
    {
        uint16_t res = audio_fs_close(listen_save_fs_handle);
        if (res != 0)
        {
            APP_PRINT_ERROR1("app_listen_save_relay_sec_create_result_handle close file fail, res:0x%x", res);
        }
        listen_save_fs_handle = NULL;
        audio_fs_spp_delete_file((uint8_t *)TEMP_FILE_NAME_STRING, sizeof(TEMP_FILE_NAME_STRING));
    }
}

static void app_listen_save_repack(uint8_t file_type, T_LISTEN_SAVE_SYNC_INFO pri_pkt)
{
    RTK_PKT_HEADER pkt_header;
    memset((uint8_t *)&pkt_header, 0, sizeof(RTK_PKT_HEADER));
    pkt_header.syncword = RTK_PKT_HEAD_SYNCWORD;
    pkt_header.pktFrameNum = pri_pkt.frame_num;
    pkt_header.pktIsInValid = 1;
    pkt_header.pktLength = pri_pkt.len;
    memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, (uint8_t *)&pkt_header,
           sizeof(RTK_PKT_HEADER));
    listen_save_db.buf_used_size += sizeof(RTK_PKT_HEADER);
    if (file_type == AUDIO_FORMAT_TYPE_SBC)
    {
        uint8_t sbc_header[SBC_HEADER_LEN] = {SBC_HEADER_CONTENT};
        memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, sbc_header, sizeof(sbc_header));
    }
    else if (file_type == AUDIO_FORMAT_TYPE_AAC)
    {
        uint8_t latm_header[AAC_LATM_HEADER_LEN] = {AAC_LATM_HEADER_CONTENT};
        memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, latm_header, sizeof(latm_header));
    }
    listen_save_db.buf_used_size += pri_pkt.len;
    listen_save_db.toal_frame_num++;
    APP_PRINT_WARN3("app_listen_save_repack, seq_num:%d, buf_used_size:%d, toal_frame_num %d",
                    pri_pkt.seq_num, listen_save_db.buf_used_size, listen_save_db.toal_frame_num);
    if ((listen_save_db.buf_used_size > LISTEN_SAVE_WRITE_LEVEL) ||
        (listen_saveing_state == LISTEN_SAVE_DONE))
    {
        app_listen_save_write_file(listen_save_db.p_temp_buf, listen_save_db.buf_used_size);
    }
}
static void app_listen_save_wait_pkt_queue_move_one_pkt(void)
{
    memcpy((uint8_t *)&p_wait_pkt->pkt_info[0], (uint8_t *)&p_wait_pkt->pkt_info[1],
           sizeof(T_LISTEN_SAVE_SYNC_INFO));
    memcpy((uint8_t *)&p_wait_pkt->pkt_info[1], (uint8_t *)&p_wait_pkt->pkt_info[2],
           sizeof(T_LISTEN_SAVE_SYNC_INFO));
    memcpy((uint8_t *)&p_wait_pkt->pkt_info[2], (uint8_t *)&p_wait_pkt->pkt_info[3],
           sizeof(T_LISTEN_SAVE_SYNC_INFO));
    memset((uint8_t *)&p_wait_pkt->pkt_info[3], 0, sizeof(T_LISTEN_SAVE_SYNC_INFO));
    p_wait_pkt->pkt_num--;
}

static uint8_t app_listen_save_sec_one_pkt_check_save(T_LISTEN_SAVE_SYNC_INFO pri_pkt)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    T_MEDIA_LISTEN_SAVE_HEADER *p_peek_pkt;
    uint16_t len_tmp;
    uint8_t pool_pkt_num = 0;

    pool_pkt_num = listen_save_get_pkt_count();
    APP_PRINT_TRACE5("app_listen_save_sec_one_pkt_check_save,"
                     "pool_pkt_num%d, p_wait_pkt num: %d, pri_pkt seq_num:%d, len%d, frame_num %d",
                     pool_pkt_num, p_wait_pkt->pkt_num, pri_pkt.seq_num, pri_pkt.len, pri_pkt.frame_num);
    p_peek_pkt = listen_save_buffer_peek(0, &len_tmp, NULL);
    if (p_peek_pkt != NULL)
    {
        APP_PRINT_TRACE3("sec pool p_peek_pkt, seq %d, len %d, frame_num %d",
                         p_peek_pkt->seq, p_peek_pkt->len, p_peek_pkt->frame_num);
    }

    // clear wait queue
    if ((p_peek_pkt != NULL) && (p_wait_pkt->pkt_num != 0))
    {
        do
        {
            // miss pkt and repack to buf
            app_listen_save_repack(listen_save_db.file_type, p_wait_pkt->pkt_info[0]);
            app_listen_save_wait_pkt_queue_move_one_pkt();
        }
        while (p_wait_pkt->pkt_num != 0);
    }
    // if buffer pool first p_peek_pkt->seq > pri_pkt.seq_num repack
    if ((p_peek_pkt != NULL) && (app_listen_save_cacu_diff_u16(p_peek_pkt->seq, pri_pkt.seq_num) > 0))
    {
        app_listen_save_repack(listen_save_db.file_type, pri_pkt);
        return res;
    }

    while (p_peek_pkt != NULL)
    {
        if (p_peek_pkt->seq == pri_pkt.seq_num)
        {
            RTK_PKT_HEADER pkt_header;
            memset((uint8_t *)&pkt_header, 0, sizeof(RTK_PKT_HEADER));
            pkt_header.syncword = RTK_PKT_HEAD_SYNCWORD;
            pkt_header.pktFrameNum = p_peek_pkt->frame_num;
            pkt_header.pktIsInValid = 0;
            pkt_header.pktLength = p_peek_pkt->len;

            memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, (uint8_t *)&pkt_header,
                   sizeof(RTK_PKT_HEADER));
            listen_save_db.buf_used_size += sizeof(RTK_PKT_HEADER);
            memcpy(listen_save_db.p_temp_buf + listen_save_db.buf_used_size, p_peek_pkt->data, p_peek_pkt->len);
            listen_save_db.buf_used_size += p_peek_pkt->len;
            listen_save_db.toal_frame_num++;
            listen_save_buffer_flush(1, NULL);
            if ((listen_save_db.buf_used_size > LISTEN_SAVE_WRITE_LEVEL) ||
                (listen_saveing_state == LISTEN_SAVE_DONE))
            {
                res = app_listen_save_write_file(listen_save_db.p_temp_buf, listen_save_db.buf_used_size);
            }
            break;
        }
        else // maybe miss pkt
        {
            APP_PRINT_WARN0("app_listen_save_sec_one_pkt_check_save flush and next");
            p_peek_pkt = p_peek_pkt->p_next;
            listen_save_buffer_flush(1, NULL);
        }
    }
    if (p_peek_pkt == NULL)
    {
        if (p_wait_pkt->pkt_num > 3)
        {
            // wait queue first pkt is miss pkt and repack to buf
            app_listen_save_repack(listen_save_db.file_type, p_wait_pkt->pkt_info[0]);
            app_listen_save_wait_pkt_queue_move_one_pkt();
        }
        // put pri_pkt to queue last pkt
        memcpy((uint8_t *)&p_wait_pkt->pkt_info[p_wait_pkt->pkt_num], (uint8_t *)&pri_pkt,
               sizeof(T_LISTEN_SAVE_SYNC_INFO));
        p_wait_pkt->pkt_num++;
        APP_PRINT_TRACE4("wait-queue s_wait_pkt-pkt_num %d,seq_num:%d, seq_num:%d, seq_num:%d",
                         p_wait_pkt->pkt_num,
                         p_wait_pkt->pkt_info[0].seq_num,
                         p_wait_pkt->pkt_info[1].seq_num,
                         p_wait_pkt->pkt_info[2].seq_num);
    }
    return res;
}

static void app_listen_save_relay_sec_check_save_handle(uint8_t *buf, uint16_t len)
{
    T_LISTEN_SAVE_PRI_PKT_INFO pri_pkt;
    uint8_t i = 0;
    memcpy((uint8_t *)&pri_pkt, (uint8_t *)buf, sizeof(T_LISTEN_SAVE_PRI_PKT_INFO));
    APP_PRINT_TRACE7("app_listen_save_relay_sec_check_save_handle,"
                     "pkt_num %d,seq_num:%d - %d,seq_num:%d - %d,seq_num:%d - %d",
                     pri_pkt.pkt_num,
                     pri_pkt.pkt_info[0].seq_num, pri_pkt.pkt_info[0].len,
                     pri_pkt.pkt_info[1].seq_num, pri_pkt.pkt_info[1].len,
                     pri_pkt.pkt_info[2].seq_num, pri_pkt.pkt_info[2].len);

    if ((pri_pkt.pkt_num > SYNC_PKT_NUM) || (pri_pkt.pkt_num == 0))
    {
        return;
    }

    i = 0;
    while (pri_pkt.pkt_num)
    {
        if ((pri_pkt.end_flag == 1) && (pri_pkt.pkt_num == 1)) // last pkt
        {
            listen_saveing_state = LISTEN_SAVE_DONE;
        }
        app_listen_save_sec_one_pkt_check_save(pri_pkt.pkt_info[i]);
        i++;
        pri_pkt.pkt_num--;
    }
}

static uint8_t app_listen_save_fs_rename(T_AUDIO_FS_HANDLE handle, uint16_t char_id,
                                         uint8_t *pFileName, uint16_t nameLen)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    char name_tail[] = ".RTKS";
    uint8_t *file_name = NULL;
    uint8_t *unicode_name = NULL;
    uint8_t file_name_len;
    uint8_t unicode_name_len;

    switch (char_id)
    {
    case UTF_8:
        file_name_len = nameLen + sizeof(name_tail);
        file_name = (uint8_t *)malloc(file_name_len);
        if (file_name == NULL)
        {
            res = LISTEN_SAVE_SYSTEM_ERROR;
            break;
        }
        memset(file_name, 0, file_name_len);
        memcpy(file_name, pFileName, nameLen);
        memcpy(file_name + nameLen, (uint8_t *)name_tail, sizeof(name_tail));
        if ((unicode_name_len = get_unicode_bytes(file_name, file_name_len)) == 0)
        {
            res = LISTEN_SAVE_SYSTEM_ERROR;
            free(file_name);
            break;
        }
        unicode_name = (uint8_t *)malloc(unicode_name_len);
        if (unicode_name == NULL)
        {
            res = LISTEN_SAVE_SYSTEM_ERROR;
            free(file_name);
            break;
        }
        memset(unicode_name, 0, unicode_name_len);
        APP_PRINT_TRACE1("app_listen_save_fs_rename %b", TRACE_BINARY(file_name_len, (uint8_t *)file_name));
        if ((res = utf8_to_unicode(file_name, file_name_len, unicode_name)) != 0)
        {
            res = LISTEN_SAVE_SYSTEM_ERROR;
            free(file_name);
            free(unicode_name);
            break;
        }
        res = audio_fs_rename(handle, unicode_name, unicode_name_len);
        free(file_name);
        free(unicode_name);
        break;


    default:
        res = LISTEN_SAVE_SYSTEM_ERROR;
        break;
    }

    return res;
}
// x > y    1-0xFFFF return 2; 0xFFFF - 1 return -2; 1-1 return 0
static int app_listen_save_cacu_diff_u16(uint16_t x, uint16_t y)
{
    int seq_diff;
    if (x > y)
    {
        if (x - y > 0x7fff) //0xff-0x00
        {
            seq_diff = -((y + 0x10000) - x);
        }
        else
        {
            seq_diff = x - y;
        }
    }
    else
    {
        if (y - x > 0x7fff) //0xff-0x00
        {
            seq_diff = x + 0x10000 - y ;
        }
        else
        {
            seq_diff = x - y;
        }
    }
    return seq_diff;
}

static void app_listen_save_file_end_handle(void)
{
    uint8_t res = LISTEN_SAVE_SUCCESS;
    bool delete_file_flag = false;

    listen_saveing_state = LISTEN_SAVE_END;
    APP_PRINT_TRACE1("app_listen_save_file_end_handle,listen_saveing_state: %d", listen_saveing_state);
    app_listen_save_write_file_header();

    if (listen_save_fs_handle != NULL)
    {
        res = app_listen_save_fs_rename(listen_save_fs_handle,
                                        p_listen_save_file->character_id,
                                        ((uint8_t *)p_listen_save_file + sizeof(LISTEN_SAVE_FILE_STRUCT) - 1),
                                        p_listen_save_file->name_len);
        if (res != 0)
        {
            APP_PRINT_TRACE0("app_listen_save_file_end_handle rename fail");
            delete_file_flag = true;
        }
        else
        {
            res = audio_fs_add_file(listen_save_fs_handle, 0);
            if (res != 0)
            {
                APP_PRINT_ERROR1("app_listen_save_file_end_handle add name fail res:0x%x", res);
            }
        }
        res = audio_fs_close(listen_save_fs_handle);
        if (res != 0)
        {
            APP_PRINT_ERROR0("app_listen_save_file_end_handle Close fail");
        }
        listen_save_fs_handle = NULL;
        if (delete_file_flag)
        {
            audio_fs_spp_delete_file((uint8_t *)TEMP_FILE_NAME_STRING, sizeof(TEMP_FILE_NAME_STRING));
        }
    }
}

static void app_listen_save_relay_sec_file_info_handle(uint8_t *buf, uint16_t len)
{
    if (p_listen_save_file != NULL)
    {
        free(p_listen_save_file);
    }
    p_listen_save_file = malloc(len);
    if (p_listen_save_file == NULL)
    {
        return;
    }
    memcpy((uint8_t *)p_listen_save_file, buf, len);
    APP_PRINT_TRACE4("app_listen_save_relay_sec_file_info_handle, character_id %d, name_len %d, %d, %b",
                     p_listen_save_file->character_id,
                     p_listen_save_file->name_len,
                     sizeof(LISTEN_SAVE_FILE_STRUCT),
                     TRACE_BINARY(p_listen_save_file->name_len,
                                  ((uint8_t *)p_listen_save_file + sizeof(LISTEN_SAVE_FILE_STRUCT) - 1)));

}



void app_listen_save_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                 T_REMOTE_RELAY_STATUS status)
{
    uint8_t res;
    APP_PRINT_TRACE4("app_listen_save_relay_cback: msg_type 0x%04x, status %d, len:%d, bud_role:%d",
                     msg_type, status, len, app_cfg_nv.bud_role);

    switch (msg_type)
    {
#if 0
    case LISTEN_SAVE_REMOTE_MSG_START_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
            status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
        {
            listen_saveing_state = LISTEN_SAVE_START;
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                APP_PRINT_TRACE1("LISTEN_SAVE_REMOTE_MSG_SEC_RESULT_SYNC,res:%d", res);
                remote_async_msg_relay(listen_save_db.relay_handle, LISTEN_SAVE_REMOTE_MSG_SEC_RESULT_SYNC,
                                       (uint8_t *)&res, sizeof(res), false);
            }
        }
        break;
#endif

    case LISTEN_SAVE_REMOTE_MSG_CREATE_FILE_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
            status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
        {
            res = app_listen_save_relay_create_handle();
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_listen_save_relay_async_msg(LISTEN_SAVE_REMOTE_MSG_SEC_CREATE_RESULT_SYNC,
                                                (uint8_t *)&res, sizeof(res));
            }
            else if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) && (res == LISTEN_SAVE_SUCCESS))
            {
                listen_save_db.pri_flag = true;
                if ((listen_save_db.pri_flag == true) && (listen_save_db.sec_flag == true))
                {
                    APP_PRINT_TRACE0("app_listen_save_relay_create_handle TRUE&TRUE");
                    uint8_t buf = 0;
                    app_listen_save_relay_sync_msg(LISTEN_SAVE_REMOTE_MSG_SAVING_SYNC, (uint8_t *)&buf, sizeof(buf),
                                                   REMOTE_TIMER_HIGH_PRECISION, 60, false);
                    listen_save_db.pri_flag = false;
                    listen_save_db.sec_flag = false;
                }
            }
        }
        break;

    case LISTEN_SAVE_REMOTE_MSG_SEC_CREATE_RESULT_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
            status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_listen_save_relay_sec_create_result_handle(buf, len);
            }
        }
        break;

    case LISTEN_SAVE_REMOTE_MSG_SAVING_SYNC:
        if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || //03
            status == REMOTE_RELAY_STATUS_SYNC_EXPIRED || //04
            status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED) //08
        {
            listen_saveing_state = LISTEN_SAVE_START;
        }
        break;

    case LISTEN_SAVE_REMOTE_MSG_FRAME_INFO_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_listen_save_relay_sec_check_save_handle(buf, len);
            }
        }
        break;

    case LISTEN_SAVE_REMOTE_MSG_SAVE_END_SYNC:
        if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || //03
            status == REMOTE_RELAY_STATUS_SYNC_EXPIRED || //04
            status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED) //08
        {
            listen_saveing_state = LISTEN_SAVE_END;
            app_listen_save_file_end_handle();
        }
        break;

    case LISTEN_SAVE_REMOTE_MSG_FILE_INFO_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                // sync file name
                app_listen_save_relay_sec_file_info_handle(buf, len);
            }
        }
        break;


    case LISTEN_SAVE_REMOTE_MSG_SEC_RESULT_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
            status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
        {
            // if sec fail stop

        }
        break;

    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////
static void app_listen_save_timer_save_end_handle(void)
{
    APP_PRINT_TRACE0("app_listen_save_timer_save_end_handle");
    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            uint8_t buf = 0;
            app_listen_save_relay_sync_msg(LISTEN_SAVE_REMOTE_MSG_SAVE_END_SYNC, (uint8_t *)&buf, sizeof(buf),
                                           REMOTE_TIMER_HIGH_PRECISION, 40, false);
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        listen_saveing_state = LISTEN_SAVE_END;
        app_listen_save_file_end_handle();
    }

}
static void app_listen_save_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_SAVE_END:
        {
            app_stop_timer(&timer_idx_save_end);
            app_listen_save_timer_save_end_handle();
        }
        break;

    default:
        break;
    }
}

void app_listen_save_init(void)
{
    listen_save_db.file_type = AUDIO_FORMAT_TYPE_SBC;
    listen_save_db.pri_flag = false;
    listen_save_db.sec_flag = false;
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        listen_save_media_buffer_init();
        app_relay_cback_register(NULL, app_listen_save_parse_cback,
                                 APP_MODULE_TYPE_LISTEN_SAVE, LISTEN_SAVE_REMOTE_MSG_MAX);
    }
    bt_mgr_cback_register(app_listen_save_bt_cback);

    listen_save_db.p_temp_buf = (uint8_t *)malloc(LISTEN_SAVE_BUF_SIZE);
    if (listen_save_db.p_temp_buf == NULL)
    {
        APP_PRINT_ERROR0("listen_save_db.p_temp_buf malloc FAIL");
    }
    p_sync_pkt = malloc(sizeof(T_LISTEN_SAVE_PRI_PKT_INFO));
    memset((uint8_t *)p_sync_pkt, 0, sizeof(T_LISTEN_SAVE_PRI_PKT_INFO));
    p_wait_pkt = malloc(sizeof(T_LISTEN_SAVE_WAIT_PKT_INFO));
    memset((uint8_t *)p_wait_pkt, 0, sizeof(T_LISTEN_SAVE_WAIT_PKT_INFO));
    app_timer_reg_cb(app_listen_save_timeout_cb, &listen_save_timer_id);
}
#endif
