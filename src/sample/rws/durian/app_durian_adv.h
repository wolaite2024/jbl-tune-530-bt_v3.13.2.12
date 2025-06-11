/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_ADV_H_
#define _APP_DURIAN_ADV_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_adv_pairing_finish(void);
void app_durian_adv_bud_connected(void);
void app_durian_adv_pairing_timeout(void);
void app_durian_adv_set_power_on_cause(void);
void app_durian_adv_init(void);
void app_durian_adv_purpose_set_none(void);
void app_durian_adv_purpose_set_open_case(void);
void app_durian_adv_close_case(bool from_remote);
void app_durian_adv_factory_reset(void);
void app_durian_adv_open_case(void);
void app_durian_adv_enter_pairing(void);
void app_durian_adv_roleswap(void);
void app_durian_adv_in_box(void);
void app_durian_adv_out_box(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ADP_DAT_H_ */
