/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_track.h"
#include "tts.h"
#include "audio_passthrough.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup  AUDIO_MANAGER_AUDIO Audio
 *
 * \brief Audio module init, register/unregister callback and change volume.
 */

/**
 * audio.h
 *
 * \brief  Audio manager event type for \ref P_AUDIO_MGR_CBACK.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef enum t_audio_event
{
    AUDIO_EVENT_UNKNOWN                             = 0x0000,

    /**
     * \brief Indicate audio volume out changed \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_VOLUME_OUT_CHANGED                  = 0x0001,

    /**
     * \brief Indicate audio volume out muted \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED.
     */
    AUDIO_EVENT_VOLUME_OUT_MUTED                    = 0x0002,

    /**
     * \brief Indicate audio volume out unmuted \ref T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED.
     */
    AUDIO_EVENT_VOLUME_OUT_UNMUTED                  = 0x0003,

    /**
     * \brief Indicate audio volume in changed \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_VOLUME_IN_CHANGED                   = 0x0004,

    /**
     * \brief Indicate audio volume in muted \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED.
     */
    AUDIO_EVENT_VOLUME_IN_MUTED                     = 0x0005,

    /**
     * \brief Indicate audio volume in unmuted \ref T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED.
     */
    AUDIO_EVENT_VOLUME_IN_UNMUTED                   = 0x0006,

    /**
     * \brief Indicate audio volume balance changed \ref T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED.
     */
    AUDIO_EVENT_VOLUME_BALANCE_CHANGED              = 0x0007,

    /**
     * \brief Indicate TTS started \ref T_AUDIO_EVENT_PARAM_TTS_STARTED.
     */
    AUDIO_EVENT_TTS_STARTED                         = 0x0100,

    /**
     * \brief Indicate TTS alerted \ref T_AUDIO_EVENT_PARAM_TTS_ALERTED.
     */
    AUDIO_EVENT_TTS_ALERTED                         = 0x0101,

    /**
     * \brief Indicate TTS paused \ref T_AUDIO_EVENT_PARAM_TTS_PAUSED.
     */
    AUDIO_EVENT_TTS_PAUSED                          = 0x0102,

    /**
     * \brief Indicate TTS resumed \ref T_AUDIO_EVENT_PARAM_TTS_RESUMED.
     */
    AUDIO_EVENT_TTS_RESUMED                         = 0x0103,

    /**
     * \brief Indicate TTS stopped \ref T_AUDIO_EVENT_PARAM_TTS_STOPPED.
     */
    AUDIO_EVENT_TTS_STOPPED                         = 0x0104,

    /**
     * \brief Indicate TTS exceeded \ref T_AUDIO_EVENT_PARAM_TTS_EXCEEDED.
     */
    AUDIO_EVENT_TTS_EXCEEDED                        = 0x0105,

    /**
     * \brief Indicate ringtone started \ref T_AUDIO_EVENT_PARAM_RINGTONE_STARTED.
     */
    AUDIO_EVENT_RINGTONE_STARTED                    = 0x0110,

    /**
     * \brief Indicate ringtone stopped \ref T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED.
     */
    AUDIO_EVENT_RINGTONE_STOPPED                    = 0x0111,

    /**
     * \brief Indicate voice prompt started \ref T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED.
     */
    AUDIO_EVENT_VOICE_PROMPT_STARTED                = 0x0120,

    /**
     * \brief Indicate voice prompt stopped \ref T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED.
     */
    AUDIO_EVENT_VOICE_PROMPT_STOPPED                = 0x0121,

    /**
     * \brief Indicate Audio Track session state changed \ref T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED.
     */
    AUDIO_EVENT_TRACK_STATE_CHANGED                 = 0x0200,

    /**
     * \brief Indicate Audio Track session volume out changed \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED            = 0x0201,

    /**
     * \brief Indicate Audio Track session volume out muted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_MUTED              = 0x0202,

    /**
     * \brief Indicate Audio Track session volume out unmuted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_OUT_UNMUTED            = 0x0203,

    /**
     * \brief Indicate Audio Track session volume in changed \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_CHANGED             = 0x0204,

    /**
     * \brief Indicate Audio Track session volume in muted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_MUTED               = 0x0205,

    /**
     * \brief Indicate Audio Track session volume in unmuted \ref T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED.
     */
    AUDIO_EVENT_TRACK_VOLUME_IN_UNMUTED             = 0x0206,

    /**
     * \brief Indicate Audio Track session data received \ref T_AUDIO_EVENT_PARAM_TRACK_DATA_IND.
     */
    AUDIO_EVENT_TRACK_DATA_IND                      = 0x0207,

    /**
     * \brief Indicate Audio Track buffer low alerted \ref T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW.
     */
    AUDIO_EVENT_TRACK_BUFFER_LOW                    = 0x0208,

    /**
     * \brief Indicate Audio Track buffer high alerted \ref T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH.
     */
    AUDIO_EVENT_TRACK_BUFFER_HIGH                   = 0x0209,

    /**
     * \brief Indicate Audio Track signal out strength in dB \ref AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED.
     */
    AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED          = 0x020a,

    /**
     * \brief Report latency when Audio Track receives \ref AUDIO_EVENT_TRACK_LATENCY_REPORTED.
     */
    AUDIO_EVENT_TRACK_LATENCY_REPORTED              = 0x020b,

    /**
     * \brief Indicate Voice Activity detected \ref T_AUDIO_EVENT_VOICE_ACTIVITY_DETECTED.
     */
    AUDIO_EVENT_VOICE_ACTIVITY_DETECTED             = 0x0300,

    /**
     * \brief Indicate ANC enabled \ref T_AUDIO_EVENT_PARAM_ANC_ENABLED.
     */
    AUDIO_EVENT_ANC_ENABLED                         = 0x0301,

    /**
     * \brief Indicate ANC disabled \ref T_AUDIO_EVENT_PARAM_ANC_DISABLED.
     */
    AUDIO_EVENT_ANC_DISABLED                        = 0x0302,

    /**
     * \brief Indicate ANC adaptive filter started \ref T_AUDIO_EVENT_PARAM_ANC_ADAPTIVE_FILTER_STARTED.
     */
    AUDIO_EVENT_ANC_ADAPTIVE_FILTER_STARTED         = 0x0303,

    /**
     * \brief Indicate ANC adaptive filter paused \ref T_AUDIO_EVENT_PARAM_ANC_ADAPTIVE_FILTER_PAUSED.
     */
    AUDIO_EVENT_ANC_ADAPTIVE_FILTER_PAUSED          = 0x0304,

    /**
     * \brief Indicate ANC adaptive filter stopped \ref T_AUDIO_EVENT_PARAM_ANC_ADAPTIVE_FILTER_STOPPED.
     */
    AUDIO_EVENT_ANC_ADAPTIVE_FILTER_STOPPED         = 0x0305,

    /**
     * \brief Indicate Passthrough enabled \ref T_AUDIO_EVENT_PARAM_AUDIO_PASSTHROUGH_ENABLED.
     */
    AUDIO_EVENT_PASSTHROUGH_ENABLED                 = 0x0310,

    /**
     * \brief Indicate Passthrough disabled \ref T_AUDIO_EVENT_PARAM_AUDIO_PASSTHROUGH_DISABLED.
     */
    AUDIO_EVENT_PASSTHROUGH_DISABLED                = 0x0311,

    /**
     * \brief Indicate Line-in started \ref T_AUDIO_EVENT_PARAM_LINE_IN_STARTED.
     */
    AUDIO_EVENT_LINE_IN_STARTED                     = 0x0400,

    /**
     * \brief Indicate Line-in stopped \ref T_AUDIO_EVENT_PARAM_LINE_IN_STOPPED.
     */
    AUDIO_EVENT_LINE_IN_STOPPED                     = 0x0401,

    /**
     * \brief Indicate Line-in volume out changed \ref T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_OUT_CHANGED.
     */
    AUDIO_EVENT_LINE_IN_VOLUME_OUT_CHANGED          = 0x0402,

    /**
     * \brief Indicate Line-in volume in changed \ref T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_IN_CHANGED.
     */
    AUDIO_EVENT_LINE_IN_VOLUME_IN_CHANGED           = 0x0403,

    /**
     * \brief Indicate Line-in volume balance changed \ref T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_BALANCE_CHANGED.
     */
    AUDIO_EVENT_LINE_IN_VOLUME_BALANCE_CHANGED      = 0x0404,

    /**
     * \brief Indicate Line-in signal out strength in dB \ref AUDIO_EVENT_PARAM_LINE_IN_SIGNAL_OUT_REFRESHED.
     */
    AUDIO_EVENT_LINE_IN_SIGNAL_OUT_REFRESHED        = 0x0405,

    /**
     * \brief Indicate buffer state turns to playing.
     */
    AUDIO_EVENT_BUFFER_STATE_PALYING                = 0x0406,
} T_AUDIO_EVENT;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume out changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_changed
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume out muted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_muted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume out unmuted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_out_unmuted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume in changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_changed
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume in muted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_muted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume in unmuted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_in_unmuted
{
    T_AUDIO_STREAM_TYPE type;
    uint8_t             volume;
} T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter audio volume balance changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_volume_balance_changed
{
    T_AUDIO_STREAM_TYPE type;
    float               prev_scale;
    float               curr_scale;
} T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_started
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_STARTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS alerted.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_alerted
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_ALERTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS paused.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_paused
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_PAUSED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS resumed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_resumed
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_RESUMED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_stopped
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_STOPPED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter TTS exceeded.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_tts_exceeded
{
    T_TTS_HANDLE handle;
} T_AUDIO_EVENT_PARAM_TTS_EXCEEDED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter ringtone started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_ringtone_started
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_RINGTONE_STARTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter ringtone stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_ringtone_stopped
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter voice prompt started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_voice_prompt_started
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter passthrough started.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_passthrough_started
{
    T_AUDIO_PASSTHROUGH_MODE     mode;
} T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter passthrough stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_passthrough_stopped
{
    T_AUDIO_PASSTHROUGH_MODE     mode;
} T_AUDIO_EVENT_PARAM_PASSTHROUGH_STOPPED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter voice prompt stopped.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_voice_prompt_stopped
{
    uint8_t     index;
} T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED;

/**
 * audio.h
 *
 * \brief   Audio track state changed event parameter for \ref AUDIO_EVENT_TRACK_STATE_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_state_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    T_AUDIO_TRACK_STATE  state;
    uint8_t              cause;
} T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED;

/**
 * audio.h
 *
 * \brief   Audio track volume out changed event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              prev_volume;
    uint8_t              curr_volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED;

/**
 * audio.h
 *
 * \brief   Audio track volume out muted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_MUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_muted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED;

/**
 * audio.h
 *
 * \brief   Audio track volume out umuted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_OUT_UNMUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_out_unmuted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED;

/**
 * audio.h
 *
 * \brief   Audio track volume in changed event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_CHANGED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_changed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              prev_volume;
    uint8_t              curr_volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED;

/**
 * audio.h
 *
 * \brief   Audio track volume in muted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_MUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_muted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED;

/**
 * audio.h
 *
 * \brief   Audio track volume in umuted event parameter for \ref AUDIO_EVENT_TRACK_VOLUME_IN_UNMUTED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_volume_in_unmuted
{
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t              volume;
} T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED;

/**
 * audio.h
 *
 * \brief   Audio track buffer low event parameter for \ref AUDIO_EVENT_TRACK_BUFFER_LOW.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_buffer_low
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t  buffer_level_ms;
} T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW;

/**
 * audio.h
 *
 * \brief   Audio track buffer high event parameter for \ref AUDIO_EVENT_TRACK_BUFFER_HIGH.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_buffer_high
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t  buffer_level_ms;
} T_AUDIO_EVENT_PARAM_TRACK_BUFFER_HIGH;

/**
 * audio.h
 *
 * \brief   Audio track data indicated event parameter for \ref AUDIO_EVENT_TRACK_DATA_IND.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_data_ind
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t             len;
} T_AUDIO_EVENT_PARAM_TRACK_DATA_IND;

/**
 * audio.h
 *
 * \brief   Audio track data indicated event parameter for \ref AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_signal_out_refreshed
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t             left_gain;
    uint16_t             right_gain;
} T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED;

/**
 * audio.h
 *
 * \brief   Audio track data indicated event parameter for \ref T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED.
 *
 * \details preset_latency           Audio Track preset latency in milliseconds.
 *          instant_latency          Audio Track stream's instant latency from reception to decoding.
 *          dejitter_compensation    Audio Track stream's de-jitter compensation based on preset latency.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_track_latency_reported
{
    T_AUDIO_TRACK_HANDLE handle;
    uint16_t             preset_latency;
    uint16_t             instant_latency;
    int32_t              dejitter_compensation;
} T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter line-in volume out changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_in_volume_out_changed
{
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_OUT_CHANGED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter line-in volume in changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_in_volume_in_changed
{
    uint8_t             prev_volume;
    uint8_t             curr_volume;
} T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_IN_CHANGED;

/**
 * audio.h
 *
 * \brief  Audio manager event parameter line-in volume balance changed.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_in_volume_balance_changed
{
    float               prev_scale;
    float               curr_scale;
} T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_BALANCE_CHANGED;

/**
 * audio.h
 *
 * \brief   Line-in signal out refreshed event parameter for \ref AUDIO_EVENT_LINE_IN_SIGNAL_OUT_REFRESHED.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef struct t_audio_event_param_line_in_signal_out_refreshed
{
    uint16_t             left_gain;
    uint16_t             right_gain;
} T_AUDIO_EVENT_PARAM_LINE_IN_SIGNAL_OUT_REFRESHED;

/**
 * audio.h
 *
 * \brief   Define Audio manager event parameter for \ref T_AUDIO_EVENT.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef union t_audio_event_param
{
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_CHANGED          volume_out_changed;
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_MUTED            volume_out_muted;
    T_AUDIO_EVENT_PARAM_VOLUME_OUT_UNMUTED          volume_out_unmuted;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_CHANGED           volume_in_changed;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_MUTED             volume_in_muted;
    T_AUDIO_EVENT_PARAM_VOLUME_IN_UNMUTED           volume_in_unmuted;
    T_AUDIO_EVENT_PARAM_VOLUME_BALANCE_CHANGED      volume_balance_changed;

    T_AUDIO_EVENT_PARAM_TTS_STARTED                 tts_started;
    T_AUDIO_EVENT_PARAM_TTS_ALERTED                 tts_alerted;
    T_AUDIO_EVENT_PARAM_TTS_PAUSED                  tts_paused;
    T_AUDIO_EVENT_PARAM_TTS_RESUMED                 tts_resumed;
    T_AUDIO_EVENT_PARAM_TTS_STOPPED                 tts_stopped;
    T_AUDIO_EVENT_PARAM_TTS_EXCEEDED                tts_exceeded;

    T_AUDIO_EVENT_PARAM_RINGTONE_STARTED            ringtone_started;
    T_AUDIO_EVENT_PARAM_RINGTONE_STOPPED            ringtone_stopped;

    T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STARTED        voice_prompt_started;
    T_AUDIO_EVENT_PARAM_VOICE_PROMPT_STOPPED        voice_prompt_stopped;

    T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED         passthrough_started;
    T_AUDIO_EVENT_PARAM_PASSTHROUGH_STOPPED         passthrough_stopped;

    T_AUDIO_EVENT_PARAM_TRACK_STATE_CHANGED         track_state_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_CHANGED    track_volume_out_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_MUTED      track_volume_out_muted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_OUT_UNMUTED    track_volume_out_unmuted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_CHANGED     track_volume_in_changed;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_MUTED       track_volume_in_muted;
    T_AUDIO_EVENT_PARAM_TRACK_VOLUME_IN_UNMUTED     track_volume_in_unmuted;
    T_AUDIO_EVENT_PARAM_TRACK_DATA_IND              track_data_ind;
    T_AUDIO_EVENT_PARAM_TRACK_SIGNAL_OUT_REFRESHED  track_signal_out_refreshed;
    T_AUDIO_EVENT_PARAM_TRACK_LATENCY_REPORTED      track_latency_reported;

    T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_OUT_CHANGED  line_in_volume_out_changed;
    T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_IN_CHANGED   line_in_volume_in_changed;
    T_AUDIO_EVENT_PARAM_LINE_IN_VOLUME_BALANCE_CHANGED line_in_volume_balance_changed;
    T_AUDIO_EVENT_PARAM_LINE_IN_SIGNAL_OUT_REFRESHED  line_in_signal_out_refreshed;
} T_AUDIO_EVENT_PARAM;

/**
 * audio.h
 *
 * \brief  Audio manager event callback prototype.
 *
 * \param[in] event_type    Audio manager event type.
 * \param[in] event_buf     Event buffer address.
 * \param[in] buf_len       Event buffer length.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
typedef void (*P_AUDIO_MGR_CBACK)(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len);

/**
 * audio.h
 *
 * \brief  Audio manager event callback register.
 *
 * \param[in] cback    Callback function \ref P_AUDIO_MGR_CBACK.
 *
 * \return    The status of audio manager event callback register.
 * \retval true    Audio manager event callback was registered successfully.
 * \retval false   Audio manager event callback was failed to register.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_cback_register(P_AUDIO_MGR_CBACK cback);

/**
 * audio.h
 *
 * \brief  Audio manager event callback unregister.
 *
 * \param[in] cback    Callback function \ref P_AUDIO_MGR_CBACK.
 *
 * \return    The status of audio manager event callback unregister.
 * \retval true    Audio manager event callback was unregistered successfully.
 * \retval false   Audio manager event callback was failed to unregister.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_cback_unregister(P_AUDIO_MGR_CBACK cback);

/**
 * audio.h
 *
 * \brief  Initialize audio manager.
 *
 * \param[in] playback_pool_size        playback pool size in bytes.
 * \param[in] voice_pool_size           voice pool size in bytes.
 * \param[in] record_pool_size          record pool size in bytes.
 * \param[in] notification_pool_size    notification pool size in bytes.
 *
 * \return    The status of initializing audio manager.
 * \retval true    Audio manager was initialized successfully.
 * \retval false   Audio manager was failed to initialize.
 *
 * \ingroup AUDIO_MANAGER_AUDIO
 */
bool audio_mgr_init(uint16_t playback_pool_size,
                    uint16_t voice_pool_size,
                    uint16_t record_pool_size,
                    uint16_t notification_pool_size);

void audio_mgr_set_max_plc_count(uint8_t plc_count);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_H_ */
