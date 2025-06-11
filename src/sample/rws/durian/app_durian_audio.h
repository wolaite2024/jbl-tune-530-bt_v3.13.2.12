/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_AUDIO_H_
#define _APP_DURIAN_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "bt_avp.h"
#include "audio_track.h"

bool app_durian_audio_abs_is_support(uint8_t status);
T_BT_AVP_MIC app_durian_audio_get_mic_set(void);
bool app_durian_audio_dac_gain_set(uint32_t level, void *gain);
void app_durian_audio_spk_check(void);
bool app_durian_audio_spk_unmute_check(void);
void app_durian_audio_spatial_stop(uint8_t app_id);
void app_durian_audio_spatial_start(uint8_t app_idx);
void app_durian_audio_vol_control(uint8_t vol_ctl_en);
void app_durian_audio_amplfy_vol_set(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
