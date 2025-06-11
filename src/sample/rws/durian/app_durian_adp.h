/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_ADP_H_
#define _APP_DURIAN_ADP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_adp_batt_report(void);
bool app_durian_adp_batt_need_report(uint8_t app_idx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
