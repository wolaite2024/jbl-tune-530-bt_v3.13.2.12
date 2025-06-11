/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      spp_ota_service.c
   * @brief     Source file for using spp OTA service
   * @author    Michael
   * @date      2018-10-15
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
#include "dfu_api.h"
#include "app_main.h"
#include "app_cmd.h"
#include "ota_service.h"
#include "spp_ota_service.h"
#include "app_transfer.h"
#include "remote.h"
#include "app_mmi.h"
#include "app_cfg.h"
#include "dm.h"
#include "app_timer.h"
/** @defgroup  SPP_OTA_SERVICE SPP OTA Service
    * @brief SPP Service to implement OTA feature
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup SPP_OTA_SERVICE_Exported_Variables SPP OTA service Exported Variables
    * @brief
    * @{
    */

#define RWS_PRIMARY_VALID_OK    0x01
#define RWS_SECONDARY_VALID_OK  0x02

static uint8_t rws_ota_valid_ret;
static uint8_t ota_timer_id = 0;
static uint8_t timer_idx_ota = 0;
/** End of SPP_OTA_SERVICE_Exported_Variables
    * @}
    */

/** @} */ /* End of group SPP_OTA_SERVICE */

/*============================================================================*
 *                              Private Functions
 *============================================================================*/
/** @defgroup SPP_OTA_SERVICE_Exported_Functions SPP OTA service Exported Functions
    * @brief
    * @{
    */

static void spp_ota_clear_local(void)
{
    ota_service_clear_local();
    rws_ota_valid_ret = 0;
}

/**
    * @brief  event callback for ota
    * @param  event_type
    * @param  event_buf
    * @param  buf_len
    * @return void
    */
static void ota_dm_cback(T_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case DM_EVENT_OTA_INFO:
        {
            uint8_t *p_para = event_buf;
            //APP_PRINT_TRACE1("audio_app_au_cb event_buff %x", *event_buf);
            if (get_ota_bud_role() == DEVICE_ROLE_PRIMARY)
            {
                if (*p_para == true)/* The secondary valid ok */
                {
#if (bankswitch_stress_test == 1)
                    erase_ota_header_of_active_bank();
#endif
                    spp_ota_clear_local();
                    if (app_cfg_store() != 0)
                    {
                        APP_PRINT_ERROR0("ota_dm_cback: save nv cfg data error");
                    }
                    dfu_fw_active_reset();
                }
            }
            else if (get_ota_bud_role() == DEVICE_ROLE_SECONDARY)
            {
                if (*p_para == true)/* The primary valid ok */
                {
                    rws_ota_valid_ret |= RWS_PRIMARY_VALID_OK;
                }
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("ota_dm_cback: event_type 0x%04x", event_type);
    }
}

void ota_timeout_cb(uint8_t timer_evt, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("ota_timeout_cb: timer_evt 0x%02X, param %d",
                     timer_evt, param);
    switch (timer_evt)
    {
    case OTA_TIMER_DELAY_RESET:
        {
            app_stop_timer(&timer_idx_ota);
            dfu_fw_active_reset();
        }
        break;

    default:
        break;
    }

}

/** End of SPP_OTA_SERVICE_Exported_Functions
    * @}
    */


/** End of OTA_SERVICE
    * @}
    */

/*============================================================================*
 *                              Public Functions
 *============================================================================*/

/**
    * @brief  The main function to handle all the spp ota command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  app_idx received rx command device index
    * @return void
    */
void spp_ota_service_handle(uint16_t length, uint8_t *p_value, uint8_t app_idx)
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
        app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
        return;
    }

    length = length - 2;
    p = p_value + 2;

    APP_PRINT_INFO2("===>SPP_OTA_Service_HandleCPReq, cmd_id:%x, length:%x\n", cmd_id, length);

    switch (cmd_id)
    {
    case CMD_OTA_DEV_INFO:
        if (length == OTA_LENGTH_OTA_GET_INFO)
        {
            uint8_t device_info_length;
            uint8_t *device_info_addr;

            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            device_info_addr  = (uint8_t *)ota_get_device_info(&device_info_length, SPP_OTA_MODE);
            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_DEV_INFO, device_info_addr,
                                 device_info_length);
        }
        else
        {
            ack_flag = true;
        }
        break;

    case CMD_OTA_GET_IMG_VER:
        if (length == OTA_LENGTH_OTA_GET_IMG_VER)
        {
            uint8_t table_length;
            uint8_t *verion_table;

            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            verion_table = (uint8_t *)ota_get_image_version(&table_length);
            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_GET_IMG_VER, verion_table,
                                 table_length);
        }
        else
        {
            ack_flag = true;
        }
        break;

    case CMD_OTA_START:
        if (length == OTA_LENGTH_START_OTA)
        {
            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            results  = ota_start_dfu_handle(p, SPP_OTA_MODE);
            if (results != DFU_ARV_SUCCESS)
            {
                spp_ota_clear_local();
            }

            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_START, &results, sizeof(results));
        }
        else
        {
            ack_flag = true;
        }
        break;

    case CMD_OTA_PACKET:
        {
            results = ota_data_packet_handle(length, p);
            if (results == DFU_ARV_FAIL_INVALID_PARAMETER)
            {
                spp_ota_clear_local();
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            }
            else if (results == DFU_ARV_FAIL_OPERATION)
            {
                spp_ota_clear_local();
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            }
            else
            {
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            }
        }
        break;

    case CMD_OTA_VALID:
        if (length == OTA_LENGTH_VALID_FW)
        {
            uint16_t sign;
            LE_ARRAY_TO_UINT16(sign, p);

            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

            results = ota_valid_handle(sign);

            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_VALID, &results, sizeof(results));
        }
        else
        {
            ack_flag = true;
        }
        break;

    case CMD_OTA_ACTIVE_RESET:
        {
            /* notify bootloader to reset and use new image */
            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

            if (get_ota_bud_role() == DEVICE_ROLE_SINGLE)
            {
#if (bankswitch_stress_test == 1)
                erase_ota_header_of_active_bank();
#endif
                spp_ota_clear_local();
                if (app_cfg_store() != 0)
                {
                    APP_PRINT_ERROR0("SPP_OTA: save nv cfg data error");
                }
                dfu_fw_active_reset();
            }
            else
            {
                results = true;
                if (get_ota_bud_role() == DEVICE_ROLE_SECONDARY)
                {
                    rws_ota_valid_ret |= RWS_SECONDARY_VALID_OK;    /* valid ok */
                }
                else if (get_ota_bud_role() == DEVICE_ROLE_PRIMARY)
                {
                    rws_ota_valid_ret |= RWS_PRIMARY_VALID_OK; /* valid ok */
                }

                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_VALID_SYNC,
                                                    (uint8_t *)&results, 1);


                if (get_ota_bud_role() == DEVICE_ROLE_PRIMARY)/* Primary will not accept reset command */
                {
                    APP_PRINT_TRACE0("SPP_OTA_ROLESWAP");
                    app_mmi_handle_action(MMI_START_ROLESWAP);
                }
                else
                {
#if (bankswitch_stress_test == 1)
                    erase_ota_header_of_active_bank();
#endif
                    app_start_timer(&timer_idx_ota, "ota_time",
                                    ota_timer_id, OTA_TIMER_DELAY_RESET, 0, false,
                                    (1 * 1000));

                }
            }
        }
        break;

    case CMD_OTA_RESET:
        {
            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            spp_ota_clear_local();
        }
        break;

    case CMD_OTA_IMG_INFO:
        if (length == OTA_LENGTH_IMAGE_INFO)
        {
            uint8_t data[9] = {0};

            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

            ota_get_img_info_handle(data, p, sizeof(data));

            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_IMG_INFO, data, sizeof(data));
        }
        else
        {
            ack_flag = true;
        }
        break;

    case CMD_OTA_BUFFER_CHECK_ENABLE:
        {
            uint8_t notif_data[5] = {0};
            uint16_t mtu_size;
            uint16_t max_buffer_size;

            if (length  == OTA_LENGTH_BUFFER_CHECK_EN)
            {
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

                max_buffer_size = MAX_BUFFER_SIZE;
                mtu_size = MTU_SIZE;

                ota_buffer_check_en(&notif_data[0], max_buffer_size, mtu_size);

                app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_BUFFER_CHECK_ENABLE, notif_data,
                                     sizeof(notif_data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_OTA_BUFFER_CHECK:
        {
            uint8_t notif_data[5] = {0};

            if (length  == OTA_LENGTH_BUFFER_CRC)
            {
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

                ota_buffer_check_handle(p, &notif_data[0]);

                app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_BUFFER_CHECK, notif_data,
                                     sizeof(notif_data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_OTA_SECTION_SIZE:
        {
            uint8_t info_length;
            uint8_t *info_addr;
            if (length == OTA_LENGTH_SECTION_SIZE)
            {
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
                info_addr = (uint8_t *)ota_get_section_size(&info_length);
                app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_SECTION_SIZE, info_addr,
                                     info_length);
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_OTA_DEV_EXTRA_INFO:
        {
            uint8_t notif_data[6] = {0};
            if (length  == OTA_LRNGTH_OTHER_INFO)
            {
                app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);

                if (((get_ota_bud_role() == DEVICE_ROLE_PRIMARY) && (rws_ota_valid_ret & RWS_PRIMARY_VALID_OK))
                    || ((get_ota_bud_role() == DEVICE_ROLE_SECONDARY) && (rws_ota_valid_ret & RWS_SECONDARY_VALID_OK)))
                {
                    notif_data[0] = 0x1;  // valid ok
                }
                app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_DEV_EXTRA_INFO, notif_data,
                                     sizeof(notif_data));
            }
            else
            {
                ack_flag = true;
            }
        }
        break;

    case CMD_OTA_PROTOCOL_TYPE:
        {
            uint16_t data;
            app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
            data = SPP_PROTOCOL_INFO;
            app_report_spp_event(&app_db.br_link[app_idx], EVENT_OTA_PROTOCOL_TYPE, (uint8_t *)&data,
                                 sizeof(data));
        }
        break;

    default:
        ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
        app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
        break;
    }

    if (ack_flag == true)
    {
        APP_PRINT_TRACE0("SPP_OTA: invalid length");
        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        app_cmd_set_event_ack(CMD_PATH_BR, app_idx, ack_pkt);
    }
}

/**
    * @brief  ota callback register
    * @param  void
    * @return void
    */
void app_ota_init(void)
{
    dm_cback_register(ota_dm_cback);
    app_timer_reg_cb(ota_timeout_cb, &ota_timer_id);
}
