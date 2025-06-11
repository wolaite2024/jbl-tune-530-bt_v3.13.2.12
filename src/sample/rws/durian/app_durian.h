/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_H_
#define _APP_DURIAN_H_

#include "sysm.h"
#include "bt_avp.h"
#include "trace.h"
#include "durian.h"
#include "durian_avp.h"
#include "durian_atti.h"
#include "durian_adv.h"
#include "app_durian_anc.h"
#include "app_durian_avp.h"
#include "app_durian_cfg.h"
#include "app_durian_adv.h"
#include "app_durian_link.h"
#include "app_durian_audio.h"
#include "app_durian_adp.h"
#include "app_durian_mmi.h"
#include "app_durian_sync.h"
#include "app_durian_cmd.h"
#include "app_durian_key.h"
#include "app_durian_loc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    DURIAN_EVENT_START             = 0x00,
    DURIAN_EVENT_INIT              = 0x01,

} T_DURIAN_EVENT;

typedef struct
{
    bool left_click_anc_en;
    bool right_click_anc_en;
    T_AVP_CYCLE_SETTING_ANC anc_cycle_setting;

    union
    {
        uint8_t local_remote_double_action;

        struct
        {
            uint8_t remote_double_click_action: 4;
            uint8_t local_double_click_action: 4;
        };
    };

    union
    {
        uint8_t local_remote_long_action;

        struct
        {
            uint8_t remote_long_action: 4;
            uint8_t local_long_action: 4;
        };
    };

    uint8_t ear_detect_en;

    T_BT_AVP_MIC mic_setting;

#if F_APP_LISTENING_MODE_SUPPORT
    uint8_t anc_one_bud_enabled;
    T_AVP_ANC_SETTINGS anc_cur_setting;
    T_AVP_ANC_SETTINGS anc_pre_setting;
    T_AVP_ANC_SETTINGS anc_one_setting;
    T_AVP_ANC_SETTINGS anc_both_setting;

    bool anc_apt_need_tone;

#if DURIAN_PRO2
    uint8_t auto_apt_en;
#endif

    bool apt_off_cause_siri;
    bool apt_off_cause_call;
#endif

#if DURIAN_PRO2
    uint8_t vol_ctl_en;
#endif

#if F_APP_TWO_GAIN_TABLE
    bool amplify_gain;
#endif

    uint8_t local_loc;
    uint8_t remote_loc;

    bool both_in_ear_src_lost;
    bool local_loc_changed;
    bool remote_loc_changed;

    bool click_speed_rec;
    uint8_t click_speed;
    uint8_t click_speed_origin;

    bool long_press_time_rec;
    uint8_t long_press_time;
    uint8_t long_press_origin;

    bool power_on_by_cmd;
    bool power_off_by_cmd;

    bool power_on_from_factory_reset;

    T_AVP_COMPACTNESS_DB local_compactness;
    T_AVP_COMPACTNESS_DB remote_compactness;

    uint8_t both_in_ear;

    uint8_t adv_purpose;
    uint8_t adv_purpose_last;
    uint8_t adv_serial;

    bool b2b_synced;

    uint8_t local_batt;
    uint8_t remote_batt;

    uint8_t adv_disallow_update_batt: 1;
    uint8_t role_decided: 1;
    uint8_t need_open_case_adv: 1;
    uint8_t fast_pair_connected: 1;
    uint8_t avp_db_rsv: 4;

    uint8_t id_is_display;
    bool remote_loc_received;
} T_DURIAN_DB;

extern T_DURIAN_DB durian_db;

void app_durian_init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
