/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_IPHONE_ABS_VOL_HANDLE_H_
#define _APP_IPHONE_ABS_VOL_HANDLE_H_

#include <string.h>
#include "trace.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_relay.h"
#include "audio_track.h"
#include "audio_type.h"
#include "audio_volume.h"
#include "app_audio_route.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DAC_GAIN_DB_NEGATIVE_65_DB  0xDF80 /**< A2DP gain db: -65 db */
#define DAC_GAIN_DB_MUTE            0xC000 /**< A2DP gain db: mute   */

#define A2DP_TOTAL_GAIN_NUM_16      15

extern const uint8_t iphone_abs_vol[16];

/**
* app_iphone_abs_vol_handle.h
*
* \brief   Find the level (index) corresponds to iphone_abs_vol[].
*
* \param[in]  abs_vol   Absolute volume from source device.
*
* \return   The level (index) corresponds to iphone_abs_vol[].
*
* \ingroup none
*/
uint8_t app_iphone_abs_vol_lv_handle(uint8_t abs_vol);

/**
 * app_iphone_abs_vol_handle.h
 *
 * \brief   It's a wrap function to execute audio_track_volume_out_set or
 *            audio_track_volume_out_set_with_gain.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] volume    The volume out level of the Audio Track session.
 * \param[in] abs_vol   Absolute volume from source device.
 *
 * \return          The status of setting the Audio Track current volume out level.
 * \retval true     Audio Track current volume out level was set successfully.
 * \retval false    Audio Track current volume out level was failed to set.
 *
 * \ingroup None
 */
bool app_iphone_abs_vol_wrap_audio_track_volume_out_set(T_AUDIO_TRACK_HANDLE handle, uint8_t volume,
                                                        uint8_t abs_vol);
/**
 * app_iphone_abs_vol_handle.h
 *
 * \brief   Primary one syncs absolute volume to secondary one.
 *
 * \param[in]  bd_addr   Source device's BT address.
 * \param[in]  abs_vol   Absolute volume from source device.
 *
 * \return         Void
 *
 * \ingroup none
 */
void app_iphone_abs_vol_sync_abs_vol(uint8_t *bd_addr, uint8_t abs_vol);

/**
 * app_iphone_abs_vol_handle.h
 *
 * \brief   It's a check funtion used to check
 *          whether the setting of total a2dp gain number in dsp config is 16 or not.
 *
 * \param[in]       Void.
 *
 * \return          Whether the setting of total a2dp gain number in dsp config is 16 or not.
 * \retval true     Total a2dp gain number is 16.
 * \retval false    Total a2dp gain number is NOT 16.
 *
 * \ingroup none
 */
bool app_iphone_abs_vol_check_a2dp_total_gain_num_16(void);

#ifdef __cplusplus
}
#endif

#endif

