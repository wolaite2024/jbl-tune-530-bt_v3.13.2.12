#include "ftl_ext.h"
#include "ftl.h"
#include "bt_ext_ftl.h"
#include "storage.h"
#include "trace.h"

#if GATTC_TBL_STORAGE_SUPPORT

#if (FTL_POOL_ENABLE == 0)
uint16_t bt_ext_ftl_start_offset = 0;
uint16_t bt_ext_ftl_max_offset = 0;
#endif

int32_t bt_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_init_module(BT_EXT_FTL_PARTITION_NAME, BT_EXT_FTL_SIZE, BT_EXT_FTL_BLOCK_LEN);
#else
    return ftl_partition_init(info->address, 4, BT_EXT_FTL_BLOCK_LEN, &bt_ext_ftl_start_offset,
                              &bt_ext_ftl_max_offset);
#endif
}

int32_t bt_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                   uint32_t len,
                                   void *buf)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_save_to_module(BT_EXT_FTL_PARTITION_NAME, buf, offset, len);
#else
    return ftl_save_to_storage((void *)buf, offset + bt_ext_ftl_start_offset, len);
#endif
}

int32_t bt_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                  uint32_t len,
                                  void *buf)
{
#if (FTL_POOL_ENABLE == 1)
    return ftl_load_from_module(BT_EXT_FTL_PARTITION_NAME, buf, offset, len);
#else
    return ftl_load_from_storage((void *)buf, offset + bt_ext_ftl_start_offset, len);
#endif
}

static const T_STORAGE_PARTITION_INFO  bt_ext_partitions =
{
    .name = BT_EXT_FTL_PARTITION_NAME,
#if (FTL_POOL_ENABLE == 1)
    .address = NULL,
    .size = NULL,
#elif (defined(BT_EXT_FTL_ADDR))
    .address = BT_EXT_FTL_ADDR,
    .size = BT_EXT_FTL_SIZE,
#endif
    .perm = STORAGE_PERMISSION_READ | STORAGE_PERMISSION_WRITE,
    .media_type = STORAGE_MEDIA_TYPE_NOR,
    .content_type = STORAGE_CONTENT_TYPE_RW_DATA,
    .init = bt_ext_ftl_partition_init,
    .read = bt_ext_ftl_partition_read,
    .write = bt_ext_ftl_partition_write,
    .erase = NULL,
    .async_read = NULL,
    .async_write = NULL,
    .async_erase = NULL,
};

bool bt_ext_ftl_init(void)
{
#if ((defined(BT_EXT_FTL_ADDR)) || (FTL_POOL_ENABLE == 1))
    if (storage_partition_init(&bt_ext_partitions, 1) == 0)
    {
        return true;
    }
#endif
    APP_PRINT_ERROR0("bt_ext_ftl_init:failed");
    return false;
}

bool bt_ext_ftl_deinit(void)
{
#if ((defined(BT_EXT_FTL_ADDR)) || (FTL_POOL_ENABLE == 1))
    if (storage_partition_deinit(&bt_ext_partitions) == 0)
    {
        return true;
    }
#endif
#if (FTL_POOL_ENABLE == 0)
    bt_ext_ftl_start_offset = 0;
    bt_ext_ftl_max_offset = 0;
#endif
    APP_PRINT_ERROR0("bt_ext_ftl_deinit:failed");
    return false;
}
#endif
