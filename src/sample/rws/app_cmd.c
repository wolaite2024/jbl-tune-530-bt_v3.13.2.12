/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_VENDOR_CMD_SUPPORT
#include <string.h>
#include "stdlib_corecrt.h"
#include "flash_map.h"
#include "trace.h"
#include "console.h"
#include "gap_scan.h"
#include "app_timer.h"
#include "os_mem.h"
#include "os_sched.h"
#include "ancs_client.h"
#include "gap.h"
#include "gap_br.h"
#include "audio.h"
#include "audio_probe.h"
#include "audio_volume.h"
#include "board.h"
#include "led_module.h"
#include "bt_hfp.h"
#include "bt_iap.h"
#include "btm.h"
#include "bt_bond.h"
#include "bt_types.h"
#include "bt_pbap.h"
#include "remote.h"
#include "stdlib.h"
#include "img_ctrl_ext.h"
#include "patch_header_check.h"
#include "fmc_api.h"
#include "test_mode.h"
#include "rtl876x_pinmux.h"
#include "app_cmd.h"
#include "app_main.h"
#include "app_audio_policy.h"
#include "app_transfer.h"
#include "app_report.h"
#include "app_ble_gap.h"
#include "app_tts.h"
#include "app_bt_policy_api.h"
#include "app_led.h"
#include "app_mmi.h"
#include "app_cfg.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif
#include "app_eq.h"
#include "app_iap.h"
#include "app_iap_rtk.h"
#include "app_relay.h"
#include "app_roleswap.h"
#include "app_multilink.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_linkback.h"
#include "app_bond.h"
#include "app_sensor.h"
#include "app_dlps.h"
#include "app_key_process.h"
#include "app_test.h"
#include "app_device.h"
#include "wdg.h"
#include "system_status_api.h"
#include "app_ble_rand_addr_mgr.h"
#include "app_ble_common_adv.h"
#include "app_util.h"
#include "fmc_api.h"

#if F_APP_CLI_BINARY_MP_SUPPORT
#include "mp_test.h"
#endif
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif
#if F_APP_ADC_SUPPORT
#include "app_adc.h"
#endif

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif
#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif
#if F_APP_PBAP_CMD_SUPPORT
#include "app_pbap.h"
#endif
#if F_APP_DUAL_AUDIO_EFFECT
#include "app_dual_audio_effect.h"
#endif
#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif

#include "app_audio_policy.h"

/* BBPro2 specialized feature */
#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "app_playback_trans.h"
#endif
#if F_APP_HEARABLE_SUPPORT
#include "ftl.h"
#include "app_hearable.h"
#endif

#if F_APP_CAP_TOUCH_SUPPORT
#include "app_cap_touch.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif
// end of BBPro2 specialized feature

#if F_APP_SS_SUPPORT
#include "app_ss_cmd.h"
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_2
#include "app_data_capture.h"
#endif

#if F_APP_SAIYAN_EQ_FITTING
#include "app_eq_fitting.h"
#endif

#if F_APP_LEA_REALCAST_SUPPORT
#include "app_lea_realcast.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if HARMAN_SPP_CMD_SUPPORT
#include "app_harman_license.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#endif

#if F_APP_LOG2FLASH_SUPPORT
#include "task_log_api.h"
#include "log_api.h"
#endif

/* Define application support status */
#define SNK_SUPPORT_GET_SET_LE_NAME                 1
#define SNK_SUPPORT_GET_SET_BR_NAME                 1
#define SNK_SUPPORT_GET_SET_VP_LANGUAGE             1
#define SNK_SUPPORT_GET_BATTERY_LEVEL               1
#define SNK_SUPPORT_GET_SET_KEY_REMAP               F_APP_KEY_EXTEND_FEATURE

#define SAVE_DATA_TO_FLASH_CMD_EVENET_OP_CODE_LEN   2

//for CMD_INFO_REQ
#define CMD_INFO_STATUS_VALID       0x00
#define CMD_INFO_STATUS_ERROR       0x01
#define CMD_SUPPORT_VER_CHECK_LEN   3

//for CMD_LINE_IN_CTRL
#define CFG_LINE_IN_STOP            0x00
#define CFG_LINE_IN_START           0x01

//for CMD_GET_FLASH_DATA and EVENT_REPORT_FLASH_DATA
#define START_TRANS                 0x00
#define CONTINUE_TRANS              0x01
#define SUPPORT_IMAGE_TYPE          0x02

#define TRANS_DATA_INFO             0x00
#define CONTINUE_TRANS_DATA         0x01
#define END_TRANS_DATA              0x02
#define SUPPORT_IMAGE_TYPE_INFO     0x03

#define SYSTEM_CONFIGS              0x00
#define ROM_PATCH_IMAGE             0x01
#define APP_IMAGE                   0x02
#define DSP_SYSTEM_IMAGE            0x03
#define DSP_APP_IMAGE               0x04
#define FTL_DATA                    0x05
#define ANC_IMAGE                   0x06
#define LOG_PARTITION               0x07
#define CORE_DUMP_PARTITION         0x08

#define FLASH_ALL                   0xFF
#define ALL_DUMP_IMAGE_MASK         ((0x01 << SYSTEM_CONFIGS) | (0x01 << ROM_PATCH_IMAGE) | (0x01 << APP_IMAGE) \
                                     | (0x01 << DSP_SYSTEM_IMAGE) | (0x01 << DSP_APP_IMAGE) \
                                     | (0x01 << FTL_DATA) |(0x01 << CORE_DUMP_PARTITION))

/* BBPro2 specialized feature */
//for CMD_DSP_DEBUG_SIGNAL_IN
#define CFG_SET_DSP_DEBUG_SIGNAL_IN     0x71
#define DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN 16
// end of BBPro2 specialized feature

#define CLEAR_BOND_INFO_SUCCESS     0x00
#define CLEAR_BOND_INFO_FAIL        0x01

#define DEVICE_BUD_SIDE_BOTH        2

#if F_APP_DEVICE_CMD_SUPPORT
static bool enable_auto_reject_conn_req_flag = false;
static bool enable_auto_accept_conn_req_flag = true;
static bool enable_report_attr_info_flag = false;
#endif

#define FLASH_BLOCK_SIZE            0x1000   // 4K

#if F_APP_RSSI_MONITOR_SUPPORT

typedef enum
{
    RSSI_MONITOR_DISABLE       = 0,
    RSSI_MONITOR_REPORT        = 1,
    RSSI_MONITOR_FLASH         = 2,
} T_RSSI_MONITOR_MODE;

typedef struct
{
    uint8_t rssi;
    uint8_t total_pkts;
    uint8_t err_pkts;
    uint8_t rsvd;
} T_REPORT_ITEM;

typedef struct
{
    uint8_t bd_addr[6];
    uint8_t link_type;
    uint8_t mode;
    uint16_t report_period;
    uint8_t report_interval;
    uint8_t timestamp[6];

    T_REPORT_ITEM report_data[10];
    uint16_t report_size;

    uint32_t rssi_start_addr;
    uint32_t rssi_flash_size;
    uint32_t rssi_real_size;
    uint16_t block_num;    // Flash sector number
    uint16_t temp_size;    // RSSI byte number in block
    int8_t *temp_buf;
} T_RSSI_MONITOR_MANAGER;

typedef struct
{
    uint8_t bd_addr[6];
    uint8_t link_type;
    uint8_t mode;
    uint16_t report_period;
    uint8_t report_interval;
    uint8_t timestamp[6];
    uint8_t rsvd[3];
} T_RSSI_FLASH_HEADER;

T_RSSI_MONITOR_MANAGER rssi_mgr;
T_RSSI_FLASH_HEADER rssi_flash_header;

#endif

#if F_APP_LOG2FLASH_SUPPORT
typedef struct
{
    uint32_t flash_start_addr;
    uint32_t flash_size;
    uint8_t  timestamp[6];
} T_LOG2FLASH_MANAGER;

T_LOG2FLASH_MANAGER log2flash_mgr;

#endif

static uint8_t uart_rx_seqn = 0; // uart receive sequence number
static uint8_t dlps_status = SET_DLPS_ENABLE;

static T_SRC_SUPPORT_VER_FORMAT src_support_version_br_link[MAX_BR_LINK_NUM];
static T_SRC_SUPPORT_VER_FORMAT src_support_version_le_link[MAX_BLE_LINK_NUM];
static T_SRC_SUPPORT_VER_FORMAT src_support_version_uart;

static T_OS_QUEUE cmd_parse_cback_list;
static uint16_t max_payload_len = 0;

T_FLASH_DATA flash_data;
T_DUMPFLASH_DB dump_flash_db;

// for get FW version type
typedef enum
{
    GET_PRIMARY_FW_VERSION           = 0x00,
    GET_SECONDARY_FW_VERSION         = 0x01,
    GET_PRIMARY_OTA_FW_VERSION       = 0x02,
    GET_SECONDARY_OTA_FW_VERSION     = 0x03,
    GET_PRIMARY_UI_OTA_VERSION       = 0x04,
    GET_SECONDARY_UI_OTA_VERSION     = 0x05,
} T_CMD_GET_FW_VER_TYPE;

typedef enum
{
    APP_TIMER_SWITCH_TO_HCI_DOWNLOAD_MODE,
    APP_TIMER_ENTER_DUT_FROM_SPP_WAIT_ACK,
    APP_TIMER_OTA_JIG_DELAY_POWER_OFF,
    APP_TIMER_OTA_JIG_DELAY_WDG_RESET,
    APP_TIMER_OTA_JIG_DLPS_ENABLE,
    APP_TIMER_IO_PIN_PULL_HIGH,
    APP_TIMER_STOP_PERIODIC_INQUIRY,
} T_APP_CMD_TIMER;

typedef enum
{
    IMU_SENSOR_DATA_START_REPORT       = 2,
    IMU_SENSOR_DATA_STOP_REPORT        = 3,
    IMU_SENSOR_CYWEE_DATA_START_REPORT = 4,
    IMU_SENSOR_CYWEE_DATA_STOP_REPORT  = 5,
} T_CMD_IMU_SENSOR_DATA;

typedef enum
{
    SAVE_FLASH_SUC       = 0x00,
    FLASH_ADDR_NOT_ALIGN = 0x01,
    SAVE_FLASH_FAIL      = 0x02,
} SAVE_DATA_TO_FLASH_STATUE;

typedef enum
{
    ERASE_BEFORE_WRITE = 0x00,
} SAVE_DATA_TO_FLASH_TYPE;

static uint8_t app_cmd_timer_id = 0;
static uint8_t timer_idx_switch_to_hci_mode = 0;
static uint8_t timer_idx_enter_dut_from_spp_wait_ack = 0;
static uint8_t timer_idx_io_pin_pull_high = 0;

#if F_APP_OTA_TOOLING_SUPPORT
static uint8_t timer_idx_ota_parking_power_off = 0;
static uint8_t timer_idx_ota_parking_wdg_reset = 0;
static uint8_t timer_idx_ota_parking_dlps_enable = 0;
#endif

#if F_APP_DEVICE_CMD_SUPPORT
static uint8_t timer_idx_stop_periodic_inquiry = 0;
#endif

#if F_APP_ANC_SUPPORT
static uint8_t anc_wait_wdg_reset_mode = 0;
#endif

static void app_cmd_handle_remote_cmd(uint16_t msg, void *buf, uint8_t len);

#if F_APP_DEVICE_CMD_SUPPORT
bool app_cmd_get_auto_reject_conn_req_flag(void)
{
    return enable_auto_reject_conn_req_flag;
}

bool app_cmd_get_auto_accept_conn_req_flag(void)
{
    return enable_auto_accept_conn_req_flag;
}

bool app_cmd_get_report_attr_info_flag(void)
{
    return enable_report_attr_info_flag;
}

void app_cmd_set_report_attr_info_flag(bool flag)
{
    enable_report_attr_info_flag = flag;
}
#endif

static bool app_cmd_distribute_payload(uint8_t *buf, uint16_t len)
{
    uint8_t module_type = buf[0];
    uint8_t msg_type    = buf[1];
    T_APP_CMD_PARSE_CBACK_ITEM *p_item;

    p_item = (T_APP_CMD_PARSE_CBACK_ITEM *)cmd_parse_cback_list.p_first;

    while (p_item != NULL)
    {
        if (p_item->module_type == module_type)
        {
            p_item->parse_cback(msg_type, buf + 2, len - 2);

            return true;
        }

        p_item = p_item->p_next;
    }

    return false;
}

// static void app_cmd_show_timestamp(uint8_t *timestamp, uint8_t len)
// {
//     if (timestamp != NULL && len == 6)
//     {
//         APP_PRINT_INFO6("app_cmd_show_timestamp: date %d-%d-%d, time %d:%d:%d",
//                         timestamp[0] + 1970, timestamp[1] + 1, timestamp[2],
//                         timestamp[3], timestamp[4], timestamp[5]);
//         BOOT_PRINT_INFO4("app_cmd_show_timestamp: day %d, time %d:%d:%d",
//                          timestamp[2], timestamp[3], timestamp[4], timestamp[5]);
//     }

//     return;
// }


static bool app_cmd_compose_payload(uint8_t *data, uint16_t data_len)
{
    static uint8_t cur_seq = 0;
    static uint8_t *buf = NULL;
    static uint16_t buf_offset = 0;

    uint8_t type = data[1];
    uint8_t seq = data[2];
    uint16_t total_len = data[3] + (data[4] << 8);

    data += 5;
    data_len -= 5;

    if (cur_seq != seq || data_len == 0)
    {
        cur_seq = 0;
        buf_offset = 0;

        if (buf)
        {
            free(buf);
            buf = NULL;
        }

        return CMD_SET_STATUS_PROCESS_FAIL;
    }

    if (type == PKT_TYPE_SINGLE || type == PKT_TYPE_START)
    {
        cur_seq = 0;
        buf_offset = 0;

        if (buf)
        {
            free(buf);
            buf = NULL;
        }

        buf = malloc(sizeof(uint8_t) * total_len);
    }

    memcpy_s(buf + buf_offset, (total_len - buf_offset), data, data_len);

    if (type == PKT_TYPE_SINGLE || type == PKT_TYPE_END)
    {
        app_cmd_distribute_payload(buf, total_len);
        free(buf);
        buf = NULL;
        cur_seq = 0;
        buf_offset = 0;
    }
    else
    {
        cur_seq += 1;
        buf_offset += data_len;
    }

    return CMD_SET_STATUS_COMPLETE;
}

#if F_APP_RSSI_MONITOR_SUPPORT

static void app_cmd_qol_enable(uint8_t  bd_addr[6])
{
    BOOT_PRINT_INFO3("app_cmd_qol_enable: rssi_mgr.mode %d, period %d, rssi_flash %x",
                     rssi_mgr.mode, rssi_mgr.report_period, rssi_mgr.rssi_start_addr);

    memcpy(&rssi_mgr.bd_addr, bd_addr, 6);
    if (bt_link_rssi_report(bd_addr, rssi_mgr.mode > 0, rssi_mgr.report_period) == false
        || bt_link_per_report(bd_addr, rssi_mgr.mode > 0, rssi_mgr.report_period) == false)
    {
        APP_PRINT_ERROR0("app_cmd_qol_enable: qol operate fail");
    }
    else
    {
        if (rssi_mgr.mode == RSSI_MONITOR_FLASH && rssi_mgr.block_num == 0)
        {
            memcpy(&rssi_flash_header.bd_addr, bd_addr, 6);
            memcpy(rssi_mgr.temp_buf, &rssi_flash_header, sizeof(rssi_flash_header));
        }
    }
}

static SAVE_DATA_TO_FLASH_STATUE app_cmd_write_data_to_flash(uint32_t flash_addr, int8_t *data,
                                                             uint16_t data_len)
{
    SAVE_DATA_TO_FLASH_STATUE status = SAVE_FLASH_SUC;
    uint8_t bp_level = 0;
    bool ret_flash_get = fmc_flash_nor_get_bp_lv(flash_addr, &bp_level);
    bool ret_flash_set = false;
    bool ret_flash_erase = false;
    ret_flash_set = fmc_flash_nor_set_bp_lv(flash_addr, 0);
    ret_flash_erase = fmc_flash_nor_erase(flash_addr, FMC_FLASH_NOR_ERASE_SECTOR);
    if (ret_flash_set && ret_flash_erase)
    {
        if (fmc_flash_nor_write(flash_addr, data, data_len) != true)
        {
            status = SAVE_FLASH_FAIL;
        }
    }
    BOOT_PRINT_INFO4("app_cmd_write_data_to_flash: flash_addr %02x, data_len %d, erase is %d, status %d",
                     flash_addr, data_len, ret_flash_erase, status);

    fmc_flash_nor_set_bp_lv(flash_addr, bp_level);

    return status;
}

static void app_cmd_qol_mode_set(void)
{
    BOOT_PRINT_INFO8("app_cmd_qol_mode_set: rssi_monitor mode %d, report_period %d, date %d-%d-%d, time %d:%d:%d",
                     rssi_mgr.mode, rssi_mgr.report_period,
                     rssi_mgr.timestamp[0] + 1970, rssi_mgr.timestamp[1] + 1, rssi_mgr.timestamp[2],
                     rssi_mgr.timestamp[3], rssi_mgr.timestamp[4], rssi_mgr.timestamp[5]);

    BOOT_PRINT_INFO2("app_cmd_qol_mode_set: link_type %d, interval %d", rssi_mgr.link_type,
                     rssi_mgr.report_interval);

    if (rssi_mgr.mode == RSSI_MONITOR_REPORT)
    {
        rssi_mgr.report_size = 0;
        memset(rssi_mgr.report_data, 0, 10 * sizeof(T_REPORT_ITEM));
    }
    else if (rssi_mgr.mode == RSSI_MONITOR_FLASH) // rssi to flash
    {
        if (flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0) == get_active_ota_bank_addr())
        {
            rssi_mgr.rssi_start_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_1) +
                                       RSSI_FLASH_ADDR_OFFSET;
        }
        else
        {
            rssi_mgr.rssi_start_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0) +
                                       RSSI_FLASH_ADDR_OFFSET;
        }
        rssi_mgr.rssi_flash_size = RSSI_FLASH_SIZE;
        rssi_mgr.rssi_real_size = 0;
        rssi_mgr.block_num = 0;

        if (rssi_mgr.temp_buf == NULL)
        {
            rssi_mgr.temp_buf = malloc(FLASH_BLOCK_SIZE);
        }

        rssi_flash_header.link_type = rssi_mgr.link_type;
        rssi_flash_header.mode = rssi_mgr.mode;
        rssi_flash_header.report_period = rssi_mgr.report_period;
        rssi_flash_header.report_interval = rssi_mgr.report_interval;
        memcpy(&rssi_flash_header.timestamp, &rssi_mgr.timestamp, 6);   // copy time stamp to flash header
        rssi_mgr.temp_size = sizeof(rssi_flash_header) / 4;
    }
    else
    {
        if (rssi_mgr.temp_buf != NULL)
        {
            uint32_t flash_addr = rssi_mgr.rssi_start_addr + rssi_mgr.block_num * FLASH_BLOCK_SIZE;
            app_cmd_write_data_to_flash(flash_addr, rssi_mgr.temp_buf, rssi_mgr.temp_size * 4);

            free(rssi_mgr.temp_buf);
            rssi_mgr.temp_buf = NULL;
        }
        app_cmd_qol_enable(rssi_mgr.bd_addr);
    }
}

#endif

static void app_cmd_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_IND:
        {
            //Stop periodic inquiry when connecting
#if F_APP_DEVICE_CMD_SUPPORT
            app_stop_timer(&timer_idx_stop_periodic_inquiry);
#endif
            bt_periodic_inquiry_stop();
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD,
                                                    APP_REMOTE_MSG_CMD_SYNC_MAX_PAYLOAD_LEN,
                                                    (uint8_t *)&max_payload_len, 2);
            }
        }
        break;
#endif

#if F_APP_RSSI_MONITOR_SUPPORT

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            app_cmd_qol_enable(param->acl_conn_disconn.bd_addr);
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            app_cmd_qol_enable(param->a2dp_stream_start_ind.bd_addr);
        }
        break;

    case BT_EVENT_LINK_RSSI_INFO:
        {
            APP_PRINT_TRACE3("BT_EVENT_LINK_RSSI_INFO: rssi %d, mode %d, report_size %d",
                             param->link_rssi_info.rssi, rssi_mgr.mode, rssi_mgr.report_size);

            if (rssi_mgr.mode == RSSI_MONITOR_REPORT)
            {
                rssi_mgr.report_data[rssi_mgr.report_size].rssi = param->link_rssi_info.rssi;
#if 0
                if (rssi_mgr.report_size == rssi_mgr.report_interval)
                {
                    app_report_event(CMD_PATH_SPP, EVENT_LEGACY_RSSI, 0, (uint8_t *)rssi_mgr.report_data,
                                     rssi_mgr.report_size);
                    rssi_mgr.report_size = 0;
                }
#endif
            }
            else if (rssi_mgr.mode == RSSI_MONITOR_FLASH)
            {
                APP_PRINT_INFO4("BT_EVENT_LINK_RSSI_INFO: rssi %d, mode %d, rssi_real_size %d, temp_size %d",
                                param->link_rssi_info.rssi, rssi_mgr.mode,
                                rssi_mgr.rssi_real_size, rssi_mgr.temp_size);

                if (rssi_mgr.rssi_real_size * 4 < rssi_mgr.rssi_flash_size)
                {
                    rssi_mgr.report_data[0].rssi = param->link_rssi_info.rssi;
#if 0
                    rssi_mgr.rssi_real_size++;
                    rssi_mgr.temp_size++;
                    if (rssi_mgr.temp_size == FLASH_BLOCK_SIZE)
                    {
                        uint32_t flash_addr = rssi_mgr.rssi_start_addr + rssi_mgr.block_num *
                                              FLASH_BLOCK_SIZE;
                        app_cmd_write_data_to_flash(flash_addr, rssi_mgr.temp_buf, rssi_mgr.temp_size);
                        rssi_mgr.block_num++;
                        rssi_mgr.temp_size = 0;
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_LINK_PER_INFO:
        {
            APP_PRINT_TRACE2("BT_EVENT_LINK_PER_INFO: total_pkts %d, err_pkts %d",
                             param->link_per_info.total_pkts, param->link_per_info.err_pkts);

            if (rssi_mgr.mode == RSSI_MONITOR_REPORT)
            {
                rssi_mgr.report_data[rssi_mgr.report_size].total_pkts = (uint8_t)param->link_per_info.total_pkts;
                rssi_mgr.report_data[rssi_mgr.report_size].err_pkts = (uint8_t)param->link_per_info.err_pkts;
                rssi_mgr.report_size++;
                if (rssi_mgr.report_size == rssi_mgr.report_interval)
                {
                    app_report_event(CMD_PATH_SPP, EVENT_LEGACY_RSSI, 0, (uint8_t *)rssi_mgr.report_data,
                                     rssi_mgr.report_size * sizeof(T_REPORT_ITEM));
                    rssi_mgr.report_size = 0;
                }
            }
            else if (rssi_mgr.mode == RSSI_MONITOR_FLASH)
            {
                if (rssi_mgr.rssi_real_size * 4 < rssi_mgr.rssi_flash_size)
                {
                    rssi_mgr.report_data[0].total_pkts = (uint8_t)param->link_per_info.total_pkts;
                    rssi_mgr.report_data[0].err_pkts = (uint8_t)param->link_per_info.err_pkts;
                    rssi_mgr.report_data[0].rsvd = 0;
                    memcpy(rssi_mgr.temp_buf + rssi_mgr.temp_size * 4, &rssi_mgr.report_data[0], sizeof(T_REPORT_ITEM));
                    BOOT_PRINT_INFO4("BT_EVENT_LINK_PER_INFO: rssi %d, total_pkts %d, err_pkts %d, rssi_real_size %d",
                                     (int8_t)rssi_mgr.report_data[0].rssi, rssi_mgr.report_data[0].total_pkts,
                                     rssi_mgr.report_data[0].err_pkts, rssi_mgr.rssi_real_size);

                    rssi_mgr.rssi_real_size++;
                    rssi_mgr.temp_size++;
                    if (rssi_mgr.temp_size == FLASH_BLOCK_SIZE / 4)
                    {
                        uint32_t flash_addr = rssi_mgr.rssi_start_addr + rssi_mgr.block_num * FLASH_BLOCK_SIZE;
                        app_cmd_write_data_to_flash(flash_addr, rssi_mgr.temp_buf, rssi_mgr.temp_size * 4);
                        rssi_mgr.block_num++;
                        rssi_mgr.temp_size = 0;
                    }
                }
                else
                {
                    APP_PRINT_ERROR0("BT_EVENT_LINK_PER_INFO: RSSI Flash size not enough");
                }
            }
        }
        break;
#endif

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_cmd_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_cmd_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_SWITCH_TO_HCI_DOWNLOAD_MODE:
        {
            app_stop_timer(&timer_idx_switch_to_hci_mode);
            sys_hall_set_hci_download_mode(true);
            set_hci_mode_flag(true);
            chip_reset(RESET_ALL_EXCEPT_AON);
        }
        break;

    case APP_TIMER_ENTER_DUT_FROM_SPP_WAIT_ACK:
        {
            app_stop_timer(&timer_idx_enter_dut_from_spp_wait_ack);
            app_mmi_handle_action(MMI_ENTER_DUT_FROM_SPP);
        }
        break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case APP_TIMER_OTA_JIG_DELAY_POWER_OFF: asdfa
        {
            app_stop_timer(&timer_idx_ota_parking_power_off);

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_relay_async_single(APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_PARKING);
            }
            else
#endif
            {
                app_cmd_ota_tooling_parking();
            }
        }
        break;

    case APP_TIMER_OTA_JIG_DELAY_WDG_RESET:
        {
            app_stop_timer(&timer_idx_ota_parking_wdg_reset);
            chip_reset(RESET_ALL);
        }
        break;

    case APP_TIMER_OTA_JIG_DLPS_ENABLE:
        {
            app_stop_timer(&timer_idx_ota_parking_dlps_enable);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_OTA_TOOLING_PARK);
        }
        break;
#endif

    case APP_TIMER_IO_PIN_PULL_HIGH:
        {
            app_stop_timer(&timer_idx_io_pin_pull_high);

            uint8_t pin_num = (uint8_t)param;

            Pad_Config(pin_num, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        }
        break;

#if F_APP_DEVICE_CMD_SUPPORT
    case APP_TIMER_STOP_PERIODIC_INQUIRY:
        {
            app_stop_timer(&timer_idx_stop_periodic_inquiry);
            bt_periodic_inquiry_stop();
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
uint16_t app_cmd_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    uint8_t eq_ctrl_by_src = app_db.eq_ctrl_by_src;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC:
        {
            msg_ptr = (uint8_t *)&eq_ctrl_by_src;
            payload_len = 1;
            skip = false;
        }
        break;

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_CMD, payload_len, msg_ptr, skip, total);
}

static void app_cmd_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_CMD_GET_FW_VERSION:
    case APP_REMOTE_MSG_CMD_REPORT_FW_VERSION:
    case APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION:
    case APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION:
    case APP_REMOTE_MSG_CMD_GET_UI_OTA_VERSION:
    case APP_REMOTE_MSG_CMD_REPORT_UI_OTA_VERSION:
        {
            app_cmd_handle_remote_cmd(msg_type, buf, len);
        }
        break;

    case APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_cmd_update_eq_ctrl(*((uint8_t *)buf), false);
            }
        }
        break;

    /* BBPro2 specialized feature*/
    case APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_probe_dsp_send(buf, len);
            }
        }
        break;
    // end of BBPro2 specialized feature

    case APP_REMOTE_MSG_SYNC_RAW_PAYLOAD:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_cmd_compose_payload(buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_SYNC_MAX_PAYLOAD_LEN:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                max_payload_len = *((uint16_t *)buf);
            }
        }
        break;

    default:
        break;
    }
}
#endif

void app_cmd_init(void)
{
    bt_mgr_cback_register(app_cmd_bt_cback);
    app_timer_reg_cb(app_cmd_timeout_cb, &app_cmd_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_cmd_relay_cback, app_cmd_parse_cback,
                             APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_TOTAL);
#endif
}

bool app_cmd_cback_register(P_APP_CMD_PARSE_CBACK parse_cb, T_APP_CMD_MODULE_TYPE module_type)
{
    T_APP_CMD_PARSE_CBACK_ITEM *p_item;

    p_item = (T_APP_CMD_PARSE_CBACK_ITEM *)cmd_parse_cback_list.p_first;

    while (p_item != NULL)
    {
        if (p_item->parse_cback == parse_cb)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = (T_APP_CMD_PARSE_CBACK_ITEM *)malloc(sizeof(T_APP_CMD_PARSE_CBACK_ITEM));

    if (p_item != NULL)
    {
        p_item->parse_cback = parse_cb;
        p_item->module_type = module_type;
        os_queue_in(&cmd_parse_cback_list, p_item);

        return true;
    }

    return false;
}

void app_cmd_set_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len)
{
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;
    uint8_t        i;

    for (i = 0; i < MAX_BR_LINK_NUM; i ++)
    {
        br_link = &app_db.br_link[i];

        if (br_link->cmd_set_enable == true)
        {
            if (br_link->connected_profile & SPP_PROFILE_MASK)
            {
                app_report_event(CMD_PATH_SPP, event_id, i, buf, len);
            }

            if (br_link->connected_profile & IAP_PROFILE_MASK)
            {
                app_report_event(CMD_PATH_IAP, event_id, i, buf, len);
            }
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        le_link = &app_db.le_link[i];

        if (le_link->state == LE_LINK_STATE_CONNECTED)
        {
            if (le_link->cmd_set_enable == true)
            {
                app_report_event(CMD_PATH_LE, event_id, i, buf, len);
            }
        }
    }
}

#if F_APP_FLASH_DUMP_SUPPORT

#if 0
static void app_cmd_read_flash(uint32_t start_addr, uint8_t cmd_path, uint8_t app_idx)
{
    uint32_t start_addr_tmp;
    uint16_t data_send_len;

    data_send_len = 0x200;// in case assert fail
    start_addr_tmp = start_addr;

    if (cmd_path == CMD_PATH_SPP)
    {
        APP_PRINT_TRACE1("app_cmd_read_flash: rfc_frame_size %d", app_db.br_link[app_idx].rfc_frame_size);
        if (app_db.br_link[app_idx].rfc_frame_size - 12 < data_send_len)
        {
            data_send_len = app_db.br_link[app_idx].rfc_frame_size - 12;
        }
    }
    else if (cmd_path == CMD_PATH_IAP)
    {
#if F_APP_IAP_RTK_SUPPORT && F_APP_IAP_SUPPORT
        T_APP_IAP_HDL app_iap_hdl = NULL;
        app_iap_hdl = app_iap_search_by_addr(app_db.br_link[app_idx].bd_addr);
        uint16_t frame_size = app_iap_get_frame_size(app_iap_hdl);

        APP_PRINT_TRACE1("app_read_flash: iap frame_size %d", frame_size);
        if (frame_size - 12 < data_send_len)
        {
            data_send_len = frame_size - 12;
        }
#endif
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        APP_PRINT_TRACE1("app_read_flash: mtu_size %d", app_db.le_link[app_idx].mtu_size);
        if (app_db.le_link[app_idx].mtu_size - 15 < data_send_len)
        {
            data_send_len = app_db.le_link[app_idx].mtu_size - 15;
        }
    }

    uint8_t *data = malloc(data_send_len + 6);

    if (data != NULL)
    {
        if (start_addr + data_send_len >= flash_data.flash_data_start_addr + flash_data.flash_data_size)
        {
            data_send_len = flash_data.flash_data_start_addr + flash_data.flash_data_size - start_addr;
            data[0] = END_TRANS_DATA;
        }
        else
        {
            data[0] = CONTINUE_TRANS_DATA;
        }

        data[1] = flash_data.flash_data_type;
        data[2] = (uint8_t)(start_addr_tmp);
        data[3] = (uint8_t)(start_addr_tmp >> 8);
        data[4] = (uint8_t)(start_addr_tmp >> 16);
        data[5] = (uint8_t)(start_addr_tmp >> 24);

        if (fmc_flash_nor_read(start_addr_tmp, &data[6], data_send_len))// read flash data
        {
            app_report_event(cmd_path, EVENT_REPORT_FLASH_DATA, app_idx, data, data_send_len + 6);
        }

        flash_data.flash_data_start_addr_tmp += data_send_len;
        free(data);
    }
}
#endif

static bool app_cmd_dump_flash_init(uint32_t start_addr, uint32_t dump_size)
{
    if ((dump_size & 0xFFF) || (start_addr < dump_flash_db.start_addr)
        || (start_addr + dump_size > dump_flash_db.start_addr + dump_flash_db.dump_size))
    {
        APP_PRINT_ERROR4("app_cmd_dump_flash_init failed: flash addr 0x%x, size 0x%x <==> dump addr 0x%x, size 0x%x",
                         dump_flash_db.start_addr, dump_flash_db.dump_size, start_addr, dump_size);
        return false;
    }
    BOOT_PRINT_INFO4("app_cmd_dump_flash_init: flash addr 0x%x, size 0x%x <==> dump addr 0x%x, size 0x%x",
                     dump_flash_db.start_addr, dump_flash_db.dump_size, start_addr, dump_size);

    // start from stack patch addr
    dump_flash_db.start_addr = start_addr;
    dump_flash_db.dump_size = dump_size;
    dump_flash_db.block_addr = dump_flash_db.start_addr;
    dump_flash_db.block_size = FLASH_BLOCK_SIZE;

    if (dump_flash_db.block_data == NULL)
    {
        dump_flash_db.block_data = malloc(dump_flash_db.block_size);
    }

    return true;
}

static void app_dump_flash_cmd_ack_handle(uint8_t cmd_path, uint8_t app_idx)
{
    uint32_t dump_size;
    uint32_t remain_size;
    uint8_t *data;
    uint8_t crc_result[2];
    extern T_DUMPFLASH_DB dump_flash_db;

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    if (!dump_flash_db.block_used)
    {
        if (!fmc_flash_nor_read(dump_flash_db.block_addr, dump_flash_db.block_data,
                                dump_flash_db.block_size))
        {
            APP_PRINT_ERROR0("app_dump_flash_cmd_ack_handle read flash failed!!");
            return;
        }
    }

//    BOOT_PRINT_INFO3("app_dump_flash_cmd_ack_handle %p, block_used 0x%x, block_size 0x%x",
//                     dump_flash_db.block_addr, dump_flash_db.block_used, dump_flash_db.block_size);
    dump_size = dump_flash_db.max_transmit_size > 512 ? 512 : dump_flash_db.max_transmit_size;
    remain_size = dump_flash_db.block_size - dump_flash_db.block_used;
    if (remain_size == 0)
    {
        return;
    }

    if (dump_size > remain_size)
    {
        dump_size = remain_size;
    }

    // BOOT_PRINT_WARN1("app_dump_flash_cmd_ack_handle: Unused memory %d", mem_peek());
    data = malloc(dump_size + 5);
    if (data != NULL)
    {
        data[0] = 0x01; //Frame_type
        LE_UINT32_TO_ARRAY(&data[1], dump_size);
        memcpy(&data[5], dump_flash_db.block_data + dump_flash_db.block_used, dump_size);
        app_report_event(cmd_path, EVENT_DUMP_FLASH, app_idx, data, dump_size + 5);

        dump_flash_db.block_used += dump_size;
        if (dump_flash_db.block_used == dump_flash_db.block_size)
        {
            uint32_t i;
            uint16_t crc16_checksum = 0;
            uint16_t *data = (uint16_t *)dump_flash_db.block_data;

            for (i = 0; i < dump_flash_db.block_size / 2; ++i)
            {
                crc16_checksum = crc16_checksum ^ (*data);
                ++data;
            }

            crc_result[0] = (uint8_t)crc16_checksum;
            crc_result[1] = (uint8_t)(crc16_checksum >> 8);
            app_report_event(cmd_path, EVENT_FLASH_BUFFER_CHECK, app_idx, crc_result, 2);
            //BOOT_PRINT_INFO2("app_dump_flash_cmd_ack_handle %p, crc 0x%x", dump_flash_db.block_addr, crc16_checksum);
        }

        // BOOT_PRINT_INFO2("app_dump_flash_cmd_ack_handle %p, block_used 0x%x", dump_flash_db.block_addr, dump_flash_db.block_used);
        free(data);
    }

    return;
}
#endif

#if 0
//T_FLASH_DATA initialization
static void app_cmd_flash_data_set_param(uint8_t flash_type, uint8_t cmd_path, uint8_t app_idx)
{
    flash_data.flash_data_type = flash_type;
    flash_data.flash_data_start_addr = 0x800000;
    flash_data.flash_data_size = 0x00;

    switch (flash_type)
    {
    case FLASH_ALL:
        {
            flash_data.flash_data_start_addr = 0x800000;
            flash_data.flash_data_size = 0x100000;
        }
        break;

    case SYSTEM_CONFIGS:
        {
            flash_data.flash_data_start_addr = flash_partition_addr_get(PARTITION_FLASH_OCCD);
            flash_data.flash_data_size = flash_partition_size_get(PARTITION_FLASH_OCCD) & 0x00FFFFFF;
        }
        break;

    case ROM_PATCH_IMAGE:
        {
            flash_data.flash_data_start_addr = flash_cur_bank_img_header_addr_get(FLASH_IMG_MCUPATCH);
            flash_data.flash_data_size = get_bank_size_by_img_id(IMG_MCUPATCH);
        }
        break;

    case APP_IMAGE:
        {
            flash_data.flash_data_start_addr = flash_cur_bank_img_header_addr_get(FLASH_IMG_MCUAPP);
            flash_data.flash_data_size = get_bank_size_by_img_id(IMG_MCUAPP);
        }
        break;

    case DSP_SYSTEM_IMAGE:
        {
            flash_data.flash_data_start_addr = flash_cur_bank_img_header_addr_get(FLASH_IMG_DSPSYSTEM);
            flash_data.flash_data_size = get_bank_size_by_img_id(IMG_DSPSYSTEM);
        }
        break;

    case DSP_APP_IMAGE:
        {
            flash_data.flash_data_start_addr = flash_cur_bank_img_header_addr_get(FLASH_IMG_DSPAPP);
            flash_data.flash_data_size = get_bank_size_by_img_id(IMG_DSPAPP);
        }
        break;

    case FTL_DATA:
        {
            flash_data.flash_data_start_addr = flash_partition_addr_get(PARTITION_FLASH_FTL);
            flash_data.flash_data_size = flash_partition_size_get(PARTITION_FLASH_FTL) & 0x00FFFFFF;
        }
        break;

    case ANC_IMAGE:
        {
            flash_data.flash_data_start_addr = flash_cur_bank_img_header_addr_get(FLASH_IMG_ANC);
            flash_data.flash_data_size = get_bank_size_by_img_id(IMG_ANC);
        }
        break;

    case LOG_PARTITION:
        {
            //add later;
        }
        break;

    case CORE_DUMP_PARTITION:
        {
            flash_data.flash_data_start_addr = flash_partition_addr_get(PARTITION_FLASH_HARDFAULT_RECORD);
            flash_data.flash_data_size = flash_partition_size_get(PARTITION_FLASH_HARDFAULT_RECORD);
        }
        break;

    default:
        break;
    }

    flash_data.flash_data_start_addr_tmp = flash_data.flash_data_start_addr;

    //report TRANS_DATA_INFO param
    uint8_t paras[10];

    paras[0] = TRANS_DATA_INFO;
    paras[1] = flash_data.flash_data_type;

    paras[2] = (uint8_t)(flash_data.flash_data_size);
    paras[3] = (uint8_t)(flash_data.flash_data_size >> 8);
    paras[4] = (uint8_t)(flash_data.flash_data_size >> 16);
    paras[5] = (uint8_t)(flash_data.flash_data_size >> 24);

    paras[6] = (uint8_t)(flash_data.flash_data_start_addr);
    paras[7] = (uint8_t)(flash_data.flash_data_start_addr >> 8);
    paras[8] = (uint8_t)(flash_data.flash_data_start_addr >> 16);
    paras[9] = (uint8_t)(flash_data.flash_data_start_addr >> 24);

    app_report_event(cmd_path, EVENT_REPORT_FLASH_DATA, app_idx, paras, sizeof(paras));
}
#endif

T_SNK_CAPABILITY app_cmd_get_system_capability(void)
{
    T_SNK_CAPABILITY snk_capability;

    memset(&snk_capability, 0, sizeof(T_SNK_CAPABILITY));
    snk_capability.snk_support_get_set_le_name = SNK_SUPPORT_GET_SET_LE_NAME;
    snk_capability.snk_support_get_set_br_name = SNK_SUPPORT_GET_SET_BR_NAME;
    snk_capability.snk_support_get_set_vp_language = SNK_SUPPORT_GET_SET_VP_LANGUAGE;
    snk_capability.snk_support_get_battery_info = SNK_SUPPORT_GET_BATTERY_LEVEL;
    snk_capability.snk_support_ota = true;
#if F_APP_TTS_SUPPORT
    snk_capability.snk_support_tts = app_cfg_const.tts_support;
#else
    snk_capability.snk_support_tts = 0;
#endif

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        snk_capability.snk_support_change_channel = 1;
        snk_capability.snk_support_get_set_rws_state = 1;
    }
    snk_capability.snk_support_get_set_apt_state = app_cfg_const.normal_apt_support;
    snk_capability.snk_support_get_set_eq_state = true;
    snk_capability.snk_support_get_set_vad_state = false;

#if F_APP_ANC_SUPPORT
    snk_capability.snk_support_get_set_anc_state = (app_anc_get_activated_scenario_cnt() > 0);
    snk_capability.snk_support_get_set_listening_mode_cycle = true;
    snk_capability.snk_support_anc_scenario_choose = (app_anc_get_activated_scenario_cnt() > 1);
#endif
#if F_APP_APT_SUPPORT
    snk_capability.snk_support_get_set_llapt_state = (app_cfg_const.llapt_support &&
                                                      (app_apt_get_llapt_activated_scenario_cnt() > 0));
#endif
    snk_capability.snk_support_ansc = false;
    snk_capability.snk_support_vibrator = false;
    snk_capability.snk_support_gaming_mode = true;

#if F_APP_KEY_EXTEND_FEATURE
    snk_capability.snk_support_key_remap = SNK_SUPPORT_GET_SET_KEY_REMAP;
    snk_capability.snk_support_reset_key_remap = SNK_SUPPORT_GET_SET_KEY_REMAP;
    snk_capability.snk_support_reset_key_map_by_bud = SNK_SUPPORT_GET_SET_KEY_REMAP;

#if F_APP_RWS_KEY_SUPPORT
    snk_capability.snk_support_rws_key_remap = app_key_is_rws_key_setting();
#endif
#endif

    snk_capability.snk_support_gaming_mode_eq = eq_utils_num_get(SPK_SW_EQ, GAMING_MODE) >= 1;
    snk_capability.snk_support_anc_eq = eq_utils_num_get(SPK_SW_EQ, ANC_MODE) >= 1;
    snk_capability.snk_support_multilink_support = app_cfg_const.enable_multi_link;
    snk_capability.snk_support_phone_set_anc_eq = true;
    snk_capability.snk_support_new_report_bud_status_flow = true;

#if (F_APP_USER_EQ_SUPPORT == 1)
    if (app_cfg_const.user_eq_spk_eq_num != 0 || app_cfg_const.user_eq_mic_eq_num != 0)
    {
        snk_capability.snk_support_user_eq = true;
    }
#endif

#if F_APP_SENSOR_SUPPORT
    if (app_cfg_const.sensor_support)
    {
        snk_capability.snk_support_ear_detection = true;
    }
#endif

#if F_APP_DURIAN_SUPPORT
    snk_capability.snk_support_avp = true;
    snk_capability.snk_support_multilink_support = true;
    snk_capability.snk_support_get_set_serial_id = true;
#else
    snk_capability.snk_support_multilink_support = app_cfg_const.enable_multi_link;
#endif

    snk_capability.snk_support_new_report_listening_status = true;

#if F_APP_APT_SUPPORT
    if ((app_cfg_const.normal_apt_support) && (eq_utils_num_get(MIC_SW_EQ, APT_MODE) != 0))
    {
        snk_capability.snk_support_apt_eq = true;

        /* RHE related features */
#if F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT
        snk_capability.snk_support_apt_eq_adjust_separate = app_cfg_const.rws_apt_eq_adjust_separate;
#endif
    }

#if F_APP_BRIGHTNESS_SUPPORT
    snk_capability.snk_support_llapt_brightness = (app_apt_brightness_get_support_bitmap() != 0x0);
#endif
#if F_APP_LLAPT_SCENARIO_CHOOSE_SUPPORT
    snk_capability.snk_support_llapt_scenario_choose = (app_apt_get_llapt_activated_scenario_cnt() > 1);
#endif
#if F_APP_POWER_ON_DELAY_APPLY_APT_SUPPORT
    snk_capability.snk_support_power_on_delay_apply_apt_on = true;
#endif
#if (F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT == 0)
    snk_capability.snk_support_apt_volume_force_adjust_sync = true;
#endif
#endif

#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    snk_capability.snk_support_notification_vol_adjustment = true;
#endif
// end of RHE related feature

    /* BBPro2 specialized feature */
#if (F_APP_LOCAL_PLAYBACK_SUPPORT == 1)
    snk_capability.snk_support_local_playback = app_cfg_const.local_playback_support;
#endif
#if F_APP_HEARABLE_SUPPORT
    if (app_cfg_const.enable_ha)
    {
        snk_capability.snk_support_HA = 1;
    }
#endif
// end of BBPro2 specialized feature
#if F_APP_SPP_CAPTURE_DSP_DATA_2
    snk_capability.snk_support_spp_2 = 1;
    snk_capability.snk_support_3bin_scenario = 1;
#endif
#if F_APP_SUPPORT_ANC_APT_COEXIST
    if (app_cfg_const.normal_apt_support)
    {
        snk_capability.snk_support_anc_apt_coexist = 1;
        snk_capability.snk_support_anc_apt_scenario_separate = 1;
    }
#endif
#if F_APP_SENSOR_MEMS_SUPPORT
    if (app_cfg_const.mems_support)
    {
        snk_capability.snk_support_spatial_audio = 1;
    }
#endif
#if F_APP_VOICE_SPK_EQ_SUPPORT
    if (eq_utils_num_get(SPK_SW_EQ, VOICE_SPK_MODE) != 0)
    {
        snk_capability.snk_support_voice_eq = 1;
    }
#endif

#if F_APP_AUDIO_VOCIE_SPK_EQ_INDEPENDENT_CFG
    snk_capability.snk_support_spk_eq_independent_cfg = 1;
#endif

#if F_APP_AUDIO_VOCIE_SPK_EQ_COMPENSATION_CFG
    snk_capability.snk_support_spk_eq_compensation_cfg = 1;
#endif

    snk_capability.snk_support_ui_ota_version = 1;

#if F_APP_SUPPORT_ANC_APT_APPLY_BURN
    snk_capability.snk_support_anc_llapt_apply_burn = 1;
#endif

#if F_APP_FLASH_DUMP_SUPPORT
    snk_capability.snk_support_dump_ftl_from_flash = 1;
#endif

#if F_APP_LOG2FLASH_SUPPORT
    snk_capability.snk_support_dump_log_from_flash = 1;
#endif

#if F_APP_RSSI_MONITOR_SUPPORT
    snk_capability.snk_support_rssi_monitor_and_dump = 1;
#endif

    return snk_capability;
}

static void app_cmd_get_fw_version(uint8_t *p_data)
{
    uint8_t temp_buff[13];
    T_IMG_HEADER_FORMAT *p_app_header = (T_IMG_HEADER_FORMAT *)flash_cur_bank_img_header_addr_get(
                                            FLASH_IMG_MCUAPP);
    T_IMG_HEADER_FORMAT *p_patch_header = (T_IMG_HEADER_FORMAT *)flash_cur_bank_img_header_addr_get(
                                              FLASH_IMG_MCUPATCH);
    T_PATCH_IMG_VER_FORMAT *p_patch_img_ver = (T_PATCH_IMG_VER_FORMAT *) & (p_patch_header->git_ver);

    T_IMG_HEADER_FORMAT *p_app_ui_header = (T_IMG_HEADER_FORMAT *)flash_cur_bank_img_header_addr_get(
                                               FLASH_IMG_MCUCONFIG);
    T_APP_UI_IMG_VER_FORMAT *p_app_ui_ver = (T_APP_UI_IMG_VER_FORMAT *) & (p_app_ui_header->git_ver);

    temp_buff[0] = p_app_header->git_ver.sub_version._version_major;
    temp_buff[1] = p_app_header->git_ver.sub_version._version_minor;
    temp_buff[2] = p_app_header->git_ver.sub_version._version_revision;

    // currently 5 bits, must be 0
    temp_buff[3] = 0; // p_app_header->git_ver.sub_version._version_reserve >> 8;
    temp_buff[4] = p_app_header->git_ver.sub_version._version_reserve;

    temp_buff[5] = p_patch_img_ver->ver_major;
    temp_buff[6] = p_patch_img_ver->ver_minor;
    temp_buff[7] = p_patch_img_ver->ver_revision >> 8;
    temp_buff[8] = p_patch_img_ver->ver_revision;

    temp_buff[9] = p_app_ui_ver->ver_reserved;
    temp_buff[10] = p_app_ui_ver->ver_revision;
    temp_buff[11] = p_app_ui_ver->ver_minor;
    temp_buff[12] = p_app_ui_ver->ver_major;

    memcpy((void *)p_data, (void *)&temp_buff, 13);
}

static void app_cmd_get_ui_ota_version(uint8_t *p_data)
{
    uint8_t temp_buff[4] = {0};

    T_IMG_HEADER_FORMAT *p_app_ui_header = (T_IMG_HEADER_FORMAT *)flash_cur_bank_img_header_addr_get(
                                               FLASH_IMG_MCUCONFIG);
    T_APP_UI_IMG_VER_FORMAT *p_app_ui_ver = (T_APP_UI_IMG_VER_FORMAT *) & (p_app_ui_header->git_ver);

    temp_buff[0] = p_app_ui_ver->ver_reserved;
    temp_buff[1] = p_app_ui_ver->ver_revision;
    temp_buff[2] = p_app_ui_ver->ver_minor;
    temp_buff[3] = p_app_ui_ver->ver_major;

    memcpy((void *)p_data, (void *)&temp_buff, sizeof(temp_buff));
}

#if F_APP_OTA_TOOLING_SUPPORT
void app_cmd_ota_tooling_parking(void)
{
    APP_PRINT_INFO2("app_cmd_ota_tooling_parking %d, %d", app_cfg_nv.ota_tooling_start,
                    app_db.device_state);

    app_dlps_disable(APP_DLPS_ENTER_CHECK_OTA_TOOLING_PARK);
    app_start_timer(&timer_idx_ota_parking_dlps_enable, "ota_parking_dlps_enable",
                    app_cmd_timer_id, APP_TIMER_OTA_JIG_DLPS_ENABLE, NULL, false,
                    3500);

    // clear phone record
    app_bond_clear_non_rws_keys();

    // clear dongle info
    app_db.jig_subcmd = 0;
    app_db.jig_dongle_id = 0;
    app_db.ota_tooling_start = 0;

    // remove OTA power on flag
    if (app_cfg_nv.ota_tooling_start)
    {
        app_cfg_nv.ota_tooling_start = 0;
        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
    }

    // power off
    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        app_db.power_off_cause = POWER_OFF_CAUSE_OTA_TOOL;
        app_mmi_handle_action(MMI_DEV_POWER_OFF);
        app_start_timer(&timer_idx_ota_parking_wdg_reset, "ota_jig_delay_wdg_reset",
                        app_cmd_timer_id, APP_TIMER_OTA_JIG_DELAY_WDG_RESET, NULL, false,
                        3000);
    }
}

void app_cmd_stop_ota_parking_power_off(void)
{
    // avoid timeout to clear ota dongle info
    app_stop_timer(&timer_idx_ota_parking_power_off);

    // avoid timeout to reset system when receive new dongle command
    app_stop_timer(&timer_idx_ota_parking_wdg_reset);

    // clear set dlps
    app_cfg_nv.need_set_lps_mode = 0;
}
#endif

bool app_cmd_relay_command_set(uint16_t cmd_id, uint8_t *cmd_ptr, uint16_t cmd_len,
                               T_APP_MODULE_TYPE module_type, uint8_t relay_cmd_id, bool sync)
{
    uint8_t error_code = 0;

    uint8_t *relay_cmd;
    uint16_t total_len;

    relay_cmd = NULL;
    total_len = 5 + cmd_len;
    relay_cmd = (uint8_t *)malloc(total_len);

    if (relay_cmd == NULL)
    {
        error_code = 1;
        goto SKIP;
    }

    /* bypass_cmd             *
     * byte [0,1]  : cmd_id   *
     * byte [2,3]  : cmd_len  *
     * byte [4]    : cmd_path *
     * byte [5-N]  : cmd      */

    relay_cmd[0] = (uint8_t)cmd_id;
    relay_cmd[1] = (uint8_t)(cmd_id >> 8);
    relay_cmd[2] = (uint8_t)cmd_len;
    relay_cmd[3] = (uint8_t)(cmd_len >> 8);


    memcpy(&relay_cmd[5], &cmd_ptr[0], cmd_len);

    if (sync)
    {
        relay_cmd[4] = CMD_PATH_RWS_SYNC;

        if (app_relay_sync_single_with_raw_msg(module_type, relay_cmd_id, relay_cmd, total_len,
                                               REMOTE_TIMER_HIGH_PRECISION, 0, false) == false)
        {
            error_code = 2;
            free(relay_cmd);
            goto SKIP;
        }
    }
    else
    {
        relay_cmd[4] = CMD_PATH_RWS_ASYNC;

        if (app_relay_async_single_with_raw_msg(module_type, relay_cmd_id, relay_cmd, total_len) == false)
        {
            error_code = 3;
            free(relay_cmd);
            goto SKIP;
        }
    }

    free(relay_cmd);
    return true;

SKIP:
    APP_PRINT_INFO2("app_cmd_relay_command_set fail cmd_id = %x, error = %d", cmd_id, error_code);
    return false;
}

bool app_cmd_relay_event(uint16_t event_id, uint8_t *event_ptr, uint16_t event_len,
                         T_APP_MODULE_TYPE module_type, uint8_t relay_event_id)
{
    uint8_t error_code = 0;

    uint16_t total_len;
    uint8_t *report_event;

    total_len = APP_CMD_RELAY_HEADER_LEN + event_len;

    report_event = (uint8_t *)malloc(total_len);

    if (report_event == NULL)
    {
        error_code = 1;
        goto SKIP;
    }

    /* report
     * byte [0,1] : event_id    *
     * byte [2,3] : report_len  *
     * byte [4-N] : report      */

    report_event[0] = (uint8_t)event_id;
    report_event[1] = (uint8_t)(event_id >> 8);
    report_event[2] = (uint8_t)event_len;
    report_event[3] = (uint8_t)(event_len >> 8);

    memcpy(&report_event[4], &event_ptr[0], event_len);

    if (app_relay_async_single_with_raw_msg(module_type, relay_event_id,
                                            report_event, total_len) == false)
    {
        error_code = 2;
        free(report_event);
        goto SKIP;
    }

    free(report_event);
    return true;

SKIP:
    APP_PRINT_INFO2("app_cmd_relay_event fail cmd_id = %x, error = %d", event_id,
                    error_code);
    return false;
}

static void app_cmd_handle_remote_cmd(uint16_t msg, void *buf, uint8_t len)
{
    switch (msg)
    {
    case APP_REMOTE_MSG_CMD_GET_UI_OTA_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t data[7] = {0};
                data[0] = p_info[0];    // cmd path
                data[1] = p_info[1];    // app idx

                data[2] = GET_SECONDARY_UI_OTA_VERSION;

                app_cmd_get_ui_ota_version(&data[3]);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_REPORT_UI_OTA_VERSION,
                                                    data, sizeof(data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_REPORT_UI_OTA_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t report_data[5] = {0};

            memcpy(&report_data[0], &p_info[2], sizeof(report_data));

            // primary return secondary's fw version
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event(p_info[0], EVENT_FW_VERSION, p_info[1], report_data, sizeof(report_data));
            }
        }
        break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case APP_REMOTE_MSG_CMD_GET_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;

            // get secondary's fw version by app_cmd_get_fw_version()
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t data[15] = {0};
                data[0] = p_info[0];    // cmd path
                data[1] = p_info[1];    // app idx

                app_cmd_get_fw_version(&data[2]);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_REPORT_FW_VERSION,
                                                    data, sizeof(data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_REPORT_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t report_data[9];

            memcpy(&report_data[0], &p_info[2], 9);

            // primary return secondary's fw version
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event(p_info[0], EVENT_FW_VERSION, p_info[1], report_data, sizeof(report_data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;

            // get secondary's fw version by app_ota_get_brief_img_version_for_dongle()
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t data[IMG_LEN_FOR_DONGLE + 3] = {0};
                data[0] = p_info[0];    // cmd path
                data[1] = p_info[1];    // app idx
                data[2] = GET_SECONDARY_OTA_FW_VERSION;
                app_ota_get_brief_img_version_for_dongle(&data[3]);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION,
                                                    data, sizeof(data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t report_data[IMG_LEN_FOR_DONGLE + 1];

            memcpy(&report_data[0], &p_info[2], IMG_LEN_FOR_DONGLE + 1);

            // primary return secondary's fw version
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event(p_info[0], EVENT_FW_VERSION, p_info[1], report_data, sizeof(report_data));
            }
        }
        break;

#endif

    default:
        break;
    }
}

bool app_cmd_get_tool_connect_status(void)
{
    bool tool_connect_status = false;
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;
    uint8_t        i;

    for (i = 0; i < MAX_BR_LINK_NUM; i ++)
    {
        br_link = &app_db.br_link[i];

        if (br_link->connected_profile & (SPP_PROFILE_MASK | IAP_PROFILE_MASK))
        {
            if (br_link->cmd_set_enable == true)
            {
                tool_connect_status = true;
            }
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        le_link = &app_db.le_link[i];
        if (le_link->state == LE_LINK_STATE_CONNECTED)
        {
            if (le_link->cmd_set_enable == true)
            {
                tool_connect_status = true;
            }
        }
    }

    return tool_connect_status;
}

static void app_cmd_update_max_payload_len_ctrl(uint8_t path, uint8_t app_idx)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (path == CMD_PATH_SPP)
        {
            max_payload_len = app_db.br_link[app_idx].rfc_frame_size;
        }
        else if (path == CMD_PATH_LE)
        {
            max_payload_len = app_db.le_link[app_idx].mtu_size - ATT_HEADER_LEN;
        }
#if F_APP_IAP_SUPPORT
        else if (path == CMD_PATH_IAP)
        {
            T_APP_IAP_HDL app_iap_hdl = NULL;

            app_iap_hdl = app_iap_search_by_addr(app_db.br_link[app_idx].bd_addr);
            max_payload_len = app_iap_get_frame_size(app_iap_hdl);
        }
#endif
#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD,
                                                APP_REMOTE_MSG_CMD_SYNC_MAX_PAYLOAD_LEN,
                                                (uint8_t *)&max_payload_len, 2);
        }
#endif
        APP_PRINT_TRACE1("app_cmd_update_max_payload_len_ctrl: max_payload_len %d", max_payload_len);
    }
}

uint16_t app_cmd_get_max_payload_len(void)
{
    return max_payload_len;
}

void app_cmd_update_eq_ctrl(uint8_t value, bool need_relay)
{
    if (app_db.eq_ctrl_by_src != value)
    {
        app_db.eq_ctrl_by_src = value;

        if (need_relay)
        {
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD,
                                                APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC,
                                                &value, sizeof(uint8_t));
        }

#if F_APP_USER_EQ_SUPPORT
        if (value == false && remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_eq_reset_unsaved_user_eq();
        }
#endif
    }

    APP_PRINT_TRACE1("app_cmd_update_eq_ctrl: connect_status %d", value);
}

T_SRC_SUPPORT_VER_FORMAT *app_cmd_get_src_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = NULL;

    if (cmd_path == CMD_PATH_UART)
    {
        version = &src_support_version_uart;
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        version = &src_support_version_le_link[app_idx];
    }
    else if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
    {
        version = &src_support_version_br_link[app_idx];
    }

    return version;
}

bool app_cmd_check_src_cmd_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

    if (version)
    {
        if (version->cmd_set_ver_major > CMD_SET_VER_MAJOR ||
            (version->cmd_set_ver_major == CMD_SET_VER_MINOR &&
             version->cmd_set_ver_minor >= CMD_SET_VER_MINOR))
        {
            // SRC support version is new, which is valid.
            return true;
        }
        else if (version->cmd_set_ver_major == 0 && version->cmd_set_ver_minor == 0)
        {
            // SRC never update support version
            return true;
        }
    }

    return false;
}

bool app_cmd_check_src_eq_spec_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

    if (version)
    {
        uint8_t eq_spec_minor = EQ_SPEC_VER_MINOR;

        if (version->eq_spec_ver_major > EQ_SPEC_VER_MAJOR ||
            (version->eq_spec_ver_major == EQ_SPEC_VER_MAJOR && version->eq_spec_ver_minor >= eq_spec_minor))
        {
            // SRC support version is new, which is valid.
            return true;
        }
        else if (version->eq_spec_ver_major == 0 && version->eq_spec_ver_minor == 0)
        {
            // SRC never update support version
            return true;
        }
    }

    return false;
}

void app_cmd_bt_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                           uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
#if F_APP_RSSI_MONITOR_SUPPORT
    case CMD_GET_LEGACY_RSSI:
        {
            rssi_mgr.link_type = cmd_ptr[2];
            rssi_mgr.mode = cmd_ptr[3];
            rssi_mgr.report_period = (uint16_t)(cmd_ptr[4] | (cmd_ptr[5] << 8));
            rssi_mgr.report_interval = cmd_ptr[6];
            memcpy(&rssi_mgr.timestamp, &cmd_ptr[7], 6);

            app_cmd_qol_mode_set();

            if (rssi_mgr.link_type == LEGACY_RSSI)
            {
                uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

                APP_PRINT_TRACE2("app_cmd_bt_cmd_handle: a2dp_idx %d, streaming_fg %d",
                                 active_a2dp_idx, app_db.br_link[active_a2dp_idx].streaming_fg);

                if (rssi_mgr.mode > RSSI_MONITOR_DISABLE && app_db.br_link[active_a2dp_idx].streaming_fg)
                {
                    app_cmd_qol_enable(app_db.br_link[active_a2dp_idx].bd_addr);
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif
    case CMD_LE_GET_ADDR:
        {
            uint8_t rand_addr[6] = {0};
            app_ble_rand_addr_get(rand_addr);
            if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(cmd_path, EVENT_LE_PUBLIC_ADDR, app_idx, rand_addr, 6);
            }
        }
        break;

    case CMD_BT_GET_LOCAL_ADDR:
        {
            uint8_t temp_buff[6];
            memcpy(&temp_buff[0], app_cfg_nv.bud_local_addr, 6);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(CMD_PATH_UART, EVENT_LOCAL_ADDR, 0, temp_buff, sizeof(temp_buff));
        }
        break;

#if 0
    case CMD_BT_READ_PAIRED_RECORD:
        {
            T_APP_LINK_RECORD *link_record;
            uint8_t bond_num = app_bond_b2s_num_get();
            uint8_t temp_buff[bond_num * 8 + 1];

            memset(temp_buff, 0, sizeof(temp_buff));
            link_record = malloc(sizeof(T_APP_LINK_RECORD) * bond_num);
            if (link_record != NULL)
            {
                bond_num = app_bond_get_b2s_link_record(link_record, bond_num);
                temp_buff[0] = bond_num;

                for (uint8_t i = 0; i < bond_num; i++)
                {
                    temp_buff[i * 8 + 1] = link_record[i].priority;
                    temp_buff[i * 8 + 2] = link_record[i].bond_flag;
                    memcpy(&temp_buff[i * 8 + 3], &(link_record[i].bd_addr), 6);
                }
                free(link_record);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_REPLY_PAIRED_RECORD, app_idx, temp_buff, sizeof(temp_buff));
            }
        }
        break;

    case CMD_BT_CREATE_CONNECTION:
        {
            //Stop periodic inquiry when connecting
#if F_APP_DEVICE_CMD_SUPPORT
            app_stop_timer(&timer_idx_stop_periodic_inquiry);
#endif
            bt_periodic_inquiry_stop();

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_bt_policy_default_connect(&cmd_ptr[3], cmd_ptr[2], false);
        }
        break;

    case CMD_BT_DISCONNECT:
        {
            T_APP_BR_LINK *p_link;
            uint8_t bd_addr[6];

            memcpy(bd_addr, &cmd_ptr[2], 6);
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                app_bt_policy_disconnect(p_link->bd_addr, cmd_ptr[8]);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_BT_READ_LINK_INFO:
        {
            uint8_t app_index = cmd_ptr[2];

            if ((app_index >= MAX_BR_LINK_NUM) || !app_link_check_b2s_link_by_id(app_index))
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t event_buff[9];

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                event_buff[0] = app_index;
                event_buff[1] = app_db.br_link[app_index].connected_profile;
                event_buff[2] = 0;
                memcpy(&event_buff[3], app_db.br_link[app_index].bd_addr, 6);
                app_report_event(CMD_PATH_UART, EVENT_REPLY_LINK_INFO, 0, &event_buff[0], sizeof(event_buff));
            }
        }
        break;

    case CMD_BT_GET_REMOTE_NAME:
        {
            bt_remote_name_req(&cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_BT_IAP_LAUNCH_APP:
        {
#if F_APP_IAP_RTK_SUPPORT && F_APP_IAP_SUPPORT
            uint8_t app_index = cmd_ptr[2];

            if (app_index < MAX_BR_LINK_NUM)
            {
                T_APP_IAP_HDL app_iap_hdl = NULL;
                app_iap_hdl = app_iap_search_by_addr(app_db.br_link[app_index].bd_addr);

                if ((app_db.br_link[app_index].connected_profile & IAP_PROFILE_MASK)
                    && (app_iap_is_authened(app_iap_hdl)))
                {
                    app_iap_rtk_launch(app_db.br_link[app_index].bd_addr, BT_IAP_APP_LAUNCH_WITH_USER_ALERT);
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

#endif
        }
        break;

    case CMD_BT_SEND_AT_CMD:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_hfp_send_vnd_at_cmd_req(app_db.br_link[app_index].bd_addr, (char *)&cmd_ptr[3]) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_BT_HFP_DIAL_WITH_NUMBER:
        {
            uint8_t app_index = app_hfp_get_active_idx();
            char *number = (char *)&cmd_ptr[2];

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                if ((app_db.br_link[app_index].app_hf_state == APP_HF_STATE_CONNECTED) &&
                    (app_hfp_get_call_status() == APP_CALL_IDLE))
                {
                    if (bt_hfp_dial_with_number_req(app_db.br_link[app_index].bd_addr, (const char *)number) == false)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_BD_ADDR:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_GET_BD_ADDR, app_idx, app_db.factory_addr,
                             sizeof(app_db.factory_addr));
        }
        break;

    case CMD_LE_START_ADVERTISING:
        {
            if (cmd_ptr[1] <= 31)
            {
                //app_ble_gap_start_advertising(APP_ADV_PURPOSE_VENDOR, cmd_ptr[0], cmd_ptr[1], &cmd_ptr[2]);
                //fixme later
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_LE_START_SCAN:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            le_scan_start();
        }
        break;

    case CMD_LE_STOP_SCAN:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            le_scan_stop();
        }
        break;




    case CMD_BT_BOND_INFO_CLEAR:
        {
            T_APP_BR_LINK *p_link = NULL;
            uint8_t *bd_addr = (uint8_t *)(&cmd_ptr[3]);
            uint8_t temp_buff = CLEAR_BOND_INFO_FAIL;

            p_link = app_link_find_br_link(bd_addr);
            if (p_link == NULL)
            {
                if (cmd_ptr[2] == 0) //clear BR/EDR bond info
                {
                    if (bt_bond_delete(bd_addr) == true)
                    {
                        temp_buff = CLEAR_BOND_INFO_SUCCESS;
                    }
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(CMD_PATH_UART, EVENT_BT_BOND_INFO_CLEAR, 0, &temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_GET_NUM_OF_CONNECTION:
        {
            uint8_t event_data = app_multi_get_acl_connect_num();

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_NUM_OF_CONNECTION, app_idx, &event_data,
                             sizeof(event_data));
        }
        break;

    case CMD_SUPPORT_MULTILINK:
        {
#if F_APP_DURIAN_SUPPORT
            app_durian_cfg_multi_on();
#endif
            app_cfg_const.enable_multi_link = 1;
            app_cfg_const.max_legacy_multilink_devices = 2;
            app_db.b2s_connected_num_max = app_cfg_const.max_legacy_multilink_devices;
            app_mmi_handle_action(MMI_DEV_ENTER_PAIRING_MODE);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_DEVICE_CMD_SUPPORT
static void app_cmd_device_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                      uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_INQUIRY:
        {
            if ((cmd_ptr[2] == START_INQUIRY) && (cmd_ptr[3] <= MAX_INQUIRY_TIME))
            {
                if (cmd_ptr[4] == NORMAL_INQUIRY)
                {
                    if (gap_br_start_inquiry(false, cmd_ptr[3]) != GAP_CAUSE_SUCCESS)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    if (bt_periodic_inquiry_start(false, 3, 2, 1))
                    {
                        app_start_timer(&timer_idx_stop_periodic_inquiry, "stop_periodic_inquiry",
                                        app_cmd_timer_id, APP_TIMER_STOP_PERIODIC_INQUIRY, app_idx, false,
                                        cmd_ptr[3] * 1280);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
            }
            else if (cmd_ptr[2] == STOP_INQUIRY)
            {
                if (cmd_ptr[4] == NORMAL_INQUIRY)
                {
                    if (gap_br_stop_inquiry() != GAP_CAUSE_SUCCESS)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                else
                {
                    app_stop_timer(&timer_idx_stop_periodic_inquiry);

                    if (bt_periodic_inquiry_stop() != true)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_SERVICES_SEARCH:
        {
            uint8_t bd_addr[6];

            memcpy(bd_addr, &cmd_ptr[3], 6);

            if (cmd_ptr[2] == START_SERVICES_SEARCH)
            {
                T_LINKBACK_SEARCH_PARAM search_param;
                if (linkback_profile_search_start(bd_addr, cmd_ptr[9], false, &search_param) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == STOP_SERVICES_SEARCH)
            {
                if (gap_br_stop_sdp_discov(bd_addr) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_SET_PAIRING_CONTROL:
        {
            if (cmd_ptr[2] == FORWARD_ACL_ACF_REQ_TO_HOST)
            {
                enable_auto_accept_conn_req_flag = false;
                enable_auto_reject_conn_req_flag = false;
            }
            else if (cmd_ptr[2] == ENABLE_AUTO_ACCEPT_ACL_ACF_REQ)
            {
                enable_auto_accept_conn_req_flag = true;
                enable_auto_reject_conn_req_flag = false;
            }
            else if (cmd_ptr[2] == ENABLE_AUTO_REJECT_ACL_ACF_REQ)
            {
                enable_auto_accept_conn_req_flag = false;
                enable_auto_reject_conn_req_flag = true;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                if (gap_br_cfg_auto_accept_acl(enable_auto_accept_conn_req_flag) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_SET_PIN_CODE:
        {
            if ((cmd_ptr[2] >= 0x01) && (cmd_ptr[2] <= 0x08))
            {
                app_cfg_nv.pin_code_size = cmd_ptr[2];
                memcpy(app_cfg_nv.pin_code, &cmd_ptr[3], cmd_ptr[2]);

                //save to flash after set pin_code
                app_cfg_store(&app_cfg_nv.mic_channel, 12);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_PIN_CODE:
        {
            uint8_t pin_code_size = app_cfg_nv.pin_code_size;
            uint8_t temp_buff[pin_code_size + 1];

            temp_buff[0] = pin_code_size;
            memcpy(&temp_buff[1], app_cfg_nv.pin_code, pin_code_size);

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_PIN_CODE, app_idx, temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_PAIR_REPLY:
        {
            uint8_t *bd_addr = app_test_get_acl_conn_ind_bd_addr();

            if (cmd_ptr[2] == ACCEPT_PAIRING_REQ)
            {
                if (gap_br_accept_acl_conn_req(bd_addr, GAP_BR_LINK_ROLE_SLAVE) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == REJECT_PAIRING_REQ)
            {
                if (gap_br_reject_acl_conn_req(bd_addr, GAP_ACL_REJECT_LIMITED_RESOURCE) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_SSP_CONFIRMATION:
        {
            uint8_t *bd_addr = app_test_get_user_confirmation_bd_addr();

            if (cmd_ptr[2] == ACCEPT_PAIRING_REQ)
            {
                if (gap_br_user_cfm_req_cfm(bd_addr, GAP_CFM_CAUSE_ACCEPT) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == REJECT_PAIRING_REQ)
            {
                if (gap_br_user_cfm_req_cfm(bd_addr, GAP_CFM_CAUSE_REJECT) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_CONNECTED_DEV_ID:
        {
            uint8_t b2s_connected_num = 0;
            uint8_t b2s_connected_id[MAX_BR_LINK_NUM] = {0};

            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    b2s_connected_id[b2s_connected_num] = i;
                    b2s_connected_num = b2s_connected_num + 1;
                }
            }

            uint8_t temp_buff[b2s_connected_num + 1];

            temp_buff[0] = b2s_connected_num;
            memcpy(&temp_buff[1], &b2s_connected_id[0], b2s_connected_num);

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_CONNECTED_DEV_ID, app_idx, temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_GET_REMOTE_DEV_ATTR_INFO:
        {
            uint8_t app_index = cmd_ptr[2];
            T_LINKBACK_SEARCH_PARAM search_param;
            uint8_t bd_addr[6];
            uint8_t prof = 0;

            memcpy(&bd_addr[0], app_db.br_link[app_index].bd_addr, 6);
            if (cmd_ptr[3] == GET_AVRCP_ATTR_INFO)
            {
                prof = AVRCP_PROFILE_MASK;
            }
            else if (cmd_ptr[3] == GET_PBAP_ATTR_INFO)
            {
                prof = PBAP_PROFILE_MASK;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                if (linkback_profile_search_start(bd_addr, prof, false, &search_param) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
                else
                {
                    app_cmd_set_report_attr_info_flag(true);
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    default:
        break;
    }
}
#endif

static void app_cmd_le_name_set(uint8_t *device_name, uint8_t name_len)
{
    memcpy(app_cfg_nv.device_name_le, device_name, name_len + 1);
    app_cfg_store(app_cfg_nv.device_name_le, 40);
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, name_len, device_name);
    app_ble_common_adv_update_scan_rsp_data();
}

static void app_cmd_legacy_name_set(uint8_t *device_name, uint8_t name_len)
{
    memcpy(app_cfg_nv.device_name_legacy, device_name, name_len + 1);
    app_cfg_store(app_cfg_nv.device_name_legacy, 40);
    bt_local_name_set(device_name, name_len);
}

#if F_APP_LOG2FLASH_SUPPORT
void app_log2flash_init(uint32_t offset, uint32_t flash_size)
{
    uint32_t start_addr;

    if ((offset & 0xFFF) || (offset < LOG2FLASH_ADDR_OFFSET))
    {
        offset = LOG2FLASH_ADDR_OFFSET;
    }

    if ((flash_size & 0xFFF) || (flash_size > LOG2FLASH_SIZE))
    {
        flash_size = LOG2FLASH_SIZE;
    }

    if (flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0) == get_active_ota_bank_addr())
    {
        start_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_1);
    }
    else
    {
        start_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
    }

    // start from stack patch addr
    log2flash_mgr.flash_start_addr = start_addr + offset;
    log2flash_mgr.flash_size = flash_size;

    APP_PRINT_INFO2("app_log2flash_init start addr: 0x%x, size: 0x%x",
                    log2flash_mgr.flash_start_addr, log2flash_mgr.flash_size);

    fmc_flash_nor_set_bp_lv(log2flash_mgr.flash_start_addr, 0);
    log_task_init(log2flash_mgr.flash_start_addr, log2flash_mgr.flash_size, 4000, RAM_TYPE_ITCM1, true);
    enable_log_to_flash(false);
}

#endif

static void app_cmd_general_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                       uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_MMI:
        {
#if F_APP_APT_SUPPORT
            if ((cmd_ptr[3] == MMI_AUDIO_APT) &&
                (app_apt_is_apt_on_state(app_db.current_listening_state) == false) &&
                (app_apt_open_condition_check() == false))
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }
#endif

#if F_APP_SENSOR_SUPPORT
            if (cmd_ptr[3] == MMI_LIGHT_SENSOR_ON_OFF)
            {
                /*not allow config sensor when b2b is not conn*/
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }
            }
#endif

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (cmd_ptr[3] == MMI_ENTER_DUT_FROM_SPP)
            {
                app_start_timer(&timer_idx_enter_dut_from_spp_wait_ack, "enter_dut_from_spp_wait_ack",
                                app_cmd_timer_id, APP_TIMER_ENTER_DUT_FROM_SPP_WAIT_ACK, app_idx, false,
                                100);
                break;
            }

            if (cmd_ptr[3] == MMI_DEV_POWER_OFF)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_CMD_SET;
            }

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                //single mode
                app_mmi_handle_action(cmd_ptr[3]);
            }
            else
            {
                if (cmd_ptr[3] == MMI_DEV_FACTORY_RESET)
                {
                    app_mmi_handle_action(cmd_ptr[3]);
                }
                else if ((cmd_ptr[3] == MMI_DEV_SPK_MUTE) || (cmd_ptr[3] == MMI_DEV_SPK_UNMUTE))
                {
                    bool ret = (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ? false : true;

                    app_relay_sync_single(APP_MODULE_TYPE_MMI, cmd_ptr[3], REMOTE_TIMER_HIGH_PRECISION, 0, ret);
                }
                else if (cmd_ptr[3] == MMI_DEV_POWER_OFF)
                {
#if F_APP_ERWS_SUPPORT
                    app_roleswap_poweroff_handle(false);
#else
                    app_mmi_handle_action(MMI_DEV_POWER_OFF);
#endif
                }
                else
                {
                    app_relay_async_single(APP_MODULE_TYPE_MMI, cmd_ptr[3]);
                    app_mmi_handle_action(cmd_ptr[3]);
                }
            }
        }
        break;

    case CMD_INFO_REQ:
        {
            uint8_t info_type = cmd_ptr[2];
            uint8_t report_to_phone_len = 6;
            uint8_t buf[report_to_phone_len];

            if (!app_db.eq_ctrl_by_src &&
                app_cmd_check_src_eq_spec_version(cmd_path, app_idx))
            {
                app_cmd_update_eq_ctrl(true, true);
            }

            if (info_type == CMD_SET_INFO_TYPE_VERSION)
            {
                app_cmd_update_max_payload_len_ctrl(cmd_path, app_idx);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if (cmd_len > CMD_SUPPORT_VER_CHECK_LEN) // update SRC support version
                {
                    T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

                    memcpy(version->version, &cmd_ptr[3], 4);

                    if (!app_cmd_check_src_eq_spec_version(cmd_path, app_idx))
                    {
                        app_cmd_update_eq_ctrl(false, true);
                    }

                    if (!app_cmd_check_src_cmd_version(cmd_path, app_idx))
                    {
                        // version not support
                    }
                }

                buf[0] = info_type;
                buf[1] = CMD_INFO_STATUS_VALID;

                buf[2] = CMD_SET_VER_MAJOR;
                buf[3] = CMD_SET_VER_MINOR;
                buf[4] = EQ_SPEC_VER_MAJOR;
                buf[5] = EQ_SPEC_VER_MINOR;

                if (report_to_phone_len > 0)
                {
                    app_report_event(cmd_path, EVENT_INFO_RSP, app_idx, buf, report_to_phone_len);
                }
            }
            else if (info_type == CMD_INFO_GET_CAPABILITY)
            {
                T_SNK_CAPABILITY current_snk_cap;
                uint8_t *evt_param = NULL;

                evt_param = malloc(sizeof(T_SNK_CAPABILITY) + 2);

                if (evt_param)
                {
                    evt_param[0] = info_type;
                    evt_param[1] = CMD_INFO_STATUS_VALID;
                    current_snk_cap = app_cmd_get_system_capability();
                    memcpy(&evt_param[2], (uint8_t *)&current_snk_cap, sizeof(T_SNK_CAPABILITY));

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_INFO_RSP, app_idx, evt_param, sizeof(T_SNK_CAPABILITY) + 2);
                    free(evt_param);
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_SET_CFG:
        {
            uint8_t relay_idx;

            if ((cmd_ptr[2] == CFG_SET_LE_NAME) || (cmd_ptr[2] == CFG_SET_LEGACY_NAME))
            {
                if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP) ||
                    (cmd_path == CMD_PATH_LE) || (cmd_path == CMD_PATH_UART))
                {
                    uint8_t name_len;
                    uint8_t device_name[40];

                    name_len = cmd_ptr[3];

                    if (name_len >= GAP_DEVICE_NAME_LEN)
                    {
                        name_len = GAP_DEVICE_NAME_LEN - 1;
                    }
                    memcpy(device_name, &cmd_ptr[4], name_len);
                    device_name[name_len] = 0;

                    if (cmd_ptr[2] == CFG_SET_LE_NAME)
                    {
                        app_cmd_le_name_set(device_name, name_len);
                        relay_idx = APP_REMOTE_MSG_LE_NAME_SYNC;

                        if (app_cfg_const.le_name_sync_to_legacy_name)
                        {
                            app_cmd_legacy_name_set(device_name, name_len);
                        }
                    }
                    else if (cmd_ptr[2] == CFG_SET_LEGACY_NAME)
                    {
                        app_cmd_legacy_name_set(device_name, name_len);
                        relay_idx = APP_REMOTE_MSG_DEVICE_NAME_SYNC;

                        if (app_cfg_const.le_name_sync_to_legacy_name)
                        {
                            app_cmd_le_name_set(device_name, name_len);
                        }
                    }

                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
                    {
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, relay_idx, &cmd_ptr[4], name_len);
                    }
                }

            }
            else if ((cmd_ptr[2] == CFG_SET_AUDIO_LATENCY) || (cmd_ptr[2] == CFG_SET_SUPPORT_CODEC))
            {
            }
#if F_APP_DURIAN_SUPPORT
            else if ((cmd_ptr[2] == CFG_SET_SERIAL_ID) || (cmd_ptr[2] == CFG_SET_SERIAL_SINGLE_ID) ||
                     (cmd_ptr[2] == CFG_SET_DISABLE_REPORT_AVP_ID))
            {
                app_durian_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
                break;
            }
#endif
            else if (cmd_ptr[2] == CFG_SET_HFP_REPORT_BATT)
            {
                app_db.hfp_report_batt = cmd_ptr[3];
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_CFG_SETTING:
        {
            uint8_t get_type = cmd_ptr[2];

            if (get_type >= CFG_GET_MAX)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if ((get_type == CFG_GET_LE_NAME) || (get_type == CFG_GET_LEGACY_NAME) ||
                (get_type == CFG_GET_IC_NAME))
            {
                uint8_t p_name[40 + 2];
                uint8_t *p_buf = NULL;
                uint8_t name_len = 0;

                if (get_type == CFG_GET_LEGACY_NAME)
                {
                    name_len = strlen((const char *)app_cfg_nv.device_name_legacy);
                    p_buf = app_cfg_nv.device_name_legacy;
                }
                else if (get_type == CFG_GET_LE_NAME)
                {
                    name_len = strlen((const char *)app_cfg_nv.device_name_le);
                    p_buf = app_cfg_nv.device_name_le;
                }
                else
                {
                    name_len = strlen((const char *)IC_NAME);
                    p_buf = IC_NAME;
                }

                if (p_buf != NULL)
                {
                    p_name[0] = get_type;
                    p_name[1] = name_len;
                    memcpy(&p_name[2], p_buf, name_len);
                }

                app_report_event(cmd_path, EVENT_REPORT_CFG_TYPE, app_idx, &p_name[0], name_len + 2);
            }
#if F_APP_DURIAN_SUPPORT
            else if ((get_type == CFG_GET_AVP_ID) || (get_type == CFG_GET_AVP_LEFT_SINGLE_ID) ||
                     (get_type == CFG_GET_AVP_RIGHT_SINGLE_ID))
            {
                app_durian_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
            }
#else
            else if (get_type == CFG_GET_COMPANY_ID_AND_MODEL_ID)
            {
                uint8_t buf[5];

                buf[0] = get_type;

                // use little endian method
                buf[1] = app_cfg_const.company_id[0];
                buf[2] = app_cfg_const.company_id[1];
                buf[3] = app_cfg_const.uuid[0];
                buf[4] = app_cfg_const.uuid[1];

                app_report_event(cmd_path, EVENT_REPORT_CFG_TYPE, app_idx, buf, sizeof(buf));
            }
#endif
        }
        break;

    case CMD_INDICATION:
        {
            if (cmd_ptr[2] == 0)//report MAC address of smart phone
            {
                memcpy(app_db.le_link[app_idx].bd_addr, &cmd_ptr[3], 6);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_LANGUAGE_GET:
        {
            uint8_t buf[2];

            buf[0] = app_cfg_nv.voice_prompt_language;
            buf[1] = voice_prompt_supported_languages_get();

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_LANGUAGE_REPORT, app_idx, buf, 2);
        }
        break;

    case CMD_LANGUAGE_SET:
        {
            if (voice_prompt_supported_languages_get() & BIT(cmd_ptr[2]))
            {
                if (voice_prompt_language_set((T_VOICE_PROMPT_LANGUAGE_ID)cmd_ptr[2]) == true)
                {
                    bool need_to_save_to_flash = false;

                    if (cmd_ptr[2] != app_cfg_nv.voice_prompt_language)
                    {
                        need_to_save_to_flash = true;
                    }

                    app_cfg_nv.voice_prompt_language = cmd_ptr[2] ;
                    app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_VP_LANGUAGE);

                    if (need_to_save_to_flash)
                    {
                        app_cfg_store(&app_cfg_nv.voice_prompt_language, 1);
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_STATUS:
        {
            /* This command is used to get specific RWS info.
            * Only one status can be reported at one time.
            * Use CMD_GET_BUD_INFO instead to get complete RWS bud info.
            */
#if F_APP_DURIAN_SUPPORT
            if ((cmd_ptr[2] >= GET_STATUS_AVP_RWS_VER) && (cmd_ptr[2] <= GET_STATUS_AVP_ANC_SETTINGS))
            {
                app_durian_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
                break;
            }
#endif
            uint8_t buf[3];
            uint8_t report_len = 2;

            buf[0] = cmd_ptr[2]; //status_index

            switch (cmd_ptr[2])
            {
            case GET_STATUS_RWS_STATE:
                {
                    buf[1] = app_db.remote_session_state;
                }
                break;

            case GET_STATUS_RWS_CHANNEL:
                {
#if F_APP_ERWS_SUPPORT
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        buf[1] = (app_cfg_nv.spk_channel << 4) | app_db.remote_spk_channel;
                    }
                    else
#endif
                    {
                        buf[1] = (app_cfg_const.solo_speaker_channel << 4);
                    }
                }
                break;

            case GET_STATUS_BATTERY_STATUS:
                {
                    buf[1] = app_db.local_batt_level;
                    buf[2] = app_db.remote_batt_level;
                    report_len = 3;
                }
                break;

#if F_APP_APT_SUPPORT
            case GET_STATUS_APT_STATUS:
                {
                    buf[0] = (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT) ? GET_STATUS_LLAPT_STATUS :
                             GET_STATUS_APT_STATUS;

                    if ((app_apt_is_apt_on_state(app_db.current_listening_state))
#if F_APP_SUPPORT_ANC_APT_COEXIST
                        || (app_listening_is_anc_apt_on_state(app_db.current_listening_state))
#endif
                       )
                    {
                        buf[1] = 1;
                    }
                    else
                    {
                        buf[1] = 0;
                    }
                }
                break;
#endif
            case GET_STATUS_APP_STATE:
                {
                    buf[1] = app_bt_policy_get_state();
                }
                break;

            case GET_STATUS_BUD_ROLE:
                {
                    buf[1] = app_cfg_const.bud_role;
                }
                break;

            case GET_STATUS_VOLUME:
                {
                    T_AUDIO_STREAM_TYPE volume_type;

                    if (app_hfp_get_call_status() != APP_CALL_IDLE)
                    {
                        volume_type = AUDIO_STREAM_TYPE_VOICE;
                    }
                    else
                    {
                        volume_type = AUDIO_STREAM_TYPE_PLAYBACK;
                    }

                    buf[1] = audio_volume_out_get(volume_type);
                    buf[2] = audio_volume_out_max_get(volume_type);
                    report_len = 3;
                }
                break;

            case GET_STATUS_RWS_DEFAULT_CHANNEL:
                {
                    buf[1] = (app_cfg_const.couple_speaker_channel << 4) | app_db.remote_default_channel;
                }
                break;

            case GET_STATUS_RWS_BUD_SIDE:
                {
                    buf[1] = app_cfg_const.bud_side;
                }
                break;

#if F_APP_APT_SUPPORT
            case GET_STATUS_RWS_SYNC_APT_VOL:
                {
                    buf[1] = RWS_SYNC_APT_VOLUME;
                }
                break;
#endif

#if F_APP_DEVICE_CMD_SUPPORT
            case GET_STATUS_FACTORY_RESET_STATUS:
                {
                    buf[1] = app_cfg_nv.factory_reset_done;
                }
                break;

            case GET_STATUS_AUTO_REJECT_CONN_REQ_STATUS:
                {
                    if (enable_auto_reject_conn_req_flag)
                    {
                        buf[1] = ENABLE_AUTO_REJECT_ACL_ACF_REQ;
                    }
                    else if (enable_auto_accept_conn_req_flag)
                    {
                        buf[1] = ENABLE_AUTO_ACCEPT_ACL_ACF_REQ;
                    }
                    else
                    {
                        buf[1] = FORWARD_ACL_ACF_REQ_TO_HOST;
                    }
                }
                break;

            case GET_STATUS_RADIO_MODE:
                {
                    buf[1] = app_bt_policy_get_radio_mode();
                }
                break;

            case GET_STATUS_SCO_STATUS:
                {
                    buf[1] = app_link_get_sco_conn_num();
                }
                break;

            case GET_STATUS_MIC_MUTE_STATUS:
                {
                    buf[1] = app_audio_is_mic_mute();
                }
                break;
#endif

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
            case CFG_GET_SPATIAL_AUDIO_MODE:
                {
                    buf[1] = app_cfg_nv.dual_audio_space;
                }
                break;
#endif

            default:
                break;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_STATUS, app_idx, buf, report_len);
        }
        break;

    case CMD_GET_BUD_INFO:
        {
            /* This command is used when snk_support_new_report_bud_status_flow is true.
             * Return complete RWS bud info.
             */
            uint8_t buf[6];

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_get_bud_info(&buf[0]);
            app_report_event(cmd_path, EVENT_REPORT_BUD_INFO, app_idx, buf, sizeof(buf));
        }
        break;

    case CMD_DEDICATE_BUD_COUPLING:
        {
            uint8_t buf[13];
            uint8_t remote_bud_role;
            uint8_t cmd_type = cmd_ptr[2];

            if (cmd_type == CMD_TYPE_QUERY)
            {
                buf[0] = app_cfg_const.bud_role;
                memcpy(&buf[1], app_db.factory_addr, 6);
                memcpy(&buf[7], app_cfg_nv.bud_peer_factory_addr, 6);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(cmd_path, EVENT_DEDICATE_BUD_COUPLING, app_idx, buf, sizeof(buf));
            }
            else if (cmd_type == CMD_TYPE_UPDATE)
            {
                if (cmd_len < 10)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }
                else
                {
                    remote_bud_role = cmd_ptr[3];

                    if (app_cfg_const.bud_role == remote_bud_role ||
                        app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE ||
                        remote_bud_role == REMOTE_SESSION_ROLE_SINGLE ||
                        app_db.device_state != APP_DEVICE_STATE_OFF)
                    {
                        // bud coupling needs one primary bud & one secondary bud
                        // need power-off state to update bud coupling info
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    }
                    else
                    {
#if F_APP_ERWS_SUPPORT
                        // update bud coupling info
                        memcpy(app_cfg_nv.bud_peer_factory_addr, &cmd_ptr[4], 6);
                        app_bt_policy_update_bud_coupling_info();

                        APP_PRINT_TRACE1("app_cmd_general_cmd_handle: bud_peer_factory_addr %s",
                                         TRACE_BDADDR(app_cfg_nv.bud_peer_factory_addr));
#endif
                    }
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_GET_FW_VERSION:
        {
            uint8_t report_data[2];
            report_data[0] = cmd_path;
            report_data[1] = app_idx;

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            switch (cmd_ptr[2])
            {
            case GET_PRIMARY_FW_VERSION:
                {
                    uint8_t data[13] = {0};

                    app_cmd_get_fw_version(&data[0]);
                    app_report_event(report_data[0], EVENT_FW_VERSION, report_data[1], data, sizeof(data));
                }
                break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            case GET_SECONDARY_FW_VERSION:
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_GET_FW_VERSION,
                                                        &report_data[0], 2);
                }
                break;

            case GET_PRIMARY_OTA_FW_VERSION:
                {
                    uint8_t data[IMG_LEN_FOR_DONGLE + 1] = {0};
                    data[0] = GET_PRIMARY_OTA_FW_VERSION;

                    app_ota_get_brief_img_version_for_dongle(&data[1]);
                    app_report_event(report_data[0], EVENT_FW_VERSION, report_data[1], data, sizeof(data));
                }
                break;

            case GET_SECONDARY_OTA_FW_VERSION:
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION,
                                                        &report_data[0], 2);
                }
                break;
#endif

            case GET_PRIMARY_UI_OTA_VERSION:
                {
                    uint8_t data[5] = {0};

                    data[0] = GET_PRIMARY_UI_OTA_VERSION;

                    app_cmd_get_ui_ota_version(&data[1]);
                    app_report_event(report_data[0], EVENT_FW_VERSION, report_data[1], data, sizeof(data));
                }
                break;

            case GET_SECONDARY_UI_OTA_VERSION:
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_GET_UI_OTA_VERSION,
                                                        &report_data[0], 2);
                }
                break;
            }
        }
        break;

    case CMD_WDG_RESET:
        {
            uint8_t wdg_status = 0x00;

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_WDG_RESET, app_idx, &wdg_status, 1);

            // for 1 wire UART download
            if (cmd_path == CMD_PATH_UART)
            {
                os_delay(20);
                chip_reset(RESET_ALL);
            }
            // for ANC, which is not use UART path
            else
            {
#if F_APP_ANC_SUPPORT
                anc_wait_wdg_reset_mode = cmd_ptr[2];
#endif
            }
        }
        break;

#if 0
    case CMD_GET_FLASH_DATA:
        {
            switch (cmd_ptr[2])
            {
            case START_TRANS:
                {
                    if ((0x01 << cmd_ptr[3]) & ALL_DUMP_IMAGE_MASK)
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_cmd_flash_data_set_param(cmd_ptr[3], cmd_path, app_idx);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                break;

            case CONTINUE_TRANS:
                {
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_cmd_read_flash(flash_data.flash_data_start_addr_tmp, cmd_path, app_idx);
                }
                break;

            case SUPPORT_IMAGE_TYPE:
                {
                    uint8_t paras[5];

                    paras[0] = SUPPORT_IMAGE_TYPE_INFO;
                    paras[1] = (uint8_t)(ALL_DUMP_IMAGE_MASK);
                    paras[2] = (uint8_t)(ALL_DUMP_IMAGE_MASK >> 8);
                    paras[3] = (uint8_t)(ALL_DUMP_IMAGE_MASK >> 16);
                    paras[4] = (uint8_t)(ALL_DUMP_IMAGE_MASK >> 24);

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_REPORT_FLASH_DATA, app_idx, paras, sizeof(paras));
                }
                break;

            default:
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
                break;
            }
        }
        break;
#endif

    case CMD_GET_PACKAGE_ID:
        {
            uint8_t temp_buff[2];

            temp_buff[0] = sys_hall_read_chip_id();
            temp_buff[1] = sys_hall_read_package_id();

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_PACKAGE_ID, app_idx, temp_buff, 2);
        }
        break;

    case CMD_GET_EAR_DETECTION_STATUS:
        {
#if F_APP_DURIAN_SUPPORT
            app_durian_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
#else
            uint8_t status = 0;

            if (LIGHT_SENSOR_ENABLED)
            {
                status = 1;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_EAR_DETECTION_STATUS, app_idx, &status, sizeof(status));
#endif
        }
        break;

    case CMD_REG_ACCESS:
        {
            uint32_t addr;
            uint32_t value;
            uint8_t report[5];
            uint32_t *report_value = (uint32_t *)(report + 1);

            memcpy(&addr, &cmd_ptr[4], sizeof(uint32_t));
            memcpy(&value, &cmd_ptr[8], sizeof(uint32_t));

            report[0] = false;

            if (cmd_ptr[2] == REG_ACCESS_READ)
            {
                switch (cmd_ptr[3])
                {
                case REG_ACCESS_TYPE_AON:
                    {
                        *report_value = btaon_fast_read_safe_8b(addr);
                    }
                    break;

                case REG_ACCESS_TYPE_AON2B:
                    {
                        *report_value = btaon_fast_read_safe(addr);
                    }
                    break;

                case REG_ACCESS_TYPE_DIRECT:
                    {
                        *report_value = HAL_READ32(addr, 0);
                    }
                    break;

                default:
                    break;
                }
            }
            else if (cmd_ptr[2] == REG_ACCESS_WRITE)
            {
                switch (cmd_ptr[3])
                {
                case REG_ACCESS_TYPE_AON:
                    {
                        btaon_fast_write_safe_8b(addr, value);
                    }
                    break;

                case REG_ACCESS_TYPE_AON2B:
                    {
                        btaon_fast_write_safe(addr, value);
                    }
                    break;

                case REG_ACCESS_TYPE_DIRECT:
                    {
                        HAL_WRITE32(addr, 0, value);
                    }
                    break;

                default:
                    break;
                }
            }
            else
            {
                report[0] = true;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REG_ACCESS, app_idx, report, sizeof(report));
        }
        break;

#if 0
    case CMD_SEND_RAW_PAYLOAD:
        {
            uint8_t direction = cmd_ptr[2];

            if (cmd_len - 2 < 6)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else if (app_audio_get_seg_send_status())
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
                    (direction != app_cfg_const.bud_side || direction == DEVICE_BUD_SIDE_BOTH))
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_SYNC_RAW_PAYLOAD,
                                                        cmd_ptr + 2, cmd_len - 2);
                }

                if (direction == app_cfg_const.bud_side || direction == DEVICE_BUD_SIDE_BOTH)
                {
                    ack_pkt[2] = app_cmd_compose_payload(cmd_ptr + 2, cmd_len - 2);
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    case CMD_GET_IMAGE_INFO:
        {
            uint8_t image_feature_info[IMG_FEATURE_STR_LEN] = {0};
            uint16_t image_id;
            LE_ARRAY_TO_UINT16(image_id, &cmd_ptr[2])

            if ((image_id < IMG_OTA) || (image_id >= IMAGE_MAX))
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                if (get_image_feature_info((IMG_ID)image_id, image_feature_info, IMG_FEATURE_STR_LEN) != 0)
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }

            uint8_t evt_buf[IMG_FEATURE_STR_LEN + 2];
            memcpy(&evt_buf[0], &cmd_ptr[2], 2);
            memcpy(&evt_buf[2], image_feature_info, IMG_FEATURE_STR_LEN);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_IMAGE_INFO, app_idx, evt_buf,
                             (IMG_FEATURE_STR_LEN + 2));
        }
        break;

#if F_APP_FLASH_DUMP_SUPPORT
#if 0
    case CMD_GET_MAX_TRANSMIT_SIZE:
        {
            uint16_t max_size = 0;

            if (cmd_path == CMD_PATH_SPP)
            {
                APP_PRINT_TRACE1("app_cmd_read_flash: rfc_frame_size %d",
                                 app_db.br_link[app_idx].rfc_frame_size);
                max_size = app_db.br_link[app_idx].rfc_frame_size - 20;
            }
            else if (cmd_path == CMD_PATH_IAP)
            {
#if F_APP_IAP_RTK_SUPPORT && F_APP_IAP_SUPPORT
                T_APP_IAP_HDL app_iap_hdl = NULL;
                app_iap_hdl = app_iap_search_by_addr(app_db.br_link[app_idx].bd_addr);
                uint16_t frame_size = app_iap_get_frame_size(app_iap_hdl);

                APP_PRINT_TRACE1("app_read_flash: iap frame_size %d", frame_size);
                max_size = frame_size - 20;
#endif
            }
            else if (cmd_path == CMD_PATH_LE)
            {
                APP_PRINT_TRACE1("app_read_flash: mtu_size %d", app_db.le_link[app_idx].mtu_size);
                max_size = app_db.le_link[app_idx].mtu_size - 20;
            }

            uint8_t data[3];
            data[0] = cmd_path;
            data[1] = (uint8_t)(max_size);
            data[2] = (uint8_t)(max_size >> 8);

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_GET_MAX_TRANSMIT_SIZE, app_idx, data, 3);
        }
        break;

    case CMD_DUMP_FLASH_DATA:
        {
            uint32_t flash_start_addr = 0x00000000;
            uint32_t flash_size = 0x00000000;
            LE_ARRAY_TO_UINT32(flash_start_addr, &cmd_ptr[2]);
            LE_ARRAY_TO_UINT32(flash_size, &cmd_ptr[6]);

            uint8_t *data = malloc(flash_size + 9);

            if (data != NULL)
            {
                memcpy_s(&data[0], 8, &cmd_ptr[2], 8);

                if (fmc_flash_nor_read(flash_start_addr, &data[8], flash_size))// read flash data
                {
                    data[flash_size + 8] = app_util_calc_checksum(&data[8], flash_size);
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_DUMP_FLASH_DATA, app_idx, data, flash_size + 9);
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }

                free(data);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;
#endif

#if F_APP_LOG2FLASH_SUPPORT
    case CMD_LOG2FLASH_STATE_SET:
        {
            uint8_t state = cmd_ptr[2];
            // TODO:
            // app_cmd_show_timestamp(&rssi_flash_header.timestamp, 6);

            if (state == 0x00)
            {
                BOOT_PRINT_INFO7("app_cmd_general_cmd_handle: log2flash state %d, date %d-%d-%d, time %d-%d-%d",
                                 state, log2flash_mgr.timestamp[0] + 1970, log2flash_mgr.timestamp[1] + 1,
                                 log2flash_mgr.timestamp[2],
                                 log2flash_mgr.timestamp[3], log2flash_mgr.timestamp[4], log2flash_mgr.timestamp[5]);
                enable_log_to_flash(false);
                memset(log2flash_mgr.timestamp, 0, sizeof(log2flash_mgr.timestamp));
            }
            else if (state == 0x01)
            {
                // log to flash init
                uint64_t mask[LEVEL_NUM];
                memcpy(log2flash_mgr.timestamp, &cmd_ptr[3], 6);

                // user can enable log which they want to dump by call api log_module_trace_set
                // only enable MODULE_BOOT
                memset(mask, 0, sizeof(mask));
                log_module_trace_init(mask);
                log_module_trace_set(MODULE_BOOT, LEVEL_ERROR, true);
                log_module_trace_set(MODULE_BOOT, LEVEL_WARN, true);
                log_module_trace_set(MODULE_BOOT, LEVEL_INFO, true);
                log_module_trace_set(MODULE_BOOT, LEVEL_TRACE, true);
                // log_enable_trace_string(false);  // disable trace string
                // enable too much log module may cause missing of logs
                enable_log_to_flash(true);
                BOOT_PRINT_INFO7("app_cmd_general_cmd_handle: log2flash state %d, date %d-%d-%d, time %d-%d-%d",
                                 state, log2flash_mgr.timestamp[0] + 1970, log2flash_mgr.timestamp[1] + 1,
                                 log2flash_mgr.timestamp[2],
                                 log2flash_mgr.timestamp[3], log2flash_mgr.timestamp[4], log2flash_mgr.timestamp[5]);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    case CMD_DUMP_FLASH:
        {
            uint32_t start_addr;
            uint32_t dump_size;
            uint8_t data[6];

            LE_ARRAY_TO_UINT32(start_addr, &cmd_ptr[2]);
            LE_ARRAY_TO_UINT32(dump_size, &cmd_ptr[6]);
            data[0] = 0x00;
            data[1] = app_cfg_const.bud_side;

            if (app_cmd_dump_flash_init(start_addr, dump_size))
            {
                dump_flash_db.max_transmit_size = app_db.br_link[app_idx].rfc_frame_size - 20;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            APP_PRINT_INFO2("app_cmd_general_cmd_handle: dump_flash start_addr 0x%x, dump_size 0x%x",
                            dump_flash_db.start_addr, dump_flash_db.dump_size);
            LE_UINT32_TO_ARRAY(&data[2], dump_flash_db.dump_size);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_DUMP_FLASH, app_idx, data, 6);
        }
        break;

    case CMD_FLASH_CHECK_RESULT:
        {
            uint8_t check_result = cmd_ptr[2];

            if (check_result > 0x01)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            if (check_result)
            {
                dump_flash_db.block_addr += dump_flash_db.block_size;
                dump_flash_db.block_used = 0;
                if (dump_flash_db.block_addr == dump_flash_db.start_addr + dump_flash_db.dump_size)
                {
                    uint8_t state = 1;

                    app_report_event(cmd_path, EVENT_REPORT_DUMP_STATE, app_idx, &state, 1);
                    break;
                }
            }

            APP_PRINT_INFO2("app_cmd_general_cmd_handle: dump_flash block_addr 0x%x, check_result 0x%x",
                            dump_flash_db.block_addr, check_result);
            app_dump_flash_cmd_ack_handle(cmd_path, app_idx);
        }
        break;

    case CMD_FLASH_STATE_GET:
        {
            uint8_t info_type = cmd_ptr[2];
            uint8_t data[9];
            // uint32_t flash_start_addr = 0;
            // uint32_t flash_size = 0;

            if (dump_flash_db.block_data != NULL)
            {
                free(dump_flash_db.block_data);
            }
            memset(&dump_flash_db, 0, sizeof(T_DUMPFLASH_DB));

            data[0] = info_type;
//            BOOT_PRINT_INFO1("app_cmd_general_cmd_handle: info_type %d", info_type);

#if F_APP_LOG2FLASH_SUPPORT
            {
                // need to disable log2flash when dump data from flash
                BOOT_PRINT_INFO6("app_cmd_general_cmd_handle: log2flash, date %d-%d-%d, time %d-%d-%d",
                                 log2flash_mgr.timestamp[0] + 1970, log2flash_mgr.timestamp[1] + 1, log2flash_mgr.timestamp[2],
                                 log2flash_mgr.timestamp[3], log2flash_mgr.timestamp[4], log2flash_mgr.timestamp[5]);
                enable_log_to_flash(false); // Note: disable log2flash before dump log from flash
                memset(log2flash_mgr.timestamp, 0, sizeof(log2flash_mgr.timestamp));
            }
#endif

            if (info_type == 0x00)
            {
                dump_flash_db.start_addr = flash_partition_addr_get(PARTITION_FLASH_FTL);
                dump_flash_db.dump_size = flash_partition_size_get(PARTITION_FLASH_FTL) & 0x00FFFFFF;
            }
#if F_APP_LOG2FLASH_SUPPORT
            else if (info_type == 0x01)
            {
                dump_flash_db.start_addr = log2flash_mgr.flash_start_addr;
                dump_flash_db.dump_size = log2flash_mgr.flash_size;
            }
#endif
#if F_APP_RSSI_MONITOR_SUPPORT
            else if (info_type == 0x02) // dump rssi from flash
            {
                dump_flash_db.start_addr = rssi_mgr.rssi_start_addr;
                dump_flash_db.dump_size = (rssi_mgr.block_num + 1) * FLASH_BLOCK_SIZE;
            }
#endif
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            LE_UINT32_TO_ARRAY(&data[1], dump_flash_db.start_addr);
            LE_UINT32_TO_ARRAY(&data[5], dump_flash_db.dump_size);
            app_report_event(cmd_path, EVENT_FLASH_STATE_REPORT, app_idx, data, 9);
            APP_PRINT_INFO3("app_cmd_general_cmd_handle: info_type %d flash_start_addr 0x%x, flash_size 0x%x",
                            data[0], dump_flash_db.start_addr, dump_flash_db.dump_size);
        }
        break;

#endif

    case CMD_FEATURE_STATE_GET:
        {
            uint8_t info_type = cmd_ptr[2];
            uint8_t data[10];
            uint8_t data_size = 0;

            data[0] = info_type;
#if F_APP_LOG2FLASH_SUPPORT
            if (info_type == 0x00)  // log2flash
            {
                LE_UINT8_TO_ARRAY(&data[1], log_to_flash_status_get());
                data_size = 1;
            }
            else
#endif
#if F_APP_RSSI_MONITOR_SUPPORT
                if (info_type == 0x01) // rssi monitor
                {
                    LE_UINT8_TO_ARRAY(&data[1], rssi_mgr.link_type);
                    LE_UINT8_TO_ARRAY(&data[2], rssi_mgr.mode);
                    LE_UINT16_TO_ARRAY(&data[3], rssi_mgr.report_period);
                    LE_UINT8_TO_ARRAY(&data[5], rssi_mgr.report_interval);
                    data_size = 5;
                }
                else
#endif
                {
                    APP_PRINT_INFO1("app_cmd_general_cmd_handle: CMD_FEATURE_STATE_GET, info_type %d", info_type);
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_FEATURE_STATE_GET_RESULT, app_idx, data, data_size + 1);
            }
        }
        break;

    default:
        break;
    }
}

static void app_cmd_other_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                     uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_ASSIGN_BUFFER_SIZE:
        {
            app_db.external_mcu_mtu = (cmd_ptr[4] | (cmd_ptr[5] << 8));
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_TONE_GEN:
        {
            ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_STRING_MODE:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
#if F_APP_CONSOLE_SUPPORT
            console_set_mode(CONSOLE_MODE_STRING);
#endif
        }
        break;

    case CMD_SET_AND_READ_DLPS:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (cmd_ptr[2] == SET_DLPS_DISABLE)
            {
                dlps_status = 0x00;
                app_dlps_disable(APP_DLPS_ENTER_CHECK_CMD);
            }
            else if (cmd_ptr[2] == SET_DLPS_ENABLE)
            {
                dlps_status = 0x01;
                app_dlps_enable(APP_DLPS_ENTER_CHECK_CMD);
            }

            app_report_event(cmd_path, EVENT_REPORT_DLPS_STATUS, app_idx, &dlps_status, 1);
        }
        break;

#if F_APP_BLE_ANCS_CLIENT_SUPPORT
    case CMD_ANCS_REGISTER:
        {
            uint8_t le_index;

            le_index = cmd_ptr[2];

            if (app_db.le_link[le_index].state == LE_LINK_STATE_CONNECTED)
            {
                if (ancs_start_discovery(app_db.le_link[le_index].conn_id) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_ANCS_GET_NOTIFICATION_ATTR:
        {
            uint8_t le_index;
            uint32_t  notification_uid;

            le_index = cmd_ptr[2];
            notification_uid = *((uint32_t *)&cmd_ptr[3]);

            if (app_db.le_link[le_index].state == LE_LINK_STATE_CONNECTED)
            {
                if (ancs_get_notification_attr(app_db.le_link[le_index].conn_id, notification_uid,
                                               &cmd_ptr[8],
                                               cmd_ptr[7]) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

#if 0
    case CMD_LED_TEST:
        {
            uint8_t led_index = cmd_ptr[2];
            uint8_t on_off_flag = cmd_ptr[3];
            uint8_t event_buff;

            if (led_index >= LED_NUM)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                break;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (on_off_flag == 0)  /*turn off*/
            {
                app_led_set_designate_led_on(led_index);
            }
            else
            {
                app_led_set_designate_led_off(led_index);
            }
            event_buff = 1;
            app_report_event(cmd_path, EVENT_LED_TEST, app_idx, &event_buff, 1);
        }
        break;

    case CMD_SWITCH_TO_HCI_DOWNLOAD_MODE:
        {
            //if uart tx shares the same pin with 3pin gpio, set uart tx pin when receive cmd
            if (app_cfg_const.enable_4pogo_pin)
            {
                if (app_cfg_const.gpio_box_detect_pinmux == app_cfg_const.data_uart_tx_pinmux)
                {
                    Pinmux_Config(app_cfg_const.data_uart_tx_pinmux, UART0_TX);
                    Pad_Config(app_cfg_const.data_uart_tx_pinmux,
                               PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_start_timer(&timer_idx_switch_to_hci_mode, "switch_to_hci_mode",
                            app_cmd_timer_id, APP_TIMER_SWITCH_TO_HCI_DOWNLOAD_MODE, app_idx, false,
                            100);
        }
        break;
#endif

#if F_APP_ADC_SUPPORT
    case CMD_GET_PAD_VOLTAGE:
        {
            uint8_t adc_pin = cmd_ptr[2];

            app_adc_set_cmd_info(cmd_path, app_idx);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (!app_adc_enable_read_adc_io(adc_pin))
            {
                uint8_t evt_param[2] = {0xFF, 0xFF};

                app_report_event(cmd_path, EVENT_REPORT_PAD_VOLTAGE, app_idx, evt_param, sizeof(evt_param));
                APP_PRINT_TRACE0("CMD_GET_PAD_VOLTAGE register ADC mgr fail");
            }
        }
        break;
#endif

    case CMD_RF_XTAK_K:
        {
#if 0
            uint8_t report_status = 0;
            uint8_t rf_channel = cmd_ptr[2];
            uint8_t freq_upperbound = cmd_ptr[3];
            uint8_t freq_lowerbound = cmd_ptr[4];
            uint8_t measure_offset = cmd_ptr[5];

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            report_status = 0;
            app_report_event(cmd_path, EVENT_RF_XTAL_K, app_idx, &report_status, 1);

            app_dlps_disable(APP_DLPS_ENTER_CHECK_RF_XTAL_K);

            /* start rf xtal K */
            uint8_t param[] = {0, rf_channel, freq_upperbound, freq_lowerbound, measure_offset, 0 /* get result */ };

            app_cfg_nv.xtal_k_result = 0;
            rf_xtal_k((uint32_t)param);
            app_cfg_nv.xtal_k_result = param[5];

            app_cfg_store(&app_cfg_nv.offset_xtal_k_result, 4);

            APP_PRINT_TRACE2("CMD_RF_XTAK_K: result %d, %b", app_cfg_nv.xtal_k_result,
                             TRACE_BINARY(3, &cmd_ptr[2]));

            /* enter pairing mode after xtal k finished */
            app_mmi_handle_action(MMI_DEV_FORCE_ENTER_PAIRING_MODE);

            app_dlps_enable(APP_DLPS_ENTER_CHECK_RF_XTAL_K);
#endif
        }
        break;

    case CMD_RF_XTAL_K_GET_RESULT:
        {
#if 0
            uint8_t xtal_k_result[3] = {0};

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            /* get xtal value */
            uint8_t tmp[2];
            uint8_t xtal_value;
            uint8_t write_times;

            get_rf_xtal_k_result((uint32_t)tmp);
            xtal_value = tmp[0];
            write_times = tmp[1];

            if (xtal_value == 0xff || write_times == 0)
            {
                app_cfg_nv.xtal_k_result = 7; /* XTAL_K_NOT_DO_YET */
            }

            xtal_k_result[0] = app_cfg_nv.xtal_k_result; /* 0: XTAL_K_SUCCESS */
            xtal_k_result[1] = xtal_value;
            xtal_k_result[2] = write_times;

            APP_PRINT_TRACE3("CMD_RF_XTAL_K_GET_RESULT: %02x %02x %02x",
                             xtal_k_result[0], xtal_k_result[1], xtal_k_result[2]);

            app_report_event(cmd_path, EVENT_RF_XTAL_K_GET_RESULT, app_idx, xtal_k_result,
                             sizeof(xtal_k_result));
#endif
        }
        break;

#if F_APP_ANC_SUPPORT
    case CMD_ANC_VENDOR_COMMAND:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            uint8_t *anc_tool_cmd_ptr;
            anc_tool_cmd_ptr = malloc(cmd_ptr[4] + 5);
            anc_tool_cmd_ptr[0] = cmd_path;
            anc_tool_cmd_ptr[1] = app_idx;
            memcpy(&anc_tool_cmd_ptr[2], &cmd_ptr[2], cmd_ptr[4] + 3);
            app_anc_handle_vendor_cmd(anc_tool_cmd_ptr);
            free(anc_tool_cmd_ptr);
        }
        break;
#endif

#if F_APP_OTA_TOOLING_SUPPORT
    case CMD_OTA_TOOLING_PARKING:
        {
            uint8_t report_status = 0;
            uint8_t dlps_stay_mode = 0;

            app_report_event(CMD_PATH_SPP, EVENT_ACK, app_idx, ack_pkt, 3);
            dlps_stay_mode = cmd_ptr[2];

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SET_LPS_SYNC,
                                                &dlps_stay_mode, 1);

            // response to HOST
            app_report_event(cmd_path, EVENT_OTA_TOOLING_PARKING, app_idx, &report_status, 1);

            // delay power off to prevent SPP traffic jam
            app_start_timer(&timer_idx_ota_parking_power_off, "ota_jig_delay_power_off",
                            app_cmd_timer_id, APP_TIMER_OTA_JIG_DELAY_POWER_OFF, NULL, false,
                            2000);
        }
        break;
#endif

    case CMD_MEMORY_DUMP:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_mmi_handle_action(MMI_MEMORY_DUMP);
        }
        break;

    case CMD_MP_TEST:
        {
#if F_APP_CLI_BINARY_MP_SUPPORT
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            mp_test_handle_cmd(app_idx, cmd_path, cmd_ptr[2], cmd_ptr[3], &cmd_ptr[4], cmd_len - 4);
#endif
        }
        break;

    default:
        break;
    }
}

#if 1
static void app_cmd_customized_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                          uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_IO_PIN_PULL_HIGH:
        {
            uint8_t report_status = 0;
            uint8_t pin_num = cmd_ptr[2];

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SYNC_IO_PIN_PULL_HIGH,
                                                    &pin_num, 1);

                app_start_timer(&timer_idx_io_pin_pull_high, "io_pin_pull_high",
                                app_cmd_timer_id, APP_TIMER_IO_PIN_PULL_HIGH, pin_num, false,
                                500);
            }
            else
#endif
            {
                Pad_Config(pin_num, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
            }

            app_report_event(cmd_path, EVENT_IO_PIN_PULL_HIGH, app_idx, &report_status, 1);
        }
        break;

    case CMD_ENTER_BAT_OFF_MODE:
        {
            uint8_t report_status = 0;

            app_led_change_mode(LED_MODE_ENTER_PCBA_SHIPPING_MODE, true, true);
            app_report_event(cmd_path, EVENT_ENTER_BAT_OFF_MODE, app_idx, &report_status, 1);
        }
        break;

#if F_APP_DUAL_AUDIO_EFFECT
    case CMD_CUSTOMIZED_SITRON_FEATURE:
        {
            uint8_t need_ack_flag = true;

            switch (cmd_ptr[2])
            {
            case SITRONIX_SECP_INDEX:
                {
                    /*ignore single effect*/
                    uint8_t actual_effect_num = app_dual_audio_get_effect_num() - 1;
                    if (app_db.gaming_mode)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        APP_PRINT_INFO0("app_handle_mmi_message: When headset comes to gaming mode, not support default effect change!");
                        break;
                    }

                    app_dual_audio_set_report_para(app_idx, cmd_path);
                    /*give the previous effect,then call mmi skip flow*/
                    if (cmd_ptr[3] <  actual_effect_num)
                    {
                        app_cfg_nv.audio_effect_normal_type = cmd_ptr[3] == 0 ?  actual_effect_num - 1 :
                                                              cmd_ptr[3] - 1;
                    }
                    else
                    {
                        app_cfg_nv.audio_effect_normal_type =  actual_effect_num - 1;
                    }

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_mmi_handle_action(MMI_AUDIO_DUAL_EFFECT_SWITCH);
                    }
                    else
                    {
                        uint8_t action;
                        action = MMI_AUDIO_DUAL_EFFECT_SWITCH;
                        app_dual_audio_sync_info();
                        app_relay_async_single(APP_MODULE_TYPE_MMI, action);
                        app_mmi_handle_action(action);
                    }
                }
                break;

            case SPP_UPADTE_DUAL_AUDIO_EFFECT:
                {
                    bool stream = false;

                    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_db.br_link[i].streaming_fg)
                        {
                            stream = true;
                        }
                    }
                    if (stream)
                    {
                        uint16_t offset_44K = cmd_ptr[3] | (cmd_ptr[4] << 8);
                        app_dual_audio_effect_spp_init(offset_44K, app_idx);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    }
                }
                break;

            case SPP_UPDATE_SECP_DATA:
                {
                    need_ack_flag = false;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_spp_update_data(&cmd_ptr[3]);
                }
                break;

            case SITRONIX_SECP_NUM:
                {
                    need_ack_flag = false;
                    app_dual_audio_set_report_para(app_idx, cmd_path);
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_report_effect();
                }
                break;

            case SITRONIX_SECP_SET_APP_VALUE:
                {
                    uint16_t index;

                    index = cmd_ptr[3] | (cmd_ptr[4] << 8);
                    if (index >= 16)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        break;
                    }
                    need_ack_flag = false;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_set_report_para(app_idx, cmd_path);
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_dual_audio_set_app_key_val((uint8_t) index, (cmd_ptr[5] | (cmd_ptr[6] << 8)));
                        app_dual_audio_report_app_key(index);
                        app_dual_audio_app_key_to_dsp();
                    }
                    else
                    {
                        app_dual_audio_sync_app_key_val(&cmd_ptr[3], true);
                    }
                }
                break;

            case SITRONIX_SECP_GET_APP_VALUE:
                {
                    uint16_t index;

                    index = cmd_ptr[3] | (cmd_ptr[4] << 8);
                    if (index >= 16)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        break;
                    }

                    need_ack_flag = false;
                    app_dual_audio_set_report_para(app_idx, cmd_path);
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_report_app_key(index);
                }
                break;

            case SITRONIX_SECP_SPP_RESTART_ACK:
                {
                    need_ack_flag = false;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_effect_restart_ack();
                }
                break;

            case SITRONIX_SECP_SPP_GET_INFO:
                {
                    need_ack_flag = false;
                    app_dual_audio_set_report_para(app_idx, cmd_path);
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_dual_audio_effect_report_version_info();
                }
                break;

            default:
                {
                    ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
                }
                break;
            }

            if (need_ack_flag)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;
#endif
#if F_APP_SENSOR_MEMS_SUPPORT
    case CMD_GET_IMU_SENSOR_DATA:
        {
            uint8_t need_ack_flag = true;

            if (app_cfg_const.mems_support == 0)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                return;
            }

            switch (cmd_ptr[2])
            {
            case IMU_SENSOR_DATA_START_REPORT:
                if (app_db.report == REPORT_NONE)
                {
                    app_sensor_mems_start(app_a2dp_get_active_idx());
                    app_db.report = REPORT_RTK;
                    app_sensor_mems_spp_report_init();
                }
                break;
            case IMU_SENSOR_DATA_STOP_REPORT:
                if (app_db.report == REPORT_RTK)
                {
                    if (app_db.br_link[app_a2dp_get_active_idx()].bt_sniffing_state == APP_BT_SNIFFING_STATE_IDLE)
                    {
                        app_sensor_mems_stop(app_a2dp_get_active_idx());
                    }
                    app_sensor_mems_spp_report_de_init();
                    app_db.report = REPORT_NONE;
                }
                break;

            case IMU_SENSOR_CYWEE_DATA_START_REPORT:
                if (app_db.report == REPORT_NONE)
                {
                    app_db.report = REPORT_CYWEE;
                    app_sensor_mems_spp_report_init();
                }
                break;

            case IMU_SENSOR_CYWEE_DATA_STOP_REPORT:
                if (app_db.report == REPORT_CYWEE)
                {
                    app_sensor_mems_spp_report_de_init();
                    app_db.report = REPORT_NONE;
                }
                break;

            default:
                break;
            }

            if (need_ack_flag)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;
#endif
    case CMD_MIC_MP_VERIFY_BY_HFP:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            uint8_t specified_mic;
            uint8_t report_status = 0;

            specified_mic = cmd_ptr[2];

            APP_PRINT_INFO4("CMD_MIC_MP_VERIFY_BY_HFP specified_mic = %x, pri_sel_ori = %x, pri_sel_new = %x, pri_type_new = %x",
                            specified_mic, app_db.mic_mp_verify_pri_sel_ori, cmd_ptr[3], cmd_ptr[4]);

            if (specified_mic)
            {
                if (app_db.mic_mp_verify_pri_sel_ori == APP_MIC_SEL_DISABLE)
                {
                    app_db.mic_mp_verify_pri_sel_ori = audio_probe_get_voice_primary_mic_sel();
                    app_db.mic_mp_verify_pri_type_ori = audio_probe_get_voice_primary_mic_type();
                    app_db.mic_mp_verify_sec_sel_ori = audio_probe_get_voice_secondary_mic_sel();
                    app_db.mic_mp_verify_sec_type_ori = audio_probe_get_voice_secondary_mic_type();
                }

                audio_probe_set_voice_primary_mic(cmd_ptr[3], cmd_ptr[4]);
                auido_probe_set_voice_secondary_mic(APP_MIC_SEL_DISABLE, app_db.mic_mp_verify_sec_type_ori);
            }
            else
            {
                if (app_db.mic_mp_verify_pri_sel_ori != APP_MIC_SEL_DISABLE)
                {
                    audio_probe_set_voice_primary_mic(app_db.mic_mp_verify_pri_sel_ori,
                                                      app_db.mic_mp_verify_pri_type_ori);
                    auido_probe_set_voice_secondary_mic(app_db.mic_mp_verify_sec_sel_ori,
                                                        app_db.mic_mp_verify_sec_type_ori);

                    app_db.mic_mp_verify_pri_sel_ori = APP_MIC_SEL_DISABLE;
                }
            }

            app_report_event(cmd_path, EVENT_MIC_MP_VERIFY_BY_HFP, app_idx, &report_status, 1);
        }
        break;

#if GFPS_SASS_SUPPORT
    case CMD_SASS_FEATURE:
        {
            if (extend_app_cfg_const.gfps_sass_support)
            {
                uint8_t need_ack_flag = true;

                switch (cmd_ptr[2])
                {
                case SASS_PREEMPTIVE_FEATURE_BIT_SET:
                    {
                        app_sass_policy_set_switch_preference(cmd_ptr[3]);
                    }
                    break;

                case SASS_PREEMPTIVE_FEATURE_BIT_GET:
                    {
                        uint8_t report_buf[2];

                        report_buf[0] = SASS_PREEMPTIVE_FEATURE_BIT_GET;
                        report_buf[1] = app_sass_policy_get_switch_preference();
                        need_ack_flag = false;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_report_event(cmd_path, EVENT_REPORT_SASS_FEATURE, app_idx, report_buf, sizeof(report_buf));
                    }
                    break;

                case SASS_LINK_SWITCH:
                    {
                        app_sass_policy_switch_active_audio_source(0xFF, app_db.br_link[app_idx].bd_addr, cmd_ptr[3],
                                                                   cmd_ptr[4]);
                    }
                    break;

                case SASS_SWITCH_BACK:
                    {
                        app_sass_policy_switch_back(cmd_ptr[3]);
                    }
                    break;

                case SASS_MULTILINK_ON_OFF:
                    {
                        app_sass_policy_set_multipoint_state(cmd_ptr[3]);
                    }
                    break;

                default:
                    {
                        ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
                    }
                    break;
                }

                if (need_ack_flag)
                {
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
            }
        }
        break;
#endif

    default:
        break;
    }
}
#endif

void app_cmd_handler(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t rx_seqn,
                     uint8_t app_idx)
{
#if (F_APP_ANC_SUPPORT | F_APP_APT_SUPPORT | F_APP_BRIGHTNESS_SUPPORT | F_APP_LISTENING_MODE_SUPPORT)
    uint16_t param_len = cmd_len - COMMAND_ID_LENGTH;
#endif
    uint16_t cmd_id;
    uint8_t  ack_pkt[3];

    cmd_id     = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    ack_pkt[0] = cmd_ptr[0];
    ack_pkt[1] = cmd_ptr[1];
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    APP_PRINT_TRACE4("app_cmd_handler: cmd_id 0x%04x, cmd_len 0x%04x, cmd_path %u, rx_seqn 0x%02x",
                     cmd_id, cmd_len, cmd_path, rx_seqn);

    /* check duplicated seq num */
    if ((cmd_id != CMD_ACK) && (rx_seqn != 0))
    {
        uint8_t *check_rx_seqn = NULL;
        uint8_t  report_app_idx = app_idx;

        if (cmd_path == CMD_PATH_UART)
        {
            check_rx_seqn = &uart_rx_seqn;
            report_app_idx = 0;
        }
        else if (cmd_path == CMD_PATH_LE)
        {
            check_rx_seqn = &app_db.le_link[app_idx].rx_cmd_seqn;
        }
        else if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
        {
            check_rx_seqn = &app_db.br_link[app_idx].rx_cmd_seqn;
        }

        if (check_rx_seqn)
        {
            if (*check_rx_seqn == rx_seqn)
            {
                app_report_event(cmd_path, EVENT_ACK, report_app_idx, ack_pkt, 3);
                return;
            }

            *check_rx_seqn = rx_seqn;
        }
    }

    if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
    {
        app_db.br_link[app_idx].cmd_set_enable = true;
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        if (app_db.le_link[app_idx].state == LE_LINK_STATE_CONNECTED)
        {
            app_db.le_link[app_idx].cmd_set_enable = true;
        }
    }

    switch (cmd_id)
    {
    case CMD_ACK:
        {
            if (cmd_path == CMD_PATH_UART)
            {
                app_transfer_pop_data_queue(CMD_PATH_UART, true);
            }
            else if ((cmd_path == CMD_PATH_LE) || (cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
            {
                uint16_t event_id = (uint16_t)(cmd_ptr[2] | (cmd_ptr[3] << 8));
                uint8_t status = cmd_ptr[4];

//                BOOT_PRINT_INFO1("app_cmd_handler: EVENT_ACK, event_id 0x%04x", event_id);
                if (!app_cfg_const.enable_dsp_capture_data_by_spp && !app_cfg_const.mems_support)
                {
                    app_transfer_queue_recv_ack_check(event_id, cmd_path);
                }

#if 0
                if (event_id == EVENT_AUDIO_EQ_PARAM_REPORT)
                {
                    if (cmd_path == CMD_PATH_LE)
                    {
                        if (status != CMD_SET_STATUS_COMPLETE)
                        {
                            app_eq_report_terminate_param_report(cmd_path, app_idx);
                        }
                        else
                        {
                            app_eq_report_eq_param(cmd_path, app_idx);
                        }
                    }
                }
#if F_APP_OTA_SUPPORT
                else if (event_id == EVENT_OTA_ACTIVE_ACK || event_id == EVENT_OTA_ROLESWAP)
                {
                    if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
                    {
                        app_ota_cmd_ack_handle(event_id, status);
                    }
                }
#endif
#if F_APP_LOCAL_PLAYBACK_SUPPORT
                else if ((event_id == EVENT_PLAYBACK_GET_LIST_DATA) && (status == CMD_SET_STATUS_COMPLETE))
                {
                    if (cmd_path == CMD_PATH_SPP || cmd_path == CMD_PATH_IAP)
                    {
                        app_playback_trans_list_data_handle();
                    }
                }
#endif
#if F_APP_ANC_SUPPORT
                else if (event_id == CMD_WDG_RESET)
                {
                    app_anc_wait_wdg_reset(anc_wait_wdg_reset_mode);
                }
#endif
#endif
#if F_APP_FLASH_DUMP_SUPPORT
                if (event_id == EVENT_DUMP_FLASH)
                {
                    app_dump_flash_cmd_ack_handle(cmd_path, app_idx);
                }
                else if (event_id == EVENT_REPORT_DUMP_STATE)
                {
                    if (dump_flash_db.block_data)
                    {
                        free(dump_flash_db.block_data);
                        dump_flash_db.block_data = NULL;
                    }
                }
#endif
            }
        }
        break;

    case CMD_BT_READ_PAIRED_RECORD:
    case CMD_BT_CREATE_CONNECTION:
    case CMD_BT_DISCONNECT:
    case CMD_BT_READ_LINK_INFO:
    case CMD_BT_GET_REMOTE_NAME:
    case CMD_BT_IAP_LAUNCH_APP:
    case CMD_BT_SEND_AT_CMD:
    case CMD_BT_HFP_DIAL_WITH_NUMBER:
    case CMD_GET_BD_ADDR:
    case CMD_LE_START_ADVERTISING:
    case CMD_LE_START_SCAN:
    case CMD_LE_STOP_SCAN:
    case CMD_LE_GET_ADDR:
    case CMD_BT_GET_LOCAL_ADDR:
#if F_APP_RSSI_MONITOR_SUPPORT
    case CMD_GET_LEGACY_RSSI:
#endif
    case CMD_BT_BOND_INFO_CLEAR:
    case CMD_GET_NUM_OF_CONNECTION:
    case CMD_SUPPORT_MULTILINK:
        {
            app_cmd_bt_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

#if 0
    case CMD_LEGACY_DATA_TRANSFER:
    case CMD_LE_DATA_TRANSFER:
        {
            app_transfer_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_MMI:
    case CMD_INFO_REQ:
    case CMD_SET_CFG:
    case CMD_GET_CFG_SETTING:
    case CMD_INDICATION:
    case CMD_LANGUAGE_GET:
    case CMD_LANGUAGE_SET:
    case CMD_GET_STATUS:
    case CMD_GET_BUD_INFO:
    case CMD_DEDICATE_BUD_COUPLING:
    case CMD_GET_FW_VERSION:
    case CMD_WDG_RESET:
    case CMD_GET_FLASH_DATA:
    case CMD_GET_PACKAGE_ID:
    case CMD_GET_EAR_DETECTION_STATUS:
    case CMD_REG_ACCESS:
    case CMD_SEND_RAW_PAYLOAD:
    case CMD_GET_IMAGE_INFO:
    case CMD_GET_MAX_TRANSMIT_SIZE:
    case CMD_DUMP_FLASH_DATA:
    case CMD_FLASH_CHECK_RESULT:
    case CMD_FLASH_STATE_GET:
    case CMD_DUMP_FLASH:
    case CMD_LOG2FLASH_STATE_SET:
    case CMD_FEATURE_STATE_GET:
        {
            app_cmd_general_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

#if F_APP_TTS_SUPPORT
    case CMD_TTS:
        {
            app_tts_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
    case CMD_SET_VP_VOLUME:
    case CMD_AUDIO_DSP_CTRL_SEND:
    case CMD_AUDIO_CODEC_CTRL_SEND:
    case CMD_SET_VOLUME:
#if F_APP_APT_SUPPORT
    case CMD_SET_APT_VOLUME_OUT_LEVEL:
    case CMD_GET_APT_VOLUME_OUT_LEVEL:
#endif
#if F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT
    case CMD_SET_NOTIFICATION_VOL_LEVEL:
    case CMD_GET_NOTIFICATION_VOL_LEVEL:
#endif
    case CMD_DSP_TOOL_FUNCTION_ADJUSTMENT:
#if F_APP_SIDETONE_SUPPORT
    case CMD_SET_SIDETONE:
#endif
    case CMD_MIC_SWITCH:
    case CMD_DSP_TEST_MODE:
    case CMD_DUAL_MIC_MP_VERIFY:
    case CMD_SOUND_PRESS_CALIBRATION:
    case CMD_GET_LOW_LATENCY_MODE_STATUS:
    case CMD_SET_LOW_LATENCY_LEVEL:
        {
            app_audio_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
    case CMD_SET_SPATIAL_AUDIO:
        {
            app_sensor_mems_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_2
    case CMD_DSP_CAPTURE_V2_START_STOP:
        {
            app_data_capture_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
    case CMD_DSP_CAPTURE_SCENARIO_ENTER_EXIT:
        {
            app_data_capture_mode_ctl(&cmd_ptr[2], cmd_len - 2, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_CAP_TOUCH_SUPPORT
    case CMD_CAP_TOUCH_CTL:
        {
            CTC_PRINT_TRACE1("CMD_CAP_TOUCH_CTL %b", TRACE_BINARY(cmd_len, cmd_ptr));
            app_dlps_disable(APP_DLPS_ENTER_CHECK_CMD);
            //app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_cap_touch_cmd_param_handle(cmd_path, cmd_ptr, app_idx);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_CMD);
        }
        break;
#endif

#if 0
    case CMD_AUDIO_EQ_QUERY:
    case CMD_AUDIO_EQ_QUERY_PARAM:
    case CMD_AUDIO_EQ_PARAM_SET:
    case CMD_AUDIO_EQ_PARAM_GET:
    case CMD_AUDIO_EQ_INDEX_SET:
    case CMD_AUDIO_EQ_INDEX_GET:
#if F_APP_APT_SUPPORT
    case CMD_APT_EQ_INDEX_SET:
    case CMD_APT_EQ_INDEX_GET:
#endif
#if F_APP_USER_EQ_SUPPORT
    case CMD_RESET_EQ_DATA:
#endif
        {
            if (!app_cmd_check_src_eq_spec_version(cmd_path, app_idx))
            {
                ack_pkt[2] = CMD_SET_STATUS_VERSION_INCOMPATIBLE;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                return;
            }

            app_eq_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_DEVICE_CMD_SUPPORT
    case CMD_INQUIRY:
    case CMD_SERVICES_SEARCH:
    case CMD_SET_PAIRING_CONTROL:
    case CMD_SET_PIN_CODE:
    case CMD_GET_PIN_CODE:
    case CMD_PAIR_REPLY:
    case CMD_SSP_CONFIRMATION:
    case CMD_GET_CONNECTED_DEV_ID:
    case CMD_GET_REMOTE_DEV_ATTR_INFO:
        {
            app_cmd_device_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_SENSOR_PX318J_SUPPORT
    case CMD_PX318J_CALIBRATION:
        {
            app_sensor_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if (F_APP_SENSOR_JSA1225_SUPPORT || F_APP_SENSOR_JSA1227_SUPPORT)
    case CMD_JSA_CALIBRATION:
        {
            app_sensor_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if (F_APP_SENSOR_IQS773_873_SUPPORT == 1)
    case CMD_IQS773_CALIBRATION:
        {
            app_sensor_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_HFP_CMD_SUPPORT
    case CMD_SEND_DTMF:
    case CMD_GET_SUBSCRIBER_NUM:
    case CMD_GET_OPERATOR:
        {
            app_hfp_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_AVRCP_CMD_SUPPORT
    case CMD_AVRCP_LIST_SETTING_ATTR:
    case CMD_AVRCP_LIST_SETTING_VALUE:
    case CMD_AVRCP_GET_CURRENT_VALUE:
    case CMD_AVRCP_SET_VALUE:
    case CMD_AVRCP_ABORT_DATA_TRANSFER:
    case CMD_AVRCP_SET_ABSOLUTE_VOLUME:
    case CMD_AVRCP_GET_PLAY_STATUS:
    case CMD_AVRCP_GET_ELEMENT_ATTR:
        {
            app_avrcp_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_OTA_SUPPORT
    case CMD_OTA_DEV_INFO:
    case CMD_OTA_IMG_VER:
    case CMD_OTA_INACTIVE_BANK_VER:
    case CMD_OTA_START:
    case CMD_OTA_PACKET:
    case CMD_OTA_VALID:
    case CMD_OTA_RESET:
    case CMD_OTA_ACTIVE_RESET:
    case CMD_OTA_BUFFER_CHECK_ENABLE:
    case CMD_OTA_BUFFER_CHECK:
    case CMD_OTA_IMG_INFO:
    case CMD_OTA_SECTION_SIZE:
    case CMD_OTA_DEV_EXTRA_INFO:
    case CMD_OTA_PROTOCOL_TYPE:
    case CMD_OTA_GET_RELEASE_VER:
    case CMD_OTA_COPY_IMG:
    case CMD_OTA_CHECK_SHA256:
    case CMD_OTA_TEST_EN:
    case CMD_OTA_ROLESWAP:
        {
            app_ota_cmd_handle(cmd_path, cmd_len, cmd_ptr, app_idx);
        }
        break;
#endif

#if F_APP_KEY_EXTEND_FEATURE
    case CMD_GET_SUPPORTED_MMI_LIST:
    case CMD_GET_SUPPORTED_CLICK_TYPE:
    case CMD_GET_SUPPORTED_CALL_STATUS:
    case CMD_GET_KEY_MMI_MAP:
    case CMD_SET_KEY_MMI_MAP:
    case CMD_RESET_KEY_MMI_MAP:
#if F_APP_RWS_KEY_SUPPORT
    case CMD_GET_RWS_KEY_MMI_MAP:
    case CMD_SET_RWS_KEY_MMI_MAP:
    case CMD_RESET_RWS_KEY_MMI_MAP:
#endif
        {
            ack_pkt[2] = app_key_handle_key_remapping_cmd(cmd_len, cmd_ptr, app_idx, cmd_path);
            if (ack_pkt[2] != CMD_SET_STATUS_COMPLETE)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;
#endif

    case CMD_VENDOR_SEPC:
        {
#if HARMAN_SPP_CMD_SUPPORT
            app_harman_spp_cmd_set_handle(cmd_ptr, cmd_len, cmd_path, app_idx);
#endif
        }
        break;

#if F_APP_ANC_SUPPORT
    //for ANC command
    case CMD_ANC_TEST_MODE:
    case CMD_ANC_WRITE_GAIN:
    case CMD_ANC_READ_GAIN:
    case CMD_ANC_BURN_GAIN:
    case CMD_ANC_COMPARE:
    case CMD_ANC_GEN_TONE:
    case CMD_ANC_CONFIG_DATA_LOG:
    case CMD_ANC_READ_DATA_LOG:
    case CMD_ANC_READ_REGISTER:
    case CMD_ANC_WRITE_REGISTER:
    case CMD_ANC_QUERY:
    case CMD_ANC_ENABLE_DISABLE:
    case CMD_SPECIFY_ANC_QUERY:
    case CMD_ANC_LLAPT_WRITE_GAIN:
    case CMD_ANC_LLAPT_READ_GAIN:
    case CMD_ANC_LLAPT_BURN_GAIN:
    case CMD_ANC_LLAPT_FEATURE_CONTROL:
    case CMD_ANC_CONFIG_MEASURE_MIC:
    case CMD_RAMP_GET_INFO:
    case CMD_RAMP_BURN_PARA_START:
    case CMD_RAMP_BURN_PARA_CONTINUE:
    case CMD_RAMP_BURN_GRP_INFO:
    case CMD_RAMP_MULTI_DEVICE_APPLY:
    case CMD_LISTENING_MODE_CYCLE_SET:
    case CMD_LISTENING_MODE_CYCLE_GET:
    case CMD_ANC_GET_ADSP_INFO:
    case CMD_ANC_SEND_ADSP_PARA_START:
    case CMD_ANC_SEND_ADSP_PARA_CONTINUE:
    case CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC:
    case CMD_ANC_SCENARIO_CHOOSE_INFO:
    case CMD_ANC_SCENARIO_CHOOSE_TRY:
    case CMD_ANC_SCENARIO_CHOOSE_RESULT:
    case CMD_ANC_GET_SCENARIO_IMAGE:
    case CMD_ANC_GET_FILTER_INFO:
        {
            app_anc_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_APT_SUPPORT
    case CMD_LLAPT_QUERY:
    case CMD_LLAPT_ENABLE_DISABLE:
    case CMD_APT_VOLUME_INFO:
    case CMD_APT_VOLUME_SET:
    case CMD_APT_VOLUME_STATUS:
    case CMD_APT_VOLUME_MUTE_SET:
    case CMD_APT_VOLUME_MUTE_STATUS:
#if F_APP_LLAPT_SCENARIO_CHOOSE_SUPPORT
    case CMD_LLAPT_SCENARIO_CHOOSE_INFO:
    case CMD_LLAPT_SCENARIO_CHOOSE_TRY:
    case CMD_LLAPT_SCENARIO_CHOOSE_RESULT:
#endif
    case CMD_APT_GET_POWER_ON_DELAY_TIME:
    case CMD_APT_SET_POWER_ON_DELAY_TIME:
    case CMD_APT_TYPE_QUERY:
    case CMD_ASSIGN_APT_TYPE:
        {
            app_apt_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_BRIGHTNESS_SUPPORT
    case CMD_LLAPT_BRIGHTNESS_INFO:
    case CMD_LLAPT_BRIGHTNESS_SET:
    case CMD_LLAPT_BRIGHTNESS_STATUS:
        {
            app_apt_brightness_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx,
                                              ack_pkt);
        }
        break;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
    case CMD_LISTENING_STATE_SET:
    case CMD_LISTENING_STATE_STATUS:
        {
            app_listening_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx,
                                         ack_pkt);
        }
        break;
#endif

#if F_APP_PBAP_CMD_SUPPORT
    case CMD_PBAP_DOWNLOAD:
    case CMD_PBAP_DOWNLOAD_CONTROL:
    case CMD_PBAP_DOWNLOAD_GET_SIZE:
        {
            app_pbap_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_DURIAN_SUPPORT
    case CMD_AVP_LD_EN:
    case CMD_AVP_ANC_SETTINGS:
    case CMD_AVP_ANC_CYCLE_SET:
    case CMD_AVP_CLICK_SET:
    case CMD_AVP_CONTROL_SET:
        {
            app_durian_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_ASSIGN_BUFFER_SIZE:
    case CMD_TONE_GEN:
    case CMD_STRING_MODE:
    case CMD_SET_AND_READ_DLPS:
#if F_APP_BLE_ANCS_CLIENT_SUPPORT
    case CMD_ANCS_REGISTER:
    case CMD_ANCS_GET_NOTIFICATION_ATTR:
#endif
    case CMD_LED_TEST:
    case CMD_SWITCH_TO_HCI_DOWNLOAD_MODE:
#if F_APP_ADC_SUPPORT
    case CMD_GET_PAD_VOLTAGE:
#endif
    case CMD_RF_XTAK_K:
    case CMD_RF_XTAL_K_GET_RESULT:
#if F_APP_ANC_SUPPORT
    case CMD_ANC_VENDOR_COMMAND:
#endif
#if F_APP_OTA_TOOLING_SUPPORT
    case CMD_OTA_TOOLING_PARKING:
#endif
    case CMD_MEMORY_DUMP:
    case CMD_MP_TEST:
        {
            app_cmd_other_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

#if F_APP_ONE_WIRE_UART_SUPPORT
    case CMD_FORCE_ENGAGE:
    case CMD_AGING_TEST_CONTROL:
    case CMD_ADP_CMD_CONTROL:
        {
            app_one_wire_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if 1
    case CMD_IO_PIN_PULL_HIGH:
    case CMD_ENTER_BAT_OFF_MODE:
#if F_APP_DUAL_AUDIO_EFFECT
    case CMD_CUSTOMIZED_SITRON_FEATURE:
#endif
#if F_APP_SENSOR_MEMS_SUPPORT
    case CMD_GET_IMU_SENSOR_DATA:
#endif
#if GFPS_SASS_SUPPORT
    case CMD_SASS_FEATURE:
#endif
    case CMD_MIC_MP_VERIFY_BY_HFP:
        {
            app_cmd_customized_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

        /* BBPro2 specialized feature */
#if F_APP_LOCAL_PLAYBACK_SUPPORT
    case CMD_PLAYBACK_QUERY_INFO:
    case CMD_PLAYBACK_GET_LIST_DATA:
    case CMD_PLAYBACK_TRANS_START:
    case CMD_PLAYBACK_TRANS_CONTINUE:
    case CMD_PLAYBACK_REPORT_BUFFER_CHECK:
    case CMD_PLAYBACK_VALID_SONG:
    case CMD_PLAYBACK_TRIGGER_ROLE_SWAP:
    case CMD_PLAYBACK_TRANS_CANCEL:
    case CMD_PLAYBACK_PERMANENT_DELETE_SONG:
    case CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG:
    case CMD_PLAYBACK_PLAYLIST_ADD_SONG:
    case CMD_PLAYBACK_PLAYLIST_DELETE_SONG:
    case CMD_PLAYBACK_EXIT_TRANS:
        {
            app_playback_trans_cmd_handle(cmd_len, cmd_ptr, app_idx);
        }
        break;
#endif

#if F_APP_SAIYAN_EQ_FITTING
    case CMD_HW_EQ_TEST_MODE:
    case CMD_HW_EQ_START_SET:
    case CMD_HW_EQ_CONTINUE_SET:
    case CMD_HW_EQ_CLEAR_CALIBRATE_FLAG:
    case CMD_HW_EQ_SET_TEST_MODE_TMP_EQ:
    case CMD_HW_EQ_SET_TEST_MODE_TMP_EQ_ADVANCED:
        {
            app_eq_fitting_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if 0
    case CMD_DSP_DEBUG_SIGNAL_IN:
        {
            if ((cmd_len - 2) != DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                uint8_t buf[4 + DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN];
                uint16_t payload_len = 4 + DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN;

                memcpy(buf + 4, &cmd_ptr[2], DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN);

                buf[0] = CFG_SET_DSP_DEBUG_SIGNAL_IN & 0xFF;
                buf[1] = (CFG_SET_DSP_DEBUG_SIGNAL_IN & 0xFF00) >> 8;
                buf[2] = (payload_len / 4) & 0xFF;
                buf[3] = ((payload_len / 4) & 0xFF00) >> 8;

                audio_probe_dsp_send(buf, payload_len);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC,
                                                    buf, payload_len);
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_SAVE_DATA_TO_FLASH:
        {
            uint8_t type = 0x10;
            uint32_t flash_addr = 0;
            uint32_t data_length = 0;
            uint8_t *data_payload = NULL;
            uint8_t status = 0;
            uint8_t bp_level = 0;

            memcpy(&type, &cmd_ptr[2], 1);
            memcpy(&flash_addr, &cmd_ptr[3], 4);
            memcpy(&data_length, &cmd_ptr[7], 2);
            if (data_length > 0 && data_length < (cmd_len - SAVE_DATA_TO_FLASH_CMD_EVENET_OP_CODE_LEN) &&
                (flash_addr % FLASH_BLOCK_SIZE == 0))
            {
                data_payload = (uint8_t *)malloc(data_length * sizeof(uint8_t));
                if (data_payload != NULL)
                {
                    memcpy(data_payload, &cmd_ptr[9], data_length);

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    bool ret_flash_get = fmc_flash_nor_get_bp_lv(flash_addr, &bp_level);
                    bool ret_flash_set = false;
                    bool ret_flash_erase = false;
                    if (type == ERASE_BEFORE_WRITE)
                    {
                        if (ret_flash_get)
                        {
                            ret_flash_set = fmc_flash_nor_set_bp_lv(flash_addr, 0);
                            ret_flash_erase = fmc_flash_nor_erase(flash_addr, FMC_FLASH_NOR_ERASE_SECTOR);
                            if (ret_flash_set && ret_flash_erase)
                            {
                                if (fmc_flash_nor_write(flash_addr, data_payload, data_length) != true)
                                {
                                    status = SAVE_FLASH_FAIL;
                                }
                                app_report_event(cmd_path, EVENT_SAVE_DATA_TO_FLASH, app_idx, &status, sizeof(status));
                            }
                        }
                    }
                    APP_PRINT_INFO4("save data to flash type is : %02x, flash get bp is %d, set bp is %d, erase is %d",
                                    type, ret_flash_get, ret_flash_set, ret_flash_erase);
                    fmc_flash_nor_set_bp_lv(flash_addr, bp_level);
                    free(data_payload);
                }
            }
        }
        break;
#endif

#if F_APP_HEARABLE_SUPPORT
    case CMD_HA_ACCESS_PROGRAM:
    case CMD_HA_SPK_RESPONSE:
    case CMD_HA_GET_DSP_CFG_GAIN:
    case CMD_HA_AUDIO_VOLUME_GET:
        {
            if (app_cfg_const.enable_ha)
            {
                app_ha_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
            }
        }
        break;
#endif /*F_APP_HEARABLE_SUPPORT */
        /* end of BBPro2 specialized feature */

#if F_APP_SS_SUPPORT
    case CMD_SS_REQ:
        {
            app_ss_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_SAIYAN_MODE
    case CMD_SAIYAN_GAIN_CTL:
        {
            app_data_capture_gain_ctl(&cmd_ptr[2], cmd_len - 2, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_LEA_REALCAST_SUPPORT
    case CMD_LEA_SYNC_BIS:
    case CMD_LEA_SYNC_CANCEL:
    case CMD_LEA_SCAN_START:
    case CMD_LEA_SCAN_STOP:
    case CMD_LEA_PA_START:
    case CMD_LEA_PA_STOP:
    case CMD_LEA_CTRL_VOL:
    case CMD_LEA_GET_DEVICE_STATE:
        {
            app_lea_realcast_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    default:
        {
            ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    }

    APP_PRINT_TRACE1("app_cmd_handler: ack 0x%02x", ack_pkt[2]);
}

#endif
