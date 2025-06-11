/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PASSTHROUGH_H_
#define _AUDIO_PASSTHROUGH_H_

#include <stdint.h>
#include <stdbool.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PASSTHROUGH Audio Passthrough Mode
 *
 * \brief   Enable, disable and control audio passthrough mode.
 * \details Audio Passthrough Mode, or Ambient Sound Mode, uses the built-in microphones to
 *          capture noise from your surroundings and then flows noise in your headphones. In
 *          the real world, it is important to hear ambient sound even while wearing headphones,
 *          so you can detect any potentially dangerous situations.
 */

/**
 * audio_passthrough.h
 *
 * \brief Define the audio passthrough mode.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
typedef enum t_audio_passthrough_mode
{
    AUDIO_PASSTHROUGH_MODE_NORMAL       = 0x00,
    AUDIO_PASSTHROUGH_MODE_LOW_LATENCY  = 0x01,
} T_AUDIO_PASSTHROUGH_MODE;


/**
 * audio_passthrough.h
 *
 * \brief Define audio passthrough states.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
typedef enum t_audio_passthrough_state
{
    AUDIO_PASSTHROUGH_STATE_RELEASED     = 0x00, /**< Audio passthrough that was released. */
    AUDIO_PASSTHROUGH_STATE_CREATED      = 0x01, /**< Audio passthrough that was created. */
    AUDIO_PASSTHROUGH_STATE_STARTING     = 0x02, /**< Audio passthrough that was starting. */
    AUDIO_PASSTHROUGH_STATE_STARTED      = 0x03, /**< Audio passthrough that was started. */
    AUDIO_PASSTHROUGH_STATE_STOPPING     = 0x04, /**< Audio passthrough that was stopping. */
    AUDIO_PASSTHROUGH_STATE_STOPPED      = 0x05, /**< Audio passthrough that was stopped. */
} T_AUDIO_PASSTHROUGH_STATE;

/**
 * audio_passthrough.h
 *
 * \brief   Create an audio passthrough instance.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  sample_rate    Sample rate for the audio passthrough instance. The supported values are 48000 and 96000.
 *
 * \note Audio passthrough is a singleton design that the application shall destroy the instance before creating a new instance.
 *
 * \return          The status of creating audio passthrough instance.
 * \retval  true    Audio passthrough instance was created successfully.
 * \retval  false   Audio passthrough instance was failed to create.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_create(uint32_t sample_rate);

/**
 * audio_passthrough.h
 *
 * \brief   Destroy the audio passthrough instance.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \return          The status of destroying audio passthrough.
 * \retval  true    Audio passthrough instance was destroyed successfully.
 * \retval  false   Audio passthrough instance was failed to destory.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_destroy(void);

/**
 * audio_passthrough.h
 *
 * \brief   Enable audio passthrough mode.
 *
 * \param[in]   mode audio passthrough mode   \ref T_AUDIO_PASSTHROUGH_MODE
 * \param[in]   llapt_scenario_id Low Latency APT scenario index
 *
 * \return          The status of enabling audio passthrough mode.
 * \retval  true    Audio passthrough mode was enabled successfully.
 * \retval  false   Audio passthrough mode was failed to enable.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_enable(T_AUDIO_PASSTHROUGH_MODE mode, uint8_t llapt_scenario_id);

/**
 * audio_passthrough.h
 *
 * \brief   Disable audio passthrough mode.
 *
 * \return          The status of disabling audio passthrough mode.
 * \retval  true    Audio passthrough mode was disabled successfully.
 * \retval  false   Audio passthrough mode was failed to disable.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_disable(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the state of the audio passthrough stream.
 *
 * \return  The state \ref T_AUDIO_PASSTHROUGH_STATE of the audio passthrough stream.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
T_AUDIO_PASSTHROUGH_STATE audio_passthrough_state_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the maximum volume out level of the audio passthrough stream.
 *
 * \return  The maximum volume out level.
 *
 * \note  The maximum volume out level is configured by \ref audio_passthrough_volume_out_max_set,
 *        or the default value provided by the Audio subsystem.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_out_max_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the maximum volume out level of the audio passthrough stream.
 *
 * \param[in]   level   The maximum volume out level to set.
 *
 * \return  The status of setting the maximum volume out level.
 * \retval  true    The maximum volume out level was set successfully.
 * \retval  false   The maximum volume out level was failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_max_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Get the minimum volume out level of the audio passthrough stream.
 *
 * \return  The minimum volume out level.
 *
 * \note  The minimum volume out level is configured by \ref audio_passthrough_volume_out_min_set,
 *        or the default value provided by the Audio subsystem.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_out_min_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the minimum volume out level of the audio passthrough stream.
 *
 * \param[in]   level   The minimum volume out level to set.
 *
 * \return  The status of setting the minimum volume out level.
 * \retval  true    The minimum volume out level was set successfully.
 * \retval  false   The minimum volume out level was failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_min_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Get the current volume out level of the audio passthrough stream.
 *
 * \return  The current volume out level.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_out_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the current volume out level of the audio passthrough stream.
 *
 * \param[in]   level   The volume out level to set.
 *
 * \return  The status of setting volume out level.
 * \retval  true    Volume out level was set successfully.
 * \retval  false   Volume out level was failed to set.
 *
 * \note  The current volume out level shall be set between \ref audio_passthrough_volume_out_min_get
 *        and \ref audio_passthrough_volume_out_max_get.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Mute the current volume out level of the audio passthrough stream.
 *
 * \return          The result of muting the audio passthrough current volume out level.
 * \retval true     Audio passthrough current volume out level was muted successfully.
 * \retval false    Audio passthrough current volume out level was failed to mute.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_mute(void);

/**
 * audio_passthrough.h
 *
 * \brief   Unmute the current volume out level of the audio passthrough stream.
 *
 * \return          The result of unmuting the audio passthrough current volume out level.
 * \retval true     Audio passthrough current volume out level was unmuted successfully.
 * \retval false    Audio passthrough current volume out level was failed to unmute.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_unmute(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the current volume out mute status of the audio passthrough stream.
 *
 * \return          The volume out mute status of the audio passthrough stream.
 * \retval true     Audio passthrough current volume out was muted.
 * \retval false    Audio passthrough current volume out was unmuted.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_out_is_muted(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the maximum volume in level of the audio passthrough stream.
 *
 * \return  The maximum volume in level.
 *
 * \note  The maximum volume in level is configured by \ref audio_passthrough_volume_in_max_set,
 *        or the default value provided by the Audio subsystem.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_in_max_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the maximum volume in level of the audio passthrough stream.
 *
 * \param[in]   level   The maximum volume in level to set.
 *
 * \return  The status of setting the maximum volume in level.
 * \retval  true    The maximum volume in level was set successfully.
 * \retval  false   The maximum volume in level was failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_max_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Get the minimum volume in level of the audio passthrough stream.
 *
 * \return  The minimum volume in level.
 *
 * \note  The minimum volume in level is configured by \ref audio_passthrough_volume_in_min_set,
 *        or the default value provided by the Audio subsystem.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_in_min_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the minimum volume in level of the audio passthrough stream.
 *
 * \param[in]   level   The minimum volume in level to set.
 *
 * \return  The status of setting the minimum volume in level.
 * \retval  true    The minimum volume in level was set successfully.
 * \retval  false   The minimum volume in level was failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_min_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Get the current volume in level of the audio passthrough stream.
 *
 * \return  The current volume in level.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
uint8_t audio_passthrough_volume_in_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the current volume in level of the audio passthrough stream.
 *
 * \param[in]   level   The volume in level to set.
 *
 * \return  The status of setting volume in level.
 * \retval  true    Volume in level was set successfully.
 * \retval  false   Volume in level was failed to set.
 *
 * \note  The current volume in level shall be set between \ref audio_passthrough_volume_in_min_get
 *        and \ref audio_passthrough_volume_in_max_get.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Mute the current volume in level of the audio passthrough stream.
 *
 * \return          The result of muting the audio passthrough current volume in level.
 * \retval true     Audio passthrough current volume in level was muted successfully.
 * \retval false    Audio passthrough current volume in level was failed to mute.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_mute(void);

/**
 * audio_passthrough.h
 *
 * \brief   Unmute the current volume in level of the audio passthrough stream.
 *
 * \return          The result of unmuting the audio passthrough current volume in level.
 * \retval true     Audio passthrough current volume in level was unmuted successfully.
 * \retval false    Audio passthrough current volume in level was failed to unmute.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_unmute(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the current volume in mute status of the audio passthrough stream.
 *
 * \return          The volume in mute status of the audio passthrough stream.
 * \retval true     Audio passthrough current volume in was muted.
 * \retval false    Audio passthrough current volume in was unmuted.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_in_is_muted(void);

/**
 * audio_passthrough.h
 *
 * \brief   Get the volume balance scale of the audio passthrough stream.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \return  The volume balance scale of the audio passthrough stream.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
float audio_passthrough_volume_balance_get(void);

/**
 * audio_passthrough.h
 *
 * \brief   Set the delta gain values of the audio passthrough stream.
 *
 * \param[in] left_delta   The delta gain value of left channel in unit of decibel.
 * \param[in] right_delta  The delta gain value of right channel in unit of decibel.
 *
 * \return          The status of setting the audio passthrough stream delta gain values.
 * \retval true     The delta gain values were set successfully.
 * \retval false    The delta gain values were failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_volume_balance_set(float left_delta, float right_delta);

/**
 * audio_passthrough.h
 *
 * \brief   Set the brightness strength of the audio passthrough stream.
 *
 * \param[in] strength  The brightness strength to set.
 *
 * \return  The status of setting brightness strength.
 * \retval  true    Brightness strength was set successfully.
 * \retval  false   Brightness strength was failed to set.
 *
 * \note  The brightness strength shall be set between 0.0 and 1.0.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_brightness_set(float strength);

/**
 * audio_passthrough.h
 *
 * \brief   Enable the Own Voice Processing (OVP) of the audio passthrough stream.
 *
 * \details The OVP separates your own voice from the rest of the soundscape and processes
 *          it in a way that makes it sound as natural as possible.
 *
 * \param[in] level The initial aggressiveness level of own voice volume.
 *
 * \return  The status of enabling the OVP.
 * \retval  true    OVP was enabled successfully.
 * \retval  false   OVP was failed to enable.
 *
 * \note  The OVP aggressiveness level shall be set between 0 and 15. The own voice
 *        volume is descreasing when the OVP aggressiveness level is increasing.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_ovp_enable(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Set the Own Voice Processing (OVP) aggressiveness level of the audio passthrough stream.
 *
 * \details The OVP separates your own voice from the rest of the soundscape and processes
 *          it in a way that makes it sound as natural as possible.
 *
 * \param[in] level The aggressiveness level of own voice volume to set.
 *
 * \return  The status of setting the OVP aggressiveness level.
 * \retval  true    OVP aggressiveness level was set successfully.
 * \retval  false   OVP aggressiveness level was failed to set.
 *
 * \note  The OVP aggressiveness level shall be set between 0 and 15. The own voice
 *        volume is descreasing when the OVP aggressiveness level is increasing.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_ovp_set(uint8_t level);

/**
 * audio_passthrough.h
 *
 * \brief   Disable the Own Voice Processing (OVP) of the audio passthrough stream.
 *
 * \details The OVP separates your own voice from the rest of the soundscape and processes
 *          it in a way that makes it sound as natural as possible.
 *
 * \return  The status of disabling the OVP.
 * \retval  true    OVP was disabled successfully.
 * \retval  false   OVP was failed to disable.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_ovp_disable(void);

/**
 * audio_passthrough.h
 *
 * \brief   Enable the Beamforming effect of the audio passthrough stream.
 *
 * \details Beamforming technique in the acoustic field is applied to enhance signals from
 *          the desired direction or angle, and suppress noises from other directions.
 *
 * \param[in] direction The desired direction for Beamforming processing.
 *
 * \return  The status of enabling the Beamforming effect.
 * \retval  true    Beamforming effect was enabled successfully.
 * \retval  false   Beamforming effect was failed to enable.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_beamforming_enable(uint8_t direction);

/**
 * audio_passthrough.h
 *
 * \brief   Set the Beamforming direction of the audio passthrough stream.
 *
 * \details Beamforming technique in the acoustic field is applied to enhance signals from
 *          the desired direction or angle, and suppress noises from other directions.
 *
 * \param[in] direction The desired direction for Beamforming processing.
 *
 * \return  The status of setting the Beamforming direction.
 * \retval  true    Beamforming direction was set successfully.
 * \retval  false   Beamforming direction was failed to set.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_beamforming_set(uint8_t direction);

/**
 * audio_passthrough.h
 *
 * \brief   Disable the Beamforming effect of the audio passthrough stream.
 *
 * \details Beamforming technique in the acoustic field is applied to enhance signals from
 *          the desired direction or angle, and suppress noises from other directions.
 *
 * \return  The status of disabling the Beamforming effect.
 * \retval  true    Beamforming effect was disabled successfully.
 * \retval  false   Beamforming effect was failed to disable.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_beamforming_disable(void);

/**
 * audio_passthrough.h
 *
 * \brief   Attach the Audio Effect instance to the Audio Pass-through stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of attaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was attached successfully.
 * \retval  false   Audio Effect instance was failed to attach.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_effect_attach(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * audio_passthrough.h
 *
 * \brief   Detach the Audio Effect instance from the Audio Pass-through stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of detaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was detached successfully.
 * \retval  false   Audio Effect instance was failed to detach.
 *
 * \ingroup AUDIO_PASSTHROUGH
 */
bool audio_passthrough_effect_detach(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PASSTHROUGH_H_ */
