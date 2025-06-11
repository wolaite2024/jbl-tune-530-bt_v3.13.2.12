/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_sdio.h
* @brief     header file of sdio driver.
* @details
* @author   elliot chen
* @date     2017-01-03
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876x_SDIO_H_
#define _RTL876x_SDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

/* Peripheral: SDIO */
/* Description: SDIO register defines */

/* Register: TF_MODE --------------------------------------------------*/
/* Description: Transfer mode Register. Offset: 0x0C. Address: 0x4005C00CUL. */
/* TF_MODE[5] :SDIO_MULTI_BLOCK. */
#define SDIO_MULTI_BLOCK_EN                 BIT(5)
#define SDIO_AUTO_CMD12_EN                  BIT(2)
#define SDIO_BLOCK_COUNT_EN                 BIT(1)

/* Register: PRESENT_STATE --------------------------------------------------*/
/* Description: Present State Register. Offset: 0x24. Address: 0x4005C024UL. */
/* PRESENT_STATE[1] :SDIO_DAT_INHIBIT. */
#define SDIO_DAT_INHIBIT                    BIT(1)
/* PRESENT_STATE[0] :SDIO_CMD_INHIBIT. */
#define SDIO_CMD_INHIBIT                    BIT(0)

/* Register: PWR_CTRL ------------------------------------------------------*/
/* Description: Power control register. Offset: 0x29. Address: 0x4005C029UL. */
/* PWR_CTRL[3:1] :SDIO_BUS_VOLTAGE. */
#define SDIO_BUS_VOLTAGE_POS                (1UL)
#define SDIO_BUS_VOLTAGE_MASK               (0x1UL << SDIO_BUS_VOLTAGE_POS)
#define SDIO_BUS_VOLTAGE_CLR                (~SDIO_BUS_VOLTAGE_MASK)
#define SDIO_VOLT_3_3V                      ((uint32_t)7 << SDIO_BUS_VOLTAGE_POS)
#define SDIO_VOLT_3_0V                      ((uint32_t)6 << SDIO_BUS_VOLTAGE_POS)
#define SDIO_VOLT_1_8V                      ((uint32_t)5 << SDIO_BUS_VOLTAGE_POS)
/* PWR_CTRL[0] :SDIO_BUS_PWR. 0x1: Power on. 0x0: Power down. */
#define SDIO_BUS_PWR_POS                    (0UL)
#define SDIO_BUS_PWR_MASK                   (0x1UL << SDIO_BUS_PWR_POS)
#define SDIO_BUS_PWR_CLR                    (~SDIO_BUS_PWR_MASK)

/* Register: PESOC_CLK_CTRL -------------------------------------------------------*/
/* Description: clock control register. Offset: 0x2C. Address: 0x4005C02CUL. */
/* PESOC_CLK_CTRL[1] :SDIO_INTERNAL_CLOCK_STABLE. 0x1: Enable. 0x0: Disable. */
#define SDIO_INTERNAL_CLK_STABLE_POS        (1UL)
#define SDIO_INTERNAL_CLK_STABLE_MASK       (0x1UL << SDIO_INTERNAL_CLK_STABLE_POS)
#define SDIO_INTERNAL_CLK_STABLE_CLR        (~SDIO_INTERNAL_CLK_STABLE_MASK)
/* CLK_CTRL[0] :SDIO_INTERNAL_CLOCK. 0x1: Enable. 0x0: Disable. */
#define SDIO_INTERNAL_CLOCK_POS             (0UL)
#define SDIO_INTERNAL_CLOCK_MASK            (0x1UL << SDIO_INTERNAL_CLOCK_POS)
#define SDIO_INTERNAL_CLOCK_CLR             (~SDIO_INTERNAL_CLOCK_MASK)

/* Register: SW_RESET -------------------------------------------------------*/
/* Description: Software reset register. Offset: 0x2F. Address: 0x4005C02FUL. */
/* SW_RESET[0] :SDIO_SF_RESET. 0x1: Reset. 0x0: Work. */
#define SDIO_SF_RESET_POS                   (0UL)
#define SDIO_SF_RESET_MASK                  (0x1UL << SDIO_SF_RESET_POS)
#define SDIO_SF_RESET_CLR                   (~SDIO_SF_RESET_MASK)

/* Register: CAPABILITIES_L ---------------------------------------------------*/
/* Description: Capabilities Register. Offset: 0x40. Address: 0x4005C040UL. */
#define SDIO_CAPA_TIMEOUT_CLK_UNIT          BIT(7)
#define SDIO_CAPA_ADMA2_SUPPORT             BIT(19)
#define SDIO_CAPA_HIGH_SPEED_SUPPORT        BIT(21)
#define SDIO_CAPA_VOLT_SUPPORT_33V          BIT(24)
#define SDIO_CAPA_VOLT_SUPPORT_30V          BIT(25)
#define SDIO_CAPA_VOLT_SUPPORT_18V          BIT(26)


/** @addtogroup 87x3e_SDIO SDIO
  * @brief SDIO driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup 87x3e_SDIO_Exported_Types SDIO Exported Types
  * @{
  */

/**
 * @brief SDIO initialize parameters
 *
 *
 */
typedef struct
{
    uint32_t SDIO_ClockDiv;         /*!< Specifies the clock frequency of the SDIO controller.
                                    This parameter can be a value of @ref SDIO_ClockDiv*/

    uint32_t SDIO_TimeOut;          /*!< Specifies whether the SDIO time out.
                                    This parameter must range from 0 to 14. */

    uint32_t SDIO_BusWide;          /*!< Specifies the SDIO bus width. */

} SDIO_InitTypeDef;

/**
 * @brief SDIO command initialize parameters
 *
 *
 */
typedef struct
{
    uint32_t SDIO_Argument;         /*!< Specifies the command argument */

    uint32_t SDIO_CmdIndex;         /*!< Specifies the command number.
                                    This parameter can be a value of 0 to 63*/

    uint32_t SDIO_CmdType;          /*!< Specifies the command type.
                                    This parameter can be a value of @ref SDIO_CMD_TYPE*/

    uint32_t SDIO_DataPresent;      /*!< Specifies whether to present data using DAT line.
                                    This parameter can be a value of @ref DATA_PRESENT_SEL*/

    uint32_t SDIO_CmdIdxCheck;      /*!< Specifies whether the host controller shall check index field.
                                    This parameter can be a value of ENABLE or DISABLE*/

    uint32_t SDIO_CmdCrcCheck;      /*!< Specifies whether the host controller shall check crc field.
                                    This parameter can be a value of ENABLE or DISABLE*/

    uint32_t SDIO_ResponseType;     /*!< Specifies response type.
                                    This parameter can be a value of @ref SDIO_RSP_TYPE*/

} SDIO_CmdInitTypeDef;

/**
 * @brief SDIO data transmission initialize parameters
 *
 *
 */
typedef struct
{
    uint32_t SDIO_Address;      /*!< Specifies the start address of the Descriptor table.
                                    This parameter can be a value 4 aligned address */

    uint32_t SDIO_BlockSize;    /*!< Specifies the block size of data trnasfers for CMD17,
                                    CMD18, CMD24, CMD25 and CMD53, in the case of high capacity
                                    memory card, the size of data block is fixed to 512 bytes.
                                    This parameter can be a value of 0 to 0x800 */

    uint32_t SDIO_BlockCount;   /*!< Specifies the block count for current transfer.
                                    This parameter can be a value of 0 to 0xffff */

    uint32_t SDIO_TransferDir;  /*!< Specifies the data transfer direction of DAT line data transfers.
                                    This parameter can be a value of @ref IS_SDIO_TRANSFER_DIR*/

    uint32_t SDIO_TransferMode; /*!< Specifies the transfer mode. */

    uint32_t SDIO_DMAEn;        /*!< Specifies whether to enable dma functionlity.
                                    This parameter can be a value of ENABLE or DISABLE */

} SDIO_DataInitTypeDef;

/**
 * @brief ADMA2 descriptor attribute initialize parameters
 *
 *
 */
typedef struct
{
    uint16_t SDIO_Valid: 1;
    uint16_t SDIO_End: 1;
    uint16_t SDIO_Int: 1;
    uint16_t SDIO_Rsvd1: 1;
    uint16_t SDIO_Act1: 1;
    uint16_t SDIO_Act2: 1;
    uint16_t SDIO_Rsvd2: 10;
} ADMA2_AttrTypedef;

/**
 * @brief ADMA2 descriptor table initialize parameters
 *
 *
 */
typedef struct
{
    ADMA2_AttrTypedef SDIO_Attribute;
    uint16_t SDIO_Length;
    uint32_t SDIO_Address;
} SDIO_ADMA2TypeDef;

/** End of group 87x3e_SDIO_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup 87x3e_SDIO_Exported_Constants SDIO Exported Constants
  * @{
  */

#define IS_SDIO_PERIPH(PERIPH) ((PERIPH) == SDIO)

/** @defgroup 87x3e_SDIO_Pin_Group SDIO Pin Group
  * @{
  */

#define SDIO_PinGroup_0                     ((uint32_t)0x00000000)
#define SDIO_PinGroup_1                     ((uint32_t)0x00000001)

#define IS_SDIO_PINGROUP_IDX(IDX) (((IDX) == SDIO_PinGroup_0) || ((IDX) == SDIO_PinGroup_1))

/** End of group 87x3e_SDIO_Pin_Group
  * @}
  */

/** @defgroup 87x3e_SDIO_Power_State SDIO Power state
  * @{
  */

#define SDIO_PowerState_OFF                 ((uint32_t)0x00000000)
#define SDIO_PowerState_ON                  ((uint32_t)0x00000001)

#define IS_SDIO_POWER_STATE(STATE) (((STATE) == SDIO_PowerState_OFF) || ((STATE) == SDIO_PowerState_ON))

/** End of group 87x3e_SDIO_Power_State
  * @}
  */

/** @defgroup S87x3e_DIO_ClockDiv SDIO ClockDiv
  * @{
  */

#define SDIO_CLOCK_DIV_1                    ((uint32_t)((0x0) << 8))
#define SDIO_CLOCK_DIV_2                    ((uint32_t)((0x1) << 8))
#define SDIO_CLOCK_DIV_4                    ((uint32_t)((0x2) << 8))
#define SDIO_CLOCK_DIV_8                    ((uint32_t)((0x4) << 8))
#define SDIO_CLOCK_DIV_16                   ((uint32_t)((0x8) << 8))
#define SDIO_CLOCK_DIV_32                   ((uint32_t)((0x10) << 8))
#define SDIO_CLOCK_DIV_64                   ((uint32_t)((0x20) << 8))
#define SDIO_CLOCK_DIV_128                  ((uint32_t)((0x40) << 8))
#define SDIO_CLOCK_DIV_256                  ((uint32_t)((0x80) << 8))

#define IS_SDIO_CLOCK_DIV(CTRL)         (((CTRL) == SDIO_CLOCK_DIV_1) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_2) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_4) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_8) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_16) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_32) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_64) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_128) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_256))

/** End of group 87x3e_SDIO_ClockDiv
  * @}
  */

/** @defgroup 87x3e_SDIO_BusWide SDIO bus Wide
  * @{
  */

#define SDIO_BusWide_1b                     ((uint32_t)0x00)
#define SDIO_BusWide_4b                     ((uint32_t)0x02)

#define IS_SDIO_BUS_WIDE(WIDE) (((WIDE) == SDIO_BusWide_1b) || ((WIDE) == SDIO_BusWide_4b))

/** End of group 87x3e_SDIO_BusWide
  * @}
  */

/** @defgroup 87x3e_SDIO_TimeOut SDIO TimeOut
  * @{
  */

#define IS_SDIO_TIME_OUT(TIME) ((TIME) <= 0x0E)

/** End of group 87x3e_SDIO_TimeOut
  * @}
  */

/** @defgroup 87x3e_SDIO_Block_Size SDIO Block Size
  * @{
  */

#define IS_SDIO_BLOCK_SIZE(SIZE)            ((SIZE) <= 0x800)

/** End of group 87x3e_SDIO_Block_Size
  * @}
  */

/** @defgroup 87x3e_SDIO_Block_COUNT SDIO Block Count
  * @{
  */

#define IS_SDIO_BLOCK_COUNT(COUNT)          ((COUNT) <= 0xFFFF)

/** End of group 87x3e_SDIO_Block_COUNT
  * @}
  */

/** @defgroup 87x3e_SDIO_Transfer_Direction
  * @{
  */

#define SDIO_TransferDir_READ               ((uint32_t)1 << 4)
#define SDIO_TransferDir_WRITE              ((uint32_t)0x00)
#define IS_SDIO_TRANSFER_DIR(DIR) (((DIR) == SDIO_TransferDir_READ) || \
                                   ((DIR) == SDIO_TransferDir_WRITE))
/** End of group 87x3e_SDIO_Transfer_Direction
  * @}
  */

/** @defgroup 87x3e_SDIO_Command_Index
  * @{
  */

#define IS_SDIO_CMD_INDEX(INDEX)            ((INDEX) < 0x40)

/** End of group 87x3e_SDIO_Command_Index
  * @}
  */

/** @defgroup 87x3e_SDIO_RSP_TYPE
  * @{
  */

typedef enum
{
    NORMAL,
    SUSPEND,
    RESUME,
    ABORT
} SDIO_CMD_TYPE;

/** End of group 87x3e_SDIO_RSP_TYPE
  * @}
  */

/** @defgroup 87x3e_SDIO_RSP_TYPE
  * @{
  */

typedef enum
{
    NO_DATA,
    WITH_DATA
} DATA_PRESENT_SEL;

/** End of group 87x3e_SDIO_RSP_TYPE
  * @}
  */

/** @defgroup 87x3e_SDIO_RSP_TYPE
  * @{
  */

typedef enum
{
    SDIO_Response_No,
    RSP_LEN_136,
    RSP_LEN_48,
    RSP_LEN_48_CHK_BUSY
} SDIO_RSP_TYPE;

/** End of group 87x3e_SDIO_RSP_TYPE
  * @}
  */

/** @defgroup 87x3e_SDIO_Interrupts_definition SDIO Interrupt Definition
  * @{
  */
#define SDIO_INT_ERROR                      ((uint32_t)1 << 15)
#define SDIO_INT_CARD                       ((uint32_t)1 << 8)
#define SDIO_CARD_REMOVAL                   ((uint32_t)1 << 7)
#define SDIO_CARD_INSERT                    ((uint32_t)1 << 6)
#define SDIO_BUF_READ_RDY                   ((uint32_t)1 << 5)
#define SDIO_BUF_WRITE_RDY                  ((uint32_t)1 << 4)
#define SDIO_INT_DMA                        ((uint32_t)1 << 3)
#define SDIO_BLOCK_GAP_EVENT                ((uint32_t)1 << 2)
#define SDIO_INT_TF_CMPL                    ((uint32_t)1 << 1)
#define SDIO_INT_CMD_CMPL                   ((uint32_t)1 << 0)

#define IS_SDIO_INT(INT) (((INT)  == SDIO_INT_ERROR)        || \
                          ((INT)  == SDIO_INT_CARD)       || \
                          ((INT)  == SDIO_INT_DMA)        || \
                          ((INT)  == SDIO_INT_TF_CMPL)    || \
                          ((INT)  == SDIO_INT_CMD_CMPL))

/** End of group 87x3e_SDIO_Interrupts_definition
  * @}
  */

/** @defgroup 87x3e_SDIO_Interrupts_definition SDIO Interrupt Definition
  * @{
  */
#define SDIO_INT_VENDOR_SPECIFIC_ERR        ((uint32_t)0xF << 12)
#define SDIO_INT_ADMA_ERR                   ((uint32_t)1 << 9)
#define SDIO_INT_AUTO_CMD12_ERR             ((uint32_t)1 << 8)
#define SDIO_INT_CURRENT_LIMIT_ERR          ((uint32_t)1 << 7)
#define SDIO_INT_DAT_END_BIT_ERR            ((uint32_t)1 << 6)
#define SDIO_INT_DAT_CRC_ERR                ((uint32_t)1 << 5)
#define SDIO_INT_DAT_TIMEOUT_ERR            ((uint32_t)1 << 4)
#define SDIO_INT_CMD_INDEX_ERR              ((uint32_t)1 << 3)
#define SDIO_INT_CMD_END_BIT_ERR            ((uint32_t)1 << 2)
#define SDIO_INT_CMD_CRC_ERR                ((uint32_t)1 << 1)
#define SDIO_INT_CMD_TIMEOUT_ERR            ((uint32_t)1 << 0)

#define IS_SDIO_ERR_INT(INT) (((INT)  == SDIO_INT_VENDOR_SPECIFIC_ERR)  || \
                              ((INT)  == SDIO_INT_ADMA_ERR)           || \
                              ((INT)  == SDIO_INT_AUTO_CMD12_ERR)     || \
                              ((INT)  == SDIO_INT_CURRENT_LIMIT_ERR)  || \
                              ((INT)  == SDIO_INT_DAT_END_BIT_ERR)    || \
                              ((INT)  == SDIO_INT_DAT_CRC_ERR)        || \
                              ((INT)  == SDIO_INT_DAT_TIMEOUT_ERR)    || \
                              ((INT)  == SDIO_INT_CMD_INDEX_ERR)      || \
                              ((INT)  == SDIO_INT_CMD_END_BIT_ERR)    || \
                              ((INT)  == SDIO_INT_CMD_CRC_ERR)        || \
                              ((INT)  == SDIO_INT_CMD_TIMEOUT_ERR))

/** End of group 87x3e_SDIO_Interrupts_definition
  * @}
  */

/** @defgroup 87x3e_SDIO_Wake_Up_definition SDIO Wake Up Definition
  * @{
  */

#define SDIO_WAKE_UP_SDCARD_REMOVAL         ((uint32_t)1 << 2)
#define SDIO_WAKE_UP_SDCARD_INSERT          ((uint32_t)1 << 1)
#define SDIO_WAKE_UP_SDCARD_INTR            ((uint32_t)1 << 0)


#define IS_SDIO_WAKE_UP(EVT) (((EVT)  == SDIO_WAKE_UP_SDCARD_REMOVAL)   || \
                              ((EVT)  == SDIO_WAKE_UP_SDCARD_INSERT)  || \
                              ((EVT)  == SDIO_WAKE_UP_SDCARD_INTR))

/** End of group 87x3e_SDIO_Wake_Up_definition
  * @}
  */

/** @defgroup 87x3e_SDIO_Flag SDIO Flag
  * @{
  */

#define SDIO_FLAG_BUF_READ_EN               ((uint32_t)1 << 11)
#define SDIO_FLAG_WRITE_BUF_EN              ((uint32_t)1 << 10)
#define SDIO_FLAG_READ_TF_ACTIVE            ((uint32_t)1 << 9)
#define SDIO_FLAG_WRITE_TF_ACTIVE           ((uint32_t)1 << 8)
#define SDIO_FLAG_DAT_LINE_ACTIVE           ((uint32_t)1 << 2)
#define SDIO_FLAG_CMD_DAT_INHIBIT           ((uint32_t)1 << 1)
#define SDIO_FLAG_CMD_INHIBIT               ((uint32_t)1 << 0)

#define IS_SDIO_FLAG(FLAG) (((FLAG)  == SDIO_FLAG_BUF_READ_EN)      || \
                            ((FLAG)  == SDIO_FLAG_WRITE_BUF_EN)     || \
                            ((FLAG)  == SDIO_FLAG_READ_TF_ACTIVE)   || \
                            ((FLAG)  == SDIO_FLAG_WRITE_TF_ACTIVE)  || \
                            ((FLAG)  == SDIO_FLAG_DAT_LINE_ACTIVE)  || \
                            ((FLAG)  == SDIO_FLAG_CMD_DAT_INHIBIT)  || \
                            ((FLAG)  == SDIO_FLAG_CMD_INHIBIT))

/** End of group 87x3e_SDIO_Flag
  * @}
  */

/** @defgroup 87x3e_SDIO_RSP_REG SDIO Response Register
  * @{
  */

#define SDIO_RSP0                          ((uint32_t)0x00000000)
#define SDIO_RSP2                          ((uint32_t)0x00000004)
#define SDIO_RSP4                          ((uint32_t)0x00000008)
#define SDIO_RSP6                          ((uint32_t)0x0000000C)

/** End of group 87x3e_SDIO_RSP_REG
  * @}
  */

/** End of group 87x3e_SDIO_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup 87x3e_SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */

/**
  * @brief  Enables or disables the SDIO output pin.
  * @param  NewState: new state of the SDIO output pin.
  *   This parameter can be the following values:
  *     @arg ENABLE:  enable sdio pin output.
  *     @arg DISABLE: disable sdio pin output.
  * @retval None
  */
void SDIO_PinOutputCmd(FunctionalState NewState);

/**
  * @brief  Select the SDIO output pin group.
  * @param  SDIO_PinGroupType:
  *  This parameter can be one of the following values:
  *  @arg SDIO_PinGroup_0 : SDH_CLK(P1_2), SDH_CMD(P1_3), SDH_WT_PROT(P3_0),SDH_CD(P3_1),
  *                         SDH_D0(P1_4), SDH_D1(P1_5), SDH_D2(P1_6), SDH_D3(P1_7)
  *  @arg SDIO_PinGroup_1:  SDH_CLK(P2_0), SDH_CMD(P2_1), SDH_WT_PROT(P2_2),SDH_CD(P2_3),
  *                         SDH_D0(P2_4), SDH_D1(P2_5), SDH_D2(P2_6), SDH_D3(P2_7)
  * @retval None
  */
void SDIO_PinGroupConfig(uint32_t SDIO_PinGroupType);

/**
  * @brief  Software reset host controller.
  * @param  void
  * @retval None
  */
void SDIO_SoftwareReset(void);

/**
  * @brief  Enables or disables the SDIO internal Clock.
  * @param  NewState: new state of the SDIO internal Clock.
  *   This parameter can be the following values:
  *     @arg ENABLE:  enable sdio internal clock.
  *     @arg DISABLE: disable sdio internal clock.
  * @retval None
  */
void SDIO_InternalClockCmd(FunctionalState NewState);

/**
  * @brief  Get Software reset status.
  * @param  void
  * @retval The new state of sdio software reset progress status(SET or RESET).
  */
FlagStatus SDIO_GetSoftwareResetStatus(void);

/**
  * @brief  Get internal clock stable or not.
  * @param  void
  * @retval The new state of sdio internal clock ready status(SET or RESET).
  */
FlagStatus SDIO_GetInternalClockStatus(void);

/**
  * @brief  Checks whether the specified SDIO flag is set or not.
  * @param  SDIO_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *       @arg SDIO_FLAG_BUF_READ_EN:This status is used for non-DMA read transfer.
  *              SET: Have data in buffer when block data is ready in the buffer.
  *              RESET:  No Data.
  *              Supplement: A change of this bit from 0 to 1 occurs when block data is ready
  *              in the buffer and generates the Buffer Read Interrupt.
  *       @arg SDIO_FLAG_WRITE_BUF_EN:This status is used for non-DMA write transfer.
  *              SET: can write in non-DMA write transfers.
  *              RESET:  can not write in non-DMA write transfers.
  *              Supplement: A change of this bit from 0 to 1 occurs when top of block data can be written to the buffer
  *              and generates the Buffer Write Ready Interrupt.
  *       @arg SDIO_FLAG_READ_TF_ACTIVE:
  *              SET: Transferring data in a read transfer.
  *              RESET:  No valid data in a read transfer.
  *       @arg SDIO_FLAG_WRITE_TF_ACTIVE:
  *              SET: Transferring data in a write transfer.
  *              RESET:  No valid data in a write transfer.
  *       @arg SDIO_FLAG_DAT_LINE_ACTIVE:
  *              SET: DAT line active
  *              RESET:  DAT line inactive
  *       @arg SDIO_FLAG_CMD_DAT_INHIBIT:
  *              SET: Can not issue command which uses the DAT line
  *              RESET: Can issue command which uses the DAT line
  *       @arg SDIO_FLAG_CMD_INHIBIT
  *              SET: Can not issue command
  *              RESET: Can issue command using only CMD line
  * @retval The new state of SDIO_FLAG (SET or RESET).
  */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG);

/**
  * @brief  Sets the SD bus power status of the controller.
  * @param  SDIO_PowerState: new state of the Power state.
  *   This parameter can be one of the following values:
  *   @arg SDIO_PowerState_OFF
  *   @arg SDIO_PowerState_ON
  * @retval None
  */
void SDIO_SetBusPower(uint32_t SDIO_PowerState);

/**
  * @brief Initializes the SDIO peripheral according to the specified
  *   parameters in the SDIO_InitStruct.
  * @param  SDIO_InitStruct: pointer to a SDIO_InitTypeDef structure that
  *   contains the configuration information for the specified SDIO peripheral.
  * @retval None
  */
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct);

/**
  * @brief  Fills each SDIO_InitStruct member with its default value.
  * @param  SDIO_InitStruct: pointer to an SDIO_InitTypeDef structure which will be initialized.
  * @retval None
  */
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct);

/**
  * @brief  Initializes the SDIO data path according to the specified
  *      parameters in the SDIO_DataInitStruct.
  * @param  SDIO_DataInitStruct : pointer to a SDIO_DataInitTypeDef
  *   structure that contains the configuration information for the SDIO command.
  * @retval None
  */
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct);

/**
  * @brief  Fills each SDIO_DataInitStruct member with its default value.
  * @param  SDIO_DataInitStruct: pointer to an SDIO_DataInitTypeDef structure which
  *         will be initialized.
  * @retval None
*/
void SDIO_DataStructInit(SDIO_DataInitTypeDef *SDIO_DataInitStruct);

/**
 * @brief  Configure SD clock.
 * @param  clock_div: value of SDIO clock divider.
 * @retval none.
 */
void SDIO_SetClock(uint32_t clock_div);

/**
  * @brief  Configure SDIO clock.
  * @param SDIO_BusWide: value of SDIO bus wide.
  *   This parameter can be the following values:
  *   @arg SDIO_BusWide_1b: 1-bit mode.
  *   @arg SDIO_BusWide_4b: 4-bit mode.
  * @retval none.
  */
void SDIO_SetBusWide(uint32_t SDIO_BusWide);

/**
  * @brief  Set start address of the descriptor table.
  * @param address: start address of the descriptor table.
  * @retval none.
  */
void SDIO_SetSystemAddress(uint32_t address);

/**
  * @brief  Set block size.
  * @param BlockSize:  block size.
  * @retval none.
  */
void SDIO_SetBlockSize(uint32_t BlockSize);

/**
  * @brief  Set block count.
  * @param BlockCount:  block count.
  * @retval none.
  */
void SDIO_SetBlockCount(uint32_t BlockCount);

/**
  * @brief  Initializes the SDIO Command according to the specified
  *         parameters in the SDIO_CmdInitStruct and send the command.
  * @param  SDIO_CmdInitStruct : pointer to a SDIO_CmdInitTypeDef
  *         structure that contains the configuration information for the SDIO command.
  * @retval None
  */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);

/**
  * @brief  Fills each SDIO_CmdInitStruct member with its default value.
  * @param  SDIO_CmdInitStruct: pointer to an SDIO_CmdInitTypeDef structure which
  *         will be initialized.
  * @retval None
*/
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);

/**
  * @brief  Read data through the SDIO peripheral.
  * @param  void
  * @retval  The value of the received data.
  */
uint32_t SDIO_ReadData(void);

/**
  * @brief  Write data through the SDIO peripheral.
  * @param  Data: Data to be transmitted.
  * @retval  none.
  */
void SDIO_WriteData(uint32_t Data);

/**
  * @brief  Enables or disables the specified SDIO interrupt status.
  * @param  SDIO_INTStatus: specifies the IR interrupt status to be enabled or disabled.
  *   This parameter can be the following values:
  *      @arg SDIO_INT_CARD: card interrupt status.
  *      @arg SDIO_INT_DMA: DMA interrupt status.
  *      @arg SDIO_INT_TF_CMPL: transfer complete interrupt status.
  *      @arg SDIO_INT_CMD_CMPL: command complete interrupt status.
  * @param  newState: new state of the specified SDIO interrupt status.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SDIO_INTStatusConfig(uint32_t SDIO_INTStatus, FunctionalState newState);

/**
  * @brief  Enables or disables the specified SDIO interrupts.
  * @param  SDIO_INT: specifies the IR interrupts sources to be enabled or disabled.
  *   This parameter can be the following values:
  *      @arg SDIO_INT_ERROR: Error interrupt signal caused by any type error in error interrupt status register
  *           which can trigger interrupt to NVIC.
  *      @arg SDIO_INT_CARD: card interrupt signal which can trigger interrupt to NVIC.
  *      @arg SDIO_INT_DMA: DMA interrupt signal which can trigger interrupt to NVIC.
  *      @arg SDIO_INT_TF_CMPL: transfer interrupt interrupt signal which can trigger interrupt to NVIC.
  *      @arg SDIO_INT_CMD_CMPL: command complete signal interrupt which can trigger interrupt to NVIC.
  * @param  newState: new state of the specified SDIO interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SDIO_INTConfig(uint32_t SDIO_INT, FunctionalState newState);

/**
  * @brief  Enables or disables the specified SDIO error interrupt status.
  * @param  SDIO_INTStatus: specifies the SDIO error interrupts status to be enabled or disabled.
  *   This parameter can be the following values:
  *      @arg SDIO_INT_VENDOR_SPECIFIC_ERR:
  *      @arg SDIO_INT_ADMA_ERR: Set when the host controller detects errors during ADMA based data transfer.
  *      @arg SDIO_INT_AUTO_CMD12_ERR: Set when detecting that one of this bits D00-D04 in Auto CMD12 Error Status register
  *           has changed from 0 to 1.
  *      @arg SDIO_INT_CURRENT_LIMIT_ERR:
  *      @arg SDIO_INT_DAT_END_BIT_ERR: Set when detecting 0 at the end bit position of read data which uses the DAT line or at
  *           the end bit position of the CRC Status.
  *      @arg SDIO_INT_DAT_CRC_ERR:
  *      @arg SDIO_INT_DAT_TIMEOUT_ERR:
  *      @arg SDIO_INT_CMD_INDEX_ERR:
  *      @arg SDIO_INT_CMD_END_BIT_ERR:
  *      @arg SDIO_INT_CMD_CRC_ERR:
  *      @arg SDIO_INT_CMD_TIMEOUT_ERR:
  * @param  newState: new state of the specified SDIO error interrupt status.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SDIO_ErrrorINTStatusConfig(uint32_t SDIO_INTStatus, FunctionalState newState);

/**
  * @brief  Enables or disables the specified SDIO error interrupts.
  * @param  SDIO_INT: specifies the SDIO error interrupts sources to be enabled or disabled.
  *   This parameter can be the following values:
  *      @arg SDIO_INT_VENDOR_SPECIFIC_ERR:
  *      @arg SDIO_INT_ADMA_ERR:
  *      @arg SDIO_INT_AUTO_CMD12_ERR:
  *      @arg SDIO_INT_CURRENT_LIMIT_ERR:
  *      @arg SDIO_INT_DAT_END_BIT_ERR:
  *      @arg SDIO_INT_DAT_CRC_ERR:
  *      @arg SDIO_INT_DAT_TIMEOUT_ERR:
  *      @arg SDIO_INT_CMD_INDEX_ERR:
  *      @arg SDIO_INT_CMD_END_BIT_ERR:
  *      @arg SDIO_INT_CMD_CRC_ERR:
  *      @arg SDIO_INT_CMD_TIMEOUT_ERR:
  * @param  newState: new state of the specified SDIO error interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SDIO_ErrrorINTConfig(uint32_t SDIO_INT, FunctionalState newState);

/**
  * @brief  Enables or disables the specified SDIO wake up event.
  * @param  SDIO_WakeUp: specifies the SDIO wake up sources to be enabled or disabled.
  *   This parameter can be the following values:
  *      @arg SDIO_WAKE_UP_SDCARD_REMOVAL:
  *      @arg SDIO_WAKE_UP_SDCARD_INSERT:
  *      @arg SDIO_WAKE_UP_SDCARD_INTR:
  * @param  newState: new state of the specified SDIO wake up event.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SDIO_WakeUpConfig(uint32_t SDIO_WakeUp, FunctionalState newState);

/**
  * @brief  Checks whether the specified SDIO interrupt is set or not.
  * @param  SDIO_INT: specifies the interrupt to check.
  *   This parameter can be one of the following values:
  *       @arg SDIO_INT_ERROR: Set when any of bits in the Error interrupt status register are set.
  *       @arg SDIO_INT_CARD: Set when generate card interrupt.
  *       @arg SDIO_INT_DMA: In case of ADMA, by setting int field in the descriptor table, host controller generates this interrupt.
  *       @arg SDIO_BLOCK_GAP_EVENT: Set when stop at Block Gap Request is set.
  *       @arg SDIO_INT_TF_CMPL: Set when a read/write transfer and a command with busy is complete.
  *       @arg SDIO_INT_CMD_CMPL: Set when get the end bit of command response(Except Auto CMD12).
  * @retval The new state of ITStatus (SET or RESET).
  */
ITStatus SDIO_GetINTStatus(uint32_t SDIO_INT);

/**
  * @brief  Checks whether the specified SDIO error interrupt is set or not.
  * @param  SDIO_INT: specifies the error interrupt to check.
  *   This parameter can be one of the following values:
  *     @arg SDIO_INT_VENDOR_SPECIFIC_ERR:
  *     @arg SDIO_INT_ADMA_ERR:
  *     @arg SDIO_INT_AUTO_CMD12_ERR:
  *     @arg SDIO_INT_CURRENT_LIMIT_ERR:
  *     @arg SDIO_INT_DAT_END_BIT_ERR:
  *     @arg SDIO_INT_DAT_CRC_ERR:
  *     @arg SDIO_INT_DAT_TIMEOUT_ERR:
  *     @arg SDIO_INT_CMD_INDEX_ERR:
  *     @arg SDIO_INT_CMD_END_BIT_ERR:
  *     @arg SDIO_INT_CMD_CRC_ERR:
  *     @arg SDIO_INT_CMD_TIMEOUT_ERR:
  * @retval The new state of ITStatus (SET or RESET).
  */
ITStatus SDIO_GetErrorINTStatus(uint32_t SDIO_INT);

/**
  * @brief  Clears the SDIO interrupt pending bits.
  * @param  SDIO_INT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg SDIO_INT_ERROR:
  *     @arg SDIO_INT_CARD:
  *     @arg SDIO_INT_DMA:
  *     @arg SDIO_INT_TF_CMPL:
  *     @arg SDIO_INT_CMD_CMPL:Clear command complete interrupt.
  * @retval None
  */
void SDIO_ClearINTPendingBit(uint32_t SDIO_INT);

/**
  * @brief  Clears the SDIO error interrupt pending bits.
  * @param SDIO_INT: specifies the error interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg SDIO_INT_VENDOR_SPECIFIC_ERR:
  *     @arg SDIO_INT_ADMA_ERR:
  *     @arg SDIO_INT_AUTO_CMD12_ERR:
  *     @arg SDIO_INT_CURRENT_LIMIT_ERR:
  *     @arg SDIO_INT_DAT_END_BIT_ERR:
  *     @arg SDIO_INT_DAT_CRC_ERR:
  *     @arg SDIO_INT_DAT_TIMEOUT_ERR:
  *     @arg SDIO_INT_CMD_INDEX_ERR:
  *     @arg SDIO_INT_CMD_END_BIT_ERR:
  *     @arg SDIO_INT_CMD_CRC_ERR:
  *     @arg SDIO_INT_CMD_TIMEOUT_ERR:
  * @retval None
  */
void SDIO_ClearErrorINTPendingBit(uint32_t SDIO_INT);

/**
  * @brief  Returns response received from the card for the last command.
  * @param  SDIO_RSP: Specifies the SDIO response register.
  *   This parameter can be one of the following values:
  *      @arg SDIO_RSP0: Response Register 0~1
  *      @arg SDIO_RSP2: Response Register 2~3
  *      @arg SDIO_RSP4: Response Register 4~5
  *      @arg SDIO_RSP6: Response Register 6~7
  * @retval value of SDIO_RESP.
  */
uint32_t SDIO_GetResponse(uint32_t SDIO_RSP);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876x_SDIO_H_ */

/** @} */ /* End of group 87x3e_SDIO_Exported_Functions */
/** @} */ /* End of group 87x3e_SDIO */

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

