#ifndef  RTK_TUYA_BLE_OTA_H_
#define  RTK_TUYA_BLE_OTA_H_

#include <string.h>
#include <stdint.h>
#include "app_msg.h"
#include "patch_header_check.h"

#define TUYA_OTA_VERSION                3
#define TUYA_OTA_TYPE                   0
#define TUYA_OTA_FILE_MD5_LEN           16

#define OTA_ROLE_RELATED_SUB_IMAGES_COUNT_MAX    2
#define OTA_BANK_RELATED_SUB_IMAGES_COUNT_MAX    5
#define OTA_NONE_RELATED_SUB_IMAGES_COUNT_MAX    4

#define OTA_SUB_BIN_COUNT_MAX               2 * (OTA_ROLE_RELATED_SUB_IMAGES_COUNT_MAX + OTA_BANK_RELATED_SUB_IMAGES_COUNT_MAX + OTA_NONE_RELATED_SUB_IMAGES_COUNT_MAX)

#define OTA_MAIN_HEADER_SIZE            20
#define OTA_CONTROL_HEADER_SIZE         sizeof(T_IMG_CTRL_HEADER_FORMAT)
#define OTA_AUTH_HEADER_SIZE            sizeof(T_AUTH_HEADER_FORMAT)
#define OTA_AUTH_CONTROL_HEADER_SIZE    OTA_CONTROL_HEADER_SIZE + OTA_AUTH_HEADER_SIZE

typedef enum t_tuya_ota_sub_state
{
    TUYA_OTA_SUB_STATE_RX_MAIN_HEADER,
    TUYA_OTA_SUB_STATE_RX_BIN0_SUB_IMAGE_HEADER,
    TUYA_OTA_SUB_STATE_RX_BIN1_SUB_IMAGE_HEADER,

//#if F_APP_HARMAN_FEATURE_SUPPORT
    TUYA_OTA_SUB_STATE_RX_AUTH_HEADER,
//#endif

    TUYA_OTA_SUB_STATE_RX_AUTH_CTRL_HEADER,
    TUYA_OTA_SUB_STATE_RX_IMAGE_PAYLOAD,
} T_TUYA_OTA_SUB_STATE;

typedef struct t_main_header
{
    uint16_t signature; // const signature: 0x9697b5c8
    uint16_t version; //version of combine spec
    uint32_t file_size;

    uint8_t num_role_related;
    uint8_t num_bank_related;
    uint8_t num_none_related;
    uint8_t num_sub_image;

    uint16_t ota_version;
    uint8_t is_rws;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint8_t is_vp_only: 1;
    uint8_t byte_rsv: 7;
    uint8_t ota_version_ext;
#else
    uint8_t rsv[2];
#endif

    uint8_t num_sub_image_each_bin; //addr count
    uint8_t cur_index;
    uint8_t end_index; // role+bank+none
} T_MAIN_HEADER;

typedef struct t_sub_image_header
{
    uint16_t image_id;
    uint8_t bud_role;
    uint8_t bank_info;
    uint32_t download_addr;
    uint32_t size;
    uint8_t rsv[4];
} T_SUB_IMAGE_HEADER; //16 bytes * 22

typedef struct t_sub_bin_info
{
    T_SUB_IMAGE_HEADER sub_image_header[OTA_SUB_BIN_COUNT_MAX];
} T_SUB_BIN_INFO;

typedef struct t_tuya_ota_rws_mgr
{
    bool update_result_local;
    bool update_result_remote;

    uint8_t local_active_bank;
    uint8_t remote_active_bank;

    T_TUYA_OTA_SUB_STATE sub_state;
    uint8_t target_sub_bin_index;

    uint8_t index_in_image_info_list;
    uint8_t cur_download_image_index;
    uint8_t end_download_image_index;
    uint32_t cur_sub_image_relative_offset;

    T_MAIN_HEADER main_header;
    T_SUB_BIN_INFO sub_bin[2];
} T_TUYA_OTA_RWS_MGR;

typedef enum t_tuya_ota_status
{
    TUYA_OTA_STATUS_NONE,
    TUYA_OTA_STATUS_FILE_INFO,
    TUYA_OTA_STATUS_FILE_OFFSET,
    TUYA_OTA_STATUS_FILE_DATA,
    TUYA_OTA_STATUS_SECONDARY_UNDERGOING,
    TUYA_OTA_STATUS_MAX,
} T_TUYA_OTA_STATUS;

typedef enum t_tuya_ota_handle_file_data_result
{
    TUYA_OTA_HANDLE_FILE_DATA_SUCCESS,
    TUYA_OTA_HANDLE_FILE_DATA_PKT_NUM_ERR,
    TUYA_OTA_HANDLE_FILE_DATA_LEN_ERR,
    TUYA_OTA_HANDLE_FILE_DATA_CRC_ERR,
    TUYA_OTA_HANDLE_FILE_DATA_FLASH_ERR,
} T_TUYA_OTA_HANDLE_FILE_DATA_RESULT;

bool tuya_ota_send_event_msg(uint16_t cmd, uint8_t *p_data, uint16_t len);
void tuya_ota_handle_msg(T_IO_MSG io_msg);
uint8_t tuya_ota_is_busy(void);
void tuya_ota_exit(void);
uint32_t tuya_ble_ota_init(void);

#endif /* RTK_TUYA_BLE_OTA_H_ */
