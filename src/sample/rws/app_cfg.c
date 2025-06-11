/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "ftl.h"
#include "fmc_api.h"
#include "rtl876x.h"
#include "test_mode.h"
#include "eq_utils.h"
#include "gap_le.h"
#include "app_cfg.h"
#include "app_main.h"
#include "wdg.h"
#include "app_mmi.h"
#include "app_adp.h"
#include "app_dsp_cfg.h"
#include "app_charger.h"
#include "app_iphone_abs_vol_handle.h"
#include "app_multilink.h"
#include "app_bt_sniffing.h"
#include "app_lea_vol_def.h"

#if F_APP_LEA_SUPPORT
#include "multitopology_ctrl.h"
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "sd.h"
#endif

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif

#if F_APP_LLAPT_SCENARIO_CHOOSE_SUPPORT || F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_bt_policy_api.h"
#endif

#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#if F_APP_DUAL_AUDIO_EFFECT
#include "app_dual_audio_effect.h"
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_OTA_TOOLING_SUPPORT
#include "app_adp_cmd.h"
#endif

#if F_APP_ANC_SUPPORT
#include "app_bt_policy_int.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_CSIS_SUPPORT
#include "app_lea_csis.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
T_SD_CONFIG sd_card_cfg =
{
    .sd_if_type = SD_IF_SD_CARD,
    .sdh_bus_width = SD_BUS_WIDTH_4B,
    .sd_debug_en = 0,
};
#endif

static const T_APP_CFG_NV app_cfg_rw_default =
{
    .hdr.sync_word = DATA_SYNC_WORD,
    .hdr.length = sizeof(T_APP_CFG_NV),
    .single_tone_timeout_val = 20 * 1000, //20s
    .single_tone_tx_gain = 0,
    .single_tone_channel_num = 20,
    .anc_apt_state = 0,
    .apt_volume_out_level = 6,
    .app_pair_idx_mapping = {0, 1, 2, 3, 4, 5, 6, 7},
    .lea_vol_out_mute = 0,
#if F_APP_VCS_SUPPORT
    .lea_vcs_change_cnt = 0,
    .lea_vcs_vol_flag = 0,
#endif
};

T_APP_CFG_CONST app_cfg_const;
T_APP_CFG2_CONST app_cfg2_const;
T_EXTEND_APP_CFG_CONST extend_app_cfg_const;
T_APP_CFG_NV app_cfg_nv;

uint32_t app_cfg_reset(void)
{
    uint8_t temp_bd_addr[6];
    uint8_t temp_device_name_legacy[40];
    uint8_t temp_device_name_le[40];
    uint8_t temp_reset_power_on;
    uint8_t temp_reset_done;
    uint32_t temp_sync_word;
    uint8_t temp_power_off_cause_cmd = 0;
    uint8_t temp_le_single_random_addr[6] = {0};
    uint8_t temp_le_rws_random_addr[6] = {0};
    uint8_t temp_one_wire_aging_test_done = 0;
    uint8_t temp_one_wire_start_force_engage = 0;
    uint32_t temp_total_playback_time = 0;
    uint32_t temp_total_power_on_time = 0;

    //Keep for restore
    temp_reset_power_on = app_cfg_nv.adp_factory_reset_power_on;
    if (app_cfg_nv.adp_factory_reset_power_on && app_cfg_nv.power_off_cause_cmd)
    {
        temp_power_off_cause_cmd = app_cfg_nv.power_off_cause_cmd;
    }

    temp_reset_done = app_cfg_nv.factory_reset_done;
    temp_sync_word = app_cfg_nv.hdr.sync_word;
    temp_one_wire_aging_test_done = app_cfg_nv.one_wire_aging_test_done;
    temp_one_wire_start_force_engage = app_cfg_nv.one_wire_start_force_engage;

    memcpy(temp_bd_addr, app_cfg_nv.bud_peer_factory_addr, 6);
    memcpy(temp_device_name_legacy, app_cfg_nv.device_name_legacy, 40);
    memcpy(temp_device_name_le, app_cfg_nv.device_name_le, 40);
    memcpy(temp_le_single_random_addr, app_cfg_nv.le_single_random_addr, 6);
    memcpy(temp_le_rws_random_addr, app_cfg_nv.le_rws_random_addr, 6);

    temp_total_playback_time = app_cfg_nv.total_playback_time;
    temp_total_power_on_time = app_cfg_nv.total_power_on_time;

    memcpy(&app_cfg_nv, &app_cfg_rw_default, sizeof(T_APP_CFG_NV));

    app_cfg_nv.total_playback_time = temp_total_playback_time;
    app_cfg_nv.total_power_on_time = temp_total_power_on_time;

    memcpy(app_cfg_nv.le_single_random_addr, temp_le_single_random_addr, 6);
    memcpy(app_cfg_nv.le_rws_random_addr, temp_le_rws_random_addr, 6);

    app_cfg_nv.adp_factory_reset_power_on = temp_reset_power_on;
    if (app_cfg_nv.adp_factory_reset_power_on && temp_power_off_cause_cmd)
    {
        app_cfg_nv.power_off_cause_cmd = temp_power_off_cause_cmd;
    }

    app_cfg_nv.factory_reset_done = temp_reset_done;
    memcpy(app_cfg_nv.bud_peer_factory_addr, temp_bd_addr, 6);
    memcpy(app_cfg_nv.device_name_legacy, temp_device_name_legacy, 40);
    memcpy(app_cfg_nv.device_name_le, temp_device_name_le, 40);

    app_cfg_nv.one_wire_aging_test_done = temp_one_wire_aging_test_done;
    app_cfg_nv.one_wire_start_force_engage = temp_one_wire_start_force_engage;

#if F_APP_CSIS_SUPPORT
    app_lea_csis_gen_sirk();
#endif

    {
        uint8_t state_of_charge = app_charger_get_soc();

        //Charger module report 0xFF when ADC not ready
        if (state_of_charge <= 100)
        {
            app_cfg_nv.local_level = state_of_charge;
        }
    }

    if ((app_cfg_const.enable_not_reset_device_name == 0) ||
        (temp_sync_word != DATA_SYNC_WORD))  //First init
    {
#if F_APP_DURIAN_SUPPORT
        app_durian_cfg_rst_name();
#else
        memcpy(&app_cfg_nv.device_name_legacy[0], &app_cfg_const.device_name_legacy_default[0], 40);
#endif

        memcpy(&app_cfg_nv.device_name_le[0], &app_cfg_const.device_name_le_default[0], 40);

        // update device name when factory reset
        bt_local_name_set(&app_cfg_nv.device_name_legacy[0], GAP_DEVICE_NAME_LEN);
        le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, &app_cfg_nv.device_name_le[0]);

        if (temp_sync_word != DATA_SYNC_WORD)
        {
            if (app_cfg_const.enable_rtk_charging_box)
            {
                app_cfg_nv.case_battery = (app_cfg_nv.case_battery & 0x80) | 0x50;
            }

            // Power on is not allowed for the first time when vbat in after factory reset by mppgtool;
            // Unlock disallow power on mode by MFB power on or slide switch power on or bud starts chargring
            app_cfg_nv.disallow_power_on_when_vbat_in = 1;
        }
    }

    app_cfg_nv.bud_role = app_cfg_const.bud_role;
    app_cfg_nv.spatial_auto_cal = 0;
    app_cfg_nv.first_engaged = 0;
    memcpy(app_cfg_nv.bud_local_addr, app_db.factory_addr, 6);
    app_cfg_nv.app_is_power_on = 0;

#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_cfg_reset();
#endif

    app_cfg_nv.apt_volume_out_level = app_dsp_cfg_vol.apt_volume_out_default;

#if APT_SUB_VOLUME_LEVEL_SUPPORT
    app_apt_volume_update_sub_level();
#endif

    app_cfg_nv.voice_prompt_volume_out_level = app_dsp_cfg_vol.voice_prompt_volume_default;
    app_cfg_nv.ringtone_volume_out_level = app_dsp_cfg_vol.ringtone_volume_default;

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    app_cfg_nv.is_notification_vol_set_from_phone = 0;
#endif

    if (app_cfg_const.box_detect_method == ADAPTOR_DETECT)
    {
        if (app_adp_get_plug_state() == ADAPTOR_PLUG)
        {
            app_cfg_nv.adaptor_is_plugged = 1;
        }
        else
        {
            app_cfg_nv.adaptor_is_plugged = 0;
        }
    }
#if F_APP_GPIO_ONOFF_SUPPORT
    else if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        // 3pin location will get error if sync_word is not initialized
        if (app_device_is_in_the_box())
        {
            app_cfg_nv.pre_3pin_status_unplug = 0;
        }
        else
        {
            app_cfg_nv.pre_3pin_status_unplug = 1;
        }
    }
#endif

#if F_APP_IPHONE_ABS_VOL_HANDLE
    memset(&app_cfg_nv.abs_vol[0], iphone_abs_vol[app_dsp_cfg_vol.playback_volume_default], 8);
#endif

    memset(&app_cfg_nv.audio_gain_level[0], app_dsp_cfg_vol.playback_volume_default, 8);
    memset(&app_cfg_nv.voice_gain_level[0], app_dsp_cfg_vol.voice_out_volume_default, 8);
    memset(&app_cfg_nv.last_btmode[0], 0, 8);
    app_cfg_nv.line_in_gain_level = app_dsp_cfg_vol.line_in_volume_out_default;

#if HARMAN_VP_DATA_HEADER_GET_SUPPORT
    app_harman_vp_data_header_get();
#else
    app_cfg_nv.voice_prompt_language = app_cfg_const.voice_prompt_language;
#endif
    app_cfg_nv.light_sensor_enable = !(app_cfg_const.light_sensor_default_disabled);
    app_cfg_nv.eq_idx_normal_mode_record = eq_utils_get_default_idx(SPK_SW_EQ, NORMAL_MODE);
    app_cfg_nv.eq_idx_gaming_mode_record = eq_utils_get_default_idx(SPK_SW_EQ, GAMING_MODE);
    app_cfg_nv.eq_idx_anc_mode_record = eq_utils_get_default_idx(SPK_SW_EQ, ANC_MODE);
    app_cfg_nv.apt_eq_idx = eq_utils_get_default_idx(MIC_SW_EQ, APT_MODE);
#if F_APP_LINEIN_SUPPORT
    app_cfg_nv.eq_idx_line_in_mode_record = eq_utils_get_default_idx(SPK_SW_EQ, LINE_IN_MODE);
#endif

    app_cfg_nv.listening_mode_cycle = app_cfg_const.listening_mode_cycle;
    app_cfg_nv.listening_mode_cycle_ext = app_cfg_const.listening_mode_cycle_ext;
    app_cfg_nv.rws_low_latency_level_record = app_cfg_const.rws_low_latency_level;

#if F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT
    app_cfg_nv.rws_disallow_sync_apt_volume = app_cfg_const.rws_disallow_sync_apt_volume;
#endif

#if F_APP_POWER_ON_DELAY_APPLY_APT_SUPPORT
    app_cfg_nv.time_delay_to_open_apt_when_power_on =
        app_cfg_const.time_delay_to_open_apt_when_power_on;
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    app_cfg_nv.allowed_source = ALLOWED_SOURCE_DONGLE; //reset to dongle link
#endif

#if F_APP_DONGLE_MULTI_PAIRING
    app_cfg_nv.is_bt_pairing = 0;
#endif

#if F_APP_BRIGHTNESS_SUPPORT
    app_cfg_nv.main_brightness_level = brightness_level_default;
    app_apt_brightness_update_sub_level();
    app_cfg_nv.rws_disallow_sync_llapt_brightness = app_cfg_const.rws_disallow_sync_llapt_brightness;
#endif

#if GFPS_SASS_SUPPORT
    app_sass_policy_reset();
#endif

    app_cfg_nv.enable_multi_link = (app_multi_is_on_by_mmi() == true) ? 1 : 0;

    app_cfg_nv.either_bud_has_vol_ctrl_key = false;

    if (app_cfg_const.enable_enter_gaming_mode_after_power_on)
    {
        app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_gaming_mode_record;
    }
    else
    {
        app_cfg_nv.eq_idx = app_cfg_nv.eq_idx_normal_mode_record;
    }

#if F_APP_DUAL_AUDIO_EFFECT
    app_cfg_nv.audio_effect_normal_type = app_cfg_const.dual_audio_normal;
    app_cfg_nv.audio_effect_gaming_type = app_cfg_const.dual_audio_gaming;

    app_dual_audio_effect_reset();
    app_dual_audio_effect_set_force(dual_audio_mode_sel);

    for (uint8_t i = 0; i < 16; i++)
    {
        app_dual_audio_set_app_key_val(i, 0x7fff);
    }
    app_dual_audio_save_app_key_val();
#endif

#if F_APP_LEA_SUPPORT
    app_cfg_nv.lea_vol_setting = VOLUME_SETTING(app_dsp_cfg_vol.playback_volume_default);
    app_cfg_nv.lea_step_size = VOLUME_STEP_SIZE;
#endif

#if F_APP_SLIDE_SWITCH_POWER_ON_OFF
    app_slide_switch_power_on_off_gpio_status_reset();
#endif

#if F_APP_DURIAN_SUPPORT
    app_durian_cfg_factory_rst();
#endif

    app_cfg_nv.pin_code_size = app_cfg_const.pin_code_size;
    memcpy(&app_cfg_nv.pin_code[0], &app_cfg_const.pin_code[0], 8);

    app_cfg_nv.anc_selected_list = 0xFFFF;
    app_cfg_nv.llapt_selected_list = 0xFFFF;

#if F_APP_HARMAN_FEATURE_SUPPORT && (F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT)
    app_cfg_const.audio_latency = 150;
#endif

    app_cfg_nv.audio_latency = app_cfg_const.audio_latency;

#if F_APP_APT_SUPPORT
    if (app_cfg_const.normal_apt_support && app_cfg_const.llapt_support)
    {
        app_cfg_nv.apt_support_type = app_cfg_const.apt_support_type;
    }
    else
    {
        if (app_cfg_const.normal_apt_support)
        {
            app_cfg_nv.apt_support_type = APT_SUPPORT_TYPE_NORMAL_APT;
        }
        else if (app_cfg_const.llapt_support)
        {
            app_cfg_nv.apt_support_type = APT_SUPPORT_TYPE_LLAPT;
        }
    }
#endif

#if F_APP_RWS_BLE_USE_RPA_SUPPORT
    memset(app_cfg_nv.primary_irk, 0, 16);
    memset(app_cfg_nv.secondary_irk, 0, 16);
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_cfg_const.enable_dongle_dual_mode)
    {
#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
        app_cfg_nv.is_dual_mode = true;
#else
        if (app_cfg_const.enable_multi_link)
        {
            app_cfg_nv.is_dual_mode = true;
        }
        else
        {
            app_cfg_nv.is_dual_mode = false;
        }
#endif
    }
#endif

#if F_APP_LEA_SUPPORT
    if (app_cfg_const.iso_mode == LE_AUDIO_CIS || app_cfg_const.iso_mode == LE_AUDIO_ALL)
    {
        app_cfg_nv.lea_mode_enable = false;
    }
#endif
    DBG_DIRECT("app_cfg_reset: lea_mode_enable %d, iso_mode %d", app_cfg_nv.lea_mode_enable,
               app_cfg_const.iso_mode);

    return ftl_save_to_storage(&app_cfg_nv, APP_RW_DATA_ADDR, sizeof(T_APP_CFG_NV));
}

static void app_cfg_load(void)
{
    uint32_t sync_word = 0;

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        APP_CONFIG_OFFSET),
                       (uint8_t *)&sync_word, DATA_SYNC_WORD_LEN);

    if (sync_word == DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            APP_CONFIG_OFFSET),
                           (uint8_t *)&app_cfg_const, sizeof(T_APP_CFG_CONST));
    }
    else
    {
        chip_reset(RESET_ALL);
    }

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        APP_CONFIG2_OFFSET),
                       (uint8_t *)&sync_word, DATA_SYNC_WORD_LEN);
    if (sync_word == DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            APP_CONFIG2_OFFSET),
                           (uint8_t *)&app_cfg2_const, sizeof(T_APP_CFG2_CONST));
    }
    else
    {
        chip_reset(RESET_ALL);
    }

    app_dsp_cfg_init(app_cfg_const.normal_apt_support);

    //read-write data
    ftl_load_from_storage(&app_cfg_nv.hdr, APP_RW_DATA_ADDR, sizeof(app_cfg_nv.hdr));

    if (app_cfg_nv.hdr.sync_word != DATA_SYNC_WORD)
    {
        //Load factory reset bit first when mppgtool factory reset
        if (app_cfg_nv.hdr.length == 0)
        {
            ftl_load_from_storage(&app_cfg_nv.eq_idx_anc_mode_record, APP_RW_DATA_ADDR + FACTORY_RESET_OFFSET,
                                  4);
        }

        app_cfg_reset();
    }
    else
    {
        uint32_t load_len = app_cfg_nv.hdr.length;

        if (load_len > sizeof(T_APP_CFG_NV))
        {
            uint32_t res = ftl_load_from_storage(&app_cfg_nv, APP_RW_DATA_ADDR, sizeof(T_APP_CFG_NV));

            if (res == 0)
            {
                app_cfg_nv.hdr.length = sizeof(T_APP_CFG_NV);
            }
            else
            {
                APP_PRINT_ERROR0("app_cfg_load, error");
                app_cfg_reset();
            }
        }
        else
        {
            uint32_t res = ftl_load_from_storage(&app_cfg_nv, APP_RW_DATA_ADDR, load_len);

            if (res == 0)
            {
                if (load_len < sizeof(T_APP_CFG_NV))
                {
                    uint8_t *p_dst = ((uint8_t *)&app_cfg_nv) + load_len;
                    uint8_t *p_src = ((uint8_t *)&app_cfg_rw_default) + load_len;

                    memcpy(p_dst, p_src, sizeof(T_APP_CFG_NV) - load_len);
                }
                app_cfg_nv.hdr.length = sizeof(T_APP_CFG_NV);
            }
            else
            {
                app_cfg_reset();
            }
        }
    }

    if (app_cfg_const.mfb_replace_key0)
    {
        app_cfg_const.key_enable_mask &= 0xfe;
        app_cfg_const.key_pinmux[0] = 0xff;
    }

    if (is_single_tone_test_mode()) //DUT test mode
    {
        if (app_cfg_const.disable_key_when_dut_mode == 1)
        {
            app_cfg_const.key_gpio_support = 0;
            app_cfg_const.mfb_replace_key0 = 0; //Disable MFB key init
        }
    }

#if F_APP_ERWS_SUPPORT
    app_cfg_const.recovery_link_a2dp_interval = A2DP_INTERVAL;
    app_cfg_const.recovery_link_a2dp_rsvd_slot = A2DP_CTRL_RSVD_SLOT;

    app_cfg_const.recovery_link_a2dp_interval_gaming_mode =
        sniffing_params_low_latency_table[0][app_cfg_nv.rws_low_latency_level_record];
    app_cfg_const.recovery_link_a2dp_flush_timeout_gaming_mode =
        sniffing_params_low_latency_table[1][app_cfg_nv.rws_low_latency_level_record];
    app_cfg_const.recovery_link_a2dp_rsvd_slot_gaming_mode =
        sniffing_params_low_latency_table[2][app_cfg_nv.rws_low_latency_level_record];
#endif

    app_cfg_const.timer_link_avrcp = 1500;

#if F_APP_OTA_TOOLING_SUPPORT
    if (app_cfg_nv.ota_tooling_start)
    {
        app_cfg_const.box_detect_method = 0;
        // disable sensor
        app_cfg_const.sensor_support = 0;
        // disalbe auto power off
        app_cfg_const.timer_auto_power_off = 0xFFFF;

        app_cfg_const.enable_power_on_linkback = 0;
        app_cfg_const.enable_disconneted_enter_pairing = 0;

        app_cfg_const.normal_apt_support = 0;
    }

    if (app_cfg_const.enable_rtk_charging_box == 0)
    {
        // Since support rtk charging box is disabled
        // We should set those value.
        app_cfg_const.smart_charger_box_cmd_set = CHARGER_BOX_CMD_SET_15BITS;
        app_cfg_const.smart_charger_box_bit_length = 1; // 1: 20ms, 0 40ms
    }
#endif

#if F_APP_MULTI_LINK_ENABLE
    if ((app_cfg_const.enable_multi_link == 0) && (app_cfg_nv.enable_multi_link != 0))
    {
        app_cfg_const.enable_multi_link = 1;
#if F_APP_DURIAN_SUPPORT
        app_cfg_const.enter_pairing_while_only_one_device_connected = 1;
        app_cfg_const.timer_pairing_while_one_conn_timeout = 30;
#endif
    }

    if (app_cfg_const.enable_multi_link)
    {
        app_cfg_const.max_legacy_multilink_devices = 2;
        app_cfg_const.enable_always_discoverable = 0;
    }
#endif

#if F_APP_QOL_MONITOR_SUPPORT
    app_cfg_const.disable_link_monitor_roleswap = 0;
    app_cfg_const.roleswap_rssi_threshold = 8;
    app_cfg_const.rssi_roleswap_judge_timeout = 1;
    app_cfg_const.enable_high_batt_primary = 0;
#endif

#if F_APP_DUAL_AUDIO_EFFECT
    app_cfg_const.rws_disable_codec_mute_when_linkback = 1;
#endif

#if F_APP_ANC_SUPPORT
    T_ANC_RESP_MEAS_MODE meas_mode = anc_tool_check_resp_meas_mode();

    APP_PRINT_TRACE1("app_cfg_load anc_resp_meas_mode: %d", meas_mode);

    if (meas_mode != ANC_RESP_MEAS_MODE_NONE)
    {
        app_cfg_const.bud_role = REMOTE_SESSION_ROLE_SINGLE;
        app_cfg_nv.app_is_power_on = 1;
        app_cfg_const.enable_power_on_linkback = 1;
        app_cfg_const.link_scenario = LINKBACK_SCENARIO_SPP_BASE;
        app_cfg_const.listening_mode_power_on_status = POWER_ON_LISTENING_MODE_ALL_OFF;
        app_cfg_const.enable_pairing_timeout_to_power_off = 0;
        app_cfg_const.timer_pairing_timeout = 600;

        if (meas_mode == ANC_RESP_MEAS_MODE_ENTER)
        {
            app_cfg_const.supported_profile_mask = SPP_PROFILE_MASK;
        }
    }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT && F_APP_LEA_SUPPORT
    app_cfg_const.cis_pd = 45; //units: 100us

    /* for le audio gaming; now support conversation mode with downstream 48k */
    app_cfg_const.convo_downstream_sample_rate = 0x02; //PAC_CONVO_DSR_48K
#endif
}

static void app_cfg_load_extend(void)
{
    uint32_t sync_word = 0;

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        EXTEND_APP_CONFIG_OFFSET),
                       (uint8_t *)&sync_word, DATA_SYNC_WORD_LEN);
    if (sync_word == DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            EXTEND_APP_CONFIG_OFFSET),
                           (uint8_t *)&extend_app_cfg_const, sizeof(T_EXTEND_APP_CFG_CONST));
    }
    else
    {
        chip_reset(RESET_ALL);
    }

#if BISTO_FEATURE_SUPPORT
    extend_app_cfg_const.bisto_support = 1;
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
    extend_app_cfg_const.teams_support = 1;
#endif

#if F_APP_OTA_TOOLING_SUPPORT
    if (app_cfg_nv.ota_tooling_start)
    {
        extend_app_cfg_const.gfps_support = 0;
    }
#endif
}

bool app_cfg_load_led_table(void *p_data, uint8_t mode, uint16_t led_table_size)
{
    uint16_t sync_word = 0;

    fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                        APP_LED_OFFSET),
                       (uint8_t *)&sync_word, APP_DATA_SYNC_WORD_LEN);
    if (sync_word == APP_DATA_SYNC_WORD)
    {
        fmc_flash_nor_read((flash_cur_bank_img_payload_addr_get(FLASH_IMG_MCUCONFIG) +
                            APP_LED_OFFSET + APP_DATA_SYNC_WORD_LEN + (mode * led_table_size)),
                           (uint8_t *)p_data, led_table_size);
        return true;
    }
    else
    {
        return false;
    }
}

uint32_t app_cfg_store_all(void)
{
    return ftl_save_to_storage(&app_cfg_nv, APP_RW_DATA_ADDR, sizeof(T_APP_CFG_NV));
}

uint32_t app_cfg_store(void *pdata, uint16_t size)
{
    uint16_t offset = (uint8_t *)pdata - (uint8_t *)&app_cfg_nv;

    if ((offset % 4) != 0)
    {
        uint16_t old_offset = offset;

        pdata = (uint8_t *)pdata - (offset % 4);
        offset -= (offset % 4);
        size += (old_offset % 4);
    }

    if ((size % 4) != 0)
    {
        size = (size / 4 + 1) * 4;
    }

    APP_PRINT_TRACE2("app_cfg_store: off 0x%x size %d", offset, size);

    return ftl_save_to_storage(pdata, offset + APP_RW_DATA_ADDR, size);
}

void app_cfg_init(void)
{
    app_cfg_load();

#if F_APP_LOCAL_PLAYBACK_SUPPORT
    /*sdio_group_select : 2; //0:Disable, 1:SDIO_PinGroup_0, 2:SDIO_PinGroup_1*/
    if (app_cfg_const.sdio_group_select > 0)
    {
        app_cfg_const.sdio_group_select--;
        sd_card_cfg.sd_power_pin = app_cfg_const.sd_pwr_ctrl_pinmux;
        sd_card_cfg.sdh_group = app_cfg_const.sdio_group_select;
        sd_card_cfg.sd_power_high_active = app_cfg_const.sd_pwr_ctrl_active_level;
        sd_card_cfg.sd_power_en = app_cfg_const.sd_pwr_ctrl_en;
        sd_config_init((T_SD_CONFIG *)&sd_card_cfg);
    }
    else
    {
        APP_PRINT_WARN0("app_cfg_init SD mode disable");
    }
#endif

    app_cfg_load_extend();

#if F_APP_TTS_SUPPORT
    app_cfg_const.tts_support = 1;
#endif

#if F_APP_POWER_TEST
    app_cfg_const.key_table[0][APP_CALL_IDLE][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_VOICE_ACTIVATION_ONGOING][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_CALL_INCOMING][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_CALL_OUTGOING][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_CALL_ACTIVE][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_CALL_ACTIVE_WITH_CALL_WAITING][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_CALL_ACTIVE_WITH_CALL_HELD][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT][KEY6] = MMI_START_ROLESWAP;
    app_cfg_const.key_table[0][APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD][KEY6] = MMI_START_ROLESWAP;

    APP_PRINT_INFO2("app_cfg_init: IDLE Key6 -> %d, Call Active Key6 -> %d",
                    app_cfg_const.key_table[0][APP_CALL_IDLE][KEY6],
                    app_cfg_const.key_table[0][APP_CALL_ACTIVE][KEY6]);
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
    app_slide_switch_cfg_init();
#endif

#if HARMAN_VP_DATA_HEADER_GET_SUPPORT
    app_harman_vp_data_header_get();
#endif
}

