/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_GFPS_H_
#define _BT_GFPS_H_


#include <stdint.h>
#include <stdbool.h>
#include "bt_rfc.h"
#include "gfps_sass_conn_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup GFPS GFPS
  * @brief
  * @{
  */


/**
 * @defgroup BT_GFPS BT GFPS profile
 * @brief Provides BT GFPS profile interfaces.
 * @{
 */

/** @brief Message group(1 byte), Message code(1 byte), Additional data length(2 byte) */
#define GFPS_SASS_VERSION           0x0102
#define GFPS_HEARABLE_VERSION       0x01

#define GFPS_HEADER_LEN             0x04
#define GFPS_ADDITIONAL_DATA_LEN    0x02

#define GFPS_MODEL_ID_LEN           0x0003
#define GFPS_BATTERY_LEN            0x0003
#define GFPS_ACTIVE_COMPONENTS_LEN  0x0001
#define GFPS_SUPPORT_CPBS_LEN       0x0001
#define GFPS_PLATFORM_TYPE_LEN      0x0002
#define GFPS_RING_LEN                   0x0001
#define GFPS_RING_TIMEOUT_LEN           0x0001
#define GFPS_RING_ACK_LEN               0x0004
#define GFPS_RING_NAK_LEN               0x0004
#define GFPS_SESSION_NONCE_LEN          0x0008
#define GFPS_MESSAGE_NONCE_LEN          0x0008
#define GFPS_MAC_LEN                    0x0008
#define GFPS_EDDYSTONE_CAP_LEN          0x0007
#define GFPS_EDDYSTONE_SECP256_INFO_LEN 0x0024
#define GFPS_EDDYSTONE_SECP160_INFO_LEN 0x0018
/** @brief left and right bud active type */
#define GFPS_RIGHT_ACTIVE           0x01
#define GFPS_LEFT_ACTIVE            0x02

/** @brief remote support capabilities type */
#define GFPS_COMPANION_APP          0x01
#define GFPS_SILENCE_MODE           0x02

/** @brief action ringtone type */
#define GFPS_ALL_STOP               0x00
#define GFPS_RIGHT_RING             0x01
#define GFPS_LEFT_RING              0x02
#define GFPS_ALL_RING               0x03

/** @brief NAK reason type */
#define GFPS_NOT_SUPPORT             0x00
#define GFPS_DEVICE_BUSY             0x01
#define GFPS_NOT_ALLOW               0x02
/*0x03: Not allowed due to incorrect message authentication code*/
#define GFPS_ERROR_MAC               0x03
#define GFPS_REDUNDANT_DEVICE_ACTION 0x04

/**
 * @brief ANC state no anc
 */
#define GFPS_NO_ANC                 0x0000

/**
 * @brief ANC state pattern 1
 * 0x01, pattern 1: only on-off ANC. The second byte:
 * 0x0, ANC - off
 * 0x1, ANC - on
 * for example:
 * pattern 1 ANC off = (GFPS_ANC_PATTERN1 | GFPS_ANC_PATTERN1_OFF)
 * pattern 1 ANC on = (GFPS_ANC_PATTERN1 | GFPS_ANC_PATTERN1_ON)
 */
#define GFPS_ANC_PATTERN1          0x0100
#define GFPS_ANC_PATTERN1_OFF      0x00
#define GFPS_ANC_PATTERN1_ON       0x01
/**
 * @brief ANC state pattern 2
 * 0x2, pattern 2: 3 levels ANC, off-medium-high. The second byte:
 * 0x0, ANC - off
 * 0x1, ANC - medium
 * 0x2, ANC - high
 * for example:
 * pattern 2 ANC off = (GFPS_ANC_PATTERN2 | GFPS_ANC_PATTERN2_OFF)
 * pattern 2 ANC medium = (GFPS_ANC_PATTERN2 | GFPS_ANC_PATTERN2_MEDIUM)
 * pattern 2 ANC high = (GFPS_ANC_PATTERN2 | GFPS_ANC_PATTERN2_HIGH)
 */
#define GFPS_ANC_PATTERN2          0x0200
#define GFPS_ANC_PATTERN2_OFF      0x00
#define GFPS_ANC_PATTERN2_MEDIUM   0x01
#define GFPS_ANC_PATTERN2_HIGH     0x02

/**
 * @brief ANC state pattern 3
 * 0x3, pattern 3: 3 ways ANC, transparent - ANC Off - ANC on. The second byte:
 * 0x0, transparent mode
 * 0x1, ANC - off
 * 0x2, ANC - on
 * for example:
 * pattern 3 ANC transparent mode = (GFPS_ANC_PATTERN3 | GFPS_ANC_PATTERN3_TRANS_MODE)
 * pattern 3 ANC off = (GFPS_ANC_PATTERN3 | GFPS_ANC_PATTERN3_OFF)
 * pattern 3 ANC on = (GFPS_ANC_PATTERN3 | GFPS_ANC_PATTERN3_ON)
 */
#define GFPS_ANC_PATTERN3            0x0300
#define GFPS_ANC_PATTERN3_TRANS_MODE 0x00
#define GFPS_ANC_PATTERN3_OFF        0x01
#define GFPS_ANC_PATTERN3_ON         0x02

/**
 * @brief SASS capability flags
 * GFPS_SASS_STATE_ON  Bit 0 (octet 6, MSB): SASS state
 * 1, if SASS state is on
 * 0, otherwise
 *
 * GFPS_SASS_MULTI_POINT_SUPPORT  Bit 1: multipoint configure
 * 1, if the device supports multipoint and it can be switched between on and off
 * 0, otherwise (does not support multipoint or multipoint is always on)
 *
 * GFPS_SASS_MULTI_POINT_ENABLE  Bit 2: multipoint current state
 * 1, if multipoint is on
 * 0, otherwise
 *
 * GFPS_SASS_ON_HEAD_SUPPORT  Bit 3: on-head detection
 * 1, if this device supports on-head detection (even if on-head detection is turned off now)
 * 0, otherwise
 *
 * GFPS_SASS_ON_HEAD_ENABLE  Bit 4: on-head detection current state
 * 1, if on-head detection is turned on
 * 0, otherwise (does not support on-head detection or on-head detection is disabled)
 * All the other bits are reserved, default 0.
 */
#define GFPS_SASS_STATE_ON                  0x8000
#define GFPS_SASS_MULTI_POINT_CFG_SUPPORT   0x4000
#define GFPS_SASS_MULTI_POINT_ENABLE        0x2000
#define GFPS_SASS_ON_HEAD_SUPPORT           0x1000
#define GFPS_SASS_ON_HEAD_ENABLE            0x0800

/**
 * @brief Multipoint switching preference flag
 * Bit 0 (MSB): A2DP vs A2DP (default 0)
 * Bit 1: HFP vs HFP (default 0)
 * Bit 2: A2DP vs HFP (default 0)
 * Bit 3: HFP vs A2DP (default 1)
 * Bit 4 - 7: reserved
 * Above represents as "new profile request" vs "current active profile"
 * 0 for not switching, 1 for switching.
 *
 */
#define GFPS_SWITCH_PREFER_A2DP_A2DP 0x80
#define GFPS_SWITCH_PREFER_HFP_HFP   0x40
#define GFPS_SWITCH_PREFER_A2DP_HFP  0x20
#define GFPS_SWITCH_PREFER_HFP_A2DP  0x10

/**
 * @brief Switching active audio source event
 * Bit 0 (MSB): 1 switch to this device, 0 switch to second connected device
 * Bit 1: 1 resume playing on switch to device after switching, 0 otherwise.
 * Resuming playing means the Provider sends a PLAY notification to the Seeker through AVRCP profile.
 * If the previous state (before switched away) was not PLAY, the Provider should ignore this flag.
 * Bit 2: 1 reject SCO on switched away device, 0 otherwise
 * Bit 3: 1 disconnect Bluetooth on switch away device, 0 otherwise.
 * Bit 4 - 7: reserved.
 *
 */
#define GFPS_SWITCH_TO_THIS_DEVICE  0x80
#define GFPS_RESUME_PLAYING         0x40
#define GFPS_REJECT_SCO             0x20
#define GFPS_DISCONN_BLUETOOTH      0x10

/**
 * @brief Switching reason
 * 0x00: Unspecified
 * 0x01: A2DP streaming
 * 0x02: HFP
 *
 */
#define GFPS_MULTIPOINT_SWITCH_REASON_UNKNOWN 0x00
#define GFPS_MULTIPOINT_SWITCH_REASON_A2DP    0x01
#define GFPS_MULTIPOINT_SWITCH_REASON_HFP     0x02

/**
 * @brief Target device
 * 0x01: this device
 * 0x02: another connected device
 *
 */
#define GFPS_MULTIPOINT_SWITCH_THIS_DEVICE    0x01
#define GFPS_MULTIPOINT_SWITCH_OTHER_DEVICE   0x02

/**
 * @brief varies
 * 0x00: this Seeker is not the active device
 * 0x01: this Seeker is the active device
 *
 */
#define GFPS_NON_ACTIVE_DEV                                       0x00
#define GFPS_ACTIVE_DEV                                           0x01
#define GFPS_NON_ACTIVE_DEV_WITH_ACTIVE_NON_SASS                  0x02
/**
 * @brief GFPS Message Group
 * GFPS_DEVICE_RUNTIME_CFG_EVENT: Device runtime configuration event
 * the Seeker will determine the optimal buffer size for current audio usage and send the new buffer size to the Provider
 *
 * GFPS_DEVICE_CAPABILITY_SYNC_EVENT: allow the Provider to directly push capabilities to the Seeker without a request first
 */
typedef enum
{
    GFPS_BLUETOOTH_EVENT              = 0x01,
    GFPS_COMPANION_APP_EVENT          = 0x02,
    GFPS_DEVICE_INFO_EVENT            = 0x03,
    GFPS_DEVICE_ACTION_EVENT          = 0x04,
    GFPS_DEVICE_RUNTIME_CFG_EVENT     = 0x05,
    GFPS_DEVICE_CAPABILITY_SYNC_EVENT = 0x06,
    GFPS_SMART_AUDIO_SWITCH           = 0x07,
    GFPS_HEARABLE_CONTROLEVENT        = 0x08,

    GFPS_ACKNOWLEDGEMENT_EVENT        = 0xFF,
} T_GFPS_MSG_GROUP;

/** @brief gfps message bluetooth event code */
typedef enum
{
    GFPS_ENABLE_SILENCE_MODE     = 0x01,
    GFPS_DISABLE_SILENCE_MODE    = 0x02,
} T_GFPS_MSG_BT_EVENT_CODE;

/** @brief gfps message companion app event code */
typedef enum
{
    GFPS_LOG_BUF_FULL            = 0x01,
} T_GFPS_MSG_APP_EVENT_CODE;

/** @brief gfps message device information event code */
typedef enum
{
    GFPS_MODEL_ID                = 0x01,
    GFPS_BLE_ADDR_UPDATED        = 0x02,
    GFPS_BATTERY_UPDATED         = 0x03,
    GFPS_REMAINING_BATTERY_TIME  = 0x04,
    GFPS_ACITVE_COMPONENTS_REQ   = 0x05,
    GFPS_ACITVE_COMPONENTS_RSP   = 0x06,
    GFPS_CAPABILITIES            = 0x07,
    GFPS_PLATFORM_TYPE           = 0x08,
    GFPS_FIRMWARE_REVISION       = 0x09,
    GFPS_SESSION_NONCE           = 0x0A,
    GFPS_EDDYSTONE_ID            = 0x0B,
} T_GFPS_MSG_DEVICE_INFO_EVENT_CODE;

typedef enum
{
    GFPS_PLATFORM_TYPE_ANDROID   = 0x01,
} T_GFPS_MSG_PLATFORM_TYPE_CODE;

/** @brief gfps message device action event code */
typedef enum
{
    GFPS_RING                   = 0x01,
} T_GFPS_MSG_DEVICE_ACTION_EVENT_CODE;

/**
 * @brief Device runtime configuration event message code
 * GFPS_DYNAMIC_BUFFER_SIZING: Dynamic buffer sizing   TODO: seeker to provider????
 */
typedef enum
{
    GFPS_DYNAMIC_BUFFER_SIZING  = 0x01,
} T_GFPS_MSG_DEVICE_RUNTIME_CFG_CODE;

/**
 * @brief Device capability sync event message code
 * We also allow the Provider to directly push capabilities to the Seeker without a request first
 * GFPS_REQ_CAPABILITY_UPDATE:sent from Seeker
 * GFPS_DYNAMIC_BUFFER_SIZE: send from provider
 */
typedef enum
{
    GFPS_REQ_CAPABILITY_UPDATE = 0x01,
    GFPS_DYNAMIC_BUFFER_SIZE   = 0x02,
    GFPS_EDDYSTONE_TRACKING    = 0x03,
} T_GFPS_MSG_DEVICE_CAPABILITY_SYNC_CODE;

/**
 * @brief Hearable control event message code
 * Active noise control:
 * GFPS_GET_ANC_STATE(send from seeker)
 * GFPS_SET_ANC_STATE(send from seeker)
 * GFPS_NOTIFY_ANC_STATE(send from provider)
 */
typedef enum
{
    GFPS_GET_ANC_STATE    = 0x11,
    GFPS_SET_ANC_STATE    = 0x12,
    GFPS_NOTIFY_ANC_STATE = 0x13,
} T_GFPS_MSG_HEARABLE_CONTROL_CODE;

typedef enum
{
    GFPS_CODEC_SBC          = 0x00,
    GFPS_CODEC_AAC          = 0x01,
    GFPS_CODEC_APTX         = 0x02,
    GFPS_CODEC_APTX_HD      = 0x03,
    GFPS_CODEC_LDAC         = 0x04,
    GFPS_CODEC_LC3_A2DP     = 0x05,
    GFPS_CODEC_LC3_LE_AUDIO = 0x20,
} T_GFPS_CODEC_TYPE;

/**
 * @brief Smart audio source switching message code
 * GFPS_GET_SASS_CAP:                  Get capability of SASS
 * GFPS_NOTIFY_SASS_CAP:               Notify capability of SASS
 * GFPS_SET_MULTI_POINT_STATE:         Set multipoint state
 * GFPS_SET_SWITCH_PREFER:             Set switching preference
 * GFPS_GET_SWITCH_PREFER:             Get switching preference
 * GFPS_NOTIFY_SWITCH_PREFER:          Notify switching preference
 * GFPS_SWITCH_ACTIVE_AUDIO_SOURCE:    Switch active audio source(to connected device)
 * GFPS_SWITCH_BACK:                   Switch back (to disconnected device)
 * GFPS_NOTIFY_MULTI_POINT_SWITCH_EVT: Notify multipoint-switch event
 * GFPS_GET_CONN_STATUS:               Get connection status
 * GFPS_NOTIFY_CONN_STATUS:            Notify connection status
 * GFPS_NOTIFY_SASS_INITIAL_CONN:      Notify SASS initiated connection
 * GFPS_IND_INUSE_ACCOUNTKEY:          Indicate in use account key
 * GFPS_SEND_CUSTOM_DATA:              Send custom data
 * GFPS_SET_DROP_CONN_TARGET:          Set drop connection target
 */
typedef enum
{
    GFPS_GET_SASS_CAP = 0x10,
    GFPS_NOTIFY_SASS_CAP = 0x11,
    GFPS_SET_MULTI_POINT_STATE = 0x12,

    GFPS_SET_SWITCH_PREFER = 0x20,
    GFPS_GET_SWITCH_PREFER = 0x21,
    GFPS_NOTIFY_SWITCH_PREFER = 0x22,

    GFPS_SWITCH_ACTIVE_AUDIO_SOURCE = 0x30,
    GFPS_SWITCH_BACK = 0x31,
    GFPS_NOTIFY_MULTI_POINT_SWITCH_EVT = 0x32,
    GFPS_GET_CONN_STATUS = 0x33,
    GFPS_NOTIFY_CONN_STATUS = 0x34,

    GFPS_NOTIFY_SASS_INITIAL_CONN = 0x40,
    GFPS_IND_INUSE_ACCOUNTKEY = 0x41,
    GFPS_SEND_CUSTOM_DATA   = 0x42,
    GFPS_SET_DROP_CONN_TARGET = 0x43,
} T_GFPS_MSG_SASS_CODE;

/** @brief gfps message acknowledgement event code */
typedef enum
{
    GFPS_ACK                    = 0x01,
    GFPS_NAK                    = 0x02,
} T_GFPS_MSG_ACKKNOWLEDGEMENT_CODE;

typedef void (* P_APP_GFPS_RFC_CBACK)(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *p_msg);

/**
  * @brief  Initialize GFPS profile manager.
  * @param  cback   callback function used to handle RFCOMM message
  * @param  server_chann   rfcomm channel num used for gfps
  * @return The result of initialization.
  * @retval true    Initialization has been completed successfully.
  * @retval false   Initialization was failed to complete.
  */
bool bt_gfps_rfc_init(P_APP_GFPS_RFC_CBACK cback, uint8_t server_chann);

/**
  * @brief  Send a request to create a RFCOMM connection.
  * @param  bd_addr             remote bd_addr
  * @param  remote_server_chann The remote server channel which can be found from the sdp info.
  * @param  local_server_chann  The local server channel of the service that sending this request.
  * @param  frame_size          The max frame_size supported by local device.
  * @param  init_credits        The number of packet that remote can be send. This para is used for flow control. A
                                sending entity may send as many frames on a RFCOMM channel as it has credits;if the
                                credit count reaches zero,the sender will stop and wait for further credits from peer.
  * @return The status of sending connection request.
  * @retval true    Request has been sent successfully.
  * @retval false   Request was fail to send.
  */
bool bt_gfps_connect_req(uint8_t *bd_addr, uint8_t remote_server_chann, uint8_t local_server_chann,
                         uint16_t frame_size, uint8_t init_credits);

/**
  * @brief  Send a confirmation to accept or reject the received RFCOMM connection request.
  * @param  bd_addr            remote bd_addr
  * @param  local_server_chann local server channel
  * @param  accept             confirmation message
  * @arg    true    Accept the received RFCOMM connection request.
  * @arg    false   Reject the received RFCOMM connection request.
  * @param  frame_size         The max frame_size supported by local device.
  * @param  init_credits       The number of packet that remote can be send. This para is used for flow control.A sending
                               entity may send as many frames on a RFCOMM channel as it has credits;if the credit count
                               reaches zero,the sender will stop and wait for further credits from peer.
  * @return The result of sending confirmation.
  * @retval true    The confirmation has benn sent successfully.
  * @retval false   The confirmation was fail to send.
  */
bool bt_gfps_connect_cfm(uint8_t *bd_addr, uint8_t local_server_chann, bool accept,
                         uint16_t frame_size,
                         uint8_t init_credits);

/**
  * @brief  Send a request to disconnect a RFCOMM channel.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_disconnect_req(uint8_t *bd_addr, uint8_t local_server_chann);

/**
  * @brief  Enable silence mode to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_enable_silence_mode(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                 uint8_t local_server_chann);

/**
  * @brief  Disable silence mode to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_disable_silence_mode(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                  uint8_t local_server_chann);

/**
  * @brief  Send log buffer full command to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_send_log_buf_full(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                               uint8_t local_server_chann);

/**
  * @brief  Send gfps model id to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  p_data                 model id
  * @param  data_len               data length
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_send_model_id(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                           uint8_t local_server_chann, uint8_t *p_data,
                           uint16_t data_len);

/**
  * @brief  Send ble random address to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  ble_addr               ble random address
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_send_ble_addr(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                           uint8_t local_server_chann, uint8_t *ble_addr);

/**
  * @brief  Update current components and charger box battery to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  p_data                 components and charger box battery
  * @param  data_len               data length
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_updated_battery(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                             uint8_t local_server_chann, uint8_t *p_data,
                             uint16_t data_len);

/**
  * @brief  Response current active components state to remote device.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  p_data                 components state
  * @param  data_len               data length
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_active_components_rsp(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                   uint8_t local_server_chann, uint8_t *p_data,
                                   uint16_t data_len);

/**
 * @brief send ring status to seeker
 *
 * @param bd_addr                 remote bd_addr
 * @param local_server_chann      local server channel
 * @param p_data                  ring status
 * @param data_len                ring status data length
 * @return true    The ring status has benn sent successfully.
 * @return false   The ring status was fail to send.
 */
bool bt_gfps_send_ring_status(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                              uint8_t local_server_chann, uint8_t *p_data,
                              uint16_t data_len);

/**
 * @brief Upon receiving a capability update request (0x0601), if the Provider has enabled support for
 * Eddystone tracking it should respond as below
 *
 * @param bd_addr
 * @param local_server_chann
 * @param p_data uint8 Eddystone provisioning state + The current BLE MAC address of the device
 * @param data_len GFPS_EDDYSTONE_CAP_LEN=7
 * @return true
 * @return false
 */
bool bt_gfps_send_eddystone_capability(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                       uint8_t local_server_chann, uint8_t *p_data,
                                       uint16_t data_len);

/**
 * @brief The Current Eddystone Identifier (code 0x0B) can be used to report the currently advertised EID
 * and the current clock value when the Provider is provisioned for Eddystone, in order to sync the
 * Seeker in case of a clock drift (for example, due to drained battery).
 *
 * @param bd_addr
 * @param local_server_chann
 * @param p_data  4 bytes Clock value + 20/32 bytes adv ei
 * @param data_len GFPS_EDDYSTONE_SECP160_INFO_LEN=24 or GFPS_EDDYSTONE_SECP256_INFO_LEN=36
 * @return true
 * @return false
 */
bool bt_gfps_send_eddystone_info(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                 uint8_t local_server_chann, uint8_t *p_data,
                                 uint16_t data_len);

/**
 * @brief provider send the firmware revision to the Seeker
 * the Provider should send the firmware revision to the Seeker via message stream when connected
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                extend_app_cfg_const.gfps_version
 * @param data_len              extend_app_cfg_const.gfps_version data length
 * @return true    send success
 * @return false   send fail
 */
bool bt_gfps_send_firmware_revision(uint8_t conn_id, uint16_t cid,
                                    uint8_t *bd_addr, uint8_t local_server_chann,
                                    uint8_t *p_data, uint16_t data_len);

/**
 * @brief The session nonce should be generated and sent to the Seeker when RFCOMM connects
 *
 * @param[in] bd_addr            remote bd_addr
 * @param local_server_chann     gfps rfcomm channel   #define RFC_GFPS_CHANN_NUM              7
 * @param[in] p_data             session nonce
 * @param data_len               8 bytes session nonce length
 * @return true   send success
 * @return false  send fail
 */
bool bt_gfps_send_session_nonce(uint8_t conn_id, uint16_t cid,
                                uint8_t *bd_addr, uint8_t local_server_chann,
                                uint8_t *p_data, uint16_t data_len);

/**
 * @brief provider send dynamic buffer size to the Seeker
 * Upon receiving Request capability update message code, if the Provider has enabled dynamic buffer sizing, it should respond it.
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                dynamic buffer size
 * @param data_len              dynamic buffer size data length
 * @return true    send success
 * @return false   send fail
 */
bool bt_gfps_send_dynamic_buffer_size(uint8_t conn_id, uint16_t cid,
                                      uint8_t *bd_addr, uint8_t local_server_chann,
                                      uint8_t *p_data, uint16_t data_len);

/**
 * @brief the Provider can Notify the ANC state to let the Seeker know its ANC capability
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                uint8_t version_code + ANC state(uint16_t)
 * for example:uint16_t anc_state = pattern 1 ANC off = (GFPS_ANC_PATTERN1 | GFPS_ANC_PATTERN1_OFF)
 * @param data_len              ANC state data length
 * @return true    send success
 * @return false   send fail
 */
bool bt_gfps_send_anc_state(uint8_t conn_id, uint16_t cid,
                            uint8_t *bd_addr, uint8_t local_server_chann,
                            uint8_t *p_data, uint16_t data_len);

/**
 * @brief check if the connected Fast Pair Seeker supports SASS or not
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @return true    send success
 * @return false   send fail
 */
bool bt_gfps_get_sass_cap(uint8_t conn_id, uint16_t cid,
                          uint8_t *bd_addr, uint8_t local_server_chann);

/**
 * @brief Upon receiving get capability of SASS message code, the SASS Provider shall respond by bt_gfps_notify_sass_cap
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                addition data    uint16_t SASS version code and uint16_t flags.
 * @param data_len              addition data length    4 if this is sent by Provider
 * @return true    send success
 * @return false   send fail
* <b>Example usage</b>
 * \code{.c}
    bool gfps_sass_rfc_notify_sass_cap_test(uint8_t * bd_addr, uint8_t local_server_chann)
    {
        uint8_t p_data[4] = {0};
        uint8_t data_len = 4;
        uint8_t *p = p_data;
        uint16_t sass_version_code = 0x0100;
        uint16_t  sass_cap_flags = GFPS_SASS_STATE_ON | GFPS_SASS_MULTI_POINT_ENABLE | GFPS_SASS_ON_HEAD_SUPPORT;
        BE_UINT16_TO_STREAM(p,sass_version_code);
        BE_UINT16_TO_STREAM(p,sass_cap_flags);
        if(bt_gfps_notify_sass_cap(bd_addr, local_server_chann, p_data, data_len))
        {
            return true;
        };
        return false;
    }
 * \endcode
 */
bool bt_gfps_notify_sass_cap(uint8_t conn_id, uint16_t cid,
                             uint8_t *bd_addr, uint8_t local_server_chann,
                             uint8_t *p_data, uint16_t data_len);

/**
 * @brief Notify switching preference
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                addition data    uint8_t multipoint switching preference flag
 *                              and uint8_t Advanced switching settings(This byte is reserved, default value should be 0)
 * @param data_len              addition data length    2 if this is sent by Provider
 * @return true    send success
 * @return false   send fail
* <b>Example usage</b>
 * \code{.c}
    bool gfps_sass_rfc_notify_switch_prefer_test(uint8_t * bd_addr, uint8_t local_server_chann)
    {
        uint8_t p_data[2] = {0};
        uint8_t data_len = 2;
        uint8_t *p = p_data;
        uint8_t switch_prefer_flag = GFPS_SWITCH_PREFER_HFP_A2DP;
        uint8_t switch_setting = 0;
        BE_UINT8_TO_STREAM(p,switch_prefer_flag);
        BE_UINT8_TO_STREAM(p,switch_setting);
        if(bt_gfps_notify_switch_prefer(bd_addr, local_server_chann, p_data, data_len))
        {
            return true;
        };
        return false;
    }
 * \endcode
 */
bool bt_gfps_notify_switch_prefer(uint8_t conn_id, uint16_t cid,
                                  uint8_t *bd_addr, uint8_t local_server_chann,
                                  uint8_t *p_data, uint16_t data_len);

/**
 * @brief Notify multipoint switch event
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                addition data    uint8_t Switching reason + uint8_t Target device + utf8 Target device name
 * @param data_len              addition data length   varies
 * @return true    send success
 * @return false   send fail
 * <b>Example usage</b>
 * \code{.c}
    bool gfps_sass_rfc_notify_multipoint_switch_test(uint8_t * bd_addr, uint8_t local_server_chann)
    {
        uint8_t switch_reason = GFPS_MULTIPOINT_SWITCH_REASON_HFP;
        uint8_t target_device = GFPS_MULTIPOINT_SWITCH_THIS_DEVICE;
        uint8_t target_device_name[4] = "SASS";

        uint8_t p_data[6] = {0};
        uint8_t data_len = 6;
        uint8_t *p = p_data;

        BE_UINT8_TO_STREAM(p, switch_reason);
        BE_UINT8_TO_STREAM(p, target_device);
        memcpy(p, target_device_name, 4);

        if (bt_gfps_notify_multipoint_switch(bd_addr, local_server_chann, p_data, data_len))
        {
            return true;
        };
        return false;
    }
 * \endcode
 */
bool bt_gfps_notify_multipoint_switch(uint8_t conn_id, uint16_t cid,
                                      uint8_t *bd_addr, uint8_t local_server_chann,
                                      uint8_t *p_data, uint16_t data_len);

/**
 * @brief Notify connection status
 *
 * @param bd_addr               remote bd_addr
 * @param local_server_chann    gfps local server channel
 * @param p_data                addition data    uint8_t Active device flag + Encrypted connection status + Message nonce
 * @param data_len              addition data length   varies
 * @return true    send success
 * @return false   send fail
 * <b>Example usage</b>
 * \code{.c}
    bool gfps_sass_rfc_notify_connection_status_test(uint8_t * bd_addr, uint8_t local_server_chann)
    {
        uint8_t flag = GFPS_ACTIVE_DEV;
        T_SASS_CONN_STATUS_FIELD conn_status;
        gfps_sass_get_conn_status(&conn_status);
        uint8_t ses_nonce[8] = {1,2,3,4,5,6,7,8};
        uint8_t msg_nonce[8] = {0};
        uint8_t account_key_index = 0;
        //0x35853809
        conn_status.length_type                        = 0x35;
        conn_status.conn_status_info.on_head_detection = 1;
        conn_status.conn_status_info.conn_availability = 0;
        conn_status.conn_status_info.focus_mode        = 0;
        conn_status.conn_status_info.auto_reconn       = 0;
        conn_status.conn_status_info.conn_state        = 5;
        conn_status.custom_data                        = 0x38;
        conn_status.conn_dev_bitmap                    = 0x09;

        if (bt_gfps_notify_connection_status(bd_addr,local_server_chann,account_key_index,flag,conn_status, msg_nonce, ses_nonce))
        {
            return true;
        };
        return false;
        }
    }
 * \endcode
 */
bool bt_gfps_notify_connection_status(uint8_t conn_id, uint16_t cid,
                                      uint8_t *bd_addr, uint8_t local_server_chann,
                                      uint8_t account_key_index, uint8_t flag,
                                      T_SASS_CONN_STATUS_FIELD conn_status,
                                      uint8_t *msg_nonce, uint8_t *ses_nonce);
/**
 * @brief Generate Message Authentication Code
 *
 * @param[in] account key      Account key for computing MAC
 * @param[in] p_session_nonce  The session nonce should be generated and sent to the Seeker when RFCOMM connects
 * @param[in] p_message_nonce  To send a message when a MAC is required, the Seeker will send a message nonce and the MAC together with the message.
 * @param[in] p_data           p_data is the additional data (excluding message nonce and MAC) of the Message stream
 * @param[in] data_len         the additional data (excluding message nonce and MAC) length
 * @param[out] p_mac           8 bytes message authentication code.
 * @return true   The MAC generate success.
 * @return false  The MAC generate fail.
 */
bool bt_gfps_generate_mac(uint8_t *account_key, uint8_t *p_session_nonce, uint8_t *p_message_nonce,
                          uint8_t *p_data, uint16_t data_len, uint8_t *p_mac);

/**
  * @brief  Send an ack message to indicate remote device the action be performed.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  p_data                 message group and code
  * @param  data_len               data length
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_send_ack(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                      uint8_t local_server_chann, uint8_t *p_data,
                      uint16_t data_len);

/**
  * @brief  Send an nak message to indicate remote device the action not be performed.
  * @param  bd_addr                remote bd_addr
  * @param  local_server_chann     local server channel
  * @param  p_data                 message group and message code
  * @param  data_len               data length
  * @param  reason                 nak reason
  * @return The result of sending request.
  * @retval true    The request has benn sent successfully.
  * @retval false   The request was fail to send.
  */
bool bt_gfps_send_nak(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                      uint8_t local_server_chann, uint8_t *p_data,
                      uint16_t data_len, uint8_t reason);

/**
 * End of BT_GFPS
 * @}
 */

/** @} End of GFPS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_GFPS_H_ */

