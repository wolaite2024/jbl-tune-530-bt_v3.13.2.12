/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _RTK_TUYA_BLE_OTA_FLASH_H_
#define _RTK_TUYA_BLE_OTA_FLASH_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"  {
#endif

uint32_t tuya_ota_get_app_ui_version(void);

uint8_t tuya_ota_calc_crc(uint32_t length, uint32_t *crc);

bool tuya_ota_check_image(void *p_header);

uint32_t tuya_ota_write_to_flash(uint32_t dfu_base_addr, uint32_t offset, uint32_t length,
                                 void *p_void, bool is_control_header);

void tuya_ota_clear_notready_flag(void);

uint8_t app_harman_ble_ota_get_active_bank(void);

#ifdef __cplusplus
}
#endif

#endif
