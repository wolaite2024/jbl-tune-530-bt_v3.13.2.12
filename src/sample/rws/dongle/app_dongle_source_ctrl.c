#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include <string.h>
#include "trace.h"
#include "bt_bond.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_dongle_source_ctrl.h"
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#include "app_bt_policy_api.h"
#include "app_link_util.h"
#include "app_bond.h"
#include "app_audio_policy.h"
#include "app_dongle_spp.h"
#include "multitopology_ctrl.h"
#include "app_lea_adv.h"
#include "app_lea_mgr.h"
#include "app_relay.h"
#include "app_dongle_record.h"

static void source_switch_to_dongle_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                   T_APP_BR_LINK *p_phone_link);
static void source_switch_to_bt_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                               T_APP_BR_LINK *p_phone_link);

#define GAMING_DONGLE_COD 0x240100

static void source_switch_to_dongle_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                                   T_APP_BR_LINK *p_phone_link)
{
    uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK;

    if (b2s_num > 1)
    {
        //connect dongle a2dp/avrcp profile
        app_bt_policy_default_connect(p_dongle_link->bd_addr, handle_prof, false);

        //disc phone a2dp/avrcp profile
        if (p_phone_link->connected_profile & handle_prof)
        {
            app_bt_policy_disconnect(p_phone_link->bd_addr, handle_prof);
        }
    }
    else if (b2s_num == 1)
    {
        if (p_dongle_link != NULL)
        {
            //connect dongle a2dp/avrcp profile
            app_bt_policy_default_connect(p_dongle_link->bd_addr, handle_prof, false);
        }
        else
        {
            //disc phone a2dp/avrcp profile
            if (p_phone_link->connected_profile & handle_prof)
            {
                app_bt_policy_disconnect(p_phone_link->bd_addr, handle_prof);
            }
        }
    }
    else
    {
        //do nothing.
    }
}

static void source_switch_to_bt_profile_handle(uint8_t b2s_num, T_APP_BR_LINK *p_dongle_link,
                                               T_APP_BR_LINK *p_phone_link)
{
    uint32_t handle_prof = A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK;

    if (b2s_num > 1)
    {
        //connect phone a2dp/avrcp profile
        app_bt_policy_default_connect(p_phone_link->bd_addr, handle_prof, false);

        //disc dongle a2dp/avrcp porifle
        if (p_dongle_link->connected_profile & handle_prof)
        {
            app_bt_policy_disconnect(p_dongle_link->bd_addr, handle_prof);
        }
    }
    else if (b2s_num == 1)
    {
        if (p_dongle_link != NULL)
        {
            //disc dongle a2dp/avrcp porifle
            if (p_dongle_link->connected_profile & handle_prof)
            {
                app_bt_policy_disconnect(p_dongle_link->bd_addr, handle_prof);
            }
        }
        else
        {
            //connect phone a2dp/avrcp profile
            app_bt_policy_default_connect(p_phone_link->bd_addr, handle_prof, false);
        }
    }
    else
    {
        //do nothing.
    }
}

#if F_APP_LEA_SUPPORT
void app_dongle_source_switch_to_dongle_le_audio_handle(void)
{
    mtc_cis_audio_conext_change(true);
    mtc_set_btmode(MULTI_PRO_BT_CIS);
}

void app_dongle_source_switch_to_bt_le_audio_handle(void)
{
    mtc_ase_release();
    mtc_cis_audio_conext_change(false);
    mtc_topology_dm(MTC_TOPO_EVENT_SET_BR_MODE);
}
#endif

void app_dongle_switch_allowed_source(void)
{
    app_cfg_nv.allowed_source ^= 1;

#if F_APP_DONGLE_MULTI_PAIRING
    if (app_cfg_const.enable_dongle_multi_pairing)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE)
        {
            app_cfg_nv.is_bt_pairing = 0;
        }
        else
        {
            app_cfg_nv.is_bt_pairing = 1;
        }
    }
#endif

    if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE)
    {
#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
        app_audio_update_dongle_flag(true);
#endif
        app_dongle_source_ctrl_evt_handler(EVENT_SOURCE_SWITCH_TO_DONGLE);

#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
        if (app_db.gaming_mode)
        {
            app_db.restore_gaming_mode = true;
            app_db.disallow_play_gaming_mode_vp = true;
            app_db.ignore_bau_first_gaming_cmd_handled = false;
            app_mmi_switch_gaming_mode();
        }

        if (app_db.dongle_is_enable_mic)
        {
            app_dongle_start_recording(app_db.connected_dongle_addr);
        }
#endif
        app_audio_tone_type_play(TONE_SWITCH_SOURCE_TO_DONGLE, true, true);
    }
    else
    {
#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
        // request dongle to exit gaming to lower down current
        app_db.ignore_bau_first_gaming_cmd = APP_IGNORE_DONGLE_SPP_GAMING_CMD_SRC_SWITCH;
        app_db.ignore_bau_first_gaming_cmd_handled = false;
        app_dongle_gaming_mode_request(false);

        app_audio_update_dongle_flag(false);
#endif

        app_dongle_source_ctrl_evt_handler(EVENT_SOURCE_SWITCH_TO_BT);

#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
        if (app_db.restore_gaming_mode && !app_db.gaming_mode)
        {
            app_db.restore_gaming_mode = false;
            app_db.disallow_play_gaming_mode_vp = true;
            app_mmi_switch_gaming_mode();
        }

        if (app_db.dongle_is_enable_mic)
        {
            app_dongle_stop_recording(app_db.connected_dongle_addr);
        }
#endif
        app_audio_tone_type_play(TONE_SWITCH_SOURCE_TO_BT, true, true);
    }

    APP_PRINT_TRACE1("app_dongle_switch_allowed_source: %d", app_cfg_nv.allowed_source);
}

void app_dongle_source_ctrl_evt_handler(T_SOURCE_CTRL_EVENT event)
{
    uint8_t b2s_num = app_link_get_b2s_link_num();
    T_APP_BR_LINK *p_dongle_link = NULL;
    T_APP_BR_LINK *p_phone_link = app_dongle_get_connected_phone_link();
    uint8_t le_audio_num = 0;

#if F_APP_LEA_SUPPORT
    le_audio_num = app_link_get_lea_link_num();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
    p_dongle_link =  app_dongle_get_connected_dongle_link();
#endif

    APP_PRINT_TRACE3("app_dongle_source_ctrl_evt_handler: event 0x%02x, b2s_num %d, le_audio %d",
                     event, b2s_num, le_audio_num);

    switch (event)
    {
    case EVENT_SOURCE_SWITCH_TO_BT:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                source_switch_to_bt_profile_handle(b2s_num, p_dongle_link, p_phone_link);
            }

#if F_APP_LEA_SUPPORT
            if (le_audio_num > 0)
            {
                app_dongle_source_switch_to_bt_le_audio_handle();
            }
#endif
        }
        break;

    case EVENT_SOURCE_SWITCH_TO_DONGLE:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                source_switch_to_dongle_profile_handle(b2s_num, p_dongle_link, p_phone_link);
            }

#if F_APP_LEA_SUPPORT
            if (le_audio_num > 0)
            {
                app_dongle_source_switch_to_dongle_le_audio_handle();
            }
            else
            {
                app_lea_adv_start();
            }
#endif

        }
        break;

    default:
        break;
    }

}

void app_dongle_source_ctrl_init(void)
{
}
#endif
