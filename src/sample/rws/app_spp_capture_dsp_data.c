#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
#include <string.h>
#include "os_mem.h"
#include "stdlib.h"
#include "sysm.h"
#include "audio.h"
#include "audio_probe.h"
#include "audio_track.h"
#include "app_spp_capture_dsp_data.h"
#include "app_bt_policy_int.h"
#include "app_sniff_mode.h"
#include "app_report.h"
#include "app_dlps.h"
#include "app_timer.h"
#include "app_main.h"
#include "app_bond.h"
#include "app_hfp.h"
#include "app_cfg.h"
#include "btm.h"
#include "bt_bond.h"
#include "gap_br.h"
#include "trace.h"
#include "pm.h"

//for CMD_AUDIO_DSP_CTRL_SEND to capture dsp data
#define VENDOR_SPP_CAPTURE_DSP_LOG      0x01
#define VENDOR_SPP_CAPTURE_DSP_RWA_DATA 0x02
#define H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU 0x1F
#define H2D_SPPCAPTURE_SET              0x0F01
#define CHANGE_MODE_EXIST               0x00
#define CHANGE_MODE_TO_SCO              0x01

static uint8_t dsp_capture_data_master_retry = 0;
static uint8_t *dsp_spp_capture_cmd_ptr = NULL;
static uint8_t dsp_spp_capture_cmd_ptr_len;
static uint8_t dsp_spp_capture_app_idx;
static uint8_t dsp_capture_data_state = 0;
static uint8_t dsp_capture_data_app_idx;

static uint8_t app_spp_capture_timer_id = 0;
static uint8_t timer_idx_dsp_spp_captrue_check_link = 0;
static uint8_t app_spp_capture_freq_handle = 0;

T_AUDIO_TRACK_HANDLE dsp_spp_capture_audio_track_handle;

typedef enum
{
    APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK,
} T_APP_SPP_CAPTURE_TIMER;

typedef struct
{
    uint16_t    data_len;
    uint8_t     *p_data;
} T_PROBE_CB_MAILBOX_DATA;

uint8_t app_spp_capture_data_state(void)
{
    return dsp_capture_data_state;
}

bool app_spp_capture_executing_check(void)
{
    return (dsp_capture_data_state & (SPP_CAPTURE_DATA_LOG_EXECUTING | SPP_CAPTURE_RAW_DATA_EXECUTING))
           ? true : false;
}

bool app_spp_capture_audio_dsp_ctrl_send_handler(uint8_t *cmd_ptr, uint16_t cmd_len,
                                                 uint8_t cmd_path, uint8_t app_idx, uint8_t *ack_pkt, bool send_flag)
{
    bool send_cmd_flag = send_flag;
    uint32_t actual_mhz;
    uint16_t h2d_cmd_id = (uint16_t)(cmd_ptr[2] | (cmd_ptr[3] << 8));
    APP_PRINT_TRACE3("CMD_AUDIO_DSP_CTRL_SEND %x %x %x", cmd_ptr[2], h2d_cmd_id,
                     app_cfg_const.enable_dsp_capture_data_by_spp);

    //for capture dsp data handle stop cmd
    if (app_cfg_const.enable_dsp_capture_data_by_spp)
    {
        if (cmd_ptr[2] == H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU)
        {
            uint8_t i = 10; //begin of para 1
            uint8_t end = 0;
            dsp_capture_data_state &= ~(SPP_CAPTURE_DATA_START_MASK | SPP_CAPTURE_DATA_SWAP_TO_MASTER);

            if (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG)
            {
                end = 13;//end of para 2
            }
            else if (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_RWA_DATA)
            {
                end = 17; //end of para 3
            }
            APP_PRINT_TRACE1("CMD_AUDIO_DSP_CTRL_SEND %b", TRACE_BINARY(end, cmd_ptr));

            while (i <= end)
            {
                if (cmd_ptr[i] != 0)
                {
                    dsp_capture_data_state |= SPP_CAPTURE_DATA_START_MASK;
                    break;
                }
                i++;
            }

            if (dsp_capture_data_state & SPP_CAPTURE_DATA_START_MASK)
            {
                //memset(&(g_vendor_spp_data), 0, sizeof(T_VENDOR_SPP_DATA));
                //g_vendor_spp_data.spp_ptr = malloc(VENDOR_SPP_ALLOCATE_SIZE);
                //g_temp_pkt = malloc(VENDOR_SPP_MAX_SIZE);
                send_cmd_flag = false;
                dsp_capture_data_state |= SPP_CAPTURE_DATA_SWAP_TO_MASTER;
                dsp_capture_data_state |= (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG) ?
                                          SPP_CAPTURE_DATA_LOG_EXECUTING : SPP_CAPTURE_RAW_DATA_EXECUTING;
                app_dlps_disable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);
                gap_br_vendor_data_rate_set(0); // force 3M not support yet, 0 : 2M/3M

                bt_acl_pkt_type_set(app_db.br_link[app_idx].bd_addr, BT_ACL_PKT_TYPE_3M);
                dsp_capture_data_state &= ~SPP_CAPTURE_DATA_START_MASK;
                dsp_spp_capture_cmd_ptr = malloc(cmd_len + 2);
                dsp_spp_capture_cmd_ptr_len = cmd_len - 2;
                memcpy(dsp_spp_capture_cmd_ptr, &cmd_ptr[2], dsp_spp_capture_cmd_ptr_len);

                pm_cpu_freq_req(&app_spp_capture_freq_handle, 100, &actual_mhz);
                bt_link_qos_set(app_db.br_link[app_idx].bd_addr, BT_QOS_TYPE_GUARANTEED, 6);
                dsp_capture_data_app_idx = app_idx;
                app_bt_policy_abandon_engage();

                if (app_hfp_get_call_status() ||
                    ((app_db.br_link[app_idx].acl_link_role == BT_LINK_ROLE_MASTER)))
                {
                    if (dsp_spp_capture_cmd_ptr)
                    {
                        // send H2D_SPPCAPTURE_SET cmd
                        audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                        free(dsp_spp_capture_cmd_ptr);
                        dsp_spp_capture_cmd_ptr = NULL;
                    }
                }
                else
                {
                    bt_link_role_switch(app_db.br_link[app_idx].bd_addr, true);
                }
            }
            else
            {
                // reset to default
                dsp_capture_data_state &= ~((cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG) ?
                                            SPP_CAPTURE_DATA_LOG_EXECUTING : SPP_CAPTURE_RAW_DATA_EXECUTING);

                if ((dsp_capture_data_state & (SPP_CAPTURE_DATA_LOG_EXECUTING | SPP_CAPTURE_RAW_DATA_EXECUTING)) ==
                    0)
                {
                    app_dlps_enable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);
                    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);
                    bt_acl_pkt_type_set(app_db.br_link[app_idx].bd_addr, BT_ACL_PKT_TYPE_2M);
                }

                pm_cpu_freq_clear(&app_spp_capture_freq_handle, &actual_mhz);
                bt_link_role_switch(app_db.br_link[app_idx].bd_addr, false);
                bt_link_qos_set(app_db.br_link[app_idx].bd_addr, BT_QOS_TYPE_GUARANTEED, 40);
            }
        }
        else if (h2d_cmd_id == H2D_SPPCAPTURE_SET)
        {
            uint32_t plan_profs;
            uint32_t bond_flag;
            dsp_spp_capture_app_idx = app_idx;

            if (cmd_ptr[6] == CHANGE_MODE_TO_SCO)
            {
                send_cmd_flag = false;
                dsp_spp_capture_cmd_ptr = malloc(cmd_len + 2);
                dsp_spp_capture_cmd_ptr_len = cmd_len - 2;
                memcpy(dsp_spp_capture_cmd_ptr, &cmd_ptr[2], dsp_spp_capture_cmd_ptr_len);
                dsp_capture_data_state |= SPP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK;
                plan_profs = (app_db.br_link[app_idx].connected_profile & (~RDTP_PROFILE_MASK) &
                              (~SPP_PROFILE_MASK));

                if (plan_profs)
                {
                    app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, plan_profs);
                }

                app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                                app_spp_capture_timer_id, APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK, app_idx, false,
                                1500);
            }
            else if (cmd_ptr[6] == CHANGE_MODE_EXIST)
            {
                if (dsp_spp_capture_audio_track_handle != NULL)
                {
                    audio_track_release(dsp_spp_capture_audio_track_handle);
                }
                dsp_capture_data_state &= ~SPP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK;

                bt_bond_flag_get(app_db.br_link[app_idx].bd_addr, &bond_flag);
                if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                {
                    plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                    app_bt_policy_default_connect(app_db.br_link[app_idx].bd_addr, plan_profs, false);
                }
            }
        }
    }

    return send_cmd_flag;
}

static void app_spp_capture_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_ACL_ROLE_MASTER:
        {
            if (dsp_capture_data_state & SPP_CAPTURE_DATA_SWAP_TO_MASTER)
            {
                if (dsp_spp_capture_cmd_ptr)
                {
                    audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                    dsp_capture_data_state |= SPP_CAPTURE_DATA_START_MASK;
                    free(dsp_spp_capture_cmd_ptr);
                    dsp_spp_capture_cmd_ptr = NULL;
                }
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_SWITCH_FAIL:
        {
            if (dsp_capture_data_state & SPP_CAPTURE_DATA_SWAP_TO_MASTER)
            {
                if (dsp_capture_data_master_retry < 3)
                {
                    bt_link_role_switch(param->acl_role_switch_fail.bd_addr, true);
                    dsp_capture_data_master_retry++;
                }
                else
                {
                    if (dsp_spp_capture_cmd_ptr)
                    {
                        audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                        dsp_capture_data_state |= SPP_CAPTURE_DATA_START_MASK;
                        dsp_capture_data_state &= ~SPP_CAPTURE_DATA_SWAP_TO_MASTER;
                        dsp_capture_data_master_retry = 0;
                        free(dsp_spp_capture_cmd_ptr);
                        dsp_spp_capture_cmd_ptr = NULL;
                    }
                }
            }
        }
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        {

        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_cmd_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_spp_capture_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            if (param->track_state_changed.handle == dsp_spp_capture_audio_track_handle)
            {
                APP_PRINT_ERROR1("track_state, %x", param->track_state_changed.state);

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                {
                    uint8_t send_pkt_num = 5;
                    uint8_t seq_num;
                    uint8_t p_audio_buf[4] = {0, 0, 0, 0};
                    uint8_t buf[4] = {0, 0, 0, 0};

                    for (seq_num = 0; seq_num < send_pkt_num; seq_num++)
                    {
                        uint16_t written_len;
                        audio_track_write(dsp_spp_capture_audio_track_handle,
                                          0,//              timestamp,
                                          seq_num,
                                          AUDIO_STREAM_STATUS_CORRECT,
                                          1,//            frame_num,
                                          (uint8_t *)p_audio_buf,
                                          sizeof(p_audio_buf),
                                          &written_len);
                    }

                    dsp_capture_data_state |= SPP_CAPTURE_DATA_ENTER_SCO_MODE_MASK;

                    if (dsp_spp_capture_cmd_ptr)
                    {
                        // send H2D_SPPCAPTURE_SET cmd
                        audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                        free(dsp_spp_capture_cmd_ptr);
                        dsp_spp_capture_cmd_ptr = NULL;
                    }

                    buf[0] = 1;
                    app_report_event(CMD_PATH_SPP, EVENT_AUDIO_DSP_CTRL_INFO, dsp_spp_capture_app_idx, buf, 4);
                }

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                {
                    dsp_spp_capture_audio_track_handle = NULL;
                    dsp_capture_data_state &= ~SPP_CAPTURE_DATA_ENTER_SCO_MODE_MASK;
                }
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (param->track_state_changed.handle == dsp_spp_capture_audio_track_handle)
                {
                    T_APP_BR_LINK *p_link;
                    p_link = app_link_find_br_link(app_db.br_link[dsp_spp_capture_app_idx].bd_addr);
                    if (p_link == NULL)
                    {
                        break;
                    }

                    APP_PRINT_TRACE1("app_cmd_audio_cback: data ind len %u", param->track_data_ind.len);
                    uint32_t timestamp;
                    uint16_t seq_num;
                    uint8_t frame_num;
                    uint16_t read_len;
                    T_AUDIO_STREAM_STATUS status;
                    uint8_t *buf = NULL;

                    buf = malloc(param->track_data_ind.len);

                    if (buf == NULL)
                    {
                        return;
                    }

                    if (audio_track_read(dsp_spp_capture_audio_track_handle,
                                         &timestamp,
                                         &seq_num,
                                         &status,
                                         &frame_num,
                                         buf,
                                         param->track_data_ind.len,
                                         &read_len) == true)
                    {
                        if (p_link->duplicate_fst_sco_data)
                        {
                            p_link->duplicate_fst_sco_data = false;
                            bt_sco_data_send(app_db.br_link[dsp_spp_capture_app_idx].bd_addr, seq_num - 1, buf, read_len);
                        }
                        bt_sco_data_send(app_db.br_link[dsp_spp_capture_app_idx].bd_addr, seq_num, buf, read_len);
                    }

                    free(buf);
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_cmd_audio_cback: event_type 0x%04x", event_type);
    }
}

static void app_spp_capture_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK:
        {
            app_stop_timer(&timer_idx_dsp_spp_captrue_check_link);

            if ((app_db.br_link[param].connected_profile & (~RDTP_PROFILE_MASK) & (~SPP_PROFILE_MASK)))
            {
                app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                                app_spp_capture_timer_id, APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK, param, false,
                                1500);
            }
            else
            {
                T_AUDIO_FORMAT_INFO format_info;

                format_info.type = AUDIO_FORMAT_TYPE_MSBC;
                format_info.attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
                format_info.attr.msbc.sample_rate = 16000;
                format_info.attr.msbc.bitpool = 26;
                format_info.attr.msbc.allocation_method = 0;
                format_info.attr.msbc.subband_num = 8;
                format_info.attr.msbc.block_length = 15;

                if (dsp_spp_capture_audio_track_handle)
                {
                    audio_track_release(dsp_spp_capture_audio_track_handle);
                }

                dsp_spp_capture_audio_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                                        AUDIO_STREAM_MODE_NORMAL,
                                                                        AUDIO_STREAM_USAGE_LOCAL,
                                                                        format_info,
                                                                        app_dsp_cfg_vol.voice_out_volume_default,
                                                                        app_dsp_cfg_vol.voice_volume_in_default,
                                                                        AUDIO_DEVICE_OUT_DEFAULT | AUDIO_DEVICE_IN_DEFAULT,
                                                                        NULL,
                                                                        NULL);

                audio_track_latency_set(dsp_spp_capture_audio_track_handle, 15, false);
                audio_track_start(dsp_spp_capture_audio_track_handle);
            }
        }
        break;

    default:
        break;
    }
}

static void app_spp_capture_dsp_data_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case AUDIO_PROBE_DSP_EVT_MAILBOX_DSP_DATA:
        {
            T_PROBE_CB_MAILBOX_DATA *p_info = (T_PROBE_CB_MAILBOX_DATA *)msg;
            uint16_t mail_box_data_seq = p_info->p_data[8] | (p_info->p_data[9] << 8);
            APP_PRINT_INFO1("app_test_dsp_event_cback mailbox seq %d", mail_box_data_seq);
            app_report_event(CMD_PATH_SPP, EVENT_AUDIO_DSP_CTRL_INFO, dsp_capture_data_app_idx,
                             p_info->p_data, p_info->data_len);
        }
        break;

    default:
        break;
    }
}

void app_spp_capture_init(void)
{
    audio_mgr_cback_register(app_spp_capture_audio_cback);
    bt_mgr_cback_register(app_spp_capture_bt_cback);
    app_timer_reg_cb(app_spp_capture_timeout_cb, &app_spp_capture_timer_id);
    audio_probe_dsp_evt_cback_register(app_spp_capture_dsp_data_cback);
}

#endif
