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
#ifndef __APP_SENSOR_icm42607__
#define __APP_SENSOR_icm42607__

// #include "app_section.h"
// #include "app_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#define MEMS_VENDOR_INV42607             0
#define MEMS_VENDOR_QMI8658              1

#define MEMS_I2C_INV42607_SLAVE_ADDR     0x68
#define MEMS_I2C_QMI8658_SLAVE_ADDR      0x6A
// #define MEMS_I2C_QMI8658_SLAVE_ADDR_H    0x6B
#define MEM_DBUG_EN                         0
typedef enum
{
    MEM_INIT_SUCC,
    MEM_INIT_FAIL,
} T_MEM_INIT_STATUS;

typedef enum
{
    REPORT_NONE,
    REPORT_RTK,
    REPORT_CYWEE,
} T_IMU_REPORT_TARGET;

typedef enum
{
    SPATIAL_AUDIO_CLOSE,
    SPATIAL_AUDIO_OPEN,
    SPATIAL_AUDIO_NO_CHANGE,
} T_SPATIAL_AUDIO_MODE;

void app_sensor_mems_cfg_pad(void);
void app_sensor_mems_int_gpio_intr_handler(void);
void app_sensor_mems_handle_data(void *sensor_data);
// void app_sensor_mems_dat_int(void);
void app_sensor_mems_post_data(void *acc_gyro_data);
void app_sensor_mems_dlps_enter(void);
void app_sensor_mems_dlps_exit(void);
void app_sensor_mems_start(uint8_t app_idx);
void app_sensor_mems_stop(uint8_t app_idx);
void app_sensor_mems_spatial_start(uint8_t app_idx);
void app_sensor_mems_spatial_stop(uint8_t app_idx);
void app_sensor_mems_init(void);
void app_sensor_mems_spp_report_init(void);
void app_sensor_mems_spp_report_de_init(void);
void app_sensor_mems_stop_sensor(void);
bool app_sensor_mems_dual_audio_spatial_off(void);
void app_sensor_mems_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                uint8_t app_idx,
                                uint8_t *ack_pkt);
void app_sensor_mems_mode_sync(uint8_t mode);
void app_sensor_mems_atti_get(int32_t *dat);
void app_sensor_mems_atti_flag_set(void);
void app_sensor_mems_atti_save(int32_t *dat);
#ifdef __cplusplus
}
#endif
#endif


