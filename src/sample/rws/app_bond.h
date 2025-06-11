/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BOND_H_
#define _APP_BOND_H_

#include <stdint.h>
#include <stdbool.h>
#include "bt_bond.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BOND App Bond
  * @brief
  * @{
  */

/** bitmask of bond flag */
#define BOND_FLAG_HFP                   0x00000001
#define BOND_FLAG_A2DP                  0x00000002
#define BOND_FLAG_SPP                   0x00000004
#define BOND_FLAG_PBAP                  0x00000008
#define BOND_FLAG_IAP                   0x00000010
#define BOND_FLAG_HSP                   0x00000020
#define BOND_FLAG_AMA                   0x00000040
#define BOND_FLAG_HID                   0x00000080
#define BOND_FLAG_DONGLE                0x00000100 /* keeping dongle link record */
#define BOND_FLAG_COMPUTER              0x00001000 /* device computer*/
#define BOND_FLAG_UCA                   0x01000000

/** bitmask of le bond flag */
#define BOND_FLAG_LEA                   0x0001

typedef struct
{
    uint8_t     bd_addr[6];
    uint8_t     priority;
    uint8_t     key_type;
    uint8_t     link_key[16];
    uint32_t    bond_flag;
} T_APP_LINK_RECORD;

/**  @brief  App define message payload of APP_REMOTE_MSG_LINK_RECORD_ADD, \ref T_APP_REMOTE_MSG. */
typedef struct
{
    uint8_t bd_addr[6];
    uint8_t key_type;
    uint8_t link_key[16];
    uint8_t reset_volume;
} T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED;

/**  @brief  App define message payload of APP_REMOTE_MSG_PROFILE_CONNECTED, \ref T_APP_REMOTE_MSG. */
typedef struct
{
    uint8_t  bd_addr[6];
    uint32_t prof_mask;
} T_APP_REMOTE_MSG_PAYLOAD_PROFILE_CONNECTED;

/**
    * @brief  Send local bud2phone link record to remote.
    * @param  void
    * @return true/false
    */
bool app_bond_sync_b2s_link_record(void);

/**
    * @brief  Handle link record message received from remote.
    * @param  msg   link record msg
    * @param  buf   msg buffer
    * @return void
    */
void app_bond_handle_remote_link_record_msg(uint16_t msg, void *buf);

/**
    * @brief  Handle profile connected message received from remote.
    * @param  buf   msg buffer
    * @return void
    */
void app_bond_handle_remote_profile_connected_msg(void *buf);

/**
    * @brief  app_bond_init.
    * @param  void
    * @return void
    */
void app_bond_init(void);

/**
    * @brief  app_bond_push_sec_diff_link_record.
    * @param  link_record
    * @return void
    */
void app_bond_push_sec_diff_link_record(T_APP_LINK_RECORD *link_record);

/**
    * @brief  app_bond_pop_sec_diff_link_record.
    * @param  bd_addr, link_record
    * @return true/false
    */
bool app_bond_pop_sec_diff_link_record(uint8_t *bd_addr, T_APP_LINK_RECORD *link_record);

/**
    * @brief  app_bond_clear_sec_diff_link_record.
    * @param  void
    * @return void
    */
void app_bond_clear_sec_diff_link_record(void);

/**
    * @brief  Set bud2phone link record to highest priority.
    * @param  bd_addr
    * @return void
    */
void app_bond_set_priority(uint8_t *bd_addr);

/**
    * @brief  app_bond_clear_non_rws_keys.
    * @param  void
    * @return void
    */
void app_bond_clear_non_rws_keys(void);

/**
    * @brief  app_bond_get_b2s_link_record.
    * @param  link_record, link_num
    * @return link num
    */
uint8_t app_bond_get_b2s_link_record(T_APP_LINK_RECORD *link_record, uint8_t link_num);

/**
    * @brief  check if phone's link key exist.
    * @param  void
    * @return true: exist
    */
bool app_bond_is_b2s_link_record_exist(void);


bool app_bond_get_pair_idx_mapping(uint8_t *bd_addr, uint8_t *index);

/**
    * @brief   Get bond device num from bond storage.
    * @param   void
    * @return  The bond device num.
    */
uint8_t app_bond_b2s_num_get(void);

/**
    * @brief   Get B2S Bluetooth device address from bond storage by priority.
    * @param   priority      The priority of bonded device.
    * @param   bd_addr       The Bluetooth device address got by priority.
    * @return  The status of getting Bluetooth device address.
    */
bool app_bond_b2s_addr_get(uint8_t priority, uint8_t *bd_addr);

/**
    * @brief   Save link key.
    * @param   bd_addr       The Bluetooth device address.
    * @param   linkkey       The link key to be saved.
    * @param   key_type      The type of link key.
    * @return  The status of saving link key.
    */
bool app_bond_key_set(uint8_t *bd_addr, uint8_t *linkkey, uint8_t key_type);

/**
    * @brief   adjust b2b bond priority to highest.
    * @param   void
    * @return  void
    */
void app_bond_adjust_b2b_priority(void);

#if F_APP_COMMON_DONGLE_SUPPORT
/**
    * @brief   adjust gaming/conference dongle bond priority to highest.
    * @param   void
    * @return  void
    */
void app_bond_adjust_dongle_priority(void);

/**
    * @brief   add dongle bond flag to specific address.
    * @param   bd_addr       The Bluetooth device address.
    * @return  void
    */
void app_bond_add_dongle_bond_info(uint8_t *bd_addr);
#endif

#if F_APP_BOND_MGR_SUPPORT
/**
    * @brief  Setting le bond flag.
    * @param  p_link_info   le link
    * @return void
    */
void app_bond_le_set_bond_flag(void *p_link_info, uint16_t bond_flag);

/**
    * @brief   Clear non-lea link keys
    * @param   void
    * @return  void
    */
void app_bond_le_clear_non_lea_keys(void);
#endif

/** End of APP_BOND
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BOND_H_ */
