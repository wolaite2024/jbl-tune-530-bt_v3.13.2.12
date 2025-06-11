#if F_APP_HARMAN_FEATURE_SUPPORT
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "pm.h"
#include "rtl876x_pinmux.h"
#include "trace.h"
#include "audio_probe.h"
#include "ftl.h"
#include "os_mem.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_a2dp.h"
#include "app_eq.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_multilink.h"
#include "app_harman_eq.h"
#include "app_harman_vendor_cmd.h"

static uint8_t stream_track_started = AUDIO_TRACK_STATE_RELEASED;

// // default dsp cfg EQ to phone info
// T_APP_HARMAN_EQ_INFO default_eq_info =
// {
//     CATEGORY_ID_OFF, // design EQ
//     0x01, // Apply + Save
//     0x05, // band count is 5
//     0x00, // calibration
//     0x00, // sample rate
//     0x00, // left total gain, may need change to -1.00dB
//     0x00, // right total gain
//     .band_info[0] =
//     {
//         EQ_FILTER_PEAK,
//         {0x00, 0x00, 0xC8, 0x42}, // 100
//         {0x00, 0x00, 0xA0, 0x40}, // 5.00
//         {0x00, 0x00, 0x00, 0x3F}, // 0.50
//     },
//     .band_info[1] =
//     {
//         EQ_FILTER_PEAK,
//         {0x00, 0x00, 0x48, 0x43}, // 200
//         {0x00, 0x00, 0x30, 0xC1}, // -11.00
//         {0xCD, 0xCC, 0x0C, 0x3F}, // 0.55
//     },
//     .band_info[2] =
//     {
//         EQ_FILTER_PEAK,
//         {0x00, 0x40, 0x1C, 0x45},  // 2500
//         {0x00, 0x00, 0x40, 0x40}, // 3.00
//         {0x00, 0x00, 0x00, 0x40}, // 2.00
//     },
//     .band_info[3] =
//     {
//         EQ_FILTER_PEAK,
//         {0x00, 0x40, 0xB5, 0x45}, // 5800
//         {0x00, 0x00, 0xC0, 0xC0}, // -6.00
//         {0x00, 0x00, 0xC0, 0x3F}, // 1.50
//     },
//     .band_info[4] =
//     {
//         EQ_FILTER_PEAK,
//         {0x00, 0x40, 0x1C, 0x46}, // 10000
//         {0x00, 0x00, 0xC0, 0x3F}, // 1.50
//         {0x00, 0x00, 0x80, 0x3F}, // 1.00
//     },
// };

uint16_t app_harman_eq_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                                     uint8_t eq_mode, uint8_t eq_index, uint8_t *eq_data)
{
    uint16_t eq_len = app_eq_dsp_param_get(eq_type, stream_type, eq_mode, eq_index, eq_data);

    if (eq_len > DSP_EQ_MAX_SIZE)
    {
        eq_len = DSP_EQ_MAX_SIZE;
    }

    return eq_len;
}

static uint8_t app_harman_eq_dsp_stage_get(uint16_t eq_data_len)
{
    uint8_t eq_stage = (eq_data_len - EQ_HEAD) / EQ_COEFF;

    return eq_stage;
}

/* convert phone eq info to eq data*/
// floating_point to fix_point, refer to dsp cfg tool
static double Double2FixPoint(double input, int ShiftBit)
{
    double c = pow(2.0, ShiftBit);

    if (input >= 0.0f)
    {
        return ((int32_t)(input * c + 0.5f));
    }

    return ((int32_t)(input * c - 0.5f));
}

// fix_point to floating_point, refer to dsp cfg tool
static double FixPoint2Double(double input, int ShiftBit)
{
    double c = pow(2.0, -ShiftBit);

    return (input * c);
}

// phone eq info's freq to eq data's freq
static uint32_t app_harman_get_eq_data_freq(char *band_freq)
{
    uint32_t data_freq;
    float    f_band_freq;

    memcpy(&f_band_freq, band_freq, 4);

    data_freq = (uint32_t)f_band_freq;

    // DBG_DIRECT("app_harman_get_eq_data_freq: band_freq %f, data_freq 0x%08x", f_band_freq,
    //             data_freq);

    return data_freq;
}

// phone eq info's q to eq data's q
static uint16_t app_harman_get_eq_data_q(char *band_q)
{
    uint16_t data_q;
    float    f_band_q;

    memcpy(&f_band_q, band_q, 4);

    data_q = (uint16_t)Double2FixPoint(f_band_q, 10); // refer to dsp cfg tool

    // DBG_DIRECT("app_harman_get_eq_data_q: band_q %f, data_q 0x%04x", f_band_q, data_q);

    return data_q;
}

// phone eq info's gain to eq data's gain
static int16_t app_harman_get_eq_data_gain(char *band_gain)
{
    int16_t  data_gain;
    float    f_band_gain;

    memcpy(&f_band_gain, band_gain, 4);

    data_gain = (int16_t)Double2FixPoint(f_band_gain, 7); // refer to dsp cfg tool

    // DBG_DIRECT("app_harman_get_eq_data_gain: band_gain %f, data_gain 0x%04x", f_band_gain, data_gain);

    return data_gain;
}

// phone eq info's type to eq data's type
static uint8_t app_harman_get_eq_data_type(uint8_t band_type)
{
    T_EQ_DATA_TYPE data_type;

    switch (band_type)
    {
    case EQ_FILTER_PEAK:
        data_type = EQ_DATA_TYPE_PEAK_FILTER;
        break;

    case EQ_FILTER_SHELVING_LP:
        data_type = EQ_DATA_TYPE_SHELVING_LP_FILTER;
        break;

    case EQ_FILTER_SHELVING_HP:
        data_type = EQ_DATA_TYPE_SHELVING_HP_FILTER;
        break;

    case EQ_FILTER_LOW_PASS:
        data_type = EQ_DATA_TYPE_LOWPASS_FILTER;
        break;

    case EQ_FILTER_HIGH_PASS:
        data_type = EQ_DATA_TYPE_HIGHPASS_FILTER;
        break;

    case EQ_FILTER_PEAK2:
        data_type = EQ_DATA_TYPE_PEAK_FILTER_COOKBOOK;
        break;

    case EQ_FILTER_BAND_PASS_0:
    case EQ_FILTER_BAND_PASS_1:
        data_type = EQ_DATA_TYPE_BANDPASS_FILTER;
        break;

    case EQ_FILTER_NOTCH:
        data_type = EQ_DATA_TYPE_BANDREJECT_FILTER;
        break;

    case EQ_FILTER_PASS_THROUGH:
        data_type = EQ_DATA_TYPE_ALL_PASS_FILTER;
        break;

    case EQ_FILTER_ALL_EQ_TYPE:
        data_type = EQ_DATA_TYPE_ALL_EQ_TYPE;
        break;

    default:
        break;
    }

    // APP_PRINT_TRACE2("app_harman_get_eq_data_type: band_type 0x%x, data_type 0x%x", band_type,
    //                  data_type);

    return (uint8_t)data_type;
}

// phone eq info to eq data
uint16_t app_harman_get_eq_data(T_APP_HARMAN_EQ_INFO *eq_info, T_APP_HARMAN_EQ_DATA *eq_data)
{
    uint8_t i;
    uint8_t phone_stage_num = eq_info->header.band_count;
    // uint8_t global_gain;
    uint16_t eq_len = 0;

    memset(eq_data, 0, sizeof(T_APP_HARMAN_EQ_DATA));

    if (phone_stage_num > PHONE_EQ_MAX_STAGE)
    {
        phone_stage_num = PHONE_EQ_MAX_STAGE;
    }

    eq_data->eq_header.stage_num = phone_stage_num;
    // eq_data->eq_header.global_gain; // 0dB

    for (i = 0; i < phone_stage_num; i++)
    {
        T_APP_HARMAN_EQ_BAND_INFO band_info = eq_info->band_info[i];

        eq_data->eq_coeff[i].freq = app_harman_get_eq_data_freq(band_info.frequency);
        eq_data->eq_coeff[i].q    = app_harman_get_eq_data_q(band_info.q_value);
        eq_data->eq_coeff[i].gain = app_harman_get_eq_data_gain(band_info.gain);
        eq_data->eq_coeff[i].type = app_harman_get_eq_data_type(band_info.type);
    }

    // may also need to add dsp cfg EQ, and send them to dsp
    T_APP_HARMAN_EQ_DATA dsp_eq_data = {0};
    uint16_t dsp_eq_len = 0;
    uint8_t dsp_stage_num = 0;

    dsp_eq_len = app_harman_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
                                             NORMAL_MODE, 0, (uint8_t *)&dsp_eq_data);

    dsp_stage_num = app_harman_eq_dsp_stage_get(dsp_eq_len);

    eq_data->eq_header.stage_num = phone_stage_num + dsp_stage_num;

    memcpy((uint8_t *)eq_data->eq_coeff + EQ_COEFF * phone_stage_num, (uint8_t *)dsp_eq_data.eq_coeff,
           dsp_eq_len - EQ_HEAD);

    eq_len = EQ_HEAD + EQ_COEFF * eq_data->eq_header.stage_num;

    APP_PRINT_TRACE2("app_harman_get_eq_data: sent to dsp eq len (0x%x), data (%b)", eq_len,
                     TRACE_BINARY(eq_len, eq_data));

    return eq_len;
}
/* convert phone eq info to eq data */

/* convert dsp eq data to default phone eq info*/
// eq data's type to phone eq info's type
static uint8_t app_harman_get_eq_info_type(uint8_t data_type)
{
    T_HARMAN_EQ_FILTER_TYPE info_type;

    switch (data_type)
    {
    case EQ_DATA_TYPE_PEAK_FILTER:
        info_type = EQ_FILTER_PEAK;
        break;

    default:
        break;
    }

    // APP_PRINT_TRACE2("app_harman_get_eq_info_type: data_type 0x%x, info_type 0x%x", data_type,
    //                  info_type);

    return (uint8_t)info_type;
}

// eq data's freq to phone eq info's freq
static void app_harman_get_eq_info_freq(uint32_t data_freq, char *info_freq)
{
    float f_data_freq = (float)data_freq;

    memcpy(info_freq, &f_data_freq, sizeof(float));

    // DBG_DIRECT("app_harman_get_eq_info_freq: data_freq %f, info_freq (%x,%x,%x,%x)", f_data_freq,
    //            info_freq[0], info_freq[1], info_freq[2], info_freq[3]);
}

// eq data's gain to phone eq info's gain
static void app_harman_get_eq_info_gain(int16_t data_gain, char *info_gain)
{
    float f_data_gain = (float)FixPoint2Double(data_gain, 7); // refer to dsp cfg tool

    memcpy(info_gain, &f_data_gain, sizeof(float));

    // DBG_DIRECT("app_harman_get_eq_info_gain: data_gain %f, info_gain (%x,%x,%x,%x)", f_data_gain,
    //            info_gain[0], info_gain[1], info_gain[2], info_gain[3]);
}

// eq data's q to phone eq info's q
static void app_harman_get_eq_info_q(uint16_t data_q, char *info_q)
{
    float f_data_q = (float)FixPoint2Double(data_q, 10); // refer to dsp cfg tool

    memcpy(info_q, &f_data_q, sizeof(float));

    // DBG_DIRECT("app_harman_get_eq_info_gain: data_q %f, info_q (%x,%x,%x,%x)", f_data_q,
    //            info_q[0], info_q[1], info_q[2], info_q[3]);
}

static uint16_t app_harman_get_default_eq_info(T_APP_HARMAN_DEFAULT_EQ_INFO *eq_info)
{
    uint8_t i;
    uint16_t eq_info_len = 0;
    T_APP_HARMAN_EQ_DATA dsp_eq_data;
    uint8_t dsp_stage_num = 0;
    uint16_t dsp_eq_len = 0;

    memset(&dsp_eq_data, 0, sizeof(T_APP_HARMAN_EQ_DATA));
    memset(eq_info, 0, sizeof(T_APP_HARMAN_DEFAULT_EQ_INFO));

    dsp_eq_len = app_harman_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
                                             NORMAL_MODE, 0, (uint8_t *)&dsp_eq_data);

    dsp_stage_num = app_harman_eq_dsp_stage_get(dsp_eq_len);

    eq_info->header.band_count = dsp_stage_num;

    for (i = 0; i < dsp_stage_num; i++)
    {
        T_APP_HARMAN_EQ_DATA_COEFFICIENT eq_coeff = dsp_eq_data.eq_coeff[i];

        eq_info->band_info[i].type      = app_harman_get_eq_info_type(eq_coeff.type);
        app_harman_get_eq_info_freq(eq_coeff.freq, eq_info->band_info[i].frequency);
        app_harman_get_eq_info_gain(eq_coeff.gain, eq_info->band_info[i].gain);
        app_harman_get_eq_info_q(eq_coeff.q, eq_info->band_info[i].q_value);
    }

    eq_info_len = APP_HARMAN_EQ_INFO_HEADER_SIZE + APP_HARMAN_EQ_BAND_INFO_SIZE * dsp_stage_num;

    // APP_PRINT_TRACE1("app_harman_get_default_eq_info: len 0x%x, info (%b)", eq_info_len,
    //                  TRACE_BINARY(eq_len,
    //                               eq_info));

    return eq_info_len;
}
/* convert dsp eq data to default phone eq info*/

void app_harman_set_eq_boost_gain(void)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);

    if (p_link == NULL || p_link->a2dp_track_handle == NULL)
    {
        APP_PRINT_TRACE0("app_harman_set_eq_boost_gain ignore");
    }
    else
    {
        if ((stream_track_started == AUDIO_TRACK_STATE_STARTED) ||
            (stream_track_started == AUDIO_TRACK_STATE_RESTARTED))
        {
            uint8_t EQ_BOOST_GAIN[12];
            memset(EQ_BOOST_GAIN, 0, 12);

            EQ_BOOST_GAIN[0] = 0x20;
            EQ_BOOST_GAIN[1] = 0x10;

            EQ_BOOST_GAIN[2] = 0x02;
            EQ_BOOST_GAIN[3] = 0x00;

            EQ_BOOST_GAIN[4] = 0x00;
            EQ_BOOST_GAIN[5] = 0x00;
            EQ_BOOST_GAIN[6] = 0x00;
            EQ_BOOST_GAIN[7] = 0x0C;

            if (app_cfg_nv.harman_category_id == CATEGORY_ID_OFF)
            {
                EQ_BOOST_GAIN[8] = 0x00;
                EQ_BOOST_GAIN[9] = 0x00;
                EQ_BOOST_GAIN[10] = 0x00;
                EQ_BOOST_GAIN[11] = 0x00;
            }
            else
            {
                EQ_BOOST_GAIN[8] = ((int32_t)((int16_t)app_cfg_nv.harman_EQmaxdB)) & 0xFF;
                EQ_BOOST_GAIN[9] = (((int32_t)((int16_t)app_cfg_nv.harman_EQmaxdB)) >> 8) & 0xFF;
                EQ_BOOST_GAIN[10] = (((int32_t)((int16_t)app_cfg_nv.harman_EQmaxdB)) >> 16) & 0xFF;
                EQ_BOOST_GAIN[11] = (((int32_t)((int16_t)app_cfg_nv.harman_EQmaxdB)) >> 24) & 0xFF;
            }
            audio_probe_dsp_send(EQ_BOOST_GAIN, 12);
        }
    }
    APP_PRINT_TRACE3("app_harman_set_eq_boost_gain harman_EQmaxdB=%d,%d,%d",
                     app_cfg_nv.harman_category_id,
                     app_cfg_nv.harman_EQmaxdB,
                     stream_track_started);
}

uint8_t app_harman_eq_utils_stage_num_get(void)
{
    uint8_t stage_num = 0;

    if (app_cfg_nv.harman_category_id != CATEGORY_ID_OFF)
    {
        stage_num = app_cfg_nv.harman_customer_stage;
    }
    else
    {
        // if (eq_len > 4)
        // {
        //     stage_num = ((eq_len - 4) / 10);
        // }
        // else
        // {
        //     stage_num = 0;
        // }
    }

    APP_PRINT_INFO2("app_harman_eq_utils_stage_num_get: harman_category_id: %d, "
                    "harman_customer_stage: %d",
                    app_cfg_nv.harman_category_id, app_cfg_nv.harman_customer_stage);

    return stage_num;
}

void app_harman_eq_power_on_handle(void)
{
    // if (app_cfg_nv.harman_category_id == CATEGORY_ID_OFF)
    // {
    //     uint16_t eq_len;
    //     uint8_t default_eq_data[EQ_SIZE_MAX] = {0};

    //     eq_len = app_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
    //                                   NORMAL_MODE, 0, default_eq_data);

    //     app_cfg_nv.harman_default_EQ_stage_num = default_eq_data[0];

    //     app_cfg_store(&app_cfg_nv.harman_category_id, 8);

    //     ftl_save_to_storage((void *)&default_eq_info,
    //                         APP_HARMAN_DEFAULT_EQ_INFO, APP_HARMAN_DEFAULT_EQ_INFO_SIZE);

    //     APP_PRINT_TRACE3("app_harman_eq_power_on_handle: eq_len: %d, stage_num: %d, p_data(%b)",
    //                      eq_len, app_cfg_nv.harman_default_EQ_stage_num,
    //                      TRACE_BINARY(APP_HARMAN_DEFAULT_EQ_INFO_SIZE, APP_HARMAN_DEFAULT_EQ_INFO));
    // }
}

static void app_harman_eq_data_save_to_ftl(uint8_t *p_eq_data, uint16_t eq_data_len)
{
    APP_PRINT_INFO1("app_harman_eq_data_save_to_ftl: EQ_data_len: %d", eq_data_len);

    if (eq_data_len == APP_HARMAN_EQ_DATA_SIZE)
    {
        ftl_save_to_storage(p_eq_data, APP_HARMAN_EQ_DATA, APP_HARMAN_EQ_DATA_SIZE);
    }
}

void app_harman_eq_info_load_from_ftl(uint8_t caterogy_id, uint8_t *p_eq_info,
                                      uint16_t eq_info_len)
{
    APP_PRINT_INFO2("app_harman_eq_info_load_from_ftl: caterogy_id: 0x%x, EQ_info_len: %d", caterogy_id,
                    eq_info_len);

    if (caterogy_id == CATEGORY_ID_OFF) // design EQ
    {
        if (eq_info_len == APP_HARMAN_DEFAULT_EQ_INFO_SIZE)
        {
            ftl_load_from_storage(p_eq_info, APP_HARMAN_DEFAULT_EQ_INFO, APP_HARMAN_DEFAULT_EQ_INFO_SIZE);
        }
    }
    else // customer EQ or preset EQ
    {
        if (eq_info_len == APP_HARMAN_CUSTOMER_EQ_INFO_SIZE)
        {
            ftl_load_from_storage(p_eq_info, APP_HARMAN_CUSTOMER_EQ_INFO, APP_HARMAN_CUSTOMER_EQ_INFO_SIZE);
        }
    }
}

static void app_harman_eq_info_save_to_ftl(uint8_t caterogy_id, uint8_t *p_eq_info,
                                           uint16_t eq_info_len)
{
    APP_PRINT_INFO2("app_harman_eq_info_save_to_ftl: caterogy_id %d, EQ_info_len: %d", caterogy_id,
                    eq_info_len);

    if (caterogy_id == CATEGORY_ID_OFF) // degign EQ
    {
        if (eq_info_len == APP_HARMAN_DEFAULT_EQ_INFO_SIZE)
        {
            ftl_save_to_storage(p_eq_info, APP_HARMAN_DEFAULT_EQ_INFO, APP_HARMAN_DEFAULT_EQ_INFO_SIZE);
        }
    }
    else // customer EQ or preset EQ
    {
        if (eq_info_len == APP_HARMAN_CUSTOMER_EQ_INFO_SIZE)
        {
            ftl_save_to_storage(p_eq_info, APP_HARMAN_CUSTOMER_EQ_INFO, APP_HARMAN_CUSTOMER_EQ_INFO_SIZE);
        }
    }
}

static void app_harman_eq_dsp_set(T_AUDIO_EFFECT_INSTANCE instance, void *info_buf,
                                  uint16_t info_len,
                                  T_EQ_TYPE eq_type)
{
    APP_PRINT_INFO4("[SD_CHECK]app_harman_eq_dsp_set category_id = %d,customer_stage = %d, info_len = %d, eq_type=%d",
                    app_cfg_nv.harman_category_id,
                    app_cfg_nv.harman_customer_stage,
                    info_len,
                    eq_type);

    if (eq_type == SPK_SW_EQ)
    {
        app_harman_set_eq_boost_gain();
    }

#if F_APP_LEA_SUPPORT
    app_eq_media_eq_set(instance, info_buf, info_len);
#else
    eq_set(instance, info_buf, eq_len);
#endif
}

void app_harman_eq_reset_unsaved(void)
{
    uint8_t category_id = app_cfg_nv.harman_category_id;
    uint16_t eq_len = 0;
    T_APP_HARMAN_EQ_DATA eq_data = {0};
    T_APP_BR_LINK *link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (category_id != CATEGORY_ID_OFF)
    {
        uint8_t harman_eq_info[APP_HARMAN_CUSTOMER_EQ_INFO_SIZE] = {0};

        app_harman_eq_info_load_from_ftl(category_id, harman_eq_info,
                                         APP_HARMAN_CUSTOMER_EQ_INFO_SIZE);

        eq_len = app_harman_get_eq_data((T_APP_HARMAN_EQ_INFO *)harman_eq_info,
                                        &eq_data);
    }
    else
    {
        eq_len = app_harman_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
                                             NORMAL_MODE, 0, (uint8_t *)&eq_data);
    }

    app_harman_eq_dsp_set(link->eq_instance, (void *)&eq_data, eq_len, SPK_SW_EQ);

    APP_PRINT_INFO0("app_harman_eq_reset_unsaved");
}

void app_harman_eq_info_handle(uint16_t feature_id, uint8_t *p_data, uint16_t data_size)
{
    uint8_t band_info_len = 0;
    T_APP_HARMAN_EQ_INFO eq_info;
    T_APP_HARMAN_EQ_DATA eq_data = {0};
    uint16_t eq_len = 0;
    T_APP_BR_LINK *link = &app_db.br_link[app_a2dp_get_active_idx()];
    uint8_t save_eq = p_data[1];

    memset(&eq_info, 0, sizeof(T_APP_HARMAN_EQ_INFO));
    if (app_cfg_nv.harman_category_id == CATEGORY_ID_OFF) // only dsp cfg EQ
    {
        eq_len = app_harman_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
                                             NORMAL_MODE, 0, (uint8_t *)&eq_data);
        app_harman_eq_dsp_set(link->eq_instance, (void *)&eq_data, eq_len, SPK_SW_EQ);
    }
    else // phone EQ and dsp cfg EQ
    {
        if (data_size >= APP_HARMAN_EQ_INFO_HEADER_SIZE)
        {
            memcpy(&eq_info.header, p_data, APP_HARMAN_EQ_INFO_HEADER_SIZE);

            band_info_len = eq_info.header.band_count * APP_HARMAN_EQ_BAND_INFO_SIZE;

            if (data_size >= (APP_HARMAN_EQ_INFO_HEADER_SIZE + band_info_len))
            {
                p_data += APP_HARMAN_EQ_INFO_HEADER_SIZE;
                memcpy(&eq_info.band_info[0], p_data, band_info_len); // phone eq info sent to soc

                eq_len = app_harman_get_eq_data(&eq_info, &eq_data);

                if (save_eq != SAVE_EQ_FIELD_PREVIEW) // apply
                {
                    app_harman_eq_dsp_set(link->eq_instance, (void *)&eq_data, eq_len, SPK_SW_EQ);
                }

                if (save_eq == SAVE_EQ_FIELD_APPLY_AND_SAVE) // save
                {
                    app_harman_eq_info_save_to_ftl(app_cfg_nv.harman_category_id, (uint8_t *)&eq_info,
                                                   sizeof(eq_info));
                    app_cfg_store(&app_cfg_nv.harman_category_id, 8);
                }
            }
        }
    }
}

void app_harman_set_stream_track_started(T_AUDIO_TRACK_STATE state)
{
    stream_track_started = state;
    APP_PRINT_INFO1("app_harman_set_stream_track_started %d", stream_track_started);
}

static bool app_harman_default_dsp_eq_set(void *p_data, uint16_t len)
{
    uint8_t *p_cmd;
    uint16_t cmd_len = (len + H2D_HEADER_LEN + 3) / 4;

    APP_PRINT_INFO2("app_harman_default_dsp_eq_set: p_data %p, len 0x%04x", p_data, len);

    p_cmd = os_mem_zalloc2(cmd_len * 4);
    if (p_cmd != NULL)
    {
        p_cmd[0] = (uint8_t)H2D_SPK_DEFAULT_EQ;
        p_cmd[1] = (uint8_t)(H2D_SPK_DEFAULT_EQ >> 8);
        p_cmd[2] = cmd_len; //eq_data + dsp_definition
        p_cmd[3] = cmd_len >> 8;

        memcpy(&p_cmd[4], p_data, len);

        if (audio_probe_dsp_send(p_cmd, cmd_len * 4) == false)
        {
            os_mem_free(p_cmd);
            return false;
        }

        os_mem_free(p_cmd);
        return true;
    }

    return false;
}

static void app_harman_dsp_event_cback(uint32_t event, void *msg)
{
    bool handle = true;

    switch (event)
    {
    case AUDIO_PROBE_DSP_MGR_EVT_HARMAN_DEFAULT_EQ:
        {
            T_AUDIO_PROBE_DSP_EVENT_MSG *p_msg = (T_AUDIO_PROBE_DSP_EVENT_MSG *)msg;

            if (p_msg->category == AUDIO_CATEGORY_AUDIO)
            {
                T_APP_HARMAN_EQ_DATA default_dsp_eq = {0};
                uint16_t eq_len = 0;
                int32_t ret = 0;

                eq_len = app_harman_eq_dsp_param_get(SPK_SW_EQ, EQ_STREAM_TYPE_AUDIO,
                                                     NORMAL_MODE, 0, (uint8_t *)&default_dsp_eq);

                ret = app_harman_default_dsp_eq_set((void *)&default_dsp_eq, eq_len);

                APP_PRINT_INFO2("app_harman_dsp_event_cback: category %d, ret %d", p_msg->category, ret);
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_harman_dsp_event_cback: event_type 0x%04x", event);
    }

}

void app_haraman_eq_init(void)
{
    // use default eq when init
    uint8_t eq_info_from_ftl[APP_HARMAN_DEFAULT_EQ_INFO_SIZE] = {0};
    T_APP_HARMAN_DEFAULT_EQ_INFO eq_info_from_dsp;
    uint16_t eq_info_from_dsp_len;

    app_harman_eq_info_load_from_ftl(CATEGORY_ID_OFF, eq_info_from_ftl,
                                     APP_HARMAN_DEFAULT_EQ_INFO_SIZE);

    // APP_PRINT_TRACE1("app_haraman_eq_init: info load from ftl len 0x%x, info (%b)",
    //                  APP_HARMAN_DEFAULT_EQ_INFO_SIZE, TRACE_BINARY(APP_HARMAN_DEFAULT_EQ_INFO_SIZE, eq_info_from_ftl));

    eq_info_from_dsp_len = app_harman_get_default_eq_info(&eq_info_from_dsp);

    if (memcmp((void *)&eq_info_from_ftl, (void *)&eq_info_from_dsp, APP_HARMAN_DEFAULT_EQ_INFO_SIZE))
    {
        APP_PRINT_INFO0("app_haraman_eq_init: info load from dsp");
        app_harman_eq_info_save_to_ftl(CATEGORY_ID_OFF, (uint8_t *)&eq_info_from_dsp,
                                       eq_info_from_dsp_len);
    }
    else
    {
        APP_PRINT_INFO0("app_haraman_eq_init: info load from ftl");
    }

    audio_probe_dsp_evt_cback_register(app_harman_dsp_event_cback);
}
#endif
