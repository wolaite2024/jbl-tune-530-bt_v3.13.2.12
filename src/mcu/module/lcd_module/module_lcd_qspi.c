#include "board.h"
#include "trace.h"
#include "string.h"
#include "os_sync.h"
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "rtl876x_if8080.h"
#include "section.h"
#include "module_lcd_qspi.h"
#include "platform_utils.h"
#include "clock_manager.h"
#include "fmc_api.h"
#include "fmc_api_ext.h"
#include "app_msg.h"
#include "app_io_resource_init.h"
#include "app_gui.h"
//#include "lcd_rm69330_454_qspi.h"
//#include "lcd_st7789_240_spi.h"
#include "lcd_sh8601z_454_qspi.h"
#include "rtk_hal_lcd.h"

static uint8_t lcd_qspi_dma_ch_num = 0xa5;

#define LCD_DMA_CHANNEL_NUM              lcd_qspi_dma_ch_num
#define LCD_DMA_CHANNEL_INDEX            DMA_CH_BASE(lcd_qspi_dma_ch_num)
#define LCD_DMA_CHANNEL_IRQ              DMA_CH_IRQ(lcd_qspi_dma_ch_num)

static GDMA_LLIDef GDMA_LLIStruct_LAST;

void lcd_set_dma_ch_num(uint8_t num)
{
    lcd_qspi_dma_ch_num = num;
}

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
    lcd_set_reset(true);
    platform_delay_ms(120);
    lcd_set_reset(false);
    platform_delay_ms(50);
}

void lcd_pad_init(void)
{
    Pinmux_Config(P9_0, IDLE_MODE); //SIO2
    Pinmux_Config(P9_1, IDLE_MODE); //SIO1
    Pinmux_Config(P9_2, IDLE_MODE); //CS
    Pinmux_Config(P9_3, IDLE_MODE); //SIO0
    Pinmux_Config(P9_4, IDLE_MODE); //CLK
    Pinmux_Config(P9_5, IDLE_MODE); //SIO3

    Pad_Config(P9_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P9_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P9_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P9_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P9_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P9_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

}

/**
  * @brief  writband lcd device init set IO config here
  * @param  None
  * @retval None
  */
void lcd_device_init(void)
{
    lcd_pad_init();
    fmc_spic2_init();
    set_clock_gen_rate(CLK_SPIC2, CLK_80MHZ, CLK_80MHZ);
    fmc_spic2_set_baud(1);
}

void rtk_lcd_hal_init(void)
{
    lcd_driver_init();
    sh8601a_init();
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    //APP_PRINT_TRACE2("rtk_lcd_hal_set_window w:%d, h:%d", w, h);
    lcd_SH8601A_qspi_454_set_window(xStart, yStart, xStart + w - 1, yStart + h - 1);
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
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

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(SPIC2->DR[0].WORD));
    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);

    len = len >> 1;
    uint32_t dma_max_block_size = 65535;
    uint32_t left_count = len % dma_max_block_size;
    uint32_t count = len / dma_max_block_size;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(buf + offset));
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, dma_max_block_size);
        offset += (dma_max_block_size << 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);

        rtk_lcd_hal_transfer_done();
    }
    if (left_count != 0)
    {
        GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(buf + offset));
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, left_count);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
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
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    lcd_SH8601A_qspi_454_set_window(xStart, yStart, xStart + w - 1, yStart + h - 1);

    static uint32_t color_buf = 0;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;

    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)&color_buf);
    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->DR[0].WORD)));

    uint32_t section_hight = 10;
    uint32_t left_line = h % section_hight;

    for (uint32_t i = 0; i < h / section_hight; i++)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * section_hight * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);

        rtk_lcd_hal_transfer_done();
    }
    if (left_line != 0)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * left_line * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    //APP_PRINT_TRACE2("buf 0x%p, len %d", buf, len);
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

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 1);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->DR[0].WORD)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}

void rtk_lcd_hal_transfer_done(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return LCD_WIDTH;
}

uint32_t rtk_lcd_hal_get_height(void)
{
    return LCD_HIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return RGB16BIT_565;
}


void rtk_hal_lcd_fill(uint8_t *addr)
{
    uint32_t section_hight = 50;
    uint32_t left_line = (LCD_HIGHT - 1) % section_hight;
    uint32_t offset = 0;
    uint32_t secton_count = 0;
    uint32_t transfer_len_16 = 0;

    rtk_lcd_hal_set_window(0, 0,  LCD_WIDTH, section_hight);
    transfer_len_16 = LCD_WIDTH * section_hight;
    rtk_lcd_hal_start_transfer((uint8_t *)addr, transfer_len_16);
    offset += transfer_len_16 * 2;

    for (secton_count = 1; secton_count < (LCD_HIGHT - 1) / section_hight; secton_count++)
    {
        rtk_lcd_hal_transfer_done();
        rtk_lcd_hal_set_window(0, secton_count * section_hight, LCD_WIDTH, section_hight);
        rtk_lcd_hal_start_transfer((uint8_t *)(addr + offset), transfer_len_16);
        offset += (transfer_len_16 * 2);
    }
    if (left_line != 0)
    {
        rtk_lcd_hal_transfer_done();
        rtk_lcd_hal_set_window(0, secton_count * section_hight, LCD_WIDTH, left_line);
        rtk_lcd_hal_start_transfer((uint8_t *)(addr + offset), left_line * LCD_WIDTH);
        rtk_lcd_hal_transfer_done();
    }
}

void rtk_lcd_hal_start_transfer_test(uint8_t *buf, uint16_t xStart, uint16_t yStart, uint16_t w,
                                     uint16_t h)
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
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->DR[0].WORD)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);

    rtk_lcd_hal_set_window(0, 0, w, h);

    uint32_t section_hight = 10;
    uint32_t left_line = h % section_hight;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < h / section_hight; i++)
    {
        GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(buf + offset));
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * section_hight * 2) >> 2);
        offset += (w * section_hight * 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);

        rtk_lcd_hal_transfer_done();
    }
    if (left_line != 0)
    {
        GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(buf + offset));
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * left_line * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }
}

