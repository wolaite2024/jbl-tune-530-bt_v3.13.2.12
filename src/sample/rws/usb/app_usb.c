#if F_APP_USB_AUDIO_SUPPORT|F_APP_USB_HID_SUPPORT
#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "os_msg.h"
#include "os_task.h"
#include "section.h"
#include "sysm.h"
#include "hal_adp.h"
#include "pm.h"
#include "system_status_api.h"
#include "usb_hid.h"
#include "usb_audio_stream.h"
#include "usb_dev.h"
#include "usb_dm.h"
#include "usb_msg.h"
#include "app_auto_power_off.h"
#include "app_cfg.h"
#include "app_device.h"
#include "app_io_msg.h"
#include "app_usb.h"
#include "app_usb_audio.h"
#include "app_usb_mmi.h"
#include "app_device.h"
#include "app_main.h"
#include "app_mmi.h"
#include "app_ipc.h"
#include "app_device.h"
#include "app_dlps.h"
#include "app_usb_hid_cmd.h"
#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#include "pmu_api.h"
#endif

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#define     CPU_FREQ_MHZ_MAX    160
#else
#define     CPU_FREQ_MHZ_MAX    100
#endif
typedef enum
{
    USB_EVT_PLUG,
    USB_EVT_UNPLUG,
    USB_EVT_PWR_STATUS_CHG,
    USB_EVT_BC12_DETECT,
} T_USB_EVT;

#define USB_DM_EVT_Q_NUM     0x20

#define USB_DM_EVT_USB_START 0x01
#define USB_DM_EVT_USB_STOP  0x02

#define READY_BIT_INIT       (0x01 << 0)
#define READY_BIT_PLUGGED    (0x01 << 1)
#define READY_BIT_POWER_ON   (0x01 << 2)
#define READY_BIT_ALL        (READY_BIT_INIT|READY_BIT_PLUGGED)


typedef struct _store_param
{
    uint32_t pre_cpu_clk;
    POWERMode pre_pwr_mode;
} T_STORE_PARAM;

typedef struct _app_usb_db
{
    T_STORE_PARAM store;
    uint8_t ready_set_to_start;
    T_USB_POWER_STATE usb_pwr_state;
    void *usb_dm_task;
    void *usb_dm_evt_q_handle;
    bool power_on_when_usb_plug;
} T_APP_USB_DB;

static T_APP_USB_DB app_usb_db;
static uint8_t app_usb_freq_handle = 0;
static bool app_usb_dev_trigger_evt(T_USB_EVT evt,  uint32_t param);

static void app_usb_set_hp_mode(void)
{
    uint32_t actual_mhz = 0;
    sys_hall_auto_sleep_in_idle(false);
    app_usb_db.store.pre_cpu_clk = pm_cpu_freq_get();
    pm_cpu_freq_req(&app_usb_freq_handle, pm_cpu_max_freq_get(), &actual_mhz);
    pm_dvfs_set_supreme();
    APP_PRINT_INFO2("app_usb_start, pre clk:%d, cur clk:%d", app_usb_db.store.pre_cpu_clk,  actual_mhz);
}

static void app_usb_set_lp_mode(void)
{
    uint32_t actual_mhz = 0;
    sys_hall_auto_sleep_in_idle(true);
    pm_cpu_freq_clear(&app_usb_freq_handle, &actual_mhz);
}

static void app_usb_start(void)
{
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_usb_set_hp_mode();
        app_auto_power_off_disable(AUTO_POWER_OFF_MASK_USB_AUDIO_MODE);
        app_dlps_disable(APP_DLPS_ENTER_CHECK_USB);
#if (TARGET_RTL8773DO == 1)
        pmu_vcore3_pon_domain_enable(PMU_USB);
        usb_dm_start(false);
#else
        usb_dm_start(true);
#endif
        app_ipc_publish(USB_IPC_TOPIC, USB_IPC_EVT_PLUG, NULL);
    }
}

static void app_usb_stop(void)
{
    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_USB_AUDIO_MODE, app_cfg_const.timer_auto_power_off);
        usb_dm_stop();
        app_dlps_enable(APP_DLPS_ENTER_CHECK_USB);
#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
        pmu_vcore3_pon_domain_disable(PMU_USB);
#endif

        app_usb_set_lp_mode();
        app_ipc_publish(USB_IPC_TOPIC, USB_IPC_EVT_UNPLUG, NULL);
    }
}

RAM_TEXT_SECTION
static bool app_usb_dev_trigger_evt(T_USB_EVT evt,  uint32_t param)
{
    T_IO_MSG gpio_msg;

    gpio_msg.type = IO_MSG_TYPE_USB;
    gpio_msg.subtype = USB_MSG(USB_MSG_GROUP_DEV, evt);
    gpio_msg.u.param = param;

    return app_io_msg_send(&gpio_msg);
}

static bool app_usb_dm_cb(T_USB_DM_EVT evt, T_USB_DM_EVT_PARAM *param)
{
    static T_USB_POWER_STATE s_usb_pwr_state = USB_PDN;
    if (evt == USB_DM_EVT_STATUS_IND)
    {
        T_USB_DM_EVT_PARAM_STATUS_IND *status_ind = (T_USB_DM_EVT_PARAM_STATUS_IND *)param;
        T_USB_POWER_STATE usb_pwr_state = status_ind->state;
        if ((s_usb_pwr_state == USB_SUSPENDED) && \
            (usb_pwr_state >= USB_POWERED))
        {
            //resume interrupt handle to do
            app_usb_set_hp_mode();
        }
        s_usb_pwr_state = usb_pwr_state;
        app_usb_dev_trigger_evt(USB_EVT_PWR_STATUS_CHG, usb_pwr_state);
    }
    else if (evt == USB_DM_EVT_BC12_DETECT)
    {
        T_USB_DM_EVT_PARAM_BC12_DET *bc12_det = (T_USB_DM_EVT_PARAM_BC12_DET *)param;
        uint8_t bc12_type = bc12_det->type;
        app_usb_dev_trigger_evt(USB_EVT_BC12_DETECT, bc12_type);
    }
    return true;
}

static void app_usb_start_ready_bit_set(uint8_t ready_bit)
{
    if (ready_bit & READY_BIT_ALL)
    {
        app_usb_db.ready_set_to_start |= ready_bit;
    }
    else
    {
        return;
    }

    APP_PRINT_INFO2("usb_start_ready_bit_set:0x%x-0x%x", app_usb_db.ready_set_to_start, ready_bit);
    if (app_usb_db.ready_set_to_start == READY_BIT_ALL)
    {
        app_usb_start();
    }
}

static void app_usb_start_ready_bit_clear(uint8_t clear_bit)
{
    if (clear_bit & READY_BIT_ALL)
    {
        if (app_usb_db.ready_set_to_start == READY_BIT_ALL)
        {
            app_usb_stop();
        }
        app_usb_db.ready_set_to_start &= ~clear_bit;
    }
    APP_PRINT_INFO2("usb_start_ready_bit_clear:0x%x-0x%x", app_usb_db.ready_set_to_start, clear_bit);
}

#if F_APP_USB_SUSPEND_SUPPORT
static void app_usb_dm_evt_suspend_handle(void)
{
    app_ipc_publish(USB_IPC_TOPIC, USB_IPC_EVT_SUSPEND, NULL);
    app_usb_set_lp_mode();
}

static void app_usb_dm_evt_resume_handle(void)
{
    app_ipc_publish(USB_IPC_TOPIC, USB_IPC_EVT_RESUME, NULL);
}
#endif

static void app_usb_dm_evt_handle(uint8_t evt, uint32_t data)
{
    switch (evt)
    {
    case USB_EVT_PLUG:
        {
            app_usb_start_ready_bit_set(READY_BIT_PLUGGED);

            if (app_usb_db.power_on_when_usb_plug && app_db.device_state != APP_DEVICE_STATE_ON)
            {
                if (app_db.bt_is_ready && app_db.ble_is_ready)
                {
                    app_mmi_handle_action(MMI_DEV_POWER_ON);
                }
            }
        }
        break;

    case USB_EVT_UNPLUG:
        {
            app_usb_start_ready_bit_clear(READY_BIT_PLUGGED);

        }
        break;

    case USB_EVT_BC12_DETECT:
        {
            APP_PRINT_INFO1("app_usb_dev_trigger_evt, bc12_type 0x%x", data);
        }
        break;

    case USB_EVT_PWR_STATUS_CHG:
        {
            T_USB_POWER_STATE usb_pwr_state = (T_USB_POWER_STATE)data;
            APP_PRINT_INFO2("app_usb_dev_trigger_evt, old:0x%x, usb_pwr_state:0x%x", app_usb_db.usb_pwr_state,
                            usb_pwr_state);
#if F_APP_USB_SUSPEND_SUPPORT
            switch (usb_pwr_state)
            {
            case USB_SUSPENDED:
                {
                    app_usb_dm_evt_suspend_handle();
                }
                break;

            case USB_POWERED:
            case USB_DEFAULT:
            case USB_ADDRESSED:
            case USB_CONFIGURED:
                {
                    if (app_usb_db.usb_pwr_state == USB_SUSPENDED)
                    {
                        app_usb_dm_evt_resume_handle();
                    }
                }
                break;

            default:
                break;
            }
#endif

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
            if (usb_pwr_state == USB_SUSPENDED)
            {
                app_dlps_enable(APP_DLPS_ENTER_CHECK_USB);
            }
            else if (usb_pwr_state > USB_PDN)
            {
                app_dlps_disable(APP_DLPS_ENTER_CHECK_USB);
            }
#endif
            app_usb_db.usb_pwr_state = usb_pwr_state;
        }
        break;

    default:
        break;
    }
}

static void app_usb_sysm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
            app_usb_start_ready_bit_clear(READY_BIT_POWER_ON);
        }
        break;

    case SYS_EVENT_POWER_ON:
        {
            app_usb_start_ready_bit_set(READY_BIT_POWER_ON);
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
        APP_PRINT_INFO1("app_usb_sysm_cback: event_type 0x%04x", event_type);
    }
}

static void app_usb_device_event_cback(uint32_t event, void *msg)
{
    if (msg)
    {
        bool *need_power_on = (bool *)msg;
        switch (event)
        {
        case APP_DEVICE_IPC_EVT_STACK_READY:
            {
                if (app_usb_db.power_on_when_usb_plug)
                {
                    if (app_usb_db.ready_set_to_start & READY_BIT_PLUGGED)
                    {
                        *need_power_on = true;
                    }
                }
                APP_PRINT_INFO2("app_usb_device_event_cback, power_on_when_usb_plug:%d, need_power_on:%d",
                                app_usb_db.power_on_when_usb_plug, *need_power_on);
            }
            break;

        default:
            break;
        }
    }

}

bool app_usb_connected(void)
{
    return (app_usb_db.usb_pwr_state > USB_PDN);
}

static void app_usb_adp_state_change_cb(T_ADP_PLUG_EVENT event, void *user_data)
{
    if (event == ADP_EVENT_PLUG_IN)
    {
        app_usb_dev_trigger_evt(USB_EVT_PLUG, 0);
    }
    else if (event == ADP_EVENT_PLUG_OUT)
    {
        app_usb_dev_trigger_evt(USB_EVT_UNPLUG, 0);
    }

}

void app_usb_msg_handle(T_IO_MSG *msg)
{
    uint16_t subtype = msg->subtype;
    uint8_t group = USB_MSG_GROUP(subtype);
    uint8_t submsg = USB_MSG_SUBTYPE(subtype);
    uint32_t param = msg->u.param;

    switch (group)
    {
    case USB_MSG_GROUP_DEV:
        {
            app_usb_dm_evt_handle(submsg, param);
        }
        break;
#if F_APP_USB_AUDIO_SUPPORT
    case USB_MSG_GROUP_IF_AUDIO:
        {
            usb_audio_stream_evt_handle(submsg, param);
        }
        break;
#endif
    default:
        break;
    }
}

bool app_usb_dvfs_check(void)
{
    return (app_usb_db.usb_pwr_state == USB_PDN || app_usb_db.usb_pwr_state == USB_SUSPENDED);
}

void app_usb_init(void)
{
    memset(&app_usb_db, 0, sizeof(T_APP_USB_DB));

    usb_dm_cb_register(app_usb_dm_cb);
    pm_dvfs_register_check_func(app_usb_dvfs_check);
    adp_register_state_change_cb(ADP_DETECT_5V, (P_ADP_PLUG_CBACK)app_usb_adp_state_change_cb, NULL);
    sys_mgr_cback_register(app_usb_sysm_cback);
    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_usb_device_event_cback);

    T_USB_CORE_CONFIG config = {.speed = app_cfg2_const.usb_config.speed, .class_set = {.hid_enable = 0, .uac_enable = 0}};
#if F_APP_USB_AUDIO_SUPPORT
    config.class_set.uac_enable = app_cfg2_const.usb_config.audio_class_en;
#endif
#if F_APP_USB_HID_SUPPORT
    config.class_set.hid_enable = app_cfg2_const.usb_config.hid_class_en;
#endif
    usb_dm_core_init(config);

    usb_dev_init();

#if F_APP_USB_AUDIO_SUPPORT
    //if (app_cfg2_const.usb_config.audio_class_en)
    {
        usb_audio_stream_init();
        app_usb_audio_init();
    }
#endif
#if F_APP_USB_HID_SUPPORT
    //if (app_cfg2_const.usb_config.hid_class_en)
    {
        app_usb_mmi_init();
        usb_hid_init();
        app_usb_hid_cmd_init();
    }
#endif

    app_usb_start_ready_bit_set(READY_BIT_INIT);
}
#endif
