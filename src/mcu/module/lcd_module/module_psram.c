#include "board.h"
#include "trace.h"
#include "os_timer.h"
#include "os_mem.h"
#include "os_sync.h"
#include "module_psram.h"
#include "app_gui.h"
#include "rtl876x_aon_reg.h"
#include "fmc_api.h"
#include "clock_manager.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gdma.h"
#include "app_io_resource_init.h"

extern bool fmc_psram_clock_switch(CLK_FREQ_TYPE clk);
#if 0
#define LCD_DMA_CHANNEL_NUM              lcd_dma_ch_num
#define LCD_DMA_CHANNEL_INDEX            DMA_CH_BASE(lcd_dma_ch_num)
#define LCD_DMA_CHANNEL_IRQ              DMA_CH_IRQ(lcd_dma_ch_num)

static uint32_t color_data[64] = {0xf800f8, 0xf800f8, 0xf800f8, 0xf800f8, 0xf800f8, 0xf800f8, 0xf800f8, 0xf800f8};
static void dma_read_write_test(uint32_t psram_addr, uint32_t ram_buf_addr, bool is_read)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;

    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;

    if (is_read)
    {
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
        GDMA_InitStruct.GDMA_DestinationAddr     = ram_buf_addr;
        GDMA_InitStruct.GDMA_SourceAddr          = psram_addr;
    }
    else
    {
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
        GDMA_InitStruct.GDMA_DestinationAddr     = psram_addr;
        GDMA_InitStruct.GDMA_SourceAddr          = ram_buf_addr;
    }

    GDMA_InitStruct.GDMA_BufferSize          = 240 * 240 * 2 / 4;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    uint32_t old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);


    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);

    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);

    uint32_t new_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    extern uint32_t SystemCpuClock;
    uint32_t time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    uint32_t time = time_per_count * (new_stamp - old_stamp);
    uint32_t time_ms = time / 1000000;
    uint32_t time_us = time / 1000 - time_ms * 1000;

    DBG_DIRECT("[psram to lcd test SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    if (is_read)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            DBG_DIRECT("data[%d] = 0x%x", i, color_data[i]);
        }
    }
    else
    {
        for (uint32_t i = 0; i < 240 * 240 * 2 / 4; i++)
        {
            if (*(uint32_t volatile *)(0x4000000 + i * 4) != 0xf800f8)
            {
                DBG_DIRECT("0x%x = 0x%x", 0x4000000 + i * 4, *(uint32_t volatile *)(0x4000000 + i * 4));
            }
        }
    }
}

#endif


void app_apm_qspi_psram_init(void)
{
    if (fmc_psram_ap_memory_qpi_init())
    {
        DBG_DIRECT("APM QSPI psram init success!");
    }
    else
    {
        DBG_DIRECT("APM QSPI psram init fail!");
    }
    if (fmc_psram_clock_switch(CLK_80MHZ))
    {
        DBG_DIRECT("APM QSPI psram switch to 40MHz success!");
    }
    else
    {
        DBG_DIRECT("APM QSPI psram switch to 40MHz fail!");
    }

//    app_apm_psram_read_write_test();
}

void app_apm_opi_psram_init(void)
{
    if (fmc_psram_ap_memory_opi_init())
    {
        DBG_DIRECT("APM OPI psram init success!");
    }
    else
    {
        DBG_DIRECT("APM OPI psram init fail!");
    }
    if (fmc_psram_clock_switch(CLK_80MHZ))
    {
        DBG_DIRECT("APM OPI psram switch to 40MHz success!");
    }
    else
    {
        DBG_DIRECT("APM OPI psram switch to 40MHz fail!");
    }

//    app_apm_psram_read_write_test();
//    dma_read_write_test(IMAGE_TEST240_BIN, (uint32_t)color_data, true);
}

void app_apm_psram_read_write_test(void)
{
    uint32_t color_buf = 0;
    uint16_t color = RED;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;

    DBG_DIRECT("write color buf = 0x%x", color_buf);
    uint32_t old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    for (uint32_t i = 0; i < 240 * 240 * 2 / 4; i++)
    {
        *(uint32_t *)(0x4000000 + i * 4) = color_buf;
    }
    uint32_t new_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    extern uint32_t SystemCpuClock;
    uint32_t time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    uint32_t time = time_per_count * (new_stamp - old_stamp);
    uint32_t time_ms = time / 1000000;
    uint32_t time_us = time / 1000 - time_ms * 1000;
    DBG_DIRECT("[psram write buffer SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    for (uint32_t i = 0; i < 240 * 240 * 2 / 4; i++)
    {
        color_buf = *(uint32_t volatile *)(0x4000000 + i * 4);
//        DBG_DIRECT("0x%x = 0x%x", 0x4000000 + i * 4, *(uint32_t volatile *)(0x4000000 + i * 4));
    }

    old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    time = time_per_count * (old_stamp - new_stamp);
    time_ms = time / 1000000;
    time_us = time / 1000 - time_ms * 1000;
    DBG_DIRECT("read color buf = 0x%x", color_buf);
    DBG_DIRECT("[psram read buffer SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    for (uint32_t i = 0; i < 240 * 2 * 2 / 4; i++)
    {
        DBG_DIRECT("0x%x = 0x%x", 0x4000000 + i * 4, *(uint32_t volatile *)(0x4000000 + i * 4));
    }
}


void app_wb_opi_psram_init(void)
{
    if (fmc_psram_winbond_opi_init())
    {
        DBG_DIRECT("WB OPI psram init success!");
    }
    else
    {
        DBG_DIRECT("WB OPI psram init fail!");
    }

    if (fmc_psram_clock_switch(CLK_80MHZ))
    {
        DBG_DIRECT("WB OPI psram switch to 40MHz success!");
    }
    else
    {
        DBG_DIRECT("WB OPI psram switch to 40MHz fail!");
    }

//    app_wb_opi_psram_read_write_test();
    app_apm_psram_read_write_test();
//    dma_read_write_test(0x4000000, (uint32_t)color_data, true);
}

void app_wb_opi_psram_read_write_test(void)
{
    uint32_t color_buf = 0;
    uint16_t color = BLUE;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;

    uint32_t old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    for (uint32_t i = 0; i < LCD_WIDTH * LCD_HIGHT * PIXEL_BYTES / 4; i++)
    {
        *(uint32_t *)(0xA000000 + i * 4) = color_buf;
    }
    uint32_t new_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    extern uint32_t SystemCpuClock;
    uint32_t time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    uint32_t time = time_per_count * (new_stamp - old_stamp);
    uint32_t time_ms = time / 1000000;
    uint32_t time_us = time / 1000 - time_ms * 1000;
    DBG_DIRECT("[opi psram write buffer SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    for (uint32_t i = 0; i < LCD_WIDTH * LCD_HIGHT * PIXEL_BYTES / 4; i++)
    {
        color_buf = *(uint32_t volatile *)(0xA000000 + i * 4);
//        DBG_DIRECT("0x%x = 0x%x", 0x4000000 + i * 4, *(uint32_t volatile *)(0x4000000 + i * 4));
    }

    old_stamp = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
    time_per_count = 1000000000 / SystemCpuClock; //use float parameter will be better
    time = time_per_count * (old_stamp - new_stamp);
    time_ms = time / 1000000;
    time_us = time / 1000 - time_ms * 1000;
    DBG_DIRECT("color buf = 0x%x", color_buf);
    DBG_DIRECT("[opi psram read buffer SystemCpuClock = %dM; t=%d.%d ms]", \
               SystemCpuClock / 1000000, time_ms, time_us);

    for (uint32_t i = 0; i < LCD_WIDTH * 2 * PIXEL_BYTES / 4; i++)
    {
        DBG_DIRECT("0x%x = 0x%x", 0xA000000 + i * 4, *(uint32_t volatile *)(0xA000000 + i * 4));
    }
}
