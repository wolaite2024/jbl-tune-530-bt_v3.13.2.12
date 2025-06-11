/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _EQ_H_
#define _EQ_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup AUDIO_EQ Equalizer
 *
 * \brief   Control and customize the Equalizer engine in the audio framework.
 * \details Application can use predefined EQ sets, or customize each EQ set's frequency bands
 *          controlled by the Equalizer engine.
 */

/**
 * eq.h
 *
 * \brief Define the supported EQ content types.
 *
 * \ingroup AUDIO_EQ
 */
typedef enum t_eq_content_type
{
    EQ_CONTENT_TYPE_AUDIO       = 0x00,
    EQ_CONTENT_TYPE_VOICE       = 0x01,
    EQ_CONTENT_TYPE_RECORD      = 0x02,
    EQ_CONTENT_TYPE_PASSTHROUGH = 0x03,
} T_EQ_CONTENT_TYPE;

/**
 * eq.h
 *
 * \brief   Create an EQ instance.
 *
 * \param[in] type      EQ content type \ref T_EQ_CONTENT_TYPE.
 * \param[in] info_buf  EQ band parameter buffer.
 * \param[in] info_len  EQ band parameter buffer length.
 *
 * \return  The instance of the EQ engine \ref T_AUDIO_EFFECT_INSTANCE. If the
 *          returned instance is NULL, the EQ instance was failed to create.
 *
 * \ingroup AUDIO_EQ
 */
T_AUDIO_EFFECT_INSTANCE eq_create(T_EQ_CONTENT_TYPE type, void *info_buf, uint16_t info_len);

/**
 * eq.h
 *
 * \brief   Enable the specific EQ instance.
 *
 * \param[in] instance  EQ instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of enabling the EQ instance.
 * \retval  true    EQ instance was enabled successfully.
 * \retval  false   EQ instance was failed to enable.
 *
 * \ingroup AUDIO_EQ
 */
bool eq_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * eq.h
 *
 * \brief   Disable the specific EQ instance.
 *
 * \param[in] instance  EQ instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of disabling the EQ instance.
 * \retval  true    EQ instance was disabled successfully.
 * \retval  false   EQ instance was failed to disable.
 *
 * \ingroup AUDIO_EQ
 */
bool eq_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * eq.h
 *
 * \brief   Set the specific EQ instance band parameter.
 *
 * \param[in] instance  EQ instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] info_buf  EQ band parameter buffer.
 * \param[in] info_len  EQ band parameter length.
 *
 * \return  The status of setting the EQ instance band parameter.
 * \retval  true    EQ instance band parameter was set successfully.
 * \retval  false   EQ instance band parameter was failed to set.
 *
 * \ingroup AUDIO_EQ
 */
bool eq_set(T_AUDIO_EFFECT_INSTANCE instance, void *info_buf, uint16_t info_len);

/**
 * eq.h
 *
 * \brief   Release the specific EQ instance.
 *
 * \param[in] instance  EQ instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the EQ instance.
 * \retval  true    EQ instance was released successfully.
 * \retval  false   EQ instance was failed to release.
 *
 * \ingroup AUDIO_EQ
 */
bool eq_release(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _EQ_H_ */
