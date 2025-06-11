/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_ACCOUNT_KEY_H_
#define _APP_GFPS_ACCOUNT_KEY_H_

#include <stdbool.h>
#include <stdint.h>
#include "gfps.h"
#include "app_flags.h"
#include "app_gfps_personalized_name.h"
#if GFPS_FEATURE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */
#define GFPS_ACCOUNT_KEY_MAX           10
#define ACCOUNT_KEY_FLASH_OFFSET       0xAF0  //account key store offset

//extern uint8_t gfps_account_key_table_size;
//extern T_ACCOUNT_KEY *account_key;

bool app_gfps_account_key_init(uint8_t key_num);
void app_gfps_account_key_clear(void);
bool app_gfps_account_key_store(uint8_t key[16], uint8_t *bd_addr);

bool app_gfps_account_key_remote_add(uint8_t key[16], uint8_t *bd_addr);
bool app_gfps_account_key_remote_sync(void);
//bool app_gfps_remote_owner_address_sync(void);
void app_gfps_account_key_remote_handle_sync(uint8_t *info_data, uint8_t info_len);

/**
 * @brief print all account key info
 */
void app_gfps_account_key_table_print(void);
bool app_gfps_account_key_verify_mac(uint8_t conn_id, uint8_t *bd_addr, uint8_t *msg,
                                     uint16_t data_len,
                                     uint8_t *msg_nonce, uint8_t *mac);
bool app_gfps_account_key_find_inuse_account_key(uint8_t *inuse_accout_idx, uint8_t conn_id,
                                                 uint8_t *bd_addr,
                                                 uint8_t *msg, uint16_t data_len, uint8_t *msg_nonce, uint8_t *mac);
void app_gfps_account_key_remote_handle_add(uint8_t key[16], uint8_t *bd_addr);

uint8_t app_gfps_account_key_get_table_size(void);
T_ACCOUNT_KEY *app_gfps_account_key_get_table(void);

void app_gfps_account_key_save_ownerkey_valid(void);
bool app_gfps_account_key_find_account_key_by_index(uint8_t index, uint8_t *p_account_key);

/** End of APP_RWS_GFPS
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
