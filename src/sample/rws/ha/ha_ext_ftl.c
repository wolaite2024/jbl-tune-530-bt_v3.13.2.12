#if F_APP_HEARABLE_SUPPORT

#include <stdint.h>
#include <string.h>

#include "ftl.h"
#include "ha_ext_ftl.h"
#include "storage.h"
#include "flash_map.h"
#include "errno.h"

#define HA_EXT_FTL_BLOCK_LEN        128
#define HA_EXT_FTL_PARTITION_NAME   "HA_FTL"

uint32_t ha_ext_ftl_load_data(void *data, uint32_t offset, uint32_t len)
{
    return storage_read(HA_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

uint32_t ha_ext_ftl_save_data(void *data, uint32_t offset, uint32_t len)
{
    return storage_write(HA_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

static int32_t ha_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info)
{
    return ftl_init_module(HA_EXT_FTL_PARTITION_NAME, HA_SIZE, HA_EXT_FTL_BLOCK_LEN);
}

static int32_t ha_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                          uint32_t len, void *buf)
{
    int32_t status = 0;
    uint32_t idx = 0;
    uint8_t tp_buf[HA_EXT_FTL_BLOCK_LEN];
    uint32_t cur_offset = 0;
    uint32_t left_len = 0;

    idx = offset / HA_EXT_FTL_BLOCK_LEN;
    cur_offset = offset % HA_EXT_FTL_BLOCK_LEN;
    left_len = len;

    while (left_len)
    {
        status = ftl_load_from_module(HA_EXT_FTL_PARTITION_NAME,
                                      (void *)tp_buf, idx * HA_EXT_FTL_BLOCK_LEN,
                                      HA_EXT_FTL_BLOCK_LEN);

        if (status)
        {
            memset(tp_buf, 0, HA_EXT_FTL_BLOCK_LEN);
            status = ftl_save_to_module(HA_EXT_FTL_PARTITION_NAME,
                                        (void *)tp_buf, idx * HA_EXT_FTL_BLOCK_LEN,
                                        HA_EXT_FTL_BLOCK_LEN);

            if (status)
            {
                break;
            }
        }

        if (cur_offset + left_len <= HA_EXT_FTL_BLOCK_LEN)
        {
            memcpy(tp_buf + cur_offset, buf, left_len);

            buf = (uint8_t *)buf + left_len;
            left_len = 0;
        }
        else
        {
            memcpy(tp_buf + cur_offset, buf, HA_EXT_FTL_BLOCK_LEN - cur_offset);
            buf = (uint8_t *)buf + (HA_EXT_FTL_BLOCK_LEN - cur_offset);
            left_len -= (HA_EXT_FTL_BLOCK_LEN - cur_offset);
        }

        status = ftl_save_to_module(HA_EXT_FTL_PARTITION_NAME,
                                    (void *)tp_buf, idx * HA_EXT_FTL_BLOCK_LEN,
                                    HA_EXT_FTL_BLOCK_LEN);

        if (status)
        {
            break;
        }

        if (cur_offset)
        {
            cur_offset = 0;
        }

        idx += 1;
    }

    return status;
}

static int32_t ha_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                         uint32_t len, void *buf)
{
    int32_t status = 0;
    uint32_t idx = 0;
    uint8_t tp_buf[HA_EXT_FTL_BLOCK_LEN];
    uint32_t cur_offset = 0;
    uint32_t left_len = 0;

    idx = offset / HA_EXT_FTL_BLOCK_LEN;
    cur_offset = offset % HA_EXT_FTL_BLOCK_LEN;
    left_len = len;

    while (left_len)
    {
        status = ftl_load_from_module(HA_EXT_FTL_PARTITION_NAME,
                                      (void *)tp_buf, idx * HA_EXT_FTL_BLOCK_LEN,
                                      HA_EXT_FTL_BLOCK_LEN);

        if (status)
        {
            break;
        }

        if (cur_offset + left_len <= HA_EXT_FTL_BLOCK_LEN)
        {
            memcpy(buf, tp_buf + cur_offset, left_len);
            buf = (uint8_t *)buf + left_len;
            left_len = 0;
        }
        else
        {
            memcpy(buf, tp_buf + cur_offset, HA_EXT_FTL_BLOCK_LEN - cur_offset);
            buf = (uint8_t *)buf + (HA_EXT_FTL_BLOCK_LEN - cur_offset);
            left_len -= (HA_EXT_FTL_BLOCK_LEN - cur_offset);
        }

        if (cur_offset)
        {
            cur_offset = 0;
        }

        idx += 1;
    }

    return status;
}

void ha_ext_ftl_storage_init(void)
{
    static const T_STORAGE_PARTITION_INFO ha_partitions[] =
    {
        {
            .name = HA_EXT_FTL_PARTITION_NAME,
            .address = NULL,
            .size = NULL,
            .perm = STORAGE_PERMISSION_READ | STORAGE_PERMISSION_WRITE,
            .media_type = STORAGE_MEDIA_TYPE_NOR,
            .content_type = STORAGE_CONTENT_TYPE_RW_DATA,
            .init = ha_ext_ftl_partition_init,
            .read = ha_ext_ftl_partition_read,
            .write = ha_ext_ftl_partition_write,
            .erase = NULL,
            .async_read = NULL,
            .async_write = NULL,
            .async_erase = NULL,
        },
    };

    storage_partition_init(ha_partitions, sizeof(ha_partitions) / sizeof(ha_partitions[0]));
}

#endif
