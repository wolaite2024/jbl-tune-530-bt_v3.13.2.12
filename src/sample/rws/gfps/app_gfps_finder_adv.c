#if GFPS_FINDER_SUPPORT
#include "stdint.h"
#include "ble_ext_adv.h"
#include "string.h"
#include "trace.h"
#include "app_cfg.h"
#include "pm.h"
#include "app_main.h"
#include "app_link_util.h"
#include "app_gfps_finder.h"
#include "app_adv_stop_cause.h"

static uint8_t gfps_finder_adv_handle = 0xFF;
static T_BLE_EXT_ADV_MGR_STATE adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
static bool key_stop_track = false;
/**
 * @brief GFPS finder beacon advertising data
 *
 */
static uint8_t gfps_finder_advdata[] =
{
    0x02,//length = 0x02
    0x01,//type = GAP_ADTYPE_FLAGS
    0x06,//data = GAP_ADTYPE_FLAGS_GENERAL|GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED

    0x19,//length = 0x18
    0x16,//type = GAP_ADTYPE_SERVICE_DATA
    0xAA, 0xFE, //data: 16-bit Eddystone UUID = 0xFEAA
    0x40,//E2EE-EID Frame type
    /*20-byte Ephemeral Identifier.*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*hashed flags?*/
    0x00,
};

static uint8_t gfps_finder_scanrspdata[] =
{
    12,//length = 0x02
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'G', 'F', 'P', 'S', '_', 'F', 'i', 'n', 'd', 'e', 'r',
};

T_BLE_EXT_ADV_MGR_STATE gfps_finder_adv_get_adv_state()
{
    T_BLE_EXT_ADV_MGR_STATE adv_state;

    adv_state = ble_ext_adv_mgr_get_adv_state(gfps_finder_adv_handle);
    APP_PRINT_TRACE1("gfps_finder_get_adv_state: adv_state %d", adv_state);

    return adv_state;
}

T_GAP_CAUSE gfps_finder_adv_update_rpa(void)
{
    uint8_t random_address[6] = {0};
    le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);
    T_GAP_CAUSE cause = ble_ext_adv_mgr_set_random(gfps_finder_adv_handle, random_address);
    APP_PRINT_TRACE2("gfps_finder_adv_update_rpa: cause %d, RPA %b", cause,
                     TRACE_BDADDR(random_address));

    return cause;
}

bool gfps_finder_adv_get_key_stop_tracking_flag(void)
{
    APP_PRINT_INFO1("gfps_finder_adv_get_key_stop_tracking_flag: %d", key_stop_track);
    return key_stop_track;
}

void gfps_finder_adv_set_key_stop_tracking_flag(bool key_stop)
{
    key_stop_track = key_stop;
    APP_PRINT_INFO1("gfps_finder_adv_set_key_stop_tracking_flag: %d", key_stop_track);
}

void gfps_finder_adv_start(uint16_t duration_10ms)
{
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (gfps_finder_adv_get_key_stop_tracking_flag() == true)
    {
        APP_PRINT_INFO0("gfps_finder_adv_start: user do not want enable finder adv for tracking");
        return;
    }

    APP_PRINT_TRACE2("gfps_finder_adv_start: duration %d, adv_state %d", duration_10ms, adv_state);

    if (adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        ble_ext_adv_mgr_enable(gfps_finder_adv_handle, duration_10ms);
    }
}

void gfps_finder_adv_stop(uint8_t app_cause)
{
    APP_PRINT_TRACE2("gfps_finder_adv_stop: app_cause %d, adv_state %d", app_cause, adv_state);
    if (adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        ble_ext_adv_mgr_disable(gfps_finder_adv_handle, app_cause);
    }
}

/**
 * @brief update frame type
 *
 * the frame type should be set to 0x41 when Unwanted Tracking Protection mode is on.
 * the frame type should be set to 0x40 when Unwanted Tracking Protection mode is off.
 * @param frame_type
 */
void gfps_finder_adv_update_frame_type(uint8_t frame_type)
{
    gfps_finder_advdata[7] = frame_type;
    ble_ext_adv_mgr_set_adv_data(gfps_finder_adv_handle,
                                 sizeof(gfps_finder_advdata),
                                 gfps_finder_advdata);
}


T_GAP_CAUSE gfps_finder_adv_update_adv_interval(uint32_t adv_interval)
{
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    result = ble_ext_adv_mgr_change_adv_interval(gfps_finder_adv_handle, adv_interval);
    APP_PRINT_TRACE2("gfps_finder_adv_update_adv_interval: result %d, adv_interval %d", result,
                     adv_interval);
    return result;
}

/**
 * @brief update adv EI and hash
 *
 * @param p_ei
 * @param hash
 */
void gfps_finder_adv_update_adv_ei_hash(uint8_t *p_ei, uint8_t hash)
{
    if (p_ei)
    {
        memcpy(&gfps_finder_advdata[8], p_ei, 20);
    }
    gfps_finder_advdata[28] = hash;
    ble_ext_adv_mgr_set_adv_data(gfps_finder_adv_handle,
                                 sizeof(gfps_finder_advdata),
                                 gfps_finder_advdata);

    APP_PRINT_TRACE2("gfps_finder_adv_update_adv_ei_hash: EI %b, hash result 0x%x",
                     TRACE_BINARY(20, p_ei), hash);
}

/**
 * @brief update hash
 *
 * @param hash
 */
void gfps_finder_adv_update_hash(uint8_t hash)
{
    gfps_finder_advdata[28] = hash;
    ble_ext_adv_mgr_set_adv_data(gfps_finder_adv_handle,
                                 sizeof(gfps_finder_advdata),
                                 gfps_finder_advdata);

    APP_PRINT_TRACE1("gfps_finder_adv_update_hash: hash 0x%x", hash);
}

static void gfps_finder_adv_state_change(T_BLE_EXT_ADV_CB_DATA cb_data)
{
    adv_state = cb_data.p_ble_state_change->state;
    uint8_t adv_handle = cb_data.p_ble_state_change->adv_handle;
    APP_PRINT_TRACE2("gfps_finder_adv_state_change: adv_state %d, adv_handle %d", adv_state,
                     adv_handle);

    if (adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        T_BLE_EXT_ADV_STOP_CAUSE stack_cause = cb_data.p_ble_state_change->stop_cause;
        uint8_t app_cause = cb_data.p_ble_state_change->app_cause;
        APP_PRINT_TRACE2("gfps_finder_adv_state_change: stop cause, stack 0x%x, app 0x%02x",
                         stack_cause, app_cause);

        if (stack_cause == BLE_EXT_ADV_STOP_CAUSE_TIMEOUT)
        {
            /*in power off mode, finder adv will exist for a limited duration,
            when timeout, device will enter power down mode, and later will be
            wakeup by RTC.*/
            if (app_db.device_state != APP_DEVICE_STATE_ON &&
                !extend_app_cfg_const.disable_finder_adv_when_power_off
                && app_gfps_finder_provisioned())
            {
                power_mode_set(POWER_POWERDOWN_MODE);
            }
        }
    }
}

static void gfps_finder_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));

    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            gfps_finder_adv_state_change(cb_data);
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        {
            /*this ble link is connected by gfps finder beacon adv*/
            uint8_t conn_id = cb_data.p_ble_conn_info->conn_id;

            app_gfps_finder_beacon_set_conn_id(conn_id);
            app_link_reg_le_link_disc_cb(cb_data.p_ble_conn_info->conn_id,
                                         app_gfps_finder_beacon_le_disconnect_cb);
        }
        break;
    };
}

void app_gfps_finder_adv_handle_mmi_action(void)
{
    if (extend_app_cfg_const.gfps_finder_support)
    {
        uint8_t finder_conn_id = app_gfps_finder_beacon_get_conn_id();
        bool    finder_provisioned = app_gfps_finder_provisioned();

        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY &&
            extend_app_cfg_const.gfps_support && finder_provisioned)
        {
            /*user keypress to disable the device (temporarily stop advertising) as required in the spec
            to ensure compliance with unwanted tracking protections*/
            gfps_finder_adv_stop(APP_STOP_ADV_CAUSE_GFPS_FINDER_KEY_PRESS);
            gfps_finder_adv_set_key_stop_tracking_flag(true);
        }

        APP_PRINT_INFO3("app_gfps_finder_adv_handle_mmi_action: bud_role %d, finder_conn_id %d, provisioned %d",
                        app_cfg_nv.bud_role, finder_conn_id, finder_provisioned);
    }
}

void gfps_finder_adv_init(uint8_t *p_adv_ei, uint8_t hash)
{
    T_GAP_CAUSE cause;
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    uint16_t adv_interval_min = 0x320; //500ms
    uint16_t adv_interval_max = 0x320; //500ms
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    uint8_t random_address[6] = {0};
    le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);

    memcpy(&gfps_finder_advdata[8], p_adv_ei, 20);
    gfps_finder_advdata[28] = hash;
    ble_ext_adv_mgr_init_adv_params(&gfps_finder_adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, sizeof(gfps_finder_advdata), gfps_finder_advdata,
                                    sizeof(gfps_finder_scanrspdata), gfps_finder_scanrspdata, random_address);

    cause = ble_ext_adv_mgr_register_callback(gfps_finder_adv_callback, gfps_finder_adv_handle);
    if (cause == GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_INFO1("gfps_finder_adv_init: adv_handle %d", gfps_finder_adv_handle);
    }
}
#endif
