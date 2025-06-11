/**
************************************************************************************************************
*            Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file      app_cap_touch.h
* @brief     Cap Touch implementation header file.
* @author    barry chu
* @date      2021-05-18
* @version   v1.0
*************************************************************************************************************
*/
#ifndef _APP_CAP_TOUCH_H
#define _APP_CAP_TOUCH_H

#include "app_msg.h"

void app_cap_touch_cmd_param_handle(uint8_t cmd_path, uint8_t *cmd_ptr, uint8_t app_idx);
void app_cap_touch_driver_init(bool first_power_on);
void app_cap_touch_msg_handle(T_IO_MSG_CAP_TOUCH subtype);
void app_cap_touch_send_event_check(void);
void app_cap_touch_set_ld_det(bool enable);
bool app_cap_touch_get_ld_det(void);
void app_cap_touch_calibration_start(void);
void app_cap_touch_init(void);

#endif

