#include "rtk_charger.h"
#include "charger_utils.h"
#include "rtl876x_pinmux.h"
#include "app_cfg.h"

struct
{
    RTK_CHARGER_STATE_CB app_state_cb;
    RTK_CHARGER_SOC_CB   app_soc_cb;
} rtk_charger_mgr;


static void rtk_charger_charge_state_cb(T_CHARGER_STATE rtk_charger_state)
{

    if ((app_cfg_const.thermistor_power_gpio_support) &&
        (app_cfg_const.thermistor_power_pinmux != 0xFF) &&
        (app_cfg_const.ntc_power_by_pad_direct == 0))
    {
        if (rtk_charger_state == STATE_CHARGER_END)
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_LOW);
        }
        else
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_HIGH);
        }
    }

    if (rtk_charger_mgr.app_state_cb)
    {
        rtk_charger_mgr.app_state_cb(rtk_charger_state);
    }
}


static void rtk_charger_soc_cb(uint8_t state_of_charge)
{
    if (rtk_charger_mgr.app_soc_cb)
    {
        rtk_charger_mgr.app_soc_cb(state_of_charge);
    }
}


void rtk_charger_init(RTK_CHARGER_STATE_CB app_state_cb, RTK_CHARGER_SOC_CB app_soc_cb)
{
    rtk_charger_mgr.app_state_cb = app_state_cb;
    rtk_charger_mgr.app_soc_cb = app_soc_cb;

    charger_api_reg_charger_state_callback(rtk_charger_charge_state_cb);
    charger_api_reg_state_of_charge_callback(rtk_charger_soc_cb);
}


T_CHARGER_STATE rtk_charger_get_charge_state(void)
{
    return charger_api_get_charger_state();
}


uint8_t rtk_charger_get_soc(void)
{
    return charger_api_get_state_of_charge();
}


uint32_t rtk_charger_get_bat_vol(void)
{
    uint16_t bat_vol = 0;

    charger_utils_get_batt_volt(&bat_vol);
    return (uint32_t)bat_vol;
}


int32_t rtk_charger_get_bat_curr(void)
{
    int16_t bat_curr = 0;

    charger_utils_get_batt_curr(&bat_curr);
    return bat_curr;
}

