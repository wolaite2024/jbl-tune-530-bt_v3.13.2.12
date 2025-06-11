#include "stdint.h"
#include "patch_header_check.h"
#include "ftl.h"

#define CFU_DEBUG 0
#define CFU_DEBUG_LOG_ONLY 0


/** @brief image exit indicator. */
#define APP_CFU_IMAGE_LOCATION_BANK0            0
#define APP_CFU_IMAGE_LOCATION_BANK1            1
#define APP_CFU_NOT_SUPPORT_BANK_SWITCH         3

#define MAX_ACCEPTED_OFFER_NUM                  2

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
#define CFW_UPDATE_PACKET_MAX_LENGTH                       (sizeof(FWUPDATE_CONTENT_COMMAND))
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

#define CFU_VERSION_SECTION     IMG_OTA
#define CFU_SELF_COMPONENT_ID   0xA1

void app_cfu_init(void);
void app_cfu_test(uint8_t *data, uint16_t length);
void app_cfu_handle_get_report(uint8_t *data, uint16_t *length);
void app_cfu_handle_set_report(uint8_t *data, uint16_t length);
