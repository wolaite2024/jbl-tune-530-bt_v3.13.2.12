/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_SASS_POLICY_H_
#define _APP_SASS_POLICY_H_

#include <stdint.h>

#if GFPS_SASS_SUPPORT
#include "gfps_sass_conn_status.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SASS_PAGESCAN_WINDOW        0x12
#define SASS_PAGESCAN_INTERVAL      0x400

#define SWITCH_RET_OK               0x00
#define SWITCH_RET_FAILED           0x01
#define SWITCH_RET_REDUNDANT        0x02

/**
 * @brief Multipoint switching preference flag
 * Bit 0 (MSB): A2DP vs A2DP (default 0)
 * Bit 1: HFP vs HFP (default 0)
 * Bit 2: A2DP vs HFP (default 0)
 * Bit 3: HFP vs A2DP (default 1)
*/
// #define SASS_A2DP_A2DP 0x0001
// #define SASS_SCO_SCO   0x0002
// #define SASS_A2DP_SCO  0x0004
// #define SASS_SCO_A2DP  0x0008
// #define SASS_A2DP_VA   0x0010
// #define SASS_SCO_VA    0x0020
// #define SASS_VA_A2DP   0x0040
// #define SASS_VA_SCO    0x0080
// #define SASS_VA_VA     0x0100

#define SASS_A2DP_A2DP 0x0080
#define SASS_SCO_SCO   0x0040
#define SASS_A2DP_SCO  0x0020
#define SASS_SCO_A2DP  0x0010
#define SASS_A2DP_VA   0x0008
#define SASS_SCO_VA    0x0004
#define SASS_VA_A2DP   0x0002
#define SASS_VA_SCO    0x0001
#define SASS_VA_VA     0x0100

typedef enum
{
    SASS_PREEMPTIVE_FEATURE_BIT_SET,
    SASS_PREEMPTIVE_FEATURE_BIT_GET,
    SASS_LINK_SWITCH,
    SASS_SWITCH_BACK,
    SASS_MULTILINK_ON_OFF,
} T_SASS_CMD;

typedef enum
{
    SASS_TYPE_UNKNOW,
    SASS_EDR_LINK,
    SASS_LE_LINK,
} T_SASS_LINK_TYPE;


typedef enum
{
    APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC,
    APP_REMOTE_MSG_SASS_SWITCH_SYNC,
    APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC,
    APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC,
    APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC,
    APP_REMOTE_MSG_SASS_DISC_LE_LINK_SYNC,

    APP_REMOTE_MSG_SASS_TOTAL
} T_SASS_REMOTE_MSG;

typedef enum
{
    SASS_ORIGINAL_SAME     = 0x0000,
    SASS_ORIGINAL_DIFF     = 0x0100,
} T_SASS_SPLIT;



typedef enum
{
    SASS_PREEM_REJECT,
    SASS_PREEM_NONE,//single link OR already active link
    SASS_PREEM_EDR,
    SASS_PREEM_LEA,
    SASS_PREEM_WAITING,
} T_SASS_PREEM;

typedef enum
{
    SASS_LE_DISCONNECTING,
    SASS_LE_DISCONNECTED,
    SASS_LE_CONNECTED,
    SASS_LE_AUTHEN_COMPL,
    SASS_LE_DSICOV_DONE,
} T_SASS_LE_EVENT;

typedef struct
{
    uint8_t bd_addr[6];
    uint16_t disc_cause;
} T_SASS_DISCONNECT_PARAM;

typedef enum
{
    MULTILINK_SASS_A2DP_PREEM,
    MULTILINK_SASS_HFP_PREEM,
    MULTILINK_SASS_AVRCP_PREEM,
    MULTILINK_SASS_FORCE_PREEM,
    MULTILINK_SASS_LEA_MEDIA_PREEM,
    MULTILINK_SASS_LEA_CALL_PREEM,
    MULTILINK_SASS_LEA_MCP_PREEM,
} T_MULTILINK_SASS_ACTON;

typedef enum
{
    MULTILINK_SASS_NO_PREEM,
    MULTILINK_SASS_PREEM_EDR,
    MULTILINK_SASS_PREEM_LEA,
} T_MULTILINK_SASS_PREEM;

void app_sass_policy_set_capability(uint8_t conn_id, uint8_t *addr);
void app_sass_policy_set_multipoint_state(uint8_t enable);
void app_sass_policy_set_switch_preference(uint8_t flag);
uint8_t app_sass_policy_get_switch_preference(void);
void app_sass_policy_set_advanced_switching_setting(uint8_t flag);
uint8_t app_sass_policy_get_advanced_switching_setting(void);
uint8_t app_sass_policy_switch_active_audio_source(uint8_t conn_id, uint8_t *bd_addr,
                                                   uint8_t switch_flag, bool self);
void app_sass_policy_switch_back(uint8_t event);
void app_sass_policy_initiated_connection(uint8_t conn_id, uint8_t *addr,
                                          bool triggered_by_audio_switch);
void app_sass_policy_init(void);
void app_sass_policy_reset(void);
void app_sass_policy_sync_conn_bit_map(uint8_t bitmap);
uint8_t app_sass_policy_get_conn_bit_map(void);
uint8_t app_sass_policy_get_disc_link(uint8_t *link_type);
void app_sass_policy_link_back_end(void);
void app_sass_policy_profile_conn_handle(uint8_t id, T_SASS_LINK_TYPE link_type);
bool app_sass_policy_is_sass_device(uint8_t *addr);
bool app_sass_policy_is_target_drop_device(uint8_t idx, T_SASS_LINK_TYPE link_type);
#if GFPS_SASS_SUPPORT
T_SASS_CONN_STATE app_sass_policy_get_connection_state(void);
#endif
bool app_sass_policy_get_original_enable_multi_link(void);
uint8_t app_sass_get_available_connection_num(void);
void *app_sass_find_other_link(uint8_t active_id, T_SASS_LINK_TYPE input_link_type,
                               T_SASS_LINK_TYPE *found_link_type);
uint8_t app_sass_find_other_idx(uint8_t app_idx);
uint16_t app_sass_preempt_policy(uint8_t app_idx, T_SASS_LINK_TYPE link_type, uint8_t action,
                                 bool a2dp_check);
bool app_sass_pause_inactive_link(void);
void app_sass_handle_le_dm(uint8_t conn_id, T_SASS_LE_EVENT event, uint8_t *p_data);
uint8_t app_multi_preemptive_judge(uint8_t app_idx, uint8_t link_type, uint8_t type);
uint8_t app_multi_get_inactive_index(uint8_t new_link_idx, uint8_t link_type, uint8_t call_num,
                                     bool force, uint8_t *inactive_link_type);
T_SASS_CONN_STATE app_sass_policy_context_to_conn_state(T_APP_LE_LINK *p_link);
#ifdef __cplusplus
}
#endif

#endif

