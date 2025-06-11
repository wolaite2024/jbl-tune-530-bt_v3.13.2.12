/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "os_mem.h"
#include "eq.h"
#include "btm.h"
#include "trace.h"
#include "app_main.h"
#include "eq_utils.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_eq.h"

#if (F_APP_USER_EQ_SUPPORT == 1)
#include "eq_ext_ftl.h"
#include "storage.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#endif

#define EQ_MSB_IDX      (10)

#define EQ_BIT(_n)     (uint32_t)(1U << (_n))

// #define EQ_MAX_INDEX                9
#define EQ_MIN_INDEX                0

#define EQ_SR_8K                    (0)
#define EQ_SR_16K                   (1)
#define EQ_SR_32K                   (2)
#define EQ_SR_44K                   (3)
#define EQ_SR_48K                   (4)
#define EQ_SR_88K                   (5)
#define EQ_SR_96K                   (6)
#define EQ_SR_192K                  (7)
#define EQ_SR_12K                   (8)
#define EQ_SR_24K                   (9)
#define EQ_SR_11K                   (10)
#define EQ_SR_22K                   (11)
#define EQ_SR_ALL                   (15)

typedef struct t_eq_db
{
    uint8_t normal_eq_map[EQ_MSB_IDX];     /**< index is T_EQ_IDX  spk eq*/
    uint8_t gaming_eq_map[EQ_MSB_IDX];     /**< index is T_EQ_IDX  spk eq*/
    uint8_t anc_eq_map[EQ_MSB_IDX];        /**< index is T_EQ_IDX  spk eq*/
    uint8_t apt_eq_map[EQ_MSB_IDX];        /**< index is T_EQ_IDX  mic eq*/
#if (F_APP_LINEIN_SUPPORT == 1)
    uint8_t line_in_eq_map[EQ_MSB_IDX];    /**< index is T_EQ_IDX  line in eq*/
#endif
#if F_APP_VOICE_SPK_EQ_SUPPORT
    uint8_t voice_spk_eq_map[EQ_MSB_IDX];  /**< index is T_EQ_IDX  voice spk eq*/
#endif
#if F_APP_VOICE_MIC_EQ_SUPPORT
    uint8_t voice_mic_eq_map[EQ_MSB_IDX];  /**< index is T_EQ_IDX  voice mic eq*/
#endif

    uint8_t normal_eq_num;                  /**< the num of eq group */
    uint8_t gaming_eq_num;                  /**< the num of eq group */
    uint8_t anc_eq_num;                  /**< the num of eq group */
    uint8_t apt_eq_num;                     /**< the num of eq group */
#if (F_APP_LINEIN_SUPPORT == 1)
    uint8_t line_in_eq_num;               /**< the num of eq group */
#endif
#if F_APP_VOICE_SPK_EQ_SUPPORT
    uint8_t voice_spk_eq_num;
#endif
#if F_APP_VOICE_MIC_EQ_SUPPORT
    uint8_t voice_mic_eq_num;
#endif
} T_EQ_DB;

static T_EQ_DB *eq_db;

uint8_t eq_utils_stage_num_get(uint16_t info_len, uint32_t sample_rate)
{
    uint16_t stage_num = 0;

    if ((sample_rate == AUDIO_EQ_SAMPLE_RATE_44_1KHZ) || (sample_rate == AUDIO_EQ_SAMPLE_RATE_16KHZ))
    {
        stage_num = ((info_len - (MCU_TO_SDK_CMD_HDR_SIZE + PUBLIC_VALUE_SIZE + EXT_PUB_VALUE_SIZE)) /
                     (STAGE_NUM_SIZE + EXT_STAGE_NUM_SIZE_VER_2));
    }
    else
    {
        stage_num = ((info_len - (MCU_TO_SDK_CMD_HDR_SIZE + PUBLIC_VALUE_SIZE)) / STAGE_NUM_SIZE);
    }

    APP_PRINT_TRACE1("eq_utils_stage_num_get: stage_num %d", stage_num);

    return stage_num;
}

static uint16_t eq_utils_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                                       uint8_t index, void *p_data, uint16_t len, T_EQ_DATA_DEST eq_data_dest, uint32_t sample_rate)
{
    uint32_t eq_offset;
    uint16_t data_len;
    uint8_t  eq_idx;
    uint16_t param_len;
    uint8_t type;
    uint8_t eq_stream_type;
    uint8_t *p_param;
    uint32_t eq_sample_rate = 0;

    APP_PRINT_TRACE5("eq_utils_dsp_param_get: eq_type %d, dsp cfg index %d, p_data %p, len %u, sample_rate %d",
                     eq_type, index, p_data, len, sample_rate);

    if ((index <= EQ_MAX_INDEX) && (p_data != NULL) && (len >= EQ_COEFF_SIZE))
    {
        for (uint8_t i = 0; i < app_dsp_cfg_data->eq_param.header->eq_num; i++)
        {
            param_len = app_dsp_cfg_data->eq_param.header->sub_header[i].param_length;

            if (param_len != 0)
            {
                p_param = calloc(1, param_len);

                if (p_param == NULL)
                {
                    return 0;
                }
                //The 44.1khz or 48khz should be distinguished for each sub_header
                eq_offset = app_dsp_cfg_data->dsp_cfg_header.eq_cmd_block_offset +
                            app_dsp_cfg_data->eq_param.header->sub_header[i].offset;
                eq_sample_rate = app_dsp_cfg_data->eq_param.header->sub_header[i].sample_rate;
                eq_idx = app_dsp_cfg_data->eq_param.header->sub_header[i].eq_idx;
                type = app_dsp_cfg_data->eq_param.header->sub_header[i].eq_type;
                eq_stream_type = app_dsp_cfg_data->eq_param.header->sub_header[i].stream_type;

                app_dsp_cfg_load_param_r_data(p_param, eq_offset, param_len);

                //H2D_AUDIO_EQ_PARA = 0x100A
                if (eq_data_dest == EQ_DATA_TO_PHONE)
                {
                    if (sample_rate == AUDIO_EQ_SAMPLE_RATE_48KHZ)
                    {
                        // for reduce image size, the eq info of 48K is been stored in 44K
                        sample_rate = AUDIO_EQ_SAMPLE_RATE_44_1KHZ;
                    }

                    if (eq_type == type && eq_idx == index && eq_sample_rate == sample_rate &&
                        eq_stream_type == stream_type)
                    {
                        param_len -= 4; //need to return total_len - cmd_id_len - len_len

                        /* only need extended eq info when sending to src */
                        uint16_t eq_len_to_dsp = MCU_TO_SDK_CMD_HDR_SIZE + PUBLIC_VALUE_SIZE +
                                                 eq_utils_stage_num_get(param_len, eq_sample_rate) * STAGE_NUM_SIZE;

                        param_len -= eq_len_to_dsp;

                        data_len = (len < param_len) ? len : param_len;

                        memcpy(p_data, &p_param[4 + eq_len_to_dsp], data_len);

                        free(p_param);

                        return data_len;
                    }
                }
                else if (eq_data_dest == EQ_DATA_TO_DSP)
                {
                    if ((eq_type == type && eq_idx == index && eq_stream_type == stream_type) &&
                        (sample_rate == 0 || eq_sample_rate == sample_rate))
                    {
                        param_len -= 4;

                        /* did not need extended eq info when sending to dsp */
                        data_len = (MCU_TO_SDK_CMD_HDR_SIZE + PUBLIC_VALUE_SIZE) +
                                   eq_utils_stage_num_get(param_len, eq_sample_rate) * STAGE_NUM_SIZE;

                        data_len = (len < data_len) ? len : data_len;
#if F_APP_HARMAN_FEATURE_SUPPORT
                        if (app_cfg_nv.harman_category_id == 0x00)
                        {
                            //default eq
                            memcpy(p_data, &p_param[4], data_len);
                            APP_PRINT_TRACE1("[SD_CHECK]CMD_THIRD_PARTY_EQ eq_off1 p_data = %b", TRACE_BINARY(data_len,
                                             p_data));
                        }
                        else
                        {
                            uint8_t set_harman_eq_data[408] = {0};
                            static const uint8_t set_eq_data_compare[408] = {0};
                            uint8_t load_check = false;

                            if (eq_sample_rate == AUDIO_EQ_SAMPLE_RATE_44_1KHZ)
                            {
//                                load_check = ftl_load_from_storage(&set_harman_eq_data[0], APP_HARMAN_EQ_DATA_44,
//                                                                   APP_HARMAN_EQ_DATA_SIZE_44);
                                //APP_PRINT_TRACE1("[SD_CHECK]CMD_THIRD_PARTY_EQ 44_1KHZ eq = %b",
                                //                 TRACE_BINARY(408, set_harman_eq_data));
                            }
                            else if (eq_sample_rate == AUDIO_EQ_SAMPLE_RATE_48KHZ)
                            {
//                                load_check = ftl_load_from_storage(&set_harman_eq_data[0], APP_HARMAN_EQ_DATA_48,
//                                                                   APP_HARMAN_EQ_DATA_SIZE_48);
                                //APP_PRINT_TRACE1("[SD_CHECK]CMD_THIRD_PARTY_EQ 48KHZ eq = %b",
                                //                 TRACE_BINARY(408, set_harman_eq_data));
                            }
                            APP_PRINT_TRACE2("[SD_CHECK]CMD_THIRD_PARTY_EQ refference sample_rate=%d, load_check =%d",
                                             eq_sample_rate,
                                             load_check);
                            if ((app_cfg_nv.harman_category_id != 0x00) &&
                                (memcmp(&set_harman_eq_data[0], &set_eq_data_compare[0], 408) == 0))
                            {
                                //default eq
                                memcpy(p_data, &p_param[4], data_len);
                                //APP_PRINT_TRACE1("[SD_CHECK]CMD_THIRD_PARTY_EQ eq_off2 p_data = %b", TRACE_BINARY(data_len,
                                //                 p_data));
                            }
                            else
                            {
                                data_len = ((app_cfg_nv.harman_customer_stage * 20) + 8);
                                memcpy(p_data, &set_harman_eq_data[0], data_len);
                                //APP_PRINT_INFO2("[SD_CHECK]CMD_THIRD_PARTY_EQ default_eq_data refference data len %d, %b",
                                //                ((app_cfg_nv.harman_customer_stage * 20) + 8),
                                //                TRACE_BINARY(data_len, p_data));
                            }
                        }
#else
                        memcpy(p_data, &p_param[4], data_len);
#endif

                        free(p_param);

                        return data_len;
                    }
                }
                free(p_param);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        APP_PRINT_ERROR1("eq_utils_dsp_param_get: error idx %d", index);
    }

    return 0;
}

#if (F_APP_USER_EQ_SUPPORT == 1)
uint32_t eq_utils_save_eq_to_ftl(uint32_t offset, uint8_t *data, uint32_t len)
{
    return storage_write(EQ_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}

uint32_t eq_utils_load_eq_from_ftl(uint32_t offset, uint8_t *data, uint32_t len)
{
    return storage_read(EQ_EXT_FTL_PARTITION_NAME, offset, len, data, NULL, NULL);
}
#endif

uint8_t eq_utils_num_get(T_EQ_TYPE eq_type, uint8_t mode)
{
    if (eq_db != NULL)
    {
        uint8_t map[EQ_TYPE_MAX][SPK_EQ_MODE_MAX] =
        {
            {eq_db->normal_eq_num, eq_db->gaming_eq_num, eq_db->anc_eq_num, 0, 0},
            {eq_db->apt_eq_num, 0, 0, 0, 0}
        };

#if (F_APP_LINEIN_SUPPORT == 1)
        map[SPK_SW_EQ][LINE_IN_MODE] = eq_db->line_in_eq_num;
#endif

#if F_APP_VOICE_SPK_EQ_SUPPORT
        map[SPK_SW_EQ][VOICE_SPK_MODE] = eq_db->voice_spk_eq_num;
#endif

#if F_APP_VOICE_MIC_EQ_SUPPORT
        map[MIC_SW_EQ][VOICE_MIC_MODE] = eq_db->voice_mic_eq_num;
#endif

        return map[eq_type][mode];
    }

    return 0;
}

uint8_t *eq_utils_map_get(T_EQ_TYPE eq_type, uint8_t mode)
{
    if (eq_db != NULL)
    {
        uint8_t *map[EQ_TYPE_MAX][SPK_EQ_MODE_MAX] =
        {
            {eq_db->normal_eq_map, eq_db->gaming_eq_map, eq_db->anc_eq_map, NULL, NULL},
            {eq_db->apt_eq_map, NULL, NULL, NULL, NULL}
        };

#if (F_APP_LINEIN_SUPPORT == 1)
        map[SPK_SW_EQ][LINE_IN_MODE] = eq_db->line_in_eq_map;
#endif

#if F_APP_VOICE_SPK_EQ_SUPPORT
        map[SPK_SW_EQ][VOICE_SPK_MODE] = eq_db->voice_spk_eq_map;
#endif

#if F_APP_VOICE_MIC_EQ_SUPPORT
        map[MIC_SW_EQ][VOICE_MIC_MODE] = eq_db->voice_mic_eq_map;
#endif

        return map[eq_type][mode];
    }
    return NULL;
}

uint16_t eq_utils_param_len_get(void)
{
    return EQ_PARAM_SIZE;
}

uint8_t eq_utils_original_eq_index_get(T_EQ_TYPE eq_type, uint8_t eq_mode, uint8_t index)
{
    uint8_t eq_index = index;

    if ((eq_type != EQ_TYPE_MAX) && (eq_mode != EQ_MODE_NULL))
    {
        uint8_t *map = eq_utils_map_get(eq_type, eq_mode);

        if (map)
        {
            eq_index = map[index];
        }
    }

    return eq_index;
}

uint16_t eq_utils_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type, uint8_t eq_mode,
                            uint8_t index, void *data, uint16_t len, T_EQ_DATA_DEST eq_data_dest, uint32_t sample_rate)
{
    uint8_t *buf;
    uint8_t *eq_map;
    uint16_t data_len = 0;
    uint8_t eq_index = eq_utils_original_eq_index_get(eq_type, eq_mode, index);

    APP_PRINT_INFO8("eq_utils_param_get: eq_type %d, stream_type %d, eq_mode %d, index %u, data %p, len 0x%04x, destin, %d, sample_rate , %d",
                    eq_type, stream_type, eq_mode, index, data, len, eq_data_dest, sample_rate);

    buf = data;

    if ((buf != NULL) && (len >= EQ_PARAM_SIZE))
    {
        if (eq_index <= EQ_MAX_INDEX)
        {
            data_len = eq_utils_dsp_param_get(eq_type, stream_type, eq_index, buf, len, eq_data_dest,
                                              sample_rate);
        }
    }

    return data_len;
}

static bool eq_utils_get_spk_capacity(uint8_t mode, uint32_t *bit_map)
{
    if (app_dsp_cfg_data->eq_param.header->sync_word == APP_DATA_SYNC_WORD)
    {
#if F_APP_VOICE_SPK_EQ_SUPPORT
        if (mode == VOICE_SPK_MODE)
        {
            if (app_dsp_cfg_data->eq_param.header->voice_eq_applications_num != 0xff)
            {
                *bit_map =
                    app_dsp_cfg_data->eq_param.header->voice_eq_application_header[0].spk_eq_idx_bitmask;
            }
            else
            {
                APP_PRINT_INFO0("eq_utils_get_spk_capacity: not support voice eq!");
                return false;
            }
        }
        else
#endif
        {
            *bit_map =
                app_dsp_cfg_data->eq_param.header->eq_spk_application_header[mode].spk_eq_idx_bitmask;
        }

        APP_PRINT_INFO2("eq_utils_get_spk_capacity: mode %d, bit map 0x%x ", mode, *bit_map);

        return true;
    }

    return false;
}

static bool eq_utils_get_mic_capacity(uint8_t mode, uint32_t *bit_map)
{
    if (app_dsp_cfg_data->eq_param.header->sync_word == APP_DATA_SYNC_WORD)
    {
#if F_APP_VOICE_MIC_EQ_SUPPORT
        if (mode == VOICE_MIC_MODE)
        {
            if (app_dsp_cfg_data->eq_param.header->voice_eq_applications_num != 0xff)
            {
                *bit_map =
                    app_dsp_cfg_data->eq_param.header->voice_eq_application_header[0].spk_eq_idx_bitmask;
            }
            else
            {
                APP_PRINT_INFO0("eq_utils_get_spk_capacity: not support voice mic eq!");
                return false;
            }
        }
        else
#endif
        {
            *bit_map =
                app_dsp_cfg_data->eq_param.header->eq_mic_application_header[mode].mic_eq_idx_bitmask;
        }

        APP_PRINT_INFO2("eq_utils_get_mic_capacity: mode %d, bit map 0x%x", mode, *bit_map);

        return true;
    }

    return false;
}

bool eq_utils_get_capacity(T_EQ_TYPE type, uint8_t mode, uint32_t *bit_map)
{
    if (type == SPK_SW_EQ)
    {
        if (eq_utils_get_spk_capacity(mode, bit_map) == false)
        {
            return false;
        }
    }
    else if (type == MIC_SW_EQ)
    {
        if (eq_utils_get_mic_capacity(mode, bit_map) == false)
        {
            return false;
        }
    }

    return true;
}

static uint8_t eq_utils_map_init(T_EQ_TYPE eq_type, uint8_t mode)
{
    uint32_t eq_capacity = 0;
    uint8_t i ;
    uint8_t eq_num = 0;
    uint8_t *map = eq_utils_map_get(eq_type, mode);

    if (eq_utils_get_capacity(eq_type, mode, &eq_capacity) == false || (map == NULL))
    {
        APP_PRINT_ERROR4("eq_utils_map_init: init fail, type %d, mode %d, capacity %d, map %p",
                         eq_type, mode, eq_capacity, map);
        return 0;
    }

    for (i = 0; i < EQ_GROUP_NUM; ++i)
    {
        if (eq_capacity & EQ_BIT(i))
        {
            map[eq_num] = i;
            eq_num++;
        }
    }

    return eq_num;
}

uint8_t eq_utils_get_default_idx(T_EQ_TYPE eq_type, uint8_t mode)
{
    if (eq_type == SPK_SW_EQ)
    {
        return app_dsp_cfg_data->eq_param.header->eq_spk_application_header[mode].app_default_spk_eq_idx;
    }
    else
    {
        return app_dsp_cfg_data->eq_param.header->eq_mic_application_header[mode].app_default_mic_eq_idx;
    }
}

bool eq_utils_init(void)
{
    eq_db = calloc(1, sizeof(T_EQ_DB));

    if (eq_db == NULL)
    {
        APP_PRINT_ERROR0("eq_utils_init: eq init fail, calloc fail");
        return false;
    }

#if (F_APP_USER_EQ_SUPPORT == 1)
    eq_ext_ftl_storage_init();
#endif

    eq_db->normal_eq_num = eq_utils_map_init(SPK_SW_EQ, NORMAL_MODE);
    eq_db->gaming_eq_num = eq_utils_map_init(SPK_SW_EQ, GAMING_MODE);

#if F_APP_ANC_SUPPORT
    eq_db->anc_eq_num = eq_utils_map_init(SPK_SW_EQ, ANC_MODE);
#endif

#if F_APP_APT_SUPPORT
    eq_db->apt_eq_num = eq_utils_map_init(MIC_SW_EQ, APT_MODE);
#endif

#if (F_APP_LINEIN_SUPPORT == 1)
    eq_db->line_in_eq_num = eq_utils_map_init(SPK_SW_EQ, LINE_IN_MODE);
#endif

#if F_APP_VOICE_SPK_EQ_SUPPORT
    eq_db->voice_spk_eq_num = eq_utils_map_init(SPK_SW_EQ, VOICE_SPK_MODE);
#endif

    return true;
}

bool eq_utils_deinit(void)
{
    if (eq_db != NULL)
    {
        free(eq_db);
    }

    return true;
}
