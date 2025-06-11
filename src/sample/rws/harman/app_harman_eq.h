#ifndef _APP_HARMAN_EQ_H_
#define _APP_HARMAN_EQ_H_

#include <stdbool.h>
#include <stdint.h>
#include "audio_track.h"
#include "eq_utils.h"

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

typedef enum
{
    EQ_FILTER_SHELVING_LP        = 0x00,
    EQ_FILTER_PEAK               = 0x01,
    EQ_FILTER_SHELVING_HP        = 0x02,
    EQ_FILTER_LOW_PASS           = 0x03,
    EQ_FILTER_HIGH_PASS          = 0x04,
    EQ_FILTER_PASS_THROUGH       = 0x05,
    EQ_FILTER_BAND_PASS_0        = 0x06,
    EQ_FILTER_BAND_PASS_1        = 0x07,
    EQ_FILTER_NOTCH              = 0x08,
    EQ_FILTER_ALL_EQ_TYPE        = 0x09,
    EQ_FILTER_QTY                = 0x0A,
    EQ_FILTER_PEAK2              = 0x0B,
} T_HARMAN_EQ_FILTER_TYPE;

/* As long as it's not CATEGORY_ID_OFF, all are customer EQ*/
typedef enum _t_app_harman_preset_eq
{
    CATEGORY_ID_JAZZ            = 0x01, // preset EQ
    CATEGORY_ID_VOCAL           = 0x02,
    CATEGORY_ID_BASS            = 0x03,
    CATEGORY_ID_ROCK            = 0x04,
    CATEGORY_ID_PIANO           = 0x05,
    CATEGORY_ID_CLUB            = 0x06,
    CATEGORY_ID_STUDIO          = 0x07,
    CATEGORY_ID_EXTREME_BASS    = 0x08,
    CATEGORY_ID_EXTREME_BASS2   = 0x09,
    CATEGORY_ID_DIABLO          = 0x0A,
    CATEGORY_ID_NATURAL         = 0x90,
    CATEGORY_ID_BRIGHT          = 0x91,
    CATEGORY_ID_POWERFUL        = 0x92,
} T_APP_HARMAN_PRESET_EQ;

typedef enum _t_app_harman_eq_caterogy_id
{
    CATEGORY_ID_OFF             = 0x00, // default EQ
    CATEGORY_ID_CUSTOMER_EQ     = 0xC1, // customer EQ, which set by JBL APP, maybe modify preset EQ or newly created
    CATEGORY_ID_PERSONNI_FI_EQ  = 0xC9, //
    CATEGORY_ID_DESIGN_EQ       = 0xCA, // which set in dsp cfg tool
    CATEGORY_ID_CURRENT_EQ      = 0xFF, // current EQ, maybe default EQ or customer EQ or Preset EQ
} T_APP_HARMAN_EQ_CATEGORY_ID;

typedef struct _t_app_harman_eq_band_info
{
    uint8_t  type; // T_HARMAN_EQ_FILTER_TYPE
    char     frequency[4];
    char     gain[4];
    char     q_value[4];
} __attribute__((__packed__)) T_APP_HARMAN_EQ_BAND_INFO;

typedef struct _t_app_harman_eq_info_header
{
    uint8_t category_id;
    uint8_t category_state;
    uint8_t band_count;
    uint32_t calibration;
    uint32_t sample_rate;
    uint32_t left_total_gain;
    uint32_t right_total_gain;
} __attribute__((__packed__)) T_APP_HARMAN_EQ_INFO_HEADER;

#define PHONE_EQ_MAX_STAGE   10
#define DSP_EQ_MAX_STAGE     7

typedef struct _t_app_harman_eq_info
{
    T_APP_HARMAN_EQ_INFO_HEADER header;
    T_APP_HARMAN_EQ_BAND_INFO   band_info[PHONE_EQ_MAX_STAGE];
    uint8_t  rsv1;
    uint16_t rsv2;
} T_APP_HARMAN_EQ_INFO;

typedef struct _t_app_harman_default_eq_info
{
    T_APP_HARMAN_EQ_INFO_HEADER header;
    T_APP_HARMAN_EQ_BAND_INFO   band_info[DSP_EQ_MAX_STAGE];
    uint16_t rsv;
} T_APP_HARMAN_DEFAULT_EQ_INFO;

#define APP_HARMAN_EQ_INFO_HEADER_SIZE      sizeof(T_APP_HARMAN_EQ_INFO_HEADER)
#define APP_HARMAN_EQ_BAND_INFO_SIZE        sizeof(T_APP_HARMAN_EQ_BAND_INFO)

typedef enum
{
    SAVE_EQ_FIELD_ONLY_APPLY      = 0x00,
    SAVE_EQ_FIELD_APPLY_AND_SAVE  = 0x01,
    SAVE_EQ_FIELD_PREVIEW         = 0x02, // Will not send
} T_APP_HARMAN_EQ_SAVE_FIELD;

/* EQ_DATA_TO_DSP */
#define TOTAL_EQ_MAX_STAGE  (PHONE_EQ_MAX_STAGE + DSP_EQ_MAX_STAGE) // 17

typedef enum
{
    EQ_DATA_TYPE_PEAK_FILTER             = 0x00,
    EQ_DATA_TYPE_SHELVING_LP_FILTER      = 0x01,
    EQ_DATA_TYPE_SHELVING_HP_FILTER      = 0x02,
    EQ_DATA_TYPE_LOWPASS_FILTER          = 0x03,
    EQ_DATA_TYPE_HIGHPASS_FILTER         = 0x04,
    EQ_DATA_TYPE_PEAK_FILTER_COOKBOOK    = 0x05, //Cookbook formulae
    EQ_DATA_TYPE_BANDPASS_FILTER         = 0x06,
    EQ_DATA_TYPE_BANDREJECT_FILTER       = 0x07,
    EQ_DATA_TYPE_ALL_PASS_FILTER         = 0x08,
    EQ_DATA_TYPE_ALL_EQ_TYPE             = 0x09,
} T_EQ_DATA_TYPE;

typedef struct _t_app_harman_eq_data_header
{
    uint8_t stage_num;
    uint8_t rsv;
    int16_t global_gain;
} __attribute__((__packed__)) T_APP_HARMAN_EQ_DATA_HEADER;

typedef struct _t_app_harman_eq_data_coefficient
{
    uint32_t freq;
    uint16_t q;    // Q10
    int16_t  gain; // Q7
    uint8_t  type; // T_EQ_DATA_TYPE
    uint8_t  rsv;
} __attribute__((__packed__)) T_APP_HARMAN_EQ_DATA_COEFFICIENT;

typedef struct _t_app_harman_eq_data
{
    T_APP_HARMAN_EQ_DATA_HEADER      eq_header;
    T_APP_HARMAN_EQ_DATA_COEFFICIENT eq_coeff[TOTAL_EQ_MAX_STAGE];
    uint16_t  rsv;
} T_APP_HARMAN_EQ_DATA;

#define EQ_HEAD     sizeof(T_APP_HARMAN_EQ_DATA_HEADER)      // 4
#define EQ_COEFF    sizeof(T_APP_HARMAN_EQ_DATA_COEFFICIENT) // 10

#define PHONE_EQ_MAX_SIZE   (EQ_HEAD + EQ_COEFF * PHONE_EQ_MAX_STAGE)   // 104
#define DSP_EQ_MAX_SIZE     (EQ_HEAD + EQ_COEFF * DSP_EQ_MAX_STAGE)     // 74
/* EQ_DATA_TO_DSP */

/* EQ_CMD_TO_DSP */
#define H2D_HEADER_LEN         (4)
#define H2D_SPK_DEFAULT_EQ     (0x1021)
/* EQ_CMD_TO_DSP */

void app_haraman_eq_init(void);
void app_harman_eq_power_on_handle(void);
void app_harman_set_eq_boost_gain(void);
void app_harman_set_stream_track_started(T_AUDIO_TRACK_STATE state);
uint16_t app_harman_eq_raw_data_len_get(void);
void app_harman_eq_raw_data_get(uint8_t *p_rsp, uint16_t rsp_len);
void app_harman_eq_info_handle(uint16_t feature_id, uint8_t *p_data, uint16_t data_size);
void app_harman_eq_info_load_from_ftl(uint8_t caterogy_id, uint8_t *p_eq_info,
                                      uint16_t eq_info_len);
void app_harman_eq_reset_unsaved(void);
uint16_t app_harman_get_eq_data(T_APP_HARMAN_EQ_INFO *eq_info, T_APP_HARMAN_EQ_DATA *eq_data);

uint16_t app_harman_eq_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                                     uint8_t eq_mode, uint8_t eq_index, uint8_t *eq_data);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* _APP_HARMAN_EQ_H_ */
