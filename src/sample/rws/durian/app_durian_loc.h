/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_LOC_H_
#define _APP_DURIAN_LOC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_loc_report(uint8_t local, uint8_t remote);
void app_durian_loc_in_ear_detect(uint8_t ear_detect_en);
bool app_durian_loc_get_action(uint8_t *action, uint8_t event);
void app_durian_loc_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
