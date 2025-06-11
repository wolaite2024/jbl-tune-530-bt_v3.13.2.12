/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_ANC_H_
#define _APP_DURIAN_ANC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    AVP_ANC_NULL                             = 0x00,
    AVP_ANC_CLOSE                            = 0x01,
    AVP_ANC_OPEN                             = 0x02,
    AVP_ANC_TRANSPARENCY                     = 0x03,
} T_AVP_ANC_SETTINGS;

typedef enum
{
    ANC_MODE_ALL_OFF                         = 0x00,
    ANC_MODE_ANC_ON                          = 0x01,
    ANC_MODE_APT_ON                          = 0x02,
} T_AVP_ANC_MODE;

typedef enum
{
    AVP_ANC_CYCLE_NULL                       = 0x00,
    AVP_ANC_CYCLE_OPEN_CLOSE                 = 0x03,
    AVP_ANC_CYCLE_TRANSPARENCY_CLOSE         = 0x05,
    AVP_ANC_CYCLE_OPEN_TRANSPARENCY          = 0x06,
    AVP_ANC_CYCLE_OPEN_TRANSPARENCY_CLOSE    = 0x07,
} T_AVP_CYCLE_SETTING_ANC;


void app_durian_anc_report(T_AVP_ANC_SETTINGS set);
void app_durian_anc_check_tone(bool *need_tone);
void app_durain_anc_siri_start_check(void);
void app_durain_anc_siri_stop_check(void);
void app_durian_anc_cylce_set(uint8_t set);
void app_durian_anc_set(uint8_t anc_cur_setting);
void app_durian_anc_call_stop_check(void);
void app_durian_anc_call_start_check(void);
void app_durian_anc_switch(void);
void app_durian_anc_switch_check(void);
void app_durian_anc_mode_switch(uint8_t mode);
void app_durian_anc_auto_apt_cfg(uint8_t auto_apt_en);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
