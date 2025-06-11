
#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "ble_ext_adv.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_cfg.h"
#include "app_ota_tooling.h"
#include "app_adv_stop_cause.h"

#define USE_BLE_EXT_ADV_RECOMMEND_FORMAT 0

#define ADV_INTERVAL 0x60 //0x20 ~ 0xB0

typedef enum
{
    APP_TIMER_OTA_TOOLING_ADV,
} T_APP_OTA_TOOLING_TIMER;

typedef struct
{
#if USE_BLE_EXT_ADV_RECOMMEND_FORMAT
    uint8_t length;
    uint8_t type;
#endif
    uint8_t bd_addr[6];
    uint8_t dongle_id;
} __attribute__((packed)) T_ADV_DATA;

static uint8_t adv_handle = 0xff;
static uint8_t ota_tooling_timer_id = 0;
static uint8_t timer_idx_ota_tooling_adv = 0;

static T_ADV_DATA adv_data =
{
#if USE_BLE_EXT_ADV_RECOMMEND_FORMAT
    .length = 0x08,
    .type = GAP_ADTYPE_MANUFACTURER_SPECIFIC,
#endif
    .bd_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .dongle_id = 0,
};

static void app_ota_tooling_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;

    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));

    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        APP_PRINT_TRACE4("app_ota_tooling_adv_callback: BLE_EXT_ADV_STATE_CHANGE, adv_handle %d, state %d, stop_cause %d, app_cause %d",
                         cb_data.p_ble_state_change->adv_handle,
                         cb_data.p_ble_state_change->state,
                         cb_data.p_ble_state_change->stop_cause,
                         cb_data.p_ble_state_change->app_cause);
        break;

    default:
        break;
    }
}

static void app_ota_tooling_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_OTA_TOOLING_ADV:
        {
            app_ota_tooling_adv_stop();
        }
        break;

    default:
        break;
    }
}

void app_ota_tooling_adv_init(void)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
        LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
    uint16_t adv_interval_min = 0x20;
    uint16_t adv_interval_max = 0x20;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    ble_ext_adv_mgr_init_adv_params(&adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, 0, NULL,
                                    0, NULL, NULL);
    ble_ext_adv_mgr_register_callback(app_ota_tooling_adv_callback, adv_handle);

    app_timer_reg_cb(app_ota_tooling_timeout_cb, &ota_tooling_timer_id);
}

void app_ota_tooling_adv_start(uint8_t dongle_id, uint16_t timeout_s)
{
    APP_PRINT_TRACE2("app_ota_tooling_adv_start: dongle_id 0x%x, timeout_s %d", dongle_id, timeout_s);

    memcpy(adv_data.bd_addr, app_cfg_nv.bud_local_addr, 6);
    adv_data.dongle_id = dongle_id;

    ble_ext_adv_mgr_change_adv_interval(adv_handle, ADV_INTERVAL);
    ble_ext_adv_mgr_set_adv_data(adv_handle, sizeof(adv_data), (uint8_t *)&adv_data);

    if (ble_ext_adv_mgr_get_adv_state(adv_handle) == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        ble_ext_adv_mgr_enable(adv_handle, 0);
    }

    app_start_timer(&timer_idx_ota_tooling_adv, "ota_tooling_adv",
                    ota_tooling_timer_id, APP_TIMER_OTA_TOOLING_ADV, 0, false,
                    timeout_s * 1000);
}

void app_ota_tooling_adv_stop(void)
{
    app_stop_timer(&timer_idx_ota_tooling_adv);
    ble_ext_adv_mgr_disable(adv_handle, APP_STOP_ADV_CAUSE_OTA);
}

void app_ota_tooling_nv_store(void)
{
    // keep current status and restart
    // app_cfg_nv.ota_tooling_start = 1;
    app_cfg_nv.ota_tooling_start = 1;
    app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

    app_cfg_nv.jig_subcmd = app_db.jig_subcmd;
    app_cfg_nv.jig_dongle_id = app_db.jig_dongle_id;
    app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
}

