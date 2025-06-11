#if F_APP_USB_MSC_SUPPORT
#include "errno.h"
#include "usb_ms_driver.h"
#include "sd_if.h"
#include "os_mem.h"
#include <string.h>

static uint32_t usb_ms_disk_blk_size = 0x200;

static int usb_ms_disk_format(void)
{
    return ESUCCESS;
}

void *usb_ms_disk_buffer_alloc(uint32_t blk_num)
{
    return os_mem_zalloc(RAM_TYPE_BUFFER_ON, blk_num * usb_ms_disk_blk_size);
}

int usb_ms_disk_buffer_free(void *buf)
{
    os_mem_free(buf);

    return ESUCCESS;
}

static int usb_ms_disk_read(uint32_t lba, uint32_t blk_num, uint8_t *data)
{
    int ret = ESUCCESS;
    if (sd_if_read(lba, (uint32_t)data, usb_ms_disk_blk_size, blk_num) != SD_OK)
    {
        ret = -EIO;
    }
    return ret;
}

static int usb_ms_disk_write(uint32_t lba, uint32_t blk_num, uint8_t *data)
{
    int ret = ESUCCESS;

    if (sd_if_write(lba, (uint32_t)data, usb_ms_disk_blk_size, blk_num) != SD_OK)
    {
        ret = -EIO;
    }

    return ret;
}

static bool usb_ms_disk_is_ready(void)
{
    return true;
}

static int usb_ms_disk_capacity_get(uint32_t *max_lba, uint32_t *blk_len)
{
//    *max_lba =  sd_if_get_dev_block_num();
    *blk_len = usb_ms_disk_blk_size;
    *max_lba =  sd_if_get_dev_capacity() / (*blk_len);
    usb_ms_disk_blk_size = *blk_len ;
    return ESUCCESS;
}

static const T_DISK_DRIVER usb_ms_disk_driver =
{
    .type = 0,
    .format = usb_ms_disk_format,
    .read = usb_ms_disk_read,
    .write = usb_ms_disk_write,
    .is_ready = usb_ms_disk_is_ready,
    .remove = NULL,
    .capacity_get = usb_ms_disk_capacity_get,
    .buffer_alloc = usb_ms_disk_buffer_alloc,
    .buffer_free = usb_ms_disk_buffer_free,
};

int usb_ms_disk_init(void)
{
//  sd_if_set_block_len(0x200);
    usb_ms_driver_disk_register((T_DISK_DRIVER *)&usb_ms_disk_driver);

    return 0;
}
#endif
