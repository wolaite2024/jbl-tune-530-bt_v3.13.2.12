/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DSP_CFG_H_
#define _APP_DSP_CFG_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct
{
    uint32_t bypass_near_end : 1;
    uint32_t bypass_far_end : 1;
    uint32_t aec : 1;
    uint32_t dmnr : 1;
    uint32_t aes : 1;
    uint32_t nr_near_end : 1;
    uint32_t nr_far_end : 1;
    uint32_t pitch_det_near_end : 1;
    uint32_t pitch_det_far_end : 1;
    uint32_t drc_near_end : 1;
    uint32_t drc_far_end : 1;
    uint32_t rmdc_near_end : 1;
    uint32_t hpf_near_end : 1;
    uint32_t hpf_far_end : 1;
    uint32_t eq_near_end8k : 1;
    uint32_t eq_near_end16k : 1;
    uint32_t eq_far_end8k : 1;
    uint32_t eq_far_end16k : 1;
    uint32_t ignore_call_active_flag : 1;
    uint32_t dmnr_post_process : 1;
    uint32_t dmnr_test_mode : 1;
    uint32_t dmnr_bypass_lorr : 1;//0 for L,1 for R,effect if Dmnr and DmnrTestMode are 1
    uint32_t side_tone : 1;
    uint32_t far_end_loopback : 1;
    uint32_t dmnr_middle : 1; // 0: follow the setting of DmnrByPassLorR, 1: output L/2+R/2
    uint32_t nr_near_end128pts : 1;
    uint32_t bone_fusion : 1;
    uint32_t resv : 5;
} T_VOICE_PROC_ENB;

typedef struct
{
    // audio effect
    uint32_t bypass_audio_effect : 1;
    uint32_t mbagc_audio : 1;
    uint32_t eq_audio : 1;
    uint32_t aw_audio : 1;

    // audio pass through
    uint32_t is_enable_normal_apt : 1;
    uint32_t vad_trigger : 1;
    uint32_t dehowling : 1;
    uint32_t nr_near_end : 1;

    uint32_t drc_near_end : 1;
    uint32_t rmdc_near_end : 1;
    uint32_t hpf_near_end : 1;
    uint32_t eq_near_end : 1;

    uint32_t fw_dsp_mix : 1;
    uint32_t bypass_near_end_effect : 1;
    uint32_t limiter : 1;
    uint32_t apt_mic_select : 1; // 0 for mic0, 1 for mic1

    uint32_t aec_alr : 1;
    uint32_t aes512 : 1;
    uint32_t is_enable_ll_apt : 1;
    uint32_t resv : 13;
} T_AUDIO_PROC_ENB;

typedef struct t_app_dsp_eq_spk_header
{
    uint32_t app_default_spk_eq_idx: 8;
    uint32_t reserved: 24;
    uint32_t spk_eq_idx_bitmask;
} T_APP_DSP_EQ_SPK_HEADER;

typedef struct t_app_dsp_eq_mic_header
{
    uint32_t app_default_mic_eq_idx: 8;
    uint32_t reserved: 24;
    uint32_t mic_eq_idx_bitmask;
} T_APP_DSP_EQ_MIC_HEADER;

typedef struct t_app_dsp_eq_line_in_header
{
    uint32_t app_default_line_in_eq_idx: 8;
    uint32_t reserved: 24;
    uint32_t line_in_eq_idx_bitmask;
} T_APP_DSP_EQ_LINE_IN_HEADER;

typedef struct t_app_dsp_eq_sub_param_header
{
    uint32_t offset;
    uint8_t  sample_rate; // old eq is sample_rate, new eq is reserved1
    uint8_t  stream_type;
    uint16_t param_length;
    uint8_t  eq_type;
    uint8_t  eq_idx;
    uint16_t reserved2;
} T_APP_DSP_EQ_SUB_PARAM_HEADER;

typedef struct t_app_dsp_eq_param_header
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t eq_param_len;
    uint16_t eq_num;
    uint16_t reserved2;
    uint32_t eq_spk_applications_num: 8;
    uint32_t voice_eq_applications_num: 8;
    uint32_t reserved3: 16;
    uint32_t eq_mic_applications_num: 8;
    uint32_t reserved4: 24;
    T_APP_DSP_EQ_SPK_HEADER *eq_spk_application_header;
    T_APP_DSP_EQ_MIC_HEADER *eq_mic_application_header;
    T_APP_DSP_EQ_SPK_HEADER *voice_eq_application_header;
    T_APP_DSP_EQ_SUB_PARAM_HEADER *sub_header;
} T_APP_DSP_EQ_PARAM_HEADER;

typedef struct t_app_dsp_eq_param
{
    T_APP_DSP_EQ_PARAM_HEADER *header;
    uint8_t *param;
} T_APP_DSP_EQ_PARAM;

typedef struct t_app_dsp_param_r_only
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t tool_version_info;
    uint16_t user_version;
    uint16_t reserved2;
    uint32_t algo_block_offset;
    uint32_t eq_cmd_block_offset;
    uint32_t gain_table_block_offset;
    uint32_t vad_param_block_offset;
    uint32_t eq_extend_info_offset;
    uint32_t hw_eq_block_offset;
    uint32_t extensible_param_offset;
    uint8_t  reserved[4];
    uint32_t pacakge_features; //need check it in patch
    T_VOICE_PROC_ENB voice_stream_feature_bit;
    T_AUDIO_PROC_ENB audio_stream_feature_bit;
} T_APP_DSP_PARAM_R_ONLY;

typedef struct t_app_dsp_gain_table_block
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t block_len;
    uint16_t table_num;
    uint16_t reserved2;
    uint8_t  audio_dac_category;
    uint8_t  audio_dac_gain_level_min;
    uint8_t  audio_dac_gain_level_max;
    uint8_t  audio_dac_gain_level_default;
    uint8_t  voice_dac_category;
    uint8_t  voice_dac_gain_level_min;
    uint8_t  voice_dac_gain_level_max;
    uint8_t  voice_dac_gain_level_default;
    uint8_t  voice_adc_category;
    uint8_t  voice_adc_gain_level_min;
    uint8_t  voice_adc_gain_level_max;
    uint8_t  voice_adc_gain_level_default;
    uint8_t  record_adc_category;
    uint8_t  record_adc_gain_level_min;
    uint8_t  record_adc_gain_level_max;
    uint8_t  record_adc_gain_level_default;
    uint8_t  aux_dac_category;
    uint8_t  aux_dac_gain_level_min;
    uint8_t  aux_dac_gain_level_max;
    uint8_t  aux_dac_gain_level_default;
    uint8_t  aux_adc_category;
    uint8_t  aux_adc_gain_level_min;
    uint8_t  aux_adc_gain_level_max;
    uint8_t  aux_adc_gain_level_default;
    uint8_t  ringtone_dac_category;
    uint8_t  ringtone_dac_gain_level_min;
    uint8_t  ringtone_dac_gain_level_max;
    uint8_t  ringtone_dac_gain_level_default;
    uint8_t  vp_dac_category;
    uint8_t  vp_dac_gain_level_min;
    uint8_t  vp_dac_gain_level_max;
    uint8_t  vp_dac_gain_level_default;
    uint8_t  apt_dac_category;
    uint8_t  apt_dac_gain_level_min;
    uint8_t  apt_dac_gain_level_max;
    uint8_t  apt_dac_gain_level_default;
    uint8_t  apt_adc_category;
    uint8_t  apt_adc_gain_level_min;
    uint8_t  apt_adc_gain_level_max;
    uint8_t  apt_adc_gain_level_default;
    uint8_t  llapt_dac_category;
    uint8_t  llapt_dac_gain_level_min;
    uint8_t  llapt_dac_gain_level_max;
    uint8_t  llapt_dac_gain_level_default;
    uint8_t  llapt_adc_category;
    uint8_t  llapt_adc_gain_level_min;
    uint8_t  llapt_adc_gain_level_max;
    uint8_t  llapt_adc_gain_level_default;
    uint8_t  anc_dac_category;
    uint8_t  anc_dac_gain_level_min;
    uint8_t  anc_dac_gain_level_max;
    uint8_t  anc_dac_gain_level_default;
    uint8_t  anc_adc_category;
    uint8_t  anc_adc_gain_level_min;
    uint8_t  anc_adc_gain_level_max;
    uint8_t  anc_adc_gain_level_default;
    uint8_t  vad_adc_category;
    uint8_t  vad_adc_gain_level_min;
    uint8_t  vad_adc_gain_level_max;
    uint8_t  vad_adc_gain_level_default;
    uint8_t  reserved3[4];
    uint32_t hw_sidetone_digital_gain : 16;
    uint32_t hw_sidetone_mic_source   : 8;
    uint32_t hw_sidetone_enable       : 1;
    uint32_t hw_sidetone_hpf_enable   : 1;
    uint32_t hw_sidetone_hpf_level    : 4;
    uint32_t hw_sidetone_reserved     : 2;
    uint8_t  reserved4[8];
    int16_t  audio_dac_gain_table[16];
    int16_t  voice_dac_gain_table[16];
    int16_t  voice_adc_gain_table[16];
    int16_t  record_adc_gain_table[16];
    int16_t  aux_dac_gain_table[16];
    int16_t  aux_adc_gain_table[16];
    int16_t  ringtone_dac_gain_table[16];
    int16_t  vp_dac_gain_table[16];
    int16_t  apt_dac_gain_table[16];
    int16_t  apt_adc_gain_table[16];
    int16_t  llapt_dac_gain_table[16];
    int16_t  llapt_adc_gain_table[16];
    int16_t  anc_dac_gain_table[16];
    int16_t  anc_adc_gain_table[16];
    int16_t  vad_adc_gain_table[16];
    int16_t  reserved5[16];
} T_APP_DSP_GAIN_TABLE_BLOCK;

typedef struct t_app_dsp_extensible_brightness
{
    uint8_t brightness_level_max;
    uint8_t brightness_level_default;
    uint16_t reserve;
    uint16_t brightness_gain_table[16];
} T_APP_DSP_EXTENSIBLE_BRIGHTNESS;

typedef struct t_app_dsp_extensible_param
{
    uint16_t sync_word;
    uint16_t extensible_len;
    uint8_t *raw_data; // [type, length, value] ... [type, length, value]
} T_APP_DSP_EXTENSIBLE_PARAM;

typedef struct t_app_dsp_cfg_data
{
    int16_t audio_dac_gain_table[16];
    int16_t voice_dac_gain_table[16];
    int16_t voice_adc_gain_table[16];
    int16_t record_adc_gain_table[16];
    int16_t aux_dac_gain_table[16];
    int16_t aux_adc_gain_table[16];
    int16_t ringtone_dac_gain_table[16];
    int16_t vp_dac_gain_table[16];
    int16_t apt_dac_gain_table[16];
    int16_t apt_adc_gain_table[16];
    int16_t llapt_dac_gain_table[16];
    int16_t llapt_adc_gain_table[16];
    int16_t anc_dac_gain_table[16];
    int16_t anc_adc_gain_table[16];
    int16_t vad_adc_gain_table[16];
    T_APP_DSP_PARAM_R_ONLY dsp_cfg_header;
    T_APP_DSP_EQ_PARAM eq_param;
    T_APP_DSP_EXTENSIBLE_PARAM dsp_extensible_param;
} T_APP_DSP_CFG_DATA;

typedef struct t_app_dsp_cfg_sidetone
{
    int16_t        gain;
    bool           hw_enable;
    uint8_t        hpf_level;
} T_APP_DSP_CFG_SIDETONE;

typedef struct
{
    uint8_t playback_volume_max;
    uint8_t playback_volume_min;
    uint8_t playback_volume_default;
    uint8_t voice_out_volume_max;
    uint8_t voice_out_volume_min;
    uint8_t voice_out_volume_default;
    uint8_t record_volume_max;
    uint8_t record_volume_min;
    uint8_t record_volume_default;
    uint8_t ringtone_volume_max;
    uint8_t ringtone_volume_min;
    uint8_t ringtone_volume_default;
    uint8_t voice_prompt_volume_max;
    uint8_t voice_prompt_volume_min;
    uint8_t voice_prompt_volume_default;
    uint8_t tts_volume_max;
    uint8_t tts_volume_min;
    uint8_t tts_volume_default;
    uint8_t apt_volume_out_max;
    uint8_t apt_volume_out_min;
    uint8_t apt_volume_out_default;
    uint8_t rsv;
    uint8_t voice_volume_in_max;
    uint8_t voice_volume_in_min;
    uint8_t voice_volume_in_default;
    uint8_t apt_volume_in_max;
    uint8_t apt_volume_in_min;
    uint8_t apt_volume_in_default;
    uint8_t line_in_volume_in_max;
    uint8_t line_in_volume_in_min;
    uint8_t line_in_volume_in_default;
    uint8_t line_in_volume_out_max;
    uint8_t line_in_volume_out_min;
    uint8_t line_in_volume_out_default;
    uint8_t vad_volume_max;
    uint8_t vad_volume_min;
    uint8_t vad_volume_default;
} T_APP_DSP_CFG_VOLUME;

extern T_APP_DSP_CFG_DATA *app_dsp_cfg_data;
extern T_APP_DSP_CFG_SIDETONE app_dsp_cfg_sidetone;
extern T_APP_DSP_CFG_VOLUME app_dsp_cfg_vol;

void app_dsp_cfg_load_param_r_data(void *p_data, uint16_t offset, uint16_t size);

void app_dsp_cfg_init(bool is_normal_apt);

#ifdef __cplusplus
}
#endif

#endif
