/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_TUYA_SUPPORT
#include <string.h>
#include <trace.h>
#include "ftl.h"
#include "os_mem.h"
#include "os_msg.h"
#include "app_main.h"
#include "app_msg.h"
#include "app_cfg.h"
#include "app_device.h"
#include "app_ota.h"
#include "app_relay.h"
#include "app_auto_power_off.h"
#include "app_sniff_mode.h"
#include "rtk_tuya_ble_ota.h"
#include "rtk_tuya_ble_ota_flash.h"
#include "tuya_ble_type.h"
#include "tuya_ble_api.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_log.h"
#include "rtk_tuya_ble_app_demo.h"
#include "audio_probe.h"

#define TUYA_OTA_MTU_SIZE   430
#define TUYA_OTA_BUF_SIZE   500

T_TUYA_OTA_RWS_MGR tuya_ota_rws_mgr;

static uint8_t tuya_ota_result = 0x00;

static uint32_t tuya_iot_image_version = 0;

static uint8_t *p_temp_buffer_head = NULL;

static uint16_t ota_tmp_buf_used_size = 0;
static uint16_t ota_tmp_buf_offset = 0;
static uint16_t ota_tmp_buf_left = 0;

static uint8_t *p_auth_ctrl_header_buf = NULL;
static uint16_t auth_ctrl_header_rx_offset = 0;
static uint8_t vp_index_bank0 = 0xff;

static uint16_t recv_pkt_idx = 0;
static uint16_t last_pkt_idx = 0;
static T_TUYA_OTA_STATUS tuya_ota_status;

static uint8_t ignore_index_in_image_info_list = 0;
static uint32_t cur_sub_image_relative_offset_for_ignore = 0;

#if F_APP_ERWS_SUPPORT
static bool need_relay_rsp_flag = false;
static uint8_t remote_index_in_image_info_list = 0;
static uint8_t remote_target_sub_bin_index = 0;
static uint8_t remote_cur_download_image_index = 0;
static uint8_t remote_end_download_image_index = 0;
static uint32_t remote_cur_sub_image_relative_offset = 0;
static void tuya_ota_handle_ota_payload(void);
static void tuya_ota_response_data_req(void);
typedef enum t_tuya_ota_remote_msg_id
{
    TUYA_OTA_P2S_MSG_OTA_START = 0x00,
    TUYA_OTA_P2S_MSG_OTA_FAIL,

    TUYA_OTA_P2S_MSG_NOTIFY_BIN_INFO = 0x10,
    TUYA_OTA_P2S_MSG_NOTIFY_MAIN_HEADER,
    TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_AUTH_CONTROL_HEADER,
    TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_PAYLOAD,
    TUYA_OTA_P2S_MSG_OTA_END,

    TUYA_OTA_S2P_NOTIFY_ACTIVE_BANK = 0x20,
    TUYA_OTA_S2P_MSG_OTA_FAIL,
    TUYA_OTA_S2P_MSG_RX_RECVD,
    TUYA_OTA_S2P_MSG_RX_COMPLETE,
    TUYA_OTA_S2P_MSG_OTA_END_ACK,

    TUYA_OTA_REMOTE_MSG_NUM,
} T_TUYA_OTA_REMOTE_MSG_ID;

typedef struct t_tuya_ota_p2s_notify_main_header
{
    T_MAIN_HEADER main_header;
} T_TUYA_OTA_P2S_NOTIFY_MAIN_HEADER;

typedef struct t_tuya_ota_p2s_notify_bin_info
{
    uint8_t sub_bin_index;
    T_SUB_BIN_INFO sub_bin;
} T_TUYA_OTA_P2S_NOTIFY_BIN_INFO;

typedef struct t_tuya_ota_s2p_notify_active_bank
{
    uint8_t active_bank;
} T_TUYA_OTA_S2P_NOTIFY_ACTIVE_BANK;

typedef struct t_tuya_ota_p2s_send_sub_image_auth_control_header
{
    T_AUTH_HEADER_FORMAT auth_header;
    T_IMG_CTRL_HEADER_FORMAT control_header;
} T_TUYA_OTA_P2S_SEND_SUB_IMAGE_AUTH_CONTROL_HEADER;

typedef struct t_tuya_ota_p2s_send_sub_image_payload
{
    uint16_t len;
    uint8_t data[0];
} T_TUYA_OTA_P2S_SEND_SUB_IMAGE_PAYLOAD;

typedef union t_tuya_ota_remote_msg
{
    T_TUYA_OTA_P2S_NOTIFY_MAIN_HEADER p2s_notify_main_header;
    T_TUYA_OTA_P2S_NOTIFY_BIN_INFO p2s_notify_bin_info;
    T_TUYA_OTA_S2P_NOTIFY_ACTIVE_BANK s2p_notify_active_bank;
    T_TUYA_OTA_P2S_SEND_SUB_IMAGE_AUTH_CONTROL_HEADER p2s_send_sub_image_auth_control_header;
    T_TUYA_OTA_P2S_SEND_SUB_IMAGE_PAYLOAD p2s_send_sub_image_payload;
} T_TUYA_OTA_REMOTE_MSG;
#endif

typedef enum t_handle_rx_data_err_code
{
    TUYA_OTA_ERR_NONE,
    TUYA_OTA_ERR_WRITE_FAIL,
    TUYA_OTA_ERR_INDEX_SERARCH_FAIL,
    TUYA_OTA_ERR_CHECK_FAIL,
    TUYA_OTA_ERR_SYNC_FAIL,
} T_HANDLE_RX_DATA_ERR_CODE;

static void tuya_ota_free_buffer(void)
{
    if (p_temp_buffer_head != NULL)
    {
        audio_probe_media_buffer_free(p_temp_buffer_head);
        p_temp_buffer_head = NULL;
        ota_tmp_buf_used_size = 0;
        ota_tmp_buf_offset = 0;
        ota_tmp_buf_left = 0;
        auth_ctrl_header_rx_offset = 0;
        if (p_auth_ctrl_header_buf)
        {
            audio_probe_media_buffer_free(p_auth_ctrl_header_buf);
            auth_ctrl_header_rx_offset = 0;
            p_auth_ctrl_header_buf = NULL;
        }
    }
}

uint8_t tuya_ota_is_busy(void)
{
    return tuya_ota_status;
}

static void tuya_ble_state_init(void)
{
    tuya_ota_status = TUYA_OTA_STATUS_NONE;
    tuya_ota_result = 0;

    ota_tmp_buf_used_size = 0;
    ota_tmp_buf_offset = 0;
    ota_tmp_buf_left = 0;

    recv_pkt_idx = 0;
    last_pkt_idx = 0;

    ignore_index_in_image_info_list = 0;
    cur_sub_image_relative_offset_for_ignore = 0;

#if F_APP_ERWS_SUPPORT
    remote_index_in_image_info_list = 0;
    remote_target_sub_bin_index = 0;
    remote_cur_download_image_index = 0;
    remote_end_download_image_index = 0;
    remote_cur_sub_image_relative_offset = 0;
#endif
}

static uint8_t tuya_ota_get_cur_sub_image_index(uint8_t *p_ctrl_header, bool remote, bool ignore,
                                                uint8_t bank_for_ignore)
{
    uint16_t image_id = ((T_IMG_CTRL_HEADER_FORMAT *)p_ctrl_header)->image_id;
    APP_PRINT_INFO4("tuya_ota_get_cur_sub_image_index: image_id 0x%04x, remote %d, ignore %d, bank_for_ignore %d",
                    image_id, remote, ignore, bank_for_ignore);
    uint8_t start_index = 0;
    uint8_t end_index = 0;
    uint8_t index;
    uint8_t active_bank = tuya_ota_rws_mgr.local_active_bank;
    uint8_t sub_bin_index = tuya_ota_rws_mgr.target_sub_bin_index;
#if F_APP_ERWS_SUPPORT
    active_bank = remote ? tuya_ota_rws_mgr.remote_active_bank : tuya_ota_rws_mgr.local_active_bank;
    sub_bin_index = remote ? remote_target_sub_bin_index : tuya_ota_rws_mgr.target_sub_bin_index;
#endif
    if (ignore)
    {
        sub_bin_index = 0;
        active_bank = bank_for_ignore == IMAGE_LOCATION_BANK0 ? IMAGE_LOCATION_BANK1 : IMAGE_LOCATION_BANK0;
    }

    if (active_bank == IMAGE_LOCATION_BANK0)
    {
        start_index = OTA_SUB_BIN_COUNT_MAX / 2;
        end_index = OTA_SUB_BIN_COUNT_MAX;
    }
    else if (active_bank == IMAGE_LOCATION_BANK1)
    {
        start_index = 0;
        end_index = OTA_SUB_BIN_COUNT_MAX / 2;
    }

    for (index = start_index; index < end_index; index++)
    {
        if (image_id == (uint16_t)tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[index].image_id)
        {
            APP_PRINT_INFO2("tuya_ota_get_cur_sub_image_index: hit index %d, image_id 0x%04x",
                            index, tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[index].image_id);
            break;
        }
    }
    if (index == end_index)
    {
        index = 0xff;
    }
    return index;
}

static void tuya_ota_response_ota_completed(void)
{
    APP_PRINT_INFO3("tuya_ota_response_ota_completed: local %d, is rws %d, remote %d",
                    tuya_ota_rws_mgr.update_result_local,  tuya_ota_rws_mgr.main_header.is_rws,
                    tuya_ota_rws_mgr.update_result_remote);
    uint8_t ota_result = (tuya_ota_rws_mgr.update_result_local &&
                          (tuya_ota_rws_mgr.main_header.is_rws == 0 || tuya_ota_rws_mgr.update_result_remote)) ? 0x00 : 0x03;
    uint8_t ota_end_rsp[2];
    tuya_ble_ota_response_t rsp_packet;

    ota_end_rsp[0] = TUYA_OTA_TYPE;
    ota_end_rsp[1] = ota_result;

    rsp_packet.type = TUYA_BLE_OTA_END;
    rsp_packet.data_len = 2;
    rsp_packet.p_data = (uint8_t *)&ota_end_rsp;

    if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
    {
        tuya_ota_exit();
    }

    if (ota_result == 0)
    {
        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_TUYA_OTA, app_cfg_const.timer_auto_power_off);
        tuya_ota_clear_notready_flag();
        app_device_reboot(2000, RESET_ALL);
    }
    else
    {
        tuya_ota_exit();
    }
}

#if F_APP_ERWS_SUPPORT
static void tuya_ota_send_remote_msg(T_TUYA_OTA_REMOTE_MSG_ID msg_id,
                                     T_TUYA_OTA_REMOTE_MSG *p_remote_info,
                                     uint16_t msg_len)
{
    APP_PRINT_TRACE1("tuya_ota_send_remote_msg: msg_id 0x%x", msg_id);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_TUYA_OTA, msg_id,
                                        (uint8_t *)p_remote_info, msg_len);
}

static void tuya_ota_s2p_send_notify_active_bank(void)
{
    T_TUYA_OTA_REMOTE_MSG_ID msg_id = TUYA_OTA_S2P_NOTIFY_ACTIVE_BANK;
    tuya_ota_rws_mgr.local_active_bank = app_ota_get_active_bank();

    T_TUYA_OTA_REMOTE_MSG remote_msg;
    remote_msg.s2p_notify_active_bank.active_bank = tuya_ota_rws_mgr.local_active_bank;

    tuya_ota_send_remote_msg(msg_id, &remote_msg, sizeof(remote_msg.s2p_notify_active_bank));
}

static void tuya_ota_p2s_send_main_header(void)
{
    T_TUYA_OTA_REMOTE_MSG_ID msg_id = TUYA_OTA_P2S_MSG_NOTIFY_MAIN_HEADER;
    T_TUYA_OTA_REMOTE_MSG remote_msg;
    memcpy(&remote_msg.p2s_notify_main_header, &tuya_ota_rws_mgr.main_header,
           sizeof(tuya_ota_rws_mgr.main_header));
    tuya_ota_send_remote_msg(msg_id, &remote_msg, sizeof(remote_msg.p2s_notify_main_header));
}

static void tuya_ota_p2s_send_bin_info(uint8_t bin_index)
{
    T_TUYA_OTA_REMOTE_MSG_ID msg_id = TUYA_OTA_P2S_MSG_NOTIFY_BIN_INFO;
    T_TUYA_OTA_REMOTE_MSG remote_msg;
    remote_msg.p2s_notify_bin_info.sub_bin_index = bin_index;
    memcpy(&remote_msg.p2s_notify_bin_info.sub_bin, &tuya_ota_rws_mgr.sub_bin[bin_index],
           sizeof(tuya_ota_rws_mgr.sub_bin[bin_index]));
    tuya_ota_send_remote_msg(msg_id, &remote_msg, sizeof(remote_msg.p2s_notify_bin_info));
}

static void tuya_ota_p2s_send_sub_image_auth_control_header(uint8_t *p_auth_data,
                                                            uint8_t *p_control_data)
{
    T_TUYA_OTA_REMOTE_MSG_ID msg_id = TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_AUTH_CONTROL_HEADER;
    T_TUYA_OTA_REMOTE_MSG remote_msg;
    memcpy(&remote_msg.p2s_send_sub_image_auth_control_header.auth_header, p_auth_data,
           OTA_AUTH_HEADER_SIZE);
    memcpy(&remote_msg.p2s_send_sub_image_auth_control_header.control_header, p_control_data,
           OTA_CONTROL_HEADER_SIZE);

    tuya_ota_send_remote_msg(msg_id, &remote_msg,
                             sizeof(remote_msg.p2s_send_sub_image_auth_control_header));
    need_relay_rsp_flag = true;
}

static void tuya_ota_p2s_send_sub_image_payload(uint8_t *p_data, uint16_t len)
{
    T_TUYA_OTA_REMOTE_MSG_ID msg_id = TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_PAYLOAD;
    T_TUYA_OTA_REMOTE_MSG *p_remote_msg = (T_TUYA_OTA_REMOTE_MSG *)audio_probe_media_buffer_malloc(
                                              sizeof(
                                                  T_TUYA_OTA_P2S_SEND_SUB_IMAGE_PAYLOAD) + len);
    if (p_remote_msg)
    {
        p_remote_msg->p2s_send_sub_image_payload.len = len;
        memcpy(p_remote_msg->p2s_send_sub_image_payload.data, p_data, len);
        tuya_ota_send_remote_msg(msg_id, p_remote_msg,
                                 sizeof(p_remote_msg->p2s_send_sub_image_payload) + len);
        audio_probe_media_buffer_free(p_remote_msg);
        need_relay_rsp_flag = true;
    }
    else
    {
        APP_PRINT_INFO0("tuya_ota_p2s_send_sub_image_payload: memory malloc fail");
    }
}
#endif

void tuya_ota_exit(void)
{
    TUYA_BLE_LOG_INFO("tuya_ota_exit: fail at state %d", tuya_ota_status);
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        tuya_ota_send_remote_msg(TUYA_OTA_P2S_MSG_OTA_FAIL, NULL, 0);
    }
#endif
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_TUYA_OTA, app_cfg_const.timer_auto_power_off);
    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_OTA);
    tuya_ota_free_buffer();
    tuya_ble_state_init();
}

static void tuya_ota_check_set_cur_sub_bin_info(void)
{
    tuya_ota_rws_mgr.target_sub_bin_index = (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
                                             app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE) ? 0 : 1;
#if F_APP_ERWS_SUPPORT
    remote_target_sub_bin_index = (tuya_ota_rws_mgr.target_sub_bin_index == 0) ? 1 : 0;
#endif

    tuya_ota_rws_mgr.cur_download_image_index = 0;
#if F_APP_ERWS_SUPPORT
    tuya_ota_rws_mgr.end_download_image_index = tuya_ota_rws_mgr.main_header.num_role_related / 2 +
                                                tuya_ota_rws_mgr.main_header.num_bank_related / 2 + tuya_ota_rws_mgr.main_header.num_none_related;
#else
    tuya_ota_rws_mgr.end_download_image_index = tuya_ota_rws_mgr.main_header.num_role_related +
                                                tuya_ota_rws_mgr.main_header.num_bank_related / 2 + tuya_ota_rws_mgr.main_header.num_none_related;
#endif
    tuya_ota_rws_mgr.cur_sub_image_relative_offset = 0; //no MP header

    APP_PRINT_INFO4("tuya_ota_check_set_cur_sub_bin_info: const_bud_role %d, current bud_role %d, target_sub_bin_index %d, cur_sub_image_relative_offset %d",
                    app_cfg_const.bud_role,
                    app_cfg_nv.bud_role,
                    tuya_ota_rws_mgr.target_sub_bin_index,
                    tuya_ota_rws_mgr.cur_sub_image_relative_offset);
}

void tuya_ota_check_control_header(uint8_t *p_data)
{
    T_IMG_CTRL_HEADER_FORMAT *p_control_header = (T_IMG_CTRL_HEADER_FORMAT *)p_data;

    APP_PRINT_INFO6("tuya_ota_check_control_header 1: "
                    " crc16 0x%04x, ic_type 0x%02x, secure_version 0x%02x, value 0x%04x, image_id 0x%04x, payload_len 0x%08x",
                    p_control_header->crc16, p_control_header->ic_type, p_control_header->secure_version,
                    p_control_header->ctrl_flag.value,
                    p_control_header->image_id, p_control_header->payload_len);

    APP_PRINT_INFO8("tuya_ota_check_control_header 2: ctrl flag:"
                    "xip %d, enc %d, load_when_boot %d, enc_load %d, enc_key_select %d, "
                    "not_ready %d, not_obsolete %d, integrity_check_en_in_boot %d",
                    p_control_header->ctrl_flag.xip, p_control_header->ctrl_flag.enc,
                    p_control_header->ctrl_flag.load_when_boot,
                    p_control_header->ctrl_flag.enc_load, p_control_header->ctrl_flag.enc_key_select,
                    p_control_header->ctrl_flag.not_ready,
                    p_control_header->ctrl_flag.not_obsolete, p_control_header->ctrl_flag.integrity_check_en_in_boot);
}

#if F_APP_ERWS_SUPPORT
static void app_tuya_ota_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                     T_REMOTE_RELAY_STATUS status)
{

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    T_TUYA_OTA_REMOTE_MSG *p_info = (T_TUYA_OTA_REMOTE_MSG *)buf;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        switch (msg_type)
        {
        case TUYA_OTA_P2S_MSG_OTA_START:
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_TUYA_OTA);
                tuya_ota_s2p_send_notify_active_bank();
                tuya_ota_status = TUYA_OTA_STATUS_SECONDARY_UNDERGOING;
            }
            break;
        case TUYA_OTA_P2S_MSG_OTA_FAIL:
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_TUYA_OTA, app_cfg_const.timer_auto_power_off);
                tuya_ota_exit();
            }
            break;
        case TUYA_OTA_P2S_MSG_NOTIFY_MAIN_HEADER:
            {
                memcpy(&tuya_ota_rws_mgr.main_header, &p_info->p2s_notify_main_header.main_header,
                       sizeof(p_info->p2s_notify_main_header.main_header));
                tuya_ota_check_set_cur_sub_bin_info();
            }
            break;
        case TUYA_OTA_P2S_MSG_NOTIFY_BIN_INFO:
            {
                uint8_t sub_bin_index = p_info->p2s_notify_bin_info.sub_bin_index;
                memcpy(&tuya_ota_rws_mgr.sub_bin[sub_bin_index], &p_info->p2s_notify_bin_info.sub_bin,
                       sizeof(tuya_ota_rws_mgr.sub_bin[sub_bin_index]));
            }
            break;
        case TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_AUTH_CONTROL_HEADER:
            {
                APP_PRINT_TRACE0("app_tuya_ota_parse_cback: handle auth and control header");
                uint8_t *p_auth_data = (uint8_t *)&p_info->p2s_send_sub_image_auth_control_header.auth_header;
                uint8_t *p_ctrl_data = (uint8_t *)&p_info->p2s_send_sub_image_auth_control_header.control_header;

                tuya_ota_check_control_header(p_ctrl_data);

                tuya_ota_rws_mgr.index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_ctrl_data, false,
                                                                                             false, 0);
                if (tuya_ota_rws_mgr.index_in_image_info_list == 0xff)
                {
                    tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_FAIL, NULL, 0);
                    break;
                }

                uint32_t dfu_base_addr =
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
                uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
                uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_AUTH_HEADER_SIZE,
                                                                p_auth_data,
                                                                0);
                if (write_result != 0)
                {
                    tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_FAIL, NULL, 0);
                    break;
                }
                tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;

                offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;

                write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_CONTROL_HEADER_SIZE, p_ctrl_data,
                                                       1);

                if (write_result != 0)
                {
                    tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_FAIL, NULL, 0);
                    break;
                }
                tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
                tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_RX_RECVD, NULL, 0);
            }
            break;
        case TUYA_OTA_P2S_MSG_SEND_SUB_IMAGE_PAYLOAD:
            {
                uint16_t len = p_info->p2s_send_sub_image_payload.len;
                uint8_t *p_data = p_info->p2s_send_sub_image_payload.data;

                APP_PRINT_INFO3("app_tuya_ota_parse_cback: cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                                tuya_ota_rws_mgr.cur_sub_image_relative_offset,
                                len,
                                tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size);

                uint32_t dfu_base_addr =
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
                uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
                uint32_t length = len;
                uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
                if (write_result != 0)
                {
                    tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_FAIL, NULL, 0);
                    break;
                }

                if (tuya_ota_rws_mgr.cur_sub_image_relative_offset + len ==
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size)
                {
                    void *p_image_header = (void *)
                                           tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;

                    bool check_image = tuya_ota_check_image(p_image_header);

                    if (check_image)
                    {
                        tuya_ota_rws_mgr.cur_download_image_index++;
                        tuya_ota_rws_mgr.cur_sub_image_relative_offset = 0;

                        if (tuya_ota_rws_mgr.cur_download_image_index == tuya_ota_rws_mgr.end_download_image_index)
                        {
                            tuya_ota_rws_mgr.update_result_local = true;
                            tuya_ota_rws_mgr.cur_download_image_index = 0;
                            tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_RX_COMPLETE, NULL, 0);
                        }
                        tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_RX_RECVD, NULL, 0);
                    }
                    else
                    {
                        tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_FAIL, NULL, 0);
                        break;
                    }
                }
                else
                {
                    tuya_ota_rws_mgr.cur_sub_image_relative_offset += len;
                    tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_RX_RECVD, NULL, 0);
                }
            }
            break;
        case TUYA_OTA_P2S_MSG_OTA_END:
            {
                if (tuya_ota_rws_mgr.update_result_local)
                {
                    tuya_ota_clear_notready_flag();
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_TUYA_OTA, app_cfg_const.timer_auto_power_off);
                    app_device_reboot(2000, RESET_ALL);
                }
                tuya_ota_send_remote_msg(TUYA_OTA_S2P_MSG_OTA_END_ACK, NULL, 0);
            }
            break;

        default:
            break;
        }
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        switch (msg_type)
        {
        case TUYA_OTA_S2P_NOTIFY_ACTIVE_BANK:
            {
                tuya_ota_rws_mgr.local_active_bank = app_ota_get_active_bank();
                tuya_ota_rws_mgr.remote_active_bank = p_info->s2p_notify_active_bank.active_bank;
                APP_PRINT_INFO2("app_tuya_ota_parse_cback: local_active_bank %d, remote_active_bank %d",
                                tuya_ota_rws_mgr.local_active_bank, tuya_ota_rws_mgr.remote_active_bank);

                tuya_ble_ota_response_t rsp_packet;
                uint8_t file_offset_rsp[5];
                uint32_t rsp_file_offset = 0;

                file_offset_rsp[0] = TUYA_OTA_TYPE;

                file_offset_rsp[1] = rsp_file_offset >> 24;
                file_offset_rsp[2] = rsp_file_offset >> 16;
                file_offset_rsp[3] = rsp_file_offset >> 8;
                file_offset_rsp[4] = (uint8_t)rsp_file_offset;

                tuya_ota_status = TUYA_OTA_STATUS_FILE_DATA;

                rsp_packet.type = TUYA_BLE_OTA_FILE_OFFSET_REQ;
                rsp_packet.data_len = 5;
                rsp_packet.p_data = (uint8_t *)&file_offset_rsp;

                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_MAIN_HEADER;

                if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
                {
                    tuya_ota_exit();
                }
            }
            break;
        case TUYA_OTA_S2P_MSG_OTA_FAIL:
            {
                tuya_ota_result = 0x03;
            }
            break;
        case TUYA_OTA_S2P_MSG_RX_RECVD:
            {
                need_relay_rsp_flag = false;
                memmove(p_temp_buffer_head, p_temp_buffer_head + ota_tmp_buf_offset, ota_tmp_buf_left);

                ota_tmp_buf_used_size = ota_tmp_buf_left;

                if (ota_tmp_buf_used_size > 0)
                {
                    tuya_ota_handle_ota_payload();
                }
                else
                {
                    tuya_ota_response_data_req();
                }
            }
            break;
        case TUYA_OTA_S2P_MSG_RX_COMPLETE:
            {
                tuya_ota_rws_mgr.update_result_remote = true; //judge for ota end
            }
            break;
        case TUYA_OTA_S2P_MSG_OTA_END_ACK:
            {
                tuya_ota_response_ota_completed();
            }
            break;

        default:
            break;
        }
    }
}
#endif

static void tuya_ota_start_req_handle(uint8_t *recv_data, uint32_t recv_len)
{
    TUYA_BLE_LOG_INFO("tuya_ota_start_req_handle");
    APP_PRINT_WARN1("tuya_ota_start_req_handle unused memory: %d", mem_peek());
    tuya_ota_free_buffer();
    tuya_ble_state_init();

    uint8_t rsp_data[10];
    tuya_ble_ota_response_t rsp_packet;

    uint32_t current_version = tuya_ota_get_app_ui_version();

    uint16_t mtu_size = TUYA_OTA_MTU_SIZE;

    if (tuya_ota_status != TUYA_OTA_STATUS_NONE)
    {
        tuya_ble_state_init();
    }

    if (p_temp_buffer_head == NULL)
    {
        p_temp_buffer_head = (uint8_t *)audio_probe_media_buffer_malloc(TUYA_OTA_BUF_SIZE);
    }

    if (p_temp_buffer_head == NULL)
    {
        tuya_ota_exit();
    }

    rsp_data[0] = TUYA_OTA_TYPE;
    rsp_data[1] = TUYA_OTA_VERSION;
    rsp_data[2] = 0;

    rsp_data[3] = current_version >> 24;
    rsp_data[4] = current_version >> 16;
    rsp_data[5] = current_version >> 8;
    rsp_data[6] = current_version;

    rsp_data[7] = mtu_size >> 8;
    rsp_data[8] = mtu_size;

    TUYA_BLE_LOG_INFO("tuya_ota_start_req_handle: current_version 0x%08x", current_version);

    rsp_packet.type = TUYA_BLE_OTA_REQ;
    rsp_packet.data_len = 9;
    rsp_packet.p_data = rsp_data;

    if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
    {
        tuya_ota_exit();
    }
    else
    {
        app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_OTA);
        tuya_ota_status = TUYA_OTA_STATUS_FILE_INFO;
    }
}

static void tuya_ota_file_info_req_handle(uint8_t *recv_data, uint32_t recv_len)
{
    TUYA_BLE_LOG_INFO("tuya_ota_file_info_response");

    uint8_t file_info_rsp[30];
    tuya_ble_ota_response_t rsp_packet;

    uint8_t recv_pid[8];

    uint32_t recv_file_length;

    uint8_t rsp_state = 0x00;

    if (tuya_ota_status != TUYA_OTA_STATUS_FILE_INFO)
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_FILE_INFO and is : %d !",
                           tuya_ota_status);
        return;
    }

    memcpy(recv_pid, &recv_data[1], 8);

    tuya_iot_image_version = recv_data[9] << 24;
    tuya_iot_image_version += recv_data[10] << 16;
    tuya_iot_image_version += recv_data[11] << 8;
    tuya_iot_image_version += recv_data[12];

    recv_file_length = recv_data[29] << 24;
    recv_file_length += recv_data[30] << 16;
    recv_file_length += recv_data[31] << 8;
    recv_file_length += recv_data[32];

    uint32_t local_version = tuya_ota_get_app_ui_version();

    if (memcmp(APP_PRODUCT_ID, recv_pid, 8) != 0)
    {
        rsp_state = 0x01;
    }
    else if (tuya_iot_image_version < local_version)
    {
        TUYA_BLE_LOG_INFO("tuya_ota_file_info_req_handle: tuya_iot_image_version 0x%08x, local version 0x%08x",
                          tuya_iot_image_version, local_version);
        rsp_state = 0x02;
    }

    file_info_rsp[0] = TUYA_OTA_TYPE;
    file_info_rsp[1] = rsp_state;

    if (rsp_state == 0x00)
    {
        memset(&file_info_rsp[2], 0, 24); //written length = 0, written crc = 0, written md5 = 0
        tuya_ota_status = TUYA_OTA_STATUS_FILE_OFFSET;
    }

    rsp_packet.type = TUYA_BLE_OTA_FILE_INFO;
    rsp_packet.data_len = 26;
    rsp_packet.p_data = file_info_rsp;

    if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
    {
        tuya_ota_exit();
    }
}

static void tuya_ota_offset_req_handle(uint8_t *recv_data, uint32_t recv_len)
{

    TUYA_BLE_LOG_INFO("tuya_ota_offset_req_handle");

    if (tuya_ota_status != TUYA_OTA_STATUS_FILE_OFFSET)
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_FILE_INFO and is : %d !",
                           tuya_ota_status);
        return;
    }

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_TUYA_OTA);

    uint32_t recv_file_offset;

    recv_file_offset = recv_data[1] << 24;
    recv_file_offset += recv_data[2] << 16;
    recv_file_offset += recv_data[3] << 8;
    recv_file_offset += recv_data[4]; //BE_STREAM_TO_UINT32

#if F_APP_ERWS_SUPPORT
    tuya_ota_send_remote_msg(TUYA_OTA_P2S_MSG_OTA_START, NULL, 0);
#else
    tuya_ble_ota_response_t rsp_packet;
    uint8_t file_offset_rsp[5];
    uint32_t rsp_file_offset = 0; //always from main header, not support resuming from break point.

    file_offset_rsp[0] = TUYA_OTA_TYPE;

    file_offset_rsp[1] = rsp_file_offset >> 24;
    file_offset_rsp[2] = rsp_file_offset >> 16;
    file_offset_rsp[3] = rsp_file_offset >> 8;
    file_offset_rsp[4] = (uint8_t)rsp_file_offset;

    tuya_ota_status = TUYA_OTA_STATUS_FILE_DATA;

    rsp_packet.type = TUYA_BLE_OTA_FILE_OFFSET_REQ;
    rsp_packet.data_len = 5;
    rsp_packet.p_data = (uint8_t *)&file_offset_rsp;

    tuya_ota_rws_mgr.local_active_bank = app_ota_get_active_bank();
    TUYA_BLE_LOG_INFO("tuya_ota_offset_req_handle: active_bank = %d",
                      tuya_ota_rws_mgr.local_active_bank);
    tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_MAIN_HEADER;

    if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
    {
        tuya_ota_exit();
    }
#endif
}

static uint8_t tuya_ota_handle_control_header_role_related(uint8_t bin_index, uint8_t *p_data)
{
    uint8_t *p_auth_header = p_data;
    uint8_t *p_control_header = p_auth_header + OTA_AUTH_HEADER_SIZE;

    if (tuya_ota_rws_mgr.target_sub_bin_index == bin_index)
    {
        tuya_ota_check_control_header(p_control_header);
        tuya_ota_rws_mgr.index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header,
                                                                                     false, false, 0);
        if (tuya_ota_rws_mgr.index_in_image_info_list == 0xff)
        {
            return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
        }

        uint32_t dfu_base_addr =
            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
        uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_AUTH_HEADER_SIZE,
                                                        p_auth_header,
                                                        0);

        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }
        else
        {
            tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
        }

        offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_CONTROL_HEADER_SIZE,
                                               p_control_header,
                                               1);

        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }
        else
        {
            tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
        }
        APP_PRINT_INFO1("tuya_ota_handle_control_header_role_related[local]: index in list %d",
                        tuya_ota_rws_mgr.index_in_image_info_list);
    }
#if F_APP_ERWS_SUPPORT
    else
    {
        remote_cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
        remote_cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
        remote_index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header, true, false,
                                                                           0);
        if (remote_index_in_image_info_list == 0xff)
        {
            return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
        }
        tuya_ota_p2s_send_sub_image_auth_control_header(p_auth_header, p_control_header);
        APP_PRINT_INFO1("tuya_ota_handle_control_header_role_related[remote]: index in list %d",
                        remote_index_in_image_info_list);
    }
#endif

    return TUYA_OTA_ERR_NONE;
}

static uint8_t tuya_ota_handle_image_payload_role_related(uint8_t bin_index, uint8_t *p_data)
{
    uint8_t *p = p_data;

    if (tuya_ota_rws_mgr.target_sub_bin_index == bin_index)
    {
        APP_PRINT_INFO3("tuya_ota_handle_image_payload_role_related: 1 cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                        tuya_ota_rws_mgr.cur_sub_image_relative_offset,
                        ota_tmp_buf_left,
                        tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size);
        if (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left >=
            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size)
        {
            uint16_t oversize = (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left) -
                                tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size;
            uint32_t dfu_base_addr =
                tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
            uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
            uint32_t length = ota_tmp_buf_left - oversize;

            uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p, 0);
            if (write_result != 0)
            {
                return TUYA_OTA_ERR_WRITE_FAIL;
            }

            //check image
            void *p_image_header = (void *)
                                   tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;

            bool check_image = tuya_ota_check_image(p_image_header);

            if (!check_image)
            {
                return TUYA_OTA_ERR_CHECK_FAIL;
            }

            tuya_ota_rws_mgr.cur_sub_image_relative_offset = 0;
            tuya_ota_rws_mgr.cur_download_image_index++;

            tuya_ota_rws_mgr.main_header.cur_index++;

            ota_tmp_buf_offset += length;
            ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
        }
        else
        {
            uint32_t dfu_base_addr =
                tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
            uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
            uint32_t length = ota_tmp_buf_left;

            uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p, 0);
            if (write_result != 0)
            {
                return TUYA_OTA_ERR_WRITE_FAIL;
            }
            tuya_ota_rws_mgr.cur_sub_image_relative_offset += length;

            ota_tmp_buf_offset += length;
            ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
        }
    }
#if F_APP_ERWS_SUPPORT
    else
    {
        APP_PRINT_INFO3("tuya_ota_handle_image_payload_role_related: 2 cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                        remote_cur_sub_image_relative_offset,
                        ota_tmp_buf_left,
                        tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size);
        if (remote_cur_sub_image_relative_offset + ota_tmp_buf_left >=
            tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size)
        {
            uint16_t oversize = (remote_cur_sub_image_relative_offset + ota_tmp_buf_left) -
                                tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size;
            uint16_t relay_length = ota_tmp_buf_left - oversize;
            tuya_ota_p2s_send_sub_image_payload(p, relay_length);
            remote_cur_download_image_index++;
            remote_cur_sub_image_relative_offset = 0;

            tuya_ota_rws_mgr.main_header.cur_index++;

            ota_tmp_buf_offset += relay_length;
            ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
        }
        else
        {
            tuya_ota_p2s_send_sub_image_payload(p, ota_tmp_buf_left);
            remote_cur_sub_image_relative_offset += ota_tmp_buf_left;
            ota_tmp_buf_offset += ota_tmp_buf_left;
            ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
        }
    }
#endif

    return TUYA_OTA_ERR_NONE;
}

static uint8_t tuya_ota_handle_control_header_none_related(uint8_t *p_data)
{
    uint8_t *p_auth_header = p_data;
    uint8_t *p_control_header = p_auth_header + OTA_AUTH_HEADER_SIZE;
    tuya_ota_check_control_header(p_control_header);
    tuya_ota_rws_mgr.index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header,
                                                                                 false, false, 0);
    if (tuya_ota_rws_mgr.index_in_image_info_list == 0xff)
    {
        return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
    }

    uint32_t dfu_base_addr =
        tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
    uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
    uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_AUTH_HEADER_SIZE,
                                                    p_auth_header,
                                                    0);

    if (write_result != 0)
    {
        return TUYA_OTA_ERR_WRITE_FAIL;
    }
    else
    {
        tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
    }

    offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
    write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_CONTROL_HEADER_SIZE,
                                           p_control_header,
                                           1);

    if (write_result != 0)
    {
        return TUYA_OTA_ERR_WRITE_FAIL;
    }
    else
    {
        tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
    }
    APP_PRINT_INFO1("tuya_ota_handle_control_header_none_related[local]: index in list %d",
                    tuya_ota_rws_mgr.index_in_image_info_list);

#if F_APP_ERWS_SUPPORT
    remote_cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
    remote_cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
    remote_index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header, true, false,
                                                                       0);
    if (remote_index_in_image_info_list == 0xff)
    {
        return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
    }
    tuya_ota_p2s_send_sub_image_auth_control_header(p_auth_header, p_control_header);
    APP_PRINT_INFO1("tuya_ota_handle_control_header_none_related[remote]: index in list %d",
                    remote_index_in_image_info_list);
#endif
    return TUYA_OTA_ERR_NONE;
}

static uint8_t tuya_ota_handle_image_payload_none_related(uint8_t *p_data)
{
    uint8_t *p = p_data;

    APP_PRINT_INFO3("tuya_ota_handle_image_payload_none_related: cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                    tuya_ota_rws_mgr.cur_sub_image_relative_offset,
                    ota_tmp_buf_left,
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size);

    uint32_t consumption_length_local = 0;
    bool cur_image_finished_local = false;
#if F_APP_ERWS_SUPPORT
    uint32_t consumption_length_remote = 0;
    bool cur_image_finished_remote = false;
#endif
    // local
    if (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left >=
        tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size)
    {

        uint16_t oversize = (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left) -
                            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size;
        uint32_t dfu_base_addr =
            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
        uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        uint32_t length = ota_tmp_buf_left - oversize;

        uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p, 0);
        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }

        //check image
        void *p_image_header = (void *)
                               tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;

        bool check_image = tuya_ota_check_image(p_image_header);

        if (!check_image)
        {
            return TUYA_OTA_ERR_CHECK_FAIL;
        }

        tuya_ota_rws_mgr.cur_sub_image_relative_offset = 0;
        tuya_ota_rws_mgr.cur_download_image_index++;
        consumption_length_local = length;
        cur_image_finished_local = true;

        if (tuya_ota_rws_mgr.cur_download_image_index == tuya_ota_rws_mgr.end_download_image_index)
        {
            tuya_ota_rws_mgr.update_result_local = true;
            tuya_ota_rws_mgr.cur_download_image_index = 0;
        }
        else
        {
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
        }
    }
    else
    {
        uint32_t dfu_base_addr =
            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
        uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        uint32_t length = ota_tmp_buf_left;
        uint8_t *p_data = p;
        uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p_data, 0);
        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }
        tuya_ota_rws_mgr.cur_sub_image_relative_offset += length;
        consumption_length_local = length;

        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
    }
#if F_APP_ERWS_SUPPORT
    // remote
    if (remote_cur_sub_image_relative_offset + ota_tmp_buf_left >=
        tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size)
    {
        uint16_t oversize = (remote_cur_sub_image_relative_offset + ota_tmp_buf_left) -
                            tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size;
        uint16_t relay_length = ota_tmp_buf_left - oversize;
        tuya_ota_p2s_send_sub_image_payload(p, relay_length);
        remote_cur_sub_image_relative_offset = 0;
        remote_cur_download_image_index++;

        consumption_length_remote = relay_length;
        cur_image_finished_remote = true;

        if (remote_cur_download_image_index == remote_end_download_image_index)
        {
            //remote finished, handle by remote and wait relay info from remote
        }
        else
        {
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
        }
    }
    else
    {
        tuya_ota_p2s_send_sub_image_payload(p, ota_tmp_buf_left);
        remote_cur_sub_image_relative_offset += ota_tmp_buf_left;
        consumption_length_remote = ota_tmp_buf_left;
        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
    }

    if (consumption_length_local != consumption_length_remote ||
        cur_image_finished_local != cur_image_finished_remote)
    {
        return TUYA_OTA_ERR_SYNC_FAIL;
    }

    if (cur_image_finished_remote == true && cur_image_finished_local == true)
    {
        tuya_ota_rws_mgr.main_header.cur_index++;
    }
#else
    if (cur_image_finished_local == true)
    {
        tuya_ota_rws_mgr.main_header.cur_index++;
    }
#endif

    ota_tmp_buf_offset += consumption_length_local;
    ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;

    return TUYA_OTA_ERR_NONE;
}

static uint8_t tuya_ota_handle_control_header_bank_related(uint8_t cur_bank, uint8_t need_bank,
                                                           uint8_t *p_data)
{
    APP_PRINT_INFO2("tuya_ota_handle_control_header_bank_related: cur_bank %d, need_bank %d",
                    cur_bank, need_bank);
    uint8_t *p_auth_header = p_data;
    uint8_t *p_control_header = p_auth_header + OTA_AUTH_HEADER_SIZE;

    tuya_ota_check_control_header(p_control_header);
#if F_APP_ERWS_SUPPORT
    if (tuya_ota_rws_mgr.local_active_bank == cur_bank &&
        tuya_ota_rws_mgr.remote_active_bank == cur_bank)
#else
    if (tuya_ota_rws_mgr.local_active_bank == cur_bank)
#endif
    {
        cur_sub_image_relative_offset_for_ignore += OTA_AUTH_HEADER_SIZE;
        cur_sub_image_relative_offset_for_ignore += OTA_CONTROL_HEADER_SIZE;
        ignore_index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header, false, true,
                                                                           cur_bank);
        if (ignore_index_in_image_info_list == 0xff)
        {
            return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
        }
        APP_PRINT_INFO1("tuya_ota_handle_control_header_bank_related[ignore]: index in list %d",
                        ignore_index_in_image_info_list);
    }

    if (tuya_ota_rws_mgr.local_active_bank == need_bank)
    {
        tuya_ota_rws_mgr.index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header,
                                                                                     false, false, cur_bank);
        if (tuya_ota_rws_mgr.index_in_image_info_list == 0xff)
        {
            return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
        }

        uint32_t dfu_base_addr =
            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
        uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_AUTH_HEADER_SIZE,
                                                        p_auth_header,
                                                        0);

        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }
        else
        {
            tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
        }

        offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
        write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, OTA_CONTROL_HEADER_SIZE,
                                               p_control_header, 1);

        if (write_result != 0)
        {
            return TUYA_OTA_ERR_WRITE_FAIL;
        }
        else
        {
            tuya_ota_rws_mgr.cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
        }
        APP_PRINT_INFO1("tuya_ota_handle_control_header_bank_related[local]: index in list %d",
                        tuya_ota_rws_mgr.index_in_image_info_list);
    }
#if F_APP_ERWS_SUPPORT
    if (tuya_ota_rws_mgr.remote_active_bank == need_bank)
    {
        remote_cur_sub_image_relative_offset += OTA_AUTH_HEADER_SIZE;
        remote_cur_sub_image_relative_offset += OTA_CONTROL_HEADER_SIZE;
        remote_index_in_image_info_list = tuya_ota_get_cur_sub_image_index(p_control_header, true, false,
                                                                           cur_bank);
        if (remote_index_in_image_info_list == 0xff)
        {
            return TUYA_OTA_ERR_INDEX_SERARCH_FAIL;
        }
        tuya_ota_p2s_send_sub_image_auth_control_header(p_auth_header, p_control_header);
        APP_PRINT_INFO1("tuya_ota_handle_control_header_bank_related[remote]: index in list %d",
                        remote_index_in_image_info_list);
    }
#endif

    return TUYA_OTA_ERR_NONE;
}

static uint8_t tuya_ota_handle_image_payload_bank_related(uint8_t cur_bank, uint8_t need_bank,
                                                          uint8_t *p_data)
{
    uint8_t *p = p_data;

    uint32_t consumption_length = 0;
    uint32_t consumption_length_local = 0;
    bool cur_image_finished_local = false;
#if F_APP_ERWS_SUPPORT
    uint32_t consumption_length_remote = 0;
    bool cur_image_finished_remote = false;
#endif

#if F_APP_ERWS_SUPPORT
    if (tuya_ota_rws_mgr.local_active_bank == cur_bank &&
        tuya_ota_rws_mgr.remote_active_bank == cur_bank)
#else
    if (tuya_ota_rws_mgr.local_active_bank == cur_bank) //ignore
#endif
    {
        APP_PRINT_INFO3("tuya_ota_handle_image_payload_bank_related_cur_bank: cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                        cur_sub_image_relative_offset_for_ignore,
                        ota_tmp_buf_left,
                        tuya_ota_rws_mgr.sub_bin[0].sub_image_header[ignore_index_in_image_info_list].size);
        if (cur_sub_image_relative_offset_for_ignore + ota_tmp_buf_left >=
            tuya_ota_rws_mgr.sub_bin[0].sub_image_header[ignore_index_in_image_info_list].size)
        {
            uint16_t oversize = (cur_sub_image_relative_offset_for_ignore + ota_tmp_buf_left) -
                                tuya_ota_rws_mgr.sub_bin[0].sub_image_header[ignore_index_in_image_info_list].size;
            consumption_length = ota_tmp_buf_left - oversize;

            cur_sub_image_relative_offset_for_ignore = 0;

            tuya_ota_rws_mgr.main_header.cur_index++;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;

        }
        else
        {
            cur_sub_image_relative_offset_for_ignore += ota_tmp_buf_left;
            consumption_length = ota_tmp_buf_left;
            tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
        }
    }
    else if (tuya_ota_rws_mgr.local_active_bank == need_bank
             || (tuya_ota_rws_mgr.main_header.is_rws == 1 && tuya_ota_rws_mgr.remote_active_bank == need_bank))
    {
        if (tuya_ota_rws_mgr.local_active_bank == need_bank)
        {
            APP_PRINT_INFO3("tuya_ota_handle_image_payload_bank_related_need_bank: cur_offset 0x%08x, coming length 0x%08x, target 0x%08x",
                            tuya_ota_rws_mgr.cur_sub_image_relative_offset,
                            ota_tmp_buf_left,
                            tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size);
            if (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left >=
                tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size)
            {
                uint16_t oversize = (tuya_ota_rws_mgr.cur_sub_image_relative_offset + ota_tmp_buf_left) -
                                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].size;
                uint32_t dfu_base_addr =
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
                uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
                uint32_t length = ota_tmp_buf_left - oversize;

                uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p, 0);
                if (write_result != 0)
                {
                    return TUYA_OTA_ERR_WRITE_FAIL;
                }

                //check image
                void *p_image_header = (void *)
                                       tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;

                bool check_image = tuya_ota_check_image(p_image_header);

                if (!check_image)
                {
                    return TUYA_OTA_ERR_CHECK_FAIL;
                }

                tuya_ota_rws_mgr.cur_sub_image_relative_offset = 0;
                tuya_ota_rws_mgr.cur_download_image_index++;
                consumption_length_local = length;
                cur_image_finished_local = true;

                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
            }
            else
            {
                uint32_t dfu_base_addr =
                    tuya_ota_rws_mgr.sub_bin[tuya_ota_rws_mgr.target_sub_bin_index].sub_image_header[tuya_ota_rws_mgr.index_in_image_info_list].download_addr;
                uint32_t offset = tuya_ota_rws_mgr.cur_sub_image_relative_offset;
                uint32_t length = ota_tmp_buf_left;

                uint32_t write_result = tuya_ota_write_to_flash(dfu_base_addr, offset, length, p, 0);
                if (write_result != 0)
                {
                    return TUYA_OTA_ERR_WRITE_FAIL;
                }
                tuya_ota_rws_mgr.cur_sub_image_relative_offset += length;
                consumption_length_local = length;

                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
            }
        }
#if F_APP_ERWS_SUPPORT
        if (tuya_ota_rws_mgr.remote_active_bank == need_bank)
        {
            if (remote_cur_sub_image_relative_offset + ota_tmp_buf_left >=
                tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size)
            {
                uint16_t oversize = (remote_cur_sub_image_relative_offset + ota_tmp_buf_left) -
                                    tuya_ota_rws_mgr.sub_bin[remote_target_sub_bin_index].sub_image_header[remote_index_in_image_info_list].size;
                uint16_t relay_length = ota_tmp_buf_left - oversize;
                tuya_ota_p2s_send_sub_image_payload(p, relay_length);
                remote_cur_sub_image_relative_offset = 0;
                remote_cur_download_image_index++;

                consumption_length_remote = relay_length;
                cur_image_finished_remote = true;

                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
            }
            else
            {
                tuya_ota_p2s_send_sub_image_payload(p, ota_tmp_buf_left);
                remote_cur_sub_image_relative_offset += ota_tmp_buf_left;
                consumption_length_remote = ota_tmp_buf_left;
                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
            }
        }
#endif
    }
#if F_APP_ERWS_SUPPORT
    if (tuya_ota_rws_mgr.local_active_bank == need_bank &&
        tuya_ota_rws_mgr.remote_active_bank == need_bank)
    {
        if (consumption_length_local != consumption_length_remote ||
            cur_image_finished_local != cur_image_finished_remote)
        {
            return TUYA_OTA_ERR_SYNC_FAIL;
        }

        if (cur_image_finished_remote == true && cur_image_finished_local == true)
        {
            tuya_ota_rws_mgr.main_header.cur_index++;
        }
        ota_tmp_buf_offset += consumption_length_local;
    }
    else if (tuya_ota_rws_mgr.local_active_bank == need_bank ||
             tuya_ota_rws_mgr.remote_active_bank == need_bank)
    {
        if (cur_image_finished_local || cur_image_finished_remote)
        {
            tuya_ota_rws_mgr.main_header.cur_index++;
        }
        if (tuya_ota_rws_mgr.local_active_bank == need_bank)
        {
            ota_tmp_buf_offset += consumption_length_local;
        }
        else if (tuya_ota_rws_mgr.remote_active_bank == need_bank)
        {
            ota_tmp_buf_offset += consumption_length_remote;
        }
    }
    else
    {
        ota_tmp_buf_offset += consumption_length;
    }
#else
    if (tuya_ota_rws_mgr.local_active_bank == need_bank)
    {
        if (cur_image_finished_local)
        {
            tuya_ota_rws_mgr.main_header.cur_index++;
        }
        ota_tmp_buf_offset += consumption_length_local;
    }
    else
    {
        ota_tmp_buf_offset += consumption_length;
    }
#endif

    ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;

    return TUYA_OTA_ERR_NONE;
}

static void tuya_ota_handle_sub_image_header(uint8_t sub_bin_index, uint8_t *p_data)
{
    for (uint8_t i = 0; i < tuya_ota_rws_mgr.main_header.num_sub_image_each_bin; i++)
    {
        LE_STREAM_TO_UINT16(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].image_id,
                            p_data);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].bud_role,
                           p_data);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].bank_info,
                           p_data);
        LE_STREAM_TO_UINT32(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr,
                            p_data);
        LE_STREAM_TO_UINT32(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].size, p_data);

        memcpy(tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].rsv, p_data, 4);
        p_data += 4;

        if (tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].image_id == 0x27a4)
        {
            if (tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr != 0)
            {
                vp_index_bank0 = i;
            }
            else
            {
                if (vp_index_bank0 != 0xff)
                {
                    tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr =
                        tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[vp_index_bank0].download_addr;
                }
            }
        }

        APP_PRINT_INFO4("tuya_ota_handle_sub_image_header: sub_bin_index %d, image_id 0x%04x, download_addr %p, size 0x%08x",
                        sub_bin_index,
                        tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].image_id,
                        tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].download_addr,
                        tuya_ota_rws_mgr.sub_bin[sub_bin_index].sub_image_header[i].size);
    }
}

static void tuya_ota_handle_auth_control_header(uint8_t *p_data)
{
    uint8_t handle_result = TUYA_OTA_ERR_NONE;
    APP_PRINT_INFO1("tuya_ota_handle_auth_control_header: cur_index %d",
                    tuya_ota_rws_mgr.main_header.cur_index);
    if (tuya_ota_rws_mgr.main_header.cur_index <
        tuya_ota_rws_mgr.main_header.num_role_related) //role related
    {
        uint8_t bin_index = 0;
#if F_APP_ERWS_SUPPORT
        if (tuya_ota_rws_mgr.main_header.cur_index >= tuya_ota_rws_mgr.main_header.num_role_related /
            2) //pri
        {
            bin_index = 1;
        }
#endif
        handle_result = tuya_ota_handle_control_header_role_related(bin_index, p_data);
    }
    else if (tuya_ota_rws_mgr.main_header.cur_index < tuya_ota_rws_mgr.main_header.num_role_related +
             tuya_ota_rws_mgr.main_header.num_bank_related)
    {
        if (tuya_ota_rws_mgr.main_header.cur_index < tuya_ota_rws_mgr.main_header.num_role_related +
            tuya_ota_rws_mgr.main_header.num_bank_related / 2) //bank0
        {
            handle_result = tuya_ota_handle_control_header_bank_related(IMAGE_LOCATION_BANK0,
                                                                        IMAGE_LOCATION_BANK1, p_data);
        }
        else //bank1
        {
            handle_result = tuya_ota_handle_control_header_bank_related(IMAGE_LOCATION_BANK1,
                                                                        IMAGE_LOCATION_BANK0, p_data);
        }
    }
    else //none related
    {
        handle_result = tuya_ota_handle_control_header_none_related(p_data);
    }
    if (handle_result != TUYA_OTA_ERR_NONE)
    {
        APP_PRINT_INFO2("tuya_ota_handle_ota_payload: control_header cur_index %d, err code: %d",
                        tuya_ota_rws_mgr.main_header.cur_index, handle_result);
        tuya_ota_result = 0x02;
    }
}

static void tuya_ota_handle_ota_payload(void)
{
    ota_tmp_buf_offset = 0;
    ota_tmp_buf_left = ota_tmp_buf_used_size;

    uint8_t *p = p_temp_buffer_head;

    if (tuya_ota_rws_mgr.sub_state == TUYA_OTA_SUB_STATE_RX_MAIN_HEADER)
    {
        //handle main header
        LE_STREAM_TO_UINT16(tuya_ota_rws_mgr.main_header.signature, p);
        LE_STREAM_TO_UINT16(tuya_ota_rws_mgr.main_header.version, p);
        LE_STREAM_TO_UINT32(tuya_ota_rws_mgr.main_header.file_size, p);

        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.main_header.num_role_related, p);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.main_header.num_bank_related, p);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.main_header.num_none_related, p);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.main_header.num_sub_image, p);

        LE_STREAM_TO_UINT16(tuya_ota_rws_mgr.main_header.ota_version, p);
        LE_STREAM_TO_UINT8(tuya_ota_rws_mgr.main_header.is_rws, p);

        p += 5;
        if (tuya_ota_rws_mgr.main_header.signature != 0x55AA)
        {
            APP_PRINT_ERROR0("tuya_ota_handle_ota_payload: err signature, please check image");
            tuya_ota_result = 0x02;
            return;
        }

        uint16_t local_version = tuya_ota_get_app_ui_version();

        if (tuya_ota_rws_mgr.main_header.ota_version != (0xffff & tuya_iot_image_version)
            || tuya_ota_rws_mgr.main_header.ota_version < local_version)
        {
            APP_PRINT_INFO3("tuya_ota_handle_ota_payload: err version, packet ver 0x%04x, iot ver 0x%04x, local ver 0x%04x",
                            tuya_ota_rws_mgr.main_header.ota_version, (0xffff & tuya_iot_image_version), local_version);
            tuya_ota_result = 0x02;
            return;
        }

        tuya_ota_rws_mgr.main_header.num_sub_image_each_bin = tuya_ota_rws_mgr.main_header.is_rws ?
                                                              tuya_ota_rws_mgr.main_header.num_sub_image / 2 : tuya_ota_rws_mgr.main_header.num_sub_image;
        tuya_ota_rws_mgr.main_header.cur_index = 0;
        tuya_ota_rws_mgr.main_header.end_index = tuya_ota_rws_mgr.main_header.num_role_related +
                                                 tuya_ota_rws_mgr.main_header.num_bank_related + tuya_ota_rws_mgr.main_header.num_none_related;

        APP_PRINT_INFO5("tuya_ota_handle_ota_payload: main header is_rws %d, num_role_related 0x%d, num_bank_related %d, num_none_related %d, num_sub_image%d",
                        tuya_ota_rws_mgr.main_header.is_rws,
                        tuya_ota_rws_mgr.main_header.num_role_related,
                        tuya_ota_rws_mgr.main_header.num_bank_related,
                        tuya_ota_rws_mgr.main_header.num_none_related,
                        tuya_ota_rws_mgr.main_header.num_sub_image);

        if ((tuya_ota_rws_mgr.main_header.is_rws && app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            || (!tuya_ota_rws_mgr.main_header.is_rws && app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE))
        {
            APP_PRINT_ERROR0("OTA image type does not correspond with bud role!");
            tuya_ota_result = 0x02;
            return;
        }

#if F_APP_ERWS_SUPPORT
        tuya_ota_p2s_send_main_header();
#endif
        tuya_ota_check_set_cur_sub_bin_info();

        ota_tmp_buf_offset += OTA_MAIN_HEADER_SIZE;
        ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;
        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_BIN0_SUB_IMAGE_HEADER;
    }
    else if (tuya_ota_rws_mgr.sub_state == TUYA_OTA_SUB_STATE_RX_BIN0_SUB_IMAGE_HEADER)
    {
        uint16_t sub_image_header_size_total = tuya_ota_rws_mgr.main_header.num_sub_image_each_bin * sizeof(
                                                   T_SUB_IMAGE_HEADER);

        if (ota_tmp_buf_left < sub_image_header_size_total)
        {
            return;
        }

        tuya_ota_handle_sub_image_header(0, p);

        ota_tmp_buf_offset += sub_image_header_size_total;
        ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;

#if F_APP_ERWS_SUPPORT
        tuya_ota_p2s_send_bin_info(0);
        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_BIN1_SUB_IMAGE_HEADER;
#else
        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
#endif
    }
#if F_APP_ERWS_SUPPORT
    else if (tuya_ota_rws_mgr.sub_state == TUYA_OTA_SUB_STATE_RX_BIN1_SUB_IMAGE_HEADER)
    {
        uint16_t sub_image_header_size_total = tuya_ota_rws_mgr.main_header.num_sub_image_each_bin * sizeof(
                                                   T_SUB_IMAGE_HEADER); //18*16
        if (ota_tmp_buf_left < sub_image_header_size_total)
        {
            return;
        }
        tuya_ota_handle_sub_image_header(1, p);

        ota_tmp_buf_offset += sub_image_header_size_total;
        ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;

        tuya_ota_p2s_send_bin_info(1);

        remote_cur_download_image_index = 0;
        remote_end_download_image_index = tuya_ota_rws_mgr.end_download_image_index;
        remote_cur_sub_image_relative_offset = 0;

        tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
    }
#endif
    else if (tuya_ota_rws_mgr.sub_state == TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER)
    {
        if (p_auth_ctrl_header_buf == NULL)
        {
            p_auth_ctrl_header_buf = (uint8_t *)audio_probe_media_buffer_malloc(
                                         OTA_AUTH_CONTROL_HEADER_SIZE); //free when ota fail or control header handled
        }
        if (p_auth_ctrl_header_buf != NULL)
        {
            uint16_t write_len = ota_tmp_buf_left < (OTA_AUTH_CONTROL_HEADER_SIZE - auth_ctrl_header_rx_offset)
                                 ?
                                 ota_tmp_buf_left :
                                 (OTA_AUTH_CONTROL_HEADER_SIZE - auth_ctrl_header_rx_offset);
            memcpy(p_auth_ctrl_header_buf + auth_ctrl_header_rx_offset, p, write_len);
            auth_ctrl_header_rx_offset += write_len;
            APP_PRINT_INFO2("tuya_ota_handle_ota_payload: TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER rcv %d / %d",
                            auth_ctrl_header_rx_offset, OTA_AUTH_CONTROL_HEADER_SIZE);

            ota_tmp_buf_offset += write_len;
            ota_tmp_buf_left = ota_tmp_buf_used_size - ota_tmp_buf_offset;

            if (auth_ctrl_header_rx_offset == OTA_AUTH_CONTROL_HEADER_SIZE)
            {
                tuya_ota_handle_auth_control_header(p_auth_ctrl_header_buf);
                if (p_auth_ctrl_header_buf)
                {
                    audio_probe_media_buffer_free(p_auth_ctrl_header_buf);
                    p_auth_ctrl_header_buf = NULL;
                }
                auth_ctrl_header_rx_offset = 0;
                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD;
            }
            else if (auth_ctrl_header_rx_offset > OTA_AUTH_CONTROL_HEADER_SIZE) //impossible
            {
                tuya_ota_result = 0x02;
                return;
            }
            else
            {
                tuya_ota_rws_mgr.sub_state = TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER;
            }
        }
        else
        {
            APP_PRINT_ERROR0("tuya_ota_handle_ota_payload: TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER malloc fail");
            tuya_ota_result = 0x02;
            return;
        }
    }
    else if (tuya_ota_rws_mgr.sub_state == TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD)
    {
        if (ota_tmp_buf_left <= 0)
        {
            tuya_ota_result = 0x02;
            return;
        }

        uint8_t handle_result = TUYA_OTA_ERR_NONE;

        if (tuya_ota_rws_mgr.main_header.cur_index <
            tuya_ota_rws_mgr.main_header.num_role_related) //role related
        {
            uint8_t bin_index = 0;
#if F_APP_ERWS_SUPPORT
            if (tuya_ota_rws_mgr.main_header.cur_index >= tuya_ota_rws_mgr.main_header.num_role_related /
                2) //pri
            {
                bin_index = 1;
            }
#endif
            handle_result = tuya_ota_handle_image_payload_role_related(bin_index, p);
        }
        else if (tuya_ota_rws_mgr.main_header.cur_index < tuya_ota_rws_mgr.main_header.num_role_related +
                 tuya_ota_rws_mgr.main_header.num_bank_related)
        {
            if (tuya_ota_rws_mgr.main_header.cur_index < tuya_ota_rws_mgr.main_header.num_role_related +
                tuya_ota_rws_mgr.main_header.num_bank_related / 2) //bank0
            {
                handle_result = tuya_ota_handle_image_payload_bank_related(IMAGE_LOCATION_BANK0,
                                                                           IMAGE_LOCATION_BANK1, p);
            }
            else //bank1
            {
                handle_result = tuya_ota_handle_image_payload_bank_related(IMAGE_LOCATION_BANK1,
                                                                           IMAGE_LOCATION_BANK0, p);
            }
        }
        else //none related
        {
            handle_result = tuya_ota_handle_image_payload_none_related(p);
        }
        if (handle_result != TUYA_OTA_ERR_NONE)
        {
            APP_PRINT_INFO3("tuya_ota_handle_ota_payload: control_header cur_index %d, download_index %d, err code: %d",
                            tuya_ota_rws_mgr.main_header.cur_index, tuya_ota_rws_mgr.cur_download_image_index, handle_result);
            tuya_ota_result = 0x02;
        }
    }
#if F_APP_ERWS_SUPPORT
    if (!need_relay_rsp_flag)
    {
        memmove(p_temp_buffer_head, p_temp_buffer_head + ota_tmp_buf_offset, ota_tmp_buf_left);
        ota_tmp_buf_used_size = ota_tmp_buf_left;
        if (ota_tmp_buf_used_size > 0)
        {
            tuya_ota_handle_ota_payload();
        }
        else
        {
            tuya_ota_response_data_req();
        }
    }
#else
    memmove(p_temp_buffer_head, p_temp_buffer_head + ota_tmp_buf_offset, ota_tmp_buf_left);
    ota_tmp_buf_used_size = ota_tmp_buf_left;
    if (ota_tmp_buf_used_size > 0)
    {
        tuya_ota_handle_ota_payload();
    }
#endif
}

static void tuya_ota_response_data_req(void)
{
    last_pkt_idx = recv_pkt_idx;
    uint8_t ota_data_rsp[2];
    tuya_ble_ota_response_t rsp_packet;

    ota_data_rsp[0] = TUYA_OTA_TYPE;
    ota_data_rsp[1] = tuya_ota_result;
    if (tuya_ota_result)
    {
        APP_PRINT_INFO1("tuya_ota_data_req_handle: tuya_ota_result err %d", tuya_ota_result);
        tuya_ota_exit(); //TODO: need?
    }
    rsp_packet.type = TUYA_BLE_OTA_DATA;
    rsp_packet.data_len = 2;
    rsp_packet.p_data = (uint8_t *)&ota_data_rsp;

    if (tuya_ble_ota_response(&rsp_packet) != TUYA_BLE_SUCCESS)
    {
        tuya_ota_exit();
    }
}

static void tuya_ota_data_req_handle(uint8_t *recv_data, uint32_t recv_len)
{
    uint16_t recv_pkt_len;
    uint8_t *p_data = &recv_data[7];

    if (tuya_ota_status != TUYA_OTA_STATUS_FILE_DATA)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_data_req_handle: current ota status is: %d !",
                           tuya_ota_status);
        return;
    }

    recv_pkt_idx = (recv_data[1] << 8) | recv_data[2];
    recv_pkt_len = (recv_data[3] << 8) | recv_data[4];

    if (recv_pkt_idx != (last_pkt_idx + 1) && (recv_pkt_idx != 0))
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_data_req_handle: coming len + used buf > TUYA_OTA_BUF_SIZE");
        tuya_ota_result = 0x01;
    }

    if (p_temp_buffer_head != NULL)
    {
        if ((recv_pkt_len + ota_tmp_buf_used_size) > TUYA_OTA_BUF_SIZE)
        {
            tuya_ota_result = 0x02;
            TUYA_BLE_LOG_ERROR("tuya_ota_data_req_handle: coming len + used buf > TUYA_OTA_BUF_SIZE");
        }
        else
        {
            memcpy(p_temp_buffer_head + ota_tmp_buf_used_size, p_data, recv_pkt_len);
            ota_tmp_buf_used_size += recv_pkt_len;
            tuya_ota_handle_ota_payload();
        }
    }
    else
    {
        tuya_ota_result = 0x02;
    }
#if F_APP_ERWS_SUPPORT
    if (tuya_ota_rws_mgr.sub_state < TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER ||
        tuya_ota_result != 0)
#endif
    {
        tuya_ota_response_data_req();
    }
}

static void tuya_ota_end_req_handle(uint8_t *recv_data, uint32_t recv_len)
{
    TUYA_BLE_LOG_INFO("tuya_ota_end_req_handle");

#if F_APP_ERWS_SUPPORT
    tuya_ota_send_remote_msg(TUYA_OTA_P2S_MSG_OTA_END, NULL, 0);
    //respond to phone when ack from secondary received
    //respond in tuya_ota_response_ota_completed()
#else
    tuya_ota_response_ota_completed();
#endif
    tuya_ota_free_buffer();
}

static void tuya_ota_proc(uint16_t cmd, uint8_t *recv_data, uint32_t recv_len)
{
    switch (cmd)
    {
    case TUYA_BLE_OTA_REQ:
        tuya_ota_start_req_handle(recv_data, recv_len);
        break;

    case TUYA_BLE_OTA_FILE_INFO:
        tuya_ota_file_info_req_handle(recv_data, recv_len);
        break;

    case TUYA_BLE_OTA_FILE_OFFSET_REQ:
        tuya_ota_offset_req_handle(recv_data, recv_len);
        break;

    case TUYA_BLE_OTA_DATA:
        tuya_ota_data_req_handle(recv_data, recv_len);
        break;

    case TUYA_BLE_OTA_END:
        tuya_ota_end_req_handle(recv_data, recv_len);
        break;

    default:
        break;
    }
}

bool tuya_ota_send_event_msg(uint16_t cmd, uint8_t *p_data, uint16_t len)
{
    tuya_ota_data_type_t *p_ota_data = audio_probe_media_buffer_malloc(sizeof(
                                                                           tuya_ota_data_type_t) + len);

    T_IO_MSG tuya_io_msg;
    unsigned char app_event = EVENT_IO_TO_APP;
    tuya_io_msg.type = IO_MSG_TYPE_TUYA_OTA;
    tuya_io_msg.subtype = cmd;

    if (p_ota_data)
    {
        p_ota_data->length = len;
        memcpy(p_ota_data->data, p_data, len);

        tuya_io_msg.u.buf = p_ota_data;

        if (os_msg_send(audio_io_queue_handle, &tuya_io_msg, 0))
        {
            if (os_msg_send(audio_evt_queue_handle, &app_event, 0))
            {
                // APP_PRINT_INFO2("tuya_ota_send_event_msg: success, evt_addr 0x%x, evt %d", tuya_io_msg.u.buf,
                //                 evt->hdr.event);
                return true;
            }
            else
            {
                audio_probe_media_buffer_free(p_ota_data);
                p_ota_data = NULL;
                return false;
            }
        }
        else
        {
            audio_probe_media_buffer_free(p_ota_data);
            p_ota_data = NULL;
            return false;
        }
    }
    else
    {
        APP_PRINT_INFO0("tuya_ota_send_event_msg: malloc fail");
        return false;
    }
}

void tuya_ota_handle_msg(T_IO_MSG io_msg)
{
    if (io_msg.u.buf)
    {
        tuya_ota_data_type_t *p_ota_data = (tuya_ota_data_type_t *)io_msg.u.buf;
        uint16_t len = p_ota_data->length;
        uint8_t *p_data = p_ota_data->data;
        tuya_ota_proc(io_msg.subtype, p_data, len);
        audio_probe_media_buffer_free(p_ota_data);
    }
    else
    {
        APP_PRINT_ERROR0("rtk_tuya_handle_event_msg: fail");
    }


}

uint32_t tuya_ble_ota_init(void)
{
    TUYA_BLE_LOG_INFO("tuya_ble_ota_init");
    bool ret = true;
    tuya_ota_status = TUYA_OTA_STATUS_NONE;
    recv_pkt_idx = 0;
    last_pkt_idx = 0;
#if F_APP_ERWS_SUPPORT
    ret = app_relay_cback_register(NULL, app_tuya_ota_parse_cback, APP_MODULE_TYPE_TUYA_OTA,
                                   TUYA_OTA_REMOTE_MSG_NUM);
#endif
    return ret;
}

#endif
