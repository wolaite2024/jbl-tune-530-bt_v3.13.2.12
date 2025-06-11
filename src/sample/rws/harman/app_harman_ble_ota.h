#ifndef _APP_HARMAM_BLE_OTA_H_
#define _APP_HARMAM_BLE_OTA_H_

#include <stdint.h>
#include <stdbool.h>
#include "patch_header_check.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OTA_PACKET_SIZE                                   256
#define OTA_WRITE_BUFFER_SIZE                             (4 * 1024)
#define FLASH_OFFSET_TO_NO_CACHE                          0x01000000
#define IMAGE_LOCATION_BANK0                              1
#define IMAGE_LOCATION_BANK1                              2
#define NOT_SUPPORT_BANK_SWITCH                           3
#define READBACK_BUFFER_SIZE                              64

#define OTA_RECVED_PACKETS_NUM_TO_BE_REPORT               20
#define OTA_PACKAGE_MISSING_CHECK_TOTAL_TIMES             2

/* OTA package header */
typedef struct t_harman_ota_package_header
{
    uint16_t protocol_version;  // Byte1:Byte0    0.1
    uint16_t pid;               // T135 : 0x2135
    uint32_t ota_version;
    uint32_t ota_size;
    uint32_t fw_table_offset;
    uint16_t fw_table_size;
    uint16_t fw_table_items;
    uint8_t compressed_ota : 1;
    uint8_t is_vp_only : 1;
    uint8_t rsvd : 6;
    uint8_t rsvd2[3];
} __attribute__((packed)) T_HARMAN_OTA_PACKAGE_HEADER;

/* OTA FW table */
typedef struct t_harman_ota_fw_table_item
{
    uint16_t  image_id;       // IMG_ID
    uint8_t   fw_minor_type;  // FW_MINOR_TYPE
    uint8_t   need_upgrade;   // always 1
    uint32_t  fw_version;
    uint32_t  fw_offset;
    uint32_t  fw_size;
    uint32_t  download_addr;
    uint8_t   ic_type;       // IC_TYPE
    uint8_t   part_number;   // RTL87X3E_PART_NUMBER / RTL87X3D_PART_NUMBER
    uint16_t  rsvd[14];
} __attribute__((packed)) T_HARMAN_OTA_FW_TABLE_ITEM;

#if HARMAN_COMPRESSED_OTA_SUPPORT
typedef struct _compress_ctrl_header_format
{
    uint8_t img_string[3]; // "COM"
    union
    {
        uint16_t value;
        struct
        {
            uint16_t not_ready: 1; // for copy image in ota, set to 0 when packed OTA bin
            uint16_t not_obsolete: 1; // for copy image in ota, set to 1 when packed OTA bin
            uint16_t rsvd: 14;
        };
    } ctrl_flag;
    uint8_t ic_type;
    uint16_t image_id;
    uint32_t payload_len;
} __attribute__((packed)) T_COMPRESS_CTRL_HEADER_FORMAT;

typedef struct _compress_image_header_format //128 bytes
{
    T_COMPRESS_CTRL_HEADER_FORMAT compress_ctrl_header; // 20bytes
    uint8_t  uuid[16]; /*bb2: 0x03,0x46,0x4e,0xfb,0x92,0x0f,0xec,0x11,0x9b,0xf7,0x67,0x70,0x0e,0x6f,0xe3,0x03*/
    uint32_t version;
    uint32_t compress_algo;
    uint8_t  uncompressed_image_sha256[32];
    uint8_t  compressed_image_sha256[32];
    uint8_t  reverved[28];
} __attribute__((packed)) T_COMPRESS_IMG_HEADER_FORMAT;
#endif

#define OTA_FILE_PACKAGE_HEAD_SIZE                   sizeof(T_HARMAN_OTA_PACKAGE_HEADER)
#define OTA_FILE_FW_TABLE_ITEM_SIZE                  sizeof(T_HARMAN_OTA_FW_TABLE_ITEM)

#define OTA_SUB_IMAGE_CMAC                           16
#define OTA_SUB_IMAGE_SIGNATURE                      256
#define OTA_SUB_IMAGE_HASH                           32
#define OTA_SUB_IMAGE_CTRL_HEADER_OFFSET             (OTA_SUB_IMAGE_CMAC + OTA_SUB_IMAGE_SIGNATURE + OTA_SUB_IMAGE_HASH)
#define OTA_SUB_IMAGE_CONTROL_HEAD_SIZE              sizeof(T_IMG_CTRL_HEADER_FORMAT)
#if HARMAN_COMPRESSED_OTA_SUPPORT
#define COMPRESSED_OTA_SUB_IMAGE_CONTROL_HEAD_SIZE   sizeof(T_COMPRESS_CTRL_HEADER_FORMAT)
#endif
// #define OTA_SUB_IMAGE_VERSION_HEADER_OFFSET       (OTA_SUB_IMAGE_CTRL_HEADER_OFFSET + OTA_SUB_IMAGE_CONTROL_HEAD_SIZE + 0x54)
// #define OTA_SUB_IMAGE_VERSION_HEAD_SIZE           sizeof(T_VERSION_FORMAT)
// #define OTA_AUTH_HEADER_SIZE                      sizeof(T_AUTH_HEADER_FORMAT)

#define OTA_PACKET_CHAEK_TIMEOUT                     3000
#define OTA_PACKET_CHECK_MAX_TIMES                   2

typedef enum _FW_MINOR_TYPE
{
    FW_IMAGE_NON_BANK     = 0x00,
    FW_IMAGE_BANK0        = 0x01,
    FW_IMAGE_BANK1        = 0x02,
} FW_MINOR_TYPE;

typedef enum _rtl87x3d_part_number
{
    RTL8773DO      = 0x00,
    RTL8773DFL     = 0x01,
} RTL87X3D_PART_NUMBER;

typedef enum _rtl87x3e_part_number
{
    RTL8763EFL      = 0x04,
    RTL8763EAU      = 0x07,
    RTL8763EWM      = 0x09,
    RTL8763ESE      = 0x0B,
    RTL8763EF       = 0x0D,
    RTL8773ESL      = 0x0E,
    RTL8763ESH      = 0x20,
    RTL8763EHA      = 0x22,
} RTL87X3E_PART_NUMBER;

typedef enum _harman_ic_type
{
    TYPE_RTL87X3D       = 0x0A,
    TYPE_RTL87X3E       = 0x0B,
    TYPE_RTL87X3G       = 0x0D,
} HARMAN_IC_TYPE;

typedef enum _harman_ota_reset_reason
{
    HARMAN_OTA_COMPLETE             = 0,
    HARMAN_OTA_FLASH_WRITE_FAIL     = 1,
    HARMAN_OTA_IMAGE_CHECK_FAIL     = 2,
    HARMAN_OTA_VP_COPY_FAIL         = 3,
} T_HARMAN_OTA_RESET_REASON;

/* OTA notification */
typedef struct t_app_harman_ota_notification
{
    uint16_t status_code;
    uint16_t percentage;
    uint32_t absolute_offset;
    uint32_t length_to_read;
} T_APP_HARMAN_OTA_NOTIFICATION;

typedef enum
{
    OTA_STATE_PROTOCOL_VERSION      = 0x00,
    OTA_STATE_PACKAGE_HEADER        = 0x01,
    OTA_STATE_FW_TABLE              = 0x02,
    OTA_STATE_IMAGE_CONTROL_HEADER  = 0x03, // T_IMG_CTRL_HEADER_FORMAT
    OTA_STATE_IMAGE_PAYLOAD         = 0x04,
    OTA_STATE_FINISH                = 0x05,
    OTA_STATE_EXIT                  = 0x06,
} T_HARMAN_OTA_STATE;

typedef enum
{
    OTA_EXIT_REASON_NONE                                = 0,
    OTA_EXIT_REASON_OTA_COMMAND                         = 1,
    OTA_EXIT_REASON_BETTERY_LOW                         = 2,
    OTA_EXIT_REASON_CALL                                = 3,
    OTA_EXIT_REASON_OUTBAND_RING                        = 4,
    OTA_EXIT_REASON_AUDIO_STREAMING                     = 5,
    OTA_EXIT_REASON_BLE_DISCONN                         = 6,
    OTA_EXIT_REASON_AI                                  = 7,
    OTA_EXIT_REASON_UNSUPPORTED_PROTOCOL_VERSION        = 8,
    OTA_EXIT_REASON_UNSUPPORTED_OTA_PACKAGE_VERSION     = 9,
    OTA_EXIT_REASON_SECOND_TIMEOUT                      = 10,
    OTA_EXIT_REASON_GENERAL_ERROR                       = 11,
    OTA_EXIT_REASON_LEA_CCP_CALL                        = 12,
} T_HARMAN_OTA_EXIT_REASON;

typedef struct _T_SUB_IMAGE_INFO
{
    T_HARMAN_OTA_PACKAGE_HEADER package_header;
    T_HARMAN_OTA_FW_TABLE_ITEM *fw_table;
} T_HARMAN_SUB_BIN_INFO;

typedef struct T_HARMAN_OTA_MGR
{
    /* recv valid packet count */
    uint8_t recved_packets_count;
    /* lost packet count */
    uint8_t recved_offset_error_retry;
    /* wait next packet 3s timeout, timeout twice would exit OTA */
    uint8_t package_missing_check_times;

    uint8_t count;
    uint8_t index;

    uint32_t start_offset;
    uint32_t total_len_to_read;

    /* the file current offset, which is the start offset of next pack */
    uint32_t cur_offset;
    /* required size of next pack, maybe recved data size is shorter then this */
    uint32_t required_size;
    /* the whole size need OTA */
    uint32_t ota_total_size;

    /* the recved data size which need to write to flash, cannot exceed OTA_WRITE_BUFFER_SIZE */
    uint32_t buf_index;
    T_HARMAN_OTA_STATE state;
    uint8_t     cur_sub_image_index;
    uint8_t     end_sub_image_index;
    /* the relative offset in one sub image */
    uint32_t    cur_sub_image_relative_offset;
    /* the length of data already transmitted */
    uint32_t    total_relative_offset;
    T_HARMAN_SUB_BIN_INFO sub_bin;
} T_HARMAN_OTA_MGR;

void app_harman_ble_ota_init(void);
void app_harman_ota_exit(T_HARMAN_OTA_EXIT_REASON exit_reason);
bool app_harman_ble_ota_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t app_idx);
uint8_t app_harman_ble_ota_get_upgrate_status(void);
void app_harman_ble_ota_set_upgrate_status(uint8_t val);
bool app_harman_ble_ota_get_notify_direct(void);
void app_harman_ble_ota_power_on_handle(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
