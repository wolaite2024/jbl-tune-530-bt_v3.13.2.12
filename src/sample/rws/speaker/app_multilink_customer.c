#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include <string.h>
#include "os_mem.h"
#include "os_sched.h"
#include "trace.h"
#include "sysm.h"
#include "bt_hfp.h"
#include "btm.h"
#include "app_cfg.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_multilink.h"
#include "app_multilink_customer.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_bt_policy_api.h"
#include "app_audio_policy.h"
#include "app_mmi.h"

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_record.h"
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#include "app_usb_audio_hid.h"
#include "app_usb_mmi.h"
#include "usb_msg.h"
#include "app_ipc.h"

static bool usb_down_stream_running = false;
static bool usb_up_stream_running = false;
static uint8_t usb_stream_status = 0;
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"

#define LINE_IN_IPC_TOPIC   "line_in"

typedef enum t_line_in_ipc_evt
{
    LINE_IN_IPC_EVT_PLUG            = 0x0,
    LINE_IN_IPC_EVT_UNPLUG          = 0x1,
    LINE_IN_IPC_EVT_AUDIO_DS_START  = 0x2,
    LINE_IN_IPC_EVT_AUDIO_DS_STOP   = 0x3,
} T_LINE_IN_IPC_EVT;
#endif

uint8_t multilink_usb_idx = 0xff;
uint8_t multilink_line_in_idx = 0xee;
uint8_t multilink_dongle_idx = MULTILINK_SRC_CONNECTED; // invalid value

static T_APP_MULTILINK_CUSTOMER_DEVICE_MGR device_mgr[CUSTOMER_APP_MAX_LINK_NUM] = {0};
static T_APP_MULTILINK_CUSTOMER_ARRAY_DATA audio_link_mgr[CUSTOMER_APP_MAX_LINK_NUM] = {0};
static uint8_t active_music_idx = 0;
static uint8_t active_record_idx = 0;

static void app_multilink_customer_record_link_start(void);
static void app_multilink_customer_record_link_stop(uint8_t link_index);
static void app_multilink_customer_check_record_index_update(uint8_t index);

static uint8_t app_multilink_customer_find_array_index(uint8_t link_index)
{
    uint8_t i = 0;

    for (i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if (audio_link_mgr[i].idx == link_index)
        {
            break;
        }
    }
    return i;
}

static T_APP_MULTILINK_CUSTOMER_ARRAY_DATA
*app_multilink_customer_find_array_data(uint8_t link_index)
{
    for (uint8_t i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if (audio_link_mgr[i].idx == link_index)
        {
            return &audio_link_mgr[i];
        }
    }

    return NULL;
}

#if F_APP_GAMING_DONGLE_SUPPORT
bool app_multilink_customer_is_dongle_priority_higher(void)
{
    uint8_t dongle_media_priority = audio_link_mgr[multilink_dongle_idx].music_priority;
    uint8_t phone_media_priority = 0xFF;

    for (uint8_t i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if (i != multilink_dongle_idx)
        {
            // a smaller priority value means a higher priority
            if (audio_link_mgr[i].music_priority < phone_media_priority)
            {
                phone_media_priority = audio_link_mgr[i].music_priority;
            }
        }
    }

    // a smaller priority value means a higher priority, return true if dongle priority is higher
    return (dongle_media_priority < phone_media_priority);
}

void app_multilink_customer_set_dongle_priority(uint8_t link_index)
{
    for (uint8_t i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if (audio_link_mgr[i].idx == link_index)
        {
            // dongle has higher music priority than non-dongle BT links
            audio_link_mgr[i].music_priority = 1;
            multilink_dongle_idx = link_index;
        }
        else
        {
            audio_link_mgr[i].music_priority = 2;
        }

        APP_PRINT_TRACE2("app_multilink_customer_set_dongle_priority: audio_link_mgr[%d] = %d", i,
                         audio_link_mgr[i].music_priority);
    }

    app_multilink_customer_update_dongle_record_status(app_dongle_get_record_state());
}

void app_multilink_customer_update_dongle_record_status(bool record_status)
{
    if (multilink_dongle_idx != MULTILINK_SRC_CONNECTED &&
        audio_link_mgr[multilink_dongle_idx].record_status != record_status)
    {
        audio_link_mgr[multilink_dongle_idx].record_status = record_status;
        app_multilink_customer_check_record_index_update(multilink_dongle_idx);
    }
}

static bool app_multilink_customer_check_dongle_upstream_running(void)
{
    return app_dongle_get_record_state() && (app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING);
}

static bool app_multilink_customer_check_dongle_upstream_preemptible(uint8_t index)
{
    bool ret = false;
    uint8_t array_idx = app_multilink_customer_find_array_index(index);
    // dongle array_idx would be the same as multilink_dongle_idx

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return ret;
    }

    if (multilink_dongle_idx == MULTILINK_SRC_CONNECTED || // no dongle connected
        multilink_dongle_idx == index)
    {
        ret = true;
    }
    else if (app_multilink_customer_check_dongle_upstream_running())
    {
        if (audio_link_mgr[array_idx].record_priority <
            audio_link_mgr[multilink_dongle_idx].record_priority)
        {
            ret = true;
        }
    }
    else
    {
        ret = true;
    }

    return ret;
}
#endif

#if F_APP_LINEIN_SUPPORT
bool app_multilink_customer_line_in_start_check(void)
{
    bool ret = false;

    if (app_line_in_plug_state_get() && (active_music_idx == multilink_line_in_idx))
    {
        ret = true;
    }

    return ret;
}

void app_multilink_customer_line_in_start(void)
{
    app_ipc_publish(LINE_IN_IPC_TOPIC, LINE_IN_IPC_EVT_AUDIO_DS_START, NULL);
}

void app_multilink_customer_line_in_stop(void)
{
    app_ipc_publish(LINE_IN_IPC_TOPIC, LINE_IN_IPC_EVT_AUDIO_DS_STOP, NULL);
}

void app_multilink_customer_line_in_plug(void)
{
    app_ipc_publish(LINE_IN_IPC_TOPIC, LINE_IN_IPC_EVT_PLUG, NULL);
}

void app_multilink_customer_line_in_unplug(void)
{
    app_ipc_publish(LINE_IN_IPC_TOPIC, LINE_IN_IPC_EVT_UNPLUG, NULL);
}
#endif

static void app_multilink_customer_a2dp_pause(uint8_t link_index)
{
    APP_PRINT_TRACE5("app_multilink_customer_a2dp_pause: idx %x active idx %x music_event %x play_status %x",
                     link_index, app_a2dp_get_active_idx(), app_db.br_link[link_index].a2dp_track_handle,
                     audio_link_mgr[link_index].music_event, app_db.br_link[link_index].avrcp_play_status);

    if ((link_index == app_a2dp_get_active_idx()) && (app_db.br_link[link_index].a2dp_track_handle))
    {
        if ((audio_link_mgr[link_index].music_event == JUDGE_EVENT_MEDIAPLAYER_PLAYING) ||
            (audio_link_mgr[link_index].music_event == JUDGE_EVENT_A2DP_START))
        {
#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
            audio_link_mgr[link_index].wait_resume_a2dp = true;
#endif

            if (app_db.br_link[link_index].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                bool pause = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_link_check_dongle_link(app_db.br_link[link_index].bd_addr))
                {
                    pause = false;
                }
#endif

                if (pause)
                {
                    bt_avrcp_pause(app_db.br_link[link_index].bd_addr);
                    app_db.br_link[link_index].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                }
            }

            audio_track_stop(app_db.br_link[link_index].a2dp_track_handle);
        }
    }
}

#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
static void app_multilink_customer_a2dp_resume(uint8_t link_index)
{
    if ((link_index == app_a2dp_get_active_idx()) &&
        (audio_link_mgr[link_index].wait_resume_a2dp) &&
        ((audio_link_mgr[link_index].music_event == JUDGE_EVENT_MEDIAPLAYER_PAUSED) ||
         (audio_link_mgr[link_index].music_event == JUDGE_EVENT_A2DP_STOP)))
    {
        audio_link_mgr[link_index].wait_resume_a2dp = false;

        if (app_db.br_link[link_index].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
        {
            bool play = true;

#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_link_check_dongle_link(app_db.br_link[link_index].bd_addr))
            {
                play = false;
            }
#endif

            if (play)
            {
                bt_avrcp_play(app_db.br_link[link_index].bd_addr);
                app_db.br_link[link_index].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
            }
        }
    }
}
#endif

static bool app_multilink_customer_check_hfp_is_idle(void)
{
    bool ret = false;

    if (app_link_get_sco_conn_num() == 0)
    {
        if (app_hfp_get_call_status() == APP_CALL_IDLE)
        {
            ret = true;
        }
#if F_APP_GAMING_DONGLE_SUPPORT
        else if (app_multilink_customer_check_dongle_upstream_running())
        {
            // dongle upstream running not seen as hfp running
            ret = true;
        }
#endif
    }

    return ret;
}

static bool app_multilink_customer_check_record_running(uint8_t record_idx)
{
    uint8_t record_array_idx = app_multilink_customer_find_array_index(record_idx);

    if (record_array_idx != CUSTOMER_APP_MAX_LINK_NUM && audio_link_mgr[record_array_idx].record_status)
    {
        return true;
    }

    return false;
}

static bool app_multilink_customer_check_upstream_preemptible(uint8_t index)
{
    bool ret = false;
    uint8_t array_idx = app_multilink_customer_find_array_index(index);
    uint8_t record_array_idx = app_multilink_customer_find_array_index(active_record_idx);

    if (!app_multilink_customer_check_hfp_is_idle() ||
#if F_APP_GAMING_DONGLE_SUPPORT
        !app_multilink_customer_check_dongle_upstream_preemptible(index) ||
#endif
        array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return ret;
    }

    if (!app_multilink_customer_check_record_running(active_record_idx)) // no record running
    {
        ret = true;
    }
    else if (record_array_idx != CUSTOMER_APP_MAX_LINK_NUM && audio_link_mgr[array_idx].record_status &&
             (record_array_idx == array_idx ||
              (audio_link_mgr[array_idx].record_priority < audio_link_mgr[record_array_idx].record_priority)))
    {
        ret = true;
    }

    APP_PRINT_TRACE4("app_multilink_customer_check_upstream_preemptible: record status %x active_record_idx %x, check index %x ret %d",
                     audio_link_mgr[record_array_idx].record_status, active_record_idx, index, ret);
    return ret;
}

void app_multilink_customer_audio_link_start(void)
{
    uint8_t array_idx = app_multilink_customer_find_array_index(active_music_idx);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    if ((audio_link_mgr[array_idx].music_event == JUDGE_EVENT_A2DP_START) ||
        (audio_link_mgr[array_idx].music_event == JUDGE_EVENT_MEDIAPLAYER_PLAYING))
    {
        if (active_music_idx == multilink_usb_idx) // usb channel
        {
#if F_APP_USB_AUDIO_SUPPORT
            if (usb_down_stream_running)
            {
                app_usb_audio_start(USB_AUDIO_SCENARIO_PLAYBACK);
            }
#endif
        }
        else if (active_music_idx == multilink_line_in_idx) // line in channel
        {
#if F_APP_LINEIN_SUPPORT
            if (!app_line_in_playing_state_get())
            {
                app_line_in_start();
            }
#endif
        }
        else // BT channel
        {
#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
            if (audio_link_mgr[active_music_idx].wait_resume_a2dp)
            {
                audio_link_mgr[active_music_idx].wait_resume_a2dp = false;
            }
#endif

            app_multi_a2dp_active_link_set(app_db.br_link[active_music_idx].bd_addr);
        }
    }
}

static void app_multilink_customer_audio_link_stop(uint8_t link_index, bool check_legacy_idx)
{
    if (link_index == multilink_usb_idx)
    {
#if F_APP_USB_AUDIO_SUPPORT
        if (usb_down_stream_running)
        {
            app_usb_audio_stop(USB_AUDIO_SCENARIO_PLAYBACK);
        }
#endif
    }
    else if (link_index == multilink_line_in_idx)
    {
#if F_APP_LINEIN_SUPPORT
        if (app_line_in_plug_state_get() && app_line_in_playing_state_get())
        {
            app_line_in_stop();
        }
#endif
    }
    else if (check_legacy_idx)
    {
        app_multilink_customer_a2dp_pause(link_index);
    }
}

static void app_multilink_customer_audio_link_resume(uint8_t link_index, bool check_legacy_idx)
{
    APP_PRINT_TRACE5("app_multilink_customer_audio_link_resume: idx %x, check legacy %d, usb up %d down %d, active a2dp idx %x",
                     link_index, check_legacy_idx, usb_up_stream_running, usb_down_stream_running,
                     app_a2dp_get_active_idx());

    if (link_index == multilink_usb_idx)
    {
#if F_APP_USB_AUDIO_SUPPORT
        if (usb_down_stream_running)
        {
            app_multilink_customer_music_event_handle(multilink_usb_idx, JUDGE_EVENT_A2DP_START);
        }
#endif
    }
    else if (link_index == multilink_line_in_idx)
    {
#if F_APP_LINEIN_SUPPORT
        if (app_cfg_const.line_in_support && app_line_in_plug_state_get())
        {
            app_multilink_customer_music_event_handle(multilink_line_in_idx, JUDGE_EVENT_A2DP_START);
        }
#endif
    }
    else if (check_legacy_idx)
    {
        if ((link_index == app_a2dp_get_active_idx()) && (app_db.br_link[link_index].a2dp_track_handle))
        {
            app_multilink_customer_music_event_handle(link_index, JUDGE_EVENT_A2DP_START);
        }
    }
}

static void app_multilink_customer_set_active_music_index(uint8_t index)
{
    if (active_music_idx == index)
    {
        return;
    }

    active_music_idx = index;

    if ((active_music_idx != multilink_usb_idx)
        && (active_music_idx != multilink_line_in_idx))
    {
        app_a2dp_set_active_idx(index);
        app_bond_set_priority(app_db.br_link[active_music_idx].bd_addr);

#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
        if ((audio_link_mgr[active_music_idx].wait_resume_a2dp) &&
            (audio_link_mgr[active_music_idx].music_event == JUDGE_EVENT_MEDIAPLAYER_PAUSED))
        {
            audio_link_mgr[active_music_idx].wait_resume_a2dp = false;
            bt_avrcp_play(app_db.br_link[active_music_idx].bd_addr);
        }
#endif
    }
}

static void app_multilink_customer_update_device_priority_array(uint8_t array_idx,
                                                                T_APP_JUDGE_A2DP_EVENT event)
{
    if (event == JUDGE_EVENT_A2DP_START)
    {
        device_mgr[array_idx].app_actived = true;
    }
    else if (event == JUDGE_EVENT_A2DP_STOP)
    {
        device_mgr[array_idx].app_end_time = os_sys_time_get();
    }
}

static uint8_t app_multilink_customer_find_newest_active_idx(void)
{
    uint8_t index = CUSTOMER_APP_MAX_LINK_NUM;
    uint32_t active_time = 0;

    for (uint8_t i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if (device_mgr[i].app_actived)
        {
            if (device_mgr[i].app_end_time > active_time)
            {
                index = device_mgr[i].idx;
                active_time = device_mgr[i].app_end_time;
            }
        }
    }

    return index;
}

static uint8_t app_multilink_customer_find_newest_connected_idx(void)
{
    uint8_t index = CUSTOMER_APP_MAX_LINK_NUM;
    uint32_t connected_time = 0;

    for (uint8_t i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if (device_mgr[i].connect_time > connected_time)
        {
            index = device_mgr[i].idx;
            connected_time = device_mgr[i].connect_time;
        }
    }

    return index;
}

uint8_t app_multilink_customer_get_active_index(void)
{
    if (!app_multilink_customer_check_hfp_is_idle()
#if F_APP_GAMING_DONGLE_SUPPORT
        || app_multilink_customer_check_dongle_upstream_running()
#endif
       )

    {
        return app_hfp_get_active_idx();
    }
    else
    {
        uint8_t array_idx = app_multilink_customer_find_array_index(active_music_idx);

        if ((array_idx != CUSTOMER_APP_MAX_LINK_NUM) &&
            ((audio_link_mgr[array_idx].music_event == JUDGE_EVENT_A2DP_START) ||
             (audio_link_mgr[array_idx].music_event == JUDGE_EVENT_MEDIAPLAYER_PLAYING)))
        {
            return active_music_idx;
        }
        else if (app_multilink_customer_find_newest_active_idx() != CUSTOMER_APP_MAX_LINK_NUM)
        {
            return app_multilink_customer_find_newest_active_idx();
        }
        else
        {
            return app_multilink_customer_find_newest_connected_idx();
        }
    }
}

uint8_t app_multilink_customer_get_active_music_link_index(void)
{
    return active_music_idx;
}

void app_multilink_customer_handle_link_connected(uint8_t link_index)
{
    uint8_t array_idx = app_multilink_customer_find_array_index(link_index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    device_mgr[array_idx].connect_time = os_sys_time_get();
}

void app_multilink_customer_handle_link_disconnected(uint8_t link_index)
{
    uint8_t array_idx = app_multilink_customer_find_array_index(link_index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    memset(&device_mgr[array_idx], 0, sizeof(T_APP_MULTILINK_CUSTOMER_DEVICE_MGR));
    device_mgr[array_idx].idx = link_index;
}

static uint8_t app_multilink_customer_audio_link_priority_check(void)
{
    uint8_t i = 0;
    uint8_t link_index = active_music_idx;
    uint8_t priority = MAX_PRIORITY_NUM;
    bool has_link_started_music = false;
    bool has_link_wait_resumed_music = false;
    bool has_upstream = false;

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_multilink_customer_check_dongle_upstream_running())
    {
        has_upstream = true;
        link_index = multilink_dongle_idx;
        goto priority_check_done;
    }
    else
#endif
        if (app_multilink_customer_check_record_running(active_record_idx))
        {
            has_upstream = true;
            link_index = audio_link_mgr[app_multilink_customer_find_array_index(active_record_idx)].idx;
            goto priority_check_done;
        }

    // check started link
    for (i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if ((audio_link_mgr[i].music_event == JUDGE_EVENT_A2DP_START) ||
            (audio_link_mgr[i].music_event == JUDGE_EVENT_MEDIAPLAYER_PLAYING))
        {
            has_link_started_music = true;

            if (audio_link_mgr[i].music_priority < priority)
            {
                priority = audio_link_mgr[i].music_priority;
                link_index = audio_link_mgr[i].idx;
            }
        }
    }

#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
    // check wait resume A2DP link
    for (i = 0; i < MULTILINK_SRC_CONNECTED; i++)
    {
        if (audio_link_mgr[i].wait_resume_a2dp == true)
        {
            has_link_wait_resumed_music = true;

            if (audio_link_mgr[i].music_priority < priority)
            {
                priority = audio_link_mgr[i].music_priority;
                link_index = audio_link_mgr[i].idx;
            }
        }
    }
#endif

    // no link started music, check connected or paused link
    if (!has_link_started_music && !has_link_wait_resumed_music)
    {
        for (i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
        {
            if ((audio_link_mgr[i].music_event == JUDGE_EVENT_A2DP_CONNECTED) ||
                (audio_link_mgr[i].music_event == JUDGE_EVENT_MEDIAPLAYER_PAUSED))
            {
                if (audio_link_mgr[i].music_priority < priority)
                {
                    priority = audio_link_mgr[i].music_priority;
                    link_index = audio_link_mgr[i].idx;
                }
            }
        }
    }

priority_check_done:
    APP_PRINT_TRACE4("app_multilink_customer_audio_link_priority_check: upstream %x music %x resume %x link_index %x",
                     has_upstream, has_link_started_music, has_link_wait_resumed_music, link_index);

    return link_index;
}

void app_multilink_customer_music_event_handle(uint8_t index, T_APP_JUDGE_A2DP_EVENT event)
{
    uint8_t new_active_music_idx = 0;
    uint8_t array_idx = app_multilink_customer_find_array_index(index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    // update app status to device priority array
    app_multilink_customer_update_device_priority_array(array_idx, event);

    if ((audio_link_mgr[array_idx].music_event != event) && (event != JUDGE_EVENT_SNIFFING_STOP))
    {
        APP_PRINT_TRACE4("app_multilink_customer_music_event_handle: link_index %d, A2DP_event(%d -> %d), active_music_idx %d",
                         index, audio_link_mgr[array_idx].music_event, event, active_music_idx);
        audio_link_mgr[array_idx].music_event = event;
    }

    if (!app_multilink_customer_check_upstream_preemptible(index))
    {
        return;
    }

    new_active_music_idx = app_multilink_customer_audio_link_priority_check();

    if (active_music_idx != new_active_music_idx)
    {
        APP_PRINT_TRACE2("app_multilink_customer_music_event_handle: active_music_idx %d -> %d",
                         active_music_idx, new_active_music_idx);

        app_multilink_customer_audio_link_stop(active_music_idx, true);

        app_multilink_customer_set_active_music_index(new_active_music_idx);
    }

    app_multilink_customer_audio_link_start();

    // update bt qos
    if ((active_music_idx != multilink_usb_idx)
        && (active_music_idx != multilink_line_in_idx))
    {
        if (event == JUDGE_EVENT_A2DP_START || event == JUDGE_EVENT_MEDIAPLAYER_PLAYING)
        {
            app_bt_policy_qos_param_update(app_db.br_link[index].bd_addr, BP_TPOLL_A2DP_PLAY_EVENT);
        }
        else if (event == JUDGE_EVENT_A2DP_DISC || event == JUDGE_EVENT_A2DP_STOP)
        {
            app_bt_policy_qos_param_update(app_db.br_link[index].bd_addr, BP_TPOLL_A2DP_STOP_EVENT);
        }

        app_multi_update_a2dp_play_status(event);
    }
}

void app_multilink_customer_hfp_status_changed(uint8_t prev_call_status, uint8_t *bd_addr)
{
#if F_APP_GAMING_DONGLE_SUPPORT
    APP_PRINT_TRACE4("app_multilink_customer_hfp_status_changed: hfp_status: %x -> %x (dongle upstream %d), active_music_idx %d",
                     prev_call_status, app_hfp_get_call_status(), app_multilink_customer_check_dongle_upstream_running(),
                     active_music_idx);
#else
    APP_PRINT_TRACE3("app_multilink_customer_hfp_status_changed: hfp_status: %x -> %x, active_music_idx %d",
                     prev_call_status, app_hfp_get_call_status(), active_music_idx);
#endif

    if (app_hfp_get_call_status() == APP_CALL_IDLE)
    {
        app_multilink_customer_check_record_index_update(active_record_idx);

        if (!app_multilink_customer_check_record_running(active_record_idx)) // downstream only
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_link_check_dongle_link(bd_addr) &&
                prev_call_status == APP_VOICE_ACTIVATION_ONGOING) // dongle call end
            {
                uint8_t array_idx = app_multilink_customer_find_array_index(active_music_idx);

                if (array_idx != CUSTOMER_APP_MAX_LINK_NUM)
                {
                    // check active music index
                    app_multilink_customer_music_event_handle(active_music_idx, audio_link_mgr[array_idx].music_event);
                }
            }
            else
#endif
            {
                app_multilink_customer_audio_link_resume(active_music_idx, false);
            }
        }
    }
#if F_APP_GAMING_DONGLE_SUPPORT
    else if (app_multilink_customer_check_dongle_upstream_running())
    {
        app_multilink_customer_record_link_stop(active_record_idx);

        if (active_music_idx != multilink_dongle_idx)
        {
            // change active music index to dongle
            app_multilink_customer_music_event_handle(multilink_dongle_idx,
                                                      audio_link_mgr[multilink_dongle_idx].music_event);
        }
        else
        {
            app_multilink_customer_audio_link_resume(active_music_idx, false);
        }
    }
#endif
    else
    {
        app_multilink_customer_record_link_stop(active_record_idx);
        app_multilink_customer_audio_link_stop(active_music_idx, false);
    }
}

void app_multilink_customer_sco_disconneted(void)
{
    app_multilink_customer_audio_link_resume(active_music_idx, true);
}

static void app_multilink_coustomer_record_track_start(uint8_t index)
{
    APP_PRINT_TRACE1("app_multilink_coustomer_record_track_start: index %d", index);

    if (index == multilink_usb_idx) // usb channel
    {
#if F_APP_USB_AUDIO_SUPPORT
        app_usb_audio_start(USB_AUDIO_SCENARIO_CAPTURE);
#endif
    }
    else // BT channel
    {
        T_APP_BR_LINK *p_link;
        T_AUDIO_TRACK_STATE audio_track_state;

        p_link = app_link_find_br_link(app_db.br_link[index].bd_addr);

        if (p_link)
        {
            if (app_link_check_dongle_link(p_link->bd_addr))
            {
                // dongle will start record track in app_dongle_start_recording()
            }
            else
            {
                audio_track_state_get(p_link->sco_track_handle, &audio_track_state);

                if (audio_track_state != AUDIO_TRACK_STATE_STARTED)
                {
                    audio_track_start(p_link->sco_track_handle);
                }
            }
        }
    }
}

static uint8_t app_multilink_customer_record_link_priority_check(void)
{
    uint8_t i = 0;
    uint8_t link_index = active_record_idx;
    uint8_t priority = MAX_PRIORITY_NUM;

    // check started link
    for (i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        if (audio_link_mgr[i].record_status)
        {
            if (audio_link_mgr[i].record_priority < priority)
            {
                priority = audio_link_mgr[i].record_priority;
                link_index = audio_link_mgr[i].idx;
            }
        }
    }

    return link_index;
}

static void app_multilink_customer_record_link_start(void)
{
    if (app_multilink_customer_check_record_running(active_record_idx))
    {
        if (active_music_idx != active_record_idx)
        {
            // stop currently active downstream
            app_multilink_customer_audio_link_stop(active_music_idx, true);
        }

        app_multilink_coustomer_record_track_start(active_record_idx);

        // check downstream of active_record_idx
        app_multilink_customer_audio_link_resume(active_record_idx, true);
    }
}

static void app_multilink_customer_record_link_stop(uint8_t link_index)
{
    if (link_index == multilink_usb_idx)
    {
#if F_APP_USB_AUDIO_SUPPORT
        if (usb_up_stream_running)
        {
            app_usb_audio_stop(USB_AUDIO_SCENARIO_CAPTURE);
        }
#endif
    }

    // dongle record link would be stopped by app_dongle_stop_recording().
}

static void app_multilink_customer_set_active_record_index(uint8_t index)
{
    uint8_t array_idx;

    if (active_record_idx == index)
    {
        return;
    }

    active_record_idx = index;

    array_idx = app_multilink_customer_find_array_index(active_record_idx);

    if (array_idx != CUSTOMER_APP_MAX_LINK_NUM && active_record_idx != multilink_usb_idx)
    {
        if ((audio_link_mgr[array_idx].music_event == JUDGE_EVENT_A2DP_START) ||
            (audio_link_mgr[array_idx].music_event == JUDGE_EVENT_MEDIAPLAYER_PLAYING))
        {
            // upstream + downstream scenario
            app_hfp_only_set_active_idx(active_record_idx);
        }

        app_bond_set_priority(app_db.br_link[active_record_idx].bd_addr);
    }
}

static void app_multilink_customer_check_record_index_update(uint8_t index)
{
    uint8_t new_active_record_idx;
    uint8_t array_idx = app_multilink_customer_find_array_index(index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    APP_PRINT_TRACE2("app_multilink_customer_check_record_index_update: idx %x, record_status: %d",
                     index, audio_link_mgr[array_idx].record_status);

    new_active_record_idx = app_multilink_customer_record_link_priority_check();

    if (active_record_idx != new_active_record_idx)
    {
        APP_PRINT_TRACE2("app_multilink_customer_check_record_index_update: active_record_idx %d -> %d",
                         active_record_idx, new_active_record_idx);

        app_multilink_customer_set_active_record_index(new_active_record_idx);
    }

    array_idx = app_multilink_customer_find_array_index(active_record_idx);

    if (array_idx != CUSTOMER_APP_MAX_LINK_NUM && audio_link_mgr[array_idx].record_status)
    {
        if (active_record_idx == index)
        {
            app_multilink_customer_record_link_start();
        }
    }
    else
    {
        // upstream end, check active music index
        app_multilink_customer_music_event_handle(index, audio_link_mgr[array_idx].music_event);
#if (F_APP_GAMING_DONGLE_SUPPORT == 0)
        app_multilink_customer_a2dp_resume(active_music_idx);
#endif
    }
}

#if F_APP_USB_AUDIO_SUPPORT
void app_multilink_customer_usb_upstream_event_handle(bool record_status)
{
    uint8_t new_active_record_idx;

    APP_PRINT_TRACE2("app_multilink_customer_usb_upstream_event_handle: record_status: %d -> %d",
                     audio_link_mgr[USB_AUDIO_LINK_INDEX].record_status, record_status);

    audio_link_mgr[USB_AUDIO_LINK_INDEX].record_status = record_status;

    if (!app_multilink_customer_check_upstream_preemptible(multilink_usb_idx) && record_status)
    {
        return;
    }

    app_multilink_customer_check_record_index_update(multilink_usb_idx);
}

static void app_multilink_customer_update_usb_stream_status(T_APP_MULTILINK_CUSTOMER_STREAM_EVENT
                                                            event)
{
    if (event == APP_MULTILINK_CUSTOMER_USB_UPSTREAM_START)
    {
        usb_stream_status |= APP_MULTILINK_USB_UPSTREAM_ACTIVE;
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_UPSTREAM_STOP)
    {
        usb_stream_status &= ~APP_MULTILINK_USB_UPSTREAM_ACTIVE;
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_START)
    {
        usb_stream_status |= APP_MULTILINK_USB_DOWNSTREAM_ACTIVE;
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_STOP)
    {
        usb_stream_status &= ~APP_MULTILINK_USB_DOWNSTREAM_ACTIVE;
    }
}

static void app_multilink_customer_handle_usb_event(T_APP_MULTILINK_CUSTOMER_STREAM_EVENT event)
{
    app_multilink_customer_update_usb_stream_status(event);

    APP_PRINT_TRACE2("app_multilink_customer_handle_usb_event: stream_event: %d, usb_stream_status: %d",
                     event, usb_stream_status);

    if (event == APP_MULTILINK_CUSTOMER_USB_UPSTREAM_START)
    {
        usb_up_stream_running = true;
        app_multilink_customer_usb_upstream_event_handle(true);
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_UPSTREAM_STOP)
    {
        app_multilink_customer_usb_upstream_event_handle(false);
        usb_up_stream_running = false;
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_START)
    {
        usb_down_stream_running = true;
        app_multilink_customer_music_event_handle(multilink_usb_idx, JUDGE_EVENT_A2DP_START);
    }
    else if (event == APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_STOP)
    {
        T_APP_MULTILINK_CUSTOMER_ARRAY_DATA *p_music_priority_data = NULL;

        p_music_priority_data = app_multilink_customer_find_array_data(multilink_usb_idx);

        if (p_music_priority_data && (p_music_priority_data->music_event == JUDGE_EVENT_A2DP_START))
        {
            app_multilink_customer_music_event_handle(multilink_usb_idx, JUDGE_EVENT_A2DP_STOP);
        }

        usb_down_stream_running = false;
    }
}

static void app_multilink_customer_usb_ipc_cback(uint32_t id, void *msg)
{
    switch (id)
    {
    case USB_IPC_EVT_PLUG:
        {
            app_multilink_customer_music_event_handle(multilink_usb_idx, JUDGE_EVENT_A2DP_CONNECTED);
            app_multilink_customer_handle_link_connected(multilink_usb_idx);
        }
        break;

    case USB_IPC_EVT_UNPLUG:
        {
            app_multilink_customer_music_event_handle(multilink_usb_idx, JUDGE_EVENT_A2DP_DISC);
            app_multilink_customer_handle_link_disconnected(multilink_usb_idx);
            usb_down_stream_running = false;
            usb_up_stream_running = false;
        }
        break;

    case USB_IPC_EVT_AUDIO_DS_START:
        {
            if (!usb_down_stream_running)
            {
                app_multilink_customer_handle_usb_event(APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_START);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_DS_STOP:
        {
            if (usb_down_stream_running)
            {
                app_multilink_customer_handle_usb_event(APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_STOP);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_US_START:
        {
            if (!usb_up_stream_running)
            {
                app_multilink_customer_handle_usb_event(APP_MULTILINK_CUSTOMER_USB_UPSTREAM_START);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_US_STOP:
        {
            if (usb_up_stream_running)
            {
                app_multilink_customer_handle_usb_event(APP_MULTILINK_CUSTOMER_USB_UPSTREAM_STOP);
            }
        }
        break;

    default:
        break;
    }
}
#endif

#if F_APP_LINEIN_SUPPORT
static void app_multilink_customer_handle_line_in_event(T_APP_MULTILINK_CUSTOMER_STREAM_EVENT event)
{
    APP_PRINT_TRACE1("app_multilink_customer_handle_line_in_event: stream_event: %d", event);

    if (event == APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_START)
    {
        if (app_multilink_customer_check_upstream_preemptible(multilink_line_in_idx))
        {
            app_multilink_customer_music_event_handle(multilink_line_in_idx, JUDGE_EVENT_A2DP_START);
        }
    }
    else if (event == APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_STOP)
    {
        T_APP_MULTILINK_CUSTOMER_ARRAY_DATA *p_music_priority_data = NULL;

        p_music_priority_data = app_multilink_customer_find_array_data(multilink_line_in_idx);

        if (p_music_priority_data && (p_music_priority_data->music_event == JUDGE_EVENT_A2DP_START))
        {
            app_line_in_stop();
            app_multilink_customer_music_event_handle(multilink_line_in_idx, JUDGE_EVENT_A2DP_STOP);
        }
    }
}

static void app_multilink_customer_line_in_ipc_cback(uint32_t id, void *msg)
{
    switch (id)
    {
    case LINE_IN_IPC_EVT_PLUG:
        {
            app_multilink_customer_music_event_handle(multilink_line_in_idx, JUDGE_EVENT_A2DP_CONNECTED);
            app_multilink_customer_handle_link_connected(multilink_line_in_idx);
            app_multilink_customer_handle_line_in_event(APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_START);
        }
        break;

    case LINE_IN_IPC_EVT_UNPLUG:
        {
            app_multilink_customer_music_event_handle(multilink_line_in_idx, JUDGE_EVENT_A2DP_DISC);
            app_multilink_customer_handle_link_disconnected(multilink_line_in_idx);
        }
        break;

    case LINE_IN_IPC_EVT_AUDIO_DS_START:
        {
            app_multilink_customer_handle_line_in_event(APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_START);
        }
        break;

    case LINE_IN_IPC_EVT_AUDIO_DS_STOP:
        {
            app_multilink_customer_handle_line_in_event(APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_STOP);
        }
        break;
    }
}
#endif

void app_multilink_customer_music_priority_set(uint8_t index, uint8_t priority)
{
    uint8_t array_idx = app_multilink_customer_find_array_index(index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    audio_link_mgr[array_idx].music_priority = priority;
}

void app_multilink_customer_record_priority_set(uint8_t index, uint8_t priority)
{
    uint8_t array_idx = app_multilink_customer_find_array_index(index);

    if (array_idx == CUSTOMER_APP_MAX_LINK_NUM)
    {
        return;
    }

    audio_link_mgr[array_idx].record_priority = priority;
}

void app_multilink_customer_init(void)
{
    for (uint8_t i = 0; i < CUSTOMER_APP_MAX_LINK_NUM; i++)
    {
        audio_link_mgr[i].idx = i;
        audio_link_mgr[i].music_priority = 1;
        audio_link_mgr[i].music_event = JUDGE_EVENT_A2DP_DISC;
        device_mgr[i].idx = i;

        audio_link_mgr[i].record_priority = 0;
        audio_link_mgr[i].record_status = false;
    }

#if F_APP_LINEIN_SUPPORT
    audio_link_mgr[LINE_IN_AUDIO_LINK_INDEX].idx = multilink_line_in_idx;
    audio_link_mgr[LINE_IN_AUDIO_LINK_INDEX].music_priority = 0;
    device_mgr[LINE_IN_AUDIO_LINK_INDEX].idx = multilink_line_in_idx;

    // no upstream scenario for line-in
    audio_link_mgr[LINE_IN_AUDIO_LINK_INDEX].record_priority = MAX_PRIORITY_NUM;

    app_ipc_subscribe(LINE_IN_IPC_TOPIC, app_multilink_customer_line_in_ipc_cback);
#endif

#if F_APP_USB_AUDIO_SUPPORT
    audio_link_mgr[USB_AUDIO_LINK_INDEX].idx = multilink_usb_idx;
    audio_link_mgr[USB_AUDIO_LINK_INDEX].music_priority = MULTILINK_SRC_CONNECTED + 1;
    device_mgr[USB_AUDIO_LINK_INDEX].idx = multilink_usb_idx;

    audio_link_mgr[USB_AUDIO_LINK_INDEX].record_priority = MULTILINK_SRC_CONNECTED + 1;

    app_ipc_subscribe(USB_IPC_TOPIC, app_multilink_customer_usb_ipc_cback);
#endif
}
#endif
