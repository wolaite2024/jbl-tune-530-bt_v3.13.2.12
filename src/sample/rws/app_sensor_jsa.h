/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __APP_SENSOR_JSA__
#define __APP_SENSOR_JSA__

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_utils.h"

//I2C Slave address
#define SENSOR_ADDR_JSA                 0x38
#define SENSOR_ADDR_JSA1227             0x38

#define SENSOR_INT_TRIGGERED            0
#define SENSOR_INT_RELEASED             1

//Vendor Defines
#define SENSOR_JSA_CAL_TIMES            10
#define SENSOR_JSA_CAL_XTALK            0
#define SENSOR_JSA_LOW_THRES            1
#define SENSOR_JSA_HIGH_THRES           2
#define SENSOR_JSA_WRITE_INT_TIME       3
#define SENSOR_JSA_WRITE_PS_GAIN        4
#define SENSOR_JSA_WRITE_PS_PULSE       5
#define SENSOR_JSA_READ_PS_DATA         6
#define SENSOR_JSA_READ_CAL_DATA        7
#define SENSOR_JSA_WRITE_LOW_THRES      8
#define SENSOR_JSA_WRITE_HIGH_THRES     9

//Vensor register
#define SENSOR_REG_JSA_SYSM_CTRL        0x00
#define SENSOR_REG_JSA_INT_CTRL         0x01
#define SENSOR_REG_JSA_INT_FLAG         0x02
#define SENSOR_REG_JSA_WAIT_TIME        0x03
#define SENSOR_REG_JSA_INT_TIME         0x06
#define SENSOR_REG_JSA_PS_GAIN          0x07
#define SENSOR_REG_JSA_PS_PULSE         0x08
#define SENSOR_REG_JSA_PS_TIME          0x09
#define SENSOR_REG_JSA_AVG              0x0A
#define SENSOR_REG_JSA_PERSISTENCE      0x0B
#define SENSOR_REG_JSA_PS_THRES_LL      0x10
#define SENSOR_REG_JSA_PS_THRES_LH      0x11
#define SENSOR_REG_JSA_PS_THRES_HL      0x12
#define SENSOR_REG_JSA_PS_THRES_HH      0x13
#define SENSOR_REG_JSA_PS_OFFSET_L      0x14
#define SENSOR_REG_JSA_PS_OFFSET_H      0x15
#define SENSOR_REG_JSA_PS_DATA_L        0x18
#define SENSOR_REG_JSA_PS_DATA_H        0x19

#define SENSOR_VAL_JSA_PS_DATA_MAX      0x03ff

//JSA1227
//Sensor register
#define SENSOR_REG_JSA1227_SYSTEM_CONTROL              0x00  //Control of basic functions
#define SENSOR_REG_JSA1227_INT_CTRL                    0x01  //Interrupt enable and auto/semi clear INT selector, duke 20190807
#define SENSOR_REG_JSA1227_INT_FLAG                    0x02  //ALS and PS interrupt status and flag output, duke 20190807
#define SENSOR_REG_JSA1227_WAIT_TIME                   0x03  //Waiting Time Setup
#define SENSOR_REG_JSA1227_PS_GAIN                     0x06  //Gain control of PS 
#define SENSOR_REG_JSA1227_PS_PULSE                    0x07  //VCSEL led pulse width
#define SENSOR_REG_JSA1227_PS_TIME                     0x09  //resolution tuning of PS
#define SENSOR_REG_JSA1227_PS_FILTER                   0x0A  //Hard ware Average filter of PS
#define SENSOR_REG_JSA1227_PS_PERSISTENCE              0x0B  //Configure the Persist count of PS
#define SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_L          0x10  //Lower part of PS low threshold
#define SENSOR_REG_JSA1227_PS_LOW_THRESHOLD_H          0x11  //Upper part of PS low threshold
#define SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_L         0x12  //Lower part of PS high threshold
#define SENSOR_REG_JSA1227_PS_HIGH_THRESHOLD_H         0x13  //Upper part of PS high threshold
#define SENSOR_REG_JSA1227_PS_CALIBRATION_L            0x14  //Offset value to eliminate the Cross talk effect
#define SENSOR_REG_JSA1227_PS_CALIBRATION_H            0x15  //Offset value to eliminate the Cross talk effect
#define SENSOR_REG_JSA1227_PS_DATA_LOW                 0x1A  //Low byte for PS ADC channel output
#define SENSOR_REG_JSA1227_PS_DATA_HIGH                0x1B  //High byte for PS ADC channel output
#define SENSOR_REG_JSA1227_CALIB_CTRL                  0x26
#define SENSOR_REG_JSA1227_CALIB_STAT                  0x28
#define SENSOR_REG_JSA1227_MANU_CDAT_L                 0x2A
#define SENSOR_REG_JSA1227_MANU_CDAT_H                 0x2B
#define SENSOR_REG_JSA1227_AUTO_CDAT_L                 0x2C
#define SENSOR_REG_JSA1227_AUTO_CDAT_H                 0x2D
#define SENSOR_REG_JSA1227_PS_PIPE_THRES               0x2E

#define SENSOR_VAL_JSA1227_VCSEL_CURRENT_10mA          0x10
#define SENSOR_VAL_JSA1227_PSCONV                      0x03  //  output data maxmum value is 0~F
#define SENSOR_VAL_JSA1227_NUM_AVG                     0x03  //  Average filer times that is maxmum value during 0~F


typedef struct t_jsa_cal_para
{
    uint16_t sensor_xtalk;
    uint16_t sensor_low_thres;
    uint16_t sensor_high_thres;
    uint8_t  sensor_integration_time;
    uint8_t  sensor_gain;
    uint8_t  sensor_pulse;
    uint8_t  rsv[3]; /* need 4 byte align, save for future */
} T_JSA_CAL_PARA;

typedef enum
{
    JSA_CAL_SUCCESS      = 0x00,
    JSA_CAL_FAIL         = 0x01,
} T_JSA_CAL_RETURN;

void app_sensor_jsa1225_init(void);
void app_sensor_jsa1227_init(void);

void app_sensor_jsa_enable(void);
void app_sensor_jsa_disable(void);
void app_sensor_jsa_int_gpio_intr_cb(uint32_t param);

T_JSA_CAL_RETURN app_sensor_jsa1225_calibration(uint8_t cal_mode, uint16_t *spp_result);
T_JSA_CAL_RETURN app_sensor_jsa1227_calibration(uint8_t cal_mode, uint16_t *spp_result);

void app_sensor_jsa_write_data(uint8_t target, uint8_t *val);
void app_sensor_jsa_read_cal_data(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
