#if F_APP_TUYA_SUPPORT

#include "trace.h"
#include "rtk_tuya_app_ext_ftl.h"
#include "ftl.h"

#define APP_TUYA_EXT_FTL_PARTITION_NAME "TUYA_EXT_FTL"

uint32_t app_tuya_ext_save_ftl_data(uint8_t *data, uint32_t offset, uint32_t len)
{
    return storage_write(APP_TUYA_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

uint32_t app_tuya_ext_load_ftl_data(uint8_t *data, uint32_t offset, uint32_t len)
{
    return storage_read(APP_TUYA_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

int32_t app_tuya_ext_ftl_partition_init(const T_STORAGE_PARTITION_INFO *info)
{
    return ftl_init_module(APP_TUYA_EXT_FTL_PARTITION_NAME, APP_TUYA_FTL_SIZE, 4);
}

int32_t app_tuya_ext_ftl_partition_write(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                         uint32_t len,
                                         void *buf)
{
    return ftl_save_to_module(APP_TUYA_EXT_FTL_PARTITION_NAME, (void *)buf, offset, len);
}

int32_t app_tuya_ext_ftl_partition_read(const T_STORAGE_PARTITION_INFO *info, uint32_t offset,
                                        uint32_t len,
                                        void *buf)
{
    return ftl_load_from_module(APP_TUYA_EXT_FTL_PARTITION_NAME, (void *)buf, offset, len);
}

static const T_STORAGE_PARTITION_INFO  app_tuya_ext_partitions =
{
    .name = APP_TUYA_EXT_FTL_PARTITION_NAME,
    .address = NULL,
    .size = NULL,
    .perm = STORAGE_PERMISSION_READ | STORAGE_PERMISSION_WRITE,
    .media_type = STORAGE_MEDIA_TYPE_NOR,
    .content_type = STORAGE_CONTENT_TYPE_RW_DATA,
    .init = app_tuya_ext_ftl_partition_init,
    .read = app_tuya_ext_ftl_partition_read,
    .write = app_tuya_ext_ftl_partition_write,
    .erase = NULL,
    .async_read = NULL,
    .async_write = NULL,
    .async_erase = NULL,
};

bool app_tuya_ext_ftl_init(void)
{
    if (storage_partition_init(&app_tuya_ext_partitions, 1) == 0)
    {
        return true;
    }
    APP_PRINT_ERROR0("app_tuya_ext_ftl_init: failed");
    return false;
}

#endif
