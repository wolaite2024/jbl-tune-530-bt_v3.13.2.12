/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_DEVICE_H_
#define _APP_BLE_DEVICE_H_

#include "app_roleswap.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_DEVICE App BLE Device
  * @brief App BLE Device
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_DEVICE_Exported_Functions App Ble Device Functions
    * @{
    */
/**
    * @brief  Ble Device module init
    * @param  void
    * @return void
    */
void app_ble_device_init(void);
void app_ble_device_handle_factory_reset(void);
void app_ble_device_handle_power_on(void);
void app_ble_device_handle_power_off(void);
void app_ble_device_handle_radio_mode(T_BT_DEVICE_MODE radio_mode);
void app_ble_device_handle_enter_pair_mode(void);
void app_ble_device_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode);
void app_ble_device_handle_b2s_bt_connected(uint8_t *bd_addr);
void app_ble_device_handle_b2s_bt_disconnected(uint8_t *bd_addr);
void app_ble_device_handle_remote_conn_cmpl(void);
void app_ble_device_handle_remote_disconn_cmpl(void);
void app_ble_device_handle_role_swap(T_BT_EVENT_PARAM_REMOTE_ROLESWAP_STATUS *p_role_swap_status);

bool app_ble_device_is_active(void);
void app_ble_rtk_adv_start(void);
void app_ble_device_req_remote_device_name_timer_stop(void);

/** @} */ /* End of group APP_BLE_DEVICE_Exported_Functions */

/** End of APP_BLE_DEVICE
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_DEVICE_H_ */
