#include "trace.h"
#include "ble_audio.h"
#include "mcp_client.h"
#include "app_lea_mcp.h"
#include "app_link_util.h"

#if F_APP_MCP_SUPPORT
static uint16_t app_lea_mcp_active_conn_handle = 0;

static uint16_t app_lea_mcp_ble_audio_cback(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    bool handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_MCP_CLIENT_DIS_DONE:
        {
            T_APP_LE_LINK *p_link;
            T_MCP_CLIENT_DIS_DONE *p_dis_done = (T_MCP_CLIENT_DIS_DONE *)buf;

            p_link = app_link_find_le_link_by_conn_handle(p_dis_done->conn_handle);
            if (p_link == NULL)
            {
                return BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            if (p_dis_done->is_found)
            {
                if (p_dis_done->gmcs)
                {
                    mcp_client_cfg_cccd(p_dis_done->conn_handle,
                                        MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT | MCP_CLIENT_CFG_CCCD_FLAG_TRACK_CHANGED |
                                        MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_STATE,
                                        true, 0, true);
                    mcp_client_read_char_value(p_dis_done->conn_handle, 0, MCS_UUID_CHAR_MEDIA_STATE, true);
                    mcp_client_read_char_value(p_dis_done->conn_handle, 0, MCS_UUID_CHAR_MEDIA_PLAYER_NAME, true);
                    p_link->gmcs = p_dis_done->gmcs;
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
                        p_link->pre_media_state = p_link->media_state;
                        p_link->media_state = p_read_result->data.media_state;
                    }
                    APP_PRINT_TRACE1("app_lea_mcp_ble_audio_cback: LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT media_state 0x%02X",
                                     p_link->media_state);
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
                    // Because dongle set MCP states is pause forever, not change state.
                    // here is workaround, when headset is streaming, local state will be change.
                    if (p_notify_result->data.media_state != MCS_MEDIA_STATE_INACTIVE)
                    {
                        p_link->pre_media_state = p_link->media_state;
                        p_link->media_state = p_notify_result->data.media_state;
                    }
                    APP_PRINT_TRACE1("app_lea_mcp_ble_audio_cback: LE_AUDIO_MSG_MCP_CLIENT_NOTIFY media_state 0x%02X",
                                     p_notify_result->data.media_state);
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
        APP_PRINT_TRACE1("app_lea_mcp_ble_audio_cback: msg 0x%04X", msg);
    }

    return cb_result;
}

uint16_t app_lea_mcp_get_active_conn_handle(void)
{
    return app_lea_mcp_active_conn_handle;
}

void app_lea_mcp_reset_active_conn_handle(void)
{
    app_lea_mcp_active_conn_handle = 0;
}

bool app_lea_mcp_set_active_conn_handle(uint16_t conn_handle)
{
    bool ret = false;
    T_APP_LE_LINK *p_link;

    p_link = app_link_find_le_link_by_conn_handle(conn_handle);
    if (p_link != NULL)
    {
        app_lea_mcp_active_conn_handle = conn_handle;
        ret = true;
    }
    APP_PRINT_TRACE2("app_lea_mcp_set_active_conn_handle: active_mcp_conn_handle 0x%02X, ret 0x%02X",
                     app_lea_mcp_active_conn_handle, ret);
    return ret;
}

void app_lea_mcp_play_pause(uint16_t conn_handle, bool play)
{
    T_APP_LE_LINK *p_link;

    p_link = app_link_find_le_link_by_conn_handle(conn_handle);
    if (p_link)
    {
        bool valid_check = true;
        T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param;

        if ((p_link->media_state != MCS_MEDIA_STATE_PLAYING) && play)
        {
            param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY;
        }
        else if ((p_link->media_state == MCS_MEDIA_STATE_PLAYING) && !play)
        {
            param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
        }
        else
        {
            valid_check = false;
        }

        if (valid_check)
        {
            mcp_client_write_media_cp(p_link->conn_handle, 0, p_link->gmcs, &param, true);
        }

        APP_PRINT_TRACE3("app_lea_mcp_play_pause: conn_handle 0x%02X, play 0x%x, media_state 0x%x",
                         conn_handle, play, p_link->media_state);
    }
}

void app_lea_mcp_init(void)
{
    ble_audio_cback_register(app_lea_mcp_ble_audio_cback);
}
#endif
