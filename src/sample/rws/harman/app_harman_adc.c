#if HARMAN_NTC_DETECT_PROTECT || HARMAN_DISCHARGER_NTC_DETECT_PROTECT
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "hal_gpio.h"
#include "section.h"
#include "trace.h"
#include "os_sched.h"
#include "single_tone.h"
#include "app_main.h"
#include "app_dlps.h"
#include "app_cfg.h"
#include "app_io_msg.h"
#include "adc_manager.h"
#include "app_mmi.h"
#include "app_harman_adc.h"
#include "app_ext_charger.h"
#include "app_timer.h"

#if GFPS_FINDER_SUPPORT
#include "app_gfps_finder.h"
#endif

static uint8_t app_harman_adc_timer_id = 0;
static uint8_t timer_idx_ntc_period_check = 0;

typedef enum
{
    TIMER_HARMAN_ADC_TIMER_NTC_CHECK,
} T_APP_HARMAN_ADC_TIMER;

static bool is_adc_mgr_init = false;
static uint8_t harman_adc_chanel_index = 0 ;
static uint8_t harman_adc_pin = HARMAN_THERMISTOR_PIN;

#if HARMAN_SECOND_NTC_DETECT_PROTECT
static uint8_t harman_adc_pin2 = HARMAN_THERMISTOR_PIN2;
#endif

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
static uint8_t vbat_adc_check_times = 0;
static int32_t adc_2_battery_sum = 0;
static int32_t adc_3_battery_sum = 0;
#endif

typedef struct
{
    uint16_t adc_2_battery;
    uint16_t adc_3_battery;
    uint16_t voltage_battery;
} APP_HARMAN_ADC_DATA_MGR;

static APP_HARMAN_ADC_DATA_MGR adc_data_mgr = {0};

static uint16_t total_check_times = 0;
static uint8_t need_power_off = 0;

static bool ignore_first_adc_value = false;

/* NTC handle */
ISR_TEXT_SECTION
static void app_harman_adc_interrupt_handler(void *pvPara, uint32_t int_status)
{
    uint16_t adc_data[3];
    T_IO_MSG gpio_msg;

    adc_mgr_read_data_req(harman_adc_chanel_index, adc_data, 0x7);
    adc_data_mgr.adc_2_battery = ADC_GetHighBypassRes(adc_data[0], EXT_SINGLE_ENDED(harman_adc_pin));
#if HARMAN_SECOND_NTC_DETECT_PROTECT
    adc_data_mgr.adc_3_battery = ADC_GetHighBypassRes(adc_data[1], EXT_SINGLE_ENDED(harman_adc_pin2));
#endif
    adc_data_mgr.voltage_battery = ADC_GetRes(adc_data[2], INTERNAL_VBAT_MODE);

    gpio_msg.type = IO_MSG_TYPE_GPIO;

    gpio_msg.subtype = IO_MSG_GPIO_EXT_CHARGER_ADC_VALUE;

    app_io_msg_send(&gpio_msg);
}

void app_harman_adc_io_read(void)
{
    uint8_t input_pin = harman_adc_pin;

    if (!IS_ADC_CHANNEL(input_pin))
    {
        APP_PRINT_ERROR1("app_harman_adc_io_read: invalid ADC IO: 0x%x", input_pin);
        return;
    }

#if HARMAN_SECOND_NTC_DETECT_PROTECT
    uint8_t input_pin2 = harman_adc_pin2;

    if (!IS_ADC_CHANNEL(input_pin))
    {
        APP_PRINT_ERROR1("app_harman_adc_io_read: invalid ADC IO: 0x%x", input_pin2);
        return;
    }
#endif

    if (is_adc_mgr_init)
    {
        APP_PRINT_INFO1("app_harman_adc_io_read: ever registered %d",
                        harman_adc_chanel_index);
        adc_mgr_enable_req(harman_adc_chanel_index);
        return;
    }

    Pad_Config(input_pin, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_PowerOrShutDownValue(input_pin, 0);
    ADC_HighBypassCmd(input_pin, (FunctionalState)ENABLE);

#if HARMAN_SECOND_NTC_DETECT_PROTECT
    Pad_Config(input_pin2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_PowerOrShutDownValue(input_pin2, 0);
    ADC_HighBypassCmd(input_pin2, (FunctionalState)ENABLE);
#endif

    ignore_first_adc_value = true;

    ADC_InitTypeDef ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.adcClock = ADC_CLK_39K;
    ADC_InitStruct.bitmap = 0x7;
    ADC_InitStruct.schIndex[0] = EXT_SINGLE_ENDED(input_pin);
#if HARMAN_SECOND_NTC_DETECT_PROTECT
    ADC_InitStruct.schIndex[1] = EXT_SINGLE_ENDED(input_pin2);
#endif
    ADC_InitStruct.schIndex[2] = INTERNAL_VBAT_MODE;

    if (!adc_mgr_register_req(&ADC_InitStruct,
                              (adc_callback_function_t)app_harman_adc_interrupt_handler,
                              &harman_adc_chanel_index))
    {
        APP_PRINT_INFO0("app_harman_adc_io_read: ADC Register Request Fail");
        return;
    }
    else
    {
        is_adc_mgr_init = true;
    }

    adc_mgr_enable_req(harman_adc_chanel_index);

    APP_PRINT_TRACE1("app_harman_adc_io_read: %d", harman_adc_chanel_index);

    return;
}

uint16_t app_harman_adc_voltage_battery_get(void)
{
    return adc_data_mgr.voltage_battery;
}

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
void app_harman_adc_vbat_check_times_clear(void)
{
    vbat_adc_check_times = 0;
    app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_ING);
}

static void app_harman_adc_vbat_detect(uint16_t check_time, uint16_t max_discrepancy_value) // ms
{
    if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_ING)
    {
        total_check_times = check_time / DISCHARGER_NTC_CHECK_PERIOD;

        if (vbat_adc_check_times <= total_check_times)
        {
            vbat_adc_check_times ++;

            adc_2_battery_sum += adc_data_mgr.adc_2_battery;
            adc_3_battery_sum += adc_data_mgr.adc_3_battery;
            if (vbat_adc_check_times == total_check_times)
            {
                uint16_t discrepancy_value = 0;

                adc_2_battery_sum = adc_2_battery_sum / total_check_times;
                adc_3_battery_sum = adc_3_battery_sum / total_check_times;
                discrepancy_value = abs(adc_3_battery_sum - adc_2_battery_sum);

#if HARMAN_VBAT_ADC_DETECTION
                max_discrepancy_value = app_cfg_nv.vbat_ntc_value * 0.1; // 10%
                discrepancy_value = abs(adc_2_battery_sum - app_cfg_nv.vbat_ntc_value);
                if (app_cfg_nv.vbat_detect_normal == true &&
                    discrepancy_value < max_discrepancy_value)
                {
                    app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_SUCCESS);

                    app_cfg_nv.vbat_ntc_value = adc_data_mgr.adc_2_battery;
                    app_cfg_store(&app_cfg_nv.vbat_ntc_value, 4);
                }
                else
                {
                    app_cfg_nv.vbat_detect_normal = false;
                    app_cfg_store(&app_cfg_nv.harman_category_id, 4);
                    app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_FAIL);

                    // stop charging and discharger NTC protect
                    app_harman_adc_ntc_check_timer_stop();
                }

#else
                if (discrepancy_value <= max_discrepancy_value)
                {
                    app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_SUCCESS);
                }
                else
                {
                    app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_FAIL);
                }
#endif

                app_dlps_enable(APP_DLPS_ENTER_VBAT_VALUE_UPDATE);
                APP_PRINT_INFO3("app_harman_adc_vbat_detect: discrepancy_value: %d, max_discrepancy_value: %d, vbat_detect_normal %d",
                                discrepancy_value, max_discrepancy_value, app_cfg_nv.vbat_detect_normal);
            }
        }
    }
    APP_PRINT_INFO3("app_harman_adc_vbat_detect: vbat_adc_check_times: %d, total_check_times: %d, vbat_is_normal: %d",
                    vbat_adc_check_times, total_check_times, app_ext_charger_vbat_is_normal_get());
}
#endif

void app_harman_adc_update_cur_ntc_value(uint16_t *p_ntc_value)
{
#if HARMAN_SECOND_NTC_DETECT_PROTECT
    if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_SUCCESS)
    {
        *p_ntc_value = adc_data_mgr.adc_2_battery;
    }
    else if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_FAIL)
    {
        *p_ntc_value = adc_data_mgr.adc_3_battery;
    }
#else
    *p_ntc_value = adc_data_mgr.adc_2_battery;
#if HARMAN_VBAT_ADC_DETECTION
    if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_SUCCESS)
    {
        app_cfg_nv.vbat_ntc_value = adc_data_mgr.adc_2_battery;
        app_cfg_store(&app_cfg_nv.vbat_ntc_value, 4);
    }
#endif
#endif
    APP_PRINT_TRACE6("app_harman_adc_update_cur_ntc_value: cur_ntc_value: %d, adc_2_battery: %d, "
                     "adc_3_battery: %d, VBAT: %d,vbat_is_normal: %d, vbat_detect_status: %d",
                     *p_ntc_value, adc_data_mgr.adc_2_battery,
                     adc_data_mgr.adc_3_battery, adc_data_mgr.voltage_battery,
                     app_ext_charger_vbat_is_normal_get(), app_cfg_nv.vbat_detect_status);
}

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
bool app_harman_discharger_ntc_check_valid(void)
{
    bool ret = false;
    uint16_t ntc_value = 0;

    if (!app_cfg_const.discharge_ntc_protection_thermistor_option || mp_hci_test_mode_is_running())
    {
        ret = true;
    }
    else if ((app_cfg_const.discharge_ntc_protection_thermistor_option) &&
             (app_db.device_state != APP_DEVICE_STATE_ON))
    {
        app_harman_adc_update_cur_ntc_value(&ntc_value);
        if ((ntc_value >= app_cfg_const.high_temperature_protect_value) &&
            (ntc_value <= app_cfg_const.low_temperature_protect_value))
        {
            ret = true;
        }
    }
    APP_PRINT_TRACE3("app_harman_discharger_ntc_check_valid: %d, ntc_value: %d, device_state: %d",
                     ret, ntc_value, app_db.device_state);
    return ret;
}

void app_harman_discharger_adc_update(void)
{
    uint16_t ntc_value = 0;

    app_harman_adc_update_cur_ntc_value(&ntc_value);
    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        if ((ntc_value < app_cfg_const.high_temperature_protect_value) ||
            (ntc_value > app_cfg_const.low_temperature_protect_value))
        {
            if (++ need_power_off == 5)
            {
                need_power_off = 0;
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
        }
        else
        {
            need_power_off = 0;
        }
    }
    else if ((app_db.device_state == APP_DEVICE_STATE_OFF) &&
             app_db.need_delay_power_on)
    {
        app_db.need_delay_power_on = false;
        app_dlps_enable(APP_DLPS_ENTER_VBAT_VALUE_UPDATE);
        if (app_harman_discharger_ntc_check_valid())
        {
            app_mmi_handle_action(MMI_DEV_POWER_ON);
        }
    }
    APP_PRINT_TRACE3("app_harman_discharger_adc_update: ntc_value: %d, device_state: %d, need_power_off: %d",
                     ntc_value, app_db.device_state, need_power_off);
}

void app_harman_discharger_ntc_timer_start(void)
{
    if (app_cfg_const.discharge_ntc_protection_thermistor_option)
    {
        app_harman_adc_io_read();
        app_harman_adc_ntc_check_timer_start(DISCHARGER_NTC_CHECK_PERIOD);
    }
}
#endif

void app_harman_adc_msg_handle(void)
{
    if (ignore_first_adc_value)
    {
        ignore_first_adc_value = false;
        return;
    }

#if HARMAN_VBAT_ADC_DETECTION
    if (app_cfg_nv.store_adc_ntc_value_when_vbat_in)
    {
        app_cfg_nv.store_adc_ntc_value_when_vbat_in = 0;
        app_cfg_store(&app_cfg_nv.offset_is_dut_test_mode, 4);

        app_cfg_nv.vbat_detect_normal = 1;
        app_cfg_store(&app_cfg_nv.harman_category_id, 4);

        app_cfg_nv.vbat_ntc_value = adc_data_mgr.adc_2_battery;
        app_cfg_store(&app_cfg_nv.vbat_ntc_value, 4);
    }
#endif

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
    uint16_t check_time = 0;
    uint16_t max_discrepancy_value = 0;

    if (app_ext_charger_check_status())
    {
        check_time = CHARGING_VBAT_DETECT_TOTAL_TIME;
        max_discrepancy_value = CHARGER_VABT_MAX_ADC_DISCREPANCY_VALUE;
    }
#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
    else
    {
        check_time = DISCHARGER_VBAT_CHECK_TOTAL_TIME;
        max_discrepancy_value = DISCHARGER_VABT_MAX_ADC_DISCREPANCY_VALUE;
    }
#endif

#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
    if (app_cfg_nv.vbat_detect_status != VBAT_DETECT_STATUS_ING)
    {
        app_ext_charger_vbat_is_normal_set(app_cfg_nv.vbat_detect_status);
    }
#endif
    {
        if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_ING)
        {
            app_harman_adc_vbat_detect(check_time, max_discrepancy_value);
            if (app_ext_charger_vbat_is_normal_get() == VBAT_DETECT_STATUS_ING)
            {
                return;
            }
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
            else
            {
                app_cfg_nv.vbat_detect_status = app_ext_charger_vbat_is_normal_get();
                app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 4);
            }
#endif
        }
    }
#endif

    if (app_ext_charger_check_status())
    {
        app_ext_charger_ntc_adc_update();
    }
#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
    else
    {
        app_harman_discharger_adc_update();
    }
#endif
}

static void app_harman_adc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_harman_adc_timeout_cb: timer_evt: 0x%02x, param: %d",
                     timer_evt, param);
    switch (timer_evt)
    {
    case TIMER_HARMAN_ADC_TIMER_NTC_CHECK:
        {
#if HARMAN_VBAT_ADC_DETECTION
            if ((!app_cfg_nv.vbat_detect_normal) &&
                (!app_cfg_nv.store_adc_ntc_value_when_vbat_in))
            {
                // stop charging and discharger NTC protect
                app_harman_adc_ntc_check_timer_stop();
                break;
            }
#endif
            //read ntc value
            if (app_ext_charger_check_status())
            {
                app_ext_charger_ntc_check_timeout();
                app_harman_adc_ntc_check_timer_start(CHARGING_NTC_CHECK_PERIOD);
            }
#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
            else
            {
                app_harman_discharger_ntc_timer_start();
            }
#endif
        }
        break;

    default:
        break;
    }
}

bool app_harman_adc_ntc_check_timer_started(void)
{
    if (timer_idx_ntc_period_check != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_harman_adc_ntc_check_timer_stop(void)
{
    app_stop_timer(&timer_idx_ntc_period_check);
}

void app_harman_adc_ntc_check_timer_start(uint32_t time)
{
#if (F_APP_PERIODIC_WAKEUP_RECHARGE && GFPS_FINDER_SUPPORT)
    APP_PRINT_INFO4("app_harman_adc_ntc_check_timer_start: finder_privisiond: 0x%x, wakeup_reason: 0x%x, "
                    "device_state: %d, dlps_bitmap: 0x%x",
                    app_gfps_finder_provisioned(), app_db.wake_up_reason,
                    app_db.device_state, app_dlps_get_dlps_bitmap());
    if ((!extend_app_cfg_const.disable_finder_adv_when_power_off)
        && (app_gfps_finder_provisioned())
        && (app_db.wake_up_reason & WAKE_UP_RTC)
        && (app_db.device_state == APP_DEVICE_STATE_OFF))
    {
#if HARMAN_VBAT_ADC_DETECTION
        if ((app_dlps_get_dlps_bitmap() & APP_DLPS_ENTER_VBAT_VALUE_UPDATE) !=
            APP_DLPS_ENTER_VBAT_VALUE_UPDATE)
#endif
        {
            return;
        }
    }
#endif

#if HARMAN_VBAT_ADC_DETECTION
    if ((!app_cfg_nv.vbat_detect_normal) &&
        (!app_cfg_nv.store_adc_ntc_value_when_vbat_in))
    {
        return;
    }
#endif

    if (timer_idx_ntc_period_check == 0)
    {
        app_start_timer(&timer_idx_ntc_period_check, "ntc_check",
                        app_harman_adc_timer_id, TIMER_HARMAN_ADC_TIMER_NTC_CHECK, 0, false,
                        time);
        app_timer_register_pm_excluded(&timer_idx_ntc_period_check);
    }
    else
    {
        app_stop_timer(&timer_idx_ntc_period_check);
        app_start_timer(&timer_idx_ntc_period_check, "ntc_check",
                        app_harman_adc_timer_id, TIMER_HARMAN_ADC_TIMER_NTC_CHECK, 0, false,
                        time);
    }
}

bool app_harman_adc_adp_in_handle(void)
{
#if HARMAN_VBAT_ADC_DETECTION
    if ((!app_cfg_nv.vbat_detect_normal) &&
        (!app_cfg_nv.store_adc_ntc_value_when_vbat_in))
    {
        return true;
    }
#endif

    app_harman_adc_io_read();
    app_harman_adc_ntc_check_timer_start(2 * CHARGING_NTC_CHECK_PERIOD);

    return false;
}

void app_harman_adc_adp_out_handle(void)
{
#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
    app_harman_adc_vbat_check_times_clear();
#endif

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
    app_harman_discharger_ntc_timer_start();
#endif
}

void app_harman_adc_init(void)
{
    app_timer_reg_cb(app_harman_adc_timeout_cb, &app_harman_adc_timer_id);

#if (HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
    app_harman_adc_vbat_check_times_clear();
#endif

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
    app_harman_discharger_ntc_timer_start();
#endif
}
#endif
