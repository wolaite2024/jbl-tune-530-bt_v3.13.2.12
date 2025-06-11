/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PIPE_H_
#define _AUDIO_PIPE_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_type.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PIPE Audio Pipe
 *
 * \brief   Create, control and destroy Audio Pipe sessions.
 * \details
 */

/**
 * audio_pipe.h
 *
 * \brief Define the Audio Pipe session handle.
 *
 * \ingroup AUDIO_PIPE
 */
typedef void *T_AUDIO_PIPE_HANDLE;

/**
 * audio_pipe.h
 *
 * \brief Define the Audio Pipe callback event.
 *
 * \ingroup AUDIO_PIPE
 */
typedef enum t_audio_pipe_event
{
    AUDIO_PIPE_EVENT_RELEASED       = 0x0,
    AUDIO_PIPE_EVENT_CREATED        = 0x1,
    AUDIO_PIPE_EVENT_STARTED        = 0x2,
    AUDIO_PIPE_EVENT_STOPPED        = 0x3,
    AUDIO_PIPE_EVENT_DATA_IND       = 0x4,
    AUDIO_PIPE_EVENT_DATA_FILLED    = 0x5,
    AUDIO_PIPE_EVENT_MIXED          = 0x6,
    AUDIO_PIPE_EVENT_DEMIXED        = 0x7,
} T_AUDIO_PIPE_EVENT;

/**
 * audio_pipe.h
 *
 * \brief Define the Audio Pipe callback prototype.
 *
 * \param[in]   handle  Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   event   Audio Pipe callback event.
 * \param[in]   param   Audio Pipe callback event parameter.
 *
 * \return  The result of the callback function.
 *
 * \ingroup AUDIO_PIPE
 */
typedef bool (*P_AUDIO_PIPE_CBACK)(T_AUDIO_PIPE_HANDLE handle,
                                   T_AUDIO_PIPE_EVENT  event,
                                   uint32_t            param);

/**
 * audio_pipe.h
 *
 * \brief Create an Audio Pipe node.
 *
 * \param[in]   src_info    Audio Pipe source endpoint format info \ref T_AUDIO_FORMAT_INFO.
 * \param[in]   snk_info    Audio Pipe sink endpoint format info \ref T_AUDIO_FORMAT_INFO.
 * \param[in]   gain        The gain value(dB) of the Audio Pipe.
 * \param[in]   cback       Audio Pipe callback \ref P_AUDIO_PIPE_CBACK.
 *
 * \return  The instance handle of Audio Pipe node. If returned handle is NULL, the Audio Pipe
 *          node instance was failed to create.
 *
 * \ingroup AUDIO_PIPE
 */
T_AUDIO_PIPE_HANDLE audio_pipe_create(T_AUDIO_FORMAT_INFO src_info,
                                      T_AUDIO_FORMAT_INFO snk_info,
                                      uint16_t            gain,
                                      P_AUDIO_PIPE_CBACK  cback);

/**
 * audio_pipe.h
 *
 * \brief Release the Audio Pipe node.
 *
 * \param[in]   handle  Audio Pipe node instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of releasing the Audio Pipe.
 * \retval true     Audio Pipe was released successfully.
 * \retval false    Audio Pipe was failed to release.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_release(T_AUDIO_PIPE_HANDLE handle);

/**
 * audio_pipe.h
 *
 * \brief Start the Audio Pipe node.
 *
 * \param[in]   handle  Audio Pipe node instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of starting the Audio Pipe.
 * \retval true     Audio Pipe was started successfully.
 * \retval false    Audio Pipe was failed to start.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_start(T_AUDIO_PIPE_HANDLE handle);

/**
 * audio_pipe.h
 *
 * \brief Stop the Audio Pipe node.
 *
 * \param[in]   handle  Audio Pipe node instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of stopping the Audio Pipe.
 * \retval true     Audio Pipe was stopped successfully.
 * \retval false    Audio Pipe was failed to stop.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_stop(T_AUDIO_PIPE_HANDLE handle);

/**
 * audio_pipe.h
 *
 * \brief Get the current gain value of the specific Audio Pipe session.
 *
 * \param[in]   handle      Audio Pipe node instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  gain_left   The left channel gain value of the Audio Pipe session.
 * \param[out]  gain_right  The right channel gain value of the Audio Pipe session.
 *
 * \return          The result of getting the gain value.
 * \retval true     Gain value was got successfully.
 * \retval false    Gain value was failed to get.
 *
 * \note            Gain value is in step unit(dB * 2^7).
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_gain_get(T_AUDIO_PIPE_HANDLE handle, uint16_t *gain_left, uint16_t *gain_right);

/**
 * audio_pipe.h
 *
 * \brief Set the current gain value of the specific Audio Pipe session.
 *
 * \param[in]   handle      Audio Pipe node instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   gain_left   The left channel gain value of the Audio Pipe session.
 * \param[in]   gain_right  The right channel gain value of the Audio Pipe session.
 *
 * \return          The result of setting the gain value.
 * \retval true     Gain value was set successfully.
 * \retval false    Gain value was failed to set.
 *
 * \note            Gain value is in step unit(dB * 2^7).
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_gain_set(T_AUDIO_PIPE_HANDLE handle, uint16_t gain_left, uint16_t gain_right);

/**
 * audio_pipe.h
 *
 * \brief Fill data into the Audio Pipe source endpoint.
 *
 * \param[in]   handle       Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   timestamp    The timestamp of the buffer passed into Audio Pipe.
 * \param[in]   seq_num      The sequence number of the buffer passed into Audio Pipe.
 * \param[in]   status       The frame status of the buffer passed into Audio Pipe.
 * \param[in]   frame_num    The frame number of the buffer passed into Audio Pipe.
 * \param[in]   buf          The buffer that holds the filled data.
 * \param[in]   len          The buffer length.
 *
 * \return          The result of filling the audio data.
 * \retval true     Audio data was filled successfully.
 * \retval false    Audio data was failed to fill.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_fill(T_AUDIO_PIPE_HANDLE    handle,
                     uint32_t               timestamp,
                     uint16_t               seq_num,
                     T_AUDIO_STREAM_STATUS  status,
                     uint8_t                frame_num,
                     void                  *buf,
                     uint16_t               len);

/**
 * audio_pipe.h
 *
 * \brief Drain data from the Audio Pipe sink endpoint.
 *
 * \param[in]   handle      Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  buf         The buffer that holds the drained data.
 * \param[out]  len         The buffer length.
 * \param[out]  frame_num   The frame number.
 *
 * \return          The result of draining the audio data.
 * \retval true     Audio data was drained successfully.
 * \retval false    Audio data was failed to drain.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_drain(T_AUDIO_PIPE_HANDLE handle, void *buf, uint16_t *len, uint16_t *frame_num);

/**
 * audio_pipe.h
 *
 * \brief Flush data pending in the Audio Pipe sink endpoint.
 *
 * \param[in]   handle  Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_flush(T_AUDIO_PIPE_HANDLE handle);

/**
 * audio_pipe.h
 *
 * \brief Add the auxiliary codec pipe session to the Pre-Mixer point of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the mixing operation.
 * \retval true     The two pipes were mixed successfully.
 * \retval false    The two pipes were failed to mix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_pre_mix(T_AUDIO_PIPE_HANDLE prime_handle, T_AUDIO_PIPE_HANDLE auxiliary_handle);

/**
 * audio_pipe.h
 *
 * \brief Add the auxiliary codec pipe session to the Post-Mixer point of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the mixing operation.
 * \retval true     The two pipes were mixed successfully.
 * \retval false    The two pipes were failed to mix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_post_mix(T_AUDIO_PIPE_HANDLE prime_handle, T_AUDIO_PIPE_HANDLE auxiliary_handle);

/**
 * audio_pipe.h
 *
 * \brief Remove the auxiliary codec pipe session from the Pre-Mixer point or the Post-Mixer point of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe instance \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the demixing operation.
 * \retval true     The two pipes were demixed successfully.
 * \retval false    The two pipes were failed to demix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_demix(T_AUDIO_PIPE_HANDLE prime_handle, T_AUDIO_PIPE_HANDLE auxiliary_handle);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PIPE_H_ */
