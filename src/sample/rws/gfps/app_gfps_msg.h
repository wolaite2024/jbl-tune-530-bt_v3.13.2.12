#ifndef _APP_GFPS_MSG_H_
#define _APP_GFPS_MSG_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GFPS_RING_PERIOD_VALUE 2
#define GFPS_STANARD_ACK_PAYLOAD_LEN    2 //MSG gourp + MSG id

#define GFPS_GET_SASS_CAP_DATA_LEN                  0
#define GFPS_NOTIFY_SASS_CAP_PARA_LEN               (4)
#define GFPS_NOTIFY_SASS_CAP_DATA_LEN               (GFPS_NOTIFY_SASS_CAP_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SET_MULTI_POINT_STATE_PARA_LEN         (1)
#define GFPS_SET_MULTI_POINT_STATE_DATA_LEN         (GFPS_SET_MULTI_POINT_STATE_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SET_SWITCH_PREFER_PARA_LEN             (2)
#define GFPS_SET_SWITCH_PREFER_DATA_LEN             (GFPS_SET_SWITCH_PREFER_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_GET_SWITCH_PREFER_PARA_LEN             0
#define GFPS_GET_SWITCH_PREFER_DATA_LEN             0
#define GFPS_NOTIFY_SWITCH_PREFER_PARA_LEN          (2)
#define GFPS_NOTIFY_SWITCH_PREFER_DATA_LEN          (GFPS_NOTIFY_SWITCH_PREFER_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_PARA_LEN    (1)
#define GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_DATA_LEN    (GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SWITCH_BACK_PARA_LEN                   (1)
#define GFPS_SWITCH_BACK_DATA_LEN                   (GFPS_SWITCH_BACK_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_GET_CONN_STATUS_DATA_LEN               0
#define GFPS_GET_CONN_STATUS_DATA_LEN               0
#define GFPS_NOTIFY_SASS_INITIAL_CONN_PARA_LEN      (1)
#define GFPS_NOTIFY_SASS_INITIAL_CONN_DATA_LEN      (GFPS_NOTIFY_SASS_INITIAL_CONN_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_IND_INUSE_ACCOUNTKEY_PARA_LEN          (6)
#define GFPS_IND_INUSE_ACCOUNTKEY_DATA_LEN          (GFPS_IND_INUSE_ACCOUNTKEY_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SEND_CUSTOM_DATA_PARA_LEN              (1)
#define GFPS_SEND_CUSTOM_DATA_DATA_LEN              (GFPS_SEND_CUSTOM_DATA_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)
#define GFPS_SET_DROP_CONN_TARGET_PARA_LEN          (1)
#define GFPS_SET_DROP_CONN_TARGET_DATA_LEN          (GFPS_SET_DROP_CONN_TARGET_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)

#define GFPS_GET_ANC_STATE_PARA_LEN                 0
#define GFPS_GET_ANC_STATE_DATA_LEN                 0
#define GFPS_SET_ANC_STATE_PARA_LEN                 (3)
#define GFPS_SET_ANC_STATE_DATA_LEN                 (GFPS_SET_ANC_STATE_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)

#define GFPS_REQ_CAPABILITY_UPDATE_PARA_LEN         0
#define GFPS_REQ_CAPABILITY_UPDATE_DATA_LEN         0

#define GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN         (3)
#define GFPS_DYNAMIC_BUFFER_SIZING_DATA_LEN         (GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN+GFPS_MESSAGE_NONCE_LEN+GFPS_MAC_LEN)

#define GFPS_BUFFER_SIZE_INFO_LEN_PER_CODEC         (7)


#define GFPS_MAC_ERROR_NAK_LEN  2
typedef enum
{
    BUD_SIDE_LEFT      = 0,
    BUD_SIDE_RIGHT     = 1,
} T_GFPS_MSG_BUD_SIDE;

typedef enum
{
    GFPS_REMOTE_MSG_FINDME_START                = 0x00,
    GFPS_REMOTE_MSG_FINDME_STOP                 = 0x01,
    GFPS_REMOTE_MSG_FINDME_SYNC                 = 0x02,
    GFPS_RFC_REMOTE_MSG_SYNC_CUSTOM_DATA        = 0x03,
    GFPS_RFC_REMOTE_MSG_SYNC_TARGET_DROP_DEVICE = 0x04,
    GFPS_REMOTE_MSG_MAX_MSG_NUM,
} T_GFPS_REMOTE_MSG;

typedef enum
{
    GFPS_MSG_RING_STOP    = 0x00,
    GFPS_MSG_RIGHT_RING   = 0x01,
    GFPS_MSG_LEFT_RING    = 0x02,
    GFPS_MSG_ALL_RING     = 0x03,
} T_GFPS_MSG_RING_STATE;

/**
 * @brief app_gfps_msg_reverse_data
 *
 * swap big endian mode or little endian mode
 * @param data data received from remote
 * @param len  data length
 */
void app_gfps_msg_reverse_data(uint8_t *data, uint16_t len);

/**
 * @brief gfps find me ring tone stop
 *
 */
void app_gfps_msg_ring_stop(void);

/**
 * @brief set current ring state
 *
 * @param ring_state @ref T_GFPS_MSG_RING_STATE
 */
void app_gfps_msg_set_ring_state(T_GFPS_MSG_RING_STATE ring_state);

/**
 * @brief gfps RFCOMM init
 *
 */
void app_gfps_msg_rfc_init(void);

/**
 * @brief Used to handle GFPS message stream
 *
 * @param bd_addr       remote bd address
 * @param server_chann  RFCOMM server channel
 * @param p_cmd         data received from remote
 * @param cmd_len       data length
 */
void app_gfps_msg_handle_data_transfer(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                       uint8_t server_chann,
                                       uint8_t *p_cmd, uint16_t cmd_len);

/**
 * @brief check data complete for gfps RFCOMM message stream
 *
 * @param bd_addr remote bd address
 * @param data    data received from remote
 * @param len     data length
 */
void app_gfps_msg_loop_check_data_complete(uint8_t *bd_addr, uint8_t *data, uint16_t len,
                                           uint8_t local_server_chann);

/**
 * @brief check data complete for gfps LE message stream
 *
 * @param conn_id  BLE link connection id
 * @param cid      LE L2cap channel id
 * @param data     data received from remote
 * @param len      data length
 */
void app_gfps_msg_le_loop_check_data_complete(uint8_t conn_id, uint16_t cid, uint8_t *data,
                                              uint16_t len);

/**
 * @brief update ble address through RFCOMM or LE L2cap channel
 *
 */
void app_gfps_msg_update_ble_addr(void);

/**
 * @brief app_gfps_msg_handle_ring_event
 *
 * @param event indicate expected ring event
 */
void app_gfps_msg_handle_ring_event(uint8_t event);

/**
 * @brief set ring timeout value
 *
 * @param ring_timeout ring timeout value
 */
void app_gfps_msg_set_ring_timeout(uint8_t ring_timeout);

/**
 * @brief get ring timeout value
 *
 * @return uint8_t ring timeout value
 */
uint8_t app_gfps_msg_get_ring_timeout(void);

/**
 * @brief get ring state
 *
 * @return T_GFPS_MSG_RING_STATE ring state
 */
T_GFPS_MSG_RING_STATE app_gfps_msg_get_ring_state(void);

/**
 * @brief app_gfps_msg_notify_multipoint_switch
 *
 * @param active_index  current active index
 * @param link_type  current link type
 * @param switch_reason  switch reason
 */
void  app_gfps_msg_notify_multipoint_switch(uint8_t active_index, uint8_t link_type,
                                            uint8_t switch_reason);

/**
 * @brief update custom data
 *
 * @param custom_data custom data
 */
void app_gfps_msg_update_custom_data(uint8_t custom_data);

/**
 * @brief update target drop device
 *
 * @param bd_addr bd address
 */
void app_gfps_msg_update_target_drop_device(uint8_t conn_id, uint8_t *bd_addr);

/**
 * @brief report session nonce
 *
 * @param bd_addr  remote bd address
 * @param local_server_chann server channel
 * @return true  success
 * @return false fail
 */

bool app_gfps_msg_report_session_nonce(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                       uint8_t local_server_chann);

/**
 * @brief notify connection status
 *
 * @return true  success
 * @return false fail
 */
bool app_gfps_msg_notify_connection_status(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_GFPS_PSM_H_ */
