/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TUYA_DEVICE_H_
#define _APP_TUYA_DEVICE_H_

#include "app_flags.h"
#if F_APP_TUYA_SUPPORT
#include "btm.h"
#include "app_link_util.h"
#include "app_msg.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_XIAOWEI App Xiaowei
  * @brief App XIAOWEI
  * @{
  */

/** @defgroup APP_XIAOWEI_DEV App xiaowei device
  * @brief App xiaowei device
  * @{
  */

void app_tuya_handle_power_off(void);
void app_tuya_handle_power_on(void);
void app_tuya_handle_radio_mode(T_BT_DEVICE_MODE radio_mode);
void app_tuya_handle_enter_pair_mode(void);
void app_tuya_handle_engage_role_decided(void);
void app_tuya_handle_remote_conn_cmpl(void);
void app_tuya_handle_remote_disconn_cmpl(void);
void app_tuya_handle_factory_reset(void);
void app_tuya_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);
void app_tuya_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);
void app_tuya_handle_b2s_bt_disconnected(uint8_t *bd_addr);
void app_tuya_handle_b2s_ble_connected(uint8_t conn_id);
void app_tuya_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                          uint8_t local_disc_cause, uint16_t disc_cause);
void app_tuya_device_init(void);
void app_tuya_device_start_adv_timer(uint16_t timeout_sec);
bool tuya_cmd_register(void);
void rtk_tuya_handle_event_msg(T_IO_MSG io_msg);
/** End of APP_XIAOWEI_DEV
* @}
*/

/** End of APP_RWS_XIAOWEI
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
