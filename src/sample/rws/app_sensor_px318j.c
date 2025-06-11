#if (F_APP_SENSOR_PX318J_SUPPORT == 1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LE_SUPPORT
#include "profile_server.h"
#include "gap_msg.h"
#endif

#include "ftl.h"
#include "trace.h"
#include "hal_gpio.h"
#include "string.h"
#include "app_dlps.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "app_cfg.h"
#include "rtl876x_pinmux.h"
#include "string.h"
#include "app_sensor_px318j.h"
#include "pm.h"
#include "app_io_msg.h"
#include "section.h"

static T_PX318J_CAL_PARA px318j_flash;

static void app_sensor_px318j_reset_default_thresh(void);
static uint32_t app_sensor_px318j_load_px318j_cal(void *p_data);
static uint32_t app_sensor_px318j_save_px318j_cal(void *p_data);

void app_sensor_px318j_enable()
{
    app_sensor_i2c_write_8(PX318J_ID, PX318J_DEV_CTRL_REG, PX318J_ON);
    platform_delay_ms(10);
}

void app_sensor_px318j_disable()
{
    app_sensor_i2c_write_8(PX318J_ID, PX318J_DEV_CTRL_REG, PX318J_OFF);
    platform_delay_ms(5);
}

void app_sensor_px318j_init(void)
{
    uint8_t ret;
    I2C_Status status;

    /* ret = 0 if success else fail */
    ret = app_sensor_px318j_load_px318j_cal((void *)&px318j_flash);

    if (ret)
    {
        px318j_flash.ps_dac_ctrl = 0;
        px318j_flash.ps_ct_dac = 0;
        px318j_flash.ps_cal[0] = 0;
        px318j_flash.ps_cal[1] = 0;
    }

    if (px318j_flash.px_pulse_duration == 0 &&
        px318j_flash.px_pulse_time == 0)
    {
        px318j_flash.px_pulse_duration = app_cfg_const.px_pulse_duration;
        px318j_flash.px_pulse_time = app_cfg_const.px_pulse_time;
    }

    /* soft reboot*/
    status = app_sensor_i2c_write_8(PX318J_ID, PX318J_SOFT_REBOOT_REG, PX318J_SOFT_REBOOT);

    if (status == I2C_Success)
    {
        /* must have delay at least 30 ms after soft reboot(first I2C command) */
        platform_delay_ms(50);
        /* PsData 10bit unsigned react time 30msC */
        app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTRL_REG, PS_CTRL);
        /* pulse width 32w = 64 ms */
        app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUW_REG, px318j_flash.px_pulse_duration);
        /* PsPuc = 0x02 = 2 times, VCSEL turn on two times. */
        app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUC_REG, px318j_flash.px_pulse_time);
        /* 12 mA current */
        app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDRV_REG, PS_DRV);
        /* wait time 170 ms */
        app_sensor_i2c_write_8(PX318J_ID, PX318J_WAITTIME_REG, WAIT_TIME);

        /* High, Low Threshold setting path. Form default or factory calibration value */
        if (ret)
        {
            app_sensor_px318j_reset_default_thresh();
        }
        else
        {
            app_sensor_i2c_write_16(PX318J_ID, PX318J_OUT_EAR_REG,
                                    px318j_flash.out_ear_thresh[0] | (px318j_flash.out_ear_thresh[1] << 8));
            app_sensor_i2c_write_16(PX318J_ID, PX318J_IN_EAR_REG,
                                    px318j_flash.in_ear_thresh[0] | (px318j_flash.in_ear_thresh[1] << 8));
        }

        if (!ret)
        {
            uint16_t ps_cal;

            /* PX318J_I2C_Write(0x65, PsDacCtrl);//set PsDacCtrl */
            app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDAC_CTRL_REG, px318j_flash.ps_dac_ctrl);
            /* PX318J_I2C_Write(0x67, PsCtDac); //set PsCtDac */
            app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTDAC_REG, px318j_flash.ps_ct_dac);
            ps_cal = ((px318j_flash.ps_cal[0] & 0xFF) | (px318j_flash.ps_cal[1] << 8));
            /* PX318J_I2C_Write(PX318J_ID, 0x69, PsCalL);  set ps_cal */
            app_sensor_i2c_write_16(PX318J_ID, PX318J_PSCAL_REG, ps_cal);
        }

        app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, 0x00);
    }
    else
    {
        APP_PRINT_TRACE0("app_sensor_px318j_init: init fail");
    }
}

ISR_TEXT_SECTION void app_sensor_px318j_int_gpio_intr_cb(uint32_t param)
{
    uint8_t int_status;
    T_IO_MSG gpio_msg;

    /* Control of entering DLPS */
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

    int_status = hal_gpio_get_input_level(app_cfg_const.sensor_detect_pinmux);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(app_cfg_const.sensor_detect_pinmux);

    /* Change GPIO Interrupt Polarity */
    if (int_status == SENSOR_INT_RELEASED)
    {
    }
    else
    {
        gpio_msg.u.param = 0;

        T_NEAR_FAR near_far_flag = NEAR_FAR_NONE;
        uint8_t int_flag;
        uint8_t buf = 0;

        app_sensor_i2c_read_8(PX318J_ID, PX318J_INTTERUPT_REG, &int_flag);
        app_sensor_i2c_read_8(PX318J_ID, PX318J_STATE_FLAG_REG, &buf);

        if (int_flag & INTERRUPT_READY)
        {
            if (buf & NEAR_FAR_DATA)
            {
                near_far_flag = FAR;    /* far event */
            }
            else
            {
                near_far_flag = NEAR;   /* near event */
            }
        }

        app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT);

        if (near_far_flag == FAR)
        {
            //Only handle click press (high -> low), not handle release
            gpio_msg.type = IO_MSG_TYPE_GPIO;
            gpio_msg.subtype = IO_MSG_GPIO_SENSOR_LD;
            gpio_msg.u.param = SENSOR_LD_OUT_EAR;

        }
        else if (near_far_flag == NEAR)
        {
            gpio_msg.type = IO_MSG_TYPE_GPIO;
            gpio_msg.subtype = IO_MSG_GPIO_SENSOR_LD;
            gpio_msg.u.param = SENSOR_LD_IN_EAR;
        }

        if (near_far_flag != NEAR_FAR_NONE)
        {
            app_io_msg_send(&gpio_msg);
        }
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(app_cfg_const.sensor_detect_pinmux);
}

static void app_sensor_px318j_reset_default_thresh(void)
{
    app_sensor_i2c_write_16(PX318J_ID, PX318J_OUT_EAR_REG, PS_DEFAULT_THRESHOLD_OUT_EAR);
    app_sensor_i2c_write_16(PX318J_ID, PX318J_IN_EAR_REG, PS_DEFAULT_THRESHOLD_IN_EAR);
    px318j_flash.out_ear_thresh[0] = PS_DEFAULT_THRESHOLD_OUT_EAR & 0xFF;
    px318j_flash.out_ear_thresh[1] = PS_DEFAULT_THRESHOLD_OUT_EAR >> 8;
    px318j_flash.in_ear_thresh[0] = PS_DEFAULT_THRESHOLD_IN_EAR & 0xFF;
    px318j_flash.in_ear_thresh[1] = PS_DEFAULT_THRESHOLD_IN_EAR >> 8;
    app_sensor_px318j_save_px318j_cal((void *)&px318j_flash);
}

void app_sensor_px318j_enter_dlps(void)
{
    POWERMode mode = power_mode_get();

    if (mode == POWER_POWERDOWN_MODE)
    {
        app_sensor_px318j_disable();
    }
    else
    {
        app_dlps_set_pad_wake_up(app_cfg_const.sensor_detect_pinmux, PAD_WAKEUP_POL_LOW);
    }
}

/*
static void app_sensor_px318j_reset(void)
{
    if (app_cfg_const.sensor_support)
    {
        app_sensor_px318j_disable();
        app_sensor_px318j_enable();
    }
}
*/

static uint32_t app_sensor_px318j_load_px318j_cal(void *p_data)
{
    return ftl_load_from_storage(p_data, APP_RW_PX318J_ADDR, APP_RW_PX318J_SIZE);
}

static uint32_t app_sensor_px318j_save_px318j_cal(void *p_data)
{
    return ftl_save_to_storage(p_data, APP_RW_PX318J_ADDR, APP_RW_PX318J_SIZE);
}

uint8_t app_sensor_px318j_auto_dac(uint8_t *spp_report)
{
    T_PX318J_CAL_DATA cal_data;
    uint16_t ps_data = 0;
    bool first_data = true;

    //Setting Variable
    uint8_t ps_ct_dac = 0;
    uint8_t ps_dac_ctrl = 0;
    uint8_t ps_ct_gain = 0;
    int16_t dac_temp = 0;

    //PI Control variable
    bool pi_control = true;
    int32_t dp = 0;
    int32_t di = 0;
    fixedpt kp = FIXEDPT_RCONST(0.015);
    fixedpt ki = FIXEDPT_RCONST(0.02);
    uint8_t last_try = 0;

    //Bisection method
    uint8_t dac_max = 96;
    uint8_t dac_min = 1;

    //Sensor Initial
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTRL_REG, PS_CTRL);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUW_REG, px318j_flash.px_pulse_duration);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUC_REG, px318j_flash.px_pulse_time);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDRV_REG, PS_DRV_CTRL);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_WAITTIME_REG, 0x00); //WaitTime = 0
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDAC_CTRL_REG, 0x01); //Reset PsDacCtrl
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTDAC_REG, 0x00); //Reset PsCtDac
    app_sensor_i2c_write_16(PX318J_ID, PX318J_PSCAL_REG, 0x00);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INT_CTRL_REG, DISABLE_INT_PIN); //Close INT pin output
    app_sensor_i2c_write_8(PX318J_ID, PX318J_HALT_CTRL_REG,
                           PX318J_DATA_HALT_ENABLE); //Enable Data Ready Interrupt Halt
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG,
                           PX318J_CLEAR_INTTERUPT); //Clear Interrupt Flag
    app_sensor_i2c_write_8(PX318J_ID, PX318J_FAST_EN_REG,
                           PX318J_FAST_EN_ENABLE); //Enable Fast-En(Factor function)
    app_sensor_px318j_enable(); //Enable Sensor

    ps_ct_gain = 0x01;
    ps_dac_ctrl = ps_ct_gain;
    ps_ct_dac = 0x00;

    //First Step
    while (1)
    {
        if (app_sensor_i2c_read(PX318J_ID, PX318J_INTTERUPT_REG, (uint8_t *)&cal_data,
                                4)) //Get Interrupt flag and PS Data.
        {
            return PX318J_CAL_SENSOR_ERROR;
        }

        if ((cal_data.interrupt_flag & PX318J_INTERRUPT_READY) == PX318J_INTERRUPT_READY)  //Data Ready flag
        {
            ps_data = (uint16_t)cal_data.ps_data[0] + ((uint16_t)cal_data.ps_data[1] << 8);

            if (first_data) //Ignore the first data.
            {
                first_data = false;
                app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG,
                                       PX318J_CLEAR_INTTERUPT);  //Clear Interrupt Flag
                continue;
            }

            //With last try and PS Data > 0, finish the calibration else keep going.
            if (last_try == 1 && ps_data > 0)
            {
                break;
            }
            else
            {
                last_try = 0;
            }

            if (ps_ct_dac > 0)
            {
                //The PsCtDac is over spec, try to use the bisection method to get the right setting.
                if (ps_data == 0)
                {
                    dac_max = (uint8_t)ps_ct_dac;
                    pi_control = false;
                }
                //PS Data <= target value, finish the calibration.
                else if (ps_data <= TARGET_PXY)
                {
                    break;
                }
                //With the bisection method, we get the last value. finish calibration.
                else if (ps_ct_dac == dac_min || ps_ct_dac == dac_max)
                {
                    break;
                }
            }
            //PS Data <= target value, finish the calibration.
            else if (ps_data <= TARGET_PXY)
            {
                break;
            }

            if (pi_control) //Get the setting with PI control.
            {
                dp = ps_data - TARGET_PXY;
                di += dp;

                dac_temp = (int16_t)ps_ct_dac
                           + (int16_t)FIXEDPT_TOINT(FIXEDPT_XMUL(kp, FIXEDPT_FROMINT(dp))
                                                    + FIXEDPT_XMUL(ki, FIXEDPT_FROMINT(di)))
                           + (dp >= 0 ? 1 : -1);

                if (dac_temp > 96)
                {
                    if (ps_ct_gain == 0x0F)
                    {
                        last_try = 1;
                        dac_temp = 96;
                    }
                    else
                    {
                        //PsCtGain = (Dac_temp >> 5) + (Dac_temp >> 4);
                        ps_ct_gain = dac_temp / 36;

                        if (ps_ct_gain > 0x0F)
                        {
                            ps_ct_gain = 0x0F;
                        }

                        dac_temp = 48;

                        ps_dac_ctrl = (ps_dac_ctrl & 0xF0) | ps_ct_gain;
                        app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDAC_CTRL_REG, ps_dac_ctrl);
                    }
                }
            }
            else
            {
                if (ps_data > TARGET_PXY)
                {
                    dac_min = (uint8_t)dac_temp;
                }

                if (ps_data < PXY_FULL_RANGE && ps_data > TARGET_PXY) //Reduce calculate time.
                {
                    dac_temp += 1;
                }
                else
                {
                    dac_temp = (uint16_t)(dac_min + dac_max) >> 1;
                }
            }

            ps_ct_dac = (uint8_t)dac_temp;

            app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTDAC_REG, ps_ct_dac);
            app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG,
                                   PX318J_CLEAR_INTTERUPT);   //Clear Interrupt Flag
        }
    }

    app_sensor_px318j_disable(); //Shutdown sensor
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT); //Clear IntFlag
    app_sensor_i2c_write_8(PX318J_ID, PX318J_HALT_CTRL_REG,
                           PX318J_DATA_HALT_DISABLE); //DataHalt Disable
    app_sensor_i2c_write_8(PX318J_ID, PX318J_FAST_EN_REG, PX318J_FAST_EN_DISABLE); //FastEn Disable

    //Second Step
    app_sensor_px318j_enable(); //Enable sensor

    uint8_t index = 0;
    uint32_t sum = 0;
    do
    {
        if (app_sensor_i2c_read(PX318J_ID, PX318J_INTTERUPT_REG, (uint8_t *)&cal_data, 4))
        {
            return PX318J_CAL_SENSOR_ERROR;
        }

        if ((cal_data.interrupt_flag & PX318J_INTERRUPT_READY) == PX318J_INTERRUPT_READY)
        {
            ps_data = (uint16_t)cal_data.ps_data[0] + ((uint16_t)cal_data.ps_data[1] << 8);
            cal_data.interrupt_flag = PX318J_INTERRUPT_READY;
            app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT);

            if (index > 1) //Ignore the first two data
            {
                sum += ps_data;
            }

            index++;
        }
    }
    while (index < 10);

    ps_data = (uint16_t)(sum >> 3) + 20;

    app_sensor_px318j_disable(); //Shutdown sensor

    app_sensor_i2c_write_16(PX318J_ID, PX318J_PSCAL_REG, ps_data);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INT_CTRL_REG, ENABLE_INT_PIN); // Open INT pin output
    app_sensor_i2c_write_8(PX318J_ID, PX318J_WAITTIME_REG, WAIT_TIME); //WaitTime = 170ms
    spp_report[0] = ps_ct_gain;
    spp_report[1] = ps_ct_dac;
    px318j_flash.ps_dac_ctrl = ps_dac_ctrl;
    px318j_flash.ps_ct_dac = ps_ct_dac;
    px318j_flash.ps_cal[0] = ps_data & 0xFF;
    px318j_flash.ps_cal[1] = ps_data >> 8;

    app_sensor_px318j_save_px318j_cal((void *)&px318j_flash);

    //Save calibration value to flash memory (this function have to add by customer)
    //reg 0x65 (PsDacCtrl)
    //reg 0x67 (PsCtDac)
    //reg 0x69, 0x6a (PsData)
    //APP_PRINT_TRACE3("[PX318J] PsCtGain = %x, PsCtDac = %x, PsData = %x", ps_dac_ctrl, ps_ct_dac, ps_data);

    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSDAC_CTRL_REG, ps_dac_ctrl);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_PSCTDAC_REG, ps_ct_dac);
    app_sensor_i2c_write_16(PX318J_ID, PX318J_PSCAL_REG, ps_data);

    app_sensor_px318j_enable(); //Enable sensor

    return PX318J_CAL_SUCCESS;
}

uint8_t app_sensor_px318j_read_ps_data_after_noise_floor_cal(uint8_t *spp_report)
{
    T_PX318J_CAL_DATA cal_data;
    uint8_t index = 0;
    uint16_t ps_data = 0;

    do
    {
        if (app_sensor_i2c_read(PX318J_ID, PX318J_INTTERUPT_REG, (uint8_t *)&cal_data, 4))
        {
            return PX318J_CAL_SENSOR_ERROR;
        }

        if ((cal_data.interrupt_flag & PX318J_INTERRUPT_READY) == PX318J_INTERRUPT_READY)
        {
            ps_data = (uint16_t)cal_data.ps_data[0] + ((uint16_t)cal_data.ps_data[1] << 8);

            cal_data.interrupt_flag = PX318J_INTERRUPT_READY;
            app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT);
            index++;
        }
    }
    while (index < 1);

    spp_report[0] = ps_data & 0xFF;
    spp_report[1] = ps_data >> 8;
    px318j_flash.read_ps_data[0] = ps_data & 0xFF;
    px318j_flash.read_ps_data[1] = ps_data >> 8;
    app_sensor_px318j_save_px318j_cal((void *)&px318j_flash);

    return PX318J_CAL_SUCCESS;
}

static void app_sensor_px318j_set_threshold(uint8_t out_ear, uint16_t value)
{
    APP_PRINT_TRACE2("app_sensor_px318j_set_threshold: out_ear %d, value 0x%x", out_ear, value);

    if (out_ear)
    {
        app_sensor_i2c_write_16(PX318J_ID, PX318J_OUT_EAR_REG, value); //low out-ear
    }
    else
    {
        app_sensor_i2c_write_16(PX318J_ID, PX318J_IN_EAR_REG, value); //high in-ear
    }
}

uint8_t app_sensor_px318j_threshold_cal(uint8_t out_ear, uint8_t *spp_report)
{
    T_PX318J_CAL_DATA cal_data;
    uint16_t ps_data = 0;
    uint8_t index = 0;
    uint32_t sum = 0;

    app_sensor_px318j_load_px318j_cal((void *)&px318j_flash);

    app_sensor_px318j_disable(); //Shutdown sensor
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT); //Clear IntFlag
    app_sensor_i2c_write_8(PX318J_ID, PX318J_WAITTIME_REG, 0x00);
    app_sensor_i2c_write_8(PX318J_ID, PX318J_HALT_CTRL_REG,
                           PX318J_DATA_HALT_DISABLE); //DataHalt Disable
    app_sensor_i2c_write_8(PX318J_ID, PX318J_FAST_EN_REG, PX318J_FAST_EN_DISABLE); //FastEn Disable

    //Second Step
    app_sensor_px318j_enable(); //Enable sensor

    do
    {
        if (app_sensor_i2c_read(PX318J_ID, PX318J_INTTERUPT_REG, (uint8_t *)&cal_data, 4))
        {
            return PX318J_CAL_SENSOR_ERROR;
        }

        if ((cal_data.interrupt_flag & PX318J_INTERRUPT_READY) == PX318J_INTERRUPT_READY)
        {
            ps_data = (uint16_t)cal_data.ps_data[0] + ((uint16_t)cal_data.ps_data[1] << 8);
            cal_data.interrupt_flag = PX318J_CLEAR_INTTERUPT;
            app_sensor_i2c_write_8(PX318J_ID, PX318J_INTTERUPT_REG, PX318J_CLEAR_INTTERUPT);

            if (index > 1)
            {
                sum += ps_data;
            }

            index++;
        }
    }
    while (index < 10);

    app_sensor_px318j_disable(); //Shutdown sensor
    app_sensor_i2c_write_8(PX318J_ID, PX318J_INT_CTRL_REG, ENABLE_INT_PIN); // Open INT pin output
    app_sensor_i2c_write_8(PX318J_ID, PX318J_WAITTIME_REG, WAIT_TIME); //WaitTime = 170ms

    ps_data = (uint16_t)(sum / 8);

    if (out_ear)
    {
        px318j_flash.out_ear_thresh[0] = ps_data & 0xFF;
        px318j_flash.out_ear_thresh[1] = ps_data >> 8;
    }
    else
    {
#if 0
        ps_data = ps_data == 0x3ff ? 0x3fd : ps_data;
#endif
        px318j_flash.in_ear_thresh[0] = ps_data & 0xFF;
        px318j_flash.in_ear_thresh[1] = ps_data >> 8;
    }

    spp_report[0] = ps_data & 0xFF;
    spp_report[1] = ps_data >> 8;

    app_sensor_px318j_save_px318j_cal((void *)&px318j_flash);
    app_sensor_px318j_set_threshold(out_ear, ps_data);
    app_sensor_px318j_enable();

    return PX318J_CAL_SUCCESS;
}

void app_sensor_px318j_get_para(uint8_t *spp_report)
{
    T_PX318J_CAL_PARA spp_px318j_flash;

    app_sensor_px318j_load_px318j_cal((void *)&spp_px318j_flash);
    memcpy((void *)spp_report, (void *)&spp_px318j_flash, 12);

    APP_PRINT_TRACE1("app_sensor_px318j_get_para: %b", TRACE_BINARY(12, spp_report));
}

void app_sensor_px318j_write_data(uint8_t target, uint8_t *val)
{
    APP_PRINT_TRACE2("app_sensor_px318j_write_data: target 0x%02x, val 0x%02x", target, val[0]);

    switch (target)
    {
    case PX318J_PSPUW_REG:
        {
            px318j_flash.px_pulse_duration = val[0];
            app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUW_REG, px318j_flash.px_pulse_duration);
        }
        break;

    case PX318J_PSPUC_REG:
        {
            px318j_flash.px_pulse_time  = val[0];
            app_sensor_i2c_write_8(PX318J_ID, PX318J_PSPUC_REG, px318j_flash.px_pulse_time);
        }
        break;

    default:
        break;
    }

    app_sensor_px318j_save_px318j_cal(&px318j_flash);
}

#ifdef __cplusplus
}
#endif

#endif
