/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_qdec.h
* \brief    The header file of the peripheral QDECODER driver.
* \details  This file provides all QDECODER firmware functions.
* \author   howie wang
* \date     2016-05-10
* \version  v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876X_QDECODER_H_
#define _RTL876X_QDECODER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup 87x3e_QDEC QDEC
  * @brief QDEC driver module
  * @{
  */

/*============================================================================*
 *                         Includes
 *============================================================================*/
#include "rtl876x.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * \defgroup    87x3e_QDEC_Exported_Types Init Params Struct
 *
 * \ingroup     87x3e_QDEC
 */

/**
 * \brief       Qdecoder initialize parameters.
 *
 * \ingroup     87x3e_QDEC_Exported_Types
 */
typedef struct
{
    uint16_t ScanClockKHZ;                 /*!< Specifies the scan clock(KHz).
                                                  Scan clock = source clock / (DIV+1), source clock is 20000KHz, DIV range is 0x0 to 0xFFF*/

    uint16_t DebonceClockKHZ;             /*!< Specifies the debounce clock(KHz).
                                                  Debounce clock = scan clock / (DEB_DIV+1), DEB_DIV range is 0x0 to 0xF  */

    uint8_t axisConfigX;                /*!< Specifies the axis X function.
                                                  This parameter can be a value of ENABLE or DISABLE */
    uint8_t axisConfigY;                /*!< Specifies the axis Y function.
                                                  This parameter can be a value of ENABLE or DISABLE */
    uint8_t axisConfigZ;                /*!< Specifies the axis Z function.
                                                  This parameter can be a value of ENABLE or DISABLE */
    uint8_t autoLoadInitPhase;          /*!< Specifies Auto-load Initphase function .
                                                  This parameter can be a value of ENABLE or DISABLE */
    uint16_t counterScaleX;             /*!< Specifies the axis X conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale */
    uint16_t debounceEnableX;           /*!< Specifies the axis X debounce.
                                                  This parameter can be a value of @ref Qdec_Debounce */
    uint8_t debounceTimeX;             /*!< Specifies the axis X debounce time.
                                                  This parameter can be a value from 0 to 0xFF*/
    uint16_t initPhaseX;                /*!< Specifies the axis X function.
                                                  This parameter can be a value of \ref Qdec_init_phase */
    uint16_t counterScaleY;             /*!< Specifies the axis Y conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale */
    uint16_t debounceEnableY;           /*!< Specifies the axis Y debounce.
                                                  This parameter can be a value of @ref Qdec_Debounce */
    uint8_t debounceTimeY;             /*!< Specifies the axis Y debounce time.
                                                  This parameter can be a value from 0 to 0xFF*/
    uint16_t initPhaseY;                /*!< Specifies the axis Y function.
                                                  This parameter can be a value of \ref Qdec_init_phase */
    uint16_t counterScaleZ;             /*!< Specifies the axis Z conter scale.
                                                  This parameter can be a value of @ref QDEC_Axis_counter_Scale */
    uint16_t debounceEnableZ;           /*!< Specifies the axis Z debounce.
                                                  This parameter can be a value of @ref Qdec_Debounce */
    uint8_t debounceTimeZ;             /*!< Specifies the axis Z debounce time.
                                                  This parameter can be a value from 0 to 0xFF*/
    uint16_t initPhaseZ;                /*!< Specifies the axis Z function.
                                                  This parameter can be a value of \ref Qdec_init_phase */
} QDEC_InitTypeDef;


/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * \defgroup    87x3e_QDEC_Exported_Constants     Macro Definitions
 *
 * \ingroup     87x3e_QDEC
 */

#define IS_QDEC_PERIPH(PERIPH) ((PERIPH) == QDEC)

#define QDEC_SOURCE_CLOCK       20000   //KHz

/** \defgroup   87x3e_QDEC_Interrupts_Definition  QDEC Interrupts Definition
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */
#define QDEC_X_INT_NEW_DATA                                   BIT(0)//get New data and state change
#define QDEC_X_INT_ILLEAGE                                    BIT(1)//illeage
#define QDEC_Y_INT_NEW_DATA                                   BIT(2)//get New data and state change
#define QDEC_Y_INT_ILLEAGE                                    BIT(3)//illeage
#define QDEC_Z_INT_NEW_DATA                                   BIT(4)//get New data and state change
#define QDEC_Z_INT_ILLEAGE                                    BIT(5)//illeage
/** \} */

#define IS_QDEC_INT_CONFIG(CONFIG) (((CONFIG) == QDEC_X_INT_NEW_DATA) || ((CONFIG) == QDEC_X_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Y_INT_NEW_DATA) || ((CONFIG) == QDEC_Y_INT_ILLEAGE)\
                                    || ((CONFIG) == QDEC_Z_INT_NEW_DATA) || ((CONFIG) == QDEC_Z_INT_ILLEAGE))

/** \defgroup   87x3e_QDEC_Interrupts_Mask    QDEC Interrupts Mask
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */
#define QDEC_X_CT_INT_MASK                                   BIT(0)//get New data and state change
#define QDEC_X_ILLEAGE_INT_MASK                              BIT(4)//illeage
#define QDEC_Y_CT_INT_MASK                                   BIT(1)//get New data and state change
#define QDEC_Y_ILLEAGE_INT_MASK                              BIT(5)//illeage
#define QDEC_Z_CT_INT_MASK                                   BIT(2)//get New data and state change
#define QDEC_Z_ILLEAGE_INT_MASK                              BIT(6)//illeage
/** \} */

#define IS_QDEC_INT_MASK_CONFIG(CONFIG) (((CONFIG) == QDEC_X_CT_INT_MASK) || ((CONFIG) == QDEC_X_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Y_CT_INT_MASK) || ((CONFIG) == QDEC_Y_ILLEAGE_INT_MASK)\
                                         || ((CONFIG) == QDEC_Z_CT_INT_MASK) || ((CONFIG) == QDEC_Z_ILLEAGE_INT_MASK))
/** \defgroup   87x3e_QDEC_Axis_counter_Scale     QDEC Axis Counter
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */
#define counterScaleEnable                        true      /*!< Update counter when 2 phase change. */
#define counterScaleDisable                       false     /*!< Update counter when 1 phase change. */
/** \} */

/** \defgroup   87x3e_QDEC_Debounce QDEC Debounce
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */
#define Debounce_Enable                        true
#define Debounce_Disable                       false
/** \} */

/** \defgroup   87x3e_QDEC_Manual_Phase   QDEC manual phase
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */

#define manualPhaseEnable                        true
#define manualPhaseDisable                       false
/** \} */

/** \defgroup   87x3e_Qdec_Init_Phase     Qdec Init Phase
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */

#define phaseMode0                        0       //phase 00
#define phaseMode1                        1       //phase 01
#define phaseMode2                        2       //phase 10
#define phaseMode3                        3       //phase 11
/** \} */

/** \defgroup   87x3e_QDEC_Clr_Flag   Qdec Clr Flag
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */
#define QDEC_CLR_ILLEGAL_CT_X                            ((uint32_t)(1 << 20))
#define QDEC_CLR_ILLEGAL_CT_Y                            ((uint32_t)(1 << 21))
#define QDEC_CLR_ILLEGAL_CT_Z                            ((uint32_t)(1 << 22))

#define QDEC_CLR_ACC_CT_X                                ((uint32_t)(1 << 16))
#define QDEC_CLR_ACC_CT_Y                                ((uint32_t)(1 << 17))
#define QDEC_CLR_ACC_CT_Z                                ((uint32_t)(1 << 18))

#define QDEC_CLR_ILLEGAL_INT_X                           ((uint32_t)(1 << 12))
#define QDEC_CLR_ILLEGAL_INT_Y                           ((uint32_t)(1 << 13))
#define QDEC_CLR_ILLEGAL_INT_Z                           ((uint32_t)(1 << 14))

#define QDEC_CLR_UNDERFLOW_X                             ((uint32_t)(1 << 8))
#define QDEC_CLR_UNDERFLOW_Y                             ((uint32_t)(1 << 9))
#define QDEC_CLR_UNDERFLOW_Z                             ((uint32_t)(1 << 10))

#define QDEC_CLR_OVERFLOW_X                              ((uint32_t)(1 << 4))
#define QDEC_CLR_OVERFLOW_Y                              ((uint32_t)(1 << 5))
#define QDEC_CLR_OVERFLOW_Z                              ((uint32_t)(1 << 6))

#define QDEC_CLR_NEW_CT_X                                ((uint32_t)(1 << 0))
#define QDEC_CLR_NEW_CT_Y                                ((uint32_t)(1 << 1))
#define QDEC_CLR_NEW_CT_Z                                ((uint32_t)(1 << 2))
/** \} */

#define IS_QDEC_INT_CLR_CONFIG(CONFIG) (((CONFIG) == QDEC_CLR_ACC_CT_X) || ((CONFIG) == QDEC_CLR_ACC_CT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ACC_CT_Z) || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Y)\
                                        || ((CONFIG) == QDEC_CLR_ILLEGAL_INT_Z) || ((CONFIG) == QDEC_CLR_UNDERFLOW_X)\
                                        || ((CONFIG) == QDEC_CLR_UNDERFLOW_Y) || ((CONFIG) == QDEC_CLR_UNDERFLOW_Z)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_X) || ((CONFIG) == QDEC_CLR_OVERFLOW_Y)\
                                        || ((CONFIG) == QDEC_CLR_OVERFLOW_Z) || ((CONFIG) == QDEC_CLR_NEW_CT_X)\
                                        || ((CONFIG) == QDEC_CLR_NEW_CT_Y) || ((CONFIG) == QDEC_CLR_NEW_CT_Z))

/** \defgroup   87x3e_QDEC_Flag QDEC Flag
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */

#define QDEC_FLAG_NEW_CT_STATUS_X                             ((uint32_t)(1 << 0))
#define QDEC_FLAG_NEW_CT_STATUS_Y                             ((uint32_t)(1 << 1))
#define QDEC_FLAG_NEW_CT_STATUS_Z                             ((uint32_t)(1 << 2))
#define QDEC_FLAG_OVERFLOW_X                                  ((uint32_t)(1 << 3))
#define QDEC_FLAG_OVERFLOW_Y                                  ((uint32_t)(1 << 4))
#define QDEC_FLAG_OVERFLOW_Z                                  ((uint32_t)(1 << 5))
#define QDEC_FLAG_UNDERFLOW_X                                 ((uint32_t)(1 << 6))
#define QDEC_FLAG_UNDERFLOW_Y                                 ((uint32_t)(1 << 7))
#define QDEC_FLAG_UNDERFLOW_Z                                 ((uint32_t)(1 << 8))
#define QDEC_FLAG_AUTO_STATUS_X                               ((uint32_t)(1 << 9))
#define QDEC_FLAG_AUTO_STATUS_Y                               ((uint32_t)(1 << 10))
#define QDEC_FLAG_AUTO_STATUS_Z                               ((uint32_t)(1 << 11))
#define QDEC_FLAG_ILLEGAL_STATUS_X                            ((uint32_t)(1 << 12))
#define QDEC_FLAG_ILLEGAL_STATUS_Y                            ((uint32_t)(1 << 13))
#define QDEC_FLAG_ILLEGAL_STATUS_Z                            ((uint32_t)(1 << 14))
/** \} */

#define IS_QDEC_STATUS_FLAG(QDEC_FLAG) (((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_X) || ((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_Y)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_ILLEGAL_STATUS_Z) || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_X)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_Y) || ((QDEC_FLAG) == QDEC_FLAG_NEW_CT_STATUS_Z)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_X) || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_Y)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_OVERFLOW_Z) || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_X)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_Y) || ((QDEC_FLAG) == QDEC_FLAG_UNDERFLOW_Z)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_AUTO_STATUS_X) || ((QDEC_FLAG) == QDEC_FLAG_AUTO_STATUS_Y)\
                                        || ((QDEC_FLAG) == QDEC_FLAG_AUTO_STATUS_Z))

/** \defgroup   87x3e_QDEC_Axis   QDEC Axis
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */

#define QDEC_AXIS_X                                     ((uint32_t)(1 << 0))
#define QDEC_AXIS_Y                                     ((uint32_t)(1 << 2))
#define QDEC_AXIS_Z                                     ((uint32_t)(1 << 3))
/** \} */

/** \defgroup   87x3e_QDEC_Axis_Direction     QDEC Axis Direction
 * \{
 * \ingroup     87x3e_QDEC_Exported_Constants
 */

#define QDEC_AXIS_DIR_UP                                 ((uint16_t)0x01)
#define QDEC_AXIS_DIR_DOWN                               ((uint16_t)0x00)
/** \} */

#define IS_QDEC_AXIS_DIR(QDEC_AXIS)     ((QDEC_AXIS == QDEC_AXIS_DIR_UP) || (QDEC_AXIS == QDEC_AXIS_DIR_DOWN))


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * \defgroup    87x3e_QDEC_Exported_Functions Peripheral APIs
 * \{
 * \ingroup     87x3e_87x3e_QDEC
 */

/**
 * \brief   Deinitializes the Qdecoder peripheral registers to their default reset values(turn off Qdecoder clock).
 * \param[in] QDECx: Selected Qdecoder peripheral.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit();
 * }
 * \endcode
 */
void QDEC_DeInit(QDEC_TypeDef *QDECx);

/**
 * \brief   Initializes the Qdecoder peripheral according to the specified
 *          parameters in the QDEC_InitStruct
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_InitStruct: Pointer to a QDEC_InitStruct structure that
 *                              contains the configuration information for the specified Qdecoder peripheral
 * \return None.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit(QDEC);
 *     RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
 *
 *     QDEC_InitTypeDef QDEC_InitStruct;
 *     QDEC_StructInit(&QDEC_InitStruct);
 *     QDEC_InitStruct.axisConfigY       = ENABLE;
 *     QDEC_InitStruct.debounceEnableY   = Debounce_Enable;
 *     QDEC_Init(QDEC, &QDEC_InitStruct);
 *
 *     QDEC_Cmd(QDEC, QDEC_AXIS_Y, ENABLE);
 * }
 * \endcode
 */
void QDEC_Init(QDEC_TypeDef *QDECx, QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * \brief  Fills each QDEC_InitStruct member with its default value.
 * \param[in]  QDEC_InitStruct: Pointer to an QDEC_InitStruct structure which will be initialized.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_DeInit(QDEC);
 *     RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
 *
 *     QDEC_InitTypeDef QDEC_InitStruct;
 *     QDEC_StructInit(&QDEC_InitStruct);
 *     QDEC_InitStruct.axisConfigY       = ENABLE;
 *     QDEC_InitStruct.debounceEnableY   = Debounce_Enable;
 *     QDEC_Init(QDEC, &QDEC_InitStruct);
 *
 *     QDEC_Cmd(QDEC, QDEC_AXIS_Y, ENABLE);
 * }
 * \endcode
 */
void QDEC_StructInit(QDEC_InitTypeDef *QDEC_InitStruct);

/**
 * \brief  Enables or disables the specified Qdecoder interrupt source.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_IT: Specifies the QDECODER interrupts sources to be enabled or disabled.
 *      This parameter parameter can be one of the following values:
 *      \arg  QDEC_X_INT_NEW_DATA: The counter interrupt for X axis.
 *      \arg  QDEC_X_INT_ILLEAGE: The illegal interrupt for X axis.
 *      \arg  QDEC_Y_INT_NEW_DATA: The counter interrupt for Y axis.
 *      \arg  QDEC_Y_INT_ILLEAGE: The illegal interrupt for Y axis.
 *      \arg  QDEC_Z_INT_NEW_DATA: The counter interrupt for Z axis.
 *      \arg  QDEC_Z_INT_ILLEAGE: The illegal interrupt for Z axis.
 * \param[in]  newState: New state of the specified QDECODER interrupt.
 *      This parameter parameter can be: ENABLE or DISABLE.
 * \return None.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_qdec_init(void)
 * {
 *     QDEC_INTConfig(QDEC, QDEC_Y_INT_NEW_DATA, ENABLE);
 * }
 * \endcode
 */
void QDEC_INTConfig(QDEC_TypeDef *QDECx, uint32_t QDEC_IT, FunctionalState newState);

/**
 * \brief  Check whether the specified Qdecoder flag is set.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_FLAG: Specifies the flag to check.
 *      This parameter can be one of the following values:
 *      \arg QDEC_FLAG_NEW_CT_STATUS_X: Status of the counter interrupt for X axis.
 *      \arg QDEC_FLAG_NEW_CT_STATUS_Y: Status of the counter interrupt for Y axis.
 *      \arg QDEC_FLAG_NEW_CT_STATUS_Z: Status of the counter interrupt for Z axis.
 *      \arg QDEC_FLAG_ILLEGAL_STATUS_X: Status of the illegal interrupt for X axis.
 *      \arg QDEC_FLAG_ILLEGAL_STATUS_Y: Status of the illegal interrupt for Y axis.
 *      \arg QDEC_FLAG_ILLEGAL_STATUS_Z: Status of the illegal interrupt for Z axis.
 *      \arg QDEC_FLAG_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *      \arg QDEC_FLAG_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *      \arg QDEC_FLAG_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *      \arg QDEC_FLAG_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *      \arg QDEC_FLAG_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *      \arg QDEC_FLAG_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 * \retval The new state of QDEC_FLAG (SET or RESET).
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     FlagStatus flag_status = QDEC_GetFlagState(QDEC, QDEC_Y_INT_NEW_DATA, ENABLE);
 * }
 * \endcode
 */
FlagStatus QDEC_GetFlagState(QDEC_TypeDef *QDECx, uint32_t QDEC_FLAG);

/**
 * \brief  Enables or disables mask the specified Qdecoder axis interrupts.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *      This parameter can be one or logical OR of the following values:
 *      \arg  QDEC_X_CT_INT_MASK: The x-axis counter interrupt mask.
 *      \arg  QDEC_X_ILLEAGE_INT_MASK: The x-axis illegal interrupt mask.
 *      \arg  QDEC_Y_CT_INT_MASK: The y-axis counter interrupt mask.
 *      \arg  QDEC_Y_ILLEAGE_INT_MASK: The y-axis illegal interrupt mask.
 *      \arg  QDEC_Z_CNT_INT_MASK: The z-axis counter interrupt mask.
 *      \arg  QDEC_Z_ILLEAGE_INT_MASK: The z-axis illegal interrupt mask.
 * \param[in]  newState: New state of the specified Qdecoder interrupts mask.
 *      This parameter can be: ENABLE or DISABLE.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *
 *     QDEC_INTMask(QDEC, QDEC_X_CT_INT_MASK, ENABLE);
 *
 * }
 * \endcode
 */
void QDEC_INTMask(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS, FunctionalState newState);

/**
 * \brief  Enable or disable the selected Qdecoder axis(x/y/z).
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *      This parameter can be one of the following values:
 *      \arg  QDEC_AXIS_X: The qdecoder X axis.
 *      \arg  QDEC_AXIS_Y: The qdecoder Y axis.
 *      \arg  QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in]  newState: New state of the selected Qdecoder axis.
 *      This parameter can be : ENABLE or DISABLE.
 * \retturn The count of the axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_Cmd(QDEC, QDEC_AXIS_X, ENABLE);
 * }
 * \endcode
 */
void QDEC_Cmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS,
              FunctionalState newState);

/**
 * \brief   Clears Qdecoder pending flags.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_FLAG: Specifies the flag to clear.
 *      This parameter parameter can be one of the following values:
 *      \arg  QDEC_CLR_OVERFLOW_X: The overflow flag for x-axis accumulation counter.
 *      \arg  QDEC_CLR_OVERFLOW_Y: The overflow flag for y-axis accumulation counter.
 *      \arg  QDEC_CLR_OVERFLOW_Z: The overflow flag for z-axis accumulation counter.
 *      \arg  QDEC_CLR_ILLEGAL_INT_X: The illegal interrupt for X axis.
 *      \arg  QDEC_CLR_ILLEGAL_INT_Y: The illegal interrupt for Y axis.
 *      \arg  QDEC_CLR_ILLEGAL_INT_Z: The illegal interrupt for Z axis.
 *      \arg  QDEC_CLR_UNDERFLOW_X: The underflow flag for x-axis accumulation counter.
 *      \arg  QDEC_CLR_UNDERFLOW_Y: The underflow flag for y-axis accumulation counter.
 *      \arg  QDEC_CLR_UNDERFLOW_Z: The underflow flag for z-axis accumulation counter.
 *      \arg  QDEC_CLR_NEW_CT_X: The counter interrupt for X axis.
 *      \arg  QDEC_CLR_NEW_CT_Y: The counter interrupt for Y axis.
 *      \arg  QDEC_CLR_NEW_CT_Z: The counter interrupt for Z axis.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_ClearFlags(QDEC, QDEC_CLR_OVERFLOW_X);
 * }
 * \endcode
 */
__STATIC_INLINE void QDEC_ClearFlags(QDEC_TypeDef *QDECx, uint32_t QDEC_FLAG)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_STATUS_FLAG(QDEC_FLAG));

    QDECx->INT_CLR |= QDEC_FLAG;

    return;
}

/**
 * \brief  Get Qdecoder Axis(x/y/z) direction.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *      This parameter parameter can be one of the following values:
 *      \arg  QDEC_AXIS_X: The qdecoder X axis.
 *      \arg  QDEC_AXIS_Y: The qdecoder Y axis.
 *      \arg  QDEC_AXIS_Z: The qdecoder Z axis.
 * \return The direction of the axis.
 *      This parameter parameter can be one of the following values:
 *     \retval QDEC_AXIS_DIR_UP: The axis is rolling up.
 *     \retval QDEC_AXIS_DIR_DOWN: The axis is rolling down.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     uint16_t dir = QDEC_GetAxisDirection(QDEC, QDEC_AXIS_X);
 * }
 * \endcode
 */
__STATIC_INLINE uint16_t QDEC_GetAxisDirection(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    return ((*((volatile uint32_t *)(&QDECx->REG_SR_X) + QDEC_AXIS / 2) & (1 << 16)) == BIT(16));
}

/**
 * \brief  Get Qdecoder Axis(x/y/z) count.
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *      This parameter parameter can be one of the following values:
 *      \arg  QDEC_AXIS_X: The qdecoder X axis.
 *      \arg  QDEC_AXIS_Y: The qdecoder Y axis.
 *      \arg  QDEC_AXIS_Z: The qdecoder Z axis.
 * \return The count of the axis.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     uint16_t counter = QDEC_GetAxisCount(QDEC, QDEC_AXIS_X);
 * }
 * \endcode
 */
__STATIC_INLINE uint16_t QDEC_GetAxisCount(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    return ((uint16_t)(*((volatile uint32_t *)(&QDECx->REG_SR_X) + QDEC_AXIS / 2)));
}

/**
 * \brief  Pause or resume Qdecoder Axis(x/y/z).
 * \param[in]  QDECx: Selected Qdecoder peripheral.
 * \param[in]  QDEC_AXIS: Specifies the Qdecoder axis.
 *      This parameter parameter can be one of the following values:
 *      \arg  QDEC_AXIS_X: The qdecoder X axis.
 *      \arg  QDEC_AXIS_Y: The qdecoder Y axis.
 *      \arg  QDEC_AXIS_Z: The qdecoder Z axis.
 * \param[in]  newState: New state of the specified Qdecoder Axis.
 *      This parameter parameter can be one of the following values:
 *      \arg  ENABLE: Pause.
 *      \arg  DISABLE: Resume.
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void qdec_demo(void)
 * {
 *     QDEC_CounterPauseCmd(QDEC, QDEC_AXIS_X, ENABLE);
 * }
 * \endcode
 */
__STATIC_INLINE void QDEC_CounterPauseCmd(QDEC_TypeDef *QDECx, uint32_t QDEC_AXIS,
                                          FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    if (newState == ENABLE)
    {
        *((volatile uint32_t *)(&QDECx->REG_CR_X) + QDEC_AXIS / 2) |= BIT3;
    }
    else
    {
        *((volatile uint32_t *)(&QDECx->REG_CR_X) + QDEC_AXIS / 2) &= ~BIT3;
    }
}

/** \} */ /* End of group 87x3e_QDEC_Exported_Functions */

/** \} */ /* End of group 87x3e_QDEC */

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_QDECODER_H_ */


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/



