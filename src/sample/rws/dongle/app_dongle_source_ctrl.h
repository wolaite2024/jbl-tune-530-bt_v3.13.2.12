/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DONGLE_SOURCE_CTRL_H_
#define _APP_DONGLE_SOURCE_CTRL_H_

#include "app_hfp.h"

typedef enum
{
    ALLOWED_SOURCE_DONGLE   = 0x00,
    ALLOWED_SOURCE_BT       = 0x01,
} T_ALLOWED_SOURCE;

typedef enum
{
    EVENT_SOURCE_SWITCH_TO_BT       = 0x01,
    EVENT_SOURCE_SWITCH_TO_DONGLE   = 0x02,
    EVENT_DONGLE_A2DP_CON_IND       = 0x03,
    EVENT_BT_A2DP_CON_IND           = 0x04,
} T_SOURCE_CTRL_EVENT;

void app_dongle_source_ctrl_init(void);
void app_dongle_switch_allowed_source(void);
void app_dongle_source_ctrl_evt_handler(T_SOURCE_CTRL_EVENT event);
#if F_APP_LEA_SUPPORT
void app_dongle_source_switch_to_dongle_le_audio_handle(void);
void app_dongle_source_switch_to_bt_le_audio_handle(void);
#endif
#endif

