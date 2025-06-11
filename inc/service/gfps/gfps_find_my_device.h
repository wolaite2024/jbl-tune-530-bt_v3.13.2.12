/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _GFPS_FIND_MY_DEVICE_H_
#define _GFPS_FIND_MY_DEVICE_H_

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "gfps.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*One byte indicating the protocols major version number. Currently it is 0x01.*/
#define GFPS_FINDER_SPEC_VERSION 0x01

#define GFPS_FINDER_SECP160R1 0x00
#define GFPS_FINDER_SECP256R1 0x01

/**
 * @brief
 * 00: battery level indication unsupported
 * 01: normal battery level
 * 10: low battery level
 * 11: critically low battery level (battery replacement needed soon)
 *
 */
#define GFPS_FINDER_BATTERY_UNSUPPORT      0x00
#define GFPS_FINDER_BATTERY_NORMAL         0x01
#define GFPS_FINDER_BATTERY_LOW            0x02
#define GFPS_FINDER_BATTERY_CRITICALLY_LOW 0x03
typedef struct
{
    uint8_t utp_mode: 1;
    uint8_t battery_level: 2;
    uint8_t rsv: 5;
} T_GFPS_FINDER_HASH_FLAG;

/**
 * @brief GFPS Finder GATT Error Codes
 * GFPS_FINDER_CAUSE_SUCCESS         success
 * GFPS_FINDER_CAUSE_UNAUTHEN        authen fail
 * GFPS_FINDER_CAUSE_INVALID_VALUE   value fail
 * GFPS_FINDER_CAUSE_NO_USER_CONSENT not in pairing mode
 */
typedef enum
{
    GFPS_FINDER_CAUSE_SUCCESS         = 0x0000,
    GFPS_FINDER_CAUSE_UNAUTHEN        = 0x0480,
    GFPS_FINDER_CAUSE_INVALID_VALUE   = 0x0481,
    GFPS_FINDER_CAUSE_NO_USER_CONSENT = 0x0482,
} T_GFPS_FINDER_CAUSE;

/**
 * @brief GFPS Finder EIK struct
 * valid   true means this EIK is valid.
 * key     32 bytes EIK
 */
typedef struct
{
    bool valid;
    uint8_t rsv[3];
    uint8_t key[32];
} T_GFPS_EIK;

typedef enum
{
    GFPS_FINDER_EVT_SET_EIK            = 0x00,
    GFPS_FINDER_EVT_CLEAR_EIK          = 0x01,
    GFPS_FINDER_EVT_KEY_RECOVERY       = 0x02,
    GFPS_FINDER_EVT_RING               = 0x03,
    GFPS_FINDER_EVT_RING_STATE         = 0x04,
    GFPS_FINDER_EVT_UTP_ACTIVE         = 0x05,
    GFPS_FINDER_EVT_UTP_DEACTIVE       = 0x06,
    GFPS_FINDER_EVT_SET_OWNERKEY_VALID = 0x07,
} T_GFPS_FINDER_CB_EVENT;

/**
 * @brief A byte indicating the number of components capable of ringing:
 * 0x00: indicates that the device is incapable of ringing.
 * 0x01: indicates that only a single component is capable of ringing.
 * 0x02: indicates that two components, left and right buds, are capable of ringing individually.
 * 0x03: indicates that three components, left and right buds and the case, are capable of ringing individually
 */
typedef enum
{
    GFPS_FINDER_RING_NOT_SUPPORT = 0x00,
    GFPS_FINDER_RING_SINGLE = 0x01,
    GFPS_FINDER_RING_RWS = 0x02,
} T_GFPS_FINDER_RING_CAP;

typedef enum
{
    GFPS_FINDER_RING_VOLUME_DISABLE,
    GFPS_FINDER_RING_VOLUME_ENABLE,
} T_GFPS_FINDER_RING_VOLUME_STATE;

typedef enum
{
    GFPS_FINDER_RING_VOLUME_DEFAULT,
    GFPS_FINDER_RING_VOLUME_LOW,
    GFPS_FINDER_RING_VOLUME_MEDIUM,
    GFPS_FINDER_RING_VOLUME_HIGH,
} T_GFPS_FINDER_RING_VOLUME_LEVEL;
typedef struct
{
    uint8_t conn_id;
    uint8_t service_id;
    uint8_t ring_type;
    uint16_t ring_time;
    uint8_t ring_volume_level;
} T_GFPS_FINDER_RING;

/**
 * @brief A byte indicating the new ringing state:
 * 0x00: started
 * 0x01: failed to start or stop (all requested components are out of range)
 * 0x02: stopped (timeout)
 * 0x03: stopped (button press)
 * 0x04: stopped (GATT request)
 *
 */
typedef enum
{
    GFPS_FINDER_RING_STARTED      = 0x00,
    GFPS_FINDER_RING_FAIL         = 0x01,
    GFPS_FINDER_RING_TIMEOUT_STOP = 0x02,
    GFPS_FINDER_RING_BUTTON_STOP  = 0x03,
    GFPS_FINDER_RING_GATT_STOP    = 0x04,
} T_GFPS_FINDER_RING_STATE;

typedef union
{
    T_GFPS_EIK eik;
    T_GFPS_FINDER_RING ring;
} T_GFPS_FINDER_CB_MSG;

typedef struct
{
    T_GFPS_FINDER_CB_EVENT evt;
    T_GFPS_FINDER_CB_MSG msg_data;
} T_GFPS_FINDER_CB_DATA;

typedef T_GFPS_FINDER_CAUSE(*P_FUN_GFPS_FINDER_CB)(T_GFPS_FINDER_CB_DATA *p_data);

typedef struct
{
    T_GFPS_EIK eik;
    uint32_t clock_value;
    uint8_t random_nonce[8];
    uint8_t adv_ei[32];//secp160r1 ei is 20 bytes, secp256r1 ei is 32 bytes
    uint8_t beacon_data[32];
    uint8_t ring_components_num;
    uint8_t ring_volume_modify;
    uint8_t secp_mode;
    bool    skip_ring_authen;
    T_GFPS_FINDER_HASH_FLAG hash_flag;
    T_GFPS_EIK temp_modified_eik;
} T_GFPS_FINDER;

/**
 * @brief gfps_finder_init
 *
 * @param p_finder @ref T_GFPS_FINDER
 * @param p_fun_cb @ref P_FUN_GFPS_FINDER_CB
 * @return true    init success
 * @return false   init fail
 */
bool gfps_finder_init(T_GFPS_FINDER *p_finder, P_FUN_GFPS_FINDER_CB p_fun_cb);

/**
 * @brief gfps_finder_handle_beacon_action
 * Fast Pair Service characteristic Beacon Actions.
 * The operations required by this extension are performed as a write operation.
 * @param conn_id
 * @param service_id
 * @param length
 * @param p_value
 * @return T_GFPS_FINDER_CAUSE
 */
T_GFPS_FINDER_CAUSE gfps_finder_handle_beacon_action(uint8_t conn_id, T_SERVER_ID service_id,
                                                     uint16_t length, uint8_t *p_value);

/**
 * @brief  gfps_finder_rsp_ring_request
 *
 * @param conn_id
 * @param service_id
 * @param ring_state @ref T_GFPS_FINDER_RING_STATE
 * @param ring_type
 * Bit 1 (0x01): ring right
 * Bit 2 (0x02): ring left
 * Bit 3 (0x04): ring case
 * @param timeout
 * Two bytes indicating the remaining time for ringing in deciseconds.
 * Note that if the device has stopped ringing, 0x0000 should be returned
 * @return true
 * @return false
 */
bool gfps_finder_rsp_ring_request(uint8_t conn_id, uint8_t service_id, uint8_t ring_state,
                                  uint8_t ring_type, uint16_t timeout);

/**
 * @brief gfps_finder_rsp_ring_state
 *
 * @param conn_id
 * @param service_id
 * @param ring_type
 * Bit 1 (0x01): ring right
 * Bit 2 (0x02): ring left
 * Bit 3 (0x04): ring case
 * @param timeout
 * Two bytes indicating the remaining time for ringing in deciseconds.
 * Note that if the device has stopped ringing, 0x0000 should be returned
 * @return true
 * @return false
 */
bool gfps_finder_rsp_ring_state(uint8_t conn_id, uint8_t service_id,
                                uint8_t ring_type, uint8_t timeout);

/**
 * @brief gfps_finder_encrypted_beacon_data
 * A random r is generated by AES-ECB-256 encrypting the beacon data with the identity key
 * @param p_eik
 * @param p_input  beacon_data
 * @param p_output  r
 * @return true
 * @return false
 */
bool gfps_finder_encrypted_beacon_data(uint8_t *p_eik, uint8_t *p_input, uint8_t *p_output);

bool gfps_finder_get_recovery_key(uint8_t *p_recovery_key);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
