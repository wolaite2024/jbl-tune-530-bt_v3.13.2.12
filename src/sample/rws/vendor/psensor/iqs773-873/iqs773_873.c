#include "rtl876x.h"
#include "rtl876x_gpio.h"
#include "rtl876x_i2c.h"
#include "rtl_delay.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "iqs773_873.h"
#include "iqs773_init.h"
#include "iqs873_init.h"


/***************************************************************************
 * \brief Writes data packet to slave
 *
 * Writes a data packet to the specified slave address on the I2C bus
 * and sends a stop or ack condition when finished.
 *
 **************************************************************************/
void i2c_iqs_burst_write(uint8_t *write_data, uint8_t write_size)
{
    WaitRdyLow();

    I2C_MasterWrite(I2C0, write_data, write_size);

    WaitRdyHigh();
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
    I2C_MasterWrite(I2C0, write_data, write_size);
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
    WaitRdyLow();

    I2C_RepeatRead(I2C0, &read_addr, 1, read_data, read_size);

    WaitRdyHigh();
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
    I2C_RepeatRead(I2C0, &read_addr, 1, read_data, read_size);
}


/***************************************************************************
 * Description: Check if the module uses iqs773 or iqs873.
 *
 **************************************************************************/
int8_t i2c_iqs_check_device(void)
{
    uint8_t status_flag;
    uint8_t iqs_data_buf[4];

    // Sync communication window
    WaitRdyLow(); WaitRdyHigh();

    // Device Recognition
    i2c_iqs_burst_read(IQS773_PROD_NUM, &iqs_data_buf[0], 0x04);

    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Chip Product ID 0x%02X  Software 0x%02X\r\n", 2,
               iqs_data_buf[0], iqs_data_buf[1]);

    // IQS773
    if (iqs_data_buf[0] == 0x41 && iqs_data_buf[1] >= 0x08)
    {
        // Soft Reset
        iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
        iqs_data_buf[1] = (SYSTEM_SETTINGS | 0x80);
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

        delayMS(150);

        // Private Bit
        i2c_iqs_burst_read(IQS773_SYSTEM_SETTINGS_0, &status_flag, 0x01);

        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Private Bit 0x%02X\r\n", 1, status_flag);

        if (status_flag == 0x08)
        {
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Find IQS773 Module\r\n", 0);

            return IQS773_MODULE;
        }
    }
    // IQS873
    else if (iqs_data_buf[0] == 0x4F && iqs_data_buf[1] >= 0x03 && iqs_data_buf[3] >= 0x03)
    {
        // Soft Reset
        iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
        iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
        iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | 0x02);
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x03);

        delayMS(150);

        // Private Bit
        i2c_iqs_burst_read(IQS873_MM_PRIVATE_FLAGS, &status_flag, 0x01);

        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Private Bit 0x%02X\r\n", 1, status_flag);

        if (status_flag == 0x01)
        {
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Find IQS873 Module\r\n", 0);

            return IQS873_MODULE;
        }
    }

    return false;
}


/***************************************************************************
 * Description: Configures the iqs773 for general working condition.
 *
 **************************************************************************/
void i2c_iqs773_setup(void)
{
#define IQS773_HALL_ENABLE  1

#define IQS773_HALL_DISABLE 0

    uint8_t iqs_data_buf[24];

    uint8_t iqs_hall_enable = IQS773_HALL_DISABLE;

    // Force to Stream Mode
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = SYSTEM_SETTINGS;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x02);

    // Sync communication window
    WaitRdyLow(); WaitRdyHigh();

    // Device and power mode settings
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = (SYSTEM_SETTINGS | ACK_RESET);
    iqs_data_buf[2] = ACTIVE_CHS;

    if (iqs_hall_enable == IQS773_HALL_ENABLE)
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

    if (iqs_hall_enable == IQS773_HALL_ENABLE)
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
        // Test
        i2c_iqs773_np_report_rate(16);
        i2c_iqs773_ulp_report_rate(16);
        i2c_iqs773_rdy_timeout(255);

        i2c_iqs773_force_ati_tgt(15);
        i2c_iqs773_force_touch_thr(13);
        i2c_iqs773_force_touch_bias(2);
        i2c_iqs773_force_touch_halt(40);

        i2c_iqs773_hall_ati_tgt(35);
        i2c_iqs773_hall_touch_thr(25);

        // Debug String
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "UI Tool Settings Setup Finish\r\n", 0);
    }

    // Redo ATI
    i2c_iqs773_redo_ati_all();

    // Event Mode
    iqs_data_buf[0] = IQS773_SYSTEM_SETTINGS_0;
    iqs_data_buf[1] = (SYSTEM_SETTINGS | EVENT_MODE);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);

    // Debug String
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IQS773 Setup Finish\r\n", 0);
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
void i2c_iqs773_force_ati_tgt(uint8_t iqs_tgt)
{
    uint8_t iqs_base_value = PXS_SETTINGS0_6;
    uint8_t iqs_data_buf[2];

    // Store base value
    iqs_base_value &= 0xC0;

    // New target
    iqs_tgt &= 0x3F;

    iqs_data_buf[0] = IQS773_PXS_SETTINGS_2_0;
    iqs_data_buf[1] = (iqs_base_value | iqs_tgt);
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

    uint16_t time_out = 1000;

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

        delayMS(1);

        i2c_iqs_burst_read(0x10, &status_flag, 0x01);

        status_flag &= 0x04;

        time_out--;

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
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Touch Source %d  Coarse:%d  fine:%d  Comp:%d\r\n", 4,
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


/***************************************************************************
 * Description: Configures the iqs873 for general working condition.
 *
 **************************************************************************/
void i2c_iqs873_setup(void)
{
    uint8_t iqs_data_buf[64];

    uint8_t ch_enable = SYS_CHB_ACTIVE;

    uint8_t hall_enable = HALL_UI_ENABLE;

    // Force to Stream Mode
    iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = I2C_GENERAL_SETTINGS;
    i2c_iqs_burst_write_no_rdy((uint8_t *)&iqs_data_buf[0], 0x03);

    // Sync communication window
    WaitRdyHigh(); WaitRdyLow();

    // PMU and System settings
    iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | 0x01);
    iqs_data_buf[3] = SYS_CHB_ACTIVE;
    iqs_data_buf[4] = ACF_LTA_FILTER_SETTINGS;
    iqs_data_buf[5] = LTA_CHB_RESEED_ENABLED;
    iqs_data_buf[6] = UIS_GLOBAL_EVENTS_MASK;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x07);

    // Report rates and timings
    iqs_data_buf[0] = IQS873_MM_RPT_SETTINGS;
    iqs_data_buf[1] = PMU_REPORT_RATE_NP;
    iqs_data_buf[2] = PMU_REPORT_RATE_LP;
    iqs_data_buf[3] = PMU_REPORT_RATE_ULP;
    iqs_data_buf[4] = PMU_MODE_TIMOUT;
    iqs_data_buf[5] = I2C_WINDOW_TIMEOUT;
    iqs_data_buf[6] = LTA_HALT_TIMEOUT;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x07);

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
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0D);

    // Channel 0 settings
    if (ch_enable & 0x01)
    {
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
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 1 settings
    if (ch_enable & 0x02)
    {
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
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 2 settings
    if (ch_enable & 0x04)
    {
        iqs_data_buf[0] = IQS873_MM_CH2_SELECT_CRX;
        iqs_data_buf[1] = PXS_CRXSEL_CH2;
        iqs_data_buf[2] = PXS_CTXSEL_CH2;
        iqs_data_buf[3] = PXS_PROXCTRL_CH2;
        iqs_data_buf[4] = PXS_PROXCFG0_CH2;
        iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH2;
        iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH2;
        iqs_data_buf[7] = ATI_MIRROR_CH2;
        iqs_data_buf[8] = ATI_PCC_CH2;
        iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH2;
        iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH2;
        iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH2;
        iqs_data_buf[12] = PXS_HYSTERESIS_CH2;
        iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH2;
        iqs_data_buf[14] = DCF_WEIGHT_CH2;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 3 settings
    if (ch_enable & 0x08)
    {
        iqs_data_buf[0] = IQS873_MM_CH3_SELECT_CRX;
        iqs_data_buf[1] = PXS_CRXSEL_CH3;
        iqs_data_buf[2] = PXS_CTXSEL_CH3;
        iqs_data_buf[3] = PXS_PROXCTRL_CH3;
        iqs_data_buf[4] = PXS_PROXCFG0_CH3;
        iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH3;
        iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH3;
        iqs_data_buf[7] = ATI_MIRROR_CH3;
        iqs_data_buf[8] = ATI_PCC_CH3;
        iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH3;
        iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH3;
        iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH3;
        iqs_data_buf[12] = PXS_HYSTERESIS_CH3;
        iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH3;
        iqs_data_buf[14] = DCF_WEIGHT_CH3;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 4 settings
    if (ch_enable & 0x10)
    {
        iqs_data_buf[0] = IQS873_MM_CH4_SELECT_CRX;
        iqs_data_buf[1] = PXS_CRXSEL_CH4;
        iqs_data_buf[2] = PXS_CTXSEL_CH4;
        iqs_data_buf[3] = PXS_PROXCTRL_CH4;
        iqs_data_buf[4] = PXS_PROXCFG0_CH4;
        iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH4;
        iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH4;
        iqs_data_buf[7] = ATI_MIRROR_CH4;
        iqs_data_buf[8] = ATI_PCC_CH4;
        iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH4;
        iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH4;
        iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH4;
        iqs_data_buf[12] = PXS_HYSTERESIS_CH4;
        iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH4;
        iqs_data_buf[14] = DCF_WEIGHT_CH4;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 5 settings
    if (ch_enable & 0x20)
    {
        iqs_data_buf[0] = IQS873_MM_CH5_SELECT_CRX;
        iqs_data_buf[1] = PXS_CRXSEL_CH5;
        iqs_data_buf[2] = PXS_CTXSEL_CH5;
        iqs_data_buf[3] = PXS_PROXCTRL_CH5;
        iqs_data_buf[4] = PXS_PROXCFG0_CH5;
        iqs_data_buf[5] = PXS_PROXCFG1_TESTREG0_CH5;
        iqs_data_buf[6] = ATI_BASE_AND_TARGET_CH5;
        iqs_data_buf[7] = ATI_MIRROR_CH5;
        iqs_data_buf[8] = ATI_PCC_CH5;
        iqs_data_buf[9] = PXS_PROX_THRESHOLD_CH5;
        iqs_data_buf[10] = PXS_TOUCH_THRESHOLD_CH5;
        iqs_data_buf[11] = PXS_DEEP_THRESHOLD_CH5;
        iqs_data_buf[12] = PXS_HYSTERESIS_CH5;
        iqs_data_buf[13] = DCF_CHB_ASSOCIATION_CH5;
        iqs_data_buf[14] = DCF_WEIGHT_CH5;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 6 settings
    if (ch_enable & 0x40)
    {
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
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Channel 7 settings
    if (ch_enable & 0x80)
    {
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
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x0F);
    }

    // Hall sensor settings
    if (hall_enable & 0x80)
    {
        // Hall UI Enable
        iqs_data_buf[0] = IQS873_MM_HALL_UI_EN;
        iqs_data_buf[1] = HALL_UI_ENABLE;
        i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x02);
    }

    // Redo ATI
    i2c_iqs873_redo_ati_ch(ch_enable);

    // Event Mode
    iqs_data_buf[0] = IQS873_MM_PMU_SETTINGS;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | 0x20);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x03);

    // Debug String
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IQS873 Setup Finish\r\n", 0);
}


/***************************************************************************
 * \brief Doing Redo-ATI for enable channels
 *
***************************************************************************/
void i2c_iqs873_redo_ati_ch(uint8_t ch)
{
    uint8_t status_flag;

    uint8_t iqs_data_buf[3];

    uint16_t time_out = 1000;

    // Enable channel reseed selection
    iqs_data_buf[0] = 0x8B;
    iqs_data_buf[1] = UIS_GESTURE_THRESHOLD;
    iqs_data_buf[2] = ch;
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x03);

    // Redo ATI
    iqs_data_buf[0] = 0x80;
    iqs_data_buf[1] = PMU_GENERAL_SETTINGS;
    iqs_data_buf[2] = (I2C_GENERAL_SETTINGS | 0x04);
    i2c_iqs_burst_write((uint8_t *)&iqs_data_buf[0], 0x03);

    // Wait for Redo ATI to complete
    do
    {
        if (!time_out)
        {
            break;
        }

        delayMS(1);

        i2c_iqs_burst_read(0x02, &status_flag, 0x01);

        status_flag &= 0x04;

        time_out--;

    }
    while (status_flag);
}


/***************************************************************************
 * \brief Read compensation & multiplier
 *
***************************************************************************/
void i2c_iqs873_read_compensation(void)
{
    uint8_t i, index;
    uint8_t iqs_data_buf[16];

    uint8_t iqs_coarse, iqs_fine;
    int16_t iqs_comp;

    i2c_iqs_burst_read(IQS873_MM_CH1_COMPENSATION, &iqs_data_buf[0], 0x02);
    i2c_iqs_burst_read(IQS873_MM_CH2_COMPENSATION, &iqs_data_buf[2], 0x02);
    i2c_iqs_burst_read(IQS873_MM_CH3_COMPENSATION, &iqs_data_buf[4], 0x02);
    i2c_iqs_burst_read(IQS873_MM_CH4_COMPENSATION, &iqs_data_buf[6], 0x02);
    i2c_iqs_burst_read(IQS873_MM_CH5_COMPENSATION, &iqs_data_buf[8], 0x02);
    i2c_iqs_burst_read(IQS873_MM_CH7_COMPENSATION, &iqs_data_buf[10], 0x02);

    for (i = 0; i < 6; i++)
    {
        index = i << 1;

        // Coarse
        iqs_coarse = (iqs_data_buf[0 + index] & 0x30) >> 4;

        // Fine
        iqs_fine = iqs_data_buf[0 + index] & 0x0F;

        // Compensation
        iqs_comp = (int16_t)(((iqs_data_buf[0 + index] & 0xC0) << 2) + iqs_data_buf[1 + index]);

        // Debug String
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Touch Source %d  Coarse:%d  fine:%d  Comp:%d\r\n", 4,
                   i, iqs_coarse, iqs_fine, iqs_comp);
    }
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
