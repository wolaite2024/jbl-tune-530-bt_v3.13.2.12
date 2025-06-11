/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <string.h>
#include "trace.h"
#include "ftl.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_sdp.h"
#include "app_adp.h"
#include "app_adp_cmd.h"

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#define AIRMAX_DAC_GAIN_LEVEL_MAX         99
#define AIRMAX_DAC_GAIN_LEVEL_DEFAULT     60

T_DURIAN durian_cfg = {0};

#if DURIAN_FOUR|DURIAN_TWO
uint8_t durian_legacy_name[7] = {'a', 'i', 'r', 'p', 'o', 'd', 's'};
#endif

#if DURIAN_PRO
uint8_t durian_legacy_name[11] = {'a', 'i', 'r', 'p', 'o', 'd', 's', ' ', 'p', 'r', 'o'};
#endif

#if DURIAN_PRO2
uint8_t durian_legacy_name[11] = {'a', 'i', 'r', 'p', 'o', 'd', 's', ' ', 'p', 'r', 'o'};
#endif

#if DURIAN_AIRMAX
uint8_t durian_legacy_name[6] = {'a', 'i', 'r', 'm', 'a', 'x'};
#endif

#if DURIAN_BEATS
uint8_t durian_legacy_name[6] = {'B', 'e', 'a', 't', 's'};
#endif

#if DURIAN_BEATS_FIT
uint8_t durian_legacy_name[13] = {'B', 'e', 'a', 't', 's', ' ', 'F', 'i', 't', ' ', 'P', 'r', 'o'};
#endif

#if DURIAN_TWO
#define VERSION_TWO_5B58                  1
#endif

#if DURIAN_FOUR
#define VERSION_FOUR_5B58                 1
#endif

#if DURIAN_PRO
#define VERSION_4E71                      1
#endif

#if DURIAN_PRO2
#define VERSION_5A377                     1
#endif

#if DURIAN_BEATS_FIT
#define VERSION_4E64                      1
#endif


static bool multilink_on_cause_cmd = false;

void app_durian_cfg_multi_on(void)
{
    multilink_on_cause_cmd = true;
}

bool app_durian_cfg_multi_is_on(void)
{
    return multilink_on_cause_cmd;
}

uint32_t app_durian_cfg_id_load_data(void *p_data)
{
    return ftl_load_from_storage(p_data, APP_RW_AVP_ID_DATA_ADDR, APP_RW_AVP_ID_DATA_SIZE);
}

uint32_t app_durian_cfg_id_save_data(void *p_data)
{
    return ftl_save_to_storage(p_data, APP_RW_AVP_ID_DATA_ADDR, APP_RW_AVP_ID_DATA_SIZE);
}

void app_durian_cfg_save_load_avp_id(void *buf)
{
    memset(durian_cfg.avp_id.id, 0, sizeof(durian_cfg.avp_id.id));
    memcpy(durian_cfg.avp_id.id, (uint8_t *)buf, sizeof(durian_cfg.avp_id.id));
    app_durian_cfg_id_save_data(&durian_cfg.avp_id);
    durian_avp_id_load(&durian_cfg.avp_id);
}

void app_durian_cfg_set_avp_id(void *buf)
{
    app_durian_cfg_save_load_avp_id(buf);

#if (F_APP_ERWS_SUPPORT == 1)
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_SYNC_AVP_ID, buf,
                                        sizeof(durian_cfg.avp_id.id));
#endif
}

#if (F_APP_ERWS_SUPPORT == 1)
void app_durian_cfg_save_load_single_id(void *buf)
{
    uint8_t *p_info = (uint8_t *)buf;

    memset(durian_cfg.avp_id.single_id, 0, sizeof(durian_cfg.avp_id.single_id));
    memset(durian_cfg.avp_id.remote_single_id, 0, sizeof(durian_cfg.avp_id.remote_single_id));

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
    {
        memcpy(durian_cfg.avp_id.single_id, &p_info[12], 12);
        memcpy(durian_cfg.avp_id.remote_single_id, &p_info[0], 12);
    }
    else
    {
        memcpy(durian_cfg.avp_id.single_id, &p_info[0], 12);
        memcpy(durian_cfg.avp_id.remote_single_id, &p_info[12], 12);
    }
    app_durian_cfg_id_save_data(&durian_cfg.avp_id);
    durian_avp_id_load(&durian_cfg.avp_id);
}

void app_durian_cfg_set_single_id(void *buf)
{
    app_durian_cfg_save_load_single_id(buf);
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_SYNC_AVP_SINGLE_ID,
                                        buf, 24);
}
#endif

void app_durian_cfg_factory_rst(void)
{
    app_adp_cmd_case_bat_check(&app_db.case_battery, &app_cfg_nv.case_battery);
    app_cfg_nv.local_level = app_db.local_batt_level;
    app_cfg_nv.remote_level = app_db.remote_batt_level;
    app_cfg_nv.local_loc = durian_db.local_loc;
    app_cfg_nv.remote_loc = durian_db.remote_loc;

    app_cfg_nv.anc_cur_setting = AVP_ANC_CLOSE;
    app_cfg_nv.anc_one_setting = AVP_ANC_TRANSPARENCY;
    app_cfg_nv.anc_both_setting = AVP_ANC_OPEN;

    app_cfg_nv.anc_one_bud_enabled = 0;
    app_cfg_nv.anc_cycle_setting = AVP_ANC_CYCLE_OPEN_TRANSPARENCY;

    app_cfg_nv.ear_detect_en = 1;
    app_cfg_nv.mic_setting = BT_AVP_MIC_AUTO;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
    app_cfg_nv.dual_audio_space = SPATIAL_AUDIO_OPEN;
#endif

#if DURIAN_TWO
    app_cfg_nv.local_remote_double_action = (BT_AVP_CONTROL_FORWARD << 4) | BT_AVP_CONTROL_FORWARD;
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    app_cfg_nv.local_remote_long_action = (BT_AVP_CONTROL_ANC << 4) | BT_AVP_CONTROL_ANC;
#endif

    if (app_cfg_const.avp_enable_one_key_trigger != 0)
    {
        app_cfg_nv.id_is_display = durian_db.id_is_display;
    }

    APP_PRINT_INFO6("app_avp_factory_reset_save_cfg: local_batt_level %d remote_batt_level local_loc %d remote_loc %d id_is_display %d id_is_display_nv %d",
                    app_db.local_batt_level, app_db.remote_batt_level, durian_db.local_loc, durian_db.remote_loc,
                    durian_db.id_is_display, app_cfg_nv.id_is_display);
}

void app_durian_cfg_switch_one_key_trig(uint8_t payload)
{
    if ((payload != ADP_PAYLOAD_AVP_OPEN) && (payload != ADP_PAYLOAD_AVP_CLOSE))
    {
        return;
    }

    if (app_cfg_const.avp_enable_one_key_trigger == 0)
    {
        return;
    }

    memset(&app_cfg_nv.device_name_legacy[0], 0, sizeof(app_cfg_nv.device_name_legacy));

    if (payload == ADP_PAYLOAD_AVP_OPEN)
    {
        durian_db.id_is_display = 1;
        memcpy(&app_cfg_nv.device_name_legacy[0], durian_legacy_name, sizeof(durian_legacy_name));
    }
    else
    {
        durian_db.id_is_display = 0;
        memcpy(&app_cfg_nv.device_name_legacy[0], &app_cfg_const.device_name_legacy_default[0],
               GAP_DEVICE_NAME_LEN);
    }

    app_cfg_store(app_cfg_nv.device_name_legacy, GAP_DEVICE_NAME_LEN);
    app_cfg_nv.id_is_display = durian_db.id_is_display;

    app_durian_adv_purpose_set_none();
    app_adp_cmd_factory_reset_link_dis(300);
    APP_PRINT_TRACE4("app_avp_switch_adv_id: payload %d id_is_display %d role_decided %d device_name_legacy %s",
                     payload, app_cfg_nv.id_is_display, durian_db.role_decided,
                     TRACE_STRING(app_cfg_nv.device_name_legacy));
}

void app_durian_cfg_rst_name(void)
{
    memset(&app_cfg_nv.device_name_legacy[0], 0, sizeof(app_cfg_nv.device_name_legacy));

    if (app_cfg_const.avp_enable_one_key_trigger != 0)
    {
        if (app_cfg_nv.factory_reset_done == 0)
        {
            durian_db.id_is_display = 0;
            app_cfg_nv.id_is_display = durian_db.id_is_display;
        }

        if (durian_db.id_is_display != 0)
        {
            memcpy(&app_cfg_nv.device_name_legacy[0], durian_legacy_name, sizeof(durian_legacy_name));
        }
        else
        {
            memcpy(&app_cfg_nv.device_name_legacy[0], &app_cfg_const.device_name_legacy_default[0],
                   GAP_DEVICE_NAME_LEN);
        }
    }
    else
    {
        memcpy(&app_cfg_nv.device_name_legacy[0], &app_cfg_const.device_name_legacy_default[0],
               GAP_DEVICE_NAME_LEN);
    }

    APP_PRINT_TRACE5("app_avp_factory_rst_name: factory_reset_done %d id_is_display_nv %d id_is_display %d	avp_enable_one_key_trigger %d device_name_legacy %s",
                     app_cfg_nv.factory_reset_done, app_cfg_nv.id_is_display, durian_db.id_is_display,
                     app_cfg_const.avp_enable_one_key_trigger, TRACE_STRING(app_cfg_nv.device_name_legacy));
}

static bool app_durian_cfg_one_trigger_need(void)
{
    bool need_avp = false;
    if (app_cfg_const.avp_enable_one_key_trigger != 0)
    {
        if (app_cfg_nv.id_is_display != 0)
        {
            need_avp = true;
        }
    }
    else
    {
        need_avp = true;
    }

    return need_avp;
}

static void app_durian_cfg_check(void)
{
    durian_cfg.one_trig_need = app_durian_cfg_one_trigger_need();
    durian_cfg.is_left = (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? true : false;

#if DURIAN_ONE
    durian_cfg.gen = DURIAN_GEN_ONE;
#endif

#if DURIAN_TWO
    durian_cfg.gen = DURIAN_GEN_TWO;
#endif

#if DURIAN_PRO
    durian_cfg.gen = DURIAN_GEN_PRO;
#endif

#if DURIAN_PRO2
    durian_cfg.gen = DURIAN_GEN_PRO2;
#endif

#if DURIAN_FOUR
    durian_cfg.gen = DURIAN_GEN_FOUR;
#endif

#if DURIAN_AIRMAX
    durian_cfg.gen = DURIAN_GEN_MAX;
#endif

#if DURIAN_BEATS
    durian_cfg.gen = DURIAN_GEN_BEATS;
#endif

#if DURIAN_BEATS_FIT
    durian_cfg.gen = DURIAN_GEN_BEATS_FIT;
#endif

#if DURIAN_BEATS_FIT
    durian_cfg.color = DURIAN_COLOR_BLACK;
#else
    durian_cfg.color = DURIAN_COLOR_WHITE;
#endif

#if (VERSION_MAX == 1)
    durian_cfg.version = DURIAN_VER_MAX;
#endif

#if (VERSION_4E71 == 1)
    durian_cfg.version = DURIAN_VER_4E71;
#endif

#if (VERSION_4E64 == 1)
    durian_cfg.version = DURIAN_VER_4E64;
#endif

#if (VERSION_5A377 == 1)
    durian_cfg.version = DURIAN_VER_5A377;
#endif

#if (VERSION_TWO_5B58 == 1)
    durian_cfg.version = DURIAN_VER_TWO_5B58;
#endif

#if (VERSION_FOUR_5B58 == 1)
    durian_cfg.version = DURIAN_VER_FOUR_5B58;
#endif

    if ((app_cfg_const.disable_report_avp_id == 0) && (app_cfg_nv.disable_report_avp_id == 0))
    {
        if (durian_cfg.one_trig_need)
        {
            durian_cfg.display_id = true;
        }
    }

    if (durian.gen == DURIAN_GEN_TWO)
    {
        if (app_cfg_const.avp_display_spatial_audio == 1)
        {
            durian_cfg.display_spatial = true;
        }
    }
    else
    {
        durian_cfg.display_spatial = true;
    }

    durian_cfg.legacy_name = app_cfg_nv.device_name_legacy;

    app_durian_cfg_avp_id_load();

    APP_PRINT_TRACE1("app_durian_cfg_check gen %d", durian.gen);
}

void app_durian_cfg_remote_id_save(uint8_t *buf)
{
    memcpy(durian_cfg.avp_id.remote_single_id, buf, sizeof(durian_cfg.avp_id.remote_single_id));
    app_durian_cfg_id_save_data(&durian_cfg.avp_id);
    durian_avp_remote_id_set(durian_cfg.avp_id.remote_single_id);
}

void app_durian_cfg_avp_id_load(void)
{
    const uint8_t zero_array[12] = {0};
    bool need_save = false;

    memset(&durian_cfg.avp_id, 0, sizeof(durian_cfg.avp_id));

    app_durian_cfg_id_load_data(&durian_cfg.avp_id);

    if (memcmp(durian_cfg.avp_id.id, zero_array, sizeof(zero_array)) == 0)
    {
        memcpy(durian_cfg.avp_id.id, app_cfg_const.avp_id, sizeof(durian_cfg.avp_id.id));
        need_save = true;
    }

    if (memcmp(durian_cfg.avp_id.single_id, zero_array, sizeof(zero_array)) == 0)
    {
        memcpy(durian_cfg.avp_id.single_id, app_cfg_const.avp_single_id,
               sizeof(durian_cfg.avp_id.single_id));
        need_save = true;
    }

    if (need_save)
    {
        app_durian_cfg_id_save_data(&durian_cfg.avp_id);
    }
}

void app_durian_cfg_power_on(void)
{
#if (F_APP_ERWS_SUPPORT == 1)
    app_adp_cmd_case_bat_check(&app_cfg_nv.case_battery, &app_db.case_battery);
    app_db.local_batt_level = app_cfg_nv.local_level;
    app_db.remote_batt_level = app_cfg_nv.remote_level;
    if (durian_db.local_loc == BUD_LOC_UNKNOWN)
    {
        durian_db.local_loc = app_cfg_nv.local_loc;
    }
    durian_db.remote_loc = app_cfg_nv.remote_loc;
#else
    app_db.local_batt_level = app_cfg_nv.local_level;

    durian_db.local_loc = (app_db.local_loc == BUD_LOC_UNKNOWN) ? app_cfg_nv.local_loc :
                          app_db.local_loc;
    if (app_cfg_const.sensor_support == 0)
    {
        durian_db.local_loc = (durian_db.local_loc == BUD_LOC_IN_CASE) ? BUD_LOC_IN_CASE : BUD_LOC_IN_EAR;
    }

    durian_db.remote_loc = BUD_LOC_IN_EAR;
    app_avp_bud_changed_single();
#endif

#if F_APP_LISTENING_MODE_SUPPORT
    durian_db.anc_cur_setting = (T_AVP_ANC_SETTINGS)app_cfg_nv.anc_cur_setting;
    durian_db.anc_one_setting = (T_AVP_ANC_SETTINGS)app_cfg_nv.anc_one_setting;
    durian_db.anc_both_setting = (T_AVP_ANC_SETTINGS)app_cfg_nv.anc_both_setting;
    durian_db.anc_pre_setting = AVP_ANC_NULL;
    durian_db.anc_one_bud_enabled = app_cfg_nv.anc_one_bud_enabled;
    durian_db.anc_cycle_setting = (T_AVP_CYCLE_SETTING_ANC)app_cfg_nv.anc_cycle_setting;
#endif

    durian_db.ear_detect_en = app_cfg_nv.ear_detect_en;
    durian_db.mic_setting = (T_BT_AVP_MIC)app_cfg_nv.mic_setting;

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    durian_db.local_remote_long_action = app_cfg_nv.local_remote_long_action;
#endif

#if DURIAN_TWO
    durian_db.local_remote_double_action = app_cfg_nv.local_remote_double_action;
#endif

    if (app_cfg_const.avp_enable_one_key_trigger != 0)
    {
        durian_db.id_is_display = app_cfg_nv.id_is_display;
    }

    durian_db.role_decided = 0;
}

void app_durian_cfg_power_off(void)
{
#if (F_APP_ERWS_SUPPORT == 1)
    /*save batt cfg in in_case/out_case handler*/
    app_adp_cmd_case_bat_check(&app_db.case_battery, &app_cfg_nv.case_battery);
    app_cfg_nv.local_level = app_db.local_batt_level;
    app_cfg_nv.remote_level = app_db.remote_batt_level;
    app_cfg_nv.local_loc = durian_db.local_loc;
    app_cfg_nv.remote_loc = durian_db.remote_loc;
#else
    app_cfg_nv.local_level = app_db.local_batt_level;
    app_cfg_nv.local_loc = durian_db.local_loc;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
    app_cfg_nv.anc_cur_setting = durian_db.anc_cur_setting;
    app_cfg_nv.anc_one_setting = durian_db.anc_one_setting;
    app_cfg_nv.anc_both_setting = durian_db.anc_both_setting;
    app_cfg_nv.anc_one_bud_enabled = durian_db.anc_one_bud_enabled;
    app_cfg_nv.anc_cycle_setting = durian_db.anc_cycle_setting;
#endif

    app_cfg_nv.ear_detect_en = durian_db.ear_detect_en;
    app_cfg_nv.mic_setting = durian_db.mic_setting;

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    app_cfg_nv.local_remote_long_action = durian_db.local_remote_long_action;
#endif

#if DURIAN_TWO
    app_cfg_nv.local_remote_double_action = durian_db.local_remote_double_action;
#endif

    durian_db.role_decided = 0;
    multilink_on_cause_cmd = false;
}

void app_durian_cfg_set_name(T_BT_AVP_EVENT_SET_NAME *set_name)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(set_name->bd_addr);
    if (p_link != NULL)
    {
        if (set_name->len < 40)
        {
            memset(&app_cfg_nv.device_name_legacy[0], 0, sizeof(app_cfg_nv.device_name_legacy));
            memcpy(&app_cfg_nv.device_name_legacy[0], set_name->data, set_name->len);
            bt_local_name_set(set_name->data, set_name->len);

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_DEVICE_NAME_SYNC,
                                                (uint8_t *)set_name->data, set_name->len);
        }
        else
        {
            memcpy(&app_cfg_nv.device_name_legacy[0], set_name->data, 40);
            bt_local_name_set(set_name->data, 40);

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_DEVICE_NAME_SYNC,
                                                (uint8_t *)set_name->data, set_name->len);
        }

        app_cfg_store(app_cfg_nv.device_name_legacy, 40);
    }
}

static void app_durian_cfg_para_init(void)
{
    app_cfg_const.key_short_press_tone_mask = 0;
    app_cfg_const.listening_mode_does_not_depend_on_in_ear_status = 1;
    app_cfg_const.disallow_charging_led_before_power_off = 1;
    app_cfg_const.enable_power_off_immediately_when_close_case = 0;

#if DURIAN_AIRMAX
    app_dsp_cfg_vol.playback_volume_max = AIRMAX_DAC_GAIN_LEVEL_MAX;
    app_dsp_cfg_vol.playback_volume_min = 0;
    app_dsp_cfg_vol.playback_volume_default = AIRMAX_DAC_GAIN_LEVEL_DEFAULT;
#endif
}

void app_durian_cfg_init(void)
{
    memset(&durian_db, 0, sizeof(durian_db));
    app_durian_cfg_check();
    durian_init(&durian_cfg);

    durian_aap_record = durian_cfg.record.aap;
    durian_sdp_record = durian_cfg.record.sdp;
    durian_did_record = durian_cfg.record.did;

    durian_db.click_speed_origin = app_cfg_const.key_multi_click_interval;
    durian_db.long_press_origin = app_cfg_const.key_long_press_interval;

    app_db.remote_bat_adv = 0x7f;

    app_durian_cfg_para_init();
}

#endif

