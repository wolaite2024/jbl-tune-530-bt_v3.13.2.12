#if (F_APP_SENSOR_JSA1225_SUPPORT == 1) || (F_APP_SENSOR_JSA1227_SUPPORT == 1)

#include "rtl876x_i2c.h"
#include "hal_gpio.h"
#include "app_cfg.h"
#include "app_dlps.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "ftl.h"
#include "trace.h"
#include "app_sensor_jsa.h"
#include "app_io_msg.h"
#include "section.h"

#ifdef __cplusplus
extern "C" {
#endif

static T_JSA_CAL_PARA jsa_flash;

static void app_sensor_jsa_reset_default_thresh(void);
static uint32_t app_sensor_jsa_save_cal(void *p_data);
static uint32_t app_sensor_jsa_load_cal(void *p_data);

void app_sensor_jsa1225_init(void)
{
    I2C_Status status;

    //Software reset
    status = app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, 0x80);

    /* must have delay at least 30 ms after power on reset after soft reboot */
    platform_delay_ms(50);

    app_sensor_jsa_load_cal(&jsa_flash);

    /* Set default threshold for non-calibration sensor */
    if ((jsa_flash.sensor_xtalk == 0) &&
        (jsa_flash.sensor_low_thres == 0) &&
        (jsa_flash.sensor_high_thres == 0))
    {
        app_sensor_jsa_reset_default_thresh();
    }

    if ((jsa_flash.sensor_gain == 0) &&
        (jsa_flash.sensor_pulse == 0) &&
        (jsa_flash.sensor_integration_time == 0))
    {
        jsa_flash.sensor_gain = app_cfg_const.jsa_gain;
        jsa_flash.sensor_pulse = app_cfg_const.jsa_pulse;
        jsa_flash.sensor_integration_time = app_cfg_const.integration_time;
    }

    if (status == I2C_Success) //sensor exist
    {
        //Setup Sensor Gain
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_GAIN, jsa_flash.sensor_gain);
        //Setup Integration Time: Detect distance
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_INT_TIME,
                               jsa_flash.sensor_integration_time);
        //Setup Pulse Numter
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_PULSE, jsa_flash.sensor_pulse);
        //Setup Resolution
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_TIME, 0x03);
        //Setup Average Number
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_AVG, 0x04);
        //Setup Waiting Time: Detect time
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_WAIT_TIME, app_cfg_const.waiting_time);
        //Setup Interrupt Persistence
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PERSISTENCE,
                               app_cfg_const.interrupt_persistence);
        //Setup X'talk
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_OFFSET_L, jsa_flash.sensor_xtalk);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_OFFSET_H,
                               jsa_flash.sensor_xtalk >> 8);
        //Setup low threshold
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_THRES_LL, jsa_flash.sensor_low_thres);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_THRES_LH,
                               jsa_flash.sensor_low_thres >> 8);
        //Setup high threshold
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_THRES_HL,
                               jsa_flash.sensor_high_thres);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_THRES_HH,
                               jsa_flash.sensor_high_thres >> 8);
        //Clear Interrupt Flag
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_INT_FLAG, 0x00);
        //Setup Interrupt Function
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_INT_CTRL, 0x03);
        //Enable Waiting Time
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, 0x40);
    }
    else
    {
        APP_PRINT_ERROR0("sensor_vendor_jsa_init fail");
    }
}

void app_sensor_jsa1227_init(void)
{
    I2C_Status status;

    status = app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL,
                                    0x80); //Software reset
    /* must have delay at least 30 ms after power on reset after soft reboot */
    platform_delay_ms(50);

    app_sensor_jsa_load_cal(&jsa_flash);

    /* Set default threshold for non-calibration sensor */
    if ((jsa_flash.sensor_xtalk == 0) &&
        (jsa_flash.sensor_low_thres == 0) &&
        (jsa_flash.sensor_high_thres == 0))
    {
        app_sensor_jsa_reset_default_thresh();
    }

    if ((jsa_flash.sensor_gain == 0) &&
        (jsa_flash.sensor_pulse == 0) &&
        (jsa_flash.sensor_integration_time == 0))
    {
        jsa_flash.sensor_gain = app_cfg_const.jsa_gain;
        jsa_flash.sensor_pulse = app_cfg_const.jsa_pulse;
        jsa_flash.sensor_integration_time = app_cfg_const.integration_time;
    }

    if (status == I2C_Success) //sensor exist
    {
        APP_PRINT_TRACE6("app_sensor_jsa1227_init: xtalk %d, low %d, high %d, integration_time %d, ps_gain %d, ps_pulse %d",
                         jsa_flash.sensor_xtalk,
                         jsa_flash.sensor_low_thres, jsa_flash.sensor_high_thres, jsa_flash.sensor_integration_time,
                         jsa_flash.sensor_gain, jsa_flash.sensor_pulse);

        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_INT_CTRL, 0x03);
        //Setup Waiting Time: Detect time
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_WAIT_TIME,
                               app_cfg_const.waiting_time);
        //Setup Sensor Gain
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_GAIN,
                               SENSOR_VAL_JSA1227_VCSEL_CURRENT_10mA | jsa_flash.sensor_gain);
        //Setup Pulse Number
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_PULSE,
                               (jsa_flash.sensor_pulse & 0x0F));
        //Setup Integration Time: Detect distance
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_TIME,
                               jsa_flash.sensor_integration_time << 4 | (SENSOR_VAL_JSA1227_PSCONV & 0x0F));
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_FILTER,
                               0xB0 | (SENSOR_VAL_JSA1227_NUM_AVG & 0x0F));
        //Setup Interrupt Persistence
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_PERSISTENCE,
                               app_cfg_const.interrupt_persistence | 0x01);
        //Setup low threshold
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_L,
                               jsa_flash.sensor_low_thres);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_H,
                               jsa_flash.sensor_low_thres >> 8);
        //Setup high threshold
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_L,
                               jsa_flash.sensor_high_thres);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_H,
                               jsa_flash.sensor_high_thres >> 8);
        //Setup digital offset
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_CALIBRATION_L, 0x00);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_CALIBRATION_H, 0x00);
        //Setup X'talk
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_MANU_CDAT_L,
                               jsa_flash.sensor_xtalk & 0xFF);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_MANU_CDAT_H,
                               (jsa_flash.sensor_xtalk >> 8) & 0x01);

        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_PIPE_THRES, 0x80);
        //Use manu calibraion
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_CTRL, 0x1F);
        //Wating time enalbe
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, 0x40);
        //Clear Interrupt Flag
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_INT_FLAG, 0x00);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_STAT, 0x00);
    }
    else
    {
        APP_PRINT_ERROR0("app_sensor_jsa1227_init fail");
    }
}

void app_sensor_jsa_enable(void)
{
    uint8_t val;

    if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
    {
        app_sensor_i2c_read_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, &val);
        val |= 0x02;
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, val);
    }
    else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
    {
        app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, &val);
        val |= 0x06;
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, val);
    }
}

void app_sensor_jsa_disable(void)
{
    uint8_t val;

    if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
    {
        app_sensor_i2c_read_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, &val);
        val &= ~0x02;
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, val);
    }
    else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
    {
        app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, &val);
        val &= ~0x06;
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, val);
    }
}

ISR_TEXT_SECTION void  app_sensor_jsa_int_gpio_intr_cb(uint32_t param)
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
        hal_gpio_irq_change_polarity(app_cfg_const.sensor_detect_pinmux,
                                     GPIO_IRQ_ACTIVE_LOW); //Polarity Low
    }
    else
    {
        uint8_t val = 0;

        hal_gpio_irq_change_polarity(app_cfg_const.sensor_detect_pinmux,
                                     GPIO_IRQ_ACTIVE_HIGH); //Polarity High

        //Get detect status, 1: appear (in-ear). 0: disapper (out-ear)
        if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
        {
            app_sensor_i2c_read_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_INT_FLAG, &val);
        }
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
        {
            app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_INT_FLAG, &val);
        }

        gpio_msg.type = IO_MSG_TYPE_GPIO;
        gpio_msg.subtype = IO_MSG_GPIO_SENSOR_LD;
        gpio_msg.u.param = (val & BIT(5)) ? SENSOR_LD_IN_EAR : SENSOR_LD_OUT_EAR;

        app_io_msg_send(&gpio_msg);
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(app_cfg_const.sensor_detect_pinmux);

    /* Need to clear interrupt flag after GPIO interrupt is enabled. The procedure is to make sure
       system would catch int pin rising edge which is pulled up by JSA1225 sensor.
    */
    if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
    {
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_INT_FLAG, 0x00);
    }
    else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
    {
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_INT_FLAG, 0x00);
    }

}

static void app_sensor_jsa_reset_default_thresh(void)
{
    jsa_flash.sensor_xtalk = 0;

    if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
    {
        jsa_flash.sensor_low_thres = 200;
        jsa_flash.sensor_high_thres = 500;
    }
    else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
    {
        jsa_flash.sensor_low_thres = 400;
        jsa_flash.sensor_high_thres = 600;
    }

    app_sensor_jsa_save_cal(&jsa_flash);
}

static uint32_t app_sensor_jsa_load_cal(void *p_data)
{
    return ftl_load_from_storage(p_data, APP_RW_JSA_ADDR, APP_RW_JSA_SIZE);
}

static uint32_t app_sensor_jsa_save_cal(void *p_data)
{
    return ftl_save_to_storage(p_data, APP_RW_JSA_ADDR, APP_RW_JSA_SIZE);
}

void app_sensor_jsa_write_data(uint8_t target, uint8_t *val)
{
    switch (target)
    {
    case SENSOR_JSA_WRITE_INT_TIME:
        {
            jsa_flash.sensor_integration_time = val[0];
        }
        break;

    case SENSOR_JSA_WRITE_PS_GAIN:
        {
            jsa_flash.sensor_gain = val[0];
        }
        break;

    case SENSOR_JSA_WRITE_PS_PULSE:
        {
            jsa_flash.sensor_pulse = val[0];
        }
        break;

    case SENSOR_JSA_WRITE_LOW_THRES:
        {
            jsa_flash.sensor_low_thres = ((val[1] << 8) + val[0]);
        }
        break;

    case SENSOR_JSA_WRITE_HIGH_THRES:
        {
            jsa_flash.sensor_high_thres = ((val[1] << 8) + val[0]);
        }
        break;

    default:
        break;
    }

    app_sensor_jsa_save_cal(&jsa_flash);
}

T_JSA_CAL_RETURN app_sensor_jsa1225_calibration(uint8_t cal_mode, uint16_t *spp_result)
{
    uint32_t cal_result = 0;
    uint32_t cal_val = 0;
    uint8_t val[2];
    T_JSA_CAL_RETURN ret = JSA_CAL_SUCCESS;

    if (cal_mode == SENSOR_JSA_CAL_XTALK)
    {
        app_sensor_jsa_reset_default_thresh();
        app_sensor_jsa_disable();
        app_sensor_jsa1225_init();
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_OFFSET_L, 0x00);
        app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_OFFSET_H, 0x00);
    }
    else
    {
        app_sensor_jsa1225_init();
    }

    if (app_sensor_i2c_write_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_SYSM_CTRL, 0x00)) //Disable Waiting Time
    {
        ret = JSA_CAL_FAIL;
        goto EXIT;
    }
    app_sensor_jsa_enable();

    //Read PS data and calculate average for calibration
    platform_delay_ms(100);

    for (uint8_t i = 0; i < SENSOR_JSA_CAL_TIMES; i++)
    {
        platform_delay_ms(100);

        app_sensor_i2c_read_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_DATA_L, &(val[0]));
        app_sensor_i2c_read_8(SENSOR_ADDR_JSA, SENSOR_REG_JSA_PS_DATA_H, &(val[1]));
        cal_val = ((val[1] << 8) + val[0]);
        cal_val = cal_val > SENSOR_VAL_JSA_PS_DATA_MAX ? SENSOR_VAL_JSA_PS_DATA_MAX : cal_val;
        cal_result += cal_val;
    }

    cal_result /= SENSOR_JSA_CAL_TIMES;

    switch (cal_mode)
    {
    case SENSOR_JSA_CAL_XTALK:
        jsa_flash.sensor_xtalk = (uint16_t)cal_result;
        break;

    case SENSOR_JSA_LOW_THRES:
        jsa_flash.sensor_low_thres = (uint16_t)cal_result;
        break;

    case SENSOR_JSA_HIGH_THRES:
        jsa_flash.sensor_high_thres = (uint16_t)cal_result;
        break;

    case SENSOR_JSA_READ_PS_DATA:
        break;
    }

    app_sensor_jsa_disable();
    app_sensor_jsa_save_cal(&jsa_flash);

    *spp_result = (uint16_t)cal_result;

EXIT:
    return ret;;
}

T_JSA_CAL_RETURN app_sensor_jsa1227_calibration(uint8_t cal_mode, uint16_t *spp_result)
{
    uint32_t cal_result = 0;
    uint8_t val[2];
    T_JSA_CAL_RETURN ret = JSA_CAL_SUCCESS;

    switch (cal_mode)
    {
    case SENSOR_JSA_CAL_XTALK:
        {
            uint8_t CALIB_STS;

            if (app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_STAT, 0x00))
            {
                ret = JSA_CAL_FAIL;
                goto EXIT;
            }

            //selecting  auto calibration
            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_CTRL, 0x0F);

            //Wating time enalbe
            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, 0x40);
            app_sensor_jsa_enable();

            //capture noise data
            do
            {
                app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_STAT,
                                      &CALIB_STS); //wating for calibrating
                platform_delay_ms(10);
            }
            while (CALIB_STS == 0);

            app_sensor_i2c_read_16(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_AUTO_CDAT_L, val);
            cal_result += ((val[1] << 8) + val[0]);

            jsa_flash.sensor_xtalk = cal_result;

            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_MANU_CDAT_L,
                                   jsa_flash.sensor_xtalk & 0xFF);
            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_MANU_CDAT_H,
                                   (jsa_flash.sensor_xtalk >> 8) & 0x01);
            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_CTRL, 0x1F);
            app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_CALIB_STAT, 0x00);
        }
        break;

    case SENSOR_JSA_LOW_THRES:
    case SENSOR_JSA_HIGH_THRES:
    case SENSOR_JSA_READ_PS_DATA:
        {
            double conversion_time;
            double measure_time;
            uint8_t ps_pulse;
            uint8_t ps_time;
            uint8_t ps_filter;
            uint8_t itc_ps;
            uint8_t psconv;
            uint8_t num_avg;
            uint8_t itw_ps;

            //Wating time enalbe
            if (app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_SYSTEM_CONTROL, 0x40))
            {
                ret = JSA_CAL_FAIL;
                goto EXIT;
            }

            app_sensor_jsa_enable();

            //Calculate measuring time
            app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_PULSE, &ps_pulse);
            app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_TIME, &ps_time);
            app_sensor_i2c_read_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_FILTER, &ps_filter);
            itc_ps = ps_time >> 4;
            itw_ps = ps_pulse & 0x0F;
            psconv = ps_time & 0x0F;
            num_avg = ps_filter & 0x0F;

            conversion_time = 1.251 + 0.0005 * (itc_ps + 1) * (144 + 64 * (itw_ps + 1)) + 0.256 * (psconv + 1);
            measure_time = (num_avg + 1) * conversion_time;

            //Read PS data and calculate average for calibration
            for (uint8_t i = 0; i < SENSOR_JSA_CAL_TIMES; i++)
            {
                platform_delay_ms(measure_time);
                app_sensor_i2c_read_16(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_DATA_LOW, val);
                cal_result += ((val[1] << 8) + val[0]);
            }

            cal_result /= SENSOR_JSA_CAL_TIMES;

            if (cal_mode == SENSOR_JSA_LOW_THRES)
            {
                jsa_flash.sensor_low_thres = cal_result;

                app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_L,
                                       jsa_flash.sensor_low_thres);
                app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_H,
                                       jsa_flash.sensor_low_thres >> 8);

            }
            else if (cal_mode == SENSOR_JSA_HIGH_THRES)
            {
                jsa_flash.sensor_high_thres = cal_result;

                app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_L,
                                       jsa_flash.sensor_high_thres);
                app_sensor_i2c_write_8(SENSOR_ADDR_JSA1227, SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_H,
                                       jsa_flash.sensor_high_thres >> 8);
            }
        }
        break;
    }

    /* don't disable sensor to prevent ps data not valid */
    //app_sensor_jsa_disable();
    app_sensor_jsa_save_cal(&jsa_flash);

    *spp_result = (uint16_t)cal_result;

EXIT:
    APP_PRINT_TRACE6("sensor_vendor_jsa1227_calibration: ret %d, cal_mode %d, cal_result 0x%04x, sensor_xtalk 0x%04x, sensor_low_thres 0x%04x, sensor_high_thres 0x%04x",
                     ret, cal_mode, cal_result, jsa_flash.sensor_xtalk,
                     jsa_flash.sensor_low_thres, jsa_flash.sensor_high_thres);

    return ret;
}

void app_sensor_jsa_read_cal_data(uint8_t *buf)
{
    buf[0] = jsa_flash.sensor_xtalk & 0x00ff;
    buf[1] = (jsa_flash.sensor_xtalk & 0xff00) >> 8;

    buf[2] = jsa_flash.sensor_low_thres & 0x00ff;
    buf[3] = (jsa_flash.sensor_low_thres & 0xff00) >> 8;

    buf[4] = jsa_flash.sensor_high_thres & 0x00ff;
    buf[5] = (jsa_flash.sensor_high_thres & 0xff00) >> 8;

    buf[6] = jsa_flash.sensor_integration_time;
    buf[7] = jsa_flash.sensor_gain;
    buf[8] = jsa_flash.sensor_pulse;

    APP_PRINT_TRACE6("app_sensor_jsa_read_cal_data: %d %d %d %d %d %d", jsa_flash.sensor_xtalk,
                     jsa_flash.sensor_low_thres, jsa_flash.sensor_high_thres, jsa_flash.sensor_integration_time,
                     jsa_flash.sensor_gain, jsa_flash.sensor_pulse);

    return;
}

#ifdef __cplusplus
}
#endif

#endif
