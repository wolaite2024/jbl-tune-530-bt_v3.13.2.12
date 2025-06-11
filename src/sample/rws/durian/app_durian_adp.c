/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <stdlib.h>
#include "app_durian.h"
#include "app_main.h"
#include "app_adp_cmd.h"
#include "app_cfg.h"

bool app_durian_adp_batt_need_report(uint8_t app_idx)
{
    bool need_report = false;
    if (((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) == 0) &&
        (app_cfg_const.display_bat_status_for_android != 0))
    {
        need_report = true;
    }
    return need_report;
}

#if DURIAN_AIRMAX
void app_durian_adp_batt_report(void)
{
    uint8_t local_batt, local_charging;

    if (!durian_cfg.one_trig_need)
    {
        return;
    }

    if (durian_db.adv_disallow_update_batt != 0)
    {
        local_batt = durian_db.local_batt;
    }
    else
    {
        local_batt = app_db.local_batt_level;
    }

    local_charging = (durian_db.local_loc == BUD_LOC_IN_CASE) ? 1 : 0;

    APP_PRINT_TRACE4("app_durian_adp_batt_report: local_batt %d local_charging %d adv_disallow_update_batt %d local_loc %d",
                     local_batt, local_charging, durian_db.adv_disallow_update_batt, durian_db.local_loc);

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & AVP_PROFILE_MASK)
        {
            APP_PRINT_TRACE0("app_durian_adp_batt_report: battery level reported");
            bt_avp_single_battery_level_report(app_db.br_link[i].bd_addr, local_batt, local_charging);
        }
    }
}
#else
void app_durian_adp_batt_report(void)
{
    uint8_t left_ear_charging, right_ear_charging;
    uint8_t local_charging, remote_charging;
    uint8_t left_ear_level, right_ear_level;
    uint8_t case_level;
    uint8_t case_status;
    uint8_t local_batt, remote_batt;

    if (!durian_cfg.one_trig_need)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            return;
        }
    }

    if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
    {
        case_level = app_db.case_battery & 0x7f;
        case_status = ((app_db.case_battery >> 7) == 0) ? 1 : 0;
    }
    else
    {
        case_level = 73;//for test
        case_status = 1;
    }

    if (durian_db.adv_disallow_update_batt != 0)
    {
        local_batt = durian_db.local_batt;
        remote_batt = durian_db.remote_batt;
    }
    else
    {
        local_batt = app_db.local_batt_level;
        remote_batt = app_db.remote_batt_level;
    }

    APP_PRINT_TRACE8("app_durian_adp_batt_report:local_loc %d remote_loc %d app_db.local_batt_level %d app_db.remote_batt_level %d bud_connected %d adv_disallow_update_batt %d local_batt %d remote_batt %d",
                     durian_db.local_loc, durian_db.remote_loc, app_db.local_batt_level, app_db.remote_batt_level,
                     app_db.remote_session_state, durian_db.adv_disallow_update_batt, durian_db.local_batt,
                     durian_db.remote_batt);

    local_charging = (durian_db.local_loc == BUD_LOC_IN_CASE) ? 1 : 0;
    remote_charging = (durian_db.remote_loc == BUD_LOC_IN_CASE) ? 1 : 0;

    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        left_ear_level = local_batt;
        left_ear_charging = local_charging;
        right_ear_level = remote_batt;
        right_ear_charging = remote_charging;
    }
    else
    {
        right_ear_level = local_batt;
        right_ear_charging = local_charging;
        left_ear_level = remote_batt;
        left_ear_charging = remote_charging;
    }

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ||
        (durian_db.role_decided != 0))
    {
        if ((local_charging && remote_charging) || (!local_charging && !remote_charging))
        {
            if (abs(right_ear_level - left_ear_level) < 10)
            {
                uint8_t min_level = (right_ear_level <= left_ear_level) ? right_ear_level : left_ear_level;
                left_ear_level = min_level;
                right_ear_level = min_level;
            }
        }
        if ((durian_db.local_loc != BUD_LOC_IN_CASE) && (durian_db.remote_loc != BUD_LOC_IN_CASE))
        {
            case_level = 0;
            case_status = 0x04;
        }
    }
    else
    {
        case_level = 0;
        case_status = 0x04;
        if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
        {
            right_ear_charging = 0x04;
        }
        else
        {
            left_ear_charging = 0x04;
        }
    }

    APP_PRINT_TRACE7("app_durian_adp_batt_report: right_ear_level %d right_ear_charging %d left_ear_level %d left_ear_charging %d case_level %d case_charging %d role_decided %d",
                     right_ear_level, right_ear_charging, left_ear_level, left_ear_charging, case_level, case_status,
                     durian_db.role_decided);

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & AVP_PROFILE_MASK)
        {
            APP_PRINT_TRACE0("app_durian_adp_batt_report: battery level reported");
            bt_avp_couple_battery_level_report(app_db.br_link[i].bd_addr, right_ear_level, right_ear_charging,
                                               left_ear_level, left_ear_charging, case_level, case_status);
        }
    }
}
#endif
#endif
