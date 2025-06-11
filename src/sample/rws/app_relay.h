/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_RELAY_H_
#define _APP_RELAY_H_

#include <stdbool.h>
#include "remote.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RELAY App Relay
  * @brief App Relay
  * @{
  */

/**  @brief app remote msg*/
typedef enum
{
    APP_REMOTE_MSG_ROLESWAP_STATE_SYNC          = 0x01,

    APP_REMOTE_MSG_COMMON_RELAY                 = 0xF3,

} T_APP_REMOTE_MSG;

#define APP_RELAY_MTU           500
#define APP_RELAY_HEADER_LEN    4

typedef void *T_APP_RELAY_MSG_LIST_HANDLE;

typedef enum
{
    APP_MODULE_TYPE_NONE              = 0x00,
    APP_MODULE_TYPE_AUDIO_POLICY      = 0x01,
    APP_MODULE_TYPE_ROLESWAP          = 0x02,
    APP_MODULE_TYPE_BT_POLICY         = 0x03,
    APP_MODULE_TYPE_BLE_BOND          = 0x04,
    APP_MODULE_TYPE_BLE_COMMON_ADV    = 0x05,
    APP_MODULE_TYPE_KEY_PROCESS       = 0x06,
    APP_MODULE_TYPE_DUAL_AUDIO_EFFECT = 0x07,
    APP_MODULE_TYPE_MMI               = 0x08,
    APP_MODULE_TYPE_CMD               = 0x09,
    APP_MODULE_TYPE_ANC               = 0x0A,
    APP_MODULE_TYPE_QOL               = 0x0B,
    APP_MODULE_TYPE_DEVICE            = 0x0C,
    APP_MODULE_TYPE_MULTI_LINK        = 0x0D,
    APP_MODULE_TYPE_APT               = 0x0E,
    APP_MODULE_TYPE_OTA               = 0x0F,
    APP_MODULE_TYPE_LISTENING_MODE    = 0x10,
    APP_MODULE_TYPE_BRIGHTNESS_MODE   = 0x11,
    APP_MODULE_TYPE_AVP               = 0x12,
    APP_MODULE_TYPE_PLAYBACK          = 0x13,
    APP_MODULE_TYPE_PLAYBACK_TRANS    = 0x14,
    APP_MODULE_TYPE_LISTEN_SAVE       = 0x15,
    APP_MODULE_TYPE_LED               = 0x16,
    APP_MODULE_TYPE_CHARGER           = 0x17,
    APP_MODULE_TYPE_BUD_LOC           = 0x18,
    APP_MODULE_TYPE_HFP               = 0x19,
    APP_MODULE_TYPE_RTK_FAST_PAIR     = 0x1A,
    APP_MODULE_TYPE_RTK_IAP           = 0x1B,
    APP_MODULE_TYPE_APP_IAP           = 0x1C,
    APP_MODULE_TYPE_LEA               = 0x1D,
    APP_MODULE_TYPE_BLE_RAND_ADDR     = 0x1E,
    APP_MODULE_TYPE_ROLESWAP_CTRL     = 0x1F,
    APP_MODULE_TYPE_LOC               = 0x20,
    APP_MODULE_TYPE_REALCAST          = 0x21,
    APP_MODULE_TYPE_UCA               = 0x23,
    APP_MODULE_TYPE_LEA_VOL_POLICY    = 0x24,

    APP_MODULE_TYPE_XIAOAI_DEVICE     = 0x80,
    APP_MODULE_TYPE_XIAOAI_TRANSPORT  = 0x81,
    APP_MODULE_TYPE_XIAOAI_OTA        = 0x82,
    APP_MODULE_TYPE_GFPS              = 0x83,
    APP_MODULE_TYPE_GFPS_MSG          = 0x84,
    APP_MODULE_TYPE_XIAOWEI_FVALUE    = 0x85,
    APP_MODULE_TYPE_XIAOWEI_DEVICE    = 0x86,
    APP_MODULE_TYPE_XIAOWEI_TRANSPORT = 0x87,
    APP_MODULE_TYPE_AMA_ROLESWAP      = 0x8B,
    APP_MODULE_TYPE_AMA_SERVICE       = 0x8C,
    APP_MODULE_TYPE_AMA_DEV           = 0x8D,
    APP_MODULE_TYPE_AMA_TRANSPORT     = 0x8E,
    APP_MODULE_TYPE_AMA_BLE           = 0x8F,
    APP_MODULE_TYPE_TEAMS_LIB         = 0x90,
    APP_MODULE_TYPE_CFU               = 0x91,
    APP_MODULE_TYPE_TEAMS_APP         = 0x92,
    APP_MODULE_TYPE_HA                = 0x93,
    APP_MODULE_TYPE_TUYA              = 0x94,
    APP_MODULE_TYPE_TUYA_OTA          = 0X95,
    APP_MODULE_TYPE_FINDMY            = 0X96,
    APP_MODULE_TYPE_DONGLE_DUAL_MODE  = 0x97,
    APP_MODULE_TYPE_GFPS_SASS         = 0x98,
} T_APP_MODULE_TYPE;

typedef uint16_t (*P_APP_RELAY_CBACK)(uint8_t *buf, uint8_t msg_type, bool total);
typedef void (*P_APP_PARSE_CBACK)(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                  T_REMOTE_RELAY_STATUS status);
/* @brief  pack msg  to relay
*
* @param  buf   used to get relay info
* @param  msg_type   used to parse info received
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  payload_len    payload length
* @param  msg_ptr        payload pointer
* @param  skip           flag for total relay
* @param  total          flag for total relay
* @return
*/
uint16_t app_relay_msg_pack(uint8_t *buf, uint8_t msg_type, T_APP_MODULE_TYPE module_type,
                            uint16_t payload_len, uint8_t *msg_ptr, bool skip, bool total);

/* @brief  register cback to app_relay modoule
*
* @param  relay_cb   used to get relay info
* @param  parse_cb   used to parse info received
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_max   msg's max number of the modoule
* @return
*/
bool app_relay_cback_register(P_APP_RELAY_CBACK relay_cb, P_APP_PARSE_CBACK parse_cb,
                              T_APP_MODULE_TYPE module_type, uint8_t msg_max);

/* @brief  async relay one msg for local variable
*
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_type   the msg need to be relayed
* @param  payload_buf   msg payload
* @param  payload_len   msg payload len
* @return bool
* @note when you can't get payload from relay cback,choose this api, otherwise use app_relay_async_single
*/
bool app_relay_async_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                         uint8_t *payload_buf, uint16_t payload_len);

/* @brief  async relay one msg
*
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_type   the msg need to be relayed
* @return bool
*/
bool app_relay_async_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type);

/* @brief  sync relay one msg for local variable
*
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_type   the msg need to be relayed
* @param  payload_buf   msg payload
* @param  payload_len   msg payload len
* @param timer_type    The synchronous timer type
* @param timer_period  The synchronous timer period in milliseconds.
* @param loopback      Set the synchronous message loopback or not.
* @return bool
* @note  when you can't get payload from relay cback,choose this api, otherwise use app_relay_sync_single
*/
bool app_relay_sync_single_with_raw_msg(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                                        uint8_t *payload_buf, uint16_t payload_len,
                                        T_REMOTE_TIMER_TYPE timer_type,
                                        uint32_t timer_period, bool loopback);
/* @brief  sync relay one msg
*
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_type   the msg need to be relayed
* @param timer_type    The synchronous timer type
* @param timer_period  The synchronous timer period in milliseconds.
* @param loopback      Set the synchronous message loopback or not.
* @return bool
*/
bool app_relay_sync_single(T_APP_MODULE_TYPE module_type, uint8_t msg_type,
                           T_REMOTE_TIMER_TYPE timer_type,
                           uint32_t timer_period, bool loopback);

/* @brief  async relay more msgs and build the list
*
* @return T_APP_RELAY_MSG_LIST_HANDLE
*/
T_APP_RELAY_MSG_LIST_HANDLE app_relay_async_build(void);

/* @brief  async relay more msgs and add msg to the list
*
* @param  msg_list_handle  msg list handle
* @param  module_type  ref to T_APP_MODULE_TYPE
* @param  msg_type   the msg need to be relayed
* @return none
*/
void app_relay_async_add(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle, T_APP_MODULE_TYPE module_type,
                         uint8_t msg_type);

/* @brief  async relay more msgs and send them
*
* @param  msg_list_handle  msg list handle
* @return none
*/
void app_relay_async_send(T_APP_RELAY_MSG_LIST_HANDLE msg_list_handle);

/* @brief  relay total msgs when b2b connected or roleswap(special usage, don't use it at will )
*
* @return bool
*/
bool app_relay_total_async(void);

/* @brief  relay module init
*
* @param  void
* @return none
*/
bool app_relay_init(void);

/** End of APP_RELAY
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_RELAY_H_ */
