#include <stdlib.h>
#include "app_dongle_spp.h"
#include "app_dongle_record.h"
#include "app_mmi.h"
#include "app_main.h"
#include "bt_spp.h"
#include "btm.h"
#include "trace.h"
#include "app_report.h"
#include "remote.h"
#include "app_relay.h"
#include "app_cfg.h"
#include "app_transfer.h"
#include "app_listening_mode.h"
#include "app_audio_passthrough.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_audio_policy.h"
#include "sysm.h"
#include "app_dongle_common.h"
#include "app_dongle_ctrl.h"
#include "app_dongle_data_ctrl.h"
#include "app_dongle_source_ctrl.h"
#include "app_cmd.h"

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

#if F_APP_CFU_SPP_SUPPORT
#include "app_cfu_transfer.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#include "gap_br.h"
#endif

static const uint8_t dongle_service_class_uuid128[16] =
{
    0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x5A, 0x01, 0x00, 0x00
};

#if F_APP_GAMING_DONGLE_SUPPORT
static bool legacy_gaming_ready = false;
static uint8_t mic_data_idx = 0;

static void app_dongle_force_tx_2dh1(bool enable)
{
    uint16_t pkt_type = 0;
    uint8_t dongle_addr[6] = {0};

    if (enable)
    {
        pkt_type = GAP_PKT_TYPE_DM1 | GAP_PKT_TYPE_DH1 | \
                   GAP_PKT_TYPE_DM3 | GAP_PKT_TYPE_DH3 | \
                   GAP_PKT_TYPE_DM5 | GAP_PKT_TYPE_DH5 | \
                   GAP_PKT_TYPE_NO_2DH3 | GAP_PKT_TYPE_NO_2DH5 | \
                   GAP_PKT_TYPE_NO_3DH1 | GAP_PKT_TYPE_NO_3DH3 | GAP_PKT_TYPE_NO_3DH5;
    }
    else
    {
        pkt_type = GAP_PKT_TYPE_DM1 | GAP_PKT_TYPE_DH1 | \
                   GAP_PKT_TYPE_DM3 | GAP_PKT_TYPE_DH3 | \
                   GAP_PKT_TYPE_DM5 | GAP_PKT_TYPE_DH5 | \
                   GAP_PKT_TYPE_NO_3DH1 | GAP_PKT_TYPE_NO_3DH3 | GAP_PKT_TYPE_NO_3DH5;
    }

    if (app_dongle_get_connected_dongle_addr(dongle_addr))
    {
        gap_br_cfg_acl_pkt_type(dongle_addr, pkt_type);
    }
}

void app_dongle_handle_gaming_mode_cmd(uint8_t action)
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_GAMING_MODE_REQUEST);
        app_relay_async_single(APP_MODULE_TYPE_MMI, action);
    }

    app_mmi_handle_action(action);
}

void app_dongle_update_is_mic_enable(bool mic_enable)
{
    app_db.dongle_is_enable_mic = mic_enable;

    if (app_cfg_const.spp_voice_smaple_rate == RECORD_SAMPLE_RATE_16K)
    {
        app_dongle_force_tx_2dh1(mic_enable);
    }

    app_dongle_clear_spp_audio_buff();

    if (mic_enable)
    {
        headset_status.rtp_enable = true;
    }
    else
    {
        headset_status.rtp_enable = false;
    }
    app_dongle_sync_headset_status();

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_DONGLE_IS_ENABLE_MIC);
    }
#endif
}

void app_dongle_control_apt(uint8_t action)
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        app_mmi_handle_action(action);
    }
    else
    {
        app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_SYNC_DONGLE_IS_DISABLE_APT);
        app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                              0, false);
    }
}
#endif

static void app_dongle_spp_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_SPP_CONN_CMPL:
        {
            if (param->spp_conn_cmpl.local_server_chann != RFC_SPP_DONGLE_CHANN_NUM)
            {
                return;
            }

            p_link = app_link_find_br_link(param->spp_credit_rcvd.bd_addr);

            if (p_link != NULL)
            {
#if F_APP_GAMING_DONGLE_SUPPORT
                legacy_gaming_verify(p_link->bd_addr);
                app_dongle_updata_mic_data_idx(true);
#endif
                p_link->rfc_spp_credit = param->spp_conn_cmpl.link_credit;
                p_link->rfc_spp_frame_size = param->spp_conn_cmpl.frame_size;

                APP_PRINT_TRACE2("app_dongle_spp_cback: param->bt_spp_conn_cmpl.link_credit %d param->bt_spp_conn_cmpl.frame_size %d",
                                 p_link->rfc_spp_credit, p_link->rfc_spp_frame_size);
            }
        }
        break;

    case BT_EVENT_SPP_CREDIT_RCVD:
        {
            if (param->spp_credit_rcvd.local_server_chann != RFC_SPP_DONGLE_CHANN_NUM)
            {
                return;
            }

            p_link = app_link_find_br_link(param->spp_credit_rcvd.bd_addr);

            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_dongle_spp_cback: no acl link found");
                return;
            }

            p_link->rfc_credit = param->spp_credit_rcvd.link_credit;
        }
        break;

    case BT_EVENT_SPP_DATA_IND:
        {
            if (param->spp_data_ind.local_server_chann != RFC_SPP_DONGLE_CHANN_NUM)
            {
                return;
            }

            uint8_t     *p_data;
            uint8_t     app_idx;
            uint16_t    data_len;

            p_link = app_link_find_br_link(param->spp_data_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_dongle_spp_cback: no acl link found");
                return;
            }

            app_idx = p_link->id;
            p_data = param->spp_data_ind.data;
            data_len = param->spp_data_ind.len;

            bt_spp_credits_give(app_db.br_link[app_idx].bd_addr, param->spp_data_ind.local_server_chann,
                                1);

            if (app_cfg_const.enable_rtk_vendor_cmd)
            {
                if (data_len > 4)
                {
                    uint16_t payload_len = (p_data[2] | ((p_data [1] & 0xF0) << 4));
                    uint8_t msg_type = p_data[1] & 0x0F;

                    APP_PRINT_TRACE2("app_dongle_spp_cback: 0x%02X, 0x%02X", msg_type, payload_len);

                    // header check
                    if ((p_data[0] == DONGLE_FORMAT_START_BIT) && (p_data[payload_len + 3] == DONGLE_FORMAT_STOP_BIT))
                    {
                        APP_PRINT_INFO6("app_dongle_spp_cback %02x, %02x, %02x, %02x, %02x, %02x,",
                                        p_data[0], p_data[1], p_data[2], p_data[3], p_data[4], p_data[5]);

                        if (msg_type == DONGLE_TYPE_CMD)
                        {
#if F_APP_GAMING_DONGLE_SUPPORT
                            legacy_gaming_proc_data(p_data + 3, payload_len, param->spp_data_ind.bd_addr);
#endif
                            if (0)
                            {
                                //for compiler flag usage
                            }
#if F_APP_GAMING_DONGLE_SUPPORT
                            else if (p_data[3] == DONGLE_CMD_SET_GAMING_MOE)
                            {
                                APP_PRINT_TRACE4("DONGLE_CMD_SET_GAMING_MOE: %02x, %02x, %02x, %02x",
                                                 app_db.ignore_bau_first_gaming_cmd, app_db.restore_gaming_mode, app_db.gaming_mode,
                                                 app_db.ignore_bau_first_gaming_cmd_handled);

                                uint8_t action = MMI_DEV_GAMING_MODE_SWITCH;

                                if (app_db.ignore_bau_first_gaming_cmd && !app_db.ignore_bau_first_gaming_cmd_handled)
                                {
                                    app_db.ignore_bau_first_gaming_cmd = APP_IGNORE_DONGLE_SPP_GAMING_CMD_RESET;
                                    app_db.ignore_bau_first_gaming_cmd_handled = true;
                                    if (app_db.restore_gaming_mode && (app_db.gaming_mode == 0))
                                    {
                                        //Restore gaming mode to on.
                                        app_db.gaming_mode_request_is_received = false;
                                        app_dongle_handle_gaming_mode_cmd(action);
                                    }

                                    app_db.restore_gaming_mode = false;
                                    break;
                                }

                                if ((p_data[4] == 1) && (app_db.gaming_mode == 0))
                                {
                                    // enable gaming mode
                                    app_db.gaming_mode_request_is_received = true;
                                    app_dongle_handle_gaming_mode_cmd(action);
                                }
                                else if ((p_data[4] == 0) && (app_db.gaming_mode == 1))
                                {
                                    //disable gaming mode
                                    app_db.gaming_mode_request_is_received = true;
                                    app_dongle_handle_gaming_mode_cmd(action);
                                }
                            }
                            else if (p_data[3] == DONGLE_CMD_REQ_OPEN_MIC)
                            {
#if F_APP_APT_SUPPORT
                                uint8_t action = MMI_NULL;
#endif

                                if (p_data[4] == 1)
                                {
                                    app_dongle_updata_mic_data_idx(true);

                                    app_transfer_queue_reset(CMD_PATH_SPP);

#if F_APP_APT_SUPPORT
                                    if ((app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT) &&
                                        app_apt_is_apt_on_state((T_ANC_APT_STATE)app_db.current_listening_state))
                                    {
                                        action = MMI_AUDIO_APT;
                                        app_db.dongle_is_disable_apt = true;
                                        app_dongle_control_apt(action);
                                    }
#endif

                                    app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_SPP_ENABLE);

                                    bool start_recording = true;

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                                    if (app_cfg_const.enable_24g_bt_audio_source_switch
                                        && (app_cfg_nv.allowed_source != ALLOWED_SOURCE_DONGLE))
                                    {
                                        start_recording = false;
                                    }
#endif

                                    if (start_recording)
                                    {
                                        app_dongle_update_is_mic_enable(true);
                                        app_dongle_start_recording(app_db.br_link[app_idx].bd_addr);
                                    }
                                }
                                else
                                {
                                    app_dongle_updata_mic_data_idx(true);

                                    app_dongle_update_is_mic_enable(false);
                                    app_dongle_stop_recording(app_db.br_link[app_idx].bd_addr);
                                    app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_SPP_DISABLE);
#if F_APP_APT_SUPPORT
                                    if (app_db.dongle_is_disable_apt &&
                                        (!app_apt_is_apt_on_state((T_ANC_APT_STATE)app_db.current_listening_state)))
                                    {
                                        action = MMI_AUDIO_APT;
                                        app_db.dongle_is_disable_apt = false;
                                        app_dongle_control_apt(action);
                                    }
#endif
                                }
                            }
                            else if (p_data[3] == DONGLE_CMD_SET_VOL_BALANCE)
                            {
                                if (p_data[4] == 1) // dongle to earphone
                                {
                                    // update record
                                    app_db.gaming_balance = p_data[5];
                                    app_db.chat_balance = p_data[6];
                                    app_dongle_volume_balance_report(DONGLE_CMD_ACK);
                                }
                            }
                            else if (p_data[3] == DONGLE_CMD_CTRL_RAW_DATA)
                            {
                                app_dongle_rcv_ctrl_data(p_data + 4, data_len - 4);
                            }
                            else if (p_data[3] == DONGLE_CMD_PASS_THROUGH_DATA)
                            {
                                if (p_data[4] == CMD_SYNC_BYTE) //AudioConnect cmd header check
                                {
                                    uint16_t cmd_len = (p_data[6] | (p_data[7] << 8)) + 4; //sync_byte, seqn, length_low, length_high
                                    uint8_t seqn = p_data[5];

                                    APP_PRINT_TRACE2("app_dongle_cmd_pass_through: 0x%02X, 0x%02X", cmd_len, seqn);

                                    app_cmd_handler(&p_data[8], (cmd_len - 4), CMD_PATH_SPP, seqn, app_idx);
                                }
                            }
#endif
#if F_APP_CFU_SPP_SUPPORT
                            else if (p_data[3] == DONGLE_CMD_CFU_DATA)
                            {
                                T_APP_CFU_DATA cfu_data;
                                memcpy(cfu_data.spp_data.bd_addr, param->spp_data_ind.bd_addr, 6);
                                if (app_cfu_spp_header_parse(payload_len - 1, p_data + 4, &cfu_data))
                                {
                                    app_cfu_handle_report(cfu_data.spp_data.report_id, APP_CFU_REPORT_SOURCE_SPP, &cfu_data);
                                }

                            }
#endif
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_SPP_CONN_IND:
        {
            if (param->spp_conn_ind.local_server_chann != RFC_SPP_DONGLE_CHANN_NUM)
            {
                return;
            }

            bt_spp_connect_cfm(param->spp_conn_ind.bd_addr, param->spp_conn_ind.local_server_chann, true,
                               param->spp_conn_ind.frame_size, 7);
        }
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            if (param->spp_disconn_cmpl.local_server_chann != RFC_SPP_DONGLE_CHANN_NUM)
            {
                return;
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true && (event_type != BT_EVENT_SPP_CREDIT_RCVD))
    {
        APP_PRINT_INFO1("app_dongle_spp_cback: event_type 0x%04x", event_type);
    }
}

#if F_APP_GAMING_DONGLE_SUPPORT
void app_dongle_report_pass_through_event(uint8_t *data, uint16_t data_len)
{
    app_dongle_send_cmd(DONGLE_CMD_PASS_THROUGH_DATA, data, data_len);
}

void app_dongle_gaming_mode_request(uint8_t status)
{
    app_dongle_send_cmd(DONGLE_CMD_REQUEST_GAMING_MOE, &status, 1);
}

void app_dongle_volume_balance_report(uint8_t msg_type)
{
    uint8_t buf[3] = {0};

    buf[0] = msg_type;
    buf[1] = app_db.gaming_balance;
    buf[2] = app_db.chat_balance;

    app_dongle_send_cmd(DONGLE_CMD_SET_VOL_BALANCE, buf, sizeof(buf));
}

void app_dongle_update_volume_balance(uint8_t gaming_balance, uint8_t chat_balance)
{
    // Valid value of gaming/chat balance: 0 to 100
    if (gaming_balance > VOL_BALANCE_VALUE_MAX)
    {
        gaming_balance = VOL_BALANCE_VALUE_MAX;
    }

    if (chat_balance > VOL_BALANCE_VALUE_MAX)
    {
        chat_balance = VOL_BALANCE_VALUE_MAX;
    }

    app_db.gaming_balance = gaming_balance;
    app_db.chat_balance   = chat_balance;
    app_dongle_volume_balance_report(DONGLE_CMD_UPDATE);
}

uint8_t app_dongle_get_mic_data_idx(void)
{
    return mic_data_idx;
}

void app_dongle_updata_mic_data_idx(bool is_reset)
{
    if (is_reset)
    {
        mic_data_idx = 0;
    }
    else
    {
        mic_data_idx++;
        if (mic_data_idx > 0x1F)
        {
            mic_data_idx = 0;
        }
    }
}

bool app_dongle_legacy_gaming_is_ready(void)
{
    return legacy_gaming_ready;
}

void app_dongle_legacy_gaming_event_cback(T_LEGACY_GAMING_EVENT event, uint8_t *addr)
{
    APP_PRINT_TRACE1("app_dongle_legacy_gaming_event_cback: event %d", event);

    switch (event)
    {
    case LEGACY_GAMING_READY:
        {
            legacy_gaming_ready = true;
        }
        break;

    case LEGACY_GAMING_STOP:
        {
            legacy_gaming_ready = false;
        }
        break;

    default:
        break;
    }
}
#endif

static void app_dongle_spp_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_cfg_const.output_ind3_link_mic_toggle)
            {
                app_mmi_handle_action(MMI_OUTPUT_INDICATION3_TOGGLE);
            }
#endif
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            uint8_t app_idx = app_a2dp_get_active_idx();

            if (app_db.dongle_is_enable_mic)
            {
                app_dongle_stop_recording(app_db.br_link[app_idx].bd_addr);
                app_mmi_handle_action(MMI_DEV_MIC_MUTE);
            }
#endif
        }
        break;

    default:
        break;
    }
}

void app_dongle_spp_init(void)
{
    bt_spp_service_register((uint8_t *)dongle_service_class_uuid128, RFC_SPP_DONGLE_CHANN_NUM);
    sys_mgr_cback_register(app_dongle_spp_dm_cback);
    bt_mgr_cback_register(app_dongle_spp_cback);

#if F_APP_GAMING_DONGLE_SUPPORT
    // set default volume balance to 100%
    app_db.gaming_balance = 100;
    app_db.chat_balance = 100;
#endif
}
