/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _WDRC_H_
#define _WDRC_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup AUDIO_WDRC Wide Dynamic Range Compression
 *
 * \brief   Control and customize the WDRC engine in the audio framework.
 * \details Wide Dynamic Range Compression (WDRC) compresses the wide range of sound intensities
 *          into the limited dynamic range, and makes a broader range of sound intensities audible
 *          for the user.
 */

/**
 * wdrc.h
 *
 * \brief Define the supported WDRC content types.
 *
 * \ingroup AUDIO_WDRC
 */
typedef enum t_wdrc_content_type
{
    WDRC_CONTENT_TYPE_AUDIO         = 0x00,
    WDRC_CONTENT_TYPE_VOICE         = 0x01,
    WDRC_CONTENT_TYPE_PASSTHROUGH   = 0x02,
} T_WDRC_CONTENT_TYPE;

/**
 * wdrc.h
 *
 * \brief   Create a WDRC instance.
 *
 * \param[in] type      WDRC content type \ref T_WDRC_CONTENT_TYPE.
 * \param[in] info_buf  WDRC parameter buffer.
 * \param[in] info_len  WDRC parameter buffer length.
 *
 * \return  The instance of the WDRC engine. If returned instance is NULL, the WDRC
 *          instance was failed to create.
 *
 * \ingroup AUDIO_WDRC
 */
T_AUDIO_EFFECT_INSTANCE wdrc_create(T_WDRC_CONTENT_TYPE type, void *info_buf, uint16_t info_len);

/**
 * wdrc.h
 *
 * \brief   Enable the specific WDRC instance.
 *
 * \param[in] instance  WDRC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of enabling the WDRC instance.
 * \retval  true    WDRC instance was enabled successfully.
 * \retval  false   WDRC instance was failed to enable.
 *
 * \ingroup AUDIO_WDRC
 */
bool wdrc_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * wdrc.h
 *
 * \brief   Disable the specific WDRC instance.
 *
 * \param[in] instance  WDRC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of disabling the WDRC instance.
 * \retval  true    WDRC instance was disabled successfully.
 * \retval  false   WDRC instance was failed to disable.
 *
 * \ingroup AUDIO_WDRC
 */
bool wdrc_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * wdrc.h
 *
 * \brief   Set the specific WDRC instance parameter.
 *
 * \param[in] instance  WDRC instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] info_buf  WDRC parameter buffer.
 * \param[in] info_len  WDRC parameter length.
 *
 * \return  The status of setting the WDRC instance parameter.
 * \retval  true    WDRC instance parameter was set successfully.
 * \retval  false   WDRC instance parameter was failed to set.
 *
 * \ingroup AUDIO_WDRC
 */
bool wdrc_set(T_AUDIO_EFFECT_INSTANCE instance, void *info_buf, uint16_t info_len);

/**
 * wdrc.h
 *
 * \brief   Release the specific WDRC instance.
 *
 * \param[in] instance  WDRC instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the WDRC instance.
 * \retval  true    WDRC instance was released successfully.
 * \retval  false   WDRC instance was failed to release.
 *
 * \ingroup AUDIO_WDRC
 */
bool wdrc_release(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WDRC_H_ */
