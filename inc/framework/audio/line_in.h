/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _LINE_IN_H_
#define _LINE_IN_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    LINE_IN Line In
 *
 * \brief   Start, stop and control the line-in playback.
 * \details Line-in is either a digital or an analog jack that can be connected to another audio
 *          output device. Line-in is used to record, relay, or amplify the incoming audio stream.
 */


/**
 * line_in.h
 *
 * \brief   Create a line-in playback instance.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  sample_rate    Sample rate for the line-in playback instance. The supported values are 48000 and 96000.
 *
 * \note Line-in is a singleton design that the application shall destroy the instance before creating a new instance.
 *
 * \return          The status of creating a line-in playback instance.
 * \retval  true    Line-in playback instance was created successfully.
 * \retval  false   Line-in playback instance was failed to create.
 *
 * \ingroup LINE_IN
 */
bool line_in_create(uint32_t sample_rate);

/**
 * line_in.h
 *
 * \brief   Destroy the line-in playback instance.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \return          The status of destroying line-in playback instance.
 * \retval  true    Line-in playback instance was destroyed successfully.
 * \retval  false   Line-in playback instance was failed to destroy.
 *
 * \ingroup LINE_IN
 */
bool line_in_destroy(void);

/**
 * line_in.h
 *
 * \brief   Start the line-in playback.
 *
 * \return          The status of starting line-in playback.
 * \retval  true    Line-in playback was started successfully.
 * \retval  false   Line-in playback mode was failed to start.
 *
 * \ingroup LINE_IN
 */
bool line_in_start(void);

/**
 * line_in.h
 *
 * \brief   Stop the line-in playback.
 *
 * \return          The status of stopping line-in playback mode.
 * \retval  true    Line-in playback mode was stopped successfully.
 * \retval  false   Line-in playback mode was failed to stop.
 *
 * \ingroup LINE_IN
 */
bool line_in_stop(void);

/**
 * line_in.h
 *
 * \brief   Get the maximum volume out level of the line-in stream.
 *
 * \return  The maximum volume out level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_out_max_get(void);

/**
 * line_in.h
 *
 * \brief   Set the maximum volume out level of the line-in stream.
 *
 * \param[in]   level   The maximum volume out level to set.
 *
 * \return  The status of setting the maximum volume out level.
 * \retval  true    The maximum volume out level was set successfully.
 * \retval  false   The maximum volume out level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_out_max_set(uint8_t level);

/**
 * line_in.h
 *
 * \brief   Get the minimum volume out level of the line-in stream.
 *
 * \return  The minimum volume out level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_out_min_get(void);

/**
 * line_in.h
 *
 * \brief   Set the minimum volume out level of the line-in stream.
 *
 * \param[in]   volume  The minimum volume out level to set.
 *
 * \return  The status of setting the minimum volume out level.
 * \retval  true    The minimum volume out level was set successfully.
 * \retval  false   The minimum volume out level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_out_min_set(uint8_t volume);

/**
 * line_in.h
 *
 * \brief   Get the current volume out level of the line-in stream.
 *
 * \return  The current volume out level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_out_get(void);

/**
 * line_in.h
 *
 * \brief   Set the current volume out level of the line-in stream.
 *
 * \param[in]   volume  The volume out level to set.
 *
 * \return  The status of setting volume out level.
 * \retval  true    Volume out level was set successfully.
 * \retval  false   Volume out level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_out_set(uint8_t volume);

/**
 * line_in.h
 *
 * \brief   Get the maximum volume in level of the line-in stream.
 *
 * \return  The maximum volume in level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_in_max_get(void);

/**
 * line_in.h
 *
 * \brief   Set the maximum volume in level of the line-in stream.
 *
 * \param[in]   volume  The maximum volume in level to set.
 *
 * \return  The status of setting the maximum volume in level.
 * \retval  true    The maximum volume in level was set successfully.
 * \retval  false   The maximum volume in level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_in_max_set(uint8_t volume);

/**
 * line_in.h
 *
 * \brief   Get the minimum volume in level of the line-in stream.
 *
 * \return  The minimum volume in level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_in_min_get(void);

/**
 * line_in.h
 *
 * \brief   Set the minimum volume in level of the line-in stream.
 *
 * \param[in]   volume  The minimum volume in level to set.
 *
 * \return  The status of setting the minimum volume in level.
 * \retval  true    The minimum volume in level was set successfully.
 * \retval  false   The minimum volume in level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_in_min_set(uint8_t volume);

/**
 * line_in.h
 *
 * \brief   Get the current volume in level of the line-in stream.
 *
 * \return  The current volume in level.
 *
 * \ingroup LINE_IN
 */
uint8_t line_in_volume_in_get(void);

/**
 * line_in.h
 *
 * \brief   Set the current volume in level of the line-in stream.
 *
 * \param[in]   volume  The volume in level to set.
 *
 * \return  The status of setting volume in level.
 * \retval  true    Volume in level was set successfully.
 * \retval  false   Volume in level was failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_in_set(uint8_t volume);

/**
 * line_in.h
 *
 * \brief  Start monitoring the signal out strength of line in.
 *
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was started successfully.
 * \retval  false   Monitoring the signal out strength was failed to start.
 *
 * \ingroup LINE_IN
 */
bool line_in_signal_out_monitoring_start(uint16_t refresh_interval);

/**
 * line_in.h
 *
 * \brief  Stop monitoring the signal out strength of line in.
 *
 * \return  Monitoring the signal out strength.
 * \retval  true    Monitoring the signal out strength was stopped successfully.
 * \retval  false   Monitoring the signal out strength was failed to stop.
 *
 * \ingroup LINE_IN
 */
bool line_in_signal_out_monitoring_stop(void);

/**
 * line_in.h
 *
 * \brief  Start monitoring the signal in strength of line in.
 *
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 *
 * \return  Monitoring the signal in strength.
 * \retval  true    Monitoring the signal in strength was started successfully.
 * \retval  false   Monitoring the signal in strength was failed to start.
 *
 * \ingroup LINE_IN
 */
bool line_in_signal_in_monitoring_start(uint16_t refresh_interval);

/**
 * line_in.h
 *
 * \brief  Stop monitoring the signal in strength of line in.
 *
 * \return  Monitoring the signal in strength.
 * \retval  true    Monitoring the signal in strength was stopped successfully.
 * \retval  false   Monitoring the signal in strength was failed to stop.
 *
 * \ingroup LINE_IN
 */
bool line_in_signal_in_monitoring_stop(void);

/**
 * line_in.h
 *
 * \brief   Get the volume balance scale of the line-in stream.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \return  The volume balance scale of the line-in stream.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup LINE_IN
 */
float line_in_volume_balance_get(void);

/**
 * line_in.h
 *
 * \brief   Set the delta gain values of the line-in stream.
 *
 * \param[in] left_delta   The delta gain value of left channel in unit of decibel.
 * \param[in] right_delta  The delta gain value of right channel in unit of decibel.
 *
 * \return          The status of setting the line-in stream delta gain values.
 * \retval true     The delta gain values were set successfully.
 * \retval false    The delta gain values were failed to set.
 *
 * \ingroup LINE_IN
 */
bool line_in_volume_balance_set(float left_delta, float right_delta);

/**
 * line_in.h
 *
 * \brief   Attach the Audio Effect instance to the line-in stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of attaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was attached successfully.
 * \retval  false   Audio Effect instance was failed to attach.
 *
 * \ingroup LINE_IN
 */
bool line_in_effect_attach(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * line_in.h
 *
 * \brief   Detach the Audio Effect instance from the line-in stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of detaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was detached successfully.
 * \retval  false   Audio Effect instance was failed to detach.
 *
 * \ingroup LINE_IN
 */
bool line_in_effect_detach(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINE_IN_H_ */
