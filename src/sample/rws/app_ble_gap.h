/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_GAP_H_
#define _APP_BLE_GAP_H_

#include <stdint.h>
#include <stdbool.h>

#include "app_msg.h"
#include "gap.h"
#include "ble_ext_adv.h"
#include "app_link_util.h"
#include "app_adv_stop_cause.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_GAP App Ble Gap
  * @brief App Ble Gap
  * @{
  */
#define RWS_LE_DEFAULT_MIN_CONN_INTERVAL   (0x12)//(0x06)//12*1.25 = 15ms
#define RWS_LE_DEFAULT_MAX_CONN_INTERVAL   (0x24)//(0x06)//24*1.25 = 30ms
#define RWS_LE_DEFAULT_SLAVE_LATENCY       (0)
#define RWS_LE_DEFAULT_SUPERVISION_TIMEOUT (500)//500*10 = 5000ms
/**  @brief  App define le link connection state status */
typedef enum
{
    LE_LINK_STATE_DISCONNECTED,
    LE_LINK_STATE_CONNECTING,
    LE_LINK_STATE_CONNECTED,
    LE_LINK_STATE_DISCONNECTING,
} T_LE_LINK_STATE;

/**  @brief  App define le link encryption state */
typedef enum
{
    LE_LINK_UNENCRYPTIONED = 0,
    LE_LINK_ENCRYPTIONED = 1,
    LE_LINK_ERROR = 2,
} T_LE_LINK_ENCRYPTION_STATE;

typedef enum
{
    LE_LOCAL_DISC_CAUSE_UNKNOWN = 0x00,
    LE_LOCAL_DISC_CAUSE_POWER_OFF = 0x01,
    LE_LOCAL_DISC_CAUSE_SWITCH_TO_OTA = 0x02,
    LE_LOCAL_DISC_CAUSE_AUTHEN_FAILED = 0x03,
    LE_LOCAL_DISC_CAUSE_TTS_DISC = 0x04,
    LE_LOCAL_DISC_CAUSE_OTA_RESET = 0x05,
    LE_LOCAL_DISC_CAUSE_GFPS_STOP = 0x06,
    LE_LOCAL_DISC_CAUSE_GFPS_FAILED = 0x07,
    LE_LOCAL_DISC_CAUSE_ROLESWAP = 0x08,
    LE_LOCAL_DISC_CAUSE_SECONDARY = 0x09,
    LE_LOCAL_DISC_CAUSE_TUYA = 0x0A,
    LE_LOCAL_DISC_CAUSE_ENGAGE_STREAM_PRIOIRTY = 0x0B,
    LE_LOCAL_DISC_CAUSE_GFPS_LINK_FULL = 0x0C,
    LE_LOCAL_DISC_CAUSE_GFPS_FINDER = 0x0D,
    LE_LOCAL_DISC_CAUSE_RWS_OTA_SUCCESS = 0x0E,
    LE_LOCAL_DISC_CAUSE_HARMAN_DEVICE_DISC = 0x0F,
    LE_LOCAL_DISC_CAUSE_APP_DISC = 0x10,
    LE_LOCAL_DISC_CAUSE_ENTER_PAIRING = 0x11,
    LE_LOCAL_DISC_CAUSE_GFPS_SASS_DISC = 0x12,
} T_LE_LOCAL_DISC_CAUSE;

extern uint8_t scan_rsp_data_len;
extern uint8_t scan_rsp_data[GAP_MAX_LEGACY_ADV_LEN];

/**
 * @brief Initialize BLE GAP parameters
 * such as device name and appearance, bond manager parameters, scan parameters,...
 *
 */
void app_ble_gap_param_init(void);

/**
 * @brief Initialize GAP module and register callback function.
 *
 */
void app_ble_gap_init(void);

/**
 * @brief Generate scan response data
 *
 * @param[out] p_scan_len
 * @param[out] p_scan_data
 * @return true  success
 * @return false fail
 */
bool app_ble_gap_gen_scan_rsp_data(uint8_t *p_scan_len, uint8_t *p_scan_data);

/**
 * @brief Disconnect BLE link
 *
 * @param[in] p_link      the link to be disconnected
 * @param[in] disc_cause  @ref T_LE_LOCAL_DISC_CAUSE
 * @return true  success
 * @return false fail
 */
bool app_ble_gap_disconnect(T_APP_LE_LINK *p_link, T_LE_LOCAL_DISC_CAUSE disc_cause);

/**
 * @brief Disconnect all BLE links
 *
 * @param disc_cause @ref T_LE_LOCAL_DISC_CAUSE
 */
void app_ble_gap_disconnect_all(T_LE_LOCAL_DISC_CAUSE disc_cause);

/**
    * @brief  All the bt gap msg events are pre-handled in this function.
    *         Then the event handling function shall be called according to the subType of BEE_IO_MSG.
    * @param  p_bee_io_msg  pointer to bee io msg
    * @return void
    */
void app_ble_gap_handle_gap_msg(T_IO_MSG *p_bee_io_msg);

/**
    * @brief  app ble get link encryption status
    * @param  T_APP_LE_LINK *
    * @return @ref T_LE_LINK_ENCRYPTION_STATE
    */
T_LE_LINK_ENCRYPTION_STATE app_ble_gap_get_link_encryption_status(T_APP_LE_LINK *p_link);

/**
    * @brief  app ble get last link connection handle for multi-link
    * @param  void
    * @return uint16_t
    */
uint16_t app_ble_gap_last_src_conn_handle(void);

/** End of APP_BLE_GAP
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_GAP_H_ */
