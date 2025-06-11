/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_ROUTE_H_
#define _AUDIO_ROUTE_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_ROUTE Audio Route
 *
 * \brief   Customize the audio path route configurations.
 */

/**
 * audio_route.h
 *
 * \brief Define Audio route sport index.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_idx
{
    AUDIO_ROUTE_SPORT_0,
    AUDIO_ROUTE_SPORT_1,
    AUDIO_ROUTE_SPORT_2,
    AUDIO_ROUTE_SPORT_3,
    AUDIO_ROUTE_SPORT_NUM,
} T_AUDIO_ROUTE_SPORT_IDX;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport role.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_role
{
    AUDIO_ROUTE_SPORT_ROLE_MASTER,
    AUDIO_ROUTE_SPORT_ROLE_SLAVE,
    AUDIO_ROUTE_SPORT_ROLE_NUM,
} T_AUDIO_ROUTE_SPORT_ROLE;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport codec selection.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_codec_sel
{
    AUDIO_ROUTE_SPORT_CODEC_INTERNAL,
    AUDIO_ROUTE_SPORT_CODEC_EXTERNAL,
    AUDIO_ROUTE_SPORT_CODEC_NUM,
} T_AUDIO_ROUTE_SPORT_CODEC_SEL;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport direction.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_ch_dir
{
    AUDIO_ROUTE_SPORT_CH_DIR_RX,
    AUDIO_ROUTE_SPORT_CH_DIR_TX,
    AUDIO_ROUTE_SPORT_CH_DIR_NUM,
} T_AUDIO_ROUTE_SPORT_CH_DIR;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport mode.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_mode
{
    AUDIO_ROUTE_SPORT_MODE_TDM2,
    AUDIO_ROUTE_SPORT_MODE_TDM4,
    AUDIO_ROUTE_SPORT_MODE_TDM6,
    AUDIO_ROUTE_SPORT_MODE_TDM8,
    AUDIO_ROUTE_SPORT_CH_NUM,
} T_AUDIO_ROUTE_SPORT_MODE;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport format.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_format
{
    AUDIO_ROUTE_SPORT_FORMAT_I2S,
    AUDIO_ROUTE_SPORT_FORMAT_LEFT,
    AUDIO_ROUTE_SPORT_FORMAT_PCM_A,
    AUDIO_ROUTE_SPORT_FORMAT_PCM_B,
    AUDIO_ROUTE_SPORT_FORMAT_NUM,
} T_AUDIO_ROUTE_SPORT_FORMAT;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport data length.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_data_len
{
    AUDIO_ROUTE_SPORT_DATA_LEN_8_BIT,
    AUDIO_ROUTE_SPORT_DATA_LEN_16_BIT,
    AUDIO_ROUTE_SPORT_DATA_LEN_20_BIT,
    AUDIO_ROUTE_SPORT_DATA_LEN_24_BIT,
    AUDIO_ROUTE_SPORT_DATA_LEN_32_BIT,
} T_AUDIO_ROUTE_SPORT_DATA_LEN;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport channel length.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_channel_len
{
    AUDIO_ROUTE_SPORT_CH_LEN_8_BIT,
    AUDIO_ROUTE_SPORT_CH_LEN_16_BIT,
    AUDIO_ROUTE_SPORT_CH_LEN_20_BIT,
    AUDIO_ROUTE_SPORT_CH_LEN_24_BIT,
    AUDIO_ROUTE_SPORT_CH_LEN_32_BIT,
} T_AUDIO_ROUTE_SPORT_CH_LEN;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport channel index.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_channel
{
    AUDIO_ROUTE_SPORT_CH_0,
    AUDIO_ROUTE_SPORT_CH_1,
    AUDIO_ROUTE_SPORT_CH_2,
    AUDIO_ROUTE_SPORT_CH_3,
    AUDIO_ROUTE_SPORT_CH_4,
    AUDIO_ROUTE_SPORT_CH_5,
    AUDIO_ROUTE_SPORT_CH_6,
    AUDIO_ROUTE_SPORT_CH_7,
} T_AUDIO_ROUTE_SPORT_CH;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport sample rate.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_sport_sample_rate
{
    AUDIO_ROUTE_SPORT_SR_DYNAMIC   = 0x0F,
    AUDIO_ROUTE_SPORT_SR_8KHZ      = 0x00,
    AUDIO_ROUTE_SPORT_SR_16KHZ     = 0x01,
    AUDIO_ROUTE_SPORT_SR_32KHZ     = 0x02,
    AUDIO_ROUTE_SPORT_SR_44P1KHZ   = 0x03,
    AUDIO_ROUTE_SPORT_SR_48KHZ     = 0x04,
    AUDIO_ROUTE_SPORT_SR_88P2KHZ   = 0x05,
    AUDIO_ROUTE_SPORT_SR_96KHZ     = 0x06,
    AUDIO_ROUTE_SPORT_SR_192KHZ    = 0x07,
    AUDIO_ROUTE_SPORT_SR_12KHZ     = 0x08,
    AUDIO_ROUTE_SPORT_SR_24KHZ     = 0x09,
    AUDIO_ROUTE_SPORT_SR_11P025KHZ = 0x0a,
    AUDIO_ROUTE_SPORT_SR_22P05KHZ  = 0x0b,
    AUDIO_ROUTE_SPORT_SR_64KHZ     = 0x0c,
    AUDIO_ROUTE_SPORT_SR_NUM,
} T_AUDIO_ROUTE_SPORT_SAMPLE_RATE;

/**
 * audio_route.h
 *
 * \brief Define Audio route mclk rate.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mclk_rate
{
    AUDIO_ROUTE_MCLK_RATE_1P024MHZ,
    AUDIO_ROUTE_MCLK_RATE_1P4112MHZ,
    AUDIO_ROUTE_MCLK_RATE_2P048MHZ,
    AUDIO_ROUTE_MCLK_RATE_2P8224MHZ,
    AUDIO_ROUTE_MCLK_RATE_3P072MHZ,
    AUDIO_ROUTE_MCLK_RATE_4P096MHZ,
    AUDIO_ROUTE_MCLK_RATE_5P6448MHZ,
    AUDIO_ROUTE_MCLK_RATE_6P144MHZ,
    AUDIO_ROUTE_MCLK_RATE_8P192MHZ,
    AUDIO_ROUTE_MCLK_RATE_11P2896MHZ,
    AUDIO_ROUTE_MCLK_RATE_12P288MHZ,
    AUDIO_ROUTE_MCLK_RATE_16P384MHZ,
    AUDIO_ROUTE_MCLK_RATE_22P5792MHZ,
    AUDIO_ROUTE_MCLK_RATE_24P576MHZ,
    AUDIO_ROUTE_MCLK_RATE_32P768MHZ,
    AUDIO_ROUTE_MCLK_RATE_NUM,
} T_AUDIO_ROUTE_MCLK_RATE;

/**
 * audio_route.h
 *
 * \brief Define Audio route logic IO type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_logic_io_type
{
    AUDIO_ROUTE_LOGIC_PRIMARY_MIC,
    AUDIO_ROUTE_LOGIC_SECONDARY_MIC,
    AUDIO_ROUTE_LOGIC_FUSION_MIC,
    AUDIO_ROUTE_LOGIC_INTERNAL_MIC_LEFT,
    AUDIO_ROUTE_LOGIC_EXTERNAL_MIC_LEFT,
    AUDIO_ROUTE_LOGIC_PRIMARY_REF_SPEAKER,
    AUDIO_ROUTE_LOGIC_PRIMARY_AUX_IN,
    AUDIO_ROUTE_LOGIC_PRIMARY_SPEAKER,
    AUDIO_ROUTE_LOGIC_SECONDARY_SPEAKER,
    AUDIO_ROUTE_LOGIC_SECONDARY_AUX_IN,
    AUDIO_ROUTE_LOGIC_BONE_MIC,
    AUDIO_ROUTE_LOGIC_PRIMARY_REF_MIC,
    AUDIO_ROUTE_LOGIC_SECONDARY_REF_SPEAKER,
    AUDIO_ROUTE_LOGIC_SECONDARY_REF_MIC,
    AUDIO_ROUTE_LOGIC_INTERNAL_MIC_RIGHT,
    AUDIO_ROUTE_LOGIC_EXTERNAL_MIC_RIGHT,
    AUDIO_ROUTE_LOGIC_IO_NUM,
} T_AUDIO_ROUTE_LOGIC_IO_TYPE;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical IO type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_physical_io_type
{
    AUDIO_ROUTE_PHYSICAL_IO_MIC,
    AUDIO_ROUTE_PHYSICAL_IO_SPEAKER,
    AUDIO_ROUTE_PHYSICAL_IO_AUX_IN,
    AUDIO_ROUTE_PHYSICAL_IO_NUM,
} T_AUDIO_ROUTE_PHYSICAL_IO_TYPE;

/**
 * audio_route.h
 *
 * \brief Define Audio route ADC channel.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_adc_channel
{
    AUDIO_ROUTE_ADC_CHANNEL0,
    AUDIO_ROUTE_ADC_CHANNEL1,
    AUDIO_ROUTE_ADC_CHANNEL2,
    AUDIO_ROUTE_ADC_CHANNEL3,
    AUDIO_ROUTE_ADC_CHANNEL4,
    AUDIO_ROUTE_ADC_CHANNEL5,
    AUDIO_ROUTE_ADC_CHANNEL_NUM,
} T_AUDIO_ROUTE_ADC_CHANNEL;

/**
 * audio_route.h
 *
 * \brief Define Audio route DAC channel.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_dac_channel
{
    AUDIO_ROUTE_DAC_CHANNEL0,
    AUDIO_ROUTE_DAC_CHANNEL1,
    AUDIO_ROUTE_DAC_CHANNEL2,
    AUDIO_ROUTE_DAC_CHANNEL3,
    AUDIO_ROUTE_DAC_CHANNEL_NUM,
} T_AUDIO_ROUTE_DAC_CHANNEL;

/**
 * audio_route.h
 *
 * \brief Define Audio route DAC mix point.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_dac_mix_point
{
    AUDIO_ROUTE_DAC_MIX_NONE,
    AUDIO_ROUTE_DAC_MIX_POINT0,
    AUDIO_ROUTE_DAC_MIX_POINT1,
} T_AUDIO_ROUTE_DAC_MIX_POINT;

/**
 * audio_route.h
 *
 * \brief Define Audio route SPK.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_sel
{
    AUDIO_ROUTE_SPK1,
    AUDIO_ROUTE_SPK2,
    AUDIO_ROUTE_SPK_NUM,
} T_AUDIO_ROUTE_SPK_SEL;

/**
 * audio_route.h
 *
 * \brief Define Audio route MIC.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_sel
{
    AUDIO_ROUTE_DMIC1,
    AUDIO_ROUTE_DMIC2,
    AUDIO_ROUTE_AMIC1,
    AUDIO_ROUTE_AMIC2,
    AUDIO_ROUTE_AMIC3,
    AUDIO_ROUTE_AMIC4,
    AUDIO_ROUTE_DMIC3,
    AUDIO_ROUTE_MIC_NUM,
} T_AUDIO_ROUTE_MIC_SEL;

/**
 * audio_route.h
 *
 * \brief Define Audio route MIC type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_type
{
    AUDIO_ROUTE_MIC_SINGLE_END,
    AUDIO_ROUTE_MIC_DIFFERENTIAL,
    AUDIO_ROUTE_MIC_FALLING,
    AUDIO_ROUTE_MIC_RAISING,
    AUDIO_ROUTE_MIC_TYPE_NUM,
} T_AUDIO_ROUTE_MIC_TYPE;

/**
 * audio_route.h
 *
 * \brief Define Audio route MIC class.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_mic_class
{
    AUDIO_ROUTE_MIC_NORMAL,
    AUDIO_ROUTE_MIC_LP,
    AUDIO_ROUTE_MIC_SNR,
    AUDIO_ROUTE_MIC_CLASS_NUM,
} T_AUDIO_ROUTE_MIC_CLASS;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical MIC.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_physical_mic
{
    T_AUDIO_ROUTE_ADC_CHANNEL adc_ch;
    T_AUDIO_ROUTE_MIC_SEL     mic_sel;
    T_AUDIO_ROUTE_MIC_TYPE    mic_type;
    T_AUDIO_ROUTE_MIC_CLASS   mic_class;
    bool                      equalizer_en;
    uint8_t                   ana_gain;
    uint8_t                   dig_gain;
    uint8_t                   dig_boost_gain;
    bool                      mic_external;
} T_AUDIO_ROUTE_PHYSICAL_MIC;

/**
 * audio_route.h
 *
 * \brief Define Audio route SPK type.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_type
{
    AUDIO_ROUTE_SPK_SINGLE_END,
    AUDIO_ROUTE_SPK_DIFFERENTIAL,
    AUDIO_ROUTE_SPK_CAPLESS,
    AUDIO_ROUTE_SPK_TYPE_NUM,
} T_AUDIO_ROUTE_SPK_TYPE;

/**
 * audio_route.h
 *
 * \brief Define Audio route SPK Class.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_spk_class
{
    AUDIO_ROUTE_SPK_CLASS_D,
    AUDIO_ROUTE_SPK_CLASS_AB,
    AUDIO_ROUTE_SPK_CLASS_NUM,
} T_AUDIO_ROUTE_SPK_CLASS;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical SPK.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_physical_spk
{
    T_AUDIO_ROUTE_DAC_CHANNEL   dac_ch;
    T_AUDIO_ROUTE_SPK_SEL       spk_sel;
    T_AUDIO_ROUTE_SPK_TYPE      spk_type;
    T_AUDIO_ROUTE_SPK_CLASS     spk_class;
    bool                        equalizer_en;
    uint8_t                     ana_gain;
    uint8_t                     dig_gain;
    bool                        spk_external;
    uint8_t                     mix_en;
} T_AUDIO_ROUTE_PHYSICAL_SPK;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical AUX in.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct  t_audio_route_physical_aux_in
{
    T_AUDIO_ROUTE_ADC_CHANNEL adc_ch;
    T_AUDIO_ROUTE_MIC_SEL     mic_sel;
    T_AUDIO_ROUTE_MIC_TYPE    mic_type;
    T_AUDIO_ROUTE_MIC_CLASS   mic_class;
    bool                      equalizer_en;
    uint8_t                   ana_gain;
    uint8_t                   dig_gain;
    uint8_t                   dig_boost_gain;
    bool                      aux_in_external;
} T_AUDIO_ROUTE_PHYSICAL_AUX_IN;

/**
 * audio_route.h
 *
 * \brief Define Audio route ADC digital gain.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_adc_gain
{
    int16_t adc_gain;
} T_AUDIO_ROUTE_ADC_GAIN;

/**
 * audio_route.h
 *
 * \brief Define Audio route DAC digital gain.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_dac_gain
{
    int16_t dac_gain;
} T_AUDIO_ROUTE_DAC_GAIN;

/**
 * audio_route.h
 *
 * \brief Define Audio route sport rtx config.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_sport_cfg
{
    uint8_t  mode;
    uint8_t  format;
    uint8_t  data_len;
    uint8_t  chann_len;
    uint32_t sample_rate;
} T_AUDIO_ROUTE_SPORT_CFG;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical IO attribute.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef union t_audio_route_physical_io_attr
{
    T_AUDIO_ROUTE_PHYSICAL_MIC      mic_attr;
    T_AUDIO_ROUTE_PHYSICAL_SPK      spk_attr;
    T_AUDIO_ROUTE_PHYSICAL_AUX_IN   aux_in_attr;
} T_AUDIO_ROUTE_PHYSICAL_IO_ATTR;

/**
 * audio_route.h
 *
 * \brief Define Audio route IO polarity.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef enum t_audio_route_io_polarity
{
    AUDIO_ROUTE_IO_POLARITY_FORWARD = 0x00,
    AUDIO_ROUTE_IO_POLARITY_REVERSE = 0x01,
} T_AUDIO_ROUTE_IO_POLARITY;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical path.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_physical_path
{
    T_AUDIO_ROUTE_LOGIC_IO_TYPE     logic_io_type;
    T_AUDIO_ROUTE_PHYSICAL_IO_TYPE  physical_io_type;
    T_AUDIO_ROUTE_PHYSICAL_IO_ATTR  attr;
    T_AUDIO_ROUTE_SPORT_IDX         sport_idx;
    T_AUDIO_ROUTE_SPORT_CH_DIR      sport_ch_dir;
    T_AUDIO_ROUTE_SPORT_CH          sport_ch;
    T_AUDIO_ROUTE_IO_POLARITY       polarity;
} T_AUDIO_ROUTE_PHYSICAL_PATH;

/**
 * audio_route.h
 *
 * \brief Define Audio route physical path group.
 *
 * \ingroup AUDIO_ROUTE
 */
typedef struct t_audio_route_physical_path_group
{
    uint8_t                      physical_path_num;
    T_AUDIO_ROUTE_PHYSICAL_PATH *physical_path;
} T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP;

typedef bool (*P_AUDIO_ROUTE_DAC_GAIN_CBACK)(T_AUDIO_CATEGORY        category,
                                             uint32_t                level,
                                             T_AUDIO_ROUTE_DAC_GAIN *gain);

typedef bool (*P_AUDIO_ROUTE_ADC_GAIN_CBACK)(T_AUDIO_CATEGORY        category,
                                             uint32_t                level,
                                             T_AUDIO_ROUTE_ADC_GAIN *gain);

/**
 * audio_route.h
 *
 * \brief   Initialize the audio route management.
 *
 * \param[in] void
 *
 * \return  The status of initializing the audio route.
 * \retval  true    Audio route was initialized successfully.
 * \retval  false   Audio route was failed to initialized.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_init(void);

/**
 * audio_route.h
 *
 * \brief   Deinitialize the audio route managerment.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_deinit(void);

/**
 * audio_route.h
 *
 * \brief   Register the audio route ADC and DAC gain get callback.
 *
 * \param[in] category    Audio category \ref T_AUDIO_CATEGORY.
 * \param[in] dac_cback   Audio route DAC gain cback \ref P_AUDIO_ROUTE_DAC_GAIN_CBACK.
 * \param[in] adc_cback   Audio route ADC gain cback \ref P_AUDIO_ROUTE_ADC_GAIN_CBACK.
 *
 * \return  The status of registering the ADC and DAC gain cback of audio route.
 * \retval  true    Audio route gain cback was registered successfully.
 * \retval  false   Audio route gain cback was failed to registered.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_gain_register(T_AUDIO_CATEGORY             category,
                               P_AUDIO_ROUTE_DAC_GAIN_CBACK dac_cback,
                               P_AUDIO_ROUTE_ADC_GAIN_CBACK adc_cback);

/**
 * audio_route.h
 *
 * \brief   Unregister the audio route ADC and DAC callback.
 *
 * \param[in] category    Audio category \ref T_AUDIO_CATEGORY.
 *
 * \return  void.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_gain_unregister(T_AUDIO_CATEGORY category);

/**
 * audio_route.h
 *
 * \brief   Set the audio route record device.
 *
 * \param[in] dev Record device
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_record_dev_set(uint32_t dev);

/**
 * audio_route.h
 *
 * \brief   Get the audio route record device.
 *
 * \return  The record device.
 *
 * \ingroup AUDIO_ROUTE
 */
uint32_t audio_route_record_dev_get(void);

/**
 * audio_route.h
 *
 * \brief   Get the audio route ADC gain.
 *
 * \param[in]  category Audio category \ref T_AUDIO_CATEGORY.
 * \param[in]  level    The ADC digital gain level of the audio category.
 * \param[out] gain     The ADC digital gain value of the audio category.
 *
 * \return  The status of getting ADC gain.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_adc_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_ADC_GAIN *gain);

/**
 * audio_route.h
 *
 * \brief   Get the audio route DAC gain.
 *
 * \param[in]  category Audio category \ref T_AUDIO_CATEGORY.
 * \param[in]  level    The DAC digital gain level of the audio category.
 * \param[out] gain     The DAC digital gain value of the audio category.
 *
 * \return  The status of getting DAC gain.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_dac_gain_get(T_AUDIO_CATEGORY        category,
                              uint32_t                level,
                              T_AUDIO_ROUTE_DAC_GAIN *gain);

/**
 * audio_route.h
 *
 * \brief   Get the audio route physical path.
 *
 * \param[in] category    Audio category \ref T_AUDIO_CATEGORY.
 *
 * \return The physical group of audio category.
 *
 * \ingroup AUDIO_ROUTE
 */
T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP audio_route_physical_path_take(T_AUDIO_CATEGORY category);

/**
 * audio_route.h
 *
 * \brief   Free the audio route physical path.
 *
 * \param[in] physical_path_group    Audio route physical_path_group \ref T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP.
 *
 * \ingroup AUDIO_ROUTE
 */
void audio_route_physical_path_give(T_AUDIO_ROUTE_PHYSICAL_PATH_GROUP *physical_path_group);

/**
 * audio_route.h
 *
 * \brief   Register the audio route physical path.
 *
 * \param[in] category              Audio category \ref T_AUDIO_CATEGORY.
 * \param[in] physical_path         Audio route physical path \ref T_AUDIO_ROUTE_PHYSICAL_PATH.
 * \param[in] physical_path_num     Audio route physical path num .
 *
 * \return The status of registering audio route physical path.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_category_path_register(T_AUDIO_CATEGORY             category,
                                        T_AUDIO_ROUTE_PHYSICAL_PATH *physical_path,
                                        uint8_t                      physical_path_num);

/**
 * audio_route.h
 *
 * \brief   Unregister the audio route physical path.
 *
 * \param[in] category   Audio category \ref T_AUDIO_CATEGORY.
 *
 * \return The status of unregistering audio route physical path.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_category_path_unregister(T_AUDIO_CATEGORY category);

/**
* audio_route.h
*
* \brief   Get the SPORT rtx config.
*
* \param[in] sport_id   SPORT ID \ref T_AUDIO_ROUTE_SPORT_IDX.
* \param[in] direction  direction \ref T_AUDIO_ROUTE_SPORT_CH_DIR.
*
* \return The SPORT rtx config.
*
* \ingroup AUDIO_ROUTE
*/
T_AUDIO_ROUTE_SPORT_CFG audio_route_sport_cfg_get(T_AUDIO_ROUTE_SPORT_IDX sport_id,
                                                  T_AUDIO_ROUTE_SPORT_CH_DIR direction);

/**
 * audio_route.h
 *
 * \brief   Get the ramp gain.
 *
 * \param[in] category   Audio category \ref T_AUDIO_CATEGORY.
 *
 * \return The ramp gain.
 *
 * \ingroup AUDIO_ROUTE
 */
int16_t audio_route_ramp_gain_get(T_AUDIO_CATEGORY category);

/**
 * audio_route.h
 *
 * \brief   Enable the logical IO for the specific category.
 *
 * \param[in] category   Audio category \ref T_AUDIO_CATEGORY.
 * \param[in] io         logic IO \ref T_AUDIO_ROUTE_LOGIC_IO_TYPE.
 *
 * \return          The status of enabling the logical IO.
 * \retval true     Logical IO was enabled successfully.
 * \retval false    Logical IO was failed to enable.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_logic_io_enable(T_AUDIO_CATEGORY category, T_AUDIO_ROUTE_LOGIC_IO_TYPE io);

/**
 * audio_route.h
 *
 * \brief   Disable the logical IO for the specific category.
 *
 * \param[in] category   Audio category \ref T_AUDIO_CATEGORY.
 * \param[in] io         logic IO \ref T_AUDIO_ROUTE_LOGIC_IO_TYPE.
 *
 * \return          The status of disabling the logical IO.
 * \retval true     Logical IO was disabled successfully.
 * \retval false    Logical IO was failed to disable.
 *
 * \ingroup AUDIO_ROUTE
 */
bool audio_route_logic_io_disable(T_AUDIO_CATEGORY category, T_AUDIO_ROUTE_LOGIC_IO_TYPE io);

/**
 * audio_route.h
 *
 * \brief   Get mic class type by different mic sel.
 *
 * \param[in] mic_sel   mic id number \ref T_AUDIO_ROUTE_MIC_SEL.
 *
 * \return          The id number of MIC.
 *
 * \ingroup AUDIO_ROUTE
 */
static T_AUDIO_ROUTE_MIC_CLASS audio_route_mic_class_get(uint8_t mic_sel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_ROUTE_H_ */
