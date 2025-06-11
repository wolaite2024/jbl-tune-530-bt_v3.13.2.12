#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "Battery_Verification.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "platform_utils.h"
#include "trace.h"
#include "hw_tim.h"
#include "section.h"
#include "rtl876x.h"
#include "pm.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "vector_table.h"
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_ir.h"

#define GPIO_NUM P0_3
static T_HW_TIMER_HANDLE demo_timer_handle = NULL;

//Modify below two functions with your own delay functions
//************************************************************
void delay_1us(unsigned int us)
{
    // Delay_Us(us);
    platform_delay_us(us);
}

void delay_1ms(unsigned char ms)
{
    // Delay_Ms(ms);
    platform_delay_ms(ms);
}

//************************************************************

/* Buffer which store receiving data */
uint32_t IR_DataStruct[100];

/* Number of data which has been receoved */
unsigned char edge_count = 0;

//????gpio_isr_cb??,?????
ISR_TEXT_SECTION
static void ir_handler(void)
{
    uint16_t len = 0;
    uint32_t high_cnt = 0;
    uint32_t low_cnt = 0;
    uint32_t high_us = 0;
    uint32_t low_us = 0;

    //DBG_DIRECT("lzy ir_handler");

    /* Receive by interrupt */
    if (IR_GetINTStatus(IR_INT_RF_LEVEL) == SET)
    {
        //DBG_DIRECT("lzy ir_handler IR_INT_RF_LEVEL");
        IR_MaskINTConfig(IR_INT_RF_LEVEL, ENABLE);
        len = IR_GetRxDataLen();
        IR_ReceiveBuf(IR_DataStruct + edge_count, len);
        edge_count += len;
        IR_ClearINTPendingBit(IR_INT_RF_LEVEL_CLR);
        IR_MaskINTConfig(IR_INT_RF_LEVEL, DISABLE);
        //IR_Cmd(IR_MODE_RX, DISABLE);
        //DBG_DIRECT("ir_handler IR_INT_RF_LEVEL %d", len);
    }

    /* Stop to receive IR data */
    if (IR_GetINTStatus(IR_INT_RX_CNT_THR) == SET)
    {
        IR_MaskINTConfig(IR_INT_RX_CNT_THR, ENABLE);
        /* Read remaining data */
        len = IR_GetRxDataLen();
        IR_ReceiveBuf(IR_DataStruct + edge_count, len);
        edge_count += len;

        IR_ClearINTPendingBit(IR_INT_RX_CNT_THR_CLR);
        IR_MaskINTConfig(IR_INT_RX_CNT_THR, DISABLE);
        IR_Cmd(IR_MODE_RX, DISABLE);

        /* This represent the total cnt */
        //DBG_DIRECT("lzy edge_count %d", edge_count);
        timeRdSt.timePointer = edge_count;
        IO_PRINT_INFO1("edge_count %d", edge_count);            //??????????????
        IO_PRINT_INFO1("timeRdSt.timePointer %d", timeRdSt.timePointer);

        /* If you want high/low information, can open this */
        for (uint16_t i = 0; i < edge_count; i++)
        {

            if (IR_DataStruct[i] & BIT(31))                     //?????????????,??????
            {
                /* High level time cnt */
                high_cnt = (IR_DataStruct[i] & 0x7FFFFFFF) + 1;
                high_us = high_cnt / 10;    //us                //???????????????
                //????:high_cnt * 1 / 10000000 * 1000000 us
                timeRdSt.timegap[i] = (unsigned char)high_us;
                //IO_PRINT_INFO3("high %d 0x%x ", i, IR_DataStruct[i], high_us);
                IO_PRINT_INFO2("timeRdSt.timegap[%d] %d", i, timeRdSt.timegap[i]);
            }
            else
            {
                /* Low level time cnt */
                low_cnt = IR_DataStruct[i] + 1;
                low_us = low_cnt / 10; //us                                 //???????????????
                timeRdSt.timegap[i] = (unsigned char)low_us;
                //IO_PRINT_INFO3("low %d 0x%x %d", i, IR_DataStruct[i], low_us);
                IO_PRINT_INFO2("timeRdSt.timegap[%d] %d", i, timeRdSt.timegap[i]);
            }
        }

    }
}

void BAT_ID_INPUT(void)
{
    //DBG_DIRECT("lzy BAT_ID_INPUT");
    edge_count = 0;
    memset(IR_DataStruct, 0, sizeof(IR_DataStruct));

    Pinmux_Deinit(GPIO_NUM);
    Pad_Config(GPIO_NUM, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(GPIO_NUM, IRDA_RX);

    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize IR */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);

    IR_InitStruct.IR_Freq               =
        10000;        //???????,???khz,????10M
    IR_InitStruct.IR_Mode               = IR_MODE_RX;
    IR_InitStruct.IR_RxStartMode        = IR_RX_AUTO_MODE;
    IR_InitStruct.IR_RxFIFOThrLevel     = 2;
    IR_InitStruct.IR_RxFIFOFullCtrl     = IR_RX_FIFO_FULL_DISCARD_NEWEST;
    IR_InitStruct.IR_RxTriggerMode      = IR_RX_DOUBLE_EDGE;
    IR_InitStruct.IR_RxFilterTime       = IR_RX_FILTER_TIME_200ns;
    IR_InitStruct.IR_RxCntThrType       = IR_RX_Count_Low_Level;
    IR_InitStruct.IR_RxCntThr           = 10000;   //??????????????????????,??????rx??
    //????:1 / 10000000 * 200000 * 1000 = 20ms
    IR_Init(&IR_InitStruct);

    RamVectorTableUpdate(IR_VECTORn, (IRQ_Fun)ir_handler);

    /* Enable IR threshold interrupt. when RX FIFO offset >= threshold value, trigger interrupt*/
    /* Enable IR counter threshold interrupt to stop receiving data */
    IR_INTConfig(IR_INT_RF_LEVEL | IR_INT_RX_CNT_THR, ENABLE);
    IR_MaskINTConfig(IR_INT_RF_LEVEL | IR_INT_RX_CNT_THR, DISABLE);

    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    IR_ClearRxFIFO();
    IR_Cmd(IR_MODE_RX, ENABLE);
}

void BAT_ID_OUTPUT(void)
{
    DBG_DIRECT("lzy BAT_ID_OUTPUT");
    hal_gpio_init_pin(GPIO_NUM, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_DOWN);
}

//??????RF RX input,??????input??
void BAT_ID_INPUT_INT_Init(void)
{
    IO_PRINT_INFO0("lzy BAT_ID_INPUT_INT_Init");

    Pinmux_Deinit(GPIO_NUM);
    Pad_Config(GPIO_NUM, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(GPIO_NUM, IRDA_RX);

    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize IR */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);

    IR_InitStruct.IR_Freq               =
        10000;        //???????,???khz,????10M
    IR_InitStruct.IR_Mode               = IR_MODE_RX;
    IR_InitStruct.IR_RxStartMode        = IR_RX_AUTO_MODE;
    IR_InitStruct.IR_RxFIFOThrLevel     = 2;
    IR_InitStruct.IR_RxFIFOFullCtrl     = IR_RX_FIFO_FULL_DISCARD_NEWEST;
    IR_InitStruct.IR_RxTriggerMode      = IR_RX_DOUBLE_EDGE;
    IR_InitStruct.IR_RxFilterTime       = IR_RX_FILTER_TIME_200ns;
    IR_InitStruct.IR_RxCntThrType       = IR_RX_Count_Low_Level;
    IR_InitStruct.IR_RxCntThr           = 10000;   //??????????????????????,??????rx??
    //????:1 / 10000000 * 200000 * 1000 = 20ms
    IR_Init(&IR_InitStruct);

    RamVectorTableUpdate(IR_VECTORn, (IRQ_Fun)ir_handler);

    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

}


//Modify below two function with your BAT_ID pin interrupt_enable and interrupt_disable function
//************************************************************
void BAT_ID_INT_Disable(void)
{
    hal_gpio_irq_disable(GPIO_NUM);
    //APP_StopBatIdExti();
}

void BAT_ID_INT_Enable(void)
{
    hal_gpio_irq_enable(GPIO_NUM);
    //APP_ConfigBatIdExti();
}
//************************************************************


//Modify below function with your timer function
//Please set your timer as the following requirement
/**
 * Timer should set to "Count Down Mode"
 * Timer counting gap should set to 1us
 * Timer counting cycle should set to 256
 * Timer interrupt should be shut down
**/
//************************************************************


//static void demo_hw_timer_callback(T_HW_TIMER_HANDLE handle)
//{
//Add User code here
//Time_Record_Callback();
// DBG_DIRECT("demo_hw_timer_callback");
//}


static void tim_driver_init(void)
{
    demo_timer_handle = hw_timer_create("demo_hw_timer", 256, true, NULL);
    if (demo_timer_handle == NULL)
    {
        DBG_DIRECT("tim_driver_init: fail to create hw timer, check hw timer usage");
        return;
    }

    DBG_DIRECT("tim_driver_init: create hw timer instance successfully, id %d",
               hw_timer_get_id(demo_timer_handle));
    hw_timer_get_id(demo_timer_handle);
}


void TIMER_Config(void)
{
    //APP_ConfigTIM2();
    tim_driver_init();
}



void TimerEnable(void)
{
    hw_timer_start(demo_timer_handle);
    //LL_TIM_EnableCounter(TIM2);
}

//Modify this function to disable your timer
void TimerDisable(void)
{
    hw_timer_stop(demo_timer_handle);
    //LL_TIM_DisableCounter(TIM2);
}
//************************************************************


void BAT_ID_OUTPUT_LOW(void)
{
    // GPIO_BAT_ID_Low();
    hal_gpio_set_level(GPIO_NUM, GPIO_LEVEL_LOW);
}

void BAT_ID_OUTPUT_HIGH(void)
{
    // GPIO_BAT_ID_High();
    hal_gpio_set_level(GPIO_NUM, GPIO_LEVEL_HIGH);
}

void BAT_ID_PULL_UP(void)
{
    // GPIO_BAT_ID_Pull_Up();
    hal_gpio_set_pull_value(GPIO_NUM, GPIO_PULL_UP);
}

void BAT_ID_PULL_DOWN(void)
{
    // GPIO_BAT_ID_Pull_Down();
    hal_gpio_set_pull_value(GPIO_NUM, GPIO_PULL_DOWN);
}
//************************************************************

//Modify this function by returning the current timer value we set in this file.
//************************************************************
uint32_t Get_Timer_Value(void)
{
    //unsigned char Timer_Value;
    //Timer_Value = LL_TIM_GetCounter(TIM2);
    uint32_t value = 0;
    hw_timer_get_current_count(demo_timer_handle, &value);
    return value;

    //return LL_TIM_GetCounter(TIM2);//Timer_Value;
}
//************************************************************

//Modify this function by returning the Value of BAT_ID GPIO.
//************************************************************
unsigned char Get_BAT_ID_Value(void)
{
    //unsigned char Gpio_Value;
    //Gpio_Value = GPIO_BAT_ID_Read();
    hal_gpio_init_pin(GPIO_NUM, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_NONE);
    return hal_gpio_get_input_level(GPIO_NUM);
}
//************************************************************
