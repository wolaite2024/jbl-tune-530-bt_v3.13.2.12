/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_ota.c
   * @brief     Source file for using OTA service
   * @author    Michael
   * @date      2019-11-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <string.h>
#include <bt_types.h>
#include "trace.h"
#include "app_main.h"
#include "app_cmd.h"
#include "ota_service.h"
#include "app_mmi.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "app_ota.h"
#include "app_ble_common_adv.h"
#include "rtl876x.h"
#include "fmc_api.h"
#include "app_bt_policy_api.h"
#include "gap_conn_le.h"
#include "app_ble_gap.h"
#include "app_report.h"
#include "stdlib.h"
#include "app_sniff_mode.h"
#include "ota_ext.h"
#include "app_sensor.h"
#include "wdg.h"
#include "audio_probe.h"
#include "app_ble_device.h"

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#define COPY_WDG_KICK_SIZE 0xF000

/** @defgroup  APP_OTA_SERVICE APP OTA handle
    * @brief APP OTA Service to implement OTA feature
    * @{
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup APP_OTA_Exported_Variables APP OTA Exported Variables
    * @brief
    * @{
    */
/** @brief  OTA timer event*/
typedef enum
{
    APP_TIMER_OTA_RESET,
    APP_TIMER_OTA_TRANSFER,
    APP_TIMER_OTA_RWS_SYNC,
} T_OTA_TIMER;

static T_APP_OTA_DB *ota_struct = NULL;
T_OS_QUEUE ota_cback_list[OTA_STAGE_MAX];

static uint8_t timer_idx_ota_reset = 0;
static uint8_t timer_idx_ota_transfer = 0;
static uint8_t timer_idx_ota_rws_sync = 0;
static uint8_t ota_timer_id = 0;

/** End of APP_OTA_Exported_Variables
    * @}
    */

/*============================================================================*
 *                              Private Functions
 *============================================================================*/
/** @defgroup APP_OTA_Exported_Functions APP OTA service Exported Functions
    * @brief
    * @{
    */

void app_ota_disc_b2s_profile(void)
{
    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
        }
    }
}

/**
    * @brief  Reset local variables
    * @return void
    */
static void app_ota_clear_local(uint8_t cause)
{
    APP_PRINT_TRACE1("app_ota_clear_local cause: %d", cause);

    app_sniff_mode_b2s_enable(ota_struct->bd_addr, SNIFF_DISABLE_MASK_OTA);

#if (FORCE_OTA_TEMP == 1)
    if (ota_struct->force_temp_mode)
    {
        force_enable_ota_temp(false);
    }
#endif

    if (ota_struct->p_ota_temp_buf_head != NULL)
    {
        audio_probe_media_buffer_free(ota_struct->p_ota_temp_buf_head);
    }

    if (ota_struct->bp_level)
    {
        fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                                ota_struct->bp_level);
    }

    if (cause != OTA_SUCCESS_REBOOT)
    {
        memset(ota_struct, 0, offsetof(T_APP_OTA_DB, test));
        free(ota_struct);
        ota_struct = NULL;
    }

}

/**
    * @brief  check ota mode
    * @return bud status
    */
static uint8_t app_ota_check_ota_mode(void)
{
    uint8_t ret;

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        ret = SINGLE_DEFAULT;
    }
    else if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        ret = RWS_B2B_CONNECT;
    }
    else
    {
        ret = SINGLE_B2B_DISC;
    }

    APP_PRINT_TRACE1("app_ota_check_ota_mode: %d", ret);

    return ret;
}

/**
    * @brief    send meessage between two buds whe rws ota
    * @param    ota_mode    ble ota or spp ota
    * @param    cmd_id  command id
    * @param    data    message data
    * @return   void
    */
static void app_ota_rws_send_msg(uint8_t ota_mode, uint8_t cmd_id, uint8_t data)
{
    APP_PRINT_TRACE1("app_ota_rws_send_msg: mcd_id %d", cmd_id);

    if (ota_struct == NULL)
    {
        ota_struct = (T_APP_OTA_DB *)calloc(1, sizeof(T_APP_OTA_DB));
    }

    ota_struct->rws_mode.msg.ota_mode = ota_mode;
    ota_struct->rws_mode.msg.cmd_id = cmd_id;
    ota_struct->rws_mode.msg.data = data;
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_VALID_SYNC,
                                        (uint8_t *)&ota_struct->rws_mode.msg, sizeof(ota_struct->rws_mode.msg));

    if (cmd_id != RWS_OTA_ACK)
    {
        app_start_timer(&timer_idx_ota_rws_sync, "ota_rws_sync",
                        ota_timer_id, APP_TIMER_OTA_RWS_SYNC, 0, false,
                        500);
    }
}

/**
    * @brief    erase the ota header of inactive bank
    * @param    void
    * @return   void
    */
static void app_ota_erase_ota_header(void)
{
    if (ota_struct->test.t_stress_test)
    {
        ota_struct->test.t_stress_test = 0;
        if (is_ota_support_bank_switch())
        {
            fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), 0);
            uint32_t header_addr = get_active_ota_bank_addr();
            fmc_flash_nor_erase(header_addr, FMC_FLASH_NOR_ERASE_SECTOR);
        }
    }
}

/**
    * @brief  get encrypt setting
    * @param  void
    * @return true:aes encrypt enable; false: aes encrypt disable
    */
static bool app_ota_enc_setting(void)
{
    return ota_struct->test.t_aes_en | app_cfg_const.enable_ota_enc;
}

/**
    * @brief  timeout callback for ota
    * @param  timer_evt  timer event
    * @param  param
    * @return void
    */
static void app_ota_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE1("app_ota_timeout_cb: timer id 0x%02X", timer_evt);

    switch (timer_evt)
    {
    case APP_TIMER_OTA_RESET:
        {
            app_stop_timer(&timer_idx_ota_reset);
            app_ota_erase_ota_header();
            chip_reset(RESET_ALL);
        }
        break;
    case APP_TIMER_OTA_TRANSFER:
        {
            app_stop_timer(&timer_idx_ota_transfer);
            app_ota_error_clear_local(OTA_IMAGE_TRANS_TIMEOUT);
        }
        break;
    case APP_TIMER_OTA_RWS_SYNC:
        {
            app_stop_timer(&timer_idx_ota_rws_sync);
            if (ota_struct->rws_mode.retry_times < 3)
            {
                ota_struct->rws_mode.retry_times ++;
                app_ota_rws_send_msg(ota_struct->rws_mode.msg.ota_mode,
                                     ota_struct->rws_mode.msg.cmd_id,
                                     ota_struct->rws_mode.msg.data);
            }
            else
            {
                ota_struct->rws_mode.retry_times = 0;
            }
        }
        break;
    default:
        break;
    }
}

/**
    * @brief    get current active bank
    * @param    void
    * @return   0x01:bank0, 0x02:bank1, 0x03:not support bank switch
    */
uint8_t app_ota_get_active_bank(void)
{
    uint8_t active_bank;
    uint32_t ota_bank0_addr;

    if (is_ota_support_bank_switch())
    {
        ota_bank0_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);

        if (ota_bank0_addr == get_active_ota_bank_addr())
        {
            active_bank = IMAGE_LOCATION_BANK0;
        }
        else
        {
            active_bank = IMAGE_LOCATION_BANK1;
        }
    }
    else
    {
        active_bank = NOT_SUPPORT_BANK_SWITCH;
    }

    return active_bank;
}

/**
    * @brief    execute ota enter callback
    * @param    void
    * @return   void
    */
static void app_ota_enter(void)
{
    T_OTACbQueueElem *p_item;
    p_item = (T_OTACbQueueElem *)ota_cback_list[OTA_ENTER].p_first;
    while (p_item != NULL)
    {
        OTAEnterCbFunc p_cback = (OTAEnterCbFunc)p_item->cback;
        p_cback();
        p_item = p_item->p_next;
    }
}

/**
    * @brief    execute ota exit callback
    * @param    void
    * @return   void
    */
static void app_ota_exit(void)
{
    T_OTACbQueueElem *p_item;
    p_item = (T_OTACbQueueElem *)ota_cback_list[OTA_EXIT].p_first;
    while (p_item != NULL)
    {
        OTAExitCbFunc p_cback = (OTAExitCbFunc)p_item->cback;
        p_cback();
        p_item = p_item->p_next;
    }
}


/**
    * @brief    Handle the start dfu control point
    * @param    p_data     data to be written
    * @return   handle result 0x01:success other:fail
    */
static uint8_t app_ota_start_dfu_handle(uint8_t *p_data)
{
    uint8_t results = DFU_ARV_SUCCESS;

    if (p_data == NULL)
    {
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
        return results;
    }

    if (ota_struct == NULL)
    {
        ota_struct = (T_APP_OTA_DB *)calloc(1, sizeof(T_APP_OTA_DB));
    }

    if (app_ota_enc_setting())
    {
        aes256_decrypt_16byte(p_data);
    }

    T_IMG_CTRL_HEADER_FORMAT *start_dfu_para = (T_IMG_CTRL_HEADER_FORMAT *)p_data;

#if (FORCE_OTA_TEMP == 1)
    ota_struct->force_temp_mode = *(uint8_t *)((T_IMG_CTRL_HEADER_FORMAT *)p_data + 1);
#endif

    ota_struct->image_total_length = start_dfu_para->payload_len + IMG_HEADER_SIZE;
    ota_struct->image_id = start_dfu_para->image_id;
    APP_PRINT_TRACE5("app_ota_start_dfu: ic_type=0x%x, CtrlFlag=0x%x, img_id=0x%x ,CRC16=0x%x, Length=0x%x",
                     start_dfu_para->ic_type,
                     start_dfu_para->ctrl_flag,
                     start_dfu_para->image_id,
                     start_dfu_para->crc16,
                     ota_struct->image_total_length
                    );

    if (app_ota_check_section_size(ota_struct->image_total_length, ota_struct->image_id) == false)
    {
        APP_PRINT_ERROR0("app_ota_check_section_size: Image is oversize");
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
        return results;
    }

#if (FORCE_OTA_TEMP == 1)
    if (ota_struct->force_temp_mode)
    {
        force_enable_ota_temp(true);
    }
#endif

    if (!is_ota_support_bank_switch()
        && ota_struct->image_id <= IMG_OTA)
    {
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
        return results;
    }

    ota_struct->ota_flag.is_ota_process = true;
    ota_struct->ota_temp_buf_used_size = 0;
    ota_struct->cur_offset = 0;

    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), 0);

    return results;
}

/**
    * @brief    Handle buffer check enable cmd
    * @param    p_data      point of output data
    * @param    max_buffer_size     set max buffer size
    * @return   void
    */
static void app_ota_buffer_check_en(uint8_t *p_data, uint16_t max_buffer_size)
{
    uint8_t size_factor = 0;

    if (p_data == NULL)
    {
        return;
    }

    ota_struct->ota_flag.buffer_check_en = true;
    ota_struct->ota_flag.ota_mode = true;

    if (ota_struct->p_ota_temp_buf_head == NULL)
    {
        app_ota_enter();
    }

    while (ota_struct->p_ota_temp_buf_head == NULL)
    {
        ota_struct->buffer_size = max_buffer_size >> size_factor;
        ota_struct->p_ota_temp_buf_head = (uint8_t *)audio_probe_media_buffer_malloc(
                                              ota_struct->buffer_size);
        size_factor ++;
        if (ota_struct->buffer_size <= 0)
        {
            break;
        }
    }

    if (ota_struct->p_ota_temp_buf_head == NULL
        || ota_struct->buffer_size == 0)
    {
        ota_struct->ota_flag.buffer_check_en = false;
    }
    p_data[0] = ota_struct->ota_flag.buffer_check_en;
    LE_UINT16_TO_ARRAY(&p_data[1], ota_struct->buffer_size);

    APP_PRINT_TRACE2("app_ota_buffer_check_en: buffer_check_en=%d, buffer_size=%d",
                     ota_struct->ota_flag.buffer_check_en, ota_struct->buffer_size);
}

/**
    * @brief    Handle written request on DFU packet
    * @param    p_data     data to be written
    * @param    length     Length of value to be written
    * @return   handle result  0x01:success other:fail
    */
static uint8_t app_ota_data_packet_handle(uint8_t *p_data, uint16_t length)
{
    uint8_t results = DFU_ARV_SUCCESS;

    APP_PRINT_TRACE4("app_ota_data_packet_handle: length=%d, nCurOffSet=%d, buffer_check_offset=%d, nImageTotalLength=%d",
                     length, ota_struct->cur_offset, ota_struct->buffer_check_offset, ota_struct->image_total_length);

    ota_struct->ota_flag.skip_flag = 1;

    if ((p_data == NULL) || (ota_struct->ota_flag.is_ota_process == false))
    {
        results = DFU_ARV_FAIL_OPERATION;
        return results;
    }

    if (ota_struct->cur_offset + length + ota_struct->ota_temp_buf_used_size >
        ota_struct->image_total_length)
    {
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
    }
    else
    {
        if (!ota_struct->ota_flag.buffer_check_en)
        {
            if (app_ota_enc_setting())
            {
                uint16_t offset = 0;
                while ((length - offset) >= 16)
                {
                    aes256_decrypt_16byte(p_data + offset);
                    offset += 16;
                }
            }
            if ((ota_struct->buffer_check_offset != 0) && (ota_struct->p_ota_temp_buf_head == NULL))
            {
                ota_struct->p_ota_temp_buf_head = (uint8_t *)malloc(ota_struct->buffer_check_offset);
            }
            uint32_t flash_write_result = app_ota_write_to_flash(ota_struct->image_id,
                                                                 ota_struct->cur_offset,
                                                                 ota_struct->next_subimage_offset,
                                                                 length,
                                                                 p_data);
            if (flash_write_result != 0)
            {
                APP_PRINT_TRACE1("app_ota_buffer_check_handle: write ret: %d", flash_write_result);
                results = DFU_ARV_FAIL_OPERATION;
            }
            else
            {
                ota_struct->cur_offset += length;
                if ((ota_struct->buffer_check_offset != 0) && (ota_struct->p_ota_temp_buf_head != NULL))
                {
                    ota_struct->buffer_check_offset = 0;
                    free(ota_struct->p_ota_temp_buf_head);
                    ota_struct->p_ota_temp_buf_head = NULL;
                }
            }
        }
        else
        {
            if (ota_struct->ota_temp_buf_used_size + ota_struct->buffer_check_offset + length <=
                ota_struct->buffer_size)
            {
                memcpy(ota_struct->p_ota_temp_buf_head + ota_struct->buffer_check_offset +
                       ota_struct->ota_temp_buf_used_size, p_data, length);
                ota_struct->ota_temp_buf_used_size += length;
            }
            else
            {
                results = DFU_ARV_FAIL_OPERATION;
            }
        }
    }

    return results;
}

/**
    * @brief    Handle written request on DFU packet
    * @param    p_data     point of input data
    * @param    data      point of output data
    * @return   void
    */
static void app_ota_buffer_check_handle(uint8_t *p_data, uint8_t *data)
{
    uint16_t data_size;
    uint16_t crc;

    if ((p_data == NULL) || (ota_struct->ota_flag.is_ota_process == false))
    {
        ota_struct->ota_temp_buf_used_size = 0;
        data[0] = DFU_ARV_FAIL_OPERATION;
        LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
        return;
    }

    LE_ARRAY_TO_UINT16(data_size, p_data);
    LE_ARRAY_TO_UINT16(crc, p_data + 2);

    APP_PRINT_TRACE4("app_ota_buffer_check_handle: data_size=%d, ota_temp_buf_used_size=%d, buffer_check_offset=%d, crc=0x%x",
                     data_size, ota_struct->ota_temp_buf_used_size, ota_struct->buffer_check_offset, crc);

    if (data_size != ota_struct->ota_temp_buf_used_size)
    {
        ota_struct->ota_temp_buf_used_size = 0;
        data[0] = DFU_ARV_FAIL_DATA_LENGTH_ERROR;
        LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
        return;
    }

    if (dfu_checkbufcrc(ota_struct->p_ota_temp_buf_head + ota_struct->buffer_check_offset,
                        ota_struct->ota_temp_buf_used_size, crc))
    {
        ota_struct->ota_temp_buf_used_size = 0;
        ota_struct->buffer_check_offset = 0;
        data[0] = DFU_ARV_FAIL_CRC_ERROR;
        LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
    }
    else
    {
        APP_PRINT_TRACE1("app_ota_buffer_check_handle: write flash, offset=0x%x", ota_struct->cur_offset);
        if (app_ota_enc_setting())
        {
            uint16_t offset = 0;
            while ((ota_struct->ota_temp_buf_used_size - offset) >= 16)
            {
                aes256_decrypt_16byte(ota_struct->p_ota_temp_buf_head + ota_struct->buffer_check_offset + offset);
                offset += 16;
            }
        }

        uint32_t flash_write_result = app_ota_write_to_flash(ota_struct->image_id,
                                                             ota_struct->cur_offset,
                                                             ota_struct->next_subimage_offset,
                                                             ota_struct->ota_temp_buf_used_size + ota_struct->buffer_check_offset,
                                                             ota_struct->p_ota_temp_buf_head);

        APP_PRINT_TRACE1("app_ota_buffer_check_handle: write ret: %d", flash_write_result);

        if (flash_write_result == 0)
        {
            ota_struct->cur_offset += ota_struct->ota_temp_buf_used_size;
            ota_struct->ota_temp_buf_used_size = 0;
            ota_struct->buffer_check_offset = 0;
            data[0] = DFU_ARV_SUCCESS;
            LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
        }
        else
        {
            uint32_t erase_time = 0;
            uint32_t resend_offset = (ota_struct->next_subimage_offset + ota_struct->cur_offset) /
                                     FMC_SEC_SECTION_LEN * FMC_SEC_SECTION_LEN;

            if (resend_offset < ota_struct->next_subimage_offset)
            {
                ota_struct->ota_temp_buf_used_size = 0;
                data[0] = DFU_ARV_FAIL_FLASH_ERASE_ERROR;
                LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
                return;
            }

            ota_struct->cur_offset = resend_offset - ota_struct->next_subimage_offset;

            while (erase_time < 3)
            {
                if (dfu_flash_erase(ota_struct->image_id, resend_offset) == true)
                {
                    ota_struct->ota_temp_buf_used_size = 0;
                    data[0] = DFU_ARV_FAIL_FLASH_WRITE_ERROR;
                    LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
                    break;
                }
                else
                {
                    erase_time++;
                }
            }
            if (erase_time >= 3)
            {
                ota_struct->ota_temp_buf_used_size = 0;
                data[0] = DFU_ARV_FAIL_FLASH_ERASE_ERROR;
                LE_UINT32_TO_ARRAY(&data[1], ota_struct->cur_offset);
            }
        }
    }
}

/**
    * @brief    Valid the image
    * @param    p_date     point of input data
    * @return   valid result
    */
static uint8_t app_ota_valid_handle(uint8_t *p_date)
{
    uint8_t results = DFU_ARV_SUCCESS;
    uint16_t image_id;

    if (!ota_struct->ota_flag.skip_flag
        && ota_struct->test.t_skip_fail)
    {
        results = DFU_ARV_FAIL_CRC_ERROR;
        return results;
    }

    ota_struct->ota_flag.skip_flag = 0;

    if (p_date == NULL)
    {
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
        return results;
    }

    LE_ARRAY_TO_UINT16(image_id, p_date);

    if (image_id == ota_struct->image_id)
    {
        if (!app_ota_checksum((IMG_ID)image_id, ota_struct->next_subimage_offset))
        {
            results = DFU_ARV_FAIL_CRC_ERROR;
        }
        else
        {
            ota_struct->next_subimage_offset += ota_struct->cur_offset;
            ota_struct->cur_offset = 0;
            if (is_ota_support_bank_switch())
            {
                ota_struct->next_subimage_offset = 0;
            }
#if (FORCE_OTA_TEMP == 1)
            uint8_t is_last_image;
            LE_ARRAY_TO_UINT8(is_last_image, p_date + 2);
            if (ota_struct->force_temp_mode && is_last_image)
            {
                dfu_set_ota_bank_flag(true);
            }
#endif
        }
    }
    else
    {
        results = DFU_ARV_FAIL_INVALID_PARAMETER;
    }

    return results;
}

/**
    * @brief    get image info for ota
    * @param    *p_data   point of input data
    * @param    *data   point of output data
    * @return   void
    */
static void app_ota_get_img_info_handle(uint8_t *p_data, uint8_t *data)
{
    uint16_t image_id;
    uint32_t dfu_base_addr;

    if ((data == NULL) || (p_data == NULL))
    {
        return;
    }

    LE_ARRAY_TO_UINT16(image_id, p_data);

    if (image_id != ota_struct->image_id)
    {
        ota_struct->cur_offset = 0;
        ota_struct->ota_temp_buf_used_size = 0;
    }

    if (check_image_id(image_id) == false)
    {
        data[0] = DFU_ARV_FAIL_INVALID_PARAMETER;
        return;
    }
    dfu_base_addr = app_get_inactive_bank_addr(image_id);
    if ((dfu_base_addr % FMC_SEC_SECTION_LEN) == 0)
    {
        ota_struct->buffer_check_offset = 0;
    }
    else
    {
        ota_struct->buffer_check_offset = dfu_base_addr % 0x1000;
    }

    data[0] = DFU_ARV_SUCCESS;
    LE_UINT32_TO_ARRAY(&data[1], 0);
    LE_UINT32_TO_ARRAY(&data[5], ota_struct->cur_offset);
    LE_UINT16_TO_ARRAY(&data[9], ota_struct->buffer_check_offset);

    APP_PRINT_TRACE2("app_ota_get_img_info_handle: cur_offset=0x%x, buffer_check_offset=0x%x",
                     ota_struct->cur_offset, ota_struct->buffer_check_offset);
}

/**
    * @brief    Disconnect peer for another OTA try
    * @param    conn_id     ID to identify the connection for disconnect
    * @return   void
    */
static void app_ota_le_disconnect(uint8_t conn_id, T_LE_LOCAL_DISC_CAUSE disc_cause)
{
    T_APP_LE_LINK *p_link;
    bool ret;

    p_link = app_link_find_le_link_by_conn_id(conn_id);
    ret = app_ble_gap_disconnect(p_link, disc_cause);

    APP_PRINT_TRACE1("app_ota_le_disconnect ret: %d", ret);
}

/**
    * @brief    Wrapper function to send notification to peer
    * @note
    * @param    conn_id     ID to identify the connection
    * @param    opcode      Notification on the specified opcode
    * @param    len         Notification data length
    * @param    data        Additional notification data
    * @return   void
    */
static void app_ota_service_prepare_send_notify(uint8_t conn_id, uint8_t opcode, uint16_t len,
                                                uint8_t *data)
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
    p_buffer = malloc(mtu_size);

    if (p_buffer == NULL)
    {
        return;
    }

    if (len < mtu_size - 2)
    {
        p_buffer[0] = DFU_OPCODE_NOTIF;
        p_buffer[1] = opcode;
        memcpy(&p_buffer[2], data, len);
        ota_service_send_notification(conn_id, p_buffer, len + 2);
        free(p_buffer);
        return;
    }

    while (remain_size)
    {
        if (remain_size == len)
        {
            p_buffer[0] = DFU_OPCODE_NOTIF;
            p_buffer[1] = opcode;
            memcpy(&p_buffer[2], p_data, mtu_size - 2);
            ota_service_send_notification(conn_id, p_buffer, mtu_size);
            p_data += (mtu_size - 2);
            remain_size -= (mtu_size - 2);
            continue;
        }

        send_len = (remain_size > mtu_size) ? mtu_size : remain_size;
        memcpy(p_buffer, p_data, send_len);
        ota_service_send_notification(conn_id, p_buffer, send_len);
        p_data += send_len;
        remain_size -= send_len;
    }

    free(p_buffer);
}

/**
    * @brief    copy image to the other bank
    * @param    p_data    point of input data
    * @return   1: success  other: fail
    */
static uint8_t app_ota_copy_img(uint8_t *p_data)
{
    uint32_t source_base_addr, dest_addr;
    uint32_t offset = 0;
    uint8_t ret = DFU_ARV_SUCCESS;
    uint16_t img_id;
    uint8_t *buffer_addr = ota_struct->p_ota_temp_buf_head;
    uint32_t buffer_size = MAX_BUFFER_SIZE;
    uint8_t *p_copy_buffer = NULL;
    uint8_t bp_lv;
    uint8_t size_factor = 0;

    ota_struct->ota_flag.skip_flag = 1;

    if (ota_struct->test.t_copy_fail)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    if (p_data == NULL)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    LE_ARRAY_TO_UINT16(img_id, p_data);

    if (check_copy_image_id(img_id))
    {
        ret = DFU_ARV_FAIL_INVALID_PARAMETER;
        return ret;
    }

    source_base_addr = app_get_active_bank_addr(img_id);
    dest_addr = app_get_inactive_bank_addr(img_id);

    if ((source_base_addr % FMC_SEC_SECTION_LEN) || (source_base_addr == 0))
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    T_IMG_HEADER_FORMAT *p_data_header = (T_IMG_HEADER_FORMAT *)source_base_addr;
    if (p_data_header->ctrl_header.image_id != img_id)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    uint32_t remain_size = p_data_header->ctrl_header.payload_len + IMG_HEADER_SIZE;
    ota_struct->image_total_length = remain_size;

    if (ota_struct->p_ota_temp_buf_head != NULL)
    {
        buffer_size = ota_struct->buffer_size;
    }
    else
    {
        while (p_copy_buffer == NULL && buffer_size)
        {
            buffer_size >>= size_factor;
            p_copy_buffer = (uint8_t *)malloc(buffer_size);
            size_factor ++;
        }

        buffer_addr = p_copy_buffer;
    }

    if (buffer_addr == NULL)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    fmc_flash_nor_get_bp_lv(dest_addr, &bp_lv);

    uint32_t copy_len;
    while (remain_size > 0)
    {
        copy_len = (remain_size > buffer_size) ? buffer_size : remain_size;

        fmc_flash_nor_read(source_base_addr + offset, buffer_addr, copy_len);
        uint32_t flash_write_result = app_ota_write_to_flash(img_id, offset, 0, copy_len, buffer_addr);

        if (flash_write_result)
        {
            APP_PRINT_TRACE1("app_ota_copy_img: write ret: %d", flash_write_result);
            ret = DFU_ARV_FAIL_FLASH_WRITE_ERROR;
            return ret;
        }
        remain_size -= copy_len;
        offset += copy_len;
        if (offset % COPY_WDG_KICK_SIZE == 0)//avoid WDT
        {
            WDG_Kick();
        }
    }

    if (app_ota_checksum((IMG_ID)img_id, 0) == false)
    {
        ret = DFU_ARV_FAIL_CRC_ERROR;
    }

    if (p_copy_buffer != NULL)
    {
        free(p_copy_buffer);
    }

    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), bp_lv);

    APP_PRINT_TRACE2("app_ota_copy_img ret: %d image_id: %x", ret, img_id);

    return ret;
}

/**
    * @brief    compare sha256 or crc value with the image in dest addr
    * @param    p_dest    address of the image
    * @param    p_data    sha256 or crc value receive from controler
    * @param    img_id    image id
    * @return   true: same  false: different
    */
static bool app_ota_cmp_checksum(T_IMG_HEADER_FORMAT *p_dest, uint8_t *p_data, IMG_ID img_id)
{
    uint8_t buffer[SHA256_LEN];

    if (check_image_id(img_id) == false)
    {
        return false;
    }

    fmc_flash_nor_read((uint32_t)&p_dest->auth.image_hash, buffer, SHA256_LEN);

    return (memcmp(p_data, buffer, SHA256_LEN) == 0);
}

/**
    * @brief    To check if the image in device is same with image needed to update
    * @param    p_data    point of input data
    * @param    data    point of output data
    * @return   void
    */
static void app_ota_check_sha256(uint8_t *p_data, uint8_t *data)
{
    T_IMG_HEADER_FORMAT *img_addr[2];
    uint16_t num = *(uint16_t *)p_data;
    uint8_t buffer[3] = {0};

    if (data == NULL)
    {
        return;
    }

    SHA256_CHECK *p_src = (SHA256_CHECK *)(p_data + 2);

    if (check_image_id(p_src->img_id) == false)
    {
        return;
    }

    for (uint16_t i = 0; i < num; i++)
    {
        memset(buffer, 0, sizeof(buffer));
        *(uint16_t *)buffer = p_src->img_id;

        img_addr[0] = (T_IMG_HEADER_FORMAT *)app_get_inactive_bank_addr(p_src->img_id);
        img_addr[1] = (T_IMG_HEADER_FORMAT *)app_get_active_bank_addr(p_src->img_id);

        for (uint8_t j = 0; j < 2; j++)
        {
            if (img_addr[j] && img_addr[j]->ctrl_header.image_id == p_src->img_id
                && app_ota_cmp_checksum(img_addr[j], p_src->sha256, p_src->img_id))
            {
                buffer[2] += (j + 1);
            }
        }
        APP_PRINT_TRACE2("app_ota_check_sha256 img_id: %x buffer[2]: %x, 0: write, 1,3: skip, 2: copy",
                         *(uint16_t *)buffer, buffer[2]);

        memcpy(data, buffer, sizeof(buffer));
        data += sizeof(buffer);
        p_src++;
    }
}

/**
    * @brief    get the ic type of current fw
    * @param    void
    * @return   ic type
    */
static uint8_t app_ota_get_ic_type(void)
{
    uint8_t ic_type = IC_TYPE;
    uint32_t image_addr = app_get_active_bank_addr(IMG_MCUAPP);

    ic_type = ((T_IMG_HEADER_FORMAT *)image_addr)->ctrl_header.ic_type;

    return ic_type;
}

/**
    * @brief    clear flag and save ftl before reboot
    * @param    void
    * @return   void
    */
static void app_ota_prepare_to_reboot(void)
{
    app_ota_clear_notready_flag();

    app_ota_exit();

    app_ota_clear_local(OTA_SUCCESS_REBOOT);

#if F_APP_DURIAN_SUPPORT
    app_durian_cfg_power_off();
#endif

    if (app_cfg_store_all() != 0)
    {
        APP_PRINT_ERROR0("app_ota_prepare_to_reboot: save nv cfg data error");
    }
}

/**
    * @brief    erase whole image
    * @param    start_address  start address
    * @return   void
    */
static void app_ota_erase_whole_image(uint32_t start_address)
{
    uint32_t erase_size = 0;
    uint32_t i, erase_block_count, erase_sector_count;
    uint32_t erase_total_size = ota_struct->image_total_length + ota_struct->buffer_check_offset;

    /*start address is 64k aligned*/
    while (start_address % FMC_SEC_BLOCK_LEN)
    {
        APP_PRINT_TRACE1("app_ota_erase_whole_image erase sector start_address 0x%x", start_address);
        fmc_flash_nor_erase(start_address, FMC_FLASH_NOR_ERASE_SECTOR);
        erase_size += FMC_SEC_SECTION_LEN;
        start_address += FMC_SEC_SECTION_LEN;

        if (erase_size >= erase_total_size)
        {
            return;
        }
    }
    erase_total_size -= erase_size;
    uint32_t remained_block_size = erase_total_size % FMC_SEC_BLOCK_LEN;

    if ((remained_block_size + FMC_SEC_BLOCK_LEN) <= erase_total_size)
    {
        erase_sector_count = remained_block_size / FMC_SEC_SECTION_LEN;
        erase_block_count = erase_total_size / FMC_SEC_BLOCK_LEN;

        if (remained_block_size % FMC_SEC_SECTION_LEN)
        {
            erase_sector_count++;
        }
        for (i = 0; i < erase_block_count; i++)
        {
            APP_PRINT_TRACE1("app_ota_erase_whole_image erase block start_address 0x%x", start_address);
            fmc_flash_nor_erase(start_address, FMC_FLASH_NOR_ERASE_BLOCK);
            start_address += FMC_SEC_BLOCK_LEN;
        }
        for (i = 0; i < erase_sector_count; i++)
        {
            APP_PRINT_TRACE1("app_ota_erase_whole_image erase sector start_address 0x%x", start_address);
            fmc_flash_nor_erase(start_address, FMC_FLASH_NOR_ERASE_SECTOR);
            start_address += FMC_SEC_SECTION_LEN;
        }
    }
    else
    {
        erase_sector_count = erase_total_size / FMC_SEC_SECTION_LEN;

        if (remained_block_size % FMC_SEC_SECTION_LEN)
        {
            erase_sector_count++;
        }
        for (i = 0; i < erase_sector_count; i++)
        {
            APP_PRINT_TRACE1("app_ota_erase_whole_image erase sector start_address 0x%x", start_address);
            fmc_flash_nor_erase(start_address, FMC_FLASH_NOR_ERASE_SECTOR);
            start_address += FMC_SEC_SECTION_LEN;
        }
    }
}
/*============================================================================*
 *                              Public Functions
 *============================================================================*/
/**
    * @brief    check the integrity of the image
    * @param    image_id    image id
    * @param    offset  address offset
    * @return   ture:success ; false: fail
    */
bool app_ota_checksum(IMG_ID image_id, uint32_t offset)
{
    uint32_t base_addr = 0;
    bool ret = false;

    base_addr = app_get_inactive_bank_addr(image_id);

    if (base_addr == 0)
    {
        return false;
    }

    base_addr += offset ;
    ret = check_image_sum(base_addr);

    if (ret == true && !is_ota_support_bank_switch())
    {
        app_ota_set_ready(base_addr);
        //btaon_fast_write_safe(BTAON_FAST_AON_GPR_15, image_id);
    }

    APP_PRINT_TRACE2("dfu_check_checksum base_addr:%x, ret:%d", base_addr, ret);

    return ret;
}

/**
    * @brief    write data to flash
    * @param    img_id  image id
    * @param    offset  image offset
    * @param    total_offset  total offset when ota temp mode
    * @param    p_void  point of data
    * @return   0: success; other: fail
    */
uint32_t app_ota_write_to_flash(uint16_t img_id, uint32_t offset, uint32_t total_offset,
                                uint32_t length, void *p_void)
{
    uint32_t dfu_base_addr;
    uint8_t readback_buffer[READBACK_BUFFER_SIZE];
    uint32_t read_back_len;
    uint32_t dest_addr;
    uint8_t *p_src = (uint8_t *)p_void;
    uint32_t remain_size = length;

    if (p_void == 0)
    {
        return __LINE__;
    }

    dfu_base_addr = app_get_inactive_bank_addr(img_id);

    if (dfu_base_addr == 0)
    {
        return __LINE__;
    }

    if (ota_struct->buffer_check_offset != 0)
    {
        dfu_base_addr -= ota_struct->buffer_check_offset;
        fmc_flash_nor_read(dfu_base_addr, ota_struct->p_ota_temp_buf_head, ota_struct->buffer_check_offset);
    }

    dfu_base_addr += total_offset;

    if (offset == 0)
    {
        T_IMG_HEADER_FORMAT *p_header = (T_IMG_HEADER_FORMAT *)p_void;

        p_header->ctrl_header.ctrl_flag.not_ready = 0x1;
        APP_PRINT_TRACE1("app_ota_write_to_flash dfu_base_addr:0x%x", dfu_base_addr);
        app_ota_erase_whole_image(dfu_base_addr);
    }


    if ((!ota_struct->ota_flag.buffer_check_en) && (ota_struct->buffer_check_offset != 0))
    {
        dest_addr = dfu_base_addr + offset + ota_struct->buffer_check_offset;
        ota_flash_dma_write(dest_addr, p_void, length);
        ota_flash_dma_write(dest_addr - ota_struct->buffer_check_offset, ota_struct->p_ota_temp_buf_head,
                            ota_struct->buffer_check_offset);
    }
    else
    {
        dest_addr = dfu_base_addr + offset;
        ota_flash_dma_write(dest_addr, p_void, length);
    }
    extern void cache_flush_by_addr(uint32_t *addr, uint32_t length);
    cache_flush_by_addr((uint32_t *)dest_addr, length);

    while (remain_size)
    {
        read_back_len = (remain_size >= READBACK_BUFFER_SIZE) ? READBACK_BUFFER_SIZE : remain_size;

        fmc_flash_nor_read(dest_addr, readback_buffer, read_back_len);
        if (memcmp(readback_buffer, p_src, read_back_len) != 0)
        {
            return __LINE__;
        }

        dest_addr += read_back_len;
        p_src += read_back_len;
        remain_size -= read_back_len;
    }

    APP_PRINT_TRACE1("app_ota_write_to_flash dest_addr: %x", dfu_base_addr + offset);
    return 0;
}

/**
    * @brief    Used to get image version for ota dongle
    * @param    *p_data   point of image version
    * @return   void
    */
void app_ota_get_brief_img_version_for_dongle(uint8_t *p_data)
{
    T_IMG_HEADER_FORMAT *addr;
    uint16_t img_id;
    uint8_t buffer[4] = {0};
    uint8_t buffer_ota_header[4] = {0};
    uint8_t *p_temp = p_data;
    uint32_t size;
    T_IMG_HEADER_FORMAT *ota_header;

    for (IMG_ID i = IMG_OTA; i < IMG_EXT1; i++)
    {
        img_id = app_change_seq_appdata_dspdata(i);

        addr = (T_IMG_HEADER_FORMAT *)app_get_active_bank_addr(img_id);

        if (img_id == IMG_OTA)
        {
            ota_header = (T_IMG_HEADER_FORMAT *)addr;
        }

        size = ota_header->image_info[(img_id - IMG_SBL) * 2 + 1];

        if (size == 0 || addr == 0 || size == 0xffffffff)
        {
            *(uint32_t *)&buffer[0] = 0xFFFFFFFF;
        }
        else
        {
            if (img_id == IMG_OTA)
            {
                *(uint32_t *)&buffer[0] = addr->ver_val;
            }
            else if (img_id > IMG_OTA)
            {
                if (addr->ctrl_header.image_id == img_id)
                {
                    *(uint32_t *)&buffer[0] = addr->git_ver.version;
                }
            }
        }

        if (img_id == IMG_OTA)
        {
            // inorder to fit OTA dongle's format, OTA header need to placed at the end
            memcpy(buffer_ota_header, buffer, sizeof(buffer));
        }
        else
        {
            memcpy(p_temp, buffer, sizeof(buffer));
            p_temp += sizeof(buffer);
        }
    }

    // copy ota header to the tail
    memcpy(p_temp, buffer_ota_header, sizeof(buffer_ota_header));
    p_temp += sizeof(buffer_ota_header);
}

/**
    * @brief    Used to get device information
    * @param    p_data    point of device info data
    * @param    ota_mode  spp ota or ble ota
    * @return   void
    */
void app_ota_get_device_info(DEVICE_INFO *p_deviceinfo)
{
    if (p_deviceinfo == NULL)
    {
        return;
    }

    if (ota_struct == NULL)
    {
        ota_struct = (T_APP_OTA_DB *)calloc(1, sizeof(T_APP_OTA_DB));
    }

    memset(p_deviceinfo, 0, sizeof(DEVICE_INFO));
    p_deviceinfo->ic_type = app_ota_get_ic_type();
    p_deviceinfo->mode.buffercheck_en = ota_struct->test.t_buffercheck_disable ? 0 : 1;
    p_deviceinfo->mode.aes_en = app_ota_enc_setting();
    p_deviceinfo->mode.aes_mode = 1;
    p_deviceinfo->mode.support_multiimage = 1;
    p_deviceinfo->status.b2b_status = app_ota_check_ota_mode();
    p_deviceinfo->status.fc_bud_role = app_cfg_const.bud_role;
    p_deviceinfo->ota_temp_size = flash_partition_size_get(PARTITION_FLASH_OTA_TMP) / UINT_4K;
    p_deviceinfo->banknum = app_ota_get_active_bank();
    p_deviceinfo->mtu_size = MTU_SIZE;

    ota_struct->ota_flag.is_devinfo = 1;
    ota_struct->ota_flag.is_rws = (p_deviceinfo->status.b2b_status == RWS_B2B_CONNECT);

    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            &ota_struct->bp_level);
}

/**
    * @brief  The main function to handle all the spp ota command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  app_idx received rx command device index
    * @return void
    */
void app_ota_cmd_handle(uint8_t path, uint16_t length, uint8_t *p_value, uint8_t app_idx)
{
    uint8_t ack_pkt[3];
    uint16_t cmd_id = *(uint16_t *)p_value;
    uint8_t *p;
    uint8_t results = DFU_ARV_SUCCESS;

    bool ack_flag = false;

    ack_pkt[0] = p_value[0];
    ack_pkt[1] = p_value[1];
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    if (length < 2)
    {
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
        APP_PRINT_ERROR0("app_ota_cmd_handle: error length");
        return;
    }

    length = length - 2;
    p = p_value + 2;

    APP_PRINT_TRACE2("===>app_ota_cmd_handle, cmd_id:%x, length:%x\n", cmd_id, length);

    if (ota_struct == NULL)
    {
        ota_struct = (T_APP_OTA_DB *)calloc(1, sizeof(T_APP_OTA_DB));
    }

    if (ota_struct->ota_flag.is_ota_process)
    {
        app_start_timer(&timer_idx_ota_transfer, "ota_transfer",
                        ota_timer_id, APP_TIMER_OTA_TRANSFER, 0, false,
                        30 * 1000);
    }

    switch (cmd_id)
    {
    case CMD_OTA_DEV_INFO:
        {
            if (length == OTA_LENGTH_OTA_GET_INFO)
            {
                DEVICE_INFO dev_info;

                memcpy(ota_struct->bd_addr, app_db.br_link[app_idx].bd_addr, sizeof(ota_struct->bd_addr));
                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ota_get_device_info(&dev_info);
                dev_info.spec_ver = SPP_OTA_VERSION;
                app_report_event(path, EVENT_OTA_DEV_INFO, app_idx, (uint8_t *)&dev_info, sizeof(dev_info));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_IMG_VER:
        {
            if (length == OTA_LENGTH_OTA_GET_IMG_VER)
            {
                uint8_t data[IMG_INFO_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                data[0] = *p;
                app_ota_get_img_version(&data[1], data[0]);
                app_report_event(path, EVENT_OTA_ACTIVE_BANK_VER, app_idx, (uint8_t *)data,
                                 IMG_INFO_LEN);
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_START:
        {
            if (length == OTA_LENGTH_START_OTA)
            {
                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                results  = app_ota_start_dfu_handle(p);
                app_sniff_mode_b2s_disable(ota_struct->bd_addr, SNIFF_DISABLE_MASK_OTA);
                app_report_event(path, EVENT_OTA_START, app_idx, &results, sizeof(results));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_PACKET:
        {
            results = app_ota_data_packet_handle(p, length);
            if (results == DFU_ARV_FAIL_INVALID_PARAMETER)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else if (results == DFU_ARV_FAIL_OPERATION)
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    case CMD_OTA_VALID:
        {
            if (length == OTA_LENGTH_VALID_FW)
            {
                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                results = app_ota_valid_handle(p);
                app_report_event(path, EVENT_OTA_VALID, app_idx, &results, sizeof(results));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_ACTIVE_RESET:
        {
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            if (app_ota_check_ota_mode() != RWS_B2B_CONNECT && ota_struct->ota_flag.is_rws)
            {
                results = DFU_ARV_FAIL_OPERATION;
            }
#if (F_APP_SENSOR_SUPPORT == 1)
            if (app_cfg_const.sensor_support)
            {
                app_sensor_ld_stop();
            }
#endif
            app_report_event(path, EVENT_OTA_ACTIVE_ACK, app_idx, &results, 1);
        }
        break;
    case CMD_OTA_ROLESWAP:
        {
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            ota_struct->ota_flag.roleswap = *p;
            if (app_ota_check_ota_mode() != RWS_B2B_CONNECT)
            {
                results = DFU_ARV_FAIL_OPERATION;
            }
            app_report_event(path, EVENT_OTA_ROLESWAP, app_idx, &results, 1);
        }
        break;
    case CMD_OTA_RESET:
        {
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_ota_error_clear_local(OTA_RESET_CMD);
        }
        break;
    case CMD_OTA_IMG_INFO:
        {
            if (length == OTA_LENGTH_IMAGE_INFO)
            {
                uint8_t data[TARGET_INFO_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ota_get_img_info_handle(p, data);
                app_report_event(path, EVENT_OTA_IMG_INFO, app_idx, data, sizeof(data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_BUFFER_CHECK_ENABLE:
        {
            if (length  == OTA_LENGTH_BUFFER_CHECK_EN)
            {
                uint8_t data[BUFFER_CHECK_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ota_buffer_check_en(data, MAX_BUFFER_SIZE);
                app_report_event(path, EVENT_OTA_BUFFER_CHECK_ENABLE, app_idx, data, sizeof(data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_BUFFER_CHECK:
        {
            if (length  == OTA_LENGTH_BUFFER_CRC)
            {
                uint8_t data[BUFFER_CHECK_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ota_buffer_check_handle(p, &data[0]);
                app_report_event(path, EVENT_OTA_BUFFER_CHECK, app_idx, data, sizeof(data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_SECTION_SIZE:
        {
            if (length == OTA_LENGTH_SECTION_SIZE)
            {
                uint8_t data[IMG_INFO_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ota_get_section_size(data);
                app_report_event(path, EVENT_OTA_SECTION_SIZE, app_idx, (uint8_t *)data,
                                 IMG_INFO_LEN);
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_DEV_EXTRA_INFO:
        {
            if (length  == OTA_LRNGTH_OTHER_INFO)
            {
                uint8_t data[EXTRA_INFO_LEN] = {0};

                app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(path, EVENT_OTA_DEV_EXTRA_INFO, app_idx, data, sizeof(data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;
    case CMD_OTA_PROTOCOL_TYPE:
        {
            uint8_t data[SPEC_VER_LEN] = {0};

            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            *(uint16_t *)data = SPP_PROTOCOL_INFO;
            data[2] = SPP_OTA_VERSION;
            app_report_event(path, EVENT_OTA_PROTOCOL_TYPE, app_idx, (uint8_t *)&data, SPEC_VER_LEN);
        }
        break;
    case CMD_OTA_GET_RELEASE_VER:
        {
            uint8_t data[RELEASE_VER_LEN * 2] = {0};
            uint32_t p_imgheader;

            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);

            p_imgheader = app_get_active_bank_addr(IMG_MCUCONFIG);
            memcpy(data, (uint8_t *)(p_imgheader + RELEASE_VER_OFFSET), RELEASE_VER_LEN);
            p_imgheader = app_get_active_bank_addr(IMG_ANC);
            memcpy(data + RELEASE_VER_LEN, (uint8_t *)(p_imgheader + RELEASE_VER_OFFSET), RELEASE_VER_LEN);

            app_report_event(path, EVENT_OTA_GET_RELEASE_VER, app_idx, data, sizeof(data));
        }
        break;
    case CMD_OTA_COPY_IMG:
        {
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            results = app_ota_copy_img(p);
            app_report_event(path, EVENT_OTA_COPY_IMG, app_idx, &results, sizeof(results));
        }
        break;
    case CMD_OTA_CHECK_SHA256:
        {
            uint8_t data[CHECK_SHA256_LEN] = {0};
            uint16_t num = *(uint16_t *)p;

            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_ota_check_sha256(p, data);
            app_report_event(path, EVENT_OTA_CHECK_SHA256, app_idx, data, num * 3);
        }
        break;
    case CMD_OTA_TEST_EN:
        {
            app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
            memcpy(&ota_struct->test.value, p, sizeof(ota_struct->test.value));
        }
        break;
    default:
        ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
        app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
        break;
    }

    if (ack_flag == true)
    {
        APP_PRINT_TRACE0("app_ota_cmd_handle: invalid length");
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
    }
}

/**
    * @brief  handle the active reset event ack
    * @param  event_id  event id of ack
    * @param  status  the status of the ack
    * @return void
    */
void app_ota_cmd_ack_handle(uint16_t event_id, uint8_t status)
{
    APP_PRINT_TRACE2("app_ota_cmd_ack_handle: event_id: %x, status: %d", event_id, status);
    if (status == CMD_SET_STATUS_COMPLETE)
    {
        if (app_ota_check_ota_mode() != RWS_B2B_CONNECT)
        {
            if (ota_struct->ota_flag.is_rws)
            {
                app_ota_error_clear_local(OTA_B2B_DISC);
                return;
            }
        }

        if (event_id == EVENT_OTA_ACTIVE_ACK)
        {
            app_stop_timer(&timer_idx_ota_transfer);
            ota_struct->ota_flag.is_ota_process = false;
            if (app_ota_check_ota_mode() != RWS_B2B_CONNECT)
            {
                app_ota_prepare_to_reboot();
                app_ota_disc_b2s_profile();
                app_start_timer(&timer_idx_ota_reset, "ota_reset",
                                ota_timer_id, APP_TIMER_OTA_RESET, 0, false,
                                3000);
            }
            else
            {
                APP_PRINT_TRACE0("app_ota_cmd_ack_handle:rws active reset");
                ota_struct->rws_mode.valid_ret.cur_bud = 1;
                app_ota_rws_send_msg(SPP_OTA_MODE, RWS_OTA_UPDATE_RET, UPDATE_SUCCESS);
            }
        }
        else if (event_id == EVENT_OTA_ROLESWAP)
        {
            APP_PRINT_TRACE1("app_ota_cmd_ack_handle roleswap mdoe: %d", ota_struct->ota_flag.roleswap);

            app_stop_timer(&timer_idx_ota_transfer);
            if (!ota_struct->ota_flag.roleswap)
            {
                ota_struct->rws_mode.valid_ret.cur_bud = 1;
                ota_struct->ota_flag.is_ota_process = false;
                app_ota_rws_send_msg(SPP_OTA_MODE, RWS_OTA_UPDATE_RET, UPDATE_SUCCESS);
            }
            ota_struct->ota_flag.roleswap = 0;
            app_mmi_handle_action(MMI_START_ROLESWAP);
        }
    }
}

/**
    * @brief    Handle written request on DFU control point characteristic
    * @param    conn_id     ID to identify the connection
    * @param    length      Length of value to be written
    * @param    p_value     Value to be written
    * @return   T_APP_RESULT
    * @retval   Handle result of this request
    */
T_APP_RESULT app_ota_ble_handle_cp_req(uint8_t conn_id, uint16_t length, uint8_t *p_value)
{
    T_APP_RESULT cause = APP_RESULT_INVALID_PDU;
    uint8_t results = DFU_ARV_SUCCESS;
    uint8_t opcode = *p_value;
    uint8_t *p = p_value + 1;
    bool error_flag = false;

    APP_PRINT_INFO2("===>app_ota_ble_handle_cp_req, opcode:%x, length:%x\n", opcode, length);

    if (opcode > DFU_OPCODE_MIN && opcode <= DFU_OPCODE_TEST_EN
        && ota_struct->ota_flag.is_ota_process)
    {
        app_start_timer(&timer_idx_ota_transfer, "ota_transfer",
                        ota_timer_id, APP_TIMER_OTA_TRANSFER, 0, false,
                        30 * 1000);
    }

    switch (opcode)
    {
    case DFU_OPCODE_START_DFU:
        {
            if (length == DFU_LENGTH_START_DFU + DATA_PADDING)   /* 4 bytes is pending for encrypt */
            {
                cause = APP_RESULT_SUCCESS;
                results = app_ota_start_dfu_handle(p);
                app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_START_DFU, sizeof(results), &results);
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_RECEIVE_FW_IMAGE_INFO:
        {
            if (length == DFU_LENGTH_RECEIVE_FW_IMAGE_INFO)
            {
                uint16_t image_id;

                LE_ARRAY_TO_UINT16(image_id, p);
                cause = APP_RESULT_SUCCESS;
                if (image_id == ota_struct->image_id)
                {
                    LE_ARRAY_TO_UINT32(ota_struct->cur_offset, p + 2);
                    APP_PRINT_TRACE2("app_ota_ble_handle_cp_req: image_id = 0x%x, nCurOffSet = %d",
                                     image_id, ota_struct->cur_offset);
                }
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_VALID_FW:
        {
            if (length == DFU_LENGTH_VALID_FW)
            {
                cause = APP_RESULT_SUCCESS;
                results = app_ota_valid_handle(p);
                app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_VALID_FW, sizeof(results), &results);
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_ACTIVE_IMAGE_RESET:
        {
            cause = APP_RESULT_SUCCESS;
            ota_struct->ota_flag.is_ota_process = false;
            app_stop_timer(&timer_idx_ota_transfer);
            if (app_ota_check_ota_mode() != RWS_B2B_CONNECT)
            {
                if (ota_struct->ota_flag.is_rws)
                {
                    cause = APP_RESULT_APP_ERR;
                    app_ota_error_clear_local(OTA_B2B_DISC);
                    break;
                }

                app_ota_prepare_to_reboot();

                app_ota_le_disconnect(conn_id, LE_LOCAL_DISC_CAUSE_OTA_RESET);
            }
            else
            {
                app_ota_le_disconnect(conn_id, LE_LOCAL_DISC_CAUSE_RWS_OTA_SUCCESS);
                ota_struct->rws_mode.valid_ret.cur_bud = 1;
                app_ota_rws_send_msg(BLE_OTA_MODE, RWS_OTA_UPDATE_RET, UPDATE_SUCCESS);
            }
        }
        break;
    case DFU_OPCODE_SYSTEM_RESET:
        {
            cause = APP_RESULT_SUCCESS;
            app_ota_error_clear_local(OTA_RESET_CMD);
            app_ota_le_disconnect(conn_id, LE_LOCAL_DISC_CAUSE_UNKNOWN);
        }
        break;
    case DFU_OPCODE_REPORT_TARGET_INFO:
        {
            if (length == DFU_LENGTH_REPORT_TARGET_INFO)
            {
                uint8_t notif_data[TARGET_INFO_LEN] = {0};

                cause = APP_RESULT_SUCCESS;
                app_ota_get_img_info_handle(p, notif_data);
                app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_REPORT_TARGET_INFO, TARGET_INFO_LEN,
                                                    notif_data);
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_BUFFER_CHECK_EN:
        {
            if (length  == DFU_LENGTH_BUFFER_CHECK_EN)
            {
                uint8_t notif_data[BUFFER_CHECK_LEN] = {0};

                cause = APP_RESULT_SUCCESS;
                app_ota_buffer_check_en(notif_data, MAX_BUFFER_SIZE);
                app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_BUFFER_CHECK_EN, BUFFER_CHECK_LEN,
                                                    notif_data);
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_REPORT_BUFFER_CRC:
        {
            if (length  == DFU_LENGTH_REPORT_BUFFER_CRC)
            {
                uint8_t notif_data[BUFFER_CHECK_LEN] = {0};

                cause = APP_RESULT_SUCCESS;
                app_ota_buffer_check_handle(p, notif_data);
                app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_REPORT_BUFFER_CRC, BUFFER_CHECK_LEN,
                                                    notif_data);
            }
            else
            {
                error_flag = true;
            }
        }
        break;
    case DFU_OPCODE_COPY_IMG:
        {
            cause = APP_RESULT_SUCCESS;
            results = app_ota_copy_img(p);
            app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_COPY_IMG, 1, &results);
        }
        break;
    case DFU_OPCODE_GET_IMAGE_VER:
        {
            uint8_t notif_data[IMG_INFO_LEN] = {0};

            cause = APP_RESULT_SUCCESS;
            notif_data[0] = *p;
            app_ota_get_img_version(&notif_data[1], notif_data[0]);
            app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_GET_IMAGE_VER, IMG_INFO_LEN, notif_data);
        }
        break;
    case DFU_OPCODE_GET_SECTION_SIZE:
        {
            uint8_t notif_data[IMG_INFO_LEN] = {0};

            cause = APP_RESULT_SUCCESS;
            app_ota_get_section_size(notif_data);
            app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_GET_SECTION_SIZE, IMG_INFO_LEN, notif_data);
        }
        break;
    case DFU_OPCODE_CHECK_SHA256:
        {
            uint8_t notif_data[CHECK_SHA256_LEN] = {0};
            uint16_t num = *(uint16_t *)p;

            cause = APP_RESULT_SUCCESS;
            app_ota_check_sha256(p, notif_data);
            app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_CHECK_SHA256, num * 3, notif_data);
        }
        break;
    case DFU_OPCODE_GET_RELEASE_VER:
        {
            uint8_t notif_data[RELEASE_VER_LEN] = {0};
            uint32_t p_imgheader = app_get_active_bank_addr(IMG_MCUCONFIG);

            cause = APP_RESULT_SUCCESS;
            memcpy(notif_data, (uint8_t *)(p_imgheader + RELEASE_VER_OFFSET), RELEASE_VER_LEN);
            app_ota_service_prepare_send_notify(conn_id, DFU_OPCODE_GET_RELEASE_VER, RELEASE_VER_LEN,
                                                notif_data);
        }
        break;
    case DFU_OPCODE_TEST_EN:
        {
            cause = APP_RESULT_SUCCESS;
            memcpy(&ota_struct->test.value, p, sizeof(ota_struct->test.value));
        }
        break;
    default:
        APP_PRINT_ERROR1("app_ota_ble_handle_cp_req, opcode not expected", opcode);
        break;
    }

    if (error_flag)
    {
        APP_PRINT_ERROR0("app_ota_ble_handle_cp_req: invalid length");
    }

    return cause;
}

/**
    * @brief    Handle written request on DFU packet characteristic
    * @param    conn_id     ID to identify the connection
    * @param    length      Length of value to be written
    * @param    p_value     Value to be written
    * @return   T_APP_RESULT
    * @retval   Handle result of this request
    */
T_APP_RESULT app_ota_ble_handle_packet(uint8_t conn_id, uint16_t length, uint8_t *p_value)
{
    uint8_t result;

    result = app_ota_data_packet_handle(p_value, length);

    if (result == DFU_ARV_FAIL_INVALID_PARAMETER)
    {
        return APP_RESULT_INVALID_PDU;
    }
    else if (result == DFU_ARV_FAIL_OPERATION)
    {
        return APP_RESULT_APP_ERR;
    }

    return APP_RESULT_SUCCESS;
}

#if F_APP_ERWS_SUPPORT
uint16_t app_ota_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    // uint16_t payload_len = 0;
    // uint8_t *msg_ptr = NULL;
    // bool skip = true;

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_OTA, 0, NULL, true, total);
}

static void app_ota_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_OTA_VALID_SYNC:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_ota_handle_remote_msg(msg_type, (void *)buf, len);
        }
        break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case APP_REMOTE_MSG_OTA_PARKING:
        {
            app_cmd_ota_tooling_parking();
        }
        break;

    case APP_REMOTE_MSG_OTA_TOOLING_POWER_OFF:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_OTA_TOOL;
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
        }
        break;
#endif

    default:
        break;
    }
}
#endif

/**
    * @brief  ota callback register
    * @param  void
    * @return void
    */
void app_ota_init(void)
{
    app_timer_reg_cb(app_ota_timeout_cb, &ota_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_ota_relay_cback, app_ota_parse_cback,
                             APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_TOTAL);
#endif
    ota_flash_dma_write_init();

}

/**
    * @brief  get image size of bank area
    * @param  image_id image ID
    * @return size
    */
uint32_t get_bank_size_by_img_id(IMG_ID image_id)
{
    uint32_t bank_size;

    T_IMG_HEADER_FORMAT *ota_header = (T_IMG_HEADER_FORMAT *)get_active_ota_bank_addr();
    bank_size = ota_header->image_info[(image_id - IMG_SBL) * 2 + 1];

    return bank_size;
}

/**
 * \brief  Handle remote msg from remote device.
 *
 * \param[in] msg   Message type.
 * \param[in] buf   Message buffer.
 * \param[in] buf   Message buffer.
 */
void app_ota_handle_remote_msg(uint8_t msg, void *buf, uint16_t len)
{

    if (ota_struct == NULL)
    {
        ota_struct = (T_APP_OTA_DB *)calloc(1, sizeof(T_APP_OTA_DB));
    }

    switch (msg)
    {
    case APP_REMOTE_MSG_OTA_VALID_SYNC:
        {
            RWS_MESSAGE_FORMAT rws_msg;

            rws_msg.ota_mode = *(uint8_t *)buf;
            rws_msg.cmd_id = *((uint8_t *)buf + 1);
            rws_msg.data = *((uint8_t *)buf + 2);

            APP_PRINT_TRACE2("app_ota_handle_remote_msg: cmd_id: %d sync_data: %d", rws_msg.cmd_id,
                             rws_msg.data);

            if ((rws_msg.cmd_id >= RWS_OTA_UPDATE_RET) && (rws_msg.cmd_id < RWS_OTA_MAX))
            {
                app_ota_rws_send_msg(rws_msg.ota_mode, RWS_OTA_ACK, rws_msg.cmd_id);
            }

            switch (rws_msg.cmd_id)
            {
            case RWS_OTA_ACK:
                {
                    if (rws_msg.data == RWS_OTA_HANDSHAKE)
                    {
                        app_ota_prepare_to_reboot();

                        app_ota_disc_b2s_profile();

                        app_start_timer(&timer_idx_ota_reset, "ota_reset",
                                        ota_timer_id, APP_TIMER_OTA_RESET, 0, false,
                                        3000);
                    }
                    app_stop_timer(&timer_idx_ota_rws_sync);
                    ota_struct->rws_mode.retry_times = 0;
                }
                break;
            case RWS_OTA_UPDATE_RET:
                {
                    if (rws_msg.data == UPDATE_SUCCESS)
                    {
                        ota_struct->rws_mode.valid_ret.oth_bud = 1;
                        if (ota_struct->rws_mode.valid_ret.cur_bud)
                        {
                            app_ota_rws_send_msg(rws_msg.ota_mode, RWS_OTA_HANDSHAKE, 0);
                        }
                        else
                        {
                            ota_struct->ota_flag.is_ota_process = true;
                            if (rws_msg.ota_mode == BLE_OTA_MODE)
                            {
                                APP_PRINT_TRACE0("app_ota_handle_remote_msg enable ble adv");
                                app_ble_common_adv_ota_data_update();
                                app_ble_common_adv_start(18000);
                            }
                        }
                    }
                    else if (rws_msg.data == UPDATE_FAIL)
                    {
                        ota_struct->rws_mode.valid_ret.oth_bud = 0;
                        if (ota_struct->rws_mode.valid_ret.cur_bud)
                        {
                            app_ota_error_clear_local(OTA_OTHBUD_FAIL);
                        }
                    }
                }
                break;
            case RWS_OTA_HANDSHAKE:
                {
                    app_ota_prepare_to_reboot();

                    app_ota_disc_b2s_profile();

                    app_start_timer(&timer_idx_ota_reset, "ota_reset",
                                    ota_timer_id, APP_TIMER_OTA_RESET, 0, false,
                                    3000);
                }
                break;
            default:
                APP_PRINT_ERROR0("app_ota_handle_remote_msg: cmd id error");
                break;
            }
        }
        break;

    default:
        break;
    }
}

void app_ota_le_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause, uint16_t disc_cause)
{
    if (app_cfg_const.rtk_app_adv_support)
    {
        if (local_disc_cause == LE_LOCAL_DISC_CAUSE_SWITCH_TO_OTA)
        {
            if (app_cfg_store_all() != 0)
            {
                APP_PRINT_ERROR0("le_handle_new_conn_state_evt: save nv cfg data error");
            }

            APP_PRINT_TRACE0("Switch into OTA mode now...");
            //dfu_switch_to_ota_mode();
            return;
        }
        else if (local_disc_cause == LE_LOCAL_DISC_CAUSE_OTA_RESET)
        {
            app_ota_disc_b2s_profile();
            app_start_timer(&timer_idx_ota_reset, "ota_reset",
                            ota_timer_id, APP_TIMER_OTA_RESET, 0, false,
                            3000);
            return;
        }
        else if (local_disc_cause == LE_LOCAL_DISC_CAUSE_UNKNOWN)
        {
            if (app_ota_link_loss_stop())
            {
                app_ota_error_clear_local(OTA_BLE_DISC);
            }
        }
    }
}

/**
    * @brief  get ota status
    * @return True:is doing ota; False: is not doing ota
    */
bool app_ota_dfu_is_busy(void)
{
    if (ota_struct == NULL)
    {
        return false;
    }
    else
    {
        return ota_struct->ota_flag.is_ota_process;
    }
}

/**
    * @brief  link loss handle flag
    * @return True:is doing ota; False: is not doing ota
    */
bool app_ota_link_loss_stop(void)
{
    if (ota_struct == NULL)
    {
        return false;
    }
    else
    {
        return ota_struct->ota_flag.is_ota_process;
    }
}

/**
 * @brief erase free bank.
 *
 * @param
 * @return
*/
static void app_ota_erase_temp_bank_header(void)
{
    if (is_ota_support_bank_switch())
    {
        uint32_t ota_temp_addr;
        if (flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0) == get_active_ota_bank_addr())
        {
            ota_temp_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_1);
        }
        else
        {
            ota_temp_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
        }

        APP_PRINT_TRACE1("ota_temp_addr:%x", ota_temp_addr);

        fmc_flash_nor_erase(ota_temp_addr, FMC_FLASH_NOR_ERASE_SECTOR);
    }
}

/**
    * @brief  Reset local variables and inactive bank ota header
    * @return void
    */
void app_ota_error_clear_local(uint8_t cause)
{
    if (ota_struct->rws_mode.valid_ret.oth_bud &&
        app_ota_check_ota_mode() == RWS_B2B_CONNECT)
    {
        app_ota_rws_send_msg(NONE, RWS_OTA_UPDATE_RET, UPDATE_FAIL);
    }

    app_ota_clear_local(cause);
    app_ota_erase_temp_bank_header();
    app_ble_rtk_adv_start();
}

/**
    * @brief  check if ota mode
    * @return True:ota mode; False: not in ota mode
    */
bool app_ota_mode_check(void)
{
    if (ota_struct == NULL)
    {
        return false;
    }
    else
    {
        return ota_struct->ota_flag.ota_mode;
    }

}


/**
    * @brief  check if ota reset
    * @return True:ota reset; False: not ota reset
    */
bool app_ota_reset_check(void)
{
    return (timer_idx_ota_reset != 0);
}

void app_ota_cback_register(void *cb_func, T_OTA_STAGE ota_stage)
{
    T_OTACbQueueElem *p_item = (T_OTACbQueueElem *)malloc(sizeof(T_OTACbQueueElem));
    if (p_item == NULL)
    {
        APP_PRINT_ERROR0("app_ota_cback_register: queue element malloc fail");
        return;
    }
    p_item->cback = cb_func;
    os_queue_in(&ota_cback_list[ota_stage], p_item);
}
/** End of APP_OTA_Exported_Functions
    * @}
    */

/** @} */ /* End of group APP_OTA_SERVICE */
