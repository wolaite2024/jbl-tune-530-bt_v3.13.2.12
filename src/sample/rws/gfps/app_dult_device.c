/*
 * Copyright (c) 2024, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FINDER_SUPPORT

#include "stdlib.h"
#include "trace.h"
#include "string.h"

#include "ble_ext_adv.h"

#include "audio.h"

#include "app_timer.h"
#include "app_dult_svc.h"
#include "app_dult.h"
#include "app_cfg.h"
#include "app_audio_policy.h"
#include "app_adv_stop_cause.h"
#include "app_cfg.h"
#include "app_dult_device.h"

/**
 * @brief DULT timer related
 */
static uint8_t           app_dult_timer_id = 0;
static uint8_t           timer_idx_dult_addr_update = 0;
static uint8_t           timer_idx_dult_exit_id_read_state = 0;
static uint8_t           timer_idx_dult_ring_duration = 0;

static bool              dult_id_read_enable = false;
static bool              keep_ring = false;
static T_DULT_RING_STATE dult_ring_state = DULT_RING_STATE_STOPPED;
typedef enum
{
    APP_DULT_DEVICE_TIMER_UPDATE_ADDR,
    APP_DULT_DEVICE_TIMER_EXIT_ID_READ_STATE,
    APP_DULT_DEVICE_TIMER_RING_DURATION,

} T_APP_DULT_DEVICE_TIMER;

#define DULT_TIMER_INTERVAL_24H       60 * 60 * 1000 * 24
#define DULT_TIMER_INTERVAL_15MIN     60 * 15 * 1000
#define DULT_TIMER_INTERVAL_5MIN      60 * 5 *1000
#define DULT_TIMER_RING_DURATION      12 * 1000

#if GFPS_FINDER_DULT_ADV_SUPPORT
/**
 * @brief DULT advertising data
 */
static uint8_t dult_advdata[31] = {0};
static uint8_t dult_static_addr[6] = {0};
static uint8_t dult_adv_handle;

static uint16_t app_dult_device_gen_adv(T_DULT_NEAR_OWNER_STATE state, uint8_t *bd_adddr)
{
    uint16_t i = 0;

    memcpy(dult_advdata + i, bd_adddr, GAP_BD_ADDR_LEN);
    i += 6;

    dult_advdata[i++] = 0x02;
    dult_advdata[i++] = GAP_ADTYPE_FLAGS;
    dult_advdata[i++] = GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED;

    dult_advdata[i++] = 0x05;
    dult_advdata[i++] = GAP_ADTYPE_SERVICE_DATA;
    dult_advdata[i++] = LO_WORD(DULT_SRV_UUID);
    dult_advdata[i++] = HI_WORD(DULT_SRV_UUID);
    dult_advdata[i++] = DULT_NETWORK_ID;
    dult_advdata[i++] = state;

    return i;
}

T_GAP_CAUSE app_dult_device_update_adv_data(T_DULT_NEAR_OWNER_STATE state)
{
    uint16_t adv_data_len = app_dult_device_gen_adv(state, dult_static_addr);
    T_GAP_CAUSE cause = ble_ext_adv_mgr_set_adv_data(dult_adv_handle,
                                                     adv_data_len,
                                                     dult_advdata);
    APP_PRINT_INFO3("app_dult_device_update_adv_data: state %d addr %b, cause %d",
                    state, TRACE_BDADDR(dult_static_addr), cause);
    return cause;
}

T_GAP_CAUSE app_dult_device_start_adv(T_DULT_NEAR_OWNER_STATE state)
{
    APP_PRINT_INFO1("app_dult_device_start_adv: near-own state %d", state);
    T_GAP_CAUSE cause = app_dult_device_update_adv_data(state);

    if (cause == GAP_CAUSE_SUCCESS)
    {
        cause = ble_ext_adv_mgr_enable(dult_adv_handle, 0);
    }
    APP_PRINT_INFO3("app_dult_start_adv: state %d addr %b, cause %d",
                    state, TRACE_BDADDR(dult_static_addr), cause);
    return cause;
}

static void app_dult_device_stop_adv(void)
{
    APP_PRINT_INFO0("app_dult_device_stop_adv");
    ble_ext_adv_mgr_disable(dult_adv_handle, APP_STOP_ADV_CAUSE_DULT_PAUSE);
}

void app_dult_device_update_adv_addr(void)
{
    le_gen_rand_addr(GAP_RAND_ADDR_STATIC, dult_static_addr);
    ble_ext_adv_mgr_set_random(dult_adv_handle, dult_static_addr);
}

static void app_dult_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));

    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            APP_PRINT_TRACE4("app_dult_adv_callback: adv_state %d, adv_handle %d, stop_cause %d, app_cause 0x%02x",
                             cb_data.p_ble_state_change->state,
                             cb_data.p_ble_state_change->adv_handle,
                             cb_data.p_ble_state_change->stop_cause,
                             cb_data.p_ble_state_change->app_cause);
        }
        break;

    default:
        break;
    }

    return;
}

void app_dult_device_start_update_timer(T_DULT_NEAR_OWNER_STATE current_state)
{
    app_stop_timer(&timer_idx_dult_addr_update);
    if (current_state == DULT_NEARING_OWNER)
    {
        app_start_timer(&timer_idx_dult_addr_update, "update_addr",
                        app_dult_timer_id,
                        APP_DULT_DEVICE_TIMER_UPDATE_ADDR, 0, false,
                        DULT_TIMER_INTERVAL_24H);
    }
    else
    {
        app_start_timer(&timer_idx_dult_addr_update, "update_addr",
                        app_dult_timer_id,
                        APP_DULT_DEVICE_TIMER_UPDATE_ADDR, 0, false,
                        DULT_TIMER_INTERVAL_15MIN);
    }
}

void app_dult_device_adv_init(void)
{
    bool ret = false;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    uint16_t adv_interval = 3200;
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    le_gen_rand_addr(GAP_RAND_ADDR_STATIC, dult_static_addr);
    ble_ext_adv_mgr_init_adv_params(&dult_adv_handle, adv_event_prop, adv_interval,
                                    adv_interval, own_address_type, peer_address_type,
                                    peer_address, filter_policy, 0, NULL,
                                    0, NULL, dult_static_addr);
    ret = ble_ext_adv_mgr_register_callback(app_dult_adv_callback, dult_adv_handle);
    APP_PRINT_INFO3("app_dult_adv_init: dult_adv_handle %d, ret %d, dult version %b",
                    dult_adv_handle, ret, TRACE_STRING(DULT_FIRMWARE_VERSION));
}
#endif

// Sound
void app_dult_ring_duration_timer_start(void)
{
    keep_ring = true;
    app_start_timer(&timer_idx_dult_ring_duration, "dult_ring_duration",
                    app_dult_timer_id,
                    APP_DULT_DEVICE_TIMER_RING_DURATION, 0, false,
                    DULT_TIMER_RING_DURATION);

    APP_PRINT_INFO0("app_dult_ring_duration_timer_start");
}

void app_dult_ring_duration_timer_stop(void)
{
    keep_ring = false;
    app_stop_timer(&timer_idx_dult_ring_duration);
    APP_PRINT_INFO0("app_dult_ring_duration_timer_stop");
}

static T_APP_AUDIO_TONE_TYPE app_dult_get_tone_type(void)
{
    T_APP_AUDIO_TONE_TYPE tone_type = TONE_GFPS_DULT_FIND;

#if F_APP_HARMAN_FEATURE_SUPPORT
    tone_type = (app_cfg_nv.language_status == 0) ? TONE_HARMAN_DULT_TONE : TONE_HARMAN_DULT_VP;
#endif

    return tone_type;
}

void app_dult_device_sound_start(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback)
{
    if (app_audio_tone_type_play(app_dult_get_tone_type(), false, false))
    {
        dult_ring_state = DULT_RING_STATE_STARTED;
        app_dult_ring_duration_timer_start();
        app_dult_report_cmd_rsp(p_callback->conn_handle, p_callback->cid, service_id,
                                DULT_OPCODE_CTRL_SOUND_START, DULT_CMD_RSP_STATE_SUCCESS);
    }
    else
    {
        app_dult_report_cmd_rsp(p_callback->conn_handle, p_callback->cid, service_id,
                                DULT_OPCODE_CTRL_SOUND_START, DULT_CMD_RSP_STATE_INVALID_STATE);
    }
}

void app_dult_device_sound_stop(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback)
{
    app_dult_ring_duration_timer_stop();
    app_audio_tone_type_cancel(app_dult_get_tone_type(), false);
    app_dult_sound_complete_handle();
}

static void app_dult_audio_cback(T_AUDIO_EVENT event_type, void *event_buf,
                                 uint16_t buf_len)
{
    bool handle = false;
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_RINGTONE_STOPPED:
    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:
        {
            uint8_t tone_stopped_idx;

            if (event_type == AUDIO_EVENT_VOICE_PROMPT_STOPPED)
            {
                tone_stopped_idx = param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX;
            }
            else
            {
                tone_stopped_idx = param->ringtone_stopped.index;
            }
#if F_APP_HARMAN_FEATURE_SUPPORT
            if (tone_stopped_idx == app_cfg_const.tone_harman_dult_tone ||
                tone_stopped_idx == app_cfg_const.tone_harman_dult_vp)
#else
            if (tone_stopped_idx == app_cfg_const.tone_gfps_dult_find)
#endif
            {
                if (keep_ring)
                {
                    app_audio_tone_type_play(app_dult_get_tone_type(), false, false);
                }

                APP_PRINT_TRACE1("app_dult_audio_cback: keep_ring %d", keep_ring);
                handle = true;
            }
        }
        break;

    default:
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_dult_audio_cback: event_type 0x%04x", event_type);
    }
}

//identify read state
bool app_dult_id_read_state_get(void)
{
    return dult_id_read_enable;
}

void app_dult_id_read_state_enable(void)
{
    APP_PRINT_INFO0("app_dult_id_read_state_enable");
    dult_id_read_enable = true;
    app_start_timer(&timer_idx_dult_exit_id_read_state, "exit_id_read_state",
                    app_dult_timer_id,
                    APP_DULT_DEVICE_TIMER_EXIT_ID_READ_STATE, 0, false,
                    DULT_TIMER_INTERVAL_5MIN);
}

static void app_dult_timer_cb(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("app_dult_timer_cb: timer_id 0x%02x, timer_chann %d",
                     timer_id, timer_chann);

    switch (timer_id)
    {
#if GFPS_FINDER_DULT_ADV_SUPPORT
    case APP_DULT_DEVICE_TIMER_UPDATE_ADDR:
        {
            app_dult_device_update_adv_addr();
        }
        break;
#endif

    case APP_DULT_DEVICE_TIMER_EXIT_ID_READ_STATE:
        {
            APP_PRINT_INFO0("app_dult_timer_cb: id_read_state_disable");
            dult_id_read_enable = false;
        }
        break;

    case APP_DULT_DEVICE_TIMER_RING_DURATION:
        {
            //service_id and p_callback will bee called internal
            APP_PRINT_INFO0("app_dult_timer_cb: ring duration timeout");
            app_dult_device_sound_stop(0xFF, NULL);
        }

    default:
        break;
    }
}

T_DULT_RING_STATE app_dult_device_get_ring_state(void)
{
    return dult_ring_state;
}

void app_dult_device_set_ring_state(T_DULT_RING_STATE ring_state)
{
    dult_ring_state = ring_state;
}

void app_dult_device_init(void)
{
#if GFPS_FINDER_DULT_ADV_SUPPORT
    app_dult_device_adv_init();
#endif
    audio_mgr_cback_register(app_dult_audio_cback);
    app_timer_reg_cb(app_dult_timer_cb, &app_dult_timer_id);
}

#endif
