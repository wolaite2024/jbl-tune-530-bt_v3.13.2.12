/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief    Peripheral definitions for the project
* @details
* @author   Chuanguo Xue
* @date     2015-4-7
* @version  v1.0
* *********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_NULL                0
#define KEY0_MASK               0x01
#define KEY1_MASK               0x02
#define KEY2_MASK               0x04
#define KEY3_MASK               0x08
#define KEY4_MASK               0x10
#define KEY5_MASK               0x20
#define KEY6_MASK               0x40
#define KEY7_MASK               0x80

#define KEY_RELEASE             0
#define KEY_PRESS               1

#define MAX_KEY_NUM             8
#define HYBRID_KEY_NUM          8
#define RWS_KEY_NUM             8

#define LED_NUM                 3 //MAX: 3 LED
#define LED_DEMO_NUM            6 //MAX: 6 LED
#define SLEEP_LED_PINMUX_NUM    10

#define DSP_TIMER               TIM7 //DSP HW fixed at timer7

#define UART_RX_BUFFER_SIZE         1024
#define RX_GDMA_START_ADDR          UART_RX_BUFFER_SIZE
#define RX_GDMA_BUFFER_SIZE         300


/** @defgroup IO Driver Config
  * @note user must config it firstly!! Do not change macro names!!
  * @{
  */

/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB      1
#define USE_USER_DEFINE_DLPS_ENTER_CB     1

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS               0
#define USE_I2C1_DLPS               0
#define USE_I2C2_DLPS               0
#define USE_TIM_DLPS                1
#define USE_QDECODER_DLPS           0
#define USE_IR_DLPS                 0
#define USE_RTC_DLPS                0
#define USE_UART0_DLPS              0
#define USE_UART1_DLPS              1
#define USE_UART2_DLPS              0
#define USE_UART3_DLPS              0
#define USE_ADC_DLPS                0
#define USE_SPI0_DLPS               0
#define USE_SPI1_DLPS               0
#define USE_SPI2W_DLPS              0
#define USE_KEYSCAN_DLPS            0
#define USE_DMIC_DLPS               0
#define USE_GPIOA_DLPS              0
#define USE_GPIOB_DLPS              0
#define USE_PWM0_DLPS               0
#define USE_PWM1_DLPS               0
#define USE_PWM2_DLPS               0
#define USE_PWM3_DLPS               0

#define USE_GDMACHANNEL0_DLPS       0
#define USE_GDMACHANNEL1_DLPS       1
#define USE_GDMACHANNEL2_DLPS       1
#define USE_GDMACHANNEL3_DLPS       0
#define USE_GDMACHANNEL4_DLPS       0
#define USE_GDMACHANNEL5_DLPS       0
#define USE_GDMACHANNEL6_DLPS       0
#define USE_GDMACHANNEL7_DLPS       0

#define USE_GDMA_DLPS               (USE_GDMACHANNEL0_DLPS | USE_GDMACHANNEL1_DLPS | USE_GDMACHANNEL2_DLPS\
                                     | USE_GDMACHANNEL3_DLPS | USE_GDMACHANNEL4_DLPS | USE_GDMACHANNEL5_DLPS\
                                     | USE_GDMACHANNEL6_DLPS | USE_GDMACHANNEL7_DLPS)

/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS   (USE_I2C0_DLPS | USE_I2C1_DLPS | USE_I2C2_DLPS | USE_TIM_DLPS | USE_QDECODER_DLPS\
                              | USE_IR_DLPS | USE_RTC_DLPS | USE_UART_DLPS | USE_UART0_DLPS |USE_UART1_DLPS |USE_UART2_DLPS |USE_UART3_DLPS |USE_SPI0_DLPS\
                              | USE_SPI1_DLPS | USE_SPI2W_DLPS | USE_KEYSCAN_DLPS | USE_DMIC_DLPS\
                              | USE_GPIOB_DLPS | USE_GPIOA_DLPS | USE_USER_DEFINE_DLPS_EXIT_CB | USE_GDMA_DLPS\
                              | USE_RTC_DLPS | USE_PWM0_DLPS | USE_PWM1_DLPS | USE_PWM2_DLPS\
                              | USE_PWM3_DLPS | USE_USER_DEFINE_DLPS_ENTER_CB)

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H_ */

