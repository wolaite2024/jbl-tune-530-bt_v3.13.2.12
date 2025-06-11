/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_MMI_H_
#define _APP_DURIAN_MMI_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_mmi_voice_recognition_disable(uint8_t app_idx);
void app_durian_mmi_voice_recognition_enable(uint8_t app_idx);
void app_durian_mmi_play_fwd(void);
void app_durian_mmi_play_bwd(void);
void app_durian_mmi_adv_enter_pairing(void);
void app_durian_mmi_call_transfer_check(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
