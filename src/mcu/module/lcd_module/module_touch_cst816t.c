#include <stddef.h>
#include "module_touch_cst816t.h"
#include "os_timer.h"
#include "platform_utils.h"
#include "vector_table.h"
#include "rtl876x_i2c.h"
#include "trace.h"
#include "app_gui.h"


#if (IC_TYPE_BB2_CCUT_EWE == 1)
#define TOUCH_I2C_SCL                             P0_0
#define TOUCH_I2C_SDA                             P0_1
#else
#define TOUCH_I2C_SCL                             P4_7
#define TOUCH_I2C_SDA                             P4_6
#endif
#define TOUCH_I2C_BUS                             I2C1
#define TOUCH_I2C_FUNC_SCL                        I2C1_CLK
#define TOUCH_I2C_FUNC_SDA                        I2C1_DAT
#define TOUCH_I2C_APBPeriph                       APBPeriph_I2C1
#define TOUCH_I2C_APBClock                        APBPeriph_I2C1_CLOCK
#define TOUCH_SLAVE_ADDRESS                       0x15

#define TOUCH_INT_APBPeriph                       APBPeriph_GPIOA
#define TOUCH_INT_APBPeriph_CLK                   APBPeriph_GPIOA_CLOCK
#define TOUCH_INT_GROUP                           GPIOA
#define TOUCH_INT                                 P0_2
#define TOUCH_INT_HANDLER                         GPIOA2_Handler
#define TOUCH_INT_IRQ                             GPIO2_IRQn
#define TOUCH_INT_VECTORn                         GPIOA2_VECTORn

#define TOUCH_RST                                 P0_3


static void touch_gesture_release_timeout(void *pxTimer);
void touch_get_chip_id(uint8_t *p_chip_id);

static void *touch_gesture_release_timer = NULL;
static TOUCH_DATA cur_point;
static void (*touch_indicate)(void *);
static void *touch_context;

/**
  * @brief  Initialize touch device
  * @param  None
  *
  * @retval None
  */
static void touch_device_init(void)
{
    const uint8_t pull_strength = 1;
    Pad_PullConfigValue(TOUCH_I2C_SCL, pull_strength);
    Pad_PullConfigValue(TOUCH_I2C_SDA, pull_strength);

    Pinmux_Config(TOUCH_I2C_SCL, TOUCH_I2C_FUNC_SCL);
    Pinmux_Config(TOUCH_I2C_SDA, TOUCH_I2C_FUNC_SDA);
    Pinmux_Config(TOUCH_INT, DWGPIO);

    Pad_Config(TOUCH_I2C_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_I2C_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* Enable GPIO and hardware timer's clock */
    RCC_PeriphClockCmd(TOUCH_INT_APBPeriph,  TOUCH_INT_APBPeriph_CLK,  ENABLE);

    RamVectorTableUpdate(TOUCH_INT_VECTORn, TOUCH_INT_HANDLER);
    /* Initialize GPIO as interrupt mode */
    GPIO_InitTypeDef GPIO_Param;
    GPIO_StructInit(&GPIO_Param);
    GPIO_Param.GPIO_PinBit = GPIO_GetPin(TOUCH_INT);
    GPIO_Param.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Param.GPIO_ITCmd = ENABLE;
    GPIO_Param.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    GPIO_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW; //GPIO_INT_POLARITY_ACTIVE_HIGH;
    GPIOx_Init(TOUCH_INT_GROUP, &GPIO_Param);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TOUCH_INT_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RCC_PeriphClockCmd(TOUCH_I2C_APBPeriph, TOUCH_I2C_APBClock, DISABLE);
    RCC_PeriphClockCmd(TOUCH_I2C_APBPeriph, TOUCH_I2C_APBClock, ENABLE);
    I2C_InitTypeDef  I2C_InitStructure;
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_Clock = 40000000;
    I2C_InitStructure.I2C_ClockSpeed   = 400000;
    I2C_InitStructure.I2C_DeviveMode   = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode  = I2C_AddressMode_7BIT;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_Init(TOUCH_I2C_BUS, &I2C_InitStructure);
    I2C_Cmd(TOUCH_I2C_BUS, ENABLE);

    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), ENABLE);
    GPIOx_INTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), DISABLE);
//    System_WakeUpPinEnable(TOUCH_INT, PAD_WAKEUP_POL_HIGH);
}

void touch_gesture_exit_dlps(void)
{
//    touch_device_init();
    Pad_Config(TOUCH_I2C_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_I2C_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

void touch_gesture_enter_dlps(void)
{
//    Pad_Config(TOUCH_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_I2C_SCL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_I2C_SDA, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TOUCH_INT, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

void touch_driver_init(void)
{
    touch_device_init();
    Pad_Config(TOUCH_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    platform_delay_ms(10);
    Pad_Config(TOUCH_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    platform_delay_ms(120);

    if (touch_gesture_release_timer == NULL)
    {
        os_timer_create(&touch_gesture_release_timer, "touch gesture release timer", 1, 20, false,
                        touch_gesture_release_timeout);
    }

    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), ENABLE);
    GPIOx_INTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), ENABLE);
    GPIOx_ClearINTPendingBit(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT));
    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), DISABLE);

    uint8_t chip_id[4];
    touch_get_chip_id(chip_id);
}

void touch_write(uint8_t reg, uint8_t data)
{
    uint8_t I2C_WriteBuf[2] = {reg, data};
    I2C_SetSlaveAddress(TOUCH_I2C_BUS, TOUCH_SLAVE_ADDRESS);
    I2C_Status res = I2C_MasterWrite(TOUCH_I2C_BUS, I2C_WriteBuf, 2);
    if (res != I2C_Success)
    {
        APP_PRINT_INFO1("ERROR! touch_write I2C_MasterWrite: %d", res);
    }
}

void touch_read(uint8_t reg, uint8_t *p_data, uint8_t len)
{
    I2C_SetSlaveAddress(TOUCH_I2C_BUS, TOUCH_SLAVE_ADDRESS);
    I2C_Status res = I2C_MasterWrite(TOUCH_I2C_BUS, &reg, 1);
    if (res != I2C_Success)
    {
        APP_PRINT_INFO1("ERROR! touch_read I2C_MasterWrite: %d", res);
    }
    platform_delay_us(1);
    res = I2C_MasterRead(TOUCH_I2C_BUS, p_data, len);
    if (res != I2C_Success)
    {
        APP_PRINT_INFO1("ERROR! touch_read I2C_MasterRead: %d", res);
    }
}

bool touch_read_key_value(TOUCH_DATA *p_touch_data)
{
    uint8_t point_num;
    touch_read(0x02, &point_num, 1);

    // Only support single point. Normally, point_num can only be 0 or 1.
    if (cur_point.get_point == 0)
    {
        cur_point.get_point = point_num ? 1 : 0;
    }

    if (point_num == 0)
    {
        return false;
    }

    uint8_t buf[4];
    touch_read(0x03, buf, 4);
    cur_point.is_press = true;
    cur_point.x = buf[1] | ((buf[0] & 0xf) << 8);
    cur_point.y = buf[3] | ((buf[2] & 0xf) << 8);
    if (cur_point.x > LCD_WIDTH || cur_point.y > LCD_HIGHT)
    {
        return false;
    }

    // Change origin of coordinate.
    cur_point.x = LCD_WIDTH - cur_point.x;
    cur_point.y = LCD_HIGHT - cur_point.y;
    APP_PRINT_INFO2("tp data x = %d, y = %d", cur_point.x, cur_point.y);

    p_touch_data->x =  cur_point.x;
    p_touch_data->y =  cur_point.y;
    p_touch_data->t = cur_point.t;
    p_touch_data->is_press = cur_point.is_press;

    return true;
}

void touch_get_chip_id(uint8_t *p_chip_id)
{
    touch_read(0xa7, p_chip_id, 4);
    APP_PRINT_INFO1("[cst0x6 get chip id] -- %b", TRACE_BINARY(4, p_chip_id));
}

void TOUCH_INT_HANDLER(void)
{
    /*  Mask GPIO interrupt */
    GPIOx_INTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), DISABLE);
    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), ENABLE);
    GPIOx_ClearINTPendingBit(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT));
    cur_point.t++;

    os_timer_restart(&touch_gesture_release_timer, 30);

    GPIOx_INTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), ENABLE);
    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), DISABLE);
}

void touch_int_config(bool is_enable)
{
    GPIOx_INTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), (FunctionalState)is_enable);
    GPIOx_MaskINTConfig(TOUCH_INT_GROUP, GPIO_GetPin(TOUCH_INT), (FunctionalState)!is_enable);
}

void touch_gesture_process_timeout(void)
{
//    APP_PRINT_INFO1("touch gesture release timeout t=%d", cur_point.t);
    cur_point.is_press = false;

    cur_point.t = 0;
    cur_point.x = 0;
    cur_point.y = 0;
    cur_point.get_point = 0;
}

static void touch_gesture_release_timeout(void *pxTimer)
{
    touch_gesture_process_timeout();
    if (touch_indicate)
    {
        touch_indicate(touch_context);
    }
}

void touch_register_irq_callback(void (*indicate)(void *), void *context)
{
    touch_indicate = indicate;
    touch_context = context;
}
