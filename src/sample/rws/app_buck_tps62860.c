/*******************************************************************************
* Copyright (C) 2021 Realtek Semiconductor Corporation. All Rights Reserved.
*/
#include "app_buck_tps62860.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "app_cfg.h"
#include "platform_utils.h"
#include "rtl876x_pinmux.h"
#include "trace.h"
#include "ext_buck.h"

#define TPS62860_ADDR    0x40

#define TPS62860_VSEL_VOUT1                  0x01
#define TPS62860_VSEL_VOUT2                  0x02

#define TPS62860_VOLTAGE_RAMP_10MV_US        0x00
#define TPS62860_VOLTAGE_RAMP_5MV_US         0x01
#define TPS62860_VOLTAGE_RAMP_1MV_US         0x02
#define TPS62860_VOLTAGE_RAMP_0P1MV_US       0x03

#define TPS62860_VOUT1_REG              0x01
#define TPS62860_VOUT2_REG              0x02
#define TPS62860_CONTROL_REG            0x03
#define TPS62860_STATUS_REG             0x05

#define TPS62860_BITS_VO1_SET                          (0x3F)
#define TPS62860_BITS_VO2_SET                          (0x3F)
#define TPS62860_BITS_OPEERATION_MODE                  (0x80)

#define TPS62860_BITS_RESET                            (0x80)
#define TPS62860_BITS_VOLTAGE_CHANGE_FPWM              (0x40)
#define TPS62860_BITS_SW_ENABLE                        (0x20)
#define TPS62860_BITS_FPWM                             (0x10)
#define TPS62860_BITS_OUTPUT_DISCHARGE                 (0x08)
#define TPS62860_BITS_VOLTAGE_RAMP                     (0x03)

#define TPS62860_VSEL_PIN                (0xFF)
#define TPS62860_VENABLE_PIN             (app_cfg_const.buck_enable_pinmux)

bool app_buck_tps62860_write_reg(uint8_t reg_addr, uint8_t data)
{
    uint8_t buf[2];

    buf[0] = reg_addr;
    buf[1] = data;

    if (app_sensor_i2c_write(TPS62860_ADDR,  buf, 2) != I2C_Success)
    {
        return false;
    }

    return true;
}

bool app_buck_tps62860_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    if (app_sensor_i2c_read(TPS62860_ADDR, reg_addr, data, len) != I2C_Success)
    {
        return false;
    }
    return true;
}

bool app_buck_tps62860_update_bits(uint8_t reg_addr, uint8_t bitmask, uint8_t data)
{
    uint8_t reg_value;

    app_buck_tps62860_read_reg(reg_addr, &reg_value, 1);

    reg_value = ((reg_value & (~bitmask)) | (data & bitmask));

    return app_buck_tps62860_write_reg(reg_addr, reg_value);
}

bool app_buck_tps62860_set_io(uint8_t pin, uint8_t level)
{
    if (pin >= TOTAL_PIN_NUM)
    {
        return false;
    }

    PAD_OUTPUT_VAL output_value = level ? PAD_OUT_HIGH : PAD_OUT_LOW;
    Pad_Config(pin, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, output_value);

    return true;
}


bool app_buck_tps62860_set_vo1(uint32_t volt_h_uv)
{
    if ((volt_h_uv < 4000) || (volt_h_uv > 19875))
    {
        return false;
    }

    uint16_t volt_offset = volt_h_uv - 4000;
    uint8_t cal_data = volt_offset / 125;

    return app_buck_tps62860_update_bits(TPS62860_VOUT1_REG, TPS62860_BITS_VO1_SET, cal_data);
}

bool app_buck_tps62860_set_vo2(uint8_t cfg)
{
    if (cfg > 0x7F)
    {
        return false;
    }

    return app_buck_tps62860_update_bits(TPS62860_VOUT2_REG, TPS62860_BITS_VO2_SET, cfg);
}

bool app_buck_tps62860_set_pwm_for_vo2(bool enable)
{
    uint8_t data = enable ? TPS62860_BITS_OPEERATION_MODE : 0;

    return app_buck_tps62860_update_bits(TPS62860_VOUT2_REG, TPS62860_BITS_OPEERATION_MODE, data);
}

bool app_buck_tps62860_sel_vout(uint8_t vsel)
{
    if ((vsel != TPS62860_VSEL_VOUT1) && (vsel != TPS62860_VSEL_VOUT2))
    {
        return false;
    }

    return app_buck_tps62860_set_io(TPS62860_VSEL_PIN, vsel);
}

bool app_buck_tps62860_reset(void)
{
    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_RESET,
                                         TPS62860_BITS_RESET);
}

bool app_buck_tps62860_set_voltage_change_fpwm(bool enable)
{
    uint8_t data = 0;

    if (enable)
    {
        data = TPS62860_BITS_VOLTAGE_CHANGE_FPWM;
    }

    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_VOLTAGE_CHANGE_FPWM, data);
}

bool app_buck_tps62860_sw_enable(bool enable)
{
    uint8_t data = enable ? TPS62860_BITS_SW_ENABLE : 0;

    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_SW_ENABLE, data);
}

bool app_buck_tps62860_output_discharge(bool enable)
{
    uint8_t data = enable ? TPS62860_BITS_OUTPUT_DISCHARGE : 0;

    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_OUTPUT_DISCHARGE, data);
}

bool app_buck_tps62860_set_voltage_ramp_speed(uint8_t speed)
{
    if ((speed & (~TPS62860_BITS_VOLTAGE_RAMP)) != 0)
    {
        return false;
    }

    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_VOLTAGE_RAMP, speed);
}

bool app_buck_tps62860_set_fpwm(bool is_enable)
{
    uint8_t data = is_enable ? TPS62860_BITS_FPWM : 0;

    return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_FPWM, data);
}

bool app_buck_tps62860_io_enable(bool is_enable)
{
    uint8_t level = is_enable ? 1 : 0;

    return app_buck_tps62860_set_io(TPS62860_VENABLE_PIN, level);
}

bool app_buck_tps62860_volt_change_fpwm_check_ramp(bool is_enable)
{
    uint8_t reg_value = 0;

    if (is_enable == false)
    {
        return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_VOLTAGE_CHANGE_FPWM, 0);
    }

    app_buck_tps62860_read_reg(TPS62860_CONTROL_REG, &reg_value, 1);

    if ((reg_value & TPS62860_BITS_VOLTAGE_RAMP) < TPS62860_VOLTAGE_RAMP_1MV_US)
    {
        APP_PRINT_ERROR1("external buck could not enable fpwm during volt change, ramp 0x%x",
                         (reg_value & TPS62860_BITS_VOLTAGE_RAMP));
        return false;
    }
    else
    {
        return app_buck_tps62860_update_bits(TPS62860_CONTROL_REG, TPS62860_BITS_VOLTAGE_CHANGE_FPWM,
                                             TPS62860_BITS_VOLTAGE_CHANGE_FPWM);
    }
}

bool app_buck_tps62860_hw_init(void)
{
    app_sensor_i2c_init(TPS62860_ADDR, DEF_I2C_CLK_SPD, false);
    app_buck_tps62860_io_enable(false);

    return true;
}

bool app_buck_tps62860_enable(void)
{
    int32_t ret;
    ret = app_buck_tps62860_io_enable(true);

    //delay max delay time of EN start switching
    platform_delay_us(1000);

    ret = app_buck_tps62860_set_voltage_ramp_speed(TPS62860_VOLTAGE_RAMP_1MV_US);

    return ret;
}

bool app_buck_tps62860_disable(void)
{
    app_buck_tps62860_io_enable(false);

    return true;
}

void app_buck_tps62860_init(void)
{
    EXT_BUCK_T buck_ops;
    buck_ops.ext_buck_hw_init = app_buck_tps62860_hw_init;
    buck_ops.ext_buck_set_voltage = app_buck_tps62860_set_vo1;
    buck_ops.ext_buck_enable = app_buck_tps62860_enable;
    buck_ops.ext_buck_disable = app_buck_tps62860_disable;
    ext_buck_init(&buck_ops);

    app_buck_tps62860_hw_init();
}


