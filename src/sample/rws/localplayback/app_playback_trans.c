/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_playback_trans.c
   * @brief     Source file for using spp updata playback file
   * @author    Lemon
   * @date      2020-05-01
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#if F_APP_LOCAL_PLAYBACK_SUPPORT
/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <string.h>
#include <bt_types.h>
#include "trace.h"
#include "app_main.h"
#include "app_cmd.h"
#include "app_mmi.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "os_msg.h"
#include "os_task.h"
#include "rtl876x.h"
#include "app_bt_policy_api.h"
#include "app_report.h"
#include "stdlib.h"
#include "patch_header_check.h"
#include "app_playback_trans.h"
#include "audio_fs.h"
#include "ff.h"
#include "gap_conn_le.h"
#include "playback_service.h"
#include "app_playback.h"
#include "app_sniff_mode.h"
#include "app_io_msg.h"

#define APP_PLAYBACK_SD_ACTIVE_TIME_S               60
/** @defgroup  APP_PLAYBACK_SERVICE APP PLAYBACK handle
    * @brief APP PLAYBACK Service to implement PLAYBACK feature
    * @{
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup APP_PLAYBACK_Exported_Variables APP PLAYBACK Exported Variables
    * @brief
    * @{
    */
static PB_TRANS_FUNCTION_STRUCT pb_trans_struct;

T_AUDIO_FS_HANDLE app_playback_write_fs_handle = NULL;

/** @brief  PLAYBACK timer callback */
typedef enum
{
    APP_TIMER_PB_TRANS_FILE,
    APP_TIMER_PB_TRANS_UP,
    APP_TIMER_PB_TRANS_SD_ACTIVE,
} T_APP_PLAYBACK_TRANS_TIMER;

static uint8_t timer_idx_pb_trans_up = 0;
static uint8_t timer_idx_pb_trans_file = 0;
static uint8_t timer_idx_pb_trans_sd_active = 0;
static uint8_t app_playback_trans_timer_id = 0;
/** End of APP_PLAYBACK_Exported_Variables
    * @}
    */

/*============================================================================*
 *                              Private Functions
 *============================================================================*/
/** @defgroup APP_PLAYBACK_Exported_Functions APP PLAYBACK service Exported Functions
    * @brief
    * @{
    */
static void app_playback_trans_sd_active_start_timer(uint8_t time_s);
static uint8_t app_playback_trans_stop_and_del_file(void);
static void app_playback_trans_service_prepare_send_notify(uint8_t conn_id, uint16_t event_id,
                                                           uint16_t len, uint8_t *data);
static void app_playback_trans_write_send_msg(char event);

/**
    * @brief  Reset local variables
    * @return void
    */
static void app_playback_trans_clear_local(void)
{
    APP_PRINT_TRACE0("app_playback_trans_clear_local");
    pb_trans_struct.file_total_length = 0;
    pb_trans_struct.file_id = 0;
    pb_trans_struct.cur_offset = 0;
    pb_trans_struct.send_cnt = 0;
    pb_trans_struct.file_list_type = PLAYBACK_HEAD_BIN;
    pb_trans_struct.is_trans_pb_process = false;

    app_sniff_mode_b2s_enable(pb_trans_struct.bd_addr, SNIFF_DISABLE_MASK_TRANS);

    pb_trans_struct.local_seq = 0;
    if (pb_trans_struct.p_temp_buf_A_head != NULL)
    {
        free(pb_trans_struct.p_temp_buf_A_head);
        pb_trans_struct.p_temp_buf_A_head = NULL;
    }

    if (pb_trans_struct.p_temp_buf_B_head != NULL)
    {
        free(pb_trans_struct.p_temp_buf_B_head);
        pb_trans_struct.p_temp_buf_B_head = NULL;
    }
    pb_trans_struct.buf_A_is_full = false;
    pb_trans_struct.buf_B_is_full = false;
    pb_trans_struct.temp_buf_A_used_size = 0;
    pb_trans_struct.temp_buf_B_used_size = 0;
    pb_trans_struct.buf_pingpang_flag = false;

    app_stop_timer(&timer_idx_pb_trans_up); // ble use
    app_stop_timer(&timer_idx_pb_trans_file);

    if (app_playback_write_fs_handle != NULL)
    {
        uint16_t res = audio_fs_close(app_playback_write_fs_handle);
        if (res != 0)
        {
            res = PB_TRANS_RET_CLOSE_ERROR;
        }
        app_playback_write_fs_handle = NULL;
    }
}

/**
    * @brief  Reset local variables
    * @return void
    */
static void app_playback_trans_error_clear_local(void)
{
    app_playback_trans_clear_local();
}

/**
    * @brief    Used to get device information
    * @param    p_data    point of device info data
    * @return   void
    */
void app_playback_trans_get_device_info(PLAYBACK_DEVICE_INFO *p_deviceinfo)
{
    if (p_deviceinfo == NULL)
    {
        return;
    }

    memset(p_deviceinfo, 0, sizeof(PLAYBACK_DEVICE_INFO));
    p_deviceinfo->ic_type = IC_TYPE;
    p_deviceinfo->pkt_size          = PLAYBACK_PKT_SIZE;
    p_deviceinfo->buf_check_size    = PLAYBACK_BUF_CHECK_SIZE;
    p_deviceinfo->protocol_ver      = PLAYBACK_PROTOCOL_VERSION;
    p_deviceinfo->mode              = 0x00;

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        p_deviceinfo->mode |= PLAYBACK_MODE_COUPLE | PLAYBACK_MODE_COUPLE_PRI;
    }
    else if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        p_deviceinfo->mode |= PLAYBACK_MODE_COUPLE | PLAYBACK_MODE_COUPLE_SEC;
    }
    else if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        p_deviceinfo->mode |= PLAYBACK_MODE_SINGLE;
    }
    p_deviceinfo->song_format_type = PB_FORMAT_TYPE_DEFAULT;
}

/**
    * @brief    Used to send playback file list and play list to host
    * @param    p       point of rx cmd
    * @param    p_data    [out] point of file one packet
    * @param    length    [out] read file length
    * @return   void
    */
static uint8_t app_playback_trans_list_data_info(uint8_t *p, uint8_t *p_data)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;
    uint8_t file_type = *p;
    uint16_t file_crc;

    app_playback_trans_clear_local();

    pb_trans_struct.file_list_type = file_type;
    if (PLAYBACK_NAME_BIN == file_type)
    {
        pb_trans_struct.file_total_length = audio_fs_get_name_list_size();
        file_crc = 0x1234;
    }
    else if (PLAYBACK_HEAD_BIN == file_type)
    {
        pb_trans_struct.file_total_length = audio_fs_get_header_list_size();
        file_crc = 0x5678;
    }
    else
    {
        res = PB_TRANS_RET_READ_ERROR;
        return res;
    }

    APP_PRINT_TRACE2("app_playback_trans_list_data_info:crc:0x%x,file_total_length:0x%x", file_crc,
                     pb_trans_struct.file_total_length);
    // send start fiel crc and length
    p_data[0] = PLAYBACK_TRANS_START;
    LE_UINT16_TO_ARRAY((uint8_t *)&p_data[1], file_crc);
    LE_UINT32_TO_ARRAY((uint8_t *)&p_data[3], pb_trans_struct.file_total_length);

    if (pb_trans_struct.p_temp_buf_A_head != NULL)
    {
        free(pb_trans_struct.p_temp_buf_A_head);
        pb_trans_struct.p_temp_buf_A_head = NULL;
    }

    pb_trans_struct.p_temp_buf_A_head = (uint8_t *)malloc(PLAYBACK_PKT_SIZE);
    if (pb_trans_struct.p_temp_buf_A_head == NULL)
    {
        res = PB_TRANS_RET_OPERATION_ERROR;
    }

    return res;
}

/**
    * @brief    Used to send playback file list and play list data to host
    * @return   void
    */
void app_playback_trans_list_data_handle(void)
{
//    uint8_t res = PB_TRANS_RET_SUCCESS;
    uint16_t fs_res = 0;
    uint16_t send_data_len = 0;
    uint16_t data_pkt_len = 0;
    uint16_t playback_read_frame_size = PLAYBACK_READ_FRAME_SIZE;

    uint8_t *p_data = pb_trans_struct.p_temp_buf_A_head;
    uint8_t *p_send_data = p_data + PLAYBACK_LIST_HEAD_LEN + PLAYBACK_LIST_READ_LENTH_LEN;

    if (pb_trans_struct.cur_offset < pb_trans_struct.file_total_length)
    {
        if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
        {
            le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &playback_read_frame_size,
                              pb_trans_struct.id.ble_conn_id);
            playback_read_frame_size -= 10; // Reserve redundancy some bytes
            playback_read_frame_size = playback_read_frame_size & 0xF0;
        }

        //set pkt header
        if ((pb_trans_struct.cur_offset + playback_read_frame_size) <
            pb_trans_struct.file_total_length) //continue
        {
            send_data_len = playback_read_frame_size;
            p_data[0] = PLAYBACK_TRANS_CONTINUE;
            LE_UINT16_TO_ARRAY((uint8_t *)&p_data[1], send_data_len);
        }
        else //end
        {
            send_data_len = pb_trans_struct.file_total_length - pb_trans_struct.cur_offset;
            p_data[0] = PLAYBACK_TRANS_END;
            LE_UINT16_TO_ARRAY((uint8_t *)&p_data[1], send_data_len);
            //may be clear some value
        }
        uint32_t read_len;
        APP_PRINT_TRACE2("app_playback_trans_list_data_handle: filetype:0x%x,send_data_len:0x%x",
                         pb_trans_struct.file_list_type, send_data_len);
        if (PLAYBACK_NAME_BIN == pb_trans_struct.file_list_type)
        {
            fs_res = audio_fs_read_name_list(pb_trans_struct.cur_offset, p_send_data, send_data_len, &read_len);
        }
        else if (PLAYBACK_HEAD_BIN == pb_trans_struct.file_list_type)
        {
            fs_res = audio_fs_read_header_list(pb_trans_struct.cur_offset, p_send_data, send_data_len,
                                               &read_len);
        }

        if ((fs_res == 0) || (fs_res == FS_END_OF_FILE))
        {
//            res = PB_TRANS_RET_SUCCESS;
        }
        else
        {
            APP_PRINT_ERROR1("app_playback_trans_list_data_handle read error,fs_res:%d", fs_res);
//            res = PB_TRANS_RET_READ_ERROR;
            app_playback_trans_error_clear_local();
            return;
        }

        data_pkt_len = send_data_len + PLAYBACK_LIST_HEAD_LEN + PLAYBACK_LIST_READ_LENTH_LEN;

        if (pb_trans_struct.cur_offset != pb_trans_struct.file_total_length)
        {
            if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_SPP_MODE)
            {
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_GET_LIST_DATA, pb_trans_struct.id.spp_idx,
                                 p_data, data_pkt_len);
            }
            else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
            {
                app_playback_trans_service_prepare_send_notify(pb_trans_struct.id.ble_conn_id,
                                                               EVENT_PLAYBACK_GET_LIST_DATA,
                                                               data_pkt_len, (uint8_t *)p_data);
            }
        }
        else
        {
            //close file
            APP_PRINT_ERROR0("app_playback_trans_list_data_handle: FILE END");
        }
        pb_trans_struct.send_cnt++;
        pb_trans_struct.cur_offset = pb_trans_struct.cur_offset + send_data_len;

        if (pb_trans_struct.cur_offset != pb_trans_struct.file_total_length)
        {
            if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
            {
                app_start_timer(&timer_idx_pb_trans_up, "pb_trans_up",
                                app_playback_trans_timer_id, APP_TIMER_PB_TRANS_UP, 0, false,
                                100);
            }
        }
        else
        {
            if (pb_trans_struct.p_temp_buf_A_head != NULL)
            {
                free(pb_trans_struct.p_temp_buf_A_head);
                pb_trans_struct.p_temp_buf_A_head = NULL;
            }
        }
        APP_PRINT_TRACE2("app_playback_trans_list_data_handle: file_total_length:%d,cur_offset:%d",
                         pb_trans_struct.file_total_length, pb_trans_struct.cur_offset);
    }
}

/**
    * @brief    get songs info for playback and create file.
    * @param    *p_data   point of input data
    * @return   void
    */
static uint8_t app_playback_trans_get_create_file_handle(uint8_t *p_data)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;

    uint32_t file_total_len;
    uint16_t file_name_len;
    uint8_t len_offset = 0;

    LE_ARRAY_TO_UINT16(file_name_len, p_data);
    uint8_t *file_name = p_data + 2;
    // total file length
    len_offset = 2 + file_name_len;

    LE_ARRAY_TO_UINT32(file_total_len, (uint8_t *)(p_data + len_offset));

    if (1 == app_db.sd_playback_switch)
    {
        app_mmi_handle_action(MMI_SD_PLAYBACK_SWITCH);
    }
    app_playback_trans_clear_local();
    APP_PRINT_TRACE3("app_playback_trans_get_create_file_handle: "\
                     "file_name_len:0x%x, file_total_len:0x%x,filename:%b",
                     file_name_len, file_total_len, TRACE_BINARY(file_name_len, file_name));

    app_playback_write_fs_handle = audio_fs_open(file_name, file_name_len, FA_CREATE_ALWAYS | FA_WRITE);
    if (app_playback_write_fs_handle == NULL)
    {
        res = PB_TRANS_RET_CREAT_ERROR;
        APP_PRINT_ERROR1("app_playback_trans_get_create_file_handle creat FAIL,res:0x%x", res);
        return res;
    }

//    res = PB_TRANS_RET_SUCCESS;
//    APP_PRINT_TRACE6("end mem, data on %d, data off %d, buf on %d, buf off %d, comm on %d, comm off %d",
//                    os_mem_peek(RAM_TYPE_DATA_ON), os_mem_peek(RAM_TYPE_DATA_OFF),
//                    os_mem_peek(RAM_TYPE_BUFFER_ON), os_mem_peek(RAM_TYPE_BUFFER_OFF),
//                    os_mem_peek(RAM_TYPE_COMMON_ON), os_mem_peek(RAM_TYPE_COMMON_OFF));
    if (pb_trans_struct.p_temp_buf_A_head != NULL)
    {
        free(pb_trans_struct.p_temp_buf_A_head);
        pb_trans_struct.p_temp_buf_A_head = NULL;
    }

    if (pb_trans_struct.p_temp_buf_B_head != NULL)
    {
        free(pb_trans_struct.p_temp_buf_B_head);
        pb_trans_struct.p_temp_buf_B_head = NULL;
    }

    pb_trans_struct.p_temp_buf_A_head = (uint8_t *)malloc(PLAYBACK_BUF_CHECK_SIZE);
    pb_trans_struct.p_temp_buf_B_head = (uint8_t *)malloc(PLAYBACK_BUF_CHECK_SIZE);

    if ((pb_trans_struct.p_temp_buf_A_head == NULL) || (pb_trans_struct.p_temp_buf_B_head == NULL))
    {
        res = PB_TRANS_RET_OPERATION_ERROR;
    }

    pb_trans_struct.file_total_length = file_total_len;
    pb_trans_struct.cur_offset = 0;
    pb_trans_struct.temp_buf_A_used_size = 0;
    pb_trans_struct.temp_buf_B_used_size = 0;
    pb_trans_struct.is_trans_pb_process = true;
    app_sniff_mode_b2s_disable(pb_trans_struct.bd_addr, SNIFF_DISABLE_MASK_TRANS);

    return res;
}

/**
 * @brief  get 16bit data swapped.
 *
 * @param  val          16bit data to be swapped.
 * @return value after being swapped.
*/
static uint16_t swap_16(uint16_t val)
{
    uint16_t result;

    /* Idiom Recognition for REV16 */
    result = ((val & 0xff) << 8) | ((val & 0xff00) >> 8);

    return result;
}

/**
* @brief calculate checksum of lenth of buffer.
*
* @param  offset             offset of the image.
* @param  length             length of data.
* @param  crcValue          ret crc value point.
* @return  0 if buffer checksum calcs successfully, error line number otherwise
*/
static uint32_t app_playback_trans_checkbufcrc(uint8_t *buf, uint32_t length, uint16_t mCrcVal)
{
    uint16_t checksum16 = 0;
    uint32_t result = 0;
    uint32_t i;
    uint16_t *p16;

    p16 = (uint16_t *)buf;
    for (i = 0; i < length / 2; ++i)
    {
        checksum16 = checksum16 ^ (*p16);
        ++p16;
    }

    checksum16 = swap_16(checksum16);
    if (checksum16 != mCrcVal)
    {
        result =  __LINE__;
        goto L_Return;
    }
    return result;

L_Return:
    APP_PRINT_ERROR3("<====app_playback_trans_checkbufcrc :checksum16 0x%x, mCrcVal 0x%x result:%d",
                     checksum16, mCrcVal, result);
    return result;
}

/**
    * @brief    Handle written request on packet
    * @param    p_data     data to be written
    * @param    length     Length of value to be written
    * @return   handle result  0x01:success other:fail
    */
static uint8_t app_playback_trans_data_packet_handle(uint8_t *p_data, uint16_t length)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t host_sequence;
    uint16_t host_pkt_crc;
    uint32_t file_offset;
    uint16_t pkt_len;
    uint8_t *p_file_data;

    LE_ARRAY_TO_UINT16(host_sequence, (p_data + 0));
    LE_ARRAY_TO_UINT16(host_pkt_crc, (p_data + 2));
    LE_ARRAY_TO_UINT32(file_offset, (p_data + 4));
    LE_ARRAY_TO_UINT16(pkt_len, (p_data + 8));
    p_file_data = p_data + 10;
    APP_PRINT_TRACE5("app_playback_trans_data_packet_handle: local_sequence:0x%x, "
                     "host_sequence 0x%x, host_pkt_crc:0x%x, file_offset=0x%x, pkt_len =%d",
                     host_sequence, pb_trans_struct.local_seq, host_pkt_crc, file_offset, pkt_len);

    if (pb_trans_struct.is_trans_pb_process == false)
    {
        results = PB_TRANS_RET_OPERATION_ERROR;
        goto L_Return;
    }

    if (pb_trans_struct.buf_A_is_full && pb_trans_struct.buf_B_is_full)
    {
        results = PB_TRANS_RET_BUF_FULL_ERROR;
        goto L_Return;
    }

    if (host_sequence != pb_trans_struct.local_seq)
    {
        results = PB_TRANS_RET_SEQUENCE_ERROR;
        goto L_Return;
    }
    pb_trans_struct.local_seq++;

    APP_PRINT_TRACE6("app_playback_trans_data_packet_handle: length:0x%x, cur_offset:0x%x,"
                     "buf_used_size A:0x%x, B:0x%x, file_total_length: 0x%x, buf_pingpang_flag %d",
                     length, pb_trans_struct.cur_offset,
                     pb_trans_struct.temp_buf_A_used_size, pb_trans_struct.temp_buf_B_used_size,
                     pb_trans_struct.file_total_length, pb_trans_struct.buf_pingpang_flag);
    if (file_offset != (pb_trans_struct.cur_offset + pb_trans_struct.temp_buf_A_used_size +
                        pb_trans_struct.temp_buf_B_used_size))
    {
        results = PB_TRANS_RET_OFFSET_ERROR;
        goto L_Return;
    }

    if (app_playback_trans_checkbufcrc(p_file_data, pkt_len, host_pkt_crc))
    {
        results = PB_TRANS_RET_CRC_ERROR;
        goto L_Return;
    }

    if (pb_trans_struct.cur_offset + pkt_len + pb_trans_struct.temp_buf_A_used_size +
        pb_trans_struct.temp_buf_B_used_size > pb_trans_struct.file_total_length)
    {
        results = PB_TRANS_RET_LENTH_ERROR;
        goto L_Return;
    }
    else
    {

        if (pb_trans_struct.buf_pingpang_flag == false) // A
        {
            if (pb_trans_struct.temp_buf_A_used_size + pkt_len <= PLAYBACK_BUF_CHECK_SIZE)
            {
                memcpy(pb_trans_struct.p_temp_buf_A_head + pb_trans_struct.temp_buf_A_used_size, p_file_data,
                       pkt_len);
                pb_trans_struct.temp_buf_A_used_size += pkt_len;
            }
            else
            {
                results = PB_TRANS_RET_OPERATION_ERROR;
            }
        }
        else // B
        {
            if (pb_trans_struct.temp_buf_B_used_size + pkt_len <= PLAYBACK_BUF_CHECK_SIZE)
            {
                memcpy(pb_trans_struct.p_temp_buf_B_head + pb_trans_struct.temp_buf_B_used_size, p_file_data,
                       pkt_len);
                pb_trans_struct.temp_buf_B_used_size += pkt_len;
            }
            else
            {
                results = PB_TRANS_RET_OPERATION_ERROR;
            }
        }
    }

    if ((pb_trans_struct.buf_pingpang_flag == false) &&
        ((pb_trans_struct.temp_buf_A_used_size == PLAYBACK_BUF_CHECK_SIZE) ||
         ((pb_trans_struct.temp_buf_A_used_size + pb_trans_struct.temp_buf_B_used_size +
           pb_trans_struct.cur_offset) == pb_trans_struct.file_total_length)))
    {
        if (results == PB_TRANS_RET_SUCCESS)
        {
            pb_trans_struct.buf_A_is_full = true;
            app_playback_trans_write_send_msg(PLAYBACK_BUF_A_EVENT); // flush buf A to sd
            pb_trans_struct.buf_pingpang_flag = true;
        }
    }
    else if ((pb_trans_struct.buf_pingpang_flag == true) &&
             ((pb_trans_struct.temp_buf_B_used_size == PLAYBACK_BUF_CHECK_SIZE) ||
              ((pb_trans_struct.temp_buf_A_used_size + pb_trans_struct.temp_buf_B_used_size +
                pb_trans_struct.cur_offset) == pb_trans_struct.file_total_length)))
    {
        if (results == PB_TRANS_RET_SUCCESS)
        {
            pb_trans_struct.buf_B_is_full = true;
            app_playback_trans_write_send_msg(PLAYBACK_BUF_B_EVENT); // flush buf B to sd
            pb_trans_struct.buf_pingpang_flag = false;
        }
    }
    return results;

L_Return:
    APP_PRINT_ERROR1("app_playback_trans_data_packet_handle: results:0x%x", results);
    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_REPORT_BUFFER_CHECK, pb_trans_struct.id.spp_idx,
                     &results, sizeof(results));
    app_playback_trans_stop_and_del_file();
    app_playback_trans_error_clear_local();
    app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);

    return results;
}

uint8_t app_playback_trans_fs_write_data(char event)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;
    uint16_t write_res = 0;
    if (pb_trans_struct.is_trans_pb_process == false)
    {
        res = PB_TRANS_RET_OPERATION_ERROR;
        goto L_Return_Err;
    }

    APP_PRINT_TRACE1("app_playback_trans_fs_write_data: write flash, cur_offset: 0x%x",
                     pb_trans_struct.cur_offset);
    if (event == PLAYBACK_BUF_A_EVENT)
    {
        uint32_t wrd_len;
        write_res = audio_fs_write(app_playback_write_fs_handle,
                                   pb_trans_struct.p_temp_buf_A_head,
                                   pb_trans_struct.temp_buf_A_used_size,
                                   &wrd_len);
        if (write_res == FR_OK)
        {
            pb_trans_struct.cur_offset += pb_trans_struct.temp_buf_A_used_size;
            pb_trans_struct.temp_buf_A_used_size = 0;
            pb_trans_struct.buf_A_is_full = false;
            res = PB_TRANS_RET_BUF_A_SUCCESS;
        }
        else
        {
            res = PB_TRANS_RET_WRITE_ERROR;
            goto L_Return_Err;
        }
    }
    else if (event == PLAYBACK_BUF_B_EVENT)
    {
        uint32_t wrd_len;
        write_res = audio_fs_write(app_playback_write_fs_handle,
                                   pb_trans_struct.p_temp_buf_B_head,
                                   pb_trans_struct.temp_buf_B_used_size,
                                   &wrd_len);
        if (write_res == FR_OK)
        {
            pb_trans_struct.cur_offset += pb_trans_struct.temp_buf_B_used_size;
            pb_trans_struct.temp_buf_B_used_size = 0;
            pb_trans_struct.buf_B_is_full = false;
            res = PB_TRANS_RET_BUF_B_SUCCESS;
        }
        else
        {
            res = PB_TRANS_RET_WRITE_ERROR;
            goto L_Return_Err;
        }
    }

    return res;
L_Return_Err:
    APP_PRINT_ERROR2("app_playback_trans_fs_write_data: Fail, write_result:%d, res:0x%x", write_res,
                     res);
    return res;
}

/**
    * @brief    Valid the image
    * @param    image_id     image id
    * @return   valid result
    */
uint8_t app_playback_trans_valid_handle(uint8_t *p_date)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint32_t host_file_len;
    uint16_t host_file_crc;
    uint32_t local_file_len;
//    uint16_t local_file_crc;

    LE_ARRAY_TO_UINT32(host_file_len, p_date);
    LE_ARRAY_TO_UINT16(host_file_crc, p_date + 4);


    local_file_len = audio_fs_size(app_playback_write_fs_handle);
    APP_PRINT_TRACE4("app_playback_trans_valid_handle: "\
                     "host_file_len:0x%x, host_crc:0x%x, file_len:0x%x, results: 0x%x",
                     host_file_len, host_file_crc, local_file_len, results);
    if (host_file_len != local_file_len)
    {
        results = PB_TRANS_RET_LENTH_ERROR;
    }

    if (results == PB_TRANS_RET_SUCCESS)
    {
        results = audio_fs_add_file(app_playback_write_fs_handle, 0);
        if (results != 0)
        {
            APP_PRINT_ERROR0("app_playback_trans_valid_handle: audio_fs_add_file fail");
        }
    }

    results = audio_fs_close(app_playback_write_fs_handle);
    if (results != 0)
    {
        results = PB_TRANS_RET_CLOSE_ERROR;
    }
    else
    {
        results = PB_TRANS_RET_SUCCESS;
    }

    app_playback_write_fs_handle = NULL;
    pb_trans_struct.is_trans_pb_process = false;

    return results;
}

static uint8_t app_playback_trans_stop_and_del_file(void)
{
    uint16_t res = PB_TRANS_RET_SUCCESS;

    if (app_playback_write_fs_handle != NULL)
    {
        uint8_t *file_name;
        uint16_t name_len = audio_fs_get_filenameLen(app_playback_write_fs_handle);
        uint8_t *delete_file = (uint8_t *)malloc(name_len);
        if (delete_file != NULL)
        {
            file_name = audio_fs_get_filename(app_playback_write_fs_handle);
            memcpy(delete_file, file_name, name_len);
        }
        else
        {
            APP_PRINT_ERROR0("app_playback_trans_stop_and_del_file malloc fail");
        }

        res = audio_fs_close(app_playback_write_fs_handle);
        app_playback_write_fs_handle = NULL;
        if (res != 0)
        {
            res = PB_TRANS_RET_CLOSE_ERROR;
        }
        else if (delete_file != NULL)
        {
            res = audio_fs_spp_delete_file(delete_file, name_len);
            if (res != 0)
            {
                res = PB_TRANS_RET_DELETE_ERROR;
            }
            else
            {
                res = PB_TRANS_RET_SUCCESS;
            }
        }

        if (delete_file != NULL)
        {
            free(delete_file);
        }
    }
    return res;
}

static uint8_t app_playback_trans_cancel_handle(void)
{
    uint16_t res = PB_TRANS_RET_SUCCESS;

    res = app_playback_trans_stop_and_del_file();
    APP_PRINT_TRACE1("app_playback_trans_cancel_handle res:0x%x", res);
    app_playback_trans_clear_local();

    return res;

}

/**
    * @brief    Delete file handle
    * @param    p_date     file info
    * @return   valid result
    */
static uint8_t app_playback_trans_delete_handle(uint8_t *p_data)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;
    PB_DELETE_FILE_STRUCT trans_info;
    uint8_t *file_name;

    memcpy((uint8_t *)&trans_info, p_data, sizeof(PB_DELETE_FILE_STRUCT));
    file_name = p_data + sizeof(PB_DELETE_FILE_STRUCT);
    APP_PRINT_TRACE3("app_playback_trans_delete_handle,file_idx:%d, LEN:%d,%b", trans_info.file_idx,
                     trans_info.name_len, TRACE_BINARY(trans_info.name_len, file_name));
    res = audio_fs_delete_file(file_name, trans_info.name_len, trans_info.file_idx); // @carol
    if (res == AUDIO_FS_ERR_FILE_OPENED)
    {
        res = PB_TRANS_RET_DELETE_OPENED_ERROR;
    }
    else if (res != 0)
    {

        res = PB_TRANS_RET_DELETE_ERROR;
    }
    else
    {
        res = PB_TRANS_RET_SUCCESS;
    }
    APP_PRINT_TRACE1("app_playback_trans_delete_handle, res:0x%x", res);

    return res;
}

static uint8_t app_playback_trans_single_delete_handle(uint8_t *p_data)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;
    uint8_t old_play_state = playback_db.sd_play_state;
    res = app_playback_trans_delete_handle(p_data);
    if (PB_TRANS_RET_DELETE_OPENED_ERROR == res)
    {
        if (playback_db.sd_play_state == PLAYBACK_STATE_PLAY)
        {
            app_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
        }

        if (playback_audio_fs_handle != NULL)
        {
            if (0 != audio_fs_close(playback_audio_fs_handle))
            {
                APP_PRINT_ERROR1("close FAIL,(%p)", playback_audio_fs_handle);
            }
            playback_audio_fs_handle = NULL;
        }
        res = app_playback_trans_delete_handle(p_data);

        if (old_play_state == PLAYBACK_STATE_PLAY)
        {
            app_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
        }
    }
    return res;
}

/**
    * @brief    exit transfer handle
    * @return   valid result
    */
static void app_playback_trans_exit_trans_handle(void)
{
    APP_PRINT_TRACE0("app_playback_trans_exit_trans_handle");
    app_playback_trans_stop_and_del_file();
    app_playback_trans_clear_local();
}

/**
    * @brief    change playlist info
    * @param    event_id   event_id
    * @param    p_date     check file info
    * @return   result
    */
static uint8_t app_playback_trans_list_updata_handle(uint16_t event_id, uint8_t *p_data)
{
    uint8_t res = PB_TRANS_RET_SUCCESS;
    PB_PLAYLIST_UPDATE_STRUCT trans_info;
    uint8_t *file_name = NULL;

    memcpy((uint8_t *)&trans_info, p_data, sizeof(PB_PLAYLIST_UPDATE_STRUCT));
    file_name = (uint8_t *)malloc(trans_info.name_len + 1);
    if (file_name == NULL)
    {
        res = PB_TRANS_RET_OPERATION_ERROR;
        return res;
    }

    memset(file_name, 0, (trans_info.name_len + 1));
    memcpy(file_name, (uint8_t *)(p_data + sizeof(PB_PLAYLIST_UPDATE_STRUCT)), trans_info.name_len);
    APP_PRINT_TRACE4("app_playback_trans_list_updata_handle: pblist:0x%x,fileidx,0x%x,len%d,%b",
                     trans_info.playlist_idx, trans_info.file_idx, trans_info.name_len,
                     TRACE_BINARY(trans_info.name_len, file_name));
    res = audio_fs_update_playlist_index(file_name, trans_info.name_len, trans_info.playlist_idx,
                                         trans_info.file_idx);
    if (res != 0)
    {
        res = PB_TRANS_RET_OPERATION_ERROR;
    }
    else
    {
        res = PB_TRANS_RET_SUCCESS;
    }
    free(file_name);

    return res;
}
#if APP_PLAYBACK_TRANS_COUPLE_EN
static void app_playback_trans_relay_async_msg(uint8_t msg_type, uint8_t *buf, uint16_t len)
{
    bool ret = false;
    ret = app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_PLAYBACK_TRANS, msg_type, buf, len);
    if (ret == false)
    {
        APP_PRINT_ERROR0("app_playback_trans_relay_async_msg: fail!!!");
    }
}

static void app_playback_trans_relay_sync_msg(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                              T_REMOTE_TIMER_TYPE timer_type,
                                              uint32_t timer_period, bool loopback)
{
    bool ret = false;
    ret = app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_PLAYBACK_TRANS, msg_type, buf, len,
                                             timer_type, timer_period, loopback);
    if (ret == false)
    {
        APP_PRINT_ERROR0("app_playback_trans_relay_sync_msg: fail!!!");
    }
}
/**
    * @brief    change couple playlist info handle
    * @param    event_id   event_id
    * @param    p_data     check file info
    * @param    len        p_data len
    * @return   result
    */
static void app_playback_trans_couple_list_updata_handle(uint16_t event_id,
                                                         uint8_t *p_data, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        results = app_playback_trans_list_updata_handle(event_id, p_data);
    }
    else
    {
        results = PB_TRANS_RET_COUPLE_DISCON_ERROR;
        APP_PRINT_ERROR0("app_playback_trans_couple_delete_handle, couple disconn");
    }

    if (PB_TRANS_RET_SUCCESS == results) //pri success
    {
        //relay sec --> sec relay result  --> ack host
        uint16_t buf_len = sizeof(results) + sizeof(event_id) + len;
        uint8_t *buf = (uint8_t *)malloc(buf_len);
        if (buf == NULL)
        {
            return;
        }
        buf[0] = results;
        LE_UINT16_TO_ARRAY((buf + sizeof(results)), event_id);
        memcpy((buf  + sizeof(event_id) +  sizeof(results)), p_data, len);
        app_playback_trans_relay_async_msg(PB_REMOTE_MSG_PLAYLIST_UPDATA_SYNC, buf, buf_len);
        free(buf);
    }
    //ack host
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_SPP_MODE)
    {
        app_report_event(CMD_PATH_SPP, event_id, pb_trans_struct.id.spp_idx, &results, sizeof(results));
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
    {
        app_playback_trans_service_prepare_send_notify(pb_trans_struct.id.ble_conn_id, event_id,
                                                       sizeof(results),
                                                       &results);
    }
}

/**
    * @brief    exit handle on couple mode
    * @param    event_id   event_id
    * @return   result
    */
static void app_playback_trans_couple_exit_handle(uint16_t event_id)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    app_playback_trans_exit_trans_handle();
    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        //relay sec --> sec relay result  --> ack host
        uint16_t buf_len = sizeof(results) + sizeof(event_id);
        uint8_t *buf = (uint8_t *)malloc(buf_len);
        if (buf == NULL)
        {
            return;
        }
        buf[0] = results;
        LE_UINT16_TO_ARRAY((buf + sizeof(results)), event_id);
        app_playback_trans_relay_async_msg(PB_REMOTE_MSG_EXIT_SYNC, buf, buf_len);
        free(buf);
    }
}

/**
    * @brief    delete file handle on couple mode
    * @param    event_id   event_id
    * @param    p_data     check file info
    * @param    len        p_data len
    * @return   result
    */

static void app_playback_trans_couple_delete_handle(uint16_t event_id,
                                                    uint8_t *p_data, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        results = app_playback_trans_delete_handle(p_data);
    }
    else
    {
        results = PB_TRANS_RET_COUPLE_DISCON_ERROR;
        APP_PRINT_ERROR0("app_playback_trans_couple_delete_handle, couple disconn");
    }

    if ((PB_TRANS_RET_SUCCESS == results) || (PB_TRANS_RET_DELETE_OPENED_ERROR == results))
    {
        //relay sec --> sec relay result  --> ack host
        uint16_t buf_len = sizeof(results) + sizeof(event_id) + len;
        uint8_t *buf = (uint8_t *)malloc(buf_len);
        if (buf == NULL)
        {
            APP_PRINT_ERROR0("app_playback_trans_couple_delete_handle: malloc fail");
            return;
        }
        buf[0] = results;
        LE_UINT16_TO_ARRAY((buf + sizeof(results)), event_id);
        memcpy((buf + sizeof(event_id) +  sizeof(results)), p_data, len);
        if (PB_TRANS_RET_SUCCESS == results)
        {
            app_playback_trans_relay_async_msg(PB_REMOTE_MSG_DELETE_FILE_SYNC, buf, buf_len);
        }
        else if (PB_TRANS_RET_DELETE_OPENED_ERROR == results)
        {
            app_playback_trans_relay_sync_msg(PB_REMOTE_MSG_DELETE_OPENED_FILE_SYNC, buf, buf_len,
                                              REMOTE_TIMER_HIGH_PRECISION, 40, false);
        }
        free(buf);
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_SPP_MODE)
    {
        app_report_event(CMD_PATH_SPP, event_id, pb_trans_struct.id.spp_idx, &results, sizeof(results));
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
    {
        app_playback_trans_service_prepare_send_notify(pb_trans_struct.id.ble_conn_id, event_id,
                                                       sizeof(results),
                                                       &results);
    }
}

static uint8_t app_playback_trans_delete_all_file(void)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    if (playback_db.sd_play_state == PLAYBACK_STATE_PLAY)
    {
//        results = PB_TRANS_RET_DELETE_ERROR;
        app_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
    }

    if (playback_audio_fs_handle != NULL)
    {
        if (0 != audio_fs_close(playback_audio_fs_handle))
        {
            APP_PRINT_ERROR1("close FAIL,(%p)", playback_audio_fs_handle);
        }
        playback_audio_fs_handle = NULL;
    }
    results = audio_fs_delete_all_files();
    if (results == 0)
    {
        results = PB_TRANS_RET_SUCCESS;
    }
    else
    {
        results = PB_TRANS_RET_DELETE_ERROR;
    }

    return results;
}

/**
    * @brief    delete file handle on couple mode
    * @param    event_id   event_id
    * @param    p_data     check file info
    * @param    len        p_data len
    * @return   result
    */
static void app_playback_trans_couple_delete_all_handle(uint16_t event_id)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    APP_PRINT_TRACE0("app_playback_trans_couple_delete_all_handle ++");
    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        results = app_playback_trans_delete_all_file();
    }
    else
    {
        results = PB_TRANS_RET_COUPLE_DISCON_ERROR;
        APP_PRINT_ERROR0("app_playback_trans_couple_delete_all_handle: couple disconn");
    }

    if (PB_TRANS_RET_SUCCESS == results) //pri success
    {
        //relay sec --> sec relay result  --> ack host
        uint16_t buf_len = sizeof(results) + sizeof(event_id);
        uint8_t *buf = (uint8_t *)malloc(buf_len);
        if (buf == NULL)
        {
            APP_PRINT_ERROR0("app_playback_trans_couple_delete_all_handle: malloc fail");
            return;
        }
        buf[0] = results;
        LE_UINT16_TO_ARRAY((buf + sizeof(results)), event_id);
        app_playback_trans_relay_async_msg(PB_REMOTE_MSG_DELETE_ALL_FILE_SYNC, buf, buf_len);
        free(buf);
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_SPP_MODE)
    {
        app_report_event(CMD_PATH_SPP, event_id, pb_trans_struct.id.spp_idx, &results, sizeof(results));
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
    {
        app_playback_trans_service_prepare_send_notify(pb_trans_struct.id.ble_conn_id, event_id,
                                                       sizeof(results),
                                                       &results);
    }
}

static void app_playback_trans_couple_get_device_info(uint16_t event_id)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    APP_PRINT_TRACE0("app_playback_trans_couple_get_device_info");

    if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
    {
        //relay sec --> sec relay result  --> ack host
        uint8_t buf[3];
        buf[0] = results;
        LE_UINT16_TO_ARRAY((buf + sizeof(results)), event_id);
        app_playback_trans_relay_async_msg(PB_REMOTE_MSG_START_SYNC, buf, sizeof(buf));
    }
}
#endif

/*============================================================================*
 *                              Public Functions
 *============================================================================*/

/**
    * @brief  The main function to handle all the spp command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  app_idx received rx command device index
    * @return void
    */
void app_playback_trans_cmd_handle(uint16_t length, uint8_t *p_value, uint8_t app_idx)
{
    uint8_t ack_pkt[3];
    uint16_t cmd_id = *(uint16_t *)p_value;
    uint8_t *p;
    uint8_t results = PB_TRANS_RET_SUCCESS;

    bool ack_flag = false;

    ack_pkt[0] = p_value[0];
    ack_pkt[1] = p_value[1];
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    if (length < 2)
    {
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
        APP_PRINT_ERROR0("app_playback_trans_cmd_handle: error length");
        return;
    }

    length = length - 2;
    p = p_value + 2;
    pb_trans_struct.trans_mode = PLAYBACK_TRANS_SPP_MODE;
    pb_trans_struct.id.spp_idx = app_idx;

    APP_PRINT_TRACE2("===>app_playback_trans_cmd_handle, cmd_id:0x%x, length:0x%x\n", cmd_id, length);

    switch (cmd_id)
    {
    case CMD_PLAYBACK_QUERY_INFO:
        {
            if (length == PLAYBACK_LENGTH_GET_INFO)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                memcpy(pb_trans_struct.bd_addr, app_db.br_link[app_idx].bd_addr, sizeof(pb_trans_struct.bd_addr));

                PLAYBACK_DEVICE_INFO deviceinfo;
                app_playback_trans_clear_local();
                app_playback_trans_get_device_info(&deviceinfo);
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_QUERY_INFO, app_idx, (uint8_t *)&deviceinfo,
                                 sizeof(deviceinfo));
                if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_playback_trans_couple_get_device_info(EVENT_PLAYBACK_QUERY_INFO);
                }
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_GET_LIST_DATA:
        {
            if (length == PLAYBACK_LENGTH_GET_LIST_DATA)
            {
                uint8_t list_info[7] = {0};
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                results = app_playback_trans_list_data_info(p, list_info);
                if (results != PB_TRANS_RET_SUCCESS)
                {
                    app_playback_trans_error_clear_local();
                    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_GET_LIST_DATA, app_idx, &results, sizeof(results));
                    break;
                }
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_GET_LIST_DATA, app_idx, list_info, sizeof(list_info));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_TRANS_START:
        {
            uint16_t file_name_len;
            uint8_t cmd_len;
            LE_ARRAY_TO_UINT16(file_name_len, p);
            cmd_len = 2 + file_name_len + 4;
            if (length == cmd_len)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                results  = app_playback_trans_get_create_file_handle(p);
                if (results != PB_TRANS_RET_SUCCESS)
                {
                    app_playback_trans_error_clear_local();
                }
                else
                {
                    app_stop_timer(&timer_idx_pb_trans_sd_active);
                }
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_TRANS_START, app_idx, &results, sizeof(results));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_TRANS_CONTINUE:
        {
            results = app_playback_trans_data_packet_handle(p, length);
            ack_pkt[2] = results;
            app_start_timer(&timer_idx_pb_trans_file, "pb_trans_file",
                            app_playback_trans_timer_id, APP_TIMER_PB_TRANS_FILE, 0, false,
                            800);
        }
        break;

    case CMD_PLAYBACK_REPORT_BUFFER_CHECK:
        {
            if (length == PLAYBACK_LENGTH_BUFFER_CHECK_EN)
            {
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
//                results = app_playback_trans_buffer_check_handle(p);
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_REPORT_BUFFER_CHECK, app_idx, &results,
                                 sizeof(results));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_VALID_SONG:
        {
            if (length == PLAYBACK_LENGTH_VALID_SONG)
            {
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                results  = app_playback_trans_valid_handle(p);
                if (results != PB_TRANS_RET_SUCCESS)
                {
                    app_playback_trans_error_clear_local();
                }
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_VALID_SONG, app_idx, &results, sizeof(results));
                app_stop_timer(&timer_idx_pb_trans_file);
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
#if 0
    case CMD_PLAYBACK_TRIGGER_ROLE_SWAP:
        {
            if (length == PLAYBACK_LRNGTH_TRIGGER_ROLE_SWAP)
            {
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
//                results  = app_playback_trans_role_swap_handle();
                if (results != PB_TRANS_RET_SUCCESS)
                {
                    app_playback_trans_error_clear_local();
                }
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_TRIGGER_ROLE_SWAP, app_idx, &results,
                                 sizeof(results));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
#endif
    case CMD_PLAYBACK_TRANS_CANCEL:
        {
            if (length == PLAYBACK_LENGTH_TRANS_CANCEL)
            {
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                results = app_playback_trans_cancel_handle();
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_TRANS_CANCEL, app_idx, &results, sizeof(results));
                app_stop_timer(&timer_idx_pb_trans_file);
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_EXIT_TRANS:
        {
            app_playback_trans_sd_active_start_timer(3);
            app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_playback_trans_exit_trans_handle();
                results = PB_TRANS_RET_SUCCESS;
                app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_EXIT_TRANS, app_idx, &results, sizeof(results));
            }
            else
            {
                app_playback_trans_couple_exit_handle(EVENT_PLAYBACK_EXIT_TRANS);
            }
        }
        break;

    case CMD_PLAYBACK_PERMANENT_DELETE_SONG:
        {
            uint16_t rx_file_len;
            LE_ARRAY_TO_UINT16(rx_file_len, (p + sizeof(PB_DELETE_FILE_STRUCT) - PLAYBACK_LEN_NAME_LENTH))
            if (length == (rx_file_len + sizeof(PB_DELETE_FILE_STRUCT)))
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_single_delete_handle(p);
                    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_PERMANENT_DELETE_SONG, app_idx, &results,
                                     sizeof(results));
                }
                else
                {
                    app_playback_trans_couple_delete_handle(EVENT_PLAYBACK_PERMANENT_DELETE_SONG, p, length);
                }
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG:
        {
            if (length == PLAYBACK_LRNGTH_DELETE_ALL_SONG)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_delete_all_file();
                    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG, app_idx, &results,
                                     sizeof(results));
                }
                else
                {
                    app_playback_trans_couple_delete_all_handle(EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG);
                }
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_PLAYLIST_ADD_SONG:
    case CMD_PLAYBACK_PLAYLIST_DELETE_SONG:
        {
            uint16_t rx_file_len;
            uint16_t playlist_event_id = 0;
            LE_ARRAY_TO_UINT16(rx_file_len, (p + sizeof(PB_PLAYLIST_UPDATE_STRUCT) - PLAYBACK_LEN_NAME_LENTH))
            if (length == (rx_file_len + sizeof(PB_PLAYLIST_UPDATE_STRUCT)))
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
                if (cmd_id == CMD_PLAYBACK_PLAYLIST_ADD_SONG)
                {
                    playlist_event_id = EVENT_PLAYBACK_PLAYLIST_ADD_SONG;
                }
                else if (cmd_id == CMD_PLAYBACK_PLAYLIST_DELETE_SONG)
                {
                    playlist_event_id = EVENT_PLAYBACK_PLAYLIST_DELETE_SONG;
                }
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_list_updata_handle(playlist_event_id, p);
                    app_report_event(CMD_PATH_SPP, playlist_event_id, app_idx, &results, sizeof(results));
                }
                else
                {
                    app_playback_trans_couple_list_updata_handle(playlist_event_id, p, length);
                }
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    default:
        ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
        app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
        break;
    }

    if (ack_flag == true)
    {
        APP_PRINT_TRACE0("app_playback_trans_cmd_handle: invalid length");
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////
//                                BLE
//////////////////////////////////////////////////////////////////////////////////////////

/**
    * @brief    Wrapper function to send notification to peer
    * @note
    * @param    conn_id     ID to identify the connection
    * @param    opcode      Notification on the specified opcode
    * @param    len         Notification data length
    * @param    data        Additional notification data
    * @return   void
    */
#define OFFSET_TEMP (0 + 2) //SATRT + EVENT len
static void app_playback_trans_service_prepare_send_notify(uint8_t conn_id, uint16_t event_id,
                                                           uint16_t len, uint8_t *data)
{
    uint8_t *p_buffer = NULL;
    uint16_t mtu_size;
    uint16_t remain_size = len;
    uint8_t *p_data = data;
    uint16_t send_len;

    if ((data == NULL) || (len == 0))
    {
        return;
    }

    le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id);
    APP_PRINT_TRACE2("app_playback_trans_service_prepare_send_notify mtu_size:%d, len:0x%x", mtu_size,
                     len);
    p_buffer = malloc(mtu_size);

    if (p_buffer == NULL)
    {
        return;
    }
    memset(p_buffer, 0, mtu_size);

    if (len < mtu_size - OFFSET_TEMP)
    {
//        p_buffer[0] = PLAYBACK_OPCODE_START_DFU;
//        p_buffer[1] = opcode;
        LE_UINT16_TO_ARRAY((uint8_t *)&p_buffer[0], event_id);
        memcpy(&p_buffer[OFFSET_TEMP], data, len);
        playback_service_send_notification(conn_id, p_buffer, len + OFFSET_TEMP);
        free(p_buffer);
        return;
    }

    while (remain_size)
    {
        if (remain_size == len)
        {
//            p_buffer[0] = PLAYBACK_OPCODE_NOTIF;
//            p_buffer[1] = opcode;
            LE_UINT16_TO_ARRAY((uint8_t *)&p_buffer[0], event_id);
            memcpy(&p_buffer[OFFSET_TEMP], p_data, mtu_size - OFFSET_TEMP);
            playback_service_send_notification(conn_id, p_buffer, mtu_size);
            p_data += (mtu_size - OFFSET_TEMP);
            remain_size -= (mtu_size - OFFSET_TEMP);
            continue;
        }

        send_len = (remain_size > mtu_size) ? mtu_size : remain_size;
        memcpy(p_buffer, p_data, send_len);
        playback_service_send_notification(conn_id, p_buffer, send_len);
        p_data += send_len;
        remain_size -= send_len;
    }

    free(p_buffer);
}

/**
    * @brief    Handle written request characteristic
    * @param    conn_id     ID to identify the connection
    * @param    length      Length of value to be written
    * @param    p_value     Value to be written
    * @return   T_APP_RESULT
    * @retval   Handle result of this request
    */

T_APP_RESULT app_playback_trans_ble_handle_cp_req(uint8_t conn_id, uint16_t length,
                                                  uint8_t *p_value)
{
    T_APP_RESULT cause = APP_RESULT_INVALID_PDU;
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t cmd_id = *(uint16_t *)p_value;
    uint8_t *p = p_value + 2;
    bool error_flag = false;

    if (length < 2)
    {
        APP_PRINT_ERROR0("app_playback_trans_ble_handle_cp_req: error length");
        return APP_RESULT_INVALID_PDU;
    }

    length = length - 2;
//    p = p_value + 2;
    pb_trans_struct.id.ble_conn_id = conn_id;
    pb_trans_struct.trans_mode = PLAYBACK_TRANS_BLE_MODE;
    APP_PRINT_TRACE2("===>app_playback_trans_ble_handle_cp_req: cmd_id:0x%x, length:0x%x\n", cmd_id,
                     length);

    switch (cmd_id)
    {
    case CMD_PLAYBACK_QUERY_INFO:
        {
            if (length == PLAYBACK_LENGTH_GET_INFO)
            {
                PLAYBACK_DEVICE_INFO device_info;
                cause = APP_RESULT_SUCCESS;
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_playback_trans_get_device_info(&device_info);
                device_info.protocol_ver = BLE_PLAYBACK_VERSION;
                device_info.pkt_size = BLE_PLAYBACK_MTU_SIZE;
                le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &device_info.pkt_size, conn_id);
                app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_QUERY_INFO,
                                                               sizeof(device_info),
                                                               (uint8_t *)&device_info);

                if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_playback_trans_couple_get_device_info(EVENT_PLAYBACK_QUERY_INFO);
                }
            }
            else
            {
                error_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_GET_LIST_DATA:
        {
            if (length == PLAYBACK_LENGTH_GET_LIST_DATA)
            {
                uint8_t list_info[7] = {0};
                cause = APP_RESULT_SUCCESS;
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                results = app_playback_trans_list_data_info(p, list_info);
                if (results != PB_TRANS_RET_SUCCESS)
                {
                    app_playback_trans_error_clear_local();
                    app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_GET_LIST_DATA,
                                                                   sizeof(results),
                                                                   &results);
                    break;
                }
                app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_GET_LIST_DATA,
                                                               sizeof(list_info),
                                                               (uint8_t *)&list_info);
                app_start_timer(&timer_idx_pb_trans_up, "pb_trans_up",
                                app_playback_trans_timer_id, APP_TIMER_PB_TRANS_UP, 0, false,
                                100);
            }
            else
            {
                error_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_EXIT_TRANS:
        {
            cause = APP_RESULT_SUCCESS;
            app_playback_trans_sd_active_start_timer(3);
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_playback_trans_exit_trans_handle();
                app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_EXIT_TRANS, sizeof(results),
                                                               &results);
            }
            else
            {
                app_playback_trans_couple_exit_handle(EVENT_PLAYBACK_EXIT_TRANS);
            }
        }
        break;

    case CMD_PLAYBACK_PERMANENT_DELETE_SONG:
        {
            uint16_t rx_file_len;
            LE_ARRAY_TO_UINT16(rx_file_len, (p + sizeof(PB_DELETE_FILE_STRUCT) - PLAYBACK_LEN_NAME_LENTH))
            if (length == (rx_file_len + sizeof(PB_DELETE_FILE_STRUCT)))
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                cause = APP_RESULT_SUCCESS;
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_single_delete_handle(p);
                    app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_PERMANENT_DELETE_SONG,
                                                                   sizeof(results), &results);
                }
                else
                {
                    app_playback_trans_couple_delete_handle(EVENT_PLAYBACK_PERMANENT_DELETE_SONG, p, length);
                }
            }
            else
            {
                error_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG:
        {
            if (length == PLAYBACK_LRNGTH_DELETE_ALL_SONG)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                cause = APP_RESULT_SUCCESS;
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_delete_all_file();
                    app_playback_trans_service_prepare_send_notify(conn_id, EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG,
                                                                   sizeof(results), &results);
                }
                else
                {
                    app_playback_trans_couple_delete_all_handle(EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG);
                }
            }
            else
            {
                error_flag = true;
            }
        }
        break;

    case CMD_PLAYBACK_PLAYLIST_ADD_SONG:
    case CMD_PLAYBACK_PLAYLIST_DELETE_SONG:
        {
            uint16_t rx_file_len;
            uint16_t playlist_event_id = 0;
            LE_ARRAY_TO_UINT16(rx_file_len, (p + sizeof(PB_PLAYLIST_UPDATE_STRUCT) - PLAYBACK_LEN_NAME_LENTH))
            if (length == (rx_file_len + sizeof(PB_PLAYLIST_UPDATE_STRUCT)))
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                cause = APP_RESULT_SUCCESS;
                if (cmd_id == CMD_PLAYBACK_PLAYLIST_ADD_SONG)
                {
                    playlist_event_id = EVENT_PLAYBACK_PLAYLIST_ADD_SONG;
                }
                else if (cmd_id == CMD_PLAYBACK_PLAYLIST_DELETE_SONG)
                {
                    playlist_event_id = EVENT_PLAYBACK_PLAYLIST_DELETE_SONG;
                }
                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    results = app_playback_trans_list_updata_handle(playlist_event_id, p);
                    app_playback_trans_service_prepare_send_notify(conn_id, playlist_event_id, sizeof(results),
                                                                   &results);
                }
                else
                {
                    app_playback_trans_couple_list_updata_handle(playlist_event_id, p, length);
                }
            }
            else
            {
                error_flag = true;
            }
        }
        break;

    default:
        APP_PRINT_ERROR1("app_playback_trans_ble_handle_cp_req, cmd_id not expected", cmd_id);
        break;
    }

    if (error_flag)
    {
        APP_PRINT_ERROR0("app_playback_trans_ble_handle_cp_req: invalid length");
    }

    return cause;
}
//////////////////////////////////////////////ble end///////////////////////////////////////////////////

/**
    * @brief  get ota status
    * @return True:is doing ota; False: is not doing ota
    */
bool app_playback_trans_is_busy(void)
{
    return pb_trans_struct.is_trans_pb_process;
}

//////////////////////////////////////couple relay///////////////////////////////////////////////
# if APP_PLAYBACK_TRANS_COUPLE_EN
/**
    * @brief    secondary handle on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_sec_start_handle(uint8_t *buf, uint16_t len)
{
//    uint8_t results = PB_TRANS_RET_SUCCESS;

    app_playback_trans_clear_local();

//    buf[0] = results;
//    remote_async_msg_relay(app_playback_trans_relay_handle, PB_REMOTE_MSG_SEC_INIT_RESULT_SYNC, buf,
//                     len, false);
}

/**
    * @brief    secondary playlist update handle on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_playlist_sec_updata_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t event_id;
    uint8_t *p_data = buf + sizeof(results) + sizeof(event_id);

    APP_PRINT_TRACE2("app_playback_trans_relay_playlist_sec_updata_handle, len:0x%x, %b", len,
                     TRACE_BINARY(len, buf));
    LE_ARRAY_TO_UINT16(event_id, (buf + sizeof(results)));
    results = app_playback_trans_list_updata_handle(event_id, p_data);
    //ack pri
    uint8_t data[3] = {0};
    data[0] = results;
    LE_UINT16_TO_ARRAY(&data[1], event_id);
    app_playback_trans_relay_async_msg(PB_REMOTE_MSG_SEC_RESULT_SYNC, data, sizeof(data));
//    app_report_event(CMD_PATH_SPP, event_id, app_idx, &results, sizeof(results));
}

/**
    * @brief    secondary delete file handle on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_sec_delete_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t event_id;
    uint8_t *p_data = buf + sizeof(results) + sizeof(event_id);

    APP_PRINT_TRACE2("app_playback_trans_relay_sec_delete_handle, len:0x%x, %b", len,
                     TRACE_BINARY(len, buf));
    LE_ARRAY_TO_UINT16(event_id, (buf + sizeof(results)));
    results = app_playback_trans_delete_handle(p_data);
    //ack pri
    uint8_t data[3] = {0};
    data[0] = results;
    LE_UINT16_TO_ARRAY(&data[1], event_id);
    app_playback_trans_relay_async_msg(PB_REMOTE_MSG_SEC_RESULT_SYNC, data, sizeof(data));
}

static void app_playback_trans_relay_delete_opened_file_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t event_id;
    uint8_t old_play_state = playback_db.sd_play_state;;
    uint8_t *p_data = buf + sizeof(results) + sizeof(event_id);

    LE_ARRAY_TO_UINT16(event_id, (buf + sizeof(results)));
    if (playback_db.sd_play_state == PLAYBACK_STATE_PLAY)
    {
        app_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
    }

    if (playback_audio_fs_handle != NULL)
    {
        if (0 != audio_fs_close(playback_audio_fs_handle))
        {
            APP_PRINT_ERROR1("close FAIL,(%p)", playback_audio_fs_handle);
        }
        playback_audio_fs_handle = NULL;
    }
    results = app_playback_trans_delete_handle(p_data);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (old_play_state == PLAYBACK_STATE_PLAY)
        {
            app_mmi_handle_action(MMI_SD_PLAYPACK_PAUSE);
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        //ack pri
        uint8_t data[3] = {0};
        data[0] = results;
        LE_UINT16_TO_ARRAY(&data[1], event_id);
        app_playback_trans_relay_async_msg(PB_REMOTE_MSG_SEC_RESULT_SYNC, data, sizeof(data));
    }
}

/**
    * @brief    secondary delete file handle on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_sec_delete_all_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t event_id;
//    uint8_t *p_data = buf + sizeof(results) + sizeof(event_id);

    APP_PRINT_TRACE2("app_playback_trans_relay_sec_delete_all_handle, len:0x%x, %b", len,
                     TRACE_BINARY(len, buf));
    LE_ARRAY_TO_UINT16(event_id, (buf + sizeof(results)));
    results = app_playback_trans_delete_all_file();
    if (results == 0)
    {
        results = PB_TRANS_RET_SUCCESS;
    }
    //ack pri
    uint8_t data[3] = {0};
    data[0] = results;
    LE_UINT16_TO_ARRAY(&data[1], event_id);
    app_playback_trans_relay_async_msg(PB_REMOTE_MSG_SEC_RESULT_SYNC, data, sizeof(data));
}

/**
    * @brief    secondary exit handle on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_sec_exit_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    app_playback_trans_exit_trans_handle();
    //ack pri
    buf[0] = results;
    app_playback_trans_relay_async_msg(PB_REMOTE_MSG_SEC_RESULT_SYNC, buf, len);
}

/**
    * @brief    secondary sync result to pri on couple mode
    * @param    buf   relay buf
    * @param    len   buf len
    * @return   result
    */
static void app_playback_trans_relay_sec_result_handle(uint8_t *buf, uint16_t len)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;
    uint16_t event_id;

    results = buf[0];
    LE_ARRAY_TO_UINT16(event_id, (buf + sizeof(results)));
    APP_PRINT_TRACE2("app_playback_trans_relay_sec_result_handle, len:0x%x, event_id: 0x%x", len,
                     event_id);

    if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_SPP_MODE)
    {
        app_report_event(CMD_PATH_SPP, event_id, pb_trans_struct.id.spp_idx, &results, sizeof(results));
    }
    else if (pb_trans_struct.trans_mode == PLAYBACK_TRANS_BLE_MODE)
    {
        app_playback_trans_service_prepare_send_notify(pb_trans_struct.id.ble_conn_id, event_id,
                                                       sizeof(results), &results);
    }
}

void app_playback_trans_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE4("app_playback_trans_parse_cback: msg_type 0x%04x, status %d, len:%d, bud_role: %d",
                     msg_type, status, len, app_cfg_nv.bud_role);
    switch (msg_type)
    {
    case PB_REMOTE_MSG_START_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_playback_trans_relay_sec_start_handle(buf, len);
            }
        }
        break;

    case PB_REMOTE_MSG_PLAYLIST_UPDATA_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_playback_trans_relay_playlist_sec_updata_handle(buf, len);
            }
        }
        break;

    case PB_REMOTE_MSG_PLAYLIST_ADD_SONG_SYNC:
    case PB_REMOTE_MSG_PLAYLIST_DELETE_SONG_SYNC:
    case PB_REMOTE_MSG_CANCEL_SYNC:
        break;

    case PB_REMOTE_MSG_DELETE_FILE_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_playback_trans_relay_sec_delete_handle(buf, len);
            }
        }
        break;

    case PB_REMOTE_MSG_DELETE_OPENED_FILE_SYNC:
        if (status == REMOTE_RELAY_STATUS_SYNC_TOUT || //03
            status == REMOTE_RELAY_STATUS_SYNC_EXPIRED || //04
            status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED) //08
        {
            app_playback_trans_relay_delete_opened_file_handle(buf, len);
        }
        break;

    case PB_REMOTE_MSG_DELETE_ALL_FILE_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
                app_playback_trans_relay_sec_delete_all_handle(buf, len);
            }
        }
        break;

    case PB_REMOTE_MSG_EXIT_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_playback_trans_relay_sec_exit_handle(buf, len);
                app_playback_trans_sd_active_start_timer(3);
            }
        }
        break;

    case PB_REMOTE_MSG_SEC_RESULT_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_playback_trans_relay_sec_result_handle(buf, len);
            }
        }
        break;

    default:
        break;
    }
}
#endif
/////////////////////////////////////////couple relay end/////////////////////////////////////////////////////
///////////////////////////////////////// timer /////////////////////////////////////////////////////
/**
    * @brief  timeout callback
    * @param  timer_evt  timer event
    * @param  timer_chann  time channel
    * @return void
    */
static void app_playback_trans_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE1("app_playback_trans_timeout_cb: timer_evt 0x%02x", timer_evt);

    switch (timer_evt)
    {
    case APP_TIMER_PB_TRANS_FILE:
        {
            app_stop_timer(&timer_idx_pb_trans_file);
            app_playback_trans_error_clear_local();
            app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
        }
        break;

    case APP_TIMER_PB_TRANS_UP:
        {
            app_stop_timer(&timer_idx_pb_trans_up);
            app_playback_trans_list_data_handle();
        }
        break;

    case APP_TIMER_PB_TRANS_SD_ACTIVE:
        {
            app_stop_timer(&timer_idx_pb_trans_sd_active);
            app_sd_card_dlps_enable(APP_SD_DLPS_ENTER_CHECK_TRANS_FILE);
            app_sd_card_power_down_enable(APP_SD_POWER_DOWN_ENTER_CHECK_TRANS_FILE);
        }
        break;

    default:
        break;
    }
}

static void app_playback_trans_sd_active_start_timer(uint8_t time_s)
{
    app_stop_timer(&timer_idx_pb_trans_sd_active);
    app_sd_card_dlps_disable(APP_SD_DLPS_ENTER_CHECK_TRANS_FILE);
    app_sd_card_power_down_disable(APP_SD_POWER_DOWN_ENTER_CHECK_TRANS_FILE);
    app_start_timer(&timer_idx_pb_trans_sd_active, "pb_trans_sd_active",
                    app_playback_trans_timer_id, APP_TIMER_PB_TRANS_SD_ACTIVE, 0, false,
                    time_s * 1000);
}
/////////////////////////////////////////sd active end/////////////////////////////////////////////////////

///////////////////////////////////////////playback write task//////////////////////////////////////////////
#define MAX_QUEUE_SIZE_PLAYBACK          10
void *playback_write_task_handle;
static void *playback_wr_msg_queue;

static void app_playback_trans_write_result_ack(uint8_t wr_result)
{
    T_IO_MSG gpio_msg;

    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_PLAYBACK_TRANS_FILE_ACK;
    gpio_msg.u.param = wr_result;
    app_io_msg_send(&gpio_msg);
}

void app_playback_trans_write_file_ack_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    uint8_t wr_result;
    wr_result = (uint8_t)io_driver_msg_recv->u.param;
    APP_PRINT_TRACE1("app_playback_trans_ack_handle_msg: wr_result 0x%x", wr_result);
    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_REPORT_BUFFER_CHECK, pb_trans_struct.id.spp_idx,
                     &wr_result, sizeof(wr_result));
    if (wr_result != PB_TRANS_RET_SUCCESS)
    {
        app_playback_trans_sd_active_start_timer(APP_PLAYBACK_SD_ACTIVE_TIME_S);
    }
}

static void app_playback_trans_write_send_msg(char event)
{
    os_msg_send(playback_wr_msg_queue, &event, 0);
    APP_PRINT_TRACE1("app_playback_trans_write_send_msg: event: %d", event);
}

static void app_playback_trans_write_data_to_sd(char event)
{
    uint8_t results = PB_TRANS_RET_SUCCESS;

    results = app_playback_trans_fs_write_data(event);
//    app_report_event(CMD_PATH_SPP, EVENT_PLAYBACK_REPORT_BUFFER_CHECK, pb_trans_struct.id.spp_idx,
//                     &results, sizeof(results));
//    send app msg
    app_playback_trans_write_result_ack(results);
}

static void app_playback_trans_write_data_task(void *arg)
{
    char event;

    os_msg_queue_create(&playback_wr_msg_queue, "app_playback_msgQ", MAX_QUEUE_SIZE_PLAYBACK,
                        sizeof(char));
    while (true)
    {
        if (os_msg_recv(playback_wr_msg_queue, &event, 0xFFFFFFFF) == true)
        {
            if ((event == PLAYBACK_BUF_A_EVENT) || (event == PLAYBACK_BUF_B_EVENT))
            {
                app_playback_trans_write_data_to_sd(event);
            }
        }
    }
}

static void app_playback_trans_create_write_task(void)
{
    if (os_task_create(&playback_write_task_handle, "app_playback_write_data",
                       app_playback_trans_write_data_task,
                       NULL, 1024, 1) == true)
    {
        APP_PRINT_TRACE0("app_playback_trans_create_write_task: success");
    }
}
///////////////////////////////////////////playback write task end//////////////////////////////////////////////////

void app_playback_trans_file_init(void)
{
    app_timer_reg_cb(app_playback_trans_timeout_cb, &app_playback_trans_timer_id);

#if APP_PLAYBACK_TRANS_COUPLE_EN
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        app_relay_cback_register(NULL, app_playback_trans_parse_cback,
                                 APP_MODULE_TYPE_PLAYBACK_TRANS, PB_REMOTE_MSG_MAX);
    }
#endif

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        app_playback_trans_create_write_task();
    }
}
/** End of APP_PLAYBACK_Exported_Functions
    * @}
    */

/** @} */ /* End of group APP_PLAYBACK_SERVICE */
#endif //F_APP_LOCAL_PLAYBACK_SUPPORT

