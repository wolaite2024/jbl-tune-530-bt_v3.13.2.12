#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include "inv_imu_transport.h"
#include "inv_imu_driver.h"

#define DEFAULT_ST_GYRO_FSR_DPS    1000
#define DEFAULT_ST_ACCEL_FSR_G     8

/** @brief IMU HW Base sensor status based upon s->sensor_on_mask
 */
enum inv_imu_sensor_on_mask
{
    INV_SENSOR_ON_MASK_ACCEL = (1L << INV_SENSOR_ACCEL),
    INV_SENSOR_ON_MASK_GYRO = (1L << INV_SENSOR_GYRO),
};

int inv_imu_run_selftest(struct inv_imu_device *s, struct inv_imu_selftest_output *st_output)
{
    uint8_t st_done;
    inv_imu_selftest_parameters_t st_params;
    int result = 0;

    result |= inv_imu_switch_on_mclk(s);

    result |= inv_imu_init_selftest_parameters_struct(s, &st_params);
    result |= inv_imu_start_dmp_selftest(s);
    result |= inv_imu_configure_selftest_parameters(s, &st_params);
    /* RC is kept on before this function in order to guarantee proper access to MCLK register without having to rewrite SCLK */
    result |= inv_imu_control_selftest(s, st_params.st_control);

    /* check st_status1/2 (active polling) */
    st_done = 0;
    do
    {
        inv_imu_sleep_us(1000);
        result |= inv_imu_check_selftest_completion(s, &st_done, &st_params);
    }
    while (!st_done);

    /* report self-test status */
    result |= inv_imu_process_selftest_end(s, st_output);

    /* Restore idle bit */
    result |= inv_imu_switch_off_mclk(s);


    return result;
}

int inv_imu_start_dmp_selftest(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t value;
    OTP_CTRL7_OTP_RELOAD_t otp_reload;

    /* Disables Gyro/Accel sensors */
    status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &value);
    value &= ~(PWR_MGMT_0_ACCEL_MODE_MASK | PWR_MGMT_0_GYRO_MODE_MASK);
    status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &value);

    // Reset SRAM to 0's
    status |= inv_imu_reset_dmp(s, APEX_CONFIG0_DMP_SRAM_RESET_APEX_ST_EN);
    if (status)
    {
        return status;
    }

    // APEX algorithms will restart from scratch after self-test
    s->dmp_is_on = 0;

    // Trigger OTP load for ST data
    status |= inv_imu_read_reg(s, OTP_CONFIG_MREG_TOP1, 1, &value);
    value &= ~OTP_CONFIG_OTP_COPY_MODE_MASK;
    value |= (uint8_t)OTP_CONFIG_OTP_COPY_ST_DATA;
    status |= inv_imu_write_reg(s, OTP_CONFIG_MREG_TOP1, 1, &value);

    status |= inv_imu_read_reg(s, OTP_CTRL7_MREG_OTP, 1, &value);
    value &= ~OTP_CTRL7_OTP_PWR_DOWN_MASK;
    value |= (uint8_t)OTP_CTRL7_PWR_DOWN_DIS;
    status |= inv_imu_write_reg(s, OTP_CTRL7_MREG_OTP, 1, &value);

    inv_imu_sleep_us(100);

    /* Host should disable INT function first before kicking off OTP copy operation */
    status |= inv_imu_read_reg(s, OTP_CTRL7_MREG_OTP, 1, &value);
    value &= ~OTP_CTRL7_OTP_RELOAD_MASK;
    value |= (uint8_t)OTP_CTRL7_OTP_RELOAD_EN;
    status |= inv_imu_write_reg(s, OTP_CTRL7_MREG_OTP, 1, &value);

    inv_imu_sleep_us(20);

    // Sanity check to reload autocleared and otp_done is raised
    status |= inv_imu_read_reg(s, OTP_CTRL7_MREG_OTP, 1, &value);
    otp_reload = (OTP_CTRL7_OTP_RELOAD_t)(value & OTP_CTRL7_OTP_RELOAD_MASK);
    status |= inv_imu_read_reg(s, MISC_1, 1, &value);
    value &= MISC_1_OTP_DONE_MASK;
    if ((value != MISC_1_OTP_DONE_MASK) || (otp_reload != OTP_CTRL7_OTP_RELOAD_DIS))
    {
        return INV_ERROR_UNEXPECTED;
    }

    status |= inv_imu_read_reg(s, INT_SOURCE0, 1, &value);
    value |=  INT_SOURCE0_INT_ST_DONE_INT1_EN_MASK;
    status |= inv_imu_write_reg(s, INT_SOURCE0, 1, &value);

    return status;
}

int inv_imu_init_selftest_parameters_struct(struct inv_imu_device *s,
                                            inv_imu_selftest_parameters_t *selftest_params)
{
    (void)s;
    selftest_params->st_num_samples = ST_CONFIG_16_SAMPLES;
    selftest_params->accel_limit = ST_CONFIG_ACCEL_ST_LIM_50;
    selftest_params->gyro_limit = ST_CONFIG_GYRO_ST_LIM_50;
    selftest_params->st_control = (SELFTEST_ACCEL_GYRO_ST_EN_t)SELFTEST_EN;
    return 0;
}

int inv_imu_configure_selftest_parameters(struct inv_imu_device *s,
                                          const inv_imu_selftest_parameters_t *selftest_params)
{
    int status = 0;
    uint8_t value;

    inv_imu_switch_on_mclk(s);

    /* Self-test configuration cannot be updated if it already running */
    status |= inv_imu_read_reg(s, SELFTEST_MREG_TOP1, 1, &value);
    if ((value & SELFTEST_EN) != SELFTEST_DIS)
    {
        return INV_ERROR_UNEXPECTED;
    }

    status |= inv_imu_read_reg(s, ST_CONFIG_MREG_TOP1, 1, &value);
    value &= ~((uint8_t)ST_CONFIG_ST_NUM_SAMPLE_MASK | (uint8_t)ST_CONFIG_ACCEL_ST_LIM_MASK |
               (uint8_t)ST_CONFIG_GYRO_ST_LIM_MASK);
    value |= (uint8_t)selftest_params->st_num_samples | (uint8_t)selftest_params->accel_limit |
             (uint8_t)selftest_params->gyro_limit;
    status |= inv_imu_write_reg(s, ST_CONFIG_MREG_TOP1, 1, &value);

    status |= inv_imu_switch_off_mclk(s);

    return status;
}

int inv_imu_control_selftest(struct inv_imu_device *s, const SELFTEST_ACCEL_GYRO_ST_EN_t st_control)
{
    /* This function can be used to either enable or abort self-test */
    uint8_t value;
    int status = 0;

    status |= inv_imu_read_reg(s, SELFTEST_MREG_TOP1, 1, &value);
    value &= ~SELFTEST_EN;
    value |= (uint8_t)st_control;

    /* Both accel and gyro MUST be enabled simulaneously for DMP to take it into account properly */
    status |= inv_imu_write_reg(s, SELFTEST_MREG_TOP1, 1, &value);

    return status;
}

int inv_imu_check_selftest_completion(struct inv_imu_device *s, uint8_t *st_done,
                                      const inv_imu_selftest_parameters_t *selftest_params)
{
    int status = 0;

    if (SELFTEST_DIS == selftest_params->st_control)
    {
        /* Nothing else required if self-test is not being run */
        return 0;
    }

    status |= inv_imu_read_reg(s, INT_STATUS, 1, st_done);
    *st_done &= INT_STATUS_INT_STATUS_ST_DONE_MASK;

    return status;
}

int inv_imu_process_selftest_end(struct inv_imu_device *s, inv_imu_selftest_output_t *st_output)
{
    uint8_t data[2] = {0};
    int status = 0;
    status |= inv_imu_read_reg(s, ST_STATUS1_MREG_TOP1, 2, &data[0]);
    if (0 == status)
    {
        st_output->accel_status = (data[0] & ST_STATUS1_DMP_ACCEL_ST_PASS_MASK) >>
                                  ST_STATUS1_DMP_ACCEL_ST_PASS_POS;
        st_output->ax_status = (data[0] & ST_STATUS1_DMP_AX_ST_PASS_MASK) >> ST_STATUS1_DMP_AX_ST_PASS_POS;
        st_output->ay_status = (data[0] & ST_STATUS1_DMP_AY_ST_PASS_MASK) >> ST_STATUS1_DMP_AY_ST_PASS_POS;
        st_output->az_status = (data[0] & ST_STATUS1_DMP_AZ_ST_PASS_MASK) >> ST_STATUS1_DMP_AZ_ST_PASS_POS;
        st_output->gyro_status = (data[1] & ST_STATUS2_DMP_GYRO_ST_PASS_MASK) >>
                                 ST_STATUS2_DMP_GYRO_ST_PASS_POS;
        st_output->gyro_status |= ((data[1] & ST_STATUS2_DMP_ST_INCOMPLETE_MASK) >>
                                   ST_STATUS2_DMP_ST_INCOMPLETE_POS) << 1;
        st_output->gx_status = (data[1] & ST_STATUS2_DMP_GX_ST_PASS_MASK) >> ST_STATUS2_DMP_GX_ST_PASS_POS;
        st_output->gy_status = (data[1] & ST_STATUS2_DMP_GY_ST_PASS_MASK) >> ST_STATUS2_DMP_GY_ST_PASS_POS;
        st_output->gz_status = (data[1] & ST_STATUS2_DMP_GZ_ST_PASS_MASK) >> ST_STATUS2_DMP_GZ_ST_PASS_POS;
    }

    /* Turn off self-test */
    status |= inv_imu_read_reg(s, SELFTEST_MREG_TOP1, 1, &data[0]);
    data[0] &= ~SELFTEST_EN;
    data[0] |= (uint8_t)SELFTEST_DIS;
    status |= inv_imu_write_reg(s, SELFTEST_MREG_TOP1, 1, &data[0]);

    /* Re-sync all clocking scheme with a dummy write */
    data[0] = 0;
    status |= inv_imu_write_reg(s, WHO_AM_I, 1, &data[0]);

    /* Reset FIFO because ST data may have been pushed to FIFO although we don't want it */
    status |= inv_imu_reset_fifo(s);

    /* Turn off OTP macro */
    status |= inv_imu_read_reg(s, OTP_CTRL7_MREG_OTP, 1, &data[0]);
    data[0] &= ~OTP_CTRL7_OTP_PWR_DOWN_MASK;
    data[0] |= (uint8_t)OTP_CTRL7_PWR_DOWN_EN;
    status |= inv_imu_write_reg(s, OTP_CTRL7_MREG_OTP, 1, &data[0]);

    inv_imu_sleep_us(100);

    return status;
}
#endif
