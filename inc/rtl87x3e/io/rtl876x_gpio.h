/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_gpio.h
* @brief
* @details
* @author    elliot chen
* @date      2015-05-20
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __RTL876X_GPIO_H
#define __RTL876X_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

/** @cond private
  * @defgroup 87x3e_GPIO Debounce register
  * @{
  */

/* GPIO Private Defines */
#define GPIO_DBCLK_DIV *((volatile uint32_t *)0x40000344UL)

/**
  * @}
  * @endcond
  */

/** @
  * @brief GPIO_Number GPIO Number
  */

#define GPIO0   0
#define GPIO1   1
#define GPIO2   2
#define GPIO3   3
#define GPIO4   4
#define GPIO5   5
#define GPIO6   6
#define GPIO7   7
#define GPIO8   8
#define GPIO9   9
#define GPIO10  10
#define GPIO11  11
#define GPIO12  12
#define GPIO13  13
#define GPIO14  14
#define GPIO15  15
#define GPIO16  16
#define GPIO17  17
#define GPIO18  18
#define GPIO19  19
#define GPIO20  20
#define GPIO21  21
#define GPIO22  22
#define GPIO23  23
#define GPIO24  24
#define GPIO25  25
#define GPIO26  26
#define GPIO27  27
#define GPIO28  28
#define GPIO29  29
#define GPIO30  30
#define GPIO31  31
#define GPIO32   32
#define GPIO33   33
#define GPIO34   34
#define GPIO35   35
#define GPIO36   36
#define GPIO37   37
#define GPIO38   38
#define GPIO39   39
#define GPIO40  40
#define GPIO41  41
#define GPIO42  42
#define GPIO43  43
#define GPIO44  44
#define GPIO45  45
#define GPIO46  46
#define GPIO47  47
#define GPIO48  48
#define GPIO49  49
#define GPIO50  50
#define GPIO51  51
#define GPIO52  52
#define GPIO53  53
#define GPIO54  54
#define GPIO55  55
#define GPIO56  56
#define GPIO57  57
#define GPIO58  58
#define GPIO59  59
#define GPIO60  60
#define GPIO61  61
#define GPIO62  62
#define GPIO63  63

#define GPIO_INVALID    (0xff)

/** @
  * @brief Pin_Number_To_GPIO_Number GPIO Number Lookup
  */
#define PIN0_GPIO_INDEX        GPIO0
#define PIN1_GPIO_INDEX        GPIO1
#define PIN2_GPIO_INDEX        GPIO2
#define PIN3_GPIO_INDEX        GPIO3
#define PIN4_GPIO_INDEX        GPIO4
#define PIN5_GPIO_INDEX        GPIO5
#define PIN6_GPIO_INDEX        GPIO6
#define PIN7_GPIO_INDEX        GPIO29
#define PIN8_GPIO_INDEX        GPIO7
#define PIN9_GPIO_INDEX        GPIO8
#define PIN10_GPIO_INDEX       GPIO9
#define PIN11_GPIO_INDEX       GPIO10
#define PIN12_GPIO_INDEX       GPIO11
#define PIN13_GPIO_INDEX       GPIO12
#define PIN14_GPIO_INDEX       GPIO13
#define PIN15_GPIO_INDEX       GPIO14
#define PIN16_GPIO_INDEX       GPIO15
#define PIN17_GPIO_INDEX       GPIO16
#define PIN18_GPIO_INDEX       GPIO17
#define PIN19_GPIO_INDEX       GPIO18
#define PIN20_GPIO_INDEX       GPIO19
#define PIN21_GPIO_INDEX       GPIO20
#define PIN22_GPIO_INDEX       GPIO21
#define PIN23_GPIO_INDEX       GPIO22
#define PIN24_GPIO_INDEX       GPIO23
#define PIN25_GPIO_INDEX       GPIO24
#define PIN26_GPIO_INDEX       GPIO34
#define PIN27_GPIO_INDEX       GPIO35
#define PIN28_GPIO_INDEX       GPIO36
#define PIN29_GPIO_INDEX       GPIO37
#define PIN30_GPIO_INDEX       GPIO38
#define PIN31_GPIO_INDEX       GPIO39
#define PIN32_GPIO_INDEX       GPIO62
#define PIN33_GPIO_INDEX       GPIO63
#define PIN34_GPIO_INDEX       GPIO27
#define PIN35_GPIO_INDEX       GPIO28
#define PIN36_GPIO_INDEX       GPIO25
#define PIN37_GPIO_INDEX       GPIO26
#define PIN38_GPIO_INDEX       GPIO54
#define PIN39_GPIO_INDEX       GPIO55
#define PIN40_GPIO_INDEX       GPIO_INVALID
#define PIN41_GPIO_INDEX       GPIO_INVALID
#define PIN42_GPIO_INDEX       GPIO_INVALID
#define PIN43_GPIO_INDEX       GPIO_INVALID
#define PIN44_GPIO_INDEX       GPIO56
#define PIN45_GPIO_INDEX       GPIO30
#define PIN46_GPIO_INDEX       GPIO31
#define PIN47_GPIO_INDEX       GPIO61
#define PIN48_GPIO_INDEX       GPIO60
#define PIN49_GPIO_INDEX       GPIO59
#define PIN50_GPIO_INDEX       GPIO58
#define PIN51_GPIO_INDEX       GPIO33
#define PIN52_GPIO_INDEX       GPIO32
#define PIN53_GPIO_INDEX       GPIO_INVALID
#define PIN54_GPIO_INDEX       GPIO_INVALID
#define PIN55_GPIO_INDEX       GPIO_INVALID
#define PIN56_GPIO_INDEX       GPIO25
#define PIN57_GPIO_INDEX       GPIO27
#define PIN58_GPIO_INDEX       GPIO50
#define PIN59_GPIO_INDEX       GPIO51
#define PIN60_GPIO_INDEX       GPIO52
#define PIN61_GPIO_INDEX       GPIO53
#define PIN62_GPIO_INDEX       GPIO54
#define PIN63_GPIO_INDEX       GPIO55
#define PIN64_GPIO_INDEX       GPIO40
#define PIN65_GPIO_INDEX       GPIO41
#define PIN66_GPIO_INDEX       GPIO42
#define PIN67_GPIO_INDEX       GPIO43
#define PIN68_GPIO_INDEX       GPIO44

/** @
  * @brief Macros to lookup GPIO handler based on gpio number
  */
#define XGPIO_HANDLER(num)     GPIO ## num ## _Handler
#define GPIO_HANDLER(num)      XGPIO_HANDLER(num)

/**
 * @brief Macros to lookup GPIO index based on Pin number.
 */
#define XGPIO_INDEX(pin_num)   PIN ## pin_num ## _GPIO_INDEX
#define GPIO_INDEX(pin_num)    XGPIO_INDEX(pin_num)


/** @
  * @brief GPIO Number to IRQ Lookup Macro
  */
#define GPIO0_IRQ_NUM GPIO0_IRQn
#define GPIO1_IRQ_NUM GPIO1_IRQn
#define GPIO2_IRQ_NUM GPIO2_IRQn
#define GPIO3_IRQ_NUM GPIO3_IRQn
#define GPIO4_IRQ_NUM GPIO4_IRQn
#define GPIO5_IRQ_NUM GPIO5_IRQn
#define GPIO6_IRQ_NUM GPIO6_IRQn
#define GPIO7_IRQ_NUM GPIO7_IRQn
#define GPIO8_IRQ_NUM GPIO8_IRQn
#define GPIO9_IRQ_NUM GPIO9_IRQn
#define GPIO10_IRQ_NUM GPIO10_IRQn
#define GPIO11_IRQ_NUM GPIO11_IRQn
#define GPIO12_IRQ_NUM GPIO12_IRQn
#define GPIO13_IRQ_NUM GPIO13_IRQn
#define GPIO14_IRQ_NUM GPIO14_IRQn
#define GPIO15_IRQ_NUM GPIO15_IRQn
#define GPIO16_IRQ_NUM GPIO16_IRQn
#define GPIO17_IRQ_NUM GPIO17_IRQn
#define GPIO18_IRQ_NUM GPIO18_IRQn
#define GPIO19_IRQ_NUM GPIO19_IRQn
#define GPIO20_IRQ_NUM GPIO20_IRQn
#define GPIO21_IRQ_NUM GPIO21_IRQn
#define GPIO22_IRQ_NUM GPIO22_IRQn
#define GPIO23_IRQ_NUM GPIO23_IRQn
#define GPIO24_IRQ_NUM GPIO24_IRQn
#define GPIO25_IRQ_NUM GPIO25_IRQn
#define GPIO26_IRQ_NUM GPIO26_IRQn
#define GPIO27_IRQ_NUM GPIO27_IRQn
#define GPIO28_IRQ_NUM GPIO28_IRQn
#define GPIO29_IRQ_NUM GPIO29_IRQn
#define GPIO30_IRQ_NUM GPIO30_IRQn
#define GPIO31_IRQ_NUM GPIO31_IRQn
#define GPIO32_IRQ_NUM GPIOB0_IRQn
#define GPIO33_IRQ_NUM GPIOB1_IRQn
#define GPIO34_IRQ_NUM GPIOB2_IRQn
#define GPIO35_IRQ_NUM GPIOB3_IRQn
#define GPIO36_IRQ_NUM GPIOB4_IRQn
#define GPIO37_IRQ_NUM GPIOB5_IRQn
#define GPIO38_IRQ_NUM GPIOB6_IRQn
#define GPIO39_IRQ_NUM GPIOB7_IRQn
#define GPIO40_IRQ_NUM GPIOB8_IRQn
#define GPIO41_IRQ_NUM GPIOB9_IRQn
#define GPIO42_IRQ_NUM GPIOB10_IRQn
#define GPIO43_IRQ_NUM GPIOB11_IRQn
#define GPIO44_IRQ_NUM GPIOB12_IRQn
#define GPIO45_IRQ_NUM GPIOB13_IRQn
#define GPIO46_IRQ_NUM GPIOB14_IRQn
#define GPIO47_IRQ_NUM GPIOB15_IRQn
#define GPIO48_IRQ_NUM GPIOB16_IRQn
#define GPIO49_IRQ_NUM GPIOB17_IRQn
#define GPIO50_IRQ_NUM GPIOB18_IRQn
#define GPIO51_IRQ_NUM GPIOB19_IRQn
#define GPIO52_IRQ_NUM GPIOB20_IRQn
#define GPIO53_IRQ_NUM GPIOB21_IRQn
#define GPIO54_IRQ_NUM GPIOB22_IRQn
#define GPIO55_IRQ_NUM GPIOB23_IRQn
#define GPIO56_IRQ_NUM GPIOB24_IRQn
#define GPIO57_IRQ_NUM GPIOB25_IRQn
#define GPIO58_IRQ_NUM GPIOB26_IRQn
#define GPIO59_IRQ_NUM GPIOB27_IRQn
#define GPIO60_IRQ_NUM GPIOB28_IRQn
#define GPIO61_IRQ_NUM GPIOB29_IRQn
#define GPIO62_IRQ_NUM GPIOB30_IRQn
#define GPIO63_IRQ_NUM GPIOB31_IRQn

/**
 * @brief Macros to lookup GPIO IRQ number based on GPIO number.
 */
#define XGPIO_IRQ_NUM(gpio_num)   GPIO ## gpio_num ## _IRQ_NUM
#define GPIO_IRQ_NUM(gpio_num)    XGPIO_IRQ_NUM(gpio_num)


/** @addtogroup 87x3e_GPIO GPIO
  * @brief GPIO driver module
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup 87x3e_GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */

/**
  * @brief GPIO mode enumeration
  */

typedef enum
{
    GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode             */
    GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode                */
} GPIOMode_TypeDef;

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN)|| ((MODE) == GPIO_Mode_OUT))

/**
 * @brief Setting interrupt's trigger type
 *
 * Setting interrupt's trigger type
 */
typedef enum
{
    GPIO_INT_Trigger_LEVEL = 0x0, /**< This interrupt is level trigger  */
    GPIO_INT_Trigger_EDGE  = 0x1, /**< This interrupt is edge trigger  */
    GPIO_INT_BOTH_EDGE = 0x2,     /**< This interrupt is both edge trigger  */
} GPIOIT_LevelType;

#define IS_GPIOIT_LEVEL_TYPE(TYPE) (((TYPE) == GPIO_INT_Trigger_LEVEL)\
                                    || ((TYPE) == GPIO_INT_Trigger_EDGE)\
                                    || ((TYPE) == GPIO_INT_BOTH_EDGE))

/**
 * @brief Setting interrupt active mode
 *
 * Setting interrupt active mode
 */
typedef enum
{
    GPIO_INT_POLARITY_ACTIVE_LOW  = 0x0, /**< Setting interrupt to low active  */
    GPIO_INT_POLARITY_ACTIVE_HIGH = 0x1, /**< Setting interrupt to high active */
} GPIOIT_PolarityType;

#define IS_GPIOIT_POLARITY_TYPE(TYPE) (((TYPE) == GPIO_INT_POLARITY_ACTIVE_LOW)\
                                       || ((TYPE) == GPIO_INT_POLARITY_ACTIVE_HIGH))

/**
 * @brief Enable/Disable interrupt debounce mode
 *
 * Enable/Disable interrupt debounce mode
 */
typedef enum
{
    GPIO_INT_DEBOUNCE_DISABLE = 0x0, /**< Disable interrupt debounce  */
    GPIO_INT_DEBOUNCE_ENABLE  = 0x1, /**< Enable interrupt debounce   */
} GPIOIT_DebounceType;

#define IS_GPIOIT_DEBOUNCE_TYPE(TYPE) (((TYPE) == GPIO_INT_DEBOUNCE_DISABLE)\
                                       || ((TYPE) == GPIO_INT_DEBOUNCE_ENABLE))

/**
* @brief hardware/software mode select
*
* Select hardware mode or software mode
*/
typedef enum
{
    GPIO_SOFTWARE_MODE = 0x0, /**< Gpio Software mode(default) */
    GPIO_HARDWARE_MODE  = 0x1, /**< Gpio Hardware control mode  */
} GPIOControlMode_Typedef;

#define IS_GPIOIT_MODDE(TYPE) (((TYPE) == GPIO_SOFTWARE_MODE)\
                               || ((TYPE) == GPIO_HARDWARE_MODE))


/**
  * @brief  Bit_SET and Bit_RESET enumeration
  */

typedef enum
{
    Bit_RESET = 0,
    Bit_SET
} BitAction;

#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))

/** End of group 87x3e_GPIO_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup 87x3e_GPIO_Exported_Types GPIO Exported Types
* @{
*/

/**
  * @brief  GPIO Init structure definition
  */

typedef struct
{
    uint32_t                  GPIO_PinBit;        /*!< Specifies the GPIO pins to be configured.
                                                             This parameter can be any value of @ref GPIO_pins_define */
    GPIOMode_TypeDef          GPIO_Mode;       /*!< Specifies the operating mode for the selected pins.
                                                             This parameter can be a value of @ref GPIOMode_TypeDef */
    FunctionalState           GPIO_ITCmd;      /**< Enable or disable GPIO interrupt.
                                                             This parameter can be a value of DISABLE or ENABLE */

    GPIOIT_LevelType          GPIO_ITTrigger;  /**< Interrupt mode is level or edge trigger.
                                                             This parameter can be a value of DISABLE or ENABLE */

    GPIOIT_PolarityType       GPIO_ITPolarity; /**< Interrupt mode is high or low active trigger */

    GPIOIT_DebounceType       GPIO_ITDebounce; /**< Enable or disable de-bounce for interrupt */

    GPIOControlMode_Typedef   GPIO_ControlMode; /**< Specifies the gpio mode */

    uint32_t GPIO_DebounceTime;                  /**< per.(ms) Specifies the gpio debounce time setting */
} GPIO_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup 87x3e_GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup 87x3e_GPIO_pins_define GPIO Pins Define
  * @{
  */
#define GPIO_Pin_0                 ((uint32_t)0x00000001)  /*!< Pin 0 selected    */
#define GPIO_Pin_1                 ((uint32_t)0x00000002)  /*!< Pin 1 selected    */
#define GPIO_Pin_2                 ((uint32_t)0x00000004)  /*!< Pin 2 selected    */
#define GPIO_Pin_3                 ((uint32_t)0x00000008)  /*!< Pin 3 selected    */
#define GPIO_Pin_4                 ((uint32_t)0x00000010)  /*!< Pin 4 selected    */
#define GPIO_Pin_5                 ((uint32_t)0x00000020)  /*!< Pin 5 selected    */
#define GPIO_Pin_6                 ((uint32_t)0x00000040)  /*!< Pin 6 selected    */
#define GPIO_Pin_7                 ((uint32_t)0x00000080)  /*!< Pin 7 selected    */
#define GPIO_Pin_8                 ((uint32_t)0x00000100)  /*!< Pin 8 selected    */
#define GPIO_Pin_9                 ((uint32_t)0x00000200)  /*!< Pin 9 selected    */
#define GPIO_Pin_10                ((uint32_t)0x00000400)  /*!< Pin 10 selected   */
#define GPIO_Pin_11                ((uint32_t)0x00000800)  /*!< Pin 11 selected   */
#define GPIO_Pin_12                ((uint32_t)0x00001000)  /*!< Pin 12 selected   */
#define GPIO_Pin_13                ((uint32_t)0x00002000)  /*!< Pin 13 selected   */
#define GPIO_Pin_14                ((uint32_t)0x00004000)  /*!< Pin 14 selected   */
#define GPIO_Pin_15                ((uint32_t)0x00008000)  /*!< Pin 15 selected   */
#define GPIO_Pin_16                ((uint32_t)0x00010000)  /*!< Pin 16 selected    */
#define GPIO_Pin_17                ((uint32_t)0x00020000)  /*!< Pin 17 selected    */
#define GPIO_Pin_18                ((uint32_t)0x00040000)  /*!< Pin 18 selected    */
#define GPIO_Pin_19                ((uint32_t)0x00080000)  /*!< Pin 19 selected    */
#define GPIO_Pin_20                ((uint32_t)0x00100000)  /*!< Pin 20 selected    */
#define GPIO_Pin_21                ((uint32_t)0x00200000)  /*!< Pin 21 selected    */
#define GPIO_Pin_22                ((uint32_t)0x00400000)  /*!< Pin 22 selected    */
#define GPIO_Pin_23                ((uint32_t)0x00800000)  /*!< Pin 23 selected    */
#define GPIO_Pin_24                ((uint32_t)0x01000000)  /*!< Pin 24 selected    */
#define GPIO_Pin_25                ((uint32_t)0x02000000)  /*!< Pin 25 selected    */
#define GPIO_Pin_26                ((uint32_t)0x04000000)  /*!< Pin 26 selected   */
#define GPIO_Pin_27                ((uint32_t)0x08000000)  /*!< Pin 27 selected   */
#define GPIO_Pin_28                ((uint32_t)0x10000000)  /*!< Pin 28 selected   */
#define GPIO_Pin_29                ((uint32_t)0x20000000)  /*!< Pin 29 selected   */
#define GPIO_Pin_30                ((uint32_t)0x40000000)  /*!< Pin 30 selected   */
#define GPIO_Pin_31                ((uint32_t)0x80000000)  /*!< Pin 31 selected   */
#define GPIO_Pin_All               ((uint32_t)0xFFFFFFFF)  /*!< All pins selected */

#define IS_GPIO_PIN(PIN) ((PIN) != (uint32_t)0x00)

#define IS_PIN_NUM(NUM) ((NUM) <= (uint8_t)TOTAL_PIN_NUM)

#define IS_GET_GPIO_PIN(PIN) (((PIN) == GPIO_Pin_0) || \
                              ((PIN) == GPIO_Pin_1) || \
                              ((PIN) == GPIO_Pin_2) || \
                              ((PIN) == GPIO_Pin_3) || \
                              ((PIN) == GPIO_Pin_4) || \
                              ((PIN) == GPIO_Pin_5) || \
                              ((PIN) == GPIO_Pin_6) || \
                              ((PIN) == GPIO_Pin_7) || \
                              ((PIN) == GPIO_Pin_8) || \
                              ((PIN) == GPIO_Pin_9) || \
                              ((PIN) == GPIO_Pin_10) || \
                              ((PIN) == GPIO_Pin_11) || \
                              ((PIN) == GPIO_Pin_12) || \
                              ((PIN) == GPIO_Pin_13) || \
                              ((PIN) == GPIO_Pin_14) || \
                              ((PIN) == GPIO_Pin_15) || \
                              ((PIN) == GPIO_Pin_16) || \
                              ((PIN) == GPIO_Pin_17) || \
                              ((PIN) == GPIO_Pin_18) || \
                              ((PIN) == GPIO_Pin_19) || \
                              ((PIN) == GPIO_Pin_20) || \
                              ((PIN) == GPIO_Pin_21) || \
                              ((PIN) == GPIO_Pin_22) || \
                              ((PIN) == GPIO_Pin_23) || \
                              ((PIN) == GPIO_Pin_24) || \
                              ((PIN) == GPIO_Pin_25) || \
                              ((PIN) == GPIO_Pin_26) || \
                              ((PIN) == GPIO_Pin_27) || \
                              ((PIN) == GPIO_Pin_28) || \
                              ((PIN) == GPIO_Pin_29) || \
                              ((PIN) == GPIO_Pin_30) || \
                              ((PIN) == GPIO_Pin_31))
/** End of group 87x3e_GPIO_pins_define
  * @}
  */

/** End of group 87x3e_GPIO_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup 87x3e_GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

/**
  * @brief  Deinitializes the GPIO peripheral registers to their default reset values.
  * @param GPIOx choose GPIOA or GPIOB
  * @retval None
  */
void GPIOx_DeInit(GPIO_TypeDef *GPIOx);

/**
  * @brief  Initializes the GPIO peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
extern void GPIOx_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);

/**
  * @brief    Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will
  *    be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);



/**
  * @brief enable the specified GPIO interrupt.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: where x can be 0 or 31.
  * @param  NewState: enable or disable interrupt
  * @retval None
  */
void GPIOx_INTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, FunctionalState NewState);


/**
  * @brief clear the specified GPIO interrupt.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: where x can be 0 or 31.
  * @retval None
  */
void GPIOx_ClearINTPendingBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit);



/**
  * @brief mask the specified GPIO interrupt.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: where x can be 0 or 31.
  * @param  NewState: disable or enable interrupt.
  * @retval None
  */
void GPIOx_MaskINTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, FunctionalState NewState);

/**
  * @brief  Gets a GPIO bit number such as GPIO[x], which corresponds to Pin_num.
  * @param  Pin_num: This parameter is  [ADC_0 , TOTAL_PIN_NUM), please refer to rtl876x.h "Pin_Number" part.
  * @retval GPIO pin for GPIO initialization.
  */
uint32_t GPIO_GetPin(uint8_t Pin_num);

/**
  * @brief  Gets the number which Pin_num been define.
  * @param  Pin_num: This parameter is  [ADC_0 , TOTAL_PIN_NUM), please refer to rtl876x.h "Pin_Number" part.
  * @retval GPIO pin number.
  */
uint8_t GPIO_GetNum(uint8_t Pin_num);

#if 0  //remove the code for save size
/**
  * @brief Enable the debance clk of GPIOx.
  * @param GPIOx choose GPIOA or GPIOB
  * @retval  none.
  */
void GPIOx_DBClkCmd(GPIO_TypeDef *GPIOx, FunctionalState NewState);
#endif
/**
  * @brief    Reads the specified input port pin.
  * @param  GPIO_Pin:  specifies the port bit to read.
  *   This parameter can be GPIO_Pin_x where x can be (0..31).
  * @retval The input port pin value.
  */
__forceinline uint8_t GPIOx_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    uint8_t bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_PinBit));

    if (GPIOx->DATAIN & GPIO_PinBit)
    {
        bitstatus = (uint8_t)SET;
    }

    return bitstatus;
}



/**
  * @brief  Reads value of all  GPIO input data port.
  *   param  GPIOx choose GPIOA or GPIOB
  * @retval GPIO input data port value.
  */
__STATIC_INLINE uint32_t GPIOx_ReadInputData(GPIO_TypeDef *GPIOx)
{
    return GPIOx->DATAIN;
}

/**
  * @brief  Reads the specified output port pin.
  *   param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit:  specifies the port bit to read.
  *   This parameter can be GPIO_Pin_x where x can be (0..31).
  * @retval The output port pin value.
  */
__forceinline uint8_t GPIOx_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    uint8_t bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_PinBit));

    if (GPIOx->DATAOUT & GPIO_PinBit)
    {
        bitstatus = (uint8_t)SET;
    }

    return bitstatus;
}

/**
  * @brief  Reads value of all  GPIO output data port.
  *   param  GPIOx choose GPIOA or GPIOB
  * @retval GPIO output data port value.
  */
__STATIC_INLINE uint32_t GPIOx_ReadOutputData(GPIO_TypeDef *GPIOx)
{
    return ((uint32_t)GPIOx->DATAOUT);
}

/**
  * @brief  Sets the selected data port bits.
  * @param  GPIO_PinBit: specifies the port bits to be written.
  *   This parameter can be GPIO_Pin_x where x can be (0..31) or GPIO_Pin_All.
  * @retval None
  */
__STATIC_INLINE void GPIOx_SetBits(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));

    GPIOx->DATAOUT |= GPIO_PinBit;
}
/**
  * @brief  Resets the selected data port bits.
  * @param  GPIO_PinBit: specifies the port bits to be written.
  *   This parameter can be GPIO_Pin_0 to GPIO_Pin_31 or GPIO_Pin_All.
  * @retval None
  */
__STATIC_INLINE void GPIOx_ResetBits(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));

    GPIOx->DATAOUT &= ~(GPIO_PinBit);
}

/**
  * @brief  Sets or clears the selected data port bit.
  * @param  GPIO_PinBit: specifies the port bit to be written.
  *   This parameter can be one of GPIO_Pin_x where x can be (0..31).
  * @param  BitVal: specifies the value to be written to the selected bit.
  *   This parameter can be one of the BitAction enum values:
  *     @arg Bit_RESET: to clear the port pin
  *     @arg Bit_SET: to set the port pin
  * @retval None
  */
__forceinline void GPIOx_WriteBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, BitAction BitVal)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));
    assert_param(IS_GPIO_BIT_ACTION(BitVal));

    if (BitVal != Bit_RESET)
    {
        GPIOx->DATAOUT |= GPIO_PinBit;
    }
    else
    {
        GPIOx->DATAOUT &= ~(GPIO_PinBit);
    }
}

/**
  * @brief  Sets or clears the selected data port .
  * param  GPIOx choose GPIOA or GPIOB
  * @param  PortVal: specifies the value to be written to the selected bit.
  * @retval None
  */
__STATIC_INLINE void GPIOx_Write(GPIO_TypeDef *GPIOx, uint32_t PortVal)
{
    GPIOx->DATAOUT = PortVal;
}



/**
  * @brief  return  GPIO interrupt status.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: specifies the port bit to be written.
  *   This parameter can be one of GPIO_Pin_x where x can be (0..31).
  * @retval ITStatus The new state of GPIO_IT (SET or RESET).
  */
__STATIC_INLINE ITStatus GPIOx_GetINTStatus(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit)
{
    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_PinBit));

    if ((GPIOx->INTSTATUS & GPIO_PinBit) == GPIO_PinBit)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/**
  * @brief  Set debounce time.
  * @param  DebounceTime: specifies interrupt debounce time
  *   This parameter can be 1ms ~ 64ms
  * @retval none
  */
__forceinline void GPIOx_Debounce_Time(GPIO_TypeDef *GPIOx, uint32_t DebounceTime)
{
    uint8_t count = 0;

    if (DebounceTime < 1)
    {
        DebounceTime = 1;
    }
    if (DebounceTime > 64)
    {
        DebounceTime = 64;
    }
#ifdef _IS_ASIC_
    //div = 14;//0xd  0b1101<<8
    if (GPIOx == GPIOA)
    {
        GPIO_DBCLK_DIV = ((0xd << 8) | (1 << 12));
    }
    else if (GPIOx == GPIOB)
    {
        GPIO_DBCLK_DIV = ((0xd << 24) | (1 << 28));
    }

#else
    //div = 13;//0xc  0b1100<<8
    if (GPIOx == GPIOA)
    {
        GPIO_DBCLK_DIV |= ((1 << 11) | (1 << 10) | (1 << 12));
        GPIO_DBCLK_DIV &= (~((1 << 9) | (1 << 8)));
    }
    else if (GPIOx == GPIOB)
    {
        GPIO_DBCLK_DIV |= ((1 << 27) | (1 << 26) | (1 << 28));
        GPIO_DBCLK_DIV &= (~((1 << 25) | (1 << 24)));
    }
#endif


    count = (244 * DebounceTime) / 100 - 1;
    if (GPIOx == GPIOA)
    {
        GPIO_DBCLK_DIV &= (~((0xff << 0)));
        GPIO_DBCLK_DIV |= GPIO_DBCLK_DIV + count;
    }
    else if (GPIOx == GPIOB)
    {
        GPIO_DBCLK_DIV &= (~((0xff << 16)));
        GPIO_DBCLK_DIV  |= (GPIO_DBCLK_DIV + count) << 16;
    }
}

/**
  * @brief  Set gpio output mode.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: specifies the port bits to be written.
  *   This parameter can be GPIO_Pin_x where x can be (0..31) or GPIO_Pin_All.
  * @retval None
  */
__forceinline void GPIOx_ModeSet(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit,
                                 GPIOMode_TypeDef GPIO_Mode)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_PinBit));

    if (GPIO_Mode == GPIO_Mode_OUT)
    {
        GPIOx->DATADIR |= GPIO_PinBit;
    }
    else
    {
        GPIOx->DATADIR &= ~GPIO_PinBit;
    }

}
/**
  * @brief  Change GPIO interrupt Polarity.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_PinBit: specifies the port bits to be written.
  *   This parameter can be GPIO_Pin_x where x can be (0..31)
  * @retval None
  */

void GPIOx_IntPolaritySet(GPIO_TypeDef *GPIOx, uint32_t GPIO_PinBit, GPIOIT_PolarityType int_type);
/** @} */ /* End of group 87x3e_GPIO_Exported_Functions */
/** @} */ /* End of group 87x3e_GPIO */


#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_GPIO_H*/



/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

