/*
* This file contains all the necessary settings for the iqs873 and this file can
* be changed from the GUI or edited here
* File:   iqs873_init.h
* Author: Azoteq
*/

#ifndef _IQS873_H_
#define _IQS873_H_

/* Change the Channel select, Power mode & System Settings */
/* Memory Map Position 0x80 - 0x82 */
#define PMU_GENERAL_SETTINGS                     0x42
#define I2C_GENERAL_SETTINGS                     0x00
#define SYS_CHB_ACTIVE                           0xE3
#define ACF_LTA_FILTER_SETTINGS                  0x10
#define LTA_CHB_RESEED_ENABLED                   0x23
#define UIS_GLOBAL_EVENTS_MASK                   0xE5

/* Change the Report Rates and Timing */
/* Memory Map Position 0x83 - 0x85 */
#define PMU_REPORT_RATE_NP                       0x20
#define PMU_REPORT_RATE_LP                       0x40
#define PMU_REPORT_RATE_ULP                      0x08
#define PMU_MODE_TIMOUT                          0x04
#define I2C_WINDOW_TIMEOUT                       0x14
#define LTA_HALT_TIMEOUT                         0x14

/* Change the Global Settings */
/* Memory Map Position 0x86 - 0x8B */
#define PXS_GENERAL_SETTINGS0                    0x00
#define PXS_GENERAL_SETTINGS1                    0x10
#define UIS_ABSOLUTE_CAPACITANCE                 0x00
#define UIS_DCF_GENERAL_SETTINGS                 0x01
#define GEM_CHB_BLOCK_NFOLLOW                    0x00
#define MOV_CHB_MOVEMENT_CHANNEL                 0x00
#define UIS_CHB_SLIDER0                          0x00
#define UIS_CHB_SLIDER1                          0x00
#define UIS_GESTURE_TAP_TIMEOUT                  0x19
#define UIS_GESTURE_SWIPE_TIMEOUT                0x7D
#define UIS_GESTURE_THRESHOLD                    0x80
#define LTA_CHB_RESEED                           0x00

/* Change the CH0 Settings */
/* Memory Map Position 0x8C - 0x92 */
#define PXS_CRXSEL_CH0                           0x05
#define PXS_CTXSEL_CH0                           0xFF
#define PXS_PROXCTRL_CH0                         0xA3
#define PXS_PROXCFG0_CH0                         0x40
#define PXS_PROXCFG1_TESTREG0_CH0                0x04
#define ATI_BASE_AND_TARGET_CH0                  0x50
#define ATI_MIRROR_CH0                           0x00
#define ATI_PCC_CH0                              0x00
#define PXS_PROX_THRESHOLD_CH0                   0x0A
#define PXS_TOUCH_THRESHOLD_CH0                  0x0E
#define PXS_DEEP_THRESHOLD_CH0                   0xFF
#define PXS_HYSTERESIS_CH0                       0x04
#define DCF_CHB_ASSOCIATION_CH0                  0x00
#define DCF_WEIGHT_CH0                           0x00

/* Change the CH1 Settings */
/* Memory Map Position 0x93 - 0x99 */
#define PXS_CRXSEL_CH1                           0x10
#define PXS_CTXSEL_CH1                           0x40
#define PXS_PROXCTRL_CH1                         0xEF
#define PXS_PROXCFG0_CH1                         0xE9
#define PXS_PROXCFG1_TESTREG0_CH1                0xCD
#define ATI_BASE_AND_TARGET_CH1                  0x58
#define ATI_MIRROR_CH1                           0x00
#define ATI_PCC_CH1                              0x00
#define PXS_PROX_THRESHOLD_CH1                   0x08
#define PXS_TOUCH_THRESHOLD_CH1                  0x08
#define PXS_DEEP_THRESHOLD_CH1                   0xFF
#define PXS_HYSTERESIS_CH1                       0x0C
#define DCF_CHB_ASSOCIATION_CH1                  0x00
#define DCF_WEIGHT_CH1                           0x00

/* Change the CH2 Settings */
/* Memory Map Position 0x9A - 0xA0 */
#define PXS_CRXSEL_CH2                           0x01
#define PXS_CTXSEL_CH2                           0xFF
#define PXS_PROXCTRL_CH2                         0xA3
#define PXS_PROXCFG0_CH2                         0x40
#define PXS_PROXCFG1_TESTREG0_CH2                0x02
#define ATI_BASE_AND_TARGET_CH2                  0x50
#define ATI_MIRROR_CH2                           0x00
#define ATI_PCC_CH2                              0x00
#define PXS_PROX_THRESHOLD_CH2                   0x10
#define PXS_TOUCH_THRESHOLD_CH2                  0x24
#define PXS_DEEP_THRESHOLD_CH2                   0xFF
#define PXS_HYSTERESIS_CH2                       0x04
#define DCF_CHB_ASSOCIATION_CH2                  0x00
#define DCF_WEIGHT_CH2                           0x00

/* Change the CH3 Settings */
/* Memory Map Position 0xA1 - 0xA7 */
#define PXS_CRXSEL_CH3                           0x04
#define PXS_CTXSEL_CH3                           0xFF
#define PXS_PROXCTRL_CH3                         0xA3
#define PXS_PROXCFG0_CH3                         0x40
#define PXS_PROXCFG1_TESTREG0_CH3                0x02
#define ATI_BASE_AND_TARGET_CH3                  0x50
#define ATI_MIRROR_CH3                           0x00
#define ATI_PCC_CH3                              0x00
#define PXS_PROX_THRESHOLD_CH3                   0x18
#define PXS_TOUCH_THRESHOLD_CH3                  0x32
#define PXS_DEEP_THRESHOLD_CH3                   0xFF
#define PXS_HYSTERESIS_CH3                       0x04
#define DCF_CHB_ASSOCIATION_CH3                  0x00
#define DCF_WEIGHT_CH3                           0x00

/* Change the CH4 Settings */
/* Memory Map Position 0xA8 - 0xAE */
#define PXS_CRXSEL_CH4                           0x04
#define PXS_CTXSEL_CH4                           0xFF
#define PXS_PROXCTRL_CH4                         0xA3
#define PXS_PROXCFG0_CH4                         0x40
#define PXS_PROXCFG1_TESTREG0_CH4                0x02
#define ATI_BASE_AND_TARGET_CH4                  0x50
#define ATI_MIRROR_CH4                           0x00
#define ATI_PCC_CH4                              0x00
#define PXS_PROX_THRESHOLD_CH4                   0x18
#define PXS_TOUCH_THRESHOLD_CH4                  0x32
#define PXS_DEEP_THRESHOLD_CH4                   0xFF
#define PXS_HYSTERESIS_CH4                       0x04
#define DCF_CHB_ASSOCIATION_CH4                  0x00
#define DCF_WEIGHT_CH4                           0x00

/* Change the CH5 Settings */
/* Memory Map Position 0xAF - 0xB5 */
#define PXS_CRXSEL_CH5                           0x01
#define PXS_CTXSEL_CH5                           0xFF
#define PXS_PROXCTRL_CH5                         0xA3
#define PXS_PROXCFG0_CH5                         0x40
#define PXS_PROXCFG1_TESTREG0_CH5                0x02
#define ATI_BASE_AND_TARGET_CH5                  0x50
#define ATI_MIRROR_CH5                           0x00
#define ATI_PCC_CH5                              0x00
#define PXS_PROX_THRESHOLD_CH5                   0x10
#define PXS_TOUCH_THRESHOLD_CH5                  0x18
#define PXS_DEEP_THRESHOLD_CH5                   0xFF
#define PXS_HYSTERESIS_CH5                       0x0D
#define DCF_CHB_ASSOCIATION_CH5                  0x00
#define DCF_WEIGHT_CH5                           0x00

/* Change the CH6 Settings */
/* Memory Map Position 0xB6 - 0xBC */
#define PXS_CRXSEL_CH6                           0x02
#define PXS_CTXSEL_CH6                           0xFF
#define PXS_PROXCTRL_CH6                         0xA0
#define PXS_PROXCFG0_CH6                         0xCE
#define PXS_PROXCFG1_TESTREG0_CH6                0x00
#define ATI_BASE_AND_TARGET_CH6                  0xD0
#define ATI_MIRROR_CH6                           0x00
#define ATI_PCC_CH6                              0x00
#define PXS_PROX_THRESHOLD_CH6                   0x10
#define PXS_TOUCH_THRESHOLD_CH6                  0x28
#define PXS_DEEP_THRESHOLD_CH6                   0xFF
#define PXS_HYSTERESIS_CH6                       0x4C
#define DCF_CHB_ASSOCIATION_CH6                  0x00
#define DCF_WEIGHT_CH6                           0x00

/* Change the CH7 Settings */
/* Memory Map Position 0xBD - 0xC3 */
#define PXS_CRXSEL_CH7                           0x42
#define PXS_CTXSEL_CH7                           0xFF
#define PXS_PROXCTRL_CH7                         0xA3
#define PXS_PROXCFG0_CH7                         0xCE
#define PXS_PROXCFG1_TESTREG0_CH7                0x00
#define ATI_BASE_AND_TARGET_CH7                  0xD0
#define ATI_MIRROR_CH7                           0x00
#define ATI_PCC_CH7                              0x00
#define PXS_PROX_THRESHOLD_CH7                   0x10
#define PXS_TOUCH_THRESHOLD_CH7                  0x28
#define PXS_DEEP_THRESHOLD_CH7                   0xFF
#define PXS_HYSTERESIS_CH7                       0x4C
#define DCF_CHB_ASSOCIATION_CH7                  0x00
#define DCF_WEIGHT_CH7                           0x00

/* Change the I2C Control Settings */
/* Memory Map Position 0xF2 - 0xF2 */
#define I2C_CONTROL_SETTINGS                     0x01

/* Change the Hall UI */
/* Memory Map Position 0xF5 - 0xF5 */
#define HALL_UI_ENABLE                           0x8C

#endif  /* _IQS873_INIT_H_ */
