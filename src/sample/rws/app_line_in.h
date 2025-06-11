/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_LINE_IN_H_
#define _APP_LINE_IN_H_

#include <stdbool.h>
#include "app_msg.h"
#include "app_listening_mode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_line_in_board_init(void);
void app_line_in_init(void);
void app_line_in_driver_init(void);
void app_line_in_power_on_check(void);
void app_line_in_power_off_check(void);
void app_line_in_detect_intr_cb(uint32_t param);
void app_line_in_detect_msg_handler(T_IO_MSG *io_io_driver_msg_recv);
bool app_line_in_playing_state_get(void);
bool app_line_in_plug_state_get(void);
bool app_line_in_start_a2dp_check(void);
void app_line_in_volume_up_handle(void);
void app_line_in_volume_down_handle(void);
T_ANC_APT_STATE app_line_in_anc_line_in_mode_state_get(void);
T_ANC_APT_STATE app_line_in_anc_non_line_in_mode_state_get(void);
void app_line_in_call_status_update(bool is_idle);
bool app_line_in_start(void);
bool app_line_in_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_LINE_IN_H_ */
