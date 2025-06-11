#if F_APP_COMMON_DONGLE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "bt_bond.h"
#include "gap_br.h"
#include "sysm.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "app_bond.h"
#include "app_main.h"
#include "app_dongle_common.h"

#if F_APP_LEA_SUPPORT
#include "app_lea_mgr.h"
#endif

static void app_dongle_common_set_ext_eir(void)
{
    uint8_t p_eir[10];

    p_eir[0] = 9; /* length */
    p_eir[1] = 0xFF;
    p_eir[2] = 0x5D;
    p_eir[3] = 0x00;
    p_eir[4] = 0x08;

    /* bit0: 0- stereo headset 1- TWS headset
       bit1: 1- support LowLatency with Gaming Dongle
             0- not support LowLatency with Gaming Dongle
    */
    p_eir[5] = (remote_session_role_get() == REMOTE_SESSION_ROLE_SINGLE) ? 0x02 : 0x03;

    /*
         bit 3~0: 0: sbc frame nums in each avdtp packet depend on Gaming Dongle
    */
    p_eir[6] = 0x0;

    //Set pairing ID
    p_eir[7] = (app_cfg_const.rws_custom_uuid >> 8);
    p_eir[8] = app_cfg_const.rws_custom_uuid & 0xFF;

    /*
        bit 1~0: Set SPP Voice Sample Rate.
        bit   2: Set Multilink feature bit.
        bit 7~3: rsv.
    */
    p_eir[9] = app_cfg_const.spp_voice_smaple_rate & 0x03;
    p_eir[9] |= 0 << 2;

    gap_br_set_ext_eir(&p_eir[0], 10);
}

static void app_dongle_common_device_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_STACK_READY:
        {
            app_dongle_common_set_ext_eir();
        }
        break;

    default:
        break;
    }
}

bool app_dongle_is_streaming(void)
{
    bool ret = false;
    T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();

    if (p_dongle_link && p_dongle_link->streaming_fg && app_db.a2dp_play_status)
    {
        ret = true;
    }

    return ret;
}

T_APP_BR_LINK *app_dongle_get_connected_dongle_link(void)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t addr[6] = {0};

    if (app_dongle_get_connected_dongle_addr(addr))
    {
        p_link = app_link_find_br_link(addr);
    }

    return p_link;
}

T_APP_BR_LINK *app_dongle_get_connected_phone_link(void)
{
    T_APP_BR_LINK *p_link = NULL;
    uint32_t bond_flag;
    uint8_t i;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            bt_bond_flag_get(app_db.br_link[i].bd_addr, &bond_flag);

            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                p_link = app_link_find_br_link(app_db.br_link[i].bd_addr);
                break;
            }
        }
    }

    return p_link;
}

bool app_dongle_get_connected_dongle_addr(uint8_t *addr)
{
    bool ret = false;
    uint8_t i = 0;
    uint32_t bond_flag;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true)
        {
            bt_bond_flag_get(app_db.br_link[i].bd_addr, &bond_flag);

            if (bond_flag & BOND_FLAG_DONGLE)
            {
                memcpy(addr, app_db.br_link[i].bd_addr, 6);
                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool app_dongle_is_dongle_addr(uint8_t *check_addr)
{
    bool ret = false;
    uint32_t bond_flag = 0;

    if (bt_bond_flag_get(check_addr, &bond_flag) &&
        (bond_flag & BOND_FLAG_DONGLE))
    {
        ret = true;
    }

    return ret;
}

#if F_APP_LEA_SUPPORT
T_APP_LE_LINK *app_dongle_get_le_audio_link(void)
{
    T_APP_LE_LINK *p_lea_link = NULL;
    uint8_t i;

    if (app_link_get_le_link_num())
    {
        for (i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if (app_db.le_link[i].used &&
                app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
            {
                p_lea_link = &app_db.le_link[i];
                break;
            }
        }
    }

    return p_lea_link;
}

#if F_APP_GAMING_DONGLE_SUPPORT
void app_dongle_check_exit_pairing_state(uint8_t link_state, uint8_t *bd_addr)
{
    if (link_state == LEA_LINK_CONNECTED && app_bt_policy_is_pairing())
    {
        if (!app_bt_policy_check_keep_pairing_state(bd_addr))
        {
            app_bt_policy_exit_pairing_mode();
        }
    }
}
#endif
#endif

void app_dongle_common_init(void)
{
    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_dongle_common_device_event_cback);
}
#endif
