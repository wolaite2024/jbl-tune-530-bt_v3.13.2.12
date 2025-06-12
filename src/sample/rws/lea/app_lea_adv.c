#include <string.h>
#include "trace.h"
#include "ascs_def.h"
#include "bass_def.h"
#include "pacs_def.h"
#include "vcs_def.h"
#include "mics_def.h"
#include "ble_ext_adv.h"
#include "bt_bond_le.h"
#include "csis_rsi.h"
#include "gatt.h"
#include "remote.h"
#include "app_timer.h"
#include "app_vendor.h"
#include "app_audio_policy.h"
#include "app_bt_point.h"
#include "app_bond.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_hfp.h"
#include "app_lea_csis.h"
#include "app_lea_mgr.h"
#include "app_lea_adv.h"
#include "app_link_util.h"
#include "app_lea_pacs.h"
#include "app_lea_profile.h"

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
#define LE_EXT_ADV_PSRI 0x0001
#define LE_EXT_ADV_ASCS 0x0002
#define LE_EXT_ADV_BASS 0x0004
#define LE_EXT_ADV_TMAS 0x0008
#define LE_EXT_ADV_PACS 0x0010
#define LE_EXT_ADV_VCS  0x0020
#define LE_EXT_ADV_MICS 0x0040

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
/*default max adv length include Flags(3), device name(42), CAS(5), ASCS(10), BASS(4), TMAS(6),
Service UUID(12), RSI(8), Appearance(4), Manufacture data(7).*/
#define APP_LEA_ADV_LEN         101
#else
/*default max adv length include Flags(3), device name(42), CAS(5), ASCS(10), BASS(4), TMAS(6),
Service UUID(12), RSI(8), Appearance(4).*/
#define APP_LEA_ADV_LEN         94
#endif

#define APP_LEA_FAST_ADV_TIME   30000

typedef enum
{
    LEA_ADV_TIMER_FAST_INTERVAL,
    LEA_ADV_TIMER_LINKBACK,
} T_APP_LEA_ADV_TIMER;

static uint8_t app_lea_adv_timer_id = 0;
static uint8_t timer_idx_slow_interval = 0;
static uint8_t timer_idx_lea_linkback = 0;
static bool lea_adv_started = false;
static bool abnormal_disconnect_linkback = false;
static T_BLE_EXT_ADV_MGR_STATE app_lea_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
static uint8_t app_lea_adv_handle = 0xFF;
static uint8_t app_lea_adv_data[APP_LEA_ADV_LEN];
static uint8_t app_lea_adv_scan_rsp_data[] =
{
    0x03,
    GAP_ADTYPE_APPEARANCE,
    LO_WORD(GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
            GAP_APPEARANCE_SUB_CATEGORY_EARBUD),
    HI_WORD(GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
            GAP_APPEARANCE_SUB_CATEGORY_EARBUD),
};

bool app_lea_adv_linkbacking(void)
{
    bool ret = false;

    if (timer_idx_lea_linkback)
    {
        ret = true;
    }
    APP_PRINT_INFO1("app_lea_adv_linkbacking: %d", ret);
    return ret;
}

void app_lea_adv_linkback_set(bool linkback)
{
    abnormal_disconnect_linkback = linkback;
    APP_PRINT_INFO1("app_lea_adv_linkback_set: %d", linkback);
}

bool app_lea_adv_linkback_check(void)
{
    bool ret = false;
    static bool power_on_linkback = true;

    if (!app_cfg_const.enable_power_on_linkback || app_bt_policy_valid_b2s_bond_bum())
    {
        power_on_linkback = false;
    }

    if ((app_bt_point_lea_link_num_get() >= app_lea_mgr_lea_bond_num()) ||
        app_bt_point_le_link_is_full())
    {
        power_on_linkback = false;
        if (timer_idx_lea_linkback)
        {
            app_stop_timer(&timer_idx_lea_linkback);
        }
    }
    else if (app_bt_policy_get_state() == BP_STATE_LINKBACK)
    {
        ret = true;
    }
    else if ((app_bt_policy_get_state() == BP_STATE_IDLE) ||
             (app_bt_policy_get_state() == BP_STATE_STANDBY))
    {
        if ((power_on_linkback || timer_idx_lea_linkback || abnormal_disconnect_linkback) &&
            app_lea_mgr_lea_bond_num())
        {
            ret = true;
        }

        if (ret && (!timer_idx_lea_linkback || abnormal_disconnect_linkback))
        {
            power_on_linkback = false;
            app_start_timer(&timer_idx_lea_linkback, "lea_linkback_timer",
                            app_lea_adv_timer_id, LEA_ADV_TIMER_LINKBACK, 0, false,
                            APP_LEA_FAST_ADV_TIME);
            // app_lea_adv_linkback_timer();
        }
    }
    else if (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
    {
        if (timer_idx_lea_linkback)
        {
            app_stop_timer(&timer_idx_lea_linkback);
        }
    }

    APP_PRINT_INFO6("app_lea_adv_linkback_check: ret %d, b2s_bond %d, enable_linkback %d, state %d, lea_bond %d, lea_num %d",
                    ret, app_bt_policy_valid_b2s_bond_bum(), app_cfg_const.enable_power_on_linkback,
                    app_bt_policy_get_state(), app_lea_mgr_lea_bond_num(), app_link_get_lea_link_num());
    return ret;
}

static void app_lea_adv_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_INFO1("app_lea_adv_timeout_cb: timer_evt %d", timer_evt);

    switch (timer_evt)
    {
    case LEA_ADV_TIMER_FAST_INTERVAL:
        {
		//ysc start
            app_gfps_adv_update_adv_interval(400);
            ENGAGE_PRINT_TRACE0("------------app_lea_adv_timeout_cb app_gfps_adv_update_adv_interval (400) -------------");    
		//ysc end	        
            app_stop_timer(&timer_idx_slow_interval);

            if (app_bt_point_lea_link_is_full())
            {
                app_lea_adv_stop();
            }
            else if (ble_ext_adv_mgr_get_adv_interval(app_lea_adv_handle) == APP_LEA_ADV_INTERVAL_FAST)
            {
                app_lea_adv_update();
                ble_ext_adv_mgr_change_adv_interval(app_lea_adv_handle, APP_LEA_ADV_INTERVAL_SLOW);
            }
        }
        break;

    case LEA_ADV_TIMER_LINKBACK:
        {
            abnormal_disconnect_linkback = false;
            app_stop_timer(&timer_idx_lea_linkback);
            app_lea_adv_update();
            app_bt_policy_enter_sched();
        }
        break;

    default:
        break;
    }
}

static bool app_lea_adv_is_discoverable(void)
{
    bool ret = false;

    if ((app_bt_policy_get_radio_mode() == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE) ||
        (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE))
    {
        ret = true;
    }
    else if (!app_cfg_const.legacy_enable)
    {
        uint32_t state;

        app_timer_state_get(&timer_idx_slow_interval, &state);
        if (state || app_lea_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
        {
            ret = true;
        }
    }

    APP_PRINT_INFO2("app_lea_adv_is_discoverable: radio_mode 0x%02x, state 0x%02x",
                    app_bt_policy_get_radio_mode(), app_bt_policy_get_state());
    return ret;
}

/*Unicast Server that sent the PDU is available for a general audio use case.
In this specification, a general audio use case means the transmission or reception of Audio Data
that has not been initiated by a higher-layer specification.*/
static uint16_t app_lea_adv_set_ext_data(uint16_t audio_adv_flag, uint8_t service_num,
                                         uint8_t announcement_type)
{
    uint16_t idx = 0;
    uint16_t appearance = 0;
    uint8_t lea_flags = 0;
    uint8_t name_len = strlen((const char *)app_cfg_nv.device_name_le);

    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        appearance = GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
                     GAP_APPEARANCE_SUB_CATEGORY_EARBUD;
    }
    else
    {
        appearance = GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
                     GAP_APPEARANCE_SUB_CATEGORY_HEADSET;
    }

    app_lea_adv_data[idx++] = 0x02;
    app_lea_adv_data[idx++] = GAP_ADTYPE_FLAGS;

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        lea_flags = GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED;
    }
    else
#endif
    {
        lea_flags = GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER |
                    GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST;


        if (app_cfg_const.legacy_enable == 0)
        {
            lea_flags |= GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED;
        }

        if (app_lea_adv_is_discoverable())
        {
            lea_flags |= GAP_ADTYPE_FLAGS_LIMITED;
        }
    }
    app_lea_adv_data[idx++] = lea_flags;

    app_lea_adv_data[idx++] = 0x04;
    app_lea_adv_data[idx++] = GAP_ADTYPE_SERVICE_DATA;
    app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_CAS);
    app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_CAS);
    app_lea_adv_data[idx++] = announcement_type;

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_ASCS)
    {
        app_lea_adv_data[idx++] = 0x09;
        app_lea_adv_data[idx++] = GAP_ADTYPE_SERVICE_DATA;
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_ASCS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_ASCS);
        app_lea_adv_data[idx++] = announcement_type;

        LE_UINT16_TO_ARRAY(app_lea_adv_data + idx,
                           app_lea_pacs_get_sink_available_contexts());
        idx += 2;
        LE_UINT16_TO_ARRAY(app_lea_adv_data + idx,
                           app_lea_pacs_get_source_available_contexts());
        idx += 2;
        app_lea_adv_data[idx++] = 0; // metadata length
    }
#endif

#if F_APP_TMAP_BMR_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_BASS)
    {
        app_lea_adv_data[idx++] = 0x03;
        app_lea_adv_data[idx++] = GAP_ADTYPE_SERVICE_DATA;
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_BASS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_BASS);
    }
#endif

#if F_APP_TMAS_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_TMAS)
    {
        uint16_t role = app_lea_profile_get_tmas_role();

        app_lea_adv_data[idx++] = 0x05;
        app_lea_adv_data[idx++] = GAP_ADTYPE_SERVICE_DATA;
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_TMAS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_TMAS);
        app_lea_adv_data[idx++] = LO_WORD(role);
        app_lea_adv_data[idx++] = HI_WORD(role);
    }
#endif

    app_lea_adv_data[idx++] = service_num * 2 + 1;
    app_lea_adv_data[idx++] = GAP_ADTYPE_16BIT_MORE;
#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_ASCS)
    {
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_ASCS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_ASCS);
    }
#endif
#if F_APP_TMAP_BMR_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_BASS)
    {
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_BASS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_BASS);
    }
#endif
#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_PACS)
    {
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_PACS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_PACS);
    }
#endif
#if F_APP_VCS_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_VCS)
    {
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_VCS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_VCS);
    }
#endif
#if F_APP_MICS_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_MICS)
    {
        app_lea_adv_data[idx++] = LO_WORD(GATT_UUID_MICS);
        app_lea_adv_data[idx++] = HI_WORD(GATT_UUID_MICS);
    }
#endif

#if F_APP_CSIS_SUPPORT
    if (audio_adv_flag & LE_EXT_ADV_PSRI)
    {
        uint8_t psri_data[CSI_RSI_LEN];

        if (csis_gen_rsi(app_cfg_nv.lea_sirk, psri_data))
        {
            app_lea_adv_data[idx++] = CSI_RSI_LEN + 1;
            app_lea_adv_data[idx++] = GAP_ADTYPE_RSI;
            memcpy(app_lea_adv_data + idx, psri_data, CSI_RSI_LEN);
            idx += CSI_RSI_LEN;
        }
    }
#endif

    app_lea_adv_data[idx++] = 0x03;
    app_lea_adv_data[idx++] = GAP_ADTYPE_APPEARANCE;
    app_lea_adv_data[idx++] = LO_WORD(appearance);
    app_lea_adv_data[idx++] = HI_WORD(appearance);

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
    app_lea_adv_data[idx++] = 0x06;
    app_lea_adv_data[idx++] = LO_WORD(app_cfg_const.rws_custom_uuid);
    app_lea_adv_data[idx++] = HI_WORD(app_cfg_const.rws_custom_uuid);
    app_lea_adv_data[idx++] = app_db.pairing_bit;

    /* use the last 3 byte of mac of factory bud role primary as bud id */
    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_lea_adv_data[idx++] = app_db.factory_addr[0];
        app_lea_adv_data[idx++] = app_db.factory_addr[1];
        app_lea_adv_data[idx++] = app_db.factory_addr[2];
    }
    else if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_lea_adv_data[idx++] = app_cfg_nv.bud_peer_factory_addr[0];
        app_lea_adv_data[idx++] = app_cfg_nv.bud_peer_factory_addr[1];
        app_lea_adv_data[idx++] = app_cfg_nv.bud_peer_factory_addr[2];
    }
#endif

    if (app_lea_adv_is_discoverable())
    {
        app_lea_adv_data[idx++] = name_len + 1;
        app_lea_adv_data[idx++] = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
        memcpy(&app_lea_adv_data[idx], app_cfg_nv.device_name_le, name_len);
        idx += name_len;
    }

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        app_gfps_update_announcement(announcement_type);
    }
#endif

    return idx;
}

void app_lea_adv_set_started_flag(bool flag)
{
    lea_adv_started = flag;
}

void app_lea_adv_check_restart_adv(void)
{
    if (app_lea_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        APP_PRINT_TRACE0("app_lea_adv_check_restart_adv: already enabled");
    }
    else if (lea_adv_started == true)
    {
        app_lea_adv_start();
    }
}

static void app_lea_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;

    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            APP_PRINT_TRACE4("app_lea_adv_callback: change from %d to %d, adv_handle %d, stop_cause %d",
                             app_lea_adv_state, cb_data.p_ble_state_change->state,
                             cb_data.p_ble_state_change->adv_handle, cb_data.p_ble_state_change->stop_cause);

            app_lea_adv_state = cb_data.p_ble_state_change->state;
            if (app_lea_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                app_lea_adv_set_started_flag(true);
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        {
            T_APP_LE_LINK *p_link;

            p_link = app_link_find_le_link_by_conn_id(cb_data.p_ble_conn_info->conn_id);
            if (p_link && p_link->lea_device)
            {
#if GFPS_LE_DEVICE_SUPPORT
                //need to start adv for GFPS LE audio multilink
                if (extend_app_cfg_const.gfps_le_device_support)
                {
                    app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
                }
#endif
            }
            else
            {
                app_lea_adv_check_restart_adv();
            }
        }
        break;

    default:
        break;
    }
    return;
}

void app_lea_adv_start(void)
{
    if (app_cfg_nv.lea_mode_enable == false)
    {
        APP_PRINT_INFO0("app_lea_adv_start: LEA mode disable");
        return;
    }

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        if (((app_cfg_const.iso_mode & LE_AUDIO_CIS) ||
             (app_cfg_const.bis_mode == LEA_BROADCAST_DELEGATOR)) &&
            (app_bt_point_lea_link_is_full() == false))
        {
            APP_PRINT_INFO0("app_lea_adv_start"); //Reserve log for HRP test

            //TODO:
            //use general announcement + non-direct adv for general connection,
            //use target announcement + direct adv for linkback connection

            ble_ext_adv_mgr_enable(app_lea_adv_handle, 0);

            ble_ext_adv_mgr_change_adv_interval(app_lea_adv_handle, APP_LEA_ADV_INTERVAL_FAST);

            app_start_timer(&timer_idx_slow_interval, "fast_interval",
                            app_lea_adv_timer_id, LEA_ADV_TIMER_FAST_INTERVAL, 0, false,
                            APP_LEA_FAST_ADV_TIME);

            //Use smallest offset between Extended Advertising packet and auxiliary packet for quick connection
            app_vendor_set_adv_extend_misc(0, 1);
        }
    }
}

void app_lea_adv_stop(void)
{
    ble_ext_adv_mgr_disable(app_lea_adv_handle, 0);
}

void app_lea_adv_update_interval(uint16_t interval)
{
    ble_ext_adv_mgr_change_adv_interval(app_lea_adv_handle, interval);
}

void app_lea_adv_update_interval_hfp(void)
{
    if (app_lea_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        uint16_t interval = ble_ext_adv_mgr_get_adv_interval(app_lea_adv_handle);

        if (app_hfp_get_call_status() == APP_CALL_IDLE)
        {
            uint32_t state;

            app_timer_state_get(&timer_idx_slow_interval, &state);
            if (state)
            {
                if (interval != APP_LEA_ADV_INTERVAL_FAST)
                {
                    ble_ext_adv_mgr_change_adv_interval(app_lea_adv_handle, APP_LEA_ADV_INTERVAL_FAST);
                }
            }
        }
        else
        {
            if (interval != APP_LEA_ADV_INTERVAL_SLOW)
            {
                ble_ext_adv_mgr_change_adv_interval(app_lea_adv_handle, APP_LEA_ADV_INTERVAL_SLOW);
                app_gfps_adv_update_adv_interval(400);
                APP_PRINT_TRACE0("------------  app_lea_adv_update_interval_hfp, app_gfps_adv_update_adv_interval (400) -------------");  
            }
        }
    }
}

T_BLE_EXT_ADV_MGR_STATE app_lea_adv_get_state(void)
{
    return app_lea_adv_state;
}

uint8_t app_lea_adv_get_handle(void)
{
    return app_lea_adv_handle;
}

void app_lea_adv_update(void)
{
    uint16_t audio_adv_len = 0;
    uint16_t audio_adv_flag = 0;
    uint8_t service_num = 0;
    uint8_t announcement_type = ADV_GENERAL_ANNOUNCEMENT;

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_ASCS;
    service_num++;
#endif

#if F_APP_CSIS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_PSRI;
#endif

#if F_APP_TMAP_BMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_BASS;
    service_num++;
#endif

#if F_APP_TMAS_SUPPORT && (F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT)
    audio_adv_flag |= LE_EXT_ADV_TMAS;
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_PACS;
    service_num++;
#endif

#if F_APP_VCS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_VCS;
    service_num++;
#endif

#if F_APP_MICS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_MICS;
    service_num++;
#endif

    if (app_lea_adv_linkback_check())
    {
        announcement_type = ADV_TARGETED_ANNOUNCEMENT;
    }

    if (announcement_type == ADV_TARGETED_ANNOUNCEMENT)
    {
        uint32_t state;

        app_timer_state_get(&timer_idx_slow_interval, &state);
        if (!state && app_lea_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            announcement_type = ADV_GENERAL_ANNOUNCEMENT;
        }
    }

    APP_PRINT_INFO2("app_lea_adv_update:state 0x%x, announcement_type %d", app_bt_policy_get_state(),
                    announcement_type);

    audio_adv_len = app_lea_adv_set_ext_data(audio_adv_flag, service_num, announcement_type);
    ble_ext_adv_mgr_set_adv_data(app_lea_adv_handle, audio_adv_len, app_lea_adv_data);
}

void app_lea_adv_init(void)
{
    uint16_t audio_adv_flag = 0;
    uint8_t service_num = 0;
    uint16_t audio_adv_len = 0;
    T_LE_EXT_ADV_EXTENDED_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED;
#if TARGET_LE_AUDIO_GAMING
    uint16_t adv_interval_min = 0x20;
    uint16_t adv_interval_max = 0x20;
#else
    uint16_t adv_interval_min = APP_LEA_ADV_INTERVAL_FAST;
    uint16_t adv_interval_max = APP_LEA_ADV_INTERVAL_FAST;
#endif
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t peer_address[6] = {0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    uint8_t announcement_type = ADV_GENERAL_ANNOUNCEMENT;

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_ASCS;
    service_num++;
#endif

#if F_APP_CSIS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_PSRI;
#endif

#if F_APP_TMAP_BMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_BASS;
    service_num++;
#endif

#if F_APP_TMAS_SUPPORT && (F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT)
    audio_adv_flag |= LE_EXT_ADV_TMAS;
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_PACS;
    service_num++;
#endif

#if F_APP_VCS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_VCS;
    service_num++;
#endif

#if F_APP_MICS_SUPPORT
    audio_adv_flag |= LE_EXT_ADV_MICS;
    service_num++;
#endif

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        app_lea_adv_scan_rsp_data[2] = LO_WORD(GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
                                               GAP_APPEARANCE_SUB_CATEGORY_HEADSET);
        app_lea_adv_scan_rsp_data[3] = HI_WORD(GAP_APPEARANCE_CATEGORY_WEARABLE_AUDIO_DEVICE << 6 |
                                               GAP_APPEARANCE_SUB_CATEGORY_HEADSET);
    }

    if (app_lea_mgr_lea_bond_num())
    {
        announcement_type = ADV_TARGETED_ANNOUNCEMENT;
    }

    audio_adv_len = app_lea_adv_set_ext_data(audio_adv_flag, service_num, announcement_type);
    ble_ext_adv_mgr_init_adv_params(&app_lea_adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, audio_adv_len, app_lea_adv_data,
                                    sizeof(app_lea_adv_scan_rsp_data), app_lea_adv_scan_rsp_data, NULL);
    ble_ext_adv_mgr_change_adv_phy(app_lea_adv_handle, GAP_PHYS_PRIM_ADV_1M, GAP_PHYS_2M);
    ble_ext_adv_mgr_register_callback(app_lea_adv_callback, app_lea_adv_handle);

    app_timer_reg_cb(app_lea_adv_timeout_cb, &app_lea_adv_timer_id);
}
#endif
