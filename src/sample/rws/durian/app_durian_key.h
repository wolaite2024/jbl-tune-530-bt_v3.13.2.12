/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_KEY_H_
#define _APP_DURIAN_KEY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool app_durian_key_press(uint8_t key_mask, uint8_t mode, uint8_t type);
bool app_durian_key_release_event(uint8_t key_mask, uint8_t pressed);
uint8_t app_durian_key_multiclick_get(uint8_t cfg);
uint8_t app_durian_key_long_press_get(uint8_t cfg);
void app_durian_key_long_press_action(bool from_remote);
void app_durian_key_control_set(uint8_t left_ear_control, uint8_t right_ear_control);
void app_durian_key_click_set(uint8_t left_right_en);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
