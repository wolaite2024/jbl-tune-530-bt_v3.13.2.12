/**
************************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     dfu_api.h
* @brief    APIs to implement device firmware update .
* @author   ranhui
* @date     2015-10-29
* @version  v1.0
*************************************************************************************************************
*/

#ifndef _DFU_API_H_
#define  _DFU_API_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <patch_header_check.h>

/** @defgroup DFU_API DFU API Sets
  * @brief API sets for device firmware update implementation
  * @{
  */

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @defgroup DFU_API_Exported_Constants DFU API Sets Exported Constants
    * @brief
    * @{
    */

/** @brief  DFU service UUID */
static const uint8_t GATT_UUID128_DFU_SERVICE[16] = {0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x62, 0x00, 0x00};

/** End of DFU_API_Exported_Constants
    * @}
    */


/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup DFU_API_Exported_Macros DFU API Sets Exported Macros
  * @{
  */

/** @brief  dfu image header size*/
#define DFU_HEADER_SIZE  12

/** @brief  vendor UUID defined for DFU service*/
#define  GATT_UUID128_DFU_SERVICE_ADV   0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x62, 0x00, 0x00
#define  GATT_UUID128_DFU_PACKET        0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x63, 0x00, 0x00
#define  GATT_UUID128_DFU_CONTROL_POINT 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x64, 0x00, 0x00

/** @brief  error code for DFU service*/
#define DFU_ERR_PROC_ALREADY_IN_PROGRESS    0x80
#define DFU_ERR_CCCD_IMPROPERLY_CONFIGURED  0x81

/** @brief  opcode code for DFU service*/
#define DFU_OPCODE_MIN                      0x00
#define DFU_OPCODE_START_DFU                0x01
#define DFU_OPCODE_RECEIVE_FW_IMAGE_INFO    0x02
#define DFU_OPCODE_VALID_FW                 0x03
#define DFU_OPCODE_ACTIVE_IMAGE_RESET       0x04
#define DFU_OPCODE_SYSTEM_RESET             0x05
#define DFU_OPCODE_REPORT_TARGET_INFO       0x06
#define DFU_OPCODE_CONN_PARA_TO_UPDATE_REQ  0x07
#define DFU_OPCODE_BUFFER_CHECK_EN          0x09
#define DFU_OPCODE_REPORT_BUFFER_CRC        0x0a
#define DFU_OPCODE_MAX                      0x0b

#define DFU_OPCODE_NOTIF                    0x10

/** @brief  length of each control point procedure*/
#define DFU_LENGTH_START_DFU                (1+12)
#define DFU_LENGTH_RECEIVE_FW_IMAGE_INFO    (1+2+4)
#define DFU_LENGTH_VALID_FW                 (1+2+1)
#define DFU_LENGTH_ACTIVE_IMAGE_RESET       0x01
#define DFU_LENGTH_SYSTEM_RESET             0x01
#define DFU_LENGTH_REPORT_TARGET_INFO       (1+2)
#define DFU_LENGTH_PKT_RX_NOTIF_REQ         (1+2)
#define DFU_LENGTH_CONN_PARA_TO_UPDATE_REQ  (1+2+2+2+2)
#define DFU_LENGTH_BUFFER_CHECK_EN          0x01
#define DFU_LENGTH_REPORT_BUFFER_CRC        (1+2+2)

#define DFU_NOTIFY_LENGTH_ARV                   3
#define DFU_NOTIFY_LENGTH_REPORT_TARGET_INFO    (3+2+4)
#define DFU_NOTIFY_LENGTH_PKT_RX_NOTIF          (3+2)

/** @brief  index for DFU characteristics*/
#define INDEX_DFU_PACKET_VALUE              0x02
#define INDEX_DFU_CONTROL_POINT_CHAR_VALUE  0x04

/** @brief  index for DFU implementation*/
#define DFU_ARV_SUCCESS                         0x01
#define DFU_ARV_FAIL_INVALID_PARAMETER          0x02
#define DFU_ARV_FAIL_OPERATION                  0x03
#define DFU_ARV_FAIL_DATA_SIZE_EXCEEDS_LIMIT    0x04
#define DFU_ARV_FAIL_CRC_ERROR                  0x05
#define DFU_ARV_FAIL_DATA_LENGTH_ERROR          0x06
#define DFU_ARV_FAIL_FLASH_WRITE_ERROR          0x07
#define DFU_ARV_FAIL_FLASH_ERASE_ERROR          0x08


/** @} */ /* End of group DFU_API_Exported_Macros */


/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup DFU_API_Exported_Types DFU API Sets Exported Types
  * @{
  */

/** @brief start dfu parameter definition*/
typedef struct
{
    uint8_t ic_type;
    uint8_t secure_ver;
    uint16_t ctrl_flag;
    uint16_t image_id;
    uint16_t crc16;
    uint32_t payload_len;
} T_START_DFU_PARA;

/** @} */ /* End of group DFU_API_Exported_Types */
/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup DFU_API_Exported_Functions DFU API Sets Exported Functions
    * @{
    */

/**
 * @brief  Get specified FW info and current OTA offset information for report.
 * @param  img_id              image id @ref IMG_ID to identify FW.
 * @param  p_origin_fw_version     To store current FW version.
 * @param  p_offset            To store current file offset.
 * @return api status
 * @retval 0             successful
 * @retval otherwise     fail
*/
uint32_t dfu_report_target_fw_info(uint16_t img_id, uint32_t *p_origin_fw_version,
                                   uint32_t *p_offset);

/**
 * @brief  Write specified image data with specified length to flash
 * @param  img_id             image id @ref IMG_ID to identify FW.
 * @param  offset             offset of the image.
 * @param  length             length of data.
 * @param  p_void             pointer to data.
 * @return api status
 * @retval 0             successful
 * @retval otherwise     fail
*/
uint32_t dfu_update(uint16_t img_id, uint32_t offset, uint32_t length, void *p_void);

/**
 * @brief Reset ota local structure.
 * @param img_id             image id @ref IMG_ID to identify FW.
 *
 * @return true
*/
bool dfu_reset(uint16_t img_id);

/**
 * @brief Indicating OTA being finished and pending for reset to apply new FW image.
 * @param void
 *
 * @return void
*/
void dfu_fw_active_reset(void);

/**
 * @brief Calculate checksum of the image and compare with given checksum value in image header.
 *
 * @param  img_id             image id @ref IMG_ID to identify FW.
 * @param  offset             start address offset of the area.
 * @return api status
 * @retval true             successful
 * @retval false     fail
*/
bool dfu_check_checksum(uint16_t image_id, uint32_t offset);

/**
 * @brief Reset device to OTA mode for coming device firmware update.
 *
*/
void dfu_switch_to_ota_mode(void);

/**
 * @brief Get specified FW version given the signature.
 *
 * @param  img_id             image id @ref IMG_ID to identify FW.
 * @param  p_origin_fw_version     To store current FW version.
 * @return api status
 * @retval true             successful
 * @retval false     fail
*/
bool dfu_get_fw_version(uint16_t img_id, uint8_t *p_origin_fw_version);

/**
 * @brief  Erase specific flash sector
 * @param  img_id             image id @ref IMG_ID to identify FW.
 * @param  locked             Indicates if need to block other tasks when erase
 * @param  offset             Start address of specific sector to erase
 * @return true if succeed, false if failed
*/
bool dfu_erase_img_flash_sector(uint16_t img_id, bool locked, uint32_t offset);

/**
 * @brief  Erase specific flash area
 * @param  start_addr       Start addr of the erea.
 * @param  size             Length of area to erase, input NULL if erase the whole area
 * @return true if succeed, false if failed
*/
bool dfu_erase_img_flash_area(uint32_t start_addr, uint32_t size);

uint32_t dfu_checkbufcrc(uint8_t *buf, uint32_t length, uint16_t mCrcVal);

bool dfu_check_crc(T_IMG_CTRL_HEADER_FORMAT *p_header, bool with_semaphore);

bool hw_aes_decrypt_16byte(uint8_t *input, uint8_t *output);

void dfu_init(void);

void erase_temp_bank_header(void);

/** @} */ /* End of group DFU_API_Exported_Functions */

/** @} */ /* End of group DFU_API */

#endif //_DFU_API_H_

