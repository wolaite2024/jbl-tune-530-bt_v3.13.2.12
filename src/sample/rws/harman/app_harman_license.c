#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "fmc_api.h"
#include "ftl.h"
#include "version.h"
#include "os_mem.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_ota.h"
#include "app_cmd.h"
#include "app_harman_license.h"
#include "app_harman_vendor_cmd.h"
#include "system_status_api.h"
#include "pm.h"
#include "app_mmi.h"
#include "app_adp.h"
#include "app_dlps.h"

#if HARMAN_DSP_LICENSE_SUPPORT
#include "audio_probe.h"
#endif

static bool need_clear_total_time = false;

static char harman_serial_number[HARMAN_SERIALS_NUM_LEN] = "KT0288-CP0000001";
static uint8_t harman_pid_idx = 0;

#if HARMAN_PACE_SUPPORT
static uint16_t harman_product_id = PRODUCT_ID_ENDURANCE_PACE;
static uint8_t harman_color_id = COLOR_ID_BLACK;
#elif HARMAN_RUN3_SUPPORT
static uint16_t harman_product_id = PRODUCT_ID_RUN3_WIRELESS;
static uint8_t harman_color_id = COLOR_ID_BLACK_GRAY;
#else
static uint16_t harman_product_id = PRODUCT_ID_T135;
static uint8_t harman_color_id = COLOR_ID_BLACK;
#endif

static void app_harman_license_gfps_info_get_from_flash(uint8_t pid_idx, uint8_t cid_idx,
                                                        T_APP_HARMAN_GFPS_INFO *p_gfps_info)
{
    uint32_t offset = pid_idx * APP_HARMAN_LICENSE_ONE_PRODUCT_LEN + APP_HARMAN_PRODUCT_INFO_LEN +
                      cid_idx * APP_HARMAN_GFPS_INFO_LEN;

    fmc_flash_nor_read(APP_HARMAN_LISENCE_OFFSET + offset, p_gfps_info, APP_HARMAN_GFPS_INFO_LEN);

    // APP_PRINT_TRACE5("app_harman_license_gfps_info_get_from_flash: pid_idx: %d, cid_idx: %d, offset: 0x%x, "
    //                  "color_id: 0x%04x, gfps_model_id: %s",
    //                  pid_idx, cid_idx, offset, p_gfps_info->color_id, TRACE_BINARY(3, p_gfps_info->model_id));
}

static void app_harman_license_product_info_get_from_flash(uint8_t pid_idx,
                                                           T_APP_HARMAN_PRODUCT_INFO *p_product_info)
{
    uint32_t offset = pid_idx * APP_HARMAN_LICENSE_ONE_PRODUCT_LEN;

    fmc_flash_nor_read(APP_HARMAN_LISENCE_OFFSET + offset, p_product_info, APP_HARMAN_PRODUCT_INFO_LEN);

    // APP_PRINT_TRACE4("app_harman_license_product_info_get_from_flash: pid_idx: %d, offset: 0x%x, product_id: 0x%04x, device_name: %s",
    //                  pid_idx, offset, p_product_info->id,
    //                  TRACE_STRING((char *)&p_product_info->device_name[0]));
}

static uint8_t app_harman_license_pid_idx_get(uint8_t *p_device_name, uint16_t name_len,
                                              uint16_t *p_pid)
{
    uint8_t pid_idx = 0;
    T_APP_HARMAN_PRODUCT_INFO product_info = {0};

    /* Find PID which need same with McuConfig setting */
    for (pid_idx = 0; pid_idx < HARMAN_PRODUCT_TOTAL_NUM; pid_idx ++)
    {
        app_harman_license_product_info_get_from_flash(pid_idx, &product_info);
        if (!memcmp(product_info.device_name, p_device_name, name_len))
        {
            break;
        }
    }

    /* Not find active PID, set to T135 */
    if (pid_idx == HARMAN_PRODUCT_TOTAL_NUM)
    {
        pid_idx = 0;
        app_harman_license_product_info_get_from_flash(pid_idx, &product_info);
        if ((product_info.id == 0xFFFF) || (product_info.id == 0))
        {
            product_info.id = harman_product_id;
        }
    }
    *p_pid = product_info.id;

    DBG_DIRECT("app_harman_license_pid_idx_get: pid_idx: 0x%x, product_id: 0x%04x", pid_idx,
               product_info.id);

    return pid_idx;
}

static bool app_harman_license_gfps_info_get(uint8_t pid_idx, uint8_t color_id,
                                             T_APP_HARMAN_GFPS_INFO *p_gfps_info)
{
    bool is_suc = false;
    uint8_t cid_idx = 0;

    /* Find CID which need same with one of device table */
    for (cid_idx = 0; cid_idx < HARMAN_COLOR_TOTAL_NUM; cid_idx ++)
    {
        app_harman_license_gfps_info_get_from_flash(pid_idx, cid_idx, p_gfps_info);
        if ((p_gfps_info->color_id != 0xFF) && (color_id == p_gfps_info->color_id))
        {
            is_suc = true;
            break;
        }
    }
    /* Not find active CID, set to BLACK */
    if (cid_idx == HARMAN_COLOR_TOTAL_NUM)
    {
        cid_idx = 0;
        app_harman_license_gfps_info_get_from_flash(pid_idx, cid_idx, p_gfps_info);
        if (p_gfps_info->color_id == 0xFF)
        {
            p_gfps_info->color_id = harman_color_id;
        }
    }

    DBG_DIRECT("app_harman_license_gfps_info_get: is_suc: %d, pid_idx: %d, color_id: 0x%x, cid_idx: 0x%x",
               is_suc, pid_idx, color_id, cid_idx);
    return is_suc;
}

void app_harman_license_product_info_dump(void)
{
    APP_PRINT_INFO3("app_harman_license_product_info_dump: harman_product_id: 0x%04x, "
                    "harman_color_id: 0x%x, serial_number: %b",
                    harman_product_id, harman_color_id,
                    TRACE_BINARY(HARMAN_SERIALS_NUM_LEN, harman_serial_number));
}

void app_harman_license_gfps_info_dump(void)
{
    APP_PRINT_INFO1("app_harman_license_gfps_info_dump: gfps_model_id: %b",
                    TRACE_BINARY(3, extend_app_cfg_const.gfps_model_id));
    // APP_PRINT_INFO1("app_harman_license_gfps_info_dump: gfps_public_key: %b",
    //                 TRACE_BINARY(64, extend_app_cfg_const.gfps_public_key));
    // APP_PRINT_INFO1("app_harman_license_gfps_info_dump: gfps_private_key: %b",
    //                 TRACE_BINARY(32, extend_app_cfg_const.gfps_private_key));
}

static void app_harman_license_save_to_ftl(void)
{
    uint8_t temp_license_data[HARMAN_LICENSE_SIZE_SAVE_TO_FTL] = {0};
    uint8_t temp_size = 0;

    memcpy(&temp_license_data[temp_size], (uint8_t *)&harman_serial_number[0], HARMAN_SERIALS_NUM_LEN);
    temp_size += HARMAN_SERIALS_NUM_LEN;
    memcpy(&temp_license_data[temp_size], (uint8_t *)&harman_product_id, HARMAN_PRODUCT_ID_LEN);
    temp_size += HARMAN_PRODUCT_ID_LEN;
    memcpy(&temp_license_data[temp_size], &harman_color_id, HARMAN_COLOR_ID_LEN);

    ftl_save_to_storage(&temp_license_data[0], HARMAN_SERIALS_NUM_OFFSET,
                        HARMAN_LICENSE_SIZE_SAVE_TO_FTL);
}

static void app_harman_license_load_from_ftl(void)
{
    uint8_t temp_license_data[HARMAN_LICENSE_SIZE_SAVE_TO_FTL] = {0};
    uint8_t temp_size = 0;
    uint32_t ret = 0;
    bool need_save = false;

    ret = ftl_load_from_storage(&temp_license_data[0], HARMAN_SERIALS_NUM_OFFSET,
                                HARMAN_LICENSE_SIZE_SAVE_TO_FTL);
    DBG_DIRECT("app_harman_license_load_from_ftl_before: harman_product_id: 0x%04x, harman_color_id: 0x%x, "
               "ret: %d, read_from_FTL: 0x%x, 0x%x, 0x%x",
               harman_product_id, harman_color_id, ret, temp_license_data[16], temp_license_data[17],
               temp_license_data[18]);
    if (!ret)
    {
        if ((temp_license_data[temp_size] == 0xFF) || (temp_license_data[temp_size] == 0))
        {
            // write default serial number to FTL
            need_save = true;
        }
        else
        {
            // update FTL serials number to default serials number
            memcpy((uint8_t *)&harman_serial_number[0], &temp_license_data[temp_size], HARMAN_SERIALS_NUM_LEN);
        }

        temp_size += HARMAN_SERIALS_NUM_LEN;
        if ((temp_license_data[temp_size] == 0xFF) || (temp_license_data[temp_size] == 0) ||
            (!memcmp((uint8_t *)&harman_product_id, &temp_license_data[temp_size], HARMAN_PRODUCT_ID_LEN)))
        {
            // write default PID to FTL
            // write new PID to FTL
            need_save = true;
        }

        temp_size += HARMAN_PRODUCT_ID_LEN;
        if ((temp_license_data[temp_size] == 0xFF) || (temp_license_data[temp_size] == 0x0))
        {
            // write default CID to FTL
            need_save = true;
        }
        else
        {
            // update FTL CID to default CID
            memcpy(&harman_color_id, &temp_license_data[temp_size], HARMAN_COLOR_ID_LEN);
        }
    }
    else if (ret == 36)
    {
        need_save = true;
    }

    if (need_save)
    {
        app_harman_license_save_to_ftl();
    }

    DBG_DIRECT("app_harman_license_load_from_ftl_after: harman_product_id: 0x%04x, harman_color_id: 0x%x, need_save: %d",
               harman_product_id, harman_color_id, need_save);
}

void app_harman_license_get(void)
{
    uint8_t cid_idx = 0;
    uint16_t product_id = 0;
    T_APP_HARMAN_GFPS_INFO gfps_info;
    bool need_save = false;

    /* get expect PID according to device name from McuConfig */
    harman_pid_idx = app_harman_license_pid_idx_get(app_cfg_const.device_name_legacy_default,
                                                    sizeof(app_cfg_const.device_name_legacy_default), &product_id);
    /* PID in flash is not same with expect PID */
    if (harman_product_id != product_id)
    {
        harman_product_id = product_id;
    }

    app_harman_license_load_from_ftl();

    if (!app_harman_license_gfps_info_get(harman_pid_idx, harman_color_id, &gfps_info))
    {
        harman_color_id = gfps_info.color_id;
    }

    app_harman_license_gfps_info_dump();
    if ((extend_app_cfg_const.gfps_model_id[0] == 0x00) &&
        (extend_app_cfg_const.gfps_public_key[0] == 0x00) &&
        (extend_app_cfg_const.gfps_private_key[0] == 0x00))
    {
        memcpy(extend_app_cfg_const.gfps_model_id, gfps_info.model_id, HARMAN_MODEL_ID_LEN);
        memcpy(extend_app_cfg_const.gfps_public_key, gfps_info.public_key, HARMAN_PUBLIC_KEY_LEN);
        memcpy(extend_app_cfg_const.gfps_private_key, gfps_info.private_key, HARMAN_PRIVATE_KEY_LEN);
    }
    app_harman_license_gfps_info_dump();
}

uint16_t app_harman_license_pid_get(void)
{
    return harman_product_id;
}

uint8_t app_harman_license_cid_get(void)
{
    return harman_color_id;
}

uint8_t *app_harman_license_serials_num_get(void)
{
    return (uint8_t *)&harman_serial_number;
}

bool app_harman_get_flag_need_clear_total_time(void)
{
    return need_clear_total_time;
}

#if HARMAN_SPP_CMD_SUPPORT
void app_harman_spp_cmd_set_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                   uint8_t app_idx)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    uint8_t sub_cmd_id = cmd_ptr[2];
    uint16_t payload_len = cmd_len - 3;
    uint8_t pid_idx = 0;
    uint8_t cid_idx = 0;
    uint8_t *p_rsp_cmd = NULL;
    uint8_t rsp_cmd_len = 0;

    APP_PRINT_INFO5("app_harman_spp_cmd_set_handle: cmd_id 0x%04x, sub_cmd_id: 0x%x, payload_len: 0x%x, app_idx: %d, data(%b)",
                    cmd_id, sub_cmd_id, payload_len, app_idx, TRACE_BINARY(payload_len, &cmd_ptr[3]));

    if ((cmd_path != CMD_PATH_SPP) || (cmd_id != CMD_VENDOR_SEPC))
    {
        return;
    }

    switch (sub_cmd_id)
    {
    case HARMAN_SPP_SUB_CMD_CID_SET:
        {
            T_APP_HARMAN_GFPS_INFO gfps_info;
            uint8_t color_id = cmd_ptr[3];

            rsp_cmd_len = 2 ;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id

            if ((harman_color_id != color_id) &&
                (app_harman_license_gfps_info_get(harman_pid_idx, color_id, &gfps_info))) // color_id is valid
            {
                p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;

                harman_color_id = color_id;
                app_harman_license_save_to_ftl();

                memcpy(extend_app_cfg_const.gfps_model_id, gfps_info.model_id, HARMAN_MODEL_ID_LEN);
                memcpy(extend_app_cfg_const.gfps_public_key, gfps_info.public_key, HARMAN_PUBLIC_KEY_LEN);
                memcpy(extend_app_cfg_const.gfps_private_key, gfps_info.private_key, HARMAN_PRIVATE_KEY_LEN);

                app_harman_license_gfps_info_dump();
            }
            else
            {
                p_rsp_cmd[1] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
        }
        break;

    case HARMAN_SPP_SUB_CMD_SERIALS_NUM_SET:
        {
            rsp_cmd_len = 2 ;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id

            if (payload_len >= HARMAN_SERIALS_NUM_LEN)
            {
                p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;

                memcpy((uint8_t *)&harman_serial_number[0], &cmd_ptr[3], HARMAN_SERIALS_NUM_LEN);
                app_harman_license_save_to_ftl();
            }
            else
            {
                p_rsp_cmd[1] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
        }
        break;

    case HARMAN_SPP_SUB_CMD_PID_GET:
        {
            rsp_cmd_len = 2 + HARMAN_PRODUCT_ID_LEN;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            memcpy(&p_rsp_cmd[2], (uint8_t *)&harman_product_id, HARMAN_PRODUCT_ID_LEN);
        }
        break;

    case HARMAN_SPP_SUB_CMD_CID_GET:
        {
            rsp_cmd_len = 2 + HARMAN_COLOR_ID_LEN;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            p_rsp_cmd[2] = harman_color_id;
        }
        break;

    case HARMAN_SPP_SUB_CMD_SERIALS_NUM_GET:
        {
            rsp_cmd_len = 2 + HARMAN_SERIALS_NUM_LEN;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            memcpy(&p_rsp_cmd[2], (uint8_t *)&harman_serial_number[0], HARMAN_SERIALS_NUM_LEN);
        }
        break;

    case HARMAN_SPP_SUB_CMD_TOTAL_TIME_GET:
        {
            if (!need_clear_total_time)
            {
                app_harman_total_power_on_time_update();
                app_harman_total_playback_time_update();
            }

            rsp_cmd_len = 2 + sizeof(app_cfg_nv.total_power_on_time) + sizeof(app_cfg_nv.total_playback_time);
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            memcpy(&p_rsp_cmd[2], (uint8_t *)&app_cfg_nv.total_power_on_time,
                   sizeof(app_cfg_nv.total_power_on_time));
            memcpy(&p_rsp_cmd[6], (uint8_t *)&app_cfg_nv.total_playback_time,
                   sizeof(app_cfg_nv.total_playback_time));
        }
        break;

    case HARMAN_SPP_SUB_CMD_TOTAL_TIME_CLEAR:
        {
            uint32_t ret = 0;

            need_clear_total_time = true;

            app_db.power_on_start_time = 0;
            app_db.playback_start_time = 0;
            app_cfg_nv.total_playback_time = 0;
            app_cfg_nv.total_power_on_time = 0;
            ret = app_cfg_store(&app_cfg_nv.total_playback_time, 8);

            rsp_cmd_len = 1 + sizeof(ret);
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            memcpy(&p_rsp_cmd[1], (uint8_t *)&ret, sizeof(ret));

            app_cfg_nv.need_set_lps_mode = 1;
            app_cfg_nv.ota_parking_lps_mode = OTA_TOOLING_SHIPPING_BOTH;

            power_mode_set(POWER_POWEROFF_MODE);
            app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
        }
        break;

    case HARMAN_SPP_SUB_CMD_CHIP_ID_GET:
        {
            uint8_t *uuid = sys_hall_get_ic_euid();

            rsp_cmd_len = 2 + 14;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            memcpy(&p_rsp_cmd[2], uuid, 14);
        }
        break;

    case HARMAN_SPP_SUB_CMD_ALGO_STATUS_GET:
        {
            rsp_cmd_len = 2 + sizeof(app_cfg_nv.algo_status);
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_COMPLETE;
            memcpy(&p_rsp_cmd[2], (uint8_t *)&app_cfg_nv.algo_status,
                   sizeof(app_cfg_nv.algo_status));
        }
        break;

    default:
        {
            rsp_cmd_len = 2;
            p_rsp_cmd = malloc(rsp_cmd_len);
            p_rsp_cmd[0] = cmd_ptr[2]; // sub_cmd_id
            p_rsp_cmd[1] = CMD_SET_STATUS_UNKNOW_CMD;
        }
        break;
    }

    if (app_idx < MAX_BR_LINK_NUM)
    {
        app_report_event(cmd_path, EVENT_VENDOR_SEPC, app_idx, p_rsp_cmd, rsp_cmd_len);
    }
}
#endif

#if HARMAN_DSP_LICENSE_SUPPORT
#define HARMAN_DEVICE_NAME_LEN     40
#define HARMAN_BD_ADDR_LEN         6
#define HARMAN_DSP_LICENSE_LEN     32
#define DSP_H2D_HEADER_LEN          4

static void app_harman_send_device_name_to_dsp(void)
{
    uint8_t *p_cmd_buf = NULL;
    uint16_t cmd_len = (HARMAN_DEVICE_NAME_LEN + DSP_H2D_HEADER_LEN + 3) / 4;

    p_cmd_buf = malloc(cmd_len * 4);
    if (p_cmd_buf != NULL)
    {
        p_cmd_buf[0] = 0x90; // H2D_BT_DEVICE_NAME
        p_cmd_buf[1] = 0x00;
        p_cmd_buf[2] = cmd_len;
        p_cmd_buf[3] = cmd_len >> 8;
        memcpy(&p_cmd_buf[4], app_cfg_const.device_name_legacy_default, HARMAN_DEVICE_NAME_LEN);
        APP_PRINT_INFO1("app_harman_send_device_name_to_dsp: %b",
                        TRACE_BINARY(HARMAN_DEVICE_NAME_LEN, app_cfg_const.device_name_legacy_default));
        audio_probe_dsp_send(p_cmd_buf, cmd_len * 4);

        free(p_cmd_buf);
    }

}

static void app_harman_send_bd_addr_to_dsp(void)
{
    uint8_t *p_cmd_buf = NULL;
    uint16_t cmd_len = (HARMAN_BD_ADDR_LEN + DSP_H2D_HEADER_LEN + 3) / 4;

    p_cmd_buf = malloc(cmd_len * 4);
    if (p_cmd_buf != NULL)
    {
        p_cmd_buf[0] = 0x2E; // H2D_SEND_BT_ADDR
        p_cmd_buf[1] = 0x00;
        p_cmd_buf[2] = cmd_len;
        p_cmd_buf[3] = cmd_len >> 8;
        memcpy(&p_cmd_buf[4], app_cfg_nv.bud_local_addr, HARMAN_BD_ADDR_LEN);
        APP_PRINT_INFO1("app_harman_send_bd_addr_to_dsp: %b",
                        TRACE_BINARY(HARMAN_BD_ADDR_LEN, app_cfg_nv.bud_local_addr));
        audio_probe_dsp_send(p_cmd_buf, cmd_len * 4);

        free(p_cmd_buf);
    }
}

static void app_harman_send_license_to_dsp(void)
{
    uint8_t *p_cmd_buf = NULL;
    uint16_t cmd_len = (HARMAN_DSP_LICENSE_LEN + DSP_H2D_HEADER_LEN + 3) / 4;
    uint8_t harman_dsp_license[HARMAN_DSP_LICENSE_LEN] = {0x3E, 0x5B, 0xFF, 0xFF, 0x3C, 0xFF, 0xFF, 0xFF,
                                                          0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x5B, 0xFF, 0xFF,
                                                          0x3C, 0xFF, 0xFF, 0xFF, 0x17, 0xFC, 0xFF, 0xFF,
                                                          0xF0, 0x4E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
                                                         };

    if ((*(uint8_t *)(0x02001180 + 256) != 0xFF) && (*(uint8_t *)(0x02001180 + 256) != 0x00))
    {
        memcpy(harman_dsp_license, (void *)(0x02001180 + 256), HARMAN_DSP_LICENSE_LEN);
    }

    p_cmd_buf = malloc(cmd_len * 4);
    if (p_cmd_buf != NULL)
    {
        p_cmd_buf[0] = 0x91; // H2D_USER_KEY_INFO
        p_cmd_buf[1] = 0x00;
        p_cmd_buf[2] = cmd_len;
        p_cmd_buf[3] = cmd_len >> 8;
        memcpy(&p_cmd_buf[4], harman_dsp_license, HARMAN_DSP_LICENSE_LEN);
        APP_PRINT_INFO1("app_harman_send_license_to_dsp: %b",
                        TRACE_BINARY(HARMAN_DSP_LICENSE_LEN, harman_dsp_license));
        audio_probe_dsp_send(p_cmd_buf, cmd_len * 4);

        free(p_cmd_buf);
    }
}

static void app_harman_audio_probe_dsp_event_cback(uint32_t event, void *msg)
{
    bool handle = true;

    switch (event)
    {
    case AUDIO_PROBE_DSP_EVT_REQ:
        {
            app_harman_send_device_name_to_dsp();
            app_harman_send_bd_addr_to_dsp();
            app_harman_send_license_to_dsp();
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
        APP_PRINT_INFO1("app_harman_audio_probe_dsp_event_cback: event_type 0x%04x", event);
    }

}

static void app_harman_audio_probe_dsp_cback(T_AUDIO_PROBE_EVENT event, void *buf)
{
    switch (event)
    {
    case PROBE_REPORT_ALGO_STATUS:
        {
            uint32_t status = (uint32_t)buf;
            app_cfg_nv.algo_status = status;
            APP_PRINT_INFO2("app_harman_audio_probe_dsp_cback: event_type 0x%04x, status %d", event, status);
        }
        break;

    default:
        break;
    }
}
#endif

void app_harman_license_init(void)
{
#if HARMAN_DSP_LICENSE_SUPPORT
    audio_probe_dsp_evt_cback_register(app_harman_audio_probe_dsp_event_cback);
    audio_probe_dsp_cback_register(app_harman_audio_probe_dsp_cback);
#endif
}
#endif
