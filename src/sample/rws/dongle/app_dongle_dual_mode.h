/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DONGLE_BT_POLICY_H_
#define _APP_DONGLE_BT_POLICY_H_

#include <stdint.h>
#include "audio_track.h"
#include "app_link_util.h"

typedef enum
{
    APP_REMOTE_MSG_SYNC_BT_MODE                           = 0x00,
    APP_REMOTE_MSG_SYNC_HEADSET_STATUS                    = 0x01,
    APP_REMOTE_MSG_DONGLE_MAX_MSG_NUM,
} T_APP_DONGLE_REMOTE_MSG;

typedef enum
{
    SWITCH_EVENT_ENTER_PAIRING,
    SWITCH_EVENT_ENTER_STANDBY,
    SWITCH_EVENT_ENTER_LINKBACK,
    SWITCH_EVENT_SWITCH_MODE,
    SWITCH_EVENT_BT_SHUTDOWN,
} T_APP_DONGLE_SWITCH_EVENT;

typedef enum
{
    /* support bt & 2.4g if multilink enabled */
    STANDBY_MODE_BT_OR_BT_24G,
    PAIRING_MODE_BT_OR_BT_24G,
    LINKBACK_MODE_BT_OR_BT_24G,
    /* 2.4g only */
    STANDBY_MODE_24G,
    PAIRING_MODE_24G,
    LINKBACK_MODE_24G,
} T_APP_DONGLE_MODE;

typedef enum
{
    DONGLE_HTPOLL_EVENT_ACL_DISC,
    DONGLE_HTPOLL_EVENT_ACL_CONN,
    DONGLE_HTPOLL_EVENT_A2DP_START,
    DONGLE_HTPOLL_EVENT_A2DP_STOP,
    DONGLE_HTPOLL_EVENT_LINKBACK_START,
    DONGLE_HTPOLL_EVENT_LINKBACK_STOP,
    DONGLE_HTPOLL_EVENT_SCO_CONNECTED,
    DONGLE_HTPOLL_EVENT_SCO_DISCONNECTED,
    DONGLE_HTPOLL_EVENT_ACTIVE_A2DP_IDX_CHANGED,
    DONGLE_HTPOLL_EVENT_BLE_CONNECTED,
    DONGLE_HTPOLL_EVENT_BLE_DISCONNECTED,
} T_APP_DONGLE_HTPOLL_EVENT;

typedef enum
{
    DONGLE_PAIRING_MODE_NONE,
    DONGLE_PAIRING_MODE_24G,
    DONGLE_PAIRING_MODE_BT,
    DONGLE_PAIRING_MODE_ALL,
} T_APP_DONGLE_PAIRING_MODE;

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
typedef enum
{
    EVENT_2_4G_STREAM_START = 0x00,
    EVENT_2_4G_STREAM_STOP  = 0x01,
    EVENT_2_4G_CALL_ACTIVE  = 0x02,
    EVENT_2_4G_CALL_END     = 0x03,
    EVENT_HFP_CALL_ACTIVE   = 0x04,
    EVENT_HFP_CALL_END      = 0x05,
    EVENT_ALL               = 0xFF,
} T_APP_DONGLE_PREEMPT_EVENT;

typedef enum
{
    ACTION_2_4G_STREAM_PREEMPT_A2DP_STREAM      = 0x00,
    ACTION_2_4G_STREAM_STOP_RESUME_A2DP_STREAM  = 0x01,
    ACTION_2_4G_CALL_ACTIVE_PREEMPT_HFP_CALL    = 0x02,
    ACTION_2_4G_CALL_END_RESUME_HFP_CALL        = 0x03,
    ACTION_ALL = 0xFF,
} T_APP_DONGLE_PREEMPT_ACTION;
#endif

typedef enum
{
    PHONE_STREAM_IDLE,
    PHONE_STREAM_A2DP,
    PHONE_STREAM_HFP,
} T_PHONE_STREAM_STATUS;

typedef struct
{
    T_PHONE_STREAM_STATUS phone_status;
    bool audio_mixing_support;
    bool headset_linkback;
    bool rtp_enable;
} T_HEADSET_STATUS;

typedef struct
{
    uint8_t length;

    /* 0xff: manufacturer data */
    uint8_t type;

    /* 0x5d, 0x00, 0x08 */
    uint8_t magic_data[3];

    /* bit0: 0- stereo headset 1- TWS headset
       bit1: 1- support LowLatency with Gaming Dongle
             0- not support LowLatency with Gaming Dongle */
    uint8_t headset_type;

    /* bit 3~0:
       0: sbc frame nums in each avdtp packet depend on dongle side */
    uint8_t sbc_frame_num;

    /* (app_cfg_const.rws_custom_uuid >> 8) & 0xFF; */
    uint8_t pairing_id_1;
    /* app_cfg_const.rws_custom_uuid & 0xFF; */
    uint8_t pairing_id_2;

    /*
        bit 1~0: Set SPP Voice Sample Rate.
        bit   2: Set Multilink feature bit.
        bit 7~3: rsv.
    */
    uint8_t feature_set;
} __attribute__((packed)) T_DONGLE_EIR_DATA;

typedef struct
{
    uint8_t length;
    uint8_t type;

    uint8_t pairing_bit : 1;
    uint8_t rsv2 : 7;

    T_DONGLE_EIR_DATA eir_data;

    uint8_t rsv[18];
} __attribute__((packed)) T_DONGLE_BLE_DATA;

typedef struct
{
    uint8_t start_linkback_when_shutdown : 1;
    uint8_t dongle_adv_enabled : 1;
    uint8_t enable_pairing : 1;
    uint8_t key_missing : 1;
    uint8_t enter_pairing_after_mode_switch : 1;
    uint8_t ignore_radio_mode_idle_when_shutdown : 1;
    uint8_t trigger_pairing_non_intentionally : 1;
    uint8_t disallow_dongle_a2dp : 1;

    uint8_t low_rssi_disconnet_dongle : 1;
    uint8_t switch_pairing_triggered : 1;
    uint8_t linkback_src_after_disc : 1;
    uint8_t keep_pairing_mode_type : 2;
    uint8_t legacy_dongle_streaming : 1;
    uint8_t rsv : 2;
} T_DONGLE_CONTROL_DATA;

typedef struct
{
    uint8_t rsv;
    uint8_t snk_audio_loc;
    uint8_t src_conn_num;
} T_DONGLE_STATUS;

extern T_DONGLE_CONTROL_DATA dongle_ctrl_data;
extern T_DONGLE_STATUS dongle_status;
extern T_HEADSET_STATUS headset_status;

#if F_APP_DONGLE_MULTI_PAIRING
void app_dongle_switch_pairing_mode(void);
#endif

void app_dongle_sync_headset_status(void);

#if (F_APP_ERWS_SUPPORT == 0)
bool app_dongle_dual_mode_linkback(void);
#endif
void app_dongle_shutdown_end_check(void);
void app_dongle_dual_mode_switch(T_APP_DONGLE_SWITCH_EVENT evt);
void app_dongle_handle_headset_adv(bool enable_adv);
uint8_t app_dongle_get_phone_bond_num(void);
uint8_t app_dongle_get_dongle_num(void);
T_APP_DONGLE_MODE app_dongle_get_mode(void);
bool app_dongle_is_24g_mode(void);
void app_dongle_handle_ble_disconnected(uint8_t *bd_addr);
void app_dongle_handle_ble_connected(uint8_t *bd_addr);
void app_dongle_adv_stop(void);
void app_dongle_adv_start(bool enable_pairing);
uint16_t app_dongle_get_gaming_latency(void);
bool app_dongle_set_gaming_latency(T_AUDIO_TRACK_HANDLE p_handle, uint16_t latency_value);
void app_dongle_htpoll_control(T_APP_DONGLE_HTPOLL_EVENT event);
void app_dongle_dual_mode_init(void);

void app_dongle_exit_pairing_mode(void);
bool app_dongle_enter_pairing_mode(T_APP_DONGLE_PAIRING_MODE mode);


#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
void app_dongle_le_audio_handle_mic(bool mic_enable);
void app_dongle_handle_hfp_call(void);
void app_dongle_handle_stream_preempt(T_APP_DONGLE_PREEMPT_EVENT event);
#endif

void app_dongle_lea_handle_dongle_status(void);
#endif

