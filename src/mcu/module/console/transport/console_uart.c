/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "board.h"
#include "os_msg.h"
#include "os_sync.h"
#include "rtl876x_uart.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "app_io_msg.h"
#include "app_dlps.h"
#include "console.h"
#include "vector_table.h"
#include "section.h"
#include "rtl876x.h"
#include "pm.h"
#include "dma_channel.h"
#include "console_uart.h"

#if (F_APP_ONE_WIRE_UART_SUPPORT == 1)
#include "app_one_wire_uart.h"
#endif

#define RX_WAKE_UP_SUPPORT      0

#if RX_WAKE_UP_SUPPORT
#include "hal_gpio.h"
#endif

/********************************************
DMA Channel/vector_table/GPIO_InitTypeDef
must follow BB2
********************************************/
// Data Uart Handler
#define Data_Uart_Handler  UART0_Handler

#undef UART_TX_DMA_CHANNEL_NUM
#undef UART_TX_DMA_CHANNEL
#undef UART_TX_DMA_IRQ

#undef UART_RX_DMA_CHANNEL_NUM
#undef UART_RX_DMA_CHANNEL
#undef UART_RX_DMA_IRQ

#define UART_TX_DMA_CHANNEL_NUM     console_tx_dma_ch_num
#define UART_TX_DMA_CHANNEL         DMA_CH_BASE(console_tx_dma_ch_num)
#define UART_TX_DMA_IRQ             DMA_CH_IRQ(console_tx_dma_ch_num)

#define UART_RX_DMA_CHANNEL_NUM     console_rx_dma_ch_num
#define UART_RX_DMA_CHANNEL         DMA_CH_BASE(console_rx_dma_ch_num)
#define UART_RX_DMA_IRQ             DMA_CH_IRQ(console_rx_dma_ch_num)

void console_uart_handle_tx_done(void);
bool uart_send_by_cpu_action(uint8_t *buf, uint32_t len);

uint8_t  uart_rx_buf_addr[RX_GDMA_BUFFER_SIZE];
uint16_t uart_tx_count = 0;

P_CONSOLE_CALLBACK p_console_callback = NULL;

uint8_t console_tx_dma_ch_num = 0xa5;
uint8_t console_rx_dma_ch_num = 0xa5;

static uint8_t *uart_tx_curr_addr;
static uint32_t uart_tx_len;

T_CONSOLE_UART_CONFIG g_console_uart;

/**

 * @brief  Uart interface process received data accordint to interrupt.
 *         If enable GDMA, GDMA will carry data from UART peripheral. GDMA only need idle interrupt.
 *         GDMA can trigger data uart interrupt every receive 128 bytes data, and
 *         the number has been setting in uart driver init.
 *         When uart rx timeout interrupt cause, app will read UART peripheral data until
 *         UART FIFO is empty.
 * @param  void
 * @return void
 */
RAM_TEXT_SECTION void Data_Uart_Handler(void)
{
    uint32_t int_status = 0;
    uint16_t rx_count = 0;
    uint16_t fifo_count;

    int_status = UART_GetIID(UART0);

    if (UART_GetFlagState(UART0, UART_FLAG_RX_IDLE) == SET)
    {
        //clear Flag
        UART_INTConfig(UART0, UART_INT_IDLE, DISABLE);

        if (g_console_uart.uart_rx_dma_enable)
        {
            //suspend will cause FIFO length + 1
            //read DMA FIFO length before suspend
            GDMA_SuspendCmd(UART_RX_DMA_CHANNEL, ENABLE);
            /*should waiting or fifo_count will not right when rx one byte or three byte*/
            while (GDMA_GetFIFOStatus(UART_RX_DMA_CHANNEL) != SET);
            //read DMA FIFO length before suspend
            fifo_count = GDMA_GetTransferLen(UART_RX_DMA_CHANNEL);
            GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, DISABLE);
            rx_count = fifo_count;
            if (p_console_callback)
            {
                p_console_callback(CONSOLE_EVT_DATA_IND, uart_rx_buf_addr, rx_count);
            }

            GDMA_SetDestinationAddress(UART_RX_DMA_CHANNEL, (uint32_t)uart_rx_buf_addr);
            GDMA_SuspendCmd(UART_RX_DMA_CHANNEL, DISABLE);
            GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, ENABLE);
            UART_INTConfig(UART0, UART_INT_IDLE, ENABLE);
        }
    }

    if ((int_status == UART_INT_ID_RX_TMEOUT) || (int_status == UART_INT_ID_RX_LEVEL_REACH))
    {
        rx_count = UART_GetRxFIFOLen(UART0);
        UART_ReceiveData(UART0, uart_rx_buf_addr, rx_count);
        if (p_console_callback)
        {
            p_console_callback(CONSOLE_EVT_DATA_IND, uart_rx_buf_addr, rx_count);
        }
    }
    else if (int_status == UART_INT_ID_TX_EMPTY)
    {
        UART_INTConfig(UART0, UART_INT_ID_TX_EMPTY, (FunctionalState)DISABLE);
        UART_GetIID(UART0);
        if (uart_tx_len)
        {
            uart_send_by_cpu_action(uart_tx_curr_addr, uart_tx_len);
        }
        else
        {
            console_uart_handle_tx_done();
        }
    }
#if F_APP_ONE_WIRE_UART_SUPPORT
    else if (int_status == UART_INT_ID_LINE_STATUS)
    {
        uint8_t line_status = UART_GetLineStatus(UART0);

        if (UART_CheckLineStatus(line_status, UART_FLAG_BREAK_ERR))
        {
            T_IO_MSG uart_msg;

            UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_IDLE | UART_INT_LINE_STS, DISABLE);

            uart_msg.type = IO_MSG_TYPE_UART_LINE_STATUS;
            uart_msg.subtype = 0;
            uart_msg.u.param =  0;
            app_io_msg_send(&uart_msg);
        }
    }
#endif
}

/**
 * @brief  GDMA0 channel 2 used for receive UART peripheral data.
 *         First clear rx GDMA interrupt mask and disable rx GDMA channel interrupt.
 *         Save data to UART rx buffer and set GDMA destination address.
 *         Then enable rx GDMA channel interrupt.
 *         Send IO_MSG_UART_RX event to app task.
 * @param  void
 * @return void
 */
ISR_TEXT_SECTION void console_rx_dma_handler(void)
{
    uint16_t rx_count;

    GDMA_INTConfig(UART_RX_DMA_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);
    GDMA_ClearINTPendingBit(UART_RX_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, DISABLE);

    rx_count = GDMA_GetTransferLen(UART_RX_DMA_CHANNEL);

    //APP_PRINT_ERROR1("console_rx_dma_handler: rx_count %d", rx_count);

    if (p_console_callback)
    {
        p_console_callback(CONSOLE_EVT_DATA_IND, uart_rx_buf_addr, rx_count);
    }

    GDMA_SetDestinationAddress(UART_RX_DMA_CHANNEL, (uint32_t)uart_rx_buf_addr);
    GDMA_INTConfig(UART_RX_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, ENABLE);
}

/**
 * @brief  GDMA0 channel 1 used for send UART peripheral data.
 *         When GDMA has been finished send all data, this interrupt will be cause.
 *         Clear tx GDMA channel all interrupt mask.
 *         Send IO_MSG_UART_TX event to app task, notify app to next tx action.
 * @param  void
 * @return void
 */
ISR_TEXT_SECTION void console_tx_dma_handler(void)
{
    GDMA_ClearAllTypeINT(UART_TX_DMA_CHANNEL_NUM);

    console_uart_handle_tx_done();
}

/**
 * @brief  UART tx wake up GPIO pinmux setting initial.
 *         Include APB peripheral clock config, UART tx GPIO parameter config.
 * @param  pin
 * @return bool
 */
bool console_uart_tx_wakeup_enable(uint8_t pin)
{
#if RX_WAKE_UP_SUPPORT
    /*gpio init*/
    hal_gpio_init_pin(pin, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_DOWN);
#endif
    return true;
}

/**
 * @brief  UART rx wake up GPIO pinmux setting initial.
 *         Include APB peripheral clock config, UART rx GPIO parameter config
 *         and UART rx GPIO interrupt mark config. Enable UART rx GPIO interrupt.
 *         When receive UART data, the system will be wake up from dlps mode.
 * @param  pin
 * @return bool
 */
bool console_uart_rx_wakeup_enable(uint8_t pin)
{
#if RX_WAKE_UP_SUPPORT
    /*gpio init*/
    hal_gpio_init_pin(pin, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_set_up_irq(pin, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);
    hal_gpio_irq_enable(pin);
#endif
    return true;
}

/**
 * @brief  UART rx wake up interrupt will be handle in this function.
 *         First read current UART rx GPIO pinmux input data bit and disable UART rx GPIO interrupt.
 *         Enable or disable app enter dlps mode according to UART rx pinmux status.
 *         Then enable UART rx GPIO interrupt.
 * @param  void
 * @return void
 */
bool console_uart_wakeup(void)
{
#if RX_WAKE_UP_SUPPORT
    uint8_t gpio_status;
    uint8_t pin_num = g_console_uart.rx_wake_up_pinmux;

    /*gpio init*/
    hal_gpio_init_pin(pin_num, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    gpio_status = hal_gpio_get_input_level(g_console_uart.rx_wake_up_pinmux);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(g_console_uart.rx_wake_up_pinmux);

    hal_gpio_set_up_irq(pin_num, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);


    /* Change GPIO Interrupt Polarity */
    if (gpio_status) //GPIO released
    {
        hal_gpio_irq_change_polarity(g_console_uart.rx_wake_up_pinmux, GPIO_IRQ_ACTIVE_LOW);//Polarity Low
        app_dlps_enable(APP_DLPS_ENTER_CHECK_UART_RX);
    }
    else //GPIO detected
    {
        hal_gpio_irq_change_polarity(g_console_uart.rx_wake_up_pinmux,
                                     GPIO_IRQ_ACTIVE_HIGH); //Polarity High
        app_dlps_disable(APP_DLPS_ENTER_CHECK_UART_RX);
    }

    /* Enable GPIO interrupt */

    hal_gpio_irq_enable(g_console_uart.rx_wake_up_pinmux);
    //hal_gpio_register_isr_callback(pin_num, NULL, 0);
#endif
    return true;
}

/**
    * @brief  UART tx channel start send data.
    *         Set GDMA source address and buffer size according to incoming parameters.
    *         Command GDMA tx channel to send data with os lock.
    * @param  void
    * @return void
    */
RAM_TEXT_SECTION bool console_uart_dma_write(uint8_t *buf, uint32_t len)
{
    uint32_t s;

    GDMA_SetSourceAddress(UART_TX_DMA_CHANNEL, (uint32_t)buf);
    GDMA_SetBufferSize(UART_TX_DMA_CHANNEL, len);
    s = os_lock();
    GDMA_Cmd(UART_TX_DMA_CHANNEL_NUM, ENABLE);
    os_unlock(s);

    return true;
}

/**
  * @brief  UART send data by cpu action.
  * @param  void
  * @return void
  */
RAM_TEXT_SECTION bool uart_send_by_cpu_action(uint8_t *buf, uint32_t len)
{
    uint32_t tx_len = len > UART_TX_FIFO_SIZE ? UART_TX_FIFO_SIZE : len;

    UART_SendData(UART0, buf, tx_len);

    uart_tx_curr_addr = buf + tx_len;
    uart_tx_len = len - tx_len;

    UART_INTConfig(UART0, UART_INT_FIFO_EMPTY, ENABLE);

    return true;
}


/**
  * @brief  UART send data by cpu action.
  * @param  void
  * @return void
  */
bool console_uart_write(uint8_t *buf, uint32_t len)
{
    //APP_PRINT_ERROR2("console_uart_write: %p, %d", buf, len);

    uart_tx_count = len;
    app_dlps_disable(APP_DLPS_ENTER_CHECK_UART_TX);

#if F_APP_ONE_WIRE_UART_SUPPORT
    // disable uart RX interrupter
    UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_IDLE | UART_INT_LINE_STS, DISABLE);
#endif

    if (g_console_uart.uart_tx_dma_enable)
    {
        return console_uart_dma_write(buf, len);
    }
    else
    {
        return uart_send_by_cpu_action(buf, len);
    }
}

/**
 * @brief  console uart handle tx data done, static function.
 * @param  void
 * @return void
 */
RAM_TEXT_SECTION void console_uart_handle_tx_done(void)
{
    uint16_t tx_count;

    /* GDMA_GetTransferLen(UART_TX_DMA_CHANNEL) is cleared when Channel disabled,
     * we just use the global variable to save it.
     * Let hardware to fix the DMA issue.
     */
    tx_count = uart_tx_count;
    app_dlps_enable(APP_DLPS_ENTER_CHECK_UART_TX);

#if F_APP_ONE_WIRE_UART_SUPPORT
    // clear Rx fifo
    UART_ClearRxFifo(UART0);
    // enable uart rx interrupter
    UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_IDLE | UART_INT_LINE_STS, ENABLE);
#endif

    if (p_console_callback)
    {
        p_console_callback(CONSOLE_EVT_DATA_XMIT, NULL, tx_count);
    }
}

/**
 * @brief  UART driver initial.
 *         Include APB peripheral clock config, UART parameter config
 * @param  void
 * @return void
 */
void console_uart_driver_init(void)
{
    UART_InitTypeDef uart_init;
    NVIC_InitTypeDef uart_nvic;

    /* Turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);
    RamVectorTableUpdate(UART0_VECTORn, Data_Uart_Handler);

    UART_StructInit(&uart_init);
    switch (g_console_uart.data_uart_baud_rate)
    {
    case BAUD_RATE_9600:
        uart_init.div = 271;
        uart_init.ovsr = 10;
        uart_init.ovsr_adj = 0x24A;
        break;

    case BAUD_RATE_19200:
        uart_init.div = 165;
        uart_init.ovsr = 7;
        uart_init.ovsr_adj = 0x5AD;
        break;

    case BAUD_RATE_115200:
        uart_init.div = 20;
        uart_init.ovsr = 12;
        uart_init.ovsr_adj = 0x252;
        break;

    case BAUD_RATE_230400:
        uart_init.div = 10;
        uart_init.ovsr = 12;
        uart_init.ovsr_adj = 0x252;
        break;

    case BAUD_RATE_460800:
        uart_init.div = 5;
        uart_init.ovsr = 12;
        uart_init.ovsr_adj = 0x252;
        break;

    case BAUD_RATE_921600:
        uart_init.div = 4;
        uart_init.ovsr = 5;
        uart_init.ovsr_adj = 0x3F7;
        break;

    case BAUD_RATE_2000000:
        uart_init.div = 2;
        uart_init.ovsr = 5;
        uart_init.ovsr_adj = 0;
        break;

    case BAUD_RATE_3000000:
        uart_init.div = 1;
        uart_init.ovsr = 8;
        uart_init.ovsr_adj = 0x492;
        break;

    case BAUD_RATE_4000000:
        uart_init.div = 1;
        uart_init.ovsr = 5;
        uart_init.ovsr_adj = 0;
        break;

    case BAUD_RATE_6000000:
        uart_init.div = 1;
        uart_init.ovsr = 1;
        uart_init.ovsr_adj = 0x36D;
        break;

    default:
        break;
    }

    uart_init.dmaEn = UART_DMA_ENABLE;
    uart_init.TxDmaEn = g_console_uart.uart_tx_dma_enable ? ENABLE : DISABLE;
    uart_init.TxWaterlevel = 15;
    uart_init.RxDmaEn = g_console_uart.uart_rx_dma_enable ? ENABLE : DISABLE;
    uart_init.RxWaterlevel = 1;

    //uart_init.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_8BYTE;
    UART_Init(UART0, &uart_init);
    if (!g_console_uart.uart_rx_dma_enable)
    {
        UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_IDLE, ENABLE);
    }
    else
    {
        UART_INTConfig(UART0, UART_INT_IDLE, ENABLE);
    }

    /* Enable UART IRQ */
    uart_nvic.NVIC_IRQChannel = UART0_IRQn;
    uart_nvic.NVIC_IRQChannelPriority = 5;
    uart_nvic.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
    NVIC_Init(&uart_nvic);
}


/**
 * @brief  UART tx dma driver initial.
 *         Include APB peripheral clock config, GDMA parameter config
 * @param  void
 * @return void
 */
void console_uart_tx_dma_init(void)
{
    GDMA_InitTypeDef tx_gdma;
    NVIC_InitTypeDef tx_nvic;

    /*-------GDMA clock enable-------*/
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /*-------UART Tx GDMA configuration -------*/
    GDMA_StructInit(&tx_gdma);
    tx_gdma.GDMA_ChannelNum = UART_TX_DMA_CHANNEL_NUM;
    tx_gdma.GDMA_DIR = GDMA_DIR_MemoryToPeripheral;
    tx_gdma.GDMA_BufferSize = 0;
    tx_gdma.GDMA_SourceInc = DMA_SourceInc_Inc;
    tx_gdma.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
    tx_gdma.GDMA_DestinationAddr = (uint32_t)(&(UART0->RB_THR));
    tx_gdma.GDMA_DestHandshake   = GDMA_Handshake_UART0_TX;
    tx_gdma.GDMA_ChannelPriority = 5; //channel prority between 0 to 6
    GDMA_Init(UART_TX_DMA_CHANNEL, &tx_gdma);

    /*-------GDMA Tx IRQ init-------*/
    tx_nvic.NVIC_IRQChannel = UART_TX_DMA_IRQ;
    tx_nvic.NVIC_IRQChannelPriority = 5;
    tx_nvic.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
    NVIC_Init(&tx_nvic);
    GDMA_INTConfig(UART_TX_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}


/**
 * @brief  UART rx dma driver initial.
 *         Include APB peripheral clock config, GDMA parameter config
 * @param  void
 * @return void
 */
void console_uart_rx_dma_init(void)
{
    GDMA_InitTypeDef rx_gdma;
    NVIC_InitTypeDef rx_nvic;

    /*-------GDMA clock enable-------*/
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /*-------UART Rx GDMA configuration -------*/
    GDMA_StructInit(&rx_gdma);
    rx_gdma.GDMA_ChannelNum      = UART_RX_DMA_CHANNEL_NUM;
    rx_gdma.GDMA_DIR             = GDMA_DIR_PeripheralToMemory;
    rx_gdma.GDMA_BufferSize      = RX_GDMA_BUFFER_SIZE;
    rx_gdma.GDMA_SourceInc       = DMA_SourceInc_Fix;
    rx_gdma.GDMA_DestinationInc  = DMA_DestinationInc_Inc;
    rx_gdma.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
    rx_gdma.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    rx_gdma.GDMA_SourceMsize     = GDMA_Msize_1;
    rx_gdma.GDMA_DestinationMsize = GDMA_Msize_1;
    rx_gdma.GDMA_SourceAddr   = (uint32_t)(&(UART0->RB_THR));
    rx_gdma.GDMA_DestinationAddr = (uint32_t)uart_rx_buf_addr;
    rx_gdma.GDMA_SourceHandshake = GDMA_Handshake_UART0_RX;
    rx_gdma.GDMA_ChannelPriority = 5; //channel prority between 0 to 6
    GDMA_Init(UART_RX_DMA_CHANNEL, &rx_gdma);

    /*-------GDMA Rx IRQ init-------*/
    rx_nvic.NVIC_IRQChannel = UART_RX_DMA_IRQ;
    rx_nvic.NVIC_IRQChannelPriority = 5;
    rx_nvic.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
    NVIC_Init(&rx_nvic);
    GDMA_INTConfig(UART_RX_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    //GDMA_Cmd(UART_TX_DMA_CHANNEL_NUM, ENABLE);
    GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, ENABLE);
}

/**
 * @brief console uart exit dlps set
 *
 * @param None
 * @retval None
 */
void console_uart_enter_low_power(POWERMode mode)
{
    if (g_console_uart.enable_rx_wake_up)
    {
        System_WakeUpPinEnable(g_console_uart.rx_wake_up_pinmux, PAD_WAKEUP_POL_LOW);
        System_WakeUpInterruptEnable(g_console_uart.rx_wake_up_pinmux);
    }
    else
    {
        uint8_t uart_rx_pinmux = g_console_uart.uart_rx_pinmux;
        System_WakeUpPinEnable(uart_rx_pinmux, PAD_WAKEUP_POL_LOW);
        System_WakeUpInterruptEnable(uart_rx_pinmux);
    }
}

/**
 * @brief console uart enter dlps set
 *
 * @param mode current low power mode to enter
 *
 */
void console_uart_exit_low_power(POWERMode mode)
{
    if (!g_console_uart.enable_rx_wake_up)
    {
        uint8_t uart_rx_pinmux = g_console_uart.uart_rx_pinmux;

        if (System_WakeUpInterruptValue(uart_rx_pinmux) == SET)
        {
            if (g_console_uart.callback)
            {
                g_console_uart.callback(CONSOLE_UART_EVENT_WAKE_UP);
            }
        }
    }

    /*need init gdma angin for exit dlps can not restort gdma setting*/
    if (g_console_uart.uart_tx_dma_enable)
    {
        console_uart_tx_dma_init();
    }

    if (g_console_uart.uart_rx_dma_enable)
    {
        console_uart_rx_dma_init();
    }
}


/**
 * @brief  UART driver initial.
 *         Include APB peripheral clock config, UART GPIO parameter config and
 *         GDMA channel parameter config. Enable UART GPIO and GDMA channel interrupt.
 *         Create UART rx queue for communication between UART and app task.
 * @param  void
 * @return void
 */
bool console_uart_init(P_CONSOLE_CALLBACK p_callback)
{
    p_console_callback = p_callback;

    if ((g_console_uart.uart_tx_dma_enable) &&
        (console_tx_dma_ch_num == 0xa5) &&
        (!GDMA_channel_request(&console_tx_dma_ch_num, console_tx_dma_handler, true)))
    {
        g_console_uart.uart_tx_dma_enable = false;
        APP_PRINT_ERROR0("console uart tx dma no free channel");
    }

    if ((g_console_uart.uart_rx_dma_enable) &&
        (console_rx_dma_ch_num == 0xa5) &&
        (!GDMA_channel_request(&console_rx_dma_ch_num, console_rx_dma_handler, true)))
    {
        g_console_uart.uart_rx_dma_enable = false;
        APP_PRINT_ERROR0("console uart rx dma no free channel");
    }

    if (g_console_uart.one_wire_uart_support)
    {
        UART_OneWireConfig(UART0, true);
    }
    else
    {
        UART_OneWireConfig(UART0, false);
        console_uart_driver_init();
    }

    if (g_console_uart.uart_tx_dma_enable)
    {
        console_uart_tx_dma_init();
    }

    if (g_console_uart.uart_rx_dma_enable)
    {
        console_uart_rx_dma_init();
    }

    return p_console_callback(CONSOLE_EVT_OPENED, NULL, 0);
}

/**
 * @brief  APP console uart configure set.
 * @param  cfg the pointer for console uart parameter.
 * @return void
 */
void console_uart_config_init(T_CONSOLE_UART_CONFIG *cfg)
{
    memcpy(&g_console_uart, cfg, sizeof(T_CONSOLE_UART_CONFIG));
}
