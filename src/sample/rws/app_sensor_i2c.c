/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_sensor_i2c.c
* @brief    This file provides all the i2c hal functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include <string.h>
#include "section.h"
#include "trace.h"
#include "platform_utils.h"
#include "rtl876x_i2c.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "hal_gpio.h"
#include "app_sensor_i2c.h"
#include "app_cfg.h"

static void *p_i2c_mutex_handle = NULL;

static void app_sensor_i2c_stop_signal(uint8_t scl_pin, uint8_t sda_pin)
{

    /*gpio init*/
    hal_gpio_init_pin(scl_pin, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_init_pin(sda_pin, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);

    APP_PRINT_INFO2("scl_pin level =%d, sda_pin level =%d ", hal_gpio_get_input_level(scl_pin),
                    hal_gpio_get_input_level(sda_pin));

    hal_gpio_init_pin(scl_pin, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_UP);
    hal_gpio_init_pin(sda_pin, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_UP);

    /*generate i2c stop signal*/
    hal_gpio_set_level(scl_pin, GPIO_LEVEL_HIGH);
    hal_gpio_set_level(sda_pin, GPIO_LEVEL_LOW);
    platform_delay_us(20);
    hal_gpio_set_level(sda_pin, GPIO_LEVEL_HIGH);

    Pinmux_Config(scl_pin, I2C0_CLK);
    Pinmux_Config(sda_pin, I2C0_DAT);
}

void app_sensor_i2c_init(uint8_t addr, T_I2C_CLK_SPD speed, bool mutex_en)
{
    I2C_InitTypeDef i2c_param;

    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);

    I2C_StructInit(&i2c_param);

    i2c_param.I2C_ClockSpeed = speed;
    i2c_param.I2C_DeviveMode = I2C_DeviveMode_Master;
    i2c_param.I2C_AddressMode = I2C_AddressMode_7BIT;
    i2c_param.I2C_SlaveAddress = addr;
    i2c_param.I2C_Ack = I2C_Ack_Enable;

    I2C_Init(I2C0, &i2c_param);
    I2C_Cmd(I2C0, ENABLE);

    // Create a mutex. only mems sensor interrupt handler not used in app task
    if (mutex_en)
    {
        if (p_i2c_mutex_handle == NULL)
        {
            os_mutex_create(&p_i2c_mutex_handle);
        }
    }
}


ISR_TEXT_SECTION I2C_Status app_sensor_i2c_read(uint8_t slave_addr, uint8_t reg_addr,
                                                uint8_t *p_read_buf, uint16_t read_len)
{
    I2C_Status status;
    bool i2c_mutex_flag = false;

    i2c_mutex_flag = I2C_LOCK(p_i2c_mutex_handle);

    if (slave_addr != I2C0->IC_TAR)
    {
        I2C_Cmd(I2C0, DISABLE);
        I2C_SetSlaveAddress(I2C0, slave_addr);
        I2C_Cmd(I2C0, ENABLE);
    }

    status = I2C_RepeatRead(I2C0, &reg_addr, 1, p_read_buf, read_len);

    if (status != I2C_Success)
    {
        app_sensor_i2c_stop_signal(app_cfg_const.i2c_0_clk_pinmux, app_cfg_const.i2c_0_dat_pinmux);
        I2C_Cmd(I2C0, DISABLE);
        APP_PRINT_ERROR3("I2C_RepeatRead: slave_addr 0x%x reg_addr 0x%x err status %d", slave_addr,
                         reg_addr, status);
        I2C_Cmd(I2C0, ENABLE);
    }

    I2C_UNLOCK(p_i2c_mutex_handle, i2c_mutex_flag);
    return status;
}

ISR_TEXT_SECTION I2C_Status app_sensor_i2c_read_8(uint8_t slave_addr, uint8_t reg_addr,
                                                  uint8_t *read_buf)
{
    return app_sensor_i2c_read(slave_addr, reg_addr, read_buf, 1);
}

I2C_Status app_sensor_i2c_read_16(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf)
{
    return app_sensor_i2c_read(slave_addr, reg_addr, read_buf, 2);
}

I2C_Status app_sensor_i2c_read_32(uint8_t slave_addr, uint8_t reg_addr, uint8_t *read_buf)
{
    return app_sensor_i2c_read(slave_addr, reg_addr, read_buf, 4);
}

I2C_Status app_sensor_i2c_32_read_8(uint8_t slave_addr, uint32_t reg_addr, uint8_t *read_buf)
{
    uint8_t write_buf[4] = {0, 0, 0, 0};
    I2C_Status status;

    write_buf[0] = (reg_addr & 0xFF000000) >> 24;
    write_buf[1] = (reg_addr & 0xFF0000) >> 16;
    write_buf[2] = (reg_addr & 0xFF00) >> 8;
    write_buf[3] = reg_addr & 0x00FF;

    bool i2c_mutex_flag = false;

    i2c_mutex_flag = I2C_LOCK(p_i2c_mutex_handle);

    if (slave_addr != I2C0->IC_TAR)
    {
        I2C_Cmd(I2C0, DISABLE);
        I2C_SetSlaveAddress(I2C0, slave_addr);
        I2C_Cmd(I2C0, ENABLE);
    }

    status = I2C_RepeatRead(I2C0, write_buf, 4, read_buf, 1);

    if (status != I2C_Success)
    {
        app_sensor_i2c_stop_signal(app_cfg_const.i2c_0_clk_pinmux, app_cfg_const.i2c_0_dat_pinmux);
        I2C_Cmd(I2C0, DISABLE);
        APP_PRINT_ERROR3("I2C_RepeatRead: slave_addr 0x%x reg_addr 0x%x err status %d", slave_addr,
                         reg_addr, status);
        I2C_Cmd(I2C0, ENABLE);
    }

    I2C_UNLOCK(p_i2c_mutex_handle, i2c_mutex_flag);
    return status;
}

ISR_TEXT_SECTION I2C_Status app_sensor_i2c_write(uint8_t slave_addr, uint8_t *write_buf,
                                                 uint16_t write_len)
{
    I2C_Status status;
    bool i2c_mutex_flag = false;

    i2c_mutex_flag = I2C_LOCK(p_i2c_mutex_handle);

    if (slave_addr != I2C0->IC_TAR)
    {
        I2C_Cmd(I2C0, DISABLE);
        I2C_SetSlaveAddress(I2C0, slave_addr);
        I2C_Cmd(I2C0, ENABLE);
    }

    status = I2C_MasterWrite(I2C0, write_buf, write_len);

    if (status != I2C_Success)
    {
        app_sensor_i2c_stop_signal(app_cfg_const.i2c_0_clk_pinmux, app_cfg_const.i2c_0_dat_pinmux);
        I2C_Cmd(I2C0, DISABLE);
        APP_PRINT_ERROR2("I2C_MasterWrite: slave_addr 0x%x err status %d", slave_addr, status);
        I2C_Cmd(I2C0, ENABLE);
    }

    I2C_UNLOCK(p_i2c_mutex_handle, i2c_mutex_flag);
    return status ;
}

ISR_TEXT_SECTION I2C_Status app_sensor_i2c_write_8(uint8_t slave_addr, uint8_t reg_addr,
                                                   uint8_t reg_value)
{
    uint8_t write_buf[2];
    I2C_Status status;

    write_buf[0] = reg_addr;
    write_buf[1] = reg_value;

    status = app_sensor_i2c_write(slave_addr, write_buf, 2);

    return status;
}

I2C_Status app_sensor_i2c_write_16(uint8_t slave_addr, uint8_t reg_addr, uint16_t reg_value)
{
    uint8_t write_buf[3];
    I2C_Status status;

    write_buf[0] = reg_addr;
    write_buf[1] = reg_value & 0x00FF;
    write_buf[2] = reg_value >> 8;

    status = app_sensor_i2c_write(slave_addr, write_buf, 3);

    return status;
}

I2C_Status app_sensor_i2c_write_32(uint8_t slave_addr, uint8_t reg_addr, uint32_t reg_value)
{
    uint8_t write_buf[5];
    I2C_Status status;

    write_buf[0] = reg_addr;
    write_buf[1] = reg_value & 0x00FF;
    write_buf[2] = (reg_value & 0xFF00) >> 8;
    write_buf[3] = (reg_value & 0xFF0000) >> 16;
    write_buf[4] = (reg_value & 0xFF000000) >> 24;

    status = app_sensor_i2c_write(slave_addr, write_buf, 5);

    return status;
}

I2C_Status app_sensor_i2c_32_write_8(uint8_t slave_addr, uint32_t reg_addr, uint8_t reg_value)
{
    uint8_t write_buf[5] = {0, 0, 0, 0, 0};
    I2C_Status status;

    write_buf[0] = (reg_addr & 0xFF000000) >> 24;
    write_buf[1] = (reg_addr & 0xFF0000) >> 16;
    write_buf[2] = (reg_addr & 0xFF00) >> 8;
    write_buf[3] = reg_addr & 0x00FF;
    write_buf[4] = reg_value;

    status = app_sensor_i2c_write(slave_addr, write_buf, 5);

    return status;
}
