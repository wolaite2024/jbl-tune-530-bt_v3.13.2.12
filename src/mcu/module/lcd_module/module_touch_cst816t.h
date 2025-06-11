#ifndef MODULE_TOUCH_CST816T_H
#define MODULE_TOUCH_CST816T_H

#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x.h"

typedef struct
{
    int16_t x;
    int16_t y;
    uint16_t t;
    bool is_press;
    uint8_t get_point;
} TOUCH_DATA;

void touch_driver_init(void);
void touch_gesture_enter_dlps(void);
bool touch_read_key_value(TOUCH_DATA *p_touch_data);
void touch_gesture_process_timeout(void);
void touch_int_config(bool is_enable);
void touch_register_irq_callback(void (*indicate)(void *), void *context);

#endif // MODULE_TOUCH_CST816T_H
