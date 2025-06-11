
#ifndef __OTA_EXT_H_
#define __OTA_EXT_H_

#include <stdint.h>
#include "patch_header_check.h"

#define IMG_HEADER_SIZE     0x400

/**
    * @brief    get active bank's image address
    * @param    image_id   image id
    * @return   image address
    */
uint32_t app_get_active_bank_addr(uint16_t image_id);

/**
    * @brief    get inactive bank's image address
    * @param    image_id   image id
    * @return   image address
    */
uint32_t app_get_inactive_bank_addr(uint16_t image_id);

bool check_image_sum(uint32_t base_addr);

/**
    * @brief    check section size before ota
    * @param    image_total_length image total length
    * @param    image id image id
    * @return   true: check success; false: check fail
    */
bool app_ota_check_section_size(uint32_t image_total_length, uint16_t image_id);

/**
    * @brief    check image id
    * @param    img_id    image id
    * @return   true: valid  false: invalid
    */
bool check_image_id(uint16_t img_id);

/**
    * @brief    clear not ready flag of specific image
    * @param    addr    address of the image
    * @return   void
    */
void app_ota_set_ready(uint32_t addr);

/**
    * @brief    clear not ready flag of all the image
    * @param    void
    * @return   void
    */
void app_ota_clear_notready_flag(void);

/**
    * @brief    change the sequence of app config bin and dsp config bin
    * @param    image_id    input image id
    * @return   image id
    */
IMG_ID app_change_seq_appdata_dspdata(IMG_ID image_id);

/**
    * @brief    Used to get image version
    * @param    *p_data   point of image version
    * @param    bank   0:active bank    1:inactive bank
    * @return   void
    */
void app_ota_get_img_version(uint8_t *p_data, uint8_t bank);

/**
    * @brief    Used to get image section size
    * @param    p_data  point of the section size
    * @return   void
    */
void app_ota_get_section_size(uint8_t *p_data);

/**
    * @brief    check copy image id
    * @param    img_id    image id
    * @return   true: valid  false: invalid
    */
bool check_copy_image_id(uint16_t img_id);

/**
    * @brief    decrypt with 16byte input
    * @param    input    data need to be decrypted
    * @return   void
    */
void aes256_decrypt_16byte(uint8_t *input);

/**
 * @brief erase a sector of the flash.
 *
 * @param  signature          signature to identify FW.
 * @param  offset             offset of the image.
 * @return  0 if erase successfully, error line number otherwise
*/
uint32_t dfu_flash_erase(uint16_t signature, uint32_t offset);

/**
* @brief calculate checksum of lenth of buffer in flash.
*
* @param  signature          signature to identify FW.
* @param  offset             offset of the image.
* @param  length             length of data.
* @param  crcValue          ret crc value point.
* @return  0 if buffer checksum calcs successfully, error line number otherwise
*/

uint32_t dfu_checkbufcrc(uint8_t *buf, uint32_t length, uint16_t mCrcVal);

/**
 * @brief  Erase specific flash area
 * @param  start_addr       Start addr of the erea.
 * @param  size             Length of area to erase, input NULL if erase the whole area
 * @return true if succeed, false if failed
*/
bool dfu_erase_img_flash_area(uint32_t start_addr, uint32_t size);

/**
 * @brief  use flash dma mode to write
 *\xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 * @param  dest_addr       destination of the copy address.
 * @param  buf             the pointer of data to be copied to flash
 * @param  len             the length of data to be copied to flash
 * @return 0 if succeed, error number if failed.
*/
int ota_flash_dma_write(size_t dest_addr, void *buf, size_t len);

/**
 * @brief  Init ota flash dma write
 *\xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 * @param  void.
 * @return 0 if succeed, error number if failed.
*/
int ota_flash_dma_write_init(void);

#endif

