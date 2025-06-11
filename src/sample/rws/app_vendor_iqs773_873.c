#if (F_APP_SENSOR_IQS773_873_SUPPORT == 1)
#include "rtl876x.h"
#include "rtl876x_i2c.h"
#include "trace.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "rtl876x_pinmux.h"
#include "hal_gpio.h"
#include "platform_utils.h"
#include "app_timer.h"
#include "app_dlps.h"
#include "app_main.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_sniff_mode.h"

#include "app_sensor_iqs773_873.h"
#include "app_vendor_iqs773_873.h"
#include "app_vendor_iqs773_init.h"
#include "app_vendor_iqs873_init.h"

//This flag needs to be turned on only when iqs vendor requires to fine tune iqs773 parameter
#define APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG 0

//for CMD_IQS773_WORK_MODE_SET and CMD_IQS773_SENSITIVITY_SET
#define IQS773_EVENT_MODE   0x00
#define IQS773_STREAM_MODE  0x01

#define IQS773_HALL_ENABLE  1
#define IQS773_HALL_DISABLE 0

#define PIN_LOW(x) Pad_Config(x, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW)
#define PIN_HIGH(x) Pad_Config(x, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_HIGH)

uint8_t timer_idx_iqs_release_debounce = 0;
#if (APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG == 1)
uint8_t timer_idx_iqs_addr_log = 0;
#endif
uint8_t timer_idx_iqs_sw_reset_check = 0;

uint8_t psensor_iqs_timer_id  = 0;
static uint8_t iqs_force_set = 0;
static uint8_t iqs_work_mode = 0;
static uint8_t iqs_spp_app_idx = 0;
static uint8_t iqs_spp_cmd_path = 0;

/* App define PSENSOR_IQS timer type */
typedef enum
{
    APP_TIMER_IQS_RELEASE_DEBOUNCE,
    APP_TIMER_IQS_ADDR_LOG,
    APP_TIMER_IQS_SW_RESET_CHECK,
} T_APP_PSENSOR_IQS_TIMER;

static void i2c_iqs_set_force_set(T_PSENSOR_I2C_EVENT event, int16_t iqs_force_cnt);

#if (APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG == 1)
static void i2c_iqs_addr_log(void);
#endif

/***************************************************************************
 * \brief Writes data packet to slave
 *
 * Writes a data packet to the specified slave address on the I2C bus
 * and sends a stop or ack condition when finished.
 *
 **************************************************************************/
void i2c_iqs_burst_write(uint8_t *write_data, uint8_t write_size)
{
    //WaitRdyLow();        /* this is unnecessary; remove to prevent system block when smart charger box enable */

    //I2C_MasterWrite(I2C0, write_data, write_size);
    app_sensor_i2c_write(I2C_AZQ_ADDR, write_data, write_size);

    //WaitRdyHigh();
}


/***************************************************************************
 * \brief Writes data packet to slave
 *
 * Writes a data packet to the specified slave address on the I2C bus
 * and sends a stop or ack condition when finished.
 *
 **************************************************************************/
void i2c_iqs_burst_write_no_rdy(uint8_t *write_data, uint8_t write_size)
{
    //I2C_MasterWrite(I2C0, write_data, write_size);
    app_sensor_i2c_write(I2C_AZQ_ADDR, write_data, write_size);
}


/***************************************************************************
 * \brief Reads data packet from slave
 *
 * Reads a data packet from the specified slave address on the I2C
 * bus and sends a stop or nack condition when finished.
 *
 **************************************************************************/
void i2c_iqs_burst_read(uint8_t read_addr, uint8_t *read_data, uint8_t read_size)
{
    //WaitRdyLow();

    //I2C_RepeatRead(I2C0, &read_addr, 1, read_data, read_size);
    app_sensor_i2c_read(I2C_AZQ_ADDR, read_addr, read_data, read_size);

    //WaitRdyHigh();
}

/***************************************************************************
 * \brief Reads data packet from slave
 *
 * Reads a data packet from the specified slave address on the I2C
 * bus and sends a stop or nack condition when finished.
 *
 **************************************************************************/
void i2c_iqs_burst_read_no_rdy(uint8_t read_addr, uint8_t *read_data, uint8_t read_size)
{
    //I2C_RepeatRead(I2C0, &read_addr, 1, read_data, read_size);
    app_sensor_i2c_read(I2C_AZQ_ADDR, read_addr, read_data, read_size);
}


/***************************************************************************
 * Description: Check if the module uses iqs773 or iqs873.
 *
 **************************************************************************/
T_IQS_MODULE i2c_iqs_check_device(void)
{
    uint8_t iqs_data_buf[4] = {0, 0, 0, 0};
    T_IQS_MODULE ret = IQS_MODULE_NOT_FOUND;

    // Sync communication window
    //WaitRdyLow(); WaitRdyHigh();

    // Device Recognition

    uint8_t read_count = 8;
    while (read_count--)
    {
        i2c_iqs_burst_read(IQS773_PROD_NUM, &iqs_data_buf[0], 0x04);
        if (iqs_data_buf[0])
        {
            break;
        }
        platform_delay_ms(10);
    }

    APP_PRINT_TRACE2("Chip Product ID 0x%02X  Software 0x%02X\r\n", iqs_data_buf[0], iqs_data_buf[1]);

    // IQS773
    if (iqs_data_buf[0] == 0x41 && iqs_data_buf[1] >= 0x08 &&
        app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)
    {
        //To support alternate materials, remove the Private Bit check
        // Soft Reset
        iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
        iqs_data_buf[1] = (SYSTEM_SETTINGS | 0x80);
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

        platform_delay_ms(20);

//        // Private Bit
//        i2c_iqs_burst_read_no_rdy(IQS773_SYSTEM_SETTINGS_0, &status_flag, 0x01);

//        APP_PRINT_TRACE1("Private Bit 0x%02X\r\n", status_flag);

//        if (status_flag == 0x08)
//        {
        APP_PRINT_TRACE0("Find IQS773 Module\r\n");

        ret = IQS773_MODULE;
//        }
    }
    // IQS873
    else if (iqs_data_buf[0] == 0x4F && iqs_data_buf[1] >= 0x03 && iqs_data_buf[3] >= 0x03 &&
             app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)
    {
        //To support alternate materials, remove the Private Bit check
        // Soft Reset
//        iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
//        iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
//        iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | IQS873_SOFT_RESET);
//        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x03);

//        platform_delay_ms(150);

//        // Private Bit
//        i2c_iqs_burst_read_no_rdy(IQS873_MM_PRIVATE_FLAGS, &status_flag, 0x01);

//        APP_PRINT_TRACE1("Private Bit 0x%02X\r\n", status_flag);

//        if (status_flag == 0x01)
//        {
        APP_PRINT_TRACE0("Find IQS873 Module\r\n");

        ret = IQS873_MODULE;
//        }
    }

    return ret;
}


/***************************************************************************
 * Description: Configures the iqs773 for general working condition.
 *
 **************************************************************************/
void i2c_iqs773_setup(void)
{
    uint8_t iqs_data_buf[24];

    app_cfg_const.iqs_p3.iqs773_sensitivity.ati_target = (app_cfg_nv.iqs773_sensitivity_ati_target != 0)
                                                         ?
                                                         app_cfg_nv.iqs773_sensitivity_ati_target : app_cfg_const.iqs_p3.iqs773_sensitivity.ati_target;
    app_cfg_const.iqs773_trigger_level = (app_cfg_nv.iqs773_trigger_level != 0) ?
                                         app_cfg_nv.iqs773_trigger_level : app_cfg_const.iqs773_trigger_level;

    // Force to Stream Mode
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = SYSTEM_SETTINGS;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);

    // Sync communication window
    //WaitRdyLow(); WaitRdyHigh();

    // Device and power mode settings
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = (SYSTEM_SETTINGS | ACK_RESET);
    iqs_data_buf[2] = ACTIVE_CHS;
    if (app_cfg_const.iqs773_hall_effect_support == IQS773_HALL_ENABLE)
    {
        iqs_data_buf[2] = (ACTIVE_CHS | 0x30);
    }
    iqs_data_buf[3] = PMU_SETTINGS;
    iqs_data_buf[4] = REPORT_RATES_TIMINGS_0;
    iqs_data_buf[5] = REPORT_RATES_TIMINGS_1;
    iqs_data_buf[6] = REPORT_RATES_TIMINGS_2;
    iqs_data_buf[7] = REPORT_RATES_TIMINGS_3;
    iqs_data_buf[8] = GLOBAL_EVENT_MASK;
    iqs_data_buf[9] = PWM_DUTY_CYCLE;
    iqs_data_buf[10] = RDY_TIMEOUT_PERIOD;
    iqs_data_buf[11] = I2C_SETTINGS;
    iqs_data_buf[12] = CH_RESEED_ENABLE;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0D);

    // ProxFusion sensor settings block 0
    iqs_data_buf[0] = IQS773_PXS_SETTINGS_0_0;
    iqs_data_buf[1] = PXS_SETTINGS0_0;
    iqs_data_buf[2] = PXS_SETTINGS0_1;
    iqs_data_buf[3] = PXS_SETTINGS0_2;
    iqs_data_buf[4] = PXS_SETTINGS0_3;
    iqs_data_buf[5] = PXS_SETTINGS0_4;
    iqs_data_buf[6] = PXS_SETTINGS0_5;
    iqs_data_buf[7] = PXS_SETTINGS0_6;
    iqs_data_buf[8] = PXS_SETTINGS0_7;
    iqs_data_buf[9] = PXS_SETTINGS0_8;
    iqs_data_buf[10] = PXS_SETTINGS0_9;
    if (app_cfg_const.iqs773_hall_effect_support == IQS773_HALL_ENABLE)
    {
        iqs_data_buf[10] = (PXS_SETTINGS0_9 | 0x88);
    }

    iqs_data_buf[11] = PXS_SETTINGS0_10;
    iqs_data_buf[12] = PXS_SETTINGS0_11;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0D);

    // ProxFusion sensor settings block 1
    iqs_data_buf[0] = IQS773_PXS_SETTINGS_4;
    iqs_data_buf[1] = PXS_SETTINGS1_0;
    iqs_data_buf[2] = PXS_SETTINGS1_1;
    iqs_data_buf[3] = PXS_SETTINGS1_2;
    iqs_data_buf[4] = PXS_SETTINGS1_3;
    iqs_data_buf[5] = PXS_SETTINGS1_4;
    iqs_data_buf[6] = PXS_SETTINGS1_5;
    iqs_data_buf[7] = PXS_SETTINGS1_6;
    iqs_data_buf[8] = PXS_SETTINGS1_7;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x09);

    // ProxFusion UI settings
    iqs_data_buf[0] = IQS773_P_THR_CH0;
    iqs_data_buf[1] = PXS_UI_SETTINGS_0;
    iqs_data_buf[2] = PXS_UI_SETTINGS_1;
    iqs_data_buf[3] = PXS_UI_SETTINGS_2;
    iqs_data_buf[4] = PXS_UI_SETTINGS_3;
    iqs_data_buf[5] = PXS_UI_SETTINGS_4;
    iqs_data_buf[6] = PXS_UI_SETTINGS_5;
    iqs_data_buf[7] = PXS_UI_SETTINGS_6;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x08);

    // SAR UI settings
    iqs_data_buf[0] = IQS773_SAR_UI_SETTINGS_0;
    iqs_data_buf[1] = SAR_UI_SETTINGS_0;
    iqs_data_buf[2] = SAR_UI_SETTINGS_1;
    iqs_data_buf[3] = SAR_UI_SETTINGS_2;
    iqs_data_buf[4] = SAR_UI_SETTINGS_3;
    iqs_data_buf[5] = SAR_UI_SETTINGS_4;
    iqs_data_buf[6] = SAR_UI_SETTINGS_5;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x07);

    // Hysteresis UI settings
    iqs_data_buf[0] = IQS773_HYST_UI_SETTINGS;
    iqs_data_buf[1] = HYSTERESIS_UI_SETTINGS_0;
    iqs_data_buf[2] = HYSTERESIS_UI_SETTINGS_1;
    iqs_data_buf[3] = HYSTERESIS_UI_SETTINGS_2;
    iqs_data_buf[4] = HYSTERESIS_UI_SETTINGS_3;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x05);

    // Hall-effect sensor settings
    iqs_data_buf[0] = IQS773_HALL_SEN_SETTINGS_0;
    iqs_data_buf[1] = HALL_SENSOR_SETTINGS_0;
    iqs_data_buf[2] = HALL_SENSOR_SETTINGS_1;
    iqs_data_buf[3] = HALL_SENSOR_SETTINGS_2;
    iqs_data_buf[4] = HALL_SENSOR_SETTINGS_3;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x05);

    // Hall-effect switch UI settings
    iqs_data_buf[0] = IQS773_HALL_UI_SETTINGS_0;
    iqs_data_buf[1] = HALL_UI_SETTINGS_0;
    iqs_data_buf[2] = HALL_UI_SETTINGS_1;
    iqs_data_buf[3] = HALL_UI_SETTINGS_2;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x04);

    // Temperature UI Settings
    iqs_data_buf[0] = IQS773_TEMP_UI_SETTINGS_0;
    iqs_data_buf[1] = TEMP_UI_SETTINGS_0;
    iqs_data_buf[2] = TEMP_UI_SETTINGS_1;
    iqs_data_buf[3] = TEMP_UI_SETTINGS_2;
    iqs_data_buf[4] = TEMP_UI_SETTINGS_3;
    iqs_data_buf[5] = TEMP_UI_SETTINGS_4;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x06);

    // UI Tool Settings
    {
        i2c_iqs773_np_report_rate(app_cfg_const.iqs_p1.iqs773_sample_rate);
        i2c_iqs773_ulp_report_rate(app_cfg_const.iqs773_ulp_rate);

        i2c_iqs773_rdy_timeout(app_cfg_const.iqs_p2.iqs773_intr_persistence);

        i2c_iqs773_force_ati_base_tgt(app_cfg_const.iqs_p3.iqs773_sensitivity.base,
                                      app_cfg_const.iqs_p3.iqs773_sensitivity.ati_target);
        i2c_iqs773_force_touch_thr(app_cfg_const.iqs773_trigger_level);
        i2c_iqs773_force_touch_bias(app_cfg_const.iqs773_bias);
        i2c_iqs773_force_touch_halt(app_cfg_const.iqs773_halt_timeout);

        i2c_iqs773_charge_frequency(app_cfg_const.iqs773_charge_freq);
        i2c_iqs773_prox_threshold(app_cfg_const.iqs773_prox_thr);

        if (app_cfg_const.iqs773_hall_effect_support)
        {
            i2c_iqs773_hall_ati_tgt(app_cfg_const.iqs_p4.iqs773_hall_effect_sensitivity);
            i2c_iqs773_hall_touch_thr(app_cfg_const.iqs773_hall_effect_trigger_level);
        }
    }

    // Redo ATI
    i2c_iqs773_redo_ati_all();

    // Event Mode
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = (SYSTEM_SETTINGS | EVENT_MODE);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
    i2c_iqs773_halt_mode_enter();

    // Debug String
    APP_PRINT_TRACE2("i2c_iqs773_setup IQS773 Setup Finish1 iqs773_sensitivity %d iqs773_trigger_leve %d",
                     app_cfg_const.iqs_p3.iqs773_sensitivity, app_cfg_const.iqs773_trigger_level);
}


/***************************************************************************
 * \brief Active Channel Setting
 *  1: Channel is enabled
 *  0: Channel is disabled
***************************************************************************/
void i2c_iqs773_active_ch(uint8_t iqs_ch)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_ACTIVE_CHAN;
    iqs_data_buf[1] = iqs_ch & 0x3F;

    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Normal Power Report Rate Setting
 *
***************************************************************************/
void i2c_iqs773_np_report_rate(uint8_t iqs_report_rate)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_NORMAL_REPORT_RATE;
    iqs_data_buf[1] = iqs_report_rate;

    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}

/***************************************************************************
 * \brief Ultra Low Power Report Rate Setting
 *
***************************************************************************/
void i2c_iqs773_ulp_report_rate(uint8_t iqs_report_rate)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_ULP_REPORT_RATE;
    iqs_data_buf[1] = iqs_report_rate;

    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}

/***************************************************************************
 * \brief RDY Timeout Setting
 *
***************************************************************************/
void i2c_iqs773_rdy_timeout(uint8_t iqs_rdy_timeout)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_RDY_TIMEOUT;
    iqs_data_buf[1] = iqs_rdy_timeout;

    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Force ATI Target Setting
 *
***************************************************************************/
void i2c_iqs773_force_ati_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt)
{
    uint8_t iqs_data_buf[2];

    iqs_base = (iqs_base << 6) & 0xC0;
    iqs_tgt &= 0x3F;

    iqs_data_buf[0] = IQS773_PXS_SETTINGS_2_0;
    iqs_data_buf[1] = (iqs_base | iqs_tgt);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Force Threshold Setting
 *
***************************************************************************/
void i2c_iqs773_force_touch_thr(uint8_t iqs_thr)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_T_THR_CH0;
    iqs_data_buf[1] = iqs_thr;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}

/***************************************************************************
 * \brief Force Bais Setting
 *
***************************************************************************/
void i2c_iqs773_force_touch_bias(uint8_t iqs_bias)
{
    uint8_t iqs_base_value = PXS_SETTINGS0_3;
    uint8_t iqs_data_buf[2];

    // Store base value
    iqs_base_value &= 0xF3;

    // New bias setting
    iqs_bias &= 0x03;
    iqs_bias <<= 2;

    iqs_data_buf[0] = IQS773_PXS_SETTINGS_1_0;
    iqs_data_buf[1] = (iqs_base_value | iqs_bias);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Force Halt Setting
 *
***************************************************************************/
void i2c_iqs773_force_touch_halt(uint8_t iqs_timeout)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_UI_HALT_TIME;
    iqs_data_buf[1] = iqs_timeout;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Hall ATI Target Setting
 *
***************************************************************************/
void i2c_iqs773_hall_ati_tgt(uint8_t iqs_tgt)
{
    uint8_t iqs_base_value = HALL_SENSOR_SETTINGS_1;
    uint8_t iqs_data_buf[2];

    // Store base value
    iqs_base_value &= 0xC0;

    // New target setting
    iqs_tgt &= 0x3F;

    iqs_data_buf[0] = IQS773_HALL_SEN_SETTINGS_1;
    iqs_data_buf[1] = (iqs_base_value | iqs_tgt);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Hall Threshold Setting
 *
***************************************************************************/
void i2c_iqs773_hall_touch_thr(uint8_t iqs_thr)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_HALL_UI_T_THR;
    iqs_data_buf[1] = iqs_thr;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Doing Redo-ATI for enable channels
 *
***************************************************************************/
void i2c_iqs773_redo_ati_all(void)
{
    uint8_t status_flag;

    uint8_t iqs_data_buf[2];

    uint16_t time_out = 30;

    // Redo ATI
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = (SYSTEM_SETTINGS | REDO_ATI_ALL | DO_RESEED);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

    // Wait for Redo ATI to complete
    do
    {
        if (!time_out)
        {
            break;
        }

        platform_delay_ms(10);

        i2c_iqs_burst_read(0x10, &status_flag, 0x01);

        status_flag &= 0x04;

        time_out--;
        APP_PRINT_TRACE2("ati_all iqs773, time_out:%d, status_flag:%d\n", time_out, status_flag);
    }
    while (status_flag);
}


/***************************************************************************
 * \brief Read compensation & multiplier
 *
***************************************************************************/
void i2c_iqs773_read_compensation(void)
{
    uint8_t i, index;
    uint8_t iqs_data_buf[6];

    uint8_t iqs_coarse, iqs_fine;
    int16_t iqs_comp;

    i2c_iqs_burst_read(IQS773_PXS_COMPENSATION, &iqs_data_buf[0], 0x01);
    i2c_iqs_burst_read(IQS773_PXS_MULTIPLIER, &iqs_data_buf[1], 0x01);
    i2c_iqs_burst_read(IQS773_HALL_COMPENSATION, &iqs_data_buf[2], 0x02);

    for (i = 0; i < 2; i++)
    {
        index = i << 1;

        // Coarse
        iqs_coarse = (iqs_data_buf[1 + index] & 0x30) >> 4;

        // Fine
        iqs_fine = iqs_data_buf[1 + index] & 0x0F;

        // Compensation
        iqs_comp = (int16_t)(((iqs_data_buf[1 + index] & 0xC0) << 2) + iqs_data_buf[0 + index]);

        // Debug String
        APP_PRINT_TRACE4("Touch Source %d  Coarse:%d  fine:%d  Comp:%d\r\n",
                         i, iqs_coarse, iqs_fine, iqs_comp);
    }
}


/***************************************************************************
 * \brief iqs773 I2C stop disable controll
 *
***************************************************************************/
void i2c_iqs773_i2c_stop_ctrl(uint8_t ctrl)
{
    uint8_t iqs_data_buf[2];

    // I2C control settings
    iqs_data_buf[0] = 0xDA;

    if (ctrl)
    {
        iqs_data_buf[1] = I2C_SETTINGS;
    }
    else
    {
        iqs_data_buf[1] = (I2C_SETTINGS | 0x80);
    }

    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}

void i2c_iqs773_sensitivity_level_set(uint8_t app_idx, uint8_t cmd_path, uint8_t sensitivity,
                                      uint8_t trig_level)
{
    iqs_spp_app_idx = app_idx;
    iqs_spp_cmd_path = cmd_path;
    app_cfg_nv.iqs773_sensitivity_ati_target = sensitivity;
    app_cfg_nv.iqs773_trigger_level = trig_level;
    app_cfg_store(&app_cfg_nv.iqs773_sensitivity_ati_target, 2);
    // Disable Interrupt
    hal_gpio_irq_disable(app_cfg_const.gsensor_int_pinmux);
    i2c_iqs773_setup();

    if (iqs_work_mode == (0x80 | IQS773_STREAM_MODE))
    {
        uint8_t iqs_data_buf[2];
        iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
        iqs_data_buf[1] = SYSTEM_SETTINGS;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
    }

    // Enable Interrupt
    hal_gpio_irq_enable(app_cfg_const.gsensor_int_pinmux);
    APP_PRINT_TRACE5("i2c_iqs773_sensitivity_level_set app_idx %d cmd_path %d iqs_work_mode %d sensitivity %d trig_level %d",
                     app_idx, cmd_path, iqs_work_mode, sensitivity,
                     trig_level);
}

void i2c_iqs773_work_mode_set(uint8_t app_idx, uint8_t cmd_path, uint8_t mode)
{
    uint8_t iqs_data_buf[2];

    iqs_work_mode = (0x80 | mode);
    iqs_spp_app_idx = app_idx;
    iqs_spp_cmd_path = cmd_path;
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    if (mode == IQS773_STREAM_MODE)
    {
        app_sniff_mode_b2s_disable(app_db.br_link[app_idx].bd_addr, SNIFF_DISABLE_MASK_TRANS);
        iqs_data_buf[1] = SYSTEM_SETTINGS;
    }
    else
    {
        app_sniff_mode_b2s_enable(app_db.br_link[app_idx].bd_addr, SNIFF_DISABLE_MASK_TRANS);
        iqs_data_buf[1] = (SYSTEM_SETTINGS | EVENT_MODE);
    }

    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
    APP_PRINT_TRACE3("i2c_iqs773_work_mode_set app_idx %d cmd_path %d mode %d", app_idx, cmd_path,
                     mode);
}

uint8_t i2c_iqs773_get_power_mode(void)
{
    uint8_t status_flag;
    i2c_iqs_burst_read(IQS773_PMU_SETTINGS_0, &status_flag, 0x01);
    return  status_flag;
}

void i2c_iqs773_halt_mode_enter(void)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_PMU_SETTINGS_0;
    iqs_data_buf[1] = 0x38;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

    APP_PRINT_TRACE1("i2c_iqs773_halt_mode_enter  PMU_SETTINGS_0(0xD2) = 0x%x",
                     i2c_iqs773_get_power_mode());
}

static void i2c_iqs773_redo_ati_after_exit_halt_mode(void)
{
    uint8_t iqs_data_buf[2];// Redo ATI

    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = 0x22;// Redo||EventMode
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}

void i2c_iqs773_halt_mode_exit(void)
{
    uint8_t status_flag;
    uint8_t iqs_data_buf[2];

    i2c_iqs_burst_read(IQS773_PMU_SETTINGS_0, &status_flag, 0x01);

    if (status_flag == 0x38)
    {
        iqs_data_buf[0] = IQS773_PMU_SETTINGS_0;
        iqs_data_buf[1] = 0x22;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
        platform_delay_ms(1);
        iqs_data_buf[0] = IQS773_PMU_SETTINGS_0;
        iqs_data_buf[1] = 0x02;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
    }
    i2c_iqs773_redo_ati_after_exit_halt_mode();
    APP_PRINT_TRACE1("i2c_iqs773_halt_mode_exit  PMU_SETTINGS_0(0xD2) = 0x%x",
                     i2c_iqs773_get_power_mode());
}

void i2c_iqs773_charge_frequency(uint8_t iqs_charge_freq)
{
    uint8_t iqs_base_value = PXS_SETTINGS0_3;
    uint8_t iqs_data_buf[2];

    // Store base value
    iqs_base_value = PXS_SETTINGS0_3 & 0xCF;

    // New charge frequency
    iqs_charge_freq = (iqs_charge_freq << 4) & 0x30;

    iqs_data_buf[0] = IQS773_PXS_SETTINGS_1_0;
    iqs_data_buf[1] = (iqs_base_value | iqs_charge_freq);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}

void i2c_iqs773_prox_threshold(uint8_t iqs_prox_thr)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = IQS773_P_THR_CH0;
    iqs_data_buf[1] = iqs_prox_thr;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

    iqs_data_buf[0] = 0x73;
    iqs_data_buf[1] = iqs_prox_thr;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
}



/***************************************************************************
 * Description: Configures the iqs873 for general working condition.
 *
 **************************************************************************/
void i2c_iqs873_setup(void)
{
    uint8_t iqs_data_buf[20];

    // PMU and System settings
    iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | IQS873_ACK_RESET);
    iqs_data_buf[3] = SYS_CHB_ACTIVE;
    iqs_data_buf[4] = ACF_LTA_FILTER_SETTINGS;
    iqs_data_buf[5] = LTA_CHB_RESEED_ENABLED;
    iqs_data_buf[6] = UIS_GLOBAL_EVENTS_MASK;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x07);

    // Report rates and timings
    iqs_data_buf[0] = IQS873_MM_RPT_SETTINGS;
    iqs_data_buf[1] = PMU_REPORT_RATE_NP;
    iqs_data_buf[2] = PMU_REPORT_RATE_LP;
    iqs_data_buf[3] = PMU_REPORT_RATE_ULP;
    iqs_data_buf[4] = PMU_MODE_TIMOUT;
    iqs_data_buf[5] = I2C_WINDOW_TIMEOUT;
    iqs_data_buf[6] = LTA_HALT_TIMEOUT;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x07);

    // Global settings
    iqs_data_buf[0] = IQS873_MM_PXS_GLBL_SETTINGS_0;
    iqs_data_buf[1] = PXS_GENERAL_SETTINGS0;
    iqs_data_buf[2] = PXS_GENERAL_SETTINGS1;
    iqs_data_buf[3] = UIS_ABSOLUTE_CAPACITANCE;
    iqs_data_buf[4] = UIS_DCF_GENERAL_SETTINGS;
    iqs_data_buf[5] = GEM_CHB_BLOCK_NFOLLOW;
    iqs_data_buf[6] = MOV_CHB_MOVEMENT_CHANNEL;
    iqs_data_buf[7] = UIS_CHB_SLIDER0;
    iqs_data_buf[8] = UIS_CHB_SLIDER1;
    iqs_data_buf[9] = UIS_GESTURE_TAP_TIMEOUT;
    iqs_data_buf[10] = UIS_GESTURE_SWIPE_TIMEOUT;
    iqs_data_buf[11] = UIS_GESTURE_THRESHOLD;
    iqs_data_buf[12] = LTA_CHB_RESEED;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x0D);

    // Channel 0 settings
    iqs_data_buf[0] = IQS873_MM_CH0_SELECT_CRX;
    iqs_data_buf[1] = PXS_CRXSEL_CH0;
    iqs_data_buf[2] = PXS_CTXSEL_CH0;
    iqs_data_buf[3] = PXS_PROXCTRL_CH0;
    iqs_data_buf[4] = PXS_PROXCFG0_CH0;
    iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH0;
    iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH0;
    iqs_data_buf[7] = ATI_MIRROR_CH0;
    iqs_data_buf[8] = ATI_PCC_CH0;
    iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH0;
    iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH0;
    iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH0;
    iqs_data_buf[12] = PXS_HYSTERESIS_CH0;
    iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH0;
    iqs_data_buf[14] = DCF_WEIGHT_CH0;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x0F);

    // Channel 1 settings
    iqs_data_buf[0] = IQS873_MM_CH1_SELECT_CRX;
    iqs_data_buf[1] = PXS_CRXSEL_CH1;
    iqs_data_buf[2] = PXS_CTXSEL_CH1;
    iqs_data_buf[3] = PXS_PROXCTRL_CH1;
    iqs_data_buf[4] = PXS_PROXCFG0_CH1;
    iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH1;
    iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH1;
    iqs_data_buf[7] = ATI_MIRROR_CH1;
    iqs_data_buf[8] = ATI_PCC_CH1;
    iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH1;
    iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH1;
    iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH1;
    iqs_data_buf[12] = PXS_HYSTERESIS_CH1;
    iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH1;
    iqs_data_buf[14] = DCF_WEIGHT_CH1;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x0F);

    // Channel 6 settings
    iqs_data_buf[0] = IQS873_MM_CH6_SELECT_CRX;
    iqs_data_buf[1] = PXS_CRXSEL_CH6;
    iqs_data_buf[2] = PXS_CTXSEL_CH6;
    iqs_data_buf[3] = PXS_PROXCTRL_CH6;
    iqs_data_buf[4] = PXS_PROXCFG0_CH6;
    iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH6;
    iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH6;
    iqs_data_buf[7] = ATI_MIRROR_CH6;
    iqs_data_buf[8] = ATI_PCC_CH6;
    iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH6;
    iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH6;
    iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH6;
    iqs_data_buf[12] = PXS_HYSTERESIS_CH6;
    iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH6;
    iqs_data_buf[14] = DCF_WEIGHT_CH6;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x0F);

    // Channel 7 settings
    iqs_data_buf[0] = IQS873_MM_CH7_SELECT_CRX;
    iqs_data_buf[1] = PXS_CRXSEL_CH7;
    iqs_data_buf[2] = PXS_CTXSEL_CH7;
    iqs_data_buf[3] = PXS_PROXCTRL_CH7;
    iqs_data_buf[4] = PXS_PROXCFG0_CH7;
    iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH7;
    iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH7;
    iqs_data_buf[7] = ATI_MIRROR_CH7;
    iqs_data_buf[8] = ATI_PCC_CH7;
    iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH7;
    iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH7;
    iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH7;
    iqs_data_buf[12] = PXS_HYSTERESIS_CH7;
    iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH7;
    iqs_data_buf[14] = DCF_WEIGHT_CH7;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x0F);

    // Hall sensor settings
    iqs_data_buf[0] = IQS873_MM_HALL_UI_EN;
    iqs_data_buf[1] = HALL_UI_ENABLE;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);

    // UI Setting Test
    {
        // bit0 : Wear Touch
        // bit1 : Force Touch
        // bit2 : Hall Touch
        if (app_cfg_const.iqs773_hall_effect_support)
        {
            i2c_iqs873_active_ch(0x07);
        }
        else
        {
            //turn off hall touch
            i2c_iqs873_active_ch(0x03);
        }

        //ULP report rate default value: 8
        i2c_iqs873_ulp_report_rate(app_cfg_const.iqs773_ulp_rate);

        //Touch timeout default value: 32
        i2c_iqs873_force_touch_halt(app_cfg_const.iqs773_halt_timeout);

        //Wear detect default value: 1,15,16
        i2c_iqs873_wear_base_tgt(app_cfg_const.iqs_p1.iqs873_wear_sensitivity.base,
                                 app_cfg_const.iqs_p1.iqs873_wear_sensitivity.ati_target);
        i2c_iqs873_wear_touch_thr(app_cfg_const.iqs_p2.iqs873_wear_trigger_level);

        //i2c_iqs873_rdy_timeout(0xFF);

        //Force touch default value: 1,15,8
        i2c_iqs873_force_base_tgt(app_cfg_const.iqs_p3.iqs873_sensitivity.base,
                                  app_cfg_const.iqs_p3.iqs873_sensitivity.ati_target);
        i2c_iqs873_force_touch_thr(app_cfg_const.iqs773_trigger_level);

        if (app_cfg_const.iqs773_hall_effect_support)
        {
            i2c_iqs873_hall_base_tgt(app_cfg_const.iqs_p4.iqs873_hall_sensitivity.base,
                                     app_cfg_const.iqs_p4.iqs873_hall_sensitivity.ati_target);
            i2c_iqs873_hall_touch_thr(app_cfg_const.iqs773_hall_effect_trigger_level);
        }
        else
        {
            //Hall effect default value: 3,15,36
            i2c_iqs873_hall_base_tgt(0x03, 0x0F);
            i2c_iqs873_hall_touch_thr(0x24);
        }
    }

    // Redo ATI
    i2c_iqs873_redo_ati_ch(SYS_CHB_ACTIVE);

    // Event Mode
    iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | EVENT_MODE);
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

    // Debug String
    APP_PRINT_TRACE0("IQS873 Setup Finish\r\n");
}


/***************************************************************************
 * \brief Doing Redo-ATI for enable channels
 *
***************************************************************************/
void i2c_iqs873_redo_ati_ch(uint8_t ch)
{
    uint8_t iqs_data_buf[3];

    // Enable channel reseed selection
    iqs_data_buf[0] = 0x8B;
    iqs_data_buf[1] = UIS_GESTURE_THRESHOLD;
    iqs_data_buf[2] = ch;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

    // Redo ATI
    iqs_data_buf[0] = 0x80;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | IQS873_REDO_ATI_ALL);
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);
}



/***************************************************************************
 * \brief Read compensation & multiplier
 *
***************************************************************************/
void i2c_iqs873_read_compensation(void)
{
    uint8_t iqs_data_buf[16];

    uint8_t iqs_coarse, iqs_fine;
    int16_t iqs_comp;


    i2c_iqs_burst_read_no_rdy(IQS873_MM_CH0_COMPENSATION, &iqs_data_buf[0], 0x02);

    iqs_coarse = (iqs_data_buf[0] & 0x30) >> 4;
    iqs_fine = iqs_data_buf[0] & 0x0F;
    iqs_comp = (int16_t)(((iqs_data_buf[0] & 0xC0) << 2) + iqs_data_buf[1]);

    // Debug String
    APP_PRINT_TRACE3("Wear Touch : Coarse:%d  fine:%d  Comp:%d\r\n",
                     iqs_coarse, iqs_fine, iqs_comp);



    {
        i2c_iqs_burst_read_no_rdy(IQS873_MM_CH1_COMPENSATION, &iqs_data_buf[2], 0x02);

        iqs_coarse = (iqs_data_buf[2] & 0x30) >> 4;
        iqs_fine = iqs_data_buf[2] & 0x0F;
        iqs_comp = (int16_t)(((iqs_data_buf[2] & 0xC0) << 2) + iqs_data_buf[3]);

        // Debug String
        APP_PRINT_TRACE3("Force Touch : Coarse:%d  fine:%d  Comp:%d\r\n",
                         iqs_coarse, iqs_fine, iqs_comp);
    }


    {
        i2c_iqs_burst_read_no_rdy(IQS873_MM_CH7_COMPENSATION, &iqs_data_buf[4], 0x02);

        iqs_coarse = (iqs_data_buf[4] & 0x30) >> 4;
        iqs_fine = iqs_data_buf[4] & 0x0F;
        iqs_comp = (int16_t)(((iqs_data_buf[4] & 0xC0) << 2) + iqs_data_buf[5]);

        // Debug String
        APP_PRINT_TRACE3("Hall Touch : Coarse:%d  fine:%d  Comp:%d\r\n",
                         iqs_coarse, iqs_fine, iqs_comp);
    }

}

/***************************************************************************
 * \brief Active Channel Setting
 *  1: Channel is enabled
 *  0: Channel is disabled
***************************************************************************/
void i2c_iqs873_active_ch(uint8_t iqs_ch)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = 0x81;
    iqs_data_buf[1] = SYS_CHB_ACTIVE;

    // Wear Touch
    if ((iqs_ch & 0x01) == 0x00)
    {
        iqs_data_buf[1] &= 0xFE;
    }

    // Force Touch
    if ((iqs_ch & 0x02) == 0x00)
    {
        iqs_data_buf[1] &= 0xFD;
    }

    // Hall Touch
    if ((iqs_ch & 0x04) == 0x00)
    {
        iqs_data_buf[1] &= 0x7F;
    }

    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Normal Power Report Rate Setting
 *
***************************************************************************/
void i2c_iqs873_np_report_rate(uint8_t iqs_report_rate)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = 0x83;
    iqs_data_buf[1] = iqs_report_rate;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Ultra Low Power Report Rate Setting
 *
***************************************************************************/
void i2c_iqs873_ulp_report_rate(uint8_t iqs_report_rate)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = 0x84;
    iqs_data_buf[1] = iqs_report_rate;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Wear Base & Target Setting
 *
***************************************************************************/
void i2c_iqs873_wear_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt)
{
    uint8_t iqs_data_buf[3];

    // New base & target setting
    iqs_base &= 0x03; iqs_tgt &= 0x3F;

    iqs_data_buf[0] = 0x8E;
    iqs_data_buf[1] = PXS_PROXCFG1_TESTREG0_CH0;
    iqs_data_buf[2] = ((iqs_base <<= 6) | iqs_tgt);
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);
}


/***************************************************************************
 * \brief Wear Threshold Setting
 *
***************************************************************************/
void i2c_iqs873_wear_touch_thr(uint8_t iqs_thr)
{
    uint8_t iqs_data_buf[4];

    iqs_data_buf[0] = 0x90;
    iqs_data_buf[1] = PXS_PROX_THRESHOLD_CH0;
    iqs_data_buf[2] = iqs_thr;
    iqs_data_buf[3] = iqs_thr;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x04);
}


/***************************************************************************
 * \brief Force Base & Target Setting
 *
***************************************************************************/
void i2c_iqs873_force_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt)
{
    uint8_t iqs_data_buf[3];

    // New base & target setting
    iqs_base &= 0x03; iqs_tgt &= 0x3F;

    iqs_data_buf[0] = 0x95;
    iqs_data_buf[1] = PXS_PROXCFG1_TESTREG0_CH1;
    iqs_data_buf[2] = ((iqs_base <<= 6) | iqs_tgt);
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);
}


/***************************************************************************
 * \brief Force Threshold Setting
 *
***************************************************************************/
void i2c_iqs873_force_touch_thr(uint8_t iqs_thr)
{
    uint8_t iqs_data_buf[4];

    iqs_data_buf[0] = 0x97;
    iqs_data_buf[1] = PXS_PROX_THRESHOLD_CH1;
    iqs_data_buf[2] = iqs_thr;
    iqs_data_buf[3] = iqs_thr;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x04);
}


/***************************************************************************
 * \brief Force Halt Setting
 *
***************************************************************************/
void i2c_iqs873_force_touch_halt(uint8_t iqs_timeout)
{
    uint8_t iqs_data_buf[3];

    iqs_data_buf[0] = 0x85;
    iqs_data_buf[1] = I2C_WINDOW_TIMEOUT;
    iqs_data_buf[2] = iqs_timeout;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);
}


/***************************************************************************
 * \brief RDY Timeout Setting
 *
***************************************************************************/
void i2c_iqs873_rdy_timeout(uint8_t iqs_rdy_timeout)
{
    uint8_t iqs_data_buf[2];

    iqs_data_buf[0] = 0x85;
    iqs_data_buf[1] = iqs_rdy_timeout;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}


/***************************************************************************
 * \brief Hall Base & Target Setting
 *
***************************************************************************/
void i2c_iqs873_hall_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt)
{
    uint8_t iqs_data_buf[3];

    // New base & target setting
    iqs_base &= 0x03; iqs_tgt &= 0x3F;

    iqs_data_buf[0] = 0xBF;
    iqs_data_buf[1] = PXS_PROXCFG1_TESTREG0_CH7;
    iqs_data_buf[2] = ((iqs_base <<= 6) | iqs_tgt);
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);
}


/***************************************************************************
 * \brief Hall Threshold Setting
 *
***************************************************************************/
void i2c_iqs873_hall_touch_thr(uint8_t iqs_thr)
{
    uint8_t iqs_data_buf[4];

    iqs_data_buf[0] = 0xC1;
    iqs_data_buf[1] = PXS_PROX_THRESHOLD_CH7;
    iqs_data_buf[2] = iqs_thr;
    iqs_data_buf[3] = iqs_thr;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x04);
}

/***************************************************************************
 * \brief Doing Redo-ATI for enable channels
 *
***************************************************************************/
void i2c_iqs873_i2c_stop_ctrl(uint8_t ctrl)
{
    uint8_t iqs_data_buf[2];

    if (ctrl)
    {
        iqs_data_buf[0] = IQS873_MM_I2C_CTRL_SETTING;
        i2c_iqs_burst_read_no_rdy(IQS873_MM_I2C_CTRL_SETTING, &iqs_data_buf[1], 0x01);
        iqs_data_buf[1] |= 0x80;
    }
    else
    {
        iqs_data_buf[0] = IQS873_MM_I2C_CTRL_SETTING;
        iqs_data_buf[1] = (I2C_CONTROL_SETTINGS | 0x40);
    }

    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
}

/***************************************************************************
 * \brief Read i2c event when iqs773 rdy pin is triggered
 *
***************************************************************************/
T_PSENSOR_I2C_EVENT i2c_iqs773_read_event(void)
{
    uint8_t iqs_data_buf[32] = {0};
    uint8_t iqs_sys_flag;
    uint8_t iqs_event_flag;
    uint8_t iqs_touch_flag;
    int16_t iqs_force_cnt;
    int16_t iqs_force_lta;
    T_PSENSOR_I2C_EVENT event = PSENSOR_I2C_EVENT_NONE;

    i2c_iqs773_i2c_stop_ctrl(DISABLE_I2C_STOP);

    i2c_iqs_burst_read_no_rdy(IQS773_SYSTEM_FLAGS, &iqs_data_buf[0], 0x0C);

    i2c_iqs_burst_read_no_rdy(IQS773_CH_COUNTS, &iqs_data_buf[16], 0x02);

    i2c_iqs_burst_read_no_rdy(IQS773_CH_LTA, &iqs_data_buf[18], 0x02);

    i2c_iqs_burst_read_no_rdy(0xD7, &iqs_data_buf[20], 0x01);

    i2c_iqs_burst_read_no_rdy(IQS773_P_THR_CH0, &iqs_data_buf[21], 0x01);

    i2c_iqs773_i2c_stop_ctrl(ENABLE_I2C_STOP);

    // System flags
    iqs_sys_flag = iqs_data_buf[0];

    // Global events
    iqs_event_flag = iqs_data_buf[1];

    // Touch UI flags
    iqs_touch_flag = iqs_data_buf[2];

    APP_PRINT_TRACE6("i2c_iqs773_read_event: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", iqs_event_flag,
                     iqs_touch_flag, iqs_data_buf[20], iqs_spp_app_idx, iqs_spp_cmd_path, iqs_work_mode);

    // Reset Indicator
    if (((iqs_sys_flag & 0x80) == 0x80) || (iqs_data_buf[20] != 0xfa))
    {
        APP_PRINT_TRACE0("IQS773 Reset\r\n");

        // Disable Interrupt
        hal_gpio_irq_disable(app_cfg_const.gsensor_int_pinmux);

        //iqs773 soft reset
        iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
        iqs_data_buf[1] = (SYSTEM_SETTINGS | 0x80);
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

        //need wait more than 150ms to sw reset
        app_start_timer(&timer_idx_iqs_sw_reset_check, "iqs_sw_reset_check",
                        psensor_iqs_timer_id, APP_TIMER_IQS_SW_RESET_CHECK, 0, false,
                        200);
    }
    else
    {
        // Prox UI Event
        if (iqs_event_flag & 0x01)
        {
            iqs_force_cnt = ((iqs_data_buf[17] << 8) + iqs_data_buf[16]);

            iqs_force_lta = ((iqs_data_buf[19] << 8) + iqs_data_buf[18]);

            iqs_force_cnt = iqs_force_cnt - iqs_force_lta;

            if ((iqs_touch_flag & 0x10) == 0x10)
            {
                if (!iqs_force_set)
                {
                    event = PSENSOR_I2C_EVENT_PRESS;
                    i2c_iqs_set_force_set(event, iqs_force_cnt);
                }

                if (timer_idx_iqs_release_debounce != 0)
                {
                    //stop timer
                    app_stop_timer(&timer_idx_iqs_release_debounce);
                    event = PSENSOR_I2C_EVENT_PRESS_INVALID;
                }
            }
            else if ((iqs_touch_flag & 0x00) == 0x00)
            {
                if (iqs_force_set)
                {
                    if (app_cfg_const.iqs_release_debounce != 0)
                    {
                        event = PSENSOR_I2C_EVENT_RELEASE_INVALID;

                        if (timer_idx_iqs_release_debounce == 0)
                        {
                            //iqs_release_debounce: 0~15(unit: 10ms)
                            app_start_timer(&timer_idx_iqs_release_debounce, "iqs_release_debounce",
                                            psensor_iqs_timer_id, APP_TIMER_IQS_RELEASE_DEBOUNCE, iqs_force_cnt, false,
                                            app_cfg_const.iqs_release_debounce * 10);
                        }
                    }
                    else
                    {
                        event = PSENSOR_I2C_EVENT_RELEASE;
                        i2c_iqs_set_force_set(event, iqs_force_cnt);
                    }
                }
            }
        }
    }

    if (iqs_work_mode == (0x80 | IQS773_STREAM_MODE))
    {
        uint8_t spp_data_buf[13];
        //ui,target,threhold
        spp_data_buf[0] = app_cfg_const.iqs_p3.iqs773_sensitivity.ati_target;
        spp_data_buf[1] = app_cfg_const.iqs773_trigger_level;
        //flag
        spp_data_buf[2] = iqs_data_buf[0];
        spp_data_buf[3] = iqs_data_buf[1];
        spp_data_buf[4] = iqs_data_buf[2];
        spp_data_buf[5] = iqs_force_set;
        //count
        spp_data_buf[6] = iqs_data_buf[16];
        spp_data_buf[7] = iqs_data_buf[17];
        //LTA
        spp_data_buf[8] = iqs_data_buf[18];
        spp_data_buf[9] = iqs_data_buf[19];
        //Delta
        iqs_force_cnt = (iqs_data_buf[17] << 8) + iqs_data_buf[16];
        iqs_force_lta = (iqs_data_buf[19] << 8) + iqs_data_buf[18];

        iqs_force_cnt -= iqs_force_lta;
        spp_data_buf[10] = iqs_force_cnt & 0xFF;
        spp_data_buf[11] = (iqs_force_cnt >> 8) & 0xFF;
        spp_data_buf[12] = iqs_data_buf[21];

        app_report_event(iqs_spp_cmd_path, EVENT_IQS773_STREAM_REPORT, iqs_spp_app_idx, spp_data_buf,
                         sizeof(spp_data_buf));
    }
    return event;
}

/***************************************************************************
 * \brief Read i2c event when iqs873 rdy pin is triggered *
***************************************************************************/

T_PSENSOR_I2C_EVENT i2c_iqs873_read_event(void)
{
    static uint8_t iqs_wear_set = 0;
    uint8_t iqs_data_buf[20];
    uint8_t iqs_sys_flag;
    uint8_t iqs_event_flag;
    //uint8_t iqs_touch_flag;
    uint8_t iqs_deep_touch_flag;
    int16_t iqs_force_cnt;
    int16_t iqs_wear_cnt;
    //int16_t iqs_hall_cnt;

    T_PSENSOR_I2C_EVENT event = PSENSOR_I2C_EVENT_NONE;

    //i2c_iqs873_i2c_stop_ctrl(DISABLE_I2C_STOP);

    // Read registers 0x02 --> 0x05
    i2c_iqs_burst_read_no_rdy(IQS873_MM_SYSTEM_FLAGS, &iqs_data_buf[0], 0x08);

    // Read registers 0x18 --> 0x19 (CH0 & CH1)
    i2c_iqs_burst_read_no_rdy(IQS873_MM_CH_DELTAS, &iqs_data_buf[10], 0x04);

    // Read registers 0x1F (CH7)
    i2c_iqs_burst_read_no_rdy((IQS873_MM_CH_DELTAS + 0x07), &iqs_data_buf[14], 0x02);

    //i2c_iqs873_i2c_stop_ctrl(ENABLE_I2C_STOP);

    // System flags
    iqs_sys_flag = iqs_data_buf[0];

    // Global events
    iqs_event_flag = iqs_data_buf[1];

    // Touch UI flags
    iqs_deep_touch_flag = iqs_data_buf[7];

    APP_PRINT_TRACE3("i2c_iqs873_read_event: %02x %02x %02x", iqs_sys_flag, iqs_event_flag,
                     iqs_deep_touch_flag);

    // Channel deltas
    iqs_wear_cnt = ((iqs_data_buf[11] << 8) + iqs_data_buf[10]);
    iqs_force_cnt = ((iqs_data_buf[13] << 8) + iqs_data_buf[12]);
    //iqs_hall_cnt = ((iqs_data_buf[15]<<8)+iqs_data_buf[14]);

    // Reset Indicator
    if ((iqs_sys_flag & 0x80) == 0x80)
    {
        APP_PRINT_TRACE0("IQS873 Reset\r\n");
        i2c_iqs873_setup();
    }
    else
    {
        // Deep Touch UI Event
        if (iqs_event_flag & 0x04)
        {
            //Wear Touch
            if (iqs_deep_touch_flag & 0x01)
            {
                // In-Ear
                if (iqs_wear_cnt > 0)
                {
                    if (!iqs_wear_set)
                    {
                        iqs_wear_set = 1;

                        // Reseed CH0
                        iqs_data_buf[0] = 0x8B;
                        iqs_data_buf[1] = UIS_GESTURE_THRESHOLD;
                        iqs_data_buf[2] = 0x01;
                        i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

                        // Inverse logic
                        iqs_data_buf[0] = 0x8D;
                        iqs_data_buf[1] = PXS_PROXCTRL_CH0;
                        iqs_data_buf[2] = (PXS_PROXCFG0_CH0 | 0x80);
                        i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

                        APP_PRINT_TRACE1("Wear Touch 1  Delta:%d\n", iqs_wear_cnt);

                        event |= PSENSOR_I2C_EVENT_IN_EAR;
                    }
                }

                // Out-Ear
                if (iqs_wear_cnt < 0)
                {
                    if (iqs_wear_set)
                    {
                        iqs_wear_set = 0;

                        // Reseed CH0
                        iqs_data_buf[0] = 0x8B;
                        iqs_data_buf[1] = UIS_GESTURE_THRESHOLD;
                        iqs_data_buf[2] = 0x01;
                        i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

                        // Inverse logic
                        iqs_data_buf[0] = 0x8D;
                        iqs_data_buf[1] = PXS_PROXCTRL_CH0;
                        iqs_data_buf[2] = PXS_PROXCFG0_CH0;
                        i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

                        APP_PRINT_TRACE1("Wear Touch 0  Delta:%d\n", iqs_wear_cnt);

                        event |= PSENSOR_I2C_EVENT_OUT_EAR;
                    }
                }

            }

            // Force Touch
            if (iqs_deep_touch_flag & 0x02)
            {
                if (!iqs_force_set)
                {
                    event |= PSENSOR_I2C_EVENT_PRESS;
                    i2c_iqs_set_force_set(event, iqs_force_cnt);
                }

                if (timer_idx_iqs_release_debounce != 0)
                {
                    //stop timer
                    app_stop_timer(&timer_idx_iqs_release_debounce);
                    event |= PSENSOR_I2C_EVENT_PRESS_INVALID;
                }
            }
            else
            {
                if (iqs_force_set)
                {
                    if (app_cfg_const.iqs_release_debounce != 0)
                    {
                        event |= PSENSOR_I2C_EVENT_RELEASE_INVALID;

                        if (timer_idx_iqs_release_debounce == 0)
                        {
                            //iqs_release_debounce: 0~15(unit: 10ms)
                            app_start_timer(&timer_idx_iqs_release_debounce, "iqs_release_debounce",
                                            psensor_iqs_timer_id, APP_TIMER_IQS_RELEASE_DEBOUNCE, iqs_force_cnt, false,
                                            app_cfg_const.iqs_release_debounce * 10);
                        }
                    }
                    else
                    {
                        event |= PSENSOR_I2C_EVENT_RELEASE;
                        i2c_iqs_set_force_set(event, iqs_force_cnt);
                    }
                }
            }
        }
    }

    return event;
}

static void i2c_iqs_set_force_set(T_PSENSOR_I2C_EVENT event, int16_t iqs_force_cnt)
{
    if ((event & PSENSOR_I2C_EVENT_PRESS) && (!iqs_force_set))
    {
        iqs_force_set = 1;

        if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)
        {
            uint8_t iqs_data_buf[20];

            // CH1 Reseed Enable
            iqs_data_buf[0] = 0x82;
            iqs_data_buf[1] = (LTA_CHB_RESEED_ENABLED | 0x02);
            i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
        }
    }
    else if ((event & PSENSOR_I2C_EVENT_RELEASE) && (iqs_force_set))
    {
        iqs_force_set = 0;

        if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)
        {
            uint8_t iqs_data_buf[20];

            // CH1 Reseed Disable
            iqs_data_buf[0] = 0x82;
            iqs_data_buf[1] = LTA_CHB_RESEED_ENABLED;
            i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);
        }
    }

    APP_PRINT_TRACE2("i2c_iqs_set_force_set %d  Delta:%d\n", iqs_force_set, iqs_force_cnt);
}

static void i2c_iqs_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_IQS_RELEASE_DEBOUNCE:
        {
            int16_t iqs_force_cnt = param;

            app_stop_timer(&timer_idx_iqs_release_debounce);

            if (app_cfg_const.psensor_support &&
                (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773 ||
                 app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873))
            {
                app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

                i2c_iqs_set_force_set(PSENSOR_I2C_EVENT_RELEASE, iqs_force_cnt);
                app_psensor_iqs_key_handle(PSENSOR_I2C_EVENT_RELEASE);

                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
            }
        }
        break;

#if (APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG == 1)
    case APP_TIMER_IQS_ADDR_LOG:
        {
            app_stop_timer(&timer_idx_iqs_addr_log);


            uint8_t iqs_data_dbg_20[2];
            uint8_t iqs_data_dbg_30[2];

            i2c_iqs773_i2c_stop_ctrl(DISABLE_I2C_STOP);

            i2c_iqs_burst_read_no_rdy(0x20, &iqs_data_dbg_20[0], 0x02);
            i2c_iqs_burst_read_no_rdy(0x30, &iqs_data_dbg_30[0], 0x02);

            i2c_iqs773_i2c_stop_ctrl(ENABLE_I2C_STOP);

            APP_PRINT_TRACE6("[idbg]i2c_iqs773_read_addr_val: addr_20:0x%02x, addr_21:0x%02x, addr_30:0x%02x ,addr_31:0x%02x ,count: %d, LTA: %d",
                             iqs_data_dbg_20[0], iqs_data_dbg_20[1], iqs_data_dbg_30[0], iqs_data_dbg_30[1],
                             iqs_data_dbg_20[0] + (iqs_data_dbg_20[1] << 8), iqs_data_dbg_30[0] + (iqs_data_dbg_30[1] << 8));

            i2c_iqs_addr_log();

        }
        break;
#endif

    case APP_TIMER_IQS_SW_RESET_CHECK:
        {
            app_stop_timer(&timer_idx_iqs_sw_reset_check);

            //iqs773 init to event mode
            i2c_iqs773_setup();
            // power on P-sensor software reset need exit halt mode
            i2c_iqs773_halt_mode_exit();
            // Enable Interrupt
            hal_gpio_irq_enable(app_cfg_const.gsensor_int_pinmux);
        }
        break;

    default:
        break;
    }
}

#if (APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG == 1)
static void i2c_iqs_addr_log(void)
{
    app_start_timer(&timer_idx_iqs_addr_log, "iqs_addr_log",
                    psensor_iqs_timer_id, APP_TIMER_IQS_ADDR_LOG, 0, false,
                    50);
}
#endif

void i2c_iqs_initial(void)
{
    app_timer_reg_cb(i2c_iqs_timeout_cb, &psensor_iqs_timer_id);

#if (APP_SENSOR_IQS773_873_VENDOR_FINE_TUNE_PARAM_LOG == 1)
    if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)
    {
        i2c_iqs_addr_log();
    }
#endif
}


#endif
