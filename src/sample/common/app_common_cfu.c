/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_ms_cfu->c
   * @brief     Source file for using cfu
   * @author    Michael
   * @date      2020-11-25
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#if F_APP_CFU_FEATURE_SUPPORT
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "stdlib_corecrt.h"
#include "trace.h"
#include "app_cfg.h"
#include "fmc_api.h"
#include "app_common_cfu.h"
#include "app_timer.h"
#include "patch_header_check.h"
#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
#include "app_bt_policy_api.h"
#include "app_sniff_mode.h"
#include "app_ota.h"
#endif
#include "ota_ext.h"
#include "wdg.h"
#include "cache.h"
#if F_APP_TEAMS_CFU_SUPPORT
#include "CryptoInterface.h"
#include "CryptoLibECC.h"
#endif
#if F_APP_USB_HID_SUPPORT
#include "usb_hid.h"
#endif

#define READ_BACK_BUFFER_SIZE           64
#define NOT_READY_OFFSET                308
#define CFU_PACKET_SIZE                 52
typedef enum t_cfu_timer
{
    APP_TIMER_CFU_CONTENT_TRANS,
    APP_TIMER_CFU_RESET,
    APP_TIMER_CFU_OFFER_TO_CONTENT,
    APP_TIMER_CFU_B2X_SYNC,
    APP_TIMER_CFU_DISC_B2S,
} T_CFU_TIMER;

typedef enum t_component_firmware_update
{
    COMPONENT_FIRMWARE_UPDATE_FLAG_DEFAULT = 0x00,
    COMPONENT_FIRMWARE_UPDATE_FLAG_FIRST_BLOCK = 0x80,        // Denotes the first block of a firmware payload.
    COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK = 0x40,         // Denotes the last block of a firmware payload.
    COMPONENT_FIRMWARE_UPDATE_FLAG_VERIFY = 0x08,             // If set, the firmware verifies the byte array in the upper block at the specified address.
} T_COMPONENT_FIRMWARE_UPDATE;

typedef enum t_fw_offer_status
{
    // The offer needs to be skipped at this time indicating to
    // the host to please offer again during next applicable period.
    FIRMWARE_UPDATE_OFFER_SKIP = 0x00,

    // Once FIRMWARE_UPDATE_FLAG_LAST_BLOCK has been issued,
    // the accessory can then determine if the offer contents
    // apply to it.
    FIRMWARE_UPDATE_OFFER_ACCEPT = 0x01,

    // Once FIRMWARE_UPDATE_FLAG_LAST_BLOCK has been issued,
    // the accessory can then determine if the offer block contents apply to it.
    FIRMWARE_UPDATE_OFFER_REJECT = 0x02,

    // The offer needs to be delayed at this time.  The device has
    // nowhere to put the incoming blob.
    FIRMWARE_UPDATE_OFFER_BUSY = 0x03,

    // Used with the Offer Other response for the OFFER_NOTIFY_ON_READY
    // request, when the Accessory is ready to accept additional Offers.
    FIRMWARE_UPDATE_OFFER_COMMAND_READY = 0x04,

    // Response applicable to when the Offer request is not recognized.
    FIRMWARE_UPDATE_CMD_NOT_SUPPORTED = 0xFF
} T_FW_OFFER_STATUS;

typedef enum t_fw_update_offer_reject_reason
{
    // The offer was rejected by the product due to the offer
    // version being older than the currently downloaded / existing firmware.
    FIRMWARE_OFFER_REJECT_OLD_FW = 0x00, //The offer was rejected by the product due to the offer version being older than the currently downloaded / existing firmware.

    // The offer was rejected due to it not being applicable to
    // the product?s primary MCU(Component ID).
    FIRMWARE_OFFER_REJECT_INV_COMPONENT = 0x01,

    // MCU Firmware has been updated and a swap is currently pending.
    // No further Firmware Update processing can occur until the
    // target has been reset.
    FIRMWARE_UPDATE_OFFER_SWAP_PENDING = 0x02,

    // The offer was rejected due to a Version mismatch(Debug / Release for example)
    FIRMWARE_OFFER_REJECT_MISMATCH = 0x03,

    // The bank being offered for the component is currently in use.
    FIRMWARE_OFFER_REJECT_BANK = 0x04,

    // The offer's Platform ID does not correlate to the receiving
    // hardware product.
    FIRMWARE_OFFER_REJECT_PLATFORM = 0x05,

    // The offer's Milestone does not correlate to the receiving
    // hardware's Build ID.
    FIRMWARE_OFFER_REJECT_MILESTONE = 0x06,

    // The offer indicates an interface Protocol Revision that
    // the receiving product does not support.
    FIRMWARE_OFFER_REJECT_INV_PCOL_REV = 0x07,

    // The combination of Milestone & Compatibility Variants Mask did
    // not match the HW.
    FIRMWARE_OFFER_REJECT_VARIANT = 0x08
} T_FW_UPDATE_OFFER_REJECT_REASON;

typedef enum t_component_firmware_update_payload_response
{
    COMPONENT_FIRMWARE_UPDATE_SUCCESS = 0x00,                             // No Error, the requested function(s) succeeded.
    COMPONENT_FIRMWARE_UPDATE_ERROR_PREPARE = 0x01,                       // Could not either: 1) Erase the upper block; 2) Initialize the swap command scratch block; 3) Copy the configuration data to the upper block.
    COMPONENT_FIRMWARE_UPDATE_ERROR_WRITE = 0x02,                         // Could not write the bytes.
    COMPONENT_FIRMWARE_UPDATE_ERROR_COMPLETE = 0x03,                      // Could not set up the swap, in response to COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK.
    COMPONENT_FIRMWARE_UPDATE_ERROR_VERIFY = 0x04,                        // Verification of the DWord failed, in response to COMPONENT_FIRMWARE_UPDATE_FLAG_VERIFY.
    COMPONENT_FIRMWARE_UPDATE_ERROR_CRC = 0x05,                           // CRC of the image failed, in response to COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK.
    COMPONENT_FIRMWARE_UPDATE_ERROR_SIGNATURE = 0x06,                     // Firmware signature verification failed, in response to COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK.
    COMPONENT_FIRMWARE_UPDATE_ERROR_VERSION = 0x07,                       // Firmware version verification failed, in response to COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK.
    COMPONENT_FIRMWARE_UPDATE_ERROR_SWAP_PENDING = 0x08,                  // Firmware has already been updated and a swap is pending.  No further Firmware Update commands can be accepted until the device has been reset.
    COMPONENT_FIRMWARE_UPDATE_ERROR_INVALID_ADDR = 0x09,                  // Firmware has detected an invalid destination address within the message data content.
    COMPONENT_FIRMWARE_UPDATE_ERROR_NO_OFFER = 0x0A,                      // The Firmware Update Content Command was received without first receiving a valid & accepted FW Update Offer.
    COMPONENT_FIRMWARE_UPDATE_ERROR_INVALID = 0x0B                        // General error for the Firmware Update Content command, such as an invalid applicable Data Length.
} T_COMPONENT_FIRMWARE_UPDATE_PAYLOAD_RESPONSE;

typedef enum t_content_status
{
    CFU_CONTENT_IDLE,
    CFU_CONTENT_RECEIVE_CFU_HEADER,
    CFU_CONTENT_RECEIVE_IMG_HEADER,
    CFU_CONTENT_RECEIVE_IMG_BLOCKS,
} T_CONTENT_STATUS;

typedef enum t_cfu_remote_msg
{
    APP_REMOTE_MSG_CFU_ACK                    = 0x00,
    APP_REMOTE_MSG_CFU_P2S_OFFER              = 0x01,
    APP_REMOTE_MSG_CFU_P2S_CONTENT            = 0x02,
    APP_REMOTE_MSG_CFU_S2P_OFFER_RSP          = 0x03,
    APP_REMOTE_MSG_CFU_S2P_CONTENT_RSP        = 0x04,

    APP_REMOTE_MSG_CFU_TOTAL                  = 0x05,
} T_CFU_REMOTE_MSG;

typedef union t_cfu_offer_version
{
    uint8_t value[4];
    struct
    {
        uint8_t signer: 2;
        uint8_t build_type: 2;
        uint8_t rsvd0: 3;
        uint8_t official: 1;
        uint8_t minor_ver[2];
        uint8_t major_ver;
    } ver;
} T_CFU_OFFER_VERSION;

typedef struct t_fw_update_standard_offer
{
    uint8_t rsvd0;
    uint8_t rsvd1: 6;
    uint8_t force_reset: 1;
    uint8_t force_ignore: 1;
    uint8_t component_id;
    uint8_t token;
    T_CFU_OFFER_VERSION version;
    uint16_t last_image_id;
    uint8_t bud_role: 2;
    uint8_t force_ignore_platform_id: 1;
    uint8_t rsvd2: 5;
    uint8_t rsvd3;
    uint8_t cfu_ver: 4;
    uint8_t bank: 2;
    uint8_t rsvd4: 2;
    uint8_t milestone: 4;
    uint8_t sfua_ver: 4;
    uint16_t platform_id;
} T_FW_UPDATE_STANDARD_OFFER;

typedef struct t_fw_update_special_offer
{
    uint8_t cmdCode;
    uint8_t rsvd0;
    uint8_t component_id;
    uint8_t token;
    uint8_t rsvd1[12];
} T_FW_UPDATE_SPECIAL_OFFER;


typedef struct t_fw_update_content_command
{
    uint8_t flags;
    uint8_t length;
    uint16_t seq_num;
    uint8_t offset[4];
    uint8_t data[52];
} T_FW_UPDATE_CONTENT_COMMAND;

static uint8_t timer_idx_cfu_reset = 0;
static uint8_t timer_idx_cfu_content_trans = 0;
static uint8_t timer_idx_cfu_offer_to_content = 0;
static uint8_t timer_idx_cfu_b2x_sync = 0;

static uint8_t cfu_timer_id = 0;

T_CFU_STRUCT *ms_cfu = NULL;

#if F_APP_TEAMS_CFU_SUPPORT
HASH_CONTEXT hash_context;
uint8_t digest[HASH_LEN];
uint8_t cfu_header[CFU_HEADER_LEN];
#else
#pragma pack(1)
typedef struct _CFU_HEADER
{
    uint16_t image_id;
    uint32_t version;
    uint32_t end_addr;
    uint32_t length;
} CFU_HEADER;
#pragma pack()
#endif

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
typedef void (* CFU_CBACK)(T_APP_BR_LINK *p_link);
CFU_CBACK p_cfu_start = NULL;
CFU_CBACK p_cfu_finish = NULL;
static uint8_t timer_idx_cfu_disc_b2s = 0;
#endif

#if F_APP_RWS_CFU_SUPPORT

#endif /*F_APP_RWS_CFU_SUPPORT*/

#if F_APP_TEAMS_CFU_SUPPORT
static uint8_t app_cfu_offer_version_cmp(T_FW_UPDATE_STANDARD_OFFER *offer_cmd,
                                         T_CFU_OFFER_VERSION *devVer)
{
    uint8_t ver_cmp = 0;

    if (offer_cmd->version.ver.major_ver != devVer->ver.major_ver)
    {
        ver_cmp = (offer_cmd->version.ver.major_ver > devVer->ver.major_ver) ?
                  BUILD_VERSION_HIGH : BUILD_VERSION_LOW;
    }
    else if (*(uint16_t *)offer_cmd->version.ver.minor_ver != *(uint16_t *)devVer->ver.minor_ver)
    {
        ver_cmp = (*(uint16_t *)offer_cmd->version.ver.minor_ver > *(uint16_t *)devVer->ver.minor_ver) ?
                  BUILD_VERSION_HIGH : BUILD_VERSION_LOW;
    }
    else
    {
        ver_cmp = BUILD_VERSION_SAME;
    }

    return ver_cmp;
}

static bool app_cfu_offer_sign_cmp(T_CFU_OFFER_VERSION *offer, T_CFU_OFFER_VERSION *dev)
{
    if ((offer->ver.signer != BUILD_SIGNING_TEST && offer->ver.signer != BUILD_SIGNING_MSFT)
        || (dev->ver.signer != BUILD_SIGNING_TEST && dev->ver.signer != BUILD_SIGNING_MSFT))
    {
        APP_PRINT_ERROR2("app_cfu_offer_sign_cmp signer err: %d - %d", offer->ver.signer,
                         dev->ver.signer);
        return false;
    }

    if (dev->ver.signer == BUILD_SIGNING_MSFT)
    {
        if (offer->ver.signer != BUILD_SIGNING_MSFT)
        {
            APP_PRINT_ERROR1("app_cfu_offer_sign_cmp signer low: %d", offer->ver.signer);
            return false;
        }
    }

    return true;
}
#endif

static uint32_t app_cfu_write_data_to_flash(uint16_t img_id, uint32_t offset, uint32_t length,
                                            void *p_void)
{
    uint32_t dfu_base_addr;
    uint8_t readback_buffer[READ_BACK_BUFFER_SIZE];
    uint32_t read_back_len;
    uint32_t dest_addr;
    uint8_t *p_src = (uint8_t *)p_void;
    uint32_t remain_size = length;

    if (p_void == 0)
    {
        return __LINE__;
    }

    dfu_base_addr = app_get_inactive_bank_addr(img_id);

    if (dfu_base_addr == 0)
    {
        return __LINE__;
    }

    /*cfu packet is 52 bytes. not ready bit offset in image header is 308 bytes*/
    if (offset == ((NOT_READY_OFFSET / CFU_PACKET_SIZE) * CFU_PACKET_SIZE))
    {
        uint8_t *not_ready = (uint8_t *)p_void + NOT_READY_OFFSET % CFU_PACKET_SIZE;
        *not_ready = (*not_ready) | 0x80;
        APP_PRINT_TRACE1("==>dfu_write_data_to_flash:set not ready bit. img_id 0x%x",
                         *((uint16_t *)((uint8_t *)p_void + NOT_READY_OFFSET % CFU_PACKET_SIZE + 2)));
    }

    if (((offset + dfu_base_addr) % FMC_SEC_SECTION_LEN) == 0)
    {
        fmc_flash_nor_erase(dfu_base_addr + offset, FMC_FLASH_NOR_ERASE_SECTOR);
    }
    else
    {
        if (((offset + dfu_base_addr) / FMC_SEC_SECTION_LEN) != ((offset + length + dfu_base_addr) /
                                                                 FMC_SEC_SECTION_LEN))
        {
            if ((offset + length + dfu_base_addr) % FMC_SEC_SECTION_LEN)
            {
                fmc_flash_nor_erase((dfu_base_addr + offset + length) & ~(FMC_SEC_SECTION_LEN - 1),
                                    FMC_FLASH_NOR_ERASE_SECTOR);
            }
        }
    }

    dest_addr = dfu_base_addr + offset;
    fmc_flash_nor_write(dest_addr, p_void, length);

    cache_flush_by_addr((uint32_t *)dest_addr, length);

    while (remain_size)
    {
        read_back_len = (remain_size >= READ_BACK_BUFFER_SIZE) ? READ_BACK_BUFFER_SIZE : remain_size;

        fmc_flash_nor_read(dest_addr, readback_buffer, read_back_len);
        if (memcmp(readback_buffer, p_src, read_back_len) != 0)
        {
            return __LINE__;
        }

        dest_addr += read_back_len;
        p_src += read_back_len;
        remain_size -= read_back_len;
    }

    APP_PRINT_TRACE1("app_cfu_write_to_flash dest_addr: %x", dfu_base_addr + offset);
    return 0;
}

/**
    * @brief    check the integrity of the image
    * @param    image_id    image id
    * @param    offset  address offset
    * @return   ture:success ; false: fail
    */
static bool app_cfu_checksum(IMG_ID image_id, uint32_t offset)
{
    uint32_t base_addr = 0;
    bool ret = false;

    base_addr = app_get_inactive_bank_addr(image_id);

    if (base_addr == 0)
    {
        return false;
    }

    base_addr += offset ;
    ret = check_image_sum(base_addr);

    if (ret == true && !is_ota_support_bank_switch())
    {
        app_ota_set_ready(base_addr);
        //btaon_fast_write_safe(BTAON_FAST_AON_GPR_15, image_id);
    }

    APP_PRINT_TRACE2("app_cfu_checksum base_addr:%x, ret:%d", base_addr, ret);

    return ret;
}

static void app_cfu_clear_local(void)
{
    ms_cfu->cur_img_offset = 0;
    ms_cfu->cur_img_total_len = 0;
    ms_cfu->cur_img_id = 0;
    ms_cfu->cfu_header_offset = 0;
}

static void app_cfu_error_handle(void)
{
    APP_PRINT_TRACE0("app_cfu_error_handle");
    if (ms_cfu->bp_level)
    {
        fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                                ms_cfu->bp_level);
    }

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
    if (p_cfu_finish)
    {
        p_cfu_finish(ms_cfu->p_link);
    }
    app_sniff_mode_b2s_enable(ms_cfu->p_link->bd_addr, SNIFF_DISABLE_MASK_OTA);
#endif

#if F_APP_TEAMS_CFU_SUPPORT
    memset((uint8_t *)&cfu_header, 0, CFU_HEADER_LEN);
#endif
    memset(ms_cfu, 0, sizeof(T_CFU_STRUCT));
    free(ms_cfu);
    ms_cfu = NULL;
}

static void app_cfu_content_update_init(void)
{
#if F_APP_TEAMS_CFU_SUPPORT
    CfuHashInit(&hash_context);
    memset((uint8_t *)&cfu_header, 0, CFU_HEADER_LEN);
#endif
    app_cfu_clear_local();
    ms_cfu->flag.cfu_in_process = 1;
}

static uint8_t app_cfu_get_bank_index(void)
{
    uint32_t ota_bank0_addr;
    uint8_t bank_num;

    if (!is_ota_support_bank_switch())
    {
        return MS_CFU_NOT_SUPPORT_BANK_SWITCH;
    }

    ota_bank0_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
#if F_APP_TEAMS_CFU_SUPPORT
    bank_num = (ota_bank0_addr == get_active_ota_bank_addr()) ?
               MS_CFU_IMAGE_LOCATION_BANK1 : MS_CFU_IMAGE_LOCATION_BANK0;
#else
    bank_num = (ota_bank0_addr == get_active_ota_bank_addr()) ?
               MS_CFU_IMAGE_LOCATION_BANK0 : MS_CFU_IMAGE_LOCATION_BANK1;
#endif
    APP_PRINT_TRACE1("app_cfu_get_bank_index: %d", bank_num);

    return bank_num;
}

static uint32_t app_cfu_get_version(void)
{
    T_IMG_HEADER_FORMAT *header_addr;

    header_addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_OTA);

    APP_PRINT_TRACE1("app_cfu_get_version: %x", header_addr->ver_val);

    return header_addr->ver_val;
}

static uint16_t app_cfu_get_platform_id(void)
{
    uint16_t platform_id = 0;
    uint32_t ota_header_addr = get_active_ota_bank_addr();
    fmc_flash_nor_read(ota_header_addr + CFU_PLATFORM_ID_OFFSET, (uint8_t *)&platform_id, 2);
    return platform_id;
}

static uint8_t app_cfu_get_component_id()
{
    uint8_t component_id = 0;
    uint32_t ota_header_addr = get_active_ota_bank_addr();
    fmc_flash_nor_read(ota_header_addr + CFU_COMPONENT_ID_OFFSET, &component_id, 1);
    return component_id;
}

static void app_cfu_special_offer_parse(uint8_t *data, FW_UPDATE_OFFER_RESPONSE *rsp)
{
    T_FW_UPDATE_SPECIAL_OFFER *p_cmd = (T_FW_UPDATE_SPECIAL_OFFER *)data;

    APP_PRINT_TRACE2("app_cfu_special_offer_parse component_id: %x cmdCode: %x",
                     p_cmd->component_id, p_cmd->cmdCode);

    rsp->status = FIRMWARE_UPDATE_OFFER_ACCEPT;

    if (p_cmd->component_id == 0xFE)
    {
        if (p_cmd->cmdCode == CFU_SPECIAL_OFFER_NOTIFY_ON_READY)
        {
            rsp->status = FIRMWARE_UPDATE_OFFER_COMMAND_READY;
        }
    }
    else if (p_cmd->component_id == 0xFF)
    {
        //rsvd add later
    }
}

static void app_cfu_standard_offer_parse(uint8_t *data, FW_UPDATE_OFFER_RESPONSE *rsp)
{
    T_FW_UPDATE_STANDARD_OFFER *offer_cmd = (T_FW_UPDATE_STANDARD_OFFER *)data;

    APP_PRINT_TRACE4("app_cfu_standard_offer_parse: component_id: %x verison: %x bank: %d milestine: %d",
                     offer_cmd->component_id, *(uint32_t *)offer_cmd->version.value, offer_cmd->bank,
                     offer_cmd->milestone);
#if F_APP_TEAMS_CFU_SUPPORT
    if (offer_cmd->bank != app_cfu_get_bank_index())
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_BANK;
        APP_PRINT_ERROR1("app_cfu_standard_offer_parse: bank num error: %d", offer_cmd->bank);
        return;
    }
#else
    if (offer_cmd->bank == app_cfu_get_bank_index())
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_BANK;
        APP_PRINT_ERROR1("app_cfu_standard_offer_parse: bank num error: %d", offer_cmd->bank);
        return;
    }
    if (offer_cmd->force_ignore_platform_id == 0)
    {
        uint16_t platform_id = app_cfu_get_platform_id();
        if (offer_cmd->platform_id != platform_id)
        {
            rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
            rsp->rejectReason = FIRMWARE_OFFER_REJECT_PLATFORM;
            APP_PRINT_ERROR2("app_cfu_standard_offer_parse platform id error: 0x%x, soc 0x%x",
                             offer_cmd->platform_id, platform_id);
            return;
        }
    }
#endif /*F_APP_TEAMS_CFU_SUPPORT*/

    if (ms_cfu->flag.bank_swap_pending)
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_UPDATE_OFFER_SWAP_PENDING;
        return;
    }

    if (offer_cmd->milestone != CFU_MILESTONE_NUM)
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_MILESTONE;
        APP_PRINT_ERROR1("app_cfu_standard_offer_parse milestone error: %d", offer_cmd->milestone);
        return;
    }

    if (offer_cmd->cfu_ver != CFU_PROTOCOL_VER ||
        offer_cmd->sfua_ver != CFU_SFUA_VER)
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_INV_PCOL_REV;
        return;
    }
    T_CFU_OFFER_VERSION version = {0};
    *(uint32_t *)version.value = app_cfu_get_version();
#if F_APP_TEAMS_CFU_SUPPORT
    /*if (!(offer_cmd->variantMask[CFU_VARIANT_MASK / 8] & (0x01 << CFU_VARIANT_MASK % 8)))
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_VARIANT;
        return;
    }*/

    if (app_cfu_offer_sign_cmp(&offer_cmd->version, &version) == false)
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_MISMATCH;
        return;
    }

    ms_cfu->ver_cmp = app_cfu_offer_version_cmp(offer_cmd, &version);

    if ((offer_cmd->version.ver.build_type != BUILD_TYPE_RELEASE &&
         offer_cmd->version.ver.build_type != BUILD_TYPE_DEBUG)
        || (version.ver.build_type != BUILD_TYPE_RELEASE && version.ver.build_type != BUILD_TYPE_DEBUG))
    {
        APP_PRINT_ERROR2("app_cfu_standard_offer_parse type err: %d - %d",
                         offer_cmd->version.ver.signer, version.ver.signer);
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_MISMATCH;
        return;
    }

    if (version.ver.build_type == BUILD_TYPE_RELEASE)
    {
        if (offer_cmd->version.ver.build_type == BUILD_TYPE_RELEASE)
        {
            if (ms_cfu->ver_cmp != BUILD_VERSION_HIGH)
            {
                rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
                rsp->rejectReason = FIRMWARE_OFFER_REJECT_OLD_FW;
                return;
            }
        }
        else if (offer_cmd->version.ver.build_type == BUILD_TYPE_DEBUG)
        {
            if (ms_cfu->ver_cmp != BUILD_VERSION_SAME)
            {
                rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
                rsp->rejectReason = FIRMWARE_OFFER_REJECT_OLD_FW;
                return;
            }

            ms_cfu->flag.ignore_ver = 1;
        }
    }
    else if (version.ver.build_type == BUILD_TYPE_DEBUG)
    {
        if (ms_cfu->ver_cmp != BUILD_VERSION_HIGH)
        {
            if (offer_cmd->force_ignore)
            {
                ms_cfu->flag.ignore_ver = 1;
            }
            else
            {
                rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
                rsp->rejectReason = FIRMWARE_OFFER_REJECT_OLD_FW;
                return;
            }
        }
    }

    ms_cfu->flag.force_reset = offer_cmd->force_reset;
    ms_cfu->signing = offer_cmd->version.ver.signer;
#else
    ms_cfu->flag.force_reset = offer_cmd->force_reset;
    ms_cfu->flag.ignore_ver = offer_cmd->force_ignore;
    if ((offer_cmd->force_ignore == 0) &&
        (*(uint32_t *)version.value >= *(uint32_t *)offer_cmd->version.value))
    {
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_OLD_FW;
        return;
    }

#ifndef F_APP_CFU_RTL8773DO_DONGLE
    if (app_cfg_const.bud_role != offer_cmd->bud_role)
    {
        APP_PRINT_ERROR2("app_cfu_standard_offer_parse bud role mismatch SOC %d pack %d",
                         app_cfg_const.bud_role, offer_cmd->bud_role);
        rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        rsp->rejectReason = FIRMWARE_OFFER_REJECT_MISMATCH;
    }
#endif
#endif

    fmc_flash_nor_get_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                            &ms_cfu->bp_level);
    fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0), 0);
    app_start_timer(&timer_idx_cfu_offer_to_content, "cfu_offer_to_content",
                    cfu_timer_id, APP_TIMER_CFU_OFFER_TO_CONTENT, 0, false,
                    5000);
}

static void app_cfu_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_cfu_timeout_cb: timer id 0x%02X ch 0x%x", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_CFU_CONTENT_TRANS:
        {
            app_stop_timer(&timer_idx_cfu_content_trans);
            app_cfu_error_handle();
        }
        break;
    case APP_TIMER_CFU_RESET:
        {
            app_stop_timer(&timer_idx_cfu_reset);
            chip_reset(RESET_ALL_EXCEPT_AON);
        }
        break;
    case APP_TIMER_CFU_OFFER_TO_CONTENT:
        {
            app_stop_timer(&timer_idx_cfu_offer_to_content);
            app_cfu_error_handle();
        }
        break;
    case APP_TIMER_CFU_B2X_SYNC:
        {
            app_stop_timer(&timer_idx_cfu_b2x_sync);
            app_cfu_error_handle();
        }
        break;

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
    case APP_TIMER_CFU_DISC_B2S:
        {
            app_stop_timer(&timer_idx_cfu_disc_b2s);
            app_ota_disc_b2s_profile();
            app_start_timer(&timer_idx_cfu_reset, "cfu_reset",
                            cfu_timer_id, APP_TIMER_CFU_RESET, 0, 0,
                            3000);
        }
        break;
#endif

    default:
        break;
    }
}

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
static void app_cfu_cback_register(CFU_CBACK a, CFU_CBACK b)
{
    p_cfu_start = a;
    p_cfu_finish = b;
}

static void app_cfu_handle_start(T_APP_BR_LINK *p_link)
{
    app_bt_policy_qos_param_update(p_link->bd_addr, BP_TPOLL_CFU_START_EVENT);
}

static void app_cfu_handle_end(T_APP_BR_LINK *p_link)
{
    app_bt_policy_qos_param_update(p_link->bd_addr, BP_TPOLL_CFU_END_EVENT);
}
#endif

void app_cfu_init(void)
{
    app_timer_reg_cb(app_cfu_timeout_cb, &cfu_timer_id);

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
    app_cfu_cback_register(app_cfu_handle_start, app_cfu_handle_end);
#endif

#if F_APP_RWS_CFU_SUPPORT
    app_relay_cback_register(app_cfu_relay_cback, app_cfu_parse_cback,
                             APP_MODULE_TYPE_CFU, APP_REMOTE_MSG_CFU_TOTAL);
#endif

#ifndef F_APP_CFU_RTL8773DO_DONGLE
#if F_APP_USB_HID_SUPPORT
    T_HID_CBS cbs = {.get_report = (UINT_IN_FUNC)app_cfu_get_report, .set_report = (UINT_OUT_FUNC)app_cfu_set_report};
    usb_hid_ual_register(cbs);
#endif
#endif
}

void app_cfu_get_version_parse(FW_UPDATE_VER_RESPONSE *ver_rsp)
{
    if (ver_rsp == NULL)
    {
        return;
    }
    ver_rsp->component_count = CFU_COMPONENT_COUNT;
    ver_rsp->cfu_ver = CFU_PROTOCOL_VER;
    ver_rsp->component_info[0].version = app_cfu_get_version();
    ver_rsp->component_info[0].platform_id = app_cfu_get_platform_id();
    ver_rsp->component_info[0].component_id = app_cfu_get_component_id();
    ver_rsp->component_info[0].bank = app_cfu_get_bank_index();

#ifndef F_APP_CFU_RTL8773DO_DONGLE
    ver_rsp->component_info[0].bud_role = app_cfg_const.bud_role;
#endif

    APP_PRINT_TRACE2("app_cfu_get_version_parse platform_id: %x component_id: %x",
                     ver_rsp->component_info[0].platform_id, ver_rsp->component_info[0].component_id);

#if APP_TEAMS_CFU_SUPPORT_EXTERNAL_DSP
    GetCfuInfo(&ver_rsp->component_info[1]);
#endif
}

T_CFU_RSP_TYPE app_cfu_offer_parse(uint8_t *data, uint16_t length,
                                   FW_UPDATE_OFFER_RESPONSE *offer_rsp)
{
    uint8_t component_id = data[2];
    uint8_t token = data[3];
    if (ms_cfu == NULL)
    {
        ms_cfu = (T_CFU_STRUCT *)malloc(sizeof(T_CFU_STRUCT));
        memset(ms_cfu, 0, sizeof(T_CFU_STRUCT));
    }

    ms_cfu->cur_component_id = component_id;

#if APP_TEAMS_CFU_SUPPORT_EXTERNAL_DSP
    if (component_id == CFU_DSP_COMPONENT_ID)
    {
        uint8_t data_buffer[17];
        data_buffer[0] = REPORT_ID_CFU_OFFER;
        memcpy_s(data_buffer, sizeof(data_buffer), data, length);
        CfuSendOffer(data_buffer, sizeof(data_buffer));
        app_start_timer(&timer_idx_cfu_b2x_sync, "cfu_b2x_sync",
                        cfu_timer_id, APP_TIMER_CFU_B2X_SYNC, 0, false,
                        5000);

        return CFU_OTH_COMPONENT_RSP;
    }
#endif

    APP_PRINT_TRACE2("app_cfu_offer_parse component_id: %x token: %x", component_id, token);

    offer_rsp->status = FIRMWARE_UPDATE_OFFER_ACCEPT;
    offer_rsp->token = token;

    if (ms_cfu->flag.cfu_in_process)
    {
        offer_rsp->status = FIRMWARE_UPDATE_OFFER_BUSY;
        return CFU_CUR_COMPONENT_RSP;
    }

    if (component_id == CFU_PRI_COMPONENT_ID || component_id == CFU_DONGLE_COMPONENT_ID)
    {
        app_cfu_standard_offer_parse(data, offer_rsp);
    }
    else if (component_id >= CFU_SPECIAL_OFFER_CMD)
    {
        app_cfu_special_offer_parse(data, offer_rsp);
    }
    else
    {
        offer_rsp->status = FIRMWARE_UPDATE_OFFER_REJECT;
        offer_rsp->rejectReason = FIRMWARE_OFFER_REJECT_INV_COMPONENT;
    }

    return CFU_CUR_COMPONENT_RSP;
}

T_CFU_RSP_TYPE app_cfu_content_parse(uint8_t *data, uint8_t length,
                                     FW_UPDATE_CONTENT_RESPONSE *content_rsp)
{
    T_FW_UPDATE_CONTENT_COMMAND *content_cmd = (T_FW_UPDATE_CONTENT_COMMAND *)data;
#if APP_TEAMS_CFU_SUPPORT_EXTERNAL_DSP
    else if (ms_cfu->cur_component_id == CFU_DSP_COMPONENT_ID)
    {
        uint8_t data_buffer[61];
        data_buffer[0] = REPORT_ID_CFU_FEATURE;
        memcpy_s(data_buffer, sizeof(data_buffer), data, length);
        CfuSendContent(data_buffer, sizeof(data_buffer));
        return CFU_OTH_COMPONENT_RSP;
    }
#endif

    content_rsp->seq_num = content_cmd->seq_num;
    content_rsp->status = COMPONENT_FIRMWARE_UPDATE_SUCCESS;

    APP_PRINT_TRACE4("app_cfu_content_parse flag: %x, seq: %d, len:%x, status:%d",
                     content_cmd->flags, content_cmd->seq_num, content_cmd->length, ms_cfu->content_state);

    if (ms_cfu->last_seq_num == content_cmd->seq_num && content_cmd->seq_num)
    {
        return CFU_CUR_COMPONENT_RSP;
    }

    if ((ms_cfu->last_seq_num + 1 != content_cmd->seq_num) && content_cmd->seq_num)
    {
        content_rsp->status = COMPONENT_FIRMWARE_UPDATE_ERROR_PREPARE;
        return CFU_CUR_COMPONENT_RSP;
    }

#if !APP_TEAMS_SUPPORT_SIGNED_IMAGE /*when update no authencated images*/
    if (content_cmd->flags & FIRMWARE_UPDATE_FLAG_FIRST_BLOCK)
    {
        app_cfu_content_update_init();

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
        app_sniff_mode_b2s_disable(ms_cfu->p_link->bd_addr, SNIFF_DISABLE_MASK_OTA);
        if (p_cfu_start)
        {
            p_cfu_start(ms_cfu->p_link);
        }
#endif

#if F_APP_TEAMS_CFU_SUPPORT
        ms_cfu->content_state = CFU_CONTENT_RECEIVE_IMG_HEADER;
#else
        ms_cfu->content_state = CFU_CONTENT_RECEIVE_CFU_HEADER;
#endif
    }
#endif

    ms_cfu->last_seq_num = content_cmd->seq_num;
    if (content_cmd->seq_num == 0)
    {
        app_stop_timer(&timer_idx_cfu_offer_to_content);
    }
    app_start_timer(&timer_idx_cfu_content_trans, "cfu_content_trans",
                    cfu_timer_id, APP_TIMER_CFU_CONTENT_TRANS, 0, false,
                    30000);

    switch (ms_cfu->content_state)
    {
#if F_APP_TEAMS_CFU_SUPPORT
    case CFU_CONTENT_IDLE:
        {
            if (content_cmd->flags & FIRMWARE_UPDATE_FLAG_FIRST_BLOCK)
            {
                app_cfu_content_update_init();

                if (p_cfu_start)
                {
                    p_cfu_start(ms_cfu->p_link);
                }

                memcpy_s((uint8_t *)&cfu_header, CFU_HEADER_LEN, content_cmd->data, content_cmd->length);

                ms_cfu->cfu_header_offset = content_cmd->length;
                ms_cfu->content_state = CFU_CONTENT_RECEIVE_CFU_HEADER;
            }
        }
        break;
#endif

    case CFU_CONTENT_RECEIVE_CFU_HEADER:
        {
#if F_APP_TEAMS_CFU_SUPPORT
            uint32_t write_len = content_cmd->length;

            if (ms_cfu->cfu_header_offset >= ((CFU_HEADER *)cfu_header)->cfuInfoOffset)
            {
                CfuHashUpdate(&hash_context, content_cmd->data, content_cmd->length);
            }
            else if (ms_cfu->cfu_header_offset + content_cmd->length > ((CFU_HEADER *)
                                                                        cfu_header)->cfuInfoOffset)
            {
                uint32_t len = ms_cfu->cfu_header_offset + content_cmd->length - ((CFU_HEADER *)
                                                                                  cfu_header)->cfuInfoOffset;
                CfuHashUpdate(&hash_context, content_cmd->data + content_cmd->length - len, len);
            }

            if (ms_cfu->cfu_header_offset + content_cmd->length > CFU_HEADER_LEN)
            {
                write_len = CFU_HEADER_LEN - ms_cfu->cfu_header_offset;
            }

            memcpy((uint8_t *)&cfu_header + ms_cfu->cfu_header_offset, content_cmd->data, write_len);
            ms_cfu->cfu_header_offset += write_len;

            if (ms_cfu->cfu_header_offset >= CFU_HEADER_LEN)
            {
                ms_cfu->content_state = CFU_CONTENT_RECEIVE_IMG_BLOCKS;
                ms_cfu->cfu_header_offset = 0;
            }
#else
            CFU_HEADER *header = (CFU_HEADER *)content_cmd->data;
            ms_cfu->cur_img_id = header->image_id;
            ms_cfu->cur_img_total_len = header->length;
            APP_PRINT_TRACE2("app_cfu_content_parse imag_id: %x, imag_len: %x", ms_cfu->cur_img_id,
                             ms_cfu->cur_img_total_len);
            ms_cfu->content_state = CFU_CONTENT_RECEIVE_IMG_BLOCKS;

#endif
        }
        break;
    case CFU_CONTENT_RECEIVE_IMG_BLOCKS:
        {
            uint32_t write_len = content_cmd->length;

            APP_PRINT_TRACE3("app_cfu_content_parse: img_id: %x, offset: %x, total_len: %x ",
                             ms_cfu->cur_img_id, ms_cfu->cur_img_offset, ms_cfu->cur_img_total_len);

#if APP_TEAMS_SUPPORT_SIGNED_IMAGE
            CfuHashUpdate(&hash_context, content_cmd->data, content_cmd->length);
#endif

            if (ms_cfu->cur_img_offset + content_cmd->length > ms_cfu->cur_img_total_len)
            {
                write_len = ms_cfu->cur_img_total_len - ms_cfu->cur_img_offset;
            }
            uint32_t ret = app_cfu_write_data_to_flash(ms_cfu->cur_img_id, ms_cfu->cur_img_offset, write_len,
                                                       content_cmd->data);
            APP_PRINT_TRACE1("app_cfu_content_parse: write to flash ret %d", ret);
            if (ret)
            {
                content_rsp->status = COMPONENT_FIRMWARE_UPDATE_ERROR_WRITE;
                app_cfu_error_handle();
                break;
            }

            ms_cfu->cur_img_offset += write_len;

            if (ms_cfu->cur_img_offset == ms_cfu->cur_img_total_len)
            {
                if (app_cfu_checksum((IMG_ID)ms_cfu->cur_img_id, 0) == true)
                {
                    APP_PRINT_TRACE1("app_cfu_content_parse: valid success img_id: %x", ms_cfu->cur_img_id);

                    app_cfu_clear_local();
                    if (content_cmd->flags & COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK)
                    {
                        app_stop_timer(&timer_idx_cfu_content_trans);

#if APP_TEAMS_SUPPORT_SIGNED_IMAGE
                        CfuHashFinalize(&hash_context, digest);
                        if (VerifyHash(digest, sizeof(digest), cfu_header + sizeof(CFU_HEADER)) != CERT_STATUS_SUCCESS)
                        {
                            content_rsp->status = COMPONENT_FIRMWARE_UPDATE_ERROR_SIGNATURE;
                            app_cfu_error_handle();
                            return CFU_CUR_COMPONENT_RSP;
                        }
#endif

                        app_ota_clear_notready_flag();

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
                        if (p_cfu_finish)
                        {
                            p_cfu_finish(ms_cfu->p_link);
                        }
                        app_sniff_mode_b2s_enable(ms_cfu->p_link->bd_addr, SNIFF_DISABLE_MASK_OTA);
#endif

                        ms_cfu->flag.bank_swap_pending = true;
                        ms_cfu->flag.cfu_in_process = 0;
                        ms_cfu->content_state = CFU_CONTENT_IDLE;

                        if (ms_cfu->flag.ignore_ver)
                        {
                            uint32_t base_addr = 0;

                            APP_PRINT_TRACE0("app_cfu_content_parse: ignore version");

                            base_addr = get_header_addr_by_img_id(IMG_SBL);
                            dfu_erase_img_flash_area(base_addr, DEFAULT_HEADER_SIZE);
                        }

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
                        if (app_cfg_store_all() != 0)
                        {
                            APP_PRINT_ERROR0("app_cfu_content_parse: save nv cfg data error");
                        }
#endif
                        if (ms_cfu->bp_level)
                        {
                            fmc_flash_nor_set_bp_lv(flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0),
                                                    ms_cfu->bp_level);
                        }
                        if (ms_cfu->flag.force_reset)
                        {
#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
                            app_start_timer(&timer_idx_cfu_disc_b2s, "cfu_disc_b2s",
                                            cfu_timer_id, APP_TIMER_CFU_DISC_B2S, 0, 0,
                                            3000);
#else
                            app_start_timer(&timer_idx_cfu_reset, "cfu_reset",
                                            cfu_timer_id, APP_TIMER_CFU_RESET, 0, 0,
                                            3000);
#endif
                        }
                        free(ms_cfu);
                        ms_cfu = NULL;
                    }
                    else
                    {
                        ms_cfu->content_state = CFU_CONTENT_RECEIVE_CFU_HEADER;
                    }
                }
                else
                {
                    APP_PRINT_ERROR0("app_cfu_content_parse: checksum error");
                    content_rsp->status = COMPONENT_FIRMWARE_UPDATE_ERROR_CRC;
                    app_cfu_error_handle();
                }
            }
        }
        break;
    default:
        {
            app_cfu_error_handle();
        }
        break;
    }

    return CFU_CUR_COMPONENT_RSP;
}

#if (F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
void app_cfu_save_link_info(uint8_t *bd_addr)
{
    if (ms_cfu == NULL)
    {
        ms_cfu = (T_CFU_STRUCT *)malloc(sizeof(T_CFU_STRUCT));
        memset(ms_cfu, 0, sizeof(T_CFU_STRUCT));
    }

    ms_cfu->p_link = app_link_find_b2s_link(bd_addr);
}

bool app_cfu_is_process_check(T_APP_BR_LINK **p_link)
{
    *p_link = ms_cfu->p_link;
    return ms_cfu->flag.cfu_in_process;
}
#endif

bool app_cfu_is_in_process(void)
{
    return ms_cfu->flag.cfu_in_process;
}

#endif /*F_APP_CFU_FEATURE_SUPPORT*/
