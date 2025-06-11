/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "audio_route.h"
#include "audio_volume.h"
#include "audio_track.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_audio_policy.h"

#if F_APP_TWO_GAIN_TABLE
#include "app_audio_route.h"
#include "app_bond.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#if DURIAN_PRO2
#include "app_relay.h"
#endif

#if DURIAN_AIRMAX
static const uint16_t airmax_audio_dac_dig_gain_table[] =
{
    0x8001, 0xe980, 0xea00, 0xea00, 0xea80, 0xea80, 0xeb00, 0xeb00, 0xeb80, 0xeb80,
    0xec00, 0xec00, 0xec80, 0xec80, 0xed00, 0xed00, 0xed80, 0xed80, 0xee00, 0xee00,
    0xee80, 0xee80, 0xef00, 0xef00, 0xef80, 0xef80, 0xf000, 0xf000, 0xf080, 0xf080,
    0xf100, 0xf100, 0xf180, 0xf180, 0xf200, 0xf200, 0xf280, 0xf280, 0xf300, 0xf300,
    0xf380, 0xf380, 0xf400, 0xf400, 0xf480, 0xf480, 0xf500, 0xf500, 0xf580, 0xf580,
    0xf600, 0xf600, 0xf680, 0xf680, 0xf700, 0xf700, 0xf780, 0xf780, 0xf800, 0xf800,
    0xf880, 0xf880, 0xf900, 0xf900, 0xf980, 0xf980, 0xfa00, 0xfa00, 0xfa80, 0xfa80,
    0xfb00, 0xfb00, 0xfb80, 0xfb80, 0xfc00, 0xfc00, 0xfc80, 0xfc80, 0xfd00, 0xfd00,
    0xfd80, 0xfd80, 0xfe00, 0xfe00, 0xfe80, 0xfe80, 0xff00, 0xff00, 0xff80, 0xff80,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};
#endif

#if F_APP_TWO_GAIN_TABLE
static const uint16_t audio_dac_dig_gain_table_iphone[] =
{
    0x8001, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};
#endif

#if DURIAN_PRO2
void app_durian_audio_vol_control(uint8_t vol_ctl_en)
{
    APP_PRINT_INFO1("app_durian_audio_vol_control: vol_control_en %d", vol_ctl_en);

    durian_db.vol_ctl_en = vol_ctl_en;
    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_VOL_CTL_EN);
}
#endif

bool app_durian_audio_abs_is_support(uint8_t status)
{
    bool abs_vol_is_support = (status == ABS_VOL_SUPPORTED) ? true : false;
    APP_PRINT_TRACE1("app_avp_abs_vol_check: abs_vol_state %d", status);

    return abs_vol_is_support;
}

T_BT_AVP_MIC app_durian_audio_get_mic_set(void)
{
    return durian_db.mic_setting;
}

bool app_durian_audio_spk_unmute_check(void)
{
    bool need_unmute = true;

#if (DURIAN_AIRMAX == 0)
    if ((durian_db.local_loc == BUD_LOC_IN_CASE) && (durian_db.remote_loc != BUD_LOC_IN_CASE))
    {
        if (app_db.remote_loc_received)
        {
            need_unmute = false;
        }
    }
#endif

    return need_unmute;
}

void app_durian_audio_spk_check(void)
{
    bool mute_need = false;

    if (durian_db.ear_detect_en)
    {
        if ((durian_db.local_loc == BUD_LOC_IN_CASE) &&
            (durian_db.remote_loc == BUD_LOC_IN_EAR)) //one in box ,one in ear
        {
            mute_need = true;
        }
    }
    else
    {
        if ((durian_db.local_loc == BUD_LOC_IN_CASE) &&
            (durian_db.remote_loc != BUD_LOC_IN_CASE)) //only one in box
        {
            mute_need = true;
        }
    }

    app_audio_spk_mute_unmute(mute_need);
    APP_PRINT_TRACE4("app_durian_audio_spk_check: local_loc %d remote_loc %d mute_need %d ear_detect_en %d",
                     durian_db.local_loc, durian_db.remote_loc, mute_need, durian_db.ear_detect_en);
}

#if F_APP_TWO_GAIN_TABLE
void app_durian_audio_amplfy_vol_set(void)
{
    uint8_t *bd_addr = NULL;
    uint8_t pair_idx_mapping = 0;
    T_APP_BR_LINK *p_link = 0;

    for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
    {
        if (app_link_check_b2s_link_by_id(app_idx))
        {
            bd_addr = app_db.br_link[app_idx].bd_addr;
            break;
        }
    }

    if (bd_addr == NULL)
    {
        return;
    }

    if (app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping))
    {
        p_link = app_link_find_br_link(bd_addr);

        if (p_link != NULL)
        {
            app_audio_vol_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx_mapping]);
        }
    }
}
#endif

bool app_durian_audio_dac_gain_set(uint32_t level, void *gain)
{
#if F_APP_TWO_GAIN_TABLE
    APP_PRINT_TRACE2("app_durian_audio_dac_gain_set: level %d amplify_gain %d", level,
                     durian_db.amplify_gain);
#endif

    T_AUDIO_ROUTE_DAC_GAIN *gain_val = (T_AUDIO_ROUTE_DAC_GAIN *)gain;

    if (level > app_dsp_cfg_vol.playback_volume_max)
    {
        return false;
    }

#if F_APP_TWO_GAIN_TABLE
    if (durian_db.amplify_gain)
    {
        gain_val->dac_gain = audio_dac_dig_gain_table_iphone[level];
    }
    else
    {
        gain_val->dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
    }
#else
#if DURIAN_AIRMAX
    gain_val->dac_gain = airmax_audio_dac_dig_gain_table[level];
#else
    gain_val->dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
#endif
#endif
    return true;
}

#if F_APP_SENSOR_MEMS_SUPPORT
void app_durian_audio_spatial_start(uint8_t app_idx)
{
    if ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) == 0)
    {
        app_sensor_mems_spatial_start(app_idx);
    }
}

void app_durian_audio_spatial_stop(uint8_t app_idx)
{
    if ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) == 0)
    {
        app_sensor_mems_spatial_stop(app_idx);
    }
}
#endif
#endif
