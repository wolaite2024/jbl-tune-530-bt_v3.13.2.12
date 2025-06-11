/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _SIDETONE_H_
#define _SIDETONE_H_

#include <stdint.h>
#include <stdbool.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup AUDIO_SIDETONE Sidetone
 *
 * \brief   Control and customize the sidetone engine in the audio framework.
 * \details Sidetone is the sound of the user voice captured by the microphone
 *          that is fed back into the earpiece. Sidetone gives indications to
 *          the user that the microphone is picking up the voice; the voice is
 *          loud enough or too low; and the voice connection is established or
 *          dropped.
 */

/**
 * sidetone.h
 *
 * \brief Define the supported sidetone types.
 *
 * \ingroup AUDIO_SIDETONE
 */
typedef enum t_sidetone_type
{
    SIDETONE_TYPE_SW    = 0x00, /**< Software sidetone engine type */
    SIDETONE_TYPE_HW    = 0x01, /**< Hardware sidetone engine type */
} T_SIDETONE_TYPE;

/**
 * sidetone.h
 *
 * \brief   Create a sidetone instance.
 *
 * \param[in] type       Sidetone type \ref T_SIDETONE_TYPE.
 * \param[in] gain       DAC digital gain value in dB unit.
 * \param[in] level      Cutoff frequency level.
 *
 * \return  The instance of the sidetone engine. If returned instance is NULL, the
 *          sidetone instance was failed to create.
 *
 * \ingroup AUDIO_SIDETONE
 */
T_AUDIO_EFFECT_INSTANCE sidetone_create(T_SIDETONE_TYPE type,
                                        int16_t         gain,
                                        uint8_t         level);

/**
 * sidetone.h
 *
 * \brief   Enable the specific sidetone instance.
 *
 * \param[in] instance  The sidetone engine session \ref AUDIO_SIDETONE.
 *
 * \return  The status of enabling the sidetone instance.
 * \retval  true    Sidetone instance was enabled successfully.
 * \retval  false   Sidetone instance was failed to enable.
 *
 * \ingroup AUDIO_SIDETONE
 */
bool sidetone_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * sidetone.h
 *
 * \brief   Disable the specific sidetone instance.
 *
 * \param[in] instance  The sidetone engine session \ref AUDIO_SIDETONE.
 *
 * \return  The status of disabling the sidetone instance.
 * \retval  true    Sidetone instance was disabled successfully.
 * \retval  false   Sidetone instance was failed to disable.
 *
 * \ingroup AUDIO_SIDETONE
 */
bool sidetone_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * sidetone.h
 *
 * \brief   Set the gain of the specific sidetone instance.
 *
 *\param[in] instance  The sidetone engine session \ref AUDIO_SIDETONE.
 *\param[in] gain      DAC digital gain value in dB unit.
 *
 * \return  The status of setting the sidetone gain.
 * \retval  true    Sidetone gain was set successfully.
 * \retval  false   Sidetone gain was failed to set.
 *
 * \ingroup AUDIO_SIDETONE
 */
bool sidetone_gain_set(T_AUDIO_EFFECT_INSTANCE instance, int16_t gain);

/**
 * sidetone.h
 *
 * \brief   Release the specific sidetone instance.
 *
 * \param[in] instance  The sidetone engine session \ref AUDIO_SIDETONE.
 *
 * \return  The status of releasing the sidetone instance.
 * \retval  true    Sidetone instance was released successfully.
 * \retval  false   Sidetone instance was failed to release.
 *
 * \ingroup AUDIO_SIDETONE
 */
bool sidetone_release(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SIDETONE_H_ */
