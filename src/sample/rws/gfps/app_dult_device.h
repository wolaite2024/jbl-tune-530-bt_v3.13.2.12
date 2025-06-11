/*
 * Copyright (c) 2024, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DULT_DEVICE_H_
#define _APP_DULT_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "app_dult_svc.h"

typedef enum
{
    DULT_RING_STATE_STOPPED,
    DULT_RING_STATE_STARTED,
} T_DULT_RING_STATE;


#if GFPS_FINDER_DULT_ADV_SUPPORT
T_GAP_CAUSE app_dult_device_start_adv(T_DULT_NEAR_OWNER_STATE state);
T_GAP_CAUSE app_dult_device_update_adv_data(T_DULT_NEAR_OWNER_STATE state);
void app_dult_device_update_adv_addr(void);
void app_dult_device_start_update_timer(T_DULT_NEAR_OWNER_STATE current_state);
#endif

void app_dult_device_sound_start(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback);
void app_dult_device_sound_stop(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback);

bool app_dult_id_read_state_get(void);
void app_dult_id_read_state_enable(void);

T_DULT_RING_STATE app_dult_device_get_ring_state(void);
void app_dult_device_set_ring_state(T_DULT_RING_STATE ring_state);

void app_dult_device_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
