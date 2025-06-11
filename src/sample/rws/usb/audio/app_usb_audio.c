/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_USB_AUDIO_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "os_sync.h"
#include "trace.h"
#include "audio_track.h"
#include "audio_type.h"
#include "audio.h"
#include "usb_audio.h"
#include "usb_audio_stream.h"
#include "usb_msg.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "app_ota.h"
#include "app_usb_audio.h"
#include "app_eq.h"
#include "app_main.h"
#include "app_audio_policy.h"
#include "low_stack.h"
#include "pm.h"

#if F_APP_SIDETONE_SUPPORT
#include "app_sidetone.h"
#endif

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_ota.h"
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
#include "app_slide_switch.h"
#endif

typedef enum
{
    USB_AUDIO_UAL_STATE_IDLE,
    USB_AUDIO_UAL_STATE_INITED,
    USB_AUDIO_UAL_STATE_STARTING,
    USB_AUDIO_UAL_STATE_STARTED,
    USB_AUDIO_UAL_STATE_STOPPING,
    USB_AUDIO_UAL_STATE_STOPED,
    USB_AUDIO_UAL_STATE_RUNNING,
} T_USB_AUDIO_UAL_STATE;

typedef struct _spk_flow_ctrl
{
    bool fresh_start;
    bool permission;
    uint16_t pkt_size;
    void *pkt_to_send;
    uint16_t upper_thr_ms;
    uint16_t pkt_num_once_ut;
    uint16_t lower_thr_ms;
    uint16_t pkt_num_once_lt;
} T_SPK_FLOW_CTL;

typedef struct _usb_audio
{
    uint32_t id;
    T_USB_AUDIO_UAL_STATE state;
    void *handle;
    void *misc;
    void *eq_instance;
    T_AUDIO_EFFECT_INSTANCE sidetone_instance;
} T_USB_AUDIO;

typedef struct app_usb_audio_db
{
    T_USB_AUDIO *playback;
    T_USB_AUDIO *capture;
} T_APP_USB_AUDIO_DB;

T_APP_USB_AUDIO_DB app_usb_audio_db;

const uint32_t sample_freq[8] = {8000, 16000, 32000, 44100, 48000, 88000, 96000, 192000};

#define UAVOL2DSPVOL(vol, range) (0x10*(vol)/range)
#define BUF_UPPER_THR_MS(sr, bw, ch_num) ((PLAYBACK_POOL_SIZE)*1000* 8 /sr/bw/ch_num * 80/100) /*80 percent of playback pool total size*/
#define BUF_LOWER_THR_MS(sr, bw, ch_num)  ((PLAYBACK_POOL_SIZE)*1000* 8/sr/bw/ch_num * 10/100) /*10 percent of playback pool total size*/
#define PKT_NUM_IN_UT(pkt_size) (0)
#define PKT_NUM_IN_LT(pkt_size) ((PLAYBACK_POOL_SIZE)*60/100/pkt_size)

static BtPowerMode bt_pwr_state = BTPOWER_ACTIVE;

static bool usb_up_stream_running = 0;
static bool usb_down_stream_running = 0;
//--------------------------------------------------------------------------//
static void app_usb_audio_ipc_cback(uint32_t id, void *msg)
{
    switch (id)
    {
    case USB_IPC_EVT_PLUG:
        {

        }
        break;

    case USB_IPC_EVT_UNPLUG:
        {
            if (usb_down_stream_running)
            {
                usb_down_stream_running = false;
                app_usb_audio_stop(USB_AUDIO_SCENARIO_PLAYBACK);
            }

            if (usb_up_stream_running)
            {
                usb_up_stream_running = false;
                app_usb_audio_stop(USB_AUDIO_SCENARIO_CAPTURE);
            }

            // TODO: check resume
        }
        break;

    case USB_IPC_EVT_AUDIO_DS_START:
        {
            if (!usb_down_stream_running)
            {
                // TODO: check start
                usb_down_stream_running = true;
                app_usb_audio_start(USB_AUDIO_SCENARIO_PLAYBACK);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_DS_STOP:
        {
            if (usb_down_stream_running)
            {
                usb_down_stream_running = false;
                app_usb_audio_stop(USB_AUDIO_SCENARIO_PLAYBACK);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_US_START:
        {
            if (!usb_up_stream_running)
            {
                // TODO: check start
                usb_up_stream_running = true;
                app_usb_audio_start(USB_AUDIO_SCENARIO_CAPTURE);
            }
        }
        break;

    case USB_IPC_EVT_AUDIO_US_STOP:
        {
            if (usb_up_stream_running)
            {
                usb_up_stream_running = false;
                app_usb_audio_stop(USB_AUDIO_SCENARIO_CAPTURE);
            }
        }
        break;

    default:
        break;
    }
}

T_AUDIO_TRACK_STATE app_usb_audio_get_ds_track_state(void)
{
    T_AUDIO_TRACK_STATE track_state = AUDIO_TRACK_STATE_RELEASED;

    audio_track_state_get(app_usb_audio_db.playback->handle, &track_state);
    return track_state;
}

static void app_usb_audio_ual_spk_vol_ctrl(void *handle, T_UAS_VOL *vol)
{
    uint8_t vol_level = UAVOL2DSPVOL(vol->cur, vol->range);

    if (vol_level > 0x0F)
    {
        vol_level = 0x0F;
    }
    audio_track_volume_out_set(handle, vol_level);
}

static void app_usb_audio_ual_spk_mute_ctrl(void *handle, bool mute)
{
    if (mute)
    {
        audio_track_volume_out_mute(handle);
    }
    else
    {
        audio_track_volume_out_unmute(handle);
    }
}

static void app_usb_audio_ual_state_set(T_USB_AUDIO *audio, T_USB_AUDIO_UAL_STATE state)
{
    if (state != audio->state)
    {
        APP_PRINT_INFO2("app_usb_audio_ual_state_set:%d-%d", audio->state, state);
        audio->state = state;
    }
}

static T_USB_AUDIO_UAL_STATE app_usb_audio_ual_state_get(T_USB_AUDIO *audio)
{
    APP_PRINT_INFO1("app_usb_audio_ual_state_get:%d", audio->state);
    return (audio->state);
}

static void *app_usb_audio_ual_spk_create(T_STREAM_ATTR attr, void *ctrl)
{
    T_AUDIO_STREAM_TYPE  type = AUDIO_STREAM_TYPE_PLAYBACK;
    T_AUDIO_STREAM_USAGE usage = AUDIO_STREAM_USAGE_LOCAL;
    T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_DIRECT;
    uint32_t device = AUDIO_DEVICE_OUT_SPK;
    T_AUDIO_FORMAT_INFO format = {.type = AUDIO_FORMAT_TYPE_PCM, .frame_num = 1};
    T_USB_AUDIO *audio =  app_usb_audio_db.playback;
    uint8_t bit_width = attr.bit_width;
    void *track = NULL;
    T_UAS_CTRL *spk_ctrl = (T_UAS_CTRL *)ctrl;
    T_SPK_FLOW_CTL *flow_ctrl = NULL;

    if (!audio)
    {
        return NULL;
    }

    flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;

    memcpy(&(format.attr.pcm), &attr, sizeof(T_STREAM_ATTR));

    if (bit_width == 16)
    {
        format.attr.pcm.frame_length = 512 / format.attr.pcm.chann_num;
        flow_ctrl->pkt_size = 512;
    }
    else
    {
        format.attr.pcm.frame_length = 768 / format.attr.pcm.chann_num;
        flow_ctrl->pkt_size = 768;
    }

    audio->eq_instance = app_eq_create(EQ_CONTENT_TYPE_AUDIO, EQ_STREAM_TYPE_AUDIO, SPK_SW_EQ,
                                       app_db.spk_eq_mode,
                                       app_cfg_nv.eq_idx);

    track = audio_track_create(type,
                               mode,
                               usage,
                               format,
                               5,
                               0,
                               device,
                               NULL,
                               (P_AUDIO_TRACK_ASYNC_IO)NULL);

    if (track != NULL)
    {
        audio_track_latency_set(track, 20, true);
        app_usb_audio_ual_spk_vol_ctrl(track, &(spk_ctrl->vol));
        app_usb_audio_ual_spk_mute_ctrl(track, spk_ctrl->mute);
        app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_INITED);

        if (audio->eq_instance)
        {
            eq_enable(audio->eq_instance);
            audio_track_effect_attach(track, audio->eq_instance);
        }

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#else
        app_usb_audio_start(USB_AUDIO_SCENARIO_PLAYBACK);
#endif
    }
    audio->handle = track;
    flow_ctrl->upper_thr_ms = BUF_UPPER_THR_MS(format.attr.pcm.sample_rate, format.attr.pcm.bit_width,
                                               format.attr.pcm.chann_num);
    flow_ctrl->pkt_num_once_ut = PKT_NUM_IN_UT(flow_ctrl->pkt_size);
    flow_ctrl->lower_thr_ms = BUF_LOWER_THR_MS(format.attr.pcm.sample_rate, format.attr.pcm.bit_width,
                                               format.attr.pcm.chann_num);
    flow_ctrl->pkt_num_once_lt = PKT_NUM_IN_LT(flow_ctrl->pkt_size);
    flow_ctrl->pkt_to_send = malloc(flow_ctrl->pkt_size);

    return track;
}

static bool app_usb_audio_ual_spk_release(void *handle)
{
    bool ret = true;
    T_USB_AUDIO *audio = app_usb_audio_db.playback;
    T_SPK_FLOW_CTL *flow_ctrl = NULL;

    if (!audio)
    {
        return false;
    }

    flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;

    if (handle)
    {
        ret = audio_track_release(handle);

        if (flow_ctrl->pkt_to_send)
        {
            free(flow_ctrl->pkt_to_send);
            flow_ctrl->pkt_to_send = NULL;
        }
        audio->handle = NULL;
        app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_IDLE);
    }

    if (audio->eq_instance)
    {
        eq_release(audio->eq_instance);
        audio->eq_instance = NULL;
    }

    return ret;
}

static bool app_usb_audio_ual_spk_start(void *handle)
{
    if (handle)
    {
        return audio_track_start(handle);
    }
    APP_PRINT_ERROR0("app_usb_audio_ual_spk_start failed, handle NULL");
    return false;
}

static bool app_usb_audio_ual_spk_stop(void *handle)
{
    if (handle)
    {
        return audio_track_stop(handle);
    }
    APP_PRINT_ERROR0("app_usb_audio_ual_spk_stop failed, handle NULL");
    return false;
}

static uint8_t app_usb_audio_pkt_num_to_send(uint32_t id, bool *dummy)
{
    uint8_t ret = 0;
    uint16_t track_buf_level_ms = 0;
    T_USB_AUDIO *audio = app_usb_audio_db.playback;
    uint32_t pkt_total = 0;
    T_SPK_FLOW_CTL *flow_ctrl = NULL;

    if (!audio)
    {
        return false;
    }

    flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;
    if (!flow_ctrl)
    {
        return false;
    }

    pkt_total = usb_audio_stream_get_data_len(id) / (flow_ctrl->pkt_size);

    if (pkt_total > 0)
    {
        ret = pkt_total;
        *dummy = false;
    }

    APP_PRINT_INFO4("app_usb_audio_pkt_num_to_send:0x%x-0x%x-0x%x-0x%x", pkt_total, id,
                    flow_ctrl->pkt_size,
                    ret);

    return ret;
}

static void app_usb_audio_spk_multi_pkt_xmit(T_USB_AUDIO *audio, uint8_t pkt_num, bool dummy)
{
    T_SPK_FLOW_CTL *flow_ctrl = NULL;
    static uint16_t s_seq_num = 0;
    uint32_t s = 0;
    uint32_t bb_clock_slot;
    uint16_t bb_clock_us;

    if (!audio)
    {
        return;
    }

    flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;
    if (!flow_ctrl)
    {
        return;
    }
    flow_ctrl->permission = true;

    for (uint8_t i = 0; i < pkt_num; i++)
    {
        if (flow_ctrl->permission == false)
        {
            break;
        }

        uint16_t written_len;
        if (!dummy)
        {
            s = os_lock();
            usb_audio_stream_data_peek(audio->id, flow_ctrl->pkt_to_send, flow_ctrl->pkt_size);
        }
        else
        {
            memset(flow_ctrl->pkt_to_send, 0, flow_ctrl->pkt_size);
        }

        lc_get_high_dpi_native_clock(&bb_clock_slot, &bb_clock_us);
        if (audio_track_write(audio->handle,
                              bb_clock_slot,//              timestamp,
                              s_seq_num,
                              AUDIO_STREAM_STATUS_CORRECT,
                              1,//            frame_num,
                              (uint8_t *)flow_ctrl->pkt_to_send,
                              flow_ctrl->pkt_size,
                              &written_len) == false)
        {
            if (!dummy)
            {
                os_unlock(s);
                APP_PRINT_ERROR0("audio_track_write fail!!!");
            }
        }
        else
        {
            if (!dummy)
            {
                usb_audio_stream_data_flush(audio->id, flow_ctrl->pkt_size);
                os_unlock(s);
            }
        }
        s_seq_num++;
    }
}

static bool app_usb_audio_ual_spk_xmit(void *handle)
{
    bool dummy = false;
    uint8_t pkt_num = 0;
    T_USB_AUDIO *audio = app_usb_audio_db.playback;

#if F_APP_OTA_SUPPORT
    if (app_ota_dfu_is_busy()
#if F_APP_TUYA_SUPPORT
        || tuya_ota_is_busy()
#endif
       )
    {
        //If ota is going on, usb data won't be written to audio track.
        return false;
    }
#endif

    pkt_num = app_usb_audio_pkt_num_to_send(audio->id, &dummy);

    app_usb_audio_spk_multi_pkt_xmit(audio, pkt_num, dummy);

    app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_RUNNING);

    return true;
}

static bool app_usb_audio_ual_spk_ctrl(void *handle, uint8_t cmd, void *param)
{
    switch (cmd)
    {
    case CTRL_CMD_VOL_CHG:
        {
            T_UAS_VOL *vol = (T_UAS_VOL *)param;

            app_usb_audio_ual_spk_vol_ctrl(handle, vol);
        }
        break;

    case CTRL_CMD_MUTE:
        {
            uint32_t mute = *((uint32_t *)param);

            app_usb_audio_ual_spk_mute_ctrl(handle, (bool)mute);
        }
        break;

    case CTRL_CMD_STOP:
        {
            app_usb_audio_stop(USB_AUDIO_SCENARIO_PLAYBACK);
        }
        break;

    default:
        break;
    }
    APP_PRINT_INFO3("usb_audio_ual_spk_ctrl, handle:0x%x, cmd:0x%x, ctrl:0x%x", handle, cmd, param);
    return true;
}

static bool app_usb_audio_ual_mic_read(T_AUDIO_TRACK_HANDLE   handle,
                                       uint32_t              *timestamp,
                                       uint16_t              *seq_num,
                                       T_AUDIO_STREAM_STATUS *status,
                                       uint8_t               *frame_num,
                                       void                  *buf,
                                       uint16_t               required_len,
                                       uint16_t              *actual_len)
{
    T_USB_AUDIO *audio = app_usb_audio_db.capture;
    T_USB_AUDIO_UAL_STATE state = app_usb_audio_ual_state_get(audio);
    uint32_t s = 0;

    if (state != USB_AUDIO_UAL_STATE_STARTED && state != USB_AUDIO_UAL_STATE_RUNNING)
    {
        return false;
    }

    app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_RUNNING);
    *actual_len = 0;
    if (buf != NULL)
    {
        s = os_lock();
        if (usb_audio_stream_get_remaining_pool_size(audio->id) >= required_len)
        {
            *actual_len = usb_audio_stream_data_write(audio->id, buf, required_len);
        }
        else
        {
            APP_PRINT_ERROR1("app_usb_audio_ual_mic_read, us ring buf full:0x%x", required_len);
            usb_audio_stream_data_flush(audio->id, required_len);
            *actual_len = usb_audio_stream_data_write(audio->id, buf, required_len);
        }
        os_unlock(s);
    }

    return true;
}

static void app_usb_audio_ual_mic_vol_ctrl(void *handle, T_UAS_VOL *vol)
{
    uint8_t vol_level = UAVOL2DSPVOL(vol->cur, vol->range);

    if (vol_level > 0x0F)
    {
        vol_level = 0x0F;
    }
    audio_track_volume_in_set(handle, vol_level);
}

static void app_usb_audio_ual_mic_mute_ctrl(void *handle, bool mute)
{
    if (mute)
    {
        audio_track_volume_in_mute(handle);
    }
    else
    {
        audio_track_volume_in_unmute(handle);
    }
}

static void *app_usb_audio_ual_mic_create(T_STREAM_ATTR attr, void *ctrl)
{
    T_AUDIO_STREAM_TYPE  type = AUDIO_STREAM_TYPE_RECORD;
    T_AUDIO_STREAM_USAGE usage = AUDIO_STREAM_USAGE_LOCAL;
    T_AUDIO_STREAM_MODE mode = AUDIO_STREAM_MODE_NORMAL;
    uint32_t device = AUDIO_DEVICE_IN_MIC;
    T_AUDIO_FORMAT_INFO format = {.type = AUDIO_FORMAT_TYPE_PCM, .frame_num = 1};
    T_USB_AUDIO *audio =  app_usb_audio_db.capture;
    uint8_t bit_width = attr.bit_width;
    void *track = NULL;
    T_UAS_CTRL *mic_ctrl = (T_UAS_CTRL *)ctrl;

    memcpy(&(format.attr.pcm), &attr, sizeof(T_STREAM_ATTR));

    if (bit_width == 16)
    {
        format.attr.pcm.frame_length = 512 / format.attr.pcm.chann_num;
    }
    else
    {
        format.attr.pcm.frame_length = 768 / format.attr.pcm.chann_num;
    }

    track = audio_track_create(type,
                               mode,
                               usage,
                               format,
                               0,
                               5,
                               device,
                               NULL,
                               (P_AUDIO_TRACK_ASYNC_IO)app_usb_audio_ual_mic_read);
    if (track)
    {
        app_usb_audio_ual_mic_vol_ctrl(track, &(mic_ctrl->vol));
        app_usb_audio_ual_mic_mute_ctrl(track, mic_ctrl->mute);
        app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_INITED);
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#else
        app_usb_audio_start(USB_AUDIO_SCENARIO_CAPTURE);
#endif
    }
    audio->handle = track;

    return track;
}

static void app_usb_audio_set_mic_mute(bool mute)
{
    T_USB_AUDIO *audio = app_usb_audio_db.capture;

    if ((audio != NULL) && (audio->handle != NULL))
    {
        if (mute)
        {
            if (audio_track_volume_in_mute(audio->handle))
            {
                app_audio_tone_type_play(TONE_MIC_MUTE_ON, false, true);
            }
        }
        else
        {
            if (audio_track_volume_in_unmute(audio->handle))
            {
                app_audio_tone_type_play(TONE_MIC_MUTE_OFF, false, true);
            }
        }
    }
}

void app_usb_audio_mute_ctrl(void)
{
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (app_db.usb_is_enable_mic)
        {
            if (app_audio_is_mic_mute())
            {
                app_usb_audio_set_mic_mute(true);
            }
            else
            {
                app_usb_audio_set_mic_mute(false);
            }
        }
    }
}

static bool app_usb_audio_ual_mic_release(void *handle)
{
    bool ret = true;
    T_USB_AUDIO *audio = app_usb_audio_db.capture;

    if (handle)
    {
        ret = audio_track_release(handle);
        audio->handle = NULL;

        app_sidetone_detach(handle, audio->sidetone_instance);
        app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_IDLE);
    }

    return ret;
}

static bool app_usb_audio_ual_mic_start(void *handle)
{
    if (handle)
    {
        T_USB_AUDIO *audio = app_usb_audio_db.capture;

        return audio_track_start(handle);
    }
    APP_PRINT_ERROR0("app_usb_audio_ual_mic_start failed, handle NULL");

    return false;
}

static bool app_usb_audio_ual_mic_stop(void *handle)
{
    if (handle)
    {
        return audio_track_stop(handle);
    }
    APP_PRINT_ERROR0("app_usb_audio_ual_mic_stop failed, handle NULL");
    return false;
}

static bool app_usb_audio_ual_mic_ctrl(void *handle, uint8_t cmd, void *param)
{
    switch (cmd)
    {
    case CTRL_CMD_VOL_CHG:
        {
            T_UAS_VOL *vol = (T_UAS_VOL *)param;

            app_usb_audio_ual_mic_vol_ctrl(handle, vol);
        }
        break;

    case CTRL_CMD_MUTE:
        {
            uint32_t mute = *((uint32_t *)param);

            app_usb_audio_ual_mic_mute_ctrl(handle, (bool)mute);
        }
        break;

    default:
        break;
    }
    APP_PRINT_INFO3("app_usb_audio_ual_mic_ctrl, handle:0x%x, cmd:0x%x, ctrl:0x%x", handle, cmd, param);
    return true;
}

static bool app_usb_audio_scenario_start(T_USB_AUDIO *audio)
{
    T_USB_AUDIO_UAL_STATE state = USB_AUDIO_UAL_STATE_IDLE;

    if (audio)
    {
        state = app_usb_audio_ual_state_get(audio);

        if (state == USB_AUDIO_UAL_STATE_INITED || state == USB_AUDIO_UAL_STATE_STOPED ||
            state == USB_AUDIO_UAL_STATE_STOPPING)
        {
            usb_audio_stream_start(audio->id, ASYNC);
            app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_STARTING);
        }

        app_db.usb_is_enable_mic = true;

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
        if (app_slide_switch_mic_mute_toggle_support())
        {
            app_usb_audio_mute_ctrl();
        }
#endif

        return true;
    }

    return false;
}

void app_usb_audio_start(uint8_t scenario)
{
    APP_PRINT_TRACE1("app_usb_audio_start ++ : scenario: %d", scenario);
    bt_pwr_state = bt_power_mode_get();
    bt_power_mode_set(BTPOWER_ACTIVE);

    if (scenario & USB_AUDIO_SCENARIO_PLAYBACK)
    {
        app_usb_audio_scenario_start(app_usb_audio_db.playback);
    }

    if (scenario & USB_AUDIO_SCENARIO_CAPTURE)
    {
        app_usb_audio_scenario_start(app_usb_audio_db.capture);
    }
}

static bool app_usb_audio_scenario_stop(T_USB_AUDIO *audio)
{
    T_USB_AUDIO_UAL_STATE state = USB_AUDIO_UAL_STATE_IDLE;

    if (audio)
    {
        state = app_usb_audio_ual_state_get(audio);

        if (state == USB_AUDIO_UAL_STATE_STARTED || state == USB_AUDIO_UAL_STATE_STARTING ||
            state == USB_AUDIO_UAL_STATE_RUNNING)
        {
            usb_audio_stream_stop(audio->id, false);
            app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_STOPPING);
        }

        app_db.usb_is_enable_mic = false;

        if (app_audio_is_mic_mute()
#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
            && !app_slide_switch_mic_mute_toggle_support()
#endif
           )
        {
            app_audio_set_mic_mute_status(0);
            app_usb_audio_set_mic_mute(false);
        }

        return true;
    }

    return false;
}

void app_usb_audio_stop(uint8_t scenario)
{
    APP_PRINT_TRACE1("app_usb_audio_stop ++: scenario %d", scenario);

    if (scenario & USB_AUDIO_SCENARIO_PLAYBACK)
    {
        app_usb_audio_scenario_stop(app_usb_audio_db.playback);
    }

    if (scenario & USB_AUDIO_SCENARIO_CAPTURE)
    {
        app_usb_audio_scenario_stop(app_usb_audio_db.capture);
    }

    bt_power_mode_set(bt_pwr_state);
}

static void app_usb_audio_policy_cback(T_AUDIO_EVENT event_type, void *event_buf,
                                       uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;
    T_USB_AUDIO *audio = app_usb_audio_db.playback;

    if (param->track_state_changed.handle != audio->handle)
    {
        audio = app_usb_audio_db.capture;
        if (param->track_state_changed.handle != audio->handle)
        {
            return;
        }
    }

    APP_PRINT_TRACE3("app_usb_audio_policy_cback: event :0x%x, handle 0x%04x-0x%04x",
                     event_type, param->track_state_changed.handle, audio->handle);
    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            uint8_t track_state = param->track_state_changed.state;

            switch (track_state)
            {
            case AUDIO_TRACK_STATE_STARTED:
            case AUDIO_TRACK_STATE_RESTARTED:
                {
                    if (track_state == AUDIO_TRACK_STATE_STARTED)
                    {
                        audio->sidetone_instance = app_sidetone_attach(audio->handle, app_dsp_cfg_sidetone);
                    }

                    usb_audio_stream_start_complete(audio->id);
                    app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_STARTED);
                }
                break;

            case AUDIO_TRACK_STATE_STOPPED:
                {
                    usb_audio_stream_stop_complete(audio->id);
                    app_usb_audio_ual_state_set(audio, USB_AUDIO_UAL_STATE_STOPED);
                }
                break;
            }
        }
        break;

    case AUDIO_EVENT_TRACK_BUFFER_LOW:
        {
            if (audio == app_usb_audio_db.playback)
            {
                T_SPK_FLOW_CTL *flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;
                app_usb_audio_spk_multi_pkt_xmit(audio,
                                                 usb_audio_stream_get_data_len(audio->id) / (flow_ctrl->pkt_size), false);
            }
        }
        break;

    case AUDIO_EVENT_TRACK_BUFFER_HIGH:
        {
            if (audio == app_usb_audio_db.playback)
            {
                T_SPK_FLOW_CTL *flow_ctrl = (T_SPK_FLOW_CTL *)audio->misc;
                flow_ctrl->permission = false;
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_usb_audio_policy_cback: event_type 0x%04x", event_type);
    }
}

uint8_t app_usb_audio_is_mic_mute(void)
{
    return 0;
}

void *app_usb_audio_get_eq_instance(void)
{
    return app_usb_audio_db.playback->eq_instance;
}

bool app_usb_audio_is_active(void)
{
//    APP_PRINT_INFO1("app_usb_audio_is_active: %d", g_usb_audio_connected);
//    return (g_usb_audio_connected == 1);
    return true;
}

static const T_UAS_FUNC app_usb_audio_ual_funcs_spk =
{
    .create = app_usb_audio_ual_spk_create,
    .release = app_usb_audio_ual_spk_release,
    .start = app_usb_audio_ual_spk_start,
    .stop = app_usb_audio_ual_spk_stop,
    .xmit = app_usb_audio_ual_spk_xmit,
    .ctrl = app_usb_audio_ual_spk_ctrl,
};

static const T_UAS_FUNC app_usb_audio_ual_funcs_mic =
{
    .create = app_usb_audio_ual_mic_create,
    .release = app_usb_audio_ual_mic_release,
    .start = app_usb_audio_ual_mic_start,
    .stop = app_usb_audio_ual_mic_stop,
    .xmit = NULL,
    .ctrl = app_usb_audio_ual_mic_ctrl,
};

void app_usb_audio_init(void)
{
    app_usb_audio_db.playback = malloc(sizeof(T_USB_AUDIO));
    app_usb_audio_db.playback->id = usb_audio_stream_ual_bind(USB_AUDIO_STREAM_TYPE_OUT,
                                                              USB_AUDIO_STREAM_LABEL_1,
                                                              (T_UAS_FUNC *)&app_usb_audio_ual_funcs_spk);
    app_usb_audio_db.playback->misc = malloc(sizeof(T_SPK_FLOW_CTL));
    app_usb_audio_ual_state_set(app_usb_audio_db.playback, USB_AUDIO_UAL_STATE_IDLE);

    app_usb_audio_db.capture = malloc(sizeof(T_USB_AUDIO));
    app_usb_audio_db.capture->id = usb_audio_stream_ual_bind(USB_AUDIO_STREAM_TYPE_IN,
                                                             USB_AUDIO_STREAM_LABEL_1,
                                                             (T_UAS_FUNC *)&app_usb_audio_ual_funcs_mic);
    app_usb_audio_ual_state_set(app_usb_audio_db.capture, USB_AUDIO_UAL_STATE_IDLE);

    audio_mgr_cback_register(app_usb_audio_policy_cback);
}
#endif
