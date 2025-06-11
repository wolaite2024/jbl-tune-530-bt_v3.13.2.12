/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_relay.h"
#include "app_hfp.h"
#include "app_audio_policy.h"

#if DURIAN_TWO
#include "app_key_process.h"
#endif

#if (F_APP_QDECODE_SUPPORT && DURIAN_AIRMAX)
#include "app_qdec.h"
#endif

#define AVP_LONG_PRESS_STEP               2
#define AVP_MULTI_CLICK_STEP              2

#if DURIAN_TWO
static const T_MMI_ACTION mmi_map[5] = {MMI_NULL, MMI_HF_INITIATE_VOICE_DIAL, MMI_AV_PLAY_PAUSE, MMI_AV_FWD, MMI_AV_BWD};
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
static const T_MMI_ACTION mmi_long_press_map[4] = {MMI_HF_INITIATE_VOICE_DIAL, MMI_LISTENING_MODE_CYCLE, MMI_DEV_SPK_VOL_DOWN, MMI_DEV_SPK_VOL_UP};
#endif

#if DURIAN_TWO
void app_durian_key_control_set(uint8_t left_ear_control, uint8_t right_ear_control)
{
    APP_PRINT_INFO2("app_durian_key_control_set: left_setting 0x%2x, right_setting 0x%2x",
                    left_ear_control,
                    right_ear_control);

    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        durian_db.local_double_click_action = left_ear_control;
        durian_db.remote_double_click_action = right_ear_control;
    }
    else
    {
        durian_db.local_double_click_action = right_ear_control;
        durian_db.remote_double_click_action = left_ear_control;
    }

    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_LOCAL_REMOTE_DOUBLE_CFG);
}
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
void app_durian_key_click_set(uint8_t left_right_action)
{
    APP_PRINT_INFO1("app_durian_key_click_set: left_right_action 0x%2x", left_right_action);
    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        durian_db.local_long_action = (left_right_action >> 4) & 0x0F;
        durian_db.remote_long_action = left_right_action & 0x0F;
    }
    else
    {
        durian_db.remote_long_action = (left_right_action >> 4) & 0x0F;
        durian_db.local_long_action = left_right_action & 0x0F;
    }

    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_LOCAL_REMOTE_LONG_CFG);
}
#endif

#if (F_APP_QDECODE_SUPPORT && DURIAN_AIRMAX)
static bool app_durian_key_vol_is_up(uint8_t direct)
{
    bool vol_is_up = false;
    if (app_cfg_nv.is_clockwise != 0)
    {
        vol_is_up = (direct != 0) ? true : false;
    }
    else
    {
        vol_is_up = (direct != 0) ? false : true;
    }

    if (app_link_get_b2s_link_num() == 0)
    {
        //play invalid key tone
        app_audio_tone_type_play(TONE_KEY_SHORT_PRESS, false, false);
    }

    APP_PRINT_TRACE3("app_durian_key_vol_is_up: direct %d is_clockwise %d vol_is_up %d ", direct,
                     app_cfg_nv.is_clockwise, vol_is_up);
    return vol_is_up;
}
#endif

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
void app_durian_key_long_press_action(bool from_remote)
{
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

    if (call_status >= APP_CALL_INCOMING)
    {
#if (F_APP_LISTENING_MODE_SUPPORT == 1)
        app_durian_anc_switch();
#endif
    }
    else
    {
        if (((durian_db.local_long_action  != BT_AVP_CONTROL_ANC) && !from_remote) ||
            ((durian_db.remote_long_action  != BT_AVP_CONTROL_ANC) && from_remote))
        {
            uint8_t action_idx = (from_remote) ? durian_db.remote_long_action : durian_db.local_long_action;
            T_MMI_ACTION action = mmi_long_press_map[action_idx];

            app_relay_async_single(APP_MODULE_TYPE_MMI, action);
            app_mmi_handle_action(action);
        }
#if (F_APP_LISTENING_MODE_SUPPORT == 1)
        else
        {
            app_durian_anc_switch();
        }
#endif
    }
}
#endif

bool app_durian_key_press(uint8_t key_mask, uint8_t mode, uint8_t type)
{
    bool exe_external = true;
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();
    APP_PRINT_TRACE8("app_durian_key_press: local_long_action %d remote_long_action %d key_mask %d mode %d type %d call_status %d mfb_local_action 0x%x fast_pair_connect %d",
                     durian_db.local_long_action, durian_db.remote_long_action, key_mask, mode, type, call_status,
                     durian_db.local_double_click_action, durian_db.fast_pair_connected);

    if (durian_db.local_loc != BUD_LOC_IN_EAR)
    {
        exe_external = false;
        return exe_external;
    }

#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    if ((key_mask == 0x01) && (mode == 0) && (type != 0)) //key0 single long press
    {
        app_durian_sync_long_press();
        exe_external = false;
    }
#elif DURIAN_TWO
    if ((key_mask == 0x01) && (mode == 1) && (type == HYBRID_KEY_2_CLICK))
    {
        if (call_status == APP_CALL_IDLE)
        {
            T_MMI_ACTION action = (durian_db.local_double_click_action > BT_AVP_CONTROL_BACKWARD) ?
                                  MMI_NULL : mmi_map[durian_db.local_double_click_action];

            app_relay_async_single(APP_MODULE_TYPE_MMI, action);
            app_mmi_handle_action(action);
            exe_external = false;
        }
    }
#endif
    return exe_external;
}

bool app_durian_key_release_event(uint8_t key_mask, uint8_t pressed)
{
    APP_PRINT_TRACE3("app_durian_key_press_release_event:local_loc=%d key mask %02x, key_pressed %d",
                     durian_db.local_loc, key_mask, pressed);
    if (durian_db.local_loc != BUD_LOC_IN_EAR)
    {
        return false;
    }

    if (pressed)
    {
#if DURIAN_AIRMAX
        if (app_link_get_b2s_link_num() == 0)
        {
            if (key_mask != 0x01)//!=KEY0_MASK
            {
                //play invalid key tone
                app_audio_tone_type_play(TONE_KEY_SHORT_PRESS, false, false);
            }
        }
#else
        app_audio_tone_type_play(TONE_KEY_SHORT_PRESS, false, false);
#endif
    }
    return true;
}

uint8_t app_durian_key_multiclick_get(uint8_t cfg)
{
    uint8_t click_interval;
    uint8_t var = cfg * AVP_MULTI_CLICK_STEP;
    if (cfg <= 2)
    {
        click_interval = durian_db.click_speed_origin + var;
    }
    else
    {
        click_interval = durian_db.click_speed_origin;
    }

    APP_PRINT_INFO3("app_durian_key_multiclick_get: origin %d cfg %d click interval %d",
                    durian_db.click_speed_origin, cfg, click_interval);

    return click_interval;
}

uint8_t  app_durian_key_long_press_get(uint8_t cfg)
{
    uint8_t long_interval;
    uint8_t var = cfg * AVP_LONG_PRESS_STEP;
    if ((cfg <= 2) && (durian_db.long_press_origin > var))
    {
        long_interval = durian_db.long_press_origin - var;
    }
    else
    {
        long_interval = durian_db.long_press_origin;

    }

    APP_PRINT_INFO3("app_durian_key_long_press_get: orign %d cfg %d long interval %d",
                    durian_db.long_press_origin, cfg, long_interval);

    return long_interval;
}
#endif
