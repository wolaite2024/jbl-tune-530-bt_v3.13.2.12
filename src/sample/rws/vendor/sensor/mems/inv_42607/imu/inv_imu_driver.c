#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include "inv_imu_driver.h"
#include "InvError.h"

#include "os_sched.h"
#include "trace.h"
#include "platform_utils.h"
#include "app_sensor_mems.h"

static int select_rcosc(struct inv_imu_device *s);
static int select_wuosc(struct inv_imu_device *s);
static int configure_serial_interface(struct inv_imu_device *s);
static int init_hardware_from_ui(struct inv_imu_device *s);
static int reload_otp(struct inv_imu_device *s);
extern inv_imu_sensor_data gyro_acc_sensor_data[FIFO_DEPTH];
extern inv_imv_report_data report_gyro_acc_sensor_data;

uint64_t inv_imu_get_time_us(void)
{
    //return os_sys_time_get();
    return 0;
}

void inv_imu_sleep_us(uint32_t temp)
{
    platform_delay_us(temp);
}

int inv_imu_init(struct inv_imu_device *s, struct inv_imu_serif *serif,
                 void (*sensor_event_cb)(inv_imu_sensor_event_t *event))
{
    int status = 0;
    memset(s, 0, sizeof(*s));

    s->transport.serif = *serif;
    /*Based on datasheet, start up time for register read/write after POR is 1ms and supply ramp time is 3ms*/
    inv_imu_sleep_us(3000);

    if ((status |= configure_serial_interface(s)) != 0)
    {
        return status;
    }
    /* register the callback to be executed each time inv_imu_get_data_from_fifo extracts
     * a packet from fifo or inv_imu_get_data_from_registers read data */
    s->sensor_event_cb = sensor_event_cb;

    /* initialize hardware */
    status |= init_hardware_from_ui(s);
    /* First Gyro data wrong after enable
     * Keep track of the time when enabling the Gyro and set a default value at init */
    s->gyro_start_time_us = UINT32_MAX;
    /* First Accel data wrong after enable
     * Keep track of the time when enabling the Accel and set a default value at init */
    s->accel_start_time_us = UINT32_MAX;
    // APP_PRINT_INFO1("inv_imu_init: status %d", status);
    return status;
}

int inv_imu_device_reset(struct inv_imu_device *s)
{
    int status = INV_ERROR_SUCCESS;
    uint8_t data;
    uint8_t saved_spi_config;
    status |= inv_imu_read_reg(s, CHIP_CONFIG_REG, 1, &saved_spi_config);
    /* Reset the internal registers and restores the default settings.
     * The bit automatically clears to 0 once the reset is done. */
    data = (uint8_t)SIGNAL_PATH_RESET_SOFT_RESET_CHIP_CONFIG_EN;
    status |= inv_imu_write_reg(s, SIGNAL_PATH_RESET, 1, &data);
    if (status)
    {
        return status;
    }
    /* Wait 1ms for soft reset to be effective before trying to perform any further read */
    inv_imu_sleep_us(1000);

    /* Force SPI-4w hardware configuration (so that next read is correct) */
    if (s->transport.serif.serif_type == UI_SPI4)
    {
        data = 1 << CHIP_CONFIG_REG_SPI_AP_4WIRES_POS;
        status |= inv_imu_write_reg(s, CHIP_CONFIG_REG, 1, &data);
    }
    /* Clear the reset done int */
    status |= inv_imu_read_reg(s, INT_STATUS, 1, &data);
    if (data != INT_STATUS_INT_STATUS_RESET_DONE_MASK)
    {
        status |= INV_ERROR_UNEXPECTED;
        return status;
    }
    /* Reload OTP procedure */
    status |= reload_otp(s);
    status |= inv_imu_write_reg(s, CHIP_CONFIG_REG, 1, &saved_spi_config);
    /* Init transport layer */
    inv_imu_init_transport(s);
    /* Read and set endianness for further processing */
    inv_imu_get_endianness(s);
    // APP_PRINT_INFO1("inv_imu_device_reset status %d,", status);
    return status;
}

int inv_imu_get_who_am_i(struct inv_imu_device *s, uint8_t *who_am_i)
{
    return inv_imu_read_reg(s, WHO_AM_I, 1, who_am_i);
}

static int select_rcosc(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
    data &= ~PWR_MGMT_0_ACCEL_LP_CLK_SEL_MASK;
    data |= PWR_MGMT_0_ACCEL_LP_CLK_RCOSC;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &data);

    return status;
}

static int select_wuosc(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
    data &= ~PWR_MGMT_0_ACCEL_LP_CLK_SEL_MASK;
    data |= PWR_MGMT_0_ACCEL_LP_CLK_WUOSC;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &data);

    return status;

}

static int reload_otp(struct inv_imu_device *s)
{
    int status = INV_ERROR_SUCCESS;
    uint8_t data;
    // APP_PRINT_INFO0("reload_otp 0");
    status |= inv_imu_switch_on_mclk(s);

    status |= inv_imu_read_reg(s, OTP_CONFIG_MREG_TOP1, 1, &data);
    data &= ~OTP_CONFIG_OTP_COPY_MODE_MASK;
    data |= OTP_CONFIG_OTP_COPY_TRIM;
    status |= inv_imu_write_reg(s, OTP_CONFIG_MREG_TOP1, 1, &data);
    // APP_PRINT_INFO0("reload_otp 1");
    status |= inv_imu_read_reg(s, 0x2806, 1, &data);
    data &= ~(0x02); /* Clear bit1 */
    data |= 0x04; /* Set bit 2*/
    status |= inv_imu_write_reg(s, 0x2806, 1, &data);
    // APP_PRINT_INFO0("reload_otp 2");
    inv_imu_sleep_us(300);

    data = 0x00;
    status |= inv_imu_write_reg(s, 0x2800, 4, &data);
    data = 0x01;
    status |= inv_imu_write_reg(s, 0x2804, 1, &data);
    data = 0x00;
    status |= inv_imu_write_reg(s, 0x2805, 1, &data);
    // APP_PRINT_INFO0("reload_otp 3");
    status |= inv_imu_read_reg(s, 0x2806, 1, &data);
    data |= 0x08; /* Set bit 3 */
    status |= inv_imu_write_reg(s, 0x2806, 1, &data);
    // APP_PRINT_INFO0("reload_otp 4");
    inv_imu_sleep_us(280);

    status |= inv_imu_switch_off_mclk(s);
    // APP_PRINT_INFO0("reload_otp 5");
    return status;
}


int inv_imu_enable_accel_low_power_mode(struct inv_imu_device *s)
{
    int status = 0;
    PWR_MGMT_0_ACCEL_MODE_t accel_mode;
    PWR_MGMT_0_GYRO_MODE_t  gyro_mode;
    ACCEL_CONFIG0_ODR_t acc_odr_bitfield;
    uint32_t accel_odr_us = 0;
    uint8_t pwr_mgmt0_reg;
    uint8_t accel_cfg_0_reg;
    uint8_t value;

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    accel_mode = (PWR_MGMT_0_ACCEL_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_ACCEL_MODE_MASK);
    gyro_mode = (PWR_MGMT_0_GYRO_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_GYRO_MODE_MASK);

    /* Check if the accelerometer is the only one enabled */
    if ((accel_mode != PWR_MGMT_0_ACCEL_MODE_LP) &&
        ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) || (gyro_mode == PWR_MGMT_0_GYRO_MODE_STANDBY)))
    {
        /* Get accelerometer's ODR for next required wait */
        status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
        acc_odr_bitfield = (ACCEL_CONFIG0_ODR_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_ODR_MASK);
        accel_odr_us = inv_imu_convert_odr_bitfield_to_us(acc_odr_bitfield);
        /* Select the RC OSC as clock source for the accelerometer */
        status |= select_rcosc(s);
    }

    /* Enable/Switch the accelerometer in/to low power mode */
    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1,
                               &pwr_mgmt0_reg); /* Read a new time because select_rcosc() modified it */
    pwr_mgmt0_reg &= ~PWR_MGMT_0_ACCEL_MODE_MASK;
    pwr_mgmt0_reg |= PWR_MGMT_0_ACCEL_MODE_LP;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    inv_imu_sleep_us(200);

    if ((accel_mode != PWR_MGMT_0_ACCEL_MODE_LP) &&
        ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) || (gyro_mode == PWR_MGMT_0_GYRO_MODE_STANDBY)))
    {
        /* Wait one accelerometer ODR before switching to the WU OSC */
        if (accel_odr_us > 200) /* if ODR is smaller than 200 us, we already waited for one ODR above. */
        {
            inv_imu_sleep_us(accel_odr_us - 200);
        }
        status |= select_wuosc(s);
    }

    if (accel_mode == PWR_MGMT_0_ACCEL_MODE_OFF)
    {
        /* First data are wrong after accel enable using IIR filter
         There is no signal that says accel start-up has completed and data are stable using FIR filter
         So keep track of the time at start-up to discard the invalid data, about 20ms after enable
        */
        if (s->fifo_is_used)
        {
            s->accel_start_time_us = inv_imu_get_time_us();
        }
    }

    /* Enable the automatic RCOSC power on so that FIFO is entirely powered on */
    status |= inv_imu_read_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &value);
    value &= ~FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK;
    status |= inv_imu_write_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &value);

    return status;
}

int inv_imu_enable_accel_low_noise_mode(struct inv_imu_device *s)
{
    int status = 0;
    PWR_MGMT_0_ACCEL_MODE_t accel_mode;
    PWR_MGMT_0_GYRO_MODE_t  gyro_mode;
    ACCEL_CONFIG0_ODR_t acc_odr_bitfield;
    uint32_t accel_odr_us;
    uint8_t pwr_mgmt0_reg;
    uint8_t accel_cfg_0_reg;

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    accel_mode = (PWR_MGMT_0_ACCEL_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_ACCEL_MODE_MASK);
    gyro_mode = (PWR_MGMT_0_GYRO_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_GYRO_MODE_MASK);
    /* Check if the accelerometer is the only one enabled */
    if ((accel_mode == PWR_MGMT_0_ACCEL_MODE_LP) &&
        ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) || (gyro_mode == PWR_MGMT_0_GYRO_MODE_STANDBY)))
    {
        /* Get accelerometer's ODR for next required wait */
        status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
        acc_odr_bitfield = (ACCEL_CONFIG0_ODR_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_ODR_MASK);
        accel_odr_us = inv_imu_convert_odr_bitfield_to_us(acc_odr_bitfield);
        /* Select the RC OSC as clock source for the accelerometer */
        status |= select_rcosc(s);
        /* Wait one accel ODR before switching to low noise mode */
        inv_imu_sleep_us(accel_odr_us);
    }

    /* Enable/Switch the accelerometer in/to low noise mode */
    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1,
                               &pwr_mgmt0_reg); /* Read a new time because select_rcosc() modified it */
    pwr_mgmt0_reg &= ~PWR_MGMT_0_ACCEL_MODE_MASK;
    pwr_mgmt0_reg |= PWR_MGMT_0_ACCEL_MODE_LN;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    inv_imu_sleep_us(200);

    if (accel_mode == PWR_MGMT_0_ACCEL_MODE_OFF)
    {
        /* First data are wrong after accel enable using IIR filter
         There is no signal that says accel start-up has completed and data are stable using FIR filter
         So keep track of the time at start-up to discard the invalid data, about 20ms after enable
        */
        if (s->fifo_is_used)
        {
            s->accel_start_time_us = inv_imu_get_time_us();
        }
    }

    return status;
}

int inv_imu_disable_accel(struct inv_imu_device *s)
{
    int status = 0;
    int stop_fifo_usage = 0;
    uint32_t accel_odr_us;
    PWR_MGMT_0_GYRO_MODE_t gyro_mode;
    ACCEL_CONFIG0_ODR_t acc_odr_bitfield;
    uint8_t pwr_mgmt0_reg;
    uint8_t accel_cfg_0_reg, fifo_cfg_6_reg;

    /* Get accelerometer's ODR for next required wait */
    status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
    acc_odr_bitfield = (ACCEL_CONFIG0_ODR_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_ODR_MASK);
    accel_odr_us = inv_imu_convert_odr_bitfield_to_us(acc_odr_bitfield);

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    gyro_mode = (PWR_MGMT_0_GYRO_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_GYRO_MODE_MASK);
    if ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) && s->fifo_is_used)
    {

        /* Accel is off and gyro is about to be turned off. Flush FIFO so that there is no old data at next enable time
         */
        stop_fifo_usage = 1;
    }

    /* Check if accel is the last sensor enabled and bit rcosc dis is not set */
    status |= inv_imu_read_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &fifo_cfg_6_reg);
    if ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) &&
        ((fifo_cfg_6_reg & FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK) == 0))
    {

        /* Disable the automatic RCOSC power on to avoid extra power consumption in sleep mode (all sensors and clocks off) */
        fifo_cfg_6_reg |= ((1 & FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK) <<
                           FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_POS);
        status |= inv_imu_write_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &fifo_cfg_6_reg);
        inv_imu_sleep_us(accel_odr_us);
    }

    pwr_mgmt0_reg &= ~PWR_MGMT_0_ACCEL_MODE_MASK;
    pwr_mgmt0_reg |= PWR_MGMT_0_ACCEL_MODE_OFF;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);

    if (stop_fifo_usage && s->fifo_is_used)
    {
        /* Reset FIFO explicitely so there is no data left in FIFO once all sensors are off */
        status |= inv_imu_reset_fifo(s);
    }

    return status;
}

void inv_imu_enter_full_sleep(struct inv_imu_device *s)
{
    uint8_t pwr_mgmt0_reg = 0;
    inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
}

int inv_imu_enable_gyro_low_noise_mode(struct inv_imu_device *s)
{
    int status = 0;
    PWR_MGMT_0_ACCEL_MODE_t accel_mode;
    PWR_MGMT_0_GYRO_MODE_t gyro_mode;
    ACCEL_CONFIG0_ODR_t acc_odr_bitfield;
    uint32_t accel_odr_us;
    uint8_t pwr_mgmt0_reg;
    uint8_t accel_cfg_0_reg;

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    accel_mode = (PWR_MGMT_0_ACCEL_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_ACCEL_MODE_MASK);
    gyro_mode = (PWR_MGMT_0_GYRO_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_GYRO_MODE_MASK);
    /* Check if the accelerometer is the only one enabled */
    if ((accel_mode == PWR_MGMT_0_ACCEL_MODE_LP) &&
        ((gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF) || (gyro_mode == PWR_MGMT_0_GYRO_MODE_STANDBY)))
    {
        /* Get accelerometer's ODR for next required wait */
        status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
        acc_odr_bitfield = (ACCEL_CONFIG0_ODR_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_ODR_MASK);
        accel_odr_us = inv_imu_convert_odr_bitfield_to_us(acc_odr_bitfield);
        /* Select the RC OSC as clock source for the accelerometer */
        status |= select_rcosc(s);
        /* Wait one accel ODR before enabling the gyroscope */
        inv_imu_sleep_us(accel_odr_us);
    }

    /* Enable/Switch the gyroscope in/to low noise mode */
    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1,
                               &pwr_mgmt0_reg); /* Read a new time because select_rcosc() modified it */
    pwr_mgmt0_reg &= ~PWR_MGMT_0_GYRO_MODE_MASK;
    pwr_mgmt0_reg |= (uint8_t)PWR_MGMT_0_GYRO_MODE_LN;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    inv_imu_sleep_us(200);

    if (gyro_mode == PWR_MGMT_0_GYRO_MODE_OFF)
    {
        /* First data are wrong after gyro enable using IIR filter
         There is no signal that says Gyro start-up has completed and data are stable using FIR filter
         and the Gyro max start-up time is 40ms
         So keep track of the time at start-up to discard the invalid data, about 60ms after enable
        */
        if (s->fifo_is_used)
        {
            s->gyro_start_time_us = inv_imu_get_time_us();
        }
    }

    return status;
}

int inv_imu_disable_gyro(struct inv_imu_device *s)
{
    int status = 0;
    int stop_fifo_usage = 0;
    ACCEL_CONFIG0_ODR_t acc_odr_bitfield;
    uint32_t accel_odr_us;
    PWR_MGMT_0_ACCEL_MODE_t accel_mode;
    uint8_t pwr_mgmt0_reg;
    uint8_t accel_cfg_0_reg, fifo_cfg_6_reg;

    /* Get accelerometer's ODR for next required wait */
    status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
    acc_odr_bitfield = (ACCEL_CONFIG0_ODR_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_ODR_MASK);
    accel_odr_us = inv_imu_convert_odr_bitfield_to_us(acc_odr_bitfield);

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);
    accel_mode = (PWR_MGMT_0_ACCEL_MODE_t)(pwr_mgmt0_reg & PWR_MGMT_0_ACCEL_MODE_MASK);

    if ((accel_mode == PWR_MGMT_0_ACCEL_MODE_OFF) && s->fifo_is_used)
    {
        /* Accel is off and gyro is about to be turned off. Flush FIFO so that there is no old data at next enable time
         */
        stop_fifo_usage = 1;
    }

    /* Check if the accelerometer is enabled in low power mode */
    if (accel_mode == PWR_MGMT_0_ACCEL_MODE_LP)
    {
        /* Select the RC OSC as clock source for the accelerometer */
        status |= select_rcosc(s);
    }

    /* Check if gyro is the last sensor enabled and bit rcosc dis is not set */
    status |= inv_imu_read_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &fifo_cfg_6_reg);
    if ((accel_mode == PWR_MGMT_0_ACCEL_MODE_OFF) &&
        ((fifo_cfg_6_reg & FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK) == 0))
    {

        GYRO_CONFIG0_ODR_t gyro_odr_bitfield;
        uint32_t gyro_odr_us;
        uint8_t gyro_cfg_0_reg;

        /* Read gyro odr to apply it to the sleep */
        status |= inv_imu_read_reg(s, GYRO_CONFIG0, 1, &gyro_cfg_0_reg);
        gyro_odr_bitfield = (GYRO_CONFIG0_ODR_t)(gyro_cfg_0_reg & GYRO_CONFIG0_GYRO_ODR_MASK);
        gyro_odr_us  = inv_imu_convert_odr_bitfield_to_us(gyro_odr_bitfield);

        /* Disable the automatic RCOSC power on to avoid extra power consumption in sleep mode (all sensors and clocks off) */
        fifo_cfg_6_reg |= ((1 & FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK) <<
                           FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_POS);
        status |= inv_imu_write_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &fifo_cfg_6_reg);
        inv_imu_sleep_us(gyro_odr_us);
    }

    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1,
                               &pwr_mgmt0_reg); /* Read a new time because select_rcosc() modified it */
    pwr_mgmt0_reg &= ~PWR_MGMT_0_GYRO_MODE_MASK;
    pwr_mgmt0_reg |= PWR_MGMT_0_GYRO_MODE_OFF;
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &pwr_mgmt0_reg);

    if (accel_mode == PWR_MGMT_0_ACCEL_MODE_LP)
    {
        /* Wait based on accelerometer ODR */
        inv_imu_sleep_us(2 * accel_odr_us);
        /* Select the WU OSC as clock source for the accelerometer */
        status |= select_wuosc(s);
    }

    if (stop_fifo_usage && s->fifo_is_used)
    {
        /* Reset FIFO explicitely so there is no data left in FIFO once all sensors are off */
        status |= inv_imu_reset_fifo(s);
    }

    return status;
}

int inv_imu_enable_fsync(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;

    status |= inv_imu_switch_on_mclk(s);

    //Enable Fsync
    status |= inv_imu_read_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);
    value &= ~FSYNC_CONFIG_FSYNC_UI_SEL_MASK;
    value |= (uint8_t)FSYNC_CONFIG_UI_SEL_TEMP;
    status |= inv_imu_write_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);

    status |= inv_imu_read_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &value);
    value &= ~TMST_CONFIG1_TMST_FSYNC_EN_MASK;
    value |= TMST_CONFIG1_TMST_FSYNC_EN;
    status |= inv_imu_write_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &value);

    status |= inv_imu_switch_off_mclk(s);

    return status;
}

int inv_imu_disable_fsync(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;

    status |= inv_imu_switch_on_mclk(s);

    // Disable Fsync
    status |= inv_imu_read_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);
    value &= ~FSYNC_CONFIG_FSYNC_UI_SEL_MASK;
    value |= (uint8_t)FSYNC_CONFIG_UI_SEL_NO;
    status |= inv_imu_write_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);

    status |= inv_imu_read_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &value);
    value &= ~TMST_CONFIG1_TMST_FSYNC_EN_MASK;
    value |= TMST_CONFIG1_TMST_FSYNC_DIS;
    status |= inv_imu_write_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &value);

    status |= inv_imu_switch_off_mclk(s);

    return status;
}

int inv_imu_configure_wom(struct inv_imu_device *s,
                          const uint8_t wom_x_th, const uint8_t wom_y_th, const uint8_t wom_z_th,
                          WOM_CONFIG_WOM_INT_MODE_t wom_int, WOM_CONFIG_WOM_INT_DUR_t wom_dur)
{
    int status = 0;
    uint8_t data[3];
    uint8_t value;

    data[0] = wom_x_th; // Set X threshold
    data[1] = wom_y_th; // Set Y threshold
    data[2] = wom_z_th; // Set Z threshold
    status |= inv_imu_write_reg(s, ACCEL_WOM_X_THR_MREG_TOP1, sizeof(data), &data[0]);

    // Compare current sample with the previous sample and WOM from the 3 axis are ORed or ANDed to produce WOM signal.
    status |= inv_imu_read_reg(s, WOM_CONFIG, 1, &value);
    value &= ~WOM_CONFIG_WOM_INT_MODE_MASK;
    value |= (uint8_t)WOM_CONFIG_WOM_MODE_CMP_PREV | (uint8_t)wom_int;

    // Configure the number of overthreshold event to wait before producing the WOM signal.
    value &= ~WOM_CONFIG_WOM_INT_DUR_MASK;
    value |= (uint8_t)wom_dur;
    status |= inv_imu_write_reg(s, WOM_CONFIG, 1, &value);

    return status;
}

int inv_imu_enable_wom(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;
    inv_imu_interrupt_parameter_t config_int = {(inv_imu_interrupt_value)0};

    /* Disable fifo threshold int1 */
    status |= inv_imu_get_config_int1(s, &config_int);
    config_int.INV_FIFO_THS = INV_IMU_DISABLE;
    status |= inv_imu_set_config_int1(s, &config_int);

    s->wom_enable = 1;

    /* Enable WOM */
    status |= inv_imu_read_reg(s, WOM_CONFIG, 1, &value);
    value &= ~WOM_CONFIG_WOM_EN_MASK;
    value |= (uint8_t)WOM_CONFIG_WOM_EN_ENABLE;
    status |= inv_imu_write_reg(s, WOM_CONFIG, 1, &value);


    return status;
}

int inv_imu_disable_wom(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;
    inv_imu_interrupt_parameter_t config_int = {(inv_imu_interrupt_value)0};

    s->wom_enable = 0;

    /* Disable WOM */
    status |= inv_imu_read_reg(s, WOM_CONFIG, 1, &value);
    value &= ~WOM_CONFIG_WOM_EN_MASK;
    value |= WOM_CONFIG_WOM_EN_DISABLE;
    status |= inv_imu_write_reg(s, WOM_CONFIG, 1, &value);

    /* Enable fifo threshold int1 */
    status |= inv_imu_get_config_int1(s, &config_int);
    config_int.INV_FIFO_THS = INV_IMU_ENABLE;
    status |= inv_imu_set_config_int1(s, &config_int);

    return status;
}

int inv_imu_get_config_int1(struct inv_imu_device *s,
                            inv_imu_interrupt_parameter_t *interrupt_to_configure)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, INT_SOURCE0, 1, &data);
    interrupt_to_configure->INV_UI_FSYNC  = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE0_INT_FSYNC_INT1_EN_MASK)  >> INT_SOURCE0_INT_FSYNC_INT1_EN_POS);
    interrupt_to_configure->INV_UI_DRDY   = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE0_INT_DRDY_INT1_EN_MASK)   >> INT_SOURCE0_INT_DRDY_INT1_EN_POS);
    interrupt_to_configure->INV_FIFO_THS  = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE0_INT_FIFO_THS_INT1_EN_MASK)  >> INT_SOURCE0_INT_FIFO_THS_INT1_EN_POS);
    interrupt_to_configure->INV_FIFO_FULL = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE0_INT_FIFO_FULL_INT1_EN_MASK) >> INT_SOURCE0_INT_FIFO_FULL_INT1_EN_POS);

    status |= inv_imu_read_reg(s, INT_SOURCE1, 1, &data);
    interrupt_to_configure->INV_SMD   = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE1_INT_SMD_INT1_EN_MASK)   >> INT_SOURCE1_INT_SMD_INT1_EN_POS);
    interrupt_to_configure->INV_WOM_X = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE1_INT_WOM_X_INT1_EN_MASK) >> INT_SOURCE1_INT_WOM_X_INT1_EN_POS);
    interrupt_to_configure->INV_WOM_Y = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK) >> INT_SOURCE1_INT_WOM_Y_INT1_EN_POS);
    interrupt_to_configure->INV_WOM_Z = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK) >> INT_SOURCE1_INT_WOM_Z_INT1_EN_POS);

    status |= inv_imu_read_reg(s, INT_SOURCE6_MREG_TOP1, 1, &data);
    interrupt_to_configure->INV_FF            = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE6_INT_FF_INT1_EN_MASK)            >> INT_SOURCE6_INT_FF_INT1_EN_POS);
    interrupt_to_configure->INV_LOWG          = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE6_INT_LOWG_INT1_EN_MASK)          >> INT_SOURCE6_INT_LOWG_INT1_EN_POS);
    interrupt_to_configure->INV_STEP_DET      = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE6_INT_STEP_DET_INT1_EN_MASK)      >> INT_SOURCE6_INT_STEP_DET_INT1_EN_POS);
    interrupt_to_configure->INV_STEP_CNT_OVFL = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_MASK) >> INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_POS);
    interrupt_to_configure->INV_TILT_DET      = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE6_INT_TILT_DET_INT1_EN_MASK)      >> INT_SOURCE6_INT_TILT_DET_INT1_EN_POS);

    return status;
}

int inv_imu_get_config_int2(struct inv_imu_device *s,
                            inv_imu_interrupt_parameter_t *interrupt_to_configure)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, INT_SOURCE3, 1, &data);
    interrupt_to_configure->INV_UI_FSYNC  = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE3_INT_FSYNC_INT2_EN_MASK)  >> INT_SOURCE3_INT_FSYNC_INT2_EN_POS);
    interrupt_to_configure->INV_UI_DRDY   = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE3_INT_DRDY_INT2_EN_MASK)   >> INT_SOURCE3_INT_DRDY_INT2_EN_POS);
    interrupt_to_configure->INV_FIFO_THS  = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE3_INT_FIFO_THS_INT2_EN_MASK)  >> INT_SOURCE3_INT_FIFO_THS_INT2_EN_POS);
    interrupt_to_configure->INV_FIFO_FULL = (inv_imu_interrupt_value)((data &
                                                                       INT_SOURCE3_INT_FIFO_FULL_INT2_EN_MASK) >> INT_SOURCE3_INT_FIFO_FULL_INT2_EN_POS);

    status |= inv_imu_read_reg(s, INT_SOURCE4, 1, &data);
    interrupt_to_configure->INV_SMD   = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE4_INT_SMD_INT2_EN_MASK)   >> INT_SOURCE4_INT_SMD_INT2_EN_POS);
    interrupt_to_configure->INV_WOM_X = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE4_INT_WOM_X_INT2_EN_MASK) >> INT_SOURCE4_INT_WOM_X_INT2_EN_POS);
    interrupt_to_configure->INV_WOM_Y = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE4_INT_WOM_Y_INT2_EN_MASK) >> INT_SOURCE4_INT_WOM_Y_INT2_EN_POS);
    interrupt_to_configure->INV_WOM_Z = (inv_imu_interrupt_value)((data &
                                                                   INT_SOURCE4_INT_WOM_Z_INT2_EN_MASK) >> INT_SOURCE4_INT_WOM_Z_INT2_EN_POS);

    status |= inv_imu_read_reg(s, INT_SOURCE7_MREG_TOP1, 1, &data);
    interrupt_to_configure->INV_FF            = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE7_INT_FF_INT2_EN_MASK)            >> INT_SOURCE7_INT_FF_INT2_EN_POS);
    interrupt_to_configure->INV_LOWG          = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE7_INT_LOWG_INT2_EN_MASK)          >> INT_SOURCE7_INT_LOWG_INT2_EN_POS);
    interrupt_to_configure->INV_STEP_DET      = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE7_INT_STEP_DET_INT2_EN_MASK)      >> INT_SOURCE7_INT_STEP_DET_INT2_EN_POS);
    interrupt_to_configure->INV_STEP_CNT_OVFL = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_MASK) >> INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_POS);
    interrupt_to_configure->INV_TILT_DET      = (inv_imu_interrupt_value)((
                                                                              data & INT_SOURCE7_INT_TILT_DET_INT2_EN_MASK)      >> INT_SOURCE7_INT_TILT_DET_INT2_EN_POS);

    return status;
}

int inv_imu_set_config_int1(struct inv_imu_device *s,
                            inv_imu_interrupt_parameter_t *interrupt_to_configure)
{
    int status = 0;
    uint8_t data[2];

    status |= inv_imu_read_reg(s, INT_SOURCE0, 2, &data[0]);

    data[0] &= ~(INT_SOURCE0_INT_FSYNC_INT1_EN_MASK
                 | INT_SOURCE0_INT_DRDY_INT1_EN_MASK
                 | INT_SOURCE0_INT_FIFO_THS_INT1_EN_MASK
                 | INT_SOURCE0_INT_FIFO_FULL_INT1_EN_MASK);
    data[0] |= ((interrupt_to_configure->INV_UI_FSYNC != 0)  << INT_SOURCE0_INT_FSYNC_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_UI_DRDY != 0)   << INT_SOURCE0_INT_DRDY_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_FIFO_THS != 0)  << INT_SOURCE0_INT_FIFO_THS_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_FIFO_FULL != 0) << INT_SOURCE0_INT_FIFO_FULL_INT1_EN_POS);

    data[1] &= ~(INT_SOURCE1_INT_SMD_INT1_EN_MASK
                 | INT_SOURCE1_INT_WOM_X_INT1_EN_MASK
                 | INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK
                 | INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK);
    data[1] |= ((interrupt_to_configure->INV_SMD   != 0) << INT_SOURCE1_INT_SMD_INT1_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_X != 0) << INT_SOURCE1_INT_WOM_X_INT1_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_Y != 0) << INT_SOURCE1_INT_WOM_Y_INT1_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_Z != 0) << INT_SOURCE1_INT_WOM_Z_INT1_EN_POS);

    // APP_PRINT_INFO3("inv_imu_set_config_int1, addr: 0x%x, data[0]: 0x%x, data[1]: 0x%x",
    //                 INT_SOURCE0,
    //                 data[0],
    //                 data[1]);
    status |= inv_imu_write_reg(s, INT_SOURCE0, 2, &data[0]);

    inv_imu_read_reg(s, INT_SOURCE0, 1, &data[0]);
    // APP_PRINT_INFO2("inv_imu_set_config_int1, addr: 0x%x, data[0]: 0x%x",
    //                 INT_SOURCE0,
    //                 data[0]);
    inv_imu_read_reg(s, INT_SOURCE1, 1, &data[0]);
    // APP_PRINT_INFO2("inv_imu_set_config_int1, addr: 0x%x, data[0]: 0x%x",
    //                 INT_SOURCE1,
    //                 data[0]);

    status |= inv_imu_read_reg(s, INT_SOURCE6_MREG_TOP1, 1, &data[0]);

    data[0] &= ~(INT_SOURCE6_INT_FF_INT1_EN_MASK
                 | INT_SOURCE6_INT_LOWG_INT1_EN_POS
                 | INT_SOURCE6_INT_STEP_DET_INT1_EN_MASK
                 | INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_MASK
                 | INT_SOURCE6_INT_TILT_DET_INT1_EN_MASK);
    data[0] |= ((interrupt_to_configure->INV_FF != 0)            << INT_SOURCE6_INT_FF_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_LOWG != 0)          << INT_SOURCE6_INT_LOWG_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_STEP_DET != 0)      <<
                INT_SOURCE6_INT_STEP_DET_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_STEP_CNT_OVFL != 0) <<
                INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_TILT_DET != 0)      <<
                INT_SOURCE6_INT_TILT_DET_INT1_EN_POS);
    status |= inv_imu_write_reg(s, INT_SOURCE6_MREG_TOP1, 1, &data[0]);

    return status;
}

int inv_imu_set_config_int2(struct inv_imu_device *s,
                            inv_imu_interrupt_parameter_t *interrupt_to_configure)
{
    int status = 0;
    uint8_t data[2];

    status |= inv_imu_read_reg(s, INT_SOURCE3, 2, &data[0]);

    data[0] &= ~(INT_SOURCE3_INT_FSYNC_INT2_EN_MASK
                 | INT_SOURCE3_INT_DRDY_INT2_EN_MASK
                 | INT_SOURCE3_INT_FIFO_THS_INT2_EN_MASK
                 | INT_SOURCE3_INT_FIFO_FULL_INT2_EN_MASK);
    data[0] |= ((interrupt_to_configure->INV_UI_FSYNC != 0)  << INT_SOURCE3_INT_FSYNC_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_UI_DRDY != 0)   << INT_SOURCE3_INT_DRDY_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_FIFO_THS != 0)  << INT_SOURCE3_INT_FIFO_THS_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_FIFO_FULL != 0) << INT_SOURCE3_INT_FIFO_FULL_INT2_EN_POS);

    data[1] &= ~(INT_SOURCE4_INT_SMD_INT2_EN_MASK
                 | INT_SOURCE4_INT_WOM_X_INT2_EN_MASK
                 | INT_SOURCE4_INT_WOM_Y_INT2_EN_MASK
                 | INT_SOURCE4_INT_WOM_Z_INT2_EN_MASK);
    data[1] |= ((interrupt_to_configure->INV_SMD   != 0) << INT_SOURCE4_INT_SMD_INT2_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_X != 0) << INT_SOURCE4_INT_WOM_X_INT2_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_Y != 0) << INT_SOURCE4_INT_WOM_Y_INT2_EN_POS);
    data[1] |= ((interrupt_to_configure->INV_WOM_Z != 0) << INT_SOURCE4_INT_WOM_Z_INT2_EN_POS);

    data[0] = 0x0;
    data[0] |= INT_SOURCE3_INT_FIFO_THS_INT2_EN_MASK;
    //data[0] |= INT_SOURCE3_INT_DRDY_INT2_EN_MASK;
    data[1] = 0x0;
    // APP_PRINT_INFO3("inv_imu_set_config_int2, write, addr: 0x%x, data[0]: 0x%x, data[1]: 0x%x",
    //                 INT_SOURCE3,
    //                 data[0],
    //                 data[1]);
    status |= inv_imu_write_reg(s, INT_SOURCE3, 2, &data[0]);
    inv_imu_read_reg(s, INT_SOURCE3, 1, &data[0]);
    // APP_PRINT_INFO2("inv_imu_set_config_int2, read, addr: 0x%x, data[0]: 0x%x",
    //                 INT_SOURCE3,
    //                 data[0]);
    inv_imu_read_reg(s, INT_SOURCE4, 1, &data[0]);
    // APP_PRINT_INFO2("inv_imu_set_config_int2, read, addr: 0x%x, data[0]: 0x%x",
    //                 INT_SOURCE4,
    //                 data[0]);
    status |= inv_imu_read_reg(s, INT_SOURCE7_MREG_TOP1, 1, &data[0]);

    data[0] &= ~(INT_SOURCE7_INT_FF_INT2_EN_MASK
                 | INT_SOURCE7_INT_LOWG_INT2_EN_MASK
                 | INT_SOURCE7_INT_STEP_DET_INT2_EN_MASK
                 | INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_MASK
                 | INT_SOURCE7_INT_TILT_DET_INT2_EN_MASK);
    data[0] |= ((interrupt_to_configure->INV_FF != 0)            << INT_SOURCE7_INT_FF_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_LOWG != 0)          << INT_SOURCE7_INT_LOWG_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_STEP_DET != 0)      <<
                INT_SOURCE7_INT_STEP_DET_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_STEP_CNT_OVFL != 0) <<
                INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_POS);
    data[0] |= ((interrupt_to_configure->INV_TILT_DET != 0)      <<
                INT_SOURCE7_INT_TILT_DET_INT2_EN_POS);

    status |= inv_imu_write_reg(s, INT_SOURCE7_MREG_TOP1, 1, &data[0]);

    return status;
}

int inv_imu_get_data_from_registers(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t int_status;
    uint8_t temperature[2];
    uint8_t accel[ACCEL_DATA_SIZE];
    uint8_t gyro[GYRO_DATA_SIZE];
    inv_imu_sensor_event_t event;

    /* Ensure data ready status bit is set */
    if ((status |= inv_imu_read_reg(s, INT_STATUS_DRDY, 1, &int_status)))
    {
        return status;
    }

    if (int_status & INT_STATUS_DRDY_INT_STATUS_DRDY_MASK)
    {

        status |= inv_imu_read_reg(s, TEMP_DATA0_UI, 2, &temperature[0]);

        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
        {
            event.temperature = (((int16_t)temperature[0]) << 8) | temperature[1];
        }
        else
        {
            event.temperature = (((int16_t)temperature[1]) << 8) | temperature[0];
        }

        status |= inv_imu_read_reg(s, ACCEL_DATA_X0_UI, ACCEL_DATA_SIZE, &accel[0]);

        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
        {
            event.accel[0] = (accel[0] << 8) | accel[1];
            event.accel[1] = (accel[2] << 8) | accel[3];
            event.accel[2] = (accel[4] << 8) | accel[5];
        }
        else
        {
            event.accel[0] = (accel[1] << 8) | accel[0];
            event.accel[1] = (accel[3] << 8) | accel[2];
            event.accel[2] = (accel[5] << 8) | accel[4];
        }

        status |= inv_imu_read_reg(s, GYRO_DATA_X0_UI, GYRO_DATA_SIZE, &gyro[0]);

        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
        {
            event.gyro[0] = (gyro[0] << 8) | gyro[1];
            event.gyro[1] = (gyro[2] << 8) | gyro[3];
            event.gyro[2] = (gyro[4] << 8) | gyro[5];
        }
        else
        {
            event.gyro[0] = (gyro[1] << 8) | gyro[0];
            event.gyro[1] = (gyro[3] << 8) | gyro[2];
            event.gyro[2] = (gyro[5] << 8) | gyro[4];
        }

        /* call sensor event callback */
        if (s->sensor_event_cb)
        {
            s->sensor_event_cb(&event);
        }

        /* Only perform dummy write if needed (fix for HW bug: COR-1672) */
        if (s->transport.serif.serif_type == UI_I2C)
        {
            status |= inv_imu_write_reg(s, WHO_AM_I, 0, 0);
        }
    }
    /*else: Data Ready was not reached*/

    return status;
}

uint8_t inv_imu_is_required_get_fifo_data(struct inv_imu_device *s)
{
    if (inv_imu_get_interrupt_status(s) == 0)
    {
        return 0;
    }
    return 1;
}

uint8_t inv_imu_get_interrupt_status(struct inv_imu_device *s)
{
    uint8_t int_status;
    int status = 0;
    if ((status |= inv_imu_read_reg(s, INT_STATUS, 1, &int_status)))
    {
        status = 0x0;
    }

    if (!(int_status & INT_STATUS_INT_STATUS_FIFO_THS_MASK))
    {
        inv_imu_reset_fifo(s);
        status = 1;
    }
    // APP_PRINT_INFO2("inv_imu_get_interrupt_status: 0x%x int_status %d", status, int_status);
    return status;
}

int inv_imu_get_data_from_fifo_wo_int_check(struct inv_imu_device *s)
{
    int status = 0;
    // uint8_t int_status;
    uint16_t packet_count_i, packet_count, total_packet_count = 0;
    uint16_t packet_size = FIFO_HEADER_SIZE + FIFO_ACCEL_DATA_SIZE + FIFO_GYRO_DATA_SIZE +
                           FIFO_TEMP_DATA_SIZE + FIFO_TS_FSYNC_SIZE;
    fifo_header_t *header;

    /* Ensure data ready status bit is set */
//  if((status |= inv_imu_read_reg(s, INT_STATUS, 1, &int_status)))
//      return status;

//  if((int_status & INT_STATUS_INT_STATUS_FIFO_THS_MASK) || (int_status & INT_STATUS_INT_STATUS_FIFO_FULL_MASK))
    {
        uint8_t data[2];

        /*
         * Force the idle bit to disable
         * The chip expects no delay between reading the FIFO count and reading FIFO data.
         * If the program is interupted for more than 1ms, the chip will switch the clock,
         * and we won't guarantee the proper functioning of the "read fifo" operation.
         */
        status |= inv_imu_switch_on_mclk(s);

        /* FIFO record mode configured at driver init, so we read packet number, not byte count */
        if ((status |= inv_imu_read_reg(s, FIFO_BYTE_COUNT1, 2, &data[0])) != INV_ERROR_SUCCESS)
        {
            status |= inv_imu_switch_off_mclk(s);
            return status;
        }

        total_packet_count = (uint16_t)(data[0] | (data[1] << 8));
        if (total_packet_count > FIFO_DEPTH)
        {
            total_packet_count = FIFO_DEPTH;
        }
        packet_count = total_packet_count;
        report_gyro_acc_sensor_data.packet_count = FIFO_DEPTH;

        while (packet_count > 0)
        {
            uint16_t invalid_frame_cnt = 0;
            /* Read FIFO only when data is expected in FIFO */
            /* fifo_idx type variable must be large enough to parse the FIFO_MIRRORING_SIZE */
            uint16_t fifo_idx = 0;

            if (s->fifo_highres_enabled)
            {
                packet_size = FIFO_20BYTES_PACKET_SIZE;
            }

            if ((status |= inv_imu_read_reg(s, FIFO_DATA_REG, packet_size * packet_count, s->fifo_data)))
            {
                /* sensor data is in FIFO according to FIFO_COUNT but failed to read FIFO,
                   reset FIFO and try next chance */
                status |= inv_imu_reset_fifo(s);
                status |= inv_imu_switch_off_mclk(s);
                return status;
            }

            for (packet_count_i = 0; packet_count_i < packet_count; packet_count_i++)
            {
                inv_imu_sensor_event_t event = {0};
                event.sensor_mask = 0;
                header = (fifo_header_t *) &s->fifo_data[fifo_idx];
                fifo_idx += FIFO_HEADER_SIZE;

                // if (packet_count_i < FIFO_DEPTH)
                // {
                //     APP_PRINT_INFO4("inv_imu_get_data_from_fifo_wo_int_check: packet_count: 0x%x, packet_count_i: 0x%x, Byte: 0x%x, msg_bit:0x%x",
                //                     packet_count,
                //                     packet_count_i,
                //                     header->Byte,
                //                     header->bits.msg_bit);
                // }

                /* Decode invalid frame, this typically happens if packet_count is greater
                than 2 in case of WOM event since FIFO_THS IRQ is disabled if WOM is enabled,
                and we wake up only upon a WOM event so we can have more than 1 ACC packet in FIFO
                and we do not wait the oscillator wake-up time so we will receive 1 invalid packet,
                which we will read again upon next FIFO read operation thanks to while() loop*/
                if ((header->Byte == 0x80))
                {
                    uint8_t is_invalid_frame = 1;
                    /* Check N-FIFO_HEADER_SIZE remaining bytes are all 0 to be invalid frame */
                    for (uint8_t i = 0 ; i < (packet_size - FIFO_HEADER_SIZE) ; i++)
                    {
                        if (s->fifo_data[fifo_idx + i])
                        {
                            is_invalid_frame = 0;
                            break;
                        }
                    }
                    /* In case of invalid frame read FIFO will be retried for this packet */
                    invalid_frame_cnt += is_invalid_frame;
                }
                else
                {
                    /* Decode packet */
                    if (header->bits.accel_bit)
                    {
                        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
                        {
                            event.accel[0] = (s->fifo_data[0 + fifo_idx] << 8) | s->fifo_data[1 + fifo_idx];
                            event.accel[1] = (s->fifo_data[2 + fifo_idx] << 8) | s->fifo_data[3 + fifo_idx];
                            event.accel[2] = (s->fifo_data[4 + fifo_idx] << 8) | s->fifo_data[5 + fifo_idx];
                        }
                        else
                        {
                            event.accel[0] = (s->fifo_data[1 + fifo_idx] << 8) | s->fifo_data[0 + fifo_idx];
                            event.accel[1] = (s->fifo_data[3 + fifo_idx] << 8) | s->fifo_data[2 + fifo_idx];
                            event.accel[2] = (s->fifo_data[5 + fifo_idx] << 8) | s->fifo_data[4 + fifo_idx];
                        }
                        fifo_idx += FIFO_ACCEL_DATA_SIZE;
                    }

                    if (header->bits.gyro_bit)
                    {
                        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
                        {
                            event.gyro[0] = (s->fifo_data[0 + fifo_idx] << 8) | s->fifo_data[1 + fifo_idx];
                            event.gyro[1] = (s->fifo_data[2 + fifo_idx] << 8) | s->fifo_data[3 + fifo_idx];
                            event.gyro[2] = (s->fifo_data[4 + fifo_idx] << 8) | s->fifo_data[5 + fifo_idx];
                        }
                        else
                        {
                            event.gyro[0] = (s->fifo_data[1 + fifo_idx] << 8) | s->fifo_data[0 + fifo_idx];
                            event.gyro[1] = (s->fifo_data[3 + fifo_idx] << 8) | s->fifo_data[2 + fifo_idx];
                            event.gyro[2] = (s->fifo_data[5 + fifo_idx] << 8) | s->fifo_data[4 + fifo_idx];
                        }
                        fifo_idx += FIFO_GYRO_DATA_SIZE;
                    }

                    if ((header->bits.accel_bit) || (header->bits.gyro_bit))
                    {
                        // The coarse temperature (8 or 16B FIFO packet format) range is ± 64 degrees with 0.5°C resolution
                        // but the fine temperature range (2 bytes) (20B FIFO packet format) is ± 256 degrees with (1/128)°C resolution
                        if (header->bits.twentybits_bit)
                        {
                            if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
                            {
                                event.temperature = (((int16_t)s->fifo_data[0 + fifo_idx]) << 8) | s->fifo_data[1 + fifo_idx];
                            }
                            else
                            {
                                event.temperature = (((int16_t)s->fifo_data[1 + fifo_idx]) << 8) | s->fifo_data[0 + fifo_idx];
                            }
                            fifo_idx += FIFO_TEMP_DATA_SIZE + FIFO_TEMP_HIGH_RES_SIZE;

                            /* new temperature data */
                            if (event.temperature != INVALID_VALUE_FIFO)
                            {
                                event.sensor_mask |= (1 << INV_SENSOR_TEMPERATURE);
                            }
                        }
                        else
                        {
                            /* cast to int8_t since FIFO is in 16 bits mode (temperature on 8 bits) */
                            event.temperature = (int8_t)s->fifo_data[0 + fifo_idx];
                            fifo_idx += FIFO_TEMP_DATA_SIZE;

                            /* new temperature data */
                            if (event.temperature != INVALID_VALUE_FIFO_1B)
                            {
                                event.sensor_mask |= (1 << INV_SENSOR_TEMPERATURE);
                            }
                        }
                    }

                    if ((header->bits.timestamp_bit) || (header->bits.fsync_bit))
                    {
                        if (s->endianness_data == INTF_CONFIG0_DATA_BIG_ENDIAN)
                        {
                            event.timestamp_fsync = (s->fifo_data[0 + fifo_idx] << 8) | s->fifo_data[1 + fifo_idx];
                        }
                        else
                        {
                            event.timestamp_fsync = (s->fifo_data[1 + fifo_idx] << 8) | s->fifo_data[0 + fifo_idx];
                        }
                        fifo_idx += FIFO_TS_FSYNC_SIZE;
                        /* new fsync event */
                        if (header->bits.fsync_bit)
                        {
                            event.sensor_mask |= (1 << INV_SENSOR_FSYNC_EVENT);
                        }
                    }

                    if (header->bits.accel_bit)
                    {
                        if ((event.accel[0] != INVALID_VALUE_FIFO) &&
                            (event.accel[1] != INVALID_VALUE_FIFO) &&
                            (event.accel[2] != INVALID_VALUE_FIFO))
                        {
                            if (s->accel_start_time_us == UINT32_MAX)
                            {
                                event.sensor_mask |= (1 << INV_SENSOR_ACCEL);
                            }
                            else
                            {
                                if (!header->bits.fsync_bit)
                                {
                                    /* First data are wrong after accel enable using IIR filter
                                    There is no signal that says accel start-up has completed and data are stable using FIR filter
                                    So test the delta time from accel start-up to valid accel data sampled and discard any accel data sampled before 20ms
                                    */
                                    if ((inv_imu_get_time_us() - s->accel_start_time_us) >= ACC_STARTUP_TIME_US)
                                    {
                                        s->accel_start_time_us = UINT32_MAX;
                                        event.sensor_mask |= (1 << INV_SENSOR_ACCEL);
                                    }
                                }
                            }

                            if ((event.sensor_mask & (1 << INV_SENSOR_ACCEL)) && (header->bits.twentybits_bit))
                            {
                                event.accel_high_res[0] = (s->fifo_data[0 + fifo_idx] >> 4) & 0xF;
                                event.accel_high_res[1] = (s->fifo_data[1 + fifo_idx] >> 4) & 0xF;
                                event.accel_high_res[2] = (s->fifo_data[2 + fifo_idx] >> 4) & 0xF;
                            }
                        }
                    }

                    if (header->bits.gyro_bit)
                    {
                        if ((event.gyro[0] != INVALID_VALUE_FIFO) &&
                            (event.gyro[1] != INVALID_VALUE_FIFO) &&
                            (event.gyro[2] != INVALID_VALUE_FIFO))
                        {
                            if (s->gyro_start_time_us == UINT32_MAX)
                            {
                                event.sensor_mask |= (1 << INV_SENSOR_GYRO);
                            }
                            else
                            {
                                if (!header->bits.fsync_bit)
                                {
                                    /* First data are wrong after gyro enable using IIR filter
                                    There is no signal that says Gyro start-up has completed and data are stable using FIR filter
                                    and the Gyro max start-up time is 40ms
                                    So test the delta time from Gyro start-up to valid Gyro data sampled and discard any Gyro data sampled before 60ms
                                    */
                                    if ((inv_imu_get_time_us() - s->gyro_start_time_us) >= GYR_STARTUP_TIME_US)
                                    {
                                        s->gyro_start_time_us = UINT32_MAX;
                                        event.sensor_mask |= (1 << INV_SENSOR_GYRO);
                                    }
                                }
                            }

                            if ((event.sensor_mask & (1 << INV_SENSOR_GYRO)) && (header->bits.twentybits_bit))
                            {
                                event.gyro_high_res[0] = (s->fifo_data[0 + fifo_idx]) & 0xF;
                                event.gyro_high_res[1] = (s->fifo_data[1 + fifo_idx]) & 0xF;
                                event.gyro_high_res[2] = (s->fifo_data[2 + fifo_idx]) & 0xF;
                            }
                        }
                    }

                    if (header->bits.twentybits_bit)
                    {
                        fifo_idx += FIFO_ACCEL_GYRO_HIGH_RES_SIZE;
                    }
                } /* end of else invalid frame */

                if (packet_count_i < FIFO_DEPTH)
                {
                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].accel_data[0] = event.accel[0];
                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].accel_data[1] = event.accel[1];
                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].accel_data[2] = event.accel[2];

                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].gyro_data[0] = event.gyro[0];
                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].gyro_data[1] = event.gyro[1];
                    report_gyro_acc_sensor_data.gyro_acc_sensor_data[packet_count_i].gyro_data[2] = event.gyro[2];

                    gyro_acc_sensor_data[packet_count_i].accel_data[0] = event.accel[0];
                    gyro_acc_sensor_data[packet_count_i].accel_data[1] = event.accel[1];
                    gyro_acc_sensor_data[packet_count_i].accel_data[2] = event.accel[2];

                    gyro_acc_sensor_data[packet_count_i].gyro_data[0] = event.gyro[0];
                    gyro_acc_sensor_data[packet_count_i].gyro_data[1] = event.gyro[1];
                    gyro_acc_sensor_data[packet_count_i].gyro_data[2] = event.gyro[2];
                    // APP_PRINT_INFO7("fifo[%d], acc: %d %d %d gyro: %d %d %d\n",
                    //                 packet_count_i, event.accel[0], event.accel[1], event.accel[2], event.gyro[0], event.gyro[1],
                    //                 event.gyro[2]);
                }

            } /* end of FIFO read for loop */
            packet_count = invalid_frame_cnt;
        } /*end of while: packet_count > 0*/


        app_sensor_mems_post_data(&report_gyro_acc_sensor_data);
        status |= inv_imu_switch_off_mclk(s);

    } /*else: FIFO threshold was not reached and FIFO was not full*/

    inv_imu_reset_fifo(s);
    // APP_PRINT_INFO3("inv_imu_get_data_from_fifo_wo_int_check: total_packet %d fifo_depth %d status 0x%x",
    //                 total_packet_count, FIFO_DEPTH, status);

    return total_packet_count;
}

uint32_t inv_imu_convert_odr_bitfield_to_us(uint32_t odr_bitfield)
{
    /*
    odr bitfield - frequency : odr ms
            0 - N/A
            1 - N/A
            2 - N/A
            3 - N/A
            4 - N/A
            5 - 1.6k      : 0.625ms
    (default) 6 - 800       : 1.25ms
            7 - 400       : 2.5 ms
            8 - 200       : 5 ms
            9 - 100       : 10 ms
            10 - 50       : 20 ms
            11 - 25       : 40 ms
            12 - 12.5     : 80 ms
            13 - 6.25     : 160 ms
            14 - 3.125    : 320 ms
            15 - 1.5625   : 640 ms
        */

    switch (odr_bitfield)
    {
    case ACCEL_CONFIG0_ODR_1600_HZ:    return 625;
    case ACCEL_CONFIG0_ODR_800_HZ:     return 1250;
    case ACCEL_CONFIG0_ODR_400_HZ:     return 2500;
    case ACCEL_CONFIG0_ODR_200_HZ:     return 5000;
    case ACCEL_CONFIG0_ODR_100_HZ:     return 10000;
    case ACCEL_CONFIG0_ODR_50_HZ:      return 20000;
    case ACCEL_CONFIG0_ODR_25_HZ:      return 40000;
    case ACCEL_CONFIG0_ODR_12_5_HZ:    return 80000;
    case ACCEL_CONFIG0_ODR_6_25_HZ:    return 160000;
    case ACCEL_CONFIG0_ODR_3_125_HZ:   return 320000;
    case ACCEL_CONFIG0_ODR_1_5625_HZ:
    default:                                    return 640000;
    }
}

int inv_imu_set_accel_frequency(struct inv_imu_device *s, const ACCEL_CONFIG0_ODR_t frequency)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &data);
    data &= ~ACCEL_CONFIG0_ACCEL_ODR_MASK;
    data |= frequency;
    status |= inv_imu_write_reg(s, ACCEL_CONFIG0, 1, &data);

    return status;
}

int inv_imu_set_gyro_frequency(struct inv_imu_device *s, const GYRO_CONFIG0_ODR_t frequency)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, GYRO_CONFIG0, 1, &data);
    data &= ~GYRO_CONFIG0_GYRO_ODR_MASK;
    data |= frequency;
    status |= inv_imu_write_reg(s, GYRO_CONFIG0, 1, &data);

    return status;
}

int inv_imu_set_accel_fsr(struct inv_imu_device *s, ACCEL_CONFIG0_FS_SEL_t accel_fsr_g)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &data);
    data &= ~ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK;
    data |= accel_fsr_g;
    status |= inv_imu_write_reg(s, ACCEL_CONFIG0, 1, &data);

    return status;
}

int inv_imu_set_gyro_fsr(struct inv_imu_device *s, GYRO_CONFIG0_FS_SEL_t gyro_fsr_dps)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, GYRO_CONFIG0, 1, &data);
    data &= ~GYRO_CONFIG0_GYRO_UI_FS_SEL_MASK;
    data |= gyro_fsr_dps;
    status |= inv_imu_write_reg(s, GYRO_CONFIG0, 1, &data);

    return status;
}

int inv_imu_get_accel_fsr(struct inv_imu_device *s, ACCEL_CONFIG0_FS_SEL_t *accel_fsr_g)
{
    int status = 0;
    uint8_t accel_cfg_0_reg;

    if ((s->fifo_highres_enabled) && (s->fifo_is_used == INV_IMU_FIFO_ENABLED))
    {
        *accel_fsr_g = ACCEL_CONFIG0_FS_SEL_MAX;
    }
    else
    {
        status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
        *accel_fsr_g = (ACCEL_CONFIG0_FS_SEL_t)(accel_cfg_0_reg & ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK);
    }

    return status;
}

int inv_imu_set_timestamp_resolution(struct inv_imu_device *s,
                                     const TMST_CONFIG1_RESOL_t timestamp_resol)
{
    int status = 0;
    uint8_t data;

    status |= inv_imu_read_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &data);
    data &= ~TMST_CONFIG1_TMST_RESOL_MASK;
    data |= timestamp_resol;
    status |= inv_imu_write_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &data);

    return status;
}

int inv_imu_reset_fifo(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t fifo_flush_status = (uint8_t)SIGNAL_PATH_RESET_FIFO_FLUSH_EN;

    status |= inv_imu_switch_on_mclk(s);

    status |= inv_imu_write_reg(s, SIGNAL_PATH_RESET, 1, &fifo_flush_status);
    inv_imu_sleep_us(10);

    /* Wait for FIFO flush (idle bit will go high at appropriate time and unlock flush) */
    while ((status == 0) &&
           ((fifo_flush_status & SIGNAL_PATH_RESET_FIFO_FLUSH_MASK) == (uint8_t)
            SIGNAL_PATH_RESET_FIFO_FLUSH_EN))
    {
        status |= inv_imu_read_reg(s, SIGNAL_PATH_RESET, 1, &fifo_flush_status);
    }

    status |= inv_imu_switch_off_mclk(s);

    return status;
}

int inv_imu_enable_high_resolution_fifo(struct inv_imu_device *s)
{
    uint8_t value;
    int status = 0;

    /* set FIFO packets to 20bit format (i.e. high res is enabled) */
    s->fifo_highres_enabled = 1;

    status |= inv_imu_read_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &value);
    value &= ~FIFO_CONFIG5_FIFO_HIRES_EN_MASK;
    value |= FIFO_CONFIG5_HIRES_EN;
    status |= inv_imu_write_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &value);

    return status;
}

int inv_imu_disable_high_resolution_fifo(struct inv_imu_device *s)
{
    uint8_t value;
    int status = 0;

    /* set FIFO packets to 16bit format (i.e. high res is disabled) */
    s->fifo_highres_enabled = 0;

    status |= inv_imu_read_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &value);
    value &= ~FIFO_CONFIG5_FIFO_HIRES_EN_MASK;
    value |= FIFO_CONFIG5_HIRES_DIS;
    status |= inv_imu_write_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &value);

    return status;
}

int inv_imu_configure_fifo(struct inv_imu_device *s, INV_IMU_FIFO_CONFIG_t fifo_config)
{
    int status = 0;
    uint8_t data;
    inv_imu_interrupt_parameter_t config_int = {(inv_imu_interrupt_value)0};

    s->fifo_is_used = fifo_config;

    inv_imu_switch_on_mclk(s);

    switch (fifo_config)
    {

    case INV_IMU_FIFO_ENABLED :
        /* Configure:
          - FIFO record mode i.e FIFO count unit is packet
          - FIFO snapshot mode i.e drop the data when the FIFO overflows
          - Timestamp is logged in FIFO
          - Little Endian fifo_count
        */

        status |= inv_imu_read_reg(s, INTF_CONFIG0, 1, &data);
        data &= ~(INTF_CONFIG0_FIFO_COUNT_FORMAT_MASK | INTF_CONFIG0_FIFO_COUNT_ENDIAN_MASK);
        data |= ((uint8_t)INTF_CONFIG0_FIFO_COUNT_REC_RECORD | (uint8_t)
                 INTF_CONFIG0_FIFO_COUNT_LITTLE_ENDIAN);
        status |= inv_imu_write_reg(s, INTF_CONFIG0, 1, &data);

        status |= inv_imu_read_reg(s, FIFO_CONFIG1, 1, &data);
        data &= ~(FIFO_CONFIG1_FIFO_MODE_MASK | FIFO_CONFIG1_FIFO_BYPASS_MASK);
        data |= ((uint8_t)FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)FIFO_CONFIG1_FIFO_BYPASS_OFF);
        status |= inv_imu_write_reg(s, FIFO_CONFIG1, 1, &data);

        status |= inv_imu_read_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &data);
        data &= ~TMST_CONFIG1_TMST_EN_MASK;
        data |= TMST_CONFIG1_TMST_EN;
        status |= inv_imu_write_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &data);

        /* restart and reset FIFO configuration */
        status |= inv_imu_read_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &data);
        data &= ~(FIFO_CONFIG5_FIFO_GYRO_EN_MASK | FIFO_CONFIG5_FIFO_ACCEL_EN_MASK
                  | FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_MASK | FIFO_CONFIG5_FIFO_HIRES_EN_MASK);
        data |= ((uint8_t)FIFO_CONFIG5_GYRO_EN | (uint8_t)FIFO_CONFIG5_ACCEL_EN
                 | (uint8_t)FIFO_CONFIG5_TMST_FSYNC_EN | (uint8_t)FIFO_CONFIG5_WM_GT_TH_EN);
        status |= inv_imu_write_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &data);

        // Configure FIFO WM so that INT is triggered for each packet
        //data = 0x32;
        data = FIFO_DEPTH;
        status |= inv_imu_write_reg(s, FIFO_CONFIG2, 1, &data);

        /* Disable Data Ready Interrupt */
        status |= inv_imu_get_config_int1(s, &config_int);
        // status |= inv_imu_get_config_int2(s, &config_int);
        config_int.INV_UI_DRDY = INV_IMU_DISABLE;
        status |= inv_imu_set_config_int1(s, &config_int);
        // status |= inv_imu_set_config_int2(s, &config_int);
        break;


    case INV_IMU_FIFO_DISABLED :
        /* make sure FIFO is disabled */
        status |= inv_imu_read_reg(s, FIFO_CONFIG1, 1, &data);
        data &= ~FIFO_CONFIG1_FIFO_BYPASS_MASK;
        data |= (uint8_t)FIFO_CONFIG1_FIFO_BYPASS_ON;
        status |= inv_imu_write_reg(s, FIFO_CONFIG1, 1, &data);

        /* restart and reset FIFO configuration */
        status |= inv_imu_read_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &data);
        data &= ~(FIFO_CONFIG5_FIFO_GYRO_EN_MASK | FIFO_CONFIG5_FIFO_ACCEL_EN_MASK
                  | FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_MASK);
        data |= ((uint8_t)FIFO_CONFIG5_GYRO_DIS | (uint8_t)FIFO_CONFIG5_ACCEL_DIS
                 | (uint8_t)FIFO_CONFIG5_TMST_FSYNC_EN);
        status |= inv_imu_write_reg(s, FIFO_CONFIG5_MREG_TOP1, 1, &data);

        /* Enable Data Ready Interrupt */
        status |= inv_imu_get_config_int1(s, &config_int);
        config_int.INV_UI_DRDY = INV_IMU_ENABLE;
        status |= inv_imu_set_config_int1(s, &config_int);
        // status |= inv_imu_set_config_int2(s, &config_int);
        break;

    default :
        status = -1;
    }

    status |= inv_imu_switch_off_mclk(s);

    return status;
}

/*
 * Static functions definition
 */
static int configure_serial_interface(struct inv_imu_device *s)
{
    uint8_t value;
    int status = 0;

    switch (s->transport.serif.serif_type)
    {
    /*remove spi interface*/
    case UI_I2C:
        // Enable I2C 50ns spike filtering
        status |= inv_imu_read_reg(s, INTF_CONFIG1, 1, &value);
        value &= ~(INTF_CONFIG1_I3C_SDR_EN_MASK | INTF_CONFIG1_I3C_DDR_EN_MASK);
        //value |= (0 | 0);
        status |= inv_imu_write_reg(s, INTF_CONFIG1, 1, &value);
        break;

    default:
        status |= INV_ERROR_BAD_ARG;
    }

    return status;
}

static int init_hardware_from_ui(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;
    inv_imu_interrupt_parameter_t config_int = {(inv_imu_interrupt_value)0};
    uint8_t gyro_cfg_0_reg, accel_cfg_0_reg, tmst_cfg_1_reg;
    // Based on datasheet, start up time for register read/write after POR is 1ms and supply ramp time is 3ms
    inv_imu_sleep_us(3000);
    status |= inv_imu_device_reset(s);
    /* Setup MEMs properties. */
    status |= inv_imu_read_reg(s, GYRO_CONFIG0, 1, &gyro_cfg_0_reg);
    status |= inv_imu_read_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
    gyro_cfg_0_reg &= ~GYRO_CONFIG0_GYRO_UI_FS_SEL_MASK;
    gyro_cfg_0_reg |= GYRO_CONFIG0_FS_SEL_2000dps;
    accel_cfg_0_reg &= ~ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK;
    accel_cfg_0_reg |= ACCEL_CONFIG0_FS_SEL_4g;
    status |= inv_imu_write_reg(s, GYRO_CONFIG0, 1, &gyro_cfg_0_reg);
    status |= inv_imu_write_reg(s, ACCEL_CONFIG0, 1, &accel_cfg_0_reg);
    /* Deactivate FSYNC by default */
    status |= inv_imu_read_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &tmst_cfg_1_reg);
    tmst_cfg_1_reg &= ~TMST_CONFIG1_TMST_FSYNC_EN_MASK;
    tmst_cfg_1_reg |= TMST_CONFIG1_TMST_FSYNC_DIS;
    status |= inv_imu_write_reg(s, TMST_CONFIG1_MREG_TOP1, 1, &tmst_cfg_1_reg);
    status |= inv_imu_read_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);
    value &= ~FSYNC_CONFIG_FSYNC_UI_SEL_MASK;
    value |= FSYNC_CONFIG_UI_SEL_NO;
    status |= inv_imu_write_reg(s, FSYNC_CONFIG_MREG_TOP1, 1, &value);
    /* Set default timestamp resolution 16us (Mobile use cases) */
    status |= inv_imu_set_timestamp_resolution(s, TMST_CONFIG1_RESOL_16us);
    /* Enable push pull on INT1 to avoid moving in Test Mode after a soft reset */
    status |= inv_imu_read_reg(s, INT_CONFIG_REG, 1, &value);
    value &= ~INT_CONFIG_REG_INT1_DRIVE_CIRCUIT_MASK;
    value |= (uint8_t)INT_CONFIG_INT1_DRIVE_CIRCUIT_PP;
    status |= inv_imu_write_reg(s, INT_CONFIG_REG, 1, &value);
    /* Configure the INT1 interrupt pulse as active high */
    status |= inv_imu_read_reg(s, INT_CONFIG_REG, 1, &value);
    // value &= ~INT_CONFIG_REG_INT1_POLARITY_MASK;
    // value |= (uint8_t)INT_CONFIG_INT1_POLARITY_HIGH;
    value = 0x12;    // INT 1/ 2 pulse, active low
    status |= inv_imu_write_reg(s, INT_CONFIG_REG, 1, &value);
    /* Set interrupt config */
    config_int.INV_UI_FSYNC      = INV_IMU_DISABLE;
    config_int.INV_UI_DRDY       = INV_IMU_DISABLE;
    //config_int.INV_UI_DRDY       = INV_IMU_ENABLE;
    config_int.INV_FIFO_THS      = INV_IMU_ENABLE;
    config_int.INV_FIFO_FULL     = INV_IMU_DISABLE;
    config_int.INV_SMD           = INV_IMU_ENABLE;
    config_int.INV_WOM_X         = INV_IMU_ENABLE;
    config_int.INV_WOM_Y         = INV_IMU_ENABLE;
    config_int.INV_WOM_Z         = INV_IMU_ENABLE;
    config_int.INV_FF            = INV_IMU_ENABLE;
    config_int.INV_LOWG          = INV_IMU_ENABLE;
    config_int.INV_STEP_DET      = INV_IMU_ENABLE;
    config_int.INV_STEP_CNT_OVFL = INV_IMU_ENABLE;
    config_int.INV_TILT_DET      = INV_IMU_ENABLE;

    // status |= inv_imu_set_config_int2(s, &config_int);
    status |= inv_imu_set_config_int1(s, &config_int);
    status |= inv_imu_configure_fifo(s, INV_IMU_FIFO_ENABLED);

    /* FIFO packets are 16bit format by default (i.e. high res is disabled) */
    status |= inv_imu_disable_high_resolution_fifo(s);

    /* Configure WOM to produce signal when all axis exceed 52 mg */
#if 0
    status |= inv_imu_configure_wom(s,
                                    DEFAULT_WOM_THS_MG,
                                    DEFAULT_WOM_THS_MG,
                                    DEFAULT_WOM_THS_MG,
                                    WOM_CONFIG_WOM_INT_MODE_ANDED,
                                    WOM_CONFIG_WOM_INT_DUR_1_SMPL);
#endif
    /* by default, IIR filter BW = 180Hz for LN, and average = 16x for LP mode */
    /* Disable the automatic RCOSC power on to avoid extra power consumption in sleep mode (all sensors and clocks off) */
    status |= inv_imu_read_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &value);
    value |= ((1 & FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK) <<
              FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_POS);
    status |= inv_imu_write_reg(s, FIFO_CONFIG6_MREG_TOP1, 1, &value);
    // APP_PRINT_INFO1("init_hardware_from_ui status 0x%x", status);
    /* Reset self-test result variable*/
    s->st_result = 0;

    return status;
}

#ifndef PSEQ_SPARE_TRIM_MREG_TOP1
#define PSEQ_SPARE_TRIM_MREG_TOP1 0x66
#endif

int inv_imu_reset_dmp(struct inv_imu_device *s, const APEX_CONFIG0_DMP_SRAM_RESET_t sram_reset)
{
    const int ref_timeout = 5000; /*50 ms*/
    int status = 0;
    int timeout = ref_timeout;
    uint8_t data_dmp_reset;
    uint8_t value = 0;

    status |= inv_imu_switch_on_mclk(s);

    // Reset DMP internal memories
    status |= inv_imu_read_reg(s, APEX_CONFIG0, 1, &value);
    value &= ~APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK;
    value |= (sram_reset & APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK);
    status |= inv_imu_write_reg(s, APEX_CONFIG0, 1, &value);

    inv_imu_sleep_us(1000);

    // Make sure reset procedure has finished by reading back mem_reset_en bit
    do
    {
        inv_imu_sleep_us(10);
        status |= inv_imu_read_reg(s, APEX_CONFIG0, 1, &data_dmp_reset);
    }
    while (((data_dmp_reset & APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK) != APEX_CONFIG0_DMP_SRAM_RESET_DIS)
           && timeout-- && !status);

    status |= inv_imu_switch_off_mclk(s);

    if (timeout <= 0)
    {
        return INV_ERROR_TIMEOUT;
    }

    return status;
}

int inv_imu_get_endianness(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;

    status |= inv_imu_read_reg(s, INTF_CONFIG0, 1, &value);
    if (!status)
    {
        s->endianness_data = value & INTF_CONFIG0_SENSOR_DATA_ENDIAN_MASK;
    }

    return status;
}
#endif

