/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/

#ifndef _CODEC_DRIVER_H_
#define _CODEC_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version Description
 *
 * AVCC control flow - [BB2 AVCC control flow_v2.docx](20210316)
 * Codec register command - [BB2_Codec_Register_Commanding_Procedure_v5.xlsx](20210401)
 * sidetone control - [BB2 HW Sidetone@5M_v1.docx], [BB2 HW Sidetone@ds_v1.docx](20210722)
 *
 */

#define CODEC_BYPASS_APAD_MUTE      0   // bypass APAD output if FT DC K not ready

/* Enumeration */
typedef enum t_codec_cb_event
{
    CODEC_CB_EVENT_NONE,
    CODEC_CB_STATE_MSG,
    CODEC_CB_GET_ANC_STATUS,
    CODEC_CB_CLEAR_SIDETONE,
    CODEC_CB_EVENT_MAX,
} T_CODEC_CB_EVENT;

typedef enum t_codec_state
{
    CODEC_STATE_OFF,
    CODEC_STATE_MUTE,
    CODEC_STATE_ON,
    CODEC_STATE_MAX,
} T_CODEC_STATE;

typedef enum t_codec_info
{
    CODEC_INFO_CUR_STATE,
    CODEC_INFO_CFG_STATE,
    CODEC_INFO_CONVERT_SAMPLE_RATE,
    CODEC_INFO_CONVERT_MIC_VOLTAGE,
} T_CODEC_INFO;

typedef enum t_codec_dac_ana_gain
{
    CODEC_DAC_ANA_GAIN_0,       // 0,      0dB
    CODEC_DAC_ANA_GAIN_0P5,     // 1,   -0.5dB
    CODEC_DAC_ANA_GAIN_1,       // 2,     -1dB
    CODEC_DAC_ANA_GAIN_1P5,     // 3,   -1.5dB
    CODEC_DAC_ANA_GAIN_2,       // 4,     -2dB
    CODEC_DAC_ANA_GAIN_2P5,     // 5,   -2.5dB
    CODEC_DAC_ANA_GAIN_3,       // 6,     -3dB
    CODEC_DAC_ANA_GAIN_3P5,     // 7,   -3.5dB
    CODEC_DAC_ANA_GAIN_4,       // 8,     -4dB
    CODEC_DAC_ANA_GAIN_4P5,     // 9,   -4.5dB
    CODEC_DAC_ANA_GAIN_5,       // 10,    -5dB
    CODEC_DAC_ANA_GAIN_5P5,     // 11,  -5.5dB
    CODEC_DAC_ANA_GAIN_6,       // 12,    -6dB
    CODEC_DAC_ANA_GAIN_6P5,     // 13,  -6.5dB
    CODEC_DAC_ANA_GAIN_7,       // 14,    -7dB
    CODEC_DAC_ANA_GAIN_7P5,     // 15,  -7.5dB
    CODEC_DAC_ANA_GAIN_8,       // 16,    -8dB
    CODEC_DAC_ANA_GAIN_8P5,     // 17,  -8.5dB
    CODEC_DAC_ANA_GAIN_9,       // 18,    -9dB
    CODEC_DAC_ANA_GAIN_9P5,     // 19,  -9.5dB
    CODEC_DAC_ANA_GAIN_10,      // 20,   -10dB
    CODEC_DAC_ANA_GAIN_10P5,    // 21, -10.5dB
    CODEC_DAC_ANA_GAIN_11,      // 22,   -11dB
    CODEC_DAC_ANA_GAIN_11P5,    // 23, -11.5dB
    CODEC_DAC_ANA_GAIN_12,      // 24,   -12dB
    CODEC_DAC_ANA_GAIN_12P5,    // 25, -12.5dB
    CODEC_DAC_ANA_GAIN_13,      // 26,   -13dB
    CODEC_DAC_ANA_GAIN_13P5,    // 27, -13.5dB
    CODEC_DAC_ANA_GAIN_14,      // 28,   -14dB
    CODEC_DAC_ANA_GAIN_14P5,    // 29, -14.5dB
    CODEC_DAC_ANA_GAIN_15,      // 30,   -15dB
    CODEC_DAC_ANA_GAIN_15P5,    // 31, -15.5dB
    CODEC_DAC_ANA_GAIN_MAX,
} T_CODEC_DAC_ANA_GAIN;

typedef enum t_codec_adc_ana_gain
{
    CODEC_ADC_ANA_GAIN_0,       // 0dB
    CODEC_ADC_ANA_GAIN_3,       // 3dB
    CODEC_ADC_ANA_GAIN_6,       // 6dB
    CODEC_ADC_ANA_GAIN_9,       // 9dB
    CODEC_ADC_ANA_GAIN_12,      // 12dB
    CODEC_ADC_ANA_GAIN_18,      // 18dB
    CODEC_ADC_ANA_GAIN_24,      // 24dB
    CODEC_ADC_ANA_GAIN_30,      // 30dB
    CODEC_ADC_ANA_GAIN_MAX,
} T_CODEC_ADC_ANA_GAIN;

typedef enum t_codec_adc_boost_gain
{
    CODEC_ADC_BOOST_GAIN_0,     // 0dB
    CODEC_ADC_BOOST_GAIN_12,    // 12dB
    CODEC_ADC_BOOST_GAIN_24,    // 24dB
    CODEC_ADC_BOOST_GAIN_36,    // 36dB
    CODEC_ADC_BOOST_GAIN_MAX,
} T_CODEC_ADC_BOOST_GAIN;

typedef enum t_codec_dmic_clk_sel
{
    DMIC_CLK_5MHZ,
    DMIC_CLK_2P5MHZ,
    DMIC_CLK_1P25MHZ,
    DMIC_CLK_625kHZ,
    DMIC_CLK_312P5kHZ,
    DMIC_CLK_RSV_1,
    DMIC_CLK_RSV_2,
    DMIC_CLK_769P2kHZ,
    DMIC_CLK_SEL_MAX,
} T_CODEC_DMIC_CLK_SEL;

typedef enum t_codec_ad_src_sel
{
    AD_SRC_DMIC,
    AD_SRC_AMIC,
    AD_SRC_MAX,
} T_CODEC_AD_SRC_SEL;

typedef enum t_codec_i2s_channel_sel
{
    I2S_CHANNEL_0,
    I2S_CHANNEL_1,
    I2S_CHANNEL_2,      // Rx only
    I2S_CHANNEL_MAX,
} T_CODEC_I2S_CHANNEL_SEL;

typedef enum t_codec_adc_channel_sel
{
    ADC_CHANNEL0,
    ADC_CHANNEL1,
    ADC_CHANNEL2,
    ADC_CHANNEL3,
    ADC_CHANNEL4,
    ADC_CHANNEL5,
    ADC_CHANNEL_MAX,
} T_CODEC_ADC_CHANNEL_SEL;

typedef enum t_codec_dac_channel_sel
{
    DAC_CHANNEL_L,
    DAC_CHANNEL_R,
    DAC2_CHANNEL,
    DAC_CHANNEL_MAX,
} T_CODEC_DAC_CHANNEL_SEL;

typedef enum t_codec_spk_channel_sel
{
    SPK_CHANNEL_L,
    SPK_CHANNEL_R,
    SPK_CHANNEL_MAX,
} T_CODEC_SPK_CHANNEL_SEL;

typedef enum t_codec_dmic_channel_sel
{
    DMIC_CHANNEL1,
    DMIC_CHANNEL2,
    DMIC_CHANNEL3,
    DMIC_CHANNEL_MAX,
} T_CODEC_DMIC_CHANNEL_SEL;

typedef enum t_codec_amic_channel_sel
{
    AMIC_CHANNEL1,
    AMIC_CHANNEL2,
    AMIC_CHANNEL3,
    AMIC_CHANNEL4,
    AMIC_CHANNEL_MAX,
} T_CODEC_AMIC_CHANNEL_SEL;

typedef enum t_codec_adc_src_amic_sel
{
    ADC_SRC_AMIC1,
    ADC_SRC_AMIC2,
    ADC_SRC_AMIC3,
    ADC_SRC_AMIC4,
    ADC_SRC_AMIC_MAX,
} T_CODEC_ADC_SRC_AMIC_SEL;

typedef enum t_codec_adc_src_dmic_sel
{
    ADC_SRC_RISING_DMIC1,
    ADC_SRC_FALLING_DMIC1,
    ADC_SRC_RISING_DMIC2,
    ADC_SRC_FALLING_DMIC2,
    ADC_SRC_RISING_DMIC3,
    ADC_SRC_FALLING_DMIC3,
    ADC_SRC_DMIC_MAX,
} T_CODEC_ADC_SRC_DMIC_SEL;

typedef enum t_codec_sidetone_channel_sel
{
    SIDETONE_CHANNEL_L,
    SIDETONE_CHANNEL_R,
    SIDETONE_CHANNEL_MAX,
} T_CODEC_SIDETONE_CHANNEL_SEL;

typedef enum t_codec_spk_type_sel
{
    SPK_TYPE_SINGLE_END,
    SPK_TYPE_DIFFERENTIAL,
    SPK_TYPE_CAPLESS,
    SPK_TYPE_MAX,
} T_CODEC_SPK_TYPE_SEL;

typedef enum t_codec_anc_channel_sel
{
    ANC_CHANNEL_EXT_L,
    ANC_CHANNEL_EXT_R,
    ANC_CHANNEL_INT_L,
    ANC_CHANNEL_INT_R,
    ANC_CHANNEL_MAX,
} T_CODEC_ANC_CHANNEL_SEL;

typedef enum t_codec_i2s_datalen_sel
{
    I2S_DATALEN_16,
    I2S_DATALEN_32,
    I2S_DATALEN_24,
    I2S_DATALEN_8,
    I2S_DATALEN_MAX,
} T_CODEC_I2S_DATALEN_SEL;

typedef enum t_codec_i2s_ch_len_sel
{
    CODEC_I2S_CH_LEN_16,
    CODEC_I2S_CH_LEN_32,
    CODEC_I2S_CH_LEN_24,
    CODEC_I2S_CH_LEN_8,
    CODEC_I2S_CH_LEN_MAX,
} T_CODEC_I2S_CH_LEN_SEL;

typedef enum t_codec_i2s_tx_ch_sel
{
    I2S_TX_CH_SEL_L_R,
    I2S_TX_CH_SEL_R_L,
    I2S_TX_CH_SEL_L_L,
    I2S_TX_CH_SEL_R_R,
    I2S_TX_CH_SEL_MAX,
} T_CODEC_I2S_TX_CH_SEL;

typedef enum t_codec_i2s_rx_ch_sel
{
    I2S_RX_CH_SEL_ADC0,
    I2S_RX_CH_SEL_ADC1,
    I2S_RX_CH_SEL_ADC2,
    I2S_RX_CH_SEL_ADC3,
    I2S_RX_CH_SEL_ADC4,
    I2S_RX_CH_SEL_ADC5,
    I2S_RX_CH_SEL_ADC6,
    I2S_RX_CH_SEL_ADC7,
    I2S_RX_CH_SEL_MAX,
} T_CODEC_I2S_RX_CH_SEL;

typedef enum t_codec_i2s_rx_ch
{
    I2S_RX_CH_0,
    I2S_RX_CH_1,
    I2S_RX_CH_2,
    I2S_RX_CH_3,
    I2S_RX_CH_4,
    I2S_RX_CH_5,
    I2S_RX_CH_6,
    I2S_RX_CH_7,
    I2S_RX_CH_MAX,
} T_CODEC_I2S_RX_CH;

typedef enum t_codec_input_dev
{
    INPUT_DEV_NONE,
    INPUT_DEV_MIC,
    INPUT_DEV_AUX,
    INPUT_DEV_MAX,
} T_CODEC_INPUT_DEV;

typedef enum t_codec_micbias_level
{
    MICBIAS_LEVEL_1P05V,
    MICBIAS_LEVEL_1P00V,
    MICBIAS_LEVEL_0P90V,
    MICBIAS_LEVEL_VCM,
    MICBIAS_LEVEL_NUM,
} T_CODEC_MICBIAS_LEVEL;

typedef enum t_codec_micbias_vol
{
    MICBIAS_VOLTAGE_1P8VDRV_1P3V_2P1VDRV_1P5V,
    MICBIAS_VOLTAGE_1P8VDRV_1P4V_2P1VDRV_1P6V,
    MICBIAS_VOLTAGE_1P8VDRV_1P5V_2P1VDRV_1P7V,
    MICBIAS_VOLTAGE_1P8VDRV_1P6V_2P1VDRV_1P8V,
    MICBIAS_VOLTAGE_1P8VDRV_1P7V_2P1VDRV_1P9V,
    MICBIAS_VOLTAGE_1P8VDRV_1P8V_2P1VDRV_2P0V,
    MICBIAS_VOLTAGE_1P8VDRV_1P9V_2P1VDRV_2P1V,
    MICBIAS_VOLTAGE_1P8VDRV_2P0V_2P1VDRV_2P1V,
    MICBIAS_VOLTAGE_NUM,
} T_CODEC_MICBIAS_VOL;

typedef enum t_codec_sidetone_boost_gain
{
    CODEC_SIDETONE_BOOST_GAIN_0,    // 0dB
    CODEC_SIDETONE_BOOST_GAIN_12,   // 12dB
    CODEC_SIDETONE_BOOST_GAIN_MAX,
} T_CODEC_SIDETONE_BOOST_GAIN;

typedef enum t_codec_sidetone_mic_src
{
    CODEC_SIDETONE_MIC_SRC_PRIMARY,
    CODEC_SIDETONE_MIC_SRC_AUXILIARY,
    CODEC_SIDETONE_MIC_SRC_MAX,
} T_CODEC_SIDETONE_MIC_SRC;

typedef enum t_codec_sidetone_input
{
    CODEC_SIDETONE_INPUT_L,
    CODEC_SIDETONE_INPUT_R,
    CODEC_SIDETONE_INPUT_L_R_AVG,
    CODEC_SIDETONE_INPUT_MAX,
} T_CODEC_SIDETONE_INPUT;

typedef enum t_codec_config_sel
{
    CODEC_CONFIG_SEL_BIAS,
    CODEC_CONFIG_SEL_SIDETONE,
    CODEC_CONFIG_SEL_SPK,
    CODEC_CONFIG_SEL_DAC,
    CODEC_CONFIG_SEL_AMIC,
    CODEC_CONFIG_SEL_DMIC,
    CODEC_CONFIG_SEL_ADC,
    CODEC_CONFIG_SEL_I2S,
    CODEC_CONFIG_SEL_MAX,
} T_CODEC_CONFIG_SEL;

typedef enum t_codec_offset_direction
{
    CODEC_DAC_DC_DIR_RSVD,
    CODEC_DAC_DC_DIR_NEG,
    CODEC_DAC_DC_DIR_POS,
    CODEC_DAC_DC_DIR_RSVD2,
} T_CODEC_DAC_DC_DIRECTION;

typedef enum t_sport_tdm_mode_sel
{
    TDM_2_CHANNEL,
    TDM_4_CHANNEL,
    TDM_6_CHANNEL,
    TDM_8_CHANNEL,
    TDM_CHANNEL_MAX,
} T_SPORT_TDM_MODE_SEL;

typedef enum t_sport_data_format_sel
{
    FORMAT_I2S,
    FORMAT_LEFT_JUSTIFY,
    FORMAT_PCM_MODE_A,
    FORMAT_PCM_MODE_B,
    FORMAT_MAX,
} T_SPORT_DATA_FORMAT_SEL;

typedef enum t_codec_eq_config_path
{
    CODEC_EQ_CONFIG_PATH_DAC = 0,
    CODEC_EQ_CONFIG_PATH_ADC,
    CODEC_EQ_CONFIG_PATH_MAX,
} T_CODEC_EQ_CONFIG_PATH;

typedef enum t_codec_hw_eq_type
{
    CODEC_SPK_HW_EQ = 0,
    CODEC_MIC_HW_EQ = 1,
} T_CODEC_HW_EQ_TYPE;

typedef enum t_codec_downlink_mix
{
    CODEC_DOWNLINK_MIX_NONE = 0x0,
    CODEC_DOWNLINK_MIX_L_R = 0x1,       // mix Rch to Lch
    //CODEC_DOWNLINK_MIX_L_2 = 0x2,     // mix 2ch to Lch
    //CODEC_DOWNLINK_MIX_L_R_2 = 0x3,   // mix Rch, 2ch to Lch
    //CODEC_DOWNLINK_MIX_R_2 = 0x4,     // mix 2ch to Lch
    CODEC_DOWNLINK_MIX_R_L = 0x8,       // mix Lch to Rch
    //CODEC_DOWNLINK_MIX_R_L_2 = 0xC,   // mix Lch,2ch to Rch
} T_CODEC_DOWNLINK_MIX;

typedef struct
{
    uint8_t     msg_type;
    uint16_t    data_len;
    void        *p_data;
} T_CODEC_SCHED_MSG;

typedef union t_codec_feature_map
{
    uint32_t d32;
    struct
    {
        uint32_t dc_k_mode: 2;              // [1:0] T_CODEC_DC_K_MODE
        uint32_t avcc_drv_volt_sel: 2;      // [3:2]
        uint32_t rsvd: 28;
    };
} T_CODEC_FEATURE_MAP;

typedef union t_codec_amic_config
{
    uint8_t d8[6];
    struct __attribute__((packed))
    {
        uint8_t enable;
        T_CODEC_AMIC_CHANNEL_SEL ch_sel;
        uint8_t mic_type;                   // 0: single-end, 1: differential
        uint8_t mic_class;
        T_CODEC_ADC_ANA_GAIN ana_gain;
        T_CODEC_INPUT_DEV input_dev;
    };
} T_CODEC_AMIC_CONFIG;

typedef union t_codec_dmic_config
{
    uint8_t d8[6];
    struct __attribute__((packed))
    {
        uint8_t enable;
        T_CODEC_DMIC_CHANNEL_SEL ch_sel;
        uint8_t mic_type;                   // 0: raising, 1: falling
        T_CODEC_DMIC_CLK_SEL dmic_clk_sel;
        uint8_t rsvd[2];                    // align with T_CODEC_AMIC_CONFIG
    };
} T_CODEC_DMIC_CONFIG;

typedef union t_codec_sidetone_dig_gain
{
    uint8_t d8[2];
    uint16_t val;
} T_CODEC_SIDETONE_DIG_GAIN;

typedef union t_codec_spk_config
{
    uint8_t d8[3];
    struct __attribute__((packed))
    {
        uint8_t power_en;
        uint8_t spk_type;                   // 0: Single end, 1: Differential, 2: Capless, deprecated
        uint8_t spk_class;                  // 0: class D 1: class AB
    };
} T_CODEC_SPK_CONFIG;

typedef union t_codec_dac_config
{
    uint8_t d8[14];
    struct __attribute__((packed))
    {
        uint8_t ana_power_en;
        uint8_t dig_power_en;
        T_CODEC_DAC_ANA_GAIN ana_gain;      // deprecated, control by HW DRE
        uint8_t dig_gain;
        uint8_t music_mute_en;
        uint8_t anc_mute_en;                // BBLite2 have no ANC
        uint8_t equalizer_en;
        uint8_t asrc_en;
        T_CODEC_I2S_CHANNEL_SEL i2s_sel;
        uint32_t sample_rate;
        T_CODEC_DOWNLINK_MIX downlink_mix;
    };
} T_CODEC_DAC_CONFIG;

typedef union t_codec_mic_config
{
    uint8_t d8[6];
    struct __attribute__((packed))
    {
        union
        {
            T_CODEC_AMIC_CONFIG amic;
            T_CODEC_DMIC_CONFIG dmic;
        };
    };
} T_CODEC_MIC_CONFIG;

typedef union t_codec_adc_config
{
    uint8_t d8[15];
    struct __attribute__((packed))
    {
        uint8_t enable;
        uint8_t asrc_en;
        uint8_t dig_gain;
        T_CODEC_ADC_BOOST_GAIN boost_gain;
        uint32_t sample_rate;
        uint8_t loopback;                   // ANC decimation path
        uint8_t equalizer_en;
        uint8_t ad_dchpf_sel;
        T_CODEC_I2S_CHANNEL_SEL i2s_sel;
        T_CODEC_AD_SRC_SEL ad_src_sel;
        uint8_t mic_sel;
    };
} T_CODEC_ADC_CONFIG;

typedef union t_codec_i2s_config
{
    uint8_t d8[24];
    struct __attribute__((packed))
    {
        T_SPORT_TDM_MODE_SEL rx_tdm_mode;           // BBLite2 have no SPORT TDM
        T_SPORT_DATA_FORMAT_SEL tx_data_format;
        T_SPORT_DATA_FORMAT_SEL rx_data_format;
        T_CODEC_I2S_DATALEN_SEL tx_data_len;
        T_CODEC_I2S_DATALEN_SEL rx_data_len;
        T_CODEC_I2S_CH_LEN_SEL tx_channel_len;
        T_CODEC_I2S_CH_LEN_SEL rx_channel_len;
        T_CODEC_I2S_TX_CH_SEL tx_channel_sel;
        uint8_t rx_data_ch_en[I2S_RX_CH_MAX];       // BBLite2 I2S Rx always enable
        T_CODEC_I2S_RX_CH_SEL rx_data_ch_sel[I2S_RX_CH_MAX];

        // open other setting if needed
    };
} T_CODEC_I2S_CONFIG;

typedef union t_codec_bias_config
{
    uint8_t d8[4];
    struct __attribute__((packed))
    {
        uint8_t gpio_mode;
        T_CODEC_MICBIAS_LEVEL micbias_level;
        T_CODEC_MICBIAS_VOL micbias_voltage;
        uint8_t micbias_state;
    };
} T_CODEC_BIAS_CONFIG;

typedef union t_codec_sidetone_config
{
    uint8_t d8[10];
    struct __attribute__((packed))
    {
        T_CODEC_SIDETONE_DIG_GAIN dig_gain;
        //-- special case for above config, since 0xFF is valid
        uint8_t enable;
        T_CODEC_SIDETONE_BOOST_GAIN boost_gain;
        T_CODEC_ADC_CHANNEL_SEL src;
        T_CODEC_SIDETONE_INPUT input;
        uint8_t eq_en;
        uint8_t hpf_en;
        uint8_t hpf_fc_sel;
    };
} T_CODEC_SIDETONE_CONFIG;

typedef union t_codec_gain_data
{
    uint16_t d16;
    struct
    {
        uint16_t dig_gain: 8;
        uint16_t dig_boost_gain: 4;
        uint16_t analog_gain: 4;
    };
} T_CODEC_GAIN_DATA;

/**
 * codec_config.h
 *
 * \brief   Define codec ADC decimation selection.
 *
 * \ingroup CODEC_CONFIG
 */
typedef enum t_codec_adc_deci_sel
{
    CODEC_ADC_DECI_SEL_AMIC,
    CODEC_ADC_DECI_SEL_ANC,
    CODEC_ADC_DECI_SEL_MUSIC,
    CODEC_ADC_DECI_SEL_ANC_MUSIC,
    CODEC_ADC_DECI_SEL_MAX,
} T_CODEC_ADC_DECI_SEL;

/* Define */
typedef bool (*P_CODEC_CBACK)(T_CODEC_CB_EVENT event, uint32_t param);
typedef void (*P_CODEC_REQ_CBACK)(uint32_t param);

#define CODEC_MICBIAS_GPIO_MODE(micbias_pinmux)  \
    ((micbias_pinmux != MICBIAS) && (micbias_pinmux != 0xFF))

// MIC select
#define MIC_SEL_DMIC_1                  0x00
#define MIC_SEL_DMIC_2                  0x01
#define MIC_SEL_AMIC_1                  0x02
#define MIC_SEL_AMIC_2                  0x03
#define MIC_SEL_AMIC_3                  0x04
#define MIC_SEL_AMIC_4                  0x05
#define MIC_SEL_DMIC_3                  0x06
#define MIC_SEL_DISABLE                 0x07

// AMIC types
#define MIC_TYPE_SINGLE_END_AMIC        0x00
#define MIC_TYPE_DIFFERENTIAL_AMIC      0x01

// DMIC types
#define MIC_TYPE_RAISING_DMIC           0x00
#define MIC_TYPE_FALLING_DMIC           0x01

// Basic function
bool codec_drv_init(T_CODEC_FEATURE_MAP feature_map, P_CODEC_CBACK cback);
bool codec_drv_deinit(void);
uint8_t codec_drv_power_on(void);
uint8_t codec_drv_power_off(void);
uint8_t codec_drv_enable(void);
uint8_t codec_drv_disable(void);
uint8_t codec_drv_set_mute(void);

// Config API
void codec_drv_config_init(T_CODEC_CONFIG_SEL config_sel, void *config_ptr);
uint8_t codec_drv_bias_config_set(T_CODEC_BIAS_CONFIG *bias_config, bool set_immediately);
uint8_t codec_drv_sidetone_config_set(T_CODEC_SIDETONE_CHANNEL_SEL ch_sel,
                                      T_CODEC_SIDETONE_CONFIG *sidetone_config,
                                      bool set_immediately);
uint8_t codec_drv_spk_config_set(T_CODEC_SPK_CHANNEL_SEL ch_sel,
                                 T_CODEC_SPK_CONFIG *spk_config,
                                 bool set_immediately);
uint8_t codec_drv_dac_config_set(T_CODEC_DAC_CHANNEL_SEL ch_sel,
                                 T_CODEC_DAC_CONFIG *dac_config,
                                 bool set_immediately);
uint8_t codec_drv_amic_config_set(T_CODEC_AMIC_CHANNEL_SEL mic_sel,
                                  T_CODEC_AMIC_CONFIG *mic_config,
                                  bool set_immediately);
uint8_t codec_drv_dmic_config_set(T_CODEC_DMIC_CHANNEL_SEL mic_sel,
                                  T_CODEC_DMIC_CONFIG *mic_config,
                                  bool set_immediately);
uint8_t codec_drv_adc_config_set(T_CODEC_ADC_CHANNEL_SEL ch_sel,
                                 T_CODEC_ADC_CONFIG *adc_config,
                                 bool set_immediately);
uint8_t codec_drv_i2s_config_set(T_CODEC_I2S_CHANNEL_SEL ch_sel,
                                 T_CODEC_I2S_CONFIG *i2s_config,
                                 bool set_immediately);
uint8_t codec_drv_bias_config_clear(bool set_immediately);
uint8_t codec_drv_sidetone_config_clear(T_CODEC_SIDETONE_CHANNEL_SEL ch_sel, bool set_immediately);
uint8_t codec_drv_spk_config_clear(T_CODEC_SPK_CHANNEL_SEL ch_sel, bool is_set_immediately);
uint8_t codec_drv_dac_config_clear(T_CODEC_DAC_CHANNEL_SEL ch_sel, bool set_immediately);
uint8_t codec_drv_amic_config_clear(T_CODEC_AMIC_CHANNEL_SEL mic_sel, bool set_immediately);
uint8_t codec_drv_dmic_config_clear(T_CODEC_DMIC_CHANNEL_SEL mic_sel, bool set_immediately);
uint8_t codec_drv_adc_config_clear(T_CODEC_ADC_CHANNEL_SEL ch_sel, bool set_immediately);
uint8_t codec_drv_i2s_config_clear(T_CODEC_I2S_CHANNEL_SEL i2s_sel, bool set_immediately);
uint8_t codec_drv_get_mic_ch_sel(uint8_t mcu_config_mic_sel, uint8_t *mic_src);
uint8_t codec_drv_get_mic_src_sel(uint8_t mcu_config_mic_sel, uint8_t mic_type);

// Run time control
void codec_drv_mbias_pow_enable(uint8_t enable);
void codec_drv_micbias_pow_enable(uint8_t enable);
void codec_drv_adda_loopback_enable(uint8_t enable);
void codec_drv_daad_loopback_enable(uint8_t enable);

// codec probe function
/**
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 */
void codec_drv_probe_dump_codec_regs(void);
void codec_drv_probe_write_codec_regs(uint16_t offset, uint32_t value);

// codec eq biquad function
void codec_drv_eq_data_clear(uint8_t eq_type, uint8_t eq_chann);
bool codec_drv_eq_data_set(uint8_t eq_type, uint8_t eq_chann, uint8_t *buf, uint16_t len);

// codec sidetone function
void codec_drv_sidetone_enable(void);
void codec_drv_sidetone_disable(void);
void codec_drv_sidetone_gain_convert(int16_t gain,             uint8_t *boost_gain,
                                     uint8_t *dig_gain);

#ifdef __cplusplus
}
#endif

#endif  // _CODEC_DRIVER_H_

