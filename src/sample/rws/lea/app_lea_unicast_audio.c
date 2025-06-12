#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "ascs_mgr.h"
#include "audio_probe.h"
#include "audio_volume.h"
#include "sidetone.h"
#include "ble_conn.h"
#include "bt_direct_msg.h"
#include "bt_types.h"
#include "bt_syscall.h"
#include "bt_bond.h"
#include "bt_bond_le.h"
#include "codec_qos.h"
#include "ccp_client.h"
#include "mcp_client.h"
#include "metadata_def.h"
#include "multitopology_ctrl.h"
#include "gap_cig_mgr.h"
#include "gap_vendor.h"
#include "gap_conn_le.h"
#include "gap_iso_data.h"
#include "pacs_def.h"
#include "app_a2dp.h"
#include "app_audio_policy.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_bond.h"
#include "app_flags.h"
#include "app_hfp.h"
#include "app_main.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif
#include "app_relay.h"
#include "app_report.h"
#include "app_sensor.h"
#include "app_sniff_mode.h"
#include "app_vendor.h"
#include "app_nrec.h"
#include "app_roleswap_control.h"
#include "app_io_msg.h"
#include "app_lea_adv.h"
#include "app_lea_ascs.h"
#include "app_lea_ccp.h"
#include "app_lea_mcp.h"
#include "app_lea_profile.h"
#include "app_lea_pacs.h"
#include "app_lea_unicast_audio.h"
#include "app_lea_vol_def.h"
#include "app_lea_vol_policy.h"
#include "app_multilink.h"

#if F_APP_SIDETONE_SUPPORT
#include "app_sidetone.h"
#endif

#if F_APP_LEA_REALCAST_SUPPORT
#include "app_lea_realcast.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_LC3_PLUS_SUPPORT
#include "app_lea_pacs.h"
#endif

#include "app_bt_point.h"

#if GFPS_SASS_LEA_SUPPORT
#include "bt_gfps.h"
#include "app_gfps_msg.h"
#include "app_sass_policy.h"
#endif

//ysc start
#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_ble.h"
#include "app_led.h"
#endif
//ysc end

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
#define CIS_PERMIT_MIN_PD_LOW_LATENCY   4500
#define CIS_PERMIT_MIN_PD_NORMAL        20000
#define CIS_PREFER_MAX_TL               60

#define LEA_ISO_DATA_DELTA_TIMESTAMP    3000

#define LEA_RECORD_PREQUEUE_PKT_NUM     4

#define LEA_ISO_TOLERABLE_TIME_DIFF     100000 // us

/* By frame duration 10ms, exceed two uplink iso packet to start drop data */
#define CIS_UPLINK_MIN_SCHEDULE_TIME 20

#define LEA_FRAME_DURATION_7_5_MS 7.5
#define LEA_FRAME_DURATION_10_MS 10

#define LEA_LOCAL_TRACK_READY    0x01
#define LEA_REMOTE_TRACK_READY   0x02
#define LEA_DS_SYNC_READY        0x04
#define LEA_BOTH_TRACK_READY     (LEA_LOCAL_TRACK_READY|LEA_REMOTE_TRACK_READY)
typedef enum
{
    UCA_MSG_SYNC_DOWNSTREAM         = 0x01,
    UCA_MSG_SYNC_DOWNSTREAM_STATE   = 0x02,
    UCA_MSG_SYNC_LEA_CONNECT        = 0x03,
    UCA_MSG_SYNC_PRI_REQ_LOC        = 0x04,
    UCA_MSG_SYNC_SEC_RSP_LOC        = 0x05,
    UCA_MSG_SYNC_GAMING_MODE        = 0x06,
    UCA_MSG_TOTAL
} T_UCA_REMOTE_MSG;

typedef struct
{
    uint8_t addr[6];
    uint8_t downstream_ready;
} T_UCA_DS_INFO;
typedef enum
{
    UCA_BOND_ACTION_ADD      = 0x01,
    UCA_BOND_ACTION_DEL      = 0x02,
    UCA_BOND_ACTION_HIGHEST  = 0x03,
    UCA_BOND_ACTION_TOTAL
} T_UCA_BOND_ACTION;

typedef struct uca_iso_elem
{
    struct uca_iso_elem *p_next;
    uint8_t *iso_data;
    uint16_t iso_seq;
    uint16_t iso_sdu_len;
    uint32_t timestamp;
} T_UCA_ISO_ELEM;

static bool app_lea_uca_diff_path = true;
static T_AUDIO_EFFECT_INSTANCE app_lea_uca_eq = NULL;
static bool app_lea_uca_eq_enabled = false;
static T_APP_LE_LINK *app_lea_uca_active_stream_link = NULL;
static T_OS_QUEUE app_lea_uca_left_ch_queue;
static T_OS_QUEUE app_lea_uca_right_ch_queue;
static T_OS_QUEUE app_lea_uca_upstream_queue;

static bool app_lea_uca_mic_mute_state = false;
static uint16_t lea_playback_resume_cnt = 0;

static const uint32_t app_lea_sample_rate_freq_map[SAMPLING_FREQUENCY_CFG_384K] =
{
    8000, 11000, 16000, 22000, 24000, 32000, 44100, 48000, 88000, 96000, 176000, 192000, 384000
};

static void app_lea_uca_track_reconfig_write_length(T_APP_LE_LINK *p_link);
static void app_lea_uca_track_release(T_APP_LE_LINK *p_link, T_LEA_ASE_ENTRY *p_ase_entry);

static void app_lea_uca_bond_action(T_UCA_BOND_ACTION action, uint8_t *bd_addr)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    APP_PRINT_INFO2("app_lea_uca_bond_action: action %d, bd_addr %s", action, TRACE_BDADDR(bd_addr));

    switch (action)
    {
    case UCA_BOND_ACTION_ADD:
        {
            uint8_t idx = 0;

            if (bt_bond_index_get(bd_addr, &idx))
            {
                T_APP_REMOTE_MSG_PAYLOAD_PROFILE_CONNECTED msg = {0};

                bt_bond_flag_add(bd_addr, BOND_FLAG_UCA);

                memcpy(msg.bd_addr, bd_addr, 6);
                msg.prof_mask = UCA_PROFILE_MASK;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK,
                                                    APP_REMOTE_MSG_PROFILE_CONNECTED,
                                                    (uint8_t *)&msg, sizeof(msg));
            }
            else
            {
                T_APP_LINK_RECORD link_record = {0};
                T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED link_key = {0};

                app_bond_key_set(bd_addr, link_record.link_key, link_record.key_type);
                bt_bond_flag_add(bd_addr, BOND_FLAG_UCA);

                memcpy(link_key.bd_addr, bd_addr, 6);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE,
                                                    APP_REMOTE_MSG_LINK_RECORD_ADD,
                                                    (uint8_t *)&link_key, sizeof(link_key));
            }
        }
        break;

    case UCA_BOND_ACTION_DEL:
        {
            uint32_t bond_flag = 0;

            bt_bond_flag_remove(bd_addr, BOND_FLAG_UCA);

            bt_bond_flag_get(bd_addr, &bond_flag);
            if (bond_flag == 0)
            {
                bt_bond_delete(bd_addr);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE,
                                                    APP_REMOTE_MSG_LINK_RECORD_DEL,
                                                    bd_addr, 6);
            }
        }
        break;

    case UCA_BOND_ACTION_HIGHEST:
        {
            app_bond_set_priority(bd_addr);
        }
        break;

    default:
        break;
    }
}

void app_lea_uca_set_stream_link(T_APP_LE_LINK *p_link)
{
    app_lea_uca_active_stream_link = p_link;

#if GFPS_SASS_LEA_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        // app_gfps_notify_conn_status();

        if (p_link && mtc_get_btmode())
        {
            switch (app_sass_policy_context_to_conn_state(p_link))
            {
            case SASS_LEA_CALL:
                {
                    app_gfps_msg_notify_multipoint_switch(p_link->id, SASS_LE_LINK, GFPS_MULTIPOINT_SWITCH_REASON_HFP);
                }
                break;

            case SASS_LEA_MEDIA_WITHOUT_CONTROL:
            case SASS_LEA_MEDIA_WITH_CONTROL:
                {
                    app_gfps_msg_notify_multipoint_switch(p_link->id, SASS_LE_LINK, GFPS_MULTIPOINT_SWITCH_REASON_A2DP);
                }
                break;

            default:
                app_gfps_msg_notify_multipoint_switch(p_link->id, SASS_LE_LINK,
                                                      GFPS_MULTIPOINT_SWITCH_REASON_UNKNOWN);
            }
        }
    }
#endif

}

T_APP_LE_LINK *app_lea_uca_get_stream_link(void)
{
    return app_lea_uca_active_stream_link;
}

T_APP_LE_LINK *app_lea_uca_get_active_link(void)
{
    T_APP_LE_LINK *p_link = NULL;

    if (app_lea_uca_active_stream_link)
    {
        p_link = app_lea_uca_active_stream_link;
    }
    else if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
    {
        p_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());
    }
    else
    {
        p_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());
    }

    if (p_link == NULL)
    {
        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if (app_db.le_link[i].used == true &&
                app_db.le_link[i].state == LE_LINK_STATE_CONNECTED &&
                app_db.le_link[i].encryption_status == LE_LINK_ENCRYPTIONED)
            {
                p_link = &app_db.le_link[i];
                break;
            }
        }
    }

    return p_link;
}

static void app_lea_uca_set_s2m_zero_packet(uint16_t cis_conn_handle, uint8_t val)
{
    static uint8_t used_val = 0xFF;
    uint8_t buffer[4] = {0};

    if (val == 0xFF)
    {
        //reset only occur at cis reconnection.
        used_val = val;
        return;
    }

    if (used_val == val)
    {
        return;
    }

    used_val = val;

    /* fixed para */
    buffer[0] = 0x1B;

    memcpy(&buffer[1], &cis_conn_handle, 2);

    /*
        1 : enable
        0 : disable
    */
    buffer[3] = val;

    gap_vendor_cmd_req(0xFD80, sizeof(buffer), buffer);

    APP_PRINT_TRACE1("app_lea_uca_set_s2m_zero_packet: %b", TRACE_BINARY(sizeof(buffer), buffer));
}

static T_GAP_CAUSE app_lea_uca_send_iso_data(uint8_t *p_data, T_LEA_ASE_ENTRY *p_ase_entry,
                                             uint16_t iso_sdu_len,
                                             bool ts_flag, uint32_t timestamp)
{
    T_SYNCCLK_LATCH_INFO_TypeDef *p_latch_info = synclk_drv_time_get(SYNCCLK_ID4);
    uint32_t next_ap = 0;
    uint16_t iso_interval = 0;
    uint16_t seq_num;
    uint16_t handle = p_ase_entry->cis_conn_handle;
    uint8_t i = 0;
    T_UCA_ISO_ELEM *iso_elem = NULL;
    T_GAP_CAUSE ret = GAP_CAUSE_ERROR_UNKNOWN;
    static uint32_t last_send_iso_time = 0;
    bool skip_seq = false;

    if (p_ase_entry->prequeue_pkt_cnt < LEA_RECORD_PREQUEUE_PKT_NUM)
    {
        iso_elem = audio_probe_media_buffer_malloc(sizeof(T_UCA_ISO_ELEM));

        bool iso_enqueue = false;

        if (iso_elem)
        {
            iso_elem->iso_data = audio_probe_media_buffer_malloc(iso_sdu_len);

            if (iso_elem->iso_data)
            {
                memcpy(iso_elem->iso_data, p_data, iso_sdu_len);
                iso_elem->iso_sdu_len = iso_sdu_len;
                iso_elem->iso_seq = p_ase_entry->pkt_seq;
                iso_elem->timestamp = p_latch_info->exp_sync_clock;

                iso_enqueue = true;
            }

            APP_PRINT_TRACE1("app_lea_uca_send_iso_data: prequeue seq 0x%x", iso_elem->iso_seq);
        }

        if (iso_enqueue)
        {
            os_queue_in(&app_lea_uca_upstream_queue, iso_elem);
            p_ase_entry->prequeue_pkt_cnt++;

            ret = GAP_CAUSE_SUCCESS;
        }
    }
    else
    {
        /* dequeue iso and send all to prevent lowerstack starvation issue
        *  (due to pkt not generate uniformly)
        */

        iso_elem = os_queue_out(&app_lea_uca_upstream_queue);

        while (iso_elem != NULL)
        {
            gap_iso_send_data(iso_elem->iso_data, handle, iso_elem->iso_sdu_len,
                              ts_flag, iso_elem->timestamp, iso_elem->iso_seq);

            audio_probe_media_buffer_free(iso_elem->iso_data);
            audio_probe_media_buffer_free(iso_elem);

            iso_elem = os_queue_out(&app_lea_uca_upstream_queue);
        }

        if (p_latch_info->exp_sync_clock > last_send_iso_time)
        {
            if ((p_latch_info->exp_sync_clock - last_send_iso_time) > LEA_ISO_TOLERABLE_TIME_DIFF)
            {
                skip_seq = true;
            }
        }
        else
        {
            if (0xffffffff - last_send_iso_time + p_latch_info->exp_sync_clock)
            {
                skip_seq = true;
            }
        }

        last_send_iso_time = p_latch_info->exp_sync_clock;

        if (skip_seq)
        {
            /* skip iso seq maybe due to audio track restart and
               time diff between subsequent sequence is too large */
            APP_PRINT_TRACE0("app_lea_uca_send_iso_data: skip iso seq");
            p_ase_entry->pkt_seq++;
        }

        ret = gap_iso_send_data(p_data, handle, iso_sdu_len,
                                ts_flag, p_latch_info->exp_sync_clock, p_ase_entry->pkt_seq);

        APP_PRINT_TRACE2("app_lea_uca_send_iso_data: send seq 0x%x len %d", p_ase_entry->pkt_seq,
                         iso_sdu_len);
    }

    p_ase_entry->pkt_seq++;

    return ret;
}

static void app_lea_uca_le_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause,
                                         uint16_t disc_cause)
{
    T_APP_LE_LINK *p_link;

    APP_PRINT_TRACE2("app_lea_uca_le_disconnect_cb: local cause 0x%02X, cause 0x%02X", local_disc_cause,
                     disc_cause);

    p_link = app_link_find_le_link_by_conn_id(conn_id);
    if (p_link)
    {
        app_lea_uca_link_sm(p_link->conn_handle, LEA_DISCONNECT, &disc_cause);

#if F_APP_GAMING_DONGLE_SUPPORT
        app_dongle_handle_ble_disconnected(p_link->bd_addr);
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
        app_harman_remote_device_name_crc_set(conn_id, NULL, false);
        app_harman_le_common_adv_update();
#endif
    }

    app_lea_adv_start();
}

static uint16_t app_lea_uca_handle_cccd_msg(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;

    switch (msg)
    {
    case LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO:
        {
            T_SERVER_ATTR_CCCD_INFO *p_cccd = (T_SERVER_ATTR_CCCD_INFO *)buf;

            if (p_cccd->char_uuid >= ASCS_UUID_CHAR_SNK_ASE &&
                p_cccd->char_uuid <= PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS)
            {
                uint8_t conn_id;
                T_APP_LE_LINK *p_link;

                le_get_conn_id_by_handle(p_cccd->conn_handle, &conn_id);
                p_link = app_link_find_le_link_by_conn_id(conn_id);
                if (p_link && ((p_link->lea_device & APP_LEA_PACS_CCCD_ENABLED) == 0))
                {
                    p_link->lea_device |= APP_LEA_PACS_CCCD_ENABLED;
                    if (p_link->lea_link_state == LEA_LINK_IDLE && p_link->conn_handle != 0)
                    {
                        app_lea_uca_link_sm(p_link->conn_handle, LEA_CONNECT, NULL);
                    }
                    app_lea_adv_set_started_flag(false);
                    app_link_reg_le_link_disc_cb(conn_id, app_lea_uca_le_disconnect_cb);
                }
            }

            APP_PRINT_TRACE5("app_lea_uca_handle_cccd_msg: LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO \
conn_handle 0x%02X, service_id %d, char_uuid 0x%02X, ccc_bits 0x%02X, param %d",
                             p_cccd->conn_handle,
                             p_cccd->service_id,
                             p_cccd->char_uuid,
                             p_cccd->ccc_bits,
                             p_cccd->param);
        }
        break;

    default:
        break;
    }
    return cb_result;
}

void app_lea_uca_handle_pending_mcp_cmd(void *buf)
{
    bool handle = true;
    T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param = {0};
    T_APP_LE_LINK *p_link;
    T_MCP_CLIENT_DIS_DONE *p_dis_done = (T_MCP_CLIENT_DIS_DONE *)buf;

    p_link = app_link_find_le_link_by_conn_handle(p_dis_done->conn_handle);
    if (p_link == NULL)
    {
        return;
    }

    if (p_link->pending_mcp_cmd != 0)
    {
        if ((p_link->media_state == MCS_MEDIA_STATE_PLAYING &&
             p_link->pending_mcp_cmd == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY) ||
            (p_link->media_state == MCS_MEDIA_STATE_PAUSED &&
             p_link->pending_mcp_cmd == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE))
        {
            APP_PRINT_TRACE1("app_lea_uca_handle_pending_mcp_cmd: drop %x", p_link->pending_mcp_cmd);
        }
        else
        {
            param.opcode = p_link->pending_mcp_cmd;
            mcp_client_write_media_cp(p_link->conn_handle, 0, p_link->gmcs, &param, true);
        }
    }
    p_link->pending_mcp_cmd = 0;
}

static uint16_t app_lea_uca_handle_ascs_msg(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    uint8_t i = 0;
    bool handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_ASCS_ASE_STATE:
        {
            T_ASCS_ASE_STATE *p_data = (T_ASCS_ASE_STATE *)buf;

            APP_PRINT_TRACE2("app_lea_uca_handle_ascs_msg: LE_AUDIO_MSG_ASCS_ASE_STATE ase_id %d, ase_state %d",
                             p_data->ase_data.ase_id,
                             p_data->ase_data.ase_state);
            switch (p_data->ase_data.ase_state)
            {
            case ASE_STATE_CODEC_CONFIGURED:
                {
                    T_LEA_ASE_ENTRY *p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_data->conn_handle,
                                                                               (void *)(&p_data->ase_data.ase_id));
                    if (p_ase_entry != NULL)
                    {
                        if (p_data->ase_data.direction == SERVER_AUDIO_SINK)
                        {
                            p_ase_entry->path_direction = DATA_PATH_OUTPUT_FLAG;
                        }
                        else
                        {
                            p_ase_entry->path_direction = DATA_PATH_INPUT_FLAG;
                        }
                        app_lea_uca_link_sm(p_data->conn_handle, LEA_CODEC_CONFIGURED, p_ase_entry);
                    }
                }
                break;

            case ASE_STATE_ENABLING:
                {
                    app_lea_uca_link_sm(p_data->conn_handle, LEA_ENABLEING, (void *)&p_data->ase_data.ase_id);
                }
                break;

            case ASE_STATE_STREAMING:
                {
                    APP_PRINT_TRACE4("app_lea_uca_handle_ascs_msg: ASE_STATE_STREAMING cig_id %d, cis_id %d, metadata_length %d, metadata %b",
                                     p_data->ase_data.param.streaming.cig_id,
                                     p_data->ase_data.param.streaming.cis_id,
                                     p_data->ase_data.param.streaming.metadata_length,
                                     TRACE_BINARY(p_data->ase_data.param.streaming.metadata_length,
                                                  p_data->ase_data.param.streaming.p_metadata));
                    app_lea_uca_link_sm(p_data->conn_handle, LEA_STREAMING, NULL);
                }
                break;

            case ASE_STATE_DISABLING:
                {
                    APP_PRINT_TRACE4("app_lea_uca_handle_ascs_msg: ASE_STATE_DISABLING cig_id %d, cis_id %d, metadata_length %d, metadata %b",
                                     p_data->ase_data.param.disabling.cig_id,
                                     p_data->ase_data.param.disabling.cis_id,
                                     p_data->ase_data.param.disabling.metadata_length,
                                     TRACE_BINARY(p_data->ase_data.param.disabling.metadata_length,
                                                  p_data->ase_data.param.disabling.p_metadata));
                    // ascs_action_rec_stop_ready(p_data->conn_handle, ASCS_TEST_ASE_ID);
                }
                break;

            case ASE_STATE_RELEASING:
                {
                    app_lea_uca_link_sm(p_data->conn_handle, LEA_RELEASING, p_data);
                }
                break;

            default:
                break;
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CP_ENABLE_IND:
        {
            T_ASCS_CP_ENABLE_IND *p_data = (T_ASCS_CP_ENABLE_IND *)buf;
            bool need_return = false;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_data->conn_handle);

#if GFPS_SASS_LEA_SUPPORT
            if (p_link)
            {
                T_MULTILINK_SASS_ACTON action = MULTILINK_SASS_LEA_MEDIA_PREEM;
                T_LEA_ASE_ENTRY *p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_data->conn_handle,
                                                                           (void *)(&p_data->param[0].ase_id));

                if ((app_sass_policy_context_to_conn_state(p_link) == SASS_LEA_CALL) ||
                    (p_ase_entry && (p_ase_entry->audio_context & AUDIO_CONTEXT_SOUND_EFFECTS)))
                {
                    action = MULTILINK_SASS_LEA_CALL_PREEM;
                }
                //run preem if the sass switch policy is different with original
                uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_LE_LINK, action, false);

                if (sass_preempt >> 8)
                {
                    if ((T_SASS_PREEM)sass_preempt == SASS_PREEM_REJECT)
                    {
                        cb_result = ASE_CP_RESP_INSUFFICIENT_RESOURCE;
                    }
                    app_multi_preemptive_judge(p_link->id, SASS_LE_LINK, action);
                    break;
                }
            }
#endif

            if (mtc_if_fm_lcis_handle(LCIS_TO_MTC_ASCS_CP_ENABLE, NULL, &need_return) == MTC_RESULT_SUCCESS)
            {
                if (need_return)
                {
                    cb_result = ASE_CP_RESP_INSUFFICIENT_RESOURCE;
                    break;
                }
            }

            if (p_link != NULL)
            {
                if (app_lea_ccp_get_call_status() != APP_CALL_IDLE && p_link->call_status == APP_CALL_IDLE)
                {
                    if ((p_link->media_state == MCS_MEDIA_STATE_PLAYING) &&
                        (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
                    {
                        T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                        param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                        mcp_client_write_media_cp(p_link->conn_handle, 0, p_link->gmcs, &param, true);
                    }
                    cb_result = ASE_CP_RESP_INSUFFICIENT_RESOURCE;
                    break;
                }
            }

            app_lea_uca_link_sm(p_data->conn_handle, LEA_ENABLE, p_data);
        }
        break;

    case LE_AUDIO_MSG_ASCS_CP_DISABLE_IND:
        {
            T_ASCS_CP_DISABLE_IND *p_data = (T_ASCS_CP_DISABLE_IND *)buf;

            app_lea_uca_link_sm(p_data->conn_handle, LEA_PAUSE, p_data);
        }
        break;

    case LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND:
        {
            T_ASCS_CP_UPDATE_METADATA_IND *p_data = (T_ASCS_CP_UPDATE_METADATA_IND *)buf;

            app_lea_uca_link_sm(p_data->conn_handle, LEA_METADATA_UPDATE, p_data);
        }
        break;

    case LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH:
        {
            T_ASCS_SETUP_DATA_PATH *p_data = (T_ASCS_SETUP_DATA_PATH *)buf;

            app_lea_uca_link_sm(p_data->conn_handle, LEA_SETUP_DATA_PATH, p_data);
        }
        break;

    case LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH:
        {
            T_ASCS_REMOVE_DATA_PATH *p_data = (T_ASCS_REMOVE_DATA_PATH *)buf;

            app_lea_uca_link_sm(p_data->conn_handle, LEA_REMOVE_DATA_PATH, p_data);
        }
        break;

#if F_APP_FRAUNHOFER_SUPPORT
    case LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND:
        {
            T_LEA_ASE_ENTRY *p_ase_entry = NULL;
            T_ASCS_CP_CONFIG_CODEC_IND *p_data = (T_ASCS_CP_CONFIG_CODEC_IND *)buf;

            for (uint8_t i = 0; i < p_data->number_of_ase; i++)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_data->conn_handle,
                                                          (void *)(&p_data->param[i].data.ase_id));
                if (p_ase_entry != NULL)
                {
                    uint16_t idx = 0;
                    uint16_t length;
                    uint8_t type;

                    for (; idx < p_data->param[i].data.codec_spec_cfg_len;)
                    {
                        length = p_data->param[i].p_codec_spec_cfg[idx];
                        idx++;
                        type = p_data->param[i].p_codec_spec_cfg[idx];

                        if (type == FRAUNHOFER_CFG_FD && length == 2)
                        {
                            uint8_t frame_duration = p_data->param[i].p_codec_spec_cfg[idx + 1];
                            p_ase_entry->codec_cfg.frame_duration = frame_duration;
                            break;
                        }
                        idx += length;
                    }
                }
            }
        }
        break;
#endif

    case LE_AUDIO_MSG_ASCS_GET_PREFER_QOS:
        {
            T_ASCS_GET_PREFER_QOS *p_data = (T_ASCS_GET_PREFER_QOS *)buf;

            app_lea_uca_link_sm(p_data->conn_handle, LEA_PREFER_QOS, p_data);
        }
        break;

    case LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND:
        {
            T_ASCS_CIS_REQUEST_IND *p_data = (T_ASCS_CIS_REQUEST_IND *)buf;
            bool need_return = false;

            APP_PRINT_TRACE6("app_lea_uca_handle_ascs_msg: LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND \
conn_handle 0x%02X, cis_conn_handle 0x%02X, snk_ase_id %d, snk_ase_state %d, src_ase_id %d, src_ase_state %d",
                             p_data->conn_handle, p_data->cis_conn_handle,
                             p_data->snk_ase_id, p_data->snk_ase_state,
                             p_data->src_ase_id, p_data->src_ase_state);

            if (mtc_if_fm_lcis_handle(LCIS_TO_MTC_ASCS_CP_ENABLE, NULL, &need_return) == MTC_RESULT_SUCCESS)
            {
                if (need_return)
                {
                    cb_result = ASE_CP_RESP_INSUFFICIENT_RESOURCE;
                    break;
                }
            }

            if (app_lea_mcp_get_active_conn_handle() == p_data->conn_handle ||
                app_lea_ccp_get_active_conn_handle() == p_data->conn_handle)
            {
                app_lea_uca_set_stream_link(app_link_find_le_link_by_conn_handle(p_data->conn_handle));
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND:
        {
            T_ASCS_AUDIO_CONTEXTS_CHECK_IND *p_data = (T_ASCS_AUDIO_CONTEXTS_CHECK_IND *)buf;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_data->conn_handle);

            APP_PRINT_TRACE4("app_lea_uca_handle_ascs_msg: LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND \
update 0x%02X, ase_id %d, direction %d, context 0x%02X",
                             p_data->is_update_metadata,
                             p_data->ase_id,
                             p_data->direction,
                             p_data->available_contexts);

            if (p_link)
            {
                uint16_t available_contexts;

                if (p_data->direction == SERVER_AUDIO_SINK)
                {
                    available_contexts = p_link->sink_available_contexts;
                }
                else
                {
                    available_contexts = p_link->source_available_contexts;
                }

                if ((available_contexts & p_data->available_contexts) == 0)
                {
                    cb_result = BLE_AUDIO_CB_RESULT_REJECT;
                }
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO:
        {
            T_ASCS_CIS_DISCONN_INFO *p_data = (T_ASCS_CIS_DISCONN_INFO *)buf;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_data->conn_handle);

            if (p_link)
            {
                T_LEA_ASE_ENTRY *p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CIS_CONN, p_data->conn_handle,
                                                                           (void *)(&p_data->cis_conn_handle));

                if (p_ase_entry)
                {
                    if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
                    {
                        p_link->stream_channel_allocation &= ~p_ase_entry->codec_cfg.audio_channel_allocation;
                    }
                }
                app_lea_uca_track_reconfig_write_length(p_link);

#if GFPS_SASS_LEA_SUPPORT
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    app_gfps_notify_conn_status();
                }
#endif
            }
        }
        break;

    case LE_AUDIO_MSG_ASCS_CIS_CONN_INFO:
        {
            T_ASCS_CIS_CONN_INFO *p_data = (T_ASCS_CIS_CONN_INFO *)buf;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_data->conn_handle);

            if (p_link)
            {
                app_lea_uca_track_reconfig_write_length(p_link);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_uca_handle_ascs_msg: msg 0x%04X", msg);
    }

    return cb_result;
}

static uint16_t app_lea_uca_handle_mcp_msg(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    bool  handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_MCP_CLIENT_DIS_DONE:
        {
            T_MCP_CLIENT_DIS_DONE *p_dis_done = (T_MCP_CLIENT_DIS_DONE *)buf;

            if (p_dis_done->is_found)
            {
                if (p_dis_done->gmcs)
                {
                    app_lea_uca_handle_pending_mcp_cmd(buf);
#if GFPS_SASS_LEA_SUPPORT
                    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(p_dis_done->conn_handle);
                    if (p_link)
                    {
                        app_sass_handle_le_dm(p_link->conn_id, SASS_LE_DSICOV_DONE, NULL);
                    }
#endif
                }
            }
        }
        break;

    case LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT:
        {
            T_APP_LE_LINK *p_link;
            T_MCP_CLIENT_READ_RESULT *p_read_result = (T_MCP_CLIENT_READ_RESULT *)buf;

            p_link = app_link_find_le_link_by_conn_handle(p_read_result->conn_handle);
            if (p_link == NULL)
            {
                return BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            if (p_read_result->cause == GAP_SUCCESS)
            {
                if (p_read_result->char_uuid == MCS_UUID_CHAR_MEDIA_STATE)
                {
                    if (p_read_result->data.media_state != MCS_MEDIA_STATE_INACTIVE)
                    {
                        app_lea_mgr_update_current_mcp_state(p_link, p_link->pre_media_state);
                    }
                }
            }
        }
        break;

    case LE_AUDIO_MSG_MCP_CLIENT_NOTIFY:
        {
            T_APP_LE_LINK *p_link;
            T_MCP_CLIENT_NOTIFY *p_notify_result = (T_MCP_CLIENT_NOTIFY *)buf;

            p_link = app_link_find_le_link_by_conn_handle(p_notify_result->conn_handle);
            if (p_link == NULL)
            {
                return BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            switch (p_notify_result->char_uuid)
            {
            case MCS_UUID_CHAR_MEDIA_STATE:
                {
                    app_lea_mgr_update_current_mcp_state(p_link, p_link->pre_media_state);
                    app_lea_uca_link_sm(p_link->conn_handle, LEA_MCP_STATE, NULL);
                }
                break;

            default:
                break;
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_uca_handle_mcp_msg: msg 0x%04X", msg);
    }

    return cb_result;
}

static uint16_t app_lea_uca_handle_ccp_msg(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    bool handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_CCP_CLIENT_READ_RESULT:
        {
            T_APP_LE_LINK *p_link;
            bool need_return = false;
            T_CCP_CLIENT_READ_RESULT *p_read_result = (T_CCP_CLIENT_READ_RESULT *)buf;

            if (mtc_if_fm_lcis_handle(LCIS_TO_MTC_ASCS_CP_ENABLE, NULL, &need_return) == MTC_RESULT_SUCCESS)
            {
                if (need_return)
                {
                    cb_result = BLE_AUDIO_CB_RESULT_REJECT;
                    break;
                }
            }

            p_link = app_link_find_le_link_by_conn_handle(p_read_result->conn_handle);
            if (p_link == NULL)
            {
                return BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            if (p_read_result->cause == GAP_SUCCESS)
            {
                if (p_read_result->char_uuid == TBS_UUID_CHAR_CALL_STATE)
                {
                    if (p_read_result->data.call_state.call_state_len >= CCP_CALL_STATE_CHARA_LEN)
                    {
                        app_lea_uca_link_sm(p_link->conn_handle, LEA_CCP_READ_RESULT, p_read_result);
                    }
                }
            }
        }
        break;

    case LE_AUDIO_MSG_CCP_CLIENT_NOTIFY:
        {
            T_APP_LE_LINK *p_link;
            bool need_return = false;
            T_CCP_CLIENT_NOTIFY *p_notify_data = (T_CCP_CLIENT_NOTIFY *)buf;

            if (mtc_if_fm_lcis_handle(LCIS_TO_MTC_ASCS_CP_ENABLE, NULL, &need_return) == MTC_RESULT_SUCCESS)
            {
                if (need_return)
                {
                    cb_result = BLE_AUDIO_CB_RESULT_REJECT;
                    break;
                }
            }

            p_link = app_link_find_le_link_by_conn_handle(p_notify_data->conn_handle);
            if (p_link == NULL)
            {
                return BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            APP_PRINT_TRACE1("app_lea_uca_handle_ccp_msg: LE_AUDIO_MSG_CCP_CLIENT_NOTIFY uuid 0x%04X",
                             p_notify_data->char_uuid);

            switch (p_notify_data->char_uuid)
            {
            case TBS_UUID_CHAR_CALL_STATE:
            case TBS_UUID_CHAR_INCOMING_CALL:
                {
                    if (((p_notify_data->char_uuid == TBS_UUID_CHAR_CALL_STATE) &&
                         (p_notify_data->data.call_state.call_state_len >= CCP_CALL_STATE_CHARA_LEN)) ||
                        ((p_notify_data->char_uuid == TBS_UUID_CHAR_INCOMING_CALL) &&
                         (p_notify_data->data.incoming_call.uri_len)))
                    {
                        app_lea_uca_link_sm(p_link->conn_handle, LEA_CCP_CALL_STATE, p_notify_data);
                    }
                }
                break;

            case TBS_UUID_CHAR_TERMINATION_REASON:
                {
                    app_lea_uca_link_sm(p_link->conn_handle, LEA_CCP_TERM_NOTIFY, p_notify_data);
                }
                break;
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_uca_handle_ccp_msg: msg 0x%04X", msg);
    }

    return cb_result;
}

static uint16_t app_lea_uca_ble_audio_cback(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    uint16_t msg_group;

    msg_group = msg & 0xFF00;
    switch (msg_group)
    {
#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    case LE_AUDIO_MSG_GROUP_SERVER:
        cb_result = app_lea_uca_handle_cccd_msg(msg, buf);
        break;

    case LE_AUDIO_MSG_GROUP_ASCS:
        cb_result = app_lea_uca_handle_ascs_msg(msg, buf);
        break;
#endif
#if F_APP_MCP_SUPPORT
    case LE_AUDIO_MSG_GROUP_MCP_CLIENT:
        cb_result = app_lea_uca_handle_mcp_msg(msg, buf);
        break;
#endif
#if F_APP_CCP_SUPPORT
    case LE_AUDIO_MSG_GROUP_CCP_CLIENT:
        cb_result = app_lea_uca_handle_ccp_msg(msg, buf);
        break;
#endif
    default:
        break;
    }

    return cb_result;
}

T_AUDIO_EFFECT_INSTANCE app_lea_uca_get_eq_instance(void)
{
    return app_lea_uca_eq;
}

T_AUDIO_EFFECT_INSTANCE *app_lea_uca_p_eq_instance(void)
{
    return &app_lea_uca_eq;
}

bool *app_lea_uca_get_eq_abled(void)
{
    return &app_lea_uca_eq_enabled;
}

bool app_lea_uca_get_diff_path(void)
{
    return app_lea_uca_diff_path;
}

#if F_APP_ERWS_SUPPORT
bool app_lea_uca_sync_cis_streaming_state(bool is_streaming)
{
    APP_PRINT_TRACE1("app_lea_uca_sync_cis_streaming_state: %d", is_streaming);

    return app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LEA,
                                               LEA_REMOTE_SEC_SYNC_CIS_STREAMING,
                                               (uint8_t *)&is_streaming, 1);
}

static void app_lea_uca_sync_mute_state(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LEA,
                                            LEA_REMOTE_SYNC_PRI_MIC_STATE,
                                            (uint8_t *)&app_lea_uca_mic_mute_state, 1);
    }
}

static void app_lea_uca_check_cis_enable_b2b_sniff_mode(void)
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            if (!app_db.remote_cis_link_is_streaming)
            {
                app_sniff_mode_b2b_enable(app_cfg_nv.bud_peer_addr, SNIFF_DISABLE_MASK_LEA);
            }
        }
        else
        {
            app_lea_uca_sync_cis_streaming_state(false);
        }
    }
}
#endif

static void app_lea_uca_dump_ase_info(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        T_LEA_ASE_ENTRY *p_ase_entry = &p_link->lea_ase_entry[i];

        APP_PRINT_TRACE8("app_lea_uca_dump_ase_info: used %d, conn_handle 0x%02X, ase_id %d, \
context 0x%02X, cis_conn_handle 0x%02X, presentation_delay 0x%02X, track_handle 0x%02X, frame_num %d",
                         p_ase_entry->used,
                         p_ase_entry->conn_handle,
                         p_ase_entry->ase_id,
                         p_ase_entry->audio_context,
                         p_ase_entry->cis_conn_handle,
                         p_ase_entry->presentation_delay,
                         p_ase_entry->track_handle,
                         p_ase_entry->frame_num);
    }
}

void app_lea_uca_eq_release(T_AUDIO_EFFECT_INSTANCE eq_instance)
{
    if (eq_instance != NULL)
    {
        eq_release(eq_instance);
        eq_instance = NULL;
    }
}

void app_lea_uca_playback_eq_setting(T_AUDIO_EFFECT_INSTANCE *eq_instance,  bool *audio_eq_enabled,
                                     T_AUDIO_TRACK_HANDLE audio_track_handle)
{
    app_eq_idx_check_accord_mode();
    *eq_instance = app_eq_create(EQ_CONTENT_TYPE_AUDIO, EQ_STREAM_TYPE_AUDIO, SPK_SW_EQ,
                                 app_db.spk_eq_mode,
                                 app_cfg_nv.eq_idx);
    *audio_eq_enabled = false;

    if (*eq_instance != NULL)
    {
        //not set default eq when audio connect,enable when set eq para from SRC
        if (!app_db.eq_ctrl_by_src)
        {
            app_eq_audio_eq_enable(eq_instance, audio_eq_enabled);
        }
#if (F_APP_USER_EQ_SUPPORT == 1)
        else if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
        {
            app_eq_audio_eq_enable(eq_instance, audio_eq_enabled);
        }
#endif

        audio_track_effect_attach(audio_track_handle, *eq_instance);
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
#if (F_APP_USER_EQ_SUPPORT == 1)
        if (app_eq_is_valid_user_eq_index(SPK_SW_EQ, app_db.spk_eq_mode, app_cfg_nv.eq_idx))
        {
            app_report_eq_idx(EQ_INDEX_REPORT_BY_GET_UNSAVED_EQ);
        }
        else
#endif
        {
            app_report_eq_idx(EQ_INDEX_REPORT_BY_PLAYING);
        }
    }
}

static void app_lea_uca_track_sync_reset(T_APP_LE_LINK *p_link, T_AUDIO_TRACK_HANDLE track_handle)
{
    if (p_link)
    {
        T_AUDIO_STREAM_TYPE stream_type;

        if (audio_track_stream_type_get(track_handle, &stream_type))
        {
            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK || stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                p_link->lea_ready_to_downstream = 0;
            }
        }
    }
}

static bool app_lea_uca_free_link(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        T_LEA_ASE_ENTRY *p_ase_entry = &p_link->lea_ase_entry[i];

        if (p_ase_entry->used == true)
        {
            if (p_ase_entry->track_handle != NULL)
            {
                syncclk_drv_timer_stop(LEA_SYNC_CLK_REF);
                ascs_action_release(p_ase_entry->conn_handle, p_ase_entry->ase_id);
                app_lea_uca_track_sync_reset(p_link, p_ase_entry->track_handle);
                audio_track_release(p_ase_entry->track_handle);
            }

            //Detach audio effect
            if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                app_lea_uca_eq_release(&app_lea_uca_eq);
            }
            else //AUDIO_STREAM_TYPE_VOICE, AUDIO_STREAM_TYPE_RECORD
            {
                app_sidetone_detach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
                app_nrec_detach(p_ase_entry->track_handle, p_ase_entry->nrec_instance);
            }

            memset(p_ase_entry, 0, sizeof(T_LEA_ASE_ENTRY));
        }
    }

    memset(p_link->lea_call_entry, 0, sizeof(T_LEA_CALL_ENTRY) * CCP_CALL_ENTRY_NUM);
    p_link->call_status = APP_CALL_IDLE;
    if (p_link->active_call_uri != NULL)
    {
        free(p_link->active_call_uri);
        p_link->active_call_uri = NULL;
    }

    return true;
}

static void app_lea_uca_clear_voice_track(T_APP_LE_LINK *p_link,
                                          T_AUDIO_TRACK_HANDLE track_handle)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        if (p_link->lea_ase_entry[i].used == true)
        {
            if (p_link->lea_ase_entry[i].track_handle == track_handle)
            {
                p_link->lea_ase_entry[i].track_handle = NULL;
            }
        }
    }
}

static bool app_lea_uca_is_voice_track_existed(T_APP_LE_LINK *p_link,
                                               T_LEA_ASE_ENTRY *p_ase_entry)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        if ((p_link->lea_ase_entry[i].used == true) && (&p_link->lea_ase_entry[i] != p_ase_entry))
        {
            if (p_link->lea_ase_entry[i].path_direction != p_ase_entry->path_direction)
            {
                if (p_link->lea_ase_entry[i].track_handle)
                {
                    p_ase_entry->track_handle = p_link->lea_ase_entry[i].track_handle;
                    p_ase_entry->stream_type = AUDIO_STREAM_TYPE_VOICE;
                    p_ase_entry->track_write_len = p_link->lea_ase_entry[i].track_write_len;
                    p_ase_entry->frame_num = p_link->lea_ase_entry[i].frame_num;
                    p_link->cis_right_ch_conn_handle = 0x0;
                    return true;
                }
            }
        }
    }
    return false;
}

static bool app_lea_uca_mapping_voice_track(T_APP_LE_LINK *p_link, T_LEA_ASE_ENTRY *p_ase_entry)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        if ((p_link->lea_ase_entry[i].used == true) && (&p_link->lea_ase_entry[i] != p_ase_entry))
        {
            if (p_link->lea_ase_entry[i].path_direction != p_ase_entry->path_direction)
            {
                if (p_link->lea_ase_entry[i].cis_conn_handle == p_ase_entry->cis_conn_handle)
                {
                    p_link->lea_ase_entry[i].track_handle = p_ase_entry->track_handle;
                    p_link->lea_ase_entry[i].stream_type = AUDIO_STREAM_TYPE_VOICE;
                    p_link->lea_ase_entry[i].track_write_len = p_ase_entry->track_write_len;
                    p_link->lea_ase_entry[i].frame_num = p_ase_entry->frame_num;
                    p_link->cis_right_ch_conn_handle = 0x0;
                    return true;
                }
            }
        }
    }
    return false;
}

static bool app_lea_uca_is_input_path_enabled(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        T_LEA_ASE_ENTRY *p_ase_entry = &p_link->lea_ase_entry[i];

        if (p_ase_entry->used == true &&
            p_ase_entry->control_point_enable == true &&
            p_ase_entry->path_direction == DATA_PATH_INPUT_FLAG)
        {
            return true;
        }
    }
    return false;
}

bool app_lea_uca_is_upstreaming_enabled(void)
{
    for (uint8_t idx = 0; idx < MAX_BLE_LINK_NUM; idx++)
    {
        if (app_db.le_link[idx].used &&
            app_db.le_link[idx].lea_link_state != LEA_LINK_IDLE)
        {
            if (app_lea_uca_is_input_path_enabled(&app_db.le_link[idx]))
            {
                return true;
            }
        }
    }

    return false;
}

static bool app_lea_uca_clear_playback_track_handle(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        T_LEA_ASE_ENTRY *p_ase_entry = &p_link->lea_ase_entry[i];

        if (p_ase_entry->used == true &&
            p_ase_entry->control_point_enable == true &&
            p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
        {
            if (p_ase_entry->track_handle)
            {
                audio_track_release(p_ase_entry->track_handle);
                p_ase_entry->track_handle = NULL;
                p_ase_entry->track_write_len = 0;
                return true;
            }
        }
    }
    return false;
}

static bool app_lea_uca_is_output_track_existed(T_APP_LE_LINK *p_link,
                                                T_LEA_ASE_ENTRY *p_ase_entry)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        if ((p_link->lea_ase_entry[i].used == true) && (&p_link->lea_ase_entry[i] != p_ase_entry))
        {
            if (p_link->lea_ase_entry[i].path_direction == DATA_PATH_OUTPUT_FLAG)
            {
                if (p_link->lea_ase_entry[i].track_handle)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

static void app_lea_uca_dump_call_info(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < CCP_CALL_ENTRY_NUM; i++)
    {
        T_LEA_CALL_ENTRY *p_call_entry = &p_link->lea_call_entry[i];

        APP_PRINT_TRACE3("app_lea_uca_dump_call_info: used %d, call_index %d, call_state %d",
                         p_call_entry->used, p_call_entry->call_index, p_call_entry->call_state);
    }

    APP_PRINT_TRACE3("app_lea_uca_dump_call_info: active_call_index %d, call_status %d, ccp_call_status %d",
                     p_link->active_call_index, p_link->call_status, app_lea_ccp_get_call_status());
}

static void app_lea_uca_stop_inactive_stream(T_APP_LE_LINK *p_link)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            T_APP_LE_LINK *p_temp_link = &app_db.le_link[i];
            if (p_temp_link != p_link)
            {
                if (p_temp_link->used == true)
                {
                    if ((p_temp_link->lea_link_state == LEA_LINK_STREAMING) &&
                        (p_temp_link->media_state == MCS_MEDIA_STATE_PLAYING))
                    {
                        T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                        param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                        mcp_client_write_media_cp(p_temp_link->conn_handle, 0, p_temp_link->gmcs, &param, true);

                    }
                }
            }
        }
    }

    APP_PRINT_TRACE1("app_lea_uca_stop_inactive_stream: active conn_handle 0x%02X",
                     p_link->conn_handle);
}

bool app_lea_uca_check_enable_mic(void)
{
    if (app_cfg_const.upstresm_mode)//single mic
    {
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            if (LIGHT_SENSOR_ENABLED)
            {
                if (app_db.local_loc == BUD_LOC_IN_EAR)
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        else
        {
            if (LIGHT_SENSOR_ENABLED)
            {
                if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                    (app_db.remote_loc != BUD_LOC_IN_EAR) &&
                    (app_db.local_loc == BUD_LOC_IN_EAR))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if (!LIGHT_SENSOR_ENABLED || (app_db.local_loc == BUD_LOC_IN_EAR))
        {
            return true;
        }
    }
    APP_PRINT_INFO1("app_lea_uca_check_enable_mic %x", app_cfg_const.upstresm_mode);
    return false;
}

static bool app_lea_uca_record_read_cb(T_AUDIO_TRACK_HANDLE   handle,
                                       uint32_t              *timestamp,
                                       uint16_t              *seq_num,
                                       T_AUDIO_STREAM_STATUS *status,
                                       uint8_t               *frame_num,
                                       void                  *buf,
                                       uint16_t               required_len,
                                       uint16_t              *actual_len)
{
    uint16_t conn_handle = 0;
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;

#if F_APP_CCP_SUPPORT
    conn_handle = app_lea_ccp_get_active_conn_handle();
#endif
    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_TRACK, conn_handle,  handle);
    if (p_ase_entry != NULL)
    {
#if F_APP_LEA_ALWAYS_CONVERSATION
        if (app_lea_uca_check_enable_mic() == false)
        {
            app_lea_uca_set_s2m_zero_packet(p_ase_entry->cis_conn_handle, 1);
            goto fail_sec_disable_mic;
        }

        if (p_ase_entry->audio_context == AUDIO_CONTEXT_CONVERSATIONAL)
        {
            //real conversation
            app_lea_uca_set_s2m_zero_packet(p_ase_entry->cis_conn_handle, 0);
        }
#endif
        app_lea_uca_send_iso_data(buf, p_ase_entry, required_len,
                                  true, *timestamp);
    }

fail_sec_disable_mic:
fail_ignore_mic_data:
    *actual_len = required_len;
    return true;
}

static void app_lea_uca_track_reconfig_write_length(T_APP_LE_LINK *p_link)
{
    for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
    {
        T_LEA_ASE_ENTRY *p_ase_entry = &p_link->lea_ase_entry[i];

        if ((p_ase_entry->used == true) &&
            (p_ase_entry->track_handle != NULL) &&
            (p_ase_entry->stream_type ==
             AUDIO_STREAM_TYPE_PLAYBACK)) //TODO: modify condition when support voice mix 2 cis
        {
            uint8_t channel_count = 0;

            //Only handle 2 cis mix situation, no need to check AUDIO_CHANNEL_LOCATION_MONO
            channel_count = __builtin_popcount(p_link->stream_channel_allocation);
            p_ase_entry->frame_num = p_ase_entry->codec_cfg.codec_frame_blocks_per_sdu * channel_count;
            p_ase_entry->track_write_len = p_ase_entry->codec_cfg.octets_per_codec_frame *
                                           p_ase_entry->frame_num;
        }
    }
}

static void app_lea_uca_track_set_format_info(T_APP_LE_LINK *p_link, T_LEA_ASE_ENTRY *p_ase_entry,
                                              T_AUDIO_FORMAT_INFO *format_info)
{
    uint32_t sample_rate;
    uint32_t chann_location;
    T_CODEC_CFG *p_codec = &p_ase_entry->codec_cfg;

    sample_rate = app_lea_sample_rate_freq_map[p_codec->sample_frequency - 1];

    if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
    {
        chann_location = p_link->stream_channel_allocation;
    }
    else
    {
        chann_location = p_codec->audio_channel_allocation;
    }

    format_info->frame_num = FIXED_FRAME_NUM;
#if F_APP_LC3_PLUS_SUPPORT
    if (p_ase_entry->codec_type == AUDIO_FORMAT_TYPE_LC3PLUS)
    {
        format_info->type = AUDIO_FORMAT_TYPE_LC3PLUS;
        format_info->attr.lc3plus.sample_rate = sample_rate;
        format_info->attr.lc3plus.bit_width = p_ase_entry->bit_depth;
        format_info->attr.lc3plus.chann_location = chann_location;
        format_info->attr.lc3plus.frame_length = p_codec->octets_per_codec_frame;
        format_info->attr.lc3plus.presentation_delay = p_ase_entry->presentation_delay;

#if F_APP_FRAUNHOFER_SUPPORT
        if (p_codec->frame_duration == FRAUNHOFER_CFG_FD_10_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_10_MS;
        }
        else if (p_codec->frame_duration == FRAUNHOFER_CFG_FD_7_5_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_7_5_MS;
        }
        else if (p_codec->frame_duration == FRAUNHOFER_CFG_FD_5_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_5_MS;
        }
        else
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_2_5_MS;
        }
#else
        if (p_codec->frame_duration == FRAME_DURATION_CFG_10_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_10_MS;
        }
        else if (p_codec->frame_duration == FRAME_DURATION_CFG_7_5_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_7_5_MS;
        }
        else if (p_codec->frame_duration == FRAME_DURATION_CFG_5_MS)
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_5_MS;
        }
        else
        {
            format_info->attr.lc3plus.frame_duration = AUDIO_LC3PLUS_FRAME_DURATION_2_5_MS;
        }
#endif
    }
    else
#endif
    {
        format_info->type = AUDIO_FORMAT_TYPE_LC3;
        format_info->attr.lc3.sample_rate = sample_rate;
        format_info->attr.lc3.chann_location = chann_location;
        format_info->attr.lc3.frame_length = p_codec->octets_per_codec_frame;
        format_info->attr.lc3.frame_duration = (T_AUDIO_LC3_FRAME_DURATION)p_codec->frame_duration;
        format_info->attr.lc3.presentation_delay = p_ase_entry->presentation_delay;
    }

    APP_PRINT_TRACE8("app_lea_uca_track_set_format_info: ase_id 0x%02X, frame_duration 0x%02X, sample_frequency 0x%02X, \
octets_per_codec_frame %d, codec_frame_blocks_per_sdu %d, audio_channel_allocation %d, stream_type %d, bit_depth %d",
                     p_ase_entry->ase_id,
                     p_codec->frame_duration,
                     p_codec->sample_frequency,
                     p_codec->octets_per_codec_frame,
                     p_codec->codec_frame_blocks_per_sdu,
                     p_codec->audio_channel_allocation,
                     p_ase_entry->stream_type,
                     p_ase_entry->bit_depth);
}

static void app_lea_uca_track_create(T_APP_LE_LINK *p_link, T_LEA_ASE_ENTRY *p_ase_entry)
{
    bool clear_playback_track = false;
    uint8_t volume_out = app_lea_vol_gain_get();
    uint8_t volume_in = 0;
    uint8_t channel_count = 0;
    uint32_t device = AUDIO_DEVICE_OUT_SPK;
    P_AUDIO_TRACK_ASYNC_IO async_read = NULL;
    T_AUDIO_FORMAT_INFO format_info = {};
    uint32_t audio_channel_allocation = 0;
    uint8_t result = 0;

    app_lea_mgr_set_media_state(ISOCH_DATA_PKT_STATUS_LOST_DATA);

    if (p_ase_entry == NULL || p_link == NULL)
    {
        result = 1;
        goto fail_create_track;
    }

    if ((p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG) &&
        (app_lea_uca_is_output_track_existed(p_link, p_ase_entry) == true))
    {
        result = 2;
        goto fail_create_track;
    }

    audio_channel_allocation = p_ase_entry->codec_cfg.audio_channel_allocation;

    if (app_lea_uca_diff_path)
    {
        if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
        {
            p_ase_entry->stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
            device = AUDIO_DEVICE_OUT_SPK;
            audio_channel_allocation = p_link->stream_channel_allocation;
        }
        else if (p_ase_entry->path_direction == DATA_PATH_INPUT_FLAG)
        {
            p_ase_entry->stream_type = AUDIO_STREAM_TYPE_RECORD;
            volume_in = app_dsp_cfg_vol.record_volume_default;
            device = AUDIO_DEVICE_IN_MIC;
            async_read = app_lea_uca_record_read_cb;
        }
    }
    else
    {
        bool create_voice_track = false;

        if (app_lea_uca_is_input_path_enabled(p_link))
        {
            clear_playback_track = app_lea_uca_clear_playback_track_handle(p_link);

            if (app_lea_uca_is_voice_track_existed(p_link, p_ase_entry))
            {
                result = 3;
                goto fail_create_track;
            }

            create_voice_track = true;
        }

        if (create_voice_track)
        {
            p_ase_entry->stream_type = AUDIO_STREAM_TYPE_VOICE;
            volume_in = app_dsp_cfg_vol.voice_volume_in_default;
            device = AUDIO_DEVICE_OUT_SPK | AUDIO_DEVICE_IN_MIC;
            p_link->cis_right_ch_conn_handle = 0x0;
        }
        else
        {
            p_ase_entry->stream_type = AUDIO_STREAM_TYPE_PLAYBACK;
            volume_in = 0;
            device = AUDIO_DEVICE_OUT_SPK;
            audio_channel_allocation = p_link->stream_channel_allocation;
        }
    }

    channel_count = (audio_channel_allocation == AUDIO_CHANNEL_LOCATION_MONO) ?
                    1 : __builtin_popcount(audio_channel_allocation);
    p_ase_entry->frame_num = p_ase_entry->codec_cfg.codec_frame_blocks_per_sdu * channel_count;

    app_lea_uca_track_set_format_info(p_link, p_ase_entry, &format_info);
    p_ase_entry->track_handle = audio_track_create(p_ase_entry->stream_type,
                                                   AUDIO_STREAM_MODE_DIRECT,
                                                   AUDIO_STREAM_USAGE_LOCAL,
                                                   format_info,
                                                   volume_out,
                                                   volume_in,
                                                   device,
                                                   NULL,
                                                   async_read);
    if (p_ase_entry->track_handle != NULL
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
        && (!app_cfg_const.enable_24g_bt_audio_source_switch ||
            (app_cfg_const.enable_24g_bt_audio_source_switch &&
             app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE))
#endif
       )
    {
        if ((p_ase_entry->stream_type == AUDIO_STREAM_TYPE_RECORD ||
             p_ase_entry->stream_type == AUDIO_STREAM_TYPE_VOICE) && p_link->call_status == APP_CALL_IDLE)
        {
            ccp_client_read_char_value(p_link->conn_handle, 0, TBS_UUID_CHAR_CALL_STATE, p_link->gtbs);
        }

        syncclk_drv_timer_start(LEA_SYNC_CLK_REF, CONN_HANDLE_TYPE_FREERUN_CLOCK, 0xFF, 0);

        p_ase_entry->track_write_len = p_ase_entry->codec_cfg.octets_per_codec_frame *
                                       p_ase_entry->frame_num;

        if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_VOICE &&
            p_ase_entry->path_direction == DATA_PATH_INPUT_FLAG && clear_playback_track)
        {
            app_lea_uca_mapping_voice_track(p_link, p_ase_entry);
        }

        if (app_cfg_nv.lea_vol_out_mute)
        {
            audio_track_volume_out_mute(p_ase_entry->track_handle);
        }
        else
        {
            audio_track_volume_out_unmute(p_ase_entry->track_handle);
        }

#if F_APP_LEA_ALWAYS_CONVERSATION
        if ((p_ase_entry->audio_context == AUDIO_CONTEXT_UNSPECIFIED) &&
            ((p_ase_entry->stream_type == AUDIO_STREAM_TYPE_RECORD) ||
             (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_VOICE)))
        {
            //fake conversation
            app_lea_uca_set_s2m_zero_packet(p_ase_entry->cis_conn_handle, 1);
        }
        else
#endif
        {
            audio_track_start(p_ase_entry->track_handle);
        }
    }
    else
    {
        result = 4;
        goto fail_create_track;
    }

    return;

fail_create_track:
    APP_PRINT_ERROR1("app_lea_uca_track_create: fail cause %d",  -result);
}

static void app_lea_uca_track_release(T_APP_LE_LINK *p_link, T_LEA_ASE_ENTRY *p_ase_entry)
{
    APP_PRINT_TRACE0("app_lea_uca_track_release");

    if (p_ase_entry != NULL)
    {
        app_lea_mgr_set_media_state(ISOCH_DATA_PKT_STATUS_LOST_DATA);
        syncclk_drv_timer_stop(LEA_SYNC_CLK_REF);
        app_lea_uca_track_sync_reset(p_link, p_ase_entry->track_handle);
        audio_track_release(p_ase_entry->track_handle);
        p_ase_entry->track_handle = NULL;
        p_ase_entry->track_write_len = 0;
    }
}

void app_lea_uca_enable_mic(void)
{
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;
    T_AUDIO_TRACK_HANDLE track_handle;
    uint8_t idx;

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ||
        (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY))
    {
        APP_PRINT_TRACE0("app_lea_uca_enable_mic: not allow enable mic");
        return;
    }
#endif

    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_UP_DIRECT, NULL, &idx);
    if ((p_ase_entry != NULL && p_ase_entry->stream_type == AUDIO_STREAM_TYPE_RECORD) &&
        (app_lea_uca_mic_mute_state == false) && app_db.le_link[idx].lea_link_state == LEA_LINK_STREAMING)
    {
        app_audio_enable_play_mic_unmute_tone(false);
        audio_volume_in_unmute(AUDIO_STREAM_TYPE_RECORD);
        return;
    }

    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_DEVICE_INOUT, NULL, &idx);
    if (p_ase_entry == NULL || app_db.le_link[idx].lea_link_state != LEA_LINK_STREAMING)
    {
        APP_PRINT_TRACE0("app_lea_uca_enable_mic: ase empty");
        return;
    }

    track_handle = p_ase_entry->track_handle;
    app_lea_uca_clear_voice_track(&app_db.le_link[idx], track_handle);
    audio_track_release(track_handle);

    if (app_lea_uca_mic_mute_state == false)
    {
        app_audio_enable_play_mic_unmute_tone(false);
        audio_volume_in_unmute(AUDIO_STREAM_TYPE_VOICE);
    }
    else
    {
        app_audio_enable_play_mic_mute_tone(false);
        audio_volume_in_mute(AUDIO_STREAM_TYPE_VOICE);
    }

    app_lea_uca_track_create(&app_db.le_link[idx], p_ase_entry);
}

static void app_lea_uca_prefer_qos(void *p_data)
{
    bool ret = false;
    T_ASCS_GET_PREFER_QOS *p_info = (T_ASCS_GET_PREFER_QOS *)p_data;
    T_QOS_CFG_PREFERRED qos_data;
    T_ASCS_PREFER_QOS_DATA prefer_qos;
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;

    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_info->conn_handle,
                                              (void *)(&p_info->ase_id));
    if (p_ase_entry)
    {
        if (p_ase_entry->codec_type == AUDIO_FORMAT_TYPE_LC3)
        {
            if ((qos_preferred_cfg_get_by_codec(&p_info->codec_cfg, p_info->target_latency, &qos_data) == true)
                && (ascs_get_ase_prefer_qos(p_info->conn_handle, p_info->ase_id, &prefer_qos) == true))
            {
                ret = true;
            }
        }
#if F_APP_LC3_PLUS_SUPPORT
        else
        {
            if (app_lea_pacs_qos_preferred_cfg_get_by_codec(&p_info->codec_cfg, p_info->target_latency,
                                                            &qos_data, p_info->conn_handle, p_info->ase_id) == true)
            {
                prefer_qos.max_transport_latency = qos_data.max_transport_latency;
                prefer_qos.preferred_retrans_number = qos_data.retransmission_number;
                prefer_qos.supported_framing = qos_data.framing;
                prefer_qos.preferred_phy = p_info->target_phy;
                ret = true;
            }
        }
#endif
    }

    if (ret == true)
    {
        uint32_t permit_min_pd;

        permit_min_pd = (app_db.gaming_mode) ? CIS_PERMIT_MIN_PD_LOW_LATENCY : CIS_PERMIT_MIN_PD_NORMAL;

        if (prefer_qos.max_transport_latency > CIS_PREFER_MAX_TL)
        {
            prefer_qos.max_transport_latency = CIS_PREFER_MAX_TL;
        }

        //Presentation delay
        prefer_qos.presentation_delay_max = qos_data.presentation_delay;
        prefer_qos.presentation_delay_min = permit_min_pd;

        if (prefer_qos.presentation_delay_max < prefer_qos.presentation_delay_min)
        {
            prefer_qos.presentation_delay_min = prefer_qos.presentation_delay_max;
        }

        //Preferred presentation delay
        if (app_cfg_const.cis_pd != 0)
        {
            uint32_t preferred_pd = app_cfg_const.cis_pd * 100;

            if (preferred_pd < permit_min_pd)
            {
                preferred_pd = permit_min_pd;
            }

            prefer_qos.preferred_presentation_delay_max = preferred_pd;
            prefer_qos.preferred_presentation_delay_min = preferred_pd;
        }
        else
        {
            prefer_qos.preferred_presentation_delay_min = 0;
            prefer_qos.preferred_presentation_delay_max = 0;
        }

        if (ascs_cfg_ase_prefer_qos(p_info->conn_handle, p_info->ase_id, &prefer_qos) == true)
        {
            T_LEA_ASE_ENTRY *p_ase_entry = NULL;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_info->conn_handle,
                                                      (void *)(&p_info->ase_id));
            if (p_ase_entry != NULL)
            {
                p_ase_entry->path_direction = p_info->direction;
            }
        }
    }
}

uint8_t app_lea_uca_count_num(void)
{
    uint8_t i, j;
    uint8_t num = 0;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
        {
            T_APP_LE_LINK *p_link = &app_db.le_link[i];

            for (j = 0; j < ASCS_ASE_ENTRY_NUM; j++)
            {
                if (p_link->lea_ase_entry[j].used == true &&
                    p_link->lea_ase_entry[j].cis_conn_handle)
                {
                    num++;
                }
            }
        }
    }

    return num;
}

static void app_lea_uca_setup_data_path(T_APP_LE_LINK *p_link, void *p_data)
{
    uint8_t pair_idx;
    T_LEA_ASE_ENTRY *p_ase_entry;
    T_ASCS_SETUP_DATA_PATH *p_info = (T_ASCS_SETUP_DATA_PATH *)p_data;

    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_link->conn_handle,
                                              (void *)(&p_info->ase_id));
    if (p_ase_entry != NULL)
    {
        app_lea_uca_track_create(p_link, p_ase_entry);

        if (p_info->path_direction == DATA_PATH_INPUT_FLAG)
        {
            p_ase_entry->prequeue_pkt_cnt = 0;
            p_ase_entry->pkt_seq = 0;
        }
    }

    //Workaround: Handle Media related control if remote device not support MCP
    if ((p_link->gmcs == false) && (app_lea_ccp_get_call_status() == APP_CALL_IDLE))
    {
        app_lea_uca_set_stream_link(p_link);
        app_lea_mcp_set_active_conn_handle(p_link->conn_handle);
    }

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_BLE_AUDIO);

    app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_UCA, BP_LINK_EVENT_CONNECT,
                                  p_link->bd_addr);

    if (bt_bond_index_get(p_link->bd_addr, &pair_idx))
    {
        app_cfg_nv.last_btmode[pair_idx] = BP_LINK_LAST_BTMODE_LEA;
        APP_PRINT_INFO1("set last mode to LEA: %s", TRACE_BDADDR(p_link->bd_addr));
    }

#if GFPS_SASS_LEA_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }
#endif
}

static void app_lea_uca_remove_data_path(T_APP_LE_LINK *p_link, void *p_data)
{
    T_ASCS_REMOVE_DATA_PATH *p_info = (T_ASCS_REMOVE_DATA_PATH *)p_data;
    T_LEA_ASE_ENTRY *p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_link->conn_handle,
                                                               (void *)(&p_info->ase_id));

    app_lea_uca_track_release(p_link, p_ase_entry);

    if (p_info->path_direction == DATA_PATH_OUTPUT_FLAG)
    {
        if (p_link->cis_left_ch_conn_handle == p_info->cis_conn_handle)
        {
            app_lea_mgr_clear_iso_queue(&app_lea_uca_left_ch_queue);
            p_link->cis_left_ch_conn_handle = 0;
            p_link->cis_left_ch_iso_state = ISOCH_DATA_PKT_STATUS_LOST_DATA;
        }
        else if (p_link->cis_right_ch_conn_handle == p_info->cis_conn_handle)
        {
            app_lea_mgr_clear_iso_queue(&app_lea_uca_right_ch_queue);
            p_link->cis_right_ch_conn_handle  = 0;
            p_link->cis_right_ch_iso_state = ISOCH_DATA_PKT_STATUS_LOST_DATA;
        }
    }

    if (app_lea_uca_active_stream_link != NULL &&
        p_link->conn_handle == app_lea_uca_active_stream_link->conn_handle)
    {
        if (p_link->cis_right_ch_conn_handle == 0 && p_link->cis_left_ch_conn_handle == 0)
        {
            app_lea_uca_set_stream_link(NULL);
        }
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_lea_uca_set_mic_mute_state(false);
#if 0
        app_lea_uca_sync_mute_state();
#endif
    }

    app_link_disallow_legacy_stream(false);
    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_LEA);
#if F_APP_ERWS_SUPPORT
    app_lea_uca_check_cis_enable_b2b_sniff_mode();
#endif
    app_bt_policy_qos_param_update(app_db.br_link[app_a2dp_get_active_idx()].bd_addr,
                                   BP_TPOLL_LEA_RESTORE_A2DP_EVENT);
    mtc_topology_dm(MTC_TOPO_EVENT_CIS_TERMINATE);

    //Workaround: Handle Media related control if remote device not support MCP
    if ((p_link->gmcs == false) && (app_lea_ccp_get_call_status() == APP_CALL_IDLE))
    {
    }

    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BLE_AUDIO, app_cfg_const.timer_auto_power_off);

    app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_UCA, BP_LINK_EVENT_DISCONNECT,
                                  p_link->bd_addr);
}

static void app_lea_uca_handle_mcp_state(T_APP_LE_LINK *p_link)
{
    if (p_link->media_state == MCS_MEDIA_STATE_PLAYING)
    {
#if GFPS_SASS_LEA_SUPPORT
        //run preem if the sass switch policy is different with original
        uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_LE_LINK,
                                                        MULTILINK_SASS_LEA_MCP_PREEM, false);

        if (sass_preempt >> 8)
        {
            app_multi_preemptive_judge(p_link->id, SASS_LE_LINK, MULTILINK_SASS_LEA_MCP_PREEM);
            return;
        }
#endif
        if ((app_lea_ccp_get_call_status() != APP_CALL_IDLE) ||
            (app_hfp_get_call_status() != APP_CALL_IDLE))
        {
#if F_APP_ERWS_SUPPORT
            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                return;
            }
#endif
        }
        else
        {
            app_lea_mcp_set_active_conn_handle(p_link->conn_handle);
            if ((p_link->lea_link_state == LEA_LINK_STREAMING) &&
                (p_link->cis_left_ch_conn_handle != 0 || p_link->cis_right_ch_conn_handle != 0))
            {
                app_lea_uca_set_stream_link(p_link);
                if (mtc_get_btmode() != MULTI_PRO_BT_CIS)
                {
                    mtc_stream_switch(false);
                    mtc_topology_dm(MTC_TOPO_EVENT_MCP_PLAYING);
                }
                else
                {
                    app_lea_uca_stop_inactive_stream(p_link);
                }

                for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
                {
                    if (p_link->lea_ase_entry[i].used == true &&
                        p_link->lea_ase_entry[i].cis_conn_handle &&
                        p_link->lea_ase_entry[i].path_direction == DATA_PATH_OUTPUT_FLAG &&
                        p_link->lea_ase_entry[i].track_handle)
                    {
                        T_AUDIO_TRACK_STATE state;

                        if (audio_track_state_get(p_link->lea_ase_entry[i].track_handle, &state) &&
                            state == AUDIO_TRACK_STATE_STOPPED)
                        {
                            audio_track_restart(p_link->lea_ase_entry[i].track_handle);
                            break;
                        }
                    }
                }
            }
        }
#if GFPS_SASS_LEA_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_gfps_notify_conn_status();
        }
#endif
    }
}

static void app_lea_uca_handle_ccp_state(T_APP_LE_LINK *p_link, void *p_data)
{
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;

    mtc_stream_switch(true);
    app_lea_uca_stop_inactive_stream(p_link);

    mtc_topology_dm(MTC_TOPO_EVENT_CCP_CALL);

#if F_APP_LISTENING_MODE_SUPPORT
    app_listening_mode_call_status_update();
#endif

    //In multi-party call scenario, if active call is ended and accept the inactive call
    //Audio effect will be disabled when call terminated, app_lea_uca_handle_ccp_term,
    //Re-enable audio effect when call state change
    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_UP_DIRECT, NULL, NULL);
    if (p_ase_entry != NULL)
    {
        sidetone_enable(p_ase_entry->sidetone_instance);
    }
}

static void app_lea_uca_handle_ccp_term(T_APP_LE_LINK *p_link, void *p_data)
{
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;

    mtc_topology_dm(MTC_TOPO_EVENT_CCP_CALL);

#if F_APP_LISTENING_MODE_SUPPORT
    app_listening_mode_call_status_update();
#endif

    //IOP: Unicast client, ex: Google Pixel 8, may delay ASE release when call terminated
    //     Cause headset stay in conversation mode and not remove data path.
    //     1. Audio track is not released, and audio effect are all enabled
    //     2. MIC is not disabled and not enter DLPS
    //Solution1: Control audio effect along with call status
    //Solution2: TBD
    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_UP_DIRECT, NULL, NULL);
    if (p_ase_entry != NULL)
    {
        sidetone_disable(p_ase_entry->sidetone_instance);

        if (p_ase_entry->track_handle != NULL)
        {
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(p_ase_entry->track_handle, &stream_type))
            {
                if (app_lea_uca_check_enable_mic())
                {
                    audio_volume_in_unmute(stream_type);
                }
            }

            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_lea_uca_set_mic_mute_state(false);
#if 0
                app_lea_uca_sync_mute_state();
#endif
            }

#if F_APP_MICS_SUPPORT
            T_MICS_PARAM mics_param;

            mics_param.mic_mute = MICS_NOT_MUTE;
            mics_set_param(&mics_param);
#endif
        }
    }
}

static void app_lea_uca_handle_avrcp_playing(T_APP_LE_LINK *p_link, void *p_data)
{
    if (p_link->media_state == MCS_MEDIA_STATE_PLAYING)
    {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
        // don't stop le audio streaming when BT start to stream
        return;
#else
        T_LEA_ASE_ENTRY *p_ase_entry;

        p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
        if (p_ase_entry != NULL)
        {
            T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

            param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
            mcp_client_write_media_cp(p_ase_entry->conn_handle, 0, p_link->gmcs, &param, true);
        }
#endif
    }

    mtc_topology_dm(MTC_TOPO_EVENT_A2DP_START);
}

static void app_lea_uca_link_state_change(T_APP_LE_LINK *p_link, T_LEA_LINK_STATE state)
{
    APP_PRINT_TRACE3("app_lea_uca_link_state_change: handle 0x%02X, change from 0x%02X to 0x%02X",
                     p_link->conn_handle, p_link->lea_link_state, state);

    p_link->lea_link_state = state;

#if TARGET_LE_AUDIO_GAMING
    if (state != LEA_LINK_IDLE)
    {
        app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_LEA_UCA_LINK_STATE);
    }
#endif

}

static void app_lea_uca_ase_ctrl(T_APP_LE_LINK *p_link, T_LEA_LINK_EVENT event, void *p_data)
{
    T_LEA_ASE_ENTRY *p_ase_entry;
    bool lea_streaming = false;

    switch (event)
    {
    case LEA_PAUSE:
        {
            uint8_t i;
            T_ASCS_CP_DISABLE_IND *p_info = (T_ASCS_CP_DISABLE_IND *)p_data;

            for (i = 0; i < p_info->number_of_ase; i++)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_info->conn_handle,
                                                          (void *)(&p_info->ase_id[i]));
                if (p_ase_entry != NULL)
                {
                    if (p_ase_entry->track_handle != NULL)
                    {
                        app_lea_uca_track_sync_reset(p_link, p_ase_entry->track_handle);
                        audio_track_stop(p_ase_entry->track_handle);
                    }

                    if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
                    {
                        p_link->stream_channel_allocation &= ~p_ase_entry->codec_cfg.audio_channel_allocation;
                    }
                }
            }
        }
        break;

    case LEA_RELEASING:
        {
            T_ASCS_ASE_STATE *p_info = (T_ASCS_ASE_STATE *)p_data;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_info->conn_handle,
                                                      (void *)(&p_info->ase_data.ase_id));
            if (p_ase_entry != NULL)
            {
                if (p_ase_entry->track_handle != NULL)
                {
                    syncclk_drv_timer_stop(LEA_SYNC_CLK_REF);

                    app_lea_uca_track_sync_reset(p_link, p_ase_entry->track_handle);
                }

                //Detach audio effect
                if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    app_lea_uca_eq_release(&app_lea_uca_eq);
                }
                else //AUDIO_STREAM_TYPE_VOICE, AUDIO_STREAM_TYPE_RECORD
                {
                    app_sidetone_detach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
                    app_nrec_detach(p_ase_entry->track_handle, p_ase_entry->nrec_instance);
                }

                if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
                {
                    p_link->stream_channel_allocation &= ~p_ase_entry->codec_cfg.audio_channel_allocation;
                }

                app_lea_ascs_free_ase_entry(p_ase_entry);
            }
        }
        break;

    default:
        return;
    }

    app_lea_uca_link_state_change(p_link, LEA_LINK_CONNECTED);

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].lea_link_state == LEA_LINK_STREAMING)
        {
            lea_streaming = true;
            break;
        }
    }

    if (lea_streaming == false)
    {
        T_AUDIO_TRACK_STATE state;
        T_APP_BR_LINK *p_edr_link;

        p_edr_link = &app_db.br_link[app_a2dp_get_active_idx()];
        audio_track_state_get(p_edr_link->a2dp_track_handle, &state);
        if (state == AUDIO_TRACK_STATE_PAUSED ||
            state == AUDIO_TRACK_STATE_STOPPED)
        {
            if (p_edr_link->streaming_fg == true)
            {
                audio_track_start(p_edr_link->a2dp_track_handle);
            }
        }
    }

    // TODO: pause state, change to 60ms
    //ble_set_prefer_conn_param(p_link->conn_id, 0x30, 0x30, 0, 500);
    APP_PRINT_TRACE1("app_lea_uca_ase_ctrl: streaming %d", lea_streaming);
}

static void app_lea_uca_adjust_active_handle_by_connection(uint16_t connect_handle, uint8_t event)
{
    T_APP_LE_LINK *p_active_link = NULL;
    uint8_t link_num = app_link_get_lea_link_num();

    if (event == LEA_CONNECT)
    {
        if (link_num <= 1)
        {
            app_lea_mcp_set_active_conn_handle(connect_handle);
            app_lea_ccp_set_active_conn_handle(connect_handle);
        }
        else
        {
            if (app_lea_mcp_get_active_conn_handle() != connect_handle)
            {
                p_active_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());

                if (p_active_link != NULL)
                {
                    if (p_active_link->media_state == MCS_MEDIA_STATE_INACTIVE ||
                        p_active_link->media_state == MCS_MEDIA_STATE_PAUSED)
                    {
                        app_lea_mcp_set_active_conn_handle(connect_handle);
                    }
                }
            }

            if (app_lea_ccp_get_active_conn_handle() != connect_handle)
            {
                p_active_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());

                if (p_active_link != NULL)
                {
                    if (p_active_link->call_status == APP_CALL_IDLE)
                    {
                        app_lea_ccp_set_active_conn_handle(connect_handle);
                    }
                }
            }
        }
    }
    else
    {
        if (link_num == 0)
        {
            app_db.is_lea_device_connected = 0;
            app_db.ever_roleswapped = 0;
            app_lea_mcp_reset_active_conn_handle();
            app_lea_ccp_reset_active_conn_handle();
        }
        else
        {
            uint8_t i;

            for (i = 0; i < MAX_BLE_LINK_NUM; i++)
            {
                if (app_db.le_link[i].used == true && app_db.le_link[i].lea_link_state != LEA_LINK_IDLE)
                {
                    p_active_link = &app_db.le_link[i];
                    break;
                }
            }

            if (p_active_link != NULL)
            {
                if (app_lea_mcp_get_active_conn_handle() == connect_handle)
                {
                    app_lea_mcp_set_active_conn_handle(p_active_link->conn_handle);
                }

                if (app_lea_ccp_get_active_conn_handle() == connect_handle)
                {
                    app_lea_ccp_set_active_conn_handle(p_active_link->conn_handle);
                }
            }
        }
    }
#if GFPS_SASS_LEA_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }
#endif
}

static void app_lea_uca_handle_le_disconnect(T_APP_LE_LINK *p_link, uint8_t event, void *p_data)
{
    uint16_t cause = *(uint16_t *)p_data;

    if (event == LEA_DISCON_LOCAL)
    {
        app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_POWER_OFF);
    }
    else
    {
        if ((cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
            (cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
        {
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                app_audio_tone_type_play(TONE_LINK_LOSS, false, false);
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_audio_tone_type_play(TONE_LINK_LOSS, false, true);
            }
            app_lea_adv_linkback_set(true);
            app_lea_adv_update();
        }
    }

    app_lea_uca_free_link(p_link);
    app_lea_uca_link_state_change(p_link, LEA_LINK_IDLE);
    app_bt_policy_msg_prof_disconn(p_link->bd_addr, UCA_PROFILE_MASK, cause);
    app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_LEA, BP_LINK_EVENT_DISCONNECT, p_link->bd_addr);
#if F_APP_ERWS_SUPPORT
    app_lea_mgr_sync_link_num();
#endif
#if F_APP_CCP_SUPPORT
    app_lea_ccp_update_call_status();
#endif
    app_lea_uca_adjust_active_handle_by_connection(p_link->conn_handle, event);

    mtc_topology_dm(MTC_TOPO_EVENT_LEA_DISCONN);
    app_bt_point_link_tone_set(p_link->bd_addr, false);
}

static void app_lea_uca_release_ascs_action(T_APP_LE_LINK *p_link, void *p_data)
{
    T_LEA_ASE_ENTRY *p_ase_entry;

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        /* don't care call status when release ase if
            source switch is enabled */
    }
    else
#endif
    {
        if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
        {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            // allow release ase when 2.4G call active because we want hfp call preempt
#else
            return;
#endif
        }
    }

    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
    if (p_ase_entry != NULL)
    {
        bool is_ascs_release = false;
        T_ASE_CHAR_DATA ase_data;

        if (ascs_get_ase_data(p_ase_entry->conn_handle, p_ase_entry->ase_id, &ase_data))
        {
            is_ascs_release = ascs_action_release_by_cig(p_ase_entry->conn_handle,
                                                         ase_data.param.enabling.cig_id);
        }

        if (is_ascs_release)
        {
            if (p_ase_entry->track_handle != NULL)
            {
                syncclk_drv_timer_stop(LEA_SYNC_CLK_REF);
            }

            //Detach audio effect
            if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                app_lea_uca_eq_release(&app_lea_uca_eq);
            }
            else //AUDIO_STREAM_TYPE_VOICE, AUDIO_STREAM_TYPE_RECORD
            {
                app_sidetone_detach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
                app_nrec_detach(p_ase_entry->track_handle, p_ase_entry->nrec_instance);
            }

            app_lea_uca_track_sync_reset(p_link, p_ase_entry->track_handle);
            app_lea_ascs_free_ase_entry(p_ase_entry);
            app_lea_uca_link_state_change(p_link, LEA_LINK_CONNECTED);
        }
    }
}

static void app_lea_uca_link_idle(T_APP_LE_LINK *p_link, uint8_t event, void *p_data)
{
    APP_PRINT_TRACE1("app_lea_uca_link_idle: event 0x%02X", event);
    switch (event)
    {
    case LEA_CONNECT:
        {
            uint8_t buf;

            app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ISO_RESTORE);
            //lea connected psd vendor command
            app_vendor_send_psd_policy(p_link->conn_handle);

            // TODO: to avoid service discovery taking long time, change to 7.5ms
            //ble_set_prefer_conn_param(p_link->conn_id, 0x06, 0x06, 0, 500);
            app_lea_uca_link_state_change(p_link, LEA_LINK_CONNECTED);
            app_lea_uca_adjust_active_handle_by_connection(p_link->conn_handle, event);
            app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_LEA, BP_LINK_EVENT_CONNECT, p_link->bd_addr);
            app_bt_policy_msg_prof_conn(p_link->bd_addr, UCA_PROFILE_MASK);
#if F_APP_ERWS_SUPPORT
            app_lea_mgr_sync_link_num();
#endif
            if (p_link->encryption_status == LE_LINK_ENCRYPTIONED)
            {
                if (app_bt_point_link_tone_get(p_link->bd_addr) == false)
                {
					//ysc start
                    app_bt_point_link_tone_set(p_link->bd_addr, true);
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
						app_led_change_mode(LED_MODE_PAIRING_COMPLETE, true, false);
                        app_audio_tone_type_play(TONE_LINK_CONNECTED, false, false);
                    }
                    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
						app_led_change_mode(LED_MODE_PAIRING_COMPLETE, true, false);
                        app_audio_tone_type_play(TONE_LINK_CONNECTED, false, true);
                    }
					//ysc end
                }
            }

            app_bond_le_set_bond_flag((void *)p_link, BOND_FLAG_LEA);

            app_lea_uca_bond_action(UCA_BOND_ACTION_ADD, p_link->bd_addr);

            mtc_topology_dm(MTC_TOPO_EVENT_LEA_CONN);
            app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_LEA);

#if F_APP_ERWS_SUPPORT
            if (!app_db.remote_cis_link_is_streaming)
            {
                app_sniff_mode_b2b_enable(app_cfg_nv.bud_peer_addr, SNIFF_DISABLE_MASK_LEA);
            }
#endif

            if (app_bt_point_lea_link_is_full())
            {
                app_lea_adv_stop();
            }
            else if (app_lea_adv_get_state() == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                app_lea_adv_start();
            }
            else
            {
                app_lea_adv_update_interval(APP_LEA_ADV_INTERVAL_SLOW);
                app_gfps_adv_update_adv_interval(400);
                APP_PRINT_TRACE0("------------  app_lea_uca_link_idle, app_gfps_adv_update_adv_interval (400) -------------");  
            }

#if F_APP_GAMING_DONGLE_SUPPORT
#else
            if (app_db.enter_gaming_mode_after_power_on && !app_db.gaming_mode)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_mmi_handle_action(MMI_DEV_GAMING_MODE_SWITCH);
                }
                else
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_relay_async_single(APP_MODULE_TYPE_UCA, UCA_MSG_SYNC_GAMING_MODE);
                        app_mmi_handle_action(MMI_DEV_GAMING_MODE_SWITCH);
                    }
                }
                app_db.enter_gaming_mode_after_power_on = false;
            }
#endif

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
            app_db.pairing_bit = false;
            app_lea_adv_update();
            app_cfg_nv.le_audio_dongle_connected_record = true;
            app_cfg_store(&app_cfg_nv.offset_one_wire_aging_test_done, 1);
#endif

            app_db.is_lea_device_connected = 1;
            buf = app_db.is_lea_device_connected;
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_UCA,
                                                UCA_MSG_SYNC_LEA_CONNECT,
                                                &buf, 1);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_audio_speaker_channel_set(AUDIO_SINGLE_SET, 0);
            }

            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (app_db.remote_is_lea_device_connected == 1))
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LEA_CONNECT);
            }
#if GFPS_SASS_LEA_SUPPORT
            app_sass_handle_le_dm(p_link->conn_id, SASS_LE_CONNECTED, NULL);
#endif
        }
        break;

    case LEA_A2DP_START:
    case LEA_AVRCP_PLAYING:
        {
            mtc_topology_dm(MTC_TOPO_EVENT_A2DP_START);
        }
        break;

    case LEA_HFP_CALL_STATE:
        {
            mtc_topology_dm(MTC_TOPO_EVENT_HFP_CALL);
        }
        break;

    default:
        break;
    }
}

static void app_lea_uca_link_connected(T_APP_LE_LINK *p_link, uint8_t event, void *p_data)
{
    APP_PRINT_TRACE1("app_lea_uca_link_connected: event 0x%02X", event);

    switch (event)
    {
    case LEA_DISCONNECT:
    case LEA_DISCON_LOCAL:
        {
            app_lea_uca_handle_le_disconnect(p_link, event, p_data);

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            app_dongle_handle_stream_preempt(EVENT_2_4G_STREAM_STOP);
#endif
        }
        break;

    case LEA_PREFER_QOS:
        {
            app_lea_uca_prefer_qos(p_data);
        }
        break;

    case LEA_ENABLE:
        {
            uint8_t i;
            T_LEA_ASE_ENTRY *p_ase_entry;
            T_ASCS_CP_ENABLE_IND *p_info = (T_ASCS_CP_ENABLE_IND *)p_data;
            uint8_t sample_frequency = 0;
            uint32_t presentation_delay = 0;
            bool freq_diff = false;
            bool pd_diff = false;
            bool lc3_plus = false;

            mtc_topology_dm(MTC_TOPO_EVENT_CIS_ENABLE);

            app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_LEA);
#if F_APP_ERWS_SUPPORT
            app_sniff_mode_b2b_disable(app_cfg_nv.bud_peer_addr, SNIFF_DISABLE_MASK_LEA);
#endif

            for (i = 0; i < p_info->number_of_ase; i++)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_info->conn_handle,
                                                          (void *)(&p_info->param[i].ase_id));
                if (p_ase_entry != NULL)
                {
#if F_APP_LC3_PLUS_SUPPORT
                    p_ase_entry->bit_depth = app_lea_pacs_check_bit_depth(p_ase_entry->codec_type,
                                                                          p_info->param[i].metadata_length, p_info->param[i].p_metadata);
                    if (p_ase_entry->codec_type == AUDIO_FORMAT_TYPE_LC3PLUS)
                    {
                        lc3_plus = true;
                    }
#endif
                    //reset
                    app_lea_uca_set_s2m_zero_packet(p_ase_entry->cis_conn_handle, 0xFF);

                    if (!sample_frequency)
                    {
                        sample_frequency = p_ase_entry->codec_cfg.sample_frequency;
                    }
                    else if (sample_frequency != p_ase_entry->codec_cfg.sample_frequency)
                    {
                        freq_diff = true;
                    }

                    if (!presentation_delay)
                    {
                        presentation_delay =  p_ase_entry->presentation_delay;
                    }
                    else if (presentation_delay != p_ase_entry->presentation_delay)
                    {
                        pd_diff = true;
                    }

                    if (p_ase_entry->track_handle != NULL)
                    {
                        T_AUDIO_TRACK_STATE track_state;

                        audio_track_state_get(p_ase_entry->track_handle, &track_state);
                        if (track_state != AUDIO_TRACK_STATE_STARTED)
                        {
                            audio_track_start(p_ase_entry->track_handle);
                        }
                    }
                }
            }

            if (freq_diff || pd_diff || lc3_plus || app_cfg_const.upstresm_algo)
            {
                app_lea_uca_diff_path = true;
            }
            else
            {
                app_lea_uca_diff_path = false;
            }

#if F_APP_LEA_REALCAST_SUPPORT
            app_lea_realcast_stop_scan();
#endif
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            app_dongle_handle_stream_preempt(EVENT_2_4G_STREAM_START);
#endif
        }
        break;

    case LEA_ENABLEING:
        {
            T_LEA_ASE_ENTRY *p_ase_entry;
            bool is_call = false;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_link->conn_handle, p_data);

            if (p_ase_entry != NULL)
            {
                if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
                {
                    p_link->stream_channel_allocation |= p_ase_entry->codec_cfg.audio_channel_allocation;
                }

                if (p_ase_entry->audio_context == AUDIO_CONTEXT_CONVERSATIONAL)
                {
                    is_call = true;
                }

                mtc_stream_switch(is_call);

                if ((app_lea_mcp_get_active_conn_handle() == 0) && (app_lea_ccp_get_active_conn_handle() == 0))
                {
                    app_lea_uca_set_stream_link(p_link);
                }
            }
        }
        break;

    case LEA_CODEC_CONFIGURED:
        {

        }
        break;

    case LEA_SETUP_DATA_PATH:
        {
            app_lea_uca_setup_data_path(p_link, p_data);
        }
        break;

    case LEA_REMOVE_DATA_PATH:
        {
            app_lea_uca_remove_data_path(p_link, p_data);

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            app_dongle_handle_stream_preempt(EVENT_2_4G_STREAM_STOP);
#endif
        }
        break;

    case LEA_STREAMING:
        {
            T_LEA_ASE_ENTRY *p_ase_entry;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
            if (p_ase_entry != NULL)
            {
#if F_APP_ERWS_SUPPORT
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
                    app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_lea_uca_sync_cis_streaming_state(true);
                }
#endif
                mtc_topology_dm(MTC_TOPO_EVENT_CIS_STREAMING);

                if (p_ase_entry->audio_context == AUDIO_CONTEXT_CONVERSATIONAL)
                {
#if F_APP_CCP_SUPPORT
                    mtc_topology_dm(MTC_TOPO_EVENT_CCP_CALL);
#endif
                }
                app_lea_uca_stop_inactive_stream(p_link);
            }

            app_lea_uca_link_state_change(p_link, LEA_LINK_STREAMING);
            // TODO: streaming state, change to 40ms
            //ble_set_prefer_conn_param(p_link->conn_id, 0x20, 0x20, 0, 500);

#if F_APP_LEA_SUPPORT && F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
            if (app_cfg_const.enable_24g_bt_audio_source_switch)
            {
                if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
                {
                    app_dongle_source_switch_to_bt_le_audio_handle();
                }
            }
#endif
        }
        break;

    case LEA_PAUSE:
        {
            app_lea_uca_ase_ctrl(p_link, LEA_PAUSE, p_data);
        }
        break;

    case LEA_RELEASING:
        {
            app_lea_uca_ase_ctrl(p_link, LEA_RELEASING, p_data);
        }
        break;

    case LEA_A2DP_START:
    case LEA_AVRCP_PLAYING:
        {
            app_lea_uca_handle_avrcp_playing(p_link, p_data);
        }
        break;

#if F_APP_MCP_SUPPORT
    case LEA_MCP_PAUSE:
        {
            if (p_link->media_state == MCS_MEDIA_STATE_PLAYING)
            {
                T_LEA_ASE_ENTRY *p_ase_entry;

                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
                if (p_ase_entry != NULL)
                {
                    T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                    mcp_client_write_media_cp(p_ase_entry->conn_handle, 0, p_link->gmcs, &param, true);
                }
            }
        }
        break;

    case LEA_MCP_STATE:
        {
            app_lea_uca_handle_mcp_state(p_link);
        }
        break;
#endif
    case LEA_HFP_CALL_STATE:
        {
            if (app_hfp_get_call_status() == APP_CALL_IDLE)
            {
                app_lea_ccp_read_all_links_state();
            }

            mtc_topology_dm(MTC_TOPO_EVENT_HFP_CALL);
        }
        break;

#if F_APP_CCP_SUPPORT
    case LEA_CCP_CALL_STATE:
        {
            app_lea_uca_handle_ccp_state(p_link, p_data);
        }
        break;

    case LEA_CCP_READ_RESULT:
        {
            mtc_topology_dm(MTC_TOPO_EVENT_CCP_CALL);
#if F_APP_LISTENING_MODE_SUPPORT
            app_listening_mode_call_status_update();
#endif
        }
        break;

    case LEA_CCP_TERM_NOTIFY:
        {
            app_lea_uca_handle_ccp_term(p_link, p_data);
        }
        break;
#endif

    case LEA_SNIFFING_STOP:
        {
            T_LEA_ASE_ENTRY *p_ase_entry;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
            if (p_ase_entry != NULL)
            {
                if (p_ase_entry->handshake_fg)
                {
                    p_ase_entry->handshake_fg = false;
                    ascs_action_rec_start_ready(p_ase_entry->conn_handle, p_ase_entry->ase_id);
                    ascs_app_ctl_handshake(p_ase_entry->conn_handle, p_ase_entry->ase_id, false);
                }
            }
        }
        break;

    case LEA_PAUSE_LOCAL:
        {
            app_lea_uca_release_ascs_action(p_link, p_data);
        }
        break;

    default:
        break;
    }
}

static void app_lea_uca_link_streaming(T_APP_LE_LINK *p_link, uint8_t event, void *p_data)
{
    APP_PRINT_TRACE1("app_lea_uca_link_streaming: event 0x%02X", event);

    switch (event)
    {
    case LEA_DISCONNECT:
    case LEA_DISCON_LOCAL:
        {
            app_lea_uca_handle_le_disconnect(p_link, event, p_data);
        }
        break;

    case LEA_PREFER_QOS:
        {
            app_lea_uca_prefer_qos(p_data);
        }
        break;

    case LEA_ENABLEING:
        {
            T_LEA_ASE_ENTRY *p_ase_entry;

            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_link->conn_handle, p_data);

            if (p_ase_entry != NULL)
            {
                if (p_ase_entry->path_direction == DATA_PATH_OUTPUT_FLAG)
                {
                    p_link->stream_channel_allocation |= p_ase_entry->codec_cfg.audio_channel_allocation;
                }
            }
        }
        break;

    case LEA_SETUP_DATA_PATH:
        {
            app_lea_uca_setup_data_path(p_link, p_data);
        }
        break;

    case LEA_REMOVE_DATA_PATH:
        {
            app_lea_uca_remove_data_path(p_link, p_data);
        }
        break;

    case LEA_PAUSE:
        {
            app_lea_uca_ase_ctrl(p_link, LEA_PAUSE, p_data);
        }
        break;

    case LEA_RELEASING:
        {
            app_lea_uca_ase_ctrl(p_link, LEA_RELEASING, p_data);
        }
        break;

    case LEA_PAUSE_LOCAL:
        {
            app_lea_uca_release_ascs_action(p_link, p_data);
        }
        break;

    case LEA_A2DP_START:
    case LEA_AVRCP_PLAYING:
        {
            app_lea_uca_handle_avrcp_playing(p_link, p_data);
        }
        break;

#if F_APP_MCP_SUPPORT
    case LEA_MCP_PAUSE:
        {
            if (p_link->media_state == MCS_MEDIA_STATE_PLAYING)
            {
                T_LEA_ASE_ENTRY *p_ase_entry;

                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, p_link->conn_handle, NULL);
                if (p_ase_entry != NULL)
                {
                    T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                    mcp_client_write_media_cp(p_ase_entry->conn_handle, 0, p_link->gmcs, &param, true);
                }
            }
        }
        break;


    case LEA_MCP_STATE:
        {
            app_lea_uca_handle_mcp_state(p_link);
        }
        break;
#endif
    case LEA_HFP_CALL_STATE:
        {
            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                if (app_lea_ccp_get_call_status() == APP_CALL_IDLE &&
                    p_link->media_state == MCS_MEDIA_STATE_PLAYING)
                {
                    T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                    mcp_client_write_media_cp(p_link->conn_handle, 0, p_link->gmcs, &param, true);
                }
            }
            else
            {
                app_lea_ccp_read_all_links_state();
            }

            mtc_topology_dm(MTC_TOPO_EVENT_HFP_CALL);
        }
        break;

#if F_APP_CCP_SUPPORT
    case LEA_CCP_CALL_STATE:
        {
            app_lea_uca_handle_ccp_state(p_link, p_data);
        }
        break;

    case LEA_CCP_TERM_NOTIFY:
        {
            app_lea_uca_handle_ccp_term(p_link, p_data);
        }
        break;
#endif

    case LEA_METADATA_UPDATE:
        {
            uint8_t i;
            T_LEA_ASE_ENTRY *p_ase_entry;
            T_ASCS_CP_UPDATE_METADATA_IND *p_metadata = (T_ASCS_CP_UPDATE_METADATA_IND *)p_data;

            for (i = 0; i < p_metadata->number_of_ase; i++)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_ASE_ID, p_metadata->conn_handle,
                                                          (void *)(&p_metadata->param[i].ase_id));
                if (p_ase_entry != NULL)
                {
#if F_APP_LEA_ALWAYS_CONVERSATION
                    if (p_ase_entry->path_direction == DATA_PATH_INPUT_FLAG)
                    {
                        uint16_t audio_context = 0;

                        audio_context = app_lea_ascs_get_context(p_metadata->param[i].p_metadata,
                                                                 p_metadata->param[i].metadata_length);

                        if ((p_ase_entry->audio_context == AUDIO_CONTEXT_UNSPECIFIED) &&
                            (audio_context == AUDIO_CONTEXT_CONVERSATIONAL) &&
                            (p_ase_entry->track_handle != NULL))
                        {
                            audio_track_start(p_ase_entry->track_handle);
                        }
                        else if ((p_ase_entry->audio_context == AUDIO_CONTEXT_CONVERSATIONAL) &&
                                 (audio_context == AUDIO_CONTEXT_UNSPECIFIED) &&
                                 (p_ase_entry->track_handle != NULL))
                        {
                            //fake conversation
                            audio_track_stop(p_ase_entry->track_handle);
                            app_lea_uca_set_s2m_zero_packet(p_ase_entry->cis_conn_handle, 1);
                        }
                    }
#endif
// #if GFPS_SASS_LEA_SUPPORT
//                     APP_PRINT_INFO1("LEA_METADATA_UPDATE: audio_context %d", p_ase_entry->audio_context);
//                     if (extend_app_cfg_const.gfps_sass_support)
//                     {
//                         app_gfps_notify_conn_status();
//                     }
// #endif
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_lea_uca_link_sm(uint16_t conn_handle, uint8_t event, void *p_data)
{
    T_APP_LE_LINK *p_link;

    p_link = app_link_find_le_link_by_conn_handle(conn_handle);

    if (p_link == NULL)
    {
        return;
    }

    switch (p_link->lea_link_state)
    {
    case LEA_LINK_IDLE:
        app_lea_uca_link_idle(p_link, event, p_data);
        break;

    case LEA_LINK_CONNECTED:
        app_lea_uca_link_connected(p_link, event, p_data);
        break;

    case LEA_LINK_STREAMING:
        app_lea_uca_link_streaming(p_link, event, p_data);
        break;

    default:
        break;
    }

    switch (event)
    {
    case LEA_CODEC_CONFIGURED:
    case LEA_PREFER_QOS:
    case LEA_ENABLE:
    case LEA_SETUP_DATA_PATH:
    case LEA_REMOVE_DATA_PATH:
    case LEA_MCP_STATE:
    case LEA_METADATA_UPDATE:
        app_lea_uca_dump_ase_info(p_link);
        break;

    case LEA_CCP_CALL_STATE:
    case LEA_CCP_READ_RESULT:
    case LEA_CCP_TERM_NOTIFY:
        app_lea_uca_dump_call_info(p_link);
        break;

    default:
        break;
    }
}

void app_lea_uca_set_sidetone_instance(T_APP_DSP_CFG_SIDETONE info)
{
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;
    int32_t ret = 0;

    if (app_lea_ccp_get_call_status() != APP_CALL_ACTIVE)
    {
        ret = 1;
        goto fail_status;
    }

    p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_UP_DIRECT, NULL, NULL);

    if (p_ase_entry == NULL)
    {
        ret = 2;
        goto fail_status;
    }

    app_sidetone_detach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
    p_ase_entry->sidetone_instance = app_sidetone_attach(p_ase_entry->track_handle, info);

    if (p_ase_entry->sidetone_instance == NULL)
    {
        ret = 3;
        goto fail_status;
    }

fail_status:
    APP_PRINT_ERROR1("app_lea_uca_set_sidetone_instance: failed %d", -ret);
}

static bool app_lea_uca_check_combination_iso_data(T_ISO_DATA_IND **p_iso_elem,
                                                   uint8_t *output_channel,
                                                   T_BT_DIRECT_CB_DATA *p_data)
{
    T_OS_QUEUE *p_mirror_queue = NULL;
    T_OS_QUEUE *p_insert_queue = NULL;
    uint16_t output_handle = 0;
    uint8_t ret = 0;

    *output_channel = AUDIO_LOCATION_MONO;

    if (app_lea_uca_active_stream_link->cis_left_ch_conn_handle == p_data->p_bt_direct_iso->conn_handle)
    {
        if (p_data->p_bt_direct_iso->pkt_status_flag == ISOCH_DATA_PKT_STATUS_VALID_DATA)
        {
            app_lea_uca_active_stream_link->cis_left_ch_iso_state = p_data->p_bt_direct_iso->pkt_status_flag;
        }

        p_mirror_queue = &app_lea_uca_right_ch_queue;
        p_insert_queue = &app_lea_uca_left_ch_queue;
        output_handle = app_lea_uca_active_stream_link->cis_left_ch_conn_handle;
        *output_channel = AUDIO_LOCATION_FL;
    }
    else if (app_lea_uca_active_stream_link->cis_right_ch_conn_handle ==
             p_data->p_bt_direct_iso->conn_handle)
    {
        if (p_data->p_bt_direct_iso->pkt_status_flag == ISOCH_DATA_PKT_STATUS_VALID_DATA)
        {
            app_lea_uca_active_stream_link->cis_right_ch_iso_state = p_data->p_bt_direct_iso->pkt_status_flag;
        }

        p_mirror_queue = &app_lea_uca_left_ch_queue;
        p_insert_queue = &app_lea_uca_right_ch_queue;
        output_handle = app_lea_uca_active_stream_link->cis_right_ch_conn_handle;
        *output_channel = AUDIO_LOCATION_FR;
    }
    else
    {
        ret = 1;
        goto fail_get_iso;
    }

    if ((app_lea_uca_active_stream_link->cis_left_ch_iso_state != ISOCH_DATA_PKT_STATUS_VALID_DATA) ||
        (app_lea_uca_active_stream_link->cis_right_ch_iso_state != ISOCH_DATA_PKT_STATUS_VALID_DATA))
    {
        ret = 2;
        goto fail_get_iso;
    }

    if (p_mirror_queue->count == 0)
    {
        //Wait for the ISO data in another channel to combine a stereo packet
        app_lea_mgr_enqueue_iso_data(p_insert_queue, p_data->p_bt_direct_iso, output_handle);
    }
    else
    {
        *p_iso_elem = app_lea_mgr_find_iso_elem(p_mirror_queue, p_data->p_bt_direct_iso);
        if (*p_iso_elem == NULL)
        {
            app_lea_mgr_enqueue_iso_data(p_insert_queue, p_data->p_bt_direct_iso, output_handle);
            ret = 3;
            goto fail_get_iso;
        }
    }

    return true;

fail_get_iso:
    APP_PRINT_ERROR7("app_lea_uca_check_combination_iso_data: result %d, left ch %d, right ch %d, \
left handle 0x%02X, right handle 0x%02X, data handle 0x%02X, output_channel %d",
                     -ret,
                     app_lea_uca_active_stream_link->cis_left_ch_iso_state,
                     app_lea_uca_active_stream_link->cis_right_ch_iso_state,
                     app_lea_uca_active_stream_link->cis_left_ch_conn_handle,
                     app_lea_uca_active_stream_link->cis_right_ch_conn_handle,
                     p_data->p_bt_direct_iso->conn_handle,
                     *output_channel);

    return false;
}

void app_lea_uca_handle_iso_data(T_BT_DIRECT_CB_DATA *p_data)
{
    uint8_t ret = 0;
    T_LEA_ASE_ENTRY *p_ase_entry;

#if F_APP_OTA_SUPPORT
    if (app_ota_dfu_is_busy()
#if F_APP_TUYA_SUPPORT
        || tuya_ota_is_busy()
#endif
       )
    {
        //If ota is going on, or not active streaming,
        //lea data won't be written to audio track.
        ret = 1;
        goto fail_track_write;
    }
#endif

    if (app_lea_uca_active_stream_link->cis_left_ch_conn_handle != 0 &&
        app_lea_uca_active_stream_link->cis_right_ch_conn_handle != 0)
    {
        p_ase_entry = app_lea_ascs_find_ase_entry_by_direction(app_lea_uca_active_stream_link,
                                                               DATA_PATH_OUTPUT_FLAG);
        if (p_ase_entry != NULL && p_ase_entry->track_handle != NULL)
        {
            if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                T_OS_QUEUE *p_mirror_queue = NULL;
                T_ISO_DATA_IND *p_iso_elem = NULL;
                uint16_t pkt_seq_num;
                uint32_t pkt_timestamp;
                uint16_t written_len;
                T_AUDIO_STREAM_STATUS status;
                uint8_t *p_send_buf = NULL;
                uint16_t sdu_len = 0;
                uint8_t output_channel = AUDIO_LOCATION_MONO;
                T_AUDIO_TRACK_STATE track_state;

                audio_track_state_get(p_ase_entry->track_handle, &track_state);
                if (track_state != AUDIO_TRACK_STATE_STARTED)
                {
                    ret = 2;
                    goto fail_track_write;
                }

                if (app_lea_uca_check_combination_iso_data(&p_iso_elem, &output_channel, p_data) == false)
                {
                    ret = 3;
                    goto fail_track_write;
                }

                if (p_iso_elem != NULL)
                {
                    sdu_len = p_ase_entry->codec_cfg.octets_per_codec_frame * p_ase_entry->frame_num;

                    p_send_buf = audio_probe_media_buffer_malloc(sdu_len);

                    if (p_send_buf == NULL)
                    {
                        ret = 4;
                        goto fail_track_write;
                    }

                    if (p_data->p_bt_direct_iso->iso_sdu_len != 0 && p_iso_elem->iso_sdu_len != 0)
                    {
                        status = AUDIO_STREAM_STATUS_CORRECT;
                    }
                    else
                    {
                        status = AUDIO_STREAM_STATUS_LOST;
                        sdu_len = 0;
                    }

                    if (output_channel == AUDIO_LOCATION_FL)
                    {
                        p_mirror_queue = &app_lea_uca_right_ch_queue;
                        if (status == AUDIO_STREAM_STATUS_CORRECT)
                        {
                            memcpy(p_send_buf, p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset,
                                   p_data->p_bt_direct_iso->iso_sdu_len);

                            memcpy(p_send_buf + p_data->p_bt_direct_iso->iso_sdu_len, p_iso_elem->p_buf,
                                   p_iso_elem->iso_sdu_len);
                        }

                        pkt_seq_num = p_data->p_bt_direct_iso->pkt_seq_num;
                        pkt_timestamp = p_data->p_bt_direct_iso->time_stamp;
                    }
                    else
                    {
                        p_mirror_queue = &app_lea_uca_left_ch_queue;
                        if (status == AUDIO_STREAM_STATUS_CORRECT)
                        {
                            memcpy(p_send_buf, p_iso_elem->p_buf,  p_iso_elem->iso_sdu_len);
                            memcpy(p_send_buf + p_iso_elem->iso_sdu_len,
                                   p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset,
                                   p_data->p_bt_direct_iso->iso_sdu_len);
                        }

                        pkt_seq_num = p_iso_elem->pkt_seq_num;
                        pkt_timestamp = p_iso_elem->time_stamp;
                    }

                    audio_track_write(p_ase_entry->track_handle, pkt_timestamp,
                                      pkt_seq_num,
                                      status,
                                      FIXED_FRAME_NUM,
                                      p_send_buf,
                                      sdu_len,
                                      &written_len);

                    os_queue_delete(p_mirror_queue, p_iso_elem);
                    audio_probe_media_buffer_free(p_iso_elem);
                    audio_probe_media_buffer_free(p_send_buf);
                }
            }
            else if (p_ase_entry->stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                app_lea_uca_active_stream_link->cis_left_ch_conn_handle = 0;
                app_lea_uca_active_stream_link->cis_right_ch_conn_handle = 0;
            }
        }
    }
    else
    {
        p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_DOWN_DIRECT,
                                                           (void *)&p_data->p_bt_direct_iso->conn_handle,
                                                           NULL);
        if (p_ase_entry != NULL && p_ase_entry->track_handle != NULL)
        {
            T_AUDIO_TRACK_STATE track_state;
            uint16_t written_len;
            T_AUDIO_STREAM_STATUS status;
            uint8_t *p_iso_data = p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset;
            T_APP_LE_LINK *p_link;

            audio_track_state_get(p_ase_entry->track_handle, &track_state);
            if (track_state != AUDIO_TRACK_STATE_STARTED)
            {
                ret = 5;
                goto fail_track_write;
            }

            p_link = app_link_find_le_link_by_conn_handle(p_ase_entry->conn_handle);
            if (p_link == NULL || !(p_link->lea_ready_to_downstream & LEA_DS_SYNC_READY))
            {
                lea_playback_resume_cnt++;

                if (lea_playback_resume_cnt == PLAYBACK_RESUME_CNT)
                {
                    T_IO_MSG msg;

                    msg.type = IO_MSG_TYPE_LEA_SNK;
                    msg.subtype = IO_MSG_LEA_ISO_SYNC_DOWNSTREAM;
                    msg.u.buf = p_link->bd_addr;
                    app_io_msg_send(&msg);

                    lea_playback_resume_cnt = 0;
                    p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;
                }

                ret = 6;
                goto fail_track_write;
            }

            if (p_data->p_bt_direct_iso->iso_sdu_len != 0)
            {
                status = AUDIO_STREAM_STATUS_CORRECT;

                if (p_ase_entry->track_write_len != p_data->p_bt_direct_iso->iso_sdu_len)
                {
                    ret = 7;
                    goto fail_track_write;
                }
            }
            else
            {
                status = AUDIO_STREAM_STATUS_LOST;
            }

            audio_track_write(p_ase_entry->track_handle, p_data->p_bt_direct_iso->time_stamp,
                              p_data->p_bt_direct_iso->pkt_seq_num,
                              status,
                              FIXED_FRAME_NUM,
                              p_iso_data,
                              p_data->p_bt_direct_iso->iso_sdu_len,
                              &written_len);
        }
    }

fail_track_write:
    if (ret != 0)
    {
        APP_PRINT_ERROR1("app_lea_uca_handle_iso_data: failed %d", -ret);
    }
}

void app_lea_uca_set_mic_mute(void *p_link_info, uint8_t action, bool play_tone)
{
    uint8_t result = 0;
    bool mic_is_muted = false;
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;
    T_AUDIO_STREAM_TYPE stream_type;
    T_APP_LE_LINK *p_link = (T_APP_LE_LINK *)p_link_info;

    if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
    {
        if (app_lea_uca_get_diff_path())
        {
            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_UP_DIRECT, p_link->conn_handle, NULL);
        }
        else
        {
            p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_DEVICE_INOUT, NULL, NULL);
        }
    }
    else
    {
        result = 1;
        goto fail_set_mic_mute;
    }

    if (p_ase_entry == NULL)
    {
        result = 2;
        goto fail_set_mic_mute;
    }

    if (audio_track_stream_type_get(p_ase_entry->track_handle, &stream_type) == false)
    {
        result = 3;
        goto fail_set_mic_mute;
    }

    if (stream_type != AUDIO_STREAM_TYPE_VOICE && stream_type != AUDIO_STREAM_TYPE_RECORD)
    {
        result = 4;
        goto fail_set_mic_mute;
    }

    mic_is_muted = audio_volume_in_is_muted(stream_type);
    if (!mic_is_muted && (action == MMI_DEV_MIC_MUTE || action == MMI_DEV_MIC_MUTE_UNMUTE))
    {
        app_audio_enable_play_mic_mute_tone(play_tone);
        if (audio_volume_in_mute(stream_type) && play_tone)
        {
            app_lea_uca_mic_mute_state = true;
        }
    }
    else if (mic_is_muted && (action == MMI_DEV_MIC_UNMUTE || action == MMI_DEV_MIC_MUTE_UNMUTE))
    {
        app_audio_enable_play_mic_unmute_tone(play_tone);
        if (audio_volume_in_unmute(stream_type) && play_tone)
        {
            app_lea_uca_mic_mute_state = false;
        }
    }

#if 0
    app_lea_uca_sync_mute_state();
#endif
    return;

fail_set_mic_mute:
    APP_PRINT_TRACE3("app_lea_mgr_set_mic_mute: result %d, action %d, mic_is_muted %d",
                     -result, action, mic_is_muted);
}

void app_lea_uca_set_mic_mute_state(bool mute)
{
    app_lea_uca_mic_mute_state = mute;
}

bool app_lea_uca_get_mic_mute_state(void)
{
    return app_lea_uca_mic_mute_state;
}

bool app_lea_uca_sync_active_ds_state(T_APP_LE_LINK *p_link)
{
    bool ret = false;

    if (p_link && p_link->lea_ready_to_downstream)
    {
        APP_PRINT_INFO2("app_lea_uca_sync_active_ds_state: addr %s, state %x",
                        TRACE_BDADDR(p_link->bd_addr), p_link->lea_ready_to_downstream);
        T_UCA_DS_INFO ds_info;

        memcpy(ds_info.addr, p_link->bd_addr, 6);
        ds_info.downstream_ready = p_link->lea_ready_to_downstream;
        ret = app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_UCA,
                                                  UCA_MSG_SYNC_DOWNSTREAM_STATE,
                                                  (uint8_t *)&ds_info, sizeof(T_UCA_DS_INFO));
    }

    return ret;
}

static void app_lea_uca_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;
            uint16_t conn_handle = 0;
            T_LEA_ASE_ENTRY *p_ase_entry = NULL;
            T_APP_LE_LINK *p_link = NULL;

#if F_APP_CCP_SUPPORT
            if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
            {
                conn_handle = app_lea_ccp_get_active_conn_handle();
            }
            else
#endif
#if F_APP_MCP_SUPPORT
            {
                conn_handle = app_lea_mcp_get_active_conn_handle();
            }
#endif

            if ((conn_handle == 0) && (app_lea_uca_active_stream_link != NULL))
            {
                conn_handle = app_lea_uca_active_stream_link->conn_handle;
            }

            p_link = app_link_find_le_link_by_conn_handle(conn_handle);
            if (p_link == NULL)
            {
                break;
            }

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_TRACK, conn_handle,
                                                          param->track_state_changed.handle);
            }
            else
            {
                p_ase_entry = app_lea_ascs_find_ase_entry_non_conn(LEA_ASE_UP_DIRECT, NULL, NULL);
            }

            if (p_ase_entry == NULL)
            {
                break;
            }

            switch (param->track_state_changed.state)
            {
            case AUDIO_TRACK_STATE_STARTED:
            case AUDIO_TRACK_STATE_RESTARTED:
                {
                    //Sync B2B play simultaneously
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK || stream_type == AUDIO_STREAM_TYPE_VOICE)
                    {
                        T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(conn_handle);

                        app_lea_uca_bond_action(UCA_BOND_ACTION_HIGHEST, p_link->bd_addr);

                        p_link->lea_ready_to_downstream |= LEA_LOCAL_TRACK_READY;
#if F_APP_ERWS_SUPPORT
                        if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;
                        }
                        else
                        {
                            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                                (p_link->lea_ready_to_downstream & LEA_REMOTE_TRACK_READY))
                            {
                                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_UCA, UCA_MSG_SYNC_DOWNSTREAM,
                                                                   p_link->bd_addr, 6, REMOTE_TIMER_HIGH_PRECISION,
                                                                   0, false);
                            }
                            else
                            {
                                app_lea_uca_sync_active_ds_state(p_link);
                            }
                        }
#else
                        p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;
#endif
                        APP_PRINT_INFO2("app_lea_uca_audio_cback: addr %s, state %x", TRACE_BDADDR(p_link->bd_addr),
                                        p_link->lea_ready_to_downstream);
                    }

                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        APP_PRINT_TRACE0("playback track changed to started, send a dummy pkt");

                        uint32_t pkt_timestamp = 0xFFFFFFFF;
                        uint16_t pkt_seq_num = 0xFF;
                        T_AUDIO_STREAM_STATUS status = AUDIO_STREAM_STATUS_DUMMY;
                        uint8_t buf[2] = {0xFF, 0xFF};
                        uint16_t sdu_len = 2;
                        uint16_t written_len;

                        audio_track_write(p_ase_entry->track_handle, pkt_timestamp,
                                          pkt_seq_num,
                                          status,
                                          FIXED_FRAME_NUM,
                                          buf,
                                          sdu_len,
                                          &written_len);
                    }

                    //Attach audio effect
                    if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                    {
                        if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                        {
                            app_lea_uca_playback_eq_setting(&app_lea_uca_eq, &app_lea_uca_eq_enabled,
                                                            p_ase_entry->track_handle);
                        }
                        else //AUDIO_STREAM_TYPE_VOICE, AUDIO_STREAM_TYPE_RECORD
                        {
                            if (app_dsp_cfg_sidetone.hw_enable)
                            {
                                audio_volume_in_unmute(stream_type);
                            }

                            p_ase_entry->sidetone_instance = app_sidetone_attach(p_ase_entry->track_handle,
                                                                                 app_dsp_cfg_sidetone);
#if F_APP_MICS_SUPPORT
                            T_MICS_PARAM mics_param;

                            if (mics_get_param(&mics_param) && (mics_param.mic_mute == MICS_MUTED))
                            {
                                audio_volume_in_mute(stream_type);
                                p_ase_entry->nrec_instance = app_nrec_attach(p_ase_entry->track_handle, false);
                            }
                            else
#endif
                            {
                                if (app_lea_uca_check_enable_mic() == false || app_lea_uca_mic_mute_state == true)
                                {
                                    audio_volume_in_mute(stream_type);
                                    p_ase_entry->nrec_instance = app_nrec_attach(p_ase_entry->track_handle, false);
                                }
                                else
                                {
                                    audio_volume_in_unmute(stream_type);
                                    p_ase_entry->nrec_instance = app_nrec_attach(p_ase_entry->track_handle, true);
                                }
                            }
                        }

                        app_lea_vol_update_track_volume();
                    }
                }
                break;

            case AUDIO_TRACK_STATE_STOPPED:
            case AUDIO_TRACK_STATE_PAUSED:
                {
                    if (stream_type == AUDIO_STREAM_TYPE_VOICE || stream_type == AUDIO_STREAM_TYPE_RECORD)
                    {
                        if (p_ase_entry != NULL)
                        {
                            app_nrec_detach(p_ase_entry->track_handle, p_ase_entry->nrec_instance);
                            app_sidetone_detach(p_ase_entry->track_handle, p_ase_entry->sidetone_instance);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {
            uint32_t timestamp;
            uint16_t seq_num;
            uint8_t frame_num;
            uint16_t read_len;
            uint8_t *buf;
            uint16_t conn_handle = 0;
            T_AUDIO_STREAM_STATUS status;

            if (param->track_data_ind.len == 0)
            {
                break;
            }

#if F_APP_CCP_SUPPORT
            conn_handle = app_lea_ccp_get_active_conn_handle();
#endif
            T_LEA_ASE_ENTRY *p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_TRACK, conn_handle,
                                                                       param->track_data_ind.handle);
            if ((p_ase_entry == NULL) ||
                (p_ase_entry->track_handle != param->track_data_ind.handle))
            {
                break;
            }

            buf = audio_probe_media_buffer_malloc(param->track_data_ind.len);

            if (buf == NULL)
            {
                break;
            }

            if (audio_track_read(param->track_data_ind.handle,
                                 &timestamp,
                                 &seq_num,
                                 &status,
                                 &frame_num,
                                 buf,
                                 param->track_data_ind.len,
                                 &read_len) == true)
            {
                if (app_lea_uca_check_enable_mic() == false)
                {
                    audio_probe_media_buffer_free(buf);
                    break;
                }

                app_lea_uca_send_iso_data(buf, p_ase_entry, param->track_data_ind.len,
                                          true, timestamp);
            }
            audio_probe_media_buffer_free(buf);
        }
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_lea_uca_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if (!app_link_check_b2b_link(param->acl_conn_disconn.bd_addr))
            {
                app_lea_adv_start();
            }
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                T_APP_LE_LINK *p_link = NULL;
                T_LEA_ASE_ENTRY *p_ase_entry = NULL;

                if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
                {
                    p_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());
                    if (p_link != NULL)
                    {
                        if (app_lea_uca_get_diff_path())
                        {
                            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_UP_DIRECT, p_link->conn_handle, NULL);
                        }
                        else
                        {
                            uint16_t audio_context = AUDIO_CONTEXT_CONVERSATIONAL;

                            p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONTEXT, p_link->conn_handle, &audio_context);
                        }

                        if (p_ase_entry != NULL)
                        {
                            app_nrec_roleswap(p_ase_entry->nrec_instance, app_lea_uca_check_enable_mic());
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
#if F_APP_ERWS_SUPPORT
            uint8_t buf;

            app_lea_uca_sync_active_ds_state(app_lea_uca_get_stream_link());

            buf = app_db.is_lea_device_connected;
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_UCA,
                                                UCA_MSG_SYNC_LEA_CONNECT,
                                                &buf, 1);

            app_lea_mgr_sync_link_num();
#endif
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            T_APP_LE_LINK *p_link = app_lea_uca_get_stream_link();
            if (p_link &&
                p_link->lea_link_state == LEA_LINK_STREAMING)
            {
                p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;

            }

            //If b2b disconnect, no need to sync track state
        }
        break;

    default:
        break;
    }
}


static uint16_t app_lea_uca_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_UCA, 0, NULL, true, total);
}

static void app_lea_uca_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_lea_uca_parse_cback: msg_type 0x%02X, status 0x%02X", msg_type, status);

    switch (msg_type)
    {
    case UCA_MSG_SYNC_DOWNSTREAM:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                T_APP_LE_LINK *p_link = app_link_find_le_link_by_addr(buf);
                if (p_link)
                {
                    p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;
                }
            }
        }
        break;

    case UCA_MSG_SYNC_LEA_CONNECT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_db.remote_is_lea_device_connected = buf[0];

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LEA_CONNECT);
                }
            }
        }
        break;

    case UCA_MSG_SYNC_DOWNSTREAM_STATE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_UCA_DS_INFO *ds_info = (T_UCA_DS_INFO *)buf;

                T_APP_LE_LINK *p_link = app_link_find_le_link_by_addr(ds_info->addr);
                if (ds_info && p_link)
                {
                    if (p_link->lea_ready_to_downstream & LEA_DS_SYNC_READY)
                    {
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_UCA,
                                                            UCA_MSG_SYNC_DOWNSTREAM,
                                                            p_link->bd_addr, 6);
                    }
                    else
                    {
                        if (ds_info->downstream_ready & LEA_DS_SYNC_READY)
                        {
                            p_link->lea_ready_to_downstream |= LEA_DS_SYNC_READY;
                        }
                        else if (ds_info->downstream_ready & LEA_LOCAL_TRACK_READY)
                        {
                            p_link->lea_ready_to_downstream |= LEA_REMOTE_TRACK_READY;

                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
                                (ds_info->downstream_ready & LEA_BOTH_TRACK_READY) &&
                                (!(ds_info->downstream_ready & LEA_DS_SYNC_READY)))
                            {
                                uint8_t addr[6];

                                memcpy(addr, p_link->bd_addr, 6);
                                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_UCA, UCA_MSG_SYNC_DOWNSTREAM,
                                                                   (uint8_t *)&addr, 6, REMOTE_TIMER_HIGH_PRECISION,
                                                                   0, false);
                            }
                        }
                    }
                    APP_PRINT_TRACE2("UCA_MSG_SYNC_DOWNSTREAM_STATE: info 0x%02X, local 0x%02X",
                                     ds_info->downstream_ready, p_link->lea_ready_to_downstream);
                }
            }
        }
        break;

    case UCA_MSG_SYNC_GAMING_MODE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_db.gaming_mode == false)
                {
                    app_mmi_handle_action(MMI_DEV_GAMING_MODE_SWITCH);
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

void app_lea_uca_msg_handle(T_IO_MSG *io_msg)
{
    T_IO_MSG_LEA sub_type = (T_IO_MSG_LEA)io_msg->subtype;

    switch (sub_type)
    {
    case IO_MSG_LEA_ISO_SYNC_DOWNSTREAM:
        {
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_UCA,
                                                UCA_MSG_SYNC_DOWNSTREAM,
                                                io_msg->u.buf, 6);
        }
        break;

    default:
        break;
    }
}

static void app_lea_uca_bond_cback(uint8_t cb_type, void *p_cb_data)
{
    uint8_t *bd_addr;

    switch (cb_type)
    {
    case BT_BOND_MSG_LE_BOND_REMOVE:
        {
            bd_addr = ((T_BT_LE_BOND_CB_DATA *)p_cb_data)->p_le_bond_remove->p_entry->remote_bd;
            app_lea_uca_bond_action(UCA_BOND_ACTION_DEL, bd_addr);
        }
        break;

    default:
        break;
    }
}

void app_lea_uca_init(void)
{
    os_queue_init(&app_lea_uca_left_ch_queue);
    os_queue_init(&app_lea_uca_right_ch_queue);
    os_queue_init(&app_lea_uca_upstream_queue);

    bt_bond_register_app_cb(app_lea_uca_bond_cback);
    audio_mgr_cback_register(app_lea_uca_audio_cback);
    ble_audio_cback_register(app_lea_uca_ble_audio_cback);
#if F_APP_ERWS_SUPPORT
    bt_mgr_cback_register(app_lea_uca_bt_cback);
    app_relay_cback_register(app_lea_uca_relay_cback, app_lea_uca_parse_cback,
                             APP_MODULE_TYPE_UCA, LEA_REMOTE_MSG_TOTAL);
#endif
}
#endif
