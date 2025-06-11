/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_i2s.h
* \brief    The header file of the peripheral I2S driver.
* \details  This file provides all I2S firmware functions.
* \author   mh_chang
* \date     2022-10-25
* \version  v1.0
* *********************************************************************************************************
*/
#ifndef _RTL876x_I2S_H
#define _RTL876x_I2S_H
#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup 87x3e_I2S I2S
  * @brief Manage the I2S peripheral functions
  * @{
  */

/*============================================================================*
 *                         Includes
 *============================================================================*/
#include "rtl876x.h"
#include "string.h"

#define TX_CH_DATA_SEL_LEN              4
#define TX_CH_DATA_SEL_MASK             0xF

/*============================================================================*
 *                         Types
 *============================================================================*/
#define I2S0_REG_BASE                   0x40020000UL
#define I2S1_REG_BASE                   0x40021000UL
#define I2S2_REG_BASE                   0x40022000UL

#define TX_FIFO_WR_ADDR_OFFSET          0x0
#define RX_FIFO_RD_ADDR_OFFSET          0x10

#define I2S0_TX_ADDR                    I2S0_REG_BASE + TX_FIFO_WR_ADDR_OFFSET
#define I2S0_RX_ADDR                    I2S0_REG_BASE + RX_FIFO_RD_ADDR_OFFSET
#define I2S1_TX_ADDR                    I2S1_REG_BASE + TX_FIFO_WR_ADDR_OFFSET
#define I2S1_RX_ADDR                    I2S1_REG_BASE + RX_FIFO_RD_ADDR_OFFSET
#define I2S2_RX_ADDR                    I2S2_REG_BASE + RX_FIFO_RD_ADDR_OFFSET

#define I2S0                            ((I2S_TypeDef*) I2S0_REG_BASE)
#define I2S1                            ((I2S_TypeDef*) I2S1_REG_BASE)
#define I2S2                            ((I2S_TypeDef*) I2S2_REG_BASE)

/*============================================================================*
 *                         Types
 *============================================================================*/
/**
 * \defgroup    I2S_Exported_Types Inti Params Struct
 *
 */
/**
 * \brief       I2S initialize parameters.
 *
 */
typedef struct
{
    uint32_t I2S_ClockSource;           /*!< Specifies the I2S clock source.
                                             This parameter can be a value of \ref I2S_Clock_Source */
    uint32_t I2S_BClockMi;              /*!< Specifies the BLCK clock speed. BCLK = 40MHz * (I2S_BClockNi / I2S_BClockMi).
                                             This parameter must range from 1 to 0xffff */
    uint32_t I2S_BClockNi;              /*!< Specifies the BLCK clock speed.
                                             This parameter must range from 1 to 0x7FFF */
    uint32_t I2S_BClockDiv;             /*!< Specifies the BLCK divider ratio. LRCK = BCLK / (I2S_BClockDiv + 1).
                                             This parameter must range from 1 to 0xff */
    uint32_t I2S_DeviceMode;            /*!< Specifies the I2S device mode.
                                             This parameter can be a value of \ref I2S_Device_Mode */
    uint32_t I2S_TxChannelType;         /*!< Specifies the channel type used for the I2S communication.
                                             This parameter can be a value of \ref I2S_Channel_Type */
    uint32_t I2S_RxChannelType;         /*!< Specifies the channel type used for the I2S communication.
                                             This parameter can be a value of \ref I2S_Channel_Type */
    uint32_t I2S_TxDataFormat;          /*!< Specifies the I2S Data format mode.
                                             This parameter can be a value of \ref I2S_Format_Mode */
    uint32_t I2S_RxDataFormat;          /*!< Specifies the I2S Data format mode.
                                             This parameter can be a value of \ref I2S_Format_Mode */
    uint32_t I2S_TxBitSequence;         /*!< Specifies the I2S Data bits sequences.
                                             This parameter can be a value of \ref I2S_Tx_Bit_Sequence */
    uint32_t I2S_RxBitSequence;         /*!< Specifies the I2S Data bits sequences.
                                             This parameter can be a value of \ref I2S_Rx_Bit_Sequence */
    uint32_t I2S_TxDataWidth;           /*!< Specifies the I2S Data width.
                                             This parameter can be a value of \ref I2S_Data_Width */
    uint32_t I2S_RxDataWidth;           /*!< Specifies the I2S Data width.
                                             This parameter can be a value of \ref I2S_Data_Width */
    uint32_t I2S_TxChannelWidth;        /*!< Specifies the I2S Channel width.
                                             This parameter can be a value of \ref I2S_Channel_Width */
    uint32_t I2S_RxChannelWidth;        /*!< Specifies the I2S Channel width.
                                             This parameter can be a value of \ref I2S_Channel_Width */
    uint32_t I2S_TxTdmMode;             /*!< Specifies the I2S TDM mode.
                                             This parameter can be a value of \ref I2S_TDM_Mode */
    uint32_t I2S_RxTdmMode;             /*!< Specifies the I2S TDM mode.
                                             This parameter can be a value of \ref I2S_TDM_Mode */
    uint32_t I2S_DMACmd;                /*!< Specifies the I2S DMA control.
                                             This parameter can be a value of \ref FunctionalState */
    uint32_t I2S_TxWaterlevel;          /*!< Specifies the dma watermark level in transmit mode.
                                             This parameter must range from 1 to 63 */
    uint32_t I2S_RxWaterlevel;          /*!< Specifies the dma watermark level in receive mode.
                                             This parameter must range from 1 to 63 */
} I2S_InitTypeDef;

typedef struct
{
    uint8_t tx_channel_map[8];
    uint8_t rx_fifo_map[8];
} I2S_DataSelTypeDef;

/*============================================================================*
 *                         Constants
 *============================================================================*/
/**
 * \defgroup    I2S_Exported_Constants  Macro Defines
 *
 */
#define IS_I2S_ALL_PERIPH(PERIPH)                   (((PERIPH) == I2S0) || ((PERIPH) == I2S1)))

/**
 * \defgroup    I2S_Clock_Source I2S Clock Source
 * \{
 */
#define I2S_CLK_XTAL                                ((uint32_t)0x04)
/** \} */
#define IS_I2S_CLK_SOURCE(CLK)                      (((CLK) == I2S_CLK_XTAL))

/**
 * \defgroup    I2S_Master_Clock_SEL I2S Master Clock SEL
 * \{
 */
#define I2S_MCLK_DIV4                               ((uint32_t)0x00)
#define I2S_MCLK_DIV2                               ((uint32_t)0x01)
#define I2S_MCLK_DIV_NONE                           ((uint32_t)0x02)
/** \} */
#define IS_I2S_MCLK_DIV(SEL)                        (((SEL) == I2S_MCLK_DIV4) || ((SEL) == I2S_MCLK_DIV2) || \
                                                     ((SEL) == I2S_MCLK_DIV_NONE))

/**
 * \defgroup    I2S_Device_Mode I2S Device Mode
 * \{
 */
#define I2S_DeviceMode_Master                       ((uint32_t)0x00)
#define I2S_DeviceMode_Slave                        ((uint32_t)0x01)
/** \} */
#define IS_I2S_DEVICE_MODE(DEVICE)                  (((DEVICE) == I2S_DeviceMode_Master) || ((DEVICE) == I2S_DeviceMode_Slave))

/**
 * \defgroup    I2S_Mode I2S Mode
 * \{
 */
#define I2S_MODE_TX                                 ((uint32_t)0x01)
#define I2S_MODE_RX                                 ((uint32_t)0x02)
#define I2S_MODE_TRX                                ((I2S_MODE_TX)|(I2S_MODE_RX))
/** \} */
#define IS_I2S_MODE(MODE)                           (((MODE) == I2S_MODE_TX) || ((MODE) == I2S_MODE_RX))

/**
 * \defgroup    I2S_Channel_Type I2S Channel Type
 * \{
 */
#define I2S_Channel_Mono                            ((uint32_t)0x01)
#define I2S_Channel_Stereo                          ((uint32_t)0x00)
/** \} */
#define IS_I2S_CHANNEL_TYPE(TYPE)                   (((TYPE) == I2S_Channel_Mono) || ((TYPE) == I2S_Channel_Stereo))

/**
 * \defgroup    I2S_Format_Mode I2S Format Mode
 * \{
 */
#define I2S_Mode                                    ((uint32_t)0x00)
#define Left_Justified_Mode                         ((uint32_t)0x01)
#define PCM_Mode_A                                  ((uint32_t)0x02)
#define PCM_Mode_B                                  ((uint32_t)0x03)
/** \} */
#define IS_I2S_DATA_FORMAT(FORMAT)                  (((FORMAT) == I2S_Mode) || ((FORMAT) == Left_Justified_Mode) || \
                                                     ((FORMAT) == PCM_Mode_A) || ((FORMAT) == PCM_Mode_B))

/**
 * \defgroup    I2S_Tx_Bit_Sequence I2S Tx Bit Sequence
 * \{
 */
#define I2S_TX_MSB_First                            ((uint32_t)0x00)
#define I2S_TX_LSB_First                            ((uint32_t)0x01)
/** \} */
#define IS_I2S_TX_BIT_SEQ(SEQ)                      (((SEQ) == I2S_TX_MSB_First) || ((SEQ) == I2S_TX_LSB_First))

/**
 * \defgroup    I2S_Rx_Bit_Sequence I2S Rx Bit Sequence
 * \{
 */
#define I2S_RX_MSB_First                            ((uint32_t)0x00)
#define I2S_RX_LSB_First                            ((uint32_t)0x01)
/** \} */
#define IS_I2S_RX_BIT_SEQ(SEQ)                      (((SEQ) == I2S_RX_MSB_First) || ((SEQ) == I2S_RX_LSB_First))

/**
 * \defgroup    I2S_Data_Width I2S Data Width
 * \{
 */
#define I2S_Width_16Bits                            ((uint32_t)0x00)
#define I2S_Width_20Bits                            ((uint32_t)0x01)
#define I2S_Width_24Bits                            ((uint32_t)0x02)
#define I2S_Width_8Bits                             ((uint32_t)0x03)
#define I2S_Width_32Bits                            ((uint32_t)0x04)
/** \} */
#define IS_I2S_DATA_WIDTH(WIDTH)                    (((WIDTH) == I2S_Width_16Bits) || ((WIDTH) == I2S_Width_24Bits) || \
                                                     ((WIDTH) == I2S_Width_8Bits)  || ((WIDTH) == I2S_Width_20Bits) || \
                                                     ((WIDTH) == I2S_Width_32Bits))

/**
 * \defgroup    I2S_DMA_Cmd I2S DMA Cmd
 * \{
 */
#define I2S_DMA_ENABLE                              ((uint32_t)0x00)
#define I2S_DMA_DISABLE                             ((uint32_t)0x01)
/** \} */
#define IS_I2S_DMA_CMD(CMD)                         (((CMD) == I2S_DMA_ENABLE) || ((CMD) == I2S_DMA_DISABLE))

/**
 * \defgroup    I2S_TDM_Mode I2S TDM Mode
 * \{
 */
#define I2S_TDM_DISABLE                             ((uint32_t)0x00)
#define I2S_TDM_MODE_4                              ((uint32_t)0x01)
/** \} */
#define IS_I2S_TDM_MODE(MODE)                       (((MODE) == I2S_TDM_DISABLE) || ((MODE) == I2S_TDM_MODE_4))

/**
 * \defgroup    I2S_Interrupt I2S Interrupt
 * \{
 */
#define I2S_MCU_INT_TX_IDLE                         BIT22
#define I2S_MCU_INT_RX_EMPTY                        BIT21
#define I2S_MCU_INT_TX_EMPTY                        BIT20
#define I2S_MCU_INT_RX_FULL                         BIT19
#define I2S_MCU_INT_TX_FULL                         BIT18
#define I2S_MCU_INT_RX_READY                        BIT17
#define I2S_MCU_INT_TX_READY                        BIT16
/** \} */
#define IS_I2S_MCU_INT_CONFIG(INT)                  (((INT) == I2S_MCU_INT_TX_IDLE) || ((INT) == I2S_MCU_INT_RX_EMPTY) || \
                                                     ((INT) == I2S_MCU_INT_TX_EMPTY) || ((INT) == I2S_MCU_INT_RX_FULL) || \
                                                     ((INT) == I2S_MCU_INT_TX_FULL) || ((INT) == I2S_MCU_INT_RX_READY) || \
                                                     ((INT) == I2S_MCU_INT_TX_READY))

/**
 * \defgroup    I2S_Clear_Interrupt I2S Clear Interrupt
 * \{
 */
#define I2S_CLEAR_INT_RX_READY                      BIT1
#define I2S_CLEAR_INT_TX_READY                      BIT0
/** \} */

typedef enum t_i2s_tx_sel
{
    I2S_TX_FIFO_0_REG_0_L,
    I2S_TX_FIFO_0_REG_0_R,
    I2S_TX_FIFO_0_REG_1_L,
    I2S_TX_FIFO_0_REG_1_R,
    I2S_TX_FIFO_1_REG_0_L,
    I2S_TX_FIFO_1_REG_0_R,
    I2S_TX_FIFO_1_REG_1_L,
    I2S_TX_FIFO_1_REG_1_R,
    I2S_TX_SEL_MAX,
} T_I2S_TX_SEL;

typedef enum t_i2s_rx_sel
{
    I2S_RX_CHANNEL_0,
    I2S_RX_CHANNEL_1,
    I2S_RX_CHANNEL_2,
    I2S_RX_CHANNEL_3,
    I2S_RX_CHANNEL_4,
    I2S_RX_CHANNEL_5,
    I2S_RX_CHANNEL_6,
    I2S_RX_CHANNEL_7,
    I2S_RX_SEL_MAX,
} T_I2S_RX_SEL;

/**
  * @brief I2S
  */
typedef struct
{
    __O uint32_t TX_FIFO_0_WR;          /**<0x00 */
    __IO uint32_t CTL_REG0;             /**<0x04 */
    __IO uint32_t CTL_REG1;             /**<0x08 */
    __IO uint32_t INT_CTLR;             /**<0x0C */
    __I uint32_t RX_FIFO_0_RD;          /**<0x10 */
    __I uint32_t DEPTH_CNT;             /**<0x14 */
    __I uint32_t ERR_CNT;               /**<0x18 */
    __IO uint32_t BCLK;                 /**<0x1C */
    __IO uint32_t BCLK_DIV;             /**<0x20 */
    __I uint32_t STATUS_IND;            /**<0x24 */
    __IO uint32_t CTL_REG2;             /**<0x28 */
    __IO uint32_t RX_BCLK;              /**<0x2C */
    __IO uint32_t INT_CLR_1;            /**<0x30 */
    __I uint32_t READ_CNT;              /**<0x34 */
    __IO uint32_t RSVD_0;               /**<0x38 */
    __IO uint32_t TX_CH_DATA_SEL;       /**<0x3C */
    __IO uint32_t RSVD_1;               /**<0x40 */
    __IO uint32_t RSVD_2;               /**<0x44 */
    __IO uint32_t RSVD_3;               /**<0x48 */
    __IO uint32_t RSVD_4;               /**<0x4C */
    __IO uint32_t RSVD_5;               /**<0x50 */
    __IO uint32_t RX_FIFO_REG0;         /**<0x54 */
    __IO uint32_t RSVD_6;               /**<0x58 */
    __IO uint32_t RX_CNT_CTRL;          /**<0x5C */
    __I uint32_t RX_CNT_VAL;            /**<0x60 */
    __I uint32_t FIFO_CNT_LATCH;        /**<0x64 */
} I2S_TypeDef;

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * \defgroup    I2S_Exported_Functions Peripheral APIs
 * \{
 */
/**
 * \brief   Deinitializes the I2S peripheral registers to their default values.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_i2s_init(void)
 * {
 *     I2S_DeInit(I2S0);
 * }
 * \endcode
 */
void I2S_DeInit(I2S_TypeDef *I2Sx);

/**
 * \brief   Initializes the I2S peripheral according to the specified
 *          parameters in the I2S_InitStruct
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] I2Sx: Selected I2S peripheral.
 * \param[in] I2S_InitStruct: Pointer to a I2S_InitTypeDef structure that
 *            contains the configuration information for the specified I2S peripheral
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_i2s_init(void)
 * {
 *     RCC_PeriphClockCmd(APB_I2S, APB_I2S_CLOCK, ENABLE);
 *
 *     I2S_InitTypeDef I2S_InitStruct;
 *
 *     I2S_StructInit(&I2S_InitStruct);
 *     I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
 *     I2S_InitStruct.I2S_BClockMi         = 0x271;
 *     I2S_InitStruct.I2S_BClockNi         = 0x10;
 *     I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Master;
 *     I2S_InitStruct.I2S_ChannelType      = I2S_Channel_Stereo;
 *     I2S_InitStruct.I2S_DataWidth        = I2S_Width_16Bits;
 *     I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
 *     I2S_InitStruct.I2S_DMACmd           = I2S_DMA_DISABLE;
 *     I2S_Init(I2S0, &I2S_InitStruct);
 *     I2S_Cmd(I2S0, I2S_MODE_TX, ENABLE);
 * }
 * \endcode
 */
void I2S_Init(I2S_TypeDef *I2Sx, I2S_InitTypeDef *I2S_InitStruct);
void I2S_DataSelInit(I2S_TypeDef *I2Sx, I2S_DataSelTypeDef *I2S_DataSelect);

/**
 * \brief   Fills each I2S_InitStruct member with its default value.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] I2S_InitStruct: Pointer to an I2S_InitTypeDef structure which will be initialized.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_i2s_init(void)
 * {
 *     RCC_PeriphClockCmd(APB_I2S, APB_I2S_CLOCK, ENABLE);
 *
 *     I2S_InitTypeDef I2S_InitStruct;
 *
 *     I2S_StructInit(&I2S_InitStruct);
 *     I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
 *     I2S_InitStruct.I2S_BClockMi         = 0x271;
 *     I2S_InitStruct.I2S_BClockNi         = 0x10;
 *     I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Master;
 *     I2S_InitStruct.I2S_ChannelType      = I2S_Channel_Stereo;
 *     I2S_InitStruct.I2S_DataWidth        = I2S_Width_16Bits;
 *     I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
 *     I2S_InitStruct.I2S_DMACmd           = I2S_DMA_DISABLE;
 *     I2S_Init(I2S0, &I2S_InitStruct);
 *     I2S_Cmd(I2S0, I2S_MODE_TX, ENABLE);
 * }
 * \endcode
 */
void I2S_StructInit(I2S_InitTypeDef *I2S_InitStruct);
void I2S_DataSelStructInit(I2S_DataSelTypeDef *I2S_DataSelect);

/**
 * \brief   Enable or disable the selected I2S mode.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] I2Sx: Selected I2S peripheral.
 * \param[in] mode: Selected I2S operation mode.
 *      This parameter can be the following values:
 *      \arg I2S_MODE_TX: Transmission mode.
 *      \arg I2S_MODE_RX: Receiving mode.
        \arg I2S_MODE_TRX: Transmission & Receiving mode.
 * \param[in] NewState: New state of the operation mode.
 *      This parameter can be: ENABLE or DISABLE.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_i2s_init(void)
 * {
 *     RCC_PeriphClockCmd(APB_I2S, APB_I2S_CLOCK, ENABLE);
 *
 *     I2S_InitTypeDef I2S_InitStruct;
 *
 *     I2S_StructInit(&I2S_InitStruct);
 *     I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
 *     I2S_InitStruct.I2S_BClockMi         = 0x271;
 *     I2S_InitStruct.I2S_BClockNi         = 0x10;
 *     I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Master;
 *     I2S_InitStruct.I2S_ChannelType      = I2S_Channel_Stereo;
 *     I2S_InitStruct.I2S_DataWidth        = I2S_Width_16Bits;
 *     I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
 *     I2S_InitStruct.I2S_DMACmd           = I2S_DMA_DISABLE;
 *     I2S_Init(I2S_NUM, &I2S_InitStruct);
 *     I2S_Cmd(I2S_NUM, I2S_MODE_TX, ENABLE);
 * }
 * \endcode
 */
void I2S_Cmd(I2S_TypeDef *I2Sx, uint32_t mode, FunctionalState NewState);

/**
 * \brief   Enable or disable the specified I2S interrupt source.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] I2S_INT: Specifies the I2S interrupt source to be enable or disable.
 *      This parameter can be the following values:
 *      \arg I2S_INT_TX_IDLE: Transmit idle interrupt source.
 *      \arg I2S_INT_RF_EMPTY: Receive FIFO empty interrupt source.
 *      \arg I2S_INT_TF_EMPTY: Transmit FIFO empty interrupt source.
 *      \arg I2S_INT_RF_FULL: Receive FIFO full interrupt source.
 *      \arg I2S_INT_TF_FULL: Transmit FIFO full interrupt source.
 *      \arg I2S_INT_RX_READY: Ready to receive interrupt source.
 *      \arg I2S_INT_TX_READY: Ready to transmit interrupt source.
 * \param[in]  NewState: New state of the specified I2S interrupt.
 *      This parameter can be: ENABLE or DISABLE.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void i2s_demo(void)
 * {
 *     I2S_INTConfig(I2S0, I2S_INT_TF_EMPTY, ENABLE);
 * }
 * \endcode
 */
void I2S_INTConfig(I2S_TypeDef *I2Sx, uint32_t I2S_MCU_INT, FunctionalState NewState);

/**
 * \brief   Get the specified I2S interrupt status.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] I2S_INT: the specified I2S interrupt.
 *      This parameter can be one of the following values:
 *      \arg I2S_INT_TX_IDLE: Transmit idle interrupt.
 *      \arg I2S_INT_RF_EMPTY: Receive FIFO empty interrupt.
 *      \arg I2S_INT_TF_EMPTY: Transmit FIFO empty interrupt.
 *      \arg I2S_INT_RF_FULL: Receive FIFO full interrupt.
 *      \arg I2S_INT_TF_FULL: Transmit FIFO full interrupt.
 *      \arg I2S_INT_RX_READY: Ready to receive interrupt.
 *      \arg I2S_INT_TX_READY: Ready to transmit interrupt.
 * \retval The new state of I2S_INT (SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void i2s_demo(void)
 * {
 *     ITStatus int_status = I2S_GetINTStatus(I2S0, I2S_INT_TF_EMPTY);
 * }
 * \endcode
 */
ITStatus I2S_GetINTStatus(I2S_TypeDef *I2Sx, uint32_t I2S_INT);

/**
  * @brief  Clears the I2S interrupt pending bits.
  * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
  *
  * @param  I2S_CLEAR_INT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg I2S_CLEAR_INT_RX_READY: Clear ready to receive interrupt.
  *     @arg I2S_CLEAR_INT_TX_READY: Clear ready to transmit interrupt.
  * @retval None
  */
void I2S_ClearINTPendingBit(I2S_TypeDef *I2Sx, uint32_t I2S_CLEAR_INT);

/*
 * @brief  Select I2S connect to external codec or internal codec.
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in]  NewState: New state of the I2S bridge selection.
 *      This parameter can be: ENABLE or DISABLE.
 * @retval None
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void i2s_demo(void)
 * {
 *     I2S_CodecSelInternal(I2S0, ENABLE);
 * }
 * \endcode
 */
void I2S_CodecSelInternal(I2S_TypeDef *I2Sx, FunctionalState newState);

/**
  * @brief  Transmits a Data through the I2Sx peripheral.
  * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
  *
  * @param  I2Sx: To select the I2Sx peripheral, x can be 0 or 1.
  * @param  Data: Data to be transmitted.
  * @retval None
  */
__STATIC_INLINE void I2S_SendData(I2S_TypeDef *I2Sx, uint32_t Data)
{
    /* Check the parameters */
    assert_param(IS_I2S_ALL_PERIPH(I2Sx));

    I2Sx->TX_FIFO_0_WR = Data;
}

/**
  * @brief  Returns the most recent received data by the I2Sx peripheral.
  * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
  *
  * @param  I2Sx: To select I2Sx peripheral, where x can be: 0 or 1.
  * @retval The value of the received data.
  */
__STATIC_INLINE uint32_t I2S_ReceiveData(I2S_TypeDef *I2Sx)
{
    /* Check the parameters */
    assert_param(IS_I2S_ALL_PERIPH(I2Sx));

    return I2Sx->RX_FIFO_0_RD;
}

/**
  * @brief  Returns the transmit FIFO free length by the I2Sx peripheral.
  * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
  *
  * @param  I2Sx: To select I2Sx peripheral, where x can be: 0 or 1.
  * @retval The free length of the transmit FIFO.
  */
__STATIC_INLINE uint8_t I2S_GetTxFIFODepth(I2S_TypeDef *I2Sx)
{
    /* Check the parameters */
    assert_param(IS_I2S_ALL_PERIPH(I2Sx));

    return ((I2Sx->DEPTH_CNT & 0x3F));
}

/**
  * @brief  Returns the receive FIFO data length by the I2Sx peripheral.
  * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
  *
  * @param  I2Sx: To select I2Sx peripheral, where x can be: 0 or 1.
  * @retval The data length of the receive FIFO.
  */
__STATIC_INLINE uint8_t I2S_GetRxFIFOLen(I2S_TypeDef *I2Sx)
{
    /* Check the parameters */
    assert_param(IS_I2S_ALL_PERIPH(I2Sx));

    return ((I2Sx->DEPTH_CNT & 0x3F00) >> 8);
}

/** \} */ /*End of group I2S_Exported_Functions */
/** @} */ /* End of group 87x3e_I2S */

#ifdef __cplusplus
}
#endif
#endif /* _RTL876x_I2S_H_ */
/******************* (C) COPYRIGHT 2022 Realtek Semiconductor Corporation *****END OF FILE****/

