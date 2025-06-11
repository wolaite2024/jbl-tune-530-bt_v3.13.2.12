
#include <stdint.h>
#include <stdbool.h>
#include "trace.h"
#include "string.h"
#include "app_cfg.h"
#include "app_flags.h"
#include "app_link_util.h"
#include "app_bt_point.h"
#if F_APP_LEA_SUPPORT
#include "app_lea_broadcast_audio.h"
#include "app_lea_unicast_audio.h"
#endif

typedef struct
{
    uint8_t bd_addr[6];
    bool used;
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;
    bool connect_tone;
} T_APP_BT_POINT;

typedef struct
{
    T_APP_BT_POINT              bt_point[F_APP_MAX_BT_POINT_NUM];
    uint8_t                     bt_point_num;
    union
    {
        uint16_t link_num_all;
        struct
        {
            uint16_t            br : 2;
            uint16_t            sco : 2;
            uint16_t            le : 2;
            uint16_t            lea : 2;
            uint16_t            uca : 2;
            uint16_t            bca : 2;
            uint16_t            rsv : 4;
        } link_num;
    };
} T_APP_BT_POINT_MGR;

static T_APP_BT_POINT_MGR bt_point_mgr;

void app_bt_point_init(void)
{
    memset(&bt_point_mgr, 0, sizeof(T_APP_BT_POINT_MGR));
}

bool app_bt_point_is_empty(void)
{
    return (bt_point_mgr.bt_point_num == 0);
}

bool app_bt_point_is_full(void)
{
    return (bt_point_mgr.bt_point_num >= F_APP_MAX_BT_POINT_NUM);
}

uint8_t app_bt_point_num_get(void)
{
    return bt_point_mgr.bt_point_num;
}

bool app_bt_point_br_link_is_empty(void)
{
    return (bt_point_mgr.link_num.br == 0);
}

bool app_bt_point_br_link_is_full(void)
{
    return (bt_point_mgr.link_num.br >= F_APP_MAX_BT_POINT_NUM);
}

uint8_t app_bt_point_br_link_num_get(void)
{
    return bt_point_mgr.link_num.br;
}

void app_bt_point_br_link_num_set(uint8_t num)
{
    bt_point_mgr.link_num.br = num;
}

uint8_t app_bt_point_br_link_sco_num_get(void)
{
    return bt_point_mgr.link_num.sco;
}

bool app_bt_point_le_link_is_empty(void)
{
    return (bt_point_mgr.link_num.le == 0);
}

bool app_bt_point_le_link_is_full(void)
{
    return (bt_point_mgr.link_num.le >= F_APP_MAX_BT_POINT_NUM);
}

uint8_t app_bt_point_le_link_num_get(void)
{
    return bt_point_mgr.link_num.le;
}

bool app_bt_point_lea_link_is_empty(void)
{
    return (bt_point_mgr.link_num.lea == 0);
}

bool app_bt_point_lea_link_is_full(void)
{
    return (bt_point_mgr.link_num.lea >= F_APP_MAX_BT_POINT_NUM);
}

uint8_t app_bt_point_lea_link_num_get(void)
{
    return bt_point_mgr.link_num.lea;
}

T_BT_POINT_HANDLE app_bt_point_search(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
    {
        T_APP_BT_POINT *point = &bt_point_mgr.bt_point[i];
        if (point->used && !memcmp(point->bd_addr, bd_addr, 6))
        {
            return (T_BT_POINT_HANDLE)point;
        }
    }
    return NULL;
}

T_APP_BR_LINK *app_bt_point_br_link_get(T_BT_POINT_HANDLE handle)
{
    return (handle ? ((T_APP_BT_POINT *)handle)->br_link : NULL);
}

T_APP_LE_LINK *app_bt_point_le_link_get(T_BT_POINT_HANDLE handle)
{
    return (handle ? ((T_APP_BT_POINT *)handle)->le_link : NULL);
}

static T_BT_POINT_HANDLE app_bt_point_alloc(uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
    {
        T_APP_BT_POINT *point = &bt_point_mgr.bt_point[i];
        if (!point->used)
        {
            point->used = true;
            memcpy(point->bd_addr, bd_addr, 6);
            return (T_BT_POINT_HANDLE)point;
        }
    }
    return NULL;
}

static void app_bt_point_br_link_insert(uint8_t *bd_addr)
{
    T_BT_POINT_HANDLE handle;

    handle = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);

    if (handle == NULL && !app_bt_point_is_full())
    {
        handle = app_bt_point_alloc(bd_addr);
    }

    if (handle != NULL)
    {
        ((T_APP_BT_POINT *)handle)->br_link = app_link_find_br_link(bd_addr);
    }
}

static void app_bt_point_le_link_insert(uint8_t *bd_addr)
{
    T_BT_POINT_HANDLE handle;

    handle = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);

    if (handle == NULL && !app_bt_point_is_full())
    {
        handle = app_bt_point_alloc(bd_addr);
    }

    if (handle != NULL)
    {
        ((T_APP_BT_POINT *)handle)->le_link = app_link_find_le_link_by_addr(bd_addr);
    }
}

static void app_bt_point_br_link_remove(uint8_t *bd_addr)
{
    T_APP_BT_POINT *point = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);

    if (point != NULL)
    {
        point->br_link = NULL;
        if (point->le_link == NULL)
        {
            memset(point, 0, sizeof(T_APP_BT_POINT));
        }
    }
}

static void app_bt_point_le_link_remove(uint8_t *bd_addr)
{
    T_APP_BT_POINT *point = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);

    if (point != NULL)
    {
        point->le_link = NULL;
        if (point->br_link == NULL)
        {
            memset(point, 0, sizeof(T_APP_BT_POINT));
        }
    }
}

bool app_bt_point_link_permission_check(T_BP_LINK_TYPE link_type, uint8_t *bd_addr)
{
    bool suc = false;

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (link_type == BP_LINK_TYPE_B2S_LE)
        {
            suc = true;
        }
    }
    else
#endif
    {
        if (link_type == BP_LINK_TYPE_B2S_BR || link_type == BP_LINK_TYPE_B2S_LE)
        {
            if (!app_bt_point_is_full())
            {
                suc = true;
            }
            else
            {
                if (app_bt_point_search(bd_addr) != NULL)
                {
                    suc = true;
                }
            }
        }
    }

    APP_PRINT_INFO3("app_bt_point_link_permission_check: bd_addr %s, link_type 0x%x, suc %d",
                    TRACE_BDADDR(bd_addr), link_type, suc);

    return suc;
}

void app_bt_point_link_tone_set(uint8_t *bd_addr, bool tone)
{
    T_APP_BT_POINT *point = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);
    if (point != NULL)
    {
        point->connect_tone = tone;
    }
    else
    {
        APP_PRINT_ERROR0("app_bt_point_link_tone_set: point is NULL");
    }
}

bool app_bt_point_link_tone_get(uint8_t *bd_addr)
{
    T_APP_BT_POINT *point = (T_APP_BT_POINT *)app_bt_point_search(bd_addr);

    if (point != NULL)
    {
        return point->connect_tone;
    }
    return false;
}

void app_bt_point_link_num_changed(T_BP_LINK_TYPE link_type, T_BP_LINK_EVENT link_event,
                                   uint8_t *bd_addr)
{
    uint32_t i;
    T_APP_BT_POINT *point;

    APP_PRINT_INFO3("app_bt_point_link_num_changed: bd_addr %s, link_type 0x%x, link_event 0x%x",
                    TRACE_BDADDR(bd_addr), link_type, link_event);

    switch (link_type)
    {
    case BP_LINK_TYPE_B2S_BR:
        {
            if (link_event == BP_LINK_EVENT_CONNECT)
            {
                app_bt_point_br_link_insert(bd_addr);
            }
            else if (link_event == BP_LINK_EVENT_DISCONNECT)
            {
                app_bt_point_br_link_remove(bd_addr);
            }

            bt_point_mgr.bt_point_num = 0;
            bt_point_mgr.link_num.br = 0;

            for (i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
            {
                point = &bt_point_mgr.bt_point[i];
                if (point->used)
                {
                    bt_point_mgr.bt_point_num++;

                    if (point->br_link != NULL)
                    {
                        bt_point_mgr.link_num.br++;
                        APP_PRINT_INFO2("connected: b2s, bd_addr %s, profs 0x%08x",
                                        TRACE_BDADDR(point->bd_addr),
                                        point->br_link->connected_profile);
                    }
                }
            }
        }
        break;

    case BP_LINK_TYPE_B2S_SCO:
        {
            bt_point_mgr.link_num.sco = 0;
            for (i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
            {
                point = &bt_point_mgr.bt_point[i];
                if (point->used && point->br_link != NULL && point->br_link->sco_handle != 0)
                {
                    bt_point_mgr.link_num.sco++;
                    APP_PRINT_INFO1("connected: sco, bd_addr %s", TRACE_BDADDR(point->bd_addr));
                }
            }
        }
        break;

    case BP_LINK_TYPE_B2S_LE:
        {
            if (link_event == BP_LINK_EVENT_CONNECT)
            {
                app_bt_point_le_link_insert(bd_addr);
            }
            else if (link_event == BP_LINK_EVENT_DISCONNECT)
            {
                app_bt_point_le_link_remove(bd_addr);
            }

            bt_point_mgr.bt_point_num = 0;
            bt_point_mgr.link_num.le = 0;
            bt_point_mgr.link_num.lea = 0;

            for (i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
            {
                point = &bt_point_mgr.bt_point[i];
                if (point->used)
                {
                    bt_point_mgr.bt_point_num++;

                    if (point->le_link != NULL)
                    {
                        bt_point_mgr.link_num.le++;
#if F_APP_LEA_SUPPORT
                        if (point->le_link->lea_link_state != LEA_LINK_IDLE)
                        {
                            bt_point_mgr.link_num.lea++;
                        }
#endif
                        APP_PRINT_INFO1("connected: le, bd_addr %s", TRACE_BDADDR(point->bd_addr));
                    }
                }
            }
        }
        break;

#if F_APP_LEA_SUPPORT
    case BP_LINK_TYPE_B2S_LEA:
        {
            bt_point_mgr.link_num.lea = 0;
            for (i = 0; i < F_APP_MAX_BT_POINT_NUM; i++)
            {
                point = &bt_point_mgr.bt_point[i];
                if (point->used && point->le_link != NULL &&
                    point->le_link->lea_link_state != LEA_LINK_IDLE)
                {
                    bt_point_mgr.link_num.lea++;
                    APP_PRINT_INFO1("connected: lea, bd_addr %s", TRACE_BDADDR(point->bd_addr));
                }
            }
        }
        break;

    case BP_LINK_TYPE_B2S_UCA:
        {
            bt_point_mgr.link_num.uca = app_lea_uca_count_num();
        }
        break;

#if F_APP_TMAP_BMR_SUPPORT
    case BP_LINK_TYPE_B2S_BCA:
        {
            bt_point_mgr.link_num.bca = app_lea_bca_count_num();
        }
        break;
#endif
#endif

    default:
        break;
    }

    APP_PRINT_INFO6("connected: bud_role %d, b2s %d, le %d, lea %d, link_num 0x%x, bt_point_num %d",
                    app_cfg_nv.bud_role, bt_point_mgr.link_num.br, bt_point_mgr.link_num.le, bt_point_mgr.link_num.lea,
                    bt_point_mgr.link_num_all, bt_point_mgr.bt_point_num);
}
