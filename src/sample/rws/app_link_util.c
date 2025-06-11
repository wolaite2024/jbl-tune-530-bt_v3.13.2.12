/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdlib.h>
#include "os_mem.h"
#include "trace.h"
#include "remote.h"
#include "eq.h"
#include "audio_track.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_a2dp.h"
#include "bt_bond.h"
#include "app_bond.h"
#include "app_bt_point.h"

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_transport.h"
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "app_xiaowei_transport.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_dongle_common.h"
#endif

uint32_t app_link_conn_profiles(void)
{
    uint32_t i, connected_profiles = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        connected_profiles |= app_db.br_link[i].connected_profile;
    }

    return connected_profiles;
}

uint8_t app_link_get_sco_conn_num(void)
{
    return app_bt_point_br_link_sco_num_get();
}

uint8_t app_link_get_a2dp_start_num(void)
{
    uint8_t i;
    uint8_t num = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            app_db.br_link[i].streaming_fg == true)
        {
            num++;
        }
    }

    return num;
}

T_APP_BR_LINK *app_link_find_br_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_db.br_link[i].used == true &&
                !memcmp(app_db.br_link[i].bd_addr, bd_addr, 6))
            {
                p_link = &app_db.br_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_BR_LINK *app_link_find_br_link_by_tts_handle(T_TTS_HANDLE handle)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t i;

    if (handle != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if ((app_db.br_link[i].used == true) &&
                (app_db.br_link[i].tts_info.tts_handle == handle))
            {
                p_link = &app_db.br_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_BR_LINK *app_link_alloc_br_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_db.br_link[i].used == false)
            {
                p_link = &app_db.br_link[i];

                p_link->used = true;
                p_link->id   = i;
                memcpy(p_link->bd_addr, bd_addr, 6);
                break;
            }
        }
    }

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        if ((p_link != NULL) && (bd_addr != NULL))
        {
            app_xiaoai_alloc_br_link(bd_addr);
        }
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        if ((p_link != NULL) && (bd_addr != NULL))
        {
            app_xiaowei_alloc_br_link(bd_addr);
        }
    }
#endif

#if GFPS_SASS_SUPPORT
    // if (extend_app_cfg_const.gfps_sass_support)
    {
        if (p_link != NULL)
        {
            p_link->gfps_inuse_account_key = 0xFF;
        }
    }
#endif

    return p_link;
}

bool app_link_free_br_link(T_APP_BR_LINK *p_link)
{
    if (p_link != NULL)
    {
        if (p_link->used == true)
        {
            if (p_link->tts_info.tts_frame_buf != NULL)
            {
                free(p_link->tts_info.tts_frame_buf);
            }

            if (p_link->p_gfps_cmd != NULL)
            {
                free(p_link->p_gfps_cmd);
            }

            if (p_link->sco_track_handle != NULL)
            {
                audio_track_release(p_link->sco_track_handle);
            }

            if (p_link->a2dp_track_handle != NULL)
            {
                audio_track_release(p_link->a2dp_track_handle);
            }

            if (p_link->eq_instance != NULL)
            {
                eq_release(p_link->eq_instance);
            }

#if F_APP_VOICE_SPK_EQ_SUPPORT
            if (p_link->voice_mic_eq_instance != NULL)
            {
                eq_release(p_link->voice_mic_eq_instance);
            }
#endif
            memset(p_link, 0, sizeof(T_APP_BR_LINK));

            return true;
        }
    }

    return false;
}

T_APP_LE_LINK *app_link_find_le_link_by_conn_id(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].conn_id == conn_id)
        {
            p_link = &app_db.le_link[i];
            break;
        }
    }

    return p_link;
}

T_APP_LE_LINK *app_link_find_le_link_by_conn_handle(uint16_t conn_handle)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].conn_handle == conn_handle)
        {
            p_link = &app_db.le_link[i];
            break;
        }
    }

    return p_link;
}

T_APP_LE_LINK *app_link_find_le_link_by_addr(uint8_t *bd_addr)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if (app_db.le_link[i].used == true &&
                !memcmp(app_db.le_link[i].bd_addr, bd_addr, 6))
            {
                p_link = &app_db.le_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_LE_LINK *app_link_find_le_link_by_tts_handle(T_TTS_HANDLE handle)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    if (handle != NULL)
    {
        for (i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if ((app_db.le_link[i].used == true) &&
                (app_db.le_link[i].tts_info.tts_handle == handle))
            {
                p_link = &app_db.le_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_LE_LINK *app_link_alloc_le_link_by_conn_id(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == false)
        {
            p_link = &app_db.le_link[i];

            p_link->used    = true;
            p_link->id      = i;
            p_link->conn_id = conn_id;
            break;
        }
    }

#if GFPS_SASS_LEA_SUPPORT
    if (p_link != NULL)
    {
        p_link->gfps_inuse_account_key = 0xFF;
    }
#endif
    return p_link;
}

bool app_link_free_le_link(T_APP_LE_LINK *p_link)
{
    if (p_link != NULL)
    {
        if (p_link->used == true)
        {
            if (p_link->tts_info.tts_frame_buf != NULL)
            {
                free(p_link->tts_info.tts_frame_buf);
            }

            if (p_link->p_embedded_cmd != NULL)
            {
                free(p_link->p_embedded_cmd);
            }

            if (p_link->audio_eq_info.eq_data_buf != NULL)
            {
                free(p_link->audio_eq_info.eq_data_buf);
            }

            while (p_link->disc_cb_list.count > 0)
            {
                T_LE_DISC_CB_ENTRY *p_entry;
                p_entry = os_queue_out(&p_link->disc_cb_list);
                if (p_entry)
                {
                    free(p_entry);
                }
            }

            memset(p_link, 0, sizeof(T_APP_LE_LINK));
            p_link->conn_id = 0xFF;
#if GFPS_FEATURE_SUPPORT
            p_link->gfps_link.gfps_conn_id = 0xFF;
#endif
            return true;
        }
    }

    return false;
}

bool app_link_reg_le_link_disc_cb(uint8_t conn_id, P_FUN_LE_LINK_DISC_CB p_fun_cb)
{
    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link != NULL)
    {
        T_LE_DISC_CB_ENTRY *p_entry;

        for (uint8_t i = 0; i < p_link->disc_cb_list.count; i++)
        {
            p_entry = os_queue_peek(&p_link->disc_cb_list, i);
            if (p_entry != NULL && p_entry->disc_callback == p_fun_cb)
            {
                APP_PRINT_INFO1("app_link_reg_le_link_disc_cb: p_entry->disc_callback 0x%x",
                                p_entry->disc_callback);
                return true;
            }
        }

        p_entry = calloc(1, sizeof(T_LE_DISC_CB_ENTRY));
        if (p_entry != NULL)
        {
            p_entry->disc_callback = p_fun_cb;
            os_queue_in(&p_link->disc_cb_list, p_entry);
            APP_PRINT_INFO1("app_link_reg_le_link_disc_cb: p_entry->disc_callback 0x%x",
                            p_entry->disc_callback);
            return true;
        }
    }

    return false;
}

uint8_t app_link_get_le_link_num(void)
{
    return app_bt_point_le_link_num_get();
}

uint8_t app_link_get_le_encrypted_link_num(void)
{
    uint8_t num = 0;
    uint8_t i = 0;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true && app_db.le_link[i].encryption_status == LE_LINK_ENCRYPTIONED)
        {
            num++;
        }
    }

    return num;
}

#if F_APP_LEA_SUPPORT
uint8_t app_link_get_lea_link_num(void)
{
    return app_bt_point_lea_link_num_get();
}
#endif

#if F_APP_ERWS_SUPPORT
bool app_link_check_b2b_link(uint8_t *bd_addr)
{
    bool ret = false;

    if (!memcmp(bd_addr, app_cfg_nv.bud_local_addr, 6) ||
        !memcmp(bd_addr, app_cfg_nv.bud_peer_addr, 6))
    {
        ret = true;
    }

    return ret;
}

bool app_link_check_b2b_link_by_id(uint8_t id)
{
    bool ret = false;

    if (app_db.br_link[id].used)
    {
        ret = app_link_check_b2b_link(app_db.br_link[id].bd_addr);
    }

    return ret;
}
#endif

bool app_link_check_b2s_link(uint8_t *bd_addr)
{
#if F_APP_ERWS_SUPPORT
    return !app_link_check_b2b_link(bd_addr);
#else
    return true;
#endif
}

bool app_link_check_b2s_link_by_id(uint8_t id)
{
    bool ret = false;

    if (app_db.br_link[id].used)
    {
        ret = app_link_check_b2s_link(app_db.br_link[id].bd_addr);
    }

    return ret;
}

bool app_link_check_dongle_link(uint8_t *bd_addr)
{
    bool ret = false;
    uint8_t zero_addr[6] = {0};
#if F_APP_LEA_SUPPORT && F_APP_COMMON_DONGLE_SUPPORT
    T_APP_LE_LINK *dongle_le_link = app_dongle_get_le_audio_link();
#endif

    if (!memcmp(bd_addr, app_db.connected_dongle_addr, 6) && memcmp(bd_addr, zero_addr, 6))
    {
        ret = true;
    }
    else
    {
        uint32_t bond_flag = 0;

        if (bt_bond_flag_get(bd_addr, &bond_flag) &&
            (bond_flag & BOND_FLAG_DONGLE))
        {
            ret = true;
        }
    }

#if F_APP_LEA_SUPPORT && F_APP_COMMON_DONGLE_SUPPORT
    if (dongle_le_link != NULL)
    {
        if (memcmp(bd_addr, dongle_le_link->bd_addr, 6) == 0)
        {
            ret = true;
        }
    }
#endif

    return ret;
}

bool app_link_check_phone_link(uint8_t *bd_addr)
{
    bool ret = false;

    if (app_link_check_b2s_link(bd_addr) && !app_link_check_dongle_link(bd_addr))
    {
        ret = true;
    }

    return ret;
}

T_APP_BR_LINK *app_link_find_b2s_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;

    if (app_link_check_b2s_link(bd_addr))
    {
        p_link = app_link_find_br_link(bd_addr);
    }

    return p_link;
}

bool app_link_is_b2s_link_num_full(void)
{
    bool ret = false;
    uint8_t cause = 0;

    if (0)
    {
    }
    else if (app_link_get_b2s_link_num() >= app_db.b2s_connected_num_max)
    {
        cause = 1;
        ret = true;
    }
#if F_APP_GAMING_DONGLE_SUPPORT
    /* 2.4 mode only max connected num is 1 */
    else if (app_cfg_const.enable_dongle_dual_mode &&
             app_cfg_nv.is_dual_mode == false && app_link_get_b2s_link_num() == 1)
    {
        cause = 2;
        ret = true;
    }
#endif
#if F_APP_LEA_SUPPORT && (F_APP_GAMING_DONGLE_SUPPORT == 0)
    else if (app_bt_point_is_full() && app_bt_point_lea_link_is_full())
    {
        cause = 3;
        ret = true;
    }
#endif
    else
    {
        ret = false;
    }

    APP_PRINT_TRACE2("app_link_is_b2s_link_num_full:%d cause: %d", ret, cause);
    return ret;
}

uint8_t app_link_get_b2s_link_num(void)
{
    return app_bt_point_br_link_num_get();
}

void app_link_set_b2s_link_num(uint8_t num)
{
    app_bt_point_br_link_num_set(num);
}

uint8_t app_link_get_b2s_link_num_with_profile(uint32_t profile_mask)
{
    uint8_t i, link_number = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & profile_mask)
        {
            link_number++;
        }
    }

    return link_number;
}

uint8_t app_link_get_misc_num(T_APP_LINK_MISC channel)
{
    uint8_t num = 0;

    for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
    {
        if (app_link_check_b2s_link_by_id(app_idx))
        {
            if (channel == APP_LINK_STREAM)
            {
                if (app_db.br_link[app_idx].streaming_fg)
                {
                    num++;
                }
            }

#if F_APP_DURIAN_SUPPORT
            if (channel == APP_LINK_OPUS)
            {
                if (app_db.br_link[app_idx].audio_opus_status == AVP_VIOCE_RECOGNITION_ENCODE_START)
                {
                    num++;
                }
            }
            else if (channel == APP_LINK_AVP)
            {
                if (app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK)
                {
                    num++;
                }
            }
#endif
#if F_APP_SENSOR_MEMS_SUPPORT
            else if (channel == APP_LINK_MEMS)
            {
                if (app_db.br_link[app_idx].mems_is_start)
                {
                    num++;
                }
            }
#endif
        }
    }

    APP_PRINT_TRACE2("app_link_get_misc_num: channel %d num %d", channel, num);
    return num;
}

bool app_link_is_dongle_active_link(void)
{
    bool ret = false;
    uint32_t bond_flag;
    uint8_t current_active_a2dp_idx = app_a2dp_get_active_idx();

    if (bt_bond_flag_get(app_db.br_link[current_active_a2dp_idx].bd_addr, &bond_flag) == true)
    {
        if (bond_flag & BOND_FLAG_DONGLE)
        {
            ret = true;
        }
    }

    APP_PRINT_INFO3("app_link_is_dongle_active_link: %d, %d, %d", app_db.gaming_mode,
                    current_active_a2dp_idx, ret);
    return ret;
}

uint8_t app_link_get_connected_phone_num(void)
{
    uint8_t i = 0;
    uint32_t bond_flag = 0;
    uint8_t num = 0;

    /* check br link but not b2b or legacy dongle */
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used &&
#if F_APP_ERWS_SUPPORT
            !app_link_check_b2b_link(app_db.br_link[i].bd_addr) &&
#endif
            !app_link_check_dongle_link(app_db.br_link[i].bd_addr))
        {
            num++;
        }
    }

#if F_APP_LEA_SUPPORT
    /* Todo: check lea link but not lea dongle */
#endif

    return num;
}

uint8_t app_link_get_connected_src_num(void)
{
    uint8_t num = 0;
    uint8_t i = 0;

    /* br link src */
    num += app_link_get_b2s_link_num();

#if F_APP_LEA_SUPPORT
    /* lea link src and exclude the link with same br addr src link */
    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
        {
            if (app_link_find_br_link(app_db.le_link[i].bd_addr) == NULL)
            {
                num++;
            }
        }
    }
#endif

    return num;
}

uint8_t app_link_get_encryption_device_num(void)
{
    uint8_t num = 0;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used
#if F_APP_ERWS_SUPPORT
            && !app_link_check_b2b_link(app_db.br_link[i].bd_addr)
#endif
           )
        {
            num++;
        }
    }
    APP_PRINT_INFO1("app_link_get_encryption_device_num: temp num %d", num);

#if F_APP_LEA_SUPPORT
    /* lea link src and exclude the link with same br addr src link */
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].state == LE_LINK_STATE_CONNECTED &&
            app_db.le_link[i].encryption_status == LE_LINK_ENCRYPTIONED)
        {
            if (app_link_find_br_link(app_db.le_link[i].bd_addr) == NULL)
            {
                num++;
            }
        }
    }
#endif
    APP_PRINT_INFO1("app_link_get_encryption_device_num: num %d", num);
    return num;
}

#if F_APP_LEA_SUPPORT
void app_link_disallow_legacy_stream(bool disable)
{
    app_db.disallow_sniff = disable;

    if (!disable)
    {
        mtc_set_sniffing(MULTI_PRO_SNIFI_NOINVO);
    }
}
#endif


