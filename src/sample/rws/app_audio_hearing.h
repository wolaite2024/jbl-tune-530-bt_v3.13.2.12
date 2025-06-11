/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdint.h>
#include <stdbool.h>
#include "audio_volume.h"
#include "audio_track.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool app_audio_hearing_start(T_AUDIO_FORMAT_INFO format_info, uint8_t volume_out);
bool app_audio_hearing_write(uint8_t      frame_num,
                             void        *buf,
                             uint16_t     len);
bool app_audio_hearing_volume_out_set(uint8_t volume);
bool app_audio_hearing_volume_out_mute(void);
bool app_audio_hearing_volume_out_unmute(void);
bool app_audio_hearing_state_get(T_AUDIO_TRACK_STATE *state);
T_AUDIO_TRACK_HANDLE app_audio_hearing_track_handle_get(void);
bool app_audio_hearing_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
