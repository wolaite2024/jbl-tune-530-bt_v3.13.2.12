/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_link_util.h"
#include "app_sniff_mode.h"
#include "app_main.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "app_bt_sniffing.h"
#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
#include "app_spp_capture_dsp_data.h"
#elif(F_APP_SPP_CAPTURE_DSP_DATA_2 == 1)
#include "app_data_capture.h"
#endif

typedef enum
{
    SNIFF_MODE_REMOTE_MSG_ROLESWAP_INFO = 0,
} T_SNIFF_MODE_REMOTE_MSG;

typedef struct
{
    uint16_t b2b_link_disable_flag;
    uint16_t b2s_disable_all_flag;
    uint16_t b2s_link_disable_flag;
} T_ROLESWAP_INFO;

static uint16_t g_b2s_disable_all_flag;
static bool g_have_disable_all;

#if F_APP_ERWS_SUPPORT
static void app_sniff_mode_b2b_enable_check(void);
static uint16_t g_b2b_link_disable_flag_temp;
static uint16_t g_b2s_link_disable_flag_temp;
#endif

#if F_APP_ERWS_SUPPORT
/*
 * sniff mode rules:
 *   1. pri decide sec
 *   2. b2s link decide b2b link
 *   3. b2s include: T_SNIFF_DISABLE_MASK
 *   4. b2s associate all link: ANC, a2dp, sco, power off
 */
void app_sniff_mode_send_roleswap_info(void)
{
    uint8_t app_idx;
    T_ROLESWAP_INFO roleswap_info;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        roleswap_info.b2b_link_disable_flag = 0;
        roleswap_info.b2s_disable_all_flag = g_b2s_disable_all_flag;
        roleswap_info.b2s_link_disable_flag = 0;

        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2b_link_by_id(app_idx))
            {
                roleswap_info.b2b_link_disable_flag = app_db.br_link[app_idx].sniff_mode_disable_flag;
            }
            else if (app_link_check_b2s_link_by_id(app_idx))
            {
                roleswap_info.b2s_link_disable_flag = app_db.br_link[app_idx].sniff_mode_disable_flag;
            }
        }

        APP_PRINT_TRACE3("app_sniff_mode_send_roleswap_info: b2b_link_disable_flag 0x%04x, b2s_disable_all_flag 0x%04x, b2s_link_disable_flag 0x%04x",
                         roleswap_info.b2b_link_disable_flag, roleswap_info.b2s_disable_all_flag,
                         roleswap_info.b2s_link_disable_flag);

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_SNIFF_MODE,
                                            (uint8_t *)&roleswap_info, sizeof(roleswap_info));
    }
}

void app_sniff_mode_recv_roleswap_info(void *buf, uint16_t len)
{
    T_ROLESWAP_INFO *p_info = (T_ROLESWAP_INFO *)buf;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (sizeof(*p_info) == len)
        {
            g_b2b_link_disable_flag_temp = p_info->b2b_link_disable_flag;
            g_b2s_disable_all_flag = p_info->b2s_disable_all_flag;
            g_b2s_link_disable_flag_temp = p_info->b2s_link_disable_flag;

            APP_PRINT_TRACE3("app_sniff_mode_recv_roleswap_info: b2b_link_disable_flag 0x%04x, b2s_disable_all_flag 0x%04x, b2s_link_disable_flag 0x%04x",
                             g_b2b_link_disable_flag_temp, g_b2s_disable_all_flag, g_b2s_link_disable_flag_temp);
        }
    }
}
#endif

void app_sniff_mode_startup(void)
{
#if F_APP_ERWS_SUPPORT
    g_b2s_link_disable_flag_temp = 0;
    g_b2b_link_disable_flag_temp = 0;
#endif
    g_b2s_disable_all_flag = 0;
    g_have_disable_all = false;
}


void app_sniff_mode_disable_all(void)
{
    uint8_t app_idx;

    APP_PRINT_TRACE0("app_sniff_mode_disable_all");

    g_have_disable_all = true;

    for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
    {
        if (app_db.br_link[app_idx].used)
        {
            bt_sniff_mode_disable(app_db.br_link[app_idx].bd_addr);
        }
    }
}

void app_sniff_mode_b2s_disable(uint8_t *bd_addr, uint16_t flag)
{
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            APP_PRINT_TRACE3("app_sniff_mode_b2s_disable: p_link 0x%p, curr flag 0x%04x, set flag 0x%04x",
                             p_link, p_link->sniff_mode_disable_flag, flag);

            p_link->sniff_mode_disable_flag |= flag;

            bt_sniff_mode_disable(bd_addr);

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                bt_sniff_mode_disable(app_cfg_nv.bud_peer_addr);
            }
#endif
        }
    }
}

void app_sniff_mode_b2s_enable(uint8_t *bd_addr, uint16_t flag)
{
    T_APP_BR_LINK *p_link;

#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
    if ((app_cfg_const.enable_dsp_capture_data_by_spp) &&
        (app_spp_capture_executing_check()))
    {
        return;
    }
#elif (F_APP_SPP_CAPTURE_DSP_DATA_2 == 1)
    if ((app_cfg_const.enable_dsp_capture_data_by_spp) &&
        (app_data_capture_executing_check()))
    {
        return;
    }
#endif

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            APP_PRINT_TRACE3("app_sniff_mode_b2s_enable: p_link 0x%p, curr flag 0x%04x, clear flag 0x%04x",
                             p_link, p_link->sniff_mode_disable_flag, flag);

            p_link->sniff_mode_disable_flag &= ~flag;

            if (0 == g_b2s_disable_all_flag &&
                0 == p_link->sniff_mode_disable_flag)
            {
                APP_PRINT_TRACE0("app_sniff_mode_b2s_enable: bt_sniff_mode_enable");

                bt_sniff_mode_enable(bd_addr, 0);
#if F_APP_ERWS_SUPPORT
                app_sniff_mode_b2b_enable_check();
#endif
            }
        }
    }
}

void app_sniff_mode_b2s_disable_all(uint16_t flag)
{
    uint8_t app_idx;

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_TRACE2("app_sniff_mode_b2s_disable_all: curr flag 0x%04x, set flag 0x%04x",
                         g_b2s_disable_all_flag, flag);

        g_b2s_disable_all_flag |= flag;

        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_db.br_link[app_idx].used)
            {
                bt_sniff_mode_disable(app_db.br_link[app_idx].bd_addr);
            }
        }
    }
}

void app_sniff_mode_b2s_enable_all(uint16_t flag)
{
    T_APP_BR_LINK *p_link;
    uint8_t app_idx;

#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
    if ((app_cfg_const.enable_dsp_capture_data_by_spp) &&
        (app_spp_capture_executing_check()))
    {
        return;
    }
#elif (F_APP_SPP_CAPTURE_DSP_DATA_2 == 1)
    if ((app_cfg_const.enable_dsp_capture_data_by_spp) &&
        (app_data_capture_executing_check()))
    {
        return;
    }
#endif

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_TRACE2("app_sniff_mode_b2s_enable_all: curr flag 0x%04x, set flag 0x%04x",
                         g_b2s_disable_all_flag, flag);

        g_b2s_disable_all_flag &= ~flag;

        if (0 == g_b2s_disable_all_flag)
        {
            for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
            {
                if (app_link_check_b2s_link_by_id(app_idx))
                {
                    p_link = &app_db.br_link[app_idx];

                    if (0 == p_link->sniff_mode_disable_flag)
                    {
                        APP_PRINT_TRACE0("app_sniff_mode_b2s_enable_all: bt_sniff_mode_enable");

                        bt_sniff_mode_enable(p_link->bd_addr, 0);
                    }
                }
            }

#if F_APP_ERWS_SUPPORT
            app_sniff_mode_b2b_enable_check();
#endif
        }
    }
}

void app_sniff_mode_b2s_check_left_flag_when_disconnect(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    if (!g_have_disable_all)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            p_link = app_link_find_br_link(bd_addr);
            if (p_link != NULL)
            {
                if (0 != p_link->sniff_mode_disable_flag)
                {
                    APP_PRINT_TRACE2("app_sniff_mode_b2s_check_left_flag_when_disconnect: p_link 0x%p, curr flag 0x%04x",
                                     p_link, p_link->sniff_mode_disable_flag);

                    p_link->sniff_mode_disable_flag = 0;

#if F_APP_ERWS_SUPPORT
                    app_sniff_mode_b2b_enable_check();
#endif
                }
            }
        }
    }
}

#if F_APP_ERWS_SUPPORT
void app_sniff_mode_b2b_disable(uint8_t *bd_addr, uint16_t flag)
{
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            APP_PRINT_TRACE3("app_sniff_mode_b2b_disable: p_link 0x%p, curr flag 0x%04x, set flag 0x%04x",
                             p_link, p_link->sniff_mode_disable_flag, flag);

            p_link->sniff_mode_disable_flag |= flag;

            bt_sniff_mode_disable(bd_addr);
        }
    }
}

void app_sniff_mode_b2b_enable(uint8_t *bd_addr, uint16_t flag)
{
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            APP_PRINT_TRACE3("app_sniff_mode_b2b_enable: p_link 0x%p, curr flag 0x%04x, clear flag 0x%04x",
                             p_link, p_link->sniff_mode_disable_flag, flag);

            p_link->sniff_mode_disable_flag &= ~flag;

            app_sniff_mode_b2b_enable_check();
        }
    }
}


static void app_sniff_mode_b2b_enable_check(void)
{
    uint8_t app_idx;
    bool have_b2b_disable_flag = false;
    bool have_b2s_disable_flag = false;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
            {
                if (app_link_check_b2b_link_by_id(app_idx))
                {
                    if (0 != app_db.br_link[app_idx].sniff_mode_disable_flag)
                    {
                        have_b2b_disable_flag = true;
                        break;
                    }
                }
                else if (app_link_check_b2s_link_by_id(app_idx))
                {
                    if (0 != app_db.br_link[app_idx].sniff_mode_disable_flag)
                    {
                        have_b2s_disable_flag = true;
                        break;
                    }
                }
            }

            if (!have_b2b_disable_flag && !g_b2s_disable_all_flag && !have_b2s_disable_flag)
            {
                APP_PRINT_TRACE0("app_sniff_mode_b2b_enable_check: bt_sniff_mode_enable");

                bt_sniff_mode_enable(app_cfg_nv.bud_peer_addr, 0);
            }
        }
    }
}

void app_sniff_mode_roleswap_suc(void)
{
    uint8_t app_idx;
    T_APP_BR_LINK *p_link;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2b_link_by_id(app_idx))
            {
                app_db.br_link[app_idx].sniff_mode_disable_flag = g_b2b_link_disable_flag_temp;

                APP_PRINT_TRACE1("app_sniff_mode_roleswap_suc: g_b2b_link_disable_flag_temp 0x%04x",
                                 g_b2b_link_disable_flag_temp);
            }
            else if (app_link_check_b2s_link_by_id(app_idx))
            {
                app_db.br_link[app_idx].sniff_mode_disable_flag = g_b2s_link_disable_flag_temp;

                APP_PRINT_TRACE1("app_sniff_mode_roleswap_suc: g_b2s_link_disable_flag_temp 0x%04x",
                                 g_b2s_link_disable_flag_temp);
            }
        }

        app_sniff_mode_b2b_enable_check();
    }
    else
    {
        p_link = app_link_find_br_link(app_cfg_nv.bud_peer_addr);
        if (p_link != NULL)
        {
            APP_PRINT_TRACE0("app_sniff_mode_roleswap_suc: bt_sniff_mode_enable");

            p_link->sniff_mode_disable_flag = 0;
            bt_sniff_mode_enable(app_cfg_nv.bud_peer_addr, 0);
        }
    }
}

#endif
