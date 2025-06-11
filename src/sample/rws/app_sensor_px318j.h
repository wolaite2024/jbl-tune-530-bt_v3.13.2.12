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
#ifndef __APP_SENSOR_PX318J__
#define __APP_SENSOR_PX318J__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "platform_utils.h"

#ifndef FIXEDPT_WBITS
#define FIXEDPT_WBITS   16
#endif

#ifndef FIXEDPT_BITS
#define FIXEDPT_BITS    32
#endif

#if FIXEDPT_WBITS >= FIXEDPT_BITS
#error "FIXEDPT_WBITS must be less than or equal to FIXEDPT_BITS"
#endif

#if FIXEDPT_BITS == 16
typedef int16_t fixedpt;
typedef int32_t fixedptd;
typedef uint16_t fixedptu;
typedef uint32_t fixedptud;
#elif FIXEDPT_BITS == 32
typedef int32_t fixedpt;
typedef int64_t fixedptd;
typedef uint32_t fixedptu;
typedef uint64_t fixedptud;
#elif FIXEDPT_BITS == 64
typedef int64_t fixedpt;
typedef __int128_t fixedptd;
typedef uint64_t fixedptu;
typedef __uint128_t fixedptud;
#else
#error "FIXEDPT_BITS must be equal to 16, 32 or 64"
#endif

#define FIXEDPT_FBITS       (FIXEDPT_BITS - FIXEDPT_WBITS)
#define FIXEDPT_FMASK       (((fixedpt)1 << FIXEDPT_FBITS) - 1)
#define FIXEDPT_RCONST(R)   ((fixedpt)((R) * FIXEDPT_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define FIXEDPT_FROMINT(I)  ((fixedptd)(I) << FIXEDPT_FBITS)
#define FIXEDPT_TOINT(F)    ((F) >> FIXEDPT_FBITS)
#define FIXEDPT_ADD(A,B)    ((A) + (B))
#define FIXEDPT_SUB(A,B)    ((A) - (B))
#define FIXEDPT_XMUL(A,B)   ((fixedpt)(((fixedptd)(A) * (fixedptd)(B)) >> FIXEDPT_FBITS))
#define FIXEDPT_XDIV(A,B)   ((fixedpt)(((fixedptd)(A) << FIXEDPT_FBITS) / (fixedptd)(B)))
#define FIXEDPT_FRACPART(A) ((fixedpt)(A) & FIXEDPT_FMASK)
#define FIXEDPT_ONE         ((fixedpt)((fixedpt)1 << FIXEDPT_FBITS))
#define FIXEDPT_ONE_HALF    (FIXEDPT_ONE >> 1)

#define PX318J_ON                       0x02
#define INTERRUPT_READY                 0x02
#define ENABLE_INT_PIN                  0x03
#define DISABLE_INT_PIN                 0x01
#define NEAR_FAR_DATA                   0x80
#define PX318J_FAST_EN_REG              0x80
#define PX318J_FAST_EN_ENABLE           0x08
#define PX318J_FAST_EN_DISABLE          0x00
#define PX318J_DATA_HALT_DISABLE        0x00
#define PX318J_DATA_HALT_ENABLE         0x10
#define PX318J_OFF                      0x00
#define PX318J_CLEAR_INTTERUPT          0x00
#define PX318J_INTERRUPT_READY          0x10
#define PX318J_SOFT_REBOOT              0xEE
#define PX318J_DEV_CTRL_REG             0xF0
#define PX318J_INT_CTRL_REG             0xF1
#define PX318J_HALT_CTRL_REG            0xF2
#define PX318J_SOFT_REBOOT_REG          0xF4
#define PX318J_INTTERUPT_REG            0xFE
#define PX318J_STATE_FLAG_REG           0xFF
#define PX318J_PSCTRL_REG               0x60
#define PX318J_PSPUW_REG                0x61
#define PX318J_PSPUC_REG                0x62
#define PX318J_PSDRV_REG                0x64
#define PX318J_PSDAC_CTRL_REG           0x65
#define PX318J_PSCTDAC_REG              0x67
#define PX318J_PSCAL_REG                0x69
#define PX318J_WAITTIME_REG             0x4F
#define PX318J_OUT_EAR_REG              0x6C
#define PX318J_IN_EAR_REG               0x6E
#define OUT_EAR_THRESH                  1
#define IN_EAR_THRESH                   0
#define SENSOR_INT_TRIGGERED            0
#define SENSOR_INT_RELEASED             1
#define PX318J_ID                       (0x1C)
#define WAIT_TIME                       (app_cfg_const.px_waiting_time)   /* default value: (0x11) */
#define PS_BITS                         (app_cfg_const.px_output_precise)  /* default value: (0x01) */
#define PS_MEAN                         (app_cfg_const.px_mean)            /* default value: (0x00) */
#define PS_PUW                          (app_cfg_const.px_pulse_duration)  /* default value: (0x20) */
#define PS_PUC                          (app_cfg_const.px_pulse_time)      /* default value: (0x02) */
#define PS_CTRL                         ((PS_BITS << 4) | (PS_MEAN << 6) | (0x05))
#define PS_DRV                          (0x0B) /* recommand value by light sensor vendor */
#define PS_DRV_CTRL                     (PS_DRV)
#define PS_WAIT_ALGO                    (0x01)
#define PS_INT_ALGO                     (0x01)
#define PS_PERS                         (0x04)
#define PS_ALGO_CTRL                    ((PS_WAIT_ALGO << 5) | (PS_INT_ALGO << 4) | (PS_PERS))
#define PS_DEFAULT_THRESHOLD_IN_EAR     600
#define PS_DEFAULT_THRESHOLD_OUT_EAR    400
#define PXY_FULL_RANGE                  ((1 << (PS_BITS + 9)) - 1)
#define TARGET_PXY                      ((PXY_FULL_RANGE + 1) >> 2)

typedef struct t_px318j_cal_para
{
    uint8_t ps_dac_ctrl;
    uint8_t ps_ct_dac;
    uint8_t ps_cal[2];
    uint8_t in_ear_thresh[2];
    uint8_t out_ear_thresh[2];
    uint8_t read_ps_data[2];
    uint8_t px_pulse_duration;
    uint8_t px_pulse_time;
} T_PX318J_CAL_PARA;

typedef struct t_px318j_cal_data
{
    uint8_t interrupt_flag;
    uint8_t state_flag;
    uint8_t ps_data[2];
} T_PX318J_CAL_DATA;

typedef enum t_px318j_cal_return
{
    PX318J_CAL_SUCCESS      = 0x00,
    PX318J_CAL_PARA_FAIL    = 0x01,
    PX318J_CAL_SENSOR_ERROR = 0x02,
} T_PX318J_CAL_RETURN;

typedef enum t_near_far
{
    NEAR,
    FAR,
    NEAR_FAR_NONE,
} T_NEAR_FAR;

void app_sensor_px318j_enable(void);
void app_sensor_px318j_disable(void);
void app_sensor_px318j_init(void);
void app_sensor_px318j_enter_dlps(void);
void app_sensor_px318j_int_gpio_intr_cb(uint32_t param);
uint8_t app_sensor_px318j_auto_dac(uint8_t *spp_report);
uint8_t app_sensor_px318j_read_ps_data_after_noise_floor_cal(uint8_t *spp_report);
uint8_t app_sensor_px318j_threshold_cal(uint8_t out_ear, uint8_t *spp_report);
void app_sensor_px318j_get_para(uint8_t *spp_report);
void app_sensor_px318j_write_data(uint8_t target, uint8_t *val);

#ifdef __cplusplus
}
#endif

#endif
