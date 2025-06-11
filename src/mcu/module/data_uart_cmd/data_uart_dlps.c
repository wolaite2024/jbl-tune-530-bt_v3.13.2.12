#include <trace.h>
#include <board.h>
#include <rtl876x_pinmux.h>
#include "io_dlps.h"
#include "pm.h"
#if F_BT_DLPS_EN
bool can_enter_dlps = true;

void data_uart_dlps_exit_cb(void)
{
    Pad_ControlSelectValue(DATA_UART_TX_PIN, PAD_PINMUX_MODE);
    Pad_ControlSelectValue(DATA_UART_RX_PIN, PAD_PINMUX_MODE);
    if (System_WakeUpInterruptValue(DATA_UART_RX_PIN) == SET)
    {
        can_enter_dlps = false;
    }
}

void data_uart_dlps_enter_cb(void)
{
    Pad_ControlSelectValue(DATA_UART_TX_PIN, PAD_SW_MODE);
    Pad_ControlSelectValue(DATA_UART_RX_PIN, PAD_SW_MODE);
    System_WakeUpPinEnable(DATA_UART_RX_PIN, PAD_WAKEUP_POL_LOW);
    System_WakeUpInterruptEnable(DATA_UART_RX_PIN);

}

bool data_uart_dlps_check_cb(void)
{
    if (can_enter_dlps)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void System_Handler(void)
{
    Pad_ClearAllWakeupINT();
}

void data_uart_can_enter_dlps(bool enter)
{
    can_enter_dlps = enter;
}

void data_uart_dlps_init(void)
{
    io_dlps_register();
    if (power_check_cb_register(data_uart_dlps_check_cb) != 0)
    {
        APP_PRINT_ERROR0("data_uart_dlps_init: dlps_check_cb_reg register failed");
    }
    io_dlps_register_enter_cb(data_uart_dlps_enter_cb);
    io_dlps_register_exit_cb(data_uart_dlps_exit_cb);
}
#endif

