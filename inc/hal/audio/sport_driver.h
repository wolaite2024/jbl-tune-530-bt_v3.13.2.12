/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      sport_driver.h
* @brief    This file provides all the SPORT firmware functions.
* @details
* @author
* @date      2019-08-26
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _SPORT_DRIVER_H_
#define _SPORT_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include "rtl876x.h"

typedef enum t_sport_id
{
    SPORT_ID0,
    SPORT_ID1,
    SPORT_ID2,
    SPORT_ID3
} T_SPORT_ID;

typedef enum t_sport_scheme
{
    SPORT_SCHEME_DEPENDENT,
    SPORT_SCHEME_SEPARATE,
} T_SPORT_SCHEME;

typedef enum t_sport_clk_src
{
    SPORT_CLK_XTAL,
    SPORT_CLK_PLL,
    SPORT_CLK_SEL_MAX,
} T_SPORT_CLK_SRC;

typedef enum t_sport_format_type
{
    SPORT_FORMAT_I2S,
    SPORT_FORMAT_LEFT_JUSTIFY,
    SPORT_FORMAT_PCM_MODE_A,
    SPORT_FORMAT_PCM_MODE_B,
    SPORT_FORMAT_SEL_MAX,
} T_SPORT_FORMAT_TYPE;

typedef enum t_sport_role
{
    SPORT_ROLE_MASTER,
    SPORT_ROLE_SLAVE,
    SPORT_ROLE_SEL_MAX,
} T_SPORT_ROLE;

typedef enum t_sport_loopback
{
    SPORT_LOOPBACK_DISABLE,
    SPORT_LOOPBACK_TX_TO_RX,
    SPORT_LOOPBACK_RX_TO_TX,
    SPORT_LOOPBACK_SEL_MAX,
} T_SPORT_LOOPBACK;

typedef enum t_sport_channel_mode
{
    SPORT_MODE_STEREO,
    SPORT_MODE_MONO,
    SPORT_MODE_SEL_MAX,
} T_SPORT_CHANNEL_MODE;

typedef enum t_sport_tdm_mode
{
    SPORT_TDM_DISABLE,
    SPORT_TDM_MODE_4,
    SPORT_TDM_MODE_6,
    SPORT_TDM_MODE_8,
} T_SPORT_TDM_MODE;

typedef enum t_sport_bclk_div_ratio
{
    SPORT_BCLK_DIV_RATIO_1 = 1,
    SPORT_BCLK_DIV_RATIO_2,
    SPORT_BCLK_DIV_RATIO_3,
    SPORT_BCLK_DIV_RATIO_4,
} T_SPORT_BCLK_DIV_RATIO;

typedef enum t_sport_fifo_use
{
    SPORT_FIFO_USE_0_REG_0 = BIT0,
    SPORT_FIFO_USE_0_REG_1 = BIT1,
    SPORT_FIFO_USE_1_REG_0 = BIT2,
    SPORT_FIFO_USE_1_REG_1 = BIT3,
} T_SPORT_FIFO_USE;

typedef enum t_sport_direct_out_ch
{
    SPORT_DIRECT_OUT_EN_0 = BIT0,
    SPORT_DIRECT_OUT_EN_1 = BIT1,
    SPORT_DIRECT_OUT_EN_2 = BIT2,
    SPORT_DIRECT_OUT_EN_3 = BIT3,
    SPORT_DIRECT_OUT_EN_4 = BIT4,
    SPORT_DIRECT_OUT_EN_5 = BIT5,
    SPORT_DIRECT_OUT_EN_6 = BIT6,
    SPORT_DIRECT_OUT_EN_7 = BIT7,
} T_SPORT_DIRECT_OUT_EN;

typedef enum t_sport_direct_mode
{
    SPORT_DIRECT_MODE_DISABLE = 0x00,
    SPORT_DIRECT_MODE_SRC_SPORT0 = 0x04,
    SPORT_DIRECT_MODE_SRC_SPORT1 = 0x05,
    SPORT_DIRECT_MODE_SRC_SPORT2 = 0x06,
    SPORT_DIRECT_MODE_SRC_SPORT3 = 0x07,
    SPORT_DIRECT_MODE_SEL_MAX,
} T_SPORT_DIRECT_MODE;

typedef enum t_sport_channel_len
{
    SPORT_CHANNEL_LEN_16_BIT,
    SPORT_CHANNEL_LEN_20_BIT,
    SPORT_CHANNEL_LEN_24_BIT,
    SPORT_CHANNEL_LEN_8_BIT,
    SPORT_CHANNEL_LEN_32_BIT,
    SPORT_CHANNEL_LEN_SEL_MAX,
} T_SPORT_CHANNEL_LEN;

typedef enum t_sport_data_len
{
    SPORT_DATA_LEN_16_BIT,
    SPORT_DATA_LEN_20_BIT,
    SPORT_DATA_LEN_24_BIT,
    SPORT_DATA_LEN_8_BIT,
    SPORT_DATA_LEN_32_BIT,
    SPORT_DATA_LEN_MAX
} T_SPORT_DATA_LEN;

typedef enum t_sport_endianness
{
    SPORT_ENDIANNESS_MSB_FIRST,
    SPORT_ENDIANNESS_LSB_FIRST,
    SPORT_ENDIANNESS_SEL_MAX,
} T_SPORT_ENDIANNESS;

typedef enum t_sport_tx_sel
{
    SPORT_TX_FIFO_0_REG_0_L,                        // 0
    SPORT_TX_FIFO_0_REG_0_R,                        // 1
    SPORT_TX_FIFO_0_REG_1_L,                        // 2
    SPORT_TX_FIFO_0_REG_1_R,                        // 3
    SPORT_TX_FIFO_1_REG_0_L,                        // 4
    SPORT_TX_FIFO_1_REG_0_R,                        // 5
    SPORT_TX_FIFO_1_REG_1_L,                        // 6
    SPORT_TX_FIFO_1_REG_1_R,                        // 7
    SPORT_TX_DIRECT_REG,                            // 8

    SPORT_TX_DIRECT_IN_A0 = SPORT_TX_DIRECT_REG,    // 8
    SPORT_TX_DIRECT_IN_A1,                          // 9
    SPORT_TX_DIRECT_IN_A2,                          // 10
    SPORT_TX_DIRECT_IN_A3,                          // 11
    SPORT_TX_DIRECT_IN_A4,                          // 12
    SPORT_TX_DIRECT_IN_A5,                          // 13
    SPORT_TX_DIRECT_IN_A6,                          // 14
    SPORT_TX_DIRECT_IN_A7,                          // 15
    SPORT_TX_DIRECT_IN_B0,                          // 16
    SPORT_TX_DIRECT_IN_B1,                          // 17
    SPORT_TX_DIRECT_IN_B2,                          // 18
    SPORT_TX_DIRECT_IN_B3,                          // 19
    SPORT_TX_DIRECT_IN_B4,                          // 20
    SPORT_TX_DIRECT_IN_B5,                          // 21
    SPORT_TX_DIRECT_IN_B6,                          // 22
    SPORT_TX_DIRECT_IN_B7,                          // 23
    SPORT_TX_DIRECT_IN_C0,                          // 24
    SPORT_TX_DIRECT_IN_C1,                          // 25
    SPORT_TX_DIRECT_IN_C2,                          // 26
    SPORT_TX_DIRECT_IN_C3,                          // 27
    SPORT_TX_DIRECT_IN_C4,                          // 28
    SPORT_TX_DIRECT_IN_C5,                          // 29
    SPORT_TX_DIRECT_IN_C6,                          // 30
    SPORT_TX_DIRECT_IN_C7,                          // 31
    SPORT_TX_SEL_MAX,
} T_SPORT_TX_SEL;

typedef enum t_sport_rx_sel
{
    SPORT_RX_CHANNEL_0,                             // 0
    SPORT_RX_CHANNEL_1,                             // 1
    SPORT_RX_CHANNEL_2,                             // 2
    SPORT_RX_CHANNEL_3,                             // 3
    SPORT_RX_CHANNEL_4,                             // 4
    SPORT_RX_CHANNEL_5,                             // 5
    SPORT_RX_CHANNEL_6,                             // 6
    SPORT_RX_CHANNEL_7,                             // 7
    SPORT_RX_DIRECT_IN_A0,                          // 8
    SPORT_RX_DIRECT_IN_A1,                          // 9
    SPORT_RX_DIRECT_IN_A2,                          // 10
    SPORT_RX_DIRECT_IN_A3,                          // 11
    SPORT_RX_DIRECT_IN_A4,                          // 12
    SPORT_RX_DIRECT_IN_A5,                          // 13
    SPORT_RX_DIRECT_IN_A6,                          // 14
    SPORT_RX_DIRECT_IN_A7,                          // 15
    SPORT_RX_DIRECT_IN_B0,                          // 16
    SPORT_RX_DIRECT_IN_B1,                          // 17
    SPORT_RX_DIRECT_IN_B2,                          // 18
    SPORT_RX_DIRECT_IN_B3,                          // 19
    SPORT_RX_DIRECT_IN_B4,                          // 20
    SPORT_RX_DIRECT_IN_B5,                          // 21
    SPORT_RX_DIRECT_IN_B6,                          // 22
    SPORT_RX_DIRECT_IN_B7,                          // 23
    SPORT_RX_DIRECT_IN_C0,                          // 24
    SPORT_RX_DIRECT_IN_C1,                          // 25
    SPORT_RX_DIRECT_IN_C2,                          // 26
    SPORT_RX_DIRECT_IN_C3,                          // 27
    SPORT_RX_DIRECT_IN_C4,                          // 28
    SPORT_RX_DIRECT_IN_C5,                          // 29
    SPORT_RX_DIRECT_IN_C6,                          // 30
    SPORT_RX_DIRECT_IN_C7,                          // 31
    SPORT_RX_SEL_MAX,
} T_SPORT_RX_SEL;

typedef enum t_sport_config_dir
{
    SPORT_CONFIG_TX = 1,
    SPORT_CONFIG_RX,
    SPORT_CONFIG_TRX,    // for bitmap usage
} T_SPORT_CONFIG_DIR;

typedef struct t_sport_trx_cfg
{
    uint32_t tx_sample_rate;
    uint32_t rx_sample_rate;
    T_SPORT_FORMAT_TYPE tx_format_type;
    T_SPORT_FORMAT_TYPE rx_format_type;
    T_SPORT_CHANNEL_MODE tx_channel_mode;
    T_SPORT_CHANNEL_MODE rx_channel_mode;
    T_SPORT_DATA_LEN tx_data_length;
    T_SPORT_DATA_LEN rx_data_length;
    T_SPORT_CHANNEL_LEN tx_channel_length;
    T_SPORT_CHANNEL_LEN rx_channel_length;
    T_SPORT_TDM_MODE tx_tdm_mode;
    T_SPORT_TDM_MODE rx_tdm_mode;
} T_SPORT_TRX_CFG;

typedef struct t_sport_data_sel_cfg
{
    T_SPORT_DIRECT_MODE direct_mode;
    uint8_t direct_out_en; // bit map, T_SPORT_DIRECT_OUT_EN
    uint8_t tx_channel_map[8];
    uint8_t rx_fifo_map[8];
} T_SPORT_DATA_SEL_CFG;

typedef struct t_sport_misc_cfg
{
    T_SPORT_ROLE role;
    T_SPORT_LOOPBACK self_loopback;
    uint8_t mode_40mhz;
} T_SPORT_MISC_CFG;

/*============================================================================*
 *                         Constants
 *============================================================================*/

/*============================================================================*
 *                         Functions
 *============================================================================*/

void sport_drv_reset(T_SPORT_ID id);
void sport_drv_clock_src_sel(T_SPORT_ID id, T_SPORT_CLK_SRC sport_clk);
void sport_drv_codec_sel(T_SPORT_ID id, bool is_internal);
void sport_drv_enable_module(T_SPORT_ID id, bool is_enable);
void sport_drv_dsp_ctrl_mode(T_SPORT_ID id, bool is_enable);

/* trx start & stop control */
void sport_drv_trx_start_ctrl(T_SPORT_ID id, uint8_t dir, bool is_enable);
void sport_drv_mcu_rx_int_ctrl(T_SPORT_ID id, uint8_t dir, bool is_enable);

uint32_t sport_drv_get_fifo_data(T_SPORT_ID id);

/* sport config API */
void sport_drv_trx_config_default(T_SPORT_TRX_CFG *config);
void sport_drv_trx_config_set(T_SPORT_ID id, T_SPORT_TRX_CFG *config);

void sport_drv_data_sel_config_default(T_SPORT_DATA_SEL_CFG *config);
void sport_drv_data_sel_config_set(T_SPORT_ID id, T_SPORT_DATA_SEL_CFG *config);

void sport_drv_misc_config_default(T_SPORT_MISC_CFG *config);
void sport_drv_misc_config_set(T_SPORT_ID id, T_SPORT_MISC_CFG *config);

/* mclk control */
void sport_drv_mclk_enable(bool enable);
void sport_drv_mclk_conifg_set(uint8_t mclk_pin, float mclk_freq);
/* other function */
void sport_drv_direct_set_sample_rate(T_SPORT_ID id, uint8_t fixed_bclk,
                                      uint32_t tx_bclk_setting,
                                      uint32_t tx_bclk_div, uint32_t rx_bclk_setting, uint32_t rx_bclk_div);
void sport_drv_dump_setting(T_SPORT_ID id);

#ifdef __cplusplus
}
#endif

#endif /* _SPORT_DRIVER_H_ */

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/


