/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_pinmux.c
* @brief    This file provides all the PINMUX firmware functions.
* @details
* @author   justin kang
* @date     2020-03-27
* @version  v0.2
*********************************************************************************************************
*/

#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_aon_reg.h"
#include "indirect_access.h"
#include "rtl8763_syson_reg.h"
#include "trace.h"

#define PAD_BIT_TABLE_OFFSET                   (12)
#define PAD_ITEM(reg_addr, bit_num)            ((bit_num << PAD_BIT_TABLE_OFFSET) | (reg_addr))
#define PAD_ITEM_ADDR(item)                    (item & 0xFFF)
#define PAD_ITEM_BIT_OFFSET(item)              ((item & 0xF000) >> PAD_BIT_TABLE_OFFSET)
#define PAD_ITEM_EMPTY                         (0)

#define PAD_WK_STS_DATA_AON_REG_7EC            (0xFFFF)
#define PAD_WK_STS_DATA_AON_REG_7EE            (0xFFFF)
#define PAD_WK_STS_DATA_AON_REG_7F0            (0xFFFF)
#define PAD_WK_STS_DATA_AON_REG_7F2            (0xFFFF)
#define PAD_WK_STS_DATA_AON_REG_7F4            (0xdFFF)
#define PAD_WK_STS_DATA_AON_REG_7F6            (0xFFFF)
#define PAD_WK_STS_DATA_AON_REG_7F8            (0xFFFF)

const uint16_t PINADDR_TABLE[TOTAL_PIN_NUM] =
{
    0x1560, 0x1562, 0x1564, 0x1566, 0x158C, 0x158E, 0x1590, 0x1592, /*P0 P3 */
    0x1568, 0x156A, 0x156C, 0x156E, 0x1570, 0x1572, 0x1574, 0x1576, /*P1*/
    0x1578, 0x157A, 0x157C, 0x157E, 0x1580, 0x1582, 0x1584, 0x1586, /*P2*/
    // 0x1588,0x158A,0x158C,0x158E, 0x1590,0x1592,0x00  ,0x00  ,/*P3*/
    /*  P3_0    P3_1    AUX_R  AUX_L   MIC1_P  MIC1_N  MIC2_P   MIC2_N */
    0x1588, 0x158A, 0x1616, 0x1614, 0x1600, 0x1602,  0x1604, 0x1606, /*P3*/
    /* MICBIAS  LOUT_P LOUT_N  ROUT_P  ROUT_N   MIC3_P  MIC3_N       */
    0x1618, 0x160C, 0x160E, 0x1610, 0x1612, 0x1608, 0x160A, 0x0,
    0x1594, 0x1596, 0x1598, 0x159A, 0x159C, 0x159E, 0x15A0, 0x15A2, /*P4*/
    0x15A4, 0x15A6, 0x15A8, 0x15AA, 0x15AC, 0x15AE, 0x15B0, 0x15B2, /*P5*/
    0x15B4, 0x15B6, 0x15B8, 0x15BA, 0x15BC, 0x15BE, 0x15C0, 0x00, /*P6*/
    0x15C2, 0x15C4, 0x15C6, 0x15C8, 0x15CA, 0x15CC, 0x15CE, 0x00, /*P7*/
    0x15D0, 0x15D2, 0x15D4, 0x15D6, 0x15D8, 0x15DA, 0x00,   0x00, /*P8*/
    0x15FC, 0x15FA, 0x15F4, 0x15F8, 0x15F6, 0x15FE, 0x161A, 0x00,   /*P9  P10_0*/

};

const uint16_t WKSTATUS_TABLE[TOTAL_PIN_NUM] =
{
    PAD_ITEM(0x7EC,  0), PAD_ITEM(0x7EC,  1), PAD_ITEM(0x7EC,  2), PAD_ITEM(0x7EC,  3),  /* P0_0 - P0_3 */
    PAD_ITEM(0x7EE, 10), PAD_ITEM(0x7EE, 11), PAD_ITEM(0x7EE, 12), PAD_ITEM(0x7EE, 13),  /* P3_2 - P3_4 */
    PAD_ITEM(0x7EC,  8), PAD_ITEM(0x7EC,  9), PAD_ITEM(0x7EC, 10), PAD_ITEM(0x7EC, 11),  /* P1_0 - P1_3 */
    PAD_ITEM(0x7EC, 12), PAD_ITEM(0x7EC, 13), PAD_ITEM(0x7EC, 14), PAD_ITEM(0x7EC, 15),  /* P1_4 - P1_7 */
    PAD_ITEM(0x7EE,  0), PAD_ITEM(0x7EE,  1), PAD_ITEM(0x7EE,  2), PAD_ITEM(0x7EE,  3),  /* P2_0 - P2_3 */
    PAD_ITEM(0x7EE,  4), PAD_ITEM(0x7EE,  5), PAD_ITEM(0x7EE,  6), PAD_ITEM(0x7EE,  7),  /* P2_4 - P2_7 */
    PAD_ITEM(0x7EE,  8), PAD_ITEM(0x7EE,  9), PAD_ITEM(0x7F6, 11), PAD_ITEM(0x7F6, 10),  /* P3_0 - P3_1  AUX_R, AUX_L */

    PAD_ITEM(0x7F6,  0), PAD_ITEM(0x7F6,  1), PAD_ITEM(0x7F6,  2), PAD_ITEM(0x7F6,  3),  /* MIC1_P,  MIC1_N,  MIC2_P,  MIC2_N */
    PAD_ITEM(0x7F6, 12), PAD_ITEM(0x7F6,  6), PAD_ITEM(0x7F6,  7), PAD_ITEM(0x7F6,  8),  /* MICBIAS, LOUT_P,  LOUT_N,  ROUT_P */
    PAD_ITEM(0x7F6, 9), PAD_ITEM(0x7F6,  4), PAD_ITEM(0x7F6,   5), PAD_ITEM_EMPTY,       /* ROUT_N,  MIC3_P,  MIC3_N  */

    PAD_ITEM(0x7F0,  0), PAD_ITEM(0x7F0,  1), PAD_ITEM(0x7F0,  2), PAD_ITEM(0x7F0,  3),  /* P4_0 - P4_3 */
    PAD_ITEM(0x7F0,  4), PAD_ITEM(0x7F0,  5), PAD_ITEM(0x7F0,  6), PAD_ITEM(0x7F0,  7),  /* P4_4 - P4_7 */
    PAD_ITEM(0x7F0,  8), PAD_ITEM(0x7F0,  9), PAD_ITEM(0x7F0, 10), PAD_ITEM(0x7F0, 11),  /* P5_0 - P5_3 */
    PAD_ITEM(0x7F0, 12), PAD_ITEM(0x7F0, 13), PAD_ITEM(0x7F0, 14), PAD_ITEM(0x7F0, 15),  /* P5_4 - P5_7 */

    PAD_ITEM(0x7F2,  8), PAD_ITEM(0x7F2,  9), PAD_ITEM(0x7F2, 10), PAD_ITEM(0x7F2, 11),  /* P6_0 - P6_3 */
    PAD_ITEM(0x7F2, 12), PAD_ITEM(0x7F2, 13), PAD_ITEM(0x7F2, 14), PAD_ITEM_EMPTY,       /* P6_4 - P6_6 */
    PAD_ITEM(0x7F2,  0), PAD_ITEM(0x7F2,  1), PAD_ITEM(0x7F2,  2), PAD_ITEM(0x7F2,  3),  /* P7_0 - P7_3 */
    PAD_ITEM(0x7F2,  4), PAD_ITEM(0x7F2,  5), PAD_ITEM(0x7F2,  6), PAD_ITEM_EMPTY,       /* P7_4 - P7_6 */

    PAD_ITEM(0x7F4,  0), PAD_ITEM(0x7F4,  1), PAD_ITEM(0x7F4,  2), PAD_ITEM(0x7F4,  3),  /* P8_0 - P8_3 */
    PAD_ITEM(0x7F4,  4), PAD_ITEM(0x7F4,  5), PAD_ITEM_EMPTY,      PAD_ITEM_EMPTY,       /* P8_4 - P8_5 */
    PAD_ITEM(0x7F4,  8), PAD_ITEM(0x7F4,  9), PAD_ITEM(0x7F4, 10), PAD_ITEM(0x7F4, 11),  /* P9_0 - P9_3 */
    PAD_ITEM(0x7F4, 12), PAD_ITEM(0x7F4, 13), PAD_ITEM(0x7F8, 0),  PAD_ITEM_EMPTY,       /* P9_4 - P9_5  P10_0*/
};

uint8_t hci_uart_rx_pin = P3_0;
uint8_t hci_uart_tx_pin = P3_1;

const uint8_t digi_debug_pin[32] =
{
    P0_0, P0_1, P0_2, P0_3, P3_2, P3_3, P1_0, P1_1,                 //digi_debug_0 ~ 7
    P1_2, P1_3, P1_4, P1_5, P1_6, P1_7, P2_0, P2_1,                 //digi_debug_8 ~ 15
    P2_2, P2_3, P2_4, P2_5, P2_6, P2_7, P3_4, P3_5,                 //digi_debug_16 ~ 23
    AUX_R, AUX_L, MICBIAS, LOUT_P, LOUT_N, ROUT_P, ROUT_N, P4_0,    //digi_debug_24 ~ 31
};


/**
  * @brief  According to the mode set to the pin , write the regster of AON which the pin coresponding .
  * @param  mode: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  *     @arg SHDN: use software mode.
  *     @arg PAD_OUT_EN: use pinmux mode.
        ......
        reference of bit of AON register mean in pinmux.h
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  value: value of the register bit ,0 or 1.
  * @retval None
  */

void Pad_TableConfig(AON_FAST_PAD_BIT_POS_TYPE pad_bit_set, uint8_t Pin_Num, uint8_t value)
{
    AON_FAST_REG_PAD_TYPE tmpVal;
    uint16_t reg_temp;
    reg_temp = PINADDR_TABLE[Pin_Num];
    if (reg_temp == 0)
    {return;}
    tmpVal.d16 = btaon_fast_read_safe(reg_temp);
    if (value == 0)
    {
        tmpVal.d16 &= ~BIT(pad_bit_set);
    }
    else if (value == 1)
    {
        tmpVal.d16 |=   BIT(pad_bit_set);
    }
    else
    {
        if (pad_bit_set == PAD_PUPD_DIR)
        {
            tmpVal.PAD_PU_EN = 0;
        }
    }
    btaon_fast_write_safe(reg_temp, tmpVal.d16);
}

/**
  * @brief  Config pin to its corresponding IO function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Pin_Func: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  * @retval None
  */
void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func)
{
    uint8_t pinmux_reg_num;
    uint8_t reg_offset;

    if (Pin_Num >= TOTAL_PIN_NUM)
    {return;}

    if (Pin_Num < PINMUX_REG1_ST_PIN)
    {
        pinmux_reg_num = Pin_Num >> 2;
        reg_offset = (Pin_Num & 0x03) << 3;

        PINMUX0->CFG[pinmux_reg_num] = (PINMUX0->CFG[pinmux_reg_num] & ~(0xFF << reg_offset))
                                       | Pin_Func << reg_offset;
    }
    else
    {
        Pin_Num = Pin_Num - PINMUX_REG1_ST_PIN;
        pinmux_reg_num = Pin_Num >> 2;
        reg_offset = (Pin_Num & 0x03) << 3;
        PINMUX1->CFG[pinmux_reg_num] = (PINMUX1->CFG[pinmux_reg_num] & ~(0xFF << reg_offset))
                                       | Pin_Func << reg_offset;
    }
    return;
}

/**
  * @brief  Deinit the IO function of one pin.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */
void Pinmux_Deinit(uint8_t Pin_Num)
{
    uint8_t pinmux_reg_num;

    if (Pin_Num < PINMUX_REG1_ST_PIN)
    {
        pinmux_reg_num = Pin_Num >> 2;
        PINMUX0->CFG[pinmux_reg_num] &= ~(0xff << ((Pin_Num % 4) << 3));
    }
    else
    {
        Pin_Num = Pin_Num - PINMUX_REG1_ST_PIN;
        pinmux_reg_num = Pin_Num >> 2;
        PINMUX1->CFG[pinmux_reg_num] &= ~(0xff << ((Pin_Num % 4) << 3));
    }
    return;
}

/**
  * @brief  Reset all pin to default value.
  * @param  None.
  * @note: two SWD pins will also be reset. Please use this function carefully.
  * @retval None
  */


void Pinmux_Reset(void)
{
    uint8_t i;

    for (i = 0; i < PINMUX_REG0_NUM; i++)
    {
        PINMUX0->CFG[i] = 0x00;
    }
    for (i = 0; i < PINMUX_REG1_NUM; i++)
    {
        PINMUX1->CFG[i] = 0x00;
    }
    return;
}

/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_NOT_PWRON: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */

void Pad_Config(uint8_t Pin_Num,
                PAD_Mode AON_PAD_Mode,
                PAD_PWR_Mode AON_PAD_PwrOn,
                PAD_Pull_Mode AON_PAD_Pull,
                PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                PAD_OUTPUT_VAL AON_PAD_O)
{

    AON_FAST_REG_PAD_TYPE tmpVal;
    uint16_t reg_temp;

    if (Pin_Num >= TOTAL_PIN_NUM)
    {return;}

    reg_temp = PINADDR_TABLE[Pin_Num];
    if (reg_temp == 0)
    {return;}
    tmpVal.d16 = btaon_fast_read_safe(reg_temp);


    tmpVal.PAD_PINMUX_M_EN = AON_PAD_Mode;
    tmpVal.PAD_SHDN_PW_ON = AON_PAD_PwrOn;
    /* Pull Config */
    if (AON_PAD_Pull == PAD_PULL_NONE)
    {
        tmpVal.PAD_PU_EN = 0;
    }
    else
    {
        tmpVal.PAD_PU_EN = 1;
        tmpVal.PAD_PUPD_DIR = AON_PAD_Pull;
    }

    tmpVal.PAD_OUT_EN = AON_PAD_E;
    tmpVal.PAD_OUT_VALUE = AON_PAD_O;

    btaon_fast_write_safe(reg_temp, tmpVal.d16);
}

/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_NOT_PWRON: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */
void Pad_ConfigExt(uint8_t Pin_Num,
                   PAD_Mode AON_PAD_Mode,
                   PAD_PWR_Mode AON_PAD_PwrOn,
                   PAD_Pull_Mode AON_PAD_Pull,
                   PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                   PAD_OUTPUT_VAL AON_PAD_O,
                   PAD_PULL_VAL AON_PAD_P)
{

    Pad_TableConfig(PAD_SHDN_PW_ON, Pin_Num, AON_PAD_PwrOn);
    Pad_TableConfig(PAD_OUT_VALUE, Pin_Num, AON_PAD_O);
    Pad_TableConfig(PAD_OUT_EN, Pin_Num, AON_PAD_E);
    Pad_TableConfig(PAD_PUPDC_WE_STR, Pin_Num, AON_PAD_P);

    if (AON_PAD_Pull == PAD_PULL_DISABLE)
    {
        Pad_TableConfig(PAD_PU_EN, Pin_Num, 0);
    }
    else if (AON_PAD_Pull == PAD_PULL_UP)
    {
        Pad_TableConfig(PAD_PUPD_DIR, Pin_Num, 1);
        Pad_TableConfig(PAD_PU_EN, Pin_Num, 1);
    }
    else
    {
        Pad_TableConfig(PAD_PUPD_DIR, Pin_Num, 0);
        Pad_TableConfig(PAD_PU_EN, Pin_Num, 1);
    }

    Pad_TableConfig(PAD_PINMUX_M_EN, Pin_Num, AON_PAD_Mode);
}

/**
  * @brief  Set all pins to the default state.
  * @param  void.
  * @retval void.
  */

void Pad_AllConfigDefault(void)
{

    uint16_t i = 0;
    /* Set Output disable, pull-none, pull down, Software mode, Output_low, wakeup_disable, Wake up polarity high */
    for (i = 0; i < TOTAL_PIN_NUM; i++)
    {
        if (PINADDR_TABLE[i] == 0x0)
        {
            continue;
        }
        Pad_TableConfig(PAD_OUT_EN, i, PAD_OUT_DISABLE);
        Pad_TableConfig(PAD_PU_EN, i, PAD_PULL_DISABLE);
        Pad_TableConfig(PAD_PUPD_DIR, i, PAD_PULL_LOW);
        Pad_TableConfig(PAD_PINMUX_M_EN, i, PAD_SW_MODE);
        Pad_TableConfig(PAD_OUT_VALUE, i, PAD_OUT_LOW);
        Pad_TableConfig(PAD_WKEN, i, PAD_WAKEUP_DISABLE);
        Pad_TableConfig(PAD_WKPOL, i, PAD_WAKEUP_POL_HIGH);
    }

}

/**
  * @brief  Enable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Polarity: PAD_WAKEUP_POL_HIGH--use high level wakeup, PAD_WAKEUP_POL_LOW-- use low level wakeup.
  * @retval None
  */
void System_WakeUpPinEnable(uint8_t Pin_Num, uint8_t Polarity)
{
    if (Pin_Num >= TOTAL_PIN_NUM)
    {return;}

    Pad_WakeupPolarityValue(Pin_Num, Polarity);
    Pad_WakeupEnableValue(Pin_Num, 1);
}

/**
  * @brief  Disable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */

void System_WakeUpPinDisable(uint8_t Pin_Num)
{
    if (Pin_Num >= TOTAL_PIN_NUM)
    {return;}

    Pad_WakeupEnableValue(Pin_Num, 0);
}

void System_WakeUpInterruptEnable(uint8_t Pin_Num)
{
    /*bbpro2 not have this bit */
    Pad_TableConfig(PAD_WKUP_INT_EN, Pin_Num,  1) ;
}

void System_WakeUpInterruptDisable(uint8_t Pin_Num)
{
    Pad_TableConfig(PAD_WKUP_INT_EN, Pin_Num,  0) ;
    Pad_ClearWakeupINTPendingBit(Pin_Num);
}

/**
  * @brief  Check wake up pin interrupt status.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval Pin interrupt status
  */


/**
  * @brief  Clear all wake up pin interrupt pending bit.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part. PAD_Px_STS[x]
  * @retval None
  */
void Pad_ClearAllWakeupINT(void)
{
    btaon_fast_write_safe(0x7EC, PAD_WK_STS_DATA_AON_REG_7EC);
    btaon_fast_write_safe(0x7EE, PAD_WK_STS_DATA_AON_REG_7EE);
    btaon_fast_write_safe(0x7F0, PAD_WK_STS_DATA_AON_REG_7F0);
    btaon_fast_write_safe(0x7F2, PAD_WK_STS_DATA_AON_REG_7F2);
    btaon_fast_write_safe(0x7F4, PAD_WK_STS_DATA_AON_REG_7F4);
    btaon_fast_write_safe(0x7F6, PAD_WK_STS_DATA_AON_REG_7F6);
    btaon_fast_write_safe(0x7F8, PAD_WK_STS_DATA_AON_REG_7F8);
}

void Pad_PullEnableValue_Dir(uint8_t Pin_Num, uint8_t value, PAD_Pull_Mode Pull_Direction_value)
{
    uint16_t addr = PINADDR_TABLE[Pin_Num];
    if (addr == 0)
    {return;}
    AON_FAST_REG_PAD_TYPE tmpVal;

    tmpVal.d16 = btaon_fast_read_safe(addr);
    if (Pull_Direction_value == PAD_PULL_NONE || value == 0)
    {
        tmpVal.PAD_PU_EN = 0;
    }
    else
    {
        tmpVal.PAD_PU_EN = 1;
    }
    tmpVal.PAD_PUPD_DIR = Pull_Direction_value;
    btaon_fast_write_safe((addr), tmpVal.d16);
}

FlagStatus Pad_WakeupInterruptValue(uint8_t Pin_Num)
{
    uint16_t pad_item = WKSTATUS_TABLE[Pin_Num];
    if (pad_item == 0)
    {
        return RESET;
    }
    if (btaon_fast_read_safe(PAD_ITEM_ADDR(pad_item)) & BIT(PAD_ITEM_BIT_OFFSET(pad_item)))
    {
        return SET;
    }
    return RESET;
}

void Pad_ClearWakeupINTPendingBit(uint8_t Pin_Num)
{
    uint16_t reg_temp;
    AON_FAST_REG_PAD_TYPE tmpVal;
    reg_temp = WKSTATUS_TABLE[Pin_Num];
    if (reg_temp == 0)
    {return;}
    if (Pin_Num == P9_5)
    {
        btaon_fast_write_safe(reg_temp, 0);
    }
    tmpVal.d16 = btaon_fast_read_safe(reg_temp);
    btaon_fast_write_safe(reg_temp, tmpVal.d16);
}

const uint16_t SLEEP_LED_PIN_REG[10] =  /*static*/
{
    /*ADC_0                          ADC_1                 P1_0                P1_1                 P2_1     */
    ((12 << 12) | 0x744), ((9 << 12) | 0x744), ((6 << 12) | 0x744), ((3 << 12) | 0x744), ((9 << 12) | 0x746),
    /*P2_2                          P2_3                P2_4                  P3_0                              P3_1                */
    ((6 << 12) | 0x746), ((3 << 12) | 0x746), ((0 << 12) | 0x746), ((12 << 12) | 0x748), ((9 << 12) | 0x748)
};
const uint8_t  pin_led[10] =
{
    ADC_0, ADC_1, P1_0, P1_1,
    P2_1,  P2_2,  P2_3,  P2_4,
    P3_0,  P3_1
};
/**
  * @brief  Config Pad Function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P7_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  PAD_FUNCTION_CONFIG_VAL: value.
  *   This parameter can be: AON_GPIO, LED0, LED1, LED2, CLK_REQ.
  * @retval None
  */
void Pad_FunctionConfig(uint8_t Pin_Num, PAD_FUNCTION_CONFIG_VAL value)
{
    uint8_t i = 0;
    uint16_t tmpVal = 0;
    uint16_t reg_temp;
    uint16_t pos_temp;

    for (i = 0; i < 10 ; i++)
    {
        if (Pin_Num == pin_led[i])
        {
            break;
        }
    }
    if (i >= 10) { return; }
    /* Pad control mode */
    reg_temp = (SLEEP_LED_PIN_REG[i] & (0xfff));
    pos_temp = (SLEEP_LED_PIN_REG[i] & (0xf000)) >> 12;
    tmpVal = btaon_fast_read_safe(reg_temp);
    tmpVal &= ~((0x7) << pos_temp);
    tmpVal |= ((value) << pos_temp);
    btaon_fast_write_safe(reg_temp, tmpVal);
}

/**
  * @brief  Get the Pad AON fast register value ,the register addr get through mode and pin.
  * @param  mode: mode of set .
  *            This parameter reference the define from DRIE2 to PAD_PUPDC_WE_STR in rtl876x_pinmux.h .
  *            Pin_Num:  pin number
  *            This parameter is from ADC_0 to P7_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval reference PAD_Mode_Status.
  */
uint8_t  Pad_GetModeConfig(AON_FAST_PAD_BIT_POS_TYPE mode, uint8_t Pin_Num)
{
    uint16_t reg_temp;

    AON_FAST_REG_PAD_TYPE tmpVal;
    if (Pin_Num < TOTAL_PIN_NUM)
    {
        reg_temp = PINADDR_TABLE[Pin_Num];
        if (reg_temp == 0)
        {return PAD_AON_PIN_ERR;}
        tmpVal.d16 = btaon_fast_read_safe(reg_temp);

        if (tmpVal.d16 & BIT(mode))
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else
    {
        return PAD_AON_PIN_ERR;
    }

}

/**
  * @brief  Get the Pad AON output value .
  * @param  Pin_Num:  pin number
  *            This parameter is from ADC_0 to H_12, please refer to rtl876x.h "Pin_Number" part.
  * @retval reference PAD_AON_Status.
  */
uint8_t Pad_GetOutputCtrl(uint8_t Pin_Num)
{
    if (Pad_GetModeConfig(PAD_OUT_EN, Pin_Num) == RESET)
    {
        return PAD_AON_OUTPUT_DISABLE;
    }
    else if (Pad_GetModeConfig(PAD_PINMUX_M_EN, Pin_Num) == SET)
    {
        return PAD_AON_PINMUX_ON;
    }
    if (Pad_GetModeConfig(PAD_OUT_VALUE, Pin_Num) == RESET)
    {
        return PAD_AON_OUTPUT_LOW;
    }
    else if (Pad_GetModeConfig(PAD_OUT_VALUE, Pin_Num) == SET)
    {
        return PAD_AON_OUTPUT_HIGH;
    }
    else
    {
        return PAD_AON_PIN_ERR;
    }
}

/**
  * @brief  set the system wakeup mode  .
  * @param  mode: mode of set .
  *            This parameter reference WAKEUP_EN_MODE .
  *        pol:  polarity to wake up
  *            This parameter WAKEUP_POL POL_HIGH means high level POL_LOW means low level to wakeup.
  *        NewState:  Enable or disable to wake up
  *            This parameter value is ENABLE or DISABLE.
  * @retval  1 means wrong mode.
  */
uint8_t  Pad_WakeUpCmd(WAKEUP_EN_MODE mode, WAKEUP_POL pol, FunctionalState NewState)
{
    AON_FAST_SET_WKEN_MISC_TYPE wkup_wken = {.d16 = btaon_fast_read(AON_FAST_SET_WKEN_MISC)};
    AON_FAST_SET_WKPOL_MISC_TYPE wkup_wkpol = {.d16 = btaon_fast_read(AON_FAST_SET_WKPOL_MISC)};
    switch (mode)
    {
    case ADP_MODE:
        wkup_wken.ADP_WKEN = NewState;
        wkup_wkpol.ADP_WKPOL = pol;
        break;
    case BAT_MODE:
        wkup_wken.BAT_WKEN = NewState;
        wkup_wkpol.BAT_WKPOL = pol;
        break;
    case MFB_MODE:
        wkup_wken.MFB_WKEN = NewState;
        wkup_wkpol.MFB_WKPOL = !pol;
        break;
    case USB_MODE:
        wkup_wken.USB_WKEN = NewState;
        wkup_wkpol.USB_WKPOL = pol;
        break;
    default:
        return 1;
    }
    btaon_fast_write_safe(AON_FAST_SET_WKEN_MISC, wkup_wken.d16);
    btaon_fast_write_safe(AON_FAST_SET_WKPOL_MISC, wkup_wkpol.d16);
    return 0;
}

typedef struct T_PAD_ANALOG_ITEM
{
    uint16_t pin_index;
    uint16_t item;
} T_PAD_ANALOG_ITEM;

void Pad_DrivingCurrentControl(uint8_t Pin_Num, uint8_t e2_value, uint8_t e3_value)
{
    Pad_TableConfig(PAD_E2, Pin_Num, e2_value);
    Pad_TableConfig(PAD_E3, Pin_Num, e3_value);
}

static const char *const pin_name[TOTAL_PIN_NUM] =
{
    "P0_0",   "P0_1",   "P0_2",   "P0_3",   "P3_2",   "P3_3",   "P3_4",   "P3_5",
    "P1_0",   "P1_1",   "P1_2",   "P1_3",   "P1_4",   "P1_5",   "P1_6",   "P1_7",
    "P2_0",   "P2_1",   "P2_2",   "P2_3",   "P2_4",   "P2_5",   "P2_6",   "P2_7",
    "P3_0",   "P3_1",   "AUX_L",   "AUX_R",   "MIC1_P",   "MIC1_N",   "MIC2_P",   "MIC2_N",
    "MICBIAS", "LOUT_P", "LOUT_N", "ROUT_P",  "ROUT_N", "MIC3_P", "MIC3_N", "NULL_PIN",
    "P4_0",   "P4_1",   "P4_2",   "P4_3",   "P4_4",   "P4_5",   "P4_6",   "P4_7",
    "P5_0",   "P5_1",   "P5_2",   "P5_3",   "P5_4",   "P5_5",   "P5_6",   "P5_7",
    "P6_0",   "P6_1",   "P6_2",   "P6_3",   "P6_4",   "P6_5",   "P6_6",   "NULL_PIN",
    "P7_0",   "P7_1",       "P7_2",   "P7_3",   "P7_4",   "P7_5",   "P7_6",   "NULL_PIN",
    "P8_0",   "P8_1",       "P8_2",   "P8_3",   "P8_4",   "P8_5",   "NULL_PIN",   "NULL_PIN",
    "P9_0",   "P9_1",       "P9_2",   "P9_3",   "P9_4",   "P9_5",   "P10_0",   "NULL_PIN",
};

const char *Pad_GetPinName(uint8_t pin_num)
{
    return pin_name[pin_num];
}

void System_SetAdpWakeUpFunction(FunctionalState NewState, WAKEUP_POL pol)
{
    AON_RG4X_TYPE aon_rg4x = {.d32 = HAL_READ32(SYSTEM_REG_BASE, AON_RG4X)};
    AON_FAST_REG9X_MBIAS_TYPE aon_fast_reg9x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG9X_MBIAS)};
    aon_fast_reg9x_mbias. MBIAS_DPD_R_5 = !pol;
    aon_fast_reg9x_mbias. MBIAS_DPD_R_6 = NewState;
    btaon_fast_write_safe(AON_FAST_REG9X_MBIAS, aon_fast_reg9x_mbias.d16);
    aon_rg4x.DPD_RCK = 1;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_RG4X, aon_rg4x.d32);
    aon_rg4x.DPD_RCK = 0;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_RG4X, aon_rg4x.d32);
}

void System_DisableAdpWakeUpFunction(void)
{
    AON_FAST_REG9X_MBIAS_TYPE aon_fast_reg9x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG9X_MBIAS)};
    aon_fast_reg9x_mbias. MBIAS_DPD_R_6 = DISABLE;
    btaon_fast_write_safe(AON_FAST_REG9X_MBIAS, aon_fast_reg9x_mbias.d16);
}

void System_SetMFBWakeUpFunction(FunctionalState NewState)
{
    AON_RG4X_TYPE aon_rg4x = {.d32 = HAL_READ32(SYSTEM_REG_BASE, AON_RG4X)};
    AON_FAST_REG9X_MBIAS_TYPE aon_fast_reg9x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG9X_MBIAS)};
    aon_fast_reg9x_mbias. MBIAS_DPD_R_7 = NewState;
    btaon_fast_write_safe(AON_FAST_REG9X_MBIAS, aon_fast_reg9x_mbias.d16);
    aon_rg4x.DPD_RCK = 1;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_RG4X, aon_rg4x.d32);
    aon_rg4x.DPD_RCK = 0;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_RG4X, aon_rg4x.d32);
}

static const uint8_t PIN_E3_EN_tbl[] =
{
    P1_0,    P1_1,    P1_2,    P1_3,    P1_4,    P1_5,    P1_6,    P1_7,
    P3_0,    P3_1,    P3_2,    P3_3,    P3_4,    P3_5,
    P5_0,    P5_1,    P5_2,    P5_3,    P5_4,    P5_5,    P5_6,    P5_7,
    P6_0,    P6_1,    P6_2,    P6_3,    P6_4,    P6_5,    P6_6,
    P7_0,    P7_1,    P7_2,    P7_3,    P7_4,    P7_5,    P7_6,
    AUX_R,    AUX_L,     MIC1_P,     MIC1_N,    MIC2_P, MIC2_N,   MICBIAS,    LOUT_P,
    LOUT_N,  ROUT_P, ROUT_N,    MIC3_P,    MIC3_N
};

static bool PAD_E3Enable(uint8_t pin)
{
    uint8_t i;

    for (i = 0; i < sizeof(PIN_E3_EN_tbl); i++)
    {
        if (PIN_E3_EN_tbl[i] == pin)
        {
            return true;
        }
    }
    return false;
}

bool Pad_SetPinDrivingCurrent(uint8_t pin, T_DRIVER_LEVEL_MODE driver_level)
{
    if (pin >= TOTAL_PIN_NUM)
    {
        return false;
    }

    if (driver_level > LEVEL1)
    {
        if (PAD_E3Enable(pin))
        {
            Pad_DrivingCurrentControl(pin, (driver_level & BIT(0)), ((driver_level & BIT(1))) >> 1);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        Pad_DrivingCurrentControl(pin, (driver_level & BIT(0)), ((driver_level & BIT(1))) >> 1);
        return true;
    }
}

const T_PAD_ANALOG_ITEM analog_table[] =
{
    {ROUT_N,   PAD_ITEM(BTAON_FAST_120, 0)},
    {ROUT_P,   PAD_ITEM(BTAON_FAST_120, 1)},
    {LOUT_N,   PAD_ITEM(BTAON_FAST_120, 2)},
    {LOUT_P,   PAD_ITEM(BTAON_FAST_120, 3)},
    {MIC2_N,   PAD_ITEM(BTAON_FAST_120, 4)},
    {MIC2_P,   PAD_ITEM(BTAON_FAST_120, 5)},
    {MIC1_N,   PAD_ITEM(BTAON_FAST_120, 6)},
    {MIC1_P,   PAD_ITEM(BTAON_FAST_120, 7)},
    {MICBIAS,  PAD_ITEM(BTAON_FAST_122, 13)},
    {AUX_R,    PAD_ITEM(BTAON_FAST_122, 14)},
    {AUX_L,    PAD_ITEM(BTAON_FAST_122, 15)},
    {MIC3_N,   PAD_ITEM(BTAON_FAST_164, 14)},
    {MIC3_P,   PAD_ITEM(BTAON_FAST_164, 15)},
};

void Pad_AnalogMode(uint8_t pin, ANA_MODE mode)
{
    if ((pin < AUX_R) || (pin > MIC3_N))
    {
        return;
    }

    for (uint16_t i = 0; i < sizeof(analog_table) / sizeof(T_PAD_ANALOG_ITEM); i++)
    {
        if (analog_table[i].pin_index == pin)
        {
            uint16_t pad_item = analog_table[i].item;

            if (pad_item != 0)
            {
                uint16_t addr = PAD_ITEM_ADDR(pad_item);
                uint16_t bit = BIT(PAD_ITEM_BIT_OFFSET(pad_item));

                if (mode == PAD_ANALOG_MODE)
                {
                    btaon_fast_update_safe(addr, bit, bit);
                }
                else
                {
                    btaon_fast_update_safe(addr, bit, 0);
                }
            }

            break;
        }
    }
}

bool Pad_WakeUpDisable(WAKEUP_EN_MODE mode)
{
    AON_FAST_SET_WKEN_MISC_TYPE wkup_wken = {.d16 = btaon_fast_read(AON_FAST_SET_WKEN_MISC)};
    switch (mode)
    {
    case ADP_MODE:
        wkup_wken.ADP_WKEN = DISABLE;
        break;
    case BAT_MODE:
        wkup_wken.BAT_WKEN = DISABLE;
        break;
    case MFB_MODE:
        wkup_wken.MFB_WKEN = DISABLE;
        break;
    case USB_MODE:
        wkup_wken.USB_WKEN = DISABLE;
        break;
    default:
        return false;
    }
    btaon_fast_write_safe(AON_FAST_SET_WKEN_MISC, wkup_wken.d16);
    return true;
}

int32_t Pad_GetConfig(uint8_t pin_num,
                      PAD_Mode *mode,
                      PAD_PWR_Mode *pwr_mode,
                      PAD_Pull_Mode *pullup_config,
                      PAD_OUTPUT_ENABLE_Mode *output_en,
                      PAD_OUTPUT_VAL *output_val)
{
    if (pin_num >= TOTAL_PIN_NUM)
    {
        APP_PRINT_ERROR1("Pad_GetConfig pin =%d is invaild", pin_num);
        return -1;
    }

    uint16_t reg_temp = PINADDR_TABLE[pin_num];
    uint16_t tmp_val;

    if (reg_temp == 0x00)
    {
        return (-1);
    }

    /* Pad control mode */
    tmp_val = btaon_fast_read_safe(reg_temp);
    if (tmp_val & BIT(PAD_PINMUX_M_EN))
    {
        *mode = PAD_PINMUX_MODE;
    }
    else
    {
        *mode = PAD_SW_MODE;
    }

    if (tmp_val & BIT(PAD_PU_EN))
    {
        if (tmp_val & BIT(PAD_PUPD_DIR))
        {
            *pullup_config = PAD_PULL_UP;
        }
        else
        {
            *pullup_config = PAD_PULL_DOWN;
        }
    }
    else
    {
        *pullup_config = PAD_PULL_NONE;
    }

    if (tmp_val & BIT(PAD_OUT_EN))
    {
        *output_en = PAD_OUT_ENABLE;
        if (tmp_val & BIT(PAD_OUT_VALUE))
        {
            *output_val = PAD_OUT_HIGH;
        }
        else
        {
            *output_val = PAD_OUT_LOW;
        }
    }
    else
    {
        *output_en = PAD_OUT_DISABLE;
    }

    if (tmp_val & BIT(PAD_SHDN_PW_ON))
    {
        *pwr_mode = PAD_IS_PWRON;
    }
    else
    {
        *pwr_mode = PAD_SHUTDOWN;
    }

    return 0;
}
