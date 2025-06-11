/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_VOLUME_H_
#define _AUDIO_VOLUME_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_VOLUME Audio Volume
 *
 * \brief   Control audio stream volume level.
 * \details Applications can get the audio stream volume level range of each stream type, and
 *          adjust the specific audio stream volume level.
 */


/**
 * audio_volume.h
 *
 * \brief Define Audio volume channel masks.
 *
 * \ingroup AUDIO_VOLUME
 */
typedef enum t_audio_volume_channel_mask
{
    AUDIO_VOLUME_CHANNEL_LEFT_RIGHT         = 0x00, /**< Audio volume channel left & right */
    AUDIO_VOLUME_CHANNEL_LEFT_ONLY          = 0x01, /**< Audio volume channel left only */
    AUDIO_VOLUME_CHANNEL_RIGHT_ONLY         = 0x02, /**< Audio volume channel right only */
    AUDIO_VOLUME_CHANNEL_LEFT_RIGHT_MIX     = 0x03, /**< Audio volume channel left/right mix */
    AUDIO_VOLUME_CHANNEL_LEFT_RIGHT_SWAP    = 0x04, /**< Audio volume channel left/right swap */
} T_AUDIO_VOLUME_CHANNEL_MASK;

/**
 * audio_volume.h
 *
 * \brief   Get the maximum volume out level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The maximum volume out level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_max_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the maximum volume out level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The maximum volume out level.
 *
 * \return  The status of setting the audio stream maximum volume out level.
 * \retval  true    Audio stream maximum volume out level was set successfully.
 * \retval  false   Audio stream maximum volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_max_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief   Get the minimum volume out level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The minimum volume out level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_min_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the minimum volume out level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The minimum volume out level.
 *
 * \return  The status of setting the audio stream minimum volume out level.
 * \retval  true    Audio stream minimum volume out level was set successfully.
 * \retval  false   Audio stream minimum volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_min_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief   Get the current volume out level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The current volume out level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the current volume out level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The volume out level of the audio stream type.
 *
 * \return          The status of setting the current audio stream volume out level.
 * \retval true     The current volume out level was set successfully.
 * \retval false    The current volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief  Mute the current volume out level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The status of muting the current volume out level.
 * \retval  true    Current audio volume out level was muted successfully.
 * \retval  false   Current audio volume out level was failed to mute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_mute(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief  Unmute the current volume out level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The status of unmuting the current volume out level.
 * \retval  true    Current audio volume out level was unmuted successfully.
 * \retval  false   Current audio volume out level was failed to unmute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_unmute(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Get the current volume out level mute status of the specific audio stream type.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \return          The volume out level mute status of the audio stream type.
 * \retval true     Audio stream current volume out level was muted.
 * \retval false    Audio stream current volume out level was unmuted.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_is_muted(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Get the maximum volume in level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The maximum volume in level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_max_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the maximum volume in level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The maximum volume in level.
 *
 * \return  The status of setting the audio stream maximum volume in level.
 * \retval  true    Audio stream maximum volume in level was set successfully.
 * \retval  false   Audio stream maximum volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_max_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief   Get the minimum volume in level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The minimum volume in level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_min_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the minimum volume in level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The minimum volume in level.
 *
 * \return  The status of setting the audio stream minimum volume in level.
 * \retval  true    Audio stream minimum volume in level was set successfully.
 * \retval  false   Audio stream minimum volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_min_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief   Get the current volume in level of the specific audio stream type.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The current volume in level of the specific audio stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the current volume in level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The volume in level of the audio stream type.
 *
 * \return          The status of setting the current audio stream volume in level.
 * \retval true     The current volume in level was set successfully.
 * \retval false    The current volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * audio_volume.h
 *
 * \brief  Mute the current volume in level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The status of muting the current volume in level.
 * \retval  true    Current audio volume in level was muted successfully.
 * \retval  false   Current audio volume in level was failed to mute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_mute(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief  Unmute the current volume in level of the specific audio stream type.
 *
 * \param[in] type      Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The status of unmuting the current volume in level.
 * \retval  true    Current audio volume in level was unmuted successfully.
 * \retval  false   Current audio volume in level was failed to unmute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_unmute(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Get the current volume in level mute status of the specific audio stream type.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \return          The volume in level mute status of the audio stream type.
 * \retval true     Audio stream current volume in level was muted.
 * \retval false    Audio stream current volume in level was unmuted.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_is_muted(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief  Set the global volume out channel mask for the audio or voice stream.
 *
 * \param[in] mask  Audio volume out channel mask \ref T_AUDIO_VOLUME_CHANNEL_MASK.
 *
 * \return  The status of setting the global volume out channel mask.
 * \retval  true    Global audio volume out channel mask was set successfully.
 * \retval  false   Global audio volume out channel mask was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_channel_set(T_AUDIO_VOLUME_CHANNEL_MASK mask);

/**
 * audio_volume.h
 *
 * \brief  Set the global volume in channel mask for the record or voice stream.
 *
 * \param[in] mask  Audio volume in channel mask \ref T_AUDIO_VOLUME_CHANNEL_MASK.
 *
 * \return  The status of setting the global volume in channel mask.
 * \retval  true    Global audio volume in channel mask was set successfully.
 * \retval  false   Global audio volume in channel mask was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_channel_set(T_AUDIO_VOLUME_CHANNEL_MASK mask);

/**
 * audio_volume.h
 *
 * \brief   Get the volume balance scale of the specific audio stream type.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \param[in] type  Audio stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The volume balance scale of the specific audio stream type.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup AUDIO_VOLUME
 */
float audio_volume_balance_get(T_AUDIO_STREAM_TYPE type);

/**
 * audio_volume.h
 *
 * \brief   Set the delta gain values of the specific audio stream type.
 *
 * \param[in] type         Audio stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] left_delta   The delta gain value of left channel in unit of decibel.
 * \param[in] right_delta  The delta gain value of right channel in unit of decibel.
 *
 * \return          The status of setting the audio stream delta gain values.
 * \retval true     The delta gain values were set successfully.
 * \retval false    The delta gain values were failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_balance_set(T_AUDIO_STREAM_TYPE type, float left_delta, float right_delta);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_VOLUME_H_ */
