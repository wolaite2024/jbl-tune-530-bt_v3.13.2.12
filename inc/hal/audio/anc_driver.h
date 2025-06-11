#ifndef _ANC_DRIVER_H_
#define _ANC_DRIVER_H_

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include "bin_loader_driver.h"

// use rtl876x.h macro to avoid non-volatile issue
#define ANC_REG_READ(offset)                 \
    HAL_READ32(ANC_REG_BASE, offset)
#define ANC_REG_WRITE(offset, value)         \
    HAL_WRITE32(ANC_REG_BASE, offset, value)
#define ANC_REG_UPDATE(offset, mask, value)  \
    HAL_UPDATE32((ANC_REG_BASE + offset), mask, value)

// content
#define ANC_DRV_DONT_CARE_4             0xF
#define ANC_DRV_DONT_CARE_8             0xFF
#define ANC_DRV_DONT_CARE_16            0xFFFF
#define ANC_DRV_DONT_CARE_32            0xFFFFFFFF

/* CMD PARAM Maximum and Minimum Length */
#define ANC_DRV_MAX_CMD_PARAM_LEN       255
#define ANC_DRV_SET_PARA_BUF_LEN        256
#define ANC_DRV_REG_GRP_SIZE            21
#define ANC_DRV_REG_GRP_DATA_NUM        5
#define ANC_DRV_REG_WRITE_HEADER        5
#define ANC_DRV_REG_WRITE_BIT_SIZE      12
#define ANC_DRV_EAR_FIT_RESPONSE_NUM    32      // sample
#define ANC_DRV_PALU_PARA_LEN           20      // 5 words
#define ANC_DRV_VOL_SLEW_GAIN_OFS       0

#define ANC_DRV_EQ_PARA_BIQUAD_NUM      4       // 4 biquad for eq

#define PALU_INSTR_MAX_NUM              64


typedef enum t_anc_drv_adsp_para_src_sel
{
    ANC_DRV_ADSP_PARA_SRC_FROM_BIN,
    ANC_DRV_ADSP_PARA_SRC_FROM_TOOL,
} T_ANC_DRV_ADSP_PARA_SRC_SEL;

typedef enum t_anc_drv_log_src_sel
{
    ANC_DRV_LOG_SRC_EXT_L,              // input path
    ANC_DRV_LOG_SRC_EXT_R,              // input path
    ANC_DRV_LOG_SRC_INT_L,              // input path
    ANC_DRV_LOG_SRC_INT_R,              // input path
    ANC_DRV_LOG_SRC_ANC_L,              // output path
    ANC_DRV_LOG_SRC_ANC_R,              // output path
    ANC_DRV_LOG_SRC_MUSIC_L,            // input path
    ANC_DRV_LOG_SRC_MUSIC_R,            // input path
} T_ANC_DRV_LOG_SRC_SEL;

typedef enum t_anc_drv_mic_sel
{
    ANC_DRV_MIC_SEL_EXT_L,
    ANC_DRV_MIC_SEL_EXT_R,
    ANC_DRV_MIC_SEL_INT_L,
    ANC_DRV_MIC_SEL_INT_R,
    ANC_DRV_MIC_SEL_MAX,
} T_ANC_DRV_MIC_SEL;

typedef enum t_anc_drv_lr_sel
{
    ANC_DRV_LR_SEL_L,
    ANC_DRV_LR_SEL_R,
    ANC_DRV_LR_SEL_MAX,
} T_ANC_DRV_LR_SEL;

typedef enum t_au_anc_gain_src
{
    ANC_DRV_GAIN_COMP_SRC_EXT,
    ANC_DRV_GAIN_COMP_SRC_EXT_L = ANC_DRV_GAIN_COMP_SRC_EXT,
    ANC_DRV_GAIN_COMP_SRC_EXT_R,
    ANC_DRV_GAIN_COMP_SRC_INT,
    ANC_DRV_GAIN_COMP_SRC_INT_L = ANC_DRV_GAIN_COMP_SRC_INT,
    ANC_DRV_GAIN_COMP_SRC_INT_R,
    ANC_DRV_GAIN_COMP_SRC_APT,
    ANC_DRV_GAIN_COMP_SRC_APT_L = ANC_DRV_GAIN_COMP_SRC_APT,
    ANC_DRV_GAIN_COMP_SRC_APT_R,
    ANC_DRV_GAIN_COMP_SRC_MAX,
} T_ANC_DRV_GAIN_COMP_SRC;

typedef enum t_anc_drv_en_code_fmt
{
    ANC_DRV_EN_CODE_FMT_ANC_ONLY,
    ANC_DRV_EN_CODE_FMT_APT,
    ANC_DRV_EN_CODE_FMT_OLD_ANC,
} T_ANC_DRV_EN_CODE_FMT;

typedef enum t_anc_info
{
    ANC_INFO_ANC_INITIAL,
    ANC_INFO_EN_CODE_FMT,
    ANC_INFO_TARGET_GAIN,
    ANC_INFO_GAIN_ADDR,
    ANC_INFO_UPDATE_DONE_MASK,
} T_ANC_INFO;

typedef enum t_anc_cb_event
{
    ANC_CB_EVENT_FADE_IN_COMPLETE,
    ANC_CB_EVENT_FADE_OUT_COMPLETE,
    ANC_CB_EVENT_GAIN_ADJUST_COMPLETE,
    ANC_CB_EVENT_EQ_ADJUST_COMPLETE,
    ANC_CB_EVENT_LOAD_CONFIGURATION_COMPLETE,
    ANC_CB_EVENT_LOAD_SCENARIO_COMPLETE,
    ANC_CB_EVENT_LOAD_ADSP_IMAGE,
    ANC_CB_EVENT_LOAD_ADSP_PARA_COMPLETE,
    ANC_CB_EVENT_FILTER_INFO_COMPLETE,
    ANC_CB_EVENT_NONE,
} T_ANC_CB_EVENT;

typedef struct
{
    uint8_t     msg_type;
    uint16_t    data_len;
    void        *p_data;
} T_ANC_SCHED_MSG;

typedef struct t_anc_cmd_pkt
{
    uint8_t param_total_length;
    uint8_t cmd_parameter[ANC_DRV_MAX_CMD_PARAM_LEN];
} T_ANC_CMD_PKT;

/* The Structure of MP extend data */
typedef union t_anc_drv_mp_ext_data
{
    uint32_t d32;
    struct
    {
        uint32_t rsvd1: 3;               /* bit[2:0], reserved */
        uint32_t grp_info: 12;           /* bit[14:3] */
        uint32_t rsvd2: 17;              /* bit[31:15], reserved */
    } data;
} T_ANC_DRV_MP_EXT_DATA;

/* The Structure of ANC_REG_IO_ENABLE_ADDR Register */
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ext_L_en: 1;           /* bit[0] */
        uint32_t ext_R_en: 1;           /* bit[1] */
        uint32_t int_L_en: 1;           /* bit[2] */
        uint32_t int_R_en: 1;           /* bit[3] */
        uint32_t music_L_en: 1;         /* bit[4] */
        uint32_t music_R_en: 1;         /* bit[5] */
        uint32_t anc_out_L_en: 1;       /* bit[6] */
        uint32_t anc_out_R_en: 1;       /* bit[7] */
        uint32_t alu_en: 1;             /* bit[8] */
        uint32_t rsvd: 23;              /* bit[31:9] */
    };
} T_ANC_DRV_IO_EN;

/* The Structure of ANC_REG_SRC_SEL_ADDR Register */
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t ext_src_sel_L: 4;      /* bit[3:0] */
        uint32_t ext_src_sel_R: 4;      /* bit[7:4] */
        uint32_t int_src_sel_L: 4;      /* bit[11:8] */
        uint32_t int_src_sel_R: 4;      /* bit[15:12] */
        uint32_t rsvd: 16;              /* bit[31:16] */
    };
} T_ANC_DRV_SRC_SEL;

/* The Structure of ANC_REG_LOG_ENABLE_ADDR Register */
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t log_en: 1;             /* bit[0] */
        uint32_t log_ds_en: 1;          /* bit[1] */
        uint32_t log_rst: 1;            /* bit[2] */
        uint32_t rsvd_1: 13;            /* bit[15:3] */
        uint32_t log0_src_sel: 3;       /* bit[18:16] */
        uint32_t log1_src_sel: 3;       /* bit[21:19] */
        uint32_t log_format: 2;         /* bit[23:22] */
        uint32_t log_dest_sel: 1;       /* bit[24] */
        uint32_t rsvd_2: 7;             /* bit[31:25] */
    };
} T_ANC_DRV_LOG_ENABLE;

/* The Structure of ANC_DRV_FEATURE_MAP */
typedef union t_anc_drv_feature_map
{
    uint32_t d32;
    struct
    {
        uint32_t set_regs: 1;           /* bit[0], 0: bypass ANC register setting script in driver */
        uint32_t enable_fading: 1;      /* bit[1], 0: bypass ANC fade in/out */
        uint32_t user_mode: 1;          /* bit[2], 0: set tool mode to block ANC user mode operation */
        uint32_t mic_path: 2;           /* bit[4:3], 0: none, 1: ANC mic path, 2: APT mic path */
        uint32_t enable_dehowling: 1;   /* bit[5], 0: bypass llapt dehowling, deprecated */
        uint32_t enable_wns: 1;         /* bit[6], 0: disable WNS limiter, deprecated */
        uint32_t rsvd: 24;              /* bit[30:7] */
        uint32_t read_regs_dbg: 1;      /* bit[31], 0: disable ANC tool read register command for debug */
    };
} T_ANC_DRV_FEATURE_MAP;


typedef union t_anc_drv_llapt_vol_slew
{
    uint32_t d32[7];
    struct
    {
        uint32_t is_binaural;
        uint32_t max_target_gain_l;
        uint32_t max_target_gain_r;
        uint32_t vol_slew_l_addr;
        uint32_t vol_slew_r_addr;
        uint32_t update_done_mask_lsb;
        uint32_t update_done_mask_msb;
    };
} T_ANC_DRV_LLAPT_VOL_SLEW;

// TODO: using ANC FIR
typedef union t_anc_drv_eq_biquad
{
    uint32_t d32[5];
} T_ANC_DRV_EQ_BIQUAD;

typedef union t_anc_drv_eq_param
{
    uint32_t d32[40];
    T_ANC_DRV_EQ_BIQUAD biquad[ANC_DRV_LR_SEL_MAX][ANC_DRV_EQ_PARA_BIQUAD_NUM];
} T_ANC_DRV_EQ_PARAM;

typedef bool (*P_ANC_CBACK)(T_ANC_CB_EVENT event);

/* Design/MP Tool usage */
uint8_t anc_drv_tool_set_para(T_ANC_CMD_PKT *anc_cmd_ptr);
uint32_t anc_drv_tool_reg_read(uint32_t anc_reg_addr);
void anc_drv_set_gain_mismatch(uint8_t gain_src, uint32_t l_gain, uint32_t r_gain);
bool anc_drv_read_gain_mismatch(uint8_t gain_src, uint8_t read_flash,
                                uint32_t *l_gain, uint32_t *r_gain);
bool anc_drv_read_mp_ext_data(T_ANC_DRV_MP_EXT_DATA *mp_ext_data);
bool anc_drv_burn_gain_mismatch(T_ANC_DRV_MP_EXT_DATA mp_ext_data);
void anc_drv_set_llapt_gain_mismatch(uint32_t l_gain, uint32_t r_gain);
bool anc_drv_read_llapt_gain_mismatch(uint8_t read_flash, uint32_t *l_gain, uint32_t *r_gain);
bool anc_drv_read_llapt_ext_data(T_ANC_DRV_MP_EXT_DATA *llapt_ext_data);
bool anc_drv_burn_llapt_gain_mismatch(T_ANC_DRV_MP_EXT_DATA llapt_ext_data);
void anc_drv_set_ear_fit_buffer(uint32_t *buffer);
void anc_drv_get_ear_fit_response(uint32_t *buffer);
void anc_drv_tool_set_feature_map(T_ANC_DRV_FEATURE_MAP feature_map);
uint32_t anc_drv_tool_get_feature_map(void);
void anc_drv_limiter_wns_switch(void);

/* Response Measurement */
bool anc_drv_play_tone_init(bool enable);
bool anc_drv_cos_table_allocate(void);
void anc_drv_config_data_log(T_ANC_DRV_LOG_SRC_SEL src0_sel,
                             T_ANC_DRV_LOG_SRC_SEL src1_sel,
                             uint16_t log_len);
void anc_drv_load_data_log(void);
uint32_t anc_drv_get_data_log_length(void);
void anc_drv_response_measure_start(void);
bool anc_drv_response_measure_enable(uint8_t enable, uint32_t *tx_freq, uint8_t freq_num,
                                     uint8_t amp_ratio);
void anc_drv_preload_cos_table(void);
void anc_drv_set_resp_meas_mode(uint8_t enter_resp_meas_mode);
uint8_t anc_drv_check_resp_meas_mode(void);

/* Fade in/out */
void anc_drv_fading_proceed(void);
void anc_drv_fade_in(void);
void anc_drv_fade_out(void);
uint32_t anc_drv_gain_db_to_amplitude(double gain_db);
void anc_drv_set_gain(double l_gain, double r_gain);

/* EQ contorl */
void anc_drv_eq_set(double strength);

/* Normal usage */
void anc_drv_mic_src_setting(uint8_t ch_sel, uint8_t mic_sel, uint8_t mic_type, uint8_t mic_class);
void anc_drv_mic_src_direct_set(uint8_t ch_sel, uint8_t mic_sel, uint8_t mic_type);
bool anc_drv_param_init(P_ANC_CBACK cback);
void anc_drv_dsp_cfg(void);
void anc_drv_enable(void);
void anc_drv_disable(void);
void anc_drv_get_info(void *buf, T_ANC_INFO info_id);
void anc_drv_get_llapt_vol_slew_addr(uint8_t *buf);
void anc_drv_eq_biquad_set(T_ANC_DRV_EQ_PARAM *eq_biquad);

/* ANC image parsing */
uint8_t anc_drv_get_scenario_info(uint8_t *scenario_mode, uint8_t sub_type,
                                  uint8_t *scenario_apt_effect);
void anc_drv_image_parser(void *anc_header, uint32_t *anc_image);

void anc_drv_set_img_load_param(uint8_t sub_type, uint8_t scenario_id);

/* PALU customer usage */
bool anc_drv_set_ds_rate(uint8_t ds_rate);

void anc_drv_anc_mp_bak_buf_config(uint32_t param1, uint32_t param2);

void anc_drv_ramp_data_write(uint32_t wdata);

void anc_drv_get_scenario_addr(uint32_t param);

void *anc_drv_get_scenario_img(uint8_t sub_type, uint8_t scenario_id, uint32_t *length);

void *anc_drv_get_filter_info_img(uint32_t *length);

void anc_drv_para_aes_encrypt(uint32_t scenario_img, uint32_t length);

void anc_drv_adaptive_anc_boot_up(void);

uint8_t anc_drv_query_para(uint8_t *adsp_info);

uint8_t anc_drv_config_adsp_para(uint16_t crc_check, uint32_t adsp_para_len, uint8_t segment_total);

uint8_t anc_drv_turn_on_adsp(uint8_t enable);

uint8_t anc_drv_enable_adaptive_anc(uint8_t enable, uint8_t grp_idx);

uint8_t anc_drv_load_adsp_para(uint8_t *para_ptr, uint8_t segment_index, uint8_t mode);

uint32_t anc_drv_get_active_adsp_scenario(void);

void anc_drv_set_adsp_load_finished(uint8_t adsp_load_finished);

uint8_t anc_drv_get_adsp_load_finished(void);

void anc_drv_set_adsp_para_source(uint8_t source);

uint8_t anc_drv_get_adsp_para_source(void);

void anc_drv_convert_data_log_addr(uint32_t *log_dest_addr);

uint8_t anc_drv_get_dynamic_para_idx_status(void);

void anc_drv_set_specfic_img(uint32_t *img_buf);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif  /* _ANC_DRIVER_H_ */
