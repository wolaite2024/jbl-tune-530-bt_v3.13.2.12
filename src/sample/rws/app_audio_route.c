/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_mem.h"
#include "trace.h"
#include "app_dsp_cfg.h"
#include "app_cfg.h"
#include "app_audio_route.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_cmd.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

bool app_audio_route_physical_mic_get(T_AUDIO_CATEGORY category,
                                      T_AUDIO_ROUTE_LOGIC_IO_TYPE logical_mic,
                                      T_AUDIO_ROUTE_PHYSICAL_MIC *physical_mic)
{
    T_AUDIO_ROUTE_PHYSICAL_PATH *physical_path;
    T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP physical_path_group;
    uint8_t physical_path_num;

    physical_path_group = audio_route_physical_path_take((T_AUDIO_CATEGORY)category);
    physical_path_num = physical_path_group.physical_path_num;
    physical_path = physical_path_group.physical_path;

    if (physical_path == NULL)
    {
        return false;
    }

    for (uint8_t i = 0; i < physical_path_num; i++)
    {
        if (physical_path->logic_io_type == logical_mic)
        {
            *physical_mic = physical_path->attr.mic_attr;
            break;
        }

        physical_path++;
    }

    return true;
}

bool app_audio_route_physical_mic_set(T_AUDIO_CATEGORY category,
                                      T_AUDIO_ROUTE_LOGIC_IO_TYPE logical_mic,
                                      T_AUDIO_ROUTE_PHYSICAL_MIC physical_mic)
{
    T_AUDIO_ROUTE_PHYSICAL_PATH *physical_path, *physical_path_ptr;
    T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP physical_path_group;
    uint8_t physical_path_num;

    audio_route_category_path_unregister(category);

    physical_path_group = audio_route_physical_path_take(category);
    physical_path_num = physical_path_group.physical_path_num;
    physical_path = os_mem_zalloc2(sizeof(T_AUDIO_ROUTE_PHYSICAL_PATH) * physical_path_num);

    if (physical_path == NULL)
    {
        return false;
    }

    memcpy(physical_path, physical_path_group.physical_path,
           sizeof(T_AUDIO_ROUTE_PHYSICAL_PATH) * physical_path_num);
    audio_route_physical_path_give(&physical_path_group);

    physical_path_ptr = physical_path;

    for (uint8_t i = 0; i < physical_path_num; i++)
    {
        if (physical_path_ptr->logic_io_type == logical_mic)
        {
            physical_path_ptr->attr.mic_attr = physical_mic;
            break;
        }

        physical_path_ptr++;
    }

    audio_route_category_path_register(category, physical_path, physical_path_num);
    os_mem_free(physical_path);

    return true;
}

bool app_audio_route_dac_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
            if (level <= app_dsp_cfg_vol.playback_volume_max)
            {
                app_dsp_cfg_data->audio_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_out_volume_max)
            {
                app_dsp_cfg_data->voice_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_ANALOG:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_out_max)
            {
                app_dsp_cfg_data->aux_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_dsp_cfg_vol.ringtone_volume_max)
            {
                app_dsp_cfg_data->ringtone_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_dsp_cfg_vol.voice_prompt_volume_max)
            {
                app_dsp_cfg_data->vp_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                app_dsp_cfg_data->apt_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                app_dsp_cfg_data->llapt_dac_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

bool app_audio_route_adc_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_volume_in_max)
            {
                app_dsp_cfg_data->voice_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            if (level <= app_dsp_cfg_vol.record_volume_max)
            {
                app_dsp_cfg_data->record_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_ANALOG:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_in_max)
            {
                app_dsp_cfg_data->aux_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                app_dsp_cfg_data->apt_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                app_dsp_cfg_data->llapt_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_dsp_cfg_vol.vad_volume_max)
            {
                app_dsp_cfg_data->vad_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

#if F_APP_TEAMS_FEATURE_SUPPORT
static int16_t app_audio_route_teams_audio_dac_gain_get(uint32_t level)
{
    int16_t dac_gain = 0;

    if ((level < app_dsp_cfg_vol.playback_volume_max) || (teams_dac_gain == 0))
    {
        dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level] + teams_dac_gain;
    }
    else
    {
        dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level - 1] + teams_dac_gain + 384;
    }

    return dac_gain;
}

static int16_t app_audio_route_teams_voice_dac_gain_get(uint32_t level)
{
    int16_t dac_gain = 0;

    if ((level < app_dsp_cfg_vol.voice_out_volume_max) || (teams_dac_gain == 0))
    {
        dac_gain = app_dsp_cfg_data->voice_dac_gain_table[level] + teams_dac_gain;
    }
    else
    {
        dac_gain = app_dsp_cfg_data->voice_dac_gain_table[level - 1] + teams_dac_gain + 384;
    }

    return dac_gain;
}
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
static int16_t app_audio_route_harman_audio_dac_gain_get(uint32_t level)
{
    int16_t dac_gain = 0;

    APP_PRINT_TRACE2("[SD_CHECK] refine vol_table %d,%d", app_cfg_nv.max_volume_limit_status,
                     level);
    if (!app_cfg_nv.max_volume_limit_status)
    {
        dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
    }
    else
    {
        if (level >= app_cfg_nv.max_volume_limit_level)
        {
            dac_gain = app_dsp_cfg_data->audio_dac_gain_table[app_cfg_nv.max_volume_limit_level];
        }
        else
        {
            dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
        }
    }

    return dac_gain;
}
#endif

static bool app_audio_route_dac_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                               T_AUDIO_ROUTE_DAC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
        {
#if F_APP_DURIAN_SUPPORT
            bool dac_set = false;
            dac_set = app_durian_audio_dac_gain_set(level, gain);

            if (!dac_set)
            {
                return false;
            }
#else
            if (level <= app_dsp_cfg_vol.playback_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                gain->dac_gain = app_audio_route_teams_audio_dac_gain_get(level);
#elif F_APP_HARMAN_FEATURE_SUPPORT
                gain->dac_gain = app_audio_route_harman_audio_dac_gain_get(level);
#else
                gain->dac_gain = app_dsp_cfg_data->audio_dac_gain_table[level];
#endif
            }
            else
            {
                return false;
            }
#endif
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_out_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                gain->dac_gain = app_audio_route_teams_voice_dac_gain_get(level);
#else
                gain->dac_gain = app_dsp_cfg_data->voice_dac_gain_table[level];
#endif
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_ANALOG:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_out_max)
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                gain->dac_gain = app_audio_route_harman_audio_dac_gain_get(level);
#else
                gain->dac_gain = app_dsp_cfg_data->aux_dac_gain_table[level];
#endif
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_dsp_cfg_vol.ringtone_volume_max)
            {
                gain->dac_gain = app_dsp_cfg_data->ringtone_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_dsp_cfg_vol.voice_prompt_volume_max)
            {
                gain->dac_gain = app_dsp_cfg_data->vp_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                gain->dac_gain = app_dsp_cfg_data->apt_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                gain->dac_gain = app_dsp_cfg_data->llapt_dac_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

static bool app_audio_route_adc_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                               T_AUDIO_ROUTE_ADC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_dsp_cfg_vol.voice_volume_in_max)
            {
                gain->adc_gain = app_dsp_cfg_data->voice_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_RECORD:
        {
            if (level <= app_dsp_cfg_vol.record_volume_max)
            {
                gain->adc_gain = app_dsp_cfg_data->record_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_ANALOG:
        {
            if (level <= app_dsp_cfg_vol.line_in_volume_in_max)
            {
                gain->adc_gain = app_dsp_cfg_data->aux_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                gain->adc_gain = app_dsp_cfg_data->apt_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {
            if (level <= app_dsp_cfg_vol.apt_volume_in_max)
            {
                gain->adc_gain = app_dsp_cfg_data->llapt_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_dsp_cfg_vol.vad_volume_max)
            {
                gain->adc_gain = app_dsp_cfg_data->vad_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void app_audio_route_gain_init(void)
{
    uint8_t index;

    for (index = 0; index < AUDIO_CATEGORY_NUMBER; index++)
    {
        audio_route_gain_register((T_AUDIO_CATEGORY)index, app_audio_route_dac_gain_get_cback,
                                  app_audio_route_adc_gain_get_cback);
    }
}
