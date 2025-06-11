
/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    rtl876x.h
  * @brief   CMSIS Cortex-M4 Peripheral Access Layer Header File for
  *          RTL876X from Realtek Semiconductor.
  * @date    2016.3.3
  * @version v1.0

 * @date     3. March 2015
 *
 * @note     Generated with SVDConv Vx.xxp
 *           from CMSIS SVD File 'RTL876X.xml' Version x.xC,
 *
 * @par      Copyright (c) 2015 Realtek Semiconductor. All Rights Reserved.
 *
 *           The information contained herein is property of Realtek Semiconductor.
 *           Terms and conditions of usage are described in detail in Realtek
 *           SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 *           Licensees are granted free, non-transferable use of the information. NO
 *           WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 *           the file.
 *
 *

  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef RTL876X_H
#define RTL876X_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup 87x3e_RTL876X Rtl876x
  * @brief   CMSIS Cortex-M4 peripheral access layer header file for
  *          RTL876X from Realtek Semiconductor
  * @{
  */


/** @defgroup 87x3e_Configuration_of_CMSIS Configuration of CMSIS
  * @brief   Configuration of the cm4 Processor and Core Peripherals
  * @{
  */
/* ----------------Configuration of the cm4 Processor and Core Peripherals---------------- */
#define __CM4_REV                      0x0001U    //!< Core revision r0p1
#define __MPU_PRESENT                  1          //!< MPU present or not
#define __FPU_PRESENT                  1          //!< FPU present
#define __NVIC_PRIO_BITS               3          //!< Number of Bits used for Priority Levels
#define __Vendor_SysTickConfig         0          //!< Set to 1 if different SysTick Config is used
/** @} */ /* End of group 87x3e_Configuration_of_CMSIS */

/*============================================================================*
 *                              Types
*============================================================================*/
/** @defgroup 87x3e_RTL876x_Exported_types RTL876X Exported types
  * @{
  */

//compatible define
#define UART                            UART0

/** brief Interrupt Number Definition */
typedef enum IRQn
{
    /* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
    NonMaskableInt_IRQn           = -14,      /**< 2 Non Maskable Interrupt */
    HardFault_IRQn                = -13,      /**< 3 HardFault Interrupt */
    MemoryManagement_IRQn         = -12,      /**< 4 Memory Management Interrupt */
    BusFault_IRQn                 = -11,      /**< 5 Bus Fault Interrupt */
    UsageFault_IRQn               = -10,      /**< 6 Usage Fault Interrupt */
    SVCall_IRQn                   =  -5,      /**< 11 SV Call Interrupt */
    DebugMonitor_IRQn             =  -4,      /**< 12 Debug Monitor Interrupt */
    PendSV_IRQn                   =  -2,      /**< 14 Pend SV Interrupt */
    SysTick_IRQn                  =  -1,      /**< 15 System Tick Interrupt */
    /* INT_NUMBER = 64 */
    System_IRQn = 0,
    WDT_IRQn,
    BTMAC_IRQn,
    DSP_IRQn,
    RXI300_IRQn,
    SPI0_IRQn,
    I2C0_IRQn,
    ADC_IRQn,
    SPORT0_RX_IRQn,
    SPORT0_TX_IRQn,
    TIM2_IRQn,
    TIM3_IRQn,
    TIM4_IRQn,
    RTC_IRQn,
    UART0_IRQn,
    UART1_IRQn,
    UART2_IRQn,
    Peripheral_IRQn,
    GPIO_A0_IRQn,
    GPIO_A1_IRQn,
    GPIO_A_2_7_IRQn,
    GPIO_A_8_15_IRQn,
    GPIO_A_16_23_IRQn,
    GPIO_A_24_31_IRQn,
    SPORT1_RX_IRQn,
    SPORT1_TX_IRQn,
    ADP_DET_NVIC_IRQn,
    VBAT_DET_NVIC_IRQn,
    GDMA0_Channel0_IRQn,
    GDMA0_Channel1_IRQn,
    GDMA0_Channel2_IRQn,
    GDMA0_Channel3_IRQn,
    GDMA0_Channel4_IRQn,
    GDMA0_Channel5_IRQn,
    GDMA0_Channel6_IRQn,
    GDMA0_Channel7_IRQn,
    GDMA0_Channel8_IRQn,
    GPIO_B_0_7_IRQn,
    GPIO_B_8_15_IRQn,
    GPIO_B_16_23_IRQn,
    GPIO_B_24_31_IRQn,
    SPI1_IRQn,
    SPI2_IRQn,
    I2C1_IRQn,
    I2C2_IRQn,
    KeyScan_IRQn,
    QDEC_IRQn,
    USB_IRQn,
    USB_ISOC_IRQn,
    SPIC0_IRQn,
    SPIC1_IRQn,
    SPIC2_IRQn,
    SPIC3_IRQn,
    TIM5_IRQn,
    TIM6_IRQn,
    TIM7_IRQn,
    ASRC0_IRQn,
    ASRC1_IRQn,
    I8080_IRQn,
    ISO7816_IRQn,
    SDIO0_IRQn,
    SPORT2_RX_IRQn,
    ANC_IRQn,
    TOUCH_IRQn,
    /* second level interrupt (Peripheral_IRQn), not directly connect to NVIC */
    MFB_DET_L_IRQn,
    PTA_Mailbox_IRQn,
    USB_UTMI_SUSPEND_N_IRQn,
    IR_IRQn,
    TRNG_IRQn,
    PSRAMC_IRQn,
    LPCOMP_IRQn,
    Timer5_Peri_IRQn,
    Timer6_Peri_IRQn,
    Timer7_Peri_IRQn,
    Peri_10_IRQn,
    Peri_11_IRQn,
    VBAT_DET_IRQn,
    ADP_DET_IRQn,
} IRQn_Type, *PIRQn_Type;

#define GPIO0_IRQn              GPIO_A0_IRQn
#define GPIO1_IRQn              GPIO_A1_IRQn
#define GPIO2_IRQn              GPIO_A_2_7_IRQn
#define GPIO3_IRQn              GPIO_A_2_7_IRQn
#define GPIO4_IRQn              GPIO_A_2_7_IRQn
#define GPIO5_IRQn              GPIO_A_2_7_IRQn
#define GPIO6_IRQn              GPIO_A_2_7_IRQn
#define GPIO7_IRQn              GPIO_A_2_7_IRQn
#define GPIO8_IRQn              GPIO_A_8_15_IRQn
#define GPIO9_IRQn              GPIO_A_8_15_IRQn
#define GPIO10_IRQn             GPIO_A_8_15_IRQn
#define GPIO11_IRQn             GPIO_A_8_15_IRQn
#define GPIO12_IRQn             GPIO_A_8_15_IRQn
#define GPIO13_IRQn             GPIO_A_8_15_IRQn
#define GPIO14_IRQn             GPIO_A_8_15_IRQn
#define GPIO15_IRQn             GPIO_A_8_15_IRQn
#define GPIO16_IRQn             GPIO_A_16_23_IRQn
#define GPIO17_IRQn             GPIO_A_16_23_IRQn
#define GPIO18_IRQn             GPIO_A_16_23_IRQn
#define GPIO19_IRQn             GPIO_A_16_23_IRQn
#define GPIO20_IRQn             GPIO_A_16_23_IRQn
#define GPIO21_IRQn             GPIO_A_16_23_IRQn
#define GPIO22_IRQn             GPIO_A_16_23_IRQn
#define GPIO23_IRQn             GPIO_A_16_23_IRQn
#define GPIO24_IRQn             GPIO_A_24_31_IRQn
#define GPIO25_IRQn             GPIO_A_24_31_IRQn
#define GPIO26_IRQn             GPIO_A_24_31_IRQn
#define GPIO27_IRQn             GPIO_A_24_31_IRQn
#define GPIO28_IRQn             GPIO_A_24_31_IRQn
#define GPIO29_IRQn             GPIO_A_24_31_IRQn
#define GPIO30_IRQn             GPIO_A_24_31_IRQn
#define GPIO31_IRQn             GPIO_A_24_31_IRQn

#define  GPIO32_IRQn            GPIO_B_0_7_IRQn
#define  GPIO33_IRQn            GPIO_B_0_7_IRQn
#define  GPIO34_IRQn            GPIO_B_0_7_IRQn
#define  GPIO35_IRQn            GPIO_B_0_7_IRQn
#define  GPIO36_IRQn            GPIO_B_0_7_IRQn
#define  GPIO37_IRQn            GPIO_B_0_7_IRQn
#define  GPIO38_IRQn            GPIO_B_0_7_IRQn
#define  GPIO39_IRQn            GPIO_B_0_7_IRQn
#define  GPIO40_IRQn            GPIO_B_8_15_IRQn
#define  GPIO41_IRQn            GPIO_B_8_15_IRQn
#define  GPIO42_IRQn            GPIO_B_8_15_IRQn
#define  GPIO43_IRQn            GPIO_B_8_15_IRQn
#define  GPIO44_IRQn            GPIO_B_8_15_IRQn
#define  GPIO45_IRQn            GPIO_B_8_15_IRQn
#define  GPIO46_IRQn            GPIO_B_8_15_IRQn
#define  GPIO47_IRQn            GPIO_B_8_15_IRQn
#define  GPIO48_IRQn            GPIO_B_16_23_IRQn
#define  GPIO49_IRQn            GPIO_B_16_23_IRQn
#define  GPIO50_IRQn            GPIO_B_16_23_IRQn
#define  GPIO51_IRQn            GPIO_B_16_23_IRQn
#define  GPIO52_IRQn            GPIO_B_16_23_IRQn
#define  GPIO53_IRQn            GPIO_B_16_23_IRQn
#define  GPIO54_IRQn            GPIO_B_16_23_IRQn
#define  GPIO55_IRQn            GPIO_B_16_23_IRQn
#define  GPIO56_IRQn            GPIO_B_24_31_IRQn
#define  GPIO57_IRQn            GPIO_B_24_31_IRQn
#define  GPIO58_IRQn            GPIO_B_24_31_IRQn
#define  GPIO59_IRQn            GPIO_B_24_31_IRQn
#define  GPIO60_IRQn            GPIO_B_24_31_IRQn
#define  GPIO61_IRQn            GPIO_B_24_31_IRQn
#define  GPIO62_IRQn            GPIO_B_24_31_IRQn
#define  GPIO63_IRQn            GPIO_B_24_31_IRQn

/** @} */ /* End of group 87x3e_RTL876x_Exported_types */

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include "core_cm4.h"                       /* Processor and core peripherals */

/*============================================================================*
 *                              Types
*============================================================================*/
/** @addtogroup 87x3e_RTL876x_Exported_types RTL876X Exported types
  * @{
  */

typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/** @} */ /* End of group 87x3e_RTL876x_Exported_types */



/*============================================================================*
 *                              RTL876X Pin Number
*============================================================================*/
/** @defgroup 87x3e_RTL876X_Pin_Number RTL876X Pin Number
  * @{
  */
#define P0_0        0       /**< GPIO0   */
#define P0_1        1       /**< GPIO1   */
#define P0_2        2       /**< GPIO2   */
#define P0_3        3       /**< GPIO3   */

#define P3_2        4       /**< GPIO4   */
#define P3_3        5       /**< GPIO5   */
#define P3_4        6       /**< GPIO6   */
#define P3_5        7       /**< GPIOB6  */

#define P1_0        8       /**< GPIO7   */
#define P1_1        9       /**< GPIO8  */
#define P1_2        10      /**< GPIO9   */
#define P1_3        11      /**< GPIO10  */

#define P1_4        12      /**< GPIO11  */
#define P1_5        13      /**< GPIO12  */
#define P1_6        14      /**< GPIO13  */
#define P1_7        15      /**< GPIO14  */

#define P2_0        16      /**< GPIO15  */
#define P2_1        17      /**< GPIO16  */
#define P2_2        18      /**< GPIO17  */
#define P2_3        19      /**< GPIO18  */

#define P2_4        20      /**< GPIO19  */
#define P2_5        21      /**< GPIO20  */
#define P2_6        22      /**< GPIO21  */
#define P2_7        23      /**< GPIO22  */

#define P3_0        24      /**< GPIO23  */
#define P3_1        25      /**< GPIO24  */
#define AUX_R       26      /**< AUX_R GPIOB2  */
#define AUX_L       27      /**< AUX_L GPIOB3  */

#define MIC1_P      28      /**< MIC1_P GPIO27  */
#define MIC1_N      29      /**< MIC1_N GPIO28  */
#define MIC2_P      30      /**< MIC2_P GPIO25  */
#define MIC2_N      31      /**< MIC2_N GPIO26  */

#define MICBIAS     32      /**< MICBIAS GPIO29  */
#define LOUT_P      33      /**< LOUT_P  GPIO30  */
#define LOUT_N      34      /**< LOUT_N  GPIO31  */
#define ROUT_P      35      /**< ROUT_P  GPIOB0  */

#define ROUT_N      36      /**< ROUT_N GPIOB1  */
#define MIC3_P      37      /**< MIC3_P GPIOB4  */
#define MIC3_N      38      /**< MIC3_N GPIOB5  */   //  0x2A4

#define P4_0        40      /**< GPIOB7   */ //offset 0x3B0
#define P4_1        41      /**< GPIOB8   */
#define P4_2        42      /**< GPIOB9   */
#define P4_3        43      /**< GPIOB10  */

#define P4_4        44      /**< GPIOB11  */
#define P4_5        45      /**< GPIOB12  */
#define P4_6        46      /**< GPIOB13  */
#define P4_7        47      /**< GPIOB14  */

#define P5_0        48      /**< GPIOB15   */ //offset 0x3B0
#define P5_1        49      /**< GPIOB16   */
#define P5_2        50      /**< GPIOB17   */
#define P5_3        51      /**< GPIOB18   */

#define P5_4        52      /**< GPIOB0   */
#define P5_5        53      /**< GPIOB1   */
#define P5_6        54      /**< GPIOB2   */
#define P5_7        55      /**< GPIOB3   */

#define P6_0        56      /**< GPIOA25   */
#define P6_1        57      /**< GPIOA26   */
#define P6_2        58      /**< GPIOA27   */
#define P6_3        59      /**< GPIOA28   */

#define P6_4        60      /**< GPIOA29   */
#define P6_5        61      /**< GPIOA30   */
#define P6_6        62      /**< GPIOA31   */
//#define P6_7      63      /**< GPIO18   */

#define P7_0        64      /**< GPIOB19   */
#define P7_1        65      /**< GPIOB20   */
#define P7_2        66      /**< GPIOB21   */
#define P7_3        67      /**< GPIOB22   */

#define P7_4        68      /**< GPIOB23   */
#define P7_5        69      /**< GPIOB24   */
#define P7_6        70      /**< GPIOB25   */
//#define P7_7        71      /**< GPIO  */

#define P8_0        72      /**< GPIOB26  */
#define P8_1        73      /**< GPIOB27   */
#define P8_2        74      /**< GPIOB28  */
#define P8_3        75      /**< GPIOB29   */

#define P8_4        76      /**< GPIOB30   */
#define P8_5        77      /**< GPIOB31   */

#define P9_0        80      /**< GPIOB0   */
#define P9_1        81      /**< GPIOB1   */
#define P9_2        82      /**< GPIOB2   */
#define P9_3        83      /**< GPIOB3   */

#define P9_4        84      /**< GPIOB4   */
#define P9_5        85      /**< GPIOB5   */
#define P10_0       86      /**< GPIO      */

#define  TOTAL_PIN_NUM             88
#define  PINMUX_REG1_ST_PIN                P4_0
#define  PINMUX_REG0_NUM                    10
#define  PINMUX_REG1_NUM                    12






#define ADC_0       P0_0    /**< GPIO0  */
#define ADC_1       P0_1    /**< GPIO1  */
#define ADC_2       P0_2    /**< GPIO2  */
#define ADC_3       P0_3    /**< GPIO3  */

/** @} */ /* End of group 87x3e_RTL876X_Pin_Number */


/* ================================================================================ */
/* ================    Peripheral Registers Structures Section     ================ */
/* ================================================================================ */

/** @defgroup 87x3e_RTL876X_Peripheral_Registers_Structures RTL876X Register Structure
  * @{
  */

/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */

/**
  * @brief Universal Asynchronous Receiver/Transmitter, version 1.0. (UART)
  */

typedef struct                                      /*!< UART Structure                                                        */
{
    __IO  uint32_t DLL;                             /*!< 0x00 */
    __IO  uint32_t DLH_INTCR;                       /*!< 0x04 */
    __IO  uint32_t INTID_FCR;                       /*!< 0x08 */
    __IO  uint32_t LCR;                             /*!< 0x0C */
    __IO  uint32_t MCR;                             /*!< 0x10 */
    __I   uint32_t LSR;                             /*!< 0x14 */
    __I   uint32_t MSR;                             /*!< 0x18 */
    __IO  uint32_t SPR;                             /*!< 0x1C */
    __IO  uint32_t STSR;                            /*!< 0x20 */
    __IO  uint32_t RB_THR;                          /*!< 0x24 */
    __IO  uint32_t MISCR;                           /*!< 0x28 may not be seen for client*/
    __IO  uint32_t TXPLSR;                          /*!< 0x2C */
    __IO  uint32_t RSVD0;                           /*!< 0x30 */
    __IO  uint32_t BaudMONR;                        /*!< 0x34 */
    __IO  uint32_t RSVD1;                           /*!< 0x38 */
    __I   uint32_t DBG_UART;                        /*!< 0x3C */
    __IO  uint32_t RX_IDLE_INTTCR;                  /*!< 0x40 */
    __IO  uint32_t RX_IDLE_SR;                      /*!< 0x44 */
    __IO  uint32_t RXIDLE_INTCR;                    /*!< 0x48 */
    __I   uint32_t FIFO_LEVEL;                      /*!< 0x4C */
    __IO  uint32_t REG_INT_MASK;                    /*!< 0x50 */
    __I   uint32_t REG_TXDONE_INT;                  /*!< 0x54 */
    __I   uint32_t REG_TX_THD_INT;                  /*!< 0x58 */
} UART_TypeDef;

typedef enum
{
    BAUD_RATE_1200,
    BAUD_RATE_9600,
    BAUD_RATE_14400,
    BAUD_RATE_19200,
    BAUD_RATE_28800,
    BAUD_RATE_38400,
    BAUD_RATE_57600,
    BAUD_RATE_76800,
    BAUD_RATE_115200,
    BAUD_RATE_128000,
    BAUD_RATE_153600,
    BAUD_RATE_230400,
    BAUD_RATE_460800,
    BAUD_RATE_500000,
    BAUD_RATE_921600,
    BAUD_RATE_1000000,
    BAUD_RATE_1382400,
    BAUD_RATE_1444400,
    BAUD_RATE_1500000,
    BAUD_RATE_1843200,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000,
    BAUD_RATE_4000000,
    BAUD_RATE_6000000
} UartBaudRate_TypeDef;

typedef enum
{
    LOG_CHANNEL_UART0,
    LOG_CHANNEL_UART1,
    LOG_CHANNEL_UART2
} LogChannel_TypeDef;

//typedef enum
//{
//    LOG_CHANNEL_LOG0_UART,
//    LOG_CHANNEL_LOG1_UART,
//    LOG_CHANNEL_DATA_UART
//} LogChannel_TypeDef;

/* ================================================================================ */
/* ================                 2WIRE_SPI                      ================ */
/* ================================================================================ */

/**
  * @brief 2wire spi, mostly used with mouse sensor. (2WIRE_SPI)
  */

typedef struct                                      /*!< 3WIRE_SPI Structure */
{
    __IO  uint32_t RSVD0[12]; // 0x00 -- 0x2C for Q-decode
    __IO  uint32_t CFGR;      // 0x30
    __IO  uint32_t CR;
    __IO  uint32_t INTCR;
    __I   uint32_t SR;
    __IO  uint32_t RD0;       // 0x40
    __IO  uint32_t RD1;
    __IO  uint32_t RD2;
    __IO  uint32_t RD3;
} SPI3WIRE_TypeDef;

/* ================================================================================ */
/* ================                 IR                             ================ */
/* ================================================================================ */

/**
  * @brief IR.
  */

typedef struct                                      /*!< IR Structure  */
{
    __IO  uint32_t CLK_DIV;                 /*!< 0x00 */
    __IO  uint32_t TX_CONFIG;               /*!< 0x04 */
    __IO  uint32_t TX_SR;                   /*!< 0x08 */
    __IO  uint32_t RESERVER;                /*!< 0x0c */
    __IO  uint32_t TX_INT_CLR;              /*!< 0x10 */
    __IO  uint32_t TX_FIFO;                 /*!< 0x14 */
    __IO  uint32_t RX_CONFIG;               /*!< 0x18 */
    __IO  uint32_t RX_SR;                   /*!< 0x1c */
    __IO  uint32_t RX_INT_CLR;              /*!< 0x20 */
    __IO  uint32_t RX_CNT_INT_SEL;          /*!< 0x24 */
    __IO  uint32_t RX_FIFO;                 /*!< 0x28 */
    __IO  uint32_t IR_VERSION;              /*!< 0x2c */
    __IO  uint32_t REVD[4];
    __IO  uint32_t TX_TIME;                 /*!< 0x40 */
    __IO  uint32_t RX_TIME;                 /*!< 0x44 */
    __IO  uint32_t IR_DBG;                  /*!< 0x48 */
} IR_TypeDef;

/* ================================================================================ */
/* ================                       SPI                      ================ */
/* ================================================================================ */

/**
  * @brief SPI master 0. (SPI)
  */
typedef struct                              /*!< SPI Structure  */
{
    __IO  uint32_t    CTRLR0;               /*!< 0x00 */
    __IO  uint32_t    CTRLR1;               /*!< 0x04 */
    __IO  uint32_t    SSIENR;               /*!< 0x08 */
    __IO  uint32_t    RSVD_0C;              /*!< 0x0C */
    __IO  uint32_t    SER;                  /*!< 0x10 */
    __IO  uint32_t    BAUDR;                /*!< 0x14 */
    __IO  uint32_t    TXFTLR;               /*!< 0x18 */
    __IO  uint32_t    RXFTLR;               /*!< 0x1C */
    __I  uint32_t     TXFLR;                /*!< 0x20 */
    __I  uint32_t     RXFLR;                /*!< 0x24 */
    __I  uint32_t     SR;                   /*!< 0x28 */
    __IO  uint32_t    IMR;                  /*!< 0x2C */
    __I  uint32_t     ISR;                  /*!< 0x30 */
    __I  uint32_t     RISR;                 /*!< 0x34 */
    __I  uint32_t     TXOICR;               /*!< 0x38 */
    __I  uint32_t     RXOICR;               /*!< 0x3C */
    __I  uint32_t     RXUICR;               /*!< 0x40 */
    __I  uint32_t     FAEICR;               /*!< 0x44 */
    __I  uint32_t     ICR;                  /*!< 0x48 */
    __IO  uint32_t    DMACR;                /*!< 0x4C */
    __IO  uint32_t    DMATDLR;              /*!< 0x50 */
    __IO  uint32_t    DMARDLR;              /*!< 0x54 */
    __I  uint32_t     TXUICR;               /*!< 0x58 */
    __I  uint32_t     SSRICR;               /*!< 0x5C */
    __IO  uint32_t    DR[36];               /*!< 0x60 - 0xEC */
    __IO  uint32_t    RX_SAMPLE_DLY;        /*!< 0xF0 */
} SPI_TypeDef;

/* ================================================================================ */
/* ================                      SD host Interface                    ================= */
/* ================================================================================ */

typedef struct
{
    __IO uint32_t SDMA_SYS_ADDR;            /*!< 0x00 */
    __IO uint16_t BLK_SIZE;                 /*!< 0x04 */
    __IO uint16_t BLK_CNT;                  /*!< 0x06 */
    __IO uint32_t ARG;                      /*!< 0x08 */
    __IO uint16_t TF_MODE;                  /*!< 0x0C */
    __IO uint16_t CMD;                      /*!< 0x0E */
    __I  uint32_t RSP0;                     /*!< 0x10 */
    __I  uint32_t RSP2;                     /*!< 0x14 */
    __I  uint32_t RSP4;                     /*!< 0x18 */
    __I  uint32_t RSP6;                     /*!< 0x1C */
    __IO uint32_t BUF_DATA_PORT;            /*!< 0x20 */
    __I  uint32_t PRESENT_STATE;            /*!< 0x24 */
    __IO uint8_t  HOST_CTRL;                /*!< 0x28 */
    __IO uint8_t  PWR_CTRL;                 /*!< 0x29 */
    __IO uint8_t  BLK_GAP_CTRL;             /*!< 0x2A */
    __IO uint8_t  WAKEUP_CTRL;              /*!< 0x2B */
    __IO uint16_t CLK_CTRL;                 /*!< 0x2C */
    __IO uint8_t  TIMEOUT_CTRL;             /*!< 0x2E */
    __IO uint8_t  SW_RESET;                 /*!< 0x2F */
    __IO uint16_t NORMAL_INTR_SR;           /*!< 0x30 */
    __IO uint16_t ERR_INTR_SR;              /*!< 0x32 */
    __IO uint16_t NORMAL_INTR_SR_EN;        /*!< 0x34 */
    __IO uint16_t ERR_INTR_SR_EN;           /*!< 0x36 */
    __IO uint16_t NORMAL_INTR_SIG_EN;       /*!< 0x38 */
    __IO uint16_t ERR_INTR_SIG_EN;          /*!< 0x3A */
    __I  uint16_t CMD12_ERR_SR;             /*!< 0x3C */
    __I  uint16_t RSVD0;                    /*!< 0x3E */
    __IO uint32_t CAPABILITIES_L;           /*!< 0x40 */
    __IO uint32_t CAPABILITIES_H;           /*!< 0x44 */
    __I  uint32_t RSVD1[4];                 /*!< 0x48 */
    __IO uint32_t ADMA_SYS_ADDR_L;          /*!< 0x58 */
    __IO uint32_t ADMA_SYS_ADDR_H;          /*!< 0x5C */
    __I  uint32_t RSVD2[1000];              /*!< 0x60 */
    __IO uint8_t  CAPABILITY_CTRL;          /*!< 0x1000 */
    __IO uint8_t  SYSTEM_CTRL;              /*!< 0x1001 */
    __IO uint8_t  LOOPBACK_CTRL;            /*!< 0x1002 */
    __I  uint8_t  DEBUG_INFO;               /*!< 0x1003 */
    __I  uint8_t  DEBUG_INFO2;              /*!< 0x1004 */
    __I  uint8_t  RSVD3[3];                 /*!< 0x1005 */
    __IO uint32_t DMA_CTRL;                 /*!< 0x1008 */
    __I  uint32_t RSVD4;                    /*!< 0x100C */
    __IO uint32_t BUS_DLY_SEL;              /*!< 0x1010 */
} SDIO_TypeDef;

/* ================================================================================ */
/* ================                      I2C                      ================= */
/* ================================================================================ */

/**
  * @brief I2C
  */
typedef struct
{
    __IO uint32_t IC_CON;                   /*!< 0x00 */
    __IO uint32_t IC_TAR;                   /*!< 0x04 */
    __IO uint32_t IC_SAR;                   /*!< 0x08 */
    __IO uint32_t IC_HS_MADDR;              /*!< 0x0C */
    __IO uint32_t IC_DATA_CMD;              /*!< 0x10 */
    __IO uint32_t IC_SS_SCL_HCNT;           /*!< 0x14 */
    __IO uint32_t IC_SS_SCL_LCNT;           /*!< 0x18 */
    __IO uint32_t IC_FS_SCL_HCNT;           /*!< 0x1C */
    __IO uint32_t IC_FS_SCL_LCNT;           /*!< 0x20 */
    __IO uint32_t IC_HS_SCL_HCNT;           /*!< 0x24 */
    __IO uint32_t IC_HS_SCL_LCNT;           /*!< 0x28 */
    __I uint32_t IC_INTR_STAT;              /*!< 0x2C */
    __IO uint32_t IC_INTR_MASK;             /*!< 0x30 */
    __I uint32_t IC_RAW_INTR_STAT;          /*!< 0x34 */
    __IO uint32_t IC_RX_TL;                 /*!< 0x38 */
    __IO uint32_t IC_TX_TL;                 /*!< 0x3C */
    __I uint32_t IC_CLR_INTR;               /*!< 0x40 */
    __I uint32_t IC_CLR_RX_UNDER;           /*!< 0x44 */
    __I uint32_t IC_CLR_RX_OVER;            /*!< 0x48 */
    __I uint32_t IC_CLR_TX_OVER;            /*!< 0x4C */
    __I uint32_t IC_CLR_RD_REQ;             /*!< 0x50 */
    __I uint32_t IC_CLR_TX_ABRT;            /*!< 0x54 */
    __I uint32_t IC_CLR_RX_DONE;            /*!< 0x58 */
    __I uint32_t IC_CLR_ACTIVITY;           /*!< 0x5C */
    __I uint32_t IC_CLR_STOP_DET;           /*!< 0x60 */
    __I uint32_t IC_CLR_START_DET;          /*!< 0x64 */
    __I uint32_t IC_CLR_GEN_CALL;           /*!< 0x68 */
    __IO uint32_t IC_ENABLE;                /*!< 0x6C */
    __I uint32_t IC_STATUS;                 /*!< 0x70 */
    __I uint32_t IC_TXFLR;                  /*!< 0x74 */
    __I uint32_t IC_RXFLR;                  /*!< 0x78 */
    __IO uint32_t IC_SDA_HOLD;              /*!< 0x7C */
    __I uint32_t IC_TX_ABRT_SOURCE;         /*!< 0x80 */
    __IO uint32_t IC_SLV_DATA_NACK_ONLY;    /*!< 0x84 */
    __IO uint32_t IC_DMA_CR;                /*!< 0x88 */
    __IO uint32_t IC_DMA_TDLR;              /*!< 0x8C */
    __IO uint32_t IC_DMA_RDLR;              /*!< 0x90 */
    __IO uint32_t IC_SDA_SETUP;             /*!< 0x94 */
    __IO uint32_t IC_ACK_GENERAL_CALL;      /*!< 0x98 */
    __IO uint32_t IC_ENABLE_STATUS;         /*!< 0x9C */
    __IO uint32_t IC_FS_SPKLEN;             /*!< 0xA0 */
} I2C_TypeDef;

/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */

/**
  * @brief Analog to digital converter. (ADC)
  */

typedef struct                              /*!< ADC Structure */
{
    __O  uint32_t FIFO;                     //!<0x00
    __IO uint32_t CR;                       //!<0x04
    __IO uint32_t SCHCR;                    //!<0x08
    __IO uint32_t INTCR;                    //!<0x0C
    __IO uint32_t SCHTAB0;                  //!<0x10
    __IO uint32_t SCHTAB1;                  //!<0x14
    __IO uint32_t SCHTAB2;                  //!<0x18
    __IO uint32_t SCHTAB3;                  //!<0x1C
    __IO uint32_t SCHTAB4;                  //!<0x20
    __IO uint32_t SCHTAB5;                  //!<0x24
    __IO uint32_t SCHTAB6;                  //!<0x28
    __IO uint32_t SCHTAB7;                  //!<0x2C
    __IO uint32_t SCHD0;                    //!<0x30
    __IO uint32_t SCHD1;                    //!<0x34
    __IO uint32_t SCHD2;                    //!<0x38
    __IO uint32_t SCHD3;                    //!<0x3C
    __IO uint32_t SCHD4;                    //!<0x40
    __IO uint32_t SCHD5;                    //!<0x44
    __IO uint32_t SCHD6;                    //!<0x48
    __IO uint32_t SCHD7;                    //!<0x4C
    __IO uint32_t PWRDLY;                   //!<0x50
    __IO uint32_t DATCLK;                   //!<0x54
    __IO uint32_t ANACTL;                   //!<0x58
    __IO uint32_t REG_5C_CLK;             //!<0x5C
    __IO uint32_t RTL_VER;                  //!<0x60
} ADC_TypeDef;

/* ================================================================================ */
/* ================                      TIM                      ================ */
/* ================================================================================ */

/**
  * @brief TIM
  */
typedef struct
{
    __IO uint32_t LoadCount;                /*!< 0x00*/
    __I  uint32_t CurrentValue;             /*!< 0x04*/
    __IO uint32_t ControlReg;               /*!< 0x08*/
    __I  uint32_t EOI;                      /*!< 0x0C*/
    __I  uint32_t IntStatus;                /*!< 0x10*/
} TIM_TypeDef;

typedef struct
{
    __I uint32_t TimersIntStatus;        /*!< 0xa0, Timers Interrupt Status Register */
    __I uint32_t TimersEOI;              /*!< 0xa4, Timers End-of-Interrupt Register */
    __I uint32_t TimersRawIntStatus;     /*!< 0xa8, Timers Raw Interrupt Status Register */
} TIM_ChannelsTypeDef;

/* ================================================================================ */
/* ================                      GDMA                      ================ */
/* ================================================================================ */
/**
  * @brief GDMA interrupt registers
  */
typedef struct
{
    __I uint32_t RAW_TFR;                   /*!< offset 0x2C0*/
    uint32_t RSVD0;
    __I uint32_t RAW_BLOCK;                 /*!< offset 0x2C8*/
    uint32_t RSVD1;
    __I uint32_t RAW_SRC_TRAN;              /*!< offset 0x2D0*/
    uint32_t RSVD2;
    __I uint32_t RAW_DST_TRAN;              /*!< offset 0x2D8*/
    uint32_t RSVD3;
    __I uint32_t RAW_ERR;                   /*!< offset 0x2E0*/
    uint32_t RSVD4;

    __I uint32_t STATUS_TFR;                /*!< offset 0x2E8*/
    uint32_t RSVD5;
    __I uint32_t STATUS_BLOCK;              /*!< offset 0x2F0*/
    uint32_t RSVD6;
    __I uint32_t STATUS_SRC_TRAN;           /*!< offset 0x2F8*/
    uint32_t RSVD7;
    __I uint32_t STATUS_DST_TRAN;           /*!< offset 0x300*/
    uint32_t RSVD8;
    __I uint32_t STATUS_ERR;                /*!< offset 0x308*/
    uint32_t RSVD9;

    __IO uint32_t MASK_TFR;                 /*!< offset 0x310*/
    uint32_t RSVD10;
    __IO uint32_t MASK_BLOCK;               /*!< offset 0x318*/
    uint32_t RSVD11;
    __IO uint32_t MASK_SRC_TRAN;            /*!< offset 0x320*/
    uint32_t RSVD12;
    __IO uint32_t MASK_DST_TRAN;            /*!< offset 0x328*/
    uint32_t RSVD13;
    __IO uint32_t MASK_ERR;                 /*!< offset 0x330*/
    uint32_t RSVD14;

    __O uint32_t CLEAR_TFR;                 /*!< offset 0x338*/
    uint32_t RSVD15;
    __O uint32_t CLEAR_BLOCK;               /*!< offset 0x340*/
    uint32_t RSVD16;
    __O uint32_t CLEAR_SRC_TRAN;            /*!< offset 0x348*/
    uint32_t RSVD17;
    __O uint32_t CLEAR_DST_TRAN;            /*!< offset 0x350*/
    uint32_t RSVD18;
    __O uint32_t CLEAR_ERR;                 /*!< offset 0x358*/
    uint32_t RSVD19;
    __O uint32_t StatusInt;                 /*!< offset 0x360, reserved in RTK DMAC IP*/
    uint32_t RSVD191;

    __IO uint32_t ReqSrcReg;                /*!< offset 0x368, reserved in RTK DMAC IP*/
    uint32_t RSVD20;
    __IO uint32_t ReqDstReg;                /*!< offset 0x370, reserved in RTK DMAC IP*/
    uint32_t RSVD21;
    __IO uint32_t SglReqSrcReg;             /*!< offset 0x378, reserved in RTK DMAC IP*/
    uint32_t RSVD22;
    __IO uint32_t SglReqDstReg;             /*!< offset 0x380, reserved in RTK DMAC IP*/
    uint32_t RSVD23;
    __IO uint32_t LstSrcReg;                /*!< offset 0x388, reserved in RTK DMAC IP*/
    uint32_t RSVD24;
    __IO uint32_t LstDstReg;                /*!< offset 0x390, reserved in RTK DMAC IP*/
    uint32_t RSVD25;

    __IO uint32_t DmaCfgReg;                /*!< offset 0x398 */
    uint32_t RSVD26;
    __IO uint32_t ChEnReg;                  /*!< offset 0x3A0 */
    uint32_t RSVD27;
    __I uint32_t DmaIdReg;                  /*!< offset 0x3A8 */
    uint32_t RSVD28;
    __IO uint32_t DmaTestReg;               /*!< offset 0x3B0 */
    uint32_t RSVD29;
} GDMA_TypeDef;

typedef struct
{
    __IO uint32_t SAR;                  /*!< offset 0x00*/
    uint32_t RSVD0;
    __IO uint32_t DAR;                  /*!< offset 0x08*/
    uint32_t RSVD1;
    __IO uint32_t LLP;                  /*!< offset 0x10*/
    uint32_t RSVD2;
    __IO uint32_t CTL_LOW;              /*!< offset 0x18*/
    __IO uint32_t CTL_HIGH;             /*!< offset 0x1C*/
    __IO uint32_t SSTAT;                /*!< offset 0x20*/
    uint32_t RSVD4;
    __IO uint32_t DSTAT;                /*!< offset 0x28*/
    uint32_t RSVD5;
    __IO uint32_t SSTATAR;              /*!< offset 0x30*/
    uint32_t RSVD6;
    __IO uint32_t DSTATAR;              /*!< offset 0x38*/
    uint32_t RSVD7;
    __IO uint32_t CFG_LOW;              /*!< offset 0x40*/
    __IO uint32_t CFG_HIGH;
    __IO uint32_t SGR_LOW;                  /*!< offset 0x48*/
    __IO uint32_t SGR_HIGH;
    __IO uint32_t DSR_LOW;                  /*!< offset 0x50*/
    __IO uint32_t DSR_HIGH;
} GDMA_ChannelTypeDef;

/* ================================================================================ */
/* ================                     I8080                      ================ */
/* ================================================================================ */

/**
  * @brief IF8080
  */
typedef struct
{
    __IO uint32_t CTRL0;            /*!< 0x00 */
    __IO uint32_t CTRL1;            /*!< 0x04 */
    __IO uint32_t IMR;              /*!< 0x08 */
    __I  uint32_t SR;               /*!< 0x0C */
    __O  uint32_t ICR;              /*!< 0x10 */
    __IO uint32_t CFG;              /*!< 0x14 */
    __IO uint32_t FIFO;             /*!< 0x18 */
    __I  uint32_t RESV0;            /*!< 0x1C */
    __I  uint32_t RXDATA;           /*!< 0x20 */
    __IO uint32_t TX_LEN;           /*!< 0x24 */
    __I  uint32_t TX_CNT;           /*!< 0x28 */
    __I  uint32_t RESV1;            /*!< 0x2C */
    __IO uint32_t RX_LEN;           /*!< 0x30 */
    __I  uint32_t RX_CNT;           /*!< 0x34 */
    __IO uint32_t CMD1;             /*!< 0x38 */
    __IO uint32_t CMD2;             /*!< 0x3C */
    __IO uint32_t CMD3;             /*!< 0x40 */
} IF8080_TypeDef;

typedef struct
{
    __IO uint32_t SAR;
    __IO uint32_t DAR;
    __IO uint32_t LLP;
    __IO uint32_t CTL_LOW;
    __IO uint32_t CTL_HIGH;
} IF8080_GDMALLITypeDef;

typedef struct
{
    __IO uint32_t CR;
    __IO uint32_t LLI;
} IF8080_GDMATypeDef;

typedef struct
{
    __IO uint32_t SAR_OFT;
    __IO uint32_t DAR_OFT;
} IF8080_GDMALLIOFTTypeDef;

/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */

/**
  * @brief Real time counter 0. (RTC)
  */

typedef struct
{
    __IO uint32_t CR0;                      /**< 0x00  */
    __IO uint32_t INT_MASK;                 /**< 0x04  */
    __IO uint32_t INT_SR;                   /**< 0x08  */
    __IO uint32_t PRESCALER;                /**< 0x0C  */
    __IO uint32_t COMP0;                    /**< 0x10  */
    __IO uint32_t COMP1;                    /**< 0x14  */
    __IO uint32_t COMP2;                    /**< 0x18  */
    __IO uint32_t COMP3;                    /**< 0x1C  */
    __IO uint32_t COMP0GT;                  /**< 0x20  */
    __IO uint32_t COMP1GT;                  /**< 0x24  */
    __IO uint32_t COMP2GT;                  /**< 0x28  */
    __IO uint32_t COMP3GT;                  /**< 0x2C  */
    __I  uint32_t CNT;                      /**< 0x30  */
    __I  uint32_t PRESCALE_CNT;             /**< 0x34  */
    __IO uint32_t PRESCALE_CMP;             /**< 0x38  */
    __IO uint32_t BACKUP;                   /**< 0x3C  */

} RTC_TypeDef;

/**
  * @brief Real time for lp (RTC)
  */
typedef struct                              // offset from 0x40000000
{
    __IO uint32_t LPC_CR0;                  /*!< 0x180 */
    __I  uint32_t LPC_SR;                   /*!< 0x184 */
    __IO uint32_t LPC_CMP_LOAD;             /*!< 0x188 */
    __IO uint32_t LPC_CMP_CNT;              /*!< 0x18C */
    //__IO uint32_t LPC_SR_IN_SLEEP_MODE;   // removed in BBLite
} LPC_TypeDef;

/**
  * @brief SLEEP MODE LED (RTC)
  */

typedef struct
{
    __IO uint32_t S_LED_CR;                 //!<0x00
    __IO uint32_t S_LED_CH0_CR0;            //!<0x04
    __IO uint32_t S_LED_CH0_CR1;            //!<0x08
    __IO uint32_t S_LED_CH0_P1_CR;          //!<0x0C
    __IO uint32_t S_LED_CH0_P2_CR;          //!<0x10
    __IO uint32_t S_LED_CH0_P3_CR;          //!<0x14
    __IO uint32_t S_LED_CH0_P4_CR;          //!<0x18
    __IO uint32_t S_LED_CH0_P5_CR;          //!<0x1C
    __IO uint32_t S_LED_CH0_P6_CR;          //!<0x20
    __IO uint32_t S_LED_CH0_P7_CR;          //!<0x24
    __IO uint32_t S_LED_CH0_P8_CR;          //!<0x28

    __IO uint32_t RSV0;                     //!<0x2C

    __IO uint32_t S_LED_CH1_CR0;            //!<0x30
    __IO uint32_t S_LED_CH1_CR1;            //!<0x34
    __IO uint32_t S_LED_CH1_P1_CR;          //!<0x38
    __IO uint32_t S_LED_CH1_P2_CR;          //!<0x3C
    __IO uint32_t S_LED_CH1_P3_CR;          //!<0x40
    __IO uint32_t S_LED_CH1_P4_CR;          //!<0x44
    __IO uint32_t S_LED_CH1_P5_CR;          //!<0x48
    __IO uint32_t S_LED_CH1_P6_CR;          //!<0x4C
    __IO uint32_t S_LED_CH1_P7_CR;          //!<0x50
    __IO uint32_t S_LED_CH1_P8_CR;          //!<0x54

    __IO uint32_t RSV1;                     //!<0x58
    __IO uint32_t RSV2;                     //!<0x5C

    __IO uint32_t S_LED_CH2_CR0;            //!<0x60
    __IO uint32_t S_LED_CH2_CR1;            //!<0x64
    __IO uint32_t S_LED_CH2_P1_CR;          //!<0x68
    __IO uint32_t S_LED_CH2_P2_CR;          //!<0x6C
    __IO uint32_t S_LED_CH2_P3_CR;          //!<0x70
    __IO uint32_t S_LED_CH2_P4_CR;          //!<0x74
    __IO uint32_t S_LED_CH2_P5_CR;          //!<0x78
    __IO uint32_t S_LED_CH2_P6_CR;          //!<0x7C
    __IO uint32_t S_LED_CH2_P7_CR;          //!<0x80
    __IO uint32_t S_LED_CH2_P8_CR;          //!<0x84
} RTC_LED_TypeDef;

/* ================================================================================ */
/* ================                      QDEC                      ================ */
/* ================================================================================ */

/**
  * @brief Rotary decoder. (QDEC)
  */

typedef struct                              /*!< QDEC Structure */
{
    __IO uint32_t   REG_DIV;                /*!< 0x00 */
    __IO uint32_t   REG_CR_X;               /*!< 0x04 */
    __IO uint32_t   REG_SR_X;               /*!< 0x08 */
    __IO uint32_t   REG_CR_Y;               /*!< 0x0C */
    __IO uint32_t   REG_SR_Y;               /*!< 0x10 */
    __IO uint32_t   REG_CR_Z;               /*!< 0x14 */
    __IO uint32_t   REG_SR_Z;               /*!< 0x18 */
    __IO uint32_t   INT_MASK;               /*!< 0x1C, Reserved */
    __IO uint32_t   INT_SR;                 /*!< 0x20 */
    __IO uint32_t   INT_CLR;                /*!< 0x24 */
    __IO uint32_t   REG_DBG;                /*!< 0x28 */
    __IO uint32_t   REG_VERSION;            /*!< 0x2C  */
} QDEC_TypeDef;

/* ================================================================================ */
/* ================                     Watch Dog                     ================ */
/* ================================================================================ */

/**
  * @brief Watch Dog. (WDG)
  */

typedef struct                              /*!< WDG Structure */
{
    __IO uint32_t WDG_CTL;                  /*!< 0x00 */
}   WDG_TypeDef;

/**
  * @brief AON Watchdog
  */
typedef struct
{
    union
    {
        __IO uint32_t CRT; /*!< 0x00 (R/W) Control Register                                       */
        struct
        {
            uint32_t EN: 2;     /*!< R/W WDT2 Enable, 2'b10: OFF, the other value: ON             */
            uint32_t CNT_CTL: 1; /*!< R/W Counter Control,
                                     1: WDG continue count in DLPS/Power Down/Hibernate,
                                     0: WDG stop count in DLPS/Power Down/Hibernate               */
            uint32_t RST_LVL: 1; /*!< R/W Reset Level,
                                   1: Level 2 reset (reset whole chip),
                                   0: Level 1 reset (reset whole chip except partial AON and RTC) */
            uint32_t CNT_RELOAD: 1; /*!< R/W Reload Counter,
                                        1: Reload counter when exit DLPS/Power Down/Hibernate,
                                        0: Not reload counter when exit DLPS/Power Down/Hibernate */
            uint32_t RSVD0: 3;
            uint32_t COMP: 18;  /*!< R/W Set the period of AON WDG : 1/450Hz ~ 1/2.3KHz           */
            uint32_t RSVD1: 6;
        } CRT_BITS;
    } u_00;
    __IO uint32_t CNT_CLR; /*!< 0x04 (R/W) Clear Register, write 1 to reload AON WDG counter      */
    __IO uint32_t WP;      /*!< 0x08 (R/W) Write Protect Register, write 1 to enable write CRT    */
} AON_WDG_TypeDef;

/* ================================================================================ */
/* ================                     random generator           ================ */
/* ================================================================================ */

/**
  * @brief random generator. (RAN_GEN)
  */

typedef struct                              /*!< RAN_GEN Structure */
{
    union
    {
        __IO uint32_t CTL;                  /*!< 0x00              */
        struct
        {
            __IO uint32_t rand_gen_en: 1;
            __IO uint32_t seed_upd: 1;
            __IO uint32_t random_req: 1;
            __IO uint32_t opt_rand_upd: 1;
            __IO uint32_t soft_rst: 1;
            __IO uint32_t rsvd: 27;
        } CTL_BITS;
    } u_00;
    __IO uint32_t POLYNOMIAL;               /*!< 0x04              */
    __IO uint32_t SEED;                     /*!< 0x08              */
    __IO uint32_t RAN_NUM;                  /*!< 0x0C              */
} RAN_GEN_TypeDef;


/* ================================================================================ */
/* ================            System Block Control            ================ */
/* ================================================================================ */

/**
  * @brief System Block Control. (SYS_BLKCTRL), SYSBLKCTRL_REG_BASE
  */

typedef struct                              /*!< SYS_BLKCTRL Structure */
{
    union
    {
        __IO uint32_t SYS_CLK_SEL;
        struct
        {
            __IO uint32_t r_cpu_slow_en: 1;
            __IO uint32_t r_cpu_slow_opt_wfi: 1;
            __IO uint32_t r_cpu_slow_opt_dsp: 1;
            __IO uint32_t r_dsp_slow_en: 1;
            __IO uint32_t r_dsp_slow_opt_dsp: 1;
            __IO uint32_t r_auto_dsp_fast_clk_en: 1;
            __IO uint32_t r_clk_cpu_f1m_en: 1;
            __IO uint32_t r_clk_cpu_32k_en: 1;
            __IO uint32_t r_aon_rd_opt: 1;
            __IO uint32_t r_bus_slow_sel: 2;
            __IO uint32_t r_dsp_fast_clk_ext_num: 8;
            __IO uint32_t r_bt_ahb_wait_cnt: 6;
            __IO uint32_t r_btaon_acc_no_block: 1;
            __IO uint32_t r_cpu_slow_opt_at_tx: 1;
            __IO uint32_t r_cpu_slow_opt_at_rx: 1;
            __IO uint32_t r_cpu_low_rate_valid_num: 4;
        } BITS_200;
    } u_200;

    union
    {
        __IO uint32_t SLOW_CTRL;
        struct
        {
            __I  uint32_t RF_RL_ID: 4;
            __I  uint32_t RF_RTL_ID: 4;
            __IO uint32_t dummy: 5;
            __IO uint32_t r_dsp_clk_src_pll_sel: 1;
            __IO uint32_t r_dsp_auto_slow_filter_en: 1;
            __IO uint32_t bzdma_autoslow_eco_disable: 1;
            __IO uint32_t dummy1: 2;
            __IO uint32_t r_auto_slow_opt: 1;
            __IO uint32_t r_cpu_slow_opt_dma: 1;
            __IO uint32_t r_cpu_slow_opt_sdio0: 1;
            __IO uint32_t r_cpu_slow_opt_usb: 1;
            __IO uint32_t r_cpu_slow_opt_bt_sram_1: 1;
            __IO uint32_t r_cpu_slow_opt_bt_sram_2: 1;
            __IO uint32_t r_dsp_slow_opt_dspram_wbuf: 1;
            __IO uint32_t r_cpu_slow_opt_dspram_wbuf: 1;
            __IO uint32_t r_dsp_slow_opt_at_tx: 1;
            __IO uint32_t r_dsp_slow_opt_at_rx: 1;
            __IO uint32_t r_dsp_low_rate_valid_num: 4;
        } BITS_204;
    } u_204;

    union
    {
        __IO uint32_t REG_0x208;
        struct
        {
            __IO uint32_t r_cpu_div_sel: 4;
            __IO uint32_t r_cpu_div_sel_slow: 4;
            __IO uint32_t r_cpu_div_en: 1;
            __IO uint32_t r_CPU_CLK_SRC_EN: 1;
            __IO uint32_t r_cpu_auto_slow_filter_en: 1;
            __IO uint32_t r_cpu_auto_slow_force_update: 1;
            __IO uint32_t r_cpu_pll_clk_cg_en: 1;
            __IO uint32_t r_cpu_xtal_clk_cg_en: 1;
            __IO uint32_t r_cpu_osc40_clk_cg_en: 1;
            __IO uint32_t r_cpu_div_en_slow: 1;
            __IO uint32_t r_dsp_div_sel: 4;
            __IO uint32_t r_dsp_div_sel_slow: 4;
            __IO uint32_t r_dsp_div_en: 1;
            __IO uint32_t r_DSP_CLK_SRC_EN: 1;
            __IO uint32_t r_dsp_clk_src_sel_1: 1;
            __IO uint32_t r_dsp_clk_src_sel_0: 1;
            __IO uint32_t r_dsp_pll_clk_cg_en: 1;
            __IO uint32_t r_dsp_xtal_clk_cg_en: 1;
            __IO uint32_t r_dsp_osc40_clk_cg_en: 1;
            __IO uint32_t r_dsp_div_en_slow: 1;
        } BITS_208;
    } u_208;

    union
    {
        __IO uint32_t REG_0x20C;
        struct
        {
            __IO uint32_t r_flash_div_sel: 4;
            __IO uint32_t r_flash_div_en: 1;
            __IO uint32_t r_FLASH_CLK_SRC_EN: 1;
            __IO uint32_t r_flash_clk_src_sel_1: 1;
            __IO uint32_t r_flash_clk_src_sel_0: 1;
            __IO uint32_t r_flash_mux_1_clk_cg_en: 1;
            __IO uint32_t r_rng_sfosc_sel: 1;
            __IO uint32_t r_rng_sfosc_div_sel: 3;
            __IO uint32_t r_flash_clk_src_pll_sel: 1;
            __IO uint32_t RSVD: 2;
            __IO uint32_t r_40m_div_sel: 3;
            __IO uint32_t RSVD1: 1;
            __IO uint32_t r_40m_div_en: 1;
            __IO uint32_t r_CLK_40M_DIV_CG_EN: 1;
            __IO uint32_t r_CLK_40M_SRC_EN: 1;
            __IO uint32_t r_40m_clk_src_sel_1: 1;
            __IO uint32_t r_40m_clk_src_sel_0: 1;
            __IO uint32_t RSVD2: 1;
            __IO uint32_t r_CLK_40M_SRC_DIV_EN: 1;
            __IO uint32_t r_CLK_20M_SRC_EN: 1;
            __IO uint32_t r_CLK_10M_SRC_EN: 1;
            __IO uint32_t r_CLK_1M_SRC_EN: 1;
            __IO uint32_t RSVD3: 2;
        } BITS_20C;
    } u_20C;

    union
    {
        __IO uint32_t SOC_FUNC_EN;
        struct
        {
            __IO uint32_t Dummy: 1;
            __IO uint32_t Dummy1: 1;
            __IO uint32_t BIT_SOC_BTBUS_EN: 1;
            __IO uint32_t Dummy2: 1;
            __IO uint32_t BIT_SOC_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_FLASH_1_EN: 1;
            __IO uint32_t BIT_SOC_FLASH_2_EN: 1;
            __IO uint32_t BIT_SOC_FLASH_3_EN: 1;
            __IO uint32_t Dummy3: 1;
            __IO uint32_t Dummy4: 2;
            __IO uint32_t DUMMY5: 1;
            __IO uint32_t BIT_SOC_UART1_EN: 1;
            __IO uint32_t BIT_SOC_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_SDH_EN: 1;
            __IO uint32_t BIT_SOC_USB_EN: 1;
            __IO uint32_t BIT_SOC_GTIMER_EN: 1;
            __IO uint32_t Dummy6: 1;
            __IO uint32_t BIT_SOC_SWR_SS_EN: 1;
            __IO uint32_t DUMMY1: 1;
            __IO uint32_t BIT_SOC_AAC_XTAL_EN: 1;
            __IO uint32_t BIT_SOC_CAP_EN: 1;
            __IO uint32_t Dummy7: 10;
        } BITS_210;
    } u_210;

    __IO uint32_t RSVD_0x214;

    union
    {
        __IO uint32_t SOC_PERI_FUNC0_EN;
        struct
        {
            __IO uint32_t BIT_PERI_UART0_EN: 1;
            __IO uint32_t BIT_PERI_UART2_EN: 1;
            __IO uint32_t BIT_PERI_AES_EN: 1;
            __IO uint32_t BIT_PERI_RNG_EN: 1;
            __IO uint32_t BIT_PERI_SIMC_EN: 1;
            __IO uint32_t BIT_PERI_LCD_EN: 1;
            __IO uint32_t RSVD: 2;
            __IO uint32_t BIT_PERI_SPI0_EN: 1;
            __IO uint32_t BIT_PERI_SPI1_EN: 1;
            __IO uint32_t BIT_PERI_IRRC_EN: 1;
            __IO uint32_t BIT_PERI_SPI2_EN: 1;
            __IO uint32_t RSVD1: 4;
            __IO uint32_t BIT_PERI_I2C0_EN: 1;
            __IO uint32_t BIT_PERI_I2C1_EN: 1;
            __IO uint32_t BIT_PERI_QDEC_EN: 1;
            __IO uint32_t BIT_PERI_KEYSCAN_EN: 1;
            __IO uint32_t BIT_PERI_I2C2_EN: 1;
            __IO uint32_t RSVD2: 1;
            __IO uint32_t BIT_PERI_PSRAM_EN: 1;
            __IO uint32_t RSVD3: 1;
            __IO uint32_t BIT_PERI_SPI2W_EN: 1;
            __IO uint32_t BIT_DSP_CORE_EN: 1;
            __IO uint32_t BIT_DSP_H2D_D2H: 1;
            __IO uint32_t BIT_DSP_MEM_EN: 1;
            __IO uint32_t BIT_ASRC_EN: 1;
            __IO uint32_t BIT_DSP_WDT_EN: 1;
            __IO uint32_t BIT_EFUSE_EN: 1;
            __IO uint32_t BIT_DATA_MEM_EN: 1;
        } BITS_218;
    } u_218;

    union
    {
        __IO uint32_t SOC_PERI_FUNC1_EN;
        struct
        {
            __IO uint32_t BIT_PERI_ADC_EN: 1;
            __IO uint32_t RSVD: 7;
            __IO uint32_t BIT_PERI_GPIO_EN: 1;
            __IO uint32_t BIT_PERI_GPIO1_EN: 1;
            __IO uint32_t RSVD1: 22;
        } BITS_21C;
    } u_21C;

    union
    {
        __IO uint32_t SOC_AUDIO_IF_EN;
        struct
        {
            __IO uint32_t r_PON_FEN_AUDIO: 1;
            __IO uint32_t r_PON_FEN_SPORT0: 1;
            __IO uint32_t r_PON_FEN_SPORT1: 1;
            __IO uint32_t Dummy: 1;
            __IO uint32_t r_CLK_EN_AUDIO: 1;
            __IO uint32_t r_CLK_EN_SPORT0: 1;
            __IO uint32_t r_CLK_EN_SPORT1: 1;
            __IO uint32_t Dummy1: 1;
            __IO uint32_t r_CLK_EN_SPORT_40M: 1;
            __IO uint32_t r_CLK_EN_SI: 1;
            __IO uint32_t r_PON_FEN_SPORT2: 1;
            __IO uint32_t Dummy2: 1;
            __IO uint32_t r_CLK_EN_SPORT2: 1;
            __IO uint32_t Dummy3: 19;
        } BITS_220;
    } u_220;

    union
    {
        __IO uint32_t SOC_AUDIO_CLK_CTRL_A;
        struct
        {
            __IO uint32_t r_SPORT0_PLL_CLK_SEL: 3;
            __IO uint32_t r_SPORT0_EXT_CODEC: 1;
            __IO uint32_t r_SPORT1_PLL_CLK_SEL: 3;
            __IO uint32_t r_CODEC_STANDALONE: 1;
            __IO uint32_t r_PLL_DIV0_SETTING: 8;
            __IO uint32_t r_PLL_DIV1_SETTING: 8;
            __IO uint32_t r_PLL_DIV2_SETTING: 8;
        } BITS_224;
    } u_224;

    union
    {
        __IO uint32_t SOC_AUDIO_CLK_CTRL_B;
        struct
        {
            __IO uint32_t Dummy: 3;
            __IO uint32_t r_SPORT0_MCLK_OUT: 1;
            __IO uint32_t r_SPORT1_MCLK_OUT: 1;
            __IO uint32_t r_SPORT2_MCLK_OUT: 1;
            __IO uint32_t Dummy1: 1;
            __IO uint32_t r_AUDIO_CLK_FROM_PLL: 1;
            __IO uint32_t r_SPORT1_EXT_CODEC: 1;
            __IO uint32_t Dummy2: 4;
            __IO uint32_t r_SPORT2_PLL_CLK_SEL: 3;
            __IO uint32_t Dummy3: 3;
            __IO uint32_t r_SPORT2_EXT_CODEC: 1;
            __IO uint32_t Dummy4: 12;
        } BITS_228;
    } u_228;

    __IO uint32_t RSVD_0x22C;

    union
    {
        __IO uint32_t PESOC_CLK_CTRL;
        struct
        {
            __IO uint32_t DUMMY: 1;
            __IO uint32_t BIT_CKE_CORDIC: 1;
            __IO uint32_t BIT_SOC_CKE_PLFM: 1;
            __IO uint32_t r_CKE_CTRLAP: 1;
            __IO uint32_t BIT_CKE_BUS_RAM_SLP: 1;
            __IO uint32_t BIT_CKE_BT_VEN: 1;
            __IO uint32_t BIT_SOC_ACTCK_VENDOR_REG_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_VENDOR_REG_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_TIMER_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_TIMER_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GPIO1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GPIO1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GPIO_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GPIO_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SDH_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SDH_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_USB_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_USB_EN: 1;
            __IO uint32_t DUMMY1: 2;
        } BITS_230;
    } u_230;

    union
    {
        __IO uint32_t PESOC_PERI_CLK_CTRL0;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_UART0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_HCI: 1;
            __IO uint32_t BIT_SOC_SLPCK_HCI: 1;
            __IO uint32_t BIT_CKE_MODEM: 1;
            __IO uint32_t BIT_CKE_CAL32K: 1;
            __IO uint32_t BIT_CKE_SWR_SS: 1;
            __IO uint32_t RSVD: 1;
            __IO uint32_t BIT_CKE_RNG: 1;
            __IO uint32_t BIT_CKE_PDCK: 1;
            __IO uint32_t BIT_CKE_AAC_XTAL: 1;
            __IO uint32_t BIT_CKE_CAP: 1;
            __IO uint32_t RSVD1: 4;
            __IO uint32_t BIT_SOC_ACTCK_SPI0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_IRRC: 1;
            __IO uint32_t BIT_SOC_SLPCK_IRRC: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI2_EN: 1;
            __IO uint32_t RSVD2: 4;
            __IO uint32_t BIT_SOC_ACTCK_FLASH3_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH3_EN: 1;
            __IO uint32_t RSVD3: 2;
        } BITS_234;
    } u_234;

    union
    {
        __IO uint32_t PESOC_PERI_CLK_CTRL1;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_I2C0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_I2C1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_QDEC_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_QDEC_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_KEYSCAN_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_KEYSCAN_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_AES_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_AES_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SIMC_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SIMC_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_I2C2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_DATA_MEM_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_DATA_MEM_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI2W_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI2W_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_LCD_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_LCD_EN: 1;
            __IO uint32_t BIT_SOC_ACTCKE_ASRC: 1;
            __IO uint32_t BIT_SOC_SLPCKE_ASRC: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_MEM: 1;
            __IO uint32_t BIT_SOC_SLPCKE_DSP_MEM: 1;
            __IO uint32_t BIT_SOC_ACTCK_ADC_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_ADC_EN: 1;
            __IO uint32_t BIT_SOC_ACTCKE_H2D_D2H: 1;
            __IO uint32_t BIT_SOC_SLPCKE_H2D_D2H: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP: 1;
            __IO uint32_t BIT_SOC_SLPCKE_DSP: 1;
            __IO uint32_t BIT_SOC_CKE_DSP_WDT: 1;
            __IO uint32_t BIT_SOC_CLK_EFUSE: 1;
        } BITS_238;
    } u_238;

    __IO uint32_t RSVD[2];//0x23C~0x240

    union
    {
        __IO uint32_t OFF_MEM_PWR_CRTL;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_BTBUS_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_BTBUS_EN: 1;
            __IO uint32_t Dummy: 30;
        } BITS_244;
    } u_244;

    union
    {
        __IO uint32_t  REG_0x248;                        /**< 0x248    */
        struct
        {
            __IO uint32_t RSVD: 8;
            __IO uint32_t r_swr_ss_div_sel: 3;
            __IO uint32_t RSVD1: 21;
        } BITS_248;
    } u_248;

    __IO uint32_t  RSVD1[2];                             /*!< 0x24C~0x250*/

    union
    {
        __IO uint32_t  REG_0x254;                         /**< 0x254    */
        struct
        {
            __IO uint32_t DSP_RUN_STALL: 1;
            __IO uint32_t DSP_STAT_VECTOR_SEL: 1;
            __IO uint32_t reg_bypass_pipe: 1;
            __IO uint32_t Dummy: 12;
            __IO uint32_t HW_ASRC_MCU_EN: 1;
            __IO uint32_t r_cpu_low_rate_valid_num1: 4;
            __IO uint32_t r_dsp_low_rate_valid_num1: 4;
            __IO uint32_t r_cpu_auto_slow_filter1_en: 1;
            __IO uint32_t r_dsp_auto_slow_filter1_en: 1;
            __IO uint32_t Dummy2: 4;
            __I  uint32_t km4_warm_rst_n_from_reg: 1;
            __IO uint32_t reg_trigger_reset_km4: 1;
        } BITS_254;
    } u_254;

    union
    {
        __IO uint32_t  AUTO_SW_PAR0_31_0;                         /**< 0x258    */
        struct
        {
            __IO uint32_t CORE0_TUNE_OCP_RES: 2;
            __IO uint32_t CORE0_TUNE_PWM_R3: 3;
            __IO uint32_t CORE0_TUNE_PWM_R2: 3;
            __IO uint32_t CORE0_TUNE_PWM_R1: 3;
            __IO uint32_t CORE0_TUNE_PWM_C3: 3;
            __IO uint32_t CORE0_TUNE_PWM_C2: 3;
            __IO uint32_t CORE0_TUNE_PWM_C1: 3;
            __IO uint32_t CORE0_BYPASS_PWM_BYPASS_RoughSS: 1;
            __IO uint32_t CORE0_BYPASS_PWM_TUNE_RoughSS: 2;
            __IO uint32_t CORE0_BYPASS_PWM_TUNE_VCL: 3;
            __IO uint32_t CORE0_BYPASS_PWM_TUNE_VCH: 3;
            __IO uint32_t CORE0_X4_PWM_COMP_IB: 1;
            __IO uint32_t CORE0_X4_POW_PWM_CLP: 1;
            __IO uint32_t CORE0_X4_TUNE_VDIV_Bit0: 1;
        } BITS_258;
    } u_258;

    union
    {
        __IO uint32_t  AUTO_SW_PAR0_63_32;                         /**< 0x25C    */
        struct
        {
            __IO uint32_t CORE0_X4_TUNE_VDIV_Bit7_Bit1: 7;
            __IO uint32_t CORE0_BYPASS_PWM_TUNE_POS_VREFPFM: 8;
            __IO uint32_t CORE0_BYPASS_PWM_TUNE_POS_VREFOCP: 3;
            __IO uint32_t CORE0_FPWM: 1;
            __IO uint32_t CORE0_POW_PFM: 1;
            __IO uint32_t CORE0_POW_PWM: 1;
            __IO uint32_t CORE0_POW_VDIV: 1;
            __IO uint32_t CORE0_XTAL_OV_RATIO: 2;
            __IO uint32_t CORE0_XTAL_OV_UNIT: 3;
            __IO uint32_t CORE0_XTAL_OV_MODE: 3;
            __IO uint32_t CORE0_EN_POWERMOS_DR8X: 1;
            __IO uint32_t CORE0_SEL_OCP_TABLE: 1;
        } BITS_25C;
    } u_25C;


    union
    {
        __IO uint32_t  AUTO_SW_PAR4_31_0;                         /**< 0x260    */
        struct
        {
            __IO uint32_t CORE4_TUNE_OCP_RES: 2;
            __IO uint32_t CORE4_TUNE_PWM_R3: 3;
            __IO uint32_t CORE4_TUNE_PWM_R2: 3;
            __IO uint32_t CORE4_TUNE_PWM_R1: 3;
            __IO uint32_t CORE4_TUNE_PWM_C3: 3;
            __IO uint32_t CORE4_TUNE_PWM_C2: 3;
            __IO uint32_t CORE4_TUNE_PWM_C1: 3;
            __IO uint32_t CORE4_BYPASS_PWM_BYPASS_RoughSS: 1;
            __IO uint32_t CORE4_BYPASS_PWM_TUNE_RoughSS: 2;
            __IO uint32_t CORE4_BYPASS_PWM_TUNE_VCL: 3;
            __IO uint32_t CORE4_BYPASS_PWM_TUNE_VCH: 3;
            __IO uint32_t CORE4_X4_PWM_COMP_IB: 1;
            __IO uint32_t CORE4_X4_POW_PWM_CLP: 1;
            __IO uint32_t CORE4_X4_TUNE_VDIV_Bit0: 1;
        } BITS_260;
    } u_260;

    union
    {
        __IO uint32_t  AUTO_SW_PAR4_63_32;                         /**< 0x264    */
        struct
        {
            __IO uint32_t CORE4_X4_TUNE_VDIV_Bit7_Bit1: 7;
            __IO uint32_t CORE4_BYPASS_PWM_TUNE_POS_VREFPFM: 8;
            __IO uint32_t CORE4_BYPASS_PWM_TUNE_POS_VREFOCP: 3;
            __IO uint32_t CORE4_FPWM: 1;
            __IO uint32_t CORE4_POW_PFM: 1;
            __IO uint32_t CORE4_POW_PWM: 1;
            __IO uint32_t CORE4_POW_VDIV: 1;
            __IO uint32_t CORE4_XTAL_OV_RATIO: 2;
            __IO uint32_t CORE4_XTAL_OV_UNIT: 3;
            __IO uint32_t CORE4_XTAL_OV_MODE: 3;
            __IO uint32_t CORE4_EN_POWERMOS_DR8X: 1;
            __IO uint32_t CORE4_SEL_OCP_TABLE: 1;
        } BITS_264;
    } u_264;

    union
    {
        __IO uint32_t  AUTO_SW_PAR5_31_0;                         /**< 0x268    */
        struct
        {
            __IO uint32_t CORE5_TUNE_OCP_RES: 2;
            __IO uint32_t CORE5_TUNE_PWM_R3: 3;
            __IO uint32_t CORE5_TUNE_PWM_R2: 3;
            __IO uint32_t CORE5_TUNE_PWM_R1: 3;
            __IO uint32_t CORE5_TUNE_PWM_C3: 3;
            __IO uint32_t CORE5_TUNE_PWM_C2: 3;
            __IO uint32_t CORE5_TUNE_PWM_C1: 3;
            __IO uint32_t CORE5_BYPASS_PWM_BYPASS_RoughSS: 1;
            __IO uint32_t CORE5_BYPASS_PWM_TUNE_RoughSS: 2;
            __IO uint32_t CORE5_BYPASS_PWM_TUNE_VCL: 3;
            __IO uint32_t CORE5_BYPASS_PWM_TUNE_VCH: 3;
            __IO uint32_t CORE5_X4_PWM_COMP_IB: 1;
            __IO uint32_t CORE5_X4_POW_PWM_CLP: 1;
            __IO uint32_t CORE5_X4_TUNE_VDIV_Bit0: 1;
        } BITS_268;
    } u_268;

    union
    {
        __IO uint32_t  AUTO_SW_PAR5_63_32;                         /**< 0x26C    */
        struct
        {
            __IO uint32_t CORE5_X4_TUNE_VDIV_Bit7_Bit1: 7;
            __IO uint32_t CORE5_BYPASS_PWM_TUNE_POS_VREFPFM: 8;
            __IO uint32_t CORE5_BYPASS_PWM_TUNE_POS_VREFOCP: 3;
            __IO uint32_t CORE5_FPWM: 1;
            __IO uint32_t CORE5_POW_PFM: 1;
            __IO uint32_t CORE5_POW_PWM: 1;
            __IO uint32_t CORE5_POW_VDIV: 1;
            __IO uint32_t CORE5_XTAL_OV_RATIO: 2;
            __IO uint32_t CORE5_XTAL_OV_UNIT: 3;
            __IO uint32_t CORE5_XTAL_OV_MODE: 3;
            __IO uint32_t CORE5_EN_POWERMOS_DR8X: 1;
            __IO uint32_t CORE5_SEL_OCP_TABLE: 1;
        } BITS_26C;
    } u_26C;

    union
    {
        __IO uint32_t  AUTO_SW_PAR4_79_64_AUTO_SW_PAR0_79_64;                         /**< 0x270    */
        struct
        {
            __IO uint32_t CORE0_EN_HVD17_LOWIQ: 1;
            __IO uint32_t CORE0_TUNE_HVD17_IB: 4;
            __IO uint32_t CORE0_X4_PFM_COMP_IB: 1;
            __IO uint32_t CORE0_TUNE_PFM_VREFOCPPFM: 3;
            __IO uint32_t CORE0_TUNE_SAW_ICLK: 6;
            __IO uint32_t DUMMY: 1;
            __IO uint32_t CORE4_EN_HVD17_LOWIQ: 1;
            __IO uint32_t CORE4_TUNE_HVD17_IB: 4;
            __IO uint32_t CORE4_X4_PFM_COMP_IB: 1;
            __IO uint32_t CORE4_TUNE_PFM_VREFOCPPFM: 3;
            __IO uint32_t CORE4_TUNE_SAW_ICLK: 6;
            __IO uint32_t DUMMY1: 1;
        } BITS_270;
    } u_270;


    union
    {
        __IO uint32_t  REG_DSS_CTRL;                    /**< 0x274    */
        struct
        {
            __IO uint32_t r_dss_data_in: 20;
            __IO uint32_t r_dss_ro_sel: 3;
            __IO uint32_t r_dss_wire_sel: 1;
            __IO uint32_t r_dss_clk_en: 1;
            __IO uint32_t r_dss_speed_en: 1;
            __IO uint32_t r_FEN_DSS: 1;
            __IO uint32_t RSVD: 5;
        } BITS_274;
    } u_274;

    union
    {
        __IO uint32_t  REG_BEST_DSS_RD;               /**< 0x278    */
        struct
        {
            __IO uint32_t bset_dss_count_out: 20;
            __IO uint32_t bset_dss_wsort_go: 1;
            __IO uint32_t bset_dss_ready: 1;
            __IO uint32_t RSVD: 10;
        } BITS_278;
    } u_278;

    __IO uint32_t RSVD_0x27C;

    union
    {
        __IO uint32_t REG_GPIO_A0_3;
        struct
        {
            __IO uint32_t PMUX_GPIO_ADC_0: 8;
            __IO uint32_t PMUX_GPIO_ADC_1: 8;
            __IO uint32_t PMUX_GPIO_ADC_2: 8;
            __IO uint32_t PMUX_GPIO_ADC_3: 8;
        } BITS_280;
    } u_280;

    union
    {
        __IO uint32_t REG_GPIO_A4_7;
        struct
        {
            __IO uint32_t PMUX_GPIO_P3_2: 8;
            __IO uint32_t PMUX_GPIO_P3_3: 8;
            __IO uint32_t PMUX_GPIO_P3_4: 8;
            __IO uint32_t PMUX_GPIO_P3_5: 8;
        } BITS_284;
    } u_284;

    union
    {
        __IO uint32_t REG_GPIO_B0_3;
        struct
        {
            __IO uint32_t PMUX_GPIO_P1_0: 8;
            __IO uint32_t PMUX_GPIO_P1_1: 8;
            __IO uint32_t PMUX_GPIO_P1_2: 8;
            __IO uint32_t PMUX_GPIO_P1_3: 8;
        } BITS_288;
    } u_288;

    union
    {
        __IO uint32_t REG_GPIO_B4_7;
        struct
        {
            __IO uint32_t PMUX_GPIO_P1_4: 8;
            __IO uint32_t PMUX_GPIO_P1_5: 8;
            __IO uint32_t PMUX_GPIO_P1_6: 8;
            __IO uint32_t PMUX_GPIO_P1_7: 8;
        } BITS_28C;
    } u_28C;

    union
    {
        __IO uint32_t REG_GPIO_C0_3;
        struct
        {
            __IO uint32_t PMUX_GPIO_P2_0: 8;
            __IO uint32_t PMUX_GPIO_P2_1: 8;
            __IO uint32_t PMUX_GPIO_P2_2: 8;
            __IO uint32_t PMUX_GPIO_P2_3: 8;
        } BITS_290;
    } u_290;

    union
    {
        __IO uint32_t REG_GPIO_C4_7;
        struct
        {
            __IO uint32_t PMUX_GPIO_P2_4: 8;
            __IO uint32_t PMUX_GPIO_P2_5: 8;
            __IO uint32_t PMUX_GPIO_P2_6: 8;
            __IO uint32_t PMUX_GPIO_P2_7: 8;
        } BITS_294;
    } u_294;

    union
    {
        __IO uint32_t REG_GPIO_D0_3;
        struct
        {
            __IO uint32_t PMUX_GPIO_P3_0: 8;
            __IO uint32_t PMUX_GPIO_P3_1: 8;
            __IO uint32_t PMUX_GPIO_H_0: 8;
            __IO uint32_t PMUX_GPIO_H_1: 8;
        } BITS_298;
    } u_298;

    union
    {
        __IO uint32_t REG_GPIO_D4_7;
        struct
        {
            __IO uint32_t PMUX_GPIO_H_2: 8;
            __IO uint32_t PMUX_GPIO_H_3: 8;
            __IO uint32_t PMUX_GPIO_H_4: 8;
            __IO uint32_t PMUX_GPIO_H_5: 8;
        } BITS_29C;
    } u_29C;

    union
    {
        __IO uint32_t REG_GPIO_E0_3;
        struct
        {
            __IO uint32_t PMUX_GPIO_H_6: 8;
            __IO uint32_t PMUX_GPIO_H_7: 8;
            __IO uint32_t PMUX_GPIO_H_8: 8;
            __IO uint32_t PMUX_GPIO_H_9: 8;
        } BITS_2A0;
    } u_2A0;

    union
    {
        __IO uint32_t REG_GPIO_E4_5;
        struct
        {
            __IO uint32_t PMUX_GPIO_H_10: 8;
            __IO uint32_t PMUX_GPIO_H_11: 8;
            __IO uint32_t PMUX_GPIO_H_12: 8;
            __IO uint32_t DUMMY: 8;
        } BITS_2A4;
    } u_2A4;

    union
    {
        __IO uint32_t  REG_TEST_MODE;                 /**< 0x2A8    */
        struct
        {
            __IO uint32_t PMUX_TEST_MODE: 4;
            __IO uint32_t RSVD: 3;
            __IO uint32_t PMUX_TEST_MODE_EN: 1;
            __IO uint32_t PMUX_DBG_INF_EN: 1;
            __IO uint32_t PMUX_DBG_FLASH_INF_EN: 1;
            __IO uint32_t PMUX_SPI_DMA_REQ_EN: 1;
            __IO uint32_t PMUX_OPI_EN: 1;
            __IO uint32_t PMUX_LCD_EN: 1;
            __IO uint32_t PMUX_LCD_VSYNC_DIS: 1;
            __IO uint32_t PMUX_LCD_RD_DIS: 1;
            __IO uint32_t PMUX_LCD_VSYNC_IO_SEL: 1;
            __IO uint32_t PMUX_DBG_MODE_SEL: 4;
            __IO uint32_t RSVD3: 2;
            __IO uint32_t r_FEN_PSRAM: 1;
            __IO uint32_t r_dbg_cpu_dsp_clk_en: 1;
            __IO uint32_t SPIC_MASTER_EN: 1;
            __IO uint32_t SPIC1_MASTER_EN: 1;
            __IO uint32_t SPIC2_MASTER_EN: 1;
            __IO uint32_t PMUX_FLASH_EN: 1;
            __IO uint32_t PMUX_DIG_SMUX_EN: 1;
            __IO uint32_t SPIC3_MASTER_EN: 1;
            __IO uint32_t bypass_dcd_dbnc: 1;
            __IO uint32_t bypass_non_std_det: 1;
        } BITS_2A8;
    } u_2A8;

    union
    {
        __IO uint32_t  REG_TEST_MODE;                 /**< 0x2AC    */
        struct
        {
            __IO uint32_t r_PMUX_UART0_1_W_CTRL: 1;
            __IO uint32_t r_PMUX_UARTLOG_1_W_CTRL: 1;
            __IO uint32_t r_PMUX_UARTLOG1_1_W_CTRL: 1;
            __IO uint32_t r_PMUX_UART0_1_W_EN: 1;
            __IO uint32_t r_PMUX_UARTLOG_1_W_EN: 1;
            __IO uint32_t r_PMUX_UARTLOG1_1_W_EN: 1;
            __IO uint32_t DUMMY: 2;
            __IO uint32_t r_SPIC0_PULL_SEL_SIO0_PULL_CTRL: 1;
            __IO uint32_t r_SPIC0_PULL_SEL_SIO1_PULL_CTRL: 1;
            __IO uint32_t r_SPIC0_PULL_SEL_SIO2_PULL_CTRL: 1;
            __IO uint32_t r_SPIC0_PULL_SEL_SIO3_PULL_CTRL: 1;
            __IO uint32_t r_SPIC1_PULL_SEL_SIO0_PULL_CTRL: 1;
            __IO uint32_t r_SPIC1_PULL_SEL_SIO1_PULL_CTRL: 1;
            __IO uint32_t r_SPIC1_PULL_SEL_SIO2_PULL_CTRL: 1;
            __IO uint32_t r_SPIC1_PULL_SEL_SIO3_PULL_CTRL: 1;
            __IO uint32_t r_SPIC2_PULL_SEL_SIO0_PULL_CTRL: 1;
            __IO uint32_t r_SPIC2_PULL_SEL_SIO1_PULL_CTRL: 1;
            __IO uint32_t r_SPIC2_PULL_SEL_SIO2_PULL_CTRL: 1;
            __IO uint32_t r_SPIC2_PULL_SEL_SIO3_PULL_CTRL: 1;
            __IO uint32_t r_SPIC3_PULL_SEL_SIO0_PULL_CTRL: 1;
            __IO uint32_t r_SPIC3_PULL_SEL_SIO1_PULL_CTRL: 1;
            __IO uint32_t r_SPIC3_PULL_SEL_SIO2_PULL_CTRL: 1;
            __IO uint32_t r_SPIC3_PULL_SEL_SIO3_PULL_CTRL: 1;
            __IO uint32_t DUMMY1: 8;
        } BITS_2AC;
    } u_2AC;


    union
    {
        __IO uint32_t  REG_0x2B0;                 /**< 0x2B0    */
        struct
        {
            __IO uint32_t SPI_FLASH_SEL: 1;
            __IO uint32_t DUMMY: 31;
        } BITS_2B0;
    } u_2B0;

    union
    {
        __IO uint32_t  REG_0x2B4;                 /**< 0x2B4    */
        struct
        {
            __IO uint32_t PMUX_SDIO_EN: 1;
            __IO uint32_t PMUX_SDIO_SEL: 1;
            __IO uint32_t DUMMY: 2;
            __IO uint32_t PMUX_SDIO_PIN_EN: 8;
            __IO uint32_t DUMMY1: 20;
        } BITS_2B4;
    } u_2B4;

    union
    {
        __IO uint32_t  REG_SPI_OPI_PHY_Ctrl;      /**< 0x2B8    */
        struct
        {
            __IO uint32_t SPIC_OPI_EN: 1;
            __IO uint32_t SPIC_QPI_EN: 1;
            __IO uint32_t r_clko_sel: 1;
            __IO uint32_t DUMMY: 1;
            __IO uint32_t spi_dqs_dly: 8;
            __IO uint32_t cko_dly_sel: 8;
            __IO uint32_t fetch_sclk_phase: 1;
            __IO uint32_t fetch_sclk_phase_2: 1;
            __IO uint32_t ds_sel: 1;
            __IO uint32_t ds_dtr: 1;
            __IO uint32_t data_phase_sel: 1;
            __IO uint32_t r_pos_data_order: 1;
            __IO uint32_t r_spi_clk_sel: 3;
            __IO uint32_t DUMMY1: 3;
        } BITS_2B8;
    } u_2B8;

    __IO uint32_t RSVD_0x2BC;

    union
    {
        __IO uint32_t  AUTO_SW_PAR6_31_0;      /**< 0x2C0    */
        struct
        {
            __IO uint32_t CORE6_TUNE_OCP_RES: 2;
            __IO uint32_t CORE6_TUNE_PWM_R3: 3;
            __IO uint32_t CORE6_TUNE_PWM_R2: 3;
            __IO uint32_t CORE6_TUNE_PWM_R1: 3;
            __IO uint32_t CORE6_TUNE_PWM_C3: 3;
            __IO uint32_t CORE6_TUNE_PWM_C2: 3;
            __IO uint32_t CORE6_TUNE_PWM_C1: 3;
            __IO uint32_t CORE6_BYPASS_PWM_BYPASS_RoughSS: 1;
            __IO uint32_t CORE6_BYPASS_PWM_TUNE_RoughSS: 2;
            __IO uint32_t CORE6_BYPASS_PWM_TUNE_VCL: 3;
            __IO uint32_t CORE6_BYPASS_PWM_TUNE_VCH: 3;
            __IO uint32_t CORE6_X4_PWM_COMP_IB: 1;
            __IO uint32_t CORE6_X4_POW_PWM_CLP: 1;
            __IO uint32_t CORE0_X4_TUNE_VDIV_Bit0: 1;
        } BITS_2C0;
    } u_2C0;

    union
    {
        __IO uint32_t  AUTO_SW_PAR6_63_32;                         /**< 0x2C4    */
        struct
        {
            __IO uint32_t CORE6_X4_TUNE_VDIV_Bit7_Bit1: 7;
            __IO uint32_t CORE6_BYPASS_PWM_TUNE_POS_VREFPFM: 8;
            __IO uint32_t CORE6_BYPASS_PWM_TUNE_POS_VREFOCP: 3;
            __IO uint32_t CORE6_FPWM: 1;
            __IO uint32_t CORE6_POW_PFM: 1;
            __IO uint32_t CORE6_POW_PWM: 1;
            __IO uint32_t CORE6_POW_VDIV: 1;
            __IO uint32_t CORE6_XTAL_OV_RATIO: 2;
            __IO uint32_t CORE6_XTAL_OV_UNIT: 3;
            __IO uint32_t CORE6_XTAL_OV_MODE: 3;
            __IO uint32_t CORE6_EN_POWERMOS_DR8X: 1;
            __IO uint32_t CORE6_SEL_OCP_TABLE: 1;
        } BITS_2C4;
    } u_2C4;

    union
    {
        __IO uint32_t  AUTO_SW_PAR7_31_0;      /**< 0x2C8    */
        struct
        {
            __IO uint32_t CORE7_TUNE_OCP_RES: 2;
            __IO uint32_t CORE7_TUNE_PWM_R3: 3;
            __IO uint32_t CORE7_TUNE_PWM_R2: 3;
            __IO uint32_t CORE7_TUNE_PWM_R1: 3;
            __IO uint32_t CORE7_TUNE_PWM_C3: 3;
            __IO uint32_t CORE7_TUNE_PWM_C2: 3;
            __IO uint32_t CORE7_TUNE_PWM_C1: 3;
            __IO uint32_t CORE7_BYPASS_PWM_BYPASS_RoughSS: 1;
            __IO uint32_t CORE7_BYPASS_PWM_TUNE_RoughSS: 2;
            __IO uint32_t CORE7_BYPASS_PWM_TUNE_VCL: 3;
            __IO uint32_t CORE7_BYPASS_PWM_TUNE_VCH: 3;
            __IO uint32_t CORE7_X4_PWM_COMP_IB: 1;
            __IO uint32_t CORE7_X4_POW_PWM_CLP: 1;
            __IO uint32_t CORE7_X4_TUNE_VDIV_Bit0: 1;
        } BITS_2C8;
    } u_2C8;

    union
    {
        __IO uint32_t  AUTO_SW_PAR7_63_32;                         /**< 0x2CC    */
        struct
        {
            __IO uint32_t CORE7_X4_TUNE_VDIV_Bit7_Bit1: 7;
            __IO uint32_t CORE7_BYPASS_PWM_TUNE_POS_VREFPFM: 8;
            __IO uint32_t CORE7_BYPASS_PWM_TUNE_POS_VREFOCP: 3;
            __IO uint32_t CORE7_FPWM: 1;
            __IO uint32_t CORE7_POW_PFM: 1;
            __IO uint32_t CORE7_POW_PWM: 1;
            __IO uint32_t CORE7_POW_VDIV: 1;
            __IO uint32_t CORE7_XTAL_OV_RATIO: 2;
            __IO uint32_t CORE7_XTAL_OV_UNIT: 3;
            __IO uint32_t CORE7_XTAL_OV_MODE: 3;
            __IO uint32_t CORE7_EN_POWERMOS_DR8X: 1;
            __IO uint32_t CORE7_SEL_OCP_TABLE: 1;
        } BITS_2CC;
    } u_2CC;

    union
    {
        __IO uint32_t  REG_0x2D0;                 /**< 0x2D0    */
        struct
        {
            __IO uint32_t flash1_div_sel: 4;
            __IO uint32_t flash1_div_en: 1;
            __IO uint32_t FLASH1_CLK_SRC_EN: 1;
            __IO uint32_t flash1_clk_src_sel_1: 1;
            __IO uint32_t flash1_clk_src_sel_0: 1;
            __IO uint32_t flash1_mux_1_clk_cg_en: 1;
            __IO uint32_t flash2_div_sel: 4;
            __IO uint32_t flash2_div_en: 1;
            __IO uint32_t FLASH2_CLK_SRC_EN: 1;
            __IO uint32_t flash2_clk_src_sel_1: 1;
            __IO uint32_t flash2_clk_src_sel_0: 1;
            __IO uint32_t flash2_mux_1_clk_cg_en: 1;
            __IO uint32_t flash3_div_sel: 4;
            __IO uint32_t flash3_div_en: 1;
            __IO uint32_t FLASH3_CLK_SRC_EN: 1;
            __IO uint32_t flash3_clk_src_sel_1: 1;
            __IO uint32_t flash3_clk_src_sel_0: 1;
            __IO uint32_t flash3_mux_1_clk_cg_en: 1;
            __IO uint32_t r_flash1_clk_src_pll_sel: 1;
            __IO uint32_t r_flash2_clk_src_pll_sel: 1;
            __IO uint32_t r_flash3_clk_src_pll_sel: 1;
            __IO uint32_t RSVD: 2;
        } BITS_2D0;
    } u_2D0;

    union
    {
        __IO uint32_t  REG_0x2D4;                 /**< 0x2D4    */
        struct
        {
            __IO uint32_t psram_div_sel: 4;
            __IO uint32_t psram_div_en: 1;
            __IO uint32_t PSRAM_CLK_SRC_EN: 1;
            __IO uint32_t psram_clk_src_sel_1: 1;
            __IO uint32_t psram_clk_src_sel_0: 1;
            __IO uint32_t psram_mux_1_clk_cg_en: 1;
            __IO uint32_t psram_pinmux_sel: 1;
            __IO uint32_t psram_clk_pll_src_sel: 1;
            __IO uint32_t RSVD: 21;
        } BITS_2D4;
    } u_2D4;

    __IO uint32_t  RSVD_0x2D8;

    union
    {
        __IO uint32_t  AUTO_SW_PAR6_79_64_AUTO_SW_PAR5_79_64;  /**< 0x2DC    */
        struct
        {
            __IO uint32_t CORE5_EN_HVD17_LOWIQ: 1;
            __IO uint32_t CORE5_TUNE_HVD17_IB: 4;
            __IO uint32_t CORE5_X4_PFM_COMP_IB: 1;
            __IO uint32_t CORE5_TUNE_PFM_VREFOCPPFM: 3;
            __IO uint32_t CORE5_TUNE_SAW_ICLK: 6;
            __IO uint32_t DUMMY: 1;
            __IO uint32_t CORE6_EN_HVD17_LOWIQ: 1;
            __IO uint32_t CORE6_TUNE_HVD17_IB: 4;
            __IO uint32_t CORE6_X4_PFM_COMP_IB: 1;
            __IO uint32_t CORE6_TUNE_PFM_VREFOCPPFM: 3;
            __IO uint32_t CORE6_TUNE_SAW_ICLK: 6;
            __IO uint32_t DUMMY1: 1;
        } BITS_2DC;
    } u_2DC;

    union
    {
        __IO uint32_t SOC_CHARGER_DET_A;              /**< 0x2E0    */
        struct
        {
            __IO uint32_t det_enable: 1;
            __IO uint32_t vbus_det: 1;
            __IO uint32_t skip_sec_detection: 1;
            __IO uint32_t Dummy: 1;
            __IO uint32_t REG_LDO_USB: 4;
            __IO uint32_t sdp_op5a: 1;
            __IO uint32_t dcp_cdp_1p5a: 1;
            __IO uint32_t cdp_det: 1;
            __IO uint32_t dcp_det: 1;
            __IO uint32_t others_op5a: 1;
            __IO uint32_t apple_1p0a: 1;
            __IO uint32_t apple_2p1a: 1;
            __IO uint32_t apple_2p4a: 1;
            __IO uint32_t det_is_done: 1;
            __IO uint32_t note_2p0a: 1;
            __IO uint32_t sony_charger: 1;
            __IO uint32_t Dummy1: 13;
        } BITS_2E0;
    } u_2E0;

    union
    {
        __IO uint32_t SOC_CHARGER_DET_B;             /**< 0x2E4    */
        struct
        {
            __IO uint32_t fw_write_bus: 8;
            __IO uint32_t fw_ctrl_mode: 1;
            __IO uint32_t Dummy: 7;
            __IO uint32_t fw_read_bus: 10;
            __IO uint32_t Dummy1: 6;
        } BITS_2E4;
    } u_2E4;

    __IO uint32_t RSVD2[6];                          /**< 0x2E8 ~ 0x2FC    */

    union
    {
        __IO uint32_t  PON_PERI_DLYSEL_CTRL;             /**< 0x300    */
        struct
        {
            __IO uint32_t PON_PERI_DLYSEL_SPIM: 8;
            __IO uint32_t PON_PERI_DLYSEL_SPIM1: 8;
            __IO uint32_t PON_PERI_DLYSEL_SPIM2: 8;
            __IO uint32_t PON_PERI_DLYSEL_SPIM3: 8;
        } BITS_300;
    } u_300;

    __IO uint32_t  RSVD_0x304;                            /*!< 0x304 */

    union
    {
        __IO uint32_t  REG_0x308;                        /**< 0x308    */
        struct
        {
            __IO uint32_t PON_SPI0_MST: 1;
            __IO uint32_t PON_SPI0_BRIDGE_EN: 1;
            __IO uint32_t PON_SPI1_BRIDGE_EN: 1;
            __IO uint32_t PON_SPI2_BRIDGE_EN: 1;
            __IO uint32_t PON_SPI0_H2S_BRG_EN: 1;
            __IO uint32_t RSVD: 27;
        } BITS_308;
    } u_308;

    __IO uint32_t  RSVD3[9];                           /*!< 0x30C ~ 0x32C */

    union
    {
        __IO uint32_t REG_0x330;
        struct
        {
            __IO uint32_t rst_n_aac: 1;
            __IO uint32_t offset_plus: 1;
            __IO uint32_t XAAC_GM_offset: 6;
            __IO uint32_t GM_STEP: 1;
            __IO uint32_t GM_INIT: 6;
            __IO uint32_t XTAL_CLK_SET: 3;
            __IO uint32_t GM_STUP: 6;
            __IO uint32_t GM_MANUAL: 6;
            __IO uint32_t r_EN_XTAL_AAC_DIGI: 1;
            __IO uint32_t r_EN_XTAL_AAC_TRIG: 1;
        } BITS_330;
    } u_330;

    union
    {
        __IO uint32_t AAC_CTRL_1;
        struct
        {
            __IO uint32_t XAAC_BUSY: 1;
            __IO uint32_t XAAC_READY: 1;
            __IO uint32_t XTAL_GM_OUT: 6;
            __IO uint32_t xaac_curr_state: 4;
            __IO uint32_t EN_XTAL_AAC_GM: 1;
            __IO uint32_t EN_XTAL_AAC_PKDET: 1;
            __IO uint32_t XTAL_PKDET_OUT: 1;
            __IO uint32_t Dummy: 17;
        } BITS_334;
    } u_334;

    union
    {
        __IO uint32_t REG_0x338;
        struct
        {
            __IO uint32_t disable_pll_pre_gating: 1;
            __IO uint32_t Dummy: 15;
            __IO uint32_t XTAL32K_OK: 1;
            __IO uint32_t OSC32K_OK: 1;
            __IO uint32_t XTAL_CTRL_DEBUG_OUT_4_0: 5;
            __IO uint32_t PLL_CKO2_READY: 1;
            __IO uint32_t BT_PLL_READY: 1;
            __IO uint32_t XTAL_OK: 1;
            __IO uint32_t XTAL_CTRL_DEBUG_OUT_7_5: 3;
            __IO uint32_t XTAL_MODE_O: 3;
        } BITS_338;
    } u_338;

    union
    {
        __IO uint32_t  XTAL_PDCK;                    /**< 0x33C    */
        struct
        {
            __IO uint32_t resetn: 1;
            __IO uint32_t EN_XTAL_PDCK_DIGI: 1;
            __IO uint32_t PDCK_SEARCH_MODE: 1;
            __IO uint32_t PDCK_WAIT_CYC_1_0: 2;
            __IO uint32_t VREF_MANUAL_4_0: 5;
            __IO uint32_t VREF_INIT_4_0: 5;
            __IO uint32_t XTAL_PDCK_UNIT_1_0: 2;
            __IO uint32_t XPDCK_VREF_SEL_4_0: 5;
            __IO uint32_t PDCK_LPOW: 1;
            __IO uint32_t reserved: 5;
            __IO uint32_t pdck_state_3_0: 4;
        } BITS_33C;
    } u_33C;

    union
    {
        __IO uint32_t  REG_0x340;                   /**< 0x340    */
        struct
        {
            __IO uint32_t iso_int_out_en: 1;
            __IO uint32_t RSVD: 31;
        } BITS_340;
    } u_340;

    union
    {
        __IO uint32_t  REG_0x344;                   /**< 0x344    */
        struct
        {
            __IO uint32_t GPIO_DBNC_CTRL: 13;
            __IO uint32_t RSVD: 3;
            __IO uint32_t GPIO1_DBNC_CTRL: 13;
            __IO uint32_t RSVD1: 3;
        } BITS_344;
    } u_344;

    union
    {
        __IO uint32_t  REG_0x348;                   /**< 0x348    */
        struct
        {
            __IO uint32_t r_timer_div_sel: 3;
            __IO uint32_t r_timer_div_en: 1;
            __IO uint32_t r_timer_cg_en: 1;
            __IO uint32_t r_timer_clk_src_sel_0: 1;
            __IO uint32_t r_timer_clk_src_sel_1: 1;
            __IO uint32_t r_timer_mux_1_clk_cg_en: 1;
            __IO uint32_t timer_clk_src_pll_sel: 1;
            __IO uint32_t RSVD: 23;
        } BITS_348;
    } u_348;

    __IO uint32_t  RSVD7[4];                           /*!< 0x34C~0x358*/

    union
    {
        __IO uint32_t  REG_PERI_GTIMER_CLK_SRC1;        /**< 0x35C    */
        struct
        {
            __IO uint32_t BIT_PERI_GT5_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT6_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT7_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_UART0_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_UART1_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_UART2_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_I2C0_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_I2C1_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_SPI0_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_SPI1_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_SPI2_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_I2C2_CLK_DIV: 2;
            __IO uint32_t r_spi0_clk_src_pll_sel: 1;
            __IO uint32_t r_irrc_clk_src_pll_sel: 1;
            __IO uint32_t r_irrc_clk_sel: 1;
            __IO uint32_t RSVD1: 1;
        } BITS_35C;
    } u_35C;

    union
    {
        __IO uint32_t  REG_PERI_GTIMER_CLK_SRC0;        /**< 0x360    */
        struct
        {
            __IO uint32_t RSVD: 8;
            __IO uint32_t BIT_TIMER_CLK_32K_EN: 1;
            __IO uint32_t BIT_TIMER_CLK_f40M_EN: 1;
            __IO uint32_t timer_apb_clk_disable: 1;
            __IO uint32_t r_timer_div1_en: 1;
            __IO uint32_t r_clk_timer_f1m_en: 1;
            __IO uint32_t r_timer38_div_en: 1;
            __IO uint32_t RSVD1: 2;
            __IO uint32_t BIT_PERI_GT0_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT1_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT2_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT3_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT4_CLK_DIV: 3;
            __IO uint32_t RSVD2: 1;
        } BITS_360;
    } u_360;

    union
    {
        __IO uint32_t  REG_PERI_PWM2_DZONE_CTRL;        /**< 0x364    */
        struct
        {
            __IO uint32_t TIMER_PWM0_CTRL: 16;
            __IO uint32_t RSVD: 16;
        } BITS_364;
    } u_364;

    union
    {
        __IO uint32_t  REG_PERI_PWM3_DZONE_CTRL;        /**< 0x368    */
        struct
        {
            __IO uint32_t TIMER_PWM0_CTRL: 16;
            __IO uint32_t RSVD: 16;
        } BITS_368;
    } u_368;

    __IO uint32_t  RSVD8[8];                           /*!< 0x36C~0x388*/

    union
    {
        __IO uint32_t  REG_0x38C;                       /**< 0x38C    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_2: 32;
        } BITS_38C;
    } u_38C;

    union
    {
        __IO uint32_t  REG_0x390;                       /**< 0x390    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_3: 32;
        } BITS_390;
    } u_390;

    union
    {
        __IO uint32_t  REG_0x394;                       /**< 0x394    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_4: 32;
        } BITS_394;
    } u_394;

    union
    {
        __IO uint32_t  REG_0x398;                       /**< 0x398    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_5: 32;
        } BITS_398;
    } u_398;

    union
    {
        __IO uint32_t  REG_0x39C;                       /**< 0x39C    */
        struct
        {
            __IO uint32_t SWR_SS_CONFIG: 16;
            __IO uint32_t RSVD: 16;
        } BITS_39C;
    } u_39C;

    union
    {
        __IO uint32_t  REG_0x3A0;                       /**< 0x3A0    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_0: 32;
        } BITS_3A0;
    } u_3A0;

    union
    {
        __IO uint32_t  REG_0x3A4;                       /**< 0x3A4    */
        struct
        {
            __IO uint32_t SWR_SS_LUT_1: 32;
        } BITS_3A4;
    } u_3A4;

    __IO uint32_t  RSVD4[2];                          /**< 0x3A8~ 0x3AC   */

    union
    {
        __IO uint32_t  REG_GPIO4_2_4_7;                /**< 0x3B0    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P4_0: 8;
            __IO uint32_t PMUX_GPIO_P4_1: 8;
            __IO uint32_t PMUX_GPIO_P4_2: 8;
            __IO uint32_t PMUX_GPIO_P4_3: 8;
        } BITS_3B0;
    } u_3B0;

    union
    {
        __IO uint32_t  REG_GPIOC4_7;                 /**< 0x3B4    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P4_4: 8;
            __IO uint32_t PMUX_GPIO_P4_5: 8;
            __IO uint32_t PMUX_GPIO_P4_6: 8;
            __IO uint32_t PMUX_GPIO_P4_7: 8;
        } BITS_3B4;
    } u_3B4;

    union
    {
        __IO uint32_t  REG_GPIO5_0_3;                 /**< 0x3B8    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P5_0: 8;
            __IO uint32_t PMUX_GPIO_P5_1: 8;
            __IO uint32_t PMUX_GPIO_P5_2: 8;
            __IO uint32_t PMUX_GPIO_P5_3: 8;
        } BITS_3B8;
    } u_3B8;

    union
    {
        __IO uint32_t  REG_GPIO5_4_7;                 /**< 0x3BC    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P5_4: 8;
            __IO uint32_t PMUX_GPIO_P5_5: 8;
            __IO uint32_t PMUX_GPIO_P5_6: 8;
            __IO uint32_t PMUX_GPIO_P5_7: 8;
        } BITS_3BC;
    } u_3BC;

    union
    {
        __IO uint32_t  REG_GPIO6_0_3;                 /**< 0x3C0    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P6_0: 8;
            __IO uint32_t PMUX_GPIO_P6_1: 8;
            __IO uint32_t PMUX_GPIO_P6_2: 8;
            __IO uint32_t PMUX_GPIO_P6_3: 8;
        } BITS_3C0;
    } u_3C0;

    union
    {
        __IO uint32_t  REG_GPIO6_4_6;                 /**< 0x3C4    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P6_4: 8;
            __IO uint32_t PMUX_GPIO_P6_5: 8;
            __IO uint32_t PMUX_GPIO_P6_6: 8;
            __IO uint32_t RSVD: 8;
        } BITS_3C4;
    } u_3C4;

    union
    {
        __IO uint32_t  REG_GPIO7_0_3;                 /**< 0x3C8    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P7_0: 8;
            __IO uint32_t PMUX_GPIO_P7_1: 8;
            __IO uint32_t PMUX_GPIO_P7_2: 8;
            __IO uint32_t PMUX_GPIO_P7_3: 8;
        } BITS_3C8;
    } u_3C8;

    union
    {
        __IO uint32_t  REG_GPIO8_4_7;                 /**< 0x3CC    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P7_4: 8;
            __IO uint32_t PMUX_GPIO_P7_5: 8;
            __IO uint32_t PMUX_GPIO_P7_6: 8;
            __IO uint32_t DUMMY: 8;
        } BITS_3CC;
    } u_3CC;

    union
    {
        __IO uint32_t  REG_GPIO8_0_3;                 /**< 0x3D0    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P8_0: 8;
            __IO uint32_t PMUX_GPIO_P8_1: 8;
            __IO uint32_t PMUX_GPIO_P8_2: 8;
            __IO uint32_t PMUX_GPIO_P8_3: 8;
        } BITS_3D0;
    } u_3D0;

    union
    {
        __IO uint32_t  REG_GPIO5_4_7;                 /**< 0x3D4    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P8_4: 8;
            __IO uint32_t PMUX_GPIO_P8_5: 8;
            __IO uint32_t DUMMY: 8;
            __IO uint32_t DUMMY1: 8;
        } BITS_3D4;
    } u_3D4;

    union
    {
        __IO uint32_t  REG_GPIO9_0_3;                 /**< 0x3D8    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P9_0: 8;
            __IO uint32_t PMUX_GPIO_P9_1: 8;
            __IO uint32_t PMUX_GPIO_P9_2: 8;
            __IO uint32_t PMUX_GPIO_P9_3: 8;
        } BITS_3D8;
    } u_3D8;

    union
    {
        __IO uint32_t  REG_GPIO9_4_5;                 /**< 0x3DC    */
        struct
        {
            __IO uint32_t PMUX_GPIO_P9_4: 8;
            __IO uint32_t PMUX_GPIO_P9_5: 8;
            __IO uint32_t DUMMY: 8;
            __IO uint32_t DUMMY1: 8;
        } BITS_3DC;
    } u_3DC;

    __IO uint32_t  RSVD5[6];                          /**< 0x3E0~ 0x3F4   */

    union
    {
        __IO uint32_t  AUTO_SW_PAR7_79_64;            /**< 0x3F8    */
        struct
        {
            __IO uint32_t CORE7_EN_HVD17_LOWIQ: 1;
            __IO uint32_t CORE7_TUNE_HVD17_IB: 4;
            __IO uint32_t CORE7_X4_PFM_COMP_IB: 1;
            __IO uint32_t CORE7_TUNE_PFM_VREFOCPPFM: 3;
            __IO uint32_t CORE7_TUNE_SAW_ICLK: 6;
            __IO uint32_t reserved: 17;
        } BITS_3F8;
    } u_3F8;
} SYS_BLKCTRL_TypeDef;

/* ================================================================================ */
/* ================                     Pinmux                     ================ */
/* ================================================================================ */

/**
  * @brief Pinmux. (Pinmux)
  */

typedef struct                                      /*!< Pinmux Structure */
{
    __IO uint32_t
    CFG[12];                              /*!<                                            */
} PINMUX_TypeDef;

/* ================================================================================ */
/* ================                     Clock Debounce             ================ */
/* ================================================================================ */

/**
  * @brief Clock Debounce. (Clock Debounce)
  */
typedef struct
{
    union
    {
        __IO uint32_t WORD0;
        struct
        {
            __IO uint32_t dbnc_div_limit: 8;
            __IO uint32_t dbnc_div_sel: 4;
            __IO uint32_t dbnc_div_en: 1;
            __IO uint32_t reserved: 19;
        } BITS_0;
    } u_0;
} DEBOUNCE_CLK_SET_TypeDef;
/**
  * @brief Peripheral. (Peripheral)
  */
/* ================================================================================ */
/* ================                   PERI_ON     0x40000200              ================ */
/* ================================================================================ */
typedef struct                                      /*!< Peripheral Structure */
{
    __IO uint32_t    SYS_CLK_SEL;       /*!< 0x200 */
    __IO uint32_t    SYS_CLK_SEL_2;     /*!< 0x204 */
    __IO uint32_t    SYS_CLK_SEL_3;     /*!< 0x208 */
    __IO uint32_t    SOC_FUNC_EN;       /*!< 0x210 */
    __IO uint32_t    RSVD0;             /*!< 0x214 */
} PHERION_TypeDef;

/* ================================================================================ */
/* ================              Peripheral Interrupt              ================ */
/* ================================================================================ */
#define PERI_IRQ_BIT_MBIAS_MFB_DET_L     BIT0
#define PERI_IRQ_BIT_mailbox_int         BIT1
#define PERI_IRQ_BIT_utmi_suspend_n      BIT2
#define PERI_IRQ_BIT_dig_trda_int_r      BIT3
#define PERI_IRQ_BIT_rng_int             BIT4
#define PERI_IRQ_BIT_psram_intr          BIT5
#define PERI_IRQ_BIT_dig_lpcomp_int_r    BIT6
#define PERI_IRQ_BIT_timer_intr_5        BIT7
#define PERI_IRQ_BIT_timer_intr_6        BIT8
#define PERI_IRQ_BIT_timer_intr_7        BIT9
#define PERI_IRQ_BIT_dig_lpcomp_int      BIT11
#define PERI_IRQ_BIT_MBIAS_VBAT_DET_L    BIT12
#define PERI_IRQ_BIT_MBIAS_ADP_DET_L     BIT13
#define PERI_IRQ_BIT_HW_ASRC_ISR1        BIT14
#define PERI_IRQ_BIT_HW_ASRC_ISR2        BIT15
#define PERI_IRQ_BIT_gpio_intr_31_6      BIT16
#define PERI_IRQ_BIT_dsp_wdt_to_mcu_intr BIT18
#define PERI_IRQ_BIT_flash_pwr_intr      BIT19
#define PERI_IRQ_BIT_sp0_intr_tx         BIT25
#define PERI_IRQ_BIT_sp0_intr_rx         BIT26
#define PERI_IRQ_BIT_sp1_intr_tx         BIT27
#define PERI_IRQ_BIT_sp1_intr_rx         BIT28

typedef enum
{
    TRIGGER_MODE_HGIH_LEVEL,
    TRIGGER_MODE_EDGE,
} TRIGGER_MODE;

typedef enum
{
    EDGE_MODE_RISING,
    EDGE_MODE_BOTH,
} EDGE_MODE;

/**
  * @brief Peripheral Interrupt. (Peripheral Interrupt)
  */

typedef struct
{
    union
    {
        __IO uint32_t REGWATCH_DOG_TIMER;       /*!< 0x00 */
        struct
        {
            __IO uint32_t Wdt_divfactor: 16;    /* Wdt is count with 32.768KHz/(divfactor+1).
                                                   Minimum dividing factor is 1.*/
            __IO uint32_t Wdt_en_byte: 8;       /* Set 0xA5 to enable watch dog timer */
            __IO uint32_t Cnt_limit: 4;         /* 0: 0x001,1: 0x003,2: 0x007,3: 0x00F,4: 0x01F,
                                                   5: 0x03F,6: 0x07F,7: 0x0FF,8: 0x1FF,9: 0x3FF,
                                                   10: 0x7FF, 11~15: 0xFFF */
            __IO uint32_t wdtaon_en: 1;         /* {Wdt_mode,wdtaon_en} == 00 interrupt cpu,
                                                   10 reset core domain,
                                                   01 reset whole chip except aon reg,
                                                   11 reset whole chip*/
            __IO uint32_t Wdt_mode: 1;
            __IO uint32_t Wdt_to: 1;            /* Watch dog timer timeout:
                                            when bit 30 = 1 (reset mode) --- 1 cycle pulse
                                            when bit 30 = 0 (interrupt mode) ---- Write 1 clear */

        };
    };
    union
    {
        __IO uint32_t REG_LOW_PRI_INT_STATUS;   /*!< 0x04, default: 0x0 */
        struct
        {
            __IO uint32_t MBIAS_MFB_DET_L: 1;
            __IO uint32_t mailbox_int: 1;
            __IO uint32_t utmi_suspend_n: 1;
            __IO uint32_t dig_trda_int_r: 1;
            __IO uint32_t rng_int: 1;
            __IO uint32_t psram_intr: 1;
            __IO uint32_t dig_lpcomp_int_r: 1;
            __IO uint32_t timer_intr_5: 1;
            __IO uint32_t timer_intr_6: 1;
            __IO uint32_t timer_intr_7: 1;
            __IO uint32_t rsvd0: 1;
            __IO uint32_t dig_lpcomp_int: 1;
            __IO uint32_t MBIAS_VBAT_DET_L: 1;
            __IO uint32_t MBIAS_ADP_DET_L: 1;
            __IO uint32_t HW_ASRC_ISR1: 1;
            __IO uint32_t HW_ASRC_ISR2: 1;
            __IO uint32_t gpio_intr_31_6: 1;
            __IO uint32_t rsvd1: 1;
            __IO uint32_t dsp_wdt_to_mcu_intr: 1;
            __IO uint32_t flash_pwr_intr: 1;
            __IO uint32_t rsvd2: 5;
            __IO uint32_t sp0_intr_tx: 1;
            __IO uint32_t sp0_intr_rx: 1;
            __IO uint32_t sp1_intr_tx: 1;
            __IO uint32_t sp1_intr_rx: 1;
            __IO uint32_t rsvd3: 3;
        };
    };
    __IO uint32_t REG_LOW_PRI_INT_MODE;         /*!< 0x08, default: 0xFFFFFFFF
                                                     0: high level active, 1: edge trigger
                                                 */
    __IO uint32_t REG_LOW_PRI_INT_EN;           /*!< 0x0C, default: 0x0 */
    union
    {
        __IO uint32_t BT_MAC_interrupt;         /*!< 0x10 */
        struct
        {
            __IO uint32_t timer_intr1_and_timer_intr0: 1;
            __IO uint32_t bluewiz_intr_r: 1;
            __IO uint32_t hci_dma_intr: 1;
            __IO uint32_t btverdor_reg_intr: 1;
            __IO uint32_t rsvd: 28;
        };
    };
    union
    {
        __IO uint32_t INT_2ND_LVL;              /*!< 0x14 */
        struct
        {
            __IO uint32_t otg_intr: 1;
            __IO uint32_t sdio_host_intr: 1;
            __IO uint32_t dummy0: 14;
            __IO uint32_t rxi300_intr: 1;
            __IO uint32_t rdp_intr: 1;
            __IO uint32_t dummy1: 6;
            __IO uint32_t rxi300_intr_en: 1;
            __IO uint32_t rdp_intr_en: 1;
            __IO uint32_t dummy2: 6;
        };
    };
    __IO uint32_t Interrupt_edge_option;        /*!< 0x18, default: 0x0
                                                     0:rising edge (HW default),1:both edge
                                                 */
} SoC_VENDOR_REG_TypeDef;

/* ================================================================================ */
/* ================                    Key Scan                    ================ */
/* ================================================================================ */

/**
  * @brief Key Scan. (KeyScan)
  */

typedef struct                                      /*!< KeyScan Structure */
{

    __IO uint32_t CLKDIV;               /*!< 0x00 */
    __IO uint32_t TIMERCR;              /*!< 0x04 */
    __IO uint32_t CR;                   /*!< 0x08 */
    __IO uint32_t COLCR;                /*!< 0x0C */
    __IO uint32_t ROWCR;                /*!< 0x10 */
    __I  uint32_t FIFODATA;             /*!< 0x14 */
    __IO uint32_t INTMASK;              /*!< 0x18 */
    __IO uint32_t INTCLR;               /*!< 0x1C */
    __I  uint32_t STATUS;               /*!< 0x20 */

} KEYSCAN_TypeDef;

/* ======================================================== */
/* ================                      GPIO                       ================ */
/* ======================================================== */

/**
  * @brief General purpose input and output. (GPIO)
  */

typedef struct
{
    __IO uint32_t DATAOUT;                              /*!< 0x00 Data register: data output */
    __IO uint32_t DATADIR;                              /*!< 0x04 Data direction register */
    __IO uint32_t DATASRC;                              /*!< 0x08 Data source register  */
    uint32_t RSVD[9];                                   /*!< 0x0C ~ 0x2C */
    __IO uint32_t INTEN;                                /*!< 0x30 Interrupt enable register */
    __IO uint32_t INTMASK;                              /*!< 0x34 Interrupt mask register */
    __IO uint32_t INTTYPE;                              /*!< 0x38 Interrupt level register */
    __IO uint32_t INTPOLARITY;                          /*!< 0x3C Interrupt polarity register */
    __IO uint32_t INTSTATUS;                            /*!< 0x40 Interrupt status of Port A  */
    __IO uint32_t
    RAWINTSTATUS;                         /*!< 0x44 Raw interrupt status of Port A (premasking) */
    __IO uint32_t DEBOUNCE;                             /*!< 0x48 Debounce enable register */
    __O  uint32_t INTCLR;                               /*!< 0x4C clear interrupt register */
    __I  uint32_t DATAIN;                               /*!< 0x50 external port register */
    uint32_t RSVD1[3];                                  /*!< 0x54 ~ 0x5C */
    __IO uint32_t
    LSSYNC;                               /*!< 0x60 Level-sensitive synchronization enable register*/
    uint32_t RSVD2;                                     /*!< 0x64 */
    //__I  uint32_t IDCODE;                               /*!< ID code register */
    __IO uint32_t INTBOTHEDGE;                          /*!< 0x68 Both edge to trigger interrupt*/
} GPIO_TypeDef;

/* ======================================================= */
/* ================                      PWM                     ================ */
/* ======================================================= */

/**
  * @brief PWM
  */
typedef struct
{
    __IO uint32_t CR;                   /*!< 0x00*/
} PWM_TypeDef;


/* ======================================================= */
/* ================                 Peri clock reg               ================ */
/* ======================================================= */

/**
  * @brief PWM
  */
typedef struct
{
    __IO uint32_t CLKSELE;                    /*!< 0x348*/
    uint32_t RSVD[4];
    __IO uint32_t CLK_SRCL;
    __IO uint32_t CLK_SRCH;                   /*!< 0x360*/
    __IO uint32_t PWM0_CTRL_L;
    __IO uint32_t PWM0_CTRL_H;
    __IO uint32_t PWM1_CTRL_L;
    __IO uint32_t PWM1_CTRL_H;                /*!< 0x370*/
    __IO uint32_t PWM2_CTRL_L;
    __IO uint32_t PWM2_CTRL_H;
    __IO uint32_t PWM3_CTRL_L;
    __IO uint32_t PWM3_CTRL_H;                /*!< 0x380*/
    __IO uint32_t TIM_EVT_CTRL;

} Peri_ClockGate_TypeDef;

/* ======================================================= */
/* ================                      ICG                     ================ */
/* ======================================================= */

/**
  * @brief cache for flash
  */
typedef struct
{
    __IO uint32_t CTRL0;            /*!< ICG cells control address register0, Address Offset: 0x00*/
    __IO uint32_t CTRL1;            /*!< ICG cells control address register1, Address Offset: 0x04*/
    __IO uint32_t CACHE_RAM_CTRL;   /*!< icache twoway control, Address Offset: 0x08*/
} ICG_TypeDef;

/* ======================================================= */
/* ================                      CACHE                     ================ */
/* ======================================================= */

/**
  * @brief cache for flash
  */
typedef struct
{
    __IO uint32_t CACHE_ENABLE;          /*!< SPIC cache Enable Register, Address Offset: 0x00*/
    __IO uint32_t FLUSH;                 /*!< Cache Flush register, Address Offset: 0x04*/
    __IO uint32_t INTR;                  /*!< Cache Interrupt register, Address Offset: 0x08*/
    __IO uint32_t RST_CNT;           /*!< Cache Reset Counter register, Address Offset: 0x0C*/
    __IO uint32_t RD_EVT_CNT;        /*!< Cache Read Event Counter register, Address Offset: 0x10*/
    __IO uint32_t HIT_EVT_CNT;       /*!< Cache HIT Event Counter register, Address Offset: 0x14*/
    __IO uint32_t HIT_LSTW_EVT_CNT;  /*!< Cache Hit lastway event counter register, Offset: 0x18*/
    __IO uint32_t RD_PND_CNT;        /*!< Cache Read pending counter register, Offset: 0x1c*/
} CACHE_TypeDef;

/* ================================================================================ */
/* ================                      SPIC                      ================ */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t CTRLR0;               /*!< Control reg 0,                         offset: 0x000 */
    __IO uint32_t RX_NDF;               /*!< User mode rx data data frame counter,  offset: 0x004 */
    __IO uint32_t SSIENR;               /*!< Enable reg,                            offset: 0x008 */
    __IO uint32_t MWCR;                 /*!< N/A,                                   offset: 0x00C */
    __IO uint32_t SER;                  /*!< Slave enable reg,                      offset: 0x010 */
    __IO uint32_t BAUDR;                /*!< Baudrate select reg,                   offset: 0x014 */
    __IO uint32_t TXFTLR;               /*!< Tx FIFO threshold level,               offset: 0x018 */
    __IO uint32_t RXFTLR;               /*!< Rx FIFO threshold level,               offset: 0x01C */
    __IO uint32_t TXFLR;                /*!< Tx FIFO level reg,                     offset: 0x020 */
    __IO uint32_t RXFLR;                /*!< Rx FIFO level reg,                     offset: 0x024 */
    __IO uint32_t SR;                   /*!< Status reg,                            offset: 0x028 */
    __IO uint32_t IMR;                  /*!< Interrupt mask reg,                    offset: 0x02C */
    __IO uint32_t ISR;                  /*!< Interrupt status reg,                  offset: 0x030 */
    __IO uint32_t RISR;                 /*!< Raw interrupt status reg,              offset: 0x034 */
    __IO uint32_t TXOICR;               /*!< Tx FIFO overflow interrupt clear reg,  offset: 0x038 */
    __IO uint32_t RXOICR;               /*!< Rx FIFO overflow interrupt clear reg,  offset: 0x03C */
    __IO uint32_t RXUICR;               /*!< Rx FIFO underflow interrupt clear reg, offset: 0x040 */
    __IO uint32_t MSTICR;               /*!< Master error interrupt clear reg,      offset: 0x044 */
    __IO uint32_t ICR;                  /*!< Interrupt clear reg,                   offset: 0x048 */
    __IO uint32_t DMACR;                /*!< DMA control reg,                       offset: 0x04C */
    __IO uint32_t DMATDLR;              /*!< DMA transimit data level reg,          offset: 0x050 */
    __IO uint32_t DMARDLR;              /*!< DMA revceive data level reg,           offset: 0x054 */
    __IO uint32_t IDR;                  /*!< Identiation reg,                       offset: 0x058 */
    __IO uint32_t SPIC_VERSION;         /*!< Version ID reg,                        offset: 0x05C */
    union
    {
        __IO uint8_t  BYTE;
        __IO uint16_t HALF;
        __IO uint32_t WORD;
    } DR[16];                           /*!< Data reg,                              offset: 0x060 */
    __IO uint32_t DM_DR[16];            /*!< Data mask data register,               offset: 0x0A0 */
    __IO uint32_t READ_FAST_SINGLE;     /*!< Fast read data cmd of flash,           offset: 0x0E0 */
    __IO uint32_t READ_DUAL_DATA;       /*!< Dual output read cmd of flash,         offset: 0x0E4 */
    __IO uint32_t READ_DUAL_ADDR_DATA;  /*!< Dual I/O read cmd of flash,            offset: 0x0E8 */
    __IO uint32_t READ_QUAD_DATA;       /*!< Quad output read cmd of flash,         offset: 0x0EC */
    __IO uint32_t READ_QUAD_ADDR_DATA;  /*!< Quad I/O read cmd of flash,            offset: 0x0F0 */
    __IO uint32_t WRITE_SINGLE;         /*!< Page program cmd of flash,             offset: 0x0F4 */
    __IO uint32_t WRITE_DUAL_DATA;      /*!< Dual data input program cmd of flash,  offset: 0x0F8 */
    __IO uint32_t WRITE_DUAL_ADDR_DATA; /*!< Dual addr & data program cmd of flash, offset: 0x0FC */
    __IO uint32_t WRITE_QUAD_DATA;      /*!< Quad data input program cmd of flash,  offset: 0x100 */
    __IO uint32_t WRITE_QUAD_ADDR_DATA; /*!< Quad addr & data program cmd of flash, offset: 0x104 */
    __IO uint32_t WRITE_ENABLE;         /*!< Write enabe cmd of flash,              offset: 0x108 */
    __IO uint32_t READ_STATUS;          /*!< Read status cmd of flash,              offset: 0x10C */
    __IO uint32_t CTRLR2;               /*!< Control reg 2,                         offset: 0x110 */
    __IO uint32_t FBAUDR;               /*!< Fast baudrate select,                  offset: 0x114 */
    __IO uint32_t USER_LENGTH;          /*!< Addr length reg,                       offset: 0x118 */
    __IO uint32_t AUTO_LENGTH;          /*!< Auto addr length reg,                  offset: 0x11C */
    __IO uint32_t VALID_CMD;            /*!< Valid cmd reg,                         offset: 0x120 */
    __IO uint32_t FLASH_SIZE_R;         /*!< Flash size reg,                        offset: 0x124 */
    __IO uint32_t FLUSH_FIFO;           /*!< Flush FIFO reg,                        offset: 0x128 */
    __IO uint32_t DUM_BYTE;             /*!< Dummy byte value,                      offset: 0x12C */
    __IO uint32_t TX_NDF;               /*!< Tx NDF,                                offset: 0x130 */
    __IO uint32_t DEVICE_INFO;          /*!< Device info,                           offset: 0x134 */
    __IO uint32_t TPR0;                 /*!< Timing parameters,                     offset: 0x138 */
    __IO uint32_t AUTO_LENGTH2;         /*!< Auto addr length reg 2,                offset: 0x13C */
} SPIC_TypeDef;

/* ================================================================================ */
/* ================                      PSRAM                     ================ */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t CCR;           /*!< Configuration control register,          Address offset: 0x000 */
    __IO uint32_t DCR;           /*!< Device configuration control register,   Address offset: 0x004 */
    __IO uint32_t IOCR0;         /*!< I/O configuration control regsiter0,     Address offset: 0x008 */
    __IO uint32_t CSR;           /*!< Controller status register,              Address offset: 0x00c */
    __IO uint32_t DRR;           /*!< Device refresh/power-up register,        Address offset: 0x010 */
    __IO uint32_t RSVD0[4];      /*!< Reserved 0,                              Address offset: 0x014 */
    __IO uint32_t CMD_DPIN_NDGE; /*!< Device cmd/addr pin register (NEDGE),    Address offset: 0x024 */
    __IO uint32_t CMD_DPIN;      /*!< Device cmd/addr pin regsiter (PEDGE),    Address offset: 0x028 */
    __IO uint32_t CR_TDPIN;      /*!< Tie DPIN register (sw ctrl dfi_reset_n), Address offset: 0x02c */
    __IO uint32_t MR_INFO;       /*!< Mode latency information regster,        Address offset: 0x030 */
    __IO uint32_t MR0;           /*!< Device CR0 register,                     Address offset: 0x034 */
    __IO uint32_t MR1;           /*!< Device CR1 register,                     Address offset: 0x038 */
    __IO uint32_t RSVD1[9];      /*!< Reserved 1,                              Address offset: 0x03c */
    __IO uint32_t DPDRI;         /*!< DPIN data index register,                Address offset: 0x060 */
    __IO uint32_t DPDR;          /*!< DPIN data register,                      Address offset: 0x064 */
    __IO uint32_t RSVD2[35];     /*!< Reserved 2,                              Address offset: 0x068 */
    __IO uint32_t PCTL_SVN_ID;   /*!< PSRAM_LPC_CTRL version number,           Address offset: 0x0f4 */
    __IO uint32_t PCTL_IDR;      /*!< PSRAM_LPC_CTRL identification register,  Address offset: 0x0f8 */
    __IO uint32_t RSVD3[193];    /*!< Reserved 3,                              Address offset: 0x0fc */
    __IO uint32_t USER0_INDEX;   /*!< User extended index,                     Address offset: 0x400 */
    __IO uint32_t USER0_DATA;    /*!< User extended data,                      Address offset: 0x404 */
} PSRAMC_TypeDef;

/* =========================== RL6736 RXI300 section ===========================*/
typedef struct
{
    __I uint32_t NAME;
    __I uint32_t VER;
    __I uint32_t REV;
    __I uint32_t INST;
    __I uint32_t IMPL_Y;
    __I uint32_t IMPL_D;
    __I uint32_t DEV;
    __I uint32_t PRO_NUM;
    __I uint32_t rsvd0[120];
    union
    {
        __I uint32_t ELR_i_PLD0;
        struct
        {
            __I uint32_t ERR_SRC: 8;
            __I uint32_t ERR_CMD: 3;
            __I uint32_t ERR_BSTTYPE: 3;
            __I uint32_t rsvd: 2;
            __I uint32_t ERR_BSTLEN: 8;
            __I uint32_t ERR_BSTINDEX: 8;
        } BITS_200;
    } u_200;
    union
    {
        __I uint32_t ELR_i_PLD1;
        struct
        {
            __I uint32_t rsvd0: 16;
            __I uint32_t ERR_SIZE: 3;
            __I uint32_t rsvd1: 4;
            __I uint32_t ERR_PROT: 3;
            __I uint32_t ERR_Cache: 4;
            __I uint32_t ERR_Lock: 2;
        } BITS_204;
    } u_204;
    union
    {
        __I uint32_t ELR_i_ID;
        struct
        {
            __I uint32_t ERR_ID;
        } BITS_208;
    } u_208;
    union
    {
        __I uint32_t ELR_i_ADR0;
        struct
        {
            __I uint32_t ERR_ADR0;
        } BITS_20C;
    } u_20C;
    union
    {
        __I uint32_t ELR_i_ADR1;
        struct
        {
            __I uint32_t ERR_ADR1;
        } BITS_210;
    } u_210;
    __I uint32_t rsvd1[7];
    union
    {
        __I uint32_t ELR_i_CODE;
        struct
        {
            __I uint32_t rsvd: 24;
            __I uint32_t ELR_CODE: 8;
        } BITS_230;
    } u_230;
    __I uint32_t rsvd2[2];
    union
    {
        __IO uint32_t ELR_i_INTR_CLR;
        struct
        {
            __IO uint32_t ELR_INTR_CLR: 1;
            __IO uint32_t rsvd: 31;
        } BITS_23C;
    } u_23C;
} RXI300_Typedef;
/** @} */ /* End of group 87x3e_RTL876X_Peripheral_Registers_Structures */

/*============================================================================*
 *                              Macros
 *============================================================================*/

/** @defgroup 87x3e_RTL876X_Exported_Macros RTL876X  Exported Macros
    * @brief
  * @{
  */
/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define SYSTEM_REG_BASE             0x40000000UL
#define PERIPH_REG_BASE             0x40000000UL
#define VENDOR_REG_BASE             0x40006000UL
#define PERI_INT_REG_BASE           0x40006004UL
#define SYSBLKCTRL_REG_BASE         0x40000200UL
#define PINMUX_REG0_BASE            0x40000280UL
#define PINMUX_REG1_BASE            0x400003B0UL

#define SPIC_DLY_CTRL_BASE          0x40000300UL
#define DEBOUNCE_CLK_SET_REG_BASE   0x40000344UL
#define PERICLKGAT_REG_BASE         0x40000348UL
#define GPIO0_REG_BASE              0x40001000UL
#define GPIO1_REG_BASE              0x40004800UL
#define RTC_REG_BASE                0x40000100UL//0x40000140UL
#define PF_RTC_REG_BASE             0x40000140UL
#define RTC_LP_REG_BASE             0x40000180UL
#define RTC_LED_REG_BASE            0x40000400UL
#define LPC_REG_BASE                0x40000180UL
#define AON_WDT_REG_BASE            0x40000190UL
#define TIM0_REG_BASE               0x40025000UL
#define TIM1_REG_BASE               0x40025014UL
#define TIM2_REG_BASE               0x40025028UL
#define TIM3_REG_BASE               0x4002503CUL
#define TIM4_REG_BASE               0x40025050UL
#define TIM5_REG_BASE               0x40025064UL
#define TIM6_REG_BASE               0x40025078UL
#define TIM7_REG_BASE               0x4002508CUL
#define PWM0_REG_BASE               0x40000364UL
#define PWM1_REG_BASE               0x40000368UL
#define PWM2_REG_BASE               0x40000374UL
#define PWM3_REG_BASE               0x4000037CUL
#define TIM_CHANNELS_REG_BASE       0x400250A0UL

#define GDMA_CHANNEL_REG_BASE       0x40027000UL // for platform_1_1_1_20160323 later
#define GDMA_REG_BASE               (GDMA_CHANNEL_REG_BASE + 0x02c0)
#define GDMA_Channel0_BASE          (GDMA_CHANNEL_REG_BASE + 0x0000)
#define GDMA_Channel1_BASE          (GDMA_CHANNEL_REG_BASE + 0x0058)
#define GDMA_Channel2_BASE          (GDMA_CHANNEL_REG_BASE + 0x00b0)
#define GDMA_Channel3_BASE          (GDMA_CHANNEL_REG_BASE + 0x0108)
#define GDMA_Channel4_BASE          (GDMA_CHANNEL_REG_BASE + 0x0160)
#define GDMA_Channel5_BASE          (GDMA_CHANNEL_REG_BASE + 0x01b8)
#define GDMA_Channel6_BASE          (GDMA_CHANNEL_REG_BASE + 0x0210)
#define GDMA_Channel7_BASE          (GDMA_CHANNEL_REG_BASE + 0x0268)
#define GDMA_Channel8_BASE          (GDMA_CHANNEL_REG_BASE + 0x0400)
#define I8080_REG_BASE              0x40028000UL
#define SPI_CODEC_REG_BASE          0x4002C000UL
#define QDEC_REG_BASE               0x40004000UL
#define SPI2WIRE_REG_BASE           0x40004000UL
#define CODEC_REG_BASE              0x4002C000UL
#define KEYSCAN_REG_BASE            0x40005000UL
#define WDG_REG_BASE                0x40006000UL
#define AON_WDG_REG_BASE            0x40000190UL
#define RANDOM_GEN_REG_BASE         0x40006150UL
#define RXI300_MCU_REG_BASE         0x40026000UL
#define ICG_REG_BASE                0x40026400UL
#define CAP_TOUCH_REG_BASE          0x40007000UL
#define ADC_REG_BASE                0x40010000UL

#define GPIOA_DMA_PORT_ADDR         0x40011200UL /* rtl87x3e */

#define UART1_REG_BASE              0x40011000UL
#define UART0_REG_BASE              0x40012000UL
#define UART2_REG_BASE              0x40024000UL

// for compatible with BBPRO
#define LOG_UART_REG_BASE           0x40011000UL
#define UART_REG_BASE               0x40012000UL
#define LOG_UART1_REG_BASE          0x40024000UL

#define HW_AES_REG_BASE             0x40014000UL
#define IR_REG_BASE                 0x40016800UL
#define PSRAM_REG_BASE              0x40017000UL
#define SPI0_HS_REG_BASE            0x40040000UL
#define RTL_SPI0_BASE               0x40042000UL
#define RTL_SPI1_BASE               0x40042400UL
#define I2C0_REG_BASE               0x40015000UL
#define I2C1_REG_BASE               0x40015400UL
#define I2C2_REG_BASE               0x40015800UL
#define SPI0_REG_BASE               0x40013000UL
#define SPI1_REG_BASE               0x40015C00UL
#define SPI2_REG_BASE               0x40016000UL
#define ISO7816_REG_BASE            0x40016400UL
#define IF8080_REG_BASE             0x40028000UL
#define IF8080_LLI_REG1_BASE        0x40028050UL
#define IF8080_LLI_REG1_GDMA_BASE   0x400280A0UL
#define IF8080_LLI_REG2_BASE        0x40028080UL
#define IF8080_LLI_REG2_GDMA_BASE   0x400280C0UL
#define IF8080_LLI_REG1_OFT_BASE    0x40028070UL
#define IF8080_LLI_REG2_OFT_BASE    0x40028078UL
#define IF8080_LLI_CR_REG_BASE      0x40028094UL
#define BT_BB_REG_BASE              0x40050000UL // actually used: #define BB_BASE_ADDR 0x40050000
#define BT_LE_REG_BASE              0x40051000UL
#define BT_VENDOR_REG_BASE          0x40058000UL
#define GDMA0_REG_BASE              0x40060000UL
#define HCI_DMA_REG_BASE            0x40064000UL
#define HCI_UART_REG_BASE           0x40068000UL
#define SPIC0_REG_BASE              0x40080000UL
#define SPIC1_REG_BASE              0x40084000UL
#define SPIC2_REG_BASE              0x40088000UL
#define SPIC3_REG_BASE              0x4008C000UL
#define H2D_D2H_REG_BASE            0x40023000UL
#define SPORT0_REG_BASE             0x40020000UL
#define SPORT1_REG_BASE             0x40021000UL
#define SPORT2_REG_BASE             0x40022000UL
#define SPDIF_REG_BASE              0x40021800UL
#define SDIO0_REG_BASE              0x4005C000UL
#define USB_OTG_CFG_BASE            0x400C0000UL

#define SPI0_MASTER_MODE_REG        *((volatile uint32_t *)0x40000308UL)
#define SPI0_MASTER_MODE_BIT        BIT(8)

/** @brief clock divider for peripheral */
#define SYSTEM_CLK_CTRL             *((volatile uint32_t *)0x4000020CUL)
#define CLK_SOURCE_REG_0            *((volatile uint32_t *)0x40000348UL)
#define CLK_SOURCE_REG_1            *((volatile uint32_t *)0x4000035CUL)
#define CLK_SOURCE_REG_2            *((volatile uint32_t *)0x40000360UL)

#define PERI_CLOCKGATE_REG_BASE     0x40000348UL

#define REG_PEON_SYS_CLK_SEL        0x0200
#define REG_PEON_SYS_CLK_SEL_2      0x0208
#define REG_PEON_SYS_CLK_SEL_3      0x020C
#define REG_SOC_FUNC_EN             0x0210
#define REG_SOC_HCI_COM_FUNC_EN     0x0214
#define REG_SOC_PERI_FUNC0_EN       0x0218
#define REG_SOC_PERI_FUNC1_EN       0x021C
#define REG_PESOC_CLK_CTRL          0x0230
#define REG_PESOC_PERI_CLK_CTRL0    0x0234
#define REG_PESOC_PERI_CLK_CTRL1    0x0238
#define REG_PESOC_DSP_SHARE_RAM     0x0250
#define REG_TEST_MODE               0x02a8
#define REG_ANAPAR_PLL1_0           0x0320
#define REG_ANAPAR_PLL3_2           0x0324
#define REG_ANAPAR_PLL5_4           0x0328
#define REG_XTAL_PLL_READY          0x0338

#define GPIO_OUTPUT_OFFSET          0x00
#define GPIO_DIRECTION_OFFSET       0x04

/** @brief AON PAD AREA */
#define REG_PAD_WKEN_ADDRESS        0x20
#define REG_PAD_WK_CTRL_ADDRESS     0x12d
#define REG_PAD_WKPOL_ADDRESS       0x25
#define REG_PAD_O_ADDRESS           0x2A
#define REG_AON_PAD_E_ADDRESS       0x2F
#define REG_AON_PAD_S_ADDRESS       0x34
#define REG_AON_PAD_PU_ADDRESS      0x39
#define REG_AON_PAD_PD_ADDRESS      0x3E
#define REG_AON_PAD_PWRON_ADDRESS   0x4C

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
/** @brief System */
#define SYSBLKCTRL                      ((SYS_BLKCTRL_TypeDef      *) SYSBLKCTRL_REG_BASE)
#define SoC_VENDOR                      ((SoC_VENDOR_REG_TypeDef   *) VENDOR_REG_BASE)

/** @brief IO */
#define PINMUX0                         ((PINMUX_TypeDef           *) PINMUX_REG0_BASE)
#define PINMUX1                         ((PINMUX_TypeDef           *) PINMUX_REG1_BASE)
#define DEBOUNCE_CLK_SET                ((DEBOUNCE_CLK_SET_TypeDef *) DEBOUNCE_CLK_SET_REG_BASE)
#define KEYSCAN                         ((KEYSCAN_TypeDef          *) KEYSCAN_REG_BASE)
#define GPIO                            ((GPIO_TypeDef             *) GPIO0_REG_BASE)
#define GPIOA                           ((GPIO_TypeDef             *) GPIO0_REG_BASE)
#define GPIOB                           ((GPIO_TypeDef             *) GPIO1_REG_BASE)
#define QDEC                            ((QDEC_TypeDef             *) QDEC_REG_BASE)
#define I2C0                            ((I2C_TypeDef              *) I2C0_REG_BASE)
#define I2C1                            ((I2C_TypeDef              *) I2C1_REG_BASE)
#define I2C2                            ((I2C_TypeDef              *) I2C2_REG_BASE)
#define SPI0                            ((SPI_TypeDef              *) SPI0_REG_BASE)
#define SPI1                            ((SPI_TypeDef              *) SPI1_REG_BASE)
#define SPI2                            ((SPI_TypeDef              *) SPI2_REG_BASE)
#define TIM0                            ((TIM_TypeDef              *) TIM0_REG_BASE)
#define TIM1                            ((TIM_TypeDef              *) TIM1_REG_BASE)
#define TIM2                            ((TIM_TypeDef              *) TIM2_REG_BASE)
#define TIM3                            ((TIM_TypeDef              *) TIM3_REG_BASE)
#define TIM4                            ((TIM_TypeDef              *) TIM4_REG_BASE)
#define TIM5                            ((TIM_TypeDef              *) TIM5_REG_BASE)
#define TIM6                            ((TIM_TypeDef              *) TIM6_REG_BASE)
#define TIM7                            ((TIM_TypeDef              *) TIM7_REG_BASE)
#define PWM0_PN                         ((PWM_TypeDef              *) PWM0_REG_BASE)
#define PWM1_PN                         ((PWM_TypeDef              *) PWM1_REG_BASE)
#define PWM2                            ((PWM_TypeDef              *) PWM2_REG_BASE)
#define PWM3                            ((PWM_TypeDef              *) PWM3_REG_BASE)
#define TIM_CHANNELS                    ((TIM_ChannelsTypeDef      *) TIM_CHANNELS_REG_BASE)
#define GDMA_BASE                       ((GDMA_TypeDef             *) GDMA_REG_BASE)
#define GDMA_Channel0                   ((GDMA_ChannelTypeDef      *) GDMA_Channel0_BASE)
#define GDMA_Channel1                   ((GDMA_ChannelTypeDef      *) GDMA_Channel1_BASE)
#define GDMA_Channel2                   ((GDMA_ChannelTypeDef      *) GDMA_Channel2_BASE)
#define GDMA_Channel3                   ((GDMA_ChannelTypeDef      *) GDMA_Channel3_BASE)
#define GDMA_Channel4                   ((GDMA_ChannelTypeDef      *) GDMA_Channel4_BASE)
#define GDMA_Channel5                   ((GDMA_ChannelTypeDef      *) GDMA_Channel5_BASE)
#define GDMA_Channel6                   ((GDMA_ChannelTypeDef      *) GDMA_Channel6_BASE)
#define GDMA_Channel7                   ((GDMA_ChannelTypeDef      *) GDMA_Channel7_BASE)
#define GDMA_Channel8                   ((GDMA_ChannelTypeDef      *) GDMA_Channel8_BASE)
#define ADC                             ((ADC_TypeDef              *) ADC_REG_BASE)
#define UART0                           ((UART_TypeDef             *) UART0_REG_BASE)
#define UART1                           ((UART_TypeDef             *) UART1_REG_BASE)
#define UART2                           ((UART_TypeDef             *) UART2_REG_BASE)

#define SPI3WIRE                        ((SPI3WIRE_TypeDef         *) SPI2WIRE_REG_BASE)
#define IR                              ((IR_TypeDef               *) IR_REG_BASE)
#define RTC                             ((RTC_TypeDef              *) RTC_REG_BASE)
#define RTC_LP                          ((RTC_LP_TypeDef           *) RTC_LP_REG_BASE)
#define RTC_LED                         ((RTC_LED_TypeDef          *) RTC_LED_REG_BASE)
#define LPC                             ((LPC_TypeDef              *) LPC_REG_BASE)
#define HWAES                           ((HW_AES_TypeDef           *) HW_AES_REG_BASE)
#define WDG                             ((WDG_TypeDef              *) WDG_REG_BASE)
#define AON_WDG                         ((AON_WDG_TypeDef          *) AON_WDG_REG_BASE)
#define RAN_GEN                         ((RAN_GEN_TypeDef          *) RANDOM_GEN_REG_BASE)
#define ICG                             ((ICG_TypeDef              *) ICG_REG_BASE)
#define SPIC0                           ((SPIC_TypeDef             *) SPIC0_REG_BASE)
#define SPIC1                           ((SPIC_TypeDef             *) SPIC1_REG_BASE)
#define SPIC2                           ((SPIC_TypeDef             *) SPIC2_REG_BASE)
#define SPIC3                           ((SPIC_TypeDef             *) SPIC3_REG_BASE)
#define PSRAMC                          ((PSRAMC_TypeDef           *) PSRAM_REG_BASE)
#define SDIO                            ((SDIO_TypeDef             *) SDIO0_REG_BASE)
#define CLK_GATE                        ((Peri_ClockGate_TypeDef   *) PERI_CLOCKGATE_REG_BASE)
#define RXI300                          ((RXI300_Typedef           *) RXI300_MCU_REG_BASE)

#define IF8080                          ((IF8080_TypeDef*) IF8080_REG_BASE)
#define IF8080_LLPGROUP1                ((IF8080_GDMALLITypeDef*) IF8080_LLI_REG1_BASE)
#define IF8080_LLPGROUP2                ((IF8080_GDMALLITypeDef*) IF8080_LLI_REG2_BASE)
#define IF8080_LLPGROUP1_GDMA           ((IF8080_GDMALLITypeDef*) IF8080_LLI_REG1_GDMA_BASE)
#define IF8080_LLPGROUP2_GDMA           ((IF8080_GDMALLITypeDef*) IF8080_LLI_REG2_GDMA_BASE)
#define IF8080_LLPGROUP1_OFT            ((IF8080_GDMALLIOFTTypeDef*) IF8080_LLI_REG1_OFT_BASE)
#define IF8080_LLPGROUP2_OFT            ((IF8080_GDMALLIOFTTypeDef*) IF8080_LLI_REG2_OFT_BASE)
#define IF8080_GDMA                     ((IF8080_GDMATypeDef*)IF8080_LLI_CR_REG_BASE)

//Add by Vendor
#define LITTLE_ENDIAN                        0
#define BIG_ENDIAN                           1
#define SYSTEM_ENDIAN                        LITTLE_ENDIAN

#define SWAP32(x) ((uint32_t)(                         \
                                                       (((uint32_t)(x) & (uint32_t)0x000000ff) << 24) |            \
                                                       (((uint32_t)(x) & (uint32_t)0x0000ff00) <<  8) |            \
                                                       (((uint32_t)(x) & (uint32_t)0x00ff0000) >>  8) |            \
                                                       (((uint32_t)(x) & (uint32_t)0xff000000) >> 24)))

#define WAP16(x) ((uint16_t)(                         \
                                                      (((uint16_t)(x) & (uint16_t)0x00ff) <<  8) |            \
                                                      (((uint16_t)(x) & (uint16_t)0xff00) >>  8)))

#if SYSTEM_ENDIAN == LITTLE_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      ((uint32_t)(x))
#define rtk_le32_to_cpu(x)      ((uint32_t)(x))
#define rtk_cpu_to_le16(x)      ((uint16_t)(x))
#define rtk_le16_to_cpu(x)      ((uint16_t)(x))
#define rtk_cpu_to_be32(x)      SWAP32((x))
#define rtk_be32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_be16(x)      WAP16((x))
#define rtk_be16_to_cpu(x)      WAP16((x))
#endif

#elif SYSTEM_ENDIAN == BIG_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      SWAP32((x))
#define rtk_le32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_le16(x)      WAP16((x))
#define rtk_le16_to_cpu(x)      WAP16((x))
#define rtk_cpu_to_be32(x)      ((uint32_t)(x))
#define rtk_be32_to_cpu(x)      ((uint32_t)(x))
#define rtk_cpu_to_be16(x)      ((uint16_t)(x))
#define rtk_be16_to_cpu(x)      ((uint16_t)(x))
#endif
#endif

#define HAL_READ32(base, addr)            \
    rtk_le32_to_cpu(*((volatile uint32_t *)(base + addr)))

#define HAL_WRITE32(base, addr, value32)  \
    ((*((volatile uint32_t *)(base + addr))) = rtk_cpu_to_le32(value32))

#define HAL_UPDATE32(addr, mask, value32)  \
    HAL_WRITE32(0, addr, (HAL_READ32(0, addr) & ~(mask)) | ((value32) & (mask)))

#define HAL_READ16(base, addr)            \
    rtk_le16_to_cpu(*((volatile uint16_t *)(base + addr)))

#define HAL_WRITE16(base, addr, value)  \
    ((*((volatile uint16_t *)(base + addr))) = rtk_cpu_to_le16(value))

#define HAL_UPDATE16(addr, mask, value16)  \
    HAL_WRITE16(0, addr, (HAL_READ16(0, addr) & ~(mask)) | ((value16) & (mask)))

#define HAL_READ8(base, addr)            \
    (*((volatile uint8_t *)(base + addr)))

#define HAL_WRITE8(base, addr, value)  \
    ((*((volatile uint8_t *)(base + addr))) = value)

#define HAL_UPDATE8(addr, mask, value8)  \
    HAL_WRITE8(0, addr, (HAL_READ8(0, addr) & ~(mask)) | ((value8) & (mask)))

#define BIT0        0x00000001
#define BIT1        0x00000002
#define BIT2        0x00000004
#define BIT3        0x00000008
#define BIT4        0x00000010
#define BIT5        0x00000020
#define BIT6        0x00000040
#define BIT7        0x00000080
#define BIT8        0x00000100
#define BIT9        0x00000200
#define BIT10       0x00000400
#define BIT11       0x00000800
#define BIT12       0x00001000
#define BIT13       0x00002000
#define BIT14       0x00004000
#define BIT15       0x00008000
#define BIT16       0x00010000
#define BIT17       0x00020000
#define BIT18       0x00040000
#define BIT19       0x00080000
#define BIT20       0x00100000
#define BIT21       0x00200000
#define BIT22       0x00400000
#define BIT23       0x00800000
#define BIT24       0x01000000
#define BIT25       0x02000000
#define BIT26       0x04000000
#define BIT27       0x08000000
#define BIT28       0x10000000
#define BIT29       0x20000000
#define BIT30       0x40000000
#define BIT31       0x80000000

#define BIT(_n)     (uint32_t)(1U << (_n))
#define BIT64(_n)   (1ULL << (_n))

/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
//#define USE_FULL_ASSERT


/** @} */ /* End of group 87x3e_RTL876X_Exported_Macros */


/*============================================================================*
  *                                Functions
 *============================================================================*/
/** @defgroup 87x3e_RTL876X_Exported_Functions RTL876X Sets Exported Functions
    * @brief
    * @{
    */
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


/**
    * @brief    Read data from aon register
    * @param    offset: register address
    * @return   data read from register
    */
extern uint16_t btaon_fast_read(uint16_t offset);
extern uint8_t btaon_fast_read_8b(uint16_t offset);

/**
    * @brief    Read data from aon register safely
    * @param    offset: register address
    * @return   data read from register
    */
extern uint16_t btaon_fast_read_safe(uint16_t offset);
extern uint8_t btaon_fast_read_safe_8b(uint16_t offset);
/**
    * @brief    Write data to aon register
    * @param    offset:  register address
    * @param    data:  data to be writen to register
    * @return
    */
extern void btaon_fast_write(uint16_t offset, uint16_t data);
extern void btaon_fast_write_8b(uint16_t offset, uint8_t data);

/**
    * @brief    Write data to aon egister safely
    * @param    offset:  register address
    * @param    data:  data to be writen to register
    * @return
    */
extern void btaon_fast_write_safe(uint16_t offset, uint16_t data);
extern void btaon_fast_write_safe_8b(uint16_t offset, uint8_t data);


/** @} */ /* End of 87x3e_RTL876X_Exported_Functions */


/** @} */ /* End of group 87x3e_RTL876X */

#ifdef __cplusplus
}
#endif
#endif  /* RTL876X_H */

