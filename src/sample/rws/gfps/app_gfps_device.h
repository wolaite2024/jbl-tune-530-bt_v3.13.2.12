/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_DEVICE_H_
#define _APP_GFPS_DEVICE_H_

#include "remote.h"
#include "btm.h"
#include "app_ble_gap.h"
#if GFPS_FEATURE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */

/**
 * @brief app_gfps_device_handle_remote_conn_cmpl
 *
 */
void app_gfps_device_handle_remote_conn_cmpl(void);

/**
 * @brief app_gfps_device_handle_remote_disconnect_cmpl
 *
 */
void app_gfps_device_handle_remote_disconnect_cmpl(void);

/**
 * @brief app_gfps_device_handle_factory_reset
 *
 */
void app_gfps_device_handle_factory_reset(void);

/**
 * @brief app_gfps_device_handle_power_off
 *
 */
void app_gfps_device_handle_power_off(void);

/**
 * @brief app_gfps_device_handle_power_on
 *
 * @param is_pairing true: in pairing mode, false: not in pairing mode
 */
void app_gfps_device_handle_power_on(bool is_pairing);

/**
 * @brief app_gfps_device_handle_radio_mode
 *
 * @param radio_mode @ref T_BT_DEVICE_MODE
 */
void app_gfps_device_handle_radio_mode(T_BT_DEVICE_MODE radio_mode);

/**
 * @brief app_gfps_device_handle_role_swap
 *
 * @param device_role @ref T_REMOTE_SESSION_ROLE
 */
void app_gfps_device_handle_role_swap(T_REMOTE_SESSION_ROLE device_role);

/**
 * @brief app_gfps_device_handle_b2s_link_disconnected
 *
 */
void app_gfps_device_handle_b2s_link_disconnected(void);

/**
 * @brief app_gfps_device_handle_ble_link_disconnected
 *
 * @param local_disc_cause
 */
void app_gfps_device_handle_ble_link_disconnected(uint8_t local_disc_cause);

/**
 * @brief app_gfps_device_handle_b2s_ble_bonded
 *
 * @param conn_id                 le connection id
 * @param p_remote_identity_addr  remote identity address
 */
void app_gfps_device_handle_b2s_ble_bonded(uint8_t conn_id, uint8_t *p_remote_identity_addr);

/**
 * @brief app_gfps_device_check_state
 *
 * check gfps advertising state
 *
 * @param device_role @ref T_REMOTE_SESSION_ROLE
 */
void app_gfps_device_check_state(T_REMOTE_SESSION_ROLE device_role);
/** End of APP_RWS_GFPS
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
