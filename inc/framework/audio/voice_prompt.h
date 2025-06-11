/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VOICE_PROMPT_H_
#define _VOICE_PROMPT_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_VOICE_PROMPT Voice Prompt
 *
 * \brief   Play and control voice prompt.
 * \details Voice Prompt (VP) plays voice files with the required languages. It is used when
 *          prompting for notification, alarm, user-confirmation, as well as other situations.
 */

/**
 * voice_prompt.h
 *
 * \brief Define the voice prompt modes.
 *
 * \details Voice prompt has three modes which are audible mode, silent mode, and volume fixed mode.
 *          Audible mode is voice prompt's default mode that voice prompt can be played and stopped,
 *          and voice prompt volume can be adjusted, muted and unmuted. Silent mode will disable
 *          voice prompt and mute voice prompt volume that voice prompt cannot be played and its
 *          volume cannot be adjusted. Fixed mode will make voice prompt volume unadjustable.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
typedef enum t_voice_prompt_mode
{
    VOICE_PROMPT_MODE_AUDIBLE   = 0x00, /**< Voice prompt is enabled and audible. */
    VOICE_PROMPT_MODE_SILENT    = 0x01, /**< Voice prompt is disabled and silent. */
    VOICE_PROMPT_MODE_FIXED     = 0x02, /**< Voice prompt is enabled, but volume is fixed and not adjustable. */
} T_VOICE_PROMPT_MODE;

/**
 * voice_prompt.h
 *
 * \brief   Get the current voice prompt mode.
 *
 * \return  The current voice prompt mode \ref T_VOICE_PROMPT_MODE.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
T_VOICE_PROMPT_MODE voice_prompt_mode_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Set the current voice prompt mode.
 *
 * \param[in] mode  The voice prompt mode \ref T_VOICE_PROMPT_MODE.
 *
 * \return          The result of setting the voice prompt mode.
 * \retval true     Voice prompt mode was set successfully.
 * \retval false    Voice prompt mode was failed to set.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_mode_set(T_VOICE_PROMPT_MODE mode);

/**
 * voice_prompt.h
 *
 * \brief   Voice prompt language list.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
typedef enum t_voice_prompt_language_id
{
    VOICE_PROMPT_LANGUAGE_ENGLISH       = 0x00,
    VOICE_PROMPT_LANGUAGE_CHINESE       = 0x01,
    VOICE_PROMPT_LANGUAGE_FRENCH        = 0x02,
    VOICE_PROMPT_LANGUAGE_PORTUGUESE    = 0x03,
    VOICE_PROMPT_LANGUAGE_INVALID       = 0x04,
} T_VOICE_PROMPT_LANGUAGE_ID;

/**
 * voice_prompt.h
 *
 * \brief   Set global voice prompt language.
 *
 * \param[in] language  Language for global voice prompt \ref T_VOICE_PROMPT_LANGUAGE_ID.
 *
 * \return          The result of setting global voice prompt language.
 * \retval true     Global voice prompt language was set successfully.
 * \retval false    Global voice prompt language was failed to set.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_language_set(T_VOICE_PROMPT_LANGUAGE_ID language);

/**
 * voice_prompt.h
 *
 * \brief   Get current global voice prompt language.
 *
 * \return  Current global voice prompt language \ref T_VOICE_PROMPT_LANGUAGE_ID. If failed,
 *          VOICE_PROMPT_LANGUAGE_INVALID is returned.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
T_VOICE_PROMPT_LANGUAGE_ID voice_prompt_language_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Get all supported voice prompt languages.
 *
 * \return  Bitmap for supported voice prompt languages \ref T_VOICE_PROMPT_LANGUAGE_ID.
 *          Bit value is 1 << \ref T_VOICE_PROMPT_LANGUAGE_ID.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
uint8_t voice_prompt_supported_languages_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Roll the next voice prompt language.
 *
 * \return          The result of rolling voice prompt.
 * \retval true     Voice prompt language was rolled successfully.
 * \retval false    Voice prompt language was failed to roll.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_language_roll(void);

/**
 * voice_prompt.h
 *
 * \brief   Get the maximum volume level of the voice prompt.
 *
 * \return  The maximum volume level of the voice prompt.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
uint8_t voice_prompt_volume_max_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Set the maximum volume level of the voice prompt.
 *
 * \param[in] volume    The maximum volume level of the voice prompt.
 *
 * \return  The result of setting the voice prompt maximum volume level.
 * \retval  true    Voice prompt maximum volume level was set successfully.
 * \retval  false   Voice prompt maximum volume level was failed to set.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_volume_max_set(uint8_t volume);

/**
 * voice_prompt.h
 *
 * \brief   Get the minimum volume level of the voice prompt.
 *
 * \return  The minimum volume level of the voice prompt.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
uint8_t voice_prompt_volume_min_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Set the minimum volume level of the voice prompt.
 *
 * \param[in] volume    The minimum volume level of the voice prompt.
 *
 * \return  The result of setting the voice prompt minimum volume level.
 * \retval  true    Voice prompt minimum volume level was set successfully.
 * \retval  false   Voice prompt minimum volume level was failed to set.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_volume_min_set(uint8_t volume);

/**
 * voice_prompt.h
 *
 * \brief   Get the current volume level of the voice prompt.
 *
 * \return  The current volume level of the voice prompt.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
uint8_t voice_prompt_volume_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Set the current volume level of the voice prompt.
 *
 * \details If the voice prompt volume was muted previously, setting the volume at any level excluding
 *          the minimum level \ref voice_prompt_volume_min_get() will unmute and activate the volume
 *          level. When the voice prompt volume was already muted, setting the volume at the minimum
 *          level will have no effect.
 *
 * \param[in] volume    The volume level to set. See \ref voice_prompt_volume_max_get() and
 *                      \ref voice_prompt_volume_min_get() for the valid volume level range.
 *
 * \return  The result of setting the voice prompt current volume level.
 * \retval  true    Voice prompt current volume level was set successfully.
 * \retval  false   Voice prompt current volume level was failed to set.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_volume_set(uint8_t volume);

/**
 * voice_prompt.h
 *
 * \brief   Mute the current volume level of the voice prompt.
 *
 * \return          The result of muting the voice prompt volume level.
 * \retval true     Voice prompt volume level was muted successfully.
 * \retval false    Voice prompt volume level was failed to mute.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_volume_mute(void);

/**
 * voice_prompt.h
 *
 * \brief   Unmute the current volume level of the voice prompt.
 *
 * \return          The result of muting the voice prompt volume level.
 * \retval true     Voice prompt volume level was unmuted successfully.
 * \retval false    Voice prompt volume level was failed to unmute.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_volume_unmute(void);

/**
 * voice_prompt.h
 *
 * \brief   Get the volume balance scale of the voice prompt.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \return  The volume balance scale of the voice prompt.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup VOICE_PROMPT
 */
float voice_prompt_volume_balance_get(void);

/**
 * voice_prompt.h
 *
 * \brief   Set the delta gain values of the voice prompt.
 *
 * \param[in] left_delta   The delta gain value of left channel in unit of decibel.
 * \param[in] right_delta  The delta gain value of right channel in unit of decibel.
 *
 * \return          The status of setting the voice prompt delta gain values.
 * \retval true     The delta gain values were set successfully.
 * \retval false    The delta gain values were failed to set.
 *
 * \ingroup VOICE_PROMPT
 */
bool voice_prompt_volume_balance_set(float left_delta, float right_delta);

/**
 * voice_prompt.h
 *
 * \brief   Play a voice prompt.
 *
 * \details The voice prompt is first pushed into the notification queue inside Audio Subsystem.
 *          When there are no pending voice prompts or ringtones in the notification queue, this
 *          voice prompt will be played immediately, otherwise, it will be scheduled until all
 *          the preceding voice prompts or ringtones are popped from the notification queue. Upper
 *          layer will be notified by the audio event \ref AUDIO_EVENT_VOICE_PROMPT_STARTED when
 *          started, and by the audio event \ref AUDIO_EVENT_VOICE_PROMPT_STOPPED when stopped.
 *
 * \note    The notification queue inside Audio Subsystem is basically a first-in, fist-out (FIFO)
 *          queue, So all the voice prompts and ringtones pending in the notification queue will
 *          be played in sequence. Upper layer can use \ref voice_prompt_cancel to cancel these
 *          voice prompts with the required index, or \ref voice_prompt_flush to flush all the
 *          voice prompts pending in the notification queue.
 *          This action will be permitted only when the voice prompt mode is \ref VOICE_PROMPT_MODE_AUDIBLE
 *          or \ref VOICE_PROMPT_MODE_FIXED. See \ref T_VOICE_PROMPT_MODE for details.
 *
 * \param[in] index     The type of the playing voice prompt.
 * \param[in] language  Language for the playing voice prompt \ref T_VOICE_PROMPT_LANGUAGE_ID.
 * \param[in] relay     Relay the voice prompt to peer identical devices.
 *
 * \return          The result of playing voice prompt.
 * \retval true     Voice prompt was played successfully.
 * \retval false    Voice prompt was failed to play.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_play(uint8_t index, T_VOICE_PROMPT_LANGUAGE_ID language, bool relay);

/**
 * voice_prompt.h
 *
 * \brief   Stop the playing voice prompt.
 *
 * \details All the voice prompts pending in the notification queue inside Audio Subsystem
 *          will not be affected.
 *
 * \note    This action will be permitted only when the voice prompt mode is \ref VOICE_PROMPT_MODE_AUDIBLE
 *          or \ref VOICE_PROMPT_MODE_FIXED. See \ref T_VOICE_PROMPT_MODE for details.
 *
 * \return          The result of stopping voice prompt.
 * \retval true     Voice prompt was stopped successfully.
 * \retval false    Voice prompt was failed to stop.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_stop(void);

/**
 * voice_prompt.h
 *
 * \brief   Cancel the specfic voice prompt in the queue.
 *
 * \details All the voice prompts with the required index pending in the notification queue
 *          inside Audio Subsystem will be canceled. Note that if there is a playing voice
 *          prompt with the same index, this playing voice prompt will also be stopped immediately.
 *
 * \note    This action will be permitted only when the voice prompt mode is \ref VOICE_PROMPT_MODE_AUDIBLE
 *          or \ref VOICE_PROMPT_MODE_FIXED. See \ref T_VOICE_PROMPT_MODE for details.
 *
 * \param[in] index     The index of the queued voice prompt.
 * \param[in] relay     Relay the Voice prompt canceling action to peer identical devices.
 *
 * \return          The result of canceling voice prompt.
 * \retval true     Voice prompt was canceled successfully.
 * \retval false    Voice prompt was failed to cancel.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_cancel(uint8_t index, bool relay);

/**
 * voice_prompt.h
 *
 * \brief   Flush all the pending voice prompts in the queue.
 *
 * \details All the voice prompts pending in the notification queue inside Audio Subsystem
 *          will be flushed immediately. If there is a voice prompt currently under playing,
 *          it will not be affected by this flush operation.
 *
 * \note    This action will be permitted only when the voice prompt mode is \ref VOICE_PROMPT_MODE_AUDIBLE
 *          or \ref VOICE_PROMPT_MODE_FIXED. See \ref T_VOICE_PROMPT_MODE for details.
 *
 * \param[in] relay Relay the voice prompt flushing action to peer identical devices.
 *
 * \return          The result of flushing the voice prompts.
 * \retval true     Voice prompts was flushed successfully.
 * \retval false    Voice prompts was failed to flush.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
bool voice_prompt_flush(bool relay);

/**
 * voice_prompt.h
 *
 * \brief   Return the count remaining voice prompts.
 *
 * \return  The count of remaining voice prompts.
 *
 * \ingroup AUDIO_VOICE_PROMPT
 */
uint8_t voice_prompt_remaining_count_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VOICE_PROMPT_H_ */
