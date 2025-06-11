/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CFU_SUPPORT
#include <string.h>
#include <bt_types.h>
#include "trace.h"
#include "dfu_api.h"

#include "app_cfg.h"
#include "os_mem.h"
#include "rtl876x.h"
#include "fmc_api.h"

#include "app_cfu.h"
#include "app_msg.h"

#include "app_timer.h"
#include "rom_uuid.h"
#include "app_ota.h"
#include "remote.h"
#include "app_hid_descriptor.h"
#include "hid_driver.h"
#include "wdg.h"

#pragma pack(1)
typedef union _COMPONENT_VERSION
{
    uint32_t AsUInt32;
    struct
    {
        uint8_t Variant;
        uint16_t MinorVersion;
        uint8_t MajorVersion;
    };
} COMPONENT_VERSION;

typedef union _COMPONENT_PROPERTY
{
    uint32_t AsUInt32;
    struct
    {
        uint8_t Bank : 2;
        uint8_t budRole: 2; //bud role:0x00 single,0x01 pri, 0x02 sec
        uint8_t VendorSpecific0: 4;
        uint8_t ComponentId;
        uint16_t VendorSpecific1;
    };
} COMPONENT_PROPERTY;

typedef struct _COMPONENT_VERSION_AND_PROPERTY
{
    COMPONENT_VERSION ComponentVersion;
    COMPONENT_PROPERTY ComponentProperty;
} COMPONENT_VERSION_AND_PROPERTY;

typedef struct _GET_FWVERSION_RESPONSE
{
    uint8_t ReportId;
    struct
    {
        uint8_t ComponentCount;
        uint16_t Reserved0;
        uint8_t ProtocolRevision : 4;
        uint8_t Reserved1 : 3;
        uint8_t ExtensionFlag : 1;
    } header;
    COMPONENT_VERSION_AND_PROPERTY componentVersionsAndProperty[7];
} GET_FWVERSION_RESPONSE;

typedef struct
{
    uint8_t ReportId;
    struct
    {
        uint8_t SegmentNumber;
        uint8_t Reserved0 : 6;
        uint8_t ForceImmediateReset : 1;
        uint8_t ForceIgnoreVersion : 1;
        uint8_t ComponentId;
        uint8_t Token;
    } ComponentInfo;
    COMPONENT_VERSION Version;
    uint16_t last_img_id;
    uint16_t reverd;
    struct
    {
        uint8_t ProtocolVersion : 4;
        uint8_t bank : 2;
        uint8_t reserved0 : 2;
        uint8_t milestone : 4;
        uint8_t Reserved1 : 4;
        uint16_t VendorSpecific;
    } MiscAndProtocolVersion;
} FWUPDATE_OFFER_COMMAND;

typedef struct
{
    struct
    {
        uint8_t infoCode;
        uint8_t reserved0;
        uint8_t shouldBe0xFF;
        uint8_t token;
    } componentInfo;

    uint32_t reserved0[3];

} FWUPDATE_OFFER_INFO_ONLY_COMMAND;

typedef struct
{
    struct
    {
        uint8_t commandCode;
        uint8_t reserved0;
        uint8_t shouldBe0xFE;
        uint8_t token;
    } componentInfo;

    uint32_t reserved0[3];

} FWUPDATE_SPECIAL_OFFER_COMMAND;

typedef struct _FWUPDATE_CONTENT_COMMAND
{
    uint8_t ReportId;
    uint8_t Flags;
    uint8_t Length;
    uint16_t SequenceNumber;
    uint32_t Address;
    uint8_t Data[52];
} FWUPDATE_CONTENT_COMMAND;

typedef enum _RESPONSE_TYPE
{
    OFFER,
    CONTENT
} RESPONSE_TYPE;

typedef enum _COMPONENT_FIRMWARE_UPDATE_FLAG
{
    COMPONENT_FIRMWARE_UPDATE_FLAG_DEFAULT      = 0x00,
    COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS = 0x01,
    COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_FAILED  = 0x02,
    COMPONENT_FIRMWARE_UPDATE_FLAG_ABORD        = 0x04,
    COMPONENT_FIRMWARE_UPDATE_FLAG_CLEAR        = 0x10,

    COMPONENT_FIRMWARE_UPDATE_FLAG_FIRST_BLOCK  = 0x80,        // Denotes the first block of a firmware payload.
    COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK   = 0x40,         // Denotes the last block of a firmware payload.
    COMPONENT_FIRMWARE_UPDATE_FLAG_VERIFY       = 0x08,             // If set, the firmware verifies the byte array in the upper block at the specified address.
} COMPONENT_FIRMWARE_UPDATE_FLAG;

enum FwUpdateOfferStatus
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
};

enum FwUpdateOfferRejectReason
{
    // The offer was rejected by the product due to the offer
    // version being older than the currently downloaded / existing firmware.
    FIRMWARE_OFFER_REJECT_OLD_FW = 0x00, //The offer was rejected by the product due to the offer version being older than the currently downloaded / existing firmware.

    // The offer was rejected due to it not being applicable to
    // the product?s primary MCU(Component ID).
    FIRMWARE_OFFER_REJECT_INV_MCU = 0x01,

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
};

typedef enum _COMPONENT_FIRMWARE_UPDATE_PAYLOAD_RESPONSE
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
    COMPONENT_FIRMWARE_UPDATE_ERROR_INVALID = 0x0B,                        // General error for the Firmware Update Content command, such as an invalid applicable Data Length.
    COMPONENT_FIRMWARE_UPDATE_ABORTED = 0x0C,
    COMPONENT_FIRMWARE_UPDATE_FLGA_CLEARED = 0x0D,
    COMPONENT_FIRMWARE_UPDATE_BUDS_FAIL_GOT = 0x0E,
    COMPONENT_FIRMWARE_UPDATE_SINGEL_BUD_SUCCESS = 0x0F,
    COMPONENT_FIRMWARE_UPDATE_BUDS_SUCCESS_GOT = 0x10,

} COMPONENT_FIRMWARE_UPDATE_PAYLOAD_RESPONSE;

typedef enum
{
    APP_TIMER_CFU_CONTENT_TRANS,
    APP_TIMER_CFU_RESET_ACTIVE
} T_APP_CFU_TIMER;

#define REPORT_ID_LENGTH            0x01

#define CFU_OFFER_RESPONSE_LENGTH_BYTES             16
#define HID_CFU_OFFER_RESPONSE_LENGTH_BYTES         (CFU_OFFER_RESPONSE_LENGTH_BYTES + REPORT_ID_LENGTH)

typedef union
{
    uint8_t AsBytes[HID_CFU_OFFER_RESPONSE_LENGTH_BYTES];
    struct HidCfuOfferResponse
    {
        uint8_t ReportId;
        struct CfuOfferResponse
        {
            uint8_t Reserved0[3];
            uint8_t Token;
            uint32_t Reserved1;
            uint8_t RejectReasonCode;
            uint8_t Reserved2[3];
            uint8_t Status;
            uint8_t Reserved3[2];
        } CfuOfferResponse;
    } HidCfuOfferResponse;
} FWUPDATE_OFFER_RESPONSE;

#define CFU_CONTENT_RESPONSE_LENGTH_BYTES           16
#define HID_CFU_CONTENT_RESPONSE_LENGTH_BYTES       (CFU_CONTENT_RESPONSE_LENGTH_BYTES + REPORT_ID_LENGTH)

typedef union
{
    uint8_t AsBytes[HID_CFU_CONTENT_RESPONSE_LENGTH_BYTES];
    struct
    {
        struct HidCfuContentResponse
        {
            uint8_t ReportId;
            struct CfuContentResponse
            {
                uint16_t SequenceNumber;
                uint16_t Reserved0;
                uint8_t Status;
                uint8_t Reserved1[3];
                uint32_t Reserved2[1];
            } CfuContentResponse;
        } HidCfuContentResponse;
    };
} FWUPDATE_CONTENT_RESPONSE;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _T_OFFER_INFO
{
    struct
    {
        uint8_t SegmentNumber;
        uint8_t last_offer_process : 6;
        uint8_t ForceImmediateReset : 1;
        uint8_t ForceIgnoreVersion : 1;
        uint8_t ComponentId;
    } ComponentInfo;
} OFFER_INFO;

//typedef struct _T_IMAGE_SEG
//{
//    uint16_t image_id;
////    uint16_t segment;
//    uint32_t max_offset;
//    bool update_result;
//} IMAGE_SEG;

typedef struct _T_CFU_STRUCT
{
    OFFER_INFO offer_info[MAX_ACCEPTED_OFFER_NUM];
    COMPONENT_VERSION update_cfu_version;
    uint8_t update_in_progress: 1;
    uint8_t is_bank_swap_pending: 1;
    uint8_t update_if_segment_verion_equal: 1;
    uint8_t update_result;
//    IMAGE_SEG image_segment[8];
    uint8_t current_flashing_index;
    uint32_t current_flashing_offset;
    uint32_t last_transmit_SequenceNumber;
    uint8_t current_updating_compomentID;
    uint8_t current_updating_immediate_reset;
    uint8_t current_updating_ignore_version;
    uint32_t current_image_ID;
    uint32_t current_image_start_addr;
    uint32_t current_image_end_addr;
    uint32_t current_image_length;
    uint16_t last_update_img_id;
} T_CFU_STRUCT;

T_CFU_STRUCT m_cfu_struct = {0};
bool app_cfu_is_image_signed;
uint8_t app_cfu_response_buffer[17];
static uint8_t app_cfu_timer_id = 0;
static uint8_t timer_idx_cfu_content_trans = 0;
static uint8_t timer_idx_cfu_reset_active = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t app_cfu_get_bank_index(bool active)// 0 inactive; 1 active
{
    if (is_ota_support_bank_switch())
    {
        uint32_t ota_bank0_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
        if (ota_bank0_addr == get_active_ota_bank_addr())
        {
            return active ? APP_CFU_IMAGE_LOCATION_BANK0 : APP_CFU_IMAGE_LOCATION_BANK1;
        }
        else
        {
            return active ? APP_CFU_IMAGE_LOCATION_BANK1 : APP_CFU_IMAGE_LOCATION_BANK0;
        }
    }
    else
    {
        return APP_CFU_NOT_SUPPORT_BANK_SWITCH;
    }
}

uint32_t app_cfu_get_version(IMG_ID img_id, bool active)
{
    uint32_t verion = 0;
//    ftl_load_from_storage(&verion, APP_RW_DATA_ADDR + sizeof(T_APP_CFG_NV), sizeof(uint32_t));
    T_IMG_HEADER_FORMAT *addr;
    if (active)
    {
        addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(img_id);
    }
    else
    {
        addr = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id(img_id);
    }
    if (addr != 0)
    {
//        verion = addr->git_ver.sub_version._version_major << 24 | addr->git_ver.sub_version._version_minor
//                 << 8 | addr->git_ver.sub_version._version_reserve;
        verion = addr->ver_val;
    }
    DFU_PRINT_INFO2("app_cfu_get_cfu_version at addr %x ver %x", addr, verion);
    return verion;
}

//uint32_t app_cfu_set_cfu_version(uint32_t verion)
//{
////    uint32_t result;
////    result = ftl_save_to_storage(&verion, APP_RW_DATA_ADDR + sizeof(T_APP_CFG_NV), sizeof(uint32_t));
//    uint32_t dfu_result = dfu_update(CFU_VERSION_SECTION, CFU_VERSION_OFFSET, 4, &verion);
//    DFU_PRINT_INFO3("app_cfu_set_cfu_version to %x ver %x, result %x", CFU_VERSION_OFFSET, verion,
//                    dfu_result);
//    uint32_t address = get_temp_ota_bank_addr_by_img_id(CFU_VERSION_SECTION);
//    verion = *(uint32_t *)(address + CFU_VERSION_OFFSET);
//    DFU_PRINT_TRACE2("app_cfu_set_cfu_version read back addr %x ver %x", address + CFU_VERSION_OFFSET,
//                     verion);
//    return dfu_result;
//}

void app_cfu_offer_parse(uint8_t *data, uint16_t length)
{
    DFU_PRINT_TRACE1("app_cfu_offer_parse %b", TRACE_BINARY(17, data));
    int i;
    uint8_t offer_process = OFFER_PROCESS_NOT_GOT;
    FWUPDATE_OFFER_COMMAND *offerCommand = (FWUPDATE_OFFER_COMMAND *)data;
    FWUPDATE_OFFER_RESPONSE *offerResponse = (FWUPDATE_OFFER_RESPONSE *)app_cfu_response_buffer;
    offerResponse->HidCfuOfferResponse.ReportId = REPORT_ID_CFU_OFFER_INPUT;
    DFU_PRINT_TRACE8("Received Offer: Component {Id 0x%x, V 0x%x, I 0x%x, Seg 0x%x, Token 0x%x, Bank %d} Version { 0x%x }last_img_id { 0x%x } \n",
                     offerCommand->ComponentInfo.ComponentId,
                     offerCommand->ComponentInfo.ForceIgnoreVersion,
                     offerCommand->ComponentInfo.ForceImmediateReset,
                     offerCommand->ComponentInfo.SegmentNumber,
                     offerCommand->ComponentInfo.Token,
                     offerCommand->MiscAndProtocolVersion.bank,
                     offerCommand->Version.AsUInt32,
                     offerCommand->last_img_id);
    offerResponse->HidCfuOfferResponse.CfuOfferResponse.Token = offerCommand->ComponentInfo.Token;

    if (m_cfu_struct.update_in_progress)
    {
        DFU_PRINT_WARN0("Offer reject update_in_progress");
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_BUSY;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.RejectReasonCode = FIRMWARE_UPDATE_OFFER_BUSY;
        return;
    }
    if (offerCommand->last_img_id < IMG_OCCD || offerCommand->last_img_id > IMAGE_MAX)
    {
        DFU_PRINT_ERROR1("last_img_id = 0x%x error", offerCommand->last_img_id);
        return;
    }
    if (offerCommand->ComponentInfo.ComponentId == CFU_SPECIAL_OFFER_CMD)
    {
        FWUPDATE_SPECIAL_OFFER_COMMAND *pCommand = (FWUPDATE_SPECIAL_OFFER_COMMAND *)offerCommand;
        DFU_PRINT_TRACE1("Received extend Offer: commandCode %x", pCommand->componentInfo.commandCode);
        if (pCommand->componentInfo.commandCode == CFU_SPECIAL_OFFER_GET_STATUS)
        {
            offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_COMMAND_READY;
            return;
        }
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_ACCEPT;
        return;
    }
    if (offerCommand->ComponentInfo.ComponentId == CFU_OFFER_METADATA_INFO_CMD)
    {
        FWUPDATE_OFFER_INFO_ONLY_COMMAND *pCommand = (FWUPDATE_OFFER_INFO_ONLY_COMMAND *)offerCommand;
        DFU_PRINT_TRACE1("Received info Offer: infoCode %x", pCommand->componentInfo.infoCode);
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_ACCEPT;
        return;
    }
    if (m_cfu_struct.is_bank_swap_pending)
    {
        DFU_PRINT_WARN0("Offer reject is_bank_swap_pending");
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_REJECT;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.RejectReasonCode =
            FIRMWARE_UPDATE_OFFER_SWAP_PENDING;
        return;
    }
    i = app_cfu_get_bank_index(0);
    if (offerCommand->MiscAndProtocolVersion.bank != i)
    {
        DFU_PRINT_WARN0("Offer reject FIRMWARE_OFFER_REJECT_BANK");
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_REJECT;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.RejectReasonCode = FIRMWARE_OFFER_REJECT_BANK;
        return;
    }
    for (i = 0; i < MAX_ACCEPTED_OFFER_NUM &&
         m_cfu_struct.offer_info[i].ComponentInfo.ComponentId != 0; i++)
    {
        if (m_cfu_struct.offer_info[i].ComponentInfo.ComponentId == offerCommand->ComponentInfo.ComponentId)
        {
            DFU_PRINT_INFO2("Offer matched i %d, ID %x", i,
                            m_cfu_struct.offer_info[i].ComponentInfo.ComponentId);
            offer_process = m_cfu_struct.offer_info[i].ComponentInfo.last_offer_process;
            m_cfu_struct.last_transmit_SequenceNumber = 0xFFFFFFFF;
            m_cfu_struct.update_result = 1;
            m_cfu_struct.update_in_progress = true;
            app_start_timer(&timer_idx_cfu_content_trans, "cfu_content_trans",
                            app_cfu_timer_queue_id, APP_TIMER_CFU_CONTENT_TRANS, 0, true,
                            3000);
            m_cfu_struct.current_updating_compomentID = offerCommand->ComponentInfo.ComponentId;
            m_cfu_struct.current_updating_immediate_reset = offerCommand->ComponentInfo.ForceImmediateReset;
            break;
        }
    }
    if (offer_process == OFFER_PROCESS_NOT_GOT && i < MAX_ACCEPTED_OFFER_NUM)
    {
        if (offerCommand->Version.AsUInt32 <= app_cfu_get_version(CFU_VERSION_SECTION, true) &&
            !offerCommand->ComponentInfo.ForceIgnoreVersion)
        {
            DFU_PRINT_INFO0("Offer reject FIRMWARE_OFFER_REJECT_OLD_FW");
            offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_REJECT;
            offerResponse->HidCfuOfferResponse.CfuOfferResponse.RejectReasonCode = FIRMWARE_OFFER_REJECT_OLD_FW;
            return;
        }
        if (offerCommand->Version.AsUInt32 < m_cfu_struct.update_cfu_version.AsUInt32)
        {
            DFU_PRINT_INFO0("Offer reject FIRMWARE_OFFER_REJECT_MISMATCH");
            offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_REJECT;
            offerResponse->HidCfuOfferResponse.CfuOfferResponse.RejectReasonCode =
                FIRMWARE_OFFER_REJECT_MISMATCH;
            return;
        }
        DFU_PRINT_INFO4("Offer skipped CID %x, seg %x, ver %x, bank %x",
                        offerCommand->ComponentInfo.ComponentId,
                        offerCommand->ComponentInfo.SegmentNumber, offerCommand->Version.AsUInt32,
                        offerCommand->MiscAndProtocolVersion.bank);
        m_cfu_struct.update_if_segment_verion_equal = true;
        m_cfu_struct.update_cfu_version.AsUInt32 = offerCommand->Version.AsUInt32;
        m_cfu_struct.offer_info[i].ComponentInfo.ComponentId = offerCommand->ComponentInfo.ComponentId;
        m_cfu_struct.offer_info[i].ComponentInfo.ForceIgnoreVersion =
            offerCommand->ComponentInfo.ForceIgnoreVersion;
        m_cfu_struct.offer_info[i].ComponentInfo.ForceImmediateReset =
            offerCommand->ComponentInfo.ForceImmediateReset;
        m_cfu_struct.offer_info[i].ComponentInfo.last_offer_process = OFFER_PROCESS_SKIPPED;
        m_cfu_struct.offer_info[i].ComponentInfo.SegmentNumber = offerCommand->ComponentInfo.SegmentNumber;
        m_cfu_struct.last_update_img_id = offerCommand->last_img_id;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_SKIP;
        return;
    }
    if (offer_process == OFFER_PROCESS_SKIPPED && i < MAX_ACCEPTED_OFFER_NUM)
    {
        DFU_PRINT_INFO4("Offer accepted CID %x, seg %x, ver %x, bank %x",
                        offerCommand->ComponentInfo.ComponentId,
                        offerCommand->ComponentInfo.SegmentNumber, offerCommand->Version.AsUInt32,
                        offerCommand->MiscAndProtocolVersion.bank);
        m_cfu_struct.offer_info[i].ComponentInfo.last_offer_process = OFFER_PROCESS_ACCEPTED;
        m_cfu_struct.current_updating_ignore_version =
            m_cfu_struct.offer_info[i].ComponentInfo.ForceIgnoreVersion;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_ACCEPT;
    }
    if (offer_process == OFFER_PROCESS_ACCEPTED && i < MAX_ACCEPTED_OFFER_NUM)
    {
        DFU_PRINT_WARN4("Offer resend CID %x, seg %x, ver %x, bank %x",
                        offerCommand->ComponentInfo.ComponentId,
                        offerCommand->ComponentInfo.SegmentNumber, offerCommand->Version.AsUInt32,
                        offerCommand->MiscAndProtocolVersion.bank);
        m_cfu_struct.offer_info[i].ComponentInfo.last_offer_process = OFFER_PROCESS_SKIPPED;
        offerResponse->HidCfuOfferResponse.CfuOfferResponse.Status = FIRMWARE_UPDATE_OFFER_SKIP;
    }
}

void app_cfu_error_process()
{
    DFU_PRINT_TRACE0("app_cfu_error_process");
    memset(&m_cfu_struct, 0, sizeof(T_CFU_STRUCT));
}

void app_cfu_content_parse(uint8_t *data, uint16_t length)
{
    uint32_t dfu_result;
    FWUPDATE_CONTENT_RESPONSE *contentResponse = (FWUPDATE_CONTENT_RESPONSE *)app_cfu_response_buffer;
    FWUPDATE_CONTENT_COMMAND *contentCommand = (FWUPDATE_CONTENT_COMMAND *)
                                               data;//app_cfu_receive_buffer;
    DFU_PRINT_TRACE1("app_cfu_content_parse command %b", TRACE_BINARY(9, data));
    DFU_PRINT_TRACE4("Content Received: { SeqNo = 0x%x Addr = 0x%x, L = 0x%x, flag %x}\n",
                     contentCommand->SequenceNumber,
                     contentCommand->Address,
                     contentCommand->Length,
                     contentCommand->Flags);
    //DFU_PRINT_TRACE1("data %b", TRACE_BINARY(contentCommand->Length, contentCommand->Data));
    contentResponse->HidCfuContentResponse.ReportId = REPORT_ID_CFU_PAYLOAD_INPUT;
    contentResponse->HidCfuContentResponse.CfuContentResponse.SequenceNumber =
        contentCommand->SequenceNumber;
    if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS) ==
        COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS)// two buds  success
    {
        m_cfu_struct.current_image_ID =  m_cfu_struct.last_update_img_id ;
        dfu_result = app_ota_checksum(m_cfu_struct.current_image_ID, 0);
        DFU_PRINT_TRACE2("dfu_check_checksum image id %x, dfu_result %x", m_cfu_struct.current_image_ID,
                         dfu_result);
        if (dfu_result == true)// 1 means dfu checksum pass
        {
            m_cfu_struct.current_flashing_offset = 0;
            m_cfu_struct.current_image_length = 0;
            m_cfu_struct.update_result &= dfu_result;
            if (((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS) ==
                 COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS) && m_cfu_struct.update_result)
            {
                app_stop_timer(&timer_idx_cfu_content_trans);
                DFU_PRINT_ERROR0("last block finished, start timer to reset");
                DFU_PRINT_ERROR3("Version from '%x bank %x' to %x",
                                 app_cfu_get_version(CFU_VERSION_SECTION, true), app_cfu_get_bank_index(true),
                                 app_cfu_get_version(CFU_VERSION_SECTION, false));
                DFU_PRINT_ERROR2("Ignore Version %d, Force reset %d", m_cfu_struct.current_updating_ignore_version,
                                 m_cfu_struct.current_updating_immediate_reset);
                if (m_cfu_struct.current_updating_ignore_version &&
                    (app_cfu_get_version(CFU_VERSION_SECTION, true) >= app_cfu_get_version(CFU_VERSION_SECTION, false)))
                {
                    if (is_ota_support_bank_switch())
                    {
                        uint32_t header_addr = get_header_addr_by_img_id(IMG_MCUAPP);
                        DFU_PRINT_ERROR2("Erase sector %x address %x", IMG_MCUAPP, header_addr);
                        dfu_erase_img_flash_area(header_addr, 0x20);
                    }
                }
                m_cfu_struct.update_in_progress = false;
                m_cfu_struct.is_bank_swap_pending = true;
                app_start_timer(&timer_idx_cfu_reset_active, "cfu_reset_active",
                                app_cfu_timer_queue_id, APP_TIMER_CFU_RESET_ACTIVE, 0, false,
                                3000);

            }
            else
            {
                m_cfu_struct.current_image_start_addr = m_cfu_struct.current_image_end_addr +
                                                        CFU_CONTENT_DATA_LENGTH;
                DFU_PRINT_TRACE1("move to next image, start addr %x", m_cfu_struct.current_image_start_addr);
            }
            contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                COMPONENT_FIRMWARE_UPDATE_BUDS_SUCCESS_GOT;
            return;
        }
        else
        {
            DFU_PRINT_ERROR2("dfu_check_checksum image id %x, check fail %x", m_cfu_struct.current_image_ID,
                             dfu_result);
            if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS) ==
                COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_SUCCESS)
            {
                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                    COMPONENT_FIRMWARE_UPDATE_ERROR_VERIFY;
            }
            else
            {
                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                    COMPONENT_FIRMWARE_UPDATE_ERROR_CRC;
            }
        }
        return;

    }
    if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_FAILED) ==
        COMPONENT_FIRMWARE_UPDATE_FLAG_BUDS_FAILED)
    {

        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
            COMPONENT_FIRMWARE_UPDATE_BUDS_FAIL_GOT;
    }
    if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_ABORD) ==
        COMPONENT_FIRMWARE_UPDATE_FLAG_ABORD)
    {
        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
            COMPONENT_FIRMWARE_UPDATE_ABORTED;
    }
    if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_CLEAR) ==
        COMPONENT_FIRMWARE_UPDATE_FLAG_CLEAR)
    {
        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
            COMPONENT_FIRMWARE_UPDATE_FLGA_CLEARED;
    }


    if (m_cfu_struct.last_transmit_SequenceNumber == contentCommand->SequenceNumber)
    {
        DFU_PRINT_TRACE1("received seg %x again", contentCommand->SequenceNumber);
        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
            COMPONENT_FIRMWARE_UPDATE_SUCCESS;
        return;
    }
    m_cfu_struct.last_transmit_SequenceNumber = contentCommand->SequenceNumber;
    if (contentCommand->Address == m_cfu_struct.current_image_start_addr)
    {
        DFU_PRINT_TRACE1("image start %b", TRACE_BINARY(14, contentCommand->Data));
        uint32_t version = *(uint32_t *)(contentCommand->Data + 2);
        if (version != m_cfu_struct.update_cfu_version.AsUInt32)
        {
            DFU_PRINT_ERROR2("CFU content wrong version %x, need %x", version,
                             m_cfu_struct.update_cfu_version.AsUInt32);
            contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                COMPONENT_FIRMWARE_UPDATE_ERROR_VERSION;
            return;
        }
        m_cfu_struct.current_flashing_index  = 0;
        m_cfu_struct.current_flashing_offset = 0;
        m_cfu_struct.current_image_ID = *(uint16_t *)contentCommand->Data;
        m_cfu_struct.current_image_end_addr = *(uint32_t *)(contentCommand->Data + 6);
        m_cfu_struct.current_image_length = *(uint32_t *)(contentCommand->Data + 10);
        DFU_PRINT_TRACE4("CFU start update version %x imageId %x, end addr %x, length %x",
                         version, m_cfu_struct.current_image_ID,
                         m_cfu_struct.current_image_end_addr,
                         m_cfu_struct.current_image_length);
        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
            COMPONENT_FIRMWARE_UPDATE_SUCCESS;
        return;
    }
    if (m_cfu_struct.current_image_ID != 0)
    {

        if (contentCommand->Address < m_cfu_struct.current_image_end_addr)
        {
            if (contentCommand->Address == m_cfu_struct.current_image_start_addr + CFU_CONTENT_DATA_LENGTH)
            {
                DFU_PRINT_TRACE1("Image header %b", TRACE_BINARY(14, contentCommand->Data));
#if 0
                DFU_PRINT_TRACE1("UUID %b", TRACE_BINARY(16,
                                                         contentCommand->Data + sizeof(T_IMG_CTRL_HEADER_FORMAT)));
                uint8_t uuid[] = DEFINE_symboltable_uuid;
                if (memcmp(uuid, contentCommand->Data + sizeof(T_IMG_CTRL_HEADER_FORMAT), 16))
                {
                    DFU_PRINT_ERROR1("Image not signed, header %b", TRACE_BINARY(14, contentCommand->Data));
                    contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                        COMPONENT_FIRMWARE_UPDATE_ERROR_SIGNATURE;
                    return;
                }
#endif
                if (app_cfu_is_image_signed && !(((T_IMG_CTRL_HEADER_FORMAT *)(contentCommand->Data))->crc16))
                {
                    DFU_PRINT_ERROR1("Image not appliable, header %b", TRACE_BINARY(14, contentCommand->Data));
                    contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                        COMPONENT_FIRMWARE_UPDATE_ERROR_SIGNATURE;
                    return;
                }
            }
            dfu_result = app_ota_write_to_flash(m_cfu_struct.current_image_ID,
                                                m_cfu_struct.current_flashing_offset, 0,
                                                CFU_CONTENT_DATA_LENGTH,
                                                contentCommand->Data);
            DFU_PRINT_TRACE4("dfu_update image id %x, offset %x, result %x, all length %x",
                             m_cfu_struct.current_image_ID,
                             m_cfu_struct.current_flashing_offset, dfu_result, m_cfu_struct.current_image_length);
            if (dfu_result == 0)
            {
                m_cfu_struct.current_flashing_offset += CFU_CONTENT_DATA_LENGTH;
                m_cfu_struct.current_image_length -= CFU_CONTENT_DATA_LENGTH;
                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                    COMPONENT_FIRMWARE_UPDATE_SUCCESS;
                return;
            }
            else
            {
                DFU_PRINT_ERROR2("dfu_update image id %x, write fail %x", m_cfu_struct.current_image_ID,
                                 dfu_result);
                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                    COMPONENT_FIRMWARE_UPDATE_ERROR_WRITE;
                return;
            }
        }
        else if (contentCommand->Address == m_cfu_struct.current_image_end_addr)
        {
            dfu_result = dfu_update(m_cfu_struct.current_image_ID, m_cfu_struct.current_flashing_offset,
                                    m_cfu_struct.current_image_length,
                                    contentCommand->Data);
            DFU_PRINT_TRACE4("dfu_update image id %x, offset %x, length %x, result %x",
                             m_cfu_struct.current_image_ID,
                             m_cfu_struct.current_flashing_offset, m_cfu_struct.current_image_length, dfu_result);

            if (dfu_result == 0)//means upadte success .
            {
//                if (m_cfu_struct.current_image_ID == CFU_VERSION_SECTION)
//                {
//                    app_cfu_set_cfu_version(m_cfu_struct.update_cfu_version.AsUInt32);
//                }
                if (m_cfu_struct.current_image_ID !=
                    m_cfu_struct.last_update_img_id) //last_update_img_id last to checksum
                {

                    dfu_result = dfu_check_checksum(m_cfu_struct.current_image_ID, 0);
                    DFU_PRINT_TRACE2("dfu_check_checksum image id %x, dfu_result %x", m_cfu_struct.current_image_ID,
                                     dfu_result);
                    if (dfu_result == true)// true means dfu checksum pass
                    {
                        m_cfu_struct.current_flashing_offset = 0;
                        m_cfu_struct.current_image_length = 0;
                        m_cfu_struct.update_result &= dfu_result;

                        m_cfu_struct.current_image_start_addr = m_cfu_struct.current_image_end_addr +
                                                                CFU_CONTENT_DATA_LENGTH;
                        DFU_PRINT_TRACE1("move to next image, start addr %x", m_cfu_struct.current_image_start_addr);
                        contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                            COMPONENT_FIRMWARE_UPDATE_SUCCESS;
                        return;
                    }
                    else
                    {
                        DFU_PRINT_ERROR2("dfu_check_checksum image id %x, check fail %x", m_cfu_struct.current_image_ID,
                                         dfu_result);
                        if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK) ==
                            COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK)
                        {
                            contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                                COMPONENT_FIRMWARE_UPDATE_ERROR_VERIFY;
                        }
                        else
                        {
                            contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                                COMPONENT_FIRMWARE_UPDATE_ERROR_CRC;
                        }
                    }

                }
                else  // last_update_img_id is get
                {
                    /*last package need wait other bud success so wait timer(30s) longer than before(3s)*/
                    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                    {
                        dfu_result = dfu_check_checksum(m_cfu_struct.current_image_ID, 0);
                        DFU_PRINT_TRACE2("dfu_check_checksum image id %x, dfu_result %x", m_cfu_struct.current_image_ID,
                                         dfu_result);
                        if (dfu_result == false)// not 1 means dfu checksum failed
                        {
                            DFU_PRINT_ERROR2("dfu_check_checksum image id %x, check fail %x", m_cfu_struct.current_image_ID,
                                             dfu_result);
                            if ((contentCommand->Flags & COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK) ==
                                COMPONENT_FIRMWARE_UPDATE_FLAG_LAST_BLOCK)
                            {
                                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                                    COMPONENT_FIRMWARE_UPDATE_ERROR_VERIFY;
                            }
                            else
                            {
                                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                                    COMPONENT_FIRMWARE_UPDATE_ERROR_CRC;
                            }
                            return;
                        }
                        app_stop_timer(&timer_idx_cfu_content_trans);
                        DFU_PRINT_ERROR0("ROLE_SINGLE last block finished, start timer to reset");
                        DFU_PRINT_ERROR3("Version from '%x bank %x' to %x",
                                         app_cfu_get_version(CFU_VERSION_SECTION, true), app_cfu_get_bank_index(true),
                                         app_cfu_get_version(CFU_VERSION_SECTION, false));
                        DFU_PRINT_ERROR2("Ignore Version %d, Force reset %d", m_cfu_struct.current_updating_ignore_version,
                                         m_cfu_struct.current_updating_immediate_reset);
                        if (m_cfu_struct.current_updating_ignore_version &&
                            (app_cfu_get_version(CFU_VERSION_SECTION, true) >= app_cfu_get_version(CFU_VERSION_SECTION, false)))
                        {
                            if (is_ota_support_bank_switch())
                            {
                                uint32_t header_addr = get_header_addr_by_img_id(IMG_MCUAPP);
                                DFU_PRINT_ERROR2("Erase sector %x address %x", IMG_MCUAPP, header_addr);
                                dfu_erase_img_flash_area(header_addr, 0x20);
                            }
                        }
                        m_cfu_struct.update_in_progress = false;
                        m_cfu_struct.is_bank_swap_pending = true;
                        app_start_timer(&timer_idx_cfu_reset_active, "cfu_reset_active",
                                        app_cfu_timer_queue_id, APP_TIMER_CFU_RESET_ACTIVE, 0, false,
                                        3000);
                    }
                    else
                    {
                        app_start_timer(&timer_idx_cfu_content_trans, "cfu_reset_active",
                                        app_cfu_timer_queue_id, APP_TIMER_CFU_CONTENT_TRANS, 0, false,
                                        30000);
                    }
                    contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                        COMPONENT_FIRMWARE_UPDATE_SUCCESS;
                }

                return;
            }
            else
            {
                DFU_PRINT_ERROR2("dfu_update image id %x, write fail %x", m_cfu_struct.current_image_ID,
                                 dfu_result);
                contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                    COMPONENT_FIRMWARE_UPDATE_ERROR_WRITE;
                return;
            }
        }
        else
        {
            contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
                COMPONENT_FIRMWARE_UPDATE_ERROR_INVALID;
            DFU_PRINT_ERROR0("CFU content parse error INVALID addr>end");
            return;
        }
    }
    contentResponse->HidCfuContentResponse.CfuContentResponse.Status =
        COMPONENT_FIRMWARE_UPDATE_ERROR_INVALID;
    DFU_PRINT_ERROR0("CFU content parse error INVALID imageID == 0");
    return;
}

#if 0 //for old
#endif

void app_cfu_handle_set_report(uint8_t *data, uint16_t length)
{
    memset(app_cfu_response_buffer, 0, sizeof(FWUPDATE_OFFER_RESPONSE));
    switch (data[0])
    {
    case REPORT_ID_CFU_OFFER_OUTPUT:
        {
//            app_cfu_offer_parse(app_cfu_receive_buffer, m_cfu_struct.usb_receive_length);
            app_cfu_offer_parse(data, length);
            break;
        }
    case REPORT_ID_CFU_PAYLOAD_OUTPUT:
        {
//            app_cfu_content_parse(app_cfu_receive_buffer, m_cfu_struct.usb_receive_length);
            app_cfu_content_parse(data, length);
            if (((FWUPDATE_CONTENT_RESPONSE *)
                 app_cfu_response_buffer)->HidCfuContentResponse.CfuContentResponse.Status !=
                COMPONENT_FIRMWARE_UPDATE_SUCCESS
                && ((FWUPDATE_CONTENT_RESPONSE *)
                    app_cfu_response_buffer)->HidCfuContentResponse.CfuContentResponse.Status !=
                COMPONENT_FIRMWARE_UPDATE_BUDS_SUCCESS_GOT
               )
            {
                app_cfu_error_process();
            }
            break;
        }
    default:
#if CFU_DEBUG
        //app_cfu_test(app_cfu_receive_buffer, m_cfu_struct.usb_receive_length);
        app_cfu_test(data, length);
#endif
        return;
    }
    hid_driver_data_hid_send(NULL, app_cfu_response_buffer, sizeof(FWUPDATE_OFFER_RESPONSE), NULL);
    return;
}

void app_cfu_handle_get_report(uint8_t *data, uint16_t *length)
{
    DFU_PRINT_TRACE0("app_cfu_handle_get_report");
    //*data = (uint8_t *)&firmwareVersionResponse;
    memset(data, 0, sizeof(GET_FWVERSION_RESPONSE));
    GET_FWVERSION_RESPONSE *response = (GET_FWVERSION_RESPONSE *)data;
    response->ReportId = REPORT_ID_CFU_FEATURE;
    response->header.ComponentCount = 1;
    response->header.ProtocolRevision = 4;
    response->componentVersionsAndProperty[0].ComponentVersion.AsUInt32 = app_cfu_get_version(
                                                                              CFU_VERSION_SECTION, true);
    response->componentVersionsAndProperty[0].ComponentProperty.ComponentId = 0xA1;
    response->componentVersionsAndProperty[0].ComponentProperty.Bank = app_cfu_get_bank_index(1);
    response->componentVersionsAndProperty[0].ComponentProperty.budRole = app_cfg_const.bud_role;
    response->componentVersionsAndProperty[0].ComponentProperty.VendorSpecific1 = 0x8773;
    *length = sizeof(GET_FWVERSION_RESPONSE);
}

void app_cfu_handle_get_reportEx(uint8_t *data, uint16_t *length)
{
}

bool app_cfu_if_can_reset()
{
    return true;
}

void app_cfu_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    DFU_PRINT_TRACE2("app_cfu_timeout_cb: timer_evt 0x%02X param 0x%x", timer_evt, param);
    static uint32_t last_seg = 0;

    switch (timer_evt)
    {
    case APP_TIMER_CFU_CONTENT_TRANS:
        {
            DFU_PRINT_TRACE2("APP_TIMER_CFU_CONTENT_TRANS: ls 0x%x nx 0x%x", last_seg,
                             m_cfu_struct.last_transmit_SequenceNumber);
            if (m_cfu_struct.last_transmit_SequenceNumber != last_seg)
            {
                last_seg = m_cfu_struct.last_transmit_SequenceNumber;
            }
            else
            {
                DFU_PRINT_ERROR1("APP_TIMER_CFU_CONTENT_TRANS: stopped at segment %x",
                                 m_cfu_struct.last_transmit_SequenceNumber);
                app_stop_timer(&timer_idx_cfu_content_trans);
                app_cfu_error_process();

            } break;
        }

    case APP_TIMER_CFU_RESET_ACTIVE:
        {
            app_stop_timer(&timer_idx_cfu_reset_active);
            if (m_cfu_struct.current_updating_compomentID == CFU_SELF_COMPONENT_ID)
            {
                if (app_cfu_if_can_reset())
                {
                    DFU_PRINT_ERROR3("APP_TIMER_CFU_RESET_ACTIVE: set version from '%x bank %x' to %x, reset device!",
                                     app_cfu_get_version(CFU_VERSION_SECTION, true), app_cfu_get_bank_index(true),
                                     app_cfu_get_version(CFU_VERSION_SECTION, false));
                    chip_reset(RESET_ALL);
                }
                else
                {
                    DFU_PRINT_WARN0("APP_TIMER_CFU_RESET_ACTIVE: not reset, for next round");
                    app_start_timer(&timer_idx_cfu_reset_active, "app_cfu_timer_reset",
                                    app_cfu_timer_queue_id, APP_TIMER_CFU_RESET_ACTIVE, 0, false,
                                    10000);
                }
            }
        }
    }
}

void app_cfu_init()
{
    T_IMG_HEADER_FORMAT *addr;
    uint8_t banknum =
        9; // banknum 1 means bank0 work 2 means bank1 work 3 means not support swapt bank 9 default value
    addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id(IMG_MCUAPP);
    app_cfu_is_image_signed = addr->ctrl_header.crc16;
    banknum = app_ota_get_active_bank();
    DFU_PRINT_TRACE3("app_cfu_init image signed state %d active_bank :%d version is 0x%x",
                     app_cfu_is_image_signed,
                     (banknum - 1), app_cfu_get_version(CFU_VERSION_SECTION, true));

    app_timer_reg_cb(app_cfu_timeout_cb, &app_cfu_timer_id);

    extern void app_cfu_usb_init(void);
    app_cfu_usb_init();
#if CFU_DEBUG_LOG_ONLY
    log_module_bitmap_trace_set(0xffff ffff ffff ffff, LEVEL_TRACE, false);
    log_module_bitmap_trace_set(0xffffffffffffffff, LEVEL_INFO, false);
    log_module_bitmap_trace_set(0xffffffffffffffff, LEVEL_WARN, false);
    log_module_bitmap_trace_set(0xffffffffffffffff, LEVEL_ERROR, false);
    log_module_trace_set(MODULE_DFU, LEVEL_TRACE, true);
    log_module_trace_set(MODULE_DFU, LEVEL_INFO, true);
    log_module_trace_set(MODULE_DFU, LEVEL_WARN, true);
    log_module_trace_set(MODULE_DFU, LEVEL_ERROR, true);
    log_module_trace_set(MODULE_USB, LEVEL_TRACE, true);
    log_module_trace_set(MODULE_USB, LEVEL_INFO, true);
    log_module_trace_set(MODULE_USB, LEVEL_WARN, true);
    log_module_trace_set(MODULE_USB, LEVEL_ERROR, true);
#endif
//    app_usb_function_cfg(USB_HID_ADDIN_CLASS_SUPPORT);
}


#if CFU_DEBUG
void app_cfu_test(uint8_t *data, uint16_t length)
{
    DFU_PRINT_TRACE1("app_cfu_test %b", TRACE_BINARY(length, data));
    uint8_t *test_data = data + 2;
    switch (data[1])
    {
    case 0:
        {
            app_cfu_get_version(CFU_VERSION_SECTION, true);
            break;
        }
    case 1:
        {
//            app_cfu_set_cfu_version(*(uint32_t *)(&data[1]));
            break;
        }
    case 2:
        {
            bool aes_rslt;
            uint8_t bsize;
            ota_get_otp_setting(aes_rslt);
            bsize = flash_get_bank_size(FLASH_OTA_TMP) / 4096;
            DFU_PRINT_TRACE2("app_cfu_get_device_info aes_rslt %d, bsize %x", aes_rslt, bsize);
            uint32_t ota_bank0_addr = flash_partition_addr_get(PARTITION_FLASH_OTA_BANK_0);
            uint32_t active_addr = get_active_ota_bank_addr();
            DFU_PRINT_TRACE2("app_cfu_get_device_info ota_bank0_addr %x, active_addr %x", ota_bank0_addr,
                             active_addr);
            break;
        }
    case 3:
        {
            T_IMG_HEADER_FORMAT *addr;
            IMG_ID i;
            for (i = IMG_OCCD; i <= IMAGE_MAX; i++)
            {
                addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id((IMG_ID)i);
                //addr = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id((IMG_ID)i);
                DFU_PRINT_TRACE2("app_cfu_get_active_bank_version id %x, addr %x", i, addr);
                if (addr != 0)
                {
                    if (i == IMG_OTA)
                    {
                        DFU_PRINT_TRACE2("app_cfu_get_active_bank_version id %x, ver %x", i, addr->ver_val);
                    }
                    else if (i > IMG_OTA)
                    {
                        if (check_header_valid((uint32_t)addr, (IMG_ID)i) == IMG_CHECK_PASS)
                        {
                            DFU_PRINT_TRACE2("app_cfu_get_active_bank_version id %x, ver %x", i, addr->git_ver.version);
                        }
                        else
                        {
                            DFU_PRINT_TRACE1("app_cfu_get_active_bank_version id %x, ver fail", i);
                        }
                    }
                }
            }
            for (i = IMG_OCCD; i <= IMAGE_MAX; i++)
            {
                //addr = (T_IMG_HEADER_FORMAT *)get_header_addr_by_img_id((IMG_ID)i);
                addr = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id((IMG_ID)i);
                DFU_PRINT_TRACE2("app_cfu_get_inactive_bank_version id %x, addr %x", i, addr);
                if (addr != 0)
                {
                    if (i == IMG_OTA)
                    {
                        DFU_PRINT_TRACE2("app_cfu_get_inactive_bank_version id %x, ver %x", i, addr->ver_val);
                    }
                    else if (i > IMG_OTA)
                    {
                        if (check_header_valid((uint32_t)addr, (IMG_ID)i) == IMG_CHECK_PASS)
                        {
                            DFU_PRINT_TRACE2("app_cfu_get_inactive_bank_version id %x, ver %x", i, addr->git_ver.version);
                        }
                        else
                        {
                            DFU_PRINT_TRACE1("app_cfu_get_inactive_bank_version id %x, ver fail", i);
                        }
                    }
                }
            }
            break;
        }
    case 4:
        {
            uint16_t img_id = *(uint16_t *)(test_data);
            uint32_t offset = *(uint32_t *)(test_data + 2);
            uint8_t length = *(uint8_t *)(test_data + 6);
            uint32_t dfu_result = app_ota_write_to_flash(img_id, offset, 0, length, (test_data + 7));
            DFU_PRINT_TRACE4("dfu_update image id %x, offset %x, length %x, value %b", img_id, offset, length,
                             TRACE_BINARY(length, (test_data + 7)));
            DFU_PRINT_TRACE1("dfu_update result %x", dfu_result);
        }
        break;
    case 5:
        {
            uint16_t img_id = *(uint16_t *)(test_data);
            uint32_t dfu_result = app_ota_checksum(img_id, 0);
            DFU_PRINT_TRACE2("dfu_check_checksum image id %x, dfu_result %x", img_id, dfu_result);
        }
        break;
    case 6:
        {
            uint16_t img_id = *(uint16_t *)(data);
            uint32_t address = get_temp_ota_bank_addr_by_img_id((IMG_ID)img_id);
            uint32_t offset = *(uint32_t *)(test_data + 2);
            uint8_t length = *(uint8_t *)(test_data + 6);
            DFU_PRINT_TRACE3("address %x length %x data is %b", address + offset, length, TRACE_BINARY(length,
                             address + offset));
        }
        break;
    case 7:
        {
            uint16_t img_id = *(uint16_t *)(test_data);
            uint32_t address = get_header_addr_by_img_id((IMG_ID)img_id);
            uint32_t offset = *(uint32_t *)(test_data);
            uint8_t length = *(uint8_t *)(test_data);
            DFU_PRINT_TRACE3("address %x length %x data is %b", address + offset, length, TRACE_BINARY(length,
                             address + offset));
        }
        break;
    case 8:
        {
            uint8_t length = *(uint8_t *)(test_data);
            DFU_PRINT_TRACE2("interrupt in length %x, %b", length, TRACE_BINARY(length, (test_data + 2)));
            hid_driver_data_send(NULL, (test_data + 1), length, NULL);
        }
        break;
    case 9:
        {
            uint32_t header_addr = get_active_ota_bank_addr();
            dfu_erase_img_flash_area(header_addr, BANK0_OTA_HDR_SIZE);
        }
    case 10:
        {
            dfu_fw_active_reset();
        }
        break;
    default:
        memset(app_cfu_response_buffer, 0, sizeof(FWUPDATE_OFFER_RESPONSE));
        for (int i = 0; i < length; i++)
        {
            app_cfu_response_buffer[i] = data[i];
        }
        hid_driver_data_send(NULL, app_cfu_response_buffer, sizeof(FWUPDATE_OFFER_RESPONSE), NULL);
        break;
    }

}
#endif
#endif
