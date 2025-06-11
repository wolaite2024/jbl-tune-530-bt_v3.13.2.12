/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AUDIO_ROUTE_H_
#define _APP_AUDIO_ROUTE_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_route.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_AUDIO_ROUTE App Audio Route
  * @brief App Audio Route
  * @{
  */

typedef struct t_dsp_tool_gain_level_data
{
    uint16_t  cmd_id;
    uint16_t  cmd_len;
    uint16_t  gain;
    uint8_t   level;
    uint8_t   category;
} T_DSP_TOOL_GAIN_LEVEL_DATA;

void app_audio_route_gain_init(void);
bool app_audio_route_physical_mic_get(T_AUDIO_CATEGORY category,
                                      T_AUDIO_ROUTE_LOGIC_IO_TYPE logical_mic,
                                      T_AUDIO_ROUTE_PHYSICAL_MIC *physical_mic);
bool app_audio_route_physical_mic_set(T_AUDIO_CATEGORY category,
                                      T_AUDIO_ROUTE_LOGIC_IO_TYPE logical_mic,
                                      T_AUDIO_ROUTE_PHYSICAL_MIC physical_mic);
bool app_audio_route_dac_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain);
bool app_audio_route_adc_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain);


/** End of APP_AUDIO_ROUTE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUDIO_ROUTE_H_ */
