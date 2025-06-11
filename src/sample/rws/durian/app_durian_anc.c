/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_hfp.h"
#include "app_audio_policy.h"
#include "app_report.h"

#if F_APP_LISTENING_MODE_SUPPORT
typedef struct
{
    uint8_t len;
    const uint8_t *loop;
} T_ANC_LOOP;

static const uint8_t click_setting_anc_index[8] = {0, 0, 0, 1, 0, 2, 3, 4};
static const uint8_t anc_action_open_close_loop[2] = {AVP_ANC_OPEN, AVP_ANC_CLOSE};
static const uint8_t anc_action_transparentcy_close_loop[2] = {AVP_ANC_TRANSPARENCY, AVP_ANC_CLOSE};
static const uint8_t anc_action_open_transparentcy_loop[2] = {AVP_ANC_OPEN, AVP_ANC_TRANSPARENCY};
static const uint8_t anc_action_open_transparentcy_close_loop[3] = {AVP_ANC_OPEN, AVP_ANC_TRANSPARENCY, AVP_ANC_CLOSE};
static const T_MMI_ACTION  anc_mmi_map[4] = {MMI_NULL, MMI_AUDIO_ANC_APT_ALL_OFF, MMI_AUDIO_ANC_ON, MMI_AUDIO_APT_ON};

static const T_ANC_LOOP anc_loop[4] =
{
    {2, anc_action_open_close_loop},
    {2, anc_action_transparentcy_close_loop},
    {2, anc_action_open_transparentcy_loop},
    {3, anc_action_open_transparentcy_close_loop},
};

void app_durian_anc_mode_switch(uint8_t mode)
{
    T_ANC_APT_EVENT event = EVENT_ALL_OFF;

    bool need_tone = (durian_db.anc_apt_need_tone != 0) ? true : false;

    if (mode == ANC_MODE_ALL_OFF)
    {
        event = EVENT_ALL_OFF;
    }
    else if (mode == ANC_MODE_ANC_ON)
    {
        event = EVENT_ANC_ON_SCENARIO_1;
    }
    else if (mode == ANC_MODE_APT_ON)
    {
        if (app_cfg_const.normal_apt_support)
        {
            event = EVENT_NORMAL_APT_ON;
        }
        else if (app_cfg_const.llapt_support)
        {
#if DURIAN_PRO2
            if (durian_db.auto_apt_en != 0)
            {
                event = EVENT_LLAPT_ON_SCENARIO_2;
            }
            else
#endif
            {
                event = EVENT_LLAPT_ON_SCENARIO_1;
            }
        }
    }

    app_listening_state_machine(event, need_tone, true);
}

void app_durian_anc_report(T_AVP_ANC_SETTINGS set)
{
    T_BT_AVP_ANC bt_set;

    bt_set = (T_BT_AVP_ANC)set;
    T_APP_BR_LINK *p_link;
    uint8_t *bd_addr;

    APP_PRINT_TRACE1("app_durian_anc_report: bt anc set %d", bt_set);

    app_durian_sync_report_event_broadcast(EVENT_AVP_ANC_SET, &bt_set, sizeof(bt_set));

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & AVP_PROFILE_MASK)
        {
            bd_addr = app_db.br_link[i].bd_addr;
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                bt_avp_anc_setting_report(bd_addr, bt_set);
                APP_PRINT_TRACE0("app_durian_anc_report: reported");
            }
        }
    }
}

static void app_durian_anc_trig(T_AVP_ANC_SETTINGS anc_mode)
{
    T_MMI_ACTION action = anc_mmi_map[anc_mode];

    app_relay_async_single(APP_MODULE_TYPE_MMI, action);
    app_mmi_handle_action(action);
}

static void app_durian_anc_action(bool need_tone)
{
    APP_PRINT_TRACE3("app_durian_anc_action: anc cur setting %d anc pre setting %d,need_tone %d",
                     durian_db.anc_cur_setting, durian_db.anc_pre_setting, need_tone);

    if (durian_db.anc_cur_setting != durian_db.anc_pre_setting)
    {
        durian_db.anc_pre_setting = durian_db.anc_cur_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_CUR_SETTING);
        if ((durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY) &&
            (app_cfg_const.normal_apt_support == 1) &&
            app_link_get_misc_num(APP_LINK_OPUS) > 0)
        {
            durian_db.apt_off_cause_siri = true;
            APP_PRINT_TRACE0("app_durian_anc_action: siri started,ignore apt on");
        }
        else if ((durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY) &&
                 (app_cfg_const.normal_apt_support == 1) &&
                 (app_hfp_get_call_status() != APP_CALL_IDLE))
        {
            durian_db.apt_off_cause_call = true;
            APP_PRINT_TRACE0("app_durian_anc_action: call active,ignore apt on");
        }
        else
        {
            durian_db.apt_off_cause_siri = false;
            durian_db.apt_off_cause_call = false;
            durian_db.anc_apt_need_tone = need_tone;
            app_durian_anc_trig(durian_db.anc_cur_setting);
        }

        if (durian_db.apt_off_cause_siri || durian_db.apt_off_cause_call)
        {
            if (durian_db.local_loc != BUD_LOC_IN_CASE)
            {
                if (need_tone)
                {
                    app_audio_tone_type_play(TONE_APT_ON, false, true);//ignore action, but play here
                }
            }
        }
        app_durian_anc_report(durian_db.anc_cur_setting);
    }
}

void app_durain_anc_siri_start_check(void)
{
    if (app_cfg_const.normal_apt_support == 1)
    {
        if (durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY)
        {
            durian_db.apt_off_cause_siri = true;
            durian_db.anc_apt_need_tone = false;
            app_durian_anc_trig(AVP_ANC_CLOSE);
        }
    }
    APP_PRINT_INFO3("app_durain_anc_siri_start_check: apt_support %d anc_cur_setting %d apt_off_cause_siri %d",
                    app_cfg_const.normal_apt_support, durian_db.anc_cur_setting, durian_db.apt_off_cause_siri);
}

void app_durain_anc_siri_stop_check(void)
{
    if (app_cfg_const.normal_apt_support == 1)
    {
        if (durian_db.apt_off_cause_siri)
        {
            durian_db.apt_off_cause_siri = false;
            if (durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY)//cur setting maybe changed
            {
                durian_db.anc_apt_need_tone = false;
                app_durian_anc_trig(durian_db.anc_cur_setting);//not report to phone as done before
            }
        }
    }
    APP_PRINT_INFO3("app_durain_anc_siri_stop_check: apt_support %d anc_cur_setting %d apt_off_cause_siri %d",
                    app_cfg_const.normal_apt_support, durian_db.anc_cur_setting, durian_db.apt_off_cause_siri);
}

void app_durian_anc_call_start_check(void)
{
    if (app_cfg_const.normal_apt_support == 1)
    {
        if (durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY)
        {
            durian_db.apt_off_cause_call = true;
            durian_db.anc_apt_need_tone = false;
            app_durian_anc_trig(AVP_ANC_CLOSE);
        }
    }
    APP_PRINT_INFO3("app_durian_anc_call_start_check: apt_support %d anc_cur_setting %d apt_off_cause_call %d",
                    app_cfg_const.normal_apt_support, durian_db.anc_cur_setting, durian_db.apt_off_cause_call);
}

void app_durian_anc_call_stop_check(void)
{
    if (app_cfg_const.normal_apt_support == 1)
    {
        if (durian_db.apt_off_cause_call)
        {
            durian_db.apt_off_cause_call = false;
            if (durian_db.anc_cur_setting == AVP_ANC_TRANSPARENCY)//cur setting maybe changed
            {
                durian_db.anc_apt_need_tone = false;
                app_durian_anc_trig(durian_db.anc_cur_setting);//not report to phone as done before
            }
        }
    }
    APP_PRINT_INFO3("app_durian_anc_call_stop_check: apt_support %d anc_cur_setting %d apt_off_cause_call %d",
                    app_cfg_const.normal_apt_support, durian_db.anc_cur_setting, durian_db.apt_off_cause_call);
}

void app_durian_anc_cylce_set(uint8_t set)
{
    APP_PRINT_INFO1("app_avp_anc_cylce_set: cycle setting 0x%2x", set);
    durian_db.anc_cycle_setting = (T_AVP_CYCLE_SETTING_ANC)set;

    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_CYCLE_SETTING);
}

void app_durian_anc_set(uint8_t anc_cur_setting)
{
    APP_PRINT_INFO1("app_durian_anc_set: anc_cur_setting 0x%2x", anc_cur_setting);
    durian_db.anc_cur_setting = (T_AVP_ANC_SETTINGS)anc_cur_setting;
    app_durian_anc_action(true);
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    if ((durian_db.remote_loc == BUD_LOC_IN_EAR) &&
        (durian_db.local_loc == BUD_LOC_IN_EAR))//both in ear
    {
        durian_db.anc_both_setting = durian_db.anc_cur_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
    }
    else if ((durian_db.remote_loc == BUD_LOC_IN_EAR) ||
             (durian_db.local_loc == BUD_LOC_IN_EAR)) //only one in ear
    {
        durian_db.anc_one_setting = durian_db.anc_cur_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_ONE_SETTING);
    }
#endif

#if DURIAN_AIRMAX
    if (durian_db.local_loc == BUD_LOC_IN_EAR) //one in ear
    {
        durian_db.anc_both_setting = durian_db.anc_cur_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
    }
#endif
}

void app_durian_anc_switch_check(void)
{
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    if (durian_db.remote_loc == BUD_LOC_IN_EAR && durian_db.local_loc == BUD_LOC_IN_EAR)//both in ear
    {
        durian_db.anc_cur_setting = durian_db.anc_both_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
    }
    else if (durian_db.remote_loc == BUD_LOC_IN_EAR ||
             durian_db.local_loc == BUD_LOC_IN_EAR) //only one in ear
    {
        durian_db.anc_cur_setting = durian_db.anc_one_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_ONE_SETTING);
    }
    else//not in ear
    {
        durian_db.anc_cur_setting = AVP_ANC_CLOSE;
    }
#endif

#if DURIAN_AIRMAX
    if (durian_db.local_loc == BUD_LOC_IN_EAR) //local in ear
    {
        durian_db.anc_cur_setting = durian_db.anc_both_setting;
        app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
    }
    else//not in ear
    {
        durian_db.anc_cur_setting = AVP_ANC_CLOSE;
    }
#endif

    if (durian_db.anc_cur_setting == AVP_ANC_NULL)
    {
        durian_db.anc_cur_setting = AVP_ANC_CLOSE;
    }

    APP_PRINT_TRACE1("app_avp_anc_switch_check: anc cur set %d", durian_db.anc_cur_setting);

    if (durian_db.anc_cur_setting <= AVP_ANC_TRANSPARENCY)
    {
        app_durian_anc_action(false);
    }
}

#if DURIAN_PRO2
void app_durian_anc_auto_apt_cfg(uint8_t auto_apt_en)
{
    APP_PRINT_INFO1("app_durian_anc_auto_apt_cfg: auto_apt_en %d", auto_apt_en);

    durian_db.auto_apt_en = auto_apt_en;
    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_AUTO_APT_ENABLE);
}
#endif

void app_durian_anc_switch(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        uint8_t index = 0, len = 0, sn = 0;
        uint8_t *loop = NULL;
        T_AVP_CYCLE_SETTING_ANC anc_cycle = AVP_ANC_CYCLE_NULL;

        /*0x03 means select 1&3, 0x05 means select 2&3,
        0x06 means select 1&2, 0x07 means select 1&2&3
        1:anc
        2.parent
        3.close
        param->bt_avp_click_settings.right_ear_control_anc;
        param->bt_avp_click_settings.left_ear_control_anc;
        param->bt_avp_click_settings.setting;*/

        APP_PRINT_TRACE3("app_durian_anc_switch: local_loc %d remote_loc %d anc_cycle_setting %d",
                         durian_db.local_loc,
                         durian_db.remote_loc, durian_db.anc_cycle_setting);

        if ((durian_db.anc_cycle_setting == AVP_ANC_CYCLE_OPEN_CLOSE) ||
            (durian_db.anc_cycle_setting == AVP_ANC_CYCLE_TRANSPARENCY_CLOSE) ||
            (durian_db.anc_cycle_setting == AVP_ANC_CYCLE_OPEN_TRANSPARENCY) ||
            (durian_db.anc_cycle_setting == AVP_ANC_CYCLE_OPEN_TRANSPARENCY_CLOSE))
        {
            anc_cycle = durian_db.anc_cycle_setting;
        }
        else
        {
            anc_cycle = AVP_ANC_CYCLE_OPEN_TRANSPARENCY_CLOSE;
        }

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
        if (durian_db.remote_loc == BUD_LOC_IN_EAR && durian_db.local_loc == BUD_LOC_IN_EAR)//both in ear
        {
            index = click_setting_anc_index[anc_cycle];
        }
        else if ((durian_db.remote_loc == BUD_LOC_IN_EAR) ||
                 (durian_db.local_loc == BUD_LOC_IN_EAR)) //only one in ear
        {
            if (durian_db.anc_one_bud_enabled)
            {
                index = click_setting_anc_index[anc_cycle];
            }
            else
            {
                index = click_setting_anc_index[AVP_ANC_CYCLE_TRANSPARENCY_CLOSE];
            }
        }
        else//not in ear
        {
            return;
        }
#endif

#if DURIAN_AIRMAX
        if (durian_db.local_loc == BUD_LOC_IN_EAR) //one in ear
        {
            index = click_setting_anc_index[anc_cycle];
        }
        else//not in ear
        {
            return;
        }
#endif

        if (index > 0 && index <= 4)
        {
            index -= 1;
            len = anc_loop[index].len;
            loop = (uint8_t *)anc_loop[index].loop;
        }
        else
        {
            APP_PRINT_ERROR1("app_durian_anc_switch: index is wrong %d", index);
            return;
        }

        for (sn = 0; sn < len; sn++)
        {
            if (loop[sn] == durian_db.anc_cur_setting)
            {
                break;
            }
        }

        APP_PRINT_TRACE4("app_durian_anc_switch: cur_setting %d sn %d len %d index %d ",
                         durian_db.anc_cur_setting, sn, len, index);

        if (sn < len)
        {
            durian_db.anc_cur_setting = (T_AVP_ANC_SETTINGS)loop[(sn + 1) % len];
        }
        else
        {
            switch (durian_db.anc_cur_setting)
            {
            case AVP_ANC_CLOSE:
                {
                    durian_db.anc_cur_setting = AVP_ANC_OPEN;
                }
                break;
            case AVP_ANC_OPEN:
                {
                    durian_db.anc_cur_setting = AVP_ANC_TRANSPARENCY;
                }
                break;
            case AVP_ANC_TRANSPARENCY:
                {
                    durian_db.anc_cur_setting = AVP_ANC_CLOSE;
                }
                break;
            default:
                {
                    durian_db.anc_cur_setting = AVP_ANC_CLOSE;
                }
                break;
            }
        }

        app_durian_anc_action(true);

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
        if (durian_db.remote_loc == BUD_LOC_IN_EAR && durian_db.local_loc == BUD_LOC_IN_EAR)//both in ear
        {
            durian_db.anc_both_setting = durian_db.anc_cur_setting;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
        }
        else if ((durian_db.remote_loc == BUD_LOC_IN_EAR) ||
                 (durian_db.local_loc == BUD_LOC_IN_EAR)) //only one in ear
        {
            durian_db.anc_one_setting = durian_db.anc_cur_setting;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_ONE_SETTING);
        }
#endif
#if DURIAN_AIRMAX
        if (durian_db.local_loc == BUD_LOC_IN_EAR) //one in ear
        {
            durian_db.anc_both_setting = durian_db.anc_cur_setting;
            app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
        }
#endif
        APP_PRINT_TRACE5("app_durian_anc_switch: index %d anc_cycle %d anc cur setting %d anc one setting %d anc both setting %d",
                         index, anc_cycle, durian_db.anc_cur_setting, durian_db.anc_one_setting,
                         durian_db.anc_both_setting);
    }
}
#endif
#endif
