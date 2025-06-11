#if F_APP_USB_AUDIO_SUPPORT
#include <string.h>
#include <bt_types.h>
#include "trace.h"
#include "os_mem.h"
#include "os_queue.h"
#include "os_timer.h"
#include "rtl876x.h"
#include "stdlib.h"
#include "app_usb_audio_hid.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_gpio.h"
#include "app_io_msg.h"
#include "app_multilink.h"
#include "app_usb_audio.h"
#include "app_cfu.h"
#include "app_audio_policy.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "asp_device_api.h"
#include "asp_device_link.h"
#include "app_asp_device.h"
#include "app_teams_cmd.h"
#include "bt_hid.h"
#include "bt_hfp.h"
#include "app_teams_audio_policy.h"
#include "app_teams_extend_led.h"
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
#include "hid_vp_update.h"
#endif
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif

const char report_desc_attrib[] =
{
    BUTTON_VOL_PLAY_CTRL_HID_DESC_ATTRIB_2,

#if F_APP_TEAMS_FEATURE_SUPPORT
    TEAMS_ASP_HID_DESC_ATTRIB,
#endif

    TELEPHONY_HID_DESC_ATTRIB,

    APP_CMD_HID_DESC_ATTRIB,

    APP_TELEMETRY_HID_DESC_ATTRIB,
};

uint16_t report_desc_attrib_length = sizeof(report_desc_attrib);

void app_usb_hid_handle_set_Telephony_cmd(uint8_t *data, uint8_t length);

void hid_handle_set_report_msg(uint8_t *data, uint16_t length)
{
    USB_PRINT_TRACE3("hid_handle_set_report_msg ID 0x%x, length 0x%x, data:%b", data[0], length,
                     TRACE_BINARY(MIN(length, 8), data));
#if F_APP_TEAMS_FEATURE_SUPPORT
    T_ASP_DEVICE_LINK_ALLOC_PARAM param_data = {0};
#endif

    switch (data[0])
    {
    case REPORT_ID_TELEPHONY_INPUT:
        app_usb_hid_handle_set_Telephony_cmd(data, length);
        break;
    case REPORT_ID_ASP_FEATURE:
#if F_APP_TEAMS_FEATURE_SUPPORT
        if (!memcmp(asp_start, data, sizeof(asp_start)))
        {
            if (!asp_device_find_link_by_addr(teams_asp_usb_addr))
            {
                param_data.bd_addr = teams_asp_usb_addr;
                asp_device_alloc_link(ASP_DEVICE_LINK_USB, &param_data);
            }
        }
        asp_device_api_receive_pkt(teams_asp_usb_addr, length, data);
#endif
        break;

    case REPORT_ID_USB_COMMAND_SET:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            T_APP_TEAMS_HID_DATA hid_data;
            hid_data.teams_usb_data.data_len = length - 1;
            hid_data.teams_usb_data.p_data = &data[1];
            app_teams_hid_handle_config_report(REPORT_ID_USB_COMMAND_SET, APP_TEAMS_HID_REPORT_SOURCE_USB,
                                               &hid_data);
#endif
        }
        break;

    case REPORT_ID_VP_FEATURE:
        {
#if F_APP_TEAMS_VP_UPDATE_SUPPORT
            hid_vp_update_handle_data_ind(teams_asp_usb_addr, length - 1, &data[1]);
#endif
        }
        break;

    default:
        USB_PRINT_ERROR3("hid_handle_set_report_msg unknow ID %x, length 0x%x, %b", data[0], length,
                         TRACE_BINARY(MIN(length, 8), data));
        break;
    }
}

void hid_handle_set_report(uint8_t *data, uint16_t length)
{
    USB_PRINT_TRACE3("hid_handle_set_report ID %x, length 0x%x, %b", data[0], length,
                     TRACE_BINARY(MIN(length, 8), data));
    T_IO_MSG gpio_msg = {0};
    gpio_msg.type = IO_MSG_TYPE_USB_HID;
    gpio_msg.subtype = USB_HID_MSG_TYPE_HID_SET_REPORT | (length << 8);
    gpio_msg.u.buf = malloc(MIN(length, 0x3f));
    if (gpio_msg.u.buf == NULL)
    {
        APP_PRINT_ERROR0("hid_handle_set_report: malloc fail");
        return;
    }
    memcpy(gpio_msg.u.buf, data, MIN(length, 0x3f));
    if (app_io_msg_send(&gpio_msg) == false)
    {
        free(gpio_msg.u.buf);
    }
}

void app_usb_hid_send_get_report_data(uint8_t *data, uint8_t length)
{
    if (!data || !length)
    {
        return;
    }
    APP_PRINT_TRACE2("app_usb_hid_send_get_report_data 0x%x %b",
                     length, TRACE_BINARY(MIN(length, 8), data));
    hid_send_get_report_data(data, length);
}

void app_usb_hid_send_get_report_zero_ack(uint8_t reportid, uint8_t length)
{
    uint8_t tmp[64] = {0};
    uint8_t len = length;
    len = MIN(len, 64);
    tmp[0] = reportid;
    app_usb_hid_send_get_report_data(tmp, len);
}

uint8_t hid_handle_get_report(uint8_t reportID, uint8_t *data, uint16_t *length)
{
    uint8_t result = 2;
    uint16_t len =  *length;
    T_IO_MSG gpio_msg = {0};
    USB_PRINT_TRACE2("hid_handle_get_report, length 0x%x, %b", *length,
                     TRACE_BINARY(MIN(*length, 8), data));

    gpio_msg.type = IO_MSG_TYPE_USB_HID;
    gpio_msg.subtype = USB_HID_MSG_TYPE_HID_GET_REPORT | (len << 8);
    gpio_msg.u.param = reportID;
    if (app_io_msg_send(&gpio_msg) == false)
    {
        result = 0;
    }
    return result;
}

void hid_handle_get_report_msg(uint8_t reportID, uint16_t get_len)
{
#if F_APP_TEAMS_FEATURE_SUPPORT
    uint8_t *p_asp_device_get_report_buffer = NULL;
    uint8_t *usb_command_get_report_buffer = NULL;
#endif
    USB_PRINT_TRACE2("hid_handle_get_report_msg ID:0x%x, get_len:%d", reportID, get_len);
    switch (reportID)
    {
    case REPORT_ID_ASP_FEATURE:
#if F_APP_TEAMS_FEATURE_SUPPORT
        p_asp_device_get_report_buffer = asp_device_api_get_usb_pkt_buf();
        if (p_asp_device_get_report_buffer)
        {
            app_usb_hid_send_get_report_data(p_asp_device_get_report_buffer, ASP_DEVICE_MAX_PKT_FOR_USB);
            USB_PRINT_TRACE1("hid_handle_get_report_msg, asp pkt %b",
                             TRACE_BINARY(8, p_asp_device_get_report_buffer));
            memset(p_asp_device_get_report_buffer, 0, ASP_DEVICE_MAX_PKT_FOR_USB);
        }
        else
        {
            app_usb_hid_send_get_report_zero_ack(reportID, get_len);
        }
#endif
        break;

    case REPORT_ID_USB_COMMAND_GET:
#if F_APP_TEAMS_FEATURE_SUPPORT
        if (app_teams_cmd_get_usb_response())
        {
            usb_command_get_report_buffer = app_teams_cmd_get_usb_response();
            app_usb_hid_send_get_report_data(usb_command_get_report_buffer, APP_COMMAND_MAX_FOR_USB);
            USB_PRINT_TRACE1("hid_handle_get_report_msg, usb cmd %b",
                             TRACE_BINARY(8, usb_command_get_report_buffer));
            os_mem_free(app_command_for_usb);
            app_command_for_usb = NULL;
        }
#endif
        break;

    case REPORT_ID_VP_FEATURE:
        {
            app_usb_hid_send_get_report_zero_ack(reportID, get_len);
        }
        break;

    default:
        {
            app_usb_hid_send_get_report_zero_ack(reportID, get_len);
            USB_PRINT_ERROR1("hid_handle_get_report_msg unknow report ID 0x%x", reportID);
        }
        break;
    }
}

T_OS_QUEUE  hid_in_queue;
uint8_t app_hid_interrupt_in_state;
void app_hid_interrupt_in(uint8_t *data, uint8_t data_size)
{
    USB_PRINT_TRACE2("app_hid_interrupt_in data_size 0x%x %b", data_size,
                     TRACE_BINARY(MIN(data_size, 4), data));
    uint8_t *buf = malloc(data_size + 1 + sizeof(T_OS_QUEUE_ELEM));
    buf[sizeof(T_OS_QUEUE_ELEM)] = data_size;
    memcpy(&buf[sizeof(T_OS_QUEUE_ELEM) + 1], data, data_size);
    os_queue_in(&hid_in_queue, buf);
    if (!app_hid_interrupt_in_state || hid_in_queue.count > 10)
    {
        T_IO_MSG gpio_msg;
        gpio_msg.type = IO_MSG_TYPE_USB_HID;
        gpio_msg.subtype = USB_HID_MSG_TYPE_HID_IN_REQUEST;
        gpio_msg.u.param = 0;
        app_hid_interrupt_in_state = 1;
        if (app_io_msg_send(&gpio_msg) == false)
        {
            app_hid_interrupt_in_state = 0;
            os_queue_delete(&hid_in_queue, buf);
            free(buf);
        }
    }
}

void hid_interrupt_in_complete_result(int result, uint8_t *buf)
{
    T_IO_MSG gpio_msg = {0};
    gpio_msg.type = IO_MSG_TYPE_USB_HID;
    gpio_msg.subtype = USB_HID_MSG_TYPE_HID_IN_COMPLETE;
    gpio_msg.u.buf = buf;
    if (app_io_msg_send(&gpio_msg) == false)
    {
        free(--buf);
    }
}

///////////////////////////patch for interrupt in lost // to slow down
static void *hid_in_timer_handle;
void hid_in_timer_cb(void *p)
{
//    USB_PRINT_TRACE2("hid_in_timer_cb state %x count %d",app_hid_interrupt_in_state, hid_in_queue.count);
    app_hid_interrupt_in_state = 0;
    if (hid_in_queue.count)
    {
        T_IO_MSG gpio_msg;
        gpio_msg.type = IO_MSG_TYPE_USB_HID;
        gpio_msg.subtype = USB_HID_MSG_TYPE_HID_IN_REQUEST;
        gpio_msg.u.param = 0;

        app_io_msg_send(&gpio_msg);
        app_hid_interrupt_in_state = 1;
    }
}

void app_usb_hid_handle_msg(T_IO_MSG *msg)
{
    uint16_t hid_msg_type = msg->subtype & 0xff;
//    USB_PRINT_TRACE1("app_usb_hid_handle_msg %x",hid_msg_type);
    switch (hid_msg_type)
    {
    case USB_HID_MSG_TYPE_HID_SET_REPORT:
        {
            hid_handle_set_report_msg(msg->u.buf, (msg->subtype) >> 8);
            free(msg->u.buf);
        }
        break;
    case USB_HID_MSG_TYPE_HID_GET_REPORT:
        {
            hid_handle_get_report_msg(msg->u.param, (msg->subtype) >> 8);
        }
        break;
    case USB_HID_MSG_TYPE_HID_SUSPEND_RESUME:
        {
            if (msg->u.param == 1)
            {
                USB_PRINT_TRACE0("USB_HID_MSG_TYPE_HID_SUSPEND");
            }
            else if (msg->u.param == 2)
            {
                USB_PRINT_TRACE0("USB_HID_MSG_TYPE_HID RESUME");
            }
        }
        break;
    case USB_HID_MSG_TYPE_HID_IN_REQUEST:
        {
//            USB_PRINT_TRACE2("USB_HID_MSG_TYPE_HID_IN_REQUEST state %x count %d",app_hid_interrupt_in_state, hid_in_queue.count);
            if (app_hid_interrupt_in_state)
            {
                if (hid_in_queue.count)
                {
                    uint8_t *buf = os_queue_out(&hid_in_queue);
                    if (hid_interrupt_in(&buf[sizeof(T_OS_QUEUE_ELEM) + 1], buf[sizeof(T_OS_QUEUE_ELEM)]))
                    {
                        USB_PRINT_TRACE2("USB_HID_MSG_TYPE_HID_IN_REQUEST addr 0x%x %b", buf, TRACE_BINARY(4,
                                         &buf[sizeof(T_OS_QUEUE_ELEM)]));
                    }
                    else
                    {
                        USB_PRINT_WARN2("USB_HID_MSG_TYPE_HID_IN_REQUEST fail 0x%x %b", buf,
                                        TRACE_BINARY(4, &buf[sizeof(T_OS_QUEUE_ELEM)]));
                        free(buf);
                        app_hid_interrupt_in_state = 0;
                        if (hid_in_queue.count)
                        {
                            T_IO_MSG gpio_msg;
                            gpio_msg.type = IO_MSG_TYPE_USB_HID;
                            gpio_msg.subtype = USB_HID_MSG_TYPE_HID_IN_REQUEST;
                            gpio_msg.u.param = 0;

                            app_io_msg_send(&gpio_msg);
                            app_hid_interrupt_in_state = 1;
                        }
                    }
                }
            }
            break;
        }
    case USB_HID_MSG_TYPE_HID_IN_COMPLETE:
        {
            uint8_t *buf = msg->u.buf;
            buf -= (sizeof(T_OS_QUEUE_ELEM) + 1);
            USB_PRINT_TRACE2("USB_HID_MSG_TYPE_HID_IN_COMPLETE buf addr 0x%x %b", buf, TRACE_BINARY(4,
                             &buf[sizeof(T_OS_QUEUE_ELEM)]));
            free(buf);
            os_timer_start(&hid_in_timer_handle);
            break;
        }
    default:
        USB_PRINT_TRACE1("app_usb_hid_handle_msg unknow 0x%x", hid_msg_type);
        break;
    }
}

void hid_handle_device_suspend(void)
{
    T_IO_MSG gpio_msg = {0};
    gpio_msg.type = IO_MSG_TYPE_USB_HID;
    gpio_msg.subtype = USB_HID_MSG_TYPE_HID_SUSPEND_RESUME;
    gpio_msg.u.param = 1;

    app_io_msg_send(&gpio_msg);
}

void hid_handle_device_resume(void)
{
    T_IO_MSG gpio_msg = {0};
    gpio_msg.type = IO_MSG_TYPE_USB_HID;
    gpio_msg.subtype = USB_HID_MSG_TYPE_HID_SUSPEND_RESUME;
    gpio_msg.u.param = 2;

    app_io_msg_send(&gpio_msg);
}

uint8_t last_data = 0x00;
bool g_teams_is_mute = false;
extern bool g_usb_sys_mic_is_mute;
bool g_usb_link_mic_is_mute = false;

bool isOutgoingCall = false;
bool isIncomingCall = false;
bool app_hfp_isInCall = false;
static bool is_hold_call = false;
static bool is_hold_mute = false;
static bool mute_status = false;
static bool hold_flag = false;
static bool not_resume_flag = false;
static bool sfb_resume_mute_flag = false;
static uint8_t hold_count = 0;
static uint8_t call_count = 0;
static uint8_t pre_hook = 0x00;
static uint8_t s_last_teams_status = USB_TEAMS_CALL_IDLE;
static void app_usb_teams_check_and_notify_call_status(void);

static void app_resume_notify_mute_status(void)
{
    APP_PRINT_TRACE4("app_resume_notify_mute_status, mute_status:%d, is_hold_mute: %d, teams_is_mute: %d, sfb_mute:%d",
                     mute_status, is_hold_mute, g_teams_is_mute, sfb_resume_mute_flag);
    if (mute_status || is_hold_mute || sfb_resume_mute_flag)
    {
        if (false == g_teams_is_mute)
        {
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            usb_teams_data.report.offHook = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            usb_teams_data.report.offHook = 1;
            usb_teams_data.report.mute = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        sfb_resume_mute_flag = false;
        mute_status = false;
    }
    else
    {
        // notify host in call state
        TELEPHONY_HID_INPUT_REPORT usb_teams_data;
        usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
        usb_teams_data.data[1] = 0;
        usb_teams_data.report.offHook = 1;
        app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
    }
}

void app_usb_hid_handle_set_Telephony_cmd(uint8_t *data, uint8_t length)
{
    TELEPHONY_HID_OUTPUT_REPORT report_data;
    memcpy((uint8_t *)&report_data, data, sizeof(report_data));
    uint8_t last_hook = 0x00;
    uint8_t last_mute = 0x00;
    uint8_t last_ring = 0x00;
    uint8_t last_hold = 0x00;
    uint8_t off_hook = report_data.report.offHook;
    uint8_t mute = report_data.report.mute;
    uint8_t ring = report_data.report.ring;
    uint8_t hold = report_data.report.hold;

    if (data[1] != last_data)
    {
        if (data[1] == 0x21 || data[1] == 0x23)
        {
            app_hfp_isInCall = false;
            isIncomingCall = false;
        }

        TELEPHONY_HID_OUTPUT_REPORT data_tmp;
        data_tmp.data[1] = last_data;
        last_hook = data_tmp.report.offHook;
        last_mute = data_tmp.report.mute;
        last_ring = data_tmp.report.ring;
        last_hold = data_tmp.report.hold;

        if (hold != last_hold)
        {
            if (hold)
            {
                pre_hook = 0;
                is_hold_call = true;
                if (g_teams_is_mute && hold_count == 0)
                {
                    is_hold_mute = true;
                }
                hold_count++;
                APP_PRINT_TRACE2("hold changed, hold_count: %d, is_hold_mute:%d", hold_count, is_hold_mute);
                if (mute && off_hook == 0x00)
                {
                    sfb_resume_mute_flag = true;
                }
            }
            else
            {
                if (data[1] == 0x00)
                {
                    isIncomingCall = false;
                    isOutgoingCall = false;
                    app_hfp_isInCall = false;
                    call_count = 0;
                    hold_flag = false;
                    hold_count --;
                    is_hold_mute = false;
                    not_resume_flag = false;
                    g_teams_is_mute = false;
                    if (g_usb_sys_mic_is_mute == false)
                    {
                        app_usb_audio_volume_in_unmute();
                        app_usb_audio_set_global_mute_status(g_teams_is_mute);
                    }
                    TELEPHONY_HID_INPUT_REPORT usb_teams_data;
                    usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
                    usb_teams_data.data[1] = 0;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                }
                else if (off_hook)
                {
                    app_hfp_isInCall = true;
                    if (mute)
                    {
                        //TEAMS_STATUS_MUTE
                    }
                    else
                    {
                        //TEAMS_IN_CALL
                    }
                }
            }
            last_data = data[1];
            app_usb_teams_check_and_notify_call_status();
            return;
        }

        APP_PRINT_TRACE1("app_usb_hid_handle_set_Telephony_cmd, current_call_num: %d", call_count);
        if (off_hook != last_hook)
        {
            APP_PRINT_TRACE4("is_hold_call: %d, hold: %d, is_hold_mute:%d, hold_count: %d", is_hold_call, hold,
                             is_hold_mute, hold_count);
            if (off_hook)
            {
                if (ring == 0x00)
                {
                    if (is_hold_call)
                    {
                        if (hold)  //call-2 outgoing
                        {
                            // send_call_dial_status
                            mute_status = g_teams_is_mute;
                            isOutgoingCall = true;
                            hold_count = 1;
                            is_hold_call = true;
                            hold_flag = false;
                        }
                        else if (!not_resume_flag)
                        {
                            APP_PRINT_TRACE2("call-1 resumed, call_count: %d, mute_status: %d ", call_count, mute_status);
                            // send_call_answer_status
                            app_resume_notify_mute_status();
                            hold_count = 0;
                            is_hold_call = false;
                            is_hold_mute = false;
                            hold_flag = false;
                            call_count = 1;
                            app_hfp_isInCall = true;
                        }
                    }
                    else     //call-1 outgoing
                    {
                        // to do call back outgoing
                        isOutgoingCall = true;
                    }
                }
            }
            else
            {
                if ((hold) && (hold_count == 1))      //call-1 hold
                {

                    //hang up 2nd call or 2nd outgoing call is rejected by far end
                    if (call_count > 0)
                    {
                        call_count--;
                    }
                    hold_flag = true;
                    // send_call_terminate_status
                    app_hfp_isInCall = false;

                }
                else if ((hold == 0x00) || ((hold == 0x1) && (call_count == 2)))
                {
                    // to do call back send_call_terminate_status
                    if (!app_usb_audio_is_us_streaming())
                    {
                        // to do call back diconnected audio_disconnect_req
                    }

                    TELEPHONY_HID_INPUT_REPORT usb_teams_data;
                    usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
                    usb_teams_data.data[1] = 0;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                    app_hfp_isInCall = false;

                    APP_PRINT_TRACE3("call terminated, hold_count: %d, call_count: %d, is_hold_mute:%d", hold_count,
                                     call_count, is_hold_mute);

                    if (hold_count > 0)
                    {
                        hold_count--;
                    }
                    if (call_count > 0)
                    {
                        call_count--;
                    }

                    if (call_count == 0)
                    {
                        is_hold_mute = false;
                        is_hold_call = false;
                        hold_count = 0;
                        mute_status = false;
                        g_teams_is_mute = false;
                        if (g_usb_sys_mic_is_mute == false)
                        {
                            app_usb_audio_volume_in_unmute();
                            app_usb_audio_set_global_mute_status(g_teams_is_mute);
                        }
                        not_resume_flag = false;
                    }
                    else
                    {
                        not_resume_flag = true;
                    }

                    if (isOutgoingCall)
                    {
                        isOutgoingCall = false;
                    }
                }
            }
        }

        if (mute != last_mute)
        {
            APP_PRINT_TRACE3("come here, g_teams_is_mute: %d, hold_flag: %d, isOutgoingCall: %d",
                             g_teams_is_mute, hold_flag, isOutgoingCall);
            if (off_hook)
            {
                app_hfp_isInCall = true;
            }
            else
            {
                app_hfp_isInCall = false;
            }
            if (mute)
            {
                g_teams_is_mute = true;
                if (isOutgoingCall)
                {
                    isOutgoingCall = false;
                }
                if (!hold_flag && off_hook)
                {
                    // TEAMS_STATUS_MUTE
                }
                else
                {
                }
                if (g_teams_is_mute)
                {
                    if (g_usb_sys_mic_is_mute == false)
                    {
                        app_usb_audio_volume_in_mute();
                        app_usb_audio_set_global_mute_status(g_teams_is_mute);
                    }
                }
            }
            else
            {
                if (!hold_flag && off_hook)
                {
                    //TEAMS_IN_CALL
                }
                else
                {
                }

                g_teams_is_mute = false;
                if (g_usb_sys_mic_is_mute == false)
                {
                    app_usb_audio_volume_in_unmute();
                    app_usb_audio_set_global_mute_status(g_teams_is_mute);
                }
            }
        }

        if (ring != last_ring)
        {
            if (ring)
            {
                if (call_count > 0)
                {
                    // send_call_terminate_status
                    // send_incoming_call_status
                    mute_status = g_teams_is_mute;
                    hold_flag = false;
                }

                isIncomingCall = true;
                pre_hook = off_hook;
                APP_PRINT_TRACE1("incoming call ring, off_hook: %d", off_hook);
            }
            else
            {
                if (off_hook)
                {
                    APP_PRINT_TRACE3("incoming call , app_hfp_isInCall: %d, pre_hook: %d, call_count: %d",
                                     app_hfp_isInCall, pre_hook, call_count);
                    if (app_hfp_isInCall && pre_hook)
                    {
                        // to do call back answer call send_call_answer_status
                        isIncomingCall = false;

                    }
                    else
                    {
                        if (call_count)
                        {
                            // to do call back answer call call_answer
                        }
                        else
                        {
                            // to do call back answer call send_call_answer_status
                        }
                        TELEPHONY_HID_INPUT_REPORT usb_teams_data;
                        usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
                        usb_teams_data.data[1] = 0;
                        usb_teams_data.report.offHook = 1;
                        app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                        if (call_count < 2)
                        {
                            call_count++;
                        }
                        app_hfp_isInCall = true;
                        isIncomingCall = false;
                    }
                }
                else
                {
                    APP_PRINT_TRACE3("incoming call rejected, call_count: %d, is_hold_call: %d, is_hold_mute:%d",
                                     call_count, is_hold_call, is_hold_mute);
                    // to do call back rejected call send_call_terminate_status
                    isIncomingCall = false;
                    if (!app_usb_audio_is_us_streaming())
                    {
                        // to do call back audio_disconnect_req
                    }

                    if (call_count == 0)
                    {
                        app_hfp_isInCall = false;
                    }
                    else
                    {
//                        curr_call_status = prev_call_status;
                    }
                }
            }
        }
        last_data = data[1];
    }
    else
    {
        if (off_hook == 0x01 && isOutgoingCall)
        {
            APP_PRINT_TRACE1("outgoing call answered, call_count: %d", call_count);
            if (call_count == 0)
            {
                // to do call back answer call call_answer
            }
            else
            {
                // to do call back answer call send_call_answer_status
            }
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            usb_teams_data.report.offHook = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));

            if ((false == g_teams_is_mute) && app_usb_audio_get_global_mute_status())
            {
                usb_teams_data.report.mute = 1;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            }

            if (call_count < 2)
            {
                call_count++;
            }
            app_hfp_isInCall = true;
            isOutgoingCall = false;
        }
        else if (data[1] == 0x01 && last_data == 0x01)
        {
            if ((false == g_teams_is_mute) && app_usb_audio_get_global_mute_status())
            {
                TELEPHONY_HID_INPUT_REPORT usb_teams_data;
                usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
                usb_teams_data.report.offHook = 1;
                usb_teams_data.report.mute = 1;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            }
        }
    }
    uint8_t teams_call_status = app_usb_teams_get_call_status();
    app_usb_teams_check_and_notify_call_status();

    APP_PRINT_TRACE5("isOutgoingCall:%d, isIncomingCall:%d, app_hfp_isInCall:%d, teams_is_mute:%d, teams_call_status:%d",
                     isOutgoingCall, isIncomingCall, app_hfp_isInCall, g_teams_is_mute, teams_call_status);
}

uint8_t app_usb_teams_is_hold(void)
{
    uint8_t hold_status = 0;

//    if (hold_count > 0)
    if ((hold_count > 0) && app_hfp_isInCall == false)
    {
        hold_status = 1;
    }
    return hold_status;
}

T_USB_TEAMS_CALL_STATUS app_usb_teams_get_call_status(void)
{
    T_USB_TEAMS_CALL_STATUS call_status = USB_TEAMS_CALL_IDLE;

    if (app_usb_audio_is_active())
    {
        if (app_hfp_isInCall && isIncomingCall)
        {
            call_status = USB_TEAMS_CALL_ACTIVE_WITH_CALL_WAITING;
        }
        else if ((hold_count > 0) && app_hfp_isInCall)
        {
            call_status = USB_TEAMS_CALL_ACTIVE_WITH_CALL_HOLD;
        }
        else if (app_hfp_isInCall)
        {
            call_status = USB_TEAMS_CALL_ACTIVE;
        }
        else if (isIncomingCall)
        {
            call_status = USB_TEAMS_INCOMING_CALL_ONGOING;
        }
        else if (isOutgoingCall)
        {
            call_status = USB_TEAMS_OUTGOING_CALL_ONGOING;
        }
        else
        {
            call_status = USB_TEAMS_CALL_IDLE;
        }
    }

    return call_status;
}

static void app_usb_teams_check_and_notify_call_status(void)
{
    uint8_t teams_call_status = app_usb_teams_get_call_status();
    if (s_last_teams_status != teams_call_status)
    {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        app_teams_multilink_voice_priority_rearrangment(multilink_usb_idx,
                                                        (T_APP_CALL_STATUS)teams_call_status);
#endif
    }
    s_last_teams_status = teams_call_status;
}

bool key_action_switch_to_usb_mode(uint8_t *action)
{
    bool key_handle = true;
    switch (*action)
    {
    case MMI_DEV_SPK_VOL_UP:
        *action = MMI_USB_AUDIO_VOL_UP;
        break;

    case MMI_DEV_SPK_VOL_DOWN:
        *action = MMI_USB_AUDIO_VOL_DOWN;
        break;

    case MMI_AV_PLAY_PAUSE:
        *action = MMI_USB_AUDIO_PLAY_PAUSE;
        break;

    case MMI_AV_STOP:
        *action = MMI_USB_AUDIO_STOP;
        break;

    case MMI_AV_FWD:
        *action = MMI_USB_AUDIO_FWD;
        break;

    case MMI_AV_BWD:
        *action = MMI_USB_AUDIO_BWD;
        break;

    case MMI_AV_FASTFORWARD:
        *action = MMI_USB_AUDIO_FASTFORWARD;
        break;

    case MMI_AV_REWIND:
        *action = MMI_USB_AUDIO_REWIND;
        break;

    case MMI_AV_FASTFORWARD_STOP:
        *action = MMI_USB_AUDIO_FASTFORWARD_STOP;
        break;

    case MMI_AV_REWIND_STOP:
        *action = MMI_USB_AUDIO_REWIND_STOP;
        break;

    case MMI_DEV_MIC_MUTE_UNMUTE:
        *action = MMI_USB_TEAMS_MIC_MUTE_UNMUTE;
        break;

    case MMI_DEV_MIC_MUTE:
        *action = MMI_USB_TEAMS_MIC_MUTE;
        break;

    case MMI_DEV_MIC_UNMUTE:
        *action = MMI_USB_TEAMS_MIC_UNMUTE;
        break;

    default:
        key_handle = false;
        break;
    }
    return key_handle;
}

void app_usb_mmi_handle_action(uint8_t action)
{
    APP_PRINT_TRACE2("app_usb_mmi_handle_action: action 0x%02x, teams_is_mute:0x%x", action,
                     g_teams_is_mute);
    switch (action)
    {
    case MMI_USB_AUDIO_VOL_UP:
    case MMI_USB_AUDIO_VOL_DOWN:
    case MMI_USB_AUDIO_PLAY_PAUSE:
    case MMI_USB_AUDIO_FWD:
    case MMI_USB_AUDIO_BWD:
        {
            uint8_t usb_audio_report_bit = action - MMI_USB_AUDIO_VOL_UP;
            uint8_t data1[2] = {USB_AUDIO_REPORT_TYPE_HID, (1 << usb_audio_report_bit)};
            uint8_t data2[2] = {USB_AUDIO_REPORT_TYPE_HID, 0x00};
            app_hid_interrupt_in(data1, sizeof(data1));
            app_hid_interrupt_in(data2, sizeof(data2));
        }
        break;

    case MMI_USB_MIC_VOL_UP:
        {
//            uint8_t data[2] = {0x09, 0x00};
        }
        break;
    case MMI_USB_MIC_VOL_DOWN:
        {
//            uint8_t data[2] = {0x01, 0x00};
        }
        break;
    case MMI_USB_TEAMS_ANSWER_CALL:
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            if ((app_teams_multilink_get_voice_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT) &&
                (app_teams_multilink_get_active_voice_index() == 0xff))
            {
                app_mmi_handle_action(action);
                return;
            }
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_asp_device_send_hook_button_status(0);
#endif
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.report.offHook = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        break;
    case MMI_USB_TEAMS_REJECT_CALL:
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            if ((app_teams_multilink_get_voice_status() == APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT) &&
                (app_teams_multilink_get_active_voice_index() == 0xff))
            {
                app_mmi_handle_action(action);
                return;
            }
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_asp_device_send_hook_button_status(1);
#endif
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.data[1] = 0;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.report.button1 = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            usb_teams_data.data[1] = 0;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        break;

    case MMI_USB_TEAMS_END_ACTIVE_CALL:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_asp_device_send_hook_button_status(1);
#endif
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            usb_teams_data.report.offHook = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            usb_teams_data.report.offHook = 0;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        break;
    case MMI_USB_TEAMS_CANCEL_VOICE_DIAL:
    case MMI_USB_TEAMS_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL:
        {
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        break;
    case MMI_USB_TEAMS_SWITCH_TO_SECOND_CALL:
        {
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            usb_teams_data.report.offHook = 1;
            usb_teams_data.report.flash = 1;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            usb_teams_data.report.offHook = 1;
            usb_teams_data.report.flash = 0;
            app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        }
        break;
    case MMI_USB_TEAMS_MIC_MUTE_UNMUTE:
        {
            APP_PRINT_TRACE1("app_usb_mmi_handle_action: call status: %d", app_usb_teams_get_call_status());
            if (app_usb_teams_get_call_status() == USB_TEAMS_CALL_IDLE)
            {
                if (app_usb_teams_is_hold())
                {
                    break;
                }
                if (app_usb_audio_get_global_mute_status() == false)
                {
                    app_usb_audio_volume_in_mute();
                    app_usb_audio_set_global_mute_status(true);
                    app_audio_tone_type_play(TONE_MIC_MUTE_ON, false, false);//mute vp
#if F_APP_TEAMS_FEATURE_SUPPORT
                    app_asp_device_send_mute_button_status(1);
#endif
                }
                else
                {
                    app_usb_audio_volume_in_unmute();
                    app_usb_audio_set_global_mute_status(false);
                    app_audio_tone_type_play(TONE_MIC_MUTE_OFF, false, false);//unmute vp
#if F_APP_TEAMS_FEATURE_SUPPORT
                    app_asp_device_send_mute_button_status(0);
#endif
                }
            }
            else
            {
                TELEPHONY_HID_INPUT_REPORT usb_teams_data;
                usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
                usb_teams_data.data[1] = 0;

                if (g_teams_is_mute == false)
                {
                    // set pc mute
                    usb_teams_data.report.offHook = 1;
                    usb_teams_data.report.mute = 1;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                    usb_teams_data.report.mute = 0;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));

                    app_audio_tone_type_play(TONE_MIC_MUTE_ON, false, false);//mute vp
#if F_APP_TEAMS_FEATURE_SUPPORT
                    app_asp_device_send_mute_button_status(1);
#endif
                }
                else
                {
                    // set pc unmute
                    usb_teams_data.report.offHook = 1;
                    usb_teams_data.report.mute = 0;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                    usb_teams_data.report.mute = 1;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                    usb_teams_data.report.mute = 0;
                    app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));

                    app_audio_tone_type_play(TONE_MIC_MUTE_OFF, false, false);//unmute vp
#if F_APP_TEAMS_FEATURE_SUPPORT
                    app_asp_device_send_mute_button_status(0);
#endif
                }
            }
        }
        break;
    case MMI_USB_TEAMS_MIC_MUTE:
        {
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            if (g_teams_is_mute == false)
            {
                // set pc mute
                usb_teams_data.report.offHook = 1;
                usb_teams_data.report.mute = 0;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                usb_teams_data.report.mute = 1;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            }
        }
        break;

    case MMI_USB_TEAMS_MIC_UNMUTE:
        {
            TELEPHONY_HID_INPUT_REPORT usb_teams_data;
            usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
            usb_teams_data.data[1] = 0;
            if (g_teams_is_mute == true)
            {
                // set pc unmute
                usb_teams_data.report.offHook = 1;
                usb_teams_data.report.mute = 1;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
                usb_teams_data.report.mute = 0;
                app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
            }
        }
        break;

    case MMI_USB_TEAMS_BUTTON_SHORT_PRESS:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_mmi_handle_action(action);
#endif
        }
        break;

    case MMI_USB_TEAMS_BUTTON_LONG_PRESS:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_mmi_handle_action(action);
#endif
        }
        break;

    case MMI_USB_ENTER_PAIRING_MODE:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_mmi_handle_action(action);
#endif
        }
        break;

    default:
        break;
    }
}

bool app_usb_audio_link_is_active(void)
{
    uint8_t teams_active_idx = USB_LINK_INDEX;
    bool usb_link_is_active = false;
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    teams_active_idx = app_teams_multilink_get_active_index();
#endif

    if (teams_active_idx == USB_LINK_INDEX)
    {
        usb_link_is_active = true;
    }
    else
    {
        usb_link_is_active = false;
    }
    return usb_link_is_active;
}

void app_usb_audio_set_global_mute_status(bool mute_status)
{
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    if (app_usb_audio_link_is_active())
    {
        app_teams_audio_set_global_mute_status(mute_status);
        g_usb_link_mic_is_mute = mute_status;
    }
#else
    g_usb_link_mic_is_mute = mute_status;
#endif
}

bool app_usb_audio_get_global_mute_status(void)
{
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    bool mute_status = false;
    if (app_usb_audio_link_is_active())
    {
        mute_status = g_usb_link_mic_is_mute | app_teams_audio_get_global_mute_status();
    }
    return mute_status;
#else
    return g_usb_link_mic_is_mute;
#endif
}

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
void app_usb_audio_sync_mute_status_handle(void)
{
    APP_PRINT_TRACE1("app_usb_audio_sync_mute_status_handle: call status: %d",
                     app_usb_teams_get_call_status());

    bool global_mute_status = app_teams_audio_get_global_mute_status();
    APP_PRINT_TRACE2("global_mute:%d, g_usb_link_mic_is_mute:%d", global_mute_status,
                     g_usb_link_mic_is_mute);

    if (app_usb_teams_get_call_status() >= USB_TEAMS_CALL_ACTIVE)
    {
        TELEPHONY_HID_INPUT_REPORT usb_teams_data;
        usb_teams_data.report.reportId = REPORT_ID_TELEPHONY_INPUT;
        usb_teams_data.data[1] = 0;

        // set pc mute
        usb_teams_data.report.offHook = 1;
        usb_teams_data.report.mute = 1;
        app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
        usb_teams_data.report.mute = 0;
        app_hid_interrupt_in((uint8_t *)&usb_teams_data, sizeof(usb_teams_data));
    }
    else if (app_usb_teams_get_call_status() == USB_TEAMS_CALL_IDLE)
    {
        if (app_usb_audio_get_global_mute_status() == true)
        {
            app_usb_audio_set_global_mute_status(true);
        }
    }
}
#endif

void app_usb_audio_hid_init(void)
{
    os_queue_init(&hid_in_queue);
    os_timer_create(&hid_in_timer_handle, "hid_in_timer", 0xfafa, 1, false, hid_in_timer_cb);
}

#endif
