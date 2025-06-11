/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VAD_H_
#define _VAD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_VAD Voice Activity Detection
 *
 * \brief   Enable and disable voice activity detection.
 * \details Voice activity detection (VAD) is a technique used in speech processing to detect
 *          the presence or absence of human speech. The main uses of VAD are in speech recognition
 *          and speech coding, e.g. hot words wakeup and voice recording. It can avoid unnecessary
 *          coding and transmission of silence packets to save on computation and network bandwidth.
 */


/**
 * vad.h
 *
 * \brief   Enable voice activity detection.
 *
 * \return          The status of enabling voice activity detection.
 * \retval  true    Voice activity detection was enabled successfully.
 * \retval  false   Voice activity detection was failed to enable.
 *
 * \ingroup AUDIO_VAD
 */
bool vad_enable(void);

/**
 * vad.h
 *
 * \brief   Disable voice activity detection.
 *
 * \return          The status of disabling voice activity detection.
 * \retval  true    Voice activity detection was disabled successfully.
 * \retval  false   Voice activity detection was failed to disable.
 *
 * \ingroup AUDIO_VAD
 */
bool vad_disable(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VAD_H_ */
