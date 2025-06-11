/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BT_POINT_H_
#define _APP_BT_POINT_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_link_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    BP_LINK_TYPE_B2B_BR      = 0x00,
    BP_LINK_TYPE_B2S_BR      = 0x01,
    BP_LINK_TYPE_B2S_SCO     = 0x02,
    BP_LINK_TYPE_B2S_LE      = 0x03,
    BP_LINK_TYPE_B2S_LEA     = 0x04,
    BP_LINK_TYPE_B2S_UCA     = 0x05,
    BP_LINK_TYPE_B2S_BCA     = 0x06,
} T_BP_LINK_TYPE;

typedef enum
{
    BP_LINK_EVENT_CONNECT    = 0x00,
    BP_LINK_EVENT_DISCONNECT = 0x01,
} T_BP_LINK_EVENT;

typedef enum
{
    BP_LINK_LAST_BTMODE_NONE   = 0x00,
    BP_LINK_LAST_BTMODE_LEA    = 0x01,
    BP_LINK_LAST_BTMODE_LEGACY = 0x02,
} T_BP_LINK_LAST_BTMODE;

typedef void *T_BT_POINT_HANDLE;

void app_bt_point_init(void);

bool app_bt_point_is_empty(void);
bool app_bt_point_is_full(void);
uint8_t app_bt_point_num_get(void);

bool app_bt_point_br_link_is_empty(void);
bool app_bt_point_br_link_is_full(void);
uint8_t app_bt_point_br_link_num_get(void);
void app_bt_point_br_link_num_set(uint8_t num);
uint8_t app_bt_point_br_link_sco_num_get(void);

bool app_bt_point_le_link_is_empty(void);
bool app_bt_point_le_link_is_full(void);
uint8_t app_bt_point_le_link_num_get(void);

bool app_bt_point_lea_link_is_empty(void);
bool app_bt_point_lea_link_is_full(void);
uint8_t app_bt_point_lea_link_num_get(void);

T_BT_POINT_HANDLE app_bt_point_search(uint8_t *bd_addr);
T_APP_BR_LINK *app_bt_point_br_link_get(T_BT_POINT_HANDLE handle);
T_APP_LE_LINK *app_bt_point_le_link_get(T_BT_POINT_HANDLE handle);

bool app_bt_point_link_permission_check(T_BP_LINK_TYPE link_type, uint8_t *bd_addr);
void app_bt_point_link_num_changed(T_BP_LINK_TYPE link_type, T_BP_LINK_EVENT link_event,
                                   uint8_t *bd_addr);

void app_bt_point_link_tone_set(uint8_t *bd_addr, bool tone);
bool app_bt_point_link_tone_get(uint8_t *bd_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_MAIN_H_ */
