#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <trace.h>
#include "ftl.h"
#include "os_sched.h"
#include "os_mem.h"
#include "ble_conn.h"
#include "patch_header_check.h"
#include "app_timer.h"
#include "app_cfg.h"
#include "flash_map.h"
#include "app_ota.h"
#include "app_relay.h"
#include "app_device.h"
#include "app_auto_power_off.h"
#include "app_sniff_mode.h"
#include "app_hfp.h"
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#include "app_harman_ble_ota.h"
#include "app_harman_report.h"
#include "app_harman_license.h"
#include "ota_ext.h"
#include "fmc_api.h"
#include "app_bt_policy_api.h"
#include "app_main.h"
#include "app_ble_common_adv.h"
#include "app_ble_gap.h"

#define HARMAN_COPY_WDG_KICK_SIZE       0xF000

// #define HARMAN_OTA_SAME_CHECK_OFFSET            BKP_DATA2_ADDR
// #define HARMAN_OTA_SAME_CHECK_LISENCE_LEN       4
// uint8_t harman_ota_same_check[HARMAN_OTA_SAME_CHECK_LISENCE_LEN] = {0};

#define OTA_DOWNLOAD_ADDR               OTA_BANK1_ADDR

static uint8_t vp_index_bank0 = 0xff;
static uint32_t vp_dest_download_addr = 0;
static uint32_t vp_src_download_addr = 0;
static uint32_t vp_src_download_size = 0;
static uint8_t vp_only_status = 0xff;
static uint8_t harman_ota_timer_queue_id = 0;
static uint8_t timer_handle_ota_check_package = 0;
static uint8_t timer_handle_harman_ota_reset = 0;
static uint8_t timer_handle_harman_ota_delay_disconn = 0;

static bool ota_notify_direct = false;

static T_HARMAN_OTA_MGR harman_ota_mgr;
static uint8_t ota_upgrate_status = 0;
static uint8_t *ota_write_buf = NULL;
static uint8_t *ota_write_buf_restored = NULL;
static uint16_t buf_restored_len;
static uint8_t *p_fw_temp = NULL;

typedef enum
{
    APP_OTA_PACKAGE_CHECK                       = 0x00,
    HARMAN_OTA_TIMER_DELAY_RESET                = 0x01,
    HARMAN_OTA_TIMER_DELAY_DISCONN_BT_AND_BLE   = 0x02,
} T_HARMAN_BLE_OTA_TIMER;

bool app_harman_ble_ota_get_notify_direct(void)
{
    return ota_notify_direct;
}

uint8_t app_harman_ble_ota_get_upgrate_status(void)
{
    return ota_upgrate_status;
}

void app_harman_ble_ota_set_upgrate_status(uint8_t val)
{
    ota_upgrate_status = val;
}

static void app_harman_ota_reset(uint8_t reason)
{
    DBG_DIRECT("app_harman_ota_reset: reason: %d", reason);

    app_start_timer(&timer_handle_harman_ota_delay_disconn, "harman_ota_delay_disconn_link",
                    harman_ota_timer_queue_id, HARMAN_OTA_TIMER_DELAY_DISCONN_BT_AND_BLE, 0, false, 1000);
}

static void app_harman_ota_notification_report(uint16_t status_code, uint32_t cur_offset,
                                               uint32_t required_size, uint8_t app_idx)
{
    uint8_t *p_cmd_rsp = NULL;
    uint8_t payloadlen = 0;
    uint16_t percentage = 0;
    T_APP_HARMAN_OTA_NOTIFICATION notification = {0};

    if (app_idx >  MAX_BR_LINK_NUM)
    {
        return;
    }

    percentage = ((harman_ota_mgr.total_relative_offset * 100) /
                  harman_ota_mgr.ota_total_size) * 100;

    APP_PRINT_INFO6("app_harman_ota_notification_report: status_code: 0x%04x, percentage: %d, "
                    "absolute_offset: 0x%04x, length_to_read: 0x%08x, "
                    "total_relative_offset: 0x%08x, ota_total_size: 0x%08x",
                    status_code, percentage, cur_offset, required_size,
                    harman_ota_mgr.total_relative_offset, harman_ota_mgr.ota_total_size);

    notification.status_code = status_code;
    notification.percentage = percentage;
    notification.absolute_offset = cur_offset;
    notification.length_to_read = required_size;

    payloadlen = sizeof(T_APP_HARMAN_OTA_NOTIFICATION);
    p_cmd_rsp = malloc(payloadlen);
    if (p_cmd_rsp != NULL)
    {
        memcpy(p_cmd_rsp, &notification, payloadlen);

        app_harman_report_le_event(&app_db.le_link[app_idx], CMD_HARMAN_OTA_NOTIFICATION, p_cmd_rsp,
                                   payloadlen);
        free(p_cmd_rsp);
        p_cmd_rsp = NULL;
    }

    harman_ota_mgr.cur_offset = cur_offset;
    harman_ota_mgr.required_size = required_size;

    harman_ota_mgr.recved_packets_count = 0;
    harman_ota_mgr.recved_offset_error_retry = 0;
}

#if HARMAN_OTA_COMPLETE_NONEED_POWERON
static void app_harman_ota_complete_set()
{
    app_cfg_nv.harman_ota_complete = 1;
    app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 4);
}
#endif

static void app_harman_ota_breakpoint_clear(void)
{
    /* TODO: clear NV about breakpoint OTA, if OTA SUCCESS or FAIL */
    app_cfg_nv.harman_breakpoint_offset = 0x00;
    app_cfg_nv.cur_sub_image_index = 0x00;
    app_cfg_nv.cur_sub_image_relative_offset = 0x00;
    app_cfg_nv.harman_record_ota_version = 0x00;
    app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 16);
}

static void app_harman_ota_breakpoint_save(uint32_t offset)
{
    /* TODO: save NV about breakpoint OTA */
    app_cfg_nv.harman_breakpoint_offset = offset;
    app_cfg_nv.cur_sub_image_index = harman_ota_mgr.cur_sub_image_index;
    app_cfg_nv.cur_sub_image_relative_offset = harman_ota_mgr.cur_sub_image_relative_offset;
    app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 16);

    APP_PRINT_INFO4("app_harman_ota_breakpoint_save: cur_sub_image_index: %d, cur_sub_image_relative_offset: 0x%x, "
                    "total_relative_offset: 0x%x, ota_total_size: 0x%x",
                    harman_ota_mgr.cur_sub_image_index,
                    harman_ota_mgr.cur_sub_image_relative_offset,
                    harman_ota_mgr.total_relative_offset,
                    harman_ota_mgr.ota_total_size);

}

static bool app_harman_ota_protocol_ver_check(void)
{
    bool ret = false;
    /* TODO: protocol version is same with last OTA */


    return ret;
}

static bool app_harman_ota_package_ver_check(void)
{
    bool ret = true;
    uint32_t local_ver;
    T_IMG_HEADER_FORMAT *addr = NULL;

    addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_MCUCONFIG);

    local_ver = (uint8_t)(addr->git_ver.version >> 16) << 16;
    local_ver += (uint8_t)(addr->git_ver.version >> 8) << 8;
    local_ver += (uint8_t)addr->git_ver.version;

    APP_PRINT_INFO5("app_harman_ota_package_ver_check: package_ota_ver: 0x%08x, local_ver: 0x%08x, "
                    "last_package_ota_ver: 0x%08x, is_vp_only: %d, ver_compare_result: %d",
                    harman_ota_mgr.sub_bin.package_header.ota_version,
                    local_ver, app_cfg_nv.harman_record_ota_version,
                    harman_ota_mgr.sub_bin.package_header.is_vp_only,
                    (harman_ota_mgr.sub_bin.package_header.ota_version >= local_ver));
    /* TODO: OTA version is larger than local version and same with last OTA */
    if (harman_ota_mgr.sub_bin.package_header.ota_version >= local_ver)
    {
        if (harman_ota_mgr.sub_bin.package_header.is_vp_only)
        {
            /* only OTA VP */
            app_harman_ota_breakpoint_clear();
        }
        else
        {
            // breakpoint OTA check
            if (app_cfg_nv.harman_record_ota_version == 0x00)
            {
                app_cfg_nv.harman_record_ota_version = harman_ota_mgr.sub_bin.package_header.ota_version;
                app_cfg_store(&app_cfg_nv.harman_record_ota_version, 4);
            }
            else if (harman_ota_mgr.sub_bin.package_header.ota_version != app_cfg_nv.harman_record_ota_version)
            {
                // Not allowed to proceed breakpoint OTA
                ret = false;
            }
        }
    }
    else
    {
        // Not allowed to proceed OTA because of version too lower
        ret = false;
    }

    return ret;
}

static bool app_harman_ota_package_pid_check(void)
{
    bool ret = false;
    uint16_t cur_pid = app_harman_license_pid_get();

    if (harman_ota_mgr.sub_bin.package_header.is_vp_only)
    {
        ret = true;
    }
    else
    {
        if (harman_ota_mgr.sub_bin.package_header.pid == cur_pid)
        {
            ret = true;
        }
    }

    APP_PRINT_INFO4("app_harman_ota_package_pid_check: ret: %d, is_vp_only: %d, "
                    "harman_product_id: 0x%04x, ota_package_pid: 0x%04x",
                    ret, harman_ota_mgr.sub_bin.package_header.is_vp_only,
                    cur_pid, harman_ota_mgr.sub_bin.package_header.pid);

    return ret;
}

static void app_harman_ble_ota_package_check_timer_start(uint32_t time)
{
    app_start_timer(&timer_handle_ota_check_package, "harman_ota_package_check",
                    harman_ota_timer_queue_id, APP_OTA_PACKAGE_CHECK, 0, false,
                    OTA_PACKET_CHAEK_TIMEOUT);
}

static void app_harman_ble_ota_package_check_timer_stop(void)
{
    if (timer_handle_ota_check_package != 0)
    {
        app_stop_timer(&timer_handle_ota_check_package);
    }
}

static void app_harman_ble_ota_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_INFO3("app_harman_ble_ota_timeout_cb: timer id: 0x%02x, timer_chann: %d, package_missing_check_times: %d",
                    timer_id, timer_chann, harman_ota_mgr.package_missing_check_times);

    switch (timer_id)
    {
    case APP_OTA_PACKAGE_CHECK:
        {
            app_harman_ble_ota_package_check_timer_stop();

            uint8_t mobile_app_idx = MAX_BR_LINK_NUM;
            uint16_t status_code = 0;

            harman_get_active_mobile_cmd_link(&mobile_app_idx);

            harman_ota_mgr.package_missing_check_times ++;
            ota_notify_direct = true;
            if ((harman_ota_mgr.package_missing_check_times != OTA_PACKAGE_MISSING_CHECK_TOTAL_TIMES)
                && (ota_upgrate_status))
            {
                // status_code = (uint16_t)(STATUS_ERROR_PROTOCOL | FIRST_TIMEOUT);

                app_harman_ota_notification_report(status_code, harman_ota_mgr.cur_offset,
                                                   harman_ota_mgr.required_size, mobile_app_idx);

                app_harman_ble_ota_package_check_timer_start(OTA_PACKET_CHAEK_TIMEOUT);
            }
            else
            {
                app_harman_ota_exit(OTA_EXIT_REASON_SECOND_TIMEOUT);
            }
        }
        break;

    case HARMAN_OTA_TIMER_DELAY_DISCONN_BT_AND_BLE:
        {
            app_stop_timer(&timer_handle_harman_ota_delay_disconn);

            app_bt_policy_disconnect_all_link();
            app_ble_gap_disconnect_all(LE_LOCAL_DISC_CAUSE_OTA_RESET);

            app_start_timer(&timer_handle_harman_ota_reset, "harman_ota_delay_reset",
                            harman_ota_timer_queue_id, HARMAN_OTA_TIMER_DELAY_RESET, 0, false, 3000);
        }
        break;

    case HARMAN_OTA_TIMER_DELAY_RESET:
        {
            chip_reset(RESET_ALL);
        }
        break;

    default:
        break;
    }
}

static uint8_t app_harman_ota_get_active_bank(void)
{
    uint8_t active_bank;

#if HARMAN_COMPRESSED_OTA_SUPPORT
    if (harman_ota_mgr.sub_bin.package_header.compressed_ota ||
        harman_ota_mgr.sub_bin.package_header.is_vp_only)
    {
        active_bank = IMAGE_LOCATION_BANK0;
    }
    else
#endif
    {
        uint32_t ota_bank0_addr;
        if (is_ota_support_bank_switch())
        {
            ota_bank0_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
            if (ota_bank0_addr == get_active_ota_bank_addr())
            {
                active_bank = IMAGE_LOCATION_BANK0;
            }
            else
            {
                active_bank = IMAGE_LOCATION_BANK1;
            }
        }
        else
        {
            active_bank = NOT_SUPPORT_BANK_SWITCH;
        }
    }

    return active_bank;
}

static void app_harman_ota_package_header_dump(void)
{
    APP_PRINT_INFO7("app_harman_ota_package_header_dump: protocol_version: 0x%04x, ota_version: 0x%08x, "
                    "ota_size: 0x%08x, fw_table_offset: 0x%04x, fw_table_size: 0x%04x, fw_table_items: 0x%04x, "
                    "compressed_ota: %d",
                    harman_ota_mgr.sub_bin.package_header.protocol_version,
                    harman_ota_mgr.sub_bin.package_header.ota_version,
                    harman_ota_mgr.sub_bin.package_header.ota_size,
                    harman_ota_mgr.sub_bin.package_header.fw_table_offset,
                    harman_ota_mgr.sub_bin.package_header.fw_table_size,
                    harman_ota_mgr.sub_bin.package_header.fw_table_items,
                    harman_ota_mgr.sub_bin.package_header.compressed_ota);
}

static void app_harman_ota_fw_table_dump(void)
{
    for (uint8_t i = 0; i < harman_ota_mgr.sub_bin.package_header.fw_table_items; i++)
    {
#if HARMAN_COMPRESSED_OTA_SUPPORT
        if (harman_ota_mgr.sub_bin.package_header.compressed_ota)
        {
            if ((i == 0) && (harman_ota_mgr.sub_bin.fw_table[0].image_id == PRE_IMG_VP)) // VP
            {
                // first imgae in FW table must be VP data
            }
            else if (i == 1)
            {
                harman_ota_mgr.sub_bin.fw_table[1].download_addr = OTA_DOWNLOAD_ADDR;
            }
            else if (i >= 2)
            {
                harman_ota_mgr.sub_bin.fw_table[i].download_addr =
                    harman_ota_mgr.sub_bin.fw_table[i - 1].download_addr  +
                    harman_ota_mgr.sub_bin.fw_table[i - 1].fw_size;
            }
        }
#endif
        APP_PRINT_INFO5("app_harman_ota_fw_table_dump: image_id: 0x%x, fw_minor_type: 0x%x, "
                        "fw_offset 0x%x, fw_size 0x%x, download_addr: 0x%x",
                        harman_ota_mgr.sub_bin.fw_table[i].image_id,
                        harman_ota_mgr.sub_bin.fw_table[i].fw_minor_type,
                        harman_ota_mgr.sub_bin.fw_table[i].fw_offset,
                        harman_ota_mgr.sub_bin.fw_table[i].fw_size,
                        harman_ota_mgr.sub_bin.fw_table[i].download_addr);
    }
}

static void app_harman_ota_sub_image_control_header_check(uint8_t *p_data, uint16_t len)
{
#if HARMAN_COMPRESSED_OTA_SUPPORT
    if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
        (len == COMPRESSED_OTA_SUB_IMAGE_CONTROL_HEAD_SIZE))
    {
        T_COMPRESS_CTRL_HEADER_FORMAT *p_img_header = (T_COMPRESS_CTRL_HEADER_FORMAT *)p_data;
        APP_PRINT_INFO6("app_harman_ota_sub_image_control_header_check: img_string: %b, not_ready: %d, not_obsolete: %d, "
                        "ic_type: 0x%02x, image_id: 0x%04x, payload_len: 0x%08x",
                        TRACE_BINARY(8, p_img_header->img_string),
                        p_img_header->ctrl_flag.not_ready,
                        p_img_header->ctrl_flag.not_obsolete,
                        p_img_header->ic_type,
                        p_img_header->image_id,
                        p_img_header->payload_len);
    }
    else
#endif
    {
        T_IMG_CTRL_HEADER_FORMAT *p_img_header = (T_IMG_CTRL_HEADER_FORMAT *)p_data;
        APP_PRINT_INFO6("app_harman_ota_sub_image_control_header_check: "
                        " crc16: 0x%04x, ic_type: 0x%02x, secure_version: 0x%02x, value: 0x%04x, image_id: 0x%04x, payload_len: 0x%08x",
                        p_img_header->crc16, p_img_header->ic_type, p_img_header->secure_version,
                        p_img_header->ctrl_flag.value,
                        p_img_header->image_id, p_img_header->payload_len);
        APP_PRINT_INFO8("app_harman_ota_sub_image_control_header_check: "
                        "xip: %d, enc: %d, load_when_boot: %d, enc_load: %d, enc_key_select: %d, "
                        "not_ready: %d, not_obsolete: %d, integrity_check_en_in_boot: %d",
                        p_img_header->ctrl_flag.xip, p_img_header->ctrl_flag.enc, p_img_header->ctrl_flag.load_when_boot,
                        p_img_header->ctrl_flag.enc_load, p_img_header->ctrl_flag.enc_key_select,
                        p_img_header->ctrl_flag.not_ready,
                        p_img_header->ctrl_flag.not_obsolete, p_img_header->ctrl_flag.integrity_check_en_in_boot);
    }
}

static void app_harman_ota_active_bank_check(void)
{
    uint8_t active_bank = app_harman_ota_get_active_bank();

#if HARMAN_COMPRESSED_OTA_SUPPORT
    if (harman_ota_mgr.sub_bin.package_header.compressed_ota ||
        harman_ota_mgr.sub_bin.package_header.is_vp_only)
    {
        harman_ota_mgr.cur_sub_image_index = 0;
        harman_ota_mgr.end_sub_image_index = harman_ota_mgr.sub_bin.package_header.fw_table_items;
    }
    else
#endif
    {
        if (active_bank == IMAGE_LOCATION_BANK0)
        {
            harman_ota_mgr.cur_sub_image_index = harman_ota_mgr.sub_bin.package_header.fw_table_items / 2;
            harman_ota_mgr.end_sub_image_index = harman_ota_mgr.sub_bin.package_header.fw_table_items;
        }
        else if (active_bank == IMAGE_LOCATION_BANK1)
        {
            harman_ota_mgr.cur_sub_image_index = 0;
            harman_ota_mgr.end_sub_image_index = harman_ota_mgr.sub_bin.package_header.fw_table_items / 2;
        }
        else
        {
            APP_PRINT_ERROR1("app_harman_ota_active_bank_check: active_bank %d", active_bank);
        }
    }

    harman_ota_mgr.ota_total_size = 0;
    for (uint8_t i = harman_ota_mgr.cur_sub_image_index; i < harman_ota_mgr.end_sub_image_index; i ++)
    {
        harman_ota_mgr.ota_total_size += harman_ota_mgr.sub_bin.fw_table[i].fw_size;
    }
    harman_ota_mgr.cur_sub_image_relative_offset = 0;
    APP_PRINT_INFO6("app_harman_ota_active_bank_check: current bud_role %d, app_cfg_const.bud_role %d, "
                    "active_bank %d, cur_sub_image_index %d, end_sub_image_index %d, ota_total_size: 0x%08x",
                    app_cfg_nv.bud_role, app_cfg_const.bud_role, active_bank,
                    harman_ota_mgr.cur_sub_image_index,
                    harman_ota_mgr.end_sub_image_index,
                    harman_ota_mgr.ota_total_size);
}

static uint32_t app_harman_ota_write_to_flash(uint32_t dfu_base_addr, uint32_t offset,
                                              uint32_t length,
                                              void *p_void)
{
    uint8_t ret = 0;
    uint8_t readback_buffer[READBACK_BUFFER_SIZE];
    uint32_t read_back_len;
    uint32_t dest_addr;
    uint8_t *p_src = (uint8_t *)p_void;
    uint32_t remain_size = length;
    uint8_t bp_level;
    uint8_t active_bank = app_harman_ota_get_active_bank();

    fmc_flash_nor_get_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), 0);

    if (offset == 0)
    {
#if HARMAN_COMPRESSED_OTA_SUPPORT
        if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
            (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
        {
            T_COMPRESS_IMG_HEADER_FORMAT *compress_image_header = (T_COMPRESS_IMG_HEADER_FORMAT *)p_void;
            compress_image_header->compress_ctrl_header.ctrl_flag.not_ready = 0x1;
        }
        else
#endif
        {
            T_IMG_HEADER_FORMAT *image_header = (T_IMG_HEADER_FORMAT *)p_void;
            image_header->ctrl_header.ctrl_flag.not_ready = 0x1;
        }
    }

    if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) ||
        (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != IMG_OTA))
    {
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
    }
    APP_PRINT_INFO3("app_harman_ota_write_to_flash: dfu_base_addr 0x%08x, offset 0x%x, length 0x%x, ",
                    dfu_base_addr, offset, length);
    if (!fmc_flash_nor_write(dfu_base_addr + offset, p_void, length))
    {
        ret = 2;
    }

    /* clear cache */
    extern void cache_flush_by_addr(uint32_t *addr, uint32_t length);
    cache_flush_by_addr((uint32_t *)dfu_base_addr, length);

    fmc_flash_nor_set_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), bp_level);

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
        APP_PRINT_ERROR1("app_harman_ota_write_to_flash: ret %d", ret);
    }
    return ret;
}

#include "sha256.h"
#define SHA256_LEN          32
#define SHA256_BUFFER_SIZE  128

static bool app_harman_check_sha256(void *p_header)
{
    uint8_t sha256sum[SHA256_LEN], sha256img[SHA256_LEN];
    uint16_t i, loop_cnt, remain_size;
    uint8_t *buf = (uint8_t *)malloc(SHA256_BUFFER_SIZE);
    uint32_t len = 0;
    uint32_t pdata = 0;
    uint32_t image_hash_addr = 0;
    T_IMG_CTRL_HEADER_FORMAT *ctrl_header = NULL;

#if HARMAN_COMPRESSED_OTA_SUPPORT
    T_COMPRESS_CTRL_HEADER_FORMAT *compress_ctrl_header = NULL;

    if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
        (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
    {
        T_COMPRESS_IMG_HEADER_FORMAT *compress_image_header = (T_COMPRESS_IMG_HEADER_FORMAT *)p_header;
        compress_ctrl_header = (T_COMPRESS_CTRL_HEADER_FORMAT *)
                               &compress_image_header->compress_ctrl_header;

        image_hash_addr = (uint32_t)&compress_image_header->compressed_image_sha256;
        pdata = (uint32_t)&compress_image_header->compress_ctrl_header;
        len = sizeof(T_COMPRESS_IMG_HEADER_FORMAT) + compress_ctrl_header->payload_len;
    }
    else
#endif
    {
        T_IMG_HEADER_FORMAT *image_header = (T_IMG_HEADER_FORMAT *)p_header;
        ctrl_header = (T_IMG_CTRL_HEADER_FORMAT *)&image_header->ctrl_header;

        image_hash_addr = (uint32_t)&image_header->auth.image_hash;
        pdata = (uint32_t)&image_header->ctrl_header;
        len = sizeof(T_IMG_HEADER_FORMAT) - sizeof(T_AUTH_HEADER_FORMAT) + ctrl_header->payload_len;
    }
    APP_PRINT_INFO3("app_harman_check_sha256: image_hash_addr: 0x%08x, pdata: 0x%08x, len: 0x%x",
                    image_hash_addr, pdata, len);

#if 0
    uint8_t bp_level;
    uint8_t active_bank = app_harman_ota_get_active_bank();

    fmc_flash_nor_get_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), 0);

    loop_cnt = len / SHA256_BUFFER_SIZE;
    remain_size = len % SHA256_BUFFER_SIZE;
    SHA256_CTX ctx = {0};
    SHA256_Init(&ctx);
    for (i = 0; i < loop_cnt; ++i)
    {
        fmc_flash_nor_read(pdata, buf, SHA256_BUFFER_SIZE);
        if (i == 0)
        {
#if HARMAN_COMPRESSED_OTA_SUPPORT
            if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
                (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
            {
                compress_ctrl_header->ctrl_flag.not_ready = 0;
            }
            else
#endif
            {
                ctrl_header->ctrl_flag.not_ready = 0;
            }
        }
        SHA256_Update(&ctx, buf, SHA256_BUFFER_SIZE);
        pdata += SHA256_BUFFER_SIZE;
    }
    if (remain_size)
    {
        fmc_flash_nor_read(pdata, buf, remain_size);
        if (loop_cnt == 0)
        {
#if HARMAN_COMPRESSED_OTA_SUPPORT
            if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
                (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
            {
                compress_ctrl_header->ctrl_flag.not_ready = 0;
            }
            else
#endif
            {
                ctrl_header->ctrl_flag.not_ready = 0;
            }
        }

        SHA256_Update(&ctx, buf, remain_size);
    }
    SHA256_Final(&ctx, sha256sum);
    free(buf);

    fmc_flash_nor_read(image_hash_addr, sha256img, SHA256_LEN);

    return (memcmp(sha256img, sha256sum, SHA256_LEN) == 0);

#else
    free(buf);
    return true;
#endif
}

static bool app_harman_ota_check_image(void *p_header)
{
    return app_harman_check_sha256(p_header);
}

static void app_harman_ota_clear_notready_flag(void)
{
    // TODO: compressed OTA check
    uint32_t base_addr;
    uint16_t ctrl_flag;
    T_IMG_HEADER_FORMAT *image_header;
    uint8_t bp_level;
    uint8_t active_bank = app_harman_ota_get_active_bank();

    fmc_flash_nor_get_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), &bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), 0);

#if HARMAN_COMPRESSED_OTA_SUPPORT
    if (harman_ota_mgr.sub_bin.package_header.compressed_ota)
    {
        T_COMPRESS_IMG_HEADER_FORMAT *compress_image_header = NULL;
        uint16_t cur_image_id = 0;
        uint8_t image_num = harman_ota_mgr.sub_bin.package_header.fw_table_items;

        for (int i = 0; i < image_num; i++)
        {
            base_addr = harman_ota_mgr.sub_bin.fw_table[i].download_addr;
            cur_image_id = harman_ota_mgr.sub_bin.fw_table[i].image_id;
            if (cur_image_id != PRE_IMG_VP)
            {
                compress_image_header = (T_COMPRESS_IMG_HEADER_FORMAT *)base_addr;
                APP_PRINT_INFO3("app_harman_ota_clear_notready_flag: 0x%x, 0x%x, 0x%x",
                                base_addr, cur_image_id, compress_image_header->compress_ctrl_header.image_id);
                if ((base_addr == 0) || (cur_image_id != compress_image_header->compress_ctrl_header.image_id))
                {
                    continue;
                }
                fmc_flash_nor_read((uint32_t)&compress_image_header->compress_ctrl_header.ctrl_flag.value,
                                   (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
                ctrl_flag &= ~0x01; //form 1 to 0
                fmc_flash_nor_write((uint32_t)&compress_image_header->compress_ctrl_header.ctrl_flag.value,
                                    (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
            }
            else // VP ctrl header handle
            {
                image_header = (T_IMG_HEADER_FORMAT *)base_addr;
                if ((base_addr == 0) || (cur_image_id != image_header->ctrl_header.image_id))
                {
                    continue;
                }
                fmc_flash_nor_read((uint32_t)&image_header->ctrl_header.ctrl_flag.value,
                                   (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
                ctrl_flag &= ~0x80; //form 1 to 0
                fmc_flash_nor_write((uint32_t)&image_header->ctrl_header.ctrl_flag.value,
                                    (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
            }
        }
    }
    else
#endif
    {
        IMG_ID img_max = IMAGE_MAX;
        T_IMG_HEADER_FORMAT *ota_header_addr = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id(
                                                   IMG_OTA);

        for (IMG_ID img_id = IMG_OTA; img_id < img_max; img_id++)
        {
            base_addr = get_temp_ota_bank_addr_by_img_id(img_id);
            image_header = (T_IMG_HEADER_FORMAT *)base_addr;
            if (img_id != IMG_OTA && !ota_header_addr->image_info[(img_id - IMG_SBL) * 2 + 1])
            {
                continue;
            }
            if (base_addr == 0 || img_id != image_header->ctrl_header.image_id)
            {
                continue;
            }
            fmc_flash_nor_read((uint32_t)&image_header->ctrl_header.ctrl_flag.value,
                               (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
            ctrl_flag &= ~0x80; //form 1 to 0
            fmc_flash_nor_write((uint32_t)&image_header->ctrl_header.ctrl_flag.value,
                                (uint8_t *)&ctrl_flag, sizeof(ctrl_flag));
        }
    }

    fmc_flash_nor_set_bp_lv(flash_partition_addr_get((T_FLASH_PARTITION_NAME)active_bank), bp_level);
}

static uint8_t app_harman_ota_copy_vp(void)
{
    uint8_t ret = DFU_ARV_SUCCESS;
    uint32_t offset = 0;
    uint16_t img_id = PRE_IMG_VP;
    uint8_t *buffer_addr = ota_write_buf;
    uint32_t buffer_size = OTA_WRITE_BUFFER_SIZE;
    uint8_t *p_copy_buffer = NULL;
    uint8_t bp_lv;
    uint8_t size_factor = 0;
    uint32_t dest_addr = vp_dest_download_addr;
    uint32_t src_addr = vp_src_download_addr;
    uint32_t image_size = vp_src_download_size;
    uint32_t remain_size = image_size;
    uint32_t copy_len = 0;

    APP_PRINT_INFO3("app_harman_ota_copy_vp: dest_addr: 0x%08x, src_addr: 0x%08x, image_size: 0x%08x",
                    dest_addr, src_addr, image_size);

    if ((dest_addr == 0) || (src_addr == 0) || (src_addr % FMC_FLASH_NOR_ERASE_SECTOR))
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    if (check_copy_image_id(img_id))
    {
        ret = DFU_ARV_FAIL_INVALID_PARAMETER;
        return ret;
    }

    T_IMG_HEADER_FORMAT *p_image_header = (T_IMG_HEADER_FORMAT *)src_addr;
    if (p_image_header->ctrl_header.image_id != img_id)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        return ret;
    }

    if (buffer_addr != NULL)
    {
        buffer_size = OTA_WRITE_BUFFER_SIZE;
    }
    else
    {
        while ((p_copy_buffer == NULL) && buffer_size)
        {
            buffer_size >>= size_factor;
            p_copy_buffer = (uint8_t *)malloc(buffer_size);
            size_factor ++;
        }
        buffer_addr = p_copy_buffer;
    }

    if (buffer_addr == NULL)
    {
        ret = DFU_ARV_FAIL_OPERATION;
        if (p_copy_buffer != NULL)
        {
            free(p_copy_buffer);
            p_copy_buffer = NULL;
        }
        return ret;
    }

    fmc_flash_nor_get_bp_lv(dest_addr, &bp_lv);

    while (remain_size > 0)
    {
        copy_len = (remain_size > buffer_size) ? buffer_size : remain_size;

        fmc_flash_nor_read(src_addr + offset, buffer_addr, copy_len);

        if (app_harman_ota_write_to_flash(dest_addr, offset, copy_len, buffer_addr) != 0)
        {
            ret = DFU_ARV_FAIL_FLASH_WRITE_ERROR;
            if (p_copy_buffer != NULL)
            {
                free(p_copy_buffer);
            }
            return ret;
        }
        remain_size -= copy_len;
        offset += copy_len;
        if (offset % HARMAN_COPY_WDG_KICK_SIZE == 0) // avoid WDT
        {
            WDG_Kick();
        }
    }

    if (app_harman_ota_check_image((void *)dest_addr) == false)
    {
        ret = DFU_ARV_FAIL_CRC_ERROR;
    }

    if (p_copy_buffer != NULL)
    {
        free(p_copy_buffer);
    }

    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), bp_lv);

    APP_PRINT_INFO2("app_harman_ota_copy_vp: ret: %d, image_id: 0x%x", ret, img_id);

    return ret;
}

static void app_harman_ble_ota_end_check(uint32_t download_addr, uint8_t app_idx)
{
    bool check_image = app_harman_ota_check_image((void *)download_addr);

    // request next sub image or stop OTA
    APP_PRINT_INFO2("app_harman_ble_ota_end_check: check_image: %d, vp_ota_status: %d",
                    check_image, app_cfg_nv.vp_ota_status);
    if (check_image)
    {
        // TODO: copy vp data
        if ((harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id == PRE_IMG_VP) &&
            (app_cfg_nv.vp_ota_status == 1))
        {
            uint8_t ota_result = DFU_ARV_SUCCESS;

            ota_result = app_harman_ota_copy_vp();

            app_cfg_nv.vp_ota_status = 0;
            app_cfg_store(&app_cfg_nv.vp_ota_status, 4);

            if (ota_result != DFU_ARV_SUCCESS)
            {
                app_harman_ota_reset(HARMAN_OTA_VP_COPY_FAIL);
            }
        }

        // update cur_sub_image_relative_offset
        harman_ota_mgr.cur_sub_image_relative_offset = 0;
        harman_ota_mgr.cur_sub_image_index ++;

        APP_PRINT_INFO2("app_harman_ble_ota_end_check: start update %d sub image, total_image_num: %d",
                        harman_ota_mgr.cur_sub_image_index, harman_ota_mgr.end_sub_image_index);
        // if it is last sub image
        if (harman_ota_mgr.cur_sub_image_index == harman_ota_mgr.end_sub_image_index)
        {
            //OTA complete
            app_harman_ble_ota_package_check_timer_stop();
            app_harman_ota_notification_report(STATUS_SUCCESS, harman_ota_mgr.ota_total_size, 0, app_idx);

            harman_ota_mgr.cur_sub_image_index = 0;
            app_harman_ota_clear_notready_flag();
            harman_ota_mgr.state = OTA_STATE_FINISH;

            if ((!harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
                (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
            {
                uint32_t header_addr = get_active_ota_bank_addr();
                APP_PRINT_INFO2("app_harman_ble_ota_end_check: header_addr: 0x%08x, erase_sector_num: 0x%x",
                                header_addr, FMC_FLASH_NOR_ERASE_SECTOR);
                fmc_flash_nor_erase(header_addr, FMC_FLASH_NOR_ERASE_SECTOR);
            }

            app_harman_ota_breakpoint_clear();

#if HARMAN_OTA_COMPLETE_NONEED_POWERON
#if HARMAN_T135_SUPPORT || HARMAN_T235_SUPPORT
            if (harman_ota_mgr.sub_bin.package_header.is_vp_only)
#endif
            {
                app_harman_ota_complete_set();
            }
#endif
            app_harman_ota_reset(HARMAN_OTA_COMPLETE);
        }
        else
        {
            // SKIP some header to get image control header
            uint32_t msg[2] = {0};

#if HARMAN_COMPRESSED_OTA_SUPPORT
            if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
                (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
            {
                msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                         harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
                msg[1] = COMPRESSED_OTA_SUB_IMAGE_CONTROL_HEAD_SIZE;
            }
            else
#endif
            {
                msg[0] = harman_ota_mgr.cur_sub_image_relative_offset + OTA_SUB_IMAGE_CTRL_HEADER_OFFSET +
                         harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
                msg[1] = OTA_SUB_IMAGE_CONTROL_HEAD_SIZE;
            }
            app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

            harman_ota_mgr.state = OTA_STATE_IMAGE_CONTROL_HEADER;
        }
    }
    else
    {
        app_harman_ota_breakpoint_clear();
        app_harman_ota_reset(HARMAN_OTA_IMAGE_CHECK_FAIL);
    }
}

static void app_harman_ble_ota_write_to_flash(void)
{
    uint32_t dfu_base_addr =
        harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].download_addr;
    uint32_t offset = harman_ota_mgr.cur_sub_image_relative_offset - harman_ota_mgr.buf_index;

    if (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id == PRE_IMG_VP)
    {
        if (harman_ota_mgr.cur_sub_image_relative_offset ==
            harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size)
        {
            app_cfg_nv.vp_ota_status = 1; // ota vp done
        }
        else
        {
            app_cfg_nv.vp_ota_status = 0; // ota vp ing
        }
        app_cfg_store(&app_cfg_nv.vp_ota_status, 4);

        vp_dest_download_addr =
            harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].download_addr;
        vp_src_download_addr = OTA_DOWNLOAD_ADDR;
        vp_src_download_size = harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size;
        dfu_base_addr = vp_src_download_addr;
    }

    APP_PRINT_INFO2("app_harman_ble_ota_write_to_flash: dfu_base_addr: 0x%x, offset: 0x%x",
                    dfu_base_addr, offset);
    uint32_t write_result = app_harman_ota_write_to_flash(dfu_base_addr, offset,
                                                          harman_ota_mgr.buf_index,
                                                          ota_write_buf);
    if (write_result != 0)
    {
        DBG_DIRECT("app_harman_ble_ota_write_to_flash: failed ret: %d", write_result);
        app_harman_ota_reset(HARMAN_OTA_FLASH_WRITE_FAIL);
    }
    else
    {
        app_harman_ota_breakpoint_save(harman_ota_mgr.total_relative_offset);
    }
    harman_ota_mgr.buf_index = 0;
}

static void app_harman_ble_ota_payload_handle(uint8_t *p_data, uint16_t pkt_len, uint8_t app_idx)
{
    if (!ota_upgrate_status)
    {
        APP_PRINT_ERROR0("OTA is not start!");
        return;
    }

    /* 3. RECV protocol version
       4. REQ ota package header */
    if (harman_ota_mgr.state == OTA_STATE_PROTOCOL_VERSION)
    {
        harman_ota_mgr.sub_bin.package_header.protocol_version = (uint16_t)(p_data[0] | (p_data[1] << 8));

        // TODO: check protocol version
        if (harman_ota_mgr.sub_bin.package_header.protocol_version)
        {
            uint32_t msg[2] = {0};

            msg[0] = 0;
            msg[1] = OTA_FILE_PACKAGE_HEAD_SIZE;
            app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

            harman_ota_mgr.start_offset = msg[0];
            harman_ota_mgr.total_len_to_read = msg[1];

            harman_ota_mgr.state = OTA_STATE_PACKAGE_HEADER;
        }
        else
        {
            app_harman_ota_breakpoint_clear();
            app_harman_ota_exit(OTA_EXIT_REASON_UNSUPPORTED_PROTOCOL_VERSION);
        }
    }
    /* 5. RECV ota package header
       6. REQ FW TABLE */
    else if (harman_ota_mgr.state == OTA_STATE_PACKAGE_HEADER)
    {
        /* Updata package header */
        memcpy(&harman_ota_mgr.sub_bin.package_header, p_data, OTA_FILE_PACKAGE_HEAD_SIZE);
        app_harman_ota_package_header_dump();

        /* Update the whole size need OTA */
#if HARMAN_COMPRESSED_OTA_SUPPORT
        if (harman_ota_mgr.sub_bin.package_header.compressed_ota)
        {
            harman_ota_mgr.ota_total_size = harman_ota_mgr.sub_bin.package_header.ota_size;
        }
        else
#endif
        {
            harman_ota_mgr.ota_total_size = harman_ota_mgr.sub_bin.package_header.ota_size -
                                            (harman_ota_mgr.sub_bin.package_header.ota_size -
                                             harman_ota_mgr.sub_bin.package_header.fw_table_offset -
                                             harman_ota_mgr.sub_bin.package_header.fw_table_size) / 2;
        }
        // TODO: check ota package version, and REQ FW table
        if (app_harman_ota_package_ver_check() && app_harman_ota_package_pid_check())
        {
            uint32_t msg[2] = {0};

            msg[0] = harman_ota_mgr.sub_bin.package_header.fw_table_offset;
            msg[1] = harman_ota_mgr.sub_bin.package_header.fw_table_size;
            app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

            harman_ota_mgr.start_offset = msg[0];
            harman_ota_mgr.total_len_to_read = msg[1];

            if (harman_ota_mgr.sub_bin.fw_table == NULL)
            {
                p_fw_temp = malloc(harman_ota_mgr.sub_bin.package_header.fw_table_size);
                harman_ota_mgr.sub_bin.fw_table = malloc(harman_ota_mgr.sub_bin.package_header.fw_table_size);
            }

            harman_ota_mgr.cur_sub_image_relative_offset = 0;
            harman_ota_mgr.state = OTA_STATE_FW_TABLE;
        }
        else
        {
            app_harman_ota_breakpoint_clear();
            app_harman_ota_exit(OTA_EXIT_REASON_UNSUPPORTED_OTA_PACKAGE_VERSION);
        }
    }
    /* 7. RECV FW data
       8. REQ FW data*/
    else if (harman_ota_mgr.state == OTA_STATE_FW_TABLE)
    {
        if (harman_ota_mgr.sub_bin.fw_table != NULL)
        {
            if ((harman_ota_mgr.cur_sub_image_relative_offset + pkt_len) >
                harman_ota_mgr.sub_bin.package_header.fw_table_size)
            {
                pkt_len = harman_ota_mgr.sub_bin.package_header.fw_table_size -
                          harman_ota_mgr.cur_sub_image_relative_offset;
            }
            APP_PRINT_INFO2("OTA_STATE_FW_TABLE: pkt_len: 0x%x, cur_sub_image_relative_offset 0x%x",
                            pkt_len, harman_ota_mgr.cur_sub_image_relative_offset);
            memcpy(p_fw_temp + harman_ota_mgr.cur_sub_image_relative_offset, p_data, pkt_len);
            harman_ota_mgr.cur_sub_image_relative_offset += pkt_len;
            harman_ota_mgr.required_size -= pkt_len;

            // TODO: REQ next packet len and start offset
            if (harman_ota_mgr.cur_sub_image_relative_offset ==
                harman_ota_mgr.sub_bin.package_header.fw_table_size)
            {
                memcpy(harman_ota_mgr.sub_bin.fw_table, p_fw_temp,
                       harman_ota_mgr.sub_bin.package_header.fw_table_size);
                free(p_fw_temp);
                p_fw_temp = NULL;

                app_harman_ota_fw_table_dump();
                app_harman_ota_active_bank_check();

                uint32_t msg[2] = {0};

                // TODO: check breakpoint_offset
                if ((app_cfg_nv.harman_breakpoint_offset != 0) &&
                    (app_cfg_nv.cur_sub_image_index >= harman_ota_mgr.cur_sub_image_index))
                {
                    harman_ota_mgr.cur_sub_image_index = app_cfg_nv.cur_sub_image_index;
                    harman_ota_mgr.cur_sub_image_relative_offset = app_cfg_nv.cur_sub_image_relative_offset;
                    harman_ota_mgr.total_relative_offset = app_cfg_nv.harman_breakpoint_offset;
                    APP_PRINT_INFO4("OTA_STATE_FW_TABLE: cur_sub_image_index: %d, cur_sub_image_relative_offset: 0x%x, "
                                    "total_relative_offset: 0x%x, ota_total_size: 0x%x",
                                    harman_ota_mgr.cur_sub_image_index,
                                    harman_ota_mgr.cur_sub_image_relative_offset,
                                    harman_ota_mgr.total_relative_offset,
                                    harman_ota_mgr.ota_total_size);
                    /* cur image has been OTA over */
                    if (harman_ota_mgr.cur_sub_image_relative_offset ==
                        harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size)
                    {
                        uint32_t dfu_base_addr =
                            harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].download_addr;

                        app_harman_ble_ota_end_check(dfu_base_addr, app_idx);
                    }
                    else
                    {
                        msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                                 harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
                        msg[1] = harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size -
                                 harman_ota_mgr.cur_sub_image_relative_offset;
                        app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

                        harman_ota_mgr.start_offset = msg[0];
                        harman_ota_mgr.total_len_to_read = msg[1];

                        harman_ota_mgr.state = OTA_STATE_IMAGE_PAYLOAD;
                        harman_ota_mgr.buf_index = 0;
                    }
                }
                else
                {
                    // SKIP some header to get image control header
#if HARMAN_COMPRESSED_OTA_SUPPORT
                    if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
                        (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
                    {
                        msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                                 harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
                        msg[1] = COMPRESSED_OTA_SUB_IMAGE_CONTROL_HEAD_SIZE;
                    }
                    else
#endif
                    {
                        msg[0] = harman_ota_mgr.cur_sub_image_relative_offset + OTA_SUB_IMAGE_CTRL_HEADER_OFFSET +
                                 harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
                        msg[1] = OTA_SUB_IMAGE_CONTROL_HEAD_SIZE;
                    }
                    app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

                    harman_ota_mgr.start_offset = msg[0];
                    harman_ota_mgr.total_len_to_read = msg[1];

                    harman_ota_mgr.state = OTA_STATE_IMAGE_CONTROL_HEADER;
                }
            }
            else if (harman_ota_mgr.recved_packets_count >= OTA_RECVED_PACKETS_NUM_TO_BE_REPORT)
            {
                uint32_t msg[2] = {0};

                msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                         harman_ota_mgr.sub_bin.package_header.fw_table_offset;
                msg[1] = harman_ota_mgr.sub_bin.package_header.fw_table_size -
                         harman_ota_mgr.cur_sub_image_relative_offset;
                app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

                APP_PRINT_INFO1("OTA_STATE_FW_TABLE: cur_sub_image_relative_offset: 0x%x",
                                harman_ota_mgr.cur_sub_image_relative_offset);
            }
        }
        else
        {
            app_harman_ota_breakpoint_clear();
            app_harman_ota_exit(OTA_EXIT_REASON_GENERAL_ERROR);
        }
    }
    else if (harman_ota_mgr.state == OTA_STATE_IMAGE_CONTROL_HEADER)
    {
#if HARMAN_COMPRESSED_OTA_SUPPORT
        if ((harman_ota_mgr.sub_bin.package_header.compressed_ota) &&
            (harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].image_id != PRE_IMG_VP))
        {
            app_harman_ota_sub_image_control_header_check(p_data, COMPRESSED_OTA_SUB_IMAGE_CONTROL_HEAD_SIZE);
        }
        else
#endif
        {
            app_harman_ota_sub_image_control_header_check(p_data, OTA_SUB_IMAGE_CONTROL_HEAD_SIZE);
        }

        // TODO: update absolote offset to APP
        uint32_t msg[2] = {0};

        msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                 harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
        msg[1] = harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size;
        app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

        harman_ota_mgr.start_offset = msg[0];
        harman_ota_mgr.total_len_to_read = msg[1];

        harman_ota_mgr.state = OTA_STATE_IMAGE_PAYLOAD;
        harman_ota_mgr.buf_index = 0;
    }
    else if (harman_ota_mgr.state == OTA_STATE_IMAGE_PAYLOAD)
    {
        if (ota_write_buf == NULL)
        {
            return;
        }

        /* recv total data exceed sub image fw_size */
        if ((harman_ota_mgr.cur_sub_image_relative_offset + pkt_len) >
            harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size)
        {
            pkt_len = harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size -
                      harman_ota_mgr.cur_sub_image_relative_offset;
        }

        harman_ota_mgr.required_size -= pkt_len;

        /* recv data len exceed ota_write_buf, restore some recv data */
        if ((harman_ota_mgr.buf_index + pkt_len) > OTA_WRITE_BUFFER_SIZE)
        {
            buf_restored_len = pkt_len;
            pkt_len = OTA_WRITE_BUFFER_SIZE - harman_ota_mgr.buf_index;
            buf_restored_len -= pkt_len;

            if ((buf_restored_len > 0) && (ota_write_buf_restored == NULL))
            {
                ota_write_buf_restored = malloc(buf_restored_len);
                memcpy(ota_write_buf_restored, p_data + pkt_len, buf_restored_len);
            }
        }

        /* copy to ota_write_buf and wait to write to flash */
        memcpy(ota_write_buf + harman_ota_mgr.buf_index, p_data, pkt_len);
        harman_ota_mgr.buf_index += pkt_len;
        harman_ota_mgr.cur_sub_image_relative_offset += pkt_len;
        harman_ota_mgr.total_relative_offset += pkt_len;

        APP_PRINT_INFO4("OTA_STATE_IMAGE_PAYLOAD: pkt_len: 0x%x, buf_index 0x%x, cur_sub_image_relative_offset: 0x%x, fw_size: 0x%x",
                        pkt_len, harman_ota_mgr.buf_index, harman_ota_mgr.cur_sub_image_relative_offset,
                        harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size);
        /* write to flash: has 2 case */
        if ((harman_ota_mgr.buf_index == OTA_WRITE_BUFFER_SIZE) ||
            (harman_ota_mgr.cur_sub_image_relative_offset ==
             harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size))
        {
            app_harman_ble_ota_write_to_flash();

            /* if has data restored, copy to ota_write_buf, and then clear restored buf */
            if (ota_write_buf_restored != NULL)
            {
                memcpy(ota_write_buf, ota_write_buf_restored, buf_restored_len);
                harman_ota_mgr.buf_index += buf_restored_len;
                harman_ota_mgr.cur_sub_image_relative_offset += buf_restored_len;
                harman_ota_mgr.total_relative_offset += buf_restored_len;

                if (harman_ota_mgr.cur_sub_image_relative_offset ==
                    harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size)
                {
                    app_harman_ble_ota_write_to_flash();
                }

                free(ota_write_buf_restored);
                ota_write_buf_restored = NULL;
            }

            //last pkt of one sub image
            if (harman_ota_mgr.cur_sub_image_relative_offset ==
                harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size)
            {
                uint32_t dfu_base_addr =
                    harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].download_addr;

                app_harman_ble_ota_end_check(dfu_base_addr, app_idx);

                return;
            }
        }

        /* if recv 20 packets, report SUCCESS to APP */
        if (harman_ota_mgr.recved_packets_count >= OTA_RECVED_PACKETS_NUM_TO_BE_REPORT)
        {
            uint32_t msg[2] = {0};
            //prepare next block
            msg[0] = harman_ota_mgr.cur_sub_image_relative_offset +
                     harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_offset;
            msg[1] = harman_ota_mgr.sub_bin.fw_table[harman_ota_mgr.cur_sub_image_index].fw_size -
                     harman_ota_mgr.cur_sub_image_relative_offset;
            app_harman_ota_notification_report(STATUS_SUCCESS, msg[0], msg[1], app_idx);

            harman_ota_mgr.state = OTA_STATE_IMAGE_PAYLOAD;
            APP_PRINT_INFO3("OTA_STATE_IMAGE_PAYLOAD: cur_offset: 0x%x, required_size: 0x%x, cur_sub_image_relative_offset: 0x%x",
                            msg[0], msg[1], harman_ota_mgr.cur_sub_image_relative_offset);
        }
    }

    /* if recv 20 packets, report SUCCESS to APP */
    if (harman_ota_mgr.recved_packets_count >= OTA_RECVED_PACKETS_NUM_TO_BE_REPORT)
    {
        harman_ota_mgr.recved_packets_count = 0;
    }
}

static void harman_ble_ota_state_reset(void)
{
    if (p_fw_temp)
    {
        free(p_fw_temp);
        p_fw_temp = NULL;
    }
    if (ota_write_buf_restored)
    {
        free(ota_write_buf_restored);
        ota_write_buf_restored = NULL;
    }

    if (harman_ota_mgr.sub_bin.fw_table)
    {
        free(harman_ota_mgr.sub_bin.fw_table);
        harman_ota_mgr.sub_bin.fw_table = NULL;
    }

    ota_upgrate_status = 0;
    app_harman_ble_ota_package_check_timer_stop();
    memset(&harman_ota_mgr, 0, sizeof(T_HARMAN_OTA_MGR));

    APP_PRINT_INFO1("harman_ble_ota_state_reset: unused memory: %d", mem_peek());
}

static void harman_ble_state_init(void)
{
    APP_PRINT_INFO2("harman_ble_state_init: unused memory: %d, 0x%x",
                    mem_peek(), ota_write_buf);
    if (ota_write_buf)
    {
        memset(ota_write_buf, 0, OTA_WRITE_BUFFER_SIZE);
    }
    else
    {
        ota_write_buf = malloc(OTA_WRITE_BUFFER_SIZE);
    }

    harman_ble_ota_state_reset();
    harman_ota_mgr.state = OTA_STATE_PROTOCOL_VERSION;
}

void app_harman_ota_exit(T_HARMAN_OTA_EXIT_REASON exit_reason)
{
    uint16_t status_code = STATUS_SUCCESS;
    uint8_t app_idx = 0;

    if ((exit_reason == OTA_EXIT_REASON_OUTBAND_RING) ||
        (exit_reason == OTA_EXIT_REASON_CALL) ||
        (exit_reason == OTA_EXIT_REASON_LEA_CCP_CALL))
    {
        status_code = (uint16_t)(STATUS_ERROR_GENERAL_BUSINESS | BUSY_IN_CALL);
    }
    else if (exit_reason == OTA_EXIT_REASON_AUDIO_STREAMING)
    {
        status_code = (uint16_t)(STATUS_ERROR_GENERAL_BUSINESS | BUSY_IN_AUDIO_STREAMING);
    }
    else if (exit_reason == OTA_EXIT_REASON_UNSUPPORTED_OTA_PACKAGE_VERSION)
    {
        status_code = (uint16_t)(STATUS_ERROR_GENERAL_BUSINESS | UNSUPPORTED_OTA_PACKAGE_VERSION);
    }
    else if (exit_reason == OTA_EXIT_REASON_UNSUPPORTED_PROTOCOL_VERSION)
    {
        status_code = (uint16_t)(STATUS_ERROR_PROTOCOL | UNSUPPORTED_PROTOCOL_VERSION);
    }
    else if (exit_reason == OTA_EXIT_REASON_GENERAL_ERROR)
    {
        status_code = (uint16_t)(STATUS_ERROR_PROTOCOL | 0xFF);
    }
    else if (exit_reason == OTA_EXIT_REASON_SECOND_TIMEOUT)
    {
        status_code = (uint16_t)(STATUS_ERROR_PROTOCOL | SECOND_TIMEOUT);
    }
    else if (exit_reason == OTA_EXIT_REASON_BETTERY_LOW)
    {
        status_code = (uint16_t)(STATUS_ERROR_GENERAL_BUSINESS | BATTERY_LOW);
    }

    if (status_code != STATUS_SUCCESS)
    {
        harman_get_active_mobile_cmd_link(&app_idx);
        app_harman_ota_notification_report(status_code, 0, 0, app_idx);
    }

    APP_PRINT_INFO4("app_harman_ota_exit: current_state: %d, exit_reason: %d, status_code: 0x%x, app_idx: %d",
                    harman_ota_mgr.state, exit_reason, status_code, app_idx);

    harman_ble_ota_state_reset();

    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_TUYA_OTA, app_cfg_const.timer_auto_power_off);
    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_OTA);
    app_harman_ble_heartbeat_notify();
}

static void app_harman_ble_ota_start_para_set(uint8_t app_idx)
{
    uint8_t *p_buf = NULL;
    uint16_t buf_len = 0;
    uint16_t pack_len = 0;

    app_cfg_nv.ota_mtu = 0x200;
    app_cfg_nv.ota_max_size_in_one_pack = 0x100;
    app_cfg_nv.ota_continuous_packets_max_count = 20;
    app_cfg_store(&app_cfg_nv.ota_mtu, 8);

    buf_len = 2 + 4 * 3 + 2 + 2 + 1;
    p_buf = malloc(buf_len);
    p_buf[0] = 0;
    p_buf[1] = 0;
    pack_len += 2;
    pack_len += app_harman_devinfo_feature_pack(p_buf + pack_len, FEATURE_MTU, 2,
                                                (uint8_t *)&app_cfg_nv.ota_mtu);
    pack_len += app_harman_devinfo_feature_pack(p_buf + pack_len, FEATURE_OTA_DATA_MAX_SIZE_IN_ONE_PACK,
                                                2, (uint8_t *)&app_cfg_nv.ota_max_size_in_one_pack);
    pack_len += app_harman_devinfo_feature_pack(p_buf + pack_len,
                                                FEATURE_OTA_CONTINUOUS_PACKETS_MAX_COUNT, 1,
                                                &app_cfg_nv.ota_continuous_packets_max_count);
    app_harman_devinfo_notification_report(p_buf, buf_len, app_idx);

    free(p_buf);
    p_buf = NULL;
}

bool app_harman_ble_ota_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t app_idx)
{
    bool ret = true;
    uint16_t cmd_id = (uint16_t)(cmd_ptr[2] | cmd_ptr[3] << 8);

    switch (cmd_id)
    {
    case CMD_HARMAN_OTA_START:
        {
            if (app_idx < MAX_BR_LINK_NUM)
            {
                ble_set_prefer_conn_param(app_idx, 12, 12, 0, 200);
            }

            app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_OTA);
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_TUYA_OTA);
            harman_ble_state_init();

            ota_upgrate_status = 1;

            app_harman_ble_ota_start_para_set(app_idx);

            /* 2. REQ protocol version */
            app_harman_ota_notification_report(STATUS_SUCCESS, 0, 2, app_idx);
            app_harman_heartbeat_check_timer_stop();
        }
        break;

    case CMD_HARMAN_OTA_STOP:
        {
            app_harman_ota_exit(OTA_EXIT_REASON_OTA_COMMAND);
        }
        break;

    case CMD_HARMAN_OTA_PACKET:
        {
            uint32_t absolute_offset = 0;
            uint8_t pkt_count = 0;
            uint8_t pkt_index = 0;
            uint16_t pkt_len = 0;
            uint16_t data_index = HARMAN_CMD_HEADER_SIZE + HARMAN_ABSOLUTE_OFFSET_SIZE;

            app_harman_ble_ota_package_check_timer_stop();
            if (ota_upgrate_status)
            {
                app_harman_ble_ota_package_check_timer_start(OTA_PACKET_CHAEK_TIMEOUT);
            }

            harman_ota_mgr.package_missing_check_times = 0;
            ota_notify_direct = true;

            if (cmd_len >= data_index)
            {
                pkt_count = cmd_ptr[4];
                pkt_index = cmd_ptr[5];
                pkt_len = (uint16_t)(cmd_ptr[6] | cmd_ptr[7] << 8) - HARMAN_ABSOLUTE_OFFSET_SIZE;
                memcpy(&absolute_offset, &cmd_ptr[HARMAN_CMD_HEADER_SIZE], HARMAN_ABSOLUTE_OFFSET_SIZE);
            }
            APP_PRINT_INFO5("CMD_HARMAN_OTA_PACKET: pkt_count: 0x%x, pkt_index: 0x%x, "
                            "pkt_len: 0x%04x, recved_packets_count: %d, recved_offset_error_retry: %d",
                            pkt_count, pkt_index, pkt_len,
                            harman_ota_mgr.recved_packets_count,
                            harman_ota_mgr.recved_offset_error_retry);
            APP_PRINT_INFO6("CMD_HARMAN_OTA_PACKET: state: %d, absolute_offset: 0x%x, "
                            "cur_offset: 0x%x, required_size: 0x%x, "
                            "start_offset: 0x%x, total_len_to_read: 0x%x",
                            harman_ota_mgr.state, absolute_offset,
                            harman_ota_mgr.cur_offset, harman_ota_mgr.required_size,
                            harman_ota_mgr.start_offset, harman_ota_mgr.total_len_to_read);
            if (absolute_offset != harman_ota_mgr.cur_offset)
            {
                harman_ota_mgr.recved_offset_error_retry ++;
                if (harman_ota_mgr.recved_offset_error_retry < 22)
                {
                    if (((harman_ota_mgr.recved_offset_error_retry + harman_ota_mgr.recved_packets_count) ==
                         OTA_RECVED_PACKETS_NUM_TO_BE_REPORT) && (app_idx < MAX_BR_LINK_NUM))
                    {
                        app_harman_ota_notification_report(STATUS_SUCCESS, harman_ota_mgr.cur_offset,
                                                           harman_ota_mgr.required_size, app_idx);
                    }
                }
                else
                {
                    harman_ota_mgr.recved_packets_count = 0;
                    harman_ota_mgr.recved_offset_error_retry = 0;
                }
            }
            else
            {
                harman_ota_mgr.recved_packets_count ++;
                harman_ota_mgr.recved_offset_error_retry = 0;
                harman_ota_mgr.cur_offset = absolute_offset + pkt_len;
                app_harman_ble_ota_payload_handle(&cmd_ptr[data_index], pkt_len, app_idx);
            }
        }
        break;

    default:
        ret = false;
        break;
    }

    return ret;
}
void app_harman_ble_ota_power_on_handle(void)
{
    // fmc_flash_nor_read(HARMAN_OTA_SAME_CHECK_OFFSET, harman_ota_same_check,
    //                    HARMAN_OTA_SAME_CHECK_LISENCE_LEN);
    // APP_PRINT_TRACE1("app_harman_ble_ota_power_on_handle: %b ", TRACE_BINARY(4, harman_ota_same_check));
}

void app_harman_ble_ota_init(void)
{
    app_timer_reg_cb(app_harman_ble_ota_timeout_cb, &harman_ota_timer_queue_id);
}
#endif
