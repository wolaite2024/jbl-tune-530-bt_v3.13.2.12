/*
* This file contains all the necessary settings for the iqs773 and this file can
* be changed from the GUI or edited here
* File:   iqs773_init.h
* Author: Azoteq
*/

#ifndef _IQS773_INIT_H_
#define _IQS773_INIT_H_

/* Change the Prox Sensor Settings 0 */
/* Memory Map Position 0x40 - 0x4B */
#define PXS_SETTINGS0_0                     0xB3
#define PXS_SETTINGS0_1                     0xB3
#define PXS_SETTINGS0_2                     0xB3
#define PXS_SETTINGS0_3                     0x2F
#define PXS_SETTINGS0_4                     0x2F
#define PXS_SETTINGS0_5                     0x2F
#define PXS_SETTINGS0_6                     0x4F
#define PXS_SETTINGS0_7                     0x4F
#define PXS_SETTINGS0_8                     0x4F
#define PXS_SETTINGS0_9                     0x06
#define PXS_SETTINGS0_10                    0x06
#define PXS_SETTINGS0_11                    0x06

/* Change the Prox Sensor Settings 1 */
/* Memory Map Position 0x50 - 0x57 */
#define PXS_SETTINGS1_0                     0x00
#define PXS_SETTINGS1_1                     0x01
#define PXS_SETTINGS1_2                     0x00
#define PXS_SETTINGS1_3                     0x00
#define PXS_SETTINGS1_4                     0x00
#define PXS_SETTINGS1_5                     0x00
#define PXS_SETTINGS1_6                     0x00
#define PXS_SETTINGS1_7                     0x00

/* Change the Prox UI Settings */
/* Memory Map Position 0x60 - 0x66 */
// #define PXS_UI_SETTINGS_0                   0x08
// #define PXS_UI_SETTINGS_0                   0x09
// #define PXS_UI_SETTINGS_0                   0x0A
// #define PXS_UI_SETTINGS_0                   0x0B
#define PXS_UI_SETTINGS_0                   0x0C

#define PXS_UI_SETTINGS_1                   0x0D
#define PXS_UI_SETTINGS_2                   0x08
#define PXS_UI_SETTINGS_3                   0x0D
#define PXS_UI_SETTINGS_4                   0x08
#define PXS_UI_SETTINGS_5                   0x0D
#define PXS_UI_SETTINGS_6                   0x28

/* Change the SAR UI Settings */
/* Memory Map Position 0x70 - 0x75 */
#define SAR_UI_SETTINGS_0                   0x08
#define SAR_UI_SETTINGS_1                   0x0D
#define SAR_UI_SETTINGS_2                   0x05

/*same as PXS_UI_SETTINGS_0*/
// #define SAR_UI_SETTINGS_3                   0x08
// #define SAR_UI_SETTINGS_3                   0x09
// #define SAR_UI_SETTINGS_3                   0x0A
// #define SAR_UI_SETTINGS_3                   0x0B
#define SAR_UI_SETTINGS_3                   0x0C
#define SAR_UI_SETTINGS_4                   0x0D
#define SAR_UI_SETTINGS_5                   0x28

/* Change the Hysteresis UI Settings */
/* Memory Map Position 0x80 - 0x83 */
#define HYSTERESIS_UI_SETTINGS_0            0xA2
#define HYSTERESIS_UI_SETTINGS_1            0x0A
#define HYSTERESIS_UI_SETTINGS_2            0x08
#define HYSTERESIS_UI_SETTINGS_3            0x0D

/* Change the HALL Sensor Settings */
/* Memory Map Position 0x90 - 0x93 */
#define HALL_SENSOR_SETTINGS_0              0x03
#define HALL_SENSOR_SETTINGS_1              0xE3
#define HALL_SENSOR_SETTINGS_2              0x00
#define HALL_SENSOR_SETTINGS_3              0x00

/* Change the HALL Switch UI Settings */
/* Memory Map Position 0xA0 - 0xA2 */
#define HALL_UI_SETTINGS_0                  0x00
#define HALL_UI_SETTINGS_1                  0x19
#define HALL_UI_SETTINGS_2                  0x19

/* Change the Temperature UI Settings */
/* Memory Map Position 0xC0 - 0xC4 */
#define TEMP_UI_SETTINGS_0                  0x00
#define TEMP_UI_SETTINGS_1                  0x04
#define TEMP_UI_SETTINGS_2                  0x02
#define TEMP_UI_SETTINGS_3                  0x0D
#define TEMP_UI_SETTINGS_4                  0x27

/* Change the Device & PMU Settings */
/* Memory Map Position 0xD0 - 0xDB */
#define SYSTEM_SETTINGS                     0x00
#define ACTIVE_CHS                          0x01
#define PMU_SETTINGS                        0x02
#define REPORT_RATES_TIMINGS_0              0x10
#define REPORT_RATES_TIMINGS_1              0x30
#define REPORT_RATES_TIMINGS_2              0x10
#define REPORT_RATES_TIMINGS_3              0x04
#define GLOBAL_EVENT_MASK                   0xFA
#define PWM_DUTY_CYCLE                      0x00
#define RDY_TIMEOUT_PERIOD                  0xFF
#define I2C_SETTINGS                        0x01
#define CH_RESEED_ENABLE                    0x01

#endif  /* _IQS773_INIT_H_ */
