/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FEATURE_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "trace.h"
#include "audio.h"
#include "remote.h"
#include "bt_gfps.h"
#include "app_audio_policy.h"
#include "app_bt_policy_api.h"
#include "app_gfps.h"
#include "app_cfg.h"
#include "app_gfps_rfc.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_sdp.h"
#include "stdlib.h"
#include "app_gfps_battery.h"
#include "app_timer.h"
#include "app_relay.h"
#include "app_gfps_msg.h"

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_dsp_cfg.h"
#endif

#if GFPS_FINDER_SUPPORT
#include "app_gfps_finder.h"
#include "gfps_find_my_device.h"
#include "audio_volume.h"
#endif
#if GFPS_SASS_SUPPORT
#include "app_multilink.h"
#include "app_sass_policy.h"
#include "app_gfps_account_key.h"
#include "app_sensor.h"
#include "app_audio_passthrough.h"
#include "app_anc.h"
#if GFPS_SASS_LEA_SUPPORT
#include "app_lea_ccp.h"
#include "app_lea_unicast_audio.h"
#endif
#endif

#define GFPS_VOICE_PROMPT_OFFSET   0x80

static uint8_t gfps_msg_timer_id          = 0;
static uint8_t timer_idx_gfps_ring_period = 0;
static uint8_t timer_idx_gfps_ring_stop   = 0;
static uint16_t gfps_msg_ring_timeout     = 0;

/** @brief indicate current ring state*/
static T_GFPS_MSG_RING_STATE gfps_msg_ring_state = GFPS_MSG_RING_STOP;

/**
 * @brief GFPS message stream timer
 * APP_TIMER_GFPS_RING_PERIOD:
 * the interval for ring voice prompt stopped and next ring voice prompt start,
 * the default interval is 2s(GFPS_RING_PERIOD_VALUE)
 * APP_TIMER_GFPS_RING_STOP: ring timeout and shall stop ting
 */
typedef enum
{
    APP_TIMER_GFPS_RING_PERIOD,
    APP_TIMER_GFPS_RING_STOP,
} T_GFPS_MSG_TIMER;

/**
 * @brief app_gfps_msg_reverse_data
 * swap big endian mode or little endian mode
 * @param data
 * @param len
 */
void app_gfps_msg_reverse_data(uint8_t *data, uint16_t len)
{
    uint8_t *header = data;
    uint8_t *tail = data;
    uint8_t temp;

    tail += (len - 1);

    while (header < tail)
    {
        temp = *header;
        *header++ = *tail;
        *tail-- = temp;
    }
}

#if GFPS_FINDER_SUPPORT
uint8_t app_gfps_msg_get_ring_timeout(void)
{
    return gfps_msg_ring_timeout;
}

void app_gfps_msg_set_ring_timeout(uint8_t ring_timeout)
{
    gfps_msg_ring_timeout = ring_timeout;
}
#endif

T_GFPS_MSG_RING_STATE app_gfps_msg_get_ring_state(void)
{
    return gfps_msg_ring_state;
}

void app_gfps_msg_set_ring_state(T_GFPS_MSG_RING_STATE ring_state)
{
    gfps_msg_ring_state = ring_state;
}

void app_gfps_msg_update_ring_status(uint8_t ring_status)
{
    uint8_t i;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            (app_db.br_link[i].connected_profile & GFPS_PROFILE_MASK))
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[i];

            if (p_link != NULL)
            {
                bt_gfps_send_ring_status(0xFF, 0, p_link->bd_addr, p_link->gfps_rfc_chann, &ring_status, 1);
            }

            APP_PRINT_INFO1("app_gfps_msg_update_ring_status: path br, ring_status %d", ring_status);
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if ((app_db.le_link[i].used == true) &&
            (app_db.le_link[i].gfps_link.gfps_conn_id != 0xFF) &&
            (app_db.le_link[i].gfps_link.gfps_msg_cid != 0))
        {
            T_APP_LE_LINK *p_link = &app_db.le_link[i];

            if (p_link != NULL)
            {
                bt_gfps_send_ring_status(app_db.le_link[i].gfps_link.gfps_conn_id,
                                         app_db.le_link[i].gfps_link.gfps_msg_cid,
                                         NULL, 0, &ring_status, 1);
            }

            APP_PRINT_INFO1("app_gfps_msg_update_ring_status: path BLE, ring_status %d", ring_status);
        }
    }
}

static void app_gfps_msg_ring_play(void)
{
    audio_volume_out_mute(AUDIO_STREAM_TYPE_PLAYBACK);
#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_play(TONE_HARMAN_GFPS_FIND_ME, false, false);
    }
    else
#endif
    {
        app_audio_tone_type_play(TONE_GFPS_FINDME, false, false);
    }

    if (gfps_msg_ring_timeout)
    {
        app_start_timer(&timer_idx_gfps_ring_stop, "gfps_ring_stop",
                        gfps_msg_timer_id, APP_TIMER_GFPS_RING_STOP, 0, false,
                        gfps_msg_ring_timeout * 1000);
    }
}

void app_gfps_msg_ring_stop(void)
{
    audio_volume_out_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
    gfps_msg_ring_timeout = 0;
#if F_APP_HARMAN_FEATURE_SUPPORT
    if (app_cfg_nv.language_status == 0)
    {
        app_audio_tone_type_cancel(TONE_HARMAN_GFPS_FIND_ME, false);
    }
    else
#endif
    {
        app_audio_tone_type_cancel(TONE_GFPS_FINDME, false);
    }
    app_stop_timer(&timer_idx_gfps_ring_period);
    app_stop_timer(&timer_idx_gfps_ring_stop);
}

/**
 * @brief current ring state is stop and need todo next ring event
 * state indicate current ring state
 * event indicate expected ring event
 * @param event
 */
static void app_gfps_msg_ring_stop_state(uint8_t event)
{
    APP_PRINT_TRACE3("app_gfps_msg_ring_stop_state: state 0x%02x, event 0x%02x, bud_side %d",
                     gfps_msg_ring_state, event, app_cfg_const.bud_side);
    switch (event)
    {
    case GFPS_ALL_STOP:
        {
        }
        break;

    case GFPS_RIGHT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    app_gfps_msg_ring_play();
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    case GFPS_LEFT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    app_gfps_msg_ring_play();
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }

        }
        break;

    case GFPS_ALL_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                       GFPS_REMOTE_MSG_FINDME_START,
                                                       &event, 1, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                    gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                    {
                        app_gfps_msg_ring_play();
                        gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                        app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                    }
                    else
                    {
                        app_gfps_msg_ring_play();
                        gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                        app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief current ring state is right ring and need todo next ring event
 * state indicate current ring state
 * event indicate expected ring event
 * @param event
 */
static void app_gfps_msg_right_ring_state(uint8_t event)
{
    APP_PRINT_TRACE3("app_gfps_msg_right_ring_state: state 0x%02x, event 0x%02x, bud_side %d",
                     gfps_msg_ring_state, event, app_cfg_const.bud_side);
    switch (event)
    {
    case GFPS_ALL_STOP:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_stop();
                gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    app_gfps_msg_ring_stop();
                    gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    case GFPS_RIGHT_RING:
        {

        }
        break;

    case GFPS_LEFT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    app_gfps_msg_ring_stop();
                    //gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                }
                else
                {
                    app_gfps_msg_ring_play();
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    case GFPS_ALL_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    app_gfps_msg_ring_play();
                    gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief current ring state is left ring and need todo next ring event
 * state indicate current ring state
 * event indicate expected ring event
 * @param event
 */
static void app_gfps_msg_left_ring_state(uint8_t event)
{
    APP_PRINT_TRACE3("app_gfps_msg_left_ring_state: state 0x%02x, event 0x%02x, bud_side %d",
                     gfps_msg_ring_state, event, app_cfg_const.bud_side);
    switch (event)
    {
    case GFPS_ALL_STOP:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_stop();
                gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    app_gfps_msg_ring_stop();
                    gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                }
            }
        }
        break;

    case GFPS_RIGHT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    app_gfps_msg_ring_stop();
                    //gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                }
                else
                {
                    app_gfps_msg_ring_play();
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    case GFPS_LEFT_RING:
        {

        }
        break;

    case GFPS_ALL_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    app_gfps_msg_ring_play();
                    gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_START,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_ALL_RING;
                    app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
                }
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief current ring state is all ring and need todo next ring event
 * state indicate current ring state
 * event indicate expected ring event
 * @param event
 */
static void app_gfps_msg_all_ring_state(uint8_t event)
{
    APP_PRINT_TRACE3("app_gfps_msg_all_ring_state: state 0x%02x, event 0x%02x, bud_side %d",
                     gfps_msg_ring_state, event, app_cfg_const.bud_side);
    switch (event)
    {
    case GFPS_ALL_STOP:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_stop();
                gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                       &event, 1, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                    gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                }
            }

        }
        break;

    case GFPS_RIGHT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    app_gfps_msg_ring_stop();
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                }
            }
        }
        break;

    case GFPS_LEFT_RING:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_gfps_msg_ring_play();
                gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                app_gfps_msg_update_ring_status((uint8_t)gfps_msg_ring_state);
            }
            else
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    app_gfps_msg_ring_stop();
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                }
                else
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                    gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                }
            }
        }
        break;

    case GFPS_ALL_RING:
        {

        }
        break;

    default:
        break;
    }
}

/**
 * @brief app_gfps_msg_handle_ring_event
 *
 * @param event indicate expected ring event
 */
void app_gfps_msg_handle_ring_event(uint8_t event)
{
    switch (gfps_msg_ring_state)
    {
    case GFPS_MSG_RING_STOP:
        app_gfps_msg_ring_stop_state(event);
        break;

    case GFPS_MSG_RIGHT_RING:
        app_gfps_msg_right_ring_state(event);
        break;

    case GFPS_MSG_LEFT_RING:
        app_gfps_msg_left_ring_state(event);
        break;

    case GFPS_MSG_ALL_RING:
        app_gfps_msg_all_ring_state(event);
        break;

    default:
        break;
    }

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        uint8_t data[3];
        data[0] = gfps_msg_ring_state;
        data[1] = gfps_msg_ring_timeout >> 8;
        data[2] = gfps_msg_ring_timeout;
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_FINDME_SYNC,
                                            data, 3);
    }
#endif
    APP_PRINT_TRACE2("app_gfps_msg_handle_ring_event: state 0x%02x, event 0x%02x", gfps_msg_ring_state,
                     event);
}

#if GFPS_SASS_SUPPORT
bool app_gfps_msg_report_session_nonce(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                       uint8_t local_server_chann)
{
    uint8_t tmp[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t session_nonce[8] = {0};
    uint8_t *p = session_nonce;
    uint8_t data_len = 8;

    gfps_gen_random_nonce(tmp);
    APP_PRINT_TRACE1("app_gfps_msg_report_session_nonce: tmp 0x%b", TRACE_BINARY(8, tmp));

    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
        if (p_link)
        {
            memcpy(p_link->gfps_session_nonce, tmp, 8);
            APP_PRINT_TRACE1("app_gfps_msg_report_session_nonce: edr tmp 0x%b", TRACE_BINARY(8,
                             p_link->gfps_session_nonce));
        }

        ARRAY_TO_STREAM(p, tmp, 8);
    }
    else
    {
        T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_link)
        {
            memcpy(p_link->gfps_session_nonce, tmp, 8);
            APP_PRINT_TRACE1("app_gfps_msg_report_session_nonce: le tmp 0x%b", TRACE_BINARY(8,
                                                                                            p_link->gfps_session_nonce));
        }

        ARRAY_TO_STREAM(p, tmp, 8);
    }

    if (bt_gfps_send_session_nonce(conn_id, cid, bd_addr, local_server_chann, session_nonce, data_len))
    {
        APP_PRINT_TRACE0("app_gfps_msg_report_session_nonce: send success");
        return true;
    }
    APP_PRINT_TRACE0("app_gfps_msg_report_session_nonce: send fail");
    return false;
}
bool app_gfps_msg_report_sass_cap(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                  uint8_t local_server_chann)
{
    uint8_t p_data[4] = {0};
    uint8_t data_len = 4;
    uint8_t *p = p_data;
    uint16_t sass_version_code = GFPS_SASS_VERSION;
    uint16_t  sass_cap_flags = 0;

    if (extend_app_cfg_const.gfps_sass_support)
    {
        sass_cap_flags |= GFPS_SASS_STATE_ON;
    }
    if (app_cfg_const.enable_multi_link)
    {
        sass_cap_flags |= GFPS_SASS_MULTI_POINT_ENABLE;
    }
    if ((app_sass_policy_get_original_enable_multi_link()) &&
        (extend_app_cfg_const.gfps_sass_support_dyamic_multilink_switch))
    {
        sass_cap_flags |= GFPS_SASS_MULTI_POINT_CFG_SUPPORT;
    }
    if (app_cfg_const.sensor_support)
    {
        sass_cap_flags |= GFPS_SASS_ON_HEAD_SUPPORT;
        if (LIGHT_SENSOR_ENABLED)
        {
            sass_cap_flags |= GFPS_SASS_ON_HEAD_ENABLE;
        }
    }
    BE_UINT16_TO_STREAM(p, sass_version_code);
    BE_UINT16_TO_STREAM(p, sass_cap_flags);

    APP_PRINT_TRACE6("app_gfps_msg_report_sass_cap: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                     app_cfg_const.enable_multi_link, app_sass_policy_get_original_enable_multi_link(),
                     extend_app_cfg_const.gfps_sass_support_dyamic_multilink_switch,
                     app_cfg_const.sensor_support, app_cfg_nv.sass_multipoint_state, sass_cap_flags);
    if (bt_gfps_notify_sass_cap(conn_id, cid, bd_addr, local_server_chann, p_data, data_len))
    {
        return true;
    }
    return false;
}

bool app_gfps_rfc_report_sass_switch_preference(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                                uint8_t local_server_chann)
{
    uint8_t p_data[2] = {0};
    uint8_t data_len = 2;
    uint8_t *p = p_data;
    uint8_t switch_prefer_flag = GFPS_SWITCH_PREFER_HFP_A2DP;
    uint8_t switch_setting = 0;
    switch_prefer_flag = app_sass_policy_get_switch_preference();
    switch_setting = app_sass_policy_get_advanced_switching_setting();
    BE_UINT8_TO_STREAM(p, switch_prefer_flag);
    BE_UINT8_TO_STREAM(p, switch_setting);
    if (bt_gfps_notify_switch_prefer(conn_id, cid, bd_addr, local_server_chann, p_data, data_len))
    {
        return true;
    };
    return false;
}

void  app_gfps_msg_notify_multipoint_switch(uint8_t active_index, uint8_t link_type,
                                            uint8_t switch_reason)
{
    uint8_t target_device = GFPS_MULTIPOINT_SWITCH_THIS_DEVICE;
    uint8_t p_data[4] = {0};
    uint8_t data_len = 4;
    uint8_t target_addr[2] = {0};

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED
         && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (link_type == SASS_EDR_LINK)
        {
            memcpy(target_addr, app_db.br_link[active_index].bd_addr, 2);
        }
        else if (link_type == SASS_LE_LINK)
        {
            memcpy(target_addr, app_db.le_link[active_index].bd_addr, 2);
        }

        APP_PRINT_INFO5("app_gfps_msg_notify_multipoint_switch: active_id %d, link_type %d, reason 0x%x, target_addr 0x%x:0x%x",
                        active_index, link_type, switch_reason, target_addr[0], target_addr[1]);

        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[i];
            if (p_link->used == true && (p_link->connected_profile & GFPS_PROFILE_MASK))
            {
                uint8_t *p = p_data;
                target_device = (active_index == i &&
                                 link_type == SASS_EDR_LINK) ? GFPS_MULTIPOINT_SWITCH_THIS_DEVICE :
                                GFPS_MULTIPOINT_SWITCH_OTHER_DEVICE;
                BE_UINT8_TO_STREAM(p, switch_reason);
                BE_UINT8_TO_STREAM(p, target_device);
                //TODO
                //use bd address now, review how much ram increased if need to use remote BT device name
                BE_UINT8_TO_STREAM(p, target_addr[0]);
                BE_UINT8_TO_STREAM(p, target_addr[1]);
                bt_gfps_notify_multipoint_switch(0xFF, 0, p_link->bd_addr, p_link->gfps_rfc_chann, p_data,
                                                 data_len);
                APP_PRINT_INFO3("app_gfps_msg_notify_multipoint_switch: edr_id %d, addr %s, target 0x%x",
                                p_link->id, TRACE_BDADDR(p_link->bd_addr), target_device);
            }
        }

        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            T_APP_LE_LINK *p_le_link = &app_db.le_link[i];

            if ((p_le_link->used == true) &&
                (p_le_link->gfps_link.gfps_conn_id != 0xff))
            {
                uint8_t *p = p_data;
                target_device = (active_index == i &&
                                 link_type == SASS_LE_LINK) ? GFPS_MULTIPOINT_SWITCH_THIS_DEVICE :
                                GFPS_MULTIPOINT_SWITCH_OTHER_DEVICE;
                BE_UINT8_TO_STREAM(p, switch_reason);
                BE_UINT8_TO_STREAM(p, target_device);
                //TODO
                //use bd address now, review how much ram increased if need to use remote BT device name
                BE_UINT8_TO_STREAM(p, target_addr[0]);
                BE_UINT8_TO_STREAM(p, target_addr[1]);
                bt_gfps_notify_multipoint_switch(p_le_link->gfps_link.gfps_conn_id,
                                                 p_le_link->gfps_link.gfps_msg_cid, NULL, 0, p_data,
                                                 data_len);
                APP_PRINT_INFO3("app_gfps_msg_notify_multipoint_switch: le_id %d, addr %s, target 0x%x",
                                p_le_link->id, TRACE_BDADDR(p_le_link->bd_addr), target_device);
            }
        }
    }
}

uint8_t app_gfps_msg_get_conn_state(T_APP_BR_LINK *p_link)
{
    uint8_t state = SASS_CONN_NO_DATA_TRANS;

    if (p_link->call_status != APP_CALL_IDLE)
    {
        state = SASS_HFP;
    }
    else if (p_link->streaming_fg)
    {
        if ((p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
            (!app_multi_get_stream_only(p_link->id)))
        {
            state = SASS_A2DP_WITH_AVRCP;
        }
        else
        {
            state = SASS_A2DP_ONLY;
        }
    }
    else
    {
        state = SASS_CONN_NO_DATA_TRANS;
    }

    return state;
}

bool app_gfps_msg_notify_connection_status(void)//per link
{
    bool ret = false;
    uint8_t msg_nonce[8] = {0x66, 0x5B, 0x52, 0x91, 0x7F, 0x25, 0x07, 0x16};
    uint8_t flag = GFPS_NON_ACTIVE_DEV;
    T_SASS_CONN_STATUS_FIELD conn_status;
    uint8_t inuse_account_key_index = gfps_get_inuse_account_key_index();

    gfps_gen_random_nonce(msg_nonce);
    gfps_sass_get_conn_status(&conn_status);
    app_gfps_gen_conn_status(&conn_status);
    gfps_sass_set_conn_status(&conn_status);

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED
         && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        bool active_streaming_no_sass = false;
        T_APP_LE_LINK *p_active_le_link = NULL;
        uint8_t active_index = MAX_BR_LINK_NUM;
        uint8_t active_key_id = 0xFF;

#if GFPS_SASS_LEA_SUPPORT
        if (mtc_get_btmode())
        {
            p_active_le_link = app_lea_uca_get_active_link();

            if (p_active_le_link)
            {
                active_key_id = p_active_le_link->gfps_inuse_account_key;
                if ((p_active_le_link->gfps_inuse_account_key == 0xff) &&
                    ((app_lea_uca_get_stream_link()) ||
                     (app_lea_ccp_get_call_status() != APP_CALL_IDLE)))
                {
                    active_streaming_no_sass = true;
                }
                APP_PRINT_INFO1("app_gfps_msg_notify_connection_status_le: active_index %d", p_active_le_link->id);
            }
        }
        else
#endif
        {
            active_index = app_multi_get_active_idx();

            if (active_index < MAX_BR_LINK_NUM)
            {
                active_key_id = app_db.br_link[active_index].gfps_inuse_account_key;
                if ((app_db.br_link[active_index].gfps_inuse_account_key == 0xff) &&
                    ((app_db.br_link[active_index].streaming_fg == true) ||
                     (app_db.br_link[active_index].call_status != APP_CALL_IDLE)))
                {
                    active_streaming_no_sass = true;
                }
                APP_PRINT_INFO1("app_gfps_msg_notify_connection_status_edr: active_index %d", active_index);
            }
        }

        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            T_APP_BR_LINK *p_edr_link = &app_db.br_link[i];

            // app_gfps_gen_conn_status(&conn_status);

            if (i == active_index)
            {
                flag = GFPS_ACTIVE_DEV;
            }
            else if (active_streaming_no_sass)
            {

                flag = GFPS_NON_ACTIVE_DEV_WITH_ACTIVE_NON_SASS;
            }
            else
            {
                flag = GFPS_NON_ACTIVE_DEV;
            }

            //notify the active account key user only
            if (p_edr_link->used == true &&
                (p_edr_link->connected_profile & GFPS_PROFILE_MASK))
            {
                uint8_t session_nonce_tmp[8];
                uint8_t *p = session_nonce_tmp;

                ARRAY_TO_STREAM(p, p_edr_link->gfps_session_nonce, 8);
                T_SASS_CONN_STATUS_FIELD link_conn_status;
                memcpy(&link_conn_status, &conn_status, sizeof(T_SASS_CONN_STATUS_FIELD));

                //active non SASS, send non SASS device conn state; active another SASS, send local conn state
                if ((active_key_id != 0xFF) &&
                    !gfps_sass_compare_account_key_by_idx(active_key_id, p_edr_link->gfps_inuse_account_key))
                {
                    link_conn_status.conn_status_info.conn_state = app_gfps_msg_get_conn_state(p_edr_link);
                }

                APP_PRINT_INFO8("app_gfps_msg_notify_connection_status_edr: addr %s, index: %d, %d, account_key: %d, %d, %d, conn_state: %d, 0x%x",
                                TRACE_BDADDR(p_edr_link->bd_addr), i, active_index, active_key_id,
                                inuse_account_key_index, p_edr_link->gfps_inuse_account_key,
                                conn_status.conn_status_info.conn_state, link_conn_status.conn_status_info.conn_state);
                APP_PRINT_INFO1("app_gfps_msg_notify_connection_status_edr: flag 0x%x", flag);
                ret = bt_gfps_notify_connection_status(0xFF, 0, p_edr_link->bd_addr, p_edr_link->gfps_rfc_chann,
                                                       p_edr_link->gfps_inuse_account_key, flag, link_conn_status,
                                                       msg_nonce, session_nonce_tmp);
            }
        }

        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            T_APP_LE_LINK *p_le_link = &app_db.le_link[i];

            if (p_le_link->used == true &&
                (p_le_link->gfps_link.gfps_conn_id != 0xff) &&
                (p_le_link->gfps_inuse_account_key != 0xff))
            {
                if ((p_active_le_link != NULL) && (p_le_link == p_active_le_link))
                {
                    flag = GFPS_ACTIVE_DEV;
                }
                else if (active_streaming_no_sass)
                {

                    flag = GFPS_NON_ACTIVE_DEV_WITH_ACTIVE_NON_SASS;
                }
                else
                {
                    flag = GFPS_NON_ACTIVE_DEV;
                }

                uint8_t session_nonce_tmp[8];
                uint8_t *p = session_nonce_tmp;

                ARRAY_TO_STREAM(p, p_le_link->gfps_session_nonce, 8);

                T_SASS_CONN_STATUS_FIELD link_conn_status;
                memcpy(&link_conn_status, &conn_status, sizeof(T_SASS_CONN_STATUS_FIELD));

                if ((active_key_id != 0xFF) &&
                    !gfps_sass_compare_account_key_by_idx(active_key_id, p_le_link->gfps_inuse_account_key))
                {
                    link_conn_status.conn_status_info.conn_state = app_sass_policy_context_to_conn_state(p_le_link);
                }

                ret = bt_gfps_notify_connection_status(p_le_link->conn_id, p_le_link->gfps_link.gfps_msg_cid, NULL,
                                                       0,
                                                       p_le_link->gfps_inuse_account_key, flag, link_conn_status,
                                                       msg_nonce, session_nonce_tmp);

                APP_PRINT_INFO7("app_gfps_msg_notify_connection_status_le: addr %s, index: %d, account_key: %d, %d, %d, conn_state: %d, cid 0x%x",
                                TRACE_BDADDR(p_le_link->bd_addr), i, active_key_id,
                                inuse_account_key_index, p_le_link->gfps_inuse_account_key,
                                conn_status.conn_status_info.conn_state, p_le_link->gfps_link.gfps_msg_cid);
                APP_PRINT_INFO4("app_gfps_msg_notify_connection_status_le: %d, %d, 0x%x, flag 0x%x",
                                p_le_link->conn_id, ret, link_conn_status.conn_status_info.conn_state, flag);
            }
        }
    }
    return ret;
}

static void app_gfps_rfc_report_nac(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                    uint8_t local_server_chann,
                                    uint8_t msg_group, uint8_t msg_code, uint8_t error_code)
{
    uint8_t ack_pkt[2];
    uint8_t *temp = ack_pkt;
    BE_UINT8_TO_STREAM(temp, msg_group);
    BE_UINT8_TO_STREAM(temp, msg_code);
    bt_gfps_send_nak(conn_id, cid, bd_addr, local_server_chann, ack_pkt, 2, error_code);
}
static void app_gfps_rfc_report_mac_error(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                          uint8_t local_server_chann,
                                          uint8_t msg_group, uint8_t msg_code)
{
    app_gfps_rfc_report_nac(conn_id, cid, bd_addr, local_server_chann, msg_group, msg_code,
                            GFPS_ERROR_MAC);
}

void app_gfps_report_anc_state(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                               uint8_t local_server_chann)
{
    uint8_t sass_version = GFPS_HEARABLE_VERSION;
    uint16_t anc_state = GFPS_NO_ANC;
#if (F_APP_ANC_SUPPORT && F_APP_APT_SUPPORT)
    uint8_t llapt_cnt = app_apt_get_llapt_selected_scenario_cnt() == 0 ?
                        app_apt_get_llapt_activated_scenario_cnt() : app_apt_get_llapt_selected_scenario_cnt();
    uint8_t anc_cnt = app_anc_get_selected_scenario_cnt() == 0 ? app_anc_get_activated_scenario_cnt() :
                      app_anc_get_selected_scenario_cnt();

    if ((llapt_cnt != 0) || (anc_cnt != 0))
    {
        if ((app_cfg_nv.listening_mode_cycle == 0) ||
            (app_cfg_nv.listening_mode_cycle == 1) ||
            (app_cfg_nv.listening_mode_cycle == 2))
        {
            anc_state |= GFPS_ANC_PATTERN3;
            if (app_apt_is_apt_on_state((T_ANC_APT_STATE)*app_db.final_listening_state))
            {
                anc_state |= GFPS_ANC_PATTERN3_TRANS_MODE;
            }
            else if (app_anc_is_anc_on_state((T_ANC_APT_STATE)*app_db.final_listening_state))
            {
                anc_state |= GFPS_ANC_PATTERN3_ON;
            }
            else
            {
                anc_state |= GFPS_ANC_PATTERN3_OFF;
            }
        }
        else if (app_cfg_nv.listening_mode_cycle == 3)
        {
            anc_state |= GFPS_ANC_PATTERN1;
            if (app_anc_is_anc_on_state((T_ANC_APT_STATE)*app_db.final_listening_state))
            {
                anc_state |= GFPS_ANC_PATTERN1_ON;
            }
            else
            {
                anc_state |= GFPS_ANC_PATTERN1_OFF;
            }
        }
    }
#endif
    uint8_t p_data[3] = {0};
    uint8_t data_len = 3;
    uint8_t *p = p_data;
    BE_UINT8_TO_STREAM(p, sass_version);
    BE_UINT16_TO_STREAM(p, anc_state);
    bt_gfps_send_anc_state(conn_id, cid, bd_addr, local_server_chann, p_data, data_len);

}
void app_gfps_notify_anc_state(void)
{
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_db.br_link[i].used == true && (app_db.br_link[i].connected_profile & GFPS_PROFILE_MASK))
            {
                T_APP_BR_LINK *p_link = &app_db.br_link[i];
                app_gfps_report_anc_state(0xFF, 0, p_link->bd_addr, p_link->gfps_rfc_chann);
            }
        }

        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if (app_db.le_link[i].used == true &&
                (app_db.le_link[i].gfps_link.gfps_conn_state == GAP_CONN_STATE_CONNECTED))
            {
                app_gfps_report_anc_state(app_db.le_link[i].conn_id, app_db.le_link[i].gfps_link.gfps_msg_cid, NULL,
                                          0);
            }
        }
    }
}
bool app_gfps_set_anc_state(uint8_t version, uint16_t anc_state)
{
    bool ret = true;
    uint8_t mmi_action =  MMI_NULL;
    if (version == GFPS_HEARABLE_VERSION)
    {
        ret = true;
        uint16_t pattern = anc_state & 0xff00;
        uint8_t action = (uint8_t) anc_state;
        switch (pattern)
        {
        case GFPS_ANC_PATTERN1:
            {
                if (action == GFPS_ANC_PATTERN1_OFF)
                {
                    mmi_action = MMI_AUDIO_ANC_APT_ALL_OFF;
                }
                else
                {
                    mmi_action = MMI_AUDIO_ANC_ON;
                }
            }
            break;
        case GFPS_ANC_PATTERN2:
            {
                if (action == GFPS_ANC_PATTERN2_OFF)
                {
                    mmi_action = MMI_AUDIO_ANC_APT_ALL_OFF;
                }
                else
                {
                    mmi_action = MMI_AUDIO_ANC_ON;
                }

            }
            break;
        case GFPS_ANC_PATTERN3:
            {
                if (action == GFPS_ANC_PATTERN3_TRANS_MODE)
                {
                    mmi_action = MMI_AUDIO_APT_ON;
                }
                else if (action == GFPS_ANC_PATTERN3_OFF)
                {
                    mmi_action = MMI_AUDIO_ANC_APT_ALL_OFF;
                }
                else
                {
                    mmi_action = MMI_AUDIO_ANC_ON;
                }
            }
            break;
        default:
            ret = false;
            break;
        }

    }
    if (ret)
    {
#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_sync_single(APP_MODULE_TYPE_MMI, mmi_action, REMOTE_TIMER_HIGH_PRECISION,
                                  0, false);
        }
        else
#endif
        {
            app_mmi_handle_action(mmi_action);

        }
    }
    return ret;
}
void app_gfps_rfc_report_dynamic_buffer_size(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                             uint8_t local_server_chann)
{
    uint8_t *p_data = NULL;
    uint8_t *p;
    uint8_t codec_num = 1;
    uint8_t data_len  = 0;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint16_t default_latency = app_cfg_nv.cmd_latency_in_ms;
#else
    uint16_t default_latency = A2DP_LATENCY_MS;
#endif
    uint16_t max_latency = default_latency;
    uint16_t min_latency = 20;

    if (app_cfg_const.a2dp_codec_type_aac)
    {
        codec_num ++;
    }
#if (F_APP_A2DP_CODEC_LDAC_SUPPORT == 1)
    if (app_cfg_const.a2dp_codec_type_ldac)
    {
        codec_num ++;
    }
#endif

    data_len = codec_num * GFPS_BUFFER_SIZE_INFO_LEN_PER_CODEC;
    p_data = malloc(data_len);

    if (p_data != NULL)
    {
        p = p_data;
        /*
         1 byte codec type
         2 bytes maximum buffer size
         2 bytes minimum buffer size
         2 bytes default buffer size
        */
        app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, AUDIO_FORMAT_TYPE_SBC, 0,
                                             &min_latency);
        BE_UINT8_TO_STREAM(p, GFPS_CODEC_SBC);
        BE_UINT16_TO_STREAM(p, max_latency);//use defefault buffer size as max
        BE_UINT16_TO_STREAM(p, min_latency);//use defefault buffer size as max
        BE_UINT16_TO_STREAM(p, default_latency);//use defefault buffer size as max
        if (app_cfg_const.a2dp_codec_type_aac)
        {
            app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, AUDIO_FORMAT_TYPE_AAC, 0,
                                                 &min_latency);
            BE_UINT8_TO_STREAM(p, GFPS_CODEC_AAC);
            BE_UINT16_TO_STREAM(p, max_latency);//use defefault buffer size as max
            BE_UINT16_TO_STREAM(p, min_latency);//use defefault buffer size as max
            BE_UINT16_TO_STREAM(p, default_latency);//use defefault buffer size as max

        }
#if (F_APP_A2DP_CODEC_LDAC_SUPPORT == 1)
        if (app_cfg_const.a2dp_codec_type_ldac)
        {
            //no LDAC low latency setting, use AAC
            app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_LOW_LATENCY, AUDIO_FORMAT_TYPE_AAC, 0,
                                                 &min_latency);
            BE_UINT8_TO_STREAM(p, GFPS_CODEC_LDAC);
            BE_UINT16_TO_STREAM(p, max_latency);//use defefault buffer size as max
            BE_UINT16_TO_STREAM(p, min_latency);//use defefault buffer size as max
            BE_UINT16_TO_STREAM(p, default_latency);//use defefault buffer size as max
        }
#endif

        bt_gfps_send_dynamic_buffer_size(conn_id, cid, bd_addr, local_server_chann, p_data, data_len);
        free(p_data);
    }

}
void app_gfps_rfc_set_buffer_size(T_GFPS_CODEC_TYPE codec_type, uint16_t latency)
{
    //TODO
}
void app_gfps_msg_update_inuse_account_key_index(uint8_t conn_id, uint8_t *bd_addr, uint8_t index)
{
    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            p_link->gfps_inuse_account_key = index;
        }
    }
    else
    {
        T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_link != NULL)
        {
            p_link->gfps_inuse_account_key = index;
        }
    }

    APP_PRINT_INFO3("app_gfps_msg_update_inuse_account_key_index: index %d, conn_id %d, bd_addr %s",
                    index, conn_id, TRACE_BDADDR(bd_addr));
}

void app_gfps_msg_update_custom_data(uint8_t custom_data)
{
    T_SASS_CONN_STATUS_FIELD conn_status;
    gfps_sass_get_conn_status(&conn_status);
    conn_status.custom_data = custom_data;
    gfps_sass_set_conn_status(&conn_status);
#if F_APP_ERWS_SUPPORT
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        uint8_t buf = custom_data;
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                            GFPS_RFC_REMOTE_MSG_SYNC_CUSTOM_DATA,
                                            &buf, 1);

    }
#endif
}
void app_gfps_msg_update_target_drop_device(uint8_t conn_id, uint8_t *bd_addr)
{
    uint8_t drop_addr[6] = {0};

    if (conn_id == 0xFF)
    {
        memcpy(drop_addr, bd_addr, 6);
    }
#if GFPS_SASS_LEA_SUPPORT
    else
    {
        T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_link)
        {
            memcpy(drop_addr, p_link->bd_addr, 6);
        }
    }
#endif

#if F_APP_ERWS_SUPPORT
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        uint8_t buf[6] = {0};
        memcpy(buf, drop_addr, 6);
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                            GFPS_RFC_REMOTE_MSG_SYNC_TARGET_DROP_DEVICE,
                                            buf, 6);

    }
#endif
    memcpy(app_db.sass_target_drop_device, bd_addr, 6);
}
#endif

/**
 * @brief Used to handle GFPS message stream
 *
 * @param bd_addr
 * @param server_chann
 * @param p_cmd
 * @param cmd_len
 */
void app_gfps_msg_handle_data_transfer(uint8_t conn_id, uint16_t cid, uint8_t *bd_addr,
                                       uint8_t server_chann,
                                       uint8_t *p_cmd, uint16_t cmd_len)
{
    uint8_t msg_group;
    uint8_t msg_code;
    uint8_t *p;
    uint16_t len;

    p = p_cmd;
    len = cmd_len;

    while (len >= GFPS_HEADER_LEN)
    {
        bool unknow_cmd = false;

        BE_STREAM_TO_UINT8(msg_group, p);
        BE_STREAM_TO_UINT8(msg_code, p);
        APP_PRINT_TRACE5("app_gfps_msg_handle_data_transfer: msg_group 0x%02x, msg_code 0x%02x, len %d, conn_id 0x%x, addr %s",
                         msg_group, msg_code, len, conn_id, TRACE_BDADDR(bd_addr));

        if (msg_group == GFPS_DEVICE_INFO_EVENT)
        {
            switch (msg_code)
            {
            case GFPS_ACITVE_COMPONENTS_REQ:
                {
                    uint8_t active_components = GFPS_RIGHT_ACTIVE;

                    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                    {
                        active_components = GFPS_RIGHT_ACTIVE;
                    }
                    else
                    {
                        if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                        {
                            active_components = GFPS_LEFT_ACTIVE;
                        }
                        else if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                        {
                            active_components = GFPS_RIGHT_ACTIVE;
                        }

                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                        {
                            active_components = (GFPS_LEFT_ACTIVE | GFPS_RIGHT_ACTIVE);
                        }
                    }

                    APP_PRINT_INFO1("active_components %d", active_components);

#if GFPS_LE_DEVICE_SUPPORT
                    if ((conn_id != 0xFF) && extend_app_cfg_const.gfps_le_device_support)
                    {
                        bt_gfps_active_components_rsp(conn_id, cid, NULL, 0,
                                                      &active_components,
                                                      GFPS_ACTIVE_COMPONENTS_LEN);
                    }
#endif

                    if (bd_addr != NULL)
                    {
                        bt_gfps_active_components_rsp(0xFF, 0, bd_addr, server_chann,
                                                      &active_components,
                                                      GFPS_ACTIVE_COMPONENTS_LEN);
                    }

                    len -= GFPS_HEADER_LEN;
                    p += GFPS_ADDITIONAL_DATA_LEN;
                }
                break;

            case GFPS_CAPABILITIES:
                {
                    uint8_t capabilities;

                    p += GFPS_ADDITIONAL_DATA_LEN;
                    BE_STREAM_TO_UINT8(capabilities, p);
                    APP_PRINT_TRACE1("app_gfps_msg_handle_data_transfer: capabilities 0x%02x", capabilities);
                    len -= (GFPS_HEADER_LEN + GFPS_SUPPORT_CPBS_LEN);
                }
                break;

            case GFPS_PLATFORM_TYPE:
                {
                    uint8_t platform_type = 0;
                    uint8_t platform_version = 0;
                    p += GFPS_ADDITIONAL_DATA_LEN;
                    BE_STREAM_TO_UINT8(platform_type, p);
                    BE_STREAM_TO_UINT8(platform_version, p);
                    APP_PRINT_TRACE2("app_gfps_msg_handle_data_transfer: platform type 0x%x, platform version 0x%x",
                                     platform_type,
                                     platform_version);
                    len -= (GFPS_HEADER_LEN + GFPS_PLATFORM_TYPE_LEN);
                }
                break;

            case GFPS_FIRMWARE_REVISION:
                {
#if GFPS_LE_DEVICE_SUPPORT
                    if ((conn_id != 0xFF) && extend_app_cfg_const.gfps_le_device_support)
                    {
                        bt_gfps_send_firmware_revision(conn_id, cid, NULL, 0,
                                                       extend_app_cfg_const.gfps_version,
                                                       sizeof(extend_app_cfg_const.gfps_version));
                    }
#endif

                    if (bd_addr != NULL)
                    {
                        bt_gfps_send_firmware_revision(0xFF, 0, bd_addr, server_chann,
                                                       extend_app_cfg_const.gfps_version,
                                                       sizeof(extend_app_cfg_const.gfps_version));
                    }
                }
                break;

            default:
                {
                    unknow_cmd = true;
                }
                break;
            }
        }
        else if (msg_group == GFPS_DEVICE_ACTION_EVENT)
        {
            switch (msg_code)
            {
            case GFPS_RING:
                {
                    T_GFPS_MSG_RING_STATE pre_state = app_gfps_msg_get_ring_state();
                    uint8_t ring_type;
                    uint16_t ring_len;
                    uint8_t data[GFPS_RING_ACK_LEN];
                    uint8_t *temp = data;

                    BE_STREAM_TO_UINT16(ring_len, p);
                    BE_STREAM_TO_UINT8(ring_type, p);

                    if (ring_type == GFPS_ALL_STOP)
                    {
                        app_gfps_msg_handle_ring_event(ring_type);
                    }
                    else
                    {
                        // The second byte in additional data, if present, represents the timeout in seconds
                        if (ring_len != GFPS_RING_LEN)
                        {
                            BE_STREAM_TO_UINT8(gfps_msg_ring_timeout, p);
                        }
                        app_gfps_msg_handle_ring_event(ring_type);
                    }
                    APP_PRINT_TRACE2("app_gfps_msg_handle_data_transfer: ring_type 0x%02x, gfps_msg_ring_timeout 0x%02x",
                                     ring_type, gfps_msg_ring_timeout);

                    BE_UINT8_TO_STREAM(temp, msg_group);
                    BE_UINT8_TO_STREAM(temp, msg_code);
                    BE_UINT8_TO_STREAM(temp, ring_type);
                    BE_UINT8_TO_STREAM(temp, gfps_msg_ring_timeout);

                    if (pre_state != app_gfps_msg_get_ring_state())
                    {
#if GFPS_LE_DEVICE_SUPPORT
                        if ((conn_id != 0xFF) && extend_app_cfg_const.gfps_le_device_support)
                        {
                            bt_gfps_send_ack(conn_id, cid, NULL, 0, data, GFPS_RING_ACK_LEN);
                        }
#endif

                        if (bd_addr != NULL)
                        {
                            bt_gfps_send_ack(0xFF, 0, bd_addr, server_chann, data, GFPS_RING_ACK_LEN);
                        }
                    }
                    else
                    {
#if GFPS_LE_DEVICE_SUPPORT
                        if ((conn_id != 0xFF) && extend_app_cfg_const.gfps_le_device_support)
                        {
                            bt_gfps_send_nak(conn_id, cid, NULL, 0, data, GFPS_RING_NAK_LEN, GFPS_NOT_ALLOW);
                        }
#endif

                        if (bd_addr != NULL)
                        {
                            bt_gfps_send_nak(0xFF, 0, bd_addr, server_chann, data, GFPS_RING_NAK_LEN, GFPS_NOT_ALLOW);
                        }
                    }

                    len -= (GFPS_HEADER_LEN + ring_len);
                }
                break;

            default:
                {
                    unknow_cmd = true;
                }
                break;
            }
        }
        else if (msg_group == GFPS_DEVICE_CAPABILITY_SYNC_EVENT)
        {
            uint16_t additional_data_len = 0;
            uint8_t message_nonce[8];
            uint8_t mac[8];
            BE_STREAM_TO_UINT16(additional_data_len, p);
            len -= (GFPS_HEADER_LEN + additional_data_len);
            switch (msg_code)
            {
            case GFPS_REQ_CAPABILITY_UPDATE:
                {
#if GFPS_SASS_SUPPORT
                    if (extend_app_cfg_const.gfps_sass_support)
                    {
                        if (additional_data_len == GFPS_REQ_CAPABILITY_UPDATE_PARA_LEN)
                        {
                            app_gfps_rfc_report_dynamic_buffer_size(conn_id, cid, bd_addr, server_chann);
                        }
                    }
#endif

#if GFPS_FINDER_SUPPORT
                    if (extend_app_cfg_const.gfps_finder_support)
                    {
                        app_gfps_finder_send_eddystone_capability(bd_addr, server_chann);
                    }
#endif
                }
                break;
            }
        }
#if GFPS_SASS_SUPPORT
        else if (msg_group == GFPS_DEVICE_RUNTIME_CFG_EVENT)
        {
            uint16_t additional_data_len = 0;
            uint8_t message_nonce[8];
            uint8_t mac[8];
            BE_STREAM_TO_UINT16(additional_data_len, p);
            len -= (GFPS_HEADER_LEN + additional_data_len);
            switch (msg_code)
            {
            case GFPS_DYNAMIC_BUFFER_SIZING:
                {
                    uint8_t codec_type = 0;
                    uint16_t latency = 0;
                    BE_STREAM_TO_UINT8(codec_type, p);
                    BE_STREAM_TO_UINT16(latency, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN)
                    {
                        uint8_t msg[3];
                        msg[0] = (uint8_t)codec_type;
                        msg[1] = latency >> 8;
                        msg[2] = (uint8_t) latency;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SET_ANC_STATE_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT16_TO_STREAM(temp, codec_type);
                            BE_UINT16_TO_STREAM(temp, latency);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_gfps_rfc_set_buffer_size((T_GFPS_CODEC_TYPE)codec_type, codec_type);
                        }
                        else
                        {
                            uint8_t ack_pkt[GFPS_MAC_ERROR_NAK_LEN + GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN];
                            uint8_t *temp = ack_pkt;
                            uint16_t current_latency = 220;
                            app_audio_get_last_used_latency(&current_latency);
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, codec_type);
                            BE_UINT16_TO_STREAM(temp, current_latency);
                            bt_gfps_send_nak(conn_id, cid, bd_addr, server_chann, ack_pkt,
                                             GFPS_MAC_ERROR_NAK_LEN + GFPS_DYNAMIC_BUFFER_SIZING_PARA_LEN, GFPS_ERROR_MAC);

                        }

                    }
                }
                break;
            }

        }
        else if (msg_group == GFPS_SMART_AUDIO_SWITCH)
        {
            uint16_t additional_data_len = 0;
            uint8_t message_nonce[8];
            uint8_t mac[8];
            BE_STREAM_TO_UINT16(additional_data_len, p);
            len -= (GFPS_HEADER_LEN + additional_data_len);
            switch (msg_code)
            {
            case GFPS_GET_SASS_CAP:
                {
                    if (additional_data_len == GFPS_GET_SASS_CAP_DATA_LEN)
                    {
                        app_sass_policy_set_capability(conn_id, bd_addr);
                        app_gfps_msg_report_sass_cap(conn_id, cid, bd_addr, server_chann);
                    }
                }
                break;
            case GFPS_NOTIFY_SASS_CAP:
                {
                    uint16_t sass_version = 0;
                    uint16_t sass_cap_flags = 0;
                    BE_STREAM_TO_UINT16(sass_version, p);
                    BE_STREAM_TO_UINT16(sass_cap_flags, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_NOTIFY_SASS_CAP_DATA_LEN)
                    {
                        //verify mac
                        uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_NOTIFY_SASS_CAP_PARA_LEN;
                        uint8_t ack_pkt[ack_len];
                        uint8_t *temp = ack_pkt;
                        BE_UINT8_TO_STREAM(temp, msg_group);
                        BE_UINT8_TO_STREAM(temp, msg_code);
                        BE_UINT16_TO_STREAM(temp, sass_version);
                        BE_UINT16_TO_STREAM(temp, sass_cap_flags);
                        bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                    }
                    APP_PRINT_INFO4("GFPS_NOTIFY_SASS_CAP: sass_version 0x%x, sass_cap_flags 0x%x, message_nonce %b, mac %b",
                                    sass_version, sass_cap_flags, TRACE_BINARY(8, message_nonce), TRACE_BINARY(8, mac));
                }
                break;
            case GFPS_SET_MULTI_POINT_STATE:
                {
                    uint8_t multi_point_state = 0;
                    BE_STREAM_TO_UINT8(multi_point_state, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SET_MULTI_POINT_STATE_DATA_LEN)
                    {
                        uint8_t msg[1] = {0};
                        msg[0] = multi_point_state;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SET_MULTI_POINT_STATE_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SET_MULTI_POINT_STATE_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, multi_point_state);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            //set multipoint enable
                            app_sass_policy_set_multipoint_state(multi_point_state);
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }

                    }
                }
                break;
            case GFPS_SET_SWITCH_PREFER:
                {
                    uint8_t switch_preference = 0;
                    uint8_t advanced_switching_setting = 0;
                    BE_STREAM_TO_UINT8(switch_preference, p);
                    BE_STREAM_TO_UINT8(advanced_switching_setting, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SET_SWITCH_PREFER_DATA_LEN)
                    {
                        uint8_t msg[2] = {0};
                        msg[0] = switch_preference;
                        msg[1] = advanced_switching_setting;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SET_SWITCH_PREFER_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SET_SWITCH_PREFER_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, switch_preference);
                            BE_UINT8_TO_STREAM(temp, advanced_switching_setting);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_sass_policy_set_switch_preference(switch_preference);//TODO
                            app_sass_policy_set_advanced_switching_setting(advanced_switching_setting);
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }

                    }
                }
                break;
            case GFPS_GET_SWITCH_PREFER:
                {
                    if (additional_data_len == GFPS_GET_SWITCH_PREFER_DATA_LEN)
                    {
                        app_gfps_rfc_report_sass_switch_preference(conn_id, cid, bd_addr, server_chann);//TODO
                    }
                }
                break;
            case GFPS_NOTIFY_SWITCH_PREFER:
                {
                    uint8_t switch_preference = 0;
                    uint8_t advanced_switching_setting = 0;
                    BE_STREAM_TO_UINT8(switch_preference, p);
                    BE_STREAM_TO_UINT8(advanced_switching_setting, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_NOTIFY_SWITCH_PREFER_DATA_LEN)
                    {
                        uint8_t msg[2] = {0};
                        msg[0] = switch_preference;
                        msg[1] = advanced_switching_setting;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_NOTIFY_SWITCH_PREFER_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_NOTIFY_SWITCH_PREFER_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, switch_preference);
                            BE_UINT8_TO_STREAM(temp, advanced_switching_setting);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_sass_policy_set_switch_preference(switch_preference);
                            app_sass_policy_set_advanced_switching_setting(advanced_switching_setting);
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }

                    }
                }
                break;
            case GFPS_SWITCH_ACTIVE_AUDIO_SOURCE:
                {
                    uint8_t sass_event_flags = 0;
                    BE_STREAM_TO_UINT8(sass_event_flags, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_DATA_LEN)
                    {
                        uint8_t msg[1];
                        msg[0] = sass_event_flags;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_PARA_LEN,
                                                            message_nonce, mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SWITCH_ACTIVE_AUDIO_SOURCE_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            uint8_t ret = SWITCH_RET_OK;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, sass_event_flags);
                            ret = app_sass_policy_switch_active_audio_source(conn_id, bd_addr, sass_event_flags,
                                                                             sass_event_flags & GFPS_SWITCH_TO_THIS_DEVICE);// 0b10000000
                            if (ret != SWITCH_RET_OK)
                            {
                                if (ret == SWITCH_RET_REDUNDANT)
                                {
                                    app_gfps_rfc_report_nac(conn_id, cid, bd_addr, server_chann, msg_group, msg_code,
                                                            GFPS_REDUNDANT_DEVICE_ACTION);
                                }
                                else
                                {
                                    app_gfps_rfc_report_nac(conn_id, cid, bd_addr, server_chann, msg_group, msg_code, GFPS_NOT_ALLOW);
                                }
                            }
                            else
                            {
                                bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            }

                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }

                    }
                }
                break;
            case GFPS_SWITCH_BACK:
                {
                    uint8_t switch_back_event = 0;
                    BE_STREAM_TO_UINT8(switch_back_event, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SWITCH_BACK_DATA_LEN)
                    {
                        uint8_t msg[1];
                        msg[0] = switch_back_event;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SWITCH_BACK_PARA_LEN, message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SWITCH_BACK_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, switch_back_event);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_sass_policy_switch_back(switch_back_event);
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }

                    }
                }
                break;
            case GFPS_GET_CONN_STATUS:
                {
                    if (additional_data_len == GFPS_GET_CONN_STATUS_DATA_LEN)
                    {
                        app_gfps_msg_notify_connection_status();
                    }
                }
                break;

            case GFPS_NOTIFY_SASS_INITIAL_CONN:
                {
                    uint8_t sass_initiated_connection = false;
                    BE_STREAM_TO_UINT8(sass_initiated_connection, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_NOTIFY_SASS_INITIAL_CONN_DATA_LEN)
                    {
                        uint8_t msg[1];
                        msg[0] = sass_initiated_connection;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_NOTIFY_SASS_INITIAL_CONN_PARA_LEN,
                                                            message_nonce, mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_NOTIFY_SASS_INITIAL_CONN_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, sass_initiated_connection);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            //app_sass_policy_switch_active_audio_source(bd_addr, 0x80, true);

                            app_sass_policy_initiated_connection(conn_id, bd_addr, sass_initiated_connection);
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }
                    }
                }
                break;

            case GFPS_IND_INUSE_ACCOUNTKEY:
                {
                    uint8_t in_use_string[6] = {0};
                    STREAM_TO_ARRAY(in_use_string, p, 6);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_IND_INUSE_ACCOUNTKEY_DATA_LEN)
                    {
                        {
                            uint8_t pair_idx_mapping = 0;
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_IND_INUSE_ACCOUNTKEY_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            uint8_t inuse_account_key_idx = 0;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            ARRAY_TO_STREAM(temp, in_use_string, 6);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            //update connection status with inuse account key
                            if (app_gfps_account_key_find_inuse_account_key(&inuse_account_key_idx, conn_id, bd_addr,
                                                                            in_use_string,
                                                                            GFPS_IND_INUSE_ACCOUNTKEY_PARA_LEN, message_nonce, mac))
                            {
                                app_gfps_msg_update_inuse_account_key_index(conn_id, bd_addr, inuse_account_key_idx);
                            }
                            app_gfps_notify_conn_status();
                        }
                    }
                }
                break;
            case GFPS_SEND_CUSTOM_DATA:
                {
                    uint8_t custom_data = 0;
                    BE_STREAM_TO_UINT8(custom_data, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SEND_CUSTOM_DATA_DATA_LEN)
                    {
                        uint8_t msg[1];
                        msg[0] = custom_data;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SEND_CUSTOM_DATA_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SEND_CUSTOM_DATA_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, custom_data);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_gfps_msg_update_custom_data(custom_data);
                            app_gfps_notify_conn_status();
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }
                    }
                }
                break;
            case GFPS_SET_DROP_CONN_TARGET:
                {
                    uint8_t is_this_device = 0;
                    BE_STREAM_TO_UINT8(is_this_device, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);
                    if (additional_data_len == GFPS_SET_DROP_CONN_TARGET_DATA_LEN)
                    {
                        uint8_t msg[1];
                        msg[0] = is_this_device;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SET_DROP_CONN_TARGET_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SET_DROP_CONN_TARGET_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, is_this_device);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            if (is_this_device)
                            {
                                app_gfps_msg_update_target_drop_device(conn_id, bd_addr);
                            }
                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }
                    }

                }
                break;
            default:
                {
                    unknow_cmd = true;
                }
                break;
            }
        }
        else if (msg_group == GFPS_HEARABLE_CONTROLEVENT)
        {
            uint16_t additional_data_len = 0;
            uint8_t message_nonce[8];
            uint8_t mac[8];
            BE_STREAM_TO_UINT16(additional_data_len, p);
            len -= (GFPS_HEADER_LEN + additional_data_len);
            switch (msg_code)
            {
            case GFPS_GET_ANC_STATE:
                {
                    if (additional_data_len == GFPS_GET_ANC_STATE_DATA_LEN)
                    {
                        app_gfps_report_anc_state(conn_id, cid, bd_addr, server_chann);
                    }
                }
                break;
            case GFPS_SET_ANC_STATE:
                {
                    uint8_t version = 0;
                    uint16_t anc_state = 0;
                    BE_STREAM_TO_UINT8(version, p);
                    BE_STREAM_TO_UINT16(anc_state, p);
                    STREAM_TO_ARRAY(message_nonce, p, 8);
                    STREAM_TO_ARRAY(mac, p, 8);

                    if (additional_data_len == GFPS_SET_ANC_STATE_DATA_LEN)
                    {
                        uint8_t msg[3];
                        msg[0] = version;
                        msg[1] = anc_state >> 8;
                        msg[2] = (uint8_t) anc_state;
                        if (app_gfps_account_key_verify_mac(conn_id, bd_addr, msg, GFPS_SET_ANC_STATE_PARA_LEN,
                                                            message_nonce,
                                                            mac))
                        {
                            uint8_t ack_len = GFPS_STANARD_ACK_PAYLOAD_LEN + GFPS_SET_ANC_STATE_PARA_LEN;
                            uint8_t ack_pkt[ack_len];
                            uint8_t *temp = ack_pkt;
                            BE_UINT8_TO_STREAM(temp, msg_group);
                            BE_UINT8_TO_STREAM(temp, msg_code);
                            BE_UINT8_TO_STREAM(temp, version);
                            BE_UINT16_TO_STREAM(temp, anc_state);
                            bt_gfps_send_ack(conn_id, cid, bd_addr, server_chann, ack_pkt, ack_len);
                            app_gfps_set_anc_state(version, anc_state);

                        }
                        else
                        {
                            app_gfps_rfc_report_mac_error(conn_id, cid, bd_addr, server_chann, msg_group, msg_code);
                        }
                    }
                }
                break;
            }
        }
#endif
        else
        {
            unknow_cmd = true;
        }

        if (unknow_cmd == true)
        {
            APP_PRINT_WARN0("app_gfps_msg_handle_data_transfer: unknow cmd");
            break;
        }
    }
}

void app_gfps_msg_loop_check_data_complete(uint8_t *bd_addr, uint8_t *data, uint16_t len,
                                           uint8_t local_server_chann)
{
    if (data == NULL)
    {
        return;
    }

    bool ret = false, cont = false;
    T_APP_BR_LINK *p_link = NULL;

    p_link = app_link_find_br_link(bd_addr);

    if (p_link != NULL)
    {
        uint8_t *p_buf;
        if (p_link->p_gfps_cmd == NULL)
        {
            p_buf = malloc(len);
            if (p_buf != NULL)
            {
                p_link->p_gfps_cmd = p_buf;
                memcpy(p_link->p_gfps_cmd, data, len);
                p_link->gfps_cmd_len = len;

                if (len >= GFPS_HEADER_LEN)
                {
                    uint8_t *p_temp;
                    uint16_t temp_len = 0;

                    p_temp = p_link->p_gfps_cmd;
                    p_temp += (GFPS_HEADER_LEN - GFPS_ADDITIONAL_DATA_LEN);
                    BE_STREAM_TO_UINT16(temp_len, p_temp);
                    uint8_t chunck_len = temp_len + GFPS_HEADER_LEN;

                    if (chunck_len == len)
                    {
                        ret = true; //one packet one cmd
                    }
                    else if (chunck_len < len)
                    {
                        //one packet several cmds
                        ret = true;
                        cont = true;
                        p_link->gfps_cmd_len = chunck_len;
                        data += chunck_len;
                        len -= chunck_len;
                    }
                    else
                    {
                        //not whole cmd, do nothing
                    }
                }
            }
        }
        else
        {
            p_buf = malloc(len + p_link->gfps_cmd_len);
            if (p_buf != NULL)
            {
                uint8_t pre_len = p_link->gfps_cmd_len;
                memcpy(p_buf, p_link->p_gfps_cmd, p_link->gfps_cmd_len);
                memcpy(p_buf + p_link->gfps_cmd_len, data, len);
                free(p_link->p_gfps_cmd);
                p_link->p_gfps_cmd = p_buf;
                p_link->gfps_cmd_len += len;

                if (p_link->gfps_cmd_len >= GFPS_HEADER_LEN)
                {
                    uint8_t *p_temp;
                    uint16_t temp_len = 0;

                    p_temp = p_buf;
                    p_temp += (GFPS_HEADER_LEN - GFPS_ADDITIONAL_DATA_LEN);
                    BE_STREAM_TO_UINT16(temp_len, p_temp);

                    uint8_t chunck_len = temp_len + GFPS_HEADER_LEN;

                    if (chunck_len == p_link->gfps_cmd_len)
                    {
                        ret = true;
                    }
                    else if (chunck_len < p_link->gfps_cmd_len)
                    {
                        ret = true;
                        cont = true;
                        p_link->gfps_cmd_len = chunck_len;
                        data += (chunck_len - pre_len);
                        len -= (chunck_len - pre_len);
                    }
                }
            }
            else
            {
                free(p_link->p_gfps_cmd);
                p_link->gfps_cmd_len = 0;
                p_link->p_gfps_cmd = NULL;
            }
        }
        APP_PRINT_TRACE4("app_gfps_msg_loop_check_data_complete: len %d, cmd_len %d, ret %d, cont %d\n",
                         len, p_link->gfps_cmd_len, ret, cont);
        if (ret)
        {
            app_gfps_msg_handle_data_transfer(0xFF, 0, bd_addr, local_server_chann,
                                              p_link->p_gfps_cmd, p_link->gfps_cmd_len);

            free(p_link->p_gfps_cmd);
            p_link->gfps_cmd_len = 0;
            p_link->p_gfps_cmd = NULL;
        }

        if (cont)
        {
            app_gfps_msg_loop_check_data_complete(bd_addr, data, len, local_server_chann);
        }
    }

    return;
}

void app_gfps_msg_le_loop_check_data_complete(uint8_t conn_id, uint16_t cid, uint8_t *data,
                                              uint16_t len)
{
    APP_PRINT_INFO3("app_gfps_msg_le_loop_check_data_complete: conn_id %d, cid 0x%x, data %b",
                    conn_id, cid, TRACE_BINARY(len, data));

    if (data == NULL)
    {
        return;
    }

    bool ret = false, cont = false;
    T_APP_LE_LINK *p_link = NULL;

    p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link != NULL)
    {
        uint8_t *p_buf;
        if (p_link->gfps_link.p_gfps_cmd == NULL)
        {
            p_buf = malloc(len);
            if (p_buf != NULL)
            {
                p_link->gfps_link.p_gfps_cmd = p_buf;
                memcpy(p_link->gfps_link.p_gfps_cmd, data, len);
                p_link->gfps_link.gfps_cmd_len = len;

                if (len >= GFPS_HEADER_LEN)
                {
                    uint8_t *p_temp;
                    uint16_t temp_len = 0;

                    p_temp = p_link->gfps_link.p_gfps_cmd;
                    p_temp += (GFPS_HEADER_LEN - GFPS_ADDITIONAL_DATA_LEN);
                    BE_STREAM_TO_UINT16(temp_len, p_temp);
                    uint8_t chunck_len = temp_len + GFPS_HEADER_LEN;

                    if (chunck_len == len)
                    {
                        ret = true; //one packet one cmd
                    }
                    else if (chunck_len < len)
                    {
                        //one packet several cmds
                        ret = true;
                        cont = true;
                        p_link->gfps_link.gfps_cmd_len = chunck_len;
                        data += chunck_len;
                        len -= chunck_len;
                    }
                    else
                    {
                        //not whole cmd, do nothing
                    }
                }
            }
        }
        else
        {
            p_buf = malloc(len + p_link->gfps_link.gfps_cmd_len);
            if (p_buf != NULL)
            {
                uint8_t pre_len = p_link->gfps_link.gfps_cmd_len;
                memcpy(p_buf, p_link->gfps_link.p_gfps_cmd, p_link->gfps_link.gfps_cmd_len);
                memcpy(p_buf + p_link->gfps_link.gfps_cmd_len, data, len);
                free(p_link->gfps_link.p_gfps_cmd);
                p_link->gfps_link.p_gfps_cmd = p_buf;
                p_link->gfps_link.gfps_cmd_len += len;

                if (p_link->gfps_link.gfps_cmd_len >= GFPS_HEADER_LEN)
                {
                    uint8_t *p_temp;
                    uint16_t temp_len = 0;

                    p_temp = p_buf;
                    p_temp += (GFPS_HEADER_LEN - GFPS_ADDITIONAL_DATA_LEN);
                    BE_STREAM_TO_UINT16(temp_len, p_temp);

                    uint8_t chunck_len = temp_len + GFPS_HEADER_LEN;

                    if (chunck_len == p_link->gfps_link.gfps_cmd_len)
                    {
                        ret = true;
                    }
                    else if (chunck_len < p_link->gfps_link.gfps_cmd_len)
                    {
                        ret = true;
                        cont = true;
                        p_link->gfps_link.gfps_cmd_len = chunck_len;
                        data += (chunck_len - pre_len);
                        len -= (chunck_len - pre_len);
                    }
                }
            }
            else
            {
                free(p_link->gfps_link.p_gfps_cmd);
                p_link->gfps_link.gfps_cmd_len = 0;
                p_link->gfps_link.p_gfps_cmd = NULL;
            }
        }
        APP_PRINT_TRACE4("app_gfps_msg_le_loop_check_data_complete: len %d, cmd_len %d, ret %d, cont %d\n",
                         len,
                         p_link->gfps_link.gfps_cmd_len, ret, cont);
        if (ret)
        {
            app_gfps_msg_handle_data_transfer(conn_id, cid, NULL, 0,
                                              p_link->gfps_link.p_gfps_cmd, p_link->gfps_link.gfps_cmd_len);

            free(p_link->gfps_link.p_gfps_cmd);
            p_link->gfps_link.gfps_cmd_len = 0;
            p_link->gfps_link.p_gfps_cmd = NULL;
        }

        if (cont)
        {
            app_gfps_msg_le_loop_check_data_complete(conn_id, cid, data, len);
        }
    }

    return;
}

static void app_gfps_msg_rfc_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
#if 0
    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                if (app_cfg_const.bud_side == BUD_SIDE_RIGHT)
                {
                    if (gfps_msg_ring_state == GFPS_MSG_LEFT_RING)
                    {
                        gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    }
                    else if (gfps_msg_ring_state == GFPS_MSG_ALL_RING)
                    {
                        gfps_msg_ring_state = GFPS_MSG_RIGHT_RING;
                    }
                }
                else if (app_cfg_const.bud_side == BUD_SIDE_LEFT)
                {
                    if (gfps_msg_ring_state == GFPS_MSG_RIGHT_RING)
                    {
                        gfps_msg_ring_state = GFPS_MSG_RING_STOP;
                    }
                    else if (gfps_msg_ring_state == GFPS_MSG_ALL_RING)
                    {
                        gfps_msg_ring_state = GFPS_MSG_LEFT_RING;
                    }
                }
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_gfps_msg_ring_stop();
                gfps_msg_ring_state = GFPS_MSG_RING_STOP;
            }
            APP_PRINT_INFO3("app_gfps_msg_rfc_bt_cback: gfps_msg_ring_state 0x%02x, bud_role %d, bud_side %d",
                            gfps_msg_ring_state, app_cfg_nv.bud_role, app_cfg_const.bud_side);
        }
        break;
#endif

#if GFPS_SASS_SUPPORT
    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if (extend_app_cfg_const.gfps_sass_support)
            {
                T_BT_EVENT_PARAM *param = event_buf;

                if (!memcmp(param->acl_conn_disconn.bd_addr, app_db.sass_target_drop_device, 6))
                {
                    uint8_t reset_value[6] = {0};
                    app_gfps_msg_update_target_drop_device(0xFF, reset_value);
                }
            }
        }
        break;
#endif
    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_gfps_msg_rfc_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_gfps_msg_rfc_audio_cback(T_AUDIO_EVENT event_type, void *event_buf,
                                         uint16_t buf_len)
{
    bool handle = true;
    T_AUDIO_EVENT_PARAM *param = event_buf;

#if GFPS_FINDER_SUPPORT
    static uint8_t default_volume = 0;
    T_GFPS_FINDER_RING_VOLUME_LEVEL ring_volume_level = GFPS_FINDER_RING_VOLUME_DEFAULT;

    if (extend_app_cfg_const.gfps_finder_support)
    {
        ring_volume_level = app_gfps_finder_get_ring_volume_level();
    }
#endif

    switch (event_type)
    {
    case AUDIO_EVENT_RINGTONE_STARTED:
    case AUDIO_EVENT_VOICE_PROMPT_STARTED:
        {
            uint8_t tone_started_idx;

            if (event_type == AUDIO_EVENT_VOICE_PROMPT_STARTED)
            {
                tone_started_idx = param->voice_prompt_started.index + GFPS_VOICE_PROMPT_OFFSET;
            }
            else
            {
                tone_started_idx = param->ringtone_started.index;
            }

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (tone_started_idx == app_cfg_const.tone_gfps_findme ||
                tone_started_idx == app_cfg_const.tone_harman_gfps_find_me)
#else
            if (tone_started_idx == app_cfg_const.tone_gfps_findme)
#endif
            {
#if GFPS_FINDER_SUPPORT
                if (extend_app_cfg_const.gfps_finder_support)
                {
                    default_volume = voice_prompt_volume_get();

#if HARMAN_OPEN_LR_FEATURE
                    harman_set_vp_ringtone_balance(__func__, __LINE__);
#endif
                    if (ring_volume_level == GFPS_FINDER_RING_VOLUME_LOW)
                    {
                        voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_min + 5);
                    }
                    else if (ring_volume_level == GFPS_FINDER_RING_VOLUME_HIGH)
                    {
                        voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_max);
                    }
                    else if (ring_volume_level == GFPS_FINDER_RING_VOLUME_MEDIUM)
                    {
                        voice_prompt_volume_set(app_dsp_cfg_vol.voice_prompt_volume_default);
                    }
                }
#endif
            }
        }
        break;

    case AUDIO_EVENT_RINGTONE_STOPPED:
    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:
        {
            uint8_t tone_stopped_idx;

            if (event_type == AUDIO_EVENT_VOICE_PROMPT_STOPPED)
            {
                tone_stopped_idx = param->voice_prompt_stopped.index + GFPS_VOICE_PROMPT_OFFSET;
            }
            else
            {
                tone_stopped_idx = param->ringtone_stopped.index;
            }

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (tone_stopped_idx == app_cfg_const.tone_gfps_findme ||
                tone_stopped_idx == app_cfg_const.tone_harman_gfps_find_me)
#else
            if (tone_stopped_idx == app_cfg_const.tone_gfps_findme)
#endif
            {
#if GFPS_FINDER_SUPPORT
                if (extend_app_cfg_const.gfps_finder_support)
                {
                    if (ring_volume_level != GFPS_FINDER_RING_VOLUME_DEFAULT)
                    {
#if HARMAN_OPEN_LR_FEATURE
                        harman_set_vp_ringtone_balance(__func__, __LINE__);
#endif
                        voice_prompt_volume_set(default_volume);
                    }
                }
#endif
                bool period_fg = false;

                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    if (gfps_msg_ring_state != GFPS_MSG_RING_STOP)
                    {
                        period_fg = true;
                    }
                }
                else
                {
                    if ((gfps_msg_ring_state == GFPS_MSG_RIGHT_RING) &&
                        (app_cfg_const.bud_side == BUD_SIDE_RIGHT))
                    {
                        period_fg = true;
                    }
                    else if ((gfps_msg_ring_state == GFPS_MSG_LEFT_RING) &&
                             (app_cfg_const.bud_side == BUD_SIDE_LEFT))
                    {
                        period_fg = true;
                    }
                    else if (gfps_msg_ring_state == GFPS_MSG_ALL_RING)
                    {
                        period_fg = true;
                    }
                }

                if (period_fg == true)
                {
                    app_start_timer(&timer_idx_gfps_ring_period, "gfps_ring_period",
                                    gfps_msg_timer_id, APP_TIMER_GFPS_RING_PERIOD, 0, false,
                                    GFPS_RING_PERIOD_VALUE * 1000);
                }
            }
        }
        break;

#if GFPS_SASS_SUPPORT
    case AUDIO_EVENT_ANC_ENABLED:
    case AUDIO_EVENT_ANC_DISABLED:
    case AUDIO_EVENT_PASSTHROUGH_ENABLED:
    case AUDIO_EVENT_PASSTHROUGH_DISABLED:
        {
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_gfps_notify_anc_state();
            }
        }
        break;
#endif

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_gfps_msg_rfc_audio_cback: event_type 0x%04x", event_type);
    }
}

static void app_gfps_msg_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE4("app_gfps_msg_timeout_cb: timer_evt %d, param %d, state 0x%02x, bud_side %d",
                     timer_evt, param, gfps_msg_ring_state, app_cfg_const.bud_side);
    switch (timer_evt)
    {
    case APP_TIMER_GFPS_RING_PERIOD:
        {
            app_stop_timer(&timer_idx_gfps_ring_period);
#if F_APP_HARMAN_FEATURE_SUPPORT
            if (app_cfg_nv.language_status == 0)
            {
                app_audio_tone_type_play(TONE_HARMAN_GFPS_FIND_ME, false, false);
            }
            else
#endif
            {
                app_audio_tone_type_play(TONE_GFPS_FINDME, false, false);
            }
        }
        break;

    case APP_TIMER_GFPS_RING_STOP:
        {
            app_gfps_msg_handle_ring_event(GFPS_ALL_STOP);
#if GFPS_FINDER_SUPPORT
            if (extend_app_cfg_const.gfps_finder_support)
            {
                uint8_t ring_state = GFPS_FINDER_RING_TIMEOUT_STOP;
                app_gfps_finder_send_ring_rsp(ring_state);
            }
#endif
        }
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
uint16_t app_gfps_msg_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    bool skip = true;
    APP_PRINT_INFO1("app_gfps_msg_relay_cback: msg_type %d", msg_type);

    uint16_t msg_len = 4;

    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_GFPS_MSG;
            buf[3] = msg_type;
        }
        APP_PRINT_INFO1("app_gfps_msg_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }

    return msg_len;
}

static void app_gfps_msg_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                     T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE3("app_gfps_msg_parse_cback: msg_type 0x%04x, status %d, ring_state 0x%02x",
                     msg_type, status, gfps_msg_ring_state);

    switch (msg_type)
    {
    case GFPS_REMOTE_MSG_FINDME_START:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_gfps_msg_ring_play();
        }
        else if (status == REMOTE_RELAY_STATUS_SYNC_TOUT)
        {
            app_gfps_msg_ring_play();
        }
        break;

    case GFPS_REMOTE_MSG_FINDME_STOP:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_gfps_msg_ring_stop();
        }
        else if (status == REMOTE_RELAY_STATUS_SYNC_TOUT)
        {
            app_gfps_msg_ring_stop();
        }
        break;

    case GFPS_REMOTE_MSG_FINDME_SYNC:
        {
            uint8_t *data = (uint8_t *)buf;

            gfps_msg_ring_state = (T_GFPS_MSG_RING_STATE)data[0];
            gfps_msg_ring_timeout = data[1] << 8 | data[2];
            APP_PRINT_TRACE2("app_gfps_msg_parse_cback: change to state 0x%02x, ring_timeout 0x%02x",
                             gfps_msg_ring_state, gfps_msg_ring_timeout);
        }
        break;

    default:
        break;
    }
}
#endif

void app_gfps_msg_update_ble_addr(void)
{
    uint8_t i;
    uint8_t ble_addr[6] = {0};
    app_gfps_get_random_addr(ble_addr);
    app_gfps_msg_reverse_data(ble_addr, 6);

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            (app_db.br_link[i].connected_profile & GFPS_PROFILE_MASK))
        {
            T_APP_BR_LINK *p_link = &app_db.br_link[i];

            if (p_link != NULL)
            {
                bt_gfps_send_ble_addr(0xFF, 0, p_link->bd_addr, p_link->gfps_rfc_chann, ble_addr);
            }

            APP_PRINT_INFO1("app_gfps_rfc_update_ble_addr: BR link_id %d", i);
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if ((app_db.le_link[i].used == true) &&
            (app_db.le_link[i].gfps_link.gfps_conn_id != 0xFF) &&
            (app_db.le_link[i].gfps_link.gfps_msg_cid != 0))
        {
            T_APP_LE_LINK *p_link = &app_db.le_link[i];

            if (p_link != NULL)
            {
                bt_gfps_send_ble_addr(app_db.le_link[i].gfps_link.gfps_conn_id,
                                      app_db.le_link[i].gfps_link.gfps_msg_cid,
                                      NULL, 0, ble_addr);
            }

            APP_PRINT_INFO1("app_gfps_rfc_update_ble_addr: BLE link_id %d", i);
        }
    }
}

void app_gfps_msg_rfc_init(void)
{
    if (app_cfg_const.supported_profile_mask & GFPS_PROFILE_MASK)
    {
        bt_gfps_rfc_init(app_gfps_rfc_cback, RFC_GFPS_CHANN_NUM);
        bt_mgr_cback_register(app_gfps_msg_rfc_bt_cback);
        audio_mgr_cback_register(app_gfps_msg_rfc_audio_cback);
        app_timer_reg_cb(app_gfps_msg_timeout_cb, &gfps_msg_timer_id);
#if F_APP_ERWS_SUPPORT
        app_relay_cback_register(app_gfps_msg_relay_cback, app_gfps_msg_parse_cback,
                                 APP_MODULE_TYPE_GFPS_MSG, GFPS_REMOTE_MSG_MAX_MSG_NUM);
#endif
    }
}
#endif
