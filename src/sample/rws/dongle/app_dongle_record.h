/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DONGLE_RECORD_H_
#define _APP_DONGLE_RECORD_H_


#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
  *                           Header Files
  *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/** @defgroup APP_RWS_DONGLE
  * @brief
  * @{
  */

#define BD_ADDR_LENGTH       6

typedef enum
{
    RECORD_SAMPLE_RATE_16K,
    RECORD_SAMPLE_RATE_32K,
} T_RECORD_SAMPLE_RATE;

/**
    * @brief        Clear spp audio buff.
    * @return       void
    */
void app_dongle_clear_spp_audio_buff(void);

void app_dongle_force_stop_recording(void);

/**
    * @brief        This function can stop the record.
    * @return       void
    */
void app_dongle_stop_recording(uint8_t bd_addr[6]);

/**
    * @brief        This function can start the record.
    * @return       void
    */
void app_dongle_start_recording(uint8_t bd_addr[6]);

void app_dongle_record_init(void);

void app_dongle_volume_in_mute(void);

void app_dongle_volume_in_unmute(void);

bool app_dongle_get_record_state(void);

/** @} End of APP_RWS_DONGLE */

#ifdef __cplusplus
}
#endif

#endif //_VOICE_SPP_H_
