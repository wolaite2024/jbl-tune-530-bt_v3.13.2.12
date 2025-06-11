/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    fmc_api.h
* @brief   This file provides fmc api wrapper for sdk customer.
* @author  yuhungweng
* @date    2020-11-26
* @version v1.0
* *************************************************************************************
*/

#ifndef __FMC_API_H_
#define __FMC_API_H_

#include <stdint.h>
#include "storage.h"
#include <clock_manager.h>

/** @defgroup  87x3e_FMC    FMC API
    * @brief fmc api wrapper
    * @{
    */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup FMC_Exported_Macros FMC Exported Macros
    * @brief
    * @{
    */
#define FMC_SEC_SECTION_LEN 0x1000
#define FMC_SEC_BLOCK_LEN   0x10000

/** End of FMC_Exported_Macros
    * @}
    */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FMC_TYPE FMC Exported Types
   * @brief
   * @{
   */
typedef enum
{
    FMC_FLASH_NOR_IDX0,
    FMC_FLASH_NOR_IDX1,
    FMC_FLASH_NOR_IDX2,
    FMC_FLASH_NOR_IDX3
} FMC_FLASH_NOR_IDX_TYPE;

typedef enum
{
    FMC_FLASH_NOR_ERASE_CHIP   = 1,
    FMC_FLASH_NOR_ERASE_SECTOR = 2,
    FMC_FLASH_NOR_ERASE_BLOCK  = 4,
} FMC_FLASH_NOR_ERASE_MODE;


typedef enum
{
    PARTITION_FLASH_OCCD,
    PARTITION_FLASH_OTA_BANK_0,
    PARTITION_FLASH_OTA_BANK_1,
    PARTITION_FLASH_OTA_TMP,
    PARTITION_FLASH_BKP_DATA1,
    PARTITION_FLASH_BKP_DATA2,
    PARTITION_FLASH_FTL,
    PARTITION_FLASH_HARDFAULT_RECORD,

    PARTITION_FLASH_TOTAL,

} T_FLASH_PARTITION_NAME;

typedef enum _FLASH_IMG_ID
{
    FLASH_IMG_OTA         = 0, /* OTA header */
    FLASH_IMG_SBL         = 1,
    FLASH_IMG_MCUPATCH    = 2,
    FLASH_IMG_MCUAPP      = 3,
    FLASH_IMG_DSPSYSTEM   = 4,
    FLASH_IMG_DSPAPP      = 5,
    FLASH_IMG_MCUCONFIG   = 6,
    FLASH_IMG_DSPCONFIG   = 7,
    FLASH_IMG_ANC         = 8,
    FLASH_IMG_EXT1        = 9,
    FLASH_IMG_EXT2        = 10,
    FLASH_IMG_ADSP        = 11,
    FLASH_IMG_SYSPATCH    = 12,
    FLASH_IMG_STACKPATCH  = 13,
    FLASH_IMG_UPPERSTACK  = 14,
    FLASH_IMG_FRAMEWORK   = 15,
    FLASH_IMG_SYSDATA     = 16,
    FLASH_IMG_MAX,
} FLASH_IMG_ID;

typedef void (*FMC_FLASH_NOR_ASYNC_CB)(void);

/** End of FMC_TYPE
    * @}
    */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FMC_Exported_Flash_Functions FMC Exported Flash Functions
    * @brief
    * @{
    */


/**
 * @brief           task-safe of @ref flash_nor_read
 * @param addr      the ram address mapping of nor flash going to be read
 * @param data      data buffer to be read into
 * @param len       read data length
 * @return          true if read successful, otherwise false
 */
bool fmc_flash_nor_read(uint32_t addr, void *data, uint32_t len);

/**
 * @brief           task-safe of @ref flash_nor_write
 * @param addr      the ram address mapping of nor flash going to be written
 * @param data      data buffer to be write into
 * @param len       write data length
 * @return          true if write successful, otherwise false
 */
bool fmc_flash_nor_write(uint32_t addr, void *data, uint32_t len);

/**
 * @brief           task-safe of @ref flash_nor_erase
 * @param addr      the ram address mapping of nor flash going to be erased
 * @param mode      erase mode defined as @ref FMC_FLASH_NOR_ERASE_MODE
 * @return          true if erase successful, otherwise false
 */
bool fmc_flash_nor_erase(uint32_t addr, FMC_FLASH_NOR_ERASE_MODE mode);

/**
 * @brief           task-safe nor flash auto dma read
 * @param src       the ram address mapping of nor flash going to be read from
 * @param dst       the ram address going to be written to
 * @param len       dma data length
 * @param cb        call back function which is to be executed when dma finishing @ref FMC_FLASH_NOR_ASYNC_CB
 * @return          true if trigger auto dma read successful, otherwise false
 */
bool fmc_flash_nor_auto_dma_read(uint32_t src, uint32_t dst, uint32_t len,
                                 FMC_FLASH_NOR_ASYNC_CB cb);
/**
 * @brief           switch nor flash clock
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @param idx       specific nor flash @ref FMC_FLASH_NOR_IDX_TYPE
 * @param bit_mode  clock @ref CLK_FREQ_TYPE
 * @return          true if switch clock successful, otherwise false
 */
bool fmc_flash_nor_clock_switch(FMC_FLASH_NOR_IDX_TYPE idx, CLK_FREQ_TYPE clk);

/**
 * @brief           get the address of a flash partition
 * @param name      a partition name in @ref T_FLASH_PARTITION_NAME
 * @return          the address of the flash partition
 */
uint32_t flash_partition_addr_get(T_FLASH_PARTITION_NAME name);

/**
 * @brief           get the size of a flash partition
 * @param name      a partition in @ref T_FLASH_PARTITION_NAME
 * @return          the size of the flash partition
 */
uint32_t flash_partition_size_get(T_FLASH_PARTITION_NAME name);

/**
 * @brief           get the payload address of an image
 * @param id        an image id in @ref FLASH_IMG_ID
 * @return          the payload address of the image
 */
uint32_t flash_cur_bank_img_payload_addr_get(FLASH_IMG_ID id);

/**
 * @brief           get the header address of the image
 * @param id        the image id in @ref FLASH_IMG_ID
 * @return          the header address of the image
 */
uint32_t flash_cur_bank_img_header_addr_get(FLASH_IMG_ID id);

/**
 * @brief           task-safe nor flash set block protect level
 * @param addr      the ram address mapping of nor flash
 * @param bp_lv     the block protect level to be set
 * @return          true if set block protect level successful, otherwise false
 */
bool fmc_flash_nor_set_bp_lv(uint32_t addr, uint8_t bp_lv);

/**
 * @brief           get nor flash block protect level
 * @param addr      the ram address mapping of nor flash
 * @param bp_lv     used to pass back the block protect level
 * @return          true if get bp level successful, otherwise false
 */
bool fmc_flash_nor_get_bp_lv(uint32_t addr, uint8_t *bp_lv);

/**
 * @brief           get nor flash OTP lock status
 * @param is_lock   used to pass back the lock status
 * @return          true if get lock status successful, otherwise false
 */
bool fmc_flash_nor_get_otp(bool *is_lock);

/**
 * @brief           set nor flash OTP lock status
 * @param is_lock   the lock status
 * @return          true if set lock status successful, otherwise false
 */
bool fmc_flash_nor_set_otp(bool is_lock);

/**
 * @brief           task-safe of nor flash write OTP
 * @param addr      the address mapping of OTP to be written
 * @param otp_data  data buffer to be written into
 * @param size      write data length
 * @return          true if write successful, otherwise false
 */
bool fmc_flash_nor_write_otp(uint32_t addr, uint8_t *otp_data, uint32_t size);

/**
 * @brief           task-safe of erase flash OTP
 * @param addr      the address mapping of OTP going to be erased
 * @return          true if erase successful, otherwise false
 */
bool fmc_flash_nor_erase_otp(uint32_t addr);

/**
 * @brief           task-safe of read flash OTP
 * @param addr      the address mapping of OTP going to be read
 * @param otp_data  data buffer to be read into
 * @param size      read data length
 * @return          true if read successful, otherwise false
 */
bool fmc_flash_nor_read_otp(uint32_t addr, uint8_t *otp_data, uint32_t size);

/**
 * @brief           get the uuid of nor flash
 * @param uuid      used to pass back the uuid of nor flash
 * @param size      the size of uuid
 * @return          true if get the uuid successful, otherwise false
 */
bool fmc_flash_nor_get_uuid(uint8_t *uuid, uint8_t size);

/**
 * @brief           init flash OTP(One Time Program)
 * @return          true if init successful, otherwise false
 */
bool fmc_flash_nor_otp_init(void);

/** @} */ /* End of group FMC_Exported_Flash_Functions */



/** @defgroup FMC_Exported_PSRAM_Functions FMC Exported PSRAM Functions
    * @brief
    * @{
    */


/**
 * @brief           init winbond opi psram (W955D8MBYA)
 * @return          true if init successful, otherwise false
 */
bool fmc_psram_winbond_opi_init(void);

/**
 * @brief           init APMemory qpi psram (APS1604M-SQR) only supports quad mode
 * @return          true if init successful, otherwise false
 */
bool fmc_psram_ap_memory_qpi_init(void);

/**
 * @brief           init APMemory opi psram (APS6408L-OBx)
 * @return          true if init successful, otherwise false
 */
bool fmc_psram_ap_memory_opi_init(void);

/**
 * @brief           psram mode switch
 * @return          true if init successful, otherwise false
 */
bool fmc_psram_mode_switch_quad(void);

//bool fmc_psram_clock_switch(CLK_FREQ_TYPE clk);

/** @} */ /* End of group FMC_Exported_PSRAM_Functions */

/** @} */ /* End of group 87x3e_FMC */

#endif  // __FMC_API_H_

