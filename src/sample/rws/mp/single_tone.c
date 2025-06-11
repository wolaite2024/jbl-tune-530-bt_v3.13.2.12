/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#include <stdint.h>
#include <string.h>
#include "os_mem.h"
#include "os_msg.h"
#include "os_task.h"
#include "os_timer.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "trace.h"
#include "sysm.h"
#include "audio.h"
#include "remote.h"
#include "gap_br.h"
#include "gap_bond_le.h"
#include "gap_msg.h"
#include "gap_le.h"
#include "gap.h"
#include "app_timer.h"
#include "console_uart.h"
#include "test_mode.h"
#include "single_tone.h"
#include "engage.h"
#include "voice_prompt.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_led.h"
#include "app_io_msg.h"
#include "app_adp.h"
#include "app_dlps.h"
#include "app_device.h"
#include "app_mmi.h"
#include "app_audio_policy.h"
#include "app_auto_power_off.h"
#include "wdg.h"

#define HCI_CMD_PKT     0x01
#define HCI_ACL_PKT     0x02
#define HCI_SCO_PKT     0x03
#define HCI_EVT_PKT     0x04
#define HCI_RESET_OPCODE                                    0x0C03
#define HCI_SET_EVENT_FILTER_OPCODE                         0x0C05
#define HCI_WRITE_LOCAL_NAME_OPCODE                         0x0C13
#define HCI_WRITE_SCAN_ENABLE_OPCODE                        0x0C1A
#define HCI_WRITE_PAGE_SCAN_ACTIVITY_OPCODE                 0x0C1C
#define HCI_WRITE_INQUIRY_SCAN_ACTIVITY_OPCODE              0x0C1E
#define HCI_WRITE_INQUIRY_SCAN_TYPE_OPCODE                  0x0C43


#define HCI_ENABLE_DEVICE_UNDER_TEST_MODE_OPCODE            0x1803

#define HCI_VENDOR_LPS_SETTING_OPCODE                       0xFC6E
#define HCI_VENDOR_SINGLE_TONE_CONT_TX_OPCODE               0xFC78

#define HCI_LOCAL_NAME_LEN      248

typedef enum
{
    APP_TIMER_DUT_TONE_REPEAT,
    APP_TIMER_DUT_MODE_AUTO_POWER_OFF,
} T_APP_SINGLE_TONE_TIMER;

typedef enum
{
    SINGLE_TONE_EVENT_LOWER_STACK_READY = 0,
} T_SINGLE_TONE_EVENT;

typedef struct
{
    uint8_t msg_id;
} T_MP_HCI_MSG_PARAM;

static bool hci_local = false;

static void *single_tone_exit_timer;
static void *mp_hci_test_task_handle;
static void *mp_hci_test_msg_queue_handle;


static bool mp_hci_test_mode_running = false;
static uint8_t hci_init_status = 0;
static T_MP_HCI_TEST_ITEM mp_hci_cur_test_item = MP_HCI_TEST_DUT_MODE;

static uint8_t dut_timer_id = 0;
static uint8_t timer_idx_dut_tone_repeat = 0;
#if F_APP_DUT_MODE_AUTO_POWER_OFF
static uint8_t timer_idx_dut_mode_auto_power_off = 0;
#endif
static void dut_timeout_cb(uint8_t timer_evt, uint16_t param);


typedef enum
{
    HCI_INIT_STATE_WRITE_SCAN_ENABLE,
    HCI_INIT_STATE_SET_EVENT_FILTER,
    HCI_INIT_STATE_WRITE_LOCAL_NAME,
    HCI_INIT_STATE_WRITE_PAGE_SCAN_ACTIVITY,
    HCI_INIT_STATE_ENABLE_DUT_MODE,
    HCI_INIT_STATE_WRITE_SCAN_TYPE,
    HCI_INIT_STATE_WRITE_INQUIRY_SCAN_ACTIVITY,
    HCI_INIT_STATE_COMPLETED
} HCI_INIT_STATE;

bool mp_hci_send_msg(uint8_t msg_id)
{
    uint8_t  event;
    T_MP_HCI_MSG_PARAM msg;

    event = EVENT_HCI_MSG;

    msg.msg_id = msg_id;

    if (os_msg_send(mp_hci_test_msg_queue_handle, &msg, 0))
    {
        return  os_msg_send(audio_evt_queue_handle, &event, 0) == true;
    }

    return false;
}

static void single_tone_hci_cmd_vendor_cont_tx(uint8_t channel_num, uint8_t tx_gain)
{
    APP_PRINT_INFO0("single_tone_start");

    T_SINGLE_TONE_VEND_CMD_PARAMS *p_vend_cmd_params = os_mem_alloc(RAM_TYPE_DATA_ON,
                                                                    sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));
    if (p_vend_cmd_params)
    {
        hci_local = true;

        p_vend_cmd_params->pkt_type = 1;
        p_vend_cmd_params->opcode = HCI_VENDOR_SINGLE_TONE_CONT_TX_OPCODE;
        p_vend_cmd_params->length = 4;
        p_vend_cmd_params->start = 1;
        p_vend_cmd_params->channel = channel_num;
        p_vend_cmd_params->tx_gain = tx_gain;
        p_vend_cmd_params->rsvd = 0;

        hci_if_write((uint8_t *)p_vend_cmd_params, sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));
    }
}

static void single_tone_handle_hci_evt_data_ind(bool status, uint8_t *p_buf, uint32_t len)
{
    return;
}

static void single_tone_exit_timeout_cb(void *p_timer)
{
    chip_reset(RESET_ALL);
}

static void single_tone_test_init(void)
{
    APP_PRINT_INFO1("single_tone_init: single_tone_timeout_val %d ms",
                    app_cfg_nv.single_tone_timeout_val);

    if (app_cfg_nv.single_tone_timeout_val == 0 || app_cfg_nv.single_tone_timeout_val == 0xFFFFFFFF)
    {
        app_cfg_nv.single_tone_timeout_val = 20 * 1000;
    }

    if (true == os_timer_create(&single_tone_exit_timer, "single_tone_exit_timer",  1,
                                app_cfg_nv.single_tone_timeout_val, false, single_tone_exit_timeout_cb))
    {
        os_timer_start(&single_tone_exit_timer);
    }
}

static void dut_hci_cmd_reset(void)
{
    uint16_t opcode = HCI_RESET_OPCODE;
    uint8_t param_len = 0;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;

    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    hci_if_write(p_buf, cmd_len);
}

/**
0x00 No Scans enabled. Default.
0x01 Inquiry Scan enabled.  Page Scan disabled.
0x02 Inquiry Scan disabled. Page Scan enabled.
0x03 Inquiry Scan enabled.  Page Scan enabled.
*/
static void dut_hci_cmd_write_scan_enable(uint8_t scan_enable)
{
    uint16_t opcode = HCI_WRITE_SCAN_ENABLE_OPCODE;
    uint8_t param_len = 1;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;

    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    LE_UINT8_TO_STREAM(p, scan_enable);

    hci_if_write(p_buf, cmd_len);
}

static void dut_hci_cmd_set_event_filter(void)
{
    uint16_t opcode = HCI_SET_EVENT_FILTER_OPCODE;
    uint8_t param_len = 3;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;
    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    LE_UINT8_TO_STREAM(p, 0x02);
    LE_UINT8_TO_STREAM(p, 0x00);
    LE_UINT8_TO_STREAM(p, 0x02);

    hci_if_write(p_buf, cmd_len);
}

static void dut_hci_cmd_write_page_scan_activity(uint16_t page_scan_interval,
                                                 uint16_t page_scan_window)
{
    uint16_t opcode = HCI_WRITE_PAGE_SCAN_ACTIVITY_OPCODE;
    uint8_t param_len = 4;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;
    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    LE_UINT16_TO_STREAM(p, page_scan_interval);
    LE_UINT16_TO_STREAM(p, page_scan_window);

    hci_if_write(p_buf, cmd_len);
}

static void dut_hci_cmd_write_local_name(uint8_t *p_name, uint8_t name_len)
{
    uint16_t opcode = HCI_WRITE_LOCAL_NAME_OPCODE;
    uint8_t param_len = HCI_LOCAL_NAME_LEN;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_zalloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;

    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    if (name_len > HCI_LOCAL_NAME_LEN - 1)
    {
        name_len = HCI_LOCAL_NAME_LEN - 1;
    }

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, HCI_LOCAL_NAME_LEN);

    memcpy(p, p_name, name_len);
    p[name_len] = '\0';
    hci_if_write(p_buf, cmd_len);
}

/**
0x00 Standard Scan (default)
0x01 Interlaced Scan
*/
static void dut_hci_cmd_write_inquiry_scan_type(uint8_t scan_type)
{
    uint16_t opcode = HCI_WRITE_INQUIRY_SCAN_TYPE_OPCODE;
    uint8_t param_len = 1;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;
    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    LE_UINT8_TO_STREAM(p, scan_type);

    hci_if_write(p_buf, cmd_len);
}


static void dut_hci_cmd_write_inquiry_scan_activity(uint16_t inquiry_scan_interval,
                                                    uint16_t inquiry_scan_window)
{
    uint16_t opcode = HCI_WRITE_INQUIRY_SCAN_ACTIVITY_OPCODE;
    uint8_t param_len = 4;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;
    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    LE_UINT16_TO_STREAM(p, inquiry_scan_interval);
    LE_UINT16_TO_STREAM(p, inquiry_scan_window);

    hci_if_write(p_buf, cmd_len);
}

static void dut_hci_cmd_enable_dut_mode(void)
{

    uint16_t opcode = HCI_ENABLE_DEVICE_UNDER_TEST_MODE_OPCODE;
    uint8_t param_len = 0;

    uint8_t cmd_len = param_len + 4;
    uint8_t *p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, cmd_len);
    uint8_t *p = p_buf;
    if (NULL == p)
    {
        return;
    }

    hci_local = true;

    LE_UINT8_TO_STREAM(p, HCI_CMD_PKT);
    LE_UINT16_TO_STREAM(p, opcode);
    LE_UINT8_TO_STREAM(p, param_len);

    hci_if_write(p_buf, cmd_len);
}

static void dut_test_handle_hci_evt_data_ind(bool status, uint8_t *p_buf, uint32_t len)
{
    APP_PRINT_INFO1("dut_test_handle_hci_evt_data_ind: hci_init_status %d", hci_init_status);
    switch (hci_init_status)
    {
    case HCI_INIT_STATE_WRITE_SCAN_ENABLE:
        {
            dut_hci_cmd_write_scan_enable(3);
        }
        break;

    case HCI_INIT_STATE_SET_EVENT_FILTER:
        {
            dut_hci_cmd_set_event_filter();
        }
        break;

    case HCI_INIT_STATE_WRITE_LOCAL_NAME:
        {
            dut_hci_cmd_write_local_name(app_cfg_nv.device_name_legacy, sizeof(app_cfg_nv.device_name_legacy));
        }
        break;

    case HCI_INIT_STATE_WRITE_PAGE_SCAN_ACTIVITY:
        {
            dut_hci_cmd_write_page_scan_activity(0x0200, 0x0012);
        }
        break;

    case HCI_INIT_STATE_ENABLE_DUT_MODE:
        {
            dut_hci_cmd_enable_dut_mode();
        }
        break;

    case HCI_INIT_STATE_WRITE_SCAN_TYPE:
        {
            dut_hci_cmd_write_inquiry_scan_type(0);
        }
        break;

    case HCI_INIT_STATE_WRITE_INQUIRY_SCAN_ACTIVITY:
        {
            dut_hci_cmd_write_inquiry_scan_activity(0x0200, 0x0012);
        }
        break;

    case HCI_INIT_STATE_COMPLETED:
        break;

    default:
        break;
    }
    hci_init_status++;
}

#if F_APP_DUT_MODE_AUTO_POWER_OFF
void dut_test_start_auto_power_off_timer(void)
{
    if (timer_idx_dut_mode_auto_power_off == NULL)
    {
        app_start_timer(&timer_idx_dut_mode_auto_power_off, "dut_mode_auto_power_off",
                        dut_timer_id, APP_TIMER_DUT_MODE_AUTO_POWER_OFF, 0, false,
                        300000); // 5 mins
    }
}
#endif

static void dut_test_init(void)
{
    return;
}

static void dut_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_VOICE_PROMPT_STOPPED:
        {
            if ((param->voice_prompt_stopped.index + VOICE_PROMPT_INDEX) ==
                app_cfg_const.tone_power_off)
            {
                chip_reset(RESET_ALL);
            }
        }
        break;

    case AUDIO_EVENT_RINGTONE_STOPPED:
        {
            if (param->ringtone_stopped.index  == app_cfg_const.tone_power_off)
            {
                chip_reset(RESET_ALL);
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("dut_audio_cback: event_type 0x%04x", event_type);
    }
}
static bool mp_test_handle_hci_cb(T_SINGLE_TONE_EVT evt, bool status, uint8_t *p_buf, uint32_t len)
{
    APP_PRINT_INFO2("mp_test_handle_hci_cb: evt %d, status(bool) %d", evt, status);

    bool result = true;
    switch (evt)
    {
    case SINGLE_TONE_EVT_OPENED:
        {
            mp_hci_send_msg(SINGLE_TONE_EVENT_LOWER_STACK_READY);
        }
        break;

    case SINGLE_TONE_EVT_CLOSED:
        break;

    case SINGLE_TONE_EVT_DATA_IND:
        {
            switch (mp_hci_cur_test_item)
            {
            case MP_HCI_TEST_DUT_MODE:
                {
                    dut_test_handle_hci_evt_data_ind(status, p_buf, len);
                }
                break;

            case MP_HCI_TEST_SINGLE_TONE:
                {
                    single_tone_handle_hci_evt_data_ind(status, p_buf, len);
                }
                break;

            default:
                break;
            }

            hci_if_confirm(p_buf);
        }
        break;

    case SINGLE_TONE_EVT_DATA_XMIT:
        {
            if (hci_local)
            {
                os_mem_free(p_buf);
                hci_local = false;
            }
        }
        break;

    case SINGLE_TONE_EVT_ERROR:
        break;

    default:
        break;
    }

    return result;
}



void mp_hci_handle_msg(uint8_t event)
{
    if (event == EVENT_HCI_MSG)
    {
        T_MP_HCI_MSG_PARAM param;
        if (os_msg_recv(mp_hci_test_msg_queue_handle, &param, 0) == true)
        {
            switch (param.msg_id)
            {
            case SINGLE_TONE_EVENT_LOWER_STACK_READY:
                {
                    switch (mp_hci_cur_test_item)
                    {
                    case MP_HCI_TEST_DUT_MODE:
                        {
                            dut_hci_cmd_reset();
                        }
                        break;

                    case MP_HCI_TEST_SINGLE_TONE:
                        {
                            single_tone_hci_cmd_vendor_cont_tx(app_cfg_nv.single_tone_channel_num,
                                                               app_cfg_nv.single_tone_tx_gain);
                        }
                        break;

                    default:
                        break;
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}

static void mp_hci_test_task(void *p_param)
{
    APP_PRINT_INFO0("mp_hci_test_task");

    uint8_t event;

    os_msg_queue_create(&mp_hci_test_msg_queue_handle, "mp_hci_msgQ", 10, sizeof(T_MP_HCI_MSG_PARAM));

    hci_if_open(mp_test_handle_hci_cb);

    if (app_cfg_const.enable_slide_switch_detect == 0)
    {
        /* if slide switch feature is open, in dut wdt reset step,
        system would consider key0 as top to down,
        so would trigger key0 press,
        we think power on mmi action should take by key handle,
        not here */
        app_mmi_handle_action(MMI_DEV_POWER_ON);
    }

    while (true)
    {
        if (os_msg_recv(audio_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (EVENT_GROUP(event) == EVENT_GROUP_IO)
            {
                T_IO_MSG io_msg;

                if (os_msg_recv(audio_io_queue_handle, &io_msg, 0) == true)
                {
                    if (event == EVENT_IO_TO_APP)
                    {
                        app_io_msg_handler(io_msg);
                    }
                }
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_STACK)
            {
                if (event == EVENT_HCI_MSG)
                {
                    mp_hci_handle_msg(event);
                }
                else
                {
                    gap_handle_msg(event);
                }

            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_FRAMEWORK)
            {
                sys_mgr_event_handle(event);
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_APP)
            {
                app_timer_handle_msg(event);
            }

            if (app_cfg_const.led_support)
            {
                app_led_check_charging_mode(0);
                app_led_check_repeat_mode();
            }
        }
    }
}

static void mp_hci_test_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            APP_PRINT_INFO3("mp_hci_test_dm_cback: bud_role %d, factory_reset_done %d, first_engaged %d",
                            app_cfg_nv.bud_role, app_cfg_nv.factory_reset_done, app_cfg_nv.first_engaged);

            app_device_state_change(APP_DEVICE_STATE_ON);

            app_led_change_mode(LED_MODE_DUT_TEST_MODE, true, false);
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_POWER_ON, app_cfg_const.timer_auto_power_off);

            /* user do not want to power off in dut mode, so stop auto power off timer in dut mode*/
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_DUT_MODE);

#if F_APP_DUT_MODE_AUTO_POWER_OFF
            if (app_cfg_const.enable_5_mins_auto_power_off_under_dut_test_mode)
            {
                dut_test_start_auto_power_off_timer();
            }
#endif

            if (app_cfg_const.enable_repeat_dut_test_tone)
            {
                app_audio_tone_type_play(TONE_DUT_TEST, false, false);
                app_start_timer(&timer_idx_dut_tone_repeat, "dut_tone_repeat",
                                dut_timer_id, APP_TIMER_DUT_TONE_REPEAT, 0, true,
                                5000);
            }
            else
            {
                app_audio_tone_type_play(TONE_DUT_TEST, false, false);
            }

            app_cfg_nv.is_dut_test_mode = 1;
            app_cfg_nv.trigger_dut_mode_from_power_off = 0;
            app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

            if (app_cfg_nv.app_is_power_on == 0)
            {
                app_cfg_nv.app_is_power_on = 1;
            }
#if F_APP_GPIO_ONOFF_SUPPORT
            if (app_cfg_const.box_detect_method == GPIO_DETECT)
            {
                if (app_device_is_in_the_box())
                {
                    app_cfg_nv.pre_3pin_status_unplug = 0;
                }
                else
                {
                    app_cfg_nv.pre_3pin_status_unplug = 1;
                }
            }
#endif
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            dut_hci_cmd_reset();
            app_cfg_nv.trigger_dut_mode_from_power_off = 0;
            app_cfg_store(&app_cfg_nv.eq_idx_gaming_mode_record, 4);

            if (app_cfg_nv.app_is_power_on)
            {
                app_cfg_nv.app_is_power_on = 0;
                app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
            }

            /* now power off mean exit dut mode, so auto power off timer open again*/
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_DUT_MODE, app_cfg_const.timer_auto_power_off);

#if F_APP_DUT_MODE_AUTO_POWER_OFF
            app_stop_timer(&timer_idx_dut_mode_auto_power_off);
#endif

            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_ALREADY_POWER_OFF);
            app_dlps_stop_power_down_wdg_timer();

            if (!app_device_is_in_the_box())
            {
                app_led_change_mode(LED_MODE_POWER_OFF, true, false);
            }

            app_dlps_enable(0xFFFF);
            power_mode_set(POWER_POWERDOWN_MODE);
            mp_hci_test_deinit();
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("mp_hci_test_dm_cback: event_type 0x%04x", event_type);
    }
}

void mp_hci_test_mmi_handle_action(uint8_t action)
{
    APP_PRINT_INFO1("mp_hci_test_mmi_handle_action: action 0x%02x", action);

    switch (action)
    {
    case MMI_DEV_POWER_ON:
        {
            sys_mgr_power_on();
        }
        break;

    case MMI_DEV_POWER_OFF:
        {
#if F_APP_HARMAN_FEATURE_SUPPORT
            if (app_cfg_nv.language_status == 0)
            {
                app_audio_tone_type_play(TONE_HARMAN_POWER_OFF, false, false);
            }
            else
#endif
            {
                app_audio_tone_type_play(TONE_POWER_OFF, false, false);
            }
            sys_mgr_power_off();
        }
        break;

    default:
        break;
    }
}

void mp_hci_test_deinit(void)
{
    sys_mgr_cback_unregister(mp_hci_test_dm_cback);
    mp_hci_test_mode_running = false;
}

static void dut_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_DUT_TONE_REPEAT:
        {
            app_audio_tone_type_play(TONE_DUT_TEST, false, false);
        }
        break;

#if F_APP_DUT_MODE_AUTO_POWER_OFF
    case APP_TIMER_DUT_MODE_AUTO_POWER_OFF:
        {
            app_stop_timer(&timer_idx_dut_mode_auto_power_off);
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
        break;
#endif

    default:
        break;
    }
}

void mp_hci_test_init(T_MP_HCI_TEST_ITEM mp_hci_test_item)
{
    APP_PRINT_INFO1("mp_hci_test_init: mp_hci_test_item %d", mp_hci_test_item);

    mp_hci_test_mode_running = true;
    mp_hci_cur_test_item = mp_hci_test_item;
    power_mode_set(POWER_ACTIVE_MODE);

    switch (mp_hci_test_item)
    {
    case MP_HCI_TEST_DUT_MODE:
        {
            dut_test_init();
        }
        break;

    case MP_HCI_TEST_SINGLE_TONE:
        {
            single_tone_test_init();
        }
        break;
    }

    sys_mgr_cback_register(mp_hci_test_dm_cback);
    audio_mgr_cback_register(dut_audio_cback);
    app_timer_reg_cb(dut_timeout_cb, &dut_timer_id);

    os_task_create(&mp_hci_test_task_handle, "mp_hci_test", mp_hci_test_task, 0, 2048, 1);
}

bool mp_hci_test_mode_is_running(void)
{
    return mp_hci_test_mode_running;
}

void mp_hci_test_set_mode(bool mp_hci_test_mode_running_status)
{
    mp_hci_test_mode_running = mp_hci_test_mode_running_status;
}

void mp_hci_test_auto_enter_dut_init(void)
{
    audio_mgr_cback_register(dut_audio_cback);
}
