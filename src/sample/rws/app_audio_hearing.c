/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "os_mem.h"
#include "trace.h"
#include "sysm.h"
#include "audio.h"
#include "audio_volume.h"
#include "audio_type.h"
#include "audio_probe.h"
#include "ringtone.h"
#include "voice_prompt.h"
#include "app_audio_hearing.h"

#define AUDIO_HEARING_UPPER_LEVEL            50 //ms
#define AUDIO_HEARING_LORWE_LEVEL            30 //ms
#define AUDIO_HEARING_LATENCY                40 //ms

static void *track_handle = NULL;
static uint16_t  seq_num = 0;

bool app_audio_hearing_start(T_AUDIO_FORMAT_INFO format_info, uint8_t volume_out)
{
    T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_NORMAL;
    T_AUDIO_STREAM_USAGE stream = AUDIO_STREAM_USAGE_LOCAL;

    if (track_handle)
    {
        audio_track_release(track_handle);
    }

    track_handle = audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                      mode,
                                      stream,
                                      format_info,
                                      volume_out,
                                      0,
                                      AUDIO_DEVICE_OUT_DEFAULT,
                                      NULL,
                                      NULL);


    if (track_handle != NULL)
    {
        seq_num = 0;
        audio_track_threshold_set(track_handle, AUDIO_HEARING_UPPER_LEVEL, AUDIO_HEARING_LORWE_LEVEL);
        audio_track_latency_set(track_handle, AUDIO_HEARING_LATENCY, false);
        audio_track_start(track_handle);
        return true;
    }

    return false;
}

bool app_audio_hearing_write(uint8_t      frame_num,
                             void        *buf,
                             uint16_t     len)
{
    uint16_t written_len;

    if (track_handle)
    {
        if (audio_track_write(track_handle,
                              0,
                              seq_num,
                              AUDIO_STREAM_STATUS_CORRECT,
                              frame_num,
                              buf,
                              len,
                              &written_len))
        {
            seq_num++;
            return true;
        }
    }

    return false;
}

bool app_audio_hearing_volume_out_set(uint8_t volume)
{
    return audio_track_volume_out_set(track_handle, volume);
}

bool app_audio_hearing_volume_out_mute(void)
{
    return audio_track_volume_out_mute(track_handle);
}

bool app_audio_hearing_volume_out_unmute(void)
{
    return audio_track_volume_out_unmute(track_handle);
}

bool app_audio_hearing_state_get(T_AUDIO_TRACK_STATE *state)
{
    return audio_track_state_get(track_handle, state);
}

T_AUDIO_TRACK_HANDLE app_audio_hearing_track_handle_get(void)
{
    return track_handle;
}

bool app_audio_hearing_stop(void)
{
    if (track_handle)
    {
        audio_track_release(track_handle);
        seq_num = 0;
        return true;
    }

    return false;
}
