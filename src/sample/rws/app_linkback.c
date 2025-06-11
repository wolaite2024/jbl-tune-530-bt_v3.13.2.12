/*
* Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
*/

#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "os_sched.h"

#include "btm.h"
#include "bt_bond.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "bt_rdtp.h"
#include "bt_iap.h"
#include "bt_pbap.h"
#include "bt_hid.h"
#include "bt_spp.h"
#include "gap_le.h"

#include "engage.h"
#include "app_cfg.h"
#include "app_linkback.h"
#include "app_link_util.h"
#include "app_bond.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_bt_policy.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT || F_APP_COMMON_DONGLE_SUPPORT
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#endif
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_behaviors.h"
#endif

#if GFPS_SASS_SUPPORT
#include "bt_gfps.h"
#endif

#define LINKBACK_NODE_ITEM_NUM (8)

const uint32_t prof_arr[] =
{
    A2DP_PROFILE_MASK,
    DID_PROFILE_MASK,
    AVRCP_PROFILE_MASK,
    HFP_PROFILE_MASK,
    HSP_PROFILE_MASK,
    SPP_PROFILE_MASK,
    IAP_PROFILE_MASK,
    HID_PROFILE_MASK,
    PBAP_PROFILE_MASK,
};

const uint32_t prof_relation[3][2] =
{
    {A2DP_PROFILE_MASK, AVRCP_PROFILE_MASK},
    {HFP_PROFILE_MASK, PBAP_PROFILE_MASK},
    {HSP_PROFILE_MASK, PBAP_PROFILE_MASK},
};



static T_LINKBACK_NODE_ITEM *linkback_node_item[LINKBACK_NODE_ITEM_NUM] = {0};
static T_LINKBACK_TODO_QUEUE linkback_todo_queue = {0};
T_LINKBACK_ACTIVE_NODE linkback_active_node = {0};

static void linkback_print_active_node(void)
{
    ENGAGE_PRINT_TRACE2("linkback_print_active_node: bd_addr %s, total retry time %d ms",
                        TRACE_BDADDR(linkback_active_node.linkback_node.bd_addr), linkback_active_node.retry_timeout);

    ENGAGE_PRINT_TRACE8("linkback_print_active_node: is_valid %d, is_exit %d, is_force %d, is_sdp_ok %d, plan_profs 0x%08x, remain_profs 0x%08x, doing_prof 0x%08x, prof_retry_cnt %d",
                        linkback_active_node.is_valid, linkback_active_node.is_exit,
                        linkback_active_node.linkback_node.is_force, linkback_active_node.is_sdp_ok,
                        linkback_active_node.linkback_node.plan_profs, linkback_active_node.remain_profs,
                        linkback_active_node.doing_prof, linkback_active_node.prof_retry_cnt);
}

bool linkback_profile_is_need_search(uint32_t prof)
{
    bool ret = true;

    switch (prof)
    {
    case HID_PROFILE_MASK:
        {
            ret = false;
        }
        break;

    default:
        break;
    }

    return ret;
}

bool linkback_profile_search_start(uint8_t *bd_addr, uint32_t prof, bool is_special,
                                   T_LINKBACK_SEARCH_PARAM *param)
{
    bool ret = true;
    T_GAP_UUID_DATA uuid;
    T_GAP_UUID_TYPE uuid_type = GAP_UUID16;

#if F_APP_ALLOW_ONE_DONGLE_AND_ONE_PHONE_ONLY
    if (app_link_get_b2s_link_num() > 0)
    {
        uint32_t bond_flag = 0;
        bool ignore_create_connection = false;
        bool acl_link_existed = false;

        bt_bond_flag_get(bd_addr, &bond_flag);

        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);

        if (p_link != NULL && p_link->used)
        {
            acl_link_existed = true;
        }

        if (app_dongle_get_connected_dongle_link() == NULL)
        {
            if (((bond_flag & BOND_FLAG_DONGLE) == 0) && (acl_link_existed == false))
            {
                ignore_create_connection = true;
            }
        }
        else
        {
            if ((bond_flag & BOND_FLAG_DONGLE) && (acl_link_existed == false))
            {
                ignore_create_connection = true;
            }
        }

        if (ignore_create_connection)
        {
            APP_PRINT_TRACE1("linkback_profile_search_start: ignore %s", TRACE_BDADDR(bd_addr));

            return false;
        }
    }
#endif

    ENGAGE_PRINT_TRACE2("linkback_profile_search_start: bd_addr %s, prof 0x%08x", TRACE_BDADDR(bd_addr),
                        prof);

    if (DID_PROFILE_MASK == prof)
    {
        if (gap_br_start_did_discov(bd_addr) != GAP_CAUSE_SUCCESS)
        {
            ret = false;
        }

        return ret;
    }

    switch (prof)
    {
    case A2DP_PROFILE_MASK:
        {
            if (is_special)
            {
                if (param->is_source)
                {
                    uuid.uuid_16 = UUID_AUDIO_SOURCE;
                }
                else
                {
                    uuid.uuid_16 = UUID_AUDIO_SINK;
                }
            }
            else
            {
                uuid.uuid_16 = UUID_AUDIO_SOURCE;
            }
        }
        break;

    case AVRCP_PROFILE_MASK:
        {
            if (is_special)
            {
                if (param->is_target)
                {
                    uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_TARGET;
                }
                else
                {
                    uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_CONTROLLER;
                }
            }
            else
            {
                uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_TARGET;
            }
        }
        break;

    case HFP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_HANDSFREE_AUDIO_GATEWAY;
        }
        break;

    case HSP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_HEADSET_AUDIO_GATEWAY;
        }
        break;

    case SPP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_RFCOMM;
        }
        break;

    case IAP_PROFILE_MASK:
        {
            uint8_t iap_service_class[16] = {0x00, 0x00, 0x00, 0x00, 0xde, 0xca, 0xfa, 0xde,
                                             0xde, 0xca, 0xde, 0xaf, 0xde, 0xca, 0xca, 0xfe
                                            };
            uuid_type = GAP_UUID128;
            memcpy(&uuid.uuid_128[0], iap_service_class, 16);
        }
        break;

    case PBAP_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_PBAP_PSE;
        }
        break;

#if F_APP_HID_SUPPORT
    case HID_PROFILE_MASK:
        {
            uuid.uuid_16 = UUID_HUMAN_INTERFACE_DEVICE_SERVICE;
        }
        break;
#endif

    default:
        {
            ret = false;
        }
        break;
    }

    if (ret)
    {
        if (gap_br_start_sdp_discov(bd_addr, uuid_type, uuid) != GAP_CAUSE_SUCCESS)
        {
            ret = false;
        }
    }

    return ret;
}

bool linkback_profile_connect_start(uint8_t *bd_addr, uint32_t prof, T_LINKBACK_CONN_PARAM *param)
{
    bool ret = true;

    ENGAGE_PRINT_TRACE2("linkback_profile_connect_start: bd_addr %s, prof 0x%08x",
                        TRACE_BDADDR(bd_addr), prof);

    switch (prof)
    {
    case A2DP_PROFILE_MASK:
        ret = bt_a2dp_connect_req(bd_addr, param->protocol_version);
        break;

    case AVRCP_PROFILE_MASK:
        ret = bt_avrcp_connect_req(bd_addr);
        break;

    case HFP_PROFILE_MASK:
        ret = bt_hfp_connect_req(bd_addr, param->server_channel, true);
        break;

    case HSP_PROFILE_MASK:
        ret = bt_hfp_connect_req(bd_addr, param->server_channel, false);
        break;

    case SPP_PROFILE_MASK:
        ret = bt_spp_connect_req(bd_addr, param->server_channel, 1012, 7, param->local_server_chann);
        break;

    case IAP_PROFILE_MASK:
        ret = bt_iap_connect_req(bd_addr, param->server_channel, 1012, 7);
        break;

    case PBAP_PROFILE_MASK:
        ret = bt_pbap_connect_req(bd_addr, param->server_channel, param->feature);
        break;

#if F_APP_HID_SUPPORT
    case HID_PROFILE_MASK:
        ret = bt_hid_connect_req(bd_addr);
        break;
#endif

    default:
        ret = false;
        break;
    }

    return ret;
}

void linkback_profile_disconnect_start(uint8_t *bd_addr, uint32_t profs)
{
    ENGAGE_PRINT_TRACE2("linkback_profile_disconnect_start: bd_addr %s, prof 0x%08x",
                        TRACE_BDADDR(bd_addr), profs);

    if (0 != profs)
    {
        if (profs & A2DP_PROFILE_MASK)
        {
            bt_a2dp_disconnect_req(bd_addr);
        }

        if (profs & AVRCP_PROFILE_MASK)
        {
            bt_avrcp_disconnect_req(bd_addr);
        }

        if (profs & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
        {
            bt_hfp_disconnect_req(bd_addr);
        }

        if (profs & SPP_PROFILE_MASK)
        {
            bt_spp_disconnect_all_req(bd_addr);
        }

        if (profs & IAP_PROFILE_MASK)
        {
            bt_iap_disconnect_req(bd_addr);
        }

        if (profs & PBAP_PROFILE_MASK)
        {
            bt_pbap_disconnect_req(bd_addr);
        }

#if F_APP_HID_SUPPORT
        if (profs & HID_PROFILE_MASK)
        {
            bt_hid_disconnect_req(bd_addr);
        }
#endif

        if (profs & RDTP_PROFILE_MASK)
        {
            bt_rdtp_disconnect_req(bd_addr);
        }

#if F_APP_DURIAN_SUPPORT
        app_durian_link_disconn_req(bd_addr);
#endif

#if GFPS_SASS_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            if (profs & GFPS_PROFILE_MASK)
            {
                T_APP_BR_LINK *p_link = NULL;
                p_link = app_link_find_br_link(bd_addr);

                bt_gfps_disconnect_req(bd_addr, p_link->gfps_rfc_chann);
            }
        }
#endif
    }
    else
    {
        gap_br_send_acl_disconn_req(bd_addr);
    }
}

void linkback_todo_queue_init(void)
{
    uint32_t i;
    static bool is_calloc_done = false;
    T_LINKBACK_NODE_ITEM *p;

    if (!is_calloc_done)
    {
        is_calloc_done = true;

        ENGAGE_PRINT_TRACE0("linkback_todo_queue_init: calloc");

        p = calloc(LINKBACK_NODE_ITEM_NUM, sizeof(T_LINKBACK_NODE_ITEM));

        for (i = 0; i < LINKBACK_NODE_ITEM_NUM; i++)
        {
            linkback_node_item[i] = p + i;
        }
    }

    for (i = 0; i < LINKBACK_NODE_ITEM_NUM; i++)
    {
        memset(linkback_node_item[i], 0, sizeof(T_LINKBACK_NODE_ITEM));
    }

    linkback_todo_queue.head = NULL;
    linkback_todo_queue.tail = NULL;
}

T_LINKBACK_NODE_ITEM *linkback_todo_queue_malloc_node_item(void)
{
    uint32_t i;
    T_LINKBACK_NODE_ITEM *p_item = NULL;

    for (i = 0; i < LINKBACK_NODE_ITEM_NUM; i++)
    {
        if (!linkback_node_item[i]->is_used)
        {
            linkback_node_item[i]->is_used = true;
            p_item = linkback_node_item[i];
            break;
        }
    }

    return p_item;
}

void linkback_todo_queue_free_node_item(T_LINKBACK_NODE_ITEM *p_item)
{
    if (p_item != NULL)
    {
        p_item->is_used = false;
    }
}

void linkback_todo_queue_all_node(void)
{
    int i;
    T_LINKBACK_NODE_ITEM *p_item;

    i = 0;
    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        ENGAGE_PRINT_TRACE6("linkback_todo_queue_all_node: item_no %d, bd_addr %s, plan_profs 0x%x, is_force %d, is_group_member %d, retry_timeout %dms",
                            ++i, TRACE_BDADDR(p_item->linkback_node.bd_addr), p_item->linkback_node.plan_profs,
                            p_item->linkback_node.is_force, p_item->linkback_node.is_group_member,
                            p_item->linkback_node.retry_timeout);

        p_item = p_item->next;
    }
}

T_LINKBACK_NODE_ITEM *linkback_todo_queue_find_node_item(uint8_t *bd_addr)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        if (!memcmp(p_item->linkback_node.bd_addr, bd_addr, 6))
        {
            break;
        }
        p_item = p_item->next;
    }

    return p_item;
}

void linkback_todo_queue_insert_normal_node(uint8_t *bd_addr, uint32_t plan_profs,
                                            uint32_t retry_timeout, bool is_group_member)
{
    T_LINKBACK_NODE_ITEM *p_item;

    if (bd_addr == NULL || plan_profs == 0)
    {
        return;
    }

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
    if (app_link_check_dongle_link(bd_addr) &&
        /* if addr is dongle addr and dongle not connected yet;
        *  due to we need to connect a2dp profile when dongle acl exist for source switch
        */
        app_link_find_br_link(bd_addr) == NULL)
    {
        return;
    }
#endif

    p_item = linkback_todo_queue_find_node_item(bd_addr);
    if (p_item != NULL)
    {
        if (!is_group_member)
        {
            p_item->linkback_node.plan_profs |= plan_profs;
            p_item->linkback_node.retry_timeout = retry_timeout;
            p_item->linkback_node.is_group_member = false;
        }
    }
    else
    {
        p_item = linkback_todo_queue_malloc_node_item();
        if (p_item != NULL)
        {
            memcpy(p_item->linkback_node.bd_addr, bd_addr, 6);
            p_item->linkback_node.plan_profs = plan_profs;
            p_item->linkback_node.is_force = false;
            p_item->linkback_node.is_special = false;
            p_item->linkback_node.retry_timeout = retry_timeout;
            p_item->linkback_node.is_group_member = is_group_member;

            p_item->next = NULL;
            if (linkback_todo_queue.head == NULL)
            {
                linkback_todo_queue.head = p_item;
            }
            else
            {
                linkback_todo_queue.tail->next = p_item;
            }
            linkback_todo_queue.tail = p_item;
        }

        ENGAGE_PRINT_TRACE1("linkback_todo_queue_insert_normal_node: p_item = %p", p_item);
    }

    linkback_todo_queue_all_node();
}

void linkback_todo_queue_insert_force_node(uint8_t *bd_addr, uint32_t plan_profs,
                                           bool is_special, T_LINKBACK_SEARCH_PARAM *search_param, bool check_bond_flag,
                                           uint32_t retry_timeout, bool is_group_member)
{
    T_LINKBACK_NODE_ITEM *p_item;

    if (bd_addr == NULL || plan_profs == 0)
    {
        return;
    }

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
    if (app_link_check_dongle_link(bd_addr) &&
        /* if addr is dongle addr and dongle not connected yet;
        *  due to we need to connect a2dp profile when dongle acl exist for source switch
        */
        app_link_find_br_link(bd_addr) == NULL)
    {
        return;
    }
#endif

    p_item = linkback_todo_queue_find_node_item(bd_addr);
    if (p_item != NULL)
    {
        if (!is_group_member)
        {
            p_item->linkback_node.plan_profs |= plan_profs;
            p_item->linkback_node.is_force = true;
            p_item->linkback_node.is_special = is_special;
            p_item->linkback_node.retry_timeout = retry_timeout;
            p_item->linkback_node.is_group_member = false;
            p_item->linkback_node.check_bond_flag = check_bond_flag;
            if (search_param != NULL)
            {
                memcpy(&p_item->linkback_node.search_param, search_param, sizeof(T_LINKBACK_SEARCH_PARAM));
            }
        }
    }
    else
    {
        p_item = linkback_todo_queue_malloc_node_item();
        if (p_item != NULL)
        {
            memcpy(p_item->linkback_node.bd_addr, bd_addr, 6);
            p_item->linkback_node.plan_profs = plan_profs;
            p_item->linkback_node.is_force = true;
            p_item->linkback_node.is_special = is_special;
            p_item->linkback_node.retry_timeout = retry_timeout;
            p_item->linkback_node.is_group_member = is_group_member;
            p_item->linkback_node.check_bond_flag = check_bond_flag;
            if (search_param != NULL)
            {
                memcpy(&p_item->linkback_node.search_param, search_param, sizeof(T_LINKBACK_SEARCH_PARAM));
            }

            if (linkback_todo_queue.head == NULL)
            {
                linkback_todo_queue.tail = p_item;
                p_item->next = NULL;
            }
            else
            {
                p_item->next = linkback_todo_queue.head;
            }
            linkback_todo_queue.head = p_item;
        }

        ENGAGE_PRINT_TRACE1("linkback_todo_queue_insert_force_node: p_item = %p", p_item);
    }

    linkback_todo_queue_all_node();
}

bool linkback_todo_queue_take_first_node(T_LINKBACK_NODE *node)
{
    bool ret = false;
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    if (p_item != NULL)
    {
        memcpy(node, &p_item->linkback_node, sizeof(T_LINKBACK_NODE));

        if (linkback_todo_queue.head->next == NULL)
        {
            linkback_todo_queue.head = linkback_todo_queue.tail = NULL;
        }
        else
        {
            linkback_todo_queue.head = linkback_todo_queue.head->next;
        }

        linkback_todo_queue_free_node_item(p_item);
        ret = true;

        ENGAGE_PRINT_TRACE2("linkback_todo_queue_take_first_node: p_item = 0x%p, addr %s", p_item,
                            TRACE_BDADDR(p_item->linkback_node.bd_addr));
    }
    else
    {
        ENGAGE_PRINT_TRACE0("linkback_todo_queue_take_first_node: NULL");
    }

    linkback_todo_queue_all_node();

    return ret;
}

void linkback_todo_queue_remove_plan_profs(uint8_t *bd_addr, uint32_t plan_profs)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        if (!memcmp(p_item->linkback_node.bd_addr, bd_addr, 6))
        {
            p_item->linkback_node.plan_profs &= ~plan_profs;
        }
        p_item = p_item->next;
    }
}

void linkback_todo_queue_delete_all_node(void)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        linkback_todo_queue_free_node_item(p_item);
        p_item = p_item->next;
    }

    linkback_todo_queue.head = NULL;
    linkback_todo_queue.tail = NULL;
}

void linkback_todo_queue_delete_group_member(void)
{
    T_LINKBACK_NODE_ITEM *p, *q;

    ENGAGE_PRINT_TRACE0("linkback_todo_queue_delete_group_member");

    while (linkback_todo_queue.head && linkback_todo_queue.head->linkback_node.is_group_member)
    {
        linkback_todo_queue.head->is_used = false;
        linkback_todo_queue.head = linkback_todo_queue.head->next;
    }

    p = linkback_todo_queue.head;
    while (p && p->next)
    {
        q = p->next;
        if (q->linkback_node.is_group_member)
        {
            q->is_used = false;
            p->next = q->next;
        }
        else
        {
            p = q;
        }
    }

    linkback_todo_queue.tail = p;

    linkback_todo_queue_all_node();
}

void linkback_todo_queue_set_group_member_retry_timeout(uint32_t retry_timeout)
{
    T_LINKBACK_NODE_ITEM *p_item;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        if (p_item->linkback_node.is_group_member)
        {
            p_item->linkback_node.retry_timeout = retry_timeout;
        }
        p_item = p_item->next;
    }
}

uint32_t linkback_todo_queue_node_num(void)
{
    T_LINKBACK_NODE_ITEM *p_item;
    uint32_t i = 0;

    p_item = (T_LINKBACK_NODE_ITEM *)linkback_todo_queue.head;
    while (p_item != NULL)
    {
        p_item = p_item->next;
        i++;
    }

    return i;
}

void linkback_active_node_init(void)
{
    memset(&linkback_active_node, 0, sizeof(linkback_active_node));
}

void linkback_active_node_load_doing_prof(void)
{
    uint32_t i;

    linkback_active_node.doing_prof = 0;

    for (i = 0; i < sizeof(prof_arr) / sizeof(prof_arr[0]); i++)
    {
        if (prof_arr[i] & linkback_active_node.remain_profs)
        {
            linkback_active_node.doing_prof = prof_arr[i];
            break;
        }
    }
}

void linkback_active_node_load(T_LINKBACK_NODE *node)
{
    memset(&linkback_active_node, 0, sizeof(linkback_active_node));

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    uint32_t bond_flag;

    bt_bond_flag_get(node->bd_addr, &bond_flag);

    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE)
        {
            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                node->plan_profs &= ~A2DP_PROFILE_MASK;
                node->plan_profs &= ~AVRCP_PROFILE_MASK;
            }
        }
        else
        {
            if (bond_flag & BOND_FLAG_DONGLE)
            {
                node->plan_profs &= ~A2DP_PROFILE_MASK;
                node->plan_profs &= ~AVRCP_PROFILE_MASK;
            }
        }
    }
#endif

    if (node->plan_profs)
    {
        linkback_active_node.is_valid = true;
        linkback_active_node.is_exit = false;
        memcpy(&linkback_active_node.linkback_node, node, sizeof(T_LINKBACK_NODE));
        linkback_active_node.retry_timeout = node->retry_timeout;
        linkback_active_node.remain_profs = node->plan_profs;
        linkback_active_node.is_sdp_ok = false;
        linkback_active_node.prof_retry_cnt = 0;

        linkback_active_node_load_doing_prof();

        linkback_print_active_node();
    }
}

void linkback_active_node_step_suc_adjust_remain_profs(void)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_step_suc_adjust_remain_profs");

    linkback_active_node.remain_profs &= ~linkback_active_node.doing_prof;
    linkback_active_node.is_sdp_ok = false;
    linkback_active_node.prof_retry_cnt = 0;
    memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

    linkback_active_node_load_doing_prof();

    linkback_print_active_node();
}

void linkback_active_node_step_fail_adjust_remain_profs(void)
{
    uint32_t i;

    ENGAGE_PRINT_TRACE0("linkback_active_node_step_fail_adjust_remain_profs");

    linkback_active_node.is_sdp_ok = false;

    if (++linkback_active_node.prof_retry_cnt > 2)
    {
        linkback_active_node.remain_profs &= ~linkback_active_node.doing_prof;
        linkback_active_node.prof_retry_cnt = 0;
        memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

        for (i = 0; i < sizeof(prof_relation) / sizeof(prof_relation[0]); i++)
        {
            if (prof_relation[i][0] == linkback_active_node.doing_prof)
            {
                linkback_active_node.remain_profs &= ~prof_relation[i][1];
            }
        }

        linkback_active_node_load_doing_prof();
    }

    linkback_print_active_node();
}

void linkback_active_node_src_conn_fail_adjust_remain_profs(void)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_src_conn_fail_adjust_remain_profs");

    linkback_active_node.remain_profs = linkback_active_node.linkback_node.plan_profs;
    linkback_active_node.is_sdp_ok = false;
    linkback_active_node.prof_retry_cnt = 0;
    memset(&linkback_active_node.linkback_conn_param, 0, sizeof(T_LINKBACK_CONN_PARAM));

    linkback_active_node_load_doing_prof();

    linkback_print_active_node();
}

void linkback_active_node_remain_profs_add(uint32_t profs, bool check_bond_flag,
                                           uint32_t retry_timeout)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_remain_profs_add");

    linkback_active_node.linkback_node.plan_profs |= profs;
    linkback_active_node.remain_profs |= profs;
    linkback_active_node.retry_timeout = retry_timeout;
    linkback_active_node.linkback_node.retry_timeout = retry_timeout;
    linkback_active_node.linkback_node.is_group_member = false;
    linkback_active_node.linkback_node.check_bond_flag = check_bond_flag;
    if (0 == linkback_todo_queue_node_num())
    {
        linkback_active_node.linkback_node.gtiming = os_sys_time_get();
    }

    linkback_print_active_node();
}

void linkback_active_node_remain_profs_sub(uint32_t profs)
{
    ENGAGE_PRINT_TRACE0("linkback_active_node_remain_profs_sub");

    linkback_active_node.linkback_node.plan_profs &= ~profs;
    linkback_active_node.remain_profs &= ~profs;

    linkback_print_active_node();
}

bool linkback_active_node_judge_cur_conn_addr(uint8_t *bd_addr)
{
    bool ret = false;

    if (linkback_active_node.is_valid && bd_addr != NULL)
    {
        if (!memcmp(linkback_active_node.linkback_node.bd_addr, bd_addr, 6))
        {
            ret = true;
        }
    }

    return ret;
}

bool linkback_active_node_judge_cur_conn_prof(uint8_t *bd_addr, uint32_t prof)
{
    bool ret = false;

    if (linkback_active_node_judge_cur_conn_addr(bd_addr))
    {
        if (linkback_active_node.doing_prof == prof &&
            linkback_active_node.is_sdp_ok)
        {
            ret = true;
        }
    }

    return ret;
}

void linkback_load_bond_list(uint8_t skip_node_num, uint16_t retry_timeout)
{
    uint8_t i;
    uint8_t max_load_num;
    uint8_t bd_addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;
    uint32_t plan_profs;
    uint8_t num;

    linkback_todo_queue_delete_all_node();

    max_load_num = (app_cfg_const.maximum_linkback_number_high_bit << 3) |
                   app_cfg_const.maximum_linkback_number;

    num = 0;
    bond_num = app_bond_b2s_num_get();

#if F_APP_TEAMS_BT_POLICY
    uint8_t dongle_index = 0;
    T_TEAMS_DEVICE_TYPE device_type;
    for (uint8_t j = 1; j <= bond_num; j++)
    {
        if (app_bond_b2s_addr_get(j, bd_addr))
        {
            device_type = app_teams_bt_policy_get_cod_type_by_addr(bd_addr);
            if (device_type == T_TEAMS_DEVICE_DONGLE_TYPE)
            {
                dongle_index = j;
                break;
            }
        }
    }

    if (dongle_index)
    {
        bt_bond_flag_get(bd_addr, &bond_flag);
        plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
        linkback_todo_queue_insert_normal_node(bd_addr, plan_profs, retry_timeout * 1000, true);
        bond_flag = 0;
    }
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_dump_link_information();
#endif

#if F_APP_COMMON_DONGLE_SUPPORT
    if (app_cfg_const.linkback_to_dongle_first)
    {
        for (i = 1; i <= bond_num; i++)
        {
            bond_flag = 0;
            if (app_bond_b2s_addr_get(i, bd_addr))
            {
                bt_bond_flag_get(bd_addr, &bond_flag);
                if (bond_flag & BOND_FLAG_DONGLE)
                {
                    plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                    linkback_todo_queue_insert_normal_node(bd_addr, plan_profs, retry_timeout * 1000, true);
                    ++num;

                    // only linkback first dongle
                    break;
                }
            }
        }
    }
#endif

    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
#if F_APP_TEAMS_BT_POLICY
        if (dongle_index != i)
        {
#endif
            if (app_bond_b2s_addr_get(i, bd_addr))
            {
                if (skip_node_num > 0)
                {
                    ++num;
                    skip_node_num--;
                    continue;
                }

                bt_bond_flag_get(bd_addr, &bond_flag);

#if F_APP_COMMON_DONGLE_SUPPORT
#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.enable_dongle_dual_mode)
                {
                    if (app_dongle_is_24g_mode())
                    {
                        if ((bond_flag & BOND_FLAG_DONGLE) == 0)
                        {
                            /* only linkback dongle */
                            continue;
                        }
                    }
                    else
                    {
                        if (app_cfg_const.enable_multi_link == false)
                        {
                            if ((bond_flag & BOND_FLAG_DONGLE))
                            {
                                continue;
                            }
                        }
                    }
                }
#endif
                if (app_cfg_const.linkback_to_dongle_first)
                {
                    // ignore other dongles
                    if (bond_flag & BOND_FLAG_DONGLE)
                    {
                        continue;
                    }
                }
#endif
                if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                {
                    plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                    linkback_todo_queue_insert_normal_node(bd_addr, plan_profs, retry_timeout * 1000, true);

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
                    if ((bond_flag & BOND_FLAG_DONGLE) == 0)
#endif
                    {
                        if (++num >= max_load_num)
                        {
                            break;
                        }
                    }
                }
            }
#if F_APP_TEAMS_BT_POLICY
        }
#endif
    }

#if HARMAN_ADJUST_LINKBACK_RETRY_TIME_SUPPORT
    if (num > 1)
    {
        linkback_todo_queue_set_group_member_retry_timeout(retry_timeout * 1000 * 2);
    }
#endif

    ENGAGE_PRINT_TRACE5("linkback_load_bond_list: bond_num %d, num %d, skip_num %d, max_load_num %d, linkback_to_dongle_first %d",
                        bond_num, num, skip_node_num, max_load_num, app_cfg_const.linkback_to_dongle_first);
}

bool linkback_check_bond_flag(uint8_t *bd_addr, uint32_t prof)
{
    uint8_t i;
    uint8_t addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;
    uint32_t bond_profs;

    bond_num = app_bond_b2s_num_get();
    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (app_bond_b2s_addr_get(i, addr))
        {
            if (!memcmp(bd_addr, addr, 6))
            {
                bt_bond_flag_get(bd_addr, &bond_flag);
                bond_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                if (prof & bond_profs)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void linkback_active_node_use_left_time_insert_to_queue_again(bool is_normal)
{
    uint32_t time_used_ms = os_sys_time_get() - linkback_active_node.linkback_node.gtiming;

    if (linkback_active_node.linkback_node.retry_timeout > time_used_ms)
    {
        uint32_t retry_timeout = linkback_active_node.linkback_node.retry_timeout - time_used_ms;

        if (linkback_active_node.linkback_node.is_group_member)
        {
            linkback_todo_queue_set_group_member_retry_timeout(retry_timeout);
        }

        if (is_normal)
        {
            linkback_todo_queue_insert_normal_node(linkback_active_node.linkback_node.bd_addr,
                                                   linkback_active_node.linkback_node.plan_profs,
                                                   retry_timeout, linkback_active_node.linkback_node.is_group_member);
        }
        else
        {
            linkback_todo_queue_insert_force_node(linkback_active_node.linkback_node.bd_addr,
                                                  linkback_active_node.linkback_node.plan_profs,
                                                  linkback_active_node.linkback_node.is_special, &(linkback_active_node.linkback_node.search_param),
                                                  linkback_active_node.linkback_node.check_bond_flag, retry_timeout,
                                                  linkback_active_node.linkback_node.is_group_member);
        }
    }
    else
    {
        if (linkback_active_node.linkback_node.is_group_member)
        {
            linkback_todo_queue_delete_group_member();
        }
    }
}



