/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BT_SNIFFING_H_
#define _APP_BT_SNIFFING_H_

#include <stdint.h>
#include <stdbool.h>
#include "btm.h"
#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_BT_SNIFFING App BT Sniffing Module
  * @brief App bt sniffing
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BT_SNIFFING_Exported_Macros App BT Sniffing Exported Macros
    * @{
    */

/* flush timeout is set according to a2dp interval and codec type
   refer to https://wiki.realtek.com/display/Bee1/BT+Audio+Legacy+Rx+Packet+Count*/
#define A2DP_FLUSH_TIME_AAC                 0x136 // 193.75ms
#define A2DP_FLUSH_TIME_SBC                 0x96 // 93.75ms
#define A2DP_INTERVAL                       0x3A // default 58, a2dp interval is set according to buffer size
#define A2DP_CTRL_RSVD_SLOT                 0x0A // default 0a
#define A2DP_FLUSH_TIME_LOW_LATENCY         0x32
#define A2DP_INTERVAL_LOW_LATENCY           0x20
#define A2DP_CTRL_RSVD_SLOT_LOW_LATENCY     0x0A
#define A2DP_FLUSH_TIME_ULTRA_LOW_LATENCY       0x10
#define A2DP_INTERVAL_ULTRA_LOW_LATENCY         0x60
#define A2DP_CTRL_RSVD_SLOT_ULTRA_LOW_LATENCY   0x0A
#define A2DP_INTERVAL_MULTILINK_PREEMPTIVE      0x30 // default 48

#if F_APP_QOL_MONITOR_SUPPORT
#define A2DP_IDLE_SKIP_PERIOD_MULTILINK_SRC_AWAY        0x04
#endif
/* End */


/** End of APP_BT_SNIFFING_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_BT_SNIFFING_Exported_Types App BT Sniffing Exported Types
    * @{
    */
typedef enum
{
    APP_BT_SNIFFING_STATE_IDLE                  = 0x00,  // no sniffing link
    APP_BT_SNIFFING_STATE_READY                 = 0x01,  // ready to start sniffing
    APP_BT_SNIFFING_STATE_STARTING              = 0x02,  // start sniffing
    APP_BT_SNIFFING_STATE_SNIFFING              = 0x03,  // sniffing
    APP_BT_SNIFFING_STATE_STOPPING              = 0x04,  // stop sniffing
    APP_BT_SNIFFING_STATE_ROLESWAP              = 0x05,  // roleswap
} T_APP_BT_SNIFFING_STATE;

typedef enum
{
    APP_BT_SNIFFING_EVENT_ROLESWITCH                = 0x00,
    APP_BT_SNIFFING_EVENT_ACL_SNIFFING_CMPL         = 0x01,
    APP_BT_SNIFFING_EVENT_ACL_SNIFFING_DISCONN_CMPL = 0x02,
    APP_BT_SNIFFING_EVENT_A2DP_START_IND            = 0x03,
    APP_BT_SNIFFING_EVENT_A2DP_SNIFFING_STARTED     = 0x04,
    APP_BT_SNIFFING_EVENT_A2DP_STOP                 = 0x05,
    APP_BT_SNIFFING_EVENT_A2DP_SNIFFING_STOPPED     = 0x06,
    APP_BT_SNIFFING_EVENT_SCO_CONN_IND              = 0x07,
    APP_BT_SNIFFING_EVENT_SCO_CONN_CMPL             = 0x08,
    APP_BT_SNIFFING_EVENT_SCO_SNIFFIG_STARTED       = 0x09,
    APP_BT_SNIFFING_EVENT_SCO_STOP                  = 0x0a,
    APP_BT_SNIFFING_EVENT_SCO_SNIFFING_STOPPED      = 0x0b,
    APP_BT_SNIFFING_EVENT_SCO_DISCONN_CMPL          = 0x0c,
    APP_BT_SNIFFING_EVENT_ROLESWAP_START            = 0x0d,
    APP_BT_SNIFFING_EVENT_ROLESWAP_CMPL             = 0x0e,
#if F_APP_MUTILINK_VA_PREEMPTIVE
    APP_BT_SNIFFING_EVENT_A2DP_MUTE                 = 0x0f,
    APP_BT_SNIFFING_EVENT_A2DP_UNMUTE               = 0x10,
#endif
    APP_BT_SNIFFING_EVENT_ROLESWAP_TERMINATED       = 0x11,
    APP_BT_SNIFFING_EVENT_ROLESWAP_FAIL             = 0x12,
} T_APP_BT_SNIFFING_STATE_EVENT;

typedef enum
{
    BT_SNIFFING_TYPE_NONE = 0x00,
    BT_SNIFFING_TYPE_A2DP = 0x01,
    BT_SNIFFING_TYPE_SCO  = 0x02,
} T_BT_SNIFFING_TYPE;

typedef enum
{
    SNIFFING_ALLOW = 0x00,
    SNIFFING_LINK_ERROR = 0x01,
    SNIFFING_B2B_ROLE_SWITCH = 0x02,
    SNIFFING_SRC_ROLE_SWITCH = 0x03,
    SNIFFING_B2B_RTDP_NOT_EXIST = 0x04,
    SNIFFING_SRC_PROFILE_NOT_EXIST = 0x05,
    SNIFFING_B2B_POWER_OFF_ING = 0x06,
    SNIFFING_B2B_DECRYPT = 0x07,
    SNIFFING_LE_AUDIO = 0x08,
} T_SNIFFING_CHECK_RESULT;

typedef enum
{
    APP_BT_SNIFFING_EVENT_BLE_DEV_STATE_CHANGE                         = 0x01,
    APP_BT_SNIFFING_EVENT_BLE_CONN_STATE_CHANGE                        = 0x02,
    APP_BT_SNIFFING_EVENT_BLE_ENCRYPT_STATE_CHANGE                     = 0x03,
    APP_BT_SNIFFING_EVENT_BLE_ACTIVE_CHANGE                            = 0x04,
    APP_BT_SNIFFING_EVENT_LEGACY_LINK_CHANGE                           = 0x05,
    APP_BT_SNIFFING_EVENT_LOW_LATENCY_CHANGE                           = 0x06,
    APP_BT_SNIFFING_EVENT_ACL_SNIFFING_LINK_CMPL_NORMAL                = 0x07,
    APP_BT_SNIFFING_EVENT_ACL_SNIFFING_LINK_CMPL_MULTILINK_PREEMPTIVE  = 0X08,
    APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE                             = 0x09,
#if F_APP_QOL_MONITOR_SUPPORT
    APP_BT_SNIFFING_EVENT_MULTI_SRC_GOES_AWAY                          = 0x0A,
    APP_BT_SNIFFING_EVENT_MULTI_SRC_GOES_NEAR                          = 0x0B,
#endif
    APP_BT_SNIFFING_EVENT_ROLESWAP_DOWNSTREAM_CMPL                     = 0x10,
    APP_BT_SNIFFING_EVENT_ISO_SUSPEND                                  = 0x11,
    APP_BT_SNIFFING_EVENT_ISO_RESTORE                                  = 0x12,
    APP_BT_SNIFFING_EVENT_DATA_CAPTURE_START                           = 0x13,
    APP_BT_SNIFFING_EVENT_DATA_CAPTURE_STOP                            = 0x14,
} T_APP_BT_SNIFFING_CFG_EVT;

typedef enum
{
    APP_BT_SNIFFING_PERIODIC_EVENT_RSSI_UPDATE                         = 0x01,
    APP_BT_SNIFFING_PERIODIC_EVENT_LEVEL_UPDATE                        = 0x02,
} T_APP_BT_SNIFFING_PERIODIC_EVT;

typedef enum
{
    BT_LINK_ENCRYPT_NONE = 0x00,
    BT_LINK_ENCRYPT_ENABLE = 0x01,  // link encrypted
    BT_LINK_ENCRYPT_DISABLE = 0x02, // force disable encrytion
} T_BT_LINK_ENCRYPT_STATE;

typedef struct
{
    uint16_t a2dp_interval;
    uint16_t flush_timeout;
    uint8_t rsvd_slot;
    uint16_t idle_slot;
    uint16_t idle_skip;
    uint8_t  nack_num;
    bool     quick_flush;
} T_APP_BT_SNIFFING_PARAM;

typedef enum
{
    APP_BT_SNIFFING_NO_BLE_MULTILINK_DISABLED = 0x00,
    APP_BT_SNIFFING_LINKBACK = 0x01,
    APP_BT_SNIFFING_PREEMP = 0x02,
    APP_BT_SNIFFING_NORMAL_BLE_ADV_ONGOING = 0x03,
    APP_BT_SNIFFING_NORMAL_BLE_ACTIVE = 0x04,
    APP_BT_SNIFFING_NORMAL_BLE_CONNECTED = 0x05,
    APP_BT_SNIFFING_NORMAL_MULTILINK_ENABLED = 0x06,
    APP_BT_SNIFFING_GAMING_BLE_ADV_ONGOING = 0x07,
    APP_BT_SNIFFING_GAMING_BLE_ENCRYPTED = 0x08,
    APP_BT_SNIFFING_GAMING_BLE_CONNECTED = 0x09,
    APP_BT_SNIFFING_GAMING_MULTILINK_ENABLED = 0x0A,
    APP_BT_SNIFFING_LEA_CIS_ADV_ONGOING = 0x0B,
    APP_BT_SNIFFING_LEA_CIS_CONNECTED = 0x0C,
    APP_BT_SNIFFING_SCENE_MAX = 0x0D,
} T_APP_BT_SNIFFING_SCENE;

typedef struct
{
    uint8_t idle_slot;
    uint8_t idle_skip;
} T_APP_BT_SNIFFING_HTPOLL;

typedef void (*BT_SNIFFING_ROLESWAP_CB)(void);

extern const uint8_t sniffing_params_low_latency_table[3][LOW_LATENCY_LEVEL_MAX];

/** End of APP_BT_SNIFFING_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BT_SNIFFING_Exported_Functions App Bt Sniffing Exported Functions
    * @{
    */

/* @brief  app bt sniffing module init.
*
* @param  void
* @return none
*/
void app_bt_sniffing_init(void);

/* @brief  start roleswap
*
* @param  stop_after_shadow     not start sniffing after roleswap
* @return bool
*/
bool app_bt_sniffing_roleswap(bool stop_after_shadow);

/* @brief  start sniffing
*
* @param  bd_addr   acl link address
* @param  type   sniffing type
* @return bool
*/
bool app_bt_sniffing_start(uint8_t *bd_addr, T_BT_SNIFFING_TYPE type);

/* @brief  stop sniffing
*
* @param  bd_addr   acl link address
* @param  type   sniffing type
* @return none
*/
void app_bt_sniffing_stop(uint8_t *bd_addr, T_BT_SNIFFING_TYPE type);


void app_bt_sniffing_audio_cfg(uint8_t *bd_addr, uint16_t interval, uint16_t flush_tout,
                               uint8_t rsvd_slot, uint8_t idle_slot, uint8_t idle_skip);

/* @brief  check roleswap
*
* @param  void
* @return bool
*/
bool app_bt_sniffing_roleswap_check(void);

/* @brief  process bt sniffing
*
* @param  bd_addr   acl link address
* @return none
*/
void app_bt_sniffing_process(uint8_t *bd_addr);

/* @brief  hfp connect
*
* @param  bd_addr   acl link address
* @return none
*/
void app_bt_sniffing_hfp_connect(uint8_t *bd_addr);

/* @brief  set low latency (gaming mode)
*
* @param  enable    start/stop
* @return bool
*/
bool app_bt_sniffing_set_low_latency(void);
void app_bt_sniffing_update_nack_flush_param(uint8_t *bd_addr);
void app_bt_sniffing_param_update(T_APP_BT_SNIFFING_CFG_EVT event);
void app_bt_sniffing_sec_set_audio_cfg_param(T_APP_BT_SNIFFING_PARAM *p_msg);
void app_bt_sniffing_set_nack_flush_param(uint8_t *bd_addr);
#if F_APP_ERWS_SUPPORT
void app_bt_sniffing_judge_dynamic_set_gaming_mode(void);
#endif
#if F_APP_MUTILINK_VA_PREEMPTIVE
void app_bt_sniffing_a2dp_mute(uint8_t *bd_addr, T_BT_SNIFFING_TYPE type);
void app_bt_sniffing_a2dp_unmute(uint8_t *bd_addr, T_BT_SNIFFING_TYPE type);
#endif

#if F_APP_QOL_MONITOR_SUPPORT
void app_bt_sniffing_handle_periodic_evt(T_APP_BT_SNIFFING_PERIODIC_EVT event);
#endif

/** @} */ /* End of group APP_BT_SNIFFING_EXPORT_Functions */
/** @} */ /* End of group APP_BT_SNIFFING */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BT_SNIFFING_H_ */
