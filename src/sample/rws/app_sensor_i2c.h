/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      app_sensor_i2c.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _APP_SENSOR_I2C_H_
#define _APP_SENSOR_I2C_H_

#include <stdbool.h>
#include <stdint.h>

#include "os_sync.h"
#include "rtl876x_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APP_SENSROR_I2C      App Sensor I2C
  * @brief App Sensor I2C
  * @{
  */

/*============================================================================*
 *                         define
 *============================================================================*/
typedef enum
{
    JSA1227_I2C_CLK_SPD      = 100000,
    PX318J_I2C_CLK_SPD       = 100000,
    IQS773_873_I2C_CLK_SPD   = 200000,
    MEMS_I2C_CLK_SPD         = 400000,
    DEF_I2C_CLK_SPD          = 400000,
} T_I2C_CLK_SPD;

#define I2C_LOCK(handle)                (handle)?((os_mutex_take(handle,0xFFFFFFFF))?true:false):false
#define I2C_UNLOCK(handle,falg)         (falg)?(os_mutex_give(handle)):false

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
    * @brief   I2C init.
    *         Initialize I2C peripheral
    * @param  addr:  i2c slave adress
    * @param  speed: i2c speed clock
    * @param  mutex_en: enable or disable lock I2C0 when I2C0 read or write
    * @return void
    */
void app_sensor_i2c_init(uint8_t addr, T_I2C_CLK_SPD speed, bool mutex_en);
/**
    * @brief  i2c read with auto switch slave addr support.
    * @param  slave_addr
    * @param  reg_addr
    * @param  read_buf
    * @param  read_len
    * @return void
    */
I2C_Status app_sensor_i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf,
                               uint16_t read_len);

/**
    * @brief  read 1 byte from i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  read_buf
    * @return void
    */
I2C_Status app_sensor_i2c_read_8(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf);

/**
    * @brief  read 2 bytes from i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  read_buf
    * @return void
    */
I2C_Status app_sensor_i2c_read_16(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf);

/**
    * @brief  read 4 bytes from i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  read_buf
    * @return void
    */
I2C_Status app_sensor_i2c_read_32(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf);

/**
    * @brief  read 1 bytes from 4 bytes i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  read_buf
    * @return void
    */
I2C_Status app_sensor_i2c_32_read_8(uint8_t slave_addr, uint32_t reg_addr, uint8_t *read_buf);

/**
    * @brief  i2c write with auto switch slave addr support.
    * @param  slave_addr
    * @param  write_buf
    * @param  write_len
    * @return void
    */
I2C_Status app_sensor_i2c_write(uint8_t slave_addr, uint8_t *write_buf, uint16_t write_len);

/**
    * @brief  write 1 byte to i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  reg_value
    * @return void
    */
I2C_Status app_sensor_i2c_write_8(uint8_t slave_addr, uint8_t reg_addr, uint8_t reg_value);

/**
    * @brief  write 2 bytes to i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  reg_value
    * @return void
    */
I2C_Status app_sensor_i2c_write_16(uint8_t slave_addr, uint8_t reg_addr, uint16_t reg_value);

/**
    * @brief  write 4 bytes to i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  reg_value
    * @return void
    */
I2C_Status app_sensor_i2c_write_32(uint8_t slave_addr, uint8_t reg_addr, uint32_t reg_value);

/**
    * @brief  write 1 bytes to 4 bytes i2c device reg.
    * @param  slave_addr
    * @param  reg_addr
    * @param  reg_value
    * @return void
    */
I2C_Status app_sensor_i2c_32_write_8(uint8_t slave_addr, uint32_t reg_addr, uint8_t reg_value);


#ifdef __cplusplus
}
#endif

#endif

/** @} */ /* End of group APP_SENSROR_I2C */

/******************* (C) COPYRIGHT 2021 Realtek Semiconductor *****END OF FILE****/
