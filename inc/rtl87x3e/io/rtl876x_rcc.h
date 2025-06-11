/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rcc.h
* @brief     header file of reset and clock control driver.
* @details
* @author    tifnan_ge
* @date      2015-05-16
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876X_RCC_H_
#define _RTL876X_RCC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"



/** @addtogroup 87x3e_RCC RCC
  * @brief RCC driver module
  * @{
  */



/** @defgroup 87x3e_RCC_Exported_Constants RCC Exported Constants
  * @{
  */


/** @defgroup 87x3e_RCC_Peripheral_Clock  RCC Peripheral Clock
  * @{
  */
/*start  offset address  28| --> bit28, (0x01 << 29) --> adress 0x230,(0x02 << 29) -->0x234, 0x00 << 10) -->0x00 sleep clock cfg?  yes*/
#define APBPeriph_CAP_CLOCK               (0)

#define APBPeriph_I2S0_CLOCK              ((uint32_t)((1 << 5) | (1 << 8) | (0x00 << 29)))
#define APBPeriph_I2S1_CLOCK              ((uint32_t)((1 << 6) | (1 << 8) | (0x00 << 29)))
#define APBPeriph_CODEC_CLOCK             ((uint32_t)((1 << 4) | (0x00 << 29)))
#define APBPeriph_USB_CLOCK                 ((uint32_t)( 28 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_SD_HOST_CLOCK             ((uint32_t)( 26 | (0x01 << 29) | 0x00 << 10))

#define APBPeriph_GPIOA_CLOCK               ((uint32_t)( 24 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_GPIO_CLOCK                APBPeriph_GPIOA_CLOCK
#define APBPeriph_GPIOB_CLOCK               ((uint32_t)( 22 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_FLASH2_CLOCK              ((uint32_t)( 20 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_FLASH1_CLOCK              ((uint32_t)( 18 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_GDMA_CLOCK                ((uint32_t)( 16 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_TIMER_CLOCK               ((uint32_t)( 14 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_TIMERA_CLOCK      APBPeriph_TIMER_CLOCK
#define APBPeriph_UART1_CLOCK               ((uint32_t)( 12 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_UART2_CLOCK               ((uint32_t)( 10 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_FLASH_CLOCK               ((uint32_t)(  8 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_VENDOR_REG_CLOCK          ((uint32_t)(  6 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_CKE_BTV_CLOCK             ((uint32_t)(  5 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_BUS_RAM_SLP_CLOCK         ((uint32_t)(  4 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_CKE_CTRLAP_CLOCK          ((uint32_t)(  3 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_CKE_PLFM_CLOCK            ((uint32_t)(  2 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_CKE_CORDIC_CLOCK          ((uint32_t)(  1 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_HWSPI_CLOCK               ((uint32_t)(  0 | (0x01 << 29) | 0x01 << 10))

#define APBPeriph_SPI2_CLOCK               ((uint32_t)( 22 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_IR_CLOCK                 ((uint32_t)( 20 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SPI1_CLOCK               ((uint32_t)( 18 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SPI0_CLOCK               ((uint32_t)( 16 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_CKE_CAP_CLOCK            ((uint32_t)( 11 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CKE_AAC_XTAL_CLOCK       ((uint32_t)( 10 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CKE_PDCK_CLOCK           ((uint32_t)(  9 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_RNG_CLOCK                ((uint32_t)(  8 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_SWR_SS_CLOCK             ((uint32_t)(  6 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CAL_32K_CLOCK            ((uint32_t)(  5 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CKE_MODEM_CLOCK          ((uint32_t)(  4 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_UART1_HCI_CLOCK          ((uint32_t)(  2 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_UART0_CLOCK              ((uint32_t)(  0 | (0x02 << 29) | 0x00 << 10))

#define APBPeriph_EFUSE_CLOCK             ((uint32_t)( 31 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_CKE_DSP_WDT_CLOCK       ((uint32_t)( 30 | (0x03 << 29) | 0x01 << 10))

#define APBPeriph_CKE_DSP_CLOCK           ((uint32_t)( 28 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_CKE_H2D_D2H             ((uint32_t)( 26 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_ADC_CLOCK               ((uint32_t)( 24 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_DSP_MEM_CLOCK           ((uint32_t)( 22 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_ASRC_CLOCK              ((uint32_t)( 20 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_LCD_CLOCK               ((uint32_t)( 18 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_IF8080_CLOCK            ((uint32_t)( 18 | (0x03 << 29) | (0x00 << 10)))
#define APBPeriph_SPI2W_CLOCK             ((uint32_t)( 16 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_SYS_RAM_CLOCK           ((uint32_t)( 14 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_I2C2_CLOCK              ((uint32_t)( 12 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_SIMC_CLOCK              ((uint32_t)( 10 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_AES_CLOCK               ((uint32_t)(  8 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_KEYSCAN_CLOCK           ((uint32_t)(  6 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_QDEC_CLOCK              ((uint32_t)(  4 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_I2C0_CLOCK              ((uint32_t)(  0 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_I2C1_CLOCK              ((uint32_t)(  2 | (0x03 << 29) | 0x00 << 10))

#define IS_APB_PERIPH_CLOCK(CLOCK) (((CLOCK) == APBPeriph_USB_CLOCK) || ((CLOCK) == APBPeriph_SD_HOST_CLOCK)\
                                    || ((CLOCK) == APBPeriph_GPIOA_CLOCK)|| ((CLOCK) == APBPeriph_GPIO_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPIOB_CLOCK)|| ((CLOCK) == APBPeriph_FLASH2_CLOCK)\
                                    || ((CLOCK) == APBPeriph_FLASH1_CLOCK) || ((CLOCK) == APBPeriph_GDMA_CLOCK)\
                                    || ((CLOCK) == APBPeriph_TIMER_CLOCK) || ((CLOCK) == APBPeriph_UART1_CLOCK)\
                                    || ((CLOCK) == APBPeriph_UART2_CLOCK) || ((CLOCK) == APBPeriph_FLASH_CLOCK)\
                                    || ((CLOCK) == APBPeriph_VENDOR_REG_CLOCK) || ((CLOCK) == APBPeriph_CKE_BTV_CLOCK)\
                                    || ((CLOCK) == APBPeriph_BUS_RAM_SLP_CLOCK) || ((CLOCK) == APBPeriph_CKE_CTRLAP_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_PLFM_CLOCK)|| ((CLOCK) == APBPeriph_CKE_CORDIC_CLOCK)\
                                    || ((CLOCK) == APBPeriph_HWSPI_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SPI2_CLOCK)|| ((CLOCK) == APBPeriph_IR_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SPI1_CLOCK)||(CLOCK) == (APBPeriph_SPI0_CLOCK)\
                                    || ((CLOCK) ==APBPeriph_CKE_CAP_CLOCK ) || (CLOCK) == (APBPeriph_CKE_AAC_XTAL_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_PDCK_CLOCK)||((CLOCK) == APBPeriph_RNG_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SWR_SS_CLOCK)|| ((CLOCK) == APBPeriph_CAL_32K_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_MODEM_CLOCK) || ((CLOCK) == APBPeriph_UART1_HCI_CLOCK)\
                                    || ((CLOCK) == APBPeriph_UART0_CLOCK)||((CLOCK) == APBPeriph_EFUSE_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_DSP_WDT_CLOCK) || ((CLOCK) == APBPeriph_CKE_DSP_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_H2D_D2H)) || ((CLOCK) == APBPeriph_ADC_CLOCK)\
|| ((CLOCK) == APBPeriph_DSP_MEM_CLOCK) || ((CLOCK) == APBPeriph_ASRC_CLOCK)\
|| ((CLOCK) == APBPeriph_LCD_CLOCK) || ((CLOCK) == APBPeriph_IF8080_CLOCK)\
|| ((CLOCK) == APBPeriph_SPI2W_CLOCK)\
|| ((CLOCK) == APBPeriph_SYS_RAM_CLOCK) || ((CLOCK) == APBPeriph_I2C2_CLOCK)\
|| ((CLOCK) == APBPeriph_SIMC_CLOCK)||(CLOCK) == (APBPeriph_AES_CLOCK)\
|| ((CLOCK) == APBPeriph_KEYSCAN_CLOCK)||((CLOCK) ==APBPeriph_QDEC_CLOCK )\
|| ((CLOCK) == APBPeriph_I2C1_CLOCK) || ((CLOCK) ==APBPeriph_I2C0_CLOCK )\
|| ((CLOCK) == APBPeriph_CAP_CLOCK )\
)

/** End of group 87x3e_RCC_Peripheral_Clock
  * @}
  */

/** @defgroup 87x3e_APB_Peripheral_Define APB Peripheral Define
  * @{
  */
#define APBPeriph_CAP                   ((uint32_t)( 21 | (0x00 << 26)))
#define APBPeriph_AAC_XTAL              ((uint32_t)( 20 | (0x00 << 26)))
#define APBPeriph_SWR_SS                ((uint32_t)( 18 | (0x00 << 26)))
#define APBPeriph_TIMER                 ((uint32_t)( 16 | (0x00 << 26)))
#define APBPeriph_TIMERA        APBPeriph_TIMER
#define APBPeriph_USB                   ((uint32_t)( 15 | (0x00 << 26)))
#define APBPeriph_SD_HOST               ((uint32_t)( 14 | (0x00 << 26)))
#define APBPeriph_GDMA                  ((uint32_t)( 13 | (0x00 << 26)))
#define APBPeriph_UART1                 ((uint32_t)( 12 | (0x00 << 26)))
#define APBPeriph_FLASH                 ((uint32_t)( 4  | (0x00 << 26)))
#define APBPeriph_FLASH3                ((uint32_t)( 7  | (0x00 << 26)))
#define APBPeriph_FLASH2                ((uint32_t)( 6  | (0x00 << 26)))
#define APBPeriph_LCD                   ((uint32_t)( 5  | (0x02 << 26)))
#define APBPeriph_I2C0                  ((uint32_t)( 16 | (0x02 << 26)))
#define APBPeriph_FLASH1                ((uint32_t)( 5  | (0x00 << 26)))
#define APBPeriph_BTBUS                 ((uint32_t)( 2  | (0x00 << 26)))


#define APBPeriph_SHA                   ((uint32_t)( 31 | (0x02 << 26)))
#define APBPeriph_EFUSE                 ((uint32_t)( 30 | (0x02 << 26)))
#define APBPeriph_DSP_WDT               ((uint32_t)( 29 | (0x02 << 26)))
#define APBPeriph_ASRC                  ((uint32_t)( 28 | (0x02 << 26)))
#define APBPeriph_DSP_MEM               ((uint32_t)( 27 | (0x02 << 26)))
#define APBPeriph_DSP_H2D_D2H           ((uint32_t)( 26 | (0x02 << 26)))
#define APBPeriph_DSP_CORE              ((uint32_t)( 25 | (0x02 << 26)))
#define APBPeriph_SPI2W                 ((uint32_t)( 24 | (0x02 << 26)))
#define APBPeriph_PSRAM                 ((uint32_t)( 22 | (0x02 << 26)))
#define APBPeriph_I2C2                  ((uint32_t)( 20 | (0x02 << 26)))
#define APBPeriph_KEYSCAN               ((uint32_t)( 19 | (0x02 << 26)))
#define APBPeriph_QDEC                  ((uint32_t)( 18 | (0x02 << 26)))
#define APBPeriph_I2C1                  ((uint32_t)( 17 | (0x02 << 26)))

#define APBPeriph_SPI0                  ((uint32_t)( 8  | (0x02 << 26)))
#define APBPeriph_SPI1                  ((uint32_t)( 9  | (0x02 << 26)))
#define APBPeriph_SPI2                  ((uint32_t)( 11  | (0x02 << 26)))
#define APBPeriph_LCD                   ((uint32_t)( 5  | (0x02 << 26)))
#define APBPeriph_IF8080                ((uint32_t)( 5  | (0x02 << 26)))
#define APBPeriph_UART0                 ((uint32_t)( 0  | (0x02 << 26)))
#define APBPeriph_UART2                 ((uint32_t)( 1  | (0x02 << 26)))
#define APBPeriph_GPIOA                 ((uint32_t)( 8  | (0x03 << 26)))
#define APBPeriph_GPIO                   APBPeriph_GPIOA
#define APBPeriph_GPIOB                 ((uint32_t)( 9  | (0x03 << 26)))
#define APBPeriph_ADC                   ((uint32_t)(0  | (0x03 << 26)))
#define APBPeriph_IR                    ((uint32_t)( 10 | (0x02 << 26)))
#define APBPeriph_SIMC                  ((uint32_t)( 4  | (0x02 << 26)))
#define APBPeriph_RNG                   ((uint32_t)( 3  | (0x02 << 26)))
#define APBPeriph_AES                   ((uint32_t)( 2  | (0x02 << 26)))

#define APBPeriph_I2S0                  ((uint32_t)((1 << 1) | (0x00 << 26)))
#define APBPeriph_I2S1                  ((uint32_t)((1 << 2) | (0x00 << 26)))
#define APBPeriph_CODEC                 ((uint32_t)((1 << 0) | (0x00 << 26)))
#define APBPeriph_UART1_HCI             (0)
#define APBPeriph_CKE_MODEM             (0)

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_CAP) || ((PERIPH) == APBPeriph_AAC_XTAL)\
                               || ((PERIPH) == APBPeriph_SWR_SS)\
                               || ((PERIPH) == APBPeriph_TIMER) || ((PERIPH) == APBPeriph_USB)\
                               || ((PERIPH) == APBPeriph_SD_HOST) || ((PERIPH) == APBPeriph_GDMA)\
                               || ((PERIPH) == APBPeriph_UART1) || ((PERIPH) == APBPeriph_FLASH3)\
                               || ((PERIPH) == APBPeriph_FLASH2) || ((PERIPH) == APBPeriph_FLASH1)\
                               || ((PERIPH) == APBPeriph_FLASH)|| ((PERIPH) == APBPeriph_BTBUS)\
                               || ((PERIPH) == APBPeriph_SHA) || ((PERIPH) == APBPeriph_EFUSE)\
                               || ((PERIPH) == APBPeriph_DSP_WDT) || ((PERIPH) == APBPeriph_ASRC)\
                               || ((PERIPH) == APBPeriph_DSP_MEM) || ((PERIPH) == APBPeriph_DSP_H2D_D2H)\
                               || ((PERIPH) == APBPeriph_DSP_CORE) || ((PERIPH) == APBPeriph_SPI2W)\
                               || ((PERIPH) == APBPeriph_PSRAM) || ((PERIPH) == APBPeriph_I2C2)\
                               || ((PERIPH) == APBPeriph_KEYSCAN) || ((PERIPH) == APBPeriph_QDEC)\
                               || ((PERIPH) == APBPeriph_I2C1) || ((PERIPH) == APBPeriph_I2C0)\
                               || ((PERIPH) == APBPeriph_SPI2)|| ((PERIPH) == APBPeriph_IR)\
                               || ((PERIPH) == APBPeriph_SPI1) || ((PERIPH) == APBPeriph_SPI0)\
                               || ((PERIPH) == APBPeriph_LCD) || ((PERIPH) == APBPeriph_SIMC)\
                               || ((PERIPH) == APBPeriph_IF8080)\
                               || ((PERIPH) == APBPeriph_RNG) || ((PERIPH) == APBPeriph_AES)\
                               | ((PERIPH) == APBPeriph_UART2) || ((PERIPH) == APBPeriph_UART0)\
                               || ((PERIPH) == APBPeriph_GPIOB) || ((PERIPH) == APBPeriph_GPIOA)\
                               || ((PERIPH) == APBPeriph_GPIO)|| ((PERIPH) == APBPeriph_ADC)\
                               || ((PERIPH) == APBPeriph_DSP_PERI) || ((PERIPH) == APBPeriph_DSP_DMA)\
                               || (PERIPH == APBPeriph_UART1_HCI) || (PERIPH == APBPeriph_CKE_MODEM)\
                              )

/** End of group 87x3e_APB_Peripheral_Define
  * @}
  */

/** @defgroup 87x3e_RCC_Exported_Macros RCC Exported Macros
  * @{
  */

/** @defgroup 87x3e_RCC_Peripheral_Clock  RCC Peripheral Clock
  * @{
  */

#define CLOCK_GATE_5M                          ((uint32_t)(0x01 << 29))/* 5M clock source for adc and keyscan */
#define CLOCK_GATE_20M                         ((uint32_t)(0x01 << 27))/* 20M clock source for 2wssi and qdec */
#define CLOCK_GATE_10M                         ((uint32_t)(0x01 << 28))/* 10M clock source for bluewiz */
#define IS_CLOCK_GATE(CLOCK) (((CLOCK) == CLOCK_GATE_5M) || ((CLOCK) == CLOCK_GATE_20M)\
                              || ((CLOCK) == CLOCK_GATE_10M))

/**
  * @}
  */
/** @defgroup 87x3e_RCC_TIM_Clock_Source_config RCC TIM Clock Source config
  * @{
  */

#define TIM_CLOCK_SOURCE_SYSTEM_CLOCK                              ((uint16_t) BIT7)
#define TIM_CLOCK_SOURCE_40MHZ                                     ((uint16_t)(BIT5 | BIT7))
#define TIM_CLOCK_SOURCE_PLL                                       ((uint16_t)(BIT6 | BIT7))
#define IS_TIM_CLOCK_SOURCE(HZ)                                    (((HZ) == TIM_CLOCK_SOURCE_SYSTEM_CLOCK) || \
                                                                    ((HZ) == TIM_CLOCK_SOURCE_40MHZ) || ((HZ) == TIM_CLOCK_SOURCE_PLL))


/** @defgroup 87x3e_TIM_DIV TIM2TO7 Clock DIV
  * @{
  */

#define TIM2TO7_CLOCK_DIV_2                    ((uint16_t)0x1)
#define TIM2TO7_CLOCK_DIV_3                    ((uint16_t)0x2)
#define TIM2TO7_CLOCK_DIV_4                    ((uint16_t)0x3)
#define TIM2TO7_CLOCK_DIV_6                    ((uint16_t)0x4)
#define TIM2TO7_CLOCK_DIV_8                    ((uint16_t)0x5)
#define TIM2TO7_CLOCK_DIV_16                   ((uint16_t)0x6)
#define TIM2TO7_CLOCK_DIV_1                    ((uint16_t)0x8)
#define TIM2TO7_TIM_DIV(DIV)              (((DIV) == TIM2TO7_CLOCK_DIV_1) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_3) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_4) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_8) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_16))
/** @defgroup 87x3e_SPI_Clock_Divider SPI Clock Divider
  * @{
  */

#define SPI_CLOCK_DIV_1                    ((uint16_t)0x0)
#define SPI_CLOCK_DIV_2                    ((uint16_t)0x1)
#define SPI_CLOCK_DIV_4                    ((uint16_t)0x2)
#define SPI_CLOCK_DIV_8                    ((uint16_t)0x3)
#define SPI_CLOCK_DIV_3_4                   ((uint16_t)0x4)

#define IS_SPI_DIV(DIV)              (((DIV) == SPI_CLOCK_DIV_1) || \
                                      ((DIV) == SPI_CLOCK_DIV_2) || \
                                      ((DIV) == SPI_CLOCK_DIV_4) || \
                                      ((DIV) == SPI_CLOCK_DIV_8) || \
                                      ((DIV) == SPI_CLOCK_DIV_3_4))
/**
  * @}
  */

/** @defgroup 87x3e_I2C_Clock_Divider I2C Clock Divider
  * @{
  */

#define I2C_CLOCK_DIV_1                    ((uint16_t)0x0)
#define I2C_CLOCK_DIV_2                    ((uint16_t)0x1)
#define I2C_CLOCK_DIV_4                    ((uint16_t)0x2)
#define I2C_CLOCK_DIV_8                    ((uint16_t)0x3)
#define IS_I2C_DIV(DIV)              (((DIV) == I2C_CLOCK_DIV_1) || \
                                      ((DIV) == I2C_CLOCK_DIV_2) || \
                                      ((DIV) == I2C_CLOCK_DIV_4) || \
                                      ((DIV) == I2C_CLOCK_DIV_8))
/**
  * @}
  */

/** @defgroup 87x3e_UART_Clock_Divider UART Clock Divider
  * @{
  */

#define UART_CLOCK_DIV_1                    ((uint16_t)0x0)
#define UART_CLOCK_DIV_2                    ((uint16_t)0x1)
#define UART_CLOCK_DIV_4                    ((uint16_t)0x2)
#define UART_CLOCK_DIV_16                   ((uint16_t)0x3)
#define IS_UART_DIV(DIV)              (((DIV) == UART_CLOCK_DIV_1) || \
                                       ((DIV) == UART_CLOCK_DIV_2) || \
                                       ((DIV) == UART_CLOCK_DIV_4) || \
                                       ((DIV) == UART_CLOCK_DIV_16))
/**
  * @}
  */


/** @defgroup 87x3e_TIM_Clock_Divider UART Clock Divider
  * @{
  */

#define TIM_CLOCK_DIV_1                    ((uint16_t)0x0)
#define TIM_CLOCK_DIV_2                    ((uint16_t)0x4)
#define TIM_CLOCK_DIV_4                    ((uint16_t)0x5)
#define TIM_CLOCK_DIV_8                  ((uint16_t)0x6)
#define TIM_CLOCK_DIV_FIX_1MHZ                  ((uint16_t)0x7)
#define IS_TIM_DIV(DIV)              (((DIV) == TIM_CLOCK_DIV_1) || \
                                      ((DIV) == TIM_CLOCK_DIV_2) || \
                                      ((DIV) == TIM_CLOCK_DIV_4) || \
                                      ((DIV) == TIM_CLOCK_DIV_8) || \
                                      ((DIV) == TIM_CLOCK_DIV_FIXED ))
/**
  * @}
  */


/**End of group 87x3e_RCC_Exported_Macros
  * @}
  */

/** @defgroup 87x3e_RCC_Exported_Functions RCC Exported Functions
  * @{
  */

extern void (*RCC_PeriphClockCmd)(uint32_t APBPeriph, uint32_t APBPeriph_Clock,
                                  FunctionalState NewState);
extern void RCC_PeriClockConfig(uint32_t APBPeriph_Clock, FunctionalState NewState);
extern void RCC_PeriFunctionConfig(uint32_t APBPeriph, FunctionalState NewState);
extern void RCC_SPIClkDivConfig(SPI_TypeDef *SPIx, uint16_t ClockDiv);
extern void RCC_I2CClkDivConfig(I2C_TypeDef *I2Cx, uint16_t ClockDiv);
extern void RCC_UARTClkDivConfig(UART_TypeDef *UARTx, uint16_t ClockDiv);
extern void RCC_TIMClkDivConfig(TIM_TypeDef *TIMx, uint16_t ClockDiv);
extern void RCC_TimSourceConfig(uint16_t clocklevel, uint16_t clocksource,
                                FunctionalState NewState);
#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_RCC_H_ */

/**End of group 87x3e_RCC_Exported_Functions
  * @}
  */

/**End of group 87x3e_RCC
  * @}
  */




/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/



