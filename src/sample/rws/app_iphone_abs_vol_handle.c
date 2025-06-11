#if F_APP_IPHONE_ABS_VOL_HANDLE
#include "app_iphone_abs_vol_handle.h"
#include "app_audio_policy.h"
#include "app_a2dp.h"

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#endif

const uint8_t iphone_abs_vol[16] =  {0x07, 0x0F, 0x17, 0x1F, 0x27, 0x2F, 0x37, 0x3F, 0x47, 0x4F, 0x57, 0x5F, 0x67, 0x6F, 0x77, 0x7F};

uint8_t app_iphone_abs_vol_lv_handle(uint8_t abs_vol)
{
    uint8_t gain_level = 0;

    if (abs_vol == 0)
    {
        //mute
        gain_level = 0;
    }
    else
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            if (abs_vol > iphone_abs_vol[i])
            {
                continue;
            }
            else
            {
                gain_level = i;
                break;
            }
        }
    }

    APP_PRINT_TRACE2("app_iphone_abs_vol_lv_handle:lv: %d, vol: 0x%02X", gain_level, abs_vol);

    return gain_level;
}

bool app_iphone_abs_vol_wrap_audio_track_volume_out_set(T_AUDIO_TRACK_HANDLE handle, uint8_t volume,
                                                        uint8_t abs_vol)
{
    bool ret = true;
    T_APP_BR_LINK *p_link = NULL;
    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link != NULL)
    {
        if (handle != p_link->a2dp_track_handle)
        {
            APP_PRINT_ERROR0("handle is not a2dp handle");

            ret = false;
            goto ERROR;
        }

        if (p_link->set_a2dp_fake_lv0_gain)
        {
            p_link->set_a2dp_fake_lv0_gain = false;
        }

#if HARMAN_OPEN_LR_FEATURE
        app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, volume, __func__, __LINE__);
#endif
        if ((p_link->remote_device_vendor_id == APP_REMOTE_DEVICE_IOS) &&
            (app_iphone_abs_vol_check_a2dp_total_gain_num_16() == true))
        {
            if ((volume == 0) && (abs_vol != 0))
            {
                // Fake level 0, create level 0.5 (-65db).
                ret = app_audio_route_dac_gain_set(AUDIO_CATEGORY_AUDIO, volume, DAC_GAIN_DB_NEGATIVE_65_DB);
                ret = audio_track_volume_out_set(handle, volume);
            }
            else if ((volume == 0) && (abs_vol == 0))
            {
                // Real level 0 (mute).
                ret = app_audio_route_dac_gain_set(AUDIO_CATEGORY_AUDIO, volume, DAC_GAIN_DB_MUTE);
                ret = audio_track_volume_out_set(handle, volume);
            }
            else
            {
                ret = audio_track_volume_out_set(handle, volume);
            }
        }
        else
        {
            ret = audio_track_volume_out_set(handle, volume);
        }
    }

ERROR:
    return ret;
}

void app_iphone_abs_vol_sync_abs_vol(uint8_t *bd_addr, uint8_t abs_vol)
{
    uint8_t cmd_ptr[7];

    memcpy(&cmd_ptr[0], bd_addr, 6);
    cmd_ptr[6] = abs_vol;

#if F_APP_IPHONE_ABS_VOL_HANDLE
    app_cfg_store(&app_cfg_nv.abs_vol[0], 8);
#endif

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_ABS_VOLUME,
                                        cmd_ptr, sizeof(cmd_ptr));

    APP_PRINT_TRACE2("app_iphone_abs_vol_sync_abs_vol: %s (0x%02X)", TRACE_BDADDR(bd_addr), abs_vol);
}

bool app_iphone_abs_vol_check_a2dp_total_gain_num_16(void)
{
    bool ret = false;
    uint8_t total_gain_lv = 0;

    total_gain_lv = audio_volume_out_max_get(AUDIO_STREAM_TYPE_PLAYBACK);

    if (total_gain_lv == A2DP_TOTAL_GAIN_NUM_16)
    {
        ret = true;
    }

    return ret;
}
#endif
