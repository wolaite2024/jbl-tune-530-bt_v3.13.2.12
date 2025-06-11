/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_SS_SUPPORT

#include <stdbool.h>

#include "app_io_msg.h"

bool app_ss_cmd_register(void);

void app_ss_cmd_handle_msg(T_IO_MSG *msg);

void app_ss_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt);

#endif

