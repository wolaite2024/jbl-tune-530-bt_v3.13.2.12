/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_TRACK_H_
#define _AUDIO_TRACK_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_TRACK Audio Track
 *
 * \brief   Create, control and destroy Audio Track sessions.
 * \details Each Audio Track represents an audio stream path from the low level hardware, which shall
 *          be unidirectional or bidirectional for the input (record) or output (playback) streams,
 *          and be attached to the audio devices. The application can pass data in and drain data out
 *          by writing to and reading from the Audio Track, or adjust the gain level by controlling
 *          the Audio Track volumes.
 */


/**
 * audio_track.h
 *
 * \brief Define Audio Track states.
 *
 * \ingroup AUDIO_TRACK
 */
typedef enum t_audio_track_state
{
    AUDIO_TRACK_STATE_RELEASED      = 0x00, /**< Audio track that was released or not created yet. */
    AUDIO_TRACK_STATE_CREATED       = 0x01, /**< Audio track that was created. */
    AUDIO_TRACK_STATE_STARTED       = 0x02, /**< Audio track that was started. */
    AUDIO_TRACK_STATE_STOPPED       = 0x03, /**< Audio track that was stopped. */
    AUDIO_TRACK_STATE_PAUSED        = 0x04, /**< Audio track that was paused. */
    AUDIO_TRACK_STATE_RESTARTED     = 0x05, /**< Audio track that was restarted. */
} T_AUDIO_TRACK_STATE;

/**
 * audio_track.h
 *
 * \brief Define Audio Track causes.
 *
 * \ingroup AUDIO_TRACK
 */
typedef enum t_audio_track_cause
{
    AUDIO_TRACK_CAUSE_NONE                  = 0x00, /**< Audio track that was handled successfully. */
    AUDIO_TRACK_CAUSE_HW_ERROR              = 0x01, /**< Audio track that encountered low layer hardware error. */
    AUDIO_TRACK_CAUSE_BUFFER_EMPTY          = 0x02, /**< Audio track that encountered buffer empty. */
    AUDIO_TRACK_CAUSE_REMOTE_JOIN_FAILED    = 0x03, /**< Audio track that encountered remote join failure. */
    AUDIO_TRACK_CAUSE_JOIN_PACKET_LOST      = 0x04, /**< Audio track that encountered too many packets lost. */
    AUDIO_TRACK_CAUSE_UNKNOWN_ERROR         = 0xFF, /**< Audio track that encountered unknown error. */
} T_AUDIO_TRACK_CAUSE;

/**
 * audio_track.h
 *
 * \brief Define Audio Track policy.
 *
 * \details Audio Track can set its stream policy.
 *          \ref AUDIO_TRACK_POLICY_SINGLE_STREAM is the default policy that only one running Audio
 *          Track instance for either decoding path or encoding path. The Audio Track instance with
 *          stream type \ref AUDIO_STREAM_TYPE_VOICE will preempt the Audio Track instance with stream
 *          type \ref AUDIO_STREAM_TYPE_PLAYBACK. The Audio Track instance with stream type \ref
 *          AUDIO_STREAM_TYPE_VOICE cannot co-exist with the Audio Track instance with stream type \ref
 *          AUDIO_STREAM_TYPE_RECORD.
 *          \ref AUDIO_TRACK_POLICY_MULTI_STREAM is the policy that multi Audio Track instances for
 *          either decoding path or encoding path can co-exist. Application can control each Audio
 *          Track instance's lifecycle individually.
 *
 * \ingroup AUDIO_TRACK
 */
typedef enum t_audio_track_policy
{
    AUDIO_TRACK_POLICY_SINGLE_STREAM = 0x00, /**< Audio Track single stream policy */
    AUDIO_TRACK_POLICY_MULTI_STREAM  = 0x01, /**< Audio Track multi stream policy */
} T_AUDIO_TRACK_POLICY;

/**
 * audio_track.h
 *
 * \brief Define the Audio Track session handle.
 *
 * \ingroup AUDIO_TRACK
 */
typedef void *T_AUDIO_TRACK_HANDLE;

/**
 * audio_track.h
 *
 * \brief Define Audio Track async read/write IO operations.
 *
 * \details Each Audio Track session can be unidirectional or bidirectional. when the stream type is
 *          set as \ref AUDIO_STREAM_TYPE_PLAYBACK in \ref audio_track_create, \ref P_AUDIO_TRACK_ASYNC_IO
 *          can be implemented as an async write operation; when the stream type is set as \ref
 *          AUDIO_STREAM_TYPE_RECORD, \ref P_AUDIO_TRACK_ASYNC_IO can be implemented as an async
 *          read operation; when the stream type is set as \ref AUDIO_STREAM_TYPE_VOICE,
 *          \ref P_AUDIO_TRACK_ASYNC_IO can be implemented as either an async write or an async read
 *          respectively.

 * \note    If P_AUDIO_TRACK_ASYNC_IO is set as NULL in \ref audio_track_create, the Audio Track
 *          shall use the sync read interface \ref audio_track_read to drain data from, or use the
 *          sync write interface \ref audio_track_write to pass data in.
 *
 * \param[in]     handle       The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in,out] timestamp    The timestamp of the buffer passed into or passed out from Audio Track.
 * \param[in,out] seq_num      The sequence number of the buffer passed into or passed out from Audio Track.
 * \param[in,out] status       The frame status of the buffer passed into or passed out from Audio Track.
 * \param[in,out] frame_num    The frame number of the buffer passed into or passed out from Audio Track.
 * \param[in,out] buf          The buffer that holds the audio data written to or read from the Audio Track.
 * \param[in]     required_len The required size in bytes written to or read from the Audio Track.
 * \param[out]    actual_len   The actual size in bytes written to or read from the Audio Track.
 *
 * \ingroup AUDIO_TRACK
 */
typedef bool (*P_AUDIO_TRACK_ASYNC_IO)(T_AUDIO_TRACK_HANDLE   handle,
                                       uint32_t              *timestamp,
                                       uint16_t              *seq_num,
                                       T_AUDIO_STREAM_STATUS *status,
                                       uint8_t               *frame_num,
                                       void                  *buf,
                                       uint16_t               required_len,
                                       uint16_t              *actual_len);

/**
 * audio_track.h
 *
 * \brief   Set Audio Track stream policy.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] policy    The Audio Track policy \ref T_AUDIO_TRACK_POLICY.
 *
 * \return  The status of setting Audio Track policy.
 * \retval  true    Audio Track policy was set successfully.
 * \retval  false   Audio Track policy was failed to set.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_policy_set(T_AUDIO_TRACK_POLICY policy);

/**
 * audio_track.h
 *
 * \brief   Create an Audio Track session instance.
 *
 * \details Audio system may have multiple coexisting or exclusive low-level audio data paths; if the
 *          application wants to control the specific audio data path, it should first create an
 *          Audio Track that is abstract from the low-level hardware device routing. The operations
 *          upon Audio Track shall focus on the corresponding Audio Track handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>created</b> state when the current state \ref AUDIO_TRACK_STATE_CREATED in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *
 * \param[in] stream_type   The stream type \ref T_AUDIO_STREAM_TYPE of the Audio Track.
 * \param[in] mode          The stream mode \ref T_AUDIO_STREAM_MODE of the Audio Track.
 * \param[in] usage         The stream usage \ref T_AUDIO_STREAM_USAGE of the Audio Track.
 * \param[in] format_info   The encoding/decoding format information \ref T_AUDIO_FORMAT_INFO of the Audio Track.
 * \param[in] volume_out    The stream volume out level of the Audio Track, and the range is between \ref
                            audio_volume_out_min_get and \ref audio_volume_out_max_get.
 * \param[in] volume_in     The stream volume in level of the Audio Track, and the range is between \ref
                            audio_volume_in_min_get and \ref audio_volume_in_max_get.
 * \param[in] device        The device bitmask \ref AUDIO_DEVICE_BITMASK of the Audio Track.
 * \param[in] async_write   The async write operation \ref P_AUDIO_TRACK_ASYNC_IO of the Audio Track.
 * \param[in] async_read    The async read operation \ref P_AUDIO_TRACK_ASYNC_IO of the Audio Track.
 *
 * \return  The instance handle of Audio Track session. If returned handle is NULL, the Audio Track
 *          session instance was failed to create.
 *
 * \ingroup AUDIO_TRACK
 */
T_AUDIO_TRACK_HANDLE audio_track_create(T_AUDIO_STREAM_TYPE    stream_type,
                                        T_AUDIO_STREAM_MODE    mode,
                                        T_AUDIO_STREAM_USAGE   usage,
                                        T_AUDIO_FORMAT_INFO    format_info,
                                        uint8_t                volume_out,
                                        uint8_t                volume_in,
                                        uint32_t               device,
                                        P_AUDIO_TRACK_ASYNC_IO async_write,
                                        P_AUDIO_TRACK_ASYNC_IO async_read);

/**
 * audio_track.h
 *
 * \brief   Start playing the Audio Track session.
 *
 * \details Audio Track shall be started before writing to or reading from the Audio Track session,
 *          otherwise, data may be dropped for writing or be empty for reading. When the Audio Track
 *          session is stopped by \ref audio_track_stop, this API can resume the Audio Track session.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>started</b> state when the current state \ref AUDIO_TRACK_STATE_STARTED in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *          Starting an Audio Track session already in <b>started</b> state will be ignored.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of starting the Audio Track.
 * \retval true     Audio Track was started successfully.
 * \retval false    Audio Track was failed to start.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_start(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Stop the Audio Track session.
 *
 * \details Audio playback or record will be paused, but data queued in the track buffer will be
 *          discarded. While the following \ref audio_track_write or \ref audio_track_read upon the
 *          stopped Audio Track session will be ignored.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>stopped</b> state when the current state \ref AUDIO_TRACK_STATE_STOPPED in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *          Stopping an Audio Track session already in <b>stopped</b> state will be ignored.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of stopping the Audio Track.
 * \retval true     Audio Track was stopped successfully.
 * \retval false    Audio Track was failed to stop.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_stop(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Pause the Audio Track session.
 *
 * \details Audio playback or record will be paused, but data queued in the track buffer will not be
 *          discarded. While the following \ref audio_track_write or \ref audio_track_read upon the
 *          paused Audio Track session will be ignored.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>paused</b> state when the current state \ref AUDIO_TRACK_STATE_PAUSED in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *          Pausing an Audio Track session already in <b>paused</b> state will be ignored.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of pausing the Audio Track.
 * \retval true     Audio Track was paused successfully.
 * \retval false    Audio Track was failed to pause.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_pause(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Restart the Audio Track session.
 *
 * \details Audio playback or record will be Restarted, but data queued in the track buffer will be
 *          discarded. While the following \ref audio_track_write or \ref audio_track_read upon the
 *          Restarting Audio Track session will be ignored.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>started</b> state when the current state \ref AUDIO_TRACK_STATE_RESTARTED in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *          Restaring an Audio Track session already in <b>restarting</b> state will be ignored.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of restarting the Audio Track.
 * \retval true     Audio Track was restarted successfully.
 * \retval false    Audio Track was failed to restart.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_restart(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Flush data in the Audio Track session.
 *
 * \details Data currently queued in the track buffer will be flushed. While the following
 *          \ref audio_track_write or \ref audio_track_read upon the recently flushed Audio Track
 *          session will be valid.
 *
 * \note    Flushing an Audio Track in <b>started</b> state is forbidden and returns failed.
 *          In order to purge the Audio Track session safely, the Audio Track can be flushed
 *          immediately after the Audio Track session created.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of flushing the Audio Track.
 * \retval true     Audio Track was flushed successfully.
 * \retval false    Audio Track was failed to flush.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_flush(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Release the Audio Track session instance.
 *
 * \details All resources and low-level hardware configurations associated with the Audio Track
 *          session will be released.
 *
 * \note    This is an async function, and the Audio Track session state will be transformed into the
 *          <b>released</b> state when the current state \ref AUDIO_TRACK_STATE_RELEASE in Audio event
 *          \ref AUDIO_EVENT_TRACK_STATE_CHANGED is received.
 *
 * \note    It is recommended that the Audio Track session should be stopped before released.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return          The status of releasing the Audio Track.
 * \retval true     Audio Track was released successfully.
 * \retval false    Audio Track was failed to release.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_release(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Write the audio data to the Audio Track session.
 *
 * \details When the Audio Track session is started by \ref audio_track_start, the application can
 *          write data to the Audio Track session. Refer to \ref P_AUDIO_TRACK_ASYNC_IO for
 *          async IO operation details.
 *
 * \param[in]  handle       The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in]  timestamp    The timestamp of the buffer passed into Audio subsystem.
 * \param[in]  seq_num      The sequence number of the buffer passed into Audio subsystem.
 * \param[in]  status       The frame status of the buffer passed into Audio subsystem.
 * \param[in]  frame_num    The frame number of the buffer passed into Audio subsystem.
 * \param[in]  buf          The buffer that holds the playback audio data.
 * \param[in]  len          The required size in bytes written from the buffer.
 * \param[out] written_len  The actual size in bytes written from the buffer.
 *
 * \return          The status of writing the audio data.
 * \retval true     Audio data was written successfully.
 * \retval false    Audio data was failed to write.
 *
 * \note    If the returned status is false, application can stop and release the Audio Track
 *          session. If the returned status is true, and the actual written buffer length is
 *          zero or any positive number less than the required buffer length, application
 *          can retry or discard the remaining buffer data.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_write(T_AUDIO_TRACK_HANDLE   handle,
                       uint32_t               timestamp,
                       uint16_t               seq_num,
                       T_AUDIO_STREAM_STATUS  status,
                       uint8_t                frame_num,
                       void                  *buf,
                       uint16_t               len,
                       uint16_t              *written_len);

/**
 * audio_track.h
 *
 * \brief   Read the audio data from the Audio Track session.
 *
 * \details When the Audio Track session is started by \ref audio_track_start, the application can
 *          read data from the Audio Track session. Refer to \ref P_AUDIO_TRACK_ASYNC_IO for
 *          async IO operation details.
 *
 * \param[in]  handle       The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] timestamp    The timestamp of the buffer passed out from Audio subsystem.
 * \param[out] seq_num      The sequence number of the buffer passed out from Audio subsystem.
 * \param[out] status       The frame status of the buffer passed out from Audio subsystem.
 * \param[out] frame_num    The frame number of the buffer passed out from Audio subsystem.
 * \param[out] buf          The buffer that holds the recording audio data.
 * \param[in]  len          The required size in bytes read from the Audio Track.
 * \param[out] read_len     The actual size in bytes read from the Audio Track.
 *
 * \return          The status of reading the audio data.
 * \retval true     Audio data was read successfully.
 * \retval false    Audio data was failed to read.
 *
 * \note    If the returned status is false, application can stop and release the Audio Track
 *          session. If the returned status is true, and the actual read buffer length is zero
 *          or any positive number less than the required buffer length, application can retry
 *          or terminate the read operation.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_read(T_AUDIO_TRACK_HANDLE   handle,
                      uint32_t              *timestamp,
                      uint16_t              *seq_num,
                      T_AUDIO_STREAM_STATUS *status,
                      uint8_t               *frame_num,
                      void                  *buf,
                      uint16_t               len,
                      uint16_t              *read_len);

/**
 * audio_track.h
 *
 * \brief   Set the threshold of the specific Audio Track.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in]  upper_threshold  The upper threshold of the Audio Track buffer in milliseconds.
 * \param[in]  lower_threshold  The lower threshold of the Audio Track buffer in milliseconds.
 *
 * \note    Upper threshold and lower threshold both range from 0 to current Audio Track latency,
 *          and upper threshold shall be larger than lower threshold.
 *
 * \return          The status of setting the Audio Track buffer threshold.
 * \retval true     Audio Track buffer threshold was get successfully.
 * \retval false    Audio Track buffer threshold was failed to set.
 *
 * \ingroup AUDIO_TRACK
 */

bool audio_track_threshold_set(T_AUDIO_TRACK_HANDLE handle,
                               uint16_t             upper_threshold,
                               uint16_t             lower_threshold);


/**
 * audio_track.h
 *
 * \brief   Get the state of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] state    The current state \ref T_AUDIO_TRACK_STATE of the Audio Track session.
 *
 * \return          The status of getting the Audio Track state.
 * \retval true     Audio Track state was got successfully.
 * \retval false    Audio Track state was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_state_get(T_AUDIO_TRACK_HANDLE handle,
                           T_AUDIO_TRACK_STATE *state);

/**
 * audio_track.h
 *
 * \brief   Get the stream type of the specific Audio Track session.
 *
 * \param[in]  handle       The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] stream_type  The stream type \ref T_AUDIO_STREAM_TYPE of the Audio Track session.
 *
 * \return          The status of getting the Audio Track stream type.
 * \retval true     Audio Track stream type was got successfully.
 * \retval false    Audio Track stream type was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_stream_type_get(T_AUDIO_TRACK_HANDLE handle,
                                 T_AUDIO_STREAM_TYPE *stream_type);

/**
 * audio_track.h
 *
 * \brief   Get the encoding/decoding format information of the specific Audio Track session.
 *
 * \param[in]  handle       The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] format_info  The encoding/decoding format information \ref T_AUDIO_FORMAT_TYPE of the Audio Track session.
 *
 * \return          The status of getting the Audio Track encoding/decoding format information.
 * \retval true     Audio Track encoding/decoding format information was got successfully.
 * \retval false    Audio Track encoding/decoding format information was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_format_info_get(T_AUDIO_TRACK_HANDLE handle,
                                 T_AUDIO_FORMAT_INFO *format_info);

/**
 * audio_track.h
 *
 * \brief   Get the device bitmask of the specific Audio Track session.
 *
 * \details The device shall be treated as the output sink endpoint of the playback stream type
 *          \ref AUDIO_STREAM_TYPE_PLAYBACK, the input source endpoint of the record stream type
 *          \ref AUDIO_STREAM_TYPE_RECORD, or the output and input endpoints of the voie stream
 *          type \ref AUDIO_STREAM_TYPE_VOICE.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] device   The device bitmask \ref AUDIO_DEVICE_BITMASK of the Audio Track session.
 *
 * \return          The status of getting the Audio Track device bitmask.
 * \retval true     Audio Track device bitmask was got successfully.
 * \retval false    Audio Track device bitmask was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_device_get(T_AUDIO_TRACK_HANDLE handle,
                            uint32_t *device);

/**
 * audio_track.h
 *
 * \brief   Get the stream mode of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] mode     The stream mode \ref T_AUDIO_STREAM_MODE of the Audio Track session.
 *
 * \return          The status of getting the Audio Track stream mode.
 * \retval true     Audio Track stream mode was got successfully.
 * \retval false    Audio Track stream mode was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_mode_get(T_AUDIO_TRACK_HANDLE  handle,
                          T_AUDIO_STREAM_MODE  *mode);

/**
 * audio_track.h
 *
 * \brief   Get the usage of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] usage    The usage \ref T_AUDIO_STREAM_USAGE of the Audio Track session.
 *
 * \return          The status of getting the Audio Track usage.
 * \retval true     Audio Track usage was got successfully.
 * \retval false    Audio Track usage was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_usage_get(T_AUDIO_TRACK_HANDLE  handle,
                           T_AUDIO_STREAM_USAGE *usage);

/**
 * audio_track.h
 *
 * \brief   Get the maximum stream latency of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] latency  The stream latency in milliseconds of the Audio Track session.
 *
 * \return          The status of getting the Audio Track maximum stream latency.
 * \retval true     Audio Track maximum stream latency was got successfully.
 * \retval false    Audio Track maximum stream latency was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_latency_max_get(T_AUDIO_TRACK_HANDLE handle,
                                 uint16_t *latency);

/**
 * audio_track.h
 *
 * \brief   Get the minimum stream latency of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] latency  The stream latency in milliseconds of the Audio Track session.
 *
 * \return          The status of getting the Audio Track minimum stream latency.
 * \retval true     Audio Track minimum stream latency was got successfully.
 * \retval false    Audio Track minimum stream latency was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_latency_min_get(T_AUDIO_TRACK_HANDLE handle,
                                 uint16_t *latency);

/**
 * audio_track.h
 *
 * \brief   Get the current stream latency of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] latency  The stream latency in milliseconds of the Audio Track session.
 *
 * \return          The status of getting the Audio Track current stream latency.
 * \retval true     Audio Track current stream latency was got successfully.
 * \retval false    Audio Track current stream latency was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_latency_get(T_AUDIO_TRACK_HANDLE  handle,
                             uint16_t             *latency);

/**
 * audio_track.h
 *
 * \brief   Set the current stream latency of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] latency   The stream latency in milliseconds of the Audio Track session.
 * \param[in] fixed     The latency is fixed and cannot be adjusted dynamically.
 *
 * \return          The status of setting the Audio Track current stream latency.
 * \retval true     Audio Track current stream latency was set successfully.
 * \retval false    Audio Track current stream latency was failed to set.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_latency_set(T_AUDIO_TRACK_HANDLE handle,
                             uint16_t             latency,
                             bool                 fixed);

/**
 * audio_track.h
 *
 * \brief   Get the current buffer level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] level     The buffer level in milliseconds of the Audio Track session.
 *
 * \return          The status of getting the Audio Track current stream buffer level.
 * \retval true     Audio Track current stream buffer level was get successfully.
 * \retval false    Audio Track current stream buffer level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_buffer_level_get(T_AUDIO_TRACK_HANDLE handle,
                                  uint16_t            *level);


/**
 * audio_track.h
 *
 * \brief   Get the maximum volume out level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume out level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track maximum volume out level.
 * \retval true     Audio Track maximum volume out level was got successfully.
 * \retval false    Audio Track maximum volume out level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_max_get(T_AUDIO_TRACK_HANDLE  handle,
                                    uint8_t              *volume);

/**
 * audio_track.h
 *
 * \brief   Get the minimum volume out level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume out level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track minimum volume out level.
 * \retval true     Audio Track minimum volume out level was got successfully.
 * \retval false    Audio Track minimum volume out level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_min_get(T_AUDIO_TRACK_HANDLE  handle,
                                    uint8_t              *volume);

/**
 * audio_track.h
 *
 * \brief   Get the current volume out level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume out level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track current volume out level.
 * \retval true     Audio Track current out volume level was got successfully.
 * \retval false    Audio Track current out volume level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_get(T_AUDIO_TRACK_HANDLE  handle,
                                uint8_t              *volume);

/**
 * audio_track.h
 *
 * \brief   Set the current volume out level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] volume    The volume out level of the Audio Track session, and the range is between \ref
                        audio_volume_out_min_get and \ref audio_volume_out_max_get.
 *
 * \return          The status of setting the Audio Track current volume out level.
 * \retval true     Audio Track current volume out level was set successfully.
 * \retval false    Audio Track current volume out level was failed to set.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_set(T_AUDIO_TRACK_HANDLE handle,
                                uint8_t              volume);

/**
 * audio_track.h
 *
 * \brief   Mute the current volume out level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  The status of muting the Audio Track current volume out level.
 * \retval  true    Audio Track current volume out level was muted successfully.
 * \retval  false   Audio Track current volume out level was failed to mute.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_mute(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Unmute the current volume out level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  The status of unmuting the Audio Track current volume out level.
 * \retval  true    Audio Track current volume out level was unmuted successfully.
 * \retval  false   Audio Track current volume out level was failed to unmute.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_unmute(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Get the current volume out mute status of the specific Audio Track session.
 *
 * \param[in]  handle     The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] is_muted   The volume out mute status of the Audio Track session.
 *
 * \return          The result of getting the Audio Track current volume out mute status.
 * \retval true     Audio Track current volume out mute status was got successfully.
 * \retval false    Audio Track current volume out mute status was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_out_is_muted(T_AUDIO_TRACK_HANDLE  handle,
                                     bool                 *is_muted);

/**
 * audio_track.h
 *
 * \brief  Start monitoring the signal out strength of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was started successfully.
 * \retval  false   Monitoring the signal out strength was failed to start.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_signal_out_monitoring_start(T_AUDIO_TRACK_HANDLE handle,
                                             uint16_t             refresh_interval);

/**
 * audio_track.h
 *
 * \brief  Stop monitoring the signal out strength of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was stopped successfully.
 * \retval  false   Monitoring the signal out strength was failed to stop.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_signal_out_monitoring_stop(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief  Start monitoring the signal in strength of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was started successfully.
 * \retval  false   Monitoring the signal out strength was failed to start.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_signal_in_monitoring_start(T_AUDIO_TRACK_HANDLE handle,
                                            uint16_t             refresh_interval);

/**
 * audio_track.h
 *
 * \brief  Stop monitoring the signal in strength of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was stopped successfully.
 * \retval  false   Monitoring the signal out strength was failed to stop.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_signal_in_monitoring_stop(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Get the maximum volume in level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume in level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track maximum volume in level.
 * \retval true     Audio Track maximum volume in level was got successfully.
 * \retval false    Audio Track maximum volume in level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_max_get(T_AUDIO_TRACK_HANDLE handle,
                                   uint8_t             *volume);

/**
 * audio_track.h
 *
 * \brief   Get the minimum volume in level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume in level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track minimum volume in level.
 * \retval true     Audio Track minimum volume in level was got successfully.
 * \retval false    Audio Track minimum volume in level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_min_get(T_AUDIO_TRACK_HANDLE  handle,
                                   uint8_t              *volume);

/**
 * audio_track.h
 *
 * \brief   Get the current volume in level of the specific Audio Track session.
 *
 * \param[in]  handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] volume   The volume in level of the Audio Track session.
 *
 * \return          The status of getting the Audio Track current volume in level.
 * \retval true     Audio Track current in volume level was got successfully.
 * \retval false    Audio Track current in volume level was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_get(T_AUDIO_TRACK_HANDLE  handle,
                               uint8_t              *volume);

/**
 * audio_track.h
 *
 * \brief   Set the current volume in level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] volume    The volume in level of the Audio Track session, and the range is between \ref
                        audio_volume_in_min_get and \ref audio_volume_in_max_get.
 *
 * \return          The status of setting the Audio Track current volume in level.
 * \retval true     Audio Track current volume in level was set successfully.
 * \retval false    Audio Track current volume in level was failed to set.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_set(T_AUDIO_TRACK_HANDLE handle,
                               uint8_t              volume);

/**
 * audio_track.h
 *
 * \brief   Mute the current volume in level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  The status of muting the Audio Track current volume in level.
 * \retval  true    Audio Track current volume in level was muted successfully.
 * \retval  false   Audio Track current volume in level was failed to mute.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_mute(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Unmute the current volume in level of the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 *
 * \return  The status of unmuting the Audio Track current volume in level.
 * \retval  true    Audio Track current volume in level was unmuted successfully.
 * \retval  false   Audio Track current volume in level was failed to unmute.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_unmute(T_AUDIO_TRACK_HANDLE handle);

/**
 * audio_track.h
 *
 * \brief   Get the current volume in mute status of the specific Audio Track session.
 *
 * \param[in]  handle     The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[out] is_muted   The volume in mute status of the Audio Track session.
 *
 * \return          The result of getting the Audio Track current volume in mute status.
 * \retval true     Audio Track current volume in mute status was got successfully.
 * \retval false    Audio Track current volume in mute status was failed to get.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_volume_in_is_muted(T_AUDIO_TRACK_HANDLE  handle,
                                    bool                 *is_muted);

/**
 * audio_track.h
 *
 * \brief   Decide the current a2dp input data is silent or not by the length of the data.
 *
 * \param[in]  handle           The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in]  packet_length    The length of a2dp input data.
 *
 * \return          The result of the status of the current a2dp input data.
 * \retval true     The current a2dp input data is silent packet.
 * \retval false    The current a2dp input data is not silent packet.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_data_in_is_silent(T_AUDIO_TRACK_HANDLE  handle,
                                   uint16_t              packet_length);

/**
 * audio_track.h
 *
 * \brief   Attach the Audio Effect instance to the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of attaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was attached successfully.
 * \retval  false   Audio Effect instance was failed to attach.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_effect_attach(T_AUDIO_TRACK_HANDLE    handle,
                               T_AUDIO_EFFECT_INSTANCE instance);

/**
 * audio_track.h
 *
 * \brief   Detach the Audio Effect instance from the specific Audio Track session.
 *
 * \param[in] handle    The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of detaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was detached successfully.
 * \retval  false   Audio Effect instance was failed to detach.
 *
 * \ingroup AUDIO_TRACK
 */
bool audio_track_effect_detach(T_AUDIO_TRACK_HANDLE    handle,
                               T_AUDIO_EFFECT_INSTANCE instance);

bool audio_remote_join_set(bool enable, bool force_join);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_TRACK_H_ */
