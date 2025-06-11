/*
* This file contains all the necessary settings for the iqs773_873 and this file can
* be changed from the GUI or edited here
* File:   iqs773_873.h
* Author: Azoteq
*/

#ifndef _IQS773_873_H_
#define _IQS773_873_H_

#include "rtl876x.h"
#include "hal_gpio.h"
#include "app_cfg.h"
#include "app_sensor.h"
#include "trace.h"

/***************************************************************************

**************************************************************************/

/* this is not necessary; this will make system block when smart charger box enabled */
#define WaitRdyLow()                    {uint16_t count = 0; while(hal_gpio_get_input_level(app_cfg_const.gsensor_int_pinmux) && count++ < 20000){;};APP_PRINT_TRACE1("WaitRdyLow iqs %d",count);}

#define WaitRdyHigh()                   {uint16_t count = 0; while(!hal_gpio_get_input_level(app_cfg_const.gsensor_int_pinmux) && count++ < 10000){;};APP_PRINT_TRACE1("WaitRdyHigh iqs %d",count);}

#define I2C_AZQ_ADDR                    0x44

typedef enum
{
    IQS_MODULE_NOT_FOUND,
    IQS773_MODULE,
    IQS873_MODULE,
} T_IQS_MODULE;

#define ENABLE_I2C_STOP                 0x01
#define DISABLE_I2C_STOP                0x00

/* Bit definitions */
#define SOFT_RESET                      0x80
#define ACK_RESET                       0x40
#define EVENT_MODE                      0x20
#define FOSC_4MHZ                       0x10
#define COMMS_IN_ATI                    0x08
#define SMALL_ATI_BAND                  0x04
#define REDO_ATI_ALL                    0x02
#define DO_RESEED                       0x01

/* Bit definitions for iqs873*/
#define IQS873_ACK_RESET                0x01
#define IQS873_SOFT_RESET               0x02
#define IQS873_REDO_ATI_ALL             0x04
#define IQS873_COMMS_ATI                0x08

/*********************** IQS773 REGISTERS *************************************/
#define IQS773_PROD_NUM                 0x00
#define IQS773_SYSTEM_FLAGS             0x10
#define IQS773_CH_COUNTS                0x20
#define IQS773_CH_LTA                   0x30
#define IQS773_PXS_SETTINGS_0_0         0x40
#define IQS773_PXS_SETTINGS_1_0         0x43
#define IQS773_PXS_SETTINGS_2_0         0x46
#define IQS773_PXS_SETTINGS_3_0         0x49
#define IQS773_PXS_SETTINGS_4           0x50
#define IQS773_PXS_COMPENSATION         0x52
#define IQS773_PXS_MULTIPLIER           0x55
#define IQS773_P_THR_CH0                0x60
#define IQS773_T_THR_CH0                0x61
#define IQS773_UI_HALT_TIME             0x66
#define IQS773_SAR_UI_SETTINGS_0        0x70
#define IQS773_HYST_UI_SETTINGS         0x80
#define IQS773_HALL_SEN_SETTINGS_0      0x90
#define IQS773_HALL_SEN_SETTINGS_1      0x91
#define IQS773_HALL_COMPENSATION        0x92
#define IQS773_HALL_UI_SETTINGS_0       0xA0
#define IQS773_HALL_UI_P_THR            0xA1
#define IQS773_HALL_UI_T_THR            0xA2
#define IQS773_TEMP_UI_SETTINGS_0       0xC0
#define IQS773_SYSTEM_SETTINGS_0        0xD0
#define IQS773_ACTIVE_CHAN              0xD1
#define IQS773_PMU_SETTINGS_0           0xD2
#define IQS773_NORMAL_REPORT_RATE       0xD3
#define IQS773_LP_REPORT_RATE           0xD4
#define IQS773_ULP_REPORT_RATE          0xD5
#define IQS773_MODE_TIME                0xD6
#define IQS773_RDY_TIMEOUT              0xD9
#define IQS773_GLOBAL_EVENT_REG         0x00


/*********************** IQS873 REGISTERS *************************************/

#define IQS873_MM_PRODUCT_NUMBER        0x00
#define IQS873_MM_SYSTEM_FLAGS          0x02
#define IQS873_MM_CH_DELTAS             0x18
#define IQS873_MM_PRIVATE_FLAGS         0x36
#define IQS873_MM_PMU_SETTINGS          0x80
#define IQS873_MM_RPT_SETTINGS          0x83
#define IQS873_MM_PXS_GLBL_SETTINGS_0   0x86
#define IQS873_MM_CH0_SELECT_CRX        0x8C
#define IQS873_MM_CH0_COMPENSATION      0x8F
#define IQS873_MM_CH1_SELECT_CRX        0x93
#define IQS873_MM_CH1_COMPENSATION      0x96
#define IQS873_MM_CH2_SELECT_CRX        0x9A
#define IQS873_MM_CH2_COMPENSATION      0x9D
#define IQS873_MM_CH3_SELECT_CRX        0xA1
#define IQS873_MM_CH3_COMPENSATION      0xA4
#define IQS873_MM_CH4_SELECT_CRX        0xA8
#define IQS873_MM_CH4_COMPENSATION      0xAB
#define IQS873_MM_CH5_SELECT_CRX        0xAF
#define IQS873_MM_CH5_COMPENSATION      0xB2
#define IQS873_MM_CH6_SELECT_CRX        0xB6
#define IQS873_MM_CH6_COMPENSATION      0xB9
#define IQS873_MM_CH7_SELECT_CRX        0xBD
#define IQS873_MM_CH7_COMPENSATION      0xC0
#define IQS873_MM_I2C_CTRL_SETTING      0xF2
#define IQS873_MM_HALL_UI_EN            0xF5

//for CMD parse
#define IQS773_CALI_WORK_MODE_SET       0
#define IQS773_CALI_SENSITIVITY_SET     1
//--------------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------------
void i2c_iqs_burst_write(uint8_t *write_data, uint8_t write_size);
void i2c_iqs_burst_write_no_rdy(uint8_t *write_data, uint8_t write_size);

void i2c_iqs_burst_read(uint8_t read_addr, uint8_t *read_data, uint8_t read_size);
void i2c_iqs_burst_read_no_rdy(uint8_t read_addr, uint8_t *read_data, uint8_t read_size);

T_IQS_MODULE i2c_iqs_check_device(void);

void i2c_iqs773_setup(void);
void i2c_iqs773_active_ch(uint8_t iqs_ch);
void i2c_iqs773_np_report_rate(uint8_t iqs_report_rate);
void i2c_iqs773_ulp_report_rate(uint8_t iqs_report_rate);
void i2c_iqs773_rdy_timeout(uint8_t iqs_rdy_timeout);
void i2c_iqs773_force_ati_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt);
void i2c_iqs773_force_touch_thr(uint8_t iqs_thr);
void i2c_iqs773_force_touch_bias(uint8_t iqs_bias);
void i2c_iqs773_force_touch_halt(uint8_t iqs_timeout);
void i2c_iqs773_hall_ati_tgt(uint8_t iqs_tgt);
void i2c_iqs773_hall_touch_thr(uint8_t iqs_thr);
void i2c_iqs773_redo_ati_all(void);
void i2c_iqs773_read_compensation(void);
void i2c_iqs773_i2c_stop_ctrl(uint8_t ctrl);
void i2c_iqs773_work_mode_set(uint8_t app_idx, uint8_t cmd_path, uint8_t mode);
void i2c_iqs773_sensitivity_level_set(uint8_t app_idx, uint8_t cmd_path, uint8_t sensitivity,
                                      uint8_t trig_level);
void i2c_iqs773_halt_mode_enter(void);
void i2c_iqs773_halt_mode_exit(void);
void i2c_iqs773_charge_frequency(uint8_t iqs_charge_freq);
void i2c_iqs773_prox_threshold(uint8_t iqs_prox_thr);

void i2c_iqs873_setup(void);
void i2c_iqs873_redo_ati_ch(uint8_t ch);
void i2c_iqs873_read_compensation(void);
void i2c_iqs873_i2c_stop_ctrl(uint8_t ctrl);
void i2c_iqs873_active_ch(uint8_t iqs_ch);
void i2c_iqs873_np_report_rate(uint8_t iqs_report_rate);
void i2c_iqs873_ulp_report_rate(uint8_t iqs_report_rate);

void i2c_iqs873_wear_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt);
void i2c_iqs873_wear_touch_thr(uint8_t iqs_thr);

void i2c_iqs873_force_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt);
void i2c_iqs873_force_touch_thr(uint8_t iqs_thr);
void i2c_iqs873_force_touch_halt(uint8_t iqs_timeout);

void i2c_iqs873_hall_base_tgt(uint8_t iqs_base, uint8_t iqs_tgt);
void i2c_iqs873_hall_touch_thr(uint8_t iqs_thr);



T_PSENSOR_I2C_EVENT i2c_iqs773_read_event(void);
T_PSENSOR_I2C_EVENT i2c_iqs873_read_event(void);

void i2c_iqs_initial(void);

#endif  /* _IQS773_H_ */
