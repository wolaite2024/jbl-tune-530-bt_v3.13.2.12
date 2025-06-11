#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_main.h"
#include "app_multilink.h"
#include "app_dongle_record.h"
#include "app_dongle_spp.h"
#include "app_report.h"
#include "audio_track.h"
#include "ring_buffer.h"
#include "stdlib.h"
#include "trace.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_roleswap_control.h"
#include "app_a2dp.h"
#include "app_sniff_mode.h"
#include "app_audio_policy.h"
#include "app_dongle_ctrl.h"
#include "app_hfp.h"
#include "app_auto_power_off.h"
#include "bt_a2dp.h"
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

#if F_APP_SIDETONE_SUPPORT
#include "app_sidetone.h"
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_A2DP_CODEC_LC3_SUPPORT
#define AUDIO_LC3_16K_FRAME_LEN  40
#define AUDIO_LC3_32K_FRAME_LEN  80
#endif

/*============================================================================*
  *                                        Variables
  *============================================================================*/
void app_dongle_stop_track(uint8_t idx)
{
    T_APP_BR_LINK *p_link = NULL;
    T_AUDIO_STREAM_USAGE usage;
    T_AUDIO_STREAM_MODE mode;
    uint8_t vol;
    T_AUDIO_FORMAT_INFO format;
    bool vol_muted;
    uint16_t latency_value = A2DP_LATENCY_MS;

    p_link = &app_db.br_link[idx];
    if (p_link != NULL && p_link->a2dp_track_handle != NULL)
    {
        audio_track_format_info_get(p_link->a2dp_track_handle, &format);
        audio_track_usage_get(p_link->a2dp_track_handle, &usage);
        audio_track_volume_out_is_muted(p_link->a2dp_track_handle, &vol_muted);
        audio_track_volume_out_get(p_link->a2dp_track_handle, &vol);

        audio_track_release(p_link->a2dp_track_handle);
        if (app_db.gaming_mode)
        {
            if (app_link_check_dongle_link(p_link->bd_addr))
            {
                mode = AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY;
            }
            else
            {
                mode = AUDIO_STREAM_MODE_LOW_LATENCY;
            }

            app_audio_get_last_used_latency(&latency_value);
        }
        else
        {
            mode = AUDIO_STREAM_MODE_NORMAL;
        }

        app_multi_stream_avrcp_set(p_link->id);
        p_link->a2dp_track_handle =  audio_track_create(AUDIO_STREAM_TYPE_PLAYBACK,
                                                        mode,
                                                        AUDIO_STREAM_USAGE_SNOOP,
                                                        format,
                                                        vol,
                                                        0,
                                                        AUDIO_DEVICE_OUT_DEFAULT,
                                                        NULL,
                                                        NULL);
        if (vol_muted)
        {
            audio_track_volume_out_mute(p_link->a2dp_track_handle);
        }

        if (app_db.gaming_mode)
        {
            if (app_link_check_dongle_link(p_link->bd_addr))
            {
                latency_value = app_dongle_get_gaming_latency();
                app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);

                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_GAMINGMODE_DONGLE);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }
            else
            {
                latency_value = app_audio_set_latency(p_link->a2dp_track_handle,
                                                      app_cfg_nv.rws_low_latency_level_record,
                                                      GAMING_MODE_DYNAMIC_LATENCY_FIX);
                bt_a2dp_stream_delay_report_request(p_link->bd_addr, latency_value);
            }

            app_audio_update_latency_record(latency_value);
        }
        else
        {
            audio_track_latency_set(p_link->a2dp_track_handle, latency_value,
                                    NORMAL_MODE_DYNAMIC_LATENCY_FIX);
            bt_a2dp_stream_delay_report_request(p_link->bd_addr, A2DP_LATENCY_MS);
        }

        audio_track_start(p_link->a2dp_track_handle);
    }
}

typedef struct
{
    bool is_start;
    uint8_t bd_addr[BD_ADDR_LENGTH];
    T_AUDIO_TRACK_HANDLE handle;
    uint8_t num_voice_buf;
    uint8_t *p_buf;
    T_RING_BUFFER voice_buf;
    T_AUDIO_EFFECT_INSTANCE sidetone_instance;
} APP_DONGLE_RECORD;

typedef struct
{
    uint8_t record_sample_rate;
    uint8_t record_bit_pool;
    uint8_t record_buffer_cnt;
    uint8_t record_frame_size;  // sbc: 2 * record_bit_pool + 8
} APP_DONGLE_RECORD_PARA;

static APP_DONGLE_RECORD dongle_record = {.is_start = false, .bd_addr = {0}, .handle = NULL};
static APP_DONGLE_RECORD_PARA dongle_record_para = {.record_sample_rate = 16, .record_bit_pool = 22, .record_buffer_cnt = 1, .record_frame_size = 52};

static uint8_t *spp_audio_buff = NULL;
static uint16_t spp_audio_size = 0;
static uint8_t spp_audio_pkt_cnt = 0;

#define COMBINE_SPP_AUDIO_PKT_NUM   1
#define DONGLE_RECORD_DATA_DBG 0

#if DONGLE_RECORD_DATA_DBG
static void dongle_dump_record_data(const char *title, uint8_t *record_data_buf, uint32_t data_len)
{
    const uint32_t bat_num = 8;
    uint32_t times = data_len / bat_num;
    uint32_t residue = data_len % bat_num;
    uint8_t *residue_buf = record_data_buf + times * bat_num;

    APP_PRINT_TRACE3("dongle_dump_record_data0: data_len %d, times %d, residue %d", data_len,
                     times, residue);
    APP_PRINT_TRACE2("dongle_dump_record_data1: record_data_buf is 0x%08x, residue_buf is 0x%08x\r\n",
                     (uint32_t)record_data_buf,
                     (uint32_t)residue_buf);

    for (int32_t i = 0; i < times; i++)
    {
        APP_PRINT_TRACE8("dongle_dump_record_data2: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         record_data_buf[i * bat_num], record_data_buf[i * bat_num + 1], record_data_buf[i * bat_num + 2],
                         record_data_buf[i * bat_num + 3],
                         record_data_buf[i * bat_num + 4], record_data_buf[i * bat_num + 5],
                         record_data_buf[i * bat_num + 6],
                         record_data_buf[i * bat_num + 7]);
    }

    switch (residue)
    {
    case 1:
        APP_PRINT_TRACE1("dongle_dump_record_data3: 0x%02x\r\n", residue_buf[0]);
        break;
    case 2:
        APP_PRINT_TRACE2("dongle_dump_record_data4: 0x%02x, 0x%02x\r\n", residue_buf[0], residue_buf[1]);
        break;
    case 3:
        APP_PRINT_TRACE3("dongle_dump_record_data5: 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1],
                         residue_buf[2]);
        break;
    case 4:
        APP_PRINT_TRACE4("dongle_dump_record_data6: 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n", residue_buf[0],
                         residue_buf[1], residue_buf[2], residue_buf[3]);
        break;
    case 5:
        APP_PRINT_TRACE5("dongle_dump_record_data7: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4]);
        break;
    case 6:
        APP_PRINT_TRACE6("dongle_dump_record_data8: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5]);
        break;
    case 7:
        APP_PRINT_TRACE7("dongle_dump_record_data9: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\r\n",
                         residue_buf[0], residue_buf[1], residue_buf[2], residue_buf[3], residue_buf[4], residue_buf[5],
                         residue_buf[6]);
        break;

    default:
        break;
    }
}
#endif

bool app_dongle_get_record_state(void)
{
    return dongle_record.is_start;
}

static void app_dongle_set_record_state(bool record_status)
{
    if (dongle_record.is_start != record_status)
    {
        dongle_record.is_start = record_status;
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
        app_multilink_customer_update_dongle_record_status(dongle_record.is_start);
#endif
    }
}

static void app_dongle_send_combine_spp_audio(uint8_t app_index, uint8_t *data, uint16_t len)
{
    uint8_t combine_buff_size = len * COMBINE_SPP_AUDIO_PKT_NUM;

    if (spp_audio_buff == NULL)
    {
        spp_audio_buff = malloc(combine_buff_size);
        if (spp_audio_buff != NULL)
        {
            spp_audio_size = combine_buff_size;
        }
    }

    if (spp_audio_buff != NULL && (spp_audio_size == combine_buff_size))
    {
        uint8_t offset = 0;

        offset = spp_audio_pkt_cnt * len;
        memcpy(spp_audio_buff + offset, data, len);

        spp_audio_pkt_cnt++;

        if (spp_audio_pkt_cnt >= COMBINE_SPP_AUDIO_PKT_NUM)
        {
            app_report_raw_data(CMD_PATH_SPP, app_index, spp_audio_buff, combine_buff_size);

            spp_audio_pkt_cnt = 0;
            memset(spp_audio_buff, 0, combine_buff_size);
        }
    }
}

void app_dongle_clear_spp_audio_buff(void)
{
    if (spp_audio_buff != NULL)
    {
        free(spp_audio_buff);
        spp_audio_buff = NULL;

        spp_audio_size = 0;
        spp_audio_pkt_cnt = 0;
    }
}

void app_dongle_mic_data_report(void *data, uint16_t required_len)
{
    uint8_t app_idx = app_link_find_br_link(app_db.connected_dongle_addr)->id;

    if (!app_link_find_br_link(app_db.connected_dongle_addr))
    {
        APP_PRINT_ERROR1("app_dongle_mic_data_report: bd_addr %b has no link",
                         TRACE_BDADDR(app_db.connected_dongle_addr));
        return;
    }

    app_idx = app_link_find_br_link(app_db.connected_dongle_addr)->id;
    if (ring_buffer_write(&dongle_record.voice_buf, data, required_len))
    {
        dongle_record.num_voice_buf++;
    }
    else
    {
        APP_PRINT_ERROR0("app_dongle_mic_data_report: dongle_record.voice_buf is full, drop pkt");
    }

    if (dongle_record.num_voice_buf == dongle_record_para.record_buffer_cnt)
    {
        uint16_t voice_data_len = dongle_record_para.record_frame_size *
                                  dongle_record_para.record_buffer_cnt;
        uint16_t malloc_len = voice_data_len + 4;
        uint16_t append_len = app_dongle_get_append_ctrl_data_len();

        malloc_len += append_len;

        uint8_t *p_data_to_send = malloc(malloc_len);

        if (p_data_to_send)
        {
            p_data_to_send[0] = DONGLE_FORMAT_START_BIT;
            p_data_to_send[1] = DONGLE_TYPE_DATA | (((voice_data_len >> 8) & 0x1) << 2);
            p_data_to_send[2] = voice_data_len;

            uint32_t actual_len = ring_buffer_read(&dongle_record.voice_buf,
                                                   voice_data_len,
                                                   p_data_to_send + 3);

            p_data_to_send[3 + voice_data_len] = DONGLE_FORMAT_STOP_BIT;

            if (actual_len == voice_data_len && app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE)
            {
                if (append_len)
                {
                    app_dongle_append_ctrl_data_to_voice_spp(p_data_to_send + 4 + voice_data_len);
                }

                /* for sbc 16k bitpoll 17, it can fit 2dh1 packet size; no need to combine */
                if (COMBINE_SPP_AUDIO_PKT_NUM > 1 &&
                    (app_cfg_const.spp_voice_smaple_rate != RECORD_SAMPLE_RATE_16K))
                {
                    app_dongle_send_combine_spp_audio(app_idx, p_data_to_send, malloc_len);
                }
                else
                {
                    app_report_raw_data(CMD_PATH_SPP, app_idx, p_data_to_send, malloc_len);
                }
            }

            free(p_data_to_send);
            dongle_record.num_voice_buf = 0;
        }
    }
}

bool app_dongle_record_read_cb(T_AUDIO_TRACK_HANDLE   handle,
                               uint32_t              *timestamp,
                               uint16_t              *seq_num,
                               T_AUDIO_STREAM_STATUS *status,
                               uint8_t               *frame_num,
                               void                  *buf,
                               uint16_t               required_len,
                               uint16_t              *actual_len)
{
    uint16_t len = dongle_record_para.record_frame_size;

    APP_PRINT_TRACE2("app_dongle_record_read_cb: buf 0x%08x, required_len %d", buf, required_len);

    if (required_len != len)
    {
        APP_PRINT_ERROR0("app_dongle_record_read_cb: required_len is incorrect");
    }

#if DONGLE_RECORD_DATA_DBG
    dongle_dump_record_data("app_dongle_record_read_cb", buf, required_len);
#endif

    if ((app_db.remote_is_dongle) && (app_db.dongle_is_enable_mic)
        && (app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE))
    {
        app_dongle_mic_data_report(buf, required_len);

    }

    *actual_len = required_len;

    return true;
}

static void dongle_record_para_update(void)
{
    if (app_cfg_const.spp_voice_smaple_rate == RECORD_SAMPLE_RATE_32K)
    {
        dongle_record_para.record_sample_rate = 32;
        dongle_record_para.record_bit_pool = 22;
        dongle_record_para.record_buffer_cnt = 6;
    }
    else    //default setting is 16k
    {
        dongle_record_para.record_sample_rate = 16;
        dongle_record_para.record_bit_pool = 17; // change bitpoll to 17 to meet 2dh1 packet size
        dongle_record_para.record_buffer_cnt = 1;
    }

#if F_APP_A2DP_CODEC_LC3_SUPPORT
    if (app_cfg_const.spp_voice_smaple_rate == RECORD_SAMPLE_RATE_32K)
    {
        dongle_record_para.record_frame_size = AUDIO_LC3_32K_FRAME_LEN;
        dongle_record_para.record_buffer_cnt = 1;
    }
    else
    {
        dongle_record_para.record_frame_size = AUDIO_LC3_16K_FRAME_LEN;
    }
#else
    dongle_record_para.record_frame_size = (2 * dongle_record_para.record_bit_pool) + 8;
#endif
}

static void dongle_voice_gen_format_info(T_AUDIO_FORMAT_INFO *p_format_info)
{
#if F_APP_A2DP_CODEC_LC3_SUPPORT
    p_format_info->type = AUDIO_FORMAT_TYPE_LC3;
    p_format_info->attr.lc3.sample_rate = (dongle_record_para.record_sample_rate * 1000);
    p_format_info->attr.lc3.chann_location = AUDIO_LC3_CHANNEL_LOCATION_MONO;
    p_format_info->attr.lc3.frame_length = dongle_record_para.record_frame_size;
    p_format_info->attr.lc3.frame_duration = AUDIO_LC3_FRAME_DURATION_10_MS;
#else
    p_format_info->type = AUDIO_FORMAT_TYPE_SBC;
    p_format_info->attr.sbc.sample_rate = (dongle_record_para.record_sample_rate * 1000);
    p_format_info->attr.sbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
    p_format_info->attr.sbc.block_length = 16;
    p_format_info->attr.sbc.subband_num = 8;
    p_format_info->attr.sbc.allocation_method = 0;
    p_format_info->attr.sbc.bitpool = dongle_record_para.record_bit_pool;
#endif
}

void app_dongle_force_stop_recording(void)
{
    APP_PRINT_TRACE0("app_dongle_force_stop_recording");

    app_dongle_set_record_state(false);
    dongle_record.num_voice_buf = 0;
    ring_buffer_clear(&dongle_record.voice_buf);
    audio_track_stop(dongle_record.handle);
    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SPP_RECORD);
}

void app_dongle_update_fake_call_status(T_APP_CALL_STATUS status)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(app_db.connected_dongle_addr);
    if (p_link != NULL)
    {
        p_link->call_status = status;

        if (p_link->call_status == APP_CALL_IDLE)
        {
            p_link->call_id_type_check = true;
            p_link->call_id_type_num = false;
        }
        app_hfp_update_call_status();
    }

    if (status == APP_CALL_IDLE)
    {
        if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
        {
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK,
                                      app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
        }

        app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_HFP_CALL_STOP);
    }
    else
    {
        if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_SOURCE_LINK);
        }

        app_audio_set_bud_stream_state(BUD_STREAM_STATE_VOICE);

        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_BUD_STREAM_STATE);
        }

        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_HFP_CALL_START);
        app_multi_pause_inactive_a2dp_link_stream(app_hfp_get_active_idx(), false);

        if ((p_link != NULL) && (app_a2dp_get_active_idx() != p_link->id))
        {
            app_dongle_stop_track(p_link->id);
        }
    }
}

/**
    * @brief        This function can stop the record.
    * @return       void
    */
void app_dongle_stop_recording(uint8_t bd_addr[6])
{
    if (memcmp(dongle_record.bd_addr, bd_addr, sizeof(dongle_record.bd_addr)) != 0)
    {
        APP_PRINT_ERROR0("dongle_voice_stop_capture: bd_addr is not matched!");
        return;
    }

    if (app_dongle_get_record_state() != true)
    {
        APP_PRINT_ERROR0("dongle_voice_stop_capture: already stopped!");
        return;
    }

    APP_PRINT_TRACE0("app_dongle_stop_recording");
    app_dongle_set_record_state(false);

    if (app_audio_is_mic_mute()
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
        && !app_slide_switch_mic_mute_toggle_support()
#endif
       )
    {
        app_audio_set_mic_mute_status(0);
        app_dongle_volume_in_unmute();
    }

    dongle_record.num_voice_buf = 0;
    ring_buffer_clear(&dongle_record.voice_buf);
    audio_track_stop(dongle_record.handle);

    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SPP_RECORD);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
    {
        T_APP_BR_LINK *p_link;
        p_link = app_link_find_br_link(app_db.connected_dongle_addr);

        app_dongle_update_fake_call_status(APP_CALL_IDLE);
        app_bt_policy_qos_param_update(app_db.connected_dongle_addr, BP_TPOLL_SPP_DISABLE);

        if (p_link == NULL)
        {
            app_hfp_update_call_status();

            app_multi_handle_sniffing_link_disconnect_event(0xff);
        }
        else
        {
            app_multi_handle_sniffing_link_disconnect_event(p_link->id);
        }
    }
}

/**
    * @brief        This function can start the record.
    * @return       void
    */
void app_dongle_start_recording(uint8_t bd_addr[6])
{
    if (app_dongle_get_record_state() != false)/*g_voice_data.is_voice_start == false8*/
    {
        APP_PRINT_ERROR0("app_dongle_start_recording: already started");
        return;
    }

    app_dongle_record_init();

    APP_PRINT_INFO0("app_dongle_start_recording");
    app_dongle_set_record_state(true);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
    {
        app_dongle_update_fake_call_status(APP_VOICE_ACTIVATION_ONGOING);
    }

    dongle_record.sidetone_instance = app_sidetone_attach(dongle_record.handle, app_dsp_cfg_sidetone);

    memcpy(dongle_record.bd_addr, bd_addr, sizeof(dongle_record.bd_addr));
    audio_track_start(dongle_record.handle);
    app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SPP_RECORD);
    app_bt_policy_qos_param_update(dongle_record.bd_addr, BP_TPOLL_SPP_ENABLE);

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
    if (app_slide_switch_mic_mute_toggle_support())
    {
        app_hfp_mute_ctrl();
    }
#endif
}

void app_dongle_volume_in_mute(void)
{
    audio_track_volume_in_mute(dongle_record.handle);
    app_audio_tone_type_play(TONE_MIC_MUTE_ON, false, true);
}

void app_dongle_volume_in_unmute(void)
{
    audio_track_volume_in_unmute(dongle_record.handle);
    app_audio_tone_type_play(TONE_MIC_MUTE_OFF, false, true);
}

void app_dongle_record_init(void)
{
    T_AUDIO_FORMAT_INFO format_info = {};

    dongle_record_para_update();
    dongle_voice_gen_format_info(&format_info);

    if (dongle_record.handle == NULL)
    {
        /* BAU dongle uses a2dp encode for recording, it is same as normal apt.*/
        dongle_record.handle = audio_track_create(AUDIO_STREAM_TYPE_RECORD,
                                                  AUDIO_STREAM_MODE_NORMAL,
                                                  AUDIO_STREAM_USAGE_LOCAL,
                                                  format_info,
                                                  0,
                                                  app_dsp_cfg_vol.record_volume_default,
                                                  AUDIO_DEVICE_IN_MIC,
                                                  NULL,
                                                  app_dongle_record_read_cb);
    }

    if (dongle_record.p_buf == NULL)
    {
        dongle_record.p_buf = malloc(dongle_record_para.record_frame_size *
                                     dongle_record_para.record_buffer_cnt + 1);
        dongle_record.num_voice_buf = 0;

        ring_buffer_init(&dongle_record.voice_buf, dongle_record.p_buf,
                         dongle_record_para.record_frame_size * dongle_record_para.record_buffer_cnt + 1);
    }

    if (dongle_record.handle == NULL)
    {
        APP_PRINT_ERROR0("app_dongle_record_init: handle is NULL");
    }
}
#endif
