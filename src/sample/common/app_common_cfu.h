/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     app_cfu.h
  * @brief
  * @details
  * @author   michael_lin
  * @date     2020-9-1
  * @version  v1.0
  * *************************************************************************************
  */

#ifndef _APP_COMMON_CFU_H_
#define _APP_COMMON_CFU_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "app_cfu_transfer.h"

#if F_APP_TEAMS_CFU_SUPPORT
#include "CfuInfoStructs.h"
#endif
#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
#include "app_link_util.h"
#endif

/** @defgroup APP_COMMON_CFU APP COMMON CFU
    * @brief app common cfu api
    * @{
    */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup APP_COMMON_CFU_Exported_Macros APP COMMON CFU Exported Macros
    * @brief
    * @{
    */

#define MS_CFU_IMAGE_LOCATION_BANK0            0
#define MS_CFU_IMAGE_LOCATION_BANK1            1
#define MS_CFU_NOT_SUPPORT_BANK_SWITCH         3

#define MAX_ACCEPTED_OFFER_NUM                 2

#define OFFER_PROCESS_NOT_GOT           0
#define OFFER_PROCESS_SKIPPED           1
#define OFFER_PROCESS_REJECTED          2
#define OFFER_PROCESS_ACCEPTED          3

#define CFU_CONTENT_DATA_LENGTH         52

// NOTE - defines should match CFU Protocol Spec definitions
#define CFU_OFFER_METADATA_INFO_CMD                        (0xFF)
#define CFU_SPECIAL_OFFER_CMD                              (0xFE)
#define CFU_SPECIAL_OFFER_GET_STATUS                       (0x03)
#define CFU_SPECIAL_OFFER_NONCE                            (0x02)
#define CFU_SPECIAL_OFFER_NOTIFY_ON_READY                  (0x01)
#define CFW_UPDATE_PACKET_MAX_LENGTH                       (sizeof(FW_UPDATE_CONTENT_COMMAND))
#define FIRMWARE_OFFER_TOKEN_DRIVER                        (0xA0)
#define FIRMWARE_OFFER_TOKEN_SPEEDFLASHER                  (0xB0)
#define FIRMWARE_UPDATE_FLAG_FIRST_BLOCK                   (0x80)
#define FIRMWARE_UPDATE_FLAG_LAST_BLOCK                    (0x40)
#define FIRMWARE_UPDATE_FLAG_VERIFY                        (0x08)
#define FIRMWARE_UPDATE_STATUS_ERROR_COMPLETE              (0x03)
#define FIRMWARE_UPDATE_STATUS_ERROR_CRC                   (0x05)
#define FIRMWARE_UPDATE_STATUS_ERROR_INVALID               (0x0B)
#define FIRMWARE_UPDATE_STATUS_ERROR_INVALID_ADDR          (0x09)
#define FIRMWARE_UPDATE_STATUS_ERROR_NO_OFFER              (0x0A)
#define FIRMWARE_UPDATE_STATUS_ERROR_PENDING               (0x08)
#define FIRMWARE_UPDATE_STATUS_ERROR_PREPARE               (0x01)
#define FIRMWARE_UPDATE_STATUS_ERROR_SIGNATURE             (0x06)
#define FIRMWARE_UPDATE_STATUS_ERROR_VERIFY                (0x04)
#define FIRMWARE_UPDATE_STATUS_ERROR_VERSION               (0x07)
#define FIRMWARE_UPDATE_STATUS_ERROR_WRITE                 (0x02)
#define FIRMWARE_UPDATE_STATUS_SUCCESS                     (0x00)

#define OFFER_INFO_START_ENTIRE_TRANSACTION                (0x00)
#define OFFER_INFO_START_OFFER_LIST                        (0x01)
#define OFFER_INFO_END_OFFER_LIST                          (0x02)

#define APP_TEAMS_CFU_SUPPORT_EXTERNAL_DSP      0
#define CFU_PLATFORM_ID_OFFSET 1022
#define CFU_COMPONENT_ID_OFFSET 1010

#if F_APP_TEAMS_CFU_SUPPORT
#define APP_TEAMS_SUPPORT_SIGNED_IMAGE          1
#define  CFU_MILESTONE_NUM  6
#define  CFU_PLATFORM_ID    0x4C
#define  CFU_VARIANT_MASK   1
#define  CFU_PRI_COMPONENT_ID   0x10
#define  CFU_DSP_COMPONENT_ID   0x11
#define  CFU_SEC_COMPONENT_ID   0x12
#define  CFU_MILESTONE_COUNT  2
#else
#define APP_TEAMS_SUPPORT_SIGNED_IMAGE          0
#define  CFU_MILESTONE_NUM  0
#define  CFU_PLATFORM_ID    0
#define  CFU_PRI_COMPONENT_ID      0xa1
#define  CFU_DONGLE_COMPONENT_ID   0xa2
#endif

#define BUILD_TYPE_DEBUG                                   0
#define BUILD_TYPE_RELEASE                                 2

#define BUILD_SIGNING_TEST                                 1
#define BUILD_SIGNING_MSFT                                 3

#define BUILD_VERSION_IGNORE                               0
#define BUILD_VERSION_HIGH                                 1
#define BUILD_VERSION_SAME                                 2
#define BUILD_VERSION_LOW                                  3

#define PACKET_SIZE         0x34
#define CFU_HEADER_CONTENT_LEN  (sizeof(T_CFU_HEADER) + 1) / PACKET_SIZE * PACKET_SIZE
#define CFU_HEADER_LEN      0x66c
#define CFU_SINATURE_LEN   CFU_HEADER_LEN - sizeof(CFU_HEADER) - sizeof(CFU_INFO) - sizeof(CFU_HW_COMPAT) - sizeof(CFU_PKI)

#define  CFU_PROTOCOL_VER   4
#define  CFU_SFUA_VER       0

#if APP_TEAMS_CFU_SUPPORT_EXTERNAL_DSP
#define  CFU_COMPONENT_COUNT  0x2
#else
#define  CFU_COMPONENT_COUNT  0x1
#endif

#define MAX_BUFFER_SIZE     4096

#define CFU_HEADER_OFFSET   0

#define REPORT_ID_LENGTH    0x01

#define CFU_OFFER_RESPONSE_LENGTH_BYTES             16
#define HID_CFU_OFFER_RESPONSE_LENGTH_BYTES         (CFU_OFFER_RESPONSE_LENGTH_BYTES + REPORT_ID_LENGTH)

#define CFU_CONTENT_RESPONSE_LENGTH_BYTES           16
#define HID_CFU_CONTENT_RESPONSE_LENGTH_BYTES       (CFU_CONTENT_RESPONSE_LENGTH_BYTES + REPORT_ID_LENGTH)

/** End of APP_COMMON_CFU_Exported_Macros
    * @}
    */

typedef enum t_cfu_rsp_type
{
    CFU_CUR_COMPONENT_RSP,
    CFU_OTH_COMPONENT_RSP,
} T_CFU_RSP_TYPE;

typedef union t_cfu_flag
{
    uint8_t value;
    struct
    {
        uint8_t cfu_in_process: 1;
        uint8_t bank_swap_pending: 1;
        uint8_t force_reset: 1;
        uint8_t ignore_ver: 1;
        uint8_t is_sec: 1;
        uint8_t rsvd: 3;
    };
} T_CFU_FLAG;

typedef struct t_cfu_struct
{
    uint32_t cur_img_offset;
    uint32_t cur_img_total_len;
    uint32_t cfu_header_offset;
#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
    T_APP_BR_LINK *p_link;
#endif
    uint16_t last_seq_num;
    uint16_t cur_img_id;
    uint8_t content_state;
    uint8_t signing;
    uint8_t ver_cmp;
    uint8_t cur_component_id;
    uint8_t cur_hid_source_type;
    uint8_t report_type;
    T_APP_CFU_REPORT_ACTION_TYPE report_action;
    T_CFU_FLAG flag;
    uint8_t bp_level;
} T_CFU_STRUCT;

extern T_CFU_STRUCT *ms_cfu;
typedef struct _FW_UPDATE_VER_RESPONSE
{
    uint8_t component_count;
    uint8_t rsvd0[2];
    uint8_t cfu_ver: 4;
    uint8_t rsvd1: 3;
    uint8_t extension: 1;

    struct
    {
        uint32_t version;
        uint8_t bank: 2;
        uint8_t bud_role: 2; //bud role:0x00 single,0x01 pri, 0x02 sec
        uint8_t milestone: 4;
        uint8_t component_id;
        uint16_t platform_id;
    } component_info[7];
} FW_UPDATE_VER_RESPONSE;

typedef struct _FW_UPDATE_OFFER_RESPONSE
{
    uint8_t rsvd0[3];
    uint8_t token;
    uint8_t challenge[4];
    uint8_t rejectReason;
    uint8_t rsvd1[3];
    uint8_t status;
    uint8_t rsvd2[3];
} FW_UPDATE_OFFER_RESPONSE;

typedef struct _FW_UPDATE_CONTENT_RESPONSE
{
    uint16_t seq_num;
    uint16_t rsvd0;
    uint8_t status;
    uint8_t rsvd1[11];
} FW_UPDATE_CONTENT_RESPONSE;

/*============================================================================*
 *                              Functions
 *============================================================================*/

/** @defgroup APP_COMMON_CFU_Exported_Functions APP COMMON CFU Exported Functions
    * @brief
    * @{
    */

/**
    * @brief initialize app cfu.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  void
    * @return void
    */
void app_cfu_init(void);

/**
    * @brief parse get version report.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  ver_rsp version response
    * @return void
    */
void app_cfu_get_version_parse(FW_UPDATE_VER_RESPONSE *ver_rsp);

/**
    * @brief parse offer.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param data offer data
    * @param length offer length
    * @param offer_rsp offer response
    * @return current component response or not
    */
T_CFU_RSP_TYPE app_cfu_offer_parse(uint8_t *data, uint16_t length,
                                   FW_UPDATE_OFFER_RESPONSE *offer_rsp);

/**
    * @brief parse content.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param data content data
    * @param length content length
    * @param content_rsp content response
    * @return current component response or not
    */
T_CFU_RSP_TYPE app_cfu_content_parse(uint8_t *data, uint8_t length,
                                     FW_UPDATE_CONTENT_RESPONSE *content_rsp);

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
/**
    * @brief save link info.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  bd_addr bd address
    * @return current component response or not
    */
void app_cfu_save_link_info(uint8_t *bd_addr);

/**
    * @brief check wether app cfu is in process.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  p_link b2s link
    * @return True: is doing cfu; False: is not doing cfu
    */
bool app_cfu_is_process_check(T_APP_BR_LINK **p_link);
#endif

/**
    * @brief check wether app cfu is in process.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  void
    * @return True: is doing cfu; False: is not doing cfu
    */
bool app_cfu_is_in_process(void);

/** @} */ /* End of group APP_COMMON_CFU_Exported_Functions */
/** @} */ /* End of group APP_COMMON_CFU */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_COMMON_CFU_H_ */
