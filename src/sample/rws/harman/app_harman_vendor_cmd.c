#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "fmc_api.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_harman_parser.h"
#include "app_harman_vendor_cmd.h"
#include "app_harman_ble_ota.h"
#include "app_harman_report.h"
#include "app_harman_license.h"
#include "app_harman_eq.h"
#include "app_harman_behaviors.h"
#include "app_harman_ble.h"
#include "app_cmd.h"
#include "app_bt_policy_api.h"
#include "os_mem.h"
#include "bt_bond.h"
#include "audio.h"
#include "app_bt_policy_int.h"
#include "app_multilink.h"
#include "app_bond.h"
#include "bt_a2dp.h"
#include "app_a2dp.h"
#include "app_mmi.h"
#include "app_ble_gap.h"
#include "app_cfg.h"
#include "app_ble_common_adv.h"
#include "transmit_service.h"
#include "app_sniff_mode.h"
#include "app_auto_power_off.h"
#include "ble_conn.h"
#include "app_types.h"
#include "app_hfp.h"
#include "sidetone.h"
#include "ftl.h"
#include "audio_probe.h"
#include "audio_volume.h"
#include "ringtone.h"
#include "voice_prompt.h"
#include "app_dsp_cfg.h"
#include "app_gfps_rfc.h"
#include "flash_map.h"
#include "app_sidetone.h"
#include "app_auto_power_off.h"
#if F_APP_LEA_SUPPORT
#include "app_lea_ascs.h"
#include "app_lea_ccp.h"
#include "app_lea_mcp.h"
#include "app_lea_unicast_audio.h"
#include "app_lea_vol_def.h"
#endif

#define JBL_APP_LEA_BUTTON      1

#if HARMAN_VP_DATA_HEADER_GET_SUPPORT
#include "storage.h"
#endif

#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
#include "app_dsp_cfg.h"
#endif

#define APP_RECONFIG_SET(bit, idx)              (bit |=  (idx + 1))
#define APP_RECONFIG_PUT(bit, idx)              (bit &= ~(idx + 1))

#define HARMAN_RECONN_PROFILE_TIME              1000
#define HARMAN_RECNN_PROFILE_PRTECT_TIME        4500
#define HARMAN_SMARTSWITCH_DISCONN_LE_CONN      500
#define HARMAN_UPDATE_TOTAL_TIME_INTERVAL       60000

#define HARMAN_AAC_BIT_RATE_256                 256
#define HARMAN_AAC_BIT_RATE_197                 197
#define HARMAN_SBC_MAX_BIT_POOL_53              53
#define HARMAN_SBC_MAX_BIT_POOL_46              46
#define HARMAN_LATENCY_150_MS                   150
#define HARMAN_LATENCY_120_MS                   120
#define HARMAN_SECOND_LATENCY_230_MS            230

#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
#define HARMAN_FIND_MY_BUDS_RING_TIME_INTERVAL          1500
#define HARMAN_FIND_MY_BUDS_RING_VOLUME_STATGE          3
#define HARMAN_FIND_MY_BUDS_RING_VOLUME_SWITCH_TIMES    5
#define HARMAN_FIND_MY_BUDS_RING_MAX_VOLUME             15

static uint16_t  find_my_ring_cnt = 0;

static uint8_t timer_handle_find_my_buds_ring = 0;
#endif

static uint8_t vendor_harman_cmd_timer_queue_id = 0;
static uint8_t timer_handle_recon_a2dp_profile = 0;
static uint8_t timer_handle_multi_recon_a2dp_profile = 0;
static uint8_t timer_handle_recon_profile_protect = 0;
static uint8_t timer_handle_multi_recon_profile_protect = 0;
static uint8_t timer_handle_smartswitch_disc_le_conn = 0;
static uint8_t timer_handle_update_total_time = 0;

typedef enum
{
    APP_OTA_PACKAGE_CHECK               = 0x00,
    APP_RECON_A2DP_PROFILE              = 0x01,
    APP_MULTI_RECON_A2DP_PROFILE        = 0x02,
    APP_RECON_PROFILE_PROTECT           = 0x03,
    APP_MULTI_RECON_PROFILE_PROTECT     = 0x04,
    APP_SMARTSWITCH_DISC_LE_CONN        = 0x05,
    APP_TIMER_FIND_MY_BUDS_RING         = 0x06,
    APP_TIMER_UPDATE_TOTAL_TIME         = 0x07,

} T_CMD_VENDOR_HARMAN_TIMER;

typedef enum
{
    DEVICE_IS_IDLE      = 0x00,
    DEVICE_IS_CALLING   = 0x80,
} T_HARMAN_DEVICE_SCO_STATUS;

uint8_t is_a2dp_disconnect_by_reconfigure = false;
uint8_t firmware_version[3] = {0x01, 0x00, 0x00};
uint8_t detail_battery_info[2] = {0xff, 0xff};
uint8_t current_vol[4] = {0xff, 0xff, 0xff, 0xff};
uint8_t anc_status = 0xff;
uint8_t auto_play_pause_enable_status = 0xff;
uint8_t tws_connectiong_status = 0xff;
uint8_t perconi_fi = 0xff;
uint8_t in_ear_status[2] = {0xff, 0xff};
uint8_t sealing_test_status[2] = {0xff, 0xff};
uint8_t bt_connection_status = 0x00;
uint16_t EQ_total_len = 888;
uint16_t EQ_save_combine_record = 0;
uint8_t *eq_p_cmd_data = NULL;

extern uint8_t timer_idx_hfp_ring;
extern void *timer_handle_harman_power_off_option;

#if HARMAN_VP_DATA_HEADER_GET_SUPPORT
#define  VP_LANGUAGE_OFFSET             0x400
#define  VP_LANGUAGE_NUM_IN_VP_DATA     4

typedef enum _t_app_harman_vp_language_id_
{
    HARMAN_VP_LANGUAGE_NONE         = 0x00,

    HARMAN_VP_LANGUAGE_ENGLISH      = 0x01,
    HARMAN_VP_LANGUAGE_DUTCH        = 0x02,
    HARMAN_VP_LANGUAGE_FRENCH       = 0x03,
    HARMAN_VP_LANGUAGE_GERMAN       = 0x04,
    HARMAN_VP_LANGUAGE_ITALIAN      = 0x05,
    HARMAN_VP_LANGUAGE_JAPANESE     = 0x06,
    HARMAN_VP_LANGUAGE_KOREAN       = 0x07,
    HARMAN_VP_LANGUAGE_PORTUGUESE   = 0x08,
    HARMAN_VP_LANGUAGE_RUSSIAN      = 0x09,
    HARMAN_VP_LANGUAGE_CHINESE      = 0x0A,
    HARMAN_VP_LANGUAGE_SPANISH      = 0x0B,

    HARMAN_VP_LANGUAGE_MAX          = 0x0C,
} T_APP_HARMAN_VP_LANGUAGE_ID;

typedef struct _t_app_harman_vp_data_header_
{
    uint16_t  sync_word;
    uint8_t   vp_total_num;
    uint8_t   vp_default_language_id;
    T_APP_HARMAN_VP_LANGUAGE_ID language_id[VP_LANGUAGE_NUM_IN_VP_DATA];
    uint8_t   ota_version[4];
    uint8_t   rsved[4];
} __attribute__((packed)) T_APP_HARMAN_VP_DATA_HEADER;

void app_harman_vp_data_header_get(void)
{
    uint8_t table_index = 0;
    T_APP_HARMAN_VP_DATA_HEADER vp_data_header = {0};
    uint32_t vp_addr = 0;
    const T_STORAGE_PARTITION_INFO *info = storage_partition_get(VP_PARTITION_NAME);

    vp_addr = info->address;

    fmc_flash_nor_read(vp_addr + VP_LANGUAGE_OFFSET, &vp_data_header,
                       sizeof(T_APP_HARMAN_VP_DATA_HEADER));

    if (vp_data_header.sync_word != 0xAA55)
    {
        APP_PRINT_ERROR0("app_harman_vp_data_header_get: vp data read fail!!");
        return;
    }

    // update vp ota version
    app_cfg_nv.language_version[0] = vp_data_header.ota_version[2];
    app_cfg_nv.language_version[1] = vp_data_header.ota_version[1];
    app_cfg_nv.language_version[2] = vp_data_header.ota_version[0];

    if ((vp_data_header.vp_default_language_id == HARMAN_VP_LANGUAGE_NONE) ||
        (vp_data_header.vp_default_language_id >= HARMAN_VP_LANGUAGE_MAX))
    {
        app_cfg_const.voice_prompt_language = vp_data_header.language_id[0];
        app_cfg_nv.voice_prompt_language = 0;
    }
    else
    {
        app_cfg_const.voice_prompt_language = vp_data_header.vp_default_language_id;

        for (table_index = 0; table_index < VP_LANGUAGE_NUM_IN_VP_DATA; table_index ++)
        {
            APP_PRINT_TRACE1("app_harman_vp_data_header_get: 0x%x",
                             vp_data_header.language_id[table_index]);
            if (vp_data_header.language_id[table_index] == vp_data_header.vp_default_language_id)
            {
                app_cfg_nv.voice_prompt_language = table_index;
                break;
            }
        }
    }
    app_cfg_store(&app_cfg_nv.remote_loc, 20);
    APP_PRINT_TRACE6("app_harman_vp_data_header_get: vp_data_header(%b), "
                     "voice_prompt_language_const: 0x%x, table_index: 0x%x, "
                     "voice_prompt_language_nv: 0x%x, "
                     "vp_version(%b), store_vp_version(%d)",
                     TRACE_BINARY(sizeof(T_APP_HARMAN_VP_DATA_HEADER), &vp_data_header),
                     app_cfg_const.voice_prompt_language,
                     table_index, app_cfg_nv.voice_prompt_language,
                     TRACE_BINARY(sizeof(vp_data_header.ota_version), vp_data_header.ota_version),
                     TRACE_BINARY(sizeof(app_cfg_nv.language_version), app_cfg_nv.language_version));
}
#endif

void app_harman_set_lea_mode(bool enable)
{
    if (app_cfg_nv.lea_mode_enable != enable)
    {
        app_cfg_nv.lea_mode_enable = enable;
        app_cfg_store(&app_cfg_nv.lea_mode_enable, 1);
        app_bond_clear_non_rws_keys();
        app_device_reboot(100, RESET_ALL);
    }
    APP_PRINT_TRACE1("app_harman_set_lea_mode: enable: %d", enable);
}

bool app_harman_get_lea_mode(void)
{
    return app_cfg_nv.lea_mode_enable;
}

void app_harman_cfg_reset(void)
{
    app_harman_is_record_a2dp_active_ever_set(false);
    app_harman_is_already_connect_one_set(false);

    app_cfg_nv.auto_power_off_status = 0;
    app_cfg_nv.auto_power_off_time = HARMAN_AUTO_POWER_OFF_DEFAULT_TIME;

    app_cfg_nv.vp_ota_status = false;

    /* clear breakpoint offset */
    app_cfg_nv.harman_breakpoint_offset = 0x00;
    app_cfg_nv.cur_sub_image_index = 0x00;
    app_cfg_nv.cur_sub_image_relative_offset = 0x00;
    app_cfg_nv.harman_record_ota_version = 0x00;

#if HARMAN_OTA_VERSION_CHECK
    uint8_t null[3] = {0x01, 0x00, 0x00};
#else
    uint8_t null[3] = {0x00, 0x00, 0x00};
#endif

    memcpy(&app_cfg_nv.language_version[0], &null[0], 3);
    app_cfg_nv.language_status = 1;

    app_cfg_nv.sidetone_switch = app_dsp_cfg_sidetone.hw_enable;
    app_cfg_nv.sidetone_level = SIDETOME_LEVEL_M;
    APP_PRINT_TRACE2("app_harman_cfg_reset: sidetone_switch: %d, sidetone_level: %d",
                     app_cfg_nv.sidetone_switch, app_cfg_nv.sidetone_level);

    app_cfg_nv.cmd_aac_bit_rate = HARMAN_AAC_BIT_RATE_256;
    app_cfg_nv.cmd_sbc_max_bitpool = HARMAN_SBC_MAX_BIT_POOL_46;//HARMAN_SBC_MAX_BIT_POOL_53;
    app_cfg_nv.cmd_latency_in_ms = HARMAN_LATENCY_150_MS;
    app_cfg_nv.cmd_second_latency_in_ms = HARMAN_SECOND_LATENCY_230_MS;

    app_cfg_nv.harman_category_id = CATEGORY_ID_OFF; // default_eq =0, customer=1
    app_cfg_nv.harman_default_EQ_stage_num = 0x00;
    app_cfg_nv.harman_customer_stage = 0x00;
    app_cfg_nv.harman_EQmaxdB = 0x00;

    app_cfg_nv.harman_max_volume_limit = 0x00;
#if HARMAN_OPEN_LR_FEATURE
    app_cfg_nv.harman_LR_balance = HARMAN_LR_BALANCE_MID_VOL_LEVEL; // default off
#endif
    app_cfg_nv.lea_mode_enable = 0;
}

uint8_t harman_get_active_mobile_cmd_link(uint8_t *idx)
{
    uint8_t i;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        APP_PRINT_TRACE5("harman_get_active_mobile_cmd_link: idx: %d, LE_statue: %d, used: %d, cmd_set_enable: %d, bd_addr: %b",
                         i,
                         app_db.le_link[i].state,
                         app_db.le_link[i].used,
                         app_db.le_link[i].cmd_set_enable,
                         TRACE_BDADDR(app_db.le_link[i].bd_addr));
        if ((app_db.le_link[i].state == LE_LINK_STATE_CONNECTED) &&
            (app_db.le_link[i].used == true) &&
            (app_db.le_link[i].cmd_set_enable == true))
        {
            *idx = i;
            return CMD_PATH_LE;
        }
    }
    return 0;
}

bool app_check_cmd_avail(void)
{
    if (timer_handle_multi_recon_profile_protect != NULL ||
        timer_handle_recon_profile_protect != NULL)
    {
        APP_PRINT_TRACE0("RECON_PROTECT exist");
        return true;
    }
    APP_PRINT_TRACE0("RECON_PROTECT avail");
    return false;
}

bool check_recon_profile(uint8_t idx)
{
    uint8_t ret = false;
    APP_PRINT_TRACE2("check_profile_protect id = %d, profile = %x", idx,
                     app_db.br_link[idx].connected_profile);
    if ((app_db.br_link[idx].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) == 0)
    {
        app_bt_policy_default_connect(app_db.br_link[idx].bd_addr, (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK),
                                      false);
        ret = true;
    }
    else
    {
        if ((app_db.br_link[idx].connected_profile & A2DP_PROFILE_MASK) == 0)
        {
            app_bt_policy_default_connect(app_db.br_link[idx].bd_addr, A2DP_PROFILE_MASK, false);
            ret = true;
        }
        else if ((app_db.br_link[idx].connected_profile & AVRCP_PROFILE_MASK) == 0)
        {
            app_bt_policy_default_connect(app_db.br_link[idx].bd_addr, AVRCP_PROFILE_MASK, false);
            ret = true;
        }
    }
    return ret;
}

void harman_notify_smartswitch_reconn(void)
{
    if (app_check_cmd_avail() == false)
    {
        uint8_t payloadlen = 0;
        uint8_t cmd_path = CMD_PATH_LE;
        uint8_t mobile_app_idx = MAX_BR_LINK_NUM;
        harman_get_active_mobile_cmd_link(&mobile_app_idx);

        payloadlen = 8;
        uint8_t *smartswitch_reconn_rsp = os_mem_alloc(RAM_TYPE_DATA_ON, payloadlen);
        if (smartswitch_reconn_rsp != NULL)
        {
            smartswitch_reconn_rsp[0] = (uint8_t)(app_cfg_nv.cmd_aac_bit_rate);
            smartswitch_reconn_rsp[1] = (uint8_t)(app_cfg_nv.cmd_aac_bit_rate >> 8);
            smartswitch_reconn_rsp[2] = (uint8_t)(app_cfg_nv.cmd_sbc_max_bitpool);
            smartswitch_reconn_rsp[3] = (uint8_t)(app_cfg_nv.cmd_sbc_max_bitpool >> 8);
            smartswitch_reconn_rsp[4] = (uint8_t)(app_cfg_nv.cmd_latency_in_ms);
            smartswitch_reconn_rsp[5] = (uint8_t)(app_cfg_nv.cmd_latency_in_ms >> 8);
            smartswitch_reconn_rsp[6] = 0xff;
            smartswitch_reconn_rsp[7] = 0xff;
            if (mobile_app_idx < MAX_BR_LINK_NUM)
            {
                app_harman_report_le_event(&app_db.le_link[mobile_app_idx], CMD_HARMAN_DEVICE_INFO_SET,
                                           smartswitch_reconn_rsp, payloadlen);
            }
            os_mem_free(smartswitch_reconn_rsp);
            smartswitch_reconn_rsp = NULL;
        }
    }
}

void app_harman_lr_balance_set(T_AUDIO_STREAM_TYPE volume_type, uint8_t vol,
                               const char *func_name, const uint32_t line_no)
{
    APP_PRINT_INFO2("app_harman_lr_balance_set %s, %d", TRACE_STRING(func_name), line_no);

    int16_t left_balance_gain = 0;
    int16_t right_balance_gain = 0;
    int8_t volume_balance = HARMAN_LR_BALANCE_MID_VOL_LEVEL - app_cfg_nv.LR_vol_level;

    left_balance_gain = (volume_balance * 7);//(volume_balance * 0.7 * 10);
    right_balance_gain = (volume_balance * (-7));//(volume_balance * (-0.7)  * 10);
    APP_PRINT_INFO6("app_harman_lr_balance_set: volume_type: %d, vol: %d, volume_balance: %d, playback_volume_max: %d, "
                    "left_balance_gain_db: %d/10, right_balance_gain_db: %d/10",
                    volume_type, vol, volume_balance, app_dsp_cfg_vol.playback_volume_max,
                    left_balance_gain, right_balance_gain);

    if (volume_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        int32_t current_dac_dig_gain_db = app_dsp_cfg_data->audio_dac_gain_table[vol];
        int16_t ori_db = ((current_dac_dig_gain_db) / 128 * 10);
        int16_t temp_left_db = (ori_db + left_balance_gain);
        int16_t temp_right_db = (ori_db + right_balance_gain);
        int16_t level_11_compare = ((app_dsp_cfg_data->audio_dac_gain_table[11]) / 128 * 10);
        int16_t level_15_compare = ((app_dsp_cfg_data->audio_dac_gain_table[15]) / 128 * 10);
        APP_PRINT_INFO6("PLAYBACK: current_dac_dig_gain_db %d, ori_db: %d/10, temp_left_db: %d/10, temp_right_db: %d/10, "
                        "level_11_compare_db: %d/10, level_15_compare_db: %d/10",
                        current_dac_dig_gain_db / 128, ori_db, temp_left_db, temp_right_db,
                        level_11_compare, level_15_compare);

        left_balance_gain = ((vol == app_dsp_cfg_vol.playback_volume_max) &&
                             (left_balance_gain > 0)) ? 0 : left_balance_gain;
        right_balance_gain = ((vol == app_dsp_cfg_vol.playback_volume_max) &&
                              (right_balance_gain > 0)) ? 0 :  right_balance_gain;
        APP_PRINT_INFO4("PLAYBACK: left_balance_gain: %d, right_balance_gain: %d, "
                        "max_volume_limit_status: %d, max_volume_limit_level: %d",
                        left_balance_gain, right_balance_gain,
                        app_cfg_nv.max_volume_limit_status, app_cfg_nv.max_volume_limit_level);
        if (app_cfg_nv.max_volume_limit_status)
        {
            //refine
            if (vol >= app_cfg_nv.max_volume_limit_level)
            {
                current_dac_dig_gain_db =
                    app_dsp_cfg_data->audio_dac_gain_table[app_cfg_nv.max_volume_limit_level];
                ori_db = ((current_dac_dig_gain_db) / 128 * 10);
                temp_left_db = (ori_db + left_balance_gain);
                temp_right_db = (ori_db + right_balance_gain);
                APP_PRINT_INFO4("PLAYBACK: current_dac_dig_gain_db: %d, ori_db: %d/10, temp_left_db: %d/10, temp_right_db: %d/10, ",
                                current_dac_dig_gain_db / 128, ori_db, temp_left_db, temp_right_db);

            }

            if ((temp_left_db >= level_11_compare) && (ori_db <= level_11_compare))
            {
                left_balance_gain = (level_11_compare - ori_db);
            }

            if ((temp_right_db >= level_11_compare) && (ori_db <= level_11_compare))
            {
                right_balance_gain = (level_11_compare - ori_db);
            }
            APP_PRINT_INFO2("PLAYBACK: left_balance_gain: %d/10, right_balance_gain: %d/10",
                            left_balance_gain, right_balance_gain);
        }
    }

#if HARMAN_OPEN_LR_FEATURE
    {
        float left_gain_balance_to_fwk = 0.0;
        float right_gain_balance_to_fwk = 0.0;
        left_gain_balance_to_fwk = (app_cfg_nv.LR_option) ? ((float)(left_balance_gain) / 10) : 0;
        right_gain_balance_to_fwk = (app_cfg_nv.LR_option) ? ((float)(right_balance_gain) / 10) : 0;
        audio_volume_balance_set(volume_type, left_gain_balance_to_fwk, right_gain_balance_to_fwk);
    }
#endif
}

void harman_set_vp_ringtone_balance(const char *func_name, const uint32_t line_no)
{
#if HARMAN_VP_LR_BALANCE_SUPPORT
    APP_PRINT_INFO2("harman_set_vp_ringtone_balance: func_name: %s, line: %d", TRACE_STRING(func_name),
                    line_no);
    int8_t volume_balance = HARMAN_LR_BALANCE_MID_VOL_LEVEL - app_cfg_nv.LR_vol_level;
    int16_t vp_ringtone_left_gain = (volume_balance * 7);//(volume_balance * 0.7 * 10);
    int16_t vp_ringtone_right_gain = (volume_balance * (-7));//(volume_balance * (-0.7)  * 10);
    APP_PRINT_INFO2("vp_ringtone_left_gain_db: %d/10, vp_ringtone_right_gain_db: %d/10",
                    vp_ringtone_left_gain, vp_ringtone_right_gain);

#if HARMAN_OPEN_LR_FEATURE
    {
        float vp_ringtone_left_gain_to_fwk = (app_cfg_nv.LR_option) ? ((float)(
                                                                           vp_ringtone_left_gain) / 10) : 0;
        float vp_ringtone_right_gain_to_fwk = (app_cfg_nv.LR_option) ? ((float)(
                                                                            vp_ringtone_right_gain) / 10) : 0;

        ringtone_volume_balance_set(vp_ringtone_left_gain_to_fwk, vp_ringtone_right_gain_to_fwk);
        voice_prompt_volume_balance_set(vp_ringtone_left_gain_to_fwk, vp_ringtone_right_gain_to_fwk);
    }
#endif
#endif
}

static void app_harman_devinfo_feature_report(uint16_t feature_id, uint16_t value_size,
                                              uint8_t *p_value,
                                              uint16_t app_idx)
{
    APP_PRINT_INFO4("app_harman_devinfo_feature_report: app_idx: %d, feature_id 0x%04x, value_size: 0x%04x, data(%b)",
                    app_idx, feature_id, value_size, TRACE_BINARY(value_size, p_value));
    uint8_t payloadlen = 0;
    uint8_t *p_cmd_rsp = NULL;

    payloadlen = 6 + value_size;
    p_cmd_rsp = malloc(payloadlen);
    if (p_cmd_rsp != NULL)
    {
        p_cmd_rsp[0] = 0;
        p_cmd_rsp[1] = 0;
        p_cmd_rsp[2] = (uint8_t)(feature_id);
        p_cmd_rsp[3] = (uint8_t)(feature_id >> 8);
        p_cmd_rsp[4] = (uint8_t)(value_size);
        p_cmd_rsp[5] = (uint8_t)(value_size >> 8);
        memcpy(&p_cmd_rsp[6], p_value, value_size);

        if (app_idx < MAX_BR_LINK_NUM)
        {
            app_harman_report_le_event(&app_db.le_link[app_idx], CMD_HARMAN_DEVICE_INFO_DEVICE_NOTIFY,
                                       p_cmd_rsp, payloadlen);
        }
        free(p_cmd_rsp);
        p_cmd_rsp = NULL;
    }
}

static uint8_t app_harman_get_battery_status(void)
{
    uint8_t battery_status = 0;

    if (app_db.local_charger_state == APP_CHARGER_STATE_CHARGING)
    {
        battery_status = (0x80 | app_db.local_batt_level);
    }
    else
    {
        battery_status = (0x7F & app_db.local_batt_level);
    }

    return battery_status;
}

void app_harman_battery_status_notify(void)
{
    uint8_t battery_status = 0;
    uint8_t mobile_app_idx = MAX_BR_LINK_NUM;

    harman_get_active_mobile_cmd_link(&mobile_app_idx);

    battery_status = app_harman_get_battery_status();

    app_harman_devinfo_feature_report(FEATURE_LEFT_DEVICE_BATT_STATUS, 1, &battery_status,
                                      mobile_app_idx);
}

/*
    ble_paired_status: 0x00 (not paired), 0x01 (paired)
*/
void app_harman_ble_paired_status_notify(uint8_t ble_paired_status, uint8_t app_idx)
{
    app_harman_devinfo_feature_report(FEATURE_PAIRED_STATUS, 1, &ble_paired_status,
                                      app_idx);
}

void app_harman_sco_status_notify(void)
{
    if (app_link_get_b2s_link_num())
    {
        uint8_t call_status = DEVICE_IS_IDLE;
        uint8_t mobile_app_idx = MAX_BR_LINK_NUM;

        harman_get_active_mobile_cmd_link(&mobile_app_idx);
        if ((app_hfp_sco_is_connected()) || (timer_idx_hfp_ring != NULL))
        {
            call_status = DEVICE_IS_CALLING;
        }
        else
        {
            call_status = DEVICE_IS_IDLE;
        }

        app_harman_devinfo_feature_report(FEATURE_CALL_STATUS, sizeof(call_status), &call_status,
                                          mobile_app_idx);
    }
}

void app_harman_ble_heartbeat_notify(void)
{
    uint8_t ble_conn_id = app_ble_common_adv_get_conn_id();

    app_harman_devinfo_feature_report(FEATURE_HEARTBEAT, 0, 0, ble_conn_id);

    app_harman_heartbeat_check_timer_start(HARMAN_HEARTBEAT_CHECK_TIME);
}

void harman_sidetone_set_dsp(void)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr);
    if ((p_link->sidetone_instance == NULL) && (app_cfg_nv.sidetone_switch == SIDETONE_SWTCH_ENABLE))
    {
        p_link->sidetone_instance = sidetone_create(SIDETONE_TYPE_HW,
                                                    app_dsp_cfg_sidetone.gain,
                                                    app_dsp_cfg_sidetone.hpf_level);

        if (p_link->sidetone_instance != NULL)
        {
            sidetone_enable(p_link->sidetone_instance);
            audio_track_effect_attach(p_link->sco_track_handle, p_link->sidetone_instance);
        }
    }

    if (app_cfg_nv.sidetone_switch == SIDETONE_SWTCH_ENABLE)
    {
        if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_L)
        {
            //L -18db
            sidetone_gain_set(p_link->sidetone_instance, (-18 * 128));
        }
        else if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_M)
        {
            //M -15.375db
            sidetone_gain_set(p_link->sidetone_instance, (-15.375 * 128));
        }
        else if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_H)
        {
            //H -13.125db
            sidetone_gain_set(p_link->sidetone_instance, (-13.125 * 128));
        }
    }

    APP_PRINT_INFO2("harman_sidetone_set_dsp %d,%d", app_cfg_nv.sidetone_switch,
                    app_cfg_nv.sidetone_level);
}

void app_harman_devinfo_notify(uint8_t le_idx)
{
    uint8_t payloadlen = 0;
    uint8_t *retdevinfo_rsp = NULL;
    uint8_t len_temp = 0;
    uint8_t local_bt_addr[6] = {0};
    uint8_t mobile_app_idx = MAX_BR_LINK_NUM;
    uint16_t product_id = app_harman_license_pid_get();
    uint8_t color_id = app_harman_license_cid_get();
    uint8_t battery_status = app_harman_get_battery_status();

    payloadlen = 2 + 4 * 8 + sizeof(app_cfg_nv.device_name_legacy) + sizeof(product_id) +
                 sizeof(color_id) + BD_ADDR_SIZE + sizeof(firmware_version) +
                 sizeof(battery_status) + sizeof(current_vol) + sizeof(tws_connectiong_status);
    retdevinfo_rsp = malloc(payloadlen);

    if (retdevinfo_rsp != NULL)
    {
        T_IMG_HEADER_FORMAT *addr;

        addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_MCUCONFIG);
        firmware_version[0] = (uint8_t)(addr->git_ver.version >> 16);
        firmware_version[1] = (uint8_t)(addr->git_ver.version >> 8);
        firmware_version[2] = (uint8_t)addr->git_ver.version;

        gap_get_param(GAP_PARAM_BD_ADDR, local_bt_addr);

        retdevinfo_rsp[0] = 0;
        retdevinfo_rsp[1] = 0;
        len_temp += 2;
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_DEVICE_NAME,
                                        sizeof(app_cfg_nv.device_name_legacy), (uint8_t *)&app_cfg_nv.device_name_legacy);
        len_temp += (4 + sizeof(app_cfg_nv.device_name_legacy));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_PID, sizeof(product_id),
                                        (uint8_t *)&product_id);
        len_temp += (4 + sizeof(product_id));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_COLOR_ID, sizeof(color_id),
                                        &color_id);
        len_temp += (4 + sizeof(color_id));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_MAC_ADDR, BD_ADDR_SIZE,
                                        local_bt_addr);
        len_temp += (4 + BD_ADDR_SIZE);
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_FIEMWARE_VERSION,
                                        sizeof(firmware_version), &firmware_version[0]);
        len_temp += (4 + sizeof(firmware_version));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_LEFT_DEVICE_BATT_STATUS,
                                        sizeof(battery_status), &battery_status);
        len_temp += (4 + sizeof(battery_status));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_PLAYER_VOLUME,
                                        sizeof(current_vol), &current_vol[0]);
        len_temp += (4 + sizeof(current_vol));
        app_harman_devinfo_feature_pack(retdevinfo_rsp + len_temp, FEATURE_TWS_INFO,
                                        sizeof(tws_connectiong_status), (uint8_t *)&tws_connectiong_status);
        len_temp += (4 + sizeof(tws_connectiong_status));

        APP_PRINT_INFO2("app_harman_devinfo_notify: payloadlen: 0x%02X, product_id: 0x%04x",
                        payloadlen, product_id);

        mobile_app_idx = app_ble_common_adv_get_conn_id();

        app_harman_devinfo_notification_report(retdevinfo_rsp, payloadlen, mobile_app_idx);

        os_mem_free(retdevinfo_rsp);
        retdevinfo_rsp = NULL;
    }
}

#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
static void app_harman_find_my_buds_ring_play(void)
{
    find_my_ring_cnt ++;
    if (find_my_ring_cnt == 1)
    {
        audio_volume_out_mute(AUDIO_STREAM_TYPE_PLAYBACK);
    }

    if (find_my_ring_cnt <= HARMAN_FIND_MY_BUDS_RING_VOLUME_SWITCH_TIMES)
    {
        voice_prompt_volume_set(HARMAN_FIND_MY_BUDS_RING_VOLUME_STATGE * find_my_ring_cnt);
    }
    else
    {
        voice_prompt_volume_set(HARMAN_FIND_MY_BUDS_RING_MAX_VOLUME);
    }

    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_play(TONE_HARMAN_GFPS_FIND_ME, false, false);
    }
    else
    {
        app_audio_tone_type_play(TONE_GFPS_FINDME, false, false);
    }

    app_start_timer(&timer_handle_find_my_buds_ring, "find_my_buds_ring_timeout",
                    vendor_harman_cmd_timer_queue_id, APP_TIMER_FIND_MY_BUDS_RING,
                    0, false, HARMAN_FIND_MY_BUDS_RING_TIME_INTERVAL);
}

void app_harman_find_my_buds_ring_stop(void)
{
    audio_volume_out_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
    app_stop_timer(&timer_handle_find_my_buds_ring);

    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_cancel(TONE_HARMAN_GFPS_FIND_ME, false);
    }
    else
    {
        app_audio_tone_type_cancel(TONE_GFPS_FINDME, false);
    }

    find_my_ring_cnt = 0;
    voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_default);

    app_cfg_nv.find_my_buds_status = 0;
    app_cfg_store(&app_cfg_nv.vp_ota_status, 4);
}
#endif

static void vendor_harman_cmd_timer_callback(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("vendor_harman_cmd_timer_callback: timer id = 0x%02X, timer channel = %d",
                     timer_id, timer_chann);

    switch (timer_id)
    {
    case APP_RECON_A2DP_PROFILE:
        {
            app_stop_timer(&timer_handle_recon_a2dp_profile);
            if (check_recon_profile(timer_chann))
            {
                if (timer_handle_recon_profile_protect != NULL)
                {
                    app_start_timer(&timer_handle_recon_a2dp_profile, "recon_a2dp_profile",
                                    vendor_harman_cmd_timer_queue_id, APP_RECON_A2DP_PROFILE, timer_chann,
                                    false, HARMAN_RECONN_PROFILE_TIME);
                }
            }
            else
            {
                app_stop_timer(&timer_handle_recon_profile_protect);
            }
        }
        break;

    case APP_MULTI_RECON_A2DP_PROFILE:
        {
            app_stop_timer(&timer_handle_multi_recon_a2dp_profile);
            if (check_recon_profile(timer_chann))
            {
                if (timer_handle_multi_recon_profile_protect != NULL)
                {
                    app_start_timer(&timer_handle_multi_recon_a2dp_profile, "recon_multi_a2dp_profile",
                                    vendor_harman_cmd_timer_queue_id, APP_MULTI_RECON_A2DP_PROFILE, timer_chann,
                                    false, HARMAN_RECONN_PROFILE_TIME);
                }
            }
            else
            {
                app_stop_timer(&timer_handle_multi_recon_profile_protect);
            }
        }
        break;

    case APP_RECON_PROFILE_PROTECT:
        {
            app_stop_timer(&timer_handle_recon_profile_protect);
            check_recon_profile(timer_chann);
        }
        break;

    case APP_MULTI_RECON_PROFILE_PROTECT:
        {
            app_stop_timer(&timer_handle_multi_recon_profile_protect);
            check_recon_profile(timer_chann);
        }
        break;

    case APP_SMARTSWITCH_DISC_LE_CONN:
        {
            app_stop_timer(&timer_handle_smartswitch_disc_le_conn);
            harman_le_common_disc();
        }
        break;

#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
    case APP_TIMER_FIND_MY_BUDS_RING:
        {
            app_stop_timer(&timer_handle_find_my_buds_ring);
            if ((app_cfg_nv.right_bud_status) || (app_cfg_nv.left_bud_status))
            {
                app_harman_find_my_buds_ring_play();
            }
            else
            {
                app_harman_find_my_buds_ring_stop();
            }
        }
        break;
#endif

    case APP_TIMER_UPDATE_TOTAL_TIME:
        {
            app_stop_timer(&timer_handle_update_total_time);

            app_harman_total_power_on_time_update();
            app_harman_total_playback_time_update();

            app_start_timer(&timer_handle_update_total_time, "harman_update_total_time",
                            vendor_harman_cmd_timer_queue_id, APP_TIMER_UPDATE_TOTAL_TIME,
                            0, false, HARMAN_UPDATE_TOTAL_TIME_INTERVAL);
        }
        break;

    default:
        break;
    }
}

void app_refine_a2dp_bitrate(void)
{
    APP_PRINT_TRACE0("app_refine_a2dp_bitrate");
    app_cfg_const.aac_bit_rate = (app_cfg_nv.cmd_aac_bit_rate * 1000);
    app_cfg_const.sbc_max_bitpool = app_cfg_nv.cmd_sbc_max_bitpool;
    bt_a2dp_role_set(BT_A2DP_ROLE_SNK);

    if (app_cfg_const.a2dp_codec_type_sbc)
    {
        T_BT_A2DP_STREAM_ENDPOINT sep;

        sep.codec_type = BT_A2DP_CODEC_TYPE_SBC;
        sep.u.codec_sbc.sampling_frequency_mask = app_cfg_const.sbc_sampling_frequency;
        sep.u.codec_sbc.channel_mode_mask = app_cfg_const.sbc_channel_mode;
        sep.u.codec_sbc.block_length_mask = app_cfg_const.sbc_block_length;
        sep.u.codec_sbc.subbands_mask = app_cfg_const.sbc_subbands;
        sep.u.codec_sbc.allocation_method_mask = app_cfg_const.sbc_allocation_method;
        sep.u.codec_sbc.min_bitpool = app_cfg_const.sbc_min_bitpool;
        sep.u.codec_sbc.max_bitpool = app_cfg_const.sbc_max_bitpool;
        bt_a2dp_stream_endpoint_add(sep);
    }

    if (app_cfg_const.a2dp_codec_type_aac)
    {
        T_BT_A2DP_STREAM_ENDPOINT sep;

        sep.codec_type = BT_A2DP_CODEC_TYPE_AAC;
        sep.u.codec_aac.object_type_mask = app_cfg_const.aac_object_type;
        sep.u.codec_aac.sampling_frequency_mask = app_cfg_const.aac_sampling_frequency;
        sep.u.codec_aac.channel_number_mask = app_cfg_const.aac_channel_number;
        sep.u.codec_aac.vbr_supported = app_cfg_const.aac_vbr_supported;
        sep.u.codec_aac.bit_rate = app_cfg_const.aac_bit_rate;
        bt_a2dp_stream_endpoint_add(sep);
    }
}

void app_set_a2dp_disc_reason(uint8_t disc_by_for_reconfigure, uint8_t idx)
{
    if (disc_by_for_reconfigure)
    {
        APP_RECONFIG_SET(is_a2dp_disconnect_by_reconfigure, idx);
    }
    else
    {
        APP_RECONFIG_PUT(is_a2dp_disconnect_by_reconfigure, idx);
    }
    APP_PRINT_INFO1("au_set_a2dp_disc_reason, is_a2dp_disconnect_by_reconfigure: %d",
                    is_a2dp_disconnect_by_reconfigure);
}

static uint8_t app_get_a2dp_disc_reason(void)
{
    APP_PRINT_INFO1("au_get_a2dp_disc_reason, is_a2dp_disconnect_by_reconfigure: %d",
                    is_a2dp_disconnect_by_reconfigure);
    return is_a2dp_disconnect_by_reconfigure;
}

static void app_harman_reconnect_avdtp_profile(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if ((app_db.br_link[i].connected_profile & A2DP_PROFILE_MASK))
        {
            if ((app_db.br_link[i].connected_profile & AVRCP_PROFILE_MASK))
            {
                APP_PRINT_TRACE2("avrcp status = %d, straming flag = %d", app_db.br_link[i].avrcp_play_status,
                                 app_db.br_link[i].streaming_fg);
                if ((app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    && (app_db.br_link[i].streaming_fg == true))
                {
                    app_db.br_link[i].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                    bt_avrcp_pause(app_db.br_link[i].bd_addr);
                }
            }
            app_set_a2dp_disc_reason(true, i);
            linkback_profile_disconnect_start(app_db.br_link[i].bd_addr, A2DP_PROFILE_MASK);
        }
    }
}

void app_harman_recon_a2dp(uint8_t idx)
{
    if (app_get_a2dp_disc_reason())
    {
        app_set_a2dp_disc_reason(false, idx);
        if (idx == 0)
        {
            app_start_timer(&timer_handle_recon_a2dp_profile, "recon_a2dp_profile",
                            vendor_harman_cmd_timer_queue_id, APP_RECON_A2DP_PROFILE, idx,
                            false, HARMAN_RECONN_PROFILE_TIME);
            app_start_timer(&timer_handle_recon_profile_protect, "recon_profile_protect",
                            vendor_harman_cmd_timer_queue_id, APP_RECON_PROFILE_PROTECT, idx,
                            false, HARMAN_RECNN_PROFILE_PRTECT_TIME);
        }
        else if (idx == 1)
        {
            app_start_timer(&timer_handle_multi_recon_a2dp_profile, "recon_multi_a2dp_profile",
                            vendor_harman_cmd_timer_queue_id, APP_MULTI_RECON_A2DP_PROFILE, idx,
                            false, HARMAN_RECONN_PROFILE_TIME);
            app_start_timer(&timer_handle_multi_recon_profile_protect, "recon_multi_profile_protect",
                            vendor_harman_cmd_timer_queue_id, APP_MULTI_RECON_PROFILE_PROTECT, idx,
                            false, HARMAN_RECNN_PROFILE_PRTECT_TIME);
        }
    }
}

uint8_t app_harman_cmd_support_check(uint16_t cmd_id, uint16_t data_len, uint8_t id)
{
    uint8_t ret = false;
    uint8_t mobile_app_idx = MAX_BR_LINK_NUM;

    harman_get_active_mobile_cmd_link(&mobile_app_idx);
    APP_PRINT_INFO3("app_harman_cmd_support_check: harman_cmd_id: 0x%04x, app_idx: %d, mobile_app_idx: %d",
                    cmd_id, id, mobile_app_idx);

    switch (cmd_id)
    {
    case CMD_HARMAN_DEVICE_INFO_GET:
    case CMD_HARMAN_DEVICE_INFO_SET:
    case CMD_HARMAN_DEVICE_INFO_APP_NOTIFY:
    case CMD_HARMAN_OTA_START:
    case CMD_HARMAN_OTA_PACKET:
    case CMD_HARMAN_OTA_STOP:
    case CMD_HARMAN_OTA_NOTIFICATION:
        {
            ret = true;
        }
        break;

    default:
        {
            APP_PRINT_WARN1("app_harman_cmd_support_check: CMD 0x%04x not_support!!", cmd_id);
        }
        break;
    }

    return ret;
}

/* return: packed size */
uint16_t app_harman_devinfo_feature_pack(uint8_t *p_buf, uint16_t id, uint16_t size,
                                         uint8_t *value)
{
    uint16_t feature_len = 0;

    feature_len = size + 4;

    if (p_buf != NULL)
    {
        p_buf[0] = id & 0xFF;
        p_buf[1] = (id >> 8) & 0xFF;
        p_buf[2] = size & 0xFF;
        p_buf[3] = (size >> 8) & 0xFF;
        if (value != NULL)
        {
            memcpy(p_buf + 4, value, size);
        }
    }

    return feature_len;
}

void app_harman_devinfo_notification_report(uint8_t *p_buf, uint16_t buf_len, uint8_t app_idx)
{
    if (app_idx < MAX_BR_LINK_NUM)
    {
        app_harman_report_le_event(&app_db.le_link[app_idx], CMD_HARMAN_DEVICE_INFO_DEVICE_NOTIFY,
                                   p_buf, buf_len);
    }
}

static void app_harman_devinfo_set_response(uint16_t status_code, uint8_t app_idx)
{
    uint8_t p_cmd_rsp[2] = {0};

    p_cmd_rsp[0] = status_code & 0xFF;
    p_cmd_rsp[1] = (status_code >> 8) & 0xFF;

    if (app_idx < MAX_BR_LINK_NUM)
    {
        app_harman_report_le_event(&app_db.le_link[app_idx], CMD_HARMAN_DEVICE_INFO_SET, p_cmd_rsp, 2);
    }
}

void app_harman_total_playback_time_update(void)
{
    uint32_t play_time = 0;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    if ((active_a2dp_idx != MAX_BR_LINK_NUM) &&
        (app_db.br_link[active_a2dp_idx].streaming_fg) &&
        (!app_harman_get_flag_need_clear_total_time()))
    {
        uint32_t cur_time = sys_timestamp_get(); // ms

        if (cur_time > app_db.playback_start_time)
        {
            play_time = cur_time - app_db.playback_start_time;
        }
        else
        {
            play_time = 0xFFFFFFFF / 1000 - app_db.playback_start_time + cur_time;
        }
        APP_PRINT_INFO4("app_harman_total_playback_time_update: play_time(ms): %d, "
                        "cur_time(ms): %d, playback_start_time(ms): %d, total_playback_time(s): %d",
                        play_time, cur_time, app_db.playback_start_time,
                        app_cfg_nv.total_playback_time);
        if (play_time >= 1000) // ms convert to s
        {
            app_db.playback_start_time = cur_time;
            app_cfg_nv.total_playback_time += (play_time / 1000);
            app_cfg_store(&app_cfg_nv.total_playback_time, 4);
        }
    }
}

void app_harman_total_power_on_time_update(void)
{
    if ((app_db.device_state == APP_DEVICE_STATE_ON) &&
        (!app_harman_get_flag_need_clear_total_time()))
    {
        uint32_t power_on_time = 0;
        uint32_t cur_time = sys_timestamp_get(); // ms

        if (cur_time > app_db.power_on_start_time)
        {
            power_on_time = cur_time - app_db.power_on_start_time;
        }
        else
        {
            power_on_time = 0xFFFFFFFF / 1000 - app_db.power_on_start_time + cur_time;
        }
        APP_PRINT_INFO4("app_harman_total_power_on_time_update: power_on_time(ms): %d, "
                        "cur_time(ms): %d, power_on_start_time(ms): %d, total_power_on_time(s): %d",
                        power_on_time, cur_time, app_db.power_on_start_time,
                        app_cfg_nv.total_power_on_time);
        if (power_on_time >= 1000) // ms convert to s
        {
            app_db.power_on_start_time = cur_time;

            app_cfg_nv.total_power_on_time += (power_on_time / 1000);
            app_cfg_store(&app_cfg_nv.total_power_on_time, 4);
        }
    }
}

static void app_harman_devinfo_get(uint8_t *p_cmd_rsp, uint16_t feature_id, uint16_t payloadlen)
{
    APP_PRINT_INFO3("app_harman_devinfo_get: p_cmd_rsp: 0x%x, feature_id: 0x%04x, value_size: 0x%04x",
                    p_cmd_rsp, feature_id, payloadlen);
    switch (feature_id)
    {
    case FEATURE_DEVICE_NAME:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.device_name_legacy);
        }
        break;

    case FEATURE_MAC_ADDR:
        {
            uint8_t local_bt_addr[6] = {0};

            gap_get_param(GAP_PARAM_BD_ADDR, local_bt_addr);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&local_bt_addr);
        }
        break;

    case FEATURE_PID:
        {
            uint16_t product_id = app_harman_license_pid_get();

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&product_id);
        }
        break;

    case FEATURE_COLOR_ID:
        {
            uint8_t color_id = app_harman_license_cid_get();

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &color_id);
        }
        break;

    case FEATURE_FIEMWARE_VERSION:
        {
            T_IMG_HEADER_FORMAT *addr;

            addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_MCUCONFIG);
            firmware_version[0] = (uint8_t)(addr->git_ver.version >> 16);
            firmware_version[1] = (uint8_t)(addr->git_ver.version >> 8);
            firmware_version[2] = (uint8_t)addr->git_ver.version;

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, firmware_version);
        }
        break;

    case FEATURE_AUTO_POWER_OFF:
        {
            APP_PRINT_INFO2("FEATURE_AUTO_POWER_OFF: status: 0x%04x, time: 0x%04x",
                            app_cfg_nv.auto_power_off_status,
                            app_cfg_nv.auto_power_off_time);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.harman_auto_power_off);
        }
        break;

    case FEATURE_AUTO_STANDBY:
        {
            APP_PRINT_INFO2("FEATURE_AUTO_STANDBY: status: 0x%04x, time: 0x%04x",
                            app_cfg_nv.auto_standby_status,
                            app_cfg_nv.auto_standby_time);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.harman_auto_standby);
        }
        break;

    case FEATURE_VOICE_PROMPT_LANGUAGE:
        {
            APP_PRINT_INFO1("FEATURE_VOICE_PROMPT_LANGUAGE: language_type: 0x%x",
                            app_cfg_const.voice_prompt_language);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            &app_cfg_const.voice_prompt_language);
        }
        break;

    case FEATURE_VOICE_PROMPT_FILE_VERSION:
        {
            APP_PRINT_INFO3("FEATURE_VOICE_PROMPT_FILE_VERSION: language_version: 0x%x, 0x%x, 0x%x",
                            app_cfg_nv.language_version[0],
                            app_cfg_nv.language_version[1],
                            app_cfg_nv.language_version[2]);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, app_cfg_nv.language_version);
        }
        break;

    case FEATURE_VOICE_PROMPT_STATUS:
        {
            APP_PRINT_INFO1("FEATURE_VOICE_PROMPT_STATUS: language_status: 0x%x",
                            app_cfg_nv.language_status);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            &app_cfg_nv.harman_language_status);
        }
        break;
#if 0
    case FEATURE_FIND_MY_BUDS_STATUS:
        {
            if ((app_gfps_rfc_get_ring_state() == GFPS_RFC_RIGHT_RING) ||
                (app_gfps_rfc_get_ring_state() == GFPS_RFC_ALL_RING))
            {
                app_cfg_nv.right_bud_status = 1;
            }
            else if ((app_gfps_rfc_get_ring_state() == GFPS_RFC_LEFT_RING) ||
                     (app_gfps_rfc_get_ring_state() == GFPS_RFC_ALL_RING))
            {
                app_cfg_nv.left_bud_status = 1;
            }
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.find_my_buds_status);
        }
        break;
#endif

    case FEATURE_MTU:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&app_cfg_nv.ota_mtu);
        }
        break;

    case FEATURE_OTA_DATA_MAX_SIZE_IN_ONE_PACK:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.ota_max_size_in_one_pack);
        }
        break;

    case FEATURE_OTA_CONTINUOUS_PACKETS_MAX_COUNT:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.ota_continuous_packets_max_count);
        }
        break;

    case FEATURE_MAX_VOLUME_LIMIT:
        {
            APP_PRINT_INFO2("FEATURE_MAX_VOLUME_LIMIT: status: 0x%x, level: 0x%x",
                            app_cfg_nv.max_volume_limit_status,
                            app_cfg_nv.max_volume_limit_level);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.harman_max_volume_limit);
        }
        break;

    case FEATURE_SIDETONE:
        {
            APP_PRINT_INFO2("FEATURE_SIDETONE: sidetone_switch: 0x%x, level: 0x%x",
                            app_cfg_nv.sidetone_switch,
                            app_cfg_nv.sidetone_level);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.harman_sidetone);
        }
        break;

    case FEATURE_CALL_STATUS:
        {
            uint8_t call_status = DEVICE_IS_IDLE;

            if (app_hfp_get_call_status())
            {
                call_status = DEVICE_IS_CALLING;
            }
            else
            {
                call_status = DEVICE_IS_IDLE;
            }

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &call_status);
        }
        break;

    case FEATURE_TOTAL_PLAYBACK_TIME:
        {
            uint32_t playback_time = 0;

            app_harman_total_playback_time_update();
            playback_time = app_cfg_nv.total_playback_time / 60; // s convert to min
            APP_PRINT_INFO1("FEATURE_TOTAL_PLAYBACK_TIME: %d minute", playback_time);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&playback_time);
        }
        break;

    case FEATURE_TOTAL_POWER_ON_TIME:
        {
            uint32_t power_on_time = 0;

            app_harman_total_power_on_time_update();
            power_on_time = app_cfg_nv.total_power_on_time / 60; // s convert to min
            APP_PRINT_INFO1("FEATURE_TOTAL_POWER_ON_TIME: %d minute", power_on_time);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&power_on_time);
        }
        break;

    case FEATURE_BATTERY_REMAIN_CAPACITY:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            detail_battery_info);
        }
        break;

    case FEATURE_PLAYER_VOLUME:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, current_vol);
        }
        break;

    case FEATURE_TWS_INFO:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &tws_connectiong_status);
        }
        break;

    case FEATURE_ANC:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &anc_status);
        }
        break;

    case FEATURE_BT_CONN_STATUS:
        {
            if (app_link_get_b2s_link_num() == 0)
            {
                bt_connection_status = 0x00;
            }
            else
            {
                bt_connection_status = 0x01;
            }
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &bt_connection_status);
        }
        break;

    case FEATURE_OTA_UPGRADE_STATUS:
        {
            uint8_t status = app_harman_ble_ota_get_upgrate_status();

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &status);
        }
        break;

    case FEATURE_AUTO_PLAY_OR_PAUSE_ENABLE:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &auto_play_pause_enable_status);
        }
        break;

    case FEATURE_PERSONIFI:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &perconi_fi);
        }
        break;

    case FEATURE_IN_EAR_STATUS:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, in_ear_status);
        }
        break;

    case FEATURE_SEALING_TEST_STATUS:
        {
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, sealing_test_status);
        }
        break;

    case FEATURE_SMART_SWITCH:
        {
            uint8_t value[8] = {0};

            memcpy(&value[0], (uint8_t *)&app_cfg_nv.cmd_aac_bit_rate, 2);
            memcpy(&value[2], (uint8_t *)&app_cfg_nv.cmd_sbc_max_bitpool, 2);
            memcpy(&value[4], (uint8_t *)&app_cfg_nv.cmd_latency_in_ms, 2);
            memcpy(&value[6], (uint8_t *)&app_cfg_nv.cmd_second_latency_in_ms, 2);
            APP_PRINT_TRACE4("FEATURE_SMART_SWITCH: %d, %d, %d, %d",
                             app_cfg_nv.cmd_aac_bit_rate,
                             app_cfg_nv.cmd_sbc_max_bitpool,
                             app_cfg_nv.cmd_latency_in_ms,
                             app_cfg_nv.cmd_second_latency_in_ms);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &value[0]);
        }
        break;

    case FEATURE_LEFT_DEVICE_SERIAL_NUM:
        {
            uint8_t *serials_num = app_harman_license_serials_num_get();

            APP_PRINT_TRACE2("FEATURE_LEFT_DEVICE_SERIAL_NUM: size: %d, %s",
                             HARMAN_SERIALS_NUM_LEN, TRACE_STRING(serials_num));
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, serials_num);
        }
        break;

    case FEATURE_LEFT_DEVICE_BATT_STATUS:
        {
            uint8_t battery_status = app_harman_get_battery_status();

            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, &battery_status);
        }
        break;

#if JBL_APP_LEA_BUTTON
    case FEATURE_LE_AUDIO_UNICAST_STATUS:
        {
            uint8_t le_switch;
            if (app_harman_get_lea_mode())
            {
                le_switch = 0xFF;
            }
            else
            {
                le_switch = 0X7F;
            }
            APP_PRINT_TRACE1("FEATURE_LE_AUDIO_UNICAST_STATUS SEND SUCCESS %x", le_switch);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen, (uint8_t *)&le_switch);
        }
        break;
#endif

#if HARMAN_OPEN_LR_FEATURE
    case FEATURE_LR_BALANCE:
        {
            APP_PRINT_INFO3("FEATURE_LR_BALANCE: LR_option: 0x%x, LR_vol_level: 0x%x, harman_EQmaxdB: 0x%x",
                            app_cfg_nv.LR_option,
                            app_cfg_nv.LR_vol_level,
                            app_cfg_nv.harman_EQmaxdB);
            app_harman_devinfo_feature_pack(p_cmd_rsp, feature_id, payloadlen,
                                            (uint8_t *)&app_cfg_nv.harman_LR_balance);
        }
        break;
#endif

    default:
        break;
    }
}

static void app_harman_devinfo_set(uint16_t feature_id, uint16_t value_size, uint8_t *p_value,
                                   uint16_t app_idx)
{
    uint8_t *p_cmd_rsp = NULL;
    uint16_t rsp_len = 0;

    APP_PRINT_INFO4("app_harman_devinfo_set: app_idx: %d, feature_id 0x%04x, value_size: 0x%04x, data(%b)",
                    app_idx, feature_id, value_size, TRACE_BINARY(value_size, p_value));

    switch (feature_id)
    {
    case FEATURE_MANUAL_POWER_OFF:
        {
            uint16_t time = (uint16_t)(p_value[0] | (p_value[1] << 8));

            APP_PRINT_INFO1("FEATURE_MANUAL_POWER_OFF: time: 0x%x", time);
            if (time == 0)
            {
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
            else
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_LE_CMD, time);
            }
        }
        break;

    case FEATURE_AUTO_POWER_OFF:
        {
            app_cfg_nv.harman_auto_power_off = (uint16_t)(p_value[0] | (p_value[1] << 8));
            app_cfg_store(&app_cfg_nv.harman_auto_power_off, 4);
            APP_PRINT_INFO2("FEATURE_AUTO_POWER_OFF: status: 0x%04x, time: 0x%04x",
                            app_cfg_nv.auto_power_off_status,
                            app_cfg_nv.auto_power_off_time);

            if (app_harman_power_off_option_timer_get() != 0)
            {
                if (app_cfg_nv.auto_power_off_status == 0)
                {
                    app_harman_connect_idle_to_power_off(ACTIVE_NEED_STOP_COUNT, app_idx);
                }
                else
                {
                    app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, app_idx);
                }
            }
        }
        break;

    case FEATURE_AUTO_STANDBY:
        {
            app_cfg_nv.harman_auto_standby = (uint16_t)(p_value[0] | (p_value[1] << 8));
            app_cfg_store(&app_cfg_nv.harman_auto_power_off, 4);
            APP_PRINT_INFO2("FEATURE_AUTO_STANDBY: status: 0x%x, time: 0x%x",
                            app_cfg_nv.auto_standby_status,
                            app_cfg_nv.auto_standby_time);
        }
        break;

    case FEATURE_FACTORY_RESET:
        {
            app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
        }
        break;

    case FEATURE_DEVICE_NAME:
        {
            memcpy(app_cfg_nv.device_name_legacy, p_value, value_size);
        }
        break;

    case FEATURE_MAX_VOLUME_LIMIT:
        {
            app_cfg_nv.harman_max_volume_limit = p_value[0];
            app_cfg_nv.max_volume_limit_level = 11;
            app_cfg_store(&app_cfg_nv.harman_sidetone, 4);

            APP_PRINT_INFO2("FEATURE_MAX_VOLUME_LIMIT: status: 0x%x, level: 0x%x",
                            app_cfg_nv.max_volume_limit_status,
                            app_cfg_nv.max_volume_limit_level);
            T_APP_BR_LINK *p_br_link = NULL;
            uint8_t active_idx = app_a2dp_get_active_idx();

            p_br_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);
            if (p_br_link)
            {
                uint8_t pair_idx_mapping;
                uint8_t curr_volume = 0;

                app_bond_get_pair_idx_mapping(app_db.br_link[active_idx].bd_addr, &pair_idx_mapping);
                curr_volume = app_cfg_nv.audio_gain_level[pair_idx_mapping];

#if HARMAN_OPEN_LR_FEATURE
                app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, curr_volume, __func__, __LINE__);
#endif
                audio_track_volume_out_set(app_db.br_link[active_idx].a2dp_track_handle, curr_volume);
            }

#if F_APP_LEA_SUPPORT
            if (app_lea_ccp_get_call_status() == APP_CALL_IDLE)
            {
                T_LEA_ASE_ENTRY *p_ase_entry = NULL;
                T_APP_LE_LINK *p_le_link = NULL;
                uint8_t lea_curr_volume = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);

                p_le_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());
                if (p_le_link)
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry_by_direction(p_le_link, DATA_PATH_OUTPUT_FLAG);
                }

                if (p_ase_entry)
                {
#if HARMAN_OPEN_LR_FEATURE
                    app_harman_lr_balance_set(p_ase_entry->stream_type, lea_curr_volume, __func__, __LINE__);
#endif
                    audio_track_volume_out_set(p_ase_entry->track_handle, lea_curr_volume);
                }
            }
#endif
        }
        break;

    case FEATURE_VOICE_PROMPT_FILE_VERSION:
        {
            memcpy(&app_cfg_nv.language_version[0], p_value, value_size);
            app_cfg_store(&app_cfg_nv.language_version, 4);
        }
        break;

    case FEATURE_VOICE_PROMPT_STATUS:
        {
            if (((app_hfp_sco_is_connected()) || (timer_idx_hfp_ring != NULL))
                && (app_cfg_nv.sidetone_switch))
            {
                APP_PRINT_TRACE0("FEATURE_VOICE_PROMPT_STATUS sco");
            }
            else
            {
                app_cfg_nv.harman_language_status = p_value[0];
                app_cfg_store(&app_cfg_nv.harman_sidetone, 4);
            }
        }
        break;

#if HARMAN_FIND_MY_BUDS_TONE_SUPPORT
    case FEATURE_FIND_MY_BUDS_STATUS:
        {
            app_cfg_nv.find_my_buds_status = p_value[0];
            app_cfg_store(&app_cfg_nv.vp_ota_status, 4);

            APP_PRINT_TRACE2("FEATURE_FIND_MY_BUDS_STATUS: right_bud_status: %d, left_bud_status: %d",
                             app_cfg_nv.right_bud_status,
                             app_cfg_nv.left_bud_status);
            if (app_cfg_nv.right_bud_status || app_cfg_nv.left_bud_status)
            {
                app_harman_find_my_buds_ring_play();
            }
            else
            {
                app_harman_find_my_buds_ring_stop();
            }
        }
        break;
#endif

    case FEATURE_HOST_TYPE:
        {
            // 0x01: Android, 0x02: IOS, 0x03: Dongle, 0x04: SoudBar
            app_cfg_nv.host_type = p_value[0];
            app_cfg_store(&app_cfg_nv.harman_sidetone, 4);
            APP_PRINT_TRACE1("FEATURE_HOST_TYPE: host_type: %d", app_cfg_nv.host_type);
        }
        break;

    case FEATURE_SIDETONE:
        {
            T_AUDIO_TRACK_STATE state;
            T_APP_BR_LINK *p_link;

            app_cfg_nv.harman_sidetone = p_value[0];
            app_cfg_store(&app_cfg_nv.harman_sidetone, 4);

            p_link = app_link_find_br_link(app_db.br_link[app_hfp_get_active_idx()].bd_addr);
            audio_track_state_get(p_link->sco_track_handle, &state);

            APP_PRINT_TRACE2("FEATURE_SIDETONE: switch: %d, level: %d",
                             app_cfg_nv.sidetone_switch,
                             app_cfg_nv.sidetone_level);
            APP_PRINT_TRACE4("FEATURE_SIDETONE: track_state: %d, hfp_active_index: %d, call_status: %d, device_os: %d",
                             state, app_hfp_get_active_idx(),
                             app_hfp_get_call_status(), p_link->remote_device_vendor_id);

            if (app_hfp_sco_is_connected() && (state == AUDIO_TRACK_STATE_STARTED)) //sco
            {
                if ((app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING))
                {
                    APP_PRINT_TRACE0("FEATURE_SIDETONE: VA not open sidetone");
                }
                else
                {
                    if (app_cfg_nv.sidetone_switch)
                    {
                        harman_sidetone_set_dsp();
                    }
                    else
                    {
                        if (p_link->sidetone_instance != NULL)
                        {
                            sidetone_disable(p_link->sidetone_instance);
                            sidetone_release(p_link->sidetone_instance);
                            p_link->sidetone_instance = NULL;
                        }
                    }
                }
            }


            if (app_lea_uca_is_upstreaming_enabled())
            {
                T_AUDIO_TRACK_STATE state;
                T_LEA_ASE_ENTRY *p_ase_entry = NULL;

                if (app_lea_uca_get_diff_path())
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_UP_DIRECT, NULL, NULL);
                }
                else
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_DEVICE_INOUT, NULL, NULL);
                }

                if (p_ase_entry)
                {
                    audio_track_state_get(p_ase_entry->track_handle, &state);

                    if (state == AUDIO_TRACK_STATE_STARTED)
                    {
                        if (app_cfg_nv.sidetone_switch)
                        {
                            if ((p_ase_entry->sidetone_instance == NULL) &&
                                (app_cfg_nv.sidetone_switch == SIDETONE_SWTCH_ENABLE))
                            {
                                p_ase_entry->sidetone_instance = sidetone_create(SIDETONE_TYPE_HW,
                                                                                 app_dsp_cfg_sidetone.gain,
                                                                                 app_dsp_cfg_sidetone.hpf_level);

                                if (p_ase_entry->sidetone_instance != NULL)
                                {
                                    sidetone_enable(p_ase_entry->sidetone_instance);
                                    audio_track_effect_attach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
                                }
                            }

                            if (app_cfg_nv.sidetone_switch == SIDETONE_SWTCH_ENABLE)
                            {
                                if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_L)
                                {
                                    //L -18db
                                    sidetone_gain_set(p_ase_entry->sidetone_instance, (-18 * 128));
                                }
                                else if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_M)
                                {
                                    //M -15.375db
                                    sidetone_gain_set(p_ase_entry->sidetone_instance, (-15.375 * 128));
                                }
                                else if (app_cfg_nv.sidetone_level == SIDETOME_LEVEL_H)
                                {
                                    //H -13.125db
                                    sidetone_gain_set(p_ase_entry->sidetone_instance, (-13.125 * 128));
                                }
                            }

                            APP_PRINT_INFO2("harman_sidetone_set_dsp %d,%d", app_cfg_nv.sidetone_switch,
                                            app_cfg_nv.sidetone_level);
                        }
                        else
                        {
                            if (p_ase_entry->sidetone_instance != NULL)
                            {
                                sidetone_disable(p_ase_entry->sidetone_instance);
                                sidetone_release(p_ase_entry->sidetone_instance);
                                p_ase_entry->sidetone_instance = NULL;
                            }
                        }
                    }
                }
            }
        }
        break;

    case FEATURE_SMART_SWITCH:
        {
            app_cfg_nv.cmd_aac_bit_rate = (p_value[1] << 8) | p_value[0];           // 1st 2bytes
            app_cfg_nv.cmd_sbc_max_bitpool = (p_value[3] << 8) | p_value[2];        // 2nd 2bytes
            app_cfg_nv.cmd_latency_in_ms = (p_value[5] << 8) | p_value[4];          // 3rd 2bytes
            app_cfg_nv.cmd_second_latency_in_ms = (p_value[7] << 8) | p_value[6];    // 4th 2bytes

            app_cfg_store(&app_cfg_nv.cmd_aac_bit_rate, 8);

            app_harman_reconnect_avdtp_profile();
            app_refine_a2dp_bitrate();

            APP_PRINT_TRACE4("FEATURE_SMART_SWITCH: %d, %d, %d, %d",
                             app_cfg_nv.cmd_aac_bit_rate,
                             app_cfg_nv.cmd_sbc_max_bitpool,
                             app_cfg_nv.cmd_latency_in_ms,
                             app_cfg_nv.cmd_second_latency_in_ms);

            app_start_timer(&timer_handle_smartswitch_disc_le_conn, "smartswitch_disc_le_conn",
                            vendor_harman_cmd_timer_queue_id, APP_SMARTSWITCH_DISC_LE_CONN, 0,
                            false, HARMAN_SMARTSWITCH_DISCONN_LE_CONN);

        }
        break;

#if HARMAN_OPEN_LR_FEATURE
    case FEATURE_LR_BALANCE:
        {
            app_cfg_nv.harman_LR_balance = p_value[0];
            app_cfg_store(&app_cfg_nv.harman_EQmaxdB, 4);
            APP_PRINT_INFO3("FEATURE_LR_BALANCE: LR_option: 0x%x, LR_vol_level: 0x%x, harman_EQmaxdB: 0x%x",
                            app_cfg_nv.LR_option,
                            app_cfg_nv.LR_vol_level,
                            app_cfg_nv.harman_EQmaxdB);

            uint8_t a2dp_pair_idx_mapping;
            uint8_t a2dp_curr_volume;
            uint8_t *a2dp_addr = app_db.br_link[app_a2dp_get_active_idx()].bd_addr;
            T_APP_BR_LINK *a2dp_p_link = app_link_find_br_link(a2dp_addr);

            app_bond_get_pair_idx_mapping(a2dp_p_link->bd_addr, &a2dp_pair_idx_mapping);
            a2dp_curr_volume = app_cfg_nv.audio_gain_level[a2dp_pair_idx_mapping];

            uint8_t hfp_pair_idx_mapping;
            uint8_t hfp_curr_volume;
            uint8_t *hfp_addr = app_db.br_link[app_hfp_get_active_idx()].bd_addr;
            T_APP_BR_LINK *hfp_p_link = app_link_find_br_link(hfp_addr);

            app_bond_get_pair_idx_mapping(hfp_p_link->bd_addr, &hfp_pair_idx_mapping);
            hfp_curr_volume = app_cfg_nv.voice_gain_level[hfp_pair_idx_mapping];

            if (hfp_p_link->sco_track_handle != NULL)
            {
                app_harman_lr_balance_set(AUDIO_STREAM_TYPE_VOICE, hfp_curr_volume, __func__, __LINE__);
                audio_track_volume_out_set(hfp_p_link->sco_track_handle, hfp_curr_volume);
            }
            else if (a2dp_p_link->a2dp_track_handle != NULL)
            {
                app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, a2dp_curr_volume, __func__, __LINE__);
                audio_track_volume_out_set(a2dp_p_link->a2dp_track_handle, a2dp_curr_volume);
            }

            uint8_t lea_curr_volume = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);
            T_LEA_ASE_ENTRY *p_ase_entry = NULL;

            if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
            {
                if (app_lea_uca_get_diff_path())
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_UP_DIRECT, NULL, NULL);
                }
                else
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_DEVICE_INOUT, NULL, NULL);
                }
            }
            else
            {
                T_APP_LE_LINK *p_link = NULL;

                p_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());
                if (p_link)
                {
                    p_ase_entry = app_lea_ascs_find_ase_entry_by_direction(p_link, DATA_PATH_OUTPUT_FLAG);
                }
            }

            if (p_ase_entry)
            {
                app_harman_lr_balance_set(p_ase_entry->stream_type, lea_curr_volume, __func__, __LINE__);
                audio_track_volume_out_set(p_ase_entry->track_handle, lea_curr_volume);
            }

            harman_set_vp_ringtone_balance(__func__, __LINE__);
            voice_prompt_volume_set(voice_prompt_volume_get());
            ringtone_volume_set(ringtone_volume_get());
        }
        break;
#endif
    case FEATURE_EQ_INFO_QUERY:
        {
            uint16_t eq_len = 0;
            T_APP_HARMAN_EQ_INFO eq_info_temp = {0};

            if (p_value[0] == CATEGORY_ID_CURRENT_EQ)
            {
                if (app_cfg_nv.harman_category_id == CATEGORY_ID_OFF)
                {
                    eq_len = APP_HARMAN_DEFAULT_EQ_INFO_SIZE;
                }
                else
                {
                    eq_len = APP_HARMAN_CUSTOMER_EQ_INFO_SIZE;
                }
                app_harman_eq_info_load_from_ftl(app_cfg_nv.harman_category_id, (uint8_t *)&eq_info_temp, eq_len);
                rsp_len = 2 + 4 + eq_len;
                p_cmd_rsp = malloc(rsp_len);
                p_cmd_rsp[0] = 0;
                p_cmd_rsp[1] = 0;
                app_harman_devinfo_feature_pack(&p_cmd_rsp[2], FEATURE_EQ_INFO, eq_len, (uint8_t *)&eq_info_temp);
            }
        }
        break;

    case FEATURE_EQ_INFO:
        {
            uint16_t eq_len = 0;
            T_APP_HARMAN_EQ_INFO eq_info_temp = {0};

            app_cfg_nv.harman_category_id = p_value[0];
            app_cfg_store(&app_cfg_nv.harman_category_id, 4);

            app_harman_eq_info_handle(feature_id, p_value, value_size);

            if (app_cfg_nv.harman_category_id == CATEGORY_ID_OFF)
            {
                eq_len = APP_HARMAN_DEFAULT_EQ_INFO_SIZE;
            }
            else
            {
                eq_len = APP_HARMAN_CUSTOMER_EQ_INFO_SIZE;
            }
            app_harman_eq_info_load_from_ftl(app_cfg_nv.harman_category_id, (uint8_t *)&eq_info_temp, eq_len);
            rsp_len = 2 + 4 + eq_len;
            p_cmd_rsp = malloc(rsp_len);
            p_cmd_rsp[0] = 0;
            p_cmd_rsp[1] = 0;
            app_harman_devinfo_feature_pack(&p_cmd_rsp[2], FEATURE_EQ_INFO, eq_len, (uint8_t *)&eq_info_temp);
        }
        break;

#if JBL_APP_LEA_BUTTON
    case FEATURE_LE_AUDIO_UNICAST_STATUS:
        {
            if (p_value[0] & 0x80)
            {
                app_harman_set_lea_mode(true);
            }
            else
            {
                app_harman_set_lea_mode(false);
            }
            APP_PRINT_TRACE1("FEATURE_LE_AUDIO_UNICAST_STATUS SET: %d", p_value[0]);
        }
        break;
#endif

#if HARMAN_SUPPORT_WATER_EJECTION
    case FEATURE_WATER_EJECTION:
        {
            uint8_t status = p_value[0];
            APP_PRINT_INFO1("FEATURE_WATER_EJECTION: status 0x%x", status);
            if (status == 0x01)
            {
                for (uint8_t i = 0; i < 4; i++)
                {
                    //reuse TONE_APT_EQ_1 for water ejection vp
                    app_audio_tone_type_play(TONE_APT_EQ_1, false, false);
                }
            }
            else if (status == 0x00)
            {
                app_audio_tone_type_stop();
                app_audio_tone_type_cancel(TONE_APT_EQ_1);
            }
        }
        break;
#endif

    case FEATURE_HEARTBEAT:
        {
            app_harman_heartbeat_check_times_set(0);
        }
        break;

    default:
        break;
    }

    if ((app_idx < MAX_BR_LINK_NUM) && p_cmd_rsp != NULL)
    {
        app_harman_devinfo_notification_report(p_cmd_rsp, rsp_len, app_idx);
        free(p_cmd_rsp);
        p_cmd_rsp = NULL;
    }
}

void app_harman_vendor_cmd_process(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                   uint8_t rx_seqn, uint8_t app_idx)
{
    uint16_t harman_sync_word = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    uint16_t harman_cmd_id = (uint16_t)(cmd_ptr[2] | (cmd_ptr[3] << 8));
    int16_t ptr_size = (uint16_t)(cmd_ptr[6] | (cmd_ptr[7] << 8));
    uint16_t ptr_index = HARMAN_CMD_HEADER_SIZE;
    uint8_t payloadlen = 0;
    uint8_t *p_cmd_rsp = NULL;
    uint8_t mobile_app_idx = MAX_BR_LINK_NUM;

    if ((harman_sync_word != HARMAN_CMD_DIRECT_SEND) &&
        (harman_sync_word != HARMAN_CMD_FORWARD))
    {
        APP_PRINT_ERROR1("app_harman_vendor_cmd_process: harman_sync_word is error: 0x%04x",
                         harman_sync_word);
        return;
    }

    app_harman_ble_adv_stop();

    if (cmd_path == CMD_PATH_LE)
    {
        if (app_db.le_link[app_idx].state == LE_LINK_STATE_CONNECTED)
        {
            app_db.le_link[app_idx].cmd_set_enable = true;
        }
    }
    harman_get_active_mobile_cmd_link(&mobile_app_idx);

    APP_PRINT_INFO5("app_harman_vendor_cmd_process: harman_cmd_id 0x%04x, len: 0x%x, app_idx: %d, mobile_app_idx: %d, data(%b)",
                    harman_cmd_id, ptr_size, app_idx, mobile_app_idx, TRACE_BINARY(ptr_size,
                                                                                   &cmd_ptr[HARMAN_CMD_HEADER_SIZE]));

    switch (harman_cmd_id)
    {
    case CMD_HARMAN_DEVICE_INFO_GET:
        {
            bool is_support = true;
            uint8_t *p_cmd_rsp = NULL;
            uint8_t feature_id_count = ptr_size / 2;
            T_HARMAN_FEATURE_ID_HEADER *p_item = NULL;
            uint8_t support_id_count = 0;
            uint8_t unsupport_id_count = 0;
            uint16_t *p_unsupport_id = NULL;
            uint16_t id_temp = 0;
            uint16_t size_temp = 0;

            p_item = malloc(feature_id_count * HARMAN_FEATURE_ID_HEADER_SIZE);
            p_unsupport_id = malloc(feature_id_count * HARMAN_FEATURE_ID_HEADER_SIZE);

            if ((p_item == NULL) || (p_unsupport_id == NULL))
            {
                break;
            }

            while ((support_id_count + unsupport_id_count) < feature_id_count)
            {
                is_support = true;
                id_temp = (uint16_t)(cmd_ptr[ptr_index] | cmd_ptr[ptr_index + 1] << 8);
                ptr_index += 2;
                ptr_size -= 2;

                switch (id_temp)
                {
                case FEATURE_ANC:
                case FEATURE_LR_BALANCE:
                case FEATURE_BT_CONN_STATUS:
                case FEATURE_OTA_UPGRADE_STATUS:
                case FEATURE_AUTO_PLAY_OR_PAUSE_ENABLE:
                case FEATURE_PERSONIFI:
                case FEATURE_SEALING_TEST_STATUS:
                case FEATURE_TWS_INFO:
                case FEATURE_PLAYER_VOLUME:
                case FEATURE_LEFT_DEVICE_BATT_STATUS:
                case FEATURE_COLOR_ID:
                case FEATURE_VOICE_PROMPT_LANGUAGE:
                case FEATURE_VOICE_PROMPT_STATUS:
                case FEATURE_OTA_CONTINUOUS_PACKETS_MAX_COUNT:
                case FEATURE_MAX_VOLUME_LIMIT:
                case FEATURE_FIND_MY_BUDS_STATUS:
                case FEATURE_SIDETONE:
                case FEATURE_CALL_STATUS:
                    {
                        size_temp = FEATURE_VALUE_SIZE_1B;
                    }
                    break;

                case FEATURE_PID:
                case FEATURE_AUTO_POWER_OFF:
                case FEATURE_AUTO_STANDBY:
                case FEATURE_IN_EAR_STATUS:
                case FEATURE_OTA_DATA_MAX_SIZE_IN_ONE_PACK:
                case FEATURE_MTU:
                    {
                        size_temp = FEATURE_VALUE_SIZE_2B;
                    }
                    break;

                case FEATURE_FIEMWARE_VERSION:
                case FEATURE_VOICE_PROMPT_FILE_VERSION:
                    {
                        size_temp = FEATURE_VALUE_SIZE_3B;
                    }
                    break;

                case FEATURE_TOTAL_PLAYBACK_TIME:
                case FEATURE_TOTAL_POWER_ON_TIME:
                    {
                        size_temp = FEATURE_VALUE_SIZE_4B;
                    }
                    break;

                case FEATURE_MAC_ADDR:
                    {
                        size_temp = BD_ADDR_SIZE;
                    }
                    break;

                case FEATURE_DEVICE_NAME:
                    {
                        size_temp = DEVICE_NAME_SIZE;
                    }
                    break;

                case FEATURE_SMART_SWITCH:
                    {
                        size_temp = SMART_SWITCH_SIZE;
                    }
                    break;

                case FEATURE_LEFT_DEVICE_SERIAL_NUM:
                    {
                        size_temp = HARMAN_SERIALS_NUM_LEN;
                    }
                    break;

#if JBL_APP_LEA_BUTTON
                case FEATURE_LE_AUDIO_UNICAST_STATUS:
                    {
                        size_temp = HARMAN_LE_UNICAST_SWITCH_LEN;
                    }
                    break;
#endif

                default:
                    {
                        is_support = false;
                        p_unsupport_id[unsupport_id_count] = id_temp;
                        payloadlen += 2;
                        unsupport_id_count ++;
                        APP_PRINT_WARN1("CMD_HARMAN_DEVICE_INFO_GET: not_support_feature_id: 0x%04x", id_temp);
                    }
                    break;
                }

                if (is_support)
                {
                    p_item[support_id_count].feature_id = id_temp;
                    p_item[support_id_count].value_size = size_temp;
                    payloadlen += (4 + size_temp);
                    support_id_count ++;
                }
            }

            if (unsupport_id_count > 0)
            {
                payloadlen += HARMAN_FEATURE_ID_HEADER_SIZE;
            }

            payloadlen += 2;
            p_cmd_rsp = malloc(payloadlen);
            if (p_cmd_rsp == NULL)
            {
                break;
            }

            p_cmd_rsp[0] = 0; // status_code
            p_cmd_rsp[1] = 0; // status_code

            APP_PRINT_INFO5("app_harman_vendor_cmd_process: feature_id_count: %d, supported_id_count: %d, "
                            "unsupport_id_count: %d, p_cmd_rsp: 0x%x, payloadlen: 0x%x",
                            feature_id_count, support_id_count, unsupport_id_count, p_cmd_rsp, payloadlen);

            uint8_t index = 0;
            uint8_t *p_cmd_rsp_temp = &p_cmd_rsp[2];

            /* pack upsupported feature id + length + value */
            if (unsupport_id_count > 0)
            {
                app_harman_devinfo_feature_pack(p_cmd_rsp_temp, FEATURE_UNSUPPORT, (unsupport_id_count * 2),
                                                (uint8_t *)&p_unsupport_id[0]);
                p_cmd_rsp_temp += (unsupport_id_count * 2 + HARMAN_FEATURE_ID_HEADER_SIZE);
            }
            free(p_unsupport_id);
            p_unsupport_id = NULL;

            /* pack supported feature id + length + value */
            for (index = 0; index < support_id_count; index ++)
            {
                app_harman_devinfo_get(p_cmd_rsp_temp, p_item[index].feature_id, p_item[index].value_size);
                p_cmd_rsp_temp += (p_item[index].value_size + HARMAN_FEATURE_ID_HEADER_SIZE);
            }
            free(p_item);
            p_item = NULL;

            /* report to JBL APP */
            if (payloadlen && (mobile_app_idx < MAX_BR_LINK_NUM))
            {
                app_harman_report_le_event(&app_db.le_link[mobile_app_idx], CMD_HARMAN_DEVICE_INFO_DEVICE_NOTIFY,
                                           p_cmd_rsp, payloadlen);
            }
            free(p_cmd_rsp);
            p_cmd_rsp = NULL;
        }
        break;

    case CMD_HARMAN_DEVICE_INFO_SET:
        {
            T_HARMAN_FEATURE_ID_HEADER item;
            uint16_t ptr_index = HARMAN_CMD_HEADER_SIZE;
            uint16_t ptr_size = (uint16_t)(cmd_ptr[6] | cmd_ptr[7] << 8);

            // TODO: multi feature id
            while ((cmd_ptr[ptr_index] != NULL) && (ptr_size > 0))
            {
                item.feature_id = (uint16_t)(cmd_ptr[ptr_index] | cmd_ptr[ptr_index + 1] << 8);
                item.value_size = (uint16_t)(cmd_ptr[ptr_index + 2] | cmd_ptr[ptr_index + 3] << 8);

                APP_PRINT_INFO3("app_harman_vendor_cmd_process: ptr_index: %d, feature_id: 0x%04x, value_size: 0x%04x",
                                ptr_index, item.feature_id, item.value_size);
                if ((item.feature_id == FEATURE_FACTORY_RESET) ||
                    (item.feature_id == FEATURE_AUTO_POWER_OFF) ||
                    (item.feature_id == FEATURE_MANUAL_POWER_OFF) ||
                    (item.feature_id == FEATURE_AUTO_STANDBY) ||
                    (item.feature_id == FEATURE_DEVICE_NAME) ||
                    (item.feature_id == FEATURE_SIDETONE) ||
                    (item.feature_id == FEATURE_SMART_SWITCH) ||
                    (item.feature_id == FEATURE_EQ_INFO_QUERY) ||
                    (item.feature_id == FEATURE_EQ_INFO) ||
                    (item.feature_id == FEATURE_DYNAMIC_EQ) ||
                    (item.feature_id == FEATURE_CUSTOM_EQ_COUNT) ||
                    (item.feature_id == FEATURE_LR_BALANCE) ||
                    (item.feature_id == FEATURE_FIND_MY_BUDS_STATUS) ||
                    (item.feature_id == FEATURE_MAX_VOLUME_LIMIT) ||
                    (item.feature_id == FEATURE_LEFT_DEVICE_SERIAL_NUM) ||
                    (item.feature_id == FEATURE_HOST_TYPE) ||
#if JBL_APP_LEA_BUTTON
                    (item.feature_id == FEATURE_LE_AUDIO_UNICAST_STATUS) ||
#endif
#if HARMAN_SUPPORT_WATER_EJECTION
                    (item.feature_id == FEATURE_WATER_EJECTION) ||
#endif
                    (item.feature_id == FEATURE_VOICE_PROMPT_STATUS) ||
                    (item.feature_id == FEATURE_HEARTBEAT))
                {
                    // response feature id is recved
                    // app_harman_devinfo_set_response(STATUS_SUCCESS, app_idx);
                    // report feature id is recvied
                    if ((item.feature_id != FEATURE_EQ_INFO_QUERY) &&
                        (item.feature_id != FEATURE_EQ_INFO) &&
                        (item.feature_id != FEATURE_HEARTBEAT))
                    {
                        app_harman_devinfo_feature_report(item.feature_id, item.value_size, &cmd_ptr[ptr_index + 4],
                                                          app_idx);
                    }
                    // excute the feature id
                    app_harman_devinfo_set(item.feature_id, item.value_size, &cmd_ptr[ptr_index + 4], app_idx);
                }
                else
                {
                    APP_PRINT_WARN1("CMD_HARMAN_DEVICE_INFO_SET: not_support_feature_id: 0x%04x", item.feature_id);
                    // app_harman_devinfo_set_response(STATUS_ERROR_PROTOCOL | UNSUPPORTED_COMMAND_ID, app_idx);
                    app_harman_devinfo_feature_report(FEATURE_UNSUPPORT, 2, (uint8_t *)&item.feature_id, app_idx);
                }
                ptr_index += (HARMAN_FEATURE_ID_HEADER_SIZE + item.value_size);
                ptr_size -= (HARMAN_FEATURE_ID_HEADER_SIZE + item.value_size);
            }
        }
        break;

    case CMD_HARMAN_DEVICE_INFO_APP_NOTIFY:
        {
            APP_PRINT_INFO0("CMD_HARMAN_DEVICE_INFO_APP_NOTIFY");
        }
        break;

    case CMD_HARMAN_OTA_START:
    case CMD_HARMAN_OTA_PACKET:
    case CMD_HARMAN_OTA_STOP:
    case CMD_HARMAN_OTA_NOTIFICATION:
        {
            app_harman_ble_ota_handle(cmd_ptr, cmd_len, app_idx);
        }
        break;

    default:
        break;
    }

    if (p_cmd_rsp != NULL)
    {
        free(p_cmd_rsp);
        p_cmd_rsp = NULL;
    }
}

void app_harman_vendor_cmd_init(void)
{
    app_timer_reg_cb(vendor_harman_cmd_timer_callback, &vendor_harman_cmd_timer_queue_id);
    eq_p_cmd_data = os_mem_alloc(RAM_TYPE_DATA_ON, EQ_total_len);

    app_start_timer(&timer_handle_update_total_time, "harman_update_total_time",
                    vendor_harman_cmd_timer_queue_id, APP_TIMER_UPDATE_TOTAL_TIME,
                    0, false, HARMAN_UPDATE_TOTAL_TIME_INTERVAL);
    app_timer_register_pm_excluded(&timer_handle_update_total_time);
}
#endif
