/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_CFG_H_
#define _APP_DURIAN_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_cfg_rst_name(void);
void app_durian_cfg_power_on(void);
void app_durian_cfg_power_off(void);
void app_durian_cfg_avp_id_load(void);
void app_durian_cfg_switch_one_key_trig(uint8_t payload);
void app_durian_cfg_factory_rst(void);
void app_durian_cfg_multi_on(void);
void app_durian_cfg_init(void);
bool app_durian_cfg_multi_is_on(void);
void app_durian_cfg_set_name(T_BT_AVP_EVENT_SET_NAME *set_name);

void app_durian_cfg_save_load_avp_id(void *buf);
void app_durian_cfg_set_avp_id(void *buf);
void app_durian_cfg_save_load_single_id(void *buf);
void app_durian_cfg_set_single_id(void *buf);
uint32_t app_durian_cfg_id_save_data(void *p_data);
void app_durian_cfg_remote_id_save(uint8_t *buf);

bool app_durian_cfg_multi_is_on(void);

extern T_DURIAN durian_cfg;
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_DURIAN_ROLESWAP_H_ */
