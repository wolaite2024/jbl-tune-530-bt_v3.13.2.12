/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_CMD_H_
#define _APP_DURIAN_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_durian_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                           uint8_t *ack_pkt);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
