/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_H_
#define _APP_GFPS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "btm.h"
#include "gfps.h"
#include "remote.h"
#include "os_queue.h"
#include "ble_ext_adv.h"
#if GFPS_SASS_SUPPORT
#include "gfps_sass_conn_status.h"
#endif
/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */

typedef enum
{
    GFPS_LOCATOR_TRACKER      = 1,
    GFPS_WATCH                = 146,
    GFPS_HEADPHONES           = 149,
    GFPS_EARPHONES            = 150,
} T_GFPS_DEVICE_TYPE;

typedef struct
{
    bool is_gfps_pairing;
    bool gfps_raw_passkey_received;
    bool le_bond_confirm_pending;
    bool edr_bond_confirm_pending;
    bool auth_param_change;

    uint8_t  io_cap;
    uint16_t auth_flags;
    uint32_t gfps_raw_passkey;
    uint32_t le_bond_passkey;
    uint32_t edr_bond_passkey;
    uint8_t  edr_bond_bd_addr[6];

    uint8_t          gfps_conn_id;
    uint16_t         gfps_msg_cid;
    T_GAP_CONN_STATE gfps_conn_state;

    uint8_t  *p_gfps_cmd;
    uint16_t gfps_cmd_len;
} T_GFPS_LINK;

typedef enum
{
    GFPS_ACTION_IDLE,
    GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID,
    GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE,
    GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI,
} T_GFPS_ACTION;

typedef struct
{
    uint8_t                    gfps_adv_handle;
    T_BLE_EXT_ADV_MGR_STATE    gfps_adv_state;
    T_GFPS_ACTION              gfps_curr_action;

    T_SERVER_ID                gfps_id;
    uint8_t                    current_conn_id;
    T_GFPS_BATTERY_INFO        gfps_battery_info;
    uint8_t                    random_address[6];
    bool                       gfps_linkback_init;
    bool                       force_enter_pair_mode;

    bool                       is_gfps_additional_pairing;
    uint8_t                    additional_default_io_cap;
    uint8_t                    additional_default_auth_flags;
    uint8_t                    gfps_additional_conn_id;
    bool                       gfps_additional_passkey_from_gfps_received;
    bool                       gfps_additional_passkey_from_gfps_pending;
    uint8_t                    gfps_additional_bond_addr[6];
    uint32_t                   gfps_additional_passkey_from_stack;
    uint32_t                   gfps_additional_passkey_from_gfps;
    T_GFPS_PASSKEY_STATUS_CODE status_code;
    //T_GFPS_ADDITIONAL_BOND_TYPE gfps_additional_bond_type;
} T_GFPS_DB;

typedef enum
{
    GFPS_KEY_FORCE_ENTER_PAIR_MODE        = 0x00,
    GFPS_LE_DISCONN_FORCE_ENTER_PAIR_MODE = 0x01,
    GFPS_EXIT_PAIR_MODE                   = 0x02,
} T_GFPS_FORCE_ENTER_PAIR_MODE;

/**
 * @brief google Fast pair initialize
 *
 * @param void
 * @return void
 */
void app_gfps_init(void);

/**
 * @brief gfps adv initialize
 *
 */
void app_gfps_adv_init(void);

/**
 * @brief start gfps adv according to expected gfps action
 *
 * @param gfps_next_action @ref T_GFPS_ACTION
 * @return true  success
 * @return false fail
 */
bool app_gfps_next_action(T_GFPS_ACTION gfps_next_action);

/**
 * @brief get resolvable private address used by gfps adv
 *
 * @param random_bd random reslovable privacy address
 * @return void
 */
void app_gfps_get_random_addr(uint8_t *random_bd);

/**
 * @brief update gfps adv interval
 *
 * @param adv_interval Range:[32,400], Unit:0.625ms
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE app_gfps_adv_update_adv_interval(uint32_t adv_interval);

/**
 * @brief handle user confirmation for legacy pairing.
 *
 * @param confirm_req  bt user confirm request
 * @return void
 */
void app_gfps_handle_bt_user_confirm(T_BT_EVENT_PARAM_LINK_USER_CONFIRMATION_REQ confirm_req);

/**
 * @brief handle user confirmation for le pairing.
 *
 * @param conn_id BLE connection id
 * @return void
 */
void app_gfps_handle_ble_user_confirm(uint8_t conn_id);

/**
 * @brief enter non discoverable mode
 *
 * @param void
 * @return void
 */
void app_gfps_enter_nondiscoverable_mode(void);

/**
 * @brief gfps handle case status
 *
 * @param open 1:case open, 0: case close
 */
void app_gfps_handle_case_status(bool open);

/**
 * @brief gfps handle b2s connected
 *
 * @param bd_addr
 */
void app_gfps_handle_b2s_connected(uint8_t *bd_addr);

/**
 * @brief gfps set ble connection parameter
 *
 * @param conn_id ble connection id
 */
void app_gfps_set_ble_conn_param(uint8_t conn_id);

#if GFPS_SASS_SUPPORT
bool app_gfps_gen_conn_status(T_SASS_CONN_STATUS_FIELD *conn_status);
void app_gfps_notify_conn_status(void);
void app_gfps_adv_update_conn_status(void);
void app_gfps_adv_check_sass_conn_status(void);
#endif

/**
 * @brief start gfps adv and le audio adv for gfps pairing
 *
 * @param device_role    bud role @ref T_REMOTE_SESSION_ROLE
 * @return void
 */
void app_gfps_le_device_adv_start(T_REMOTE_SESSION_ROLE device_role);

/**
 * @brief gfps handle ble connection information
 *
 * @param remote_bd_addr   remote bd address
 * @param remote_addr_type remote bd address type
 * @return true  success
 * @return false fail
 */
bool app_gfps_ble_conn_info_handle(uint8_t *remote_bd_addr, uint8_t remote_addr_type);

/**
 * @brief gfps le linkback information init
 *
 * @param conn_id le connection id
 */
void app_gfps_linkback_info_init(uint8_t conn_id);

/**
 * @brief gfps force enter pairing mode
 *
 * @param status @ref T_GFPS_FORCE_ENTER_PAIR_MODE
 */
void app_gfps_force_enter_pairing_mode(uint8_t status);

/**
 * @brief secondary bud handle addition ble user confirm
 *
 * @param conn_id gfps addition connection id
 */
void app_gfps_handle_additional_ble_user_confirm(uint8_t conn_id);

/**
 * @brief secondary bud handle addition ble bond info
 *
 * @param buf @ref T_GFPS_ADDITIONAL_BOND_DATA
 */
void app_gfps_sec_handle_addition_bond_info(uint8_t *buf);

/**
 * @brief primary bud handle addition ble bond info
 *
 * @param buf @ref T_GFPS_ADDITIONAL_BOND_DATA
 */
void app_gfps_pri_handle_additional_bond_info(uint8_t *buf);

/**
 * @brief secondary bud handle addition change IO CAP info
 *
 * @param change_io_cap true change to GAP_IO_CAP_DISPLAY_YES_NO
 */
void app_gfps_sec_handle_additional_io_cap(bool change_io_cap);

/**
 * @brief gfps update resolvable private address
 *
 */
void app_gfps_update_rpa(void);

/**
 * @brief gfps update announcement
 *
 */
void app_gfps_update_announcement(uint8_t announcement);
/** End of APP_RWS_GFPS
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
