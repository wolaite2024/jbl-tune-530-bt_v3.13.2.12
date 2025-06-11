/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sd.h
* @brief    This file provides application functions for sd card or emmc card library..
* @details
* @author   elliot_chen
* @date     2021-02-01
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "sd_define.h"


/* Defines ------------------------------------------------------------------*/

typedef void(*RW_DONE_CBACK)(void *);

typedef enum
{
    GROUP_0,
    GROUP_1,
    GROUP_MAX,
} T_SDIO_PIN_GROUP;

typedef enum
{
    SD_IF_NONE    = 0x00,
    SD_IF_SD_CARD = 0x01,
    SD_IF_MMC     = 0x02,
} T_SD_IF_TYPE;

typedef enum
{
    SD_BUS_WIDTH_1B,
    SD_BUS_WIDTH_4B = 2,
    SD_BUS_WIDTH_MAX
} T_SD_BUS_WIDTH;

typedef enum
{
    /*80M 40M only 8773do support*/
    SD_BUS_CLK_80M = 0x100,
    SD_BUS_CLK_40M = 0x101,

    SD_BUS_CLK_20M = 0x00,
    SD_BUS_CLK_10M = 0x01,
    SD_BUS_CLK_5M = 0x02,
    SD_BUS_CLK_2M5 = 0x04,
    SD_BUS_CLK_1M25 = 0x08,
    SD_BUS_CLK_625K = 0x10,
    SD_BUS_CLK_312K5 = 0x20,
    SD_BUS_CLK_156K25 = 0x40,
    SD_BUS_CLK_78K125 = 0x80,

} T_SD_BUS_CLK_FREQ;

typedef struct
{
    uint8_t     sd_if_type: 2;
    uint8_t     rsv0: 2;
    uint8_t     sd_power_en: 1;
    uint8_t     sd_power_high_active: 1;
    uint8_t     sdh_bus_width: 2;

    uint8_t     sdh_group: 2;
    uint8_t     sd_debug_en: 1;
    uint8_t     rsv1: 5;
    uint16_t    sd_bus_clk_sel;
    uint8_t     sd_power_pin;
    RW_DONE_CBACK rw_cback;
    void       *rw_user_data;
} T_SD_CONFIG;

typedef enum
{
    SDH_CLK,
    SDH_CMD,
    SDH_D0,
    SDH_D1,
    SDH_D2,
    SDH_D3,
    SDH_WT_PROT,
    SDH_CD,
} T_SD_PIN_NUM;

/* Exported functions -------------------------------------------------------*/

/**
 * sd.h
 *
 * \brief   Config the sdcard type information for sdcard.
 *
 * \param[in]  app_sd_cfg    Point to sdcard configuration parameters.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * static const T_SD_CONFIG    sd_card_cfg =
 * {
 *     .sd_if_type = SD_IF_SD_CARD,
 *     .sdh_group = GROUP_0,
 *     .sdh_bus_width = SD_BUS_WIDTH_4B,
 *     .sd_bus_clk_sel = SD_BUS_CLK_20M
 * };
 *
 * void sd_init(void)
 * {
 *    sd_config_init((T_SD_CONFIG *)&sd_card_cfg);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_config_init(T_SD_CONFIG *app_sd_cfg);

/**
 * sd.h
 *
 * \brief   Config the pad and pinmux for sdcard.
 *
 * \param[in]  none.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_config_init((T_SD_CONFIG *)&sd_card_cfg);
 *    sd_board_init();
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_board_init(void);

/**
 * sd.h
 *
 * \brief   Initial the sd card.
 *
 * \param[in]  none.
 *
 * \return The sd card init status, 0 is SD_OK.
 * \retval 0         The sd card was initialized successfully.
 * \retval 0x1-0xFF  The sd card was failed to initialized, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_config_init((T_SD_CONFIG *)&sd_card_cfg);
 *    sd_board_init();
 *    sd_card_init();
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_card_init(void);

/**
 * sd.h
 *
 * \brief   Config the pad when sdcard choose to enter dlps.
 *
 * \param[in]  disk_powerdown    The sdcard choose to enter dlps, could be set to true or false.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *    sd_enter_dlps(true);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_enter_dlps(bool disk_powerdown);

/**
 * sd.h
 *
 * \brief   Initialize sdcard when the sdcard choose to exit dlps.
 *
 * \param[in]  disk_powerdown    The sdcard choose to exit dlps, could be set to true or false.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *    sd_exit_dlps(true);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_exit_dlps(bool disk_powerdown);

/**
 * sd.h
 *
 * \brief   Print the sdcard binary data by TRACE_BINARY.
 *
 * \param[in]  p    Point to test buffer for sd read or write.
 * \param[in]  len  The data length to print.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define OPER_SD_CARD_ADDR       ((uint32_t)0x8000)
 * uint8_t *test_buf = NULL;
 *
 * void sd_test(void)
 * {
 *    test_buf = os_mem_alloc(OS_MEM_TYPE_BUFFER, 512);
 *    sd_read(OPER_SD_CARD_ADDR, (uint32_t)test_buf, 512, 1);
 *    sd_print_binary_data(test_buf, 512);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_print_binary_data(uint8_t *p, uint32_t len);

/**
 * sd.h
 *
 * \brief   Erase sdcard from the specified start address to end address.
 *
 * \param[in]  start_addr    The sdcard start address to be erased.
 *
 * \param[in]  end_addr     The sdcard end address to be erased.
 *
 * \return The sd card erase status, 0 is SD_OK.
 * \retval 0         The sd card was erased successfully.
 * \retval 0x1-0xFF  The sd card was failed to erased, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     sd_erase(start_addr, end_addr);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_erase(uint32_t start_addr, uint32_t end_addr);

/**
 * sd.h
 *
 * \brief   Read data from a specified address in sdcard.
 *
 * \param[in]  sector    The specified sdcard address to read.
 *
 * \param[in]  buf       The buffer of sdcard to read data.
 *
 * \param[in]  blk_size   The block size of sdcard to read.
 *
 * \param[in]  blk_num   The block number of sdcard to read.
 *
 * \return The sd card read status, 0 is SD_OK.
 * \retval 0         The sd card was read successfully.
 * \retval 0x1-0xFF  The sd card was failed to read, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_read(OPER_SD_CARD_ADDR, (uint32_t)test_buf, SINGLE_BLOCK_SIZE, BLOCK_NUM);
 *     if (sd_status != 0)
 *     {
 *        IO_PRINT_ERROR0("sd_read fail");
 *        return ;
 *     }
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_read(uint32_t sector, uint32_t buf, uint16_t blk_size, uint16_t blk_num);

/**
 * sd.h
 *
 * \brief   Write data from a specified address in sdcard.
 *
 * \param[in]  sector    The specified sdcard address to write.
 *
 * \param[in]  buf       The buffer of sdcard to write data.
 *
 * \param[in]  blk_size   The block size of sdcard to write.
 *
 * \param[in]  blk_num   The block number of sdcard to write.
 *
 * \return The sd card write status, 0 is SD_OK.
 * \retval 0         The sd card was write successfully.
 * \retval 0x1-0xFF  The sd card was failed to write, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_write(OPER_SD_CARD_ADDR, (uint32_t)test_buf, SINGLE_BLOCK_SIZE, BLOCK_NUM);
 *     if (sd_status != 0)
 *     {
 *        IO_PRINT_ERROR0("sd_write fail");
 *        return ;
 *     }
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_write(uint32_t sector, uint32_t buf, uint16_t blk_size, uint16_t blk_num);

/**
 * sd.h
 *
 * \brief   Set block length for standard capacity SD card.
 *
 * \param[in]   block_len   The block length to set.
 *
 * \return The sd card set block length status, 0 is SD_OK.
 * \retval 0         The sd card block length was set successfully.
 * \retval 0x1-0xFF  The sd card block length was failed to set, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void sd_test(void)
 * {
 *     sd_set_block_len(block_len);
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_set_block_len(uint32_t block_len);

/**
 * sd.h
 *
 * \brief   Start or stop the SD host operation clock.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  NewState   new state of the SD host operation clock, can be set to true or false.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_init(void)
 * {
 *    sd_sdh_clk_cmd(true);
 * }
 *
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
void sd_sdh_clk_cmd(bool NewState);

/**
 * sd.h
 *
 * \brief   Check if the sdcard is in programming state.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The sd card check program status, 0 is SD_OK.
 * \retval 0         The sd card was checked successfully.
 * \retval 0x1-0xFF  The sd card was failed to check, error status can refer to T_SD_STATUS.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sd_test(void)
 * {
 *     uint32_t sd_status = 0;
 *     sd_status = sd_check_program_status();
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
T_SD_STATUS sd_check_program_status(void);

/**
 * sd.h
 *
 * \brief   Get the device block size.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The block size of device.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void test(void)
 * {
 *     uint32_t block_size = 0;
 *     block_size = sd_get_dev_block_size();
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
uint32_t sd_get_dev_block_size(void);

/**
 * sd.h
 *
 * \brief   Get the device capacity in bytes.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  None.
 *
 * \return The capacity of device.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void test(void)
 * {
 *     uint64_t capacity = 0;
 *     capacity = sd_get_dev_capacity();
 * }
 * \endcode
 *
 * \ingroup  SD_Exported_Functions
 */
uint64_t sd_get_dev_capacity(void);

#ifdef __cplusplus
}
#endif

#endif /*__SD_H*/

/******************* (C) COPYRIGHT 2021 Realtek Semiconductor Corporation *****END OF FILE****/

