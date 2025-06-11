/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_SYNC_H_
#define _APP_DURIAN_SYNC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**  @brief  App define roleswap event */
typedef enum
{
    SYNC_EVENT_DURIAN_RSV                      = 0x00,
    SYNC_EVENT_DURIAN_LOCAL_REMOTE_DOUBLE_CFG  = 0x01,
    SYNC_EVENT_DURIAN_EAR_DETECT_ENABLE        = 0x02,
    SYNC_EVENT_DURIAN_MIC_SETTING              = 0x03,
    SYNC_EVENT_DURIAN_LOCAL_REMOTE_LONG_CFG    = 0x04,
    SYNC_EVENT_DURIAN_RSV0                     = 0x05,
    SYNC_EVENT_DURIAN_ANC_MFB_EVENT            = 0x06,
    SYNC_EVENT_DURIAN_ANC_CYCLE_SETTING        = 0x07,
    SYNC_EVENT_DURIAN_ANC_ONE_BUD_ENABLE       = 0x08,
    SYNC_EVENT_DURIAN_ANC_CUR_SETTING          = 0x09,
    SYNC_EVENT_DURIAN_ANC_ONE_SETTING          = 0x0A,
    SYNC_EVENT_DURIAN_ANC_BOTH_SETTING         = 0x0B,
    SYNC_EVENT_DURIAN_AUTO_APT_ENABLE          = 0x0C,
    SYNC_EVENT_DURIAN_KEY_CLICK_SPEED          = 0x0D,
    SYNC_EVENT_DURIAN_KEY_LONG_PRESS_TIME      = 0x0E,
    SYNC_EVENT_DURIAN_DISABLE_DISPLAY_AVP_ID   = 0x0F,
    SYNC_EVENT_DURIAN_VOL_CTL_EN               = 0x10,
    SYNC_EVENT_DURIAN_RSV1                     = 0x11,
    SYNC_EVENT_DURIAN_ADV_PURPOSE              = 0x12,
    SYNC_EVENT_DURIAN_AMPLIFY_GAIN             = 0x13,
    SYNC_EVENT_DURIAN_REMOTE_SINGLE_ID         = 0x14,
    SYNC_EVENT_DURIAN_BUD_SYNC                 = 0x15,
    SYNC_EVENT_DURIAN_SYNC_AVP_ID              = 0x16,
    SYNC_EVENT_DURIAN_SYNC_AVP_SINGLE_ID       = 0x17,
    SYNC_EVENT_DURIAN_SYNC_ONE_KEY_TRIGGER     = 0x18,
    SYNC_EVENT_DURIAN_FAST_PAIR_CONNETED       = 0x19,

    SYNC_EVENT_DURIAN_MAX,
} T_DURIAN_SYNC_EVENT;

typedef struct
{
    union
    {
        uint8_t local_remote_double_action;

        struct
        {
            uint8_t remote_double_click_action: 4;
            uint8_t local_double_click_action: 4;
        };
    };

    uint8_t click_speed;
    uint8_t long_press_time;

    uint8_t mic_setting;

    uint8_t ear_detect_en;


    union
    {
        uint8_t local_remote_long_action;

        struct
        {
            uint8_t remote_long_action: 4;
            uint8_t local_long_action: 4;
        };
    };

#if F_APP_LISTENING_MODE_SUPPORT
    uint8_t anc_cycle_setting;
    uint8_t anc_one_bud_enabled;
    uint8_t anc_cur_setting;
    uint8_t anc_one_setting;
    uint8_t anc_both_setting;
#if DURIAN_PRO2
    uint8_t auto_apt_en;
#endif
#endif

#if DURIAN_PRO2
    uint8_t vol_ctl_en;
#endif

#if F_APP_TWO_GAIN_TABLE
    bool amplify_gain;
#endif

    uint8_t adv_purpose;
} T_DURIAN_SYNC_BUD;

void app_durian_sync_report_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len);
void app_durian_sync_long_press(void);
void app_durian_sync_fast_pair(void);
void app_durian_sync_init(void);
void app_durian_sync_fastpair(void);
void app_durian_sync_amplify_gain(uint8_t *bd_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_DURIAN_ROLESWAP_H_ */
