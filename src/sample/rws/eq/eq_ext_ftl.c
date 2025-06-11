#if (F_APP_USER_EQ_SUPPORT == 1)

#include <stdint.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "ftl.h"
#include "eq_ext_ftl.h"
#include "storage.h"
#include "flash_map.h"
#include "errno.h"

#if (FTL_POOL_ENABLE == 1)
#define EQ_SIZE     (AUDIO_AND_APT_EQ_SIZE + VOCIE_SPK_EQ_SIZE)// audio spk eq, dsp apt eq, voice spk eq
#else
uint16_t eq_ext_ftl_start_offset = 0;
uint16_t eq_ext_ftl_max_offset = 0;
#endif

#define EQ_EXT_FTL_BLOCK_LEN     68

int32_t eq_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info);
int32_t eq_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                   uint32_t len, void *buf);
int32_t eq_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                  uint32_t len, void *buf);

void eq_ext_ftl_storage_init(void)
{
    static const T_STORAGE_PARTITION_INFO eq_partitions[] =
    {
        {
            .name = EQ_EXT_FTL_PARTITION_NAME,
#if (FTL_POOL_ENABLE == 1)
            .address = NULL,
            .size = NULL,
#else
            .address = EQ_EXT_FTL_ADDR,
            .size = EQ_EXT_FTL_SIZE,
#endif
            .perm = STORAGE_PERMISSION_READ | STORAGE_PERMISSION_WRITE,
            .media_type = STORAGE_MEDIA_TYPE_NOR,
            .content_type = STORAGE_CONTENT_TYPE_RW_DATA,
            .init = eq_ext_ftl_partition_init,
            .read = eq_ext_ftl_partition_read,
            .write = eq_ext_ftl_partition_write,
            .erase = NULL,
            .async_read = NULL,
            .async_write = NULL,
            .async_erase = NULL,
        },
    };

    storage_partition_init(eq_partitions, sizeof(eq_partitions) / sizeof(eq_partitions[0]));
}

int32_t eq_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_init_module(EQ_EXT_FTL_PARTITION_NAME, EQ_SIZE, EQ_EXT_FTL_BLOCK_LEN);
#else
    return ftl_partition_init(info->address, EQ_EXT_FTL_SIZE / 0x1000, EQ_EXT_FTL_BLOCK_LEN,
                              &eq_ext_ftl_start_offset,
                              &eq_ext_ftl_max_offset);
#endif
}

int32_t eq_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                   uint32_t len, void *buf)
{
    uint32_t offset_mod_block_len = 0;
    uint32_t new_offset = 0;
    uint32_t new_len = 0;
    uint32_t new_len_mod_block_len = 0;
    uint8_t *data = NULL;
    uint32_t ret = 0;

    offset_mod_block_len = offset % EQ_EXT_FTL_BLOCK_LEN;
    new_offset = offset - offset_mod_block_len;
    new_len = len + offset_mod_block_len;
    new_len_mod_block_len = new_len % EQ_EXT_FTL_BLOCK_LEN;

    if (new_len_mod_block_len != 0)
    {
        new_len = new_len + EQ_EXT_FTL_BLOCK_LEN - new_len_mod_block_len;
    }

    data = malloc(new_len);
    if (data != NULL)
    {
        if (new_offset != offset)
        {
#if (FTL_POOL_ENABLE == 1)
            ret = ftl_load_from_module(EQ_EXT_FTL_PARTITION_NAME, (void *)data, new_offset,
                                       EQ_EXT_FTL_BLOCK_LEN);
#else
            ret = ftl_load_from_storage((void *)data, new_offset + eq_ext_ftl_start_offset,
                                        EQ_EXT_FTL_BLOCK_LEN);
#endif

            if (ret == ENOF)
            {
                memset(data, 0, EQ_EXT_FTL_BLOCK_LEN);
                ret = 0;
            }
        }

        if (ret == 0 && (offset + len) != (new_offset + new_len))
        {
#if (FTL_POOL_ENABLE == 1)
            ret = ftl_load_from_module(EQ_EXT_FTL_PARTITION_NAME, (void *)&data[new_len - EQ_EXT_FTL_BLOCK_LEN],
                                       new_offset + new_len - EQ_EXT_FTL_BLOCK_LEN, EQ_EXT_FTL_BLOCK_LEN);
#else
            ret = ftl_load_from_storage((void *)&data[new_len - EQ_EXT_FTL_BLOCK_LEN],
                                        new_offset + new_len - EQ_EXT_FTL_BLOCK_LEN + eq_ext_ftl_start_offset, EQ_EXT_FTL_BLOCK_LEN);
#endif

            if (ret == ENOF)
            {
                memset(&data[new_len - EQ_EXT_FTL_BLOCK_LEN], 0, EQ_EXT_FTL_BLOCK_LEN);
                ret = 0;
            }
        }

        if (ret == 0)
        {
            memcpy_s(&data[offset - new_offset], (new_len - (offset - new_offset)), buf, len);
#if (FTL_POOL_ENABLE == 1)
            ret = ftl_save_to_module(EQ_EXT_FTL_PARTITION_NAME, (void *)data, new_offset, new_len);
#else
            ret = ftl_save_to_storage((void *)data, new_offset + eq_ext_ftl_start_offset, new_len);
#endif
        }

        free(data);
    }

    return ret;
}

int32_t eq_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                  uint32_t len, void *buf)
{
    uint32_t offset_mod_block_len = 0;
    uint32_t new_offset = 0;
    uint32_t new_len = 0;
    uint32_t new_len_mod_block_len = 0;
    uint8_t *data = NULL;
    uint32_t ret = 0;

    offset_mod_block_len = offset % EQ_EXT_FTL_BLOCK_LEN;
    new_offset = offset - offset_mod_block_len;
    new_len = len + offset_mod_block_len;
    new_len_mod_block_len = new_len % EQ_EXT_FTL_BLOCK_LEN;

    if (new_len_mod_block_len != 0)
    {
        new_len = new_len + EQ_EXT_FTL_BLOCK_LEN - new_len_mod_block_len;
    }

    data = malloc(new_len);
    if (data != NULL)
    {
#if (FTL_POOL_ENABLE == 1)
        ret = ftl_load_from_module(EQ_EXT_FTL_PARTITION_NAME, (void *)data, new_offset, new_len);
#else
        ret = ftl_load_from_storage((void *)data, new_offset + eq_ext_ftl_start_offset, new_len);
#endif
        if (ret == 0)
        {
            memcpy(buf, &data[offset - new_offset], len);
        }

        free(data);
    }

    return ret;
}

#endif
