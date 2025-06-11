/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BUD_LOC_H_
#define _APP_BUD_LOC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    APP_BUD_LOC_EVENT_CASE_IN_CASE                      = 0x00,
    APP_BUD_LOC_EVENT_CASE_OUT_CASE                     = 0x01,
    APP_BUD_LOC_EVENT_CASE_OPEN_CASE                    = 0x02,
    APP_BUD_LOC_EVENT_CASE_CLOSE_CASE                   = 0x03,

    APP_BUD_LOC_EVENT_SENSOR_IN_EAR                     = 0x04,
    APP_BUD_LOC_EVENT_SENSOR_OUT_EAR                    = 0x05,
    APP_BUD_LOC_EVENT_SENSOR_LD_CONFIG                  = 0x06,
    APP_BUD_LOC_EVENT_SENSOR_OUT_EAR_A2DP_PLAYING       = 0x07,
    APP_BUD_LOC_EVENT_SENSOR_SUSPEND_BY_OUT_EAR         = 0x08,
    APP_BUD_LOC_EVENT_SENSOR_RSV                        = 0x09,

    APP_BUD_LOC_EVENT_MAX                               = 0x0A,
} T_APP_BUD_LOC_EVENT;

/* @brief  detect pause by out ear timer started.
* @param  none.
* @return true if started.
*/
bool app_bud_loc_detect_pause_by_out_ear_timer_started(void);

/* @brief  update in ear recover a2dp flag
* @param  true/false
* @return none
*/
void app_bud_loc_update_in_ear_recover_a2dp(bool flag);
void app_bud_loc_update_detect_suspend_by_out_ear(bool flag);

/* @brief  stop_pause_by_out_ear_timer
* @param  void
* @return none
*/
void app_bud_loc_stop_pause_by_out_ear_timer(void);
void app_bud_loc_init(void);
void app_bud_loc_evt_handle(uint8_t event, bool from_remote, uint8_t para);
void app_bud_loc_cause_action_flag_set(bool sensor_cause_action);
bool app_bud_loc_cause_action_flag_get(void);

#ifdef __cplusplus
}
#endif
#endif
