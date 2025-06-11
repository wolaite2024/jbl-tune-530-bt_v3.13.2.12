/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_FINDER_ADV_H_
#define _APP_GFPS_FINDER_ADV_H_

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "gfps.h"
#include "ble_ext_adv.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief gfps_finder_adv_start
 * start gfps finder adv
 * @param duration_10ms gfps finder adv duration time in 10ms
 */
void gfps_finder_adv_start(uint16_t duration_10ms);

/**
 * @brief gfps_finder_adv_stop
 * stop gfps finder adv
 * @param app_cause @ref app_adv_stop_cause.h
 */
void gfps_finder_adv_stop(uint8_t app_cause);

/**
 * @brief gfps_finder_adv_get_adv_state
 *  get gfps finder adv state @ref T_BLE_EXT_ADV_MGR_STATE
 * @return T_BLE_EXT_ADV_MGR_STATE
 */
T_BLE_EXT_ADV_MGR_STATE gfps_finder_adv_get_adv_state(void);

/**
 * @brief set frame type
 *
 * the frame type should be set to 0x41 when Unwanted Tracking Protection mode is on.
 * the frame type should be set to 0x40 when Unwanted Tracking Protection mode is off.
 * @param frame_type
 */
void gfps_finder_adv_update_frame_type(uint8_t frame_type);

/**
 * @brief gfps_finder_adv_update_adv_ei_hash
 * update adv EI and hash
 * @param p_ei  20 bytes indicating the current E2EE EID advertised by the beacon
 * @param hash
 */
void gfps_finder_adv_update_adv_ei_hash(uint8_t *p_ei, uint8_t hash);

/**
 * @brief update hash
 *
 * @param hash
 */
void gfps_finder_adv_update_hash(uint8_t hash);

/**
 * @brief update resolvable private address
 *
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE gfps_finder_adv_update_rpa(void);

/**
 * @brief  user can disable the device (temporarily stop advertising) as required in the spec to ensure compliance with unwanted tracking protections.
 * Google will display this URL in the UI when an unwanted tracker alert is triggered.
 *
 * @param key_stop true:key press to temporarily stop advertising
 * if key_stop is true, This flag will be maintained until the next time you power on the device.
 */
void gfps_finder_adv_set_key_stop_tracking_flag(bool key_stop);

/**
 * @brief update advertising interval
 *
 * @param adv_interval unit 0.625ms, range (32,3200)
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE gfps_finder_adv_update_adv_interval(uint32_t adv_interval);

/**
 * @brief handle mmi action
 *
 */
void app_gfps_finder_adv_handle_mmi_action(void);

/**
 * @brief gfps_finder_adv_init
 * init gfps finder adv
 * @param p_adv_ei  20 bytes indicating the current E2EE EID advertised by the beacon.
 */
void gfps_finder_adv_init(uint8_t *p_adv_ei, uint8_t hash);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
