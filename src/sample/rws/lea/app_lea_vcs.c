#if F_APP_VCS_SUPPORT
#include "trace.h"
#include "gap_conn_le.h"
#include "vcs_mgr.h"
#include "app_lea_profile.h"
#include "app_lea_vol_def.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_link_util.h"

T_APP_LE_LINK *app_lea_vcs_find_vc_link(void)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if ((app_db.le_link[i].used == true) && (app_db.le_link[i].lea_device & APP_LEA_VCS_CCCD_ENABLED))
        {
            p_link = &app_db.le_link[i];
        }
    }

    return p_link;
}

static uint16_t app_lea_vcs_handle_cccd_msg(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;

    switch (msg)
    {
    case LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO:
        {
            T_SERVER_ATTR_CCCD_INFO *p_cccd = (T_SERVER_ATTR_CCCD_INFO *)buf;

            if (p_cccd->char_uuid == VCS_UUID_CHAR_VOLUME_STATE)
            {
                uint8_t conn_id;
                T_APP_LE_LINK *p_link;

                le_get_conn_id_by_handle(p_cccd->conn_handle, &conn_id);
                p_link = app_link_find_le_link_by_conn_id(conn_id);
                if (p_link)
                {
                    p_link->lea_device |= APP_LEA_VCS_CCCD_ENABLED;
                }
            }

            APP_PRINT_TRACE5("app_lea_vcs_handle_cccd_msg: conn_handle 0x%02X, service_id %d, char_uuid 0x%02X, ccc_bits 0x%02X, param %d",
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

static uint16_t app_lea_vcs_ble_audio_cback(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    uint16_t msg_group;

    msg_group = msg & 0xFF00;
    switch (msg_group)
    {
    case LE_AUDIO_MSG_GROUP_SERVER:
        cb_result = app_lea_vcs_handle_cccd_msg(msg, buf);
        break;

    default:
        break;
    }

    return cb_result;
}

void app_lea_vcs_init(void)
{
    T_BLE_AUDIO_VC_MIC_PARAMS vc_mic_param = {0};
    T_VCS_PARAM vcs_param;

    vc_mic_param.vcs_enable = true;
    ble_audio_vc_mic_init(&vc_mic_param);

    vcs_param.volume_setting = app_cfg_nv.lea_vol_setting;
    vcs_param.mute = app_cfg_nv.lea_vol_out_mute;
    vcs_param.change_counter = app_cfg_nv.lea_vcs_change_cnt;
    vcs_param.volume_flags = VCS_RESET_VOLUME_SETTING;
    vcs_param.step_size = VOLUME_STEP_SIZE;
    vcs_set_param(&vcs_param);
    ble_audio_cback_register(app_lea_vcs_ble_audio_cback);
}
#endif
