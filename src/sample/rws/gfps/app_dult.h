/*
 * Copyright (c) 2024, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DULT_H_
#define _APP_DULT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "app_dult_svc.h"

#define DULT_ACCESSORY_CAP_PLAY_SOUND            0b00000001
#define DULT_ACCESSORY_CAP_MONITOR_DETECT_UT     0b00000010
#define DULT_ACCESSORY_CAP_LOOK_UP_BY_NFC        0b00000100
#define DULT_ACCESSORY_CAP_LOOK_UP_BY_LE         0b00001000

#define DULT_NETWORK_ID                          0x02       // google
#define DULT_FIRMWARE_VERSION                    0x00030100 // sync with gfps
#define DULT_BATTERY_TYPE                        0x02       // rechargeable battery
// #define DULT_MANUFACTURE_NAME                    "Realtek Semiconductor Corporation"
// #define DULT_MODLE_NAME                          "dult_finder"

typedef enum
{
    DULT_BATT_FULL,
    DULT_BATT_MEDIUM,
    DULT_BATT_LOW,
    DULT_BATT_CRITICALLY_LOW,
} T_DULT_BATT_LEVEL;

void app_dult_svc_init(void);
void app_dult_sound_complete_handle(void);
void app_dult_handle_power_on(void);
void app_dult_handle_none_owner_state(T_DULT_NEAR_OWNER_STATE state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
