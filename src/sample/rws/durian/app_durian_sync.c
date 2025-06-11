/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <string.h>
#include "trace.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_relay.h"
#include "app_adp.h"
#include "app_cfg.h"
#include "app_report.h"

#if F_APP_TWO_GAIN_TABLE
#include "app_a2dp.h"
#endif

#if (F_APP_ERWS_SUPPORT == 1)
static uint8_t app_durian_sync_timer_id = 0;

#define DURIAN_BUD_SYNC_MS            100
#define DURIAN_ADV_START_MS           500

typedef enum
{
    APP_TIMER_DURIAN_RS_BUD_SYNC,
    APP_TIMER_DURIAN_RS_ADV_START,
} T_APP_DURIAN_ROLESWAP_TIMER;

static uint8_t timer_idx_durian_rs_bud_sync = 0;
static uint8_t timer_idx_durian_rs_adv_start = 0;

void app_durian_sync_fast_pair(void)
{
    if (durian_db.fast_pair_connected == 0)
    {
        durian_db.fast_pair_connected = 1;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_FAST_PAIR_CONNETED);
    }
}

void app_durian_sync_amplify_gain(uint8_t *bd_addr)
{
#if F_APP_TWO_GAIN_TABLE
    T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
    uint8_t a2dp_active_idx = app_a2dp_get_active_idx();

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) && (p_link != NULL) &&
        (p_link->id == a2dp_active_idx))
    {
        if (((p_link->connected_profile & AVP_PROFILE_MASK) != 0) && (p_link->audio_sharing == 0))
        {
            durian_db.amplify_gain = true;
        }
        else
        {
            durian_db.amplify_gain = false;
        }

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_AMPLIFY_GAIN,
                                            (uint8_t *)&durian_db.amplify_gain, 1);

        APP_PRINT_INFO5("app_durian_sync_amplify_gain: 0x%08x audio_sharing %d connected_profile 0x%08x amplify_gain %d a2dp_active_idx %d",
                        p_link, p_link->audio_sharing, p_link->connected_profile, durian_db.amplify_gain, a2dp_active_idx);
    }
#endif
}

void app_durian_sync_report_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len)
{
    if (durian_db.fast_pair_connected != 0)
    {
        app_report_event_broadcast(event_id, buf, len);
    }
}

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
void app_durian_sync_long_press(void)
{
#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_durian_key_long_press_action(false);
    }
    else
    {
        app_relay_async_single(APP_MODULE_TYPE_AVP,
                               SYNC_EVENT_DURIAN_ANC_MFB_EVENT);//inform pri to exec
    }
#endif
}
#endif

void app_durian_sync_one_key_trig(uint8_t remote_is_play)
{
    if (((remote_is_play == 1) && (durian_db.id_is_display == 0)) || ((remote_is_play == 0) &&
                                                                      (durian_db.id_is_display == 1)))
    {
        app_durian_cfg_switch_one_key_trig(ADP_PAYLOAD_AVP_OPEN);
    }

    APP_PRINT_INFO2("app_durian_sync_one_key_trig: remote_is_play %d id_is_display %d",
                    remote_is_play, durian_db.id_is_display);
}

static void app_durian_sync_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            T_BT_ROLESWAP_STATUS roleswap_status = param->remote_roleswap_status.status;
            APP_PRINT_INFO2("app_durian_sync_bt_cback: roleswap_status %d bud_role %d", roleswap_status,
                            app_cfg_nv.bud_role);

            if (roleswap_status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                app_start_timer(&timer_idx_durian_rs_adv_start, "durian_rs_adv_start",
                                app_durian_sync_timer_id, APP_TIMER_DURIAN_RS_ADV_START, 0, false,
                                DURIAN_ADV_START_MS);

                app_durian_avp_voice_recognition_check();

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
#if(F_APP_LISTENING_MODE_SUPPORT == 1)
                    app_durian_anc_report(durian_db.anc_cur_setting);
#endif
                    app_durian_loc_report(durian_db.local_loc, durian_db.remote_loc);
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->remote_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (app_db.device_state != APP_DEVICE_STATE_OFF)
                {
                    //sync msg later
                    app_start_timer(&timer_idx_durian_rs_bud_sync, "durian_rs_bud_sync",
                                    app_durian_sync_timer_id, APP_TIMER_DURIAN_RS_BUD_SYNC, 0, false,
                                    DURIAN_BUD_SYNC_MS);
                }

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_DEVICE_NAME_SYNC,
                                                        app_cfg_nv.device_name_legacy, strlen((char *)app_cfg_nv.device_name_legacy));
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_durian_roleswap_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_durian_sync_parse_cback(uint8_t event, uint8_t *buf, uint16_t len,
                                        T_REMOTE_RELAY_STATUS status)
{
    uint8_t para = *(uint8_t *)buf;
    APP_PRINT_TRACE4("app_durian_sync_parse_cback: event 0x%x para 0x%x len %d status %d",
                     event, para, len, status);

    if ((status != REMOTE_RELAY_STATUS_ASYNC_RCVD) && (status != REMOTE_RELAY_STATUS_SYNC_TOUT))
    {
        return;
    }

    if (event == SYNC_EVENT_DURIAN_EAR_DETECT_ENABLE)
    {
        durian_db.ear_detect_en = para;
    }
#if DURIAN_PRO2
    if (event == SYNC_EVENT_DURIAN_VOL_CTL_EN)
    {
        durian_db.vol_ctl_en = para;
    }
#endif
#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    else if (event == SYNC_EVENT_DURIAN_ANC_CUR_SETTING)
    {
        durian_db.anc_cur_setting = (T_AVP_ANC_SETTINGS)para;
        durian_db.anc_pre_setting = durian_db.anc_cur_setting;
    }
    else if (event == SYNC_EVENT_DURIAN_ANC_BOTH_SETTING)
    {
        durian_db.anc_both_setting = (T_AVP_ANC_SETTINGS)para;
    }
#if DURIAN_PRO2
    if (event == SYNC_EVENT_DURIAN_AUTO_APT_ENABLE)
    {
        durian_db.auto_apt_en = para;
    }
#endif
    else if (event == SYNC_EVENT_DURIAN_ANC_MFB_EVENT)
    {
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_durian_key_long_press_action(true);
        }
    }
    else if (event == SYNC_EVENT_DURIAN_ANC_CYCLE_SETTING)
    {
        durian_db.anc_cycle_setting = (T_AVP_CYCLE_SETTING_ANC)para;
    }
    else if (event == SYNC_EVENT_DURIAN_ANC_ONE_BUD_ENABLE)
    {
        durian_db.anc_one_bud_enabled = para;
    }
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    else if (event == SYNC_EVENT_DURIAN_ANC_ONE_SETTING)
    {
        durian_db.anc_one_setting = (T_AVP_ANC_SETTINGS)para;
    }
#endif
#endif
#if DURIAN_TWO
    else if (event == SYNC_EVENT_DURIAN_LOCAL_REMOTE_DOUBLE_CFG)
    {
        durian_db.remote_double_click_action = (para >> 4) & 0x0F;
        durian_db.local_double_click_action = para & 0x0F;
    }
#endif
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    else if (event == SYNC_EVENT_DURIAN_LOCAL_REMOTE_LONG_CFG)
    {
        durian_db.remote_long_action  = (para >> 4) & 0x0F;
        durian_db.local_long_action  = para & 0x0F;
    }
#endif
    else if (event == SYNC_EVENT_DURIAN_KEY_CLICK_SPEED)
    {
        durian_db.click_speed_rec = true;
        durian_db.click_speed = para;
        app_cfg_const.key_multi_click_interval = app_durian_key_multiclick_get(durian_db.click_speed);
    }
    else if (event == SYNC_EVENT_DURIAN_KEY_LONG_PRESS_TIME)
    {
        durian_db.long_press_time_rec = true;
        durian_db.long_press_time = para;
        app_cfg_const.key_long_press_interval = app_durian_key_long_press_get(durian_db.long_press_time);
    }
    else if (event == SYNC_EVENT_DURIAN_MIC_SETTING)
    {
        durian_db.mic_setting = (T_BT_AVP_MIC)para;
    }
    else if (event == SYNC_EVENT_DURIAN_ADV_PURPOSE)
    {
        durian_db.adv_purpose = para;
    }
    else if (event == SYNC_EVENT_DURIAN_REMOTE_SINGLE_ID)
    {
        if (len == sizeof(durian_cfg.avp_id.remote_single_id))
        {
            app_durian_cfg_remote_id_save(buf);
        }
    }
    else if (event == SYNC_EVENT_DURIAN_BUD_SYNC)
    {
        if (len == sizeof(T_DURIAN_SYNC_BUD))
        {
            T_DURIAN_SYNC_BUD *p_bud_sync;

            p_bud_sync = (T_DURIAN_SYNC_BUD *)buf;

#if DURIAN_TWO
            durian_db.local_double_click_action = p_bud_sync->remote_double_click_action;
            durian_db.remote_double_click_action = p_bud_sync->local_double_click_action;
#endif

            durian_db.click_speed_rec = true;
            durian_db.click_speed = p_bud_sync->click_speed;
            app_cfg_const.key_multi_click_interval = app_durian_key_multiclick_get(durian_db.click_speed);

            durian_db.long_press_time_rec = true;
            durian_db.long_press_time = p_bud_sync->long_press_time;
            app_cfg_const.key_long_press_interval = app_durian_key_long_press_get(durian_db.long_press_time);

            durian_db.mic_setting = (T_BT_AVP_MIC)p_bud_sync->mic_setting ;
            durian_db.ear_detect_en = p_bud_sync->ear_detect_en ;

            durian_db.remote_long_action = p_bud_sync->local_long_action;
            durian_db.local_long_action = p_bud_sync->remote_long_action;

#if F_APP_LISTENING_MODE_SUPPORT
            durian_db.anc_cycle_setting = (T_AVP_CYCLE_SETTING_ANC)p_bud_sync->anc_cycle_setting;
            durian_db.anc_one_bud_enabled = p_bud_sync->anc_one_bud_enabled;

            durian_db.anc_cur_setting = (T_AVP_ANC_SETTINGS)p_bud_sync->anc_cur_setting;
            durian_db.anc_pre_setting = durian_db.anc_cur_setting;
            durian_db.anc_one_setting = (T_AVP_ANC_SETTINGS)p_bud_sync->anc_one_setting;
            durian_db.anc_both_setting = (T_AVP_ANC_SETTINGS)p_bud_sync->anc_both_setting;
#if DURIAN_PRO2
            durian_db.auto_apt_en = p_bud_sync->auto_apt_en;
#endif
#endif

#if DURIAN_PRO2
            durian_db.vol_ctl_en = p_bud_sync->vol_ctl_en;
#endif

#if F_APP_TWO_GAIN_TABLE
            durian_db.amplify_gain = p_bud_sync->amplify_gain;
#endif
            durian_db.adv_purpose = p_bud_sync->adv_purpose;
        }
    }
    else if (event == SYNC_EVENT_DURIAN_SYNC_AVP_ID)
    {
        app_durian_cfg_save_load_avp_id(buf);
    }
#if (F_APP_ERWS_SUPPORT == 1)
    else if (event == SYNC_EVENT_DURIAN_SYNC_AVP_SINGLE_ID)
    {
        app_durian_cfg_save_load_single_id(buf);
    }
#endif
    else if (event == SYNC_EVENT_DURIAN_DISABLE_DISPLAY_AVP_ID)
    {
        app_cfg_nv.disable_report_avp_id = para;
    }
    else if (event == SYNC_EVENT_DURIAN_SYNC_ONE_KEY_TRIGGER)
    {
        app_durian_sync_one_key_trig(para);
    }
    else if (event == SYNC_EVENT_DURIAN_FAST_PAIR_CONNETED)
    {
        durian_db.fast_pair_connected = para;
    }
#if F_APP_TWO_GAIN_TABLE
    else if (event == SYNC_EVENT_DURIAN_AMPLIFY_GAIN)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            durian_db.amplify_gain = para;
            app_durian_audio_amplfy_vol_set();
        }
    }
#endif
    else
    {
        APP_PRINT_WARN1("app_durian_sync_parse_cback: Invalid roleswap, event 0x%x", event);
    }
}

static void app_durian_sync_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_durian_sync_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_DURIAN_RS_BUD_SYNC:
        {
            app_stop_timer(&timer_idx_durian_rs_bud_sync);

            durian_db.b2b_synced = true;
            T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();

            if (app_cfg_const.avp_enable_one_key_trigger != 0)
            {
                app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                                    SYNC_EVENT_DURIAN_SYNC_ONE_KEY_TRIGGER);
            }

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)//inform sec bud
            {
                app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                                    SYNC_EVENT_DURIAN_FAST_PAIR_CONNETED);
            }

            app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                                SYNC_EVENT_DURIAN_REMOTE_SINGLE_ID);
            app_relay_async_send(relay_msg_handle);

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)//inform sec bud
            {
                app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_BUD_SYNC);
            }
        }
        break;

    case APP_TIMER_DURIAN_RS_ADV_START:
        {
            app_stop_timer(&timer_idx_durian_rs_adv_start);
            app_durian_adv_roleswap();
        }
        break;

    default:
        break;
    }
}

uint16_t app_durian_sync_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    T_DURIAN_SYNC_BUD bud_sync = {0};
    uint8_t para;

    switch (msg_type)
    {
#if DURIAN_TWO
    case SYNC_EVENT_DURIAN_LOCAL_REMOTE_DOUBLE_CFG:
        {
            payload_len = 1;
            msg_ptr = &durian_db.local_remote_double_action;
        }
        break;
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    case SYNC_EVENT_DURIAN_LOCAL_REMOTE_LONG_CFG:
        {
            payload_len = 1;
            msg_ptr = &durian_db.local_remote_long_action;
        }
        break;
#endif

    case SYNC_EVENT_DURIAN_EAR_DETECT_ENABLE:
        {
            payload_len = 1;
            msg_ptr = &durian_db.ear_detect_en;
        }
        break;

    case SYNC_EVENT_DURIAN_MIC_SETTING:
        {
            payload_len = 1;
            msg_ptr = &durian_db.mic_setting;
        }
        break;

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    case SYNC_EVENT_DURIAN_ANC_MFB_EVENT:
        {
            payload_len = 0;
            msg_ptr = NULL;
        }
        break;

    case SYNC_EVENT_DURIAN_ANC_CYCLE_SETTING:
        {
            payload_len = 1;
            msg_ptr = &durian_db.anc_cycle_setting;
        }
        break;

    case SYNC_EVENT_DURIAN_ANC_ONE_BUD_ENABLE:
        {
            payload_len = 1;
            msg_ptr = &durian_db.anc_one_bud_enabled;
        }
        break;

    case SYNC_EVENT_DURIAN_ANC_CUR_SETTING:
        {
            payload_len = 1;
            msg_ptr = &durian_db.anc_cur_setting;
        }
        break;

    case SYNC_EVENT_DURIAN_ANC_ONE_SETTING:
        {
            payload_len = 1;
            msg_ptr = &durian_db.anc_one_setting;
        }
        break;

#if DURIAN_PRO2
    case SYNC_EVENT_DURIAN_AUTO_APT_ENABLE:
        {
            payload_len = 1;
            msg_ptr = &durian_db.auto_apt_en;
        }
        break;

    case SYNC_EVENT_DURIAN_VOL_CTL_EN:
        {
            payload_len = 1;
            msg_ptr = &durian_db.vol_ctl_en;
        }
        break;
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    case SYNC_EVENT_DURIAN_ANC_BOTH_SETTING:
        {
            payload_len = 1;
            msg_ptr = &durian_db.anc_both_setting;
        }
        break;
#endif
#endif

    case SYNC_EVENT_DURIAN_KEY_CLICK_SPEED:
        {
            payload_len = 1;
            msg_ptr = &durian_db.click_speed;
        }
        break;

    case SYNC_EVENT_DURIAN_KEY_LONG_PRESS_TIME:
        {
            payload_len = 1;
            msg_ptr = &durian_db.long_press_time;
        }
        break;

    case SYNC_EVENT_DURIAN_ADV_PURPOSE:
        {
            payload_len = 1;
            msg_ptr = &durian_db.adv_purpose;
        }
        break;

    case SYNC_EVENT_DURIAN_REMOTE_SINGLE_ID:
        {
            payload_len = sizeof(durian_cfg.avp_id.single_id);
            msg_ptr = durian_cfg.avp_id.single_id;
        }
        break;

    case SYNC_EVENT_DURIAN_BUD_SYNC:
        {
#if DURIAN_TWO
            bud_sync.remote_double_click_action = durian_db.remote_double_click_action;
            bud_sync.local_double_click_action = durian_db.local_double_click_action;
#endif

            if (durian_db.click_speed_rec)
            {
                bud_sync.click_speed = durian_db.click_speed;

            }
            if (durian_db.long_press_time_rec)
            {
                bud_sync.long_press_time = durian_db.long_press_time;
            }

            bud_sync.mic_setting = durian_db.mic_setting;
            bud_sync.ear_detect_en = durian_db.ear_detect_en;
            bud_sync.local_long_action = durian_db.local_long_action;
            bud_sync.remote_long_action = durian_db.remote_long_action;

#if F_APP_LISTENING_MODE_SUPPORT
            bud_sync.anc_cycle_setting = durian_db.anc_cycle_setting;
            bud_sync.anc_one_bud_enabled = durian_db.anc_one_bud_enabled;
            bud_sync.anc_cur_setting = durian_db.anc_cur_setting;
            bud_sync.anc_one_setting = durian_db.anc_one_setting;
            bud_sync.anc_both_setting = durian_db.anc_both_setting;
#if DURIAN_PRO2
            bud_sync.auto_apt_en = durian_db.auto_apt_en;
#endif
#endif

#if DURIAN_PRO2
            bud_sync.vol_ctl_en = durian_db.vol_ctl_en;
#endif

#if F_APP_TWO_GAIN_TABLE
            bud_sync.amplify_gain = durian_db.amplify_gain;
#endif
            bud_sync.adv_purpose = durian_db.adv_purpose;

            payload_len = sizeof(T_DURIAN_SYNC_BUD);
            msg_ptr = (uint8_t *)&bud_sync;
        }
        break;

    case SYNC_EVENT_DURIAN_SYNC_ONE_KEY_TRIGGER:
        {
            payload_len = 1;
            msg_ptr = &durian_db.id_is_display;
        }
        break;

    case SYNC_EVENT_DURIAN_FAST_PAIR_CONNETED:
        {
            para = durian_db.fast_pair_connected;
            payload_len = 1;
            msg_ptr = &para;
        }
        break;

    default:
        break;

    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_AVP, payload_len, msg_ptr, skip, total);
}

void app_durian_sync_init(void)
{
    bt_mgr_cback_register(app_durian_sync_bt_cback);
    app_timer_reg_cb(app_durian_sync_timeout_cb, &app_durian_sync_timer_id);
    app_relay_cback_register(app_durian_sync_relay_cback, app_durian_sync_parse_cback,
                             APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_MAX);
}
#endif
#endif
