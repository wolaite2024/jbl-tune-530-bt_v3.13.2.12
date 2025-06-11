#include <string.h>
#include "trace.h"
#include "ascs_def.h"
#include "ascs_mgr.h"
#include "metadata_def.h"
#include "pacs_mgr.h"
#include "os_mem.h"
#include "codec_qos.h"
#include "app_link_util.h"
#include "app_cfg.h"
#include "app_lea_pacs.h"

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
#define SINK_PAC_CNT 1
#define SOURCE_PAC_CNT 1

#if F_APP_LC3_PLUS_SUPPORT
#define LC3_PLUS_SINK_PAC_CNT   1
#define LC3_PLUS_SOURCE_PAC_CNT 1

#if F_APP_FRAUNHOFER_SUPPORT
#define FRAUNHOFER_FD_ENABLE (FRAUNHOFER_FD_10_MS_BIT | FRAUNHOFER_FD_7_5_MS_BIT | FRAUNHOFER_FD_5_MS_BIT | FRAUNHOFER_FD_2_5_MS_BIT)

typedef enum
{
    LC3_PLUS_CODEC_CFG_ITEM_48_1 = 0,
    LC3_PLUS_CODEC_CFG_ITEM_48_2 = 1,
    LC3_PLUS_CODEC_CFG_ITEM_48_3 = 2,
    LC3_PLUS_CODEC_CFG_ITEM_48_4 = 3,
    LC3_PLUS_CODEC_CFG_ITEM_48_5 = 4,
    LC3_PLUS_CODEC_CFG_ITEM_48_6 = 5,
    LC3_PLUS_CODEC_CFG_ITEM_96_1 = 6,
    LC3_PLUS_CODEC_CFG_ITEM_96_2 = 7,
    LC3_PLUS_CODEC_CFG_ITEM_96_3 = 8,
    LC3_PLUS_CODEC_CFG_ITEM_96_4 = 9,
    LC3_PLUS_CODEC_CFG_ITEM_96_5 = 10,
    LC3_PLUS_CODEC_CFG_ITEM_96_6 = 11,
    LC3_PLUS_CODEC_CFG_ITEM_LC3_MAX,
} T_LC3_PLUS_CODEC_CFG_ITEM;

const T_CODEC_CFG lc3_plus_codec_cfg[] =
{
    {0x1B, FRAUNHOFER_CFG_FD_10_MS,  SAMPLING_FREQUENCY_CFG_48K, 1, 160, AUDIO_LOCATION_MONO},// bitrate 128kbps
    {0x1B, FRAUNHOFER_CFG_FD_10_MS,  SAMPLING_FREQUENCY_CFG_48K, 1, 310, AUDIO_LOCATION_MONO},// bitrate 248kbps
    {0x1B, FRAUNHOFER_CFG_FD_7_5_MS, SAMPLING_FREQUENCY_CFG_48K, 1, 117, AUDIO_LOCATION_MONO},// bitrate 124.8kbps
    {0x1B, FRAUNHOFER_CFG_FD_7_5_MS, SAMPLING_FREQUENCY_CFG_48K, 1, 180, AUDIO_LOCATION_MONO},// bitrate 192kbps
    {0x1B, FRAUNHOFER_CFG_FD_5_MS,   SAMPLING_FREQUENCY_CFG_48K, 1, 120, AUDIO_LOCATION_MONO},// bitrate 192kbps
    {0x1B, FRAUNHOFER_CFG_FD_2_5_MS, SAMPLING_FREQUENCY_CFG_48K, 1, 60,  AUDIO_LOCATION_MONO},// bitrate 192kbps
    {0x1B, FRAUNHOFER_CFG_FD_10_MS,  SAMPLING_FREQUENCY_CFG_96K, 1, 190, AUDIO_LOCATION_MONO},// bitrate 152kbps
    {0x1B, FRAUNHOFER_CFG_FD_10_MS,  SAMPLING_FREQUENCY_CFG_96K, 1, 310, AUDIO_LOCATION_MONO},// bitrate 248kbps
    {0x1B, FRAUNHOFER_CFG_FD_7_5_MS, SAMPLING_FREQUENCY_CFG_96K, 1, 141, AUDIO_LOCATION_MONO},// bitrate 150.4kbps
    {0x1B, FRAUNHOFER_CFG_FD_7_5_MS, SAMPLING_FREQUENCY_CFG_96K, 1, 225, AUDIO_LOCATION_MONO},// bitrate 240kbps
    {0x1B, FRAUNHOFER_CFG_FD_5_MS,   SAMPLING_FREQUENCY_CFG_96K, 1, 120, AUDIO_LOCATION_MONO},// bitrate 192kbps
    {0x1B, FRAUNHOFER_CFG_FD_2_5_MS, SAMPLING_FREQUENCY_CFG_96K, 1, 65,  AUDIO_LOCATION_MONO},// bitrate 208kbps
};

const T_QOS_CFG_PREFERRED lc3_plus_cis_low_latency_qos[] =
{
    {10000, AUDIO_UNFRAMED, 160, 2, 10, 40000},//LC3 plus CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 310, 2, 10, 40000},//LC3 plus CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 117, 2, 8,  40000},//LC3 plus CODEC_CFG_ITEM_48_3
    {7500,  AUDIO_UNFRAMED, 180, 2, 8,  40000},//LC3 plus CODEC_CFG_ITEM_48_4
    {5000,  AUDIO_UNFRAMED, 120, 2, 5,  40000},//LC3 plus CODEC_CFG_ITEM_48_5
    {2500,  AUDIO_UNFRAMED, 60,  2, 3,  40000},//LC3 plus CODEC_CFG_ITEM_48_6
    {10000, AUDIO_UNFRAMED, 190, 2, 10, 40000},//LC3 plus CODEC_CFG_ITEM_96_1
    {10000, AUDIO_UNFRAMED, 310, 2, 10, 40000},//LC3 plus CODEC_CFG_ITEM_96_2
    {7500,  AUDIO_UNFRAMED, 141, 2, 8,  40000},//LC3 plus CODEC_CFG_ITEM_96_3
    {7500,  AUDIO_UNFRAMED, 225, 2, 8,  40000},//LC3 plus CODEC_CFG_ITEM_96_4
    {5000,  AUDIO_UNFRAMED, 120, 2, 5,  40000},//LC3 plus CODEC_CFG_ITEM_96_5
    {2500,  AUDIO_UNFRAMED, 65,  2, 3,  40000},//LC3 plus CODEC_CFG_ITEM_96_6
};

const T_QOS_CFG_PREFERRED lc3_plus_cis_high_reliability_qos[] =
{
    {10000, AUDIO_UNFRAMED, 160, 13, 100, 40000},//LC3 plus CODEC_CFG_ITEM_48_1
    {10000, AUDIO_UNFRAMED, 310, 13, 100, 40000},//LC3 plus CODEC_CFG_ITEM_48_2
    {7500,  AUDIO_UNFRAMED, 117, 13, 75,  40000},//LC3 plus CODEC_CFG_ITEM_48_3
    {7500,  AUDIO_UNFRAMED, 180, 13, 75,  40000},//LC3 plus CODEC_CFG_ITEM_48_4
    {5000,  AUDIO_UNFRAMED, 120, 13, 50,  40000},//LC3 plus CODEC_CFG_ITEM_48_5
    {2500,  AUDIO_UNFRAMED, 60,  13, 25,  40000},//LC3 plus CODEC_CFG_ITEM_48_6
    {10000, AUDIO_UNFRAMED, 190, 13, 100, 40000},//LC3 plus CODEC_CFG_ITEM_96_1
    {10000, AUDIO_UNFRAMED, 310, 13, 100, 40000},//LC3 plus CODEC_CFG_ITEM_96_2
    {7500,  AUDIO_UNFRAMED, 141, 13, 75,  40000},//LC3 plus CODEC_CFG_ITEM_96_3
    {7500,  AUDIO_UNFRAMED, 225, 13, 75,  40000},//LC3 plus CODEC_CFG_ITEM_96_4
    {5000,  AUDIO_UNFRAMED, 120, 13, 50,  40000},//LC3 plus CODEC_CFG_ITEM_96_5
    {2500,  AUDIO_UNFRAMED, 65,  13, 25,  40000},//LC3 plus CODEC_CFG_ITEM_96_6
};

static const uint16_t app_lea_pac_lc3_plus_codec_item_sr_map[LC3_PLUS_CODEC_CFG_ITEM_LC3_MAX] =
{
    SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K,
    SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K,
};

const static T_LC3_PLUS_CODEC_CFG_ITEM app_lea_pac_lc3_plus_media_codec[] =
{
    LC3_PLUS_CODEC_CFG_ITEM_48_1,
    LC3_PLUS_CODEC_CFG_ITEM_48_2,
    LC3_PLUS_CODEC_CFG_ITEM_48_3,
    LC3_PLUS_CODEC_CFG_ITEM_48_4,
    LC3_PLUS_CODEC_CFG_ITEM_48_5,
    LC3_PLUS_CODEC_CFG_ITEM_48_6,
    LC3_PLUS_CODEC_CFG_ITEM_96_1,
    LC3_PLUS_CODEC_CFG_ITEM_96_2,
    LC3_PLUS_CODEC_CFG_ITEM_96_3,
    LC3_PLUS_CODEC_CFG_ITEM_96_4,
    LC3_PLUS_CODEC_CFG_ITEM_96_5,
    LC3_PLUS_CODEC_CFG_ITEM_96_6,
};

const static T_LC3_PLUS_CODEC_CFG_ITEM app_lea_pac_lc3_plus_convo_codec[] =
{
    LC3_PLUS_CODEC_CFG_ITEM_48_1,
    LC3_PLUS_CODEC_CFG_ITEM_48_2,
    LC3_PLUS_CODEC_CFG_ITEM_48_3,
    LC3_PLUS_CODEC_CFG_ITEM_48_4,
    LC3_PLUS_CODEC_CFG_ITEM_48_5,
    LC3_PLUS_CODEC_CFG_ITEM_48_6,
};

typedef struct
{
    uint8_t hr_frame_len;
    uint8_t hr_frame[5];
} T_LEA_HR_FRAME;

typedef struct
{
    uint8_t frame_duration_bit;
    uint8_t frame_duration;
} T_LEA_HR_FD;

typedef struct
{
    uint8_t metadata_len;
    uint8_t metadata[4];
} T_LEA_HR_META_DATA;

typedef struct
{
    uint8_t codec_id[5];
    uint8_t capabilities_len;
    uint8_t sample_rate_len;
    uint8_t sample_rate[3];
    uint8_t channel_cnt_len;
    uint8_t channel_cnt[2];
    uint8_t frames_per_sdu_len;
    uint8_t frames_per_sdu[2];
    uint8_t hr_duration_len;
    uint8_t hr_duration[3];
} T_LEA_HR_PAC_RECORD;

static const T_LEA_HR_FD app_lea_pac_lc3_plus_frame_duration_map[] =
{
    {FRAUNHOFER_CFG_FD_10_MS,  FRAUNHOFER_SUPPORTED_OCTETS_10_MS},
    {FRAUNHOFER_CFG_FD_7_5_MS, FRAUNHOFER_SUPPORTED_OCTETS_7_5_MS},
    {FRAUNHOFER_CFG_FD_5_MS,   FRAUNHOFER_SUPPORTED_OCTETS_5_MS},
    {FRAUNHOFER_CFG_FD_2_5_MS, FRAUNHOFER_SUPPORTED_OCTETS_2_5_MS},
};
#else
typedef enum
{
    LC3_PLUS_CODEC_CFG_ITEM_32_1 = 0,
    LC3_PLUS_CODEC_CFG_ITEM_32_2 = 1,
    LC3_PLUS_CODEC_CFG_ITEM_32_3 = 2,
    LC3_PLUS_CODEC_CFG_ITEM_48_1 = 3,
    LC3_PLUS_CODEC_CFG_ITEM_48_2 = 4,
    LC3_PLUS_CODEC_CFG_ITEM_48_3 = 5,
    LC3_PLUS_CODEC_CFG_ITEM_96_1 = 6,
    LC3_PLUS_CODEC_CFG_ITEM_96_2 = 7,
    LC3_PLUS_CODEC_CFG_ITEM_96_3 = 8,
    LC3_PLUS_CODEC_CFG_ITEM_LC3_MAX,
} T_LC3_PLUS_CODEC_CFG_ITEM;

const T_CODEC_CFG lc3_plus_codec_cfg[] =
{
    {0x1B, FRAME_DURATION_CFG_2_5_MS, SAMPLING_FREQUENCY_CFG_32K, 1, 20,  AUDIO_LOCATION_MONO},// bitrate 64kbps
    {0x1B, FRAME_DURATION_CFG_5_MS,   SAMPLING_FREQUENCY_CFG_32K, 1, 40,  AUDIO_LOCATION_MONO},// bitrate 64kbps
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_32K, 1, 80,  AUDIO_LOCATION_MONO},// bitrate 64kbps
    {0x1B, FRAME_DURATION_CFG_2_5_MS, SAMPLING_FREQUENCY_CFG_48K, 1, 25,  AUDIO_LOCATION_MONO},// bitrate 80kbps
    {0x1B, FRAME_DURATION_CFG_5_MS,   SAMPLING_FREQUENCY_CFG_48K, 1, 50,  AUDIO_LOCATION_MONO},// bitrate 80kbps
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_48K, 1, 100, AUDIO_LOCATION_MONO},// bitrate 80kbps
    {0x1B, FRAME_DURATION_CFG_2_5_MS, SAMPLING_FREQUENCY_CFG_96K, 1, 40,  AUDIO_LOCATION_MONO},// bitrate 128kbps
    {0x1B, FRAME_DURATION_CFG_5_MS,   SAMPLING_FREQUENCY_CFG_96K, 1, 80,  AUDIO_LOCATION_MONO},// bitrate 128kbps
    {0x1B, FRAME_DURATION_CFG_10_MS,  SAMPLING_FREQUENCY_CFG_96K, 1, 160, AUDIO_LOCATION_MONO},// bitrate 128kbps
};

const T_QOS_CFG_PREFERRED lc3_plus_cis_low_latency_qos[] =
{
    {2500,  AUDIO_UNFRAMED, 20,  2, 8, 40000},//LC3 plus CODEC_CFG_ITEM_32_1
    {5000,  AUDIO_UNFRAMED, 40,  2, 10, 40000},//LC3 plus CODEC_CFG_ITEM_32_2
    {10000, AUDIO_UNFRAMED, 80,  2, 15, 40000},//LC3 plus CODEC_CFG_ITEM_32_3
    {2500,  AUDIO_UNFRAMED, 25,  5, 15, 40000},//LC3 plus CODEC_CFG_ITEM_48_1
    {5000,  AUDIO_UNFRAMED, 50,  5, 20, 40000},//LC3 plus CODEC_CFG_ITEM_48_2
    {10000, AUDIO_UNFRAMED, 100, 5, 25, 40000},//LC3 plus CODEC_CFG_ITEM_48_3
    {2500,  AUDIO_UNFRAMED, 40,  5, 15, 40000},//LC3 plus CODEC_CFG_ITEM_96_1
    {5000,  AUDIO_UNFRAMED, 80,  5, 20, 40000},//LC3 plus CODEC_CFG_ITEM_96_2
    {10000, AUDIO_UNFRAMED, 160, 5, 25, 40000},//LC3 plus CODEC_CFG_ITEM_96_3
};

const T_QOS_CFG_PREFERRED lc3_plus_cis_high_reliability_qos[] =
{
    {2500,  AUDIO_UNFRAMED, 20,  13, 75, 40000},//LC3 plus CODEC_CFG_ITEM_32_1
    {5000,  AUDIO_UNFRAMED, 40,  13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_32_2
    {10000, AUDIO_UNFRAMED, 80,  13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_32_3
    {2500,  AUDIO_UNFRAMED, 25,  13, 75, 40000},//LC3 plus CODEC_CFG_ITEM_48_1
    {5000,  AUDIO_UNFRAMED, 50,  13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_48_2
    {10000, AUDIO_UNFRAMED, 100, 13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_48_3
    {2500,  AUDIO_UNFRAMED, 40,  13, 75, 40000},//LC3 plus CODEC_CFG_ITEM_96_1
    {5000,  AUDIO_UNFRAMED, 80,  13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_96_2
    {10000, AUDIO_UNFRAMED, 160, 13, 95, 40000},//LC3 plus CODEC_CFG_ITEM_96_3
};

static const uint16_t app_lea_pac_lc3_plus_codec_item_sr_map[LC3_PLUS_CODEC_CFG_ITEM_LC3_MAX] =
{
    SAMPLING_FREQUENCY_32K, SAMPLING_FREQUENCY_32K, SAMPLING_FREQUENCY_32K,
    SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K,
    SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K, SAMPLING_FREQUENCY_96K,
};

const static T_LC3_PLUS_CODEC_CFG_ITEM app_lea_pac_lc3_plus_media_codec[] =
{
    LC3_PLUS_CODEC_CFG_ITEM_32_1,
    LC3_PLUS_CODEC_CFG_ITEM_32_2,
    LC3_PLUS_CODEC_CFG_ITEM_32_3,
    LC3_PLUS_CODEC_CFG_ITEM_48_1,
    LC3_PLUS_CODEC_CFG_ITEM_48_2,
    LC3_PLUS_CODEC_CFG_ITEM_48_3,
    LC3_PLUS_CODEC_CFG_ITEM_96_1,
    LC3_PLUS_CODEC_CFG_ITEM_96_2,
    LC3_PLUS_CODEC_CFG_ITEM_96_3,
};

const static T_LC3_PLUS_CODEC_CFG_ITEM app_lea_pac_lc3_plus_convo_codec[] =
{
    LC3_PLUS_CODEC_CFG_ITEM_32_1,
    LC3_PLUS_CODEC_CFG_ITEM_32_2,
    LC3_PLUS_CODEC_CFG_ITEM_32_3,
    LC3_PLUS_CODEC_CFG_ITEM_48_1,
    LC3_PLUS_CODEC_CFG_ITEM_48_2,
    LC3_PLUS_CODEC_CFG_ITEM_48_3,
};
#endif
#endif

typedef struct
{
    uint8_t codec_id[5];
    uint8_t capabilities_len;
    uint8_t sample_rate_len;
    uint8_t sample_rate[3];
    uint8_t duration_len;
    uint8_t duration[2];
    uint8_t channel_cnt_len;
    uint8_t channel_cnt[2];
    uint8_t frame_len;
    uint8_t frame[5];
    uint8_t frames_per_sdu_len;
    uint8_t frames_per_sdu[2];
    uint8_t metadata_len;
    uint8_t metadata[4];
} T_LEA_PAC_RECORD;

typedef struct
{
    uint16_t frame_len;
    bool update;
} T_LEA_PAC_FRAME_LEN;

typedef struct
{
    uint16_t sample_rate;
    T_LEA_PAC_FRAME_LEN frame_len_min;
    T_LEA_PAC_FRAME_LEN frame_len_max;
} T_LEA_PAC_FRAME_LEN_INFO;

typedef struct
{
    uint8_t  codec_id;
    uint16_t company_id;
    uint16_t vendor_codec_id;
    uint16_t sample_rate;
    uint8_t  frame_duration;
    uint8_t  channel_cnt;
    uint16_t frame_len_min;
    uint16_t frame_len_max;
    uint8_t  frames_per_sdu;
    uint16_t audio_context;
} T_LEA_PAC_INFO;

static const uint16_t app_lea_pac_codec_item_sr_map[CODEC_CFG_ITEM_LC3_MAX] =
{
    SAMPLING_FREQUENCY_8K, SAMPLING_FREQUENCY_8K,
    SAMPLING_FREQUENCY_16K, SAMPLING_FREQUENCY_16K,
    SAMPLING_FREQUENCY_24K, SAMPLING_FREQUENCY_24K,
    SAMPLING_FREQUENCY_32K, SAMPLING_FREQUENCY_32K,
    SAMPLING_FREQUENCY_44_1K, SAMPLING_FREQUENCY_44_1K,
    SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K,
    SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K,
    SAMPLING_FREQUENCY_48K, SAMPLING_FREQUENCY_48K
};

const static T_CODEC_CFG_ITEM app_lea_pac_sink_codec[] =
{
    CODEC_CFG_ITEM_16_1,
    CODEC_CFG_ITEM_16_2,
    CODEC_CFG_ITEM_24_1,
    CODEC_CFG_ITEM_24_2,
    CODEC_CFG_ITEM_32_1,
    CODEC_CFG_ITEM_32_2,
    CODEC_CFG_ITEM_441_1,
    CODEC_CFG_ITEM_441_2,
    CODEC_CFG_ITEM_48_1,
    CODEC_CFG_ITEM_48_4,
    CODEC_CFG_ITEM_48_6,
};

const static T_CODEC_CFG_ITEM app_lea_pac_src_codec[] =
{
    CODEC_CFG_ITEM_16_1,
    CODEC_CFG_ITEM_16_2,
    CODEC_CFG_ITEM_24_1,
    CODEC_CFG_ITEM_24_2,
    CODEC_CFG_ITEM_32_1,
    CODEC_CFG_ITEM_32_2,
};

static uint16_t app_lea_pac_sink_available_contexts = AUDIO_CONTEXT_MASK;

static uint16_t app_lea_pac_source_available_contexts = AUDIO_CONTEXT_MASK;

static uint16_t app_lea_pac_sink_supported_contexts = AUDIO_CONTEXT_MASK;

static uint16_t app_lea_pac_source_supported_contexts = AUDIO_CONTEXT_MASK;

static uint8_t app_lea_pac_sink_record_size = 0;
static uint8_t app_lea_pac_source_record_size = 0;
static uint8_t *p_app_lea_pac_sink_record = NULL;
static uint8_t *p_app_lea_pac_source_record = NULL;

#if F_APP_FRAUNHOFER_SUPPORT
static void app_lea_pacs_lc3_plus_get_frame_len(T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info,
                                                uint8_t frame_duration);
#endif

static uint16_t app_lea_pacs_ble_audio_cback(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    bool handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND:
        {
            T_PACS_READ_AVAILABLE_CONTEXTS_IND *p_data = (T_PACS_READ_AVAILABLE_CONTEXTS_IND *)buf;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_data->conn_handle);

            if (p_link)
            {
                pacs_available_contexts_read_cfm(p_data->conn_handle, p_data->cid, p_link->sink_available_contexts,
                                                 p_link->source_available_contexts);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_pacs_ble_audio_cback: msg 0x%04X", msg);
    }

    return cb_result;
}

static void app_lea_pacs_update_frame_len(T_CODEC_CFG_ITEM item,
                                          T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info)
{
    T_CODEC_CFG preferred_cfg = {0};

    codec_preferred_cfg_get(item, &preferred_cfg);

    if (preferred_cfg.octets_per_codec_frame >= p_frame_len_info->frame_len_max.frame_len)
    {
        if (p_frame_len_info->frame_len_max.update)
        {
            if (p_frame_len_info->frame_len_min.update)
            {
                if (p_frame_len_info->frame_len_min.frame_len < preferred_cfg.octets_per_codec_frame)
                {
                    p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
                }
            }
            else
            {
                p_frame_len_info->frame_len_min.frame_len = p_frame_len_info->frame_len_max.frame_len;
                p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
                p_frame_len_info->frame_len_min.update = true;
            }
        }
        else
        {
            p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
            p_frame_len_info->frame_len_max.update = true;
        }
    }
    else
    {
        if (p_frame_len_info->frame_len_min.update)
        {
            if (p_frame_len_info->frame_len_max.update)
            {
                if (preferred_cfg.octets_per_codec_frame < p_frame_len_info->frame_len_min.frame_len)
                {
                    p_frame_len_info->frame_len_min.frame_len = preferred_cfg.octets_per_codec_frame;
                }
            }
            else
            {
                p_frame_len_info->frame_len_max.frame_len = p_frame_len_info->frame_len_min.frame_len;
                p_frame_len_info->frame_len_max.update = true;
            }
        }
        else
        {
            p_frame_len_info->frame_len_min.frame_len = preferred_cfg.octets_per_codec_frame;
            p_frame_len_info->frame_len_min.update = true;
        }
    }
}

static bool app_lea_pacs_frame_len_setting(const T_CODEC_CFG_ITEM *p_codec, uint8_t codec_num,
                                           T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info)
{
    if (p_codec == NULL)
    {
        return false;
    }

    for (uint8_t i = 0; i < codec_num; i++)
    {
        if (app_lea_pac_codec_item_sr_map[p_codec[i]] & p_frame_len_info->sample_rate)
        {
            app_lea_pacs_update_frame_len(p_codec[i], p_frame_len_info);
        }
    }

    if (!p_frame_len_info->frame_len_min.update && !p_frame_len_info->frame_len_max.update)
    {
        APP_PRINT_TRACE0("app_lea_pacs_frame_len_setting: set frame length fail");
        return false;
    }

    if (!p_frame_len_info->frame_len_min.update)
    {
        p_frame_len_info->frame_len_min.frame_len = p_frame_len_info->frame_len_max.frame_len;
    }
    else if (!p_frame_len_info->frame_len_max.update)
    {
        p_frame_len_info->frame_len_max.frame_len = p_frame_len_info->frame_len_min.frame_len;
    }
    return true;
}

#if F_APP_LC3_PLUS_SUPPORT
static void app_lea_pacs_lc3_plus_update_frame_len(T_LC3_PLUS_CODEC_CFG_ITEM item,
                                                   T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info)
{
    T_CODEC_CFG preferred_cfg = {0};

    memcpy(&preferred_cfg, &lc3_plus_codec_cfg[item], sizeof(T_CODEC_CFG));

    if (preferred_cfg.octets_per_codec_frame >= p_frame_len_info->frame_len_max.frame_len)
    {
        if (p_frame_len_info->frame_len_max.update)
        {
            if (p_frame_len_info->frame_len_min.update)
            {
                if (p_frame_len_info->frame_len_min.frame_len < preferred_cfg.octets_per_codec_frame)
                {
                    p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
                }
            }
            else
            {
                p_frame_len_info->frame_len_min.frame_len = p_frame_len_info->frame_len_max.frame_len;
                p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
                p_frame_len_info->frame_len_min.update = true;
            }
        }
        else
        {
            p_frame_len_info->frame_len_max.frame_len = preferred_cfg.octets_per_codec_frame;
            p_frame_len_info->frame_len_max.update = true;
        }
    }
    else
    {
        if (p_frame_len_info->frame_len_min.update)
        {
            if (p_frame_len_info->frame_len_max.update)
            {
                if (preferred_cfg.octets_per_codec_frame < p_frame_len_info->frame_len_min.frame_len)
                {
                    p_frame_len_info->frame_len_min.frame_len = preferred_cfg.octets_per_codec_frame;
                }
            }
            else
            {
                p_frame_len_info->frame_len_max.frame_len = p_frame_len_info->frame_len_min.frame_len;
                p_frame_len_info->frame_len_max.update = true;
            }
        }
        else
        {
            p_frame_len_info->frame_len_min.frame_len = preferred_cfg.octets_per_codec_frame;
            p_frame_len_info->frame_len_min.update = true;
        }
    }
}

static bool app_lea_pacs_lc3_plus_frame_len_setting(const T_LC3_PLUS_CODEC_CFG_ITEM *p_codec,
                                                    uint8_t codec_num,
                                                    T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info)
{
    if (p_codec == NULL)
    {
        return false;
    }

    for (uint8_t i = 0; i < codec_num; i++)
    {
        if (app_lea_pac_lc3_plus_codec_item_sr_map[p_codec[i]] & p_frame_len_info->sample_rate)
        {
            app_lea_pacs_lc3_plus_update_frame_len(p_codec[i], p_frame_len_info);
        }
    }

    if (!p_frame_len_info->frame_len_min.update && !p_frame_len_info->frame_len_max.update)
    {
        APP_PRINT_TRACE0("app_lea_pacs_frame_len_setting: set frame length fail");
        return false;
    }

    if (!p_frame_len_info->frame_len_min.update)
    {
        p_frame_len_info->frame_len_min.frame_len = p_frame_len_info->frame_len_max.frame_len;
    }
    else if (!p_frame_len_info->frame_len_max.update)
    {
        p_frame_len_info->frame_len_max.frame_len = p_frame_len_info->frame_len_min.frame_len;
    }
    return true;
}

#if F_APP_FRAUNHOFER_SUPPORT
static void app_lea_pacs_lc3_plus_get_frame_len(T_LEA_PAC_FRAME_LEN_INFO *p_frame_len_info,
                                                uint8_t frame_duration)
{

    for (uint8_t i = 0; i < sizeof(app_lea_pac_lc3_plus_media_codec); i++)
    {
        if (lc3_plus_codec_cfg[i].frame_duration == frame_duration &&
            (1 << (lc3_plus_codec_cfg[i].sample_frequency - 1) & p_frame_len_info->sample_rate))
        {
            app_lea_pacs_lc3_plus_update_frame_len(app_lea_pac_lc3_plus_media_codec[i], p_frame_len_info);
        }
    }

    if (!p_frame_len_info->frame_len_min.update && !p_frame_len_info->frame_len_max.update)
    {
        APP_PRINT_TRACE0("app_lea_pacs_lc3_plus_get_frame_len: get frame length fail");
        return;
    }

    if (!p_frame_len_info->frame_len_min.update)
    {
        p_frame_len_info->frame_len_min.frame_len = p_frame_len_info->frame_len_max.frame_len;
    }
    else if (!p_frame_len_info->frame_len_max.update)
    {
        p_frame_len_info->frame_len_max.frame_len = p_frame_len_info->frame_len_min.frame_len;
    }
}
#endif

bool app_lea_pacs_qos_preferred_cfg_get_by_codec(T_CODEC_CFG *p_cfg, uint8_t target_latency,
                                                 T_QOS_CFG_PREFERRED *p_qos, uint16_t conn_handle, uint8_t ase_id)
{
    uint8_t i;
    T_CODEC_CFG *p_temp_cfg;

    if (p_cfg == NULL || p_qos == NULL)
    {
        return false;
    }

#if F_APP_FRAUNHOFER_SUPPORT
    uint8_t frame_duration = 0;
    T_ASE_DATA_CODEC_CONFIGURED vendor_cfg = {0};

    if (p_cfg->type_exist & CODEC_CFG_VENDOR_INFO_EXIST)
    {
        if (ascs_get_codec_cfg(conn_handle, ase_id, &vendor_cfg))
        {
            uint16_t idx = 0;
            uint16_t length;
            uint8_t type;

            for (; idx < vendor_cfg.data.codec_spec_cfg_len;)
            {
                length = vendor_cfg.p_codec_spec_cfg[idx];
                idx++;
                type = vendor_cfg.p_codec_spec_cfg[idx];

                if (type == FRAUNHOFER_CFG_FD && length == 2)
                {
                    frame_duration = vendor_cfg.p_codec_spec_cfg[idx + 1];
                    break;
                }
                idx += length;
            }
        }
    }
#endif

    for (i = 0; i < LC3_PLUS_CODEC_CFG_ITEM_LC3_MAX; i++)
    {
        p_temp_cfg = (T_CODEC_CFG *)&lc3_plus_codec_cfg[i];
        if (
#if F_APP_FRAUNHOFER_SUPPORT
            p_temp_cfg->frame_duration == frame_duration &&
#else
            p_temp_cfg->frame_duration == p_cfg->frame_duration &&
#endif
            p_temp_cfg->sample_frequency == p_cfg->sample_frequency &&
            p_temp_cfg->octets_per_codec_frame == p_cfg->octets_per_codec_frame)
        {
            if (target_latency == ASE_TARGET_HIGHER_RELIABILITY)
            {
                memcpy(p_qos, &lc3_plus_cis_high_reliability_qos[i], sizeof(T_QOS_CFG_PREFERRED));
            }
            else
            {
                memcpy(p_qos, &lc3_plus_cis_low_latency_qos[i], sizeof(T_QOS_CFG_PREFERRED));
            }
            return true;
        }
    }

    return false;
}

T_AUDIO_FORMAT_TYPE app_lea_pacs_check_codec_type(uint8_t codec_id[CODEC_ID_LEN])
{
    T_AUDIO_FORMAT_TYPE ret = AUDIO_FORMAT_TYPE_LC3;
    uint8_t codec_type;

    LE_STREAM_TO_UINT8(codec_type, codec_id);
    if (codec_type == VENDOR_CODEC_ID)
    {
        uint16_t vendor_id;

        LE_STREAM_TO_UINT16(vendor_id, codec_id);
#if F_APP_FRAUNHOFER_SUPPORT
        if (vendor_id == FRAUNHOFER_COMPANY_ID)
        {
            uint16_t format_id;

            LE_STREAM_TO_UINT16(format_id, codec_id);
            if (format_id == FRAUNHOFER_HR_CBR_ID) // fraunhofer IIs lc3 plus
            {
                ret = AUDIO_FORMAT_TYPE_LC3PLUS;
            }
        }
#else
        if (vendor_id == RTK_COMPANY_ID)
        {
            uint16_t format_id;

            LE_STREAM_TO_UINT16(format_id, codec_id);
            if (format_id == VENDOR_SPECIFIC_LC3_PLUS_ID) // rtk lc3 plus
            {
                ret = AUDIO_FORMAT_TYPE_LC3PLUS;
            }
        }
#endif
    }

    return ret;
}

uint8_t app_lea_pacs_check_bit_depth(T_AUDIO_FORMAT_TYPE codec_type, uint8_t metadata_length,
                                     uint8_t *p_metadata)
{
    uint8_t i = 0;
    uint8_t offset;
    uint8_t length;
    uint8_t type;
    uint8_t bit_depth = 16;

    if (codec_type == AUDIO_FORMAT_TYPE_LC3 || p_metadata == NULL ||
        metadata_length < LEA_VENDOR_PARA_LEN)
    {
        return bit_depth;
    }

    while (i < metadata_length)
    {
        LE_STREAM_TO_UINT8(length, p_metadata);
        LE_STREAM_TO_UINT8(type, p_metadata);
        offset = length - 1;

        if (length == LEA_VENDOR_PARA_LEN && type == VENDOR_CODEC_ID)
        {
            uint16_t company_id;

            LE_STREAM_TO_UINT16(company_id, p_metadata);
            offset -= 2;

            if (company_id == RTK_COMPANY_ID)
            {
                LE_STREAM_TO_UINT8(length, p_metadata);
                LE_STREAM_TO_UINT8(type, p_metadata);

                if (length == 2 && type == LEA_BIT_DEPTH)
                {
                    LE_STREAM_TO_UINT8(bit_depth, p_metadata);
                }
                break;
            }
        }
        i += length;
        p_metadata += offset;
    }

    APP_PRINT_TRACE1("app_lea_uca_check_bit_depth: bit_depth %d", bit_depth);
    return bit_depth;
}

T_AUDIO_LC3PLUS_MODE app_lea_pacs_get_mode(uint32_t sample_rate,
                                           T_AUDIO_LC3PLUS_FRAME_DURATION duration, uint16_t octets)
{
    T_AUDIO_LC3PLUS_MODE mode = AUDIO_LC3PLUS_MODE_NOMRAL;

    if (sample_rate == 96000)
    {
        mode = AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION;
    }
    else if (sample_rate == 48000)
    {
        mode = AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION;
        switch (duration)
        {
        case AUDIO_LC3PLUS_FRAME_DURATION_2_5_MS:
            {
                mode = (octets >= 54) ? AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION : AUDIO_LC3PLUS_MODE_NOMRAL;
            }
            break;

        case AUDIO_LC3PLUS_FRAME_DURATION_5_MS:
            {
                mode = (octets >= 93) ? AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION : AUDIO_LC3PLUS_MODE_NOMRAL;
            }
            break;

        case AUDIO_LC3PLUS_FRAME_DURATION_7_5_MS:
            {
                mode = (octets >= 117) ? AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION : AUDIO_LC3PLUS_MODE_NOMRAL;
            }
            break;

        case AUDIO_LC3PLUS_FRAME_DURATION_10_MS:
            {
                mode = (octets >= 156) ? AUDIO_LC3PLUS_MODE_HIGH_RESOLUTION : AUDIO_LC3PLUS_MODE_NOMRAL;
            }
            break;

        default:
            break;
        }
    }
    else
    {
        mode = AUDIO_LC3PLUS_MODE_NOMRAL;
    }


    APP_PRINT_INFO4("app_lea_pacs_get_mode: sample_rate %d, duration %d, octects %d, mode %d",
                    sample_rate, duration,
                    octets, mode);
    return mode;
}
#endif


static void app_lea_pacs_update_pac(T_LEA_PAC_RECORD *pac_record, T_LEA_PAC_INFO *pac_info,
                                    uint8_t low_latency)
{
    pac_record->codec_id[0] = pac_info->codec_id;

#if F_APP_LC3_PLUS_SUPPORT
#if F_APP_FRAUNHOFER_SUPPORT
    if (pac_info->codec_id == VENDOR_CODEC_ID)
    {
        uint8_t len = 0;
        uint8_t i = 0;
        uint8_t frame_duration_supported = FRAUNHOFER_FD_ENABLE;
        T_LEA_HR_PAC_RECORD hr_pac_record = {0};

        hr_pac_record.codec_id[0] = VENDOR_CODEC_ID;
        hr_pac_record.codec_id[1] = FRAUNHOFER_COMPANY_ID & 0xFF;
        hr_pac_record.codec_id[2] = FRAUNHOFER_COMPANY_ID >> 8;
        hr_pac_record.codec_id[3] = FRAUNHOFER_HR_CBR_ID & 0xFF;
        hr_pac_record.codec_id[4] = FRAUNHOFER_HR_CBR_ID >> 8;

        hr_pac_record.sample_rate_len = 3;
        hr_pac_record.sample_rate[0] = CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES;
        hr_pac_record.sample_rate[1] = pac_info->sample_rate;
        hr_pac_record.sample_rate[2] = pac_info->sample_rate >> 8;

        hr_pac_record.channel_cnt_len = 2;
        hr_pac_record.channel_cnt[0] = CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS;
        hr_pac_record.channel_cnt[1] = pac_info->channel_cnt;

        hr_pac_record.frames_per_sdu_len = 2;
        hr_pac_record.frames_per_sdu[0] = CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU;
        hr_pac_record.frames_per_sdu[1] = pac_info->frames_per_sdu;

        hr_pac_record.hr_duration_len = 3;
        hr_pac_record.hr_duration[0] = FRAUNHOFER_SUPPORTED_FD;
        hr_pac_record.hr_duration[1] = frame_duration_supported;
        hr_pac_record.hr_duration[2] = (low_latency ? FRAUNHOFER_FD_PREFER_5_MS_BIT :
                                        FRAUNHOFER_FD_PREFER_10_MS_BIT) >> 8;
        memcpy((uint8_t *)pac_record, &hr_pac_record, sizeof(T_LEA_HR_PAC_RECORD));
        len += sizeof(T_LEA_HR_PAC_RECORD);

        while (frame_duration_supported)
        {
            if (frame_duration_supported & 0x01)
            {
                T_LEA_HR_FRAME frame_infor;
                T_LEA_HR_FD frame_duration_info = {0};
                T_LEA_PAC_FRAME_LEN_INFO frame_len_info = {0};

                frame_len_info.sample_rate = pac_info->sample_rate;
                frame_duration_info.frame_duration_bit =
                    app_lea_pac_lc3_plus_frame_duration_map[i].frame_duration_bit;
                frame_duration_info.frame_duration = app_lea_pac_lc3_plus_frame_duration_map[i].frame_duration;
                app_lea_pacs_lc3_plus_get_frame_len(&frame_len_info, frame_duration_info.frame_duration_bit);

                frame_infor.hr_frame_len = 5;
                frame_infor.hr_frame[0] = frame_duration_info.frame_duration;
                frame_infor.hr_frame[1] = frame_len_info.frame_len_min.frame_len;
                frame_infor.hr_frame[2] = frame_len_info.frame_len_min.frame_len >> 8;
                frame_infor.hr_frame[3] = frame_len_info.frame_len_max.frame_len;
                frame_infor.hr_frame[4] = frame_len_info.frame_len_max.frame_len >> 8;
                memcpy((uint8_t *)pac_record + len, &frame_infor, sizeof(T_LEA_HR_FRAME));
                len += sizeof(T_LEA_HR_FRAME);
            }
            i += 1;
            frame_duration_supported >>= 1;
        }

        pac_record->capabilities_len = len - offsetof(T_LEA_HR_PAC_RECORD, sample_rate_len);

        T_LEA_HR_META_DATA meta_data_info = {0};

        meta_data_info.metadata_len = 4;
        meta_data_info.metadata[0] = 3;
        meta_data_info.metadata[1] = METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS;
        meta_data_info.metadata[2] = pac_info->audio_context;
        meta_data_info.metadata[3] = pac_info->audio_context >> 8;
        memcpy((uint8_t *)pac_record + len, &meta_data_info, sizeof(T_LEA_HR_META_DATA));
        return;
    }
#else
    if (pac_info->codec_id == VENDOR_CODEC_ID)
    {
        pac_record->codec_id[1] = pac_info->company_id;
        pac_record->codec_id[2] = pac_info->company_id >> 8;
        pac_record->codec_id[3] = pac_info->vendor_codec_id;
        pac_record->codec_id[4] = pac_info->vendor_codec_id >> 8;
    }
#endif
#endif

    pac_record->capabilities_len = 19;
    pac_record->sample_rate_len = 3;

    pac_record->sample_rate[0] = CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES;
    pac_record->sample_rate[1] = pac_info->sample_rate;
    pac_record->sample_rate[2] = pac_info->sample_rate >> 8;

    pac_record->duration_len = 2;
    pac_record->duration[0] = CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS;
    pac_record->duration[1] = pac_info->frame_duration;

    pac_record->channel_cnt_len = 2;
    pac_record->channel_cnt[0] = CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS;
    pac_record->channel_cnt[1] = pac_info->channel_cnt;

    pac_record->frame_len = 5;
    pac_record->frame[0] = CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME;
    pac_record->frame[1] = pac_info->frame_len_min;
    pac_record->frame[2] = pac_info->frame_len_min >> 8;
    pac_record->frame[3] = pac_info->frame_len_max;
    pac_record->frame[4] = pac_info->frame_len_max >> 8;

    pac_record->frames_per_sdu_len = 2;
    pac_record->frames_per_sdu[0] = CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU;
    pac_record->frames_per_sdu[1] = pac_info->frames_per_sdu;

    pac_record->metadata_len = 4;
    pac_record->metadata[0] = 3;
    pac_record->metadata[1] = METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS;
    pac_record->metadata[2] = pac_info->audio_context;
    pac_record->metadata[3] = pac_info->audio_context >> 8;
}

static void app_lea_pacs_set_pac(T_LEA_PAC_RECORD *pac_record, bool is_snk_pac,
                                 uint16_t sample_rate,
                                 bool low_latency)

{
    T_LEA_PAC_INFO pac_info = {0};
    T_LEA_PAC_FRAME_LEN_INFO frame_len_info = {0};
    const T_CODEC_CFG_ITEM *codec_cfg_item = NULL;
    uint8_t codec_cfg_size;
    uint16_t audio_context;
#if F_APP_LC3_PLUS_SUPPORT
    const T_LC3_PLUS_CODEC_CFG_ITEM *lc3plus_codec_cfg_item = NULL;
#endif

    if (is_snk_pac)
    {
        codec_cfg_item = app_lea_pac_sink_codec;
        codec_cfg_size = sizeof(app_lea_pac_sink_codec);
        audio_context = AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_MEDIA | AUDIO_CONTEXT_CONVERSATIONAL;
    }
    else
    {
        codec_cfg_item = app_lea_pac_src_codec;
        codec_cfg_size = sizeof(app_lea_pac_src_codec);
        audio_context = AUDIO_CONTEXT_UNSPECIFIED | AUDIO_CONTEXT_CONVERSATIONAL;
    }

    pac_info.codec_id = LC3_CODEC_ID;
    pac_info.sample_rate = sample_rate;

    if (low_latency)
    {
        pac_info.frame_duration = FRAME_DURATION_10_MS_BIT | FRAME_DURATION_7_5_MS_BIT |
                                  FRAME_DURATION_PREFER_7_5_MS_BIT;
    }
    else
    {
        pac_info.frame_duration = FRAME_DURATION_10_MS_BIT | FRAME_DURATION_7_5_MS_BIT |
                                  FRAME_DURATION_PREFER_10_MS_BIT;
    }

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE && low_latency)
    {
        //not all lea source support this topology
        pac_info.channel_cnt = AUDIO_CHANNEL_COUNTS_1 | AUDIO_CHANNEL_COUNTS_2;
        pac_info.frames_per_sdu = 2;
    }
    else
    {
        pac_info.channel_cnt = AUDIO_CHANNEL_COUNTS_1;
        pac_info.frames_per_sdu = 1;
    }

    memset(&frame_len_info, 0, sizeof(frame_len_info));
    frame_len_info.sample_rate = sample_rate;
    app_lea_pacs_frame_len_setting(codec_cfg_item, codec_cfg_size, &frame_len_info);

    pac_info.frame_len_min = frame_len_info.frame_len_min.frame_len;
    pac_info.frame_len_max = frame_len_info.frame_len_max.frame_len;

    pac_info.audio_context = audio_context;

    app_lea_pacs_update_pac(pac_record, &pac_info, low_latency);

#if F_APP_LC3_PLUS_SUPPORT
    pac_record++;

    pac_info.codec_id = VENDOR_CODEC_ID;
    pac_info.company_id = RTK_COMPANY_ID;
    pac_info.vendor_codec_id = VENDOR_SPECIFIC_LC3_PLUS_ID;

    if (low_latency)
    {
        pac_info.frame_duration = FRAME_DURATION_10_MS_BIT | FRAME_DURATION_5_MS_BIT |
                                  FRAME_DURATION_2_5_MS_BIT;
    }
    else
    {
        pac_info.frame_duration = FRAME_DURATION_10_MS_BIT | FRAME_DURATION_5_MS_BIT |
                                  FRAME_DURATION_2_5_MS_BIT;
    }

    if (is_snk_pac)
    {
        lc3plus_codec_cfg_item = app_lea_pac_lc3_plus_media_codec;
        codec_cfg_size = sizeof(app_lea_pac_lc3_plus_media_codec);
    }
    else
    {
        lc3plus_codec_cfg_item = app_lea_pac_lc3_plus_convo_codec;
        codec_cfg_size = sizeof(app_lea_pac_lc3_plus_convo_codec);
    }

    memset(&frame_len_info, 0, sizeof(T_LEA_PAC_FRAME_LEN_INFO));
    frame_len_info.sample_rate = sample_rate;
    app_lea_pacs_lc3_plus_frame_len_setting(lc3plus_codec_cfg_item, codec_cfg_size, &frame_len_info);

    pac_info.frame_len_min = frame_len_info.frame_len_min.frame_len;
    pac_info.frame_len_max = frame_len_info.frame_len_max.frame_len;
    app_lea_pacs_update_pac(pac_record, &pac_info, low_latency);
#endif

}

static void app_lea_pacs_set_pac_record(bool low_latency)
{
    uint16_t sink_sample_rate = app_cfg_const.sink_sample_rate;
    uint16_t src_sample_rate = app_cfg_const.src_sample_rate;

    p_app_lea_pac_sink_record[0] = SINK_PAC_CNT; //PAC record num for pac_check_record()
#if F_APP_LC3_PLUS_SUPPORT
    p_app_lea_pac_sink_record[0] += LC3_PLUS_SINK_PAC_CNT;
#endif
    app_lea_pacs_set_pac((T_LEA_PAC_RECORD *)&p_app_lea_pac_sink_record[1], true, sink_sample_rate,
                         low_latency);

    p_app_lea_pac_source_record[0] = SOURCE_PAC_CNT; //PAC record num for pac_check_record()
#if F_APP_LC3_PLUS_SUPPORT
    p_app_lea_pac_source_record[0] += LC3_PLUS_SOURCE_PAC_CNT;
#endif
    app_lea_pacs_set_pac((T_LEA_PAC_RECORD *)&p_app_lea_pac_source_record[1], false, src_sample_rate,
                         low_latency);

    APP_PRINT_TRACE1("app_lea_pacs_set_pac_record: low_latency %d", low_latency);
}

bool app_lea_pacs_init_available_context(uint16_t conn_handle)
{
    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(conn_handle);

    if (p_link)
    {
        p_link->sink_available_contexts = app_lea_pac_sink_available_contexts;
        p_link->source_available_contexts = app_lea_pac_source_available_contexts;
        pacs_update_available_contexts(p_link->conn_handle, p_link->sink_available_contexts,
                                       p_link->source_available_contexts);
        return true;
    }
    return false;
}

bool app_lea_pacs_set_sink_available_contexts(uint16_t conn_handle, uint16_t sink_contexts,
                                              bool enable)
{
    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(conn_handle);

    if (p_link)
    {
        uint16_t pre_available_contexts = p_link->sink_available_contexts;

        if (enable)
        {
            p_link->sink_available_contexts |= sink_contexts;
        }
        else
        {
            p_link->sink_available_contexts &= ~sink_contexts;
        }

        if (pre_available_contexts != p_link->sink_available_contexts)
        {
            APP_PRINT_TRACE3("app_lea_pacs_set_sink_available_contexts: avail 0x%02X, set 0x%02X, enable %d",
                             p_link->sink_available_contexts, sink_contexts, enable);

            return pacs_update_available_contexts(conn_handle, p_link->sink_available_contexts,
                                                  p_link->source_available_contexts);
        }
    }
    return false;
}

bool app_lea_pacs_set_source_available_contexts(uint16_t conn_handle, uint16_t source_contexts,
                                                bool enable)
{
    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(conn_handle);

    if (p_link)
    {
        if (enable)
        {
            p_link->source_available_contexts |= source_contexts;
        }
        else
        {
            p_link->source_available_contexts &= ~source_contexts;
        }

        APP_PRINT_TRACE3("app_lea_pacs_set_source_available_contexts: avail 0x%02X, set 0x%02X, enable %d",
                         p_link->source_available_contexts, source_contexts, enable);

        return pacs_update_available_contexts(conn_handle, p_link->sink_available_contexts,
                                              p_link->source_available_contexts);
    }
    return false;
}

uint16_t app_lea_pacs_get_sink_available_contexts(void)
{
    return app_lea_pac_sink_available_contexts;
}

uint16_t app_lea_pacs_get_source_available_contexts(void)
{
    return app_lea_pac_source_available_contexts;
}

#if 0
void app_lea_pacs_update_low_latency(bool low_latency)
{
    app_lea_pacs_set_pac_record(low_latency);
    pacs_pac_update(app_lea_pac_sink_pac_id, (const uint8_t *)p_app_lea_pac_sink_record,
                    app_lea_pac_sink_record_size);
    pacs_pac_update(app_lea_pac_source_pac_id, (const uint8_t *)p_app_lea_pac_source_record,
                    app_lea_pac_source_record_size);
}
#endif

void app_lea_pacs_init(void)
{
    T_PACS_PARAMS pacs_params = {0};
    uint32_t sink_loc = AUDIO_LOCATION_FL | AUDIO_LOCATION_FR;
    uint32_t source_loc = AUDIO_LOCATION_FL | AUDIO_LOCATION_FR;

    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
        {
            sink_loc = AUDIO_LOCATION_FL;
            source_loc = AUDIO_LOCATION_FL;
        }
        else
        {
            sink_loc = AUDIO_LOCATION_FR;
            source_loc = AUDIO_LOCATION_FR;
        }
    }

    app_lea_pac_sink_record_size = (SINK_PAC_CNT * sizeof(T_LEA_PAC_RECORD)) + 1;
    app_lea_pac_source_record_size = (SOURCE_PAC_CNT * sizeof(T_LEA_PAC_RECORD))  + 1;

#if F_APP_LC3_PLUS_SUPPORT
    uint8_t sink_record_size = app_lea_pac_sink_record_size;
    uint8_t source_record_size = app_lea_pac_source_record_size;

    app_lea_pac_sink_record_size = sink_record_size + LC3_PLUS_SINK_PAC_CNT * sizeof(T_LEA_PAC_RECORD);
    app_lea_pac_source_record_size = source_record_size + LC3_PLUS_SINK_PAC_CNT * sizeof(
                                         T_LEA_PAC_RECORD);
#if F_APP_FRAUNHOFER_SUPPORT
    uint8_t count = 0;
    uint8_t pac_record_size = 0;
    uint8_t frame_duration_supported = FRAUNHOFER_FD_ENABLE;

    while (frame_duration_supported)
    {
        frame_duration_supported &= frame_duration_supported - 1;
        count++;
    }

    pac_record_size = sizeof(T_LEA_HR_PAC_RECORD) + sizeof(T_LEA_HR_FRAME) * count + sizeof(
                          T_LEA_HR_META_DATA);
    app_lea_pac_sink_record_size = sink_record_size + LC3_PLUS_SINK_PAC_CNT * pac_record_size;
    app_lea_pac_source_record_size = source_record_size + LC3_PLUS_SINK_PAC_CNT * pac_record_size;
#endif
#endif

    p_app_lea_pac_sink_record = os_mem_zalloc(RAM_TYPE_DATA_ON, app_lea_pac_sink_record_size);
    p_app_lea_pac_source_record = os_mem_zalloc(RAM_TYPE_DATA_ON, app_lea_pac_source_record_size);

    pacs_params.sink_locations.is_exist = true;
    pacs_params.sink_locations.is_notify = true;
    pacs_params.sink_locations.is_write = false;
    pacs_params.sink_locations.sink_audio_location = sink_loc;

    pacs_params.source_locations.is_exist = true;
    pacs_params.source_locations.is_notify = true;
    pacs_params.source_locations.is_write = false;
    pacs_params.source_locations.source_audio_location = source_loc;

    pacs_params.supported_contexts.is_notify = true;
    pacs_params.supported_contexts.sink_supported_contexts = app_lea_pac_sink_supported_contexts;
    pacs_params.supported_contexts.source_supported_contexts = app_lea_pac_source_supported_contexts;

    app_lea_pacs_set_pac_record(app_cfg_const.enable_enter_gaming_mode_after_power_on);

    pacs_pac_add(SERVER_AUDIO_SINK,
                 (const uint8_t *)p_app_lea_pac_sink_record,
                 app_lea_pac_sink_record_size, true);
    pacs_pac_add(SERVER_AUDIO_SOURCE,
                 (const uint8_t *)p_app_lea_pac_source_record,
                 app_lea_pac_source_record_size, true);
    pacs_init(&pacs_params);
    ble_audio_cback_register(app_lea_pacs_ble_audio_cback);
}
#endif
