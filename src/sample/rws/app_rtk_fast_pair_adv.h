/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_RTK_FAST_PAIR_ADV_H_
#define _APP_RTK_FAST_PAIR_ADV_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    RTK_FAST_PAIR_ADV_TYPE_POWER_ON            = 0x00,
    RTK_FAST_PAIR_ADV_TYPE_ENGAGEMENT          = 0x01,
    RTK_FAST_PAIR_ADV_TYPE_IN_OUT_CASE         = 0x02,
    RTK_FAST_PAIR_ADV_TYPE_BATT_CHANGE         = 0x03,
} T_RTK_FAST_PAIR_ADV_TYPE;

void app_rtk_fast_pair_adv_init(void);
void app_rtk_fast_pair_handle_power_on(void);
void app_rtk_fast_pair_handle_remote_conn_cmpl(void);
void app_rtk_fast_pair_handle_in_out_case(void);
void app_rtk_fast_pair_handle_batt_change(void);

#ifdef __cplusplus
}
#endif

#endif

