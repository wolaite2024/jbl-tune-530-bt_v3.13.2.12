/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_EQ_FITTING_H_
#define _APP_EQ_FITTING_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void app_eq_fitting_init(void);

void app_eq_fitting_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt);

void app_eq_fitting_deinit(void);

bool app_eq_fitting_is_busy(void);

#ifdef __cplusplus
}
#endif

#endif
