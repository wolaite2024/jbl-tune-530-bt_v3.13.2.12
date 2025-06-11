
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TUYA_SUPPORT

#include <string.h>
#include <stdint.h>
#include <trace.h>
#include <rtl876x.h>
#include "rtk_tuya_ble_ota_flash.h"
#include "dfu_api.h"
#include "fmc_api.h"
//#include "rom_ext.h"
#include "app_ota.h"
#include "ota_ext.h"
#include "sha256.h"

#define TUYA_OTA_DEBUG_BYPASS_FLASH       0

uint32_t tuya_ota_get_app_ui_version(void)
{
    T_IMG_HEADER_FORMAT *addr;
    addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_MCUCONFIG);
    APP_PRINT_INFO1("tuya_ota_get_app_ui_version: 0x%08x", addr->git_ver.version);
    return (0xffff & addr->git_ver.version);
}

#define SHA256_LENGTH 32
#define SHA256_BUFFER_SIZE 128
bool tuya_ota_check_sha256(T_IMG_HEADER_FORMAT *addr)
{
#if TUYA_OTA_DEBUG_BYPASS_FLASH
    return true;
#else

    uint8_t sha256sum[SHA256_LENGTH];
    uint8_t sha256img[SHA256_LENGTH];
    uint32_t len;
    uint16_t i;
    uint16_t loop_cnt, remain_size;
    uint32_t p_data = (uint32_t)&addr->ctrl_header;

    len = sizeof(T_IMG_HEADER_FORMAT) - sizeof(T_AUTH_HEADER_FORMAT) + addr->ctrl_header.payload_len;

    uint8_t *buf = (uint8_t *)malloc(SHA256_BUFFER_SIZE);

    loop_cnt = len / SHA256_BUFFER_SIZE;
    remain_size = len % SHA256_BUFFER_SIZE;

    SHA256_CTX ctx = {0};
    SHA256_Init(&ctx);

    for (i = 0; i < loop_cnt; ++i)
    {
        fmc_flash_nor_read(p_data, buf, SHA256_BUFFER_SIZE);
        if (i == 0)
        {
            ((T_IMG_CTRL_HEADER_FORMAT *)buf)->ctrl_flag.not_ready = 0;
        }
        SHA256_Update(&ctx, buf, SHA256_BUFFER_SIZE);
        p_data += SHA256_BUFFER_SIZE;
    }
    if (remain_size)
    {
        fmc_flash_nor_read(p_data, buf, remain_size);
        if (loop_cnt == 0)
        {
            ((T_IMG_CTRL_HEADER_FORMAT *)buf)->ctrl_flag.not_ready = 0;
        }

        SHA256_Update(&ctx, buf, remain_size);
    }
    SHA256_Final(&ctx, sha256sum);

    free(buf);

    fmc_flash_nor_read((uint32_t)&addr->auth.image_hash, sha256img, SHA256_LENGTH);

    bool ret = (memcmp(sha256img, sha256sum, SHA256_LENGTH) == 0);

    return ret;

#endif
}

bool tuya_ota_check_image(void *p_header)
{
    bool check_result = tuya_ota_check_sha256((T_IMG_HEADER_FORMAT *)p_header);
    APP_PRINT_INFO1("tuya_ota_check_image: check_result %d", check_result);
    return check_result;
}

uint32_t tuya_ota_write_to_flash(uint32_t dfu_base_addr, uint32_t offset, uint32_t length,
                                 void *p_void, bool is_control_header)
{
#if TUYA_OTA_DEBUG_BYPASS_FLASH
    return 0;
#else
    uint8_t ret = 0;
    uint8_t readback_buffer[READBACK_BUFFER_SIZE];
    uint32_t read_back_len;
    uint32_t dest_addr;
    uint8_t *p_src = (uint8_t *)p_void;
    uint32_t remain_size = length;

    uint8_t bp_level;
    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            0);


    if (is_control_header)
    {
        T_IMG_CTRL_HEADER_FORMAT *p_header = (T_IMG_CTRL_HEADER_FORMAT *)p_void;
        p_header->ctrl_flag.not_ready = 0x1;
    }

    if (((offset + dfu_base_addr) % 0x1000) == 0)
    {
        fmc_flash_nor_erase(dfu_base_addr + offset, FMC_FLASH_NOR_ERASE_SECTOR);
    }
    else
    {
        if (((offset + dfu_base_addr) / 0x1000) != ((offset + length + dfu_base_addr) / 0x1000))
        {
            if ((offset + length + dfu_base_addr) % 0x1000)
            {
                fmc_flash_nor_erase((dfu_base_addr + offset + length) & ~(0x1000 - 1), FMC_FLASH_NOR_ERASE_SECTOR);
            }
        }
    }

    if (!fmc_flash_nor_write(dfu_base_addr + offset, p_void, length))
    {
        ret = 2;
    }

    extern void cache_flush_by_addr(uint32_t *addr, uint32_t length);
    cache_flush_by_addr((uint32_t *)(dfu_base_addr + offset), length);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            bp_level);

    dest_addr = dfu_base_addr + offset;
    while (remain_size)
    {
        read_back_len = (remain_size >= READBACK_BUFFER_SIZE) ? READBACK_BUFFER_SIZE : remain_size;

        if (!fmc_flash_nor_read(dest_addr, readback_buffer, read_back_len))
        {
            ret = 3;
            break;
        }

        if (memcmp(readback_buffer, p_src, read_back_len) != 0)
        {
            ret = 4;
            break;
        }

        dest_addr += read_back_len;
        p_src += read_back_len;
        remain_size -= read_back_len;
    }

    if (ret)
    {
        APP_PRINT_ERROR1("tuya_ota_write_to_flash: ret %d", ret);
    }

    return ret;

#endif
}

void tuya_ota_clear_notready_flag(void)
{
#if TUYA_OTA_DEBUG_BYPASS_FLASH
    return;
#else
    uint8_t bp_level;
    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), 0);
    app_ota_clear_notready_flag();
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), bp_level);
#endif
}

#endif
