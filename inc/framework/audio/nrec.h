/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _NREC_H_
#define _NREC_H_

#include <stdbool.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_NREC Noise Reduction & Echo Canceling
 *
 * \brief   Enable and disable Noise Reduction & Echo Canceling.
 * \details NR reduces unwanted ambient sounds using active noise control, and EC improves voice
 *          quality by preventing echos from being captured or created, or possibly removing it
 *          in post-processing.
 */

/**
 * nrec.h
 *
 * \brief   Define NREC content type.
 *
 * \ingroup AUDIO_NREC
 */
typedef enum t_nrec_content_type
{
    NREC_CONTENT_TYPE_VOICE       = 0x00,
    NREC_CONTENT_TYPE_PASSTHROUGH = 0x01,
} T_NREC_CONTENT_TYPE;

/**
 * nrec.h
 *
 * \brief   Define NREC mode.
 *
 * \ingroup AUDIO_NREC
 */
typedef enum t_nrec_mode
{
    NREC_MODE_HIGH_SOUND_QUALITY     = 0x00,
    NREC_MODE_STRONG_NOISE_REDUCTION = 0x01,
} T_NREC_MODE;

/**
 * nrec.h
 *
 * \brief   Create an NREC instance.
 *
 * \param[in] type  NREC content type \ref T_NREC_CONTENT_TYPE.
 * \param[in] mode  NREC mode \ref T_NREC_MODE.
 * \param[in] level NREC aggressiveness level ranges from 0x00 to 0x0f.
 *
 * \return  The instance of the NREC engine \ref T_AUDIO_EFFECT_INSTANCE. If the
 *          returned instance is NULL, the NREC instance was failed to create.
 *
 * \ingroup AUDIO_NREC
 */
T_AUDIO_EFFECT_INSTANCE nrec_create(T_NREC_CONTENT_TYPE type, T_NREC_MODE mode, uint8_t level);

/**
 * nrec.h
 *
 * \brief   Enable the specific NREC instance.
 *
 * \param[in] instance  NREC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of enabling the NREC instance.
 * \retval  true    NREC instance was enabled successfully.
 * \retval  false   NREC instance was failed to enable.
 *
 * \ingroup AUDIO_NREC
 */
bool nrec_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * nrec.h
 *
 * \brief   Disable the specific NREC instance.
 *
 * \param[in] instance  NREC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of disabling the NREC instance.
 * \retval  true    NREC instance was disabled successfully.
 * \retval  false   NREC instance was failed to disable.
 *
 * \ingroup AUDIO_NREC
 */
bool nrec_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * nrec.h
 *
 * \brief   Release the specific NREC instance.
 *
 * \param[in] instancE  NREC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the NREC instance.
 * \retval  true    NREC instance was released successfully.
 * \retval  false   NREC instance was failed to release.
 *
 * \ingroup AUDIO_NREC
 */
bool nrec_release(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * nrec.h
 *
 * \brief   Set the mode of NREC instance.
 *
 * \param[in] instancE  NREC instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] mode      NREC mode \ref T_NREC_MODE.
 *
 * \return  The status of setting the mode of NREC instance.
 * \retval  true    Mode of NREC instance was set successfully.
 * \retval  false   Mode of NREC instance was failed to set.
 *
 * \ingroup AUDIO_NREC
 */
bool nrec_mode_set(T_AUDIO_EFFECT_INSTANCE instance, T_NREC_MODE mode);

/**
 * nrec.h
 *
 * \brief   Set the aggressiveness level of NREC instance.
 *
 * \param[in] instance  NREC instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] level     NREC aggressiveness level ranges from 0x00 to 0x0f.
 *
 * \return  The status of setting the aggressiveness level of NREC instance.
 * \retval  true    Aggressiveness level of NREC instance was set successfully.
 * \retval  false   Aggressiveness level of NREC instance was failed to set.
 *
 * \ingroup AUDIO_NREC
 */
bool nrec_level_set(T_AUDIO_EFFECT_INSTANCE instance, uint8_t level);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NREC_H_ */
