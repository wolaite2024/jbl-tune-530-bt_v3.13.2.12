/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_USB_AUDIO_H_
#define _APP_USB_AUDIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_track.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_USB_AUDIO APP USB Audio
  * @brief usb audio stream user application-playback/capture.
  * @{
  */

/**
 * app_usb_audio.h
 *
 * \brief   USB Audio scenario definition
 *
 * \ingroup APP_USB_AUDIO
 */
#define USB_AUDIO_SCENARIO_PLAYBACK  (1<<0)
#define USB_AUDIO_SCENARIO_CAPTURE   (1<<1)
#define USB_AUDIO_SCENARIO_ALL       (USB_AUDIO_SCENARIO_PLAYBACK|USB_AUDIO_SCENARIO_CAPTURE)

/**
 * app_usb_audio.h
 *
 * \brief   get downstream track state(temporary api)
 *
 * \return track state
 *
 * \ingroup APP_USB_AUDIO
 */
T_AUDIO_TRACK_STATE app_usb_audio_get_ds_track_state(void);

/**
 * app_usb_audio.h
 *
 * \brief   get app usb audio active state(temporary api)
 *
 * \return true active false inactive
 *
 * \ingroup APP_USB_AUDIO
 */
bool app_usb_audio_is_active(void);

/**
 * app_usb_audio.h
 *
 * \brief   get app usb audio mic mute state(temporary api)
 *
 * \return 0 unmute
 *
 * \ingroup APP_USB_AUDIO
 */
uint8_t app_usb_audio_is_mic_mute(void);

/**
 * app_usb_audio.h
 *
 * \brief  update app usb audio mic mute status
 *
 * \param  void
 *
 * \return void
 *
 * \ingroup APP_USB_AUDIO
 */
void app_usb_audio_mute_ctrl(void);

/**
 * app_usb_audio.h
 *
 * \brief   start app usb audio scenario
 *
 * \param[in] scenario \ref USB_AUDIO_SCENARIO_PLAYBACK \ref USB_AUDIO_SCENARIO_CAPTURE
 *
 * \ingroup APP_USB_AUDIO
 */
void app_usb_audio_start(uint8_t scenario);

/**
 * app_usb_audio.h
 *
 * \brief   get usb audio eq instance
 *
 * \return  usb audio eq instance
 *
 * \ingroup APP_USB_AUDIO
 */
void *app_usb_audio_get_eq_instance(void);

/**
 * app_usb_audio.h
 *
 * \brief   stop app usb audio scenario
 *
 * \param[in] scenario \ref USB_AUDIO_SCENARIO_PLAYBACK \ref USB_AUDIO_SCENARIO_CAPTURE
 *
 * \ingroup APP_USB_AUDIO
 */
void app_usb_audio_stop(uint8_t scenario);

/**
 * app_usb_audio.h
 *
 * \brief   init app usb audio
 *
 * \ingroup APP_USB_AUDIO
 */
void app_usb_audio_init(void);
/** @}*/
/** End of APP_USB_AUDIO
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_USB_AUDIO_H_ */
