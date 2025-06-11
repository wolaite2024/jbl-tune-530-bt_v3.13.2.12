#include "board.h"
#include "trace.h"
#include "os_timer.h"
#include "os_mem.h"
#include "os_sync.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_if8080.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "hub_display.h"
#include "gui_lcd_callback.h"
#include "gui_core.h"
#include "app_msg.h"
#include "module_lcd_qspi.h"
#include "platform_utils.h"
#include "wristband_pic_res.h"
#include "clock_manager.h"
#include "lcd_nt35510_480_8080.h"
#include "app_io_resource_init.h"

#define LCD_DMA_CHANNEL_NUM              lcd_dma_ch_num
#define LCD_DMA_CHANNEL_INDEX            DMA_CH_BASE(lcd_dma_ch_num)
#define LCD_DMA_CHANNEL_IRQ              DMA_CH_IRQ(lcd_dma_ch_num)

#define LCD_8080_BL                      P9_4
#define LCD_8080_D0                      P2_6
#define LCD_8080_D1                      P2_7
#define LCD_8080_D2                      P4_0
#define LCD_8080_D3                      P4_1
#define LCD_8080_D4                      P4_2
#define LCD_8080_D5                      P4_3
#define LCD_8080_D6                      P4_4
#define LCD_8080_D7                      P4_5

#define LCD_8080_CS                      P2_3
#define LCD_8080_DCX                     P2_4

#if (IC_TYPE_BB2_CCUT_EWE == 1)
#define LCD_8080_RD                      P9_0
#else
#define LCD_8080_RD                      P2_1
#endif
#define LCD_8080_WR                      P2_5

#define LCD_RST                          P9_5


static GDMA_LLIDef GDMA_LLIStruct_LAST;

void lcd_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(LCD_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(LCD_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
}

/**
  * @brief  wristband driver init
  * @param  None
  * @retval None
  */
void lcd_driver_init(void)
{
    lcd_device_init();
    lcd_set_reset(false);
    platform_delay_ms(100);
    lcd_set_reset(true);
    platform_delay_ms(50);
    lcd_set_reset(false);
    platform_delay_ms(50);
    lcd_te_device_init();

    user_lcd_power_on_cb = lcd_nt35510_power_on;
    user_lcd_power_off_cb = lcd_nt35510_power_off;
    user_lcd_set_window_cb = lcd_nt35510_set_window;
    user_lcd_init_cb = lcd_nt35510_init;

    user_lcd_te_enable_cb = lcd_te_enable;
    user_lcd_te_disable_cb = lcd_te_disable;
    user_lcd_clear_cb = lcd_clear;
    user_dma_single_block_init_cb = lcd_dma_single_block_init;
    user_dma_single_block_start_cb = lcd_dma_single_block_start;
    user_wait_dma_transfer_cb = lcd_wait_dma_transfer;
    user_wait_lcd_transfer_cb = lcd_wait_lcd_control_transfer;
    user_dma_set_multi_block_for_x_cb = lcd_dma_set_multi_block_for_x;
//    user_dma_set_multi_block_for_y_cb = lcd_dma_set_multi_block_for_y;
    user_lcd_get_font_dot_cb = get_font_dot;
    user_show_char_cb = lcd_show_char;

}


void lcd_pad_init(void)
{
    Pad_Config(LCD_8080_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* RD */
//    Pad_Config(LCD_8080_RD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* 8080 interface: D0~D7 */
    Pinmux_Config(LCD_8080_D0, IDLE_MODE);
    Pinmux_Config(LCD_8080_D1, IDLE_MODE);
    Pinmux_Config(LCD_8080_D2, IDLE_MODE);
    Pinmux_Config(LCD_8080_D3, IDLE_MODE);
    Pinmux_Config(LCD_8080_D4, IDLE_MODE);
    Pinmux_Config(LCD_8080_D5, IDLE_MODE);
    Pinmux_Config(LCD_8080_D6, IDLE_MODE);
    Pinmux_Config(LCD_8080_D7, IDLE_MODE);

    /* CS */
    Pinmux_Config(LCD_8080_CS, IDLE_MODE);
    /* DCX */
    Pinmux_Config(LCD_8080_DCX, IDLE_MODE);
    /* RD */
//    Pinmux_Config(LCD_8080_RD, IDLE_MODE);
    /* WR */
    Pinmux_Config(LCD_8080_WR, IDLE_MODE);

    /*BL AND RESET ARE NOT FIX*/
    Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);

}
void lcd_enter_dlps(void)
{
    Pad_Config(LCD_8080_D0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* RD */
//    Pad_Config(LCD_8080_RD, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(LCD_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
//    Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
}
void lcd_exit_dlps(void)
{
    Pad_Config(LCD_8080_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* RD */
//    Pad_Config(LCD_8080_RD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

//    Pad_Config(LCD_8080_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}


/**
  * @brief  writband lcd device init set IO config here
  * @param  None
  * @retval None
  */
void lcd_device_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, ENABLE);

    IF8080_PinGroupConfig();
    IF8080_InitTypeDef IF8080_InitStruct;
    IF8080_StructInit(&IF8080_InitStruct);
    extern uint32_t SystemCpuClock;
    if (SystemCpuClock == 100000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_5;
    }
    else if (SystemCpuClock == 90000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_5;
    }
    else if (SystemCpuClock == 80000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_4;
    }
    else if (SystemCpuClock == 40000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_2;
    }
    IF8080_InitStruct.IF8080_Mode              = IF8080_MODE_MANUAL;
    IF8080_InitStruct.IF8080_AutoModeDirection = IF8080_Auto_Mode_Direction_WRITE;
    IF8080_InitStruct.IF8080_GuardTimeCmd      = IF8080_GUARD_TIME_DISABLE;
    IF8080_InitStruct.IF8080_GuardTime         = IF8080_GUARD_TIME_2T;
    IF8080_InitStruct.IF8080_8BitSwap          = IF8080_8BitSwap_DISABLE;
    IF8080_InitStruct.IF8080_16BitSwap         = IF8080_16BitSwap_DISABLE;
    IF8080_InitStruct.IF8080_TxThr             = 10;
    IF8080_InitStruct.IF8080_TxDMACmd          = IF8080_TX_DMA_DISABLE;
    IF8080_InitStruct.IF8080_VsyncCmd          = IF8080_VSYNC_DISABLE;
    IF8080_InitStruct.IF8080_VsyncPolarity     = IF8080_VSYNC_POLARITY_FALLING;
    IF8080_Init(&IF8080_InitStruct);
    lcd_pad_init();
}

void lcd_set_backlight(uint32_t percent)
{
    if (percent)
    {
        Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
    }
    else
    {
        Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    return;
}



static void lcd_dma_clear(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,
                          uint16_t color)
{
    static uint32_t color_buf = 0;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_I8080;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    rtl_gui_lcd_sectionconfig(xStart, yStart, xEnd, yEnd);

    uint32_t old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    for (uint8_t i = 0; i < rtk_gui_config.total_section_count - 1; i++)
    {
        lcd_dma_single_block_start(NULL, (uint32_t)&color_buf,
                                   rtk_gui_config.lcd_width * rtk_gui_config.lcd_section_height * rtk_gui_config.pixel_bytes);
        lcd_wait_dma_transfer();
    }
    uint32_t last_len = 0;
    if (rtk_gui_config.lcd_hight % rtk_gui_config.lcd_section_height == 0)
    {
        last_len = rtk_gui_config.lcd_section_height * rtk_gui_config.lcd_width *
                   rtk_gui_config.pixel_bytes;
    }
    else
    {
        last_len = (rtk_gui_config.lcd_hight % rtk_gui_config.lcd_section_height) * rtk_gui_config.lcd_width
                   * rtk_gui_config.pixel_bytes;
    }
    lcd_dma_single_block_start(NULL, (uint32_t)&color_buf, last_len);
    lcd_wait_lcd_control_transfer(rtk_gui_config.lcd_width * rtk_gui_config.lcd_hight *
                                  rtk_gui_config.pixel_bytes);

    uint32_t new_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    extern uint32_t SystemCpuClock;
    uint32_t time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    uint32_t time = time_per_count * (new_stamp - old_stamp);
    uint32_t time_ms = time / 1000000;
    uint32_t time_us = time / 1000 - time_ms * 1000;

    DBG_DIRECT("[lcd clear test SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    IF8080_SwitchMode(IF8080_MODE_MANUAL);
}


void lcd_clear(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    lcd_dma_clear(xStart, yStart, xEnd, yEnd, color);
    IF8080_SwitchMode(IF8080_MODE_MANUAL);
}

void lcd_dma_single_block_init(uint32_t dir_type)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;


    if (dir_type == GDMA_DIR_MemoryToMemory)
    {
        GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
        GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
        GDMA_InitStruct.GDMA_DIR                 = dir_type;
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    }
    else if (dir_type == GDMA_DIR_MemoryToPeripheral)
    {
        GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
        GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
        GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_I8080;
        GDMA_InitStruct.GDMA_DIR                 = dir_type;
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    }

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}


void lcd_dma_single_block_start(uint32_t destination_addr, uint32_t source_addr, uint32_t len)
{
    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);
    if (destination_addr != 0)
    {
        GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, destination_addr);
    }
    else
    {
        GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(IF8080->FIFO)));
    }
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)source_addr);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}

void lcd_wait_dma_transfer(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

void lcd_wait_lcd_control_transfer(uint32_t count)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    uint32_t counter = 0;
    while (counter != count) //LCD_SECTION_HEIGHT
    {
        counter = IF8080_GetTxCounter();
    }
    IF8080_SwitchMode(IF8080_MODE_MANUAL);
}

void GDMA_Config_LLIStruct(uint32_t g1_addr, uint32_t g2_addr, uint32_t offset, \
                           GDMA_InitTypeDef *GDMA_InitStruct)//offset mean up menu not display in current menu
{
    GDMA_LLIDef GDMA_LLIStruct_G1;
    GDMA_LLIDef GDMA_LLIStruct_G2;

    IF8080_GDMALLIOFTTypeDef GDMA_LLIStruct_G1_oft;
    IF8080_GDMALLIOFTTypeDef GDMA_LLIStruct_G2_oft;

    GDMA_LLIStruct_G1_oft.SAR_OFT = rtk_gui_config.lcd_width * rtk_gui_config.pixel_bytes;
    GDMA_LLIStruct_G1_oft.DAR_OFT = 0;
    GDMA_LLIStruct_G2_oft.SAR_OFT = rtk_gui_config.lcd_width * rtk_gui_config.pixel_bytes;
    GDMA_LLIStruct_G2_oft.DAR_OFT = 0;

    GDMA_LLIStruct_G1.SAR = (uint32_t)(g1_addr + offset * rtk_gui_config.pixel_bytes);
    GDMA_LLIStruct_G1.DAR = (uint32_t)(&(IF8080->FIFO));
    GDMA_LLIStruct_G1.LLP = 0;
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_G1.CTL_LOW = BIT(0)
                                | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                | (GDMA_InitStruct->GDMA_DIR << 20)
                                | (GDMA_InitStruct->GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
    /* configure high 32 bit of CTL register */ /* this means buffer size */
    GDMA_LLIStruct_G1.CTL_HIGH = rtk_gui_config.lcd_width * rtk_gui_config.pixel_bytes / 4 - offset *
                                 rtk_gui_config.pixel_bytes / 4;

    GDMA_LLIStruct_G2.SAR = (uint32_t)(g2_addr);
    GDMA_LLIStruct_G2.DAR = (uint32_t)(&(IF8080->FIFO));
    GDMA_LLIStruct_G2.LLP = 0;
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_G2.CTL_LOW = BIT(0)
                                | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                | (GDMA_InitStruct->GDMA_DIR << 20)
                                | (GDMA_InitStruct->GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
    /* configure high 32 bit of CTL register */
    GDMA_LLIStruct_G2.CTL_HIGH = offset * rtk_gui_config.pixel_bytes / 4;

    GDMA_LLIStruct_LAST.LLP = 0;
    GDMA_LLIStruct_LAST.SAR = (uint32_t)(g2_addr + rtk_gui_config.lcd_width *
                                         (rtk_gui_config.lcd_hight - 1) * rtk_gui_config.pixel_bytes);
    GDMA_LLIStruct_LAST.DAR = (uint32_t)(&(IF8080->FIFO));
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_LAST.CTL_LOW = BIT(0)
                                  | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                  | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                  | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                  | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                  | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                  | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                  | (GDMA_InitStruct->GDMA_DIR << 20);
    /* configure high 32 bit of CTL register */
    GDMA_LLIStruct_LAST.CTL_HIGH = offset * rtk_gui_config.pixel_bytes / 4;

    IF8080_GDMALLIConfig((IF8080_GDMALLITypeDef *)(&GDMA_LLIStruct_G1),
                         (IF8080_GDMALLITypeDef *)(&GDMA_LLIStruct_G2),
                         (IF8080_GDMALLIOFTTypeDef *)(&GDMA_LLIStruct_G1_oft),
                         (IF8080_GDMALLIOFTTypeDef *)(&GDMA_LLIStruct_G2_oft),
                         rtk_gui_config.lcd_hight * 2 - 1,
                         (uint32_t)(&GDMA_LLIStruct_LAST));
}

void lcd_dma_set_multi_block_for_x(uint32_t picture0_addr, uint32_t picture1_addr,
                                   uint32_t offset,
                                   uint32_t yStart, uint32_t yEnd)//offset mean up menu not display in current menu
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = rtk_gui_config.lcd_width * rtk_gui_config.pixel_bytes /
                                               4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr          = 0;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(IF8080->FIFO));
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_I8080;

    GDMA_InitStruct.GDMA_Multi_Block_Mode   = LLI_TRANSFER;//LLI_TRANSFER;
    GDMA_InitStruct.GDMA_Multi_Block_En     = 1;

    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)IF8080_LLI_REG1_GDMA_BASE;


    offset = (offset >> 1) << 1;


    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);

    GDMA_Config_LLIStruct(picture0_addr, picture1_addr, offset, &GDMA_InitStruct);

    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}


bool get_font_dot(uint16_t unicode, uint32_t *addr, uint8_t *dot_width, uint8_t *font_width)
{
    uint16_t offset = *(uint16_t *)(unicode * 2 + FONT_UNICODE_TABLE_ADDR);

    *addr = offset * 128 + FONT_DATA_ADDR;

    if (*addr == 0xFFFFFFFF)
    {
        return false;
    }

    if ((unicode >= 0x20) && (unicode <= 0x80))
    {
        *font_width = 16;
    }
    else
    {
        *font_width = 32;
    }
    *dot_width = 32;
    return true;
}

void lcd_show_char(uint16_t font_width, uint16_t font_height,
                   const unsigned char BMP[], int16_t xs,
                   int16_t ys, \
                   uint32_t colour, int16_t Zs, int16_t Ze, uint8_t *buf)
{
    if ((ys >= Ze) || ((ys + font_height) <= Zs) || (xs >= LCD_WIDTH) || (xs + font_width <= 0))
    {
        return;
    }
    uint8_t *pBMP;
    uint8_t row_bytes = (font_width + 7) >> 3;
    pBMP = (uint8_t *)BMP;
    uint8_t loopx;
    uint8_t loopy;
    uint8_t addr_hs;
    uint8_t addr_hh;
    uint8_t looph;
    uint16_t write_off;
#if PIXEL_FORMAT == RGB16BIT_565
    uint16_t *writebuf = (uint16_t *)buf;
#elif PIXEL_FORMAT == RGB24BIT_888
    uint8_t *writebuf = (uint8_t *)buf;
#endif
    if (ys > Zs)
    {
        addr_hs = 0;
        addr_hh = Ze - ys;
        addr_hh = (addr_hh > font_height) ? font_height : addr_hh;
        ys = ys - Zs;
    }
    else
    {
        if ((ys + font_height) < Ze)
        {
            addr_hs = Zs - ys;
            addr_hh = (ys + font_height) - Zs;
        }
        else
        {
            addr_hs = Zs - ys;
            addr_hh = Ze - Zs;
        }
        ys = 0;
    }
    looph = ys;
    pBMP = pBMP + row_bytes * addr_hs;

    if (xs < 0)
    {
        if (xs + font_width < 0)
        {
            return;
        }
        int width_cache;
        width_cache = font_width + xs;
        for (loopy = 0; loopy < addr_hh; looph ++, loopy ++)
        {
            write_off = looph * LCD_WIDTH;
            for (loopx = 0; loopx < width_cache; loopx ++)
            {
                if ((pBMP[loopy * row_bytes + (loopx - xs) / 8] >> (7 - (loopx - xs) % 8)) & 0x01)
                {
#if PIXEL_FORMAT == RGB16BIT_565
                    writebuf[write_off + loopx]  = colour;
#elif PIXEL_FORMAT == RGB24BIT_888
                    writebuf[(write_off + loopx) * rtk_gui_config.pixel_bytes] = colour;
                    writebuf[(write_off + loopx) * rtk_gui_config.pixel_bytes + 1] = colour >> 8;
                    writebuf[(write_off + loopx) * rtk_gui_config.pixel_bytes + 2] = colour >> 16;
#endif
                }
            }
        }
    }
    else if ((xs + font_width) > LCD_WIDTH)
    {
        int width_cache;
        width_cache = LCD_WIDTH - xs;
        for (loopy = 0; loopy < addr_hh; looph ++, loopy ++)
        {
            write_off = looph * LCD_WIDTH;
            for (loopx = 0; loopx < width_cache; loopx ++)
            {
                if ((pBMP[loopy * row_bytes + loopx / 8] >> (7 - loopx % 8)) & 0x01)
                {
#if PIXEL_FORMAT == RGB16BIT_565
                    writebuf[write_off + xs + loopx]  = colour;
#elif PIXEL_FORMAT == RGB24BIT_888
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes] = colour;
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes + 1] = colour >> 8;
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes + 2] = colour >> 16;
#endif
                }
            }
        }
    }
    else
    {
        for (loopy = 0; loopy < addr_hh; loopy ++, looph ++)
        {
            for (loopx = 0; loopx < font_width; loopx ++)
            {
                write_off = looph * LCD_WIDTH;
                if ((pBMP[loopy * row_bytes + loopx / 8] >> (7 - loopx % 8)) & 0x01)
                {
#if PIXEL_FORMAT == RGB16BIT_565
                    writebuf[write_off + xs + loopx]  = colour;
#elif PIXEL_FORMAT == RGB24BIT_888
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes] = colour;
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes + 1] = colour >> 8;
                    writebuf[(write_off + xs + loopx) * rtk_gui_config.pixel_bytes + 2] = colour >> 16;
#endif
                }
            }
        }
    }
}
