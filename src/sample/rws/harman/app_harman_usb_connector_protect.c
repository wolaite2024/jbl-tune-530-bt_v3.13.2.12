#if HARMAN_USB_CONNECTOR_PROTECT
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "hal_gpio.h"
#include "pm.h"
#include "section.h"
#include "trace.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_io_msg.h"
#include "adc_manager.h"
#include "app_ext_charger.h"
#include "app_harman_usb_connector_protect.h"
#include "hal_adp.h"
#include "app_timer.h"

static uint8_t app_harman_usb_adc_timer_id = 0;
static uint8_t timer_idx_usb_ntc_period_check = 0;
static uint8_t timer_idx_usb_hiccup_check  = 0;

typedef enum
{
    TIMER_HARMAN_USB_ADC_TIMER_NTC_CHECK   = 0x00,
    TIMER_HARMAN_USB_HICCUP_CHECK          = 0x01,
} T_APP_HARMAN_USB_ADC_TIMER;

static bool is_usb_adc_mgr_init = false;
static uint8_t harman_usb_adc_chanel_index = 0 ;
static uint8_t harman_usb_adc_pin = HARMAN_USB_CONNECTOR_PROTECT_PIN;
static uint8_t harman_usb_hiccup_pin = HARMAN_USB_CONNECTOR_HICCUP_PIN;
static uint16_t usb_ntc_value = 0;
static uint16_t usb_hiccup_value = 0;
static T_APP_HARMAN_USB_CONNECTOR_STATUS usb_status = STATUS_NULL;

#define IGNORE_CHECK_USB_NTC_VALUE_TOTAL_TIME     6
static uint8_t ignore_check_usb_ntc = 0;

static bool usb_is_plug_in = false;

bool app_harman_usb_hiccup_check_timer_started(void)
{
    return (timer_idx_usb_hiccup_check != NULL);
}

static void app_harman_usb_hiccup_check_timer_stop(void)
{
    if (timer_idx_usb_hiccup_check != 0)

    {
        app_stop_timer(&timer_idx_usb_hiccup_check);
    }
}

static void app_harman_usb_hiccup_check_timer_start(uint32_t time)
{
    app_start_timer(&timer_idx_usb_hiccup_check, "usb_hiccup_check",
                    app_harman_usb_adc_timer_id, TIMER_HARMAN_USB_HICCUP_CHECK, 0, false,
                    time);
    app_timer_register_pm_excluded(&timer_idx_usb_hiccup_check);
}

void app_harman_usb_adc_ntc_check_timer_stop(void)
{
    if (timer_idx_usb_ntc_period_check != 0)
    {
        app_stop_timer(&timer_idx_usb_ntc_period_check);
    }
}

static void app_harman_usb_adc_ntc_check_timer_start(uint32_t time)
{
    app_harman_usb_adc_ntc_check_timer_stop();
    if (usb_is_plug_in)
    {
        app_start_timer(&timer_idx_usb_ntc_period_check, "usb_ntc_check",
                        app_harman_usb_adc_timer_id, TIMER_HARMAN_USB_ADC_TIMER_NTC_CHECK, 0, false,
                        time);
    }
}

/* NTC handle */
ISR_TEXT_SECTION
static void app_harman_usb_adc_interrupt_handler(void *pvPara, uint32_t int_status)
{
    uint16_t adc_data[2];

    adc_mgr_read_data_req(harman_usb_adc_chanel_index, adc_data, 0x3);
    usb_ntc_value = ADC_GetHighBypassRes(adc_data[0], EXT_SINGLE_ENDED(harman_usb_adc_pin));
    usb_hiccup_value = ADC_GetHighBypassRes(adc_data[1], EXT_SINGLE_ENDED(harman_usb_hiccup_pin));

    T_IO_MSG gpio_msg;
    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_USB_CONNECTOR_ADC_VALUE;

    app_io_msg_send(&gpio_msg);
}

void app_harman_usb_adc_io_read(void)
{
    uint8_t input_pin = harman_usb_adc_pin;
    uint8_t input_pin2 = harman_usb_hiccup_pin;
    if (!IS_ADC_CHANNEL(input_pin) || !IS_ADC_CHANNEL(input_pin2))
    {
        APP_PRINT_INFO2("app_harman_usb_adc_io_read: invalid ADC IO: 0x%x, 0x%x",
                        input_pin, input_pin2);
        return;
    }
    if (is_usb_adc_mgr_init)
    {
        APP_PRINT_INFO1("app_harman_usb_adc_io_read: ever registered %d",
                        harman_usb_adc_chanel_index);
        adc_mgr_enable_req(harman_usb_adc_chanel_index);
        return;
    }
    Pad_Config(input_pin, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_PowerOrShutDownValue(input_pin, 0);
    ADC_HighBypassCmd(input_pin, (FunctionalState)ENABLE);
    Pad_Config(input_pin2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_PowerOrShutDownValue(input_pin2, 0);
    ADC_HighBypassCmd(input_pin2, (FunctionalState)ENABLE);

    ignore_check_usb_ntc = IGNORE_CHECK_USB_NTC_VALUE_TOTAL_TIME;

    ADC_InitTypeDef ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.adcClock = ADC_CLK_39K;
    ADC_InitStruct.bitmap = 0x3;
    ADC_InitStruct.schIndex[0] = EXT_SINGLE_ENDED(input_pin);
    ADC_InitStruct.schIndex[1] = EXT_SINGLE_ENDED(input_pin2);
    if (!adc_mgr_register_req(&ADC_InitStruct,
                              (adc_callback_function_t)app_harman_usb_adc_interrupt_handler,
                              &harman_usb_adc_chanel_index))
    {
        APP_PRINT_INFO0("app_harman_usb_adc_io_read: ADC Register Request Fail");
        return;
    }
    else
    {
        is_usb_adc_mgr_init = true;
    }
    adc_mgr_enable_req(harman_usb_adc_chanel_index);
    APP_PRINT_TRACE1("app_harman_usb_adc_io_read: %d", harman_usb_adc_chanel_index);
    return;
}

static void app_harman_usb_connector_mosfet_open(void)
{
    Pad_Config(HARMAN_USB_CONNECTOR_MOSFET_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
}

static void app_harman_usb_connector_mosfet_close(void)
{
    Pad_Config(HARMAN_USB_CONNECTOR_MOSFET_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_LOW);
}

static void app_harman_usb_adc_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("app_harman_usb_adc_timeout_cb: timer_id 0x%02x, timer_chann %d ",
                     timer_id, timer_chann);
    switch (timer_id)
    {
    case TIMER_HARMAN_USB_ADC_TIMER_NTC_CHECK:
        {
            app_harman_usb_adc_ntc_check_timer_stop();
            app_harman_usb_adc_io_read();
            app_harman_usb_adc_ntc_check_timer_start(USB_CONNECTOR_NTC_CHECK_PERIOD);
        }
        break;

    case TIMER_HARMAN_USB_HICCUP_CHECK:
        {
            APP_PRINT_INFO4("app_harman_usb_adc_timeout_cb: usb_ntc_value: %d, usb_hiccup_value: %d, "
                            "usb_status: %d, usb_connector_protection_enabled: %d",
                            usb_ntc_value, usb_hiccup_value, usb_status,
                            app_cfg_nv.usb_connector_protection_enabled);
            if ((!app_cfg_nv.usb_connector_protection_enabled) ||
                (usb_ntc_value >= USB_PROTECT_NTC_THRESHOLD_LOW) ||
                (((usb_ntc_value < (USB_PROTECT_NTC_THRESHOLD_LOW)) &&
                  (usb_ntc_value > (USB_PROTECT_NTC_THRESHOLD_MID)))
                 || (usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_HIGH)))
            {
                if (usb_hiccup_value < 20)
                {
                    usb_is_plug_in = false;
                    app_harman_usb_adc_ntc_check_timer_stop();
                    app_harman_usb_hiccup_check_timer_stop();
                    app_harman_usb_connector_protect_nv_clear();
                    app_harman_usb_connector_mosfet_close();
                    usb_status = STATUS_NULL;
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_harman_usb_connector_protect_nv_set(void)
{
    app_cfg_nv.usb_connector_protection_enabled = 0x01;
    app_cfg_store(&app_cfg_nv.language_version, 4);
}

void app_harman_usb_connector_protect_nv_clear(void)
{
    app_cfg_nv.usb_connector_protection_enabled = 0x00;
    app_cfg_store(&app_cfg_nv.language_version, 4);
}

void app_harman_usb_connector_adc_update(void)
{

    T_ADP_STATE adp_state = adp_get_current_state(ADP_DETECT_IO);

    if (ignore_check_usb_ntc)
    {
        APP_PRINT_INFO4("app_harman_usb_connector_adc_update: usb_ntc_value: %d, "
                        "usb_hiccup_value: %d, ignore_check_usb_ntc: %d adp_state %d",
                        usb_ntc_value, usb_hiccup_value, ignore_check_usb_ntc, adp_state);
        ignore_check_usb_ntc --;
        return;
    }

    // <=34°
    if (usb_ntc_value >= USB_PROTECT_NTC_THRESHOLD_LOW)
    {
        app_harman_usb_connector_protect_nv_clear();

        usb_status = STATUS_ENABLE_CHARGER_CLOSE_MOSFET;
    }
    // >34° && <=57°
    else if ((usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_LOW) &&
             (usb_ntc_value >= USB_PROTECT_NTC_THRESHOLD_MID))
    {
        // TODO: >34° && <=57° && adp out => clear NV
        if (adp_state == ADP_STATE_OUT)
        {
            if (timer_idx_usb_hiccup_check == 0)
            {
                app_harman_usb_hiccup_check_timer_start(USB_CONNECTOR_HICCUP_CHECK_TIME);
            }
        }

        if (app_cfg_nv.usb_connector_protection_enabled)
        {
            usb_status = STATUS_DISABLE_CHARGER_OPEN_MOSFET;
        }
        else
        {
            usb_status = STATUS_ENABLE_CHARGER_CLOSE_MOSFET;
        }
    }
    // >57°
    else // if (usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_MID)
    {
        app_harman_usb_connector_protect_nv_set();

        //  >57° && <=67°
        if ((usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_MID) &&
            (usb_ntc_value >= USB_PROTECT_NTC_THRESHOLD_HIGH))
        {
            if (usb_hiccup_value >= USB_CONNECTOR_HICCUP_VALUE)
            {
                usb_status = STATUS_DISABLE_CHARGER_OPEN_MOSFET;
            }
            else
            {
                usb_status = STATUS_DISABLE_CHARGER_CLOSE_MOSFET;
            }
        }
        //  >67°
        else // if (usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_HIGH)
        {
            // TODO: >67° && adp out => clear NV
            if (adp_state == ADP_STATE_OUT)
            {
                app_harman_usb_connector_protect_nv_clear();
            }

            usb_status = STATUS_DISABLE_CHARGER_CLOSE_MOSFET;
        }
    }

    if ((usb_status == STATUS_DISABLE_CHARGER_CLOSE_MOSFET) ||
        (usb_status == STATUS_ENABLE_CHARGER_CLOSE_MOSFET))
    {
        // close mosfet
        app_harman_usb_connector_mosfet_close();
    }
    else
    {
        // check hiccup
        // open mosfet
        app_harman_usb_connector_mosfet_open();
        app_harman_usb_adc_ntc_check_timer_start(USB_CONNECTOR_NTC_CHECK_PERIOD);
    }

    // start charger NTC check
#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
    app_ext_charger_ntc_adc_update();
#endif
    APP_PRINT_INFO4("app_harman_usb_connector_adc_update: usb_ntc_value: %d, usb_hiccup_value: %d, "
                    "usb_status: %d, usb_connector_protection_enabled: %d",
                    usb_ntc_value, usb_hiccup_value, usb_status,
                    app_cfg_nv.usb_connector_protection_enabled);
}

bool app_harman_usb_connector_detect_is_normal(void)
{
    bool ret = true;

    if ((usb_status == STATUS_DISABLE_CHARGER_CLOSE_MOSFET) ||
        (usb_status == STATUS_DISABLE_CHARGER_OPEN_MOSFET) ||
        (app_cfg_nv.usb_connector_protection_enabled))
    {
        ret = false;
    }

    return ret;
}

void app_harman_usb_connector_adp_in_handle(void)
{
    usb_is_plug_in = true;
    app_harman_usb_adc_io_read();
    app_harman_usb_adc_ntc_check_timer_start(USB_CONNECTOR_NTC_CHECK_PERIOD);
}

void app_harman_usb_connector_adp_out_handle(void)
{
    APP_PRINT_INFO4("app_harman_usb_connector_adp_out_handle: usb_ntc_value: %d, usb_hiccup_value: %d, "
                    "usb_status: %d, usb_connector_protection_enabled: %d",
                    usb_ntc_value, usb_hiccup_value, usb_status,
                    app_cfg_nv.usb_connector_protection_enabled);
    // TODO: ()>34° && <=57° || >67°) && adp out => clear NV
    if ((!app_cfg_nv.usb_connector_protection_enabled) ||
        (usb_ntc_value >= USB_PROTECT_NTC_THRESHOLD_LOW) ||
        (((usb_ntc_value < (USB_PROTECT_NTC_THRESHOLD_LOW)) &&
          (usb_ntc_value > (USB_PROTECT_NTC_THRESHOLD_MID)))
         || (usb_ntc_value < USB_PROTECT_NTC_THRESHOLD_HIGH)))
    {
        app_harman_usb_hiccup_check_timer_start(USB_CONNECTOR_HICCUP_CHECK_TIME);
    }
}

void app_harman_usb_connector_protect_init(void)
{
    app_timer_reg_cb(app_harman_usb_adc_timeout_cb, &app_harman_usb_adc_timer_id);
    hal_gpio_init_pin(HARMAN_USB_CONNECTOR_HICCUP_PIN, GPIO_TYPE_AUTO, GPIO_DIR_INPUT, GPIO_PULL_UP);
    app_harman_usb_connector_mosfet_close();
}
#endif
