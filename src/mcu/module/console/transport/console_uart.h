/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _CONSOLE_UART_H_
#define _CONSOLE_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "pm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_CONSOLE APP Console
  * @brief App Console
  * @{
  */
typedef enum
{
    CONSOLE_UART_EVENT_WAKE_UP,
} T_CONSOLE_UART_EVENT;


typedef void(*P_APP_CONSOLE_UART_CALLBACK)(T_CONSOLE_UART_EVENT);

typedef struct
{
    bool    uart_tx_dma_enable;
    bool    uart_rx_dma_enable;
    uint8_t one_wire_uart_support;
    uint8_t uart_tx_pinmux;
    uint8_t uart_rx_pinmux;
    uint8_t rx_wake_up_pinmux;
    uint8_t enable_rx_wake_up;
    uint8_t data_uart_baud_rate;
    P_APP_CONSOLE_UART_CALLBACK callback;
} T_CONSOLE_UART_CONFIG;

/**
 * @brief Enable console uart tx wakeup
 *
 * @param pin Pin used to wakeup
 * @retval true Success
 * @retval false Failure
 */
bool console_uart_tx_wakeup_enable(uint8_t pin);

/**
 * @brief Enable console uart rx wakeup
 *
 * @param pin Pin used to wakeup
 * @retval true
 * @retval false
 */
bool console_uart_rx_wakeup_enable(uint8_t pin);

/**
 * @brief Wakeup console uart
 *
 * @retval true Success
 * @retval false Failure
 */
bool console_uart_wakeup(void);

/**
 * @brief Tx data by console uart
 *
 * @param buf Pointer to the buffer to be tx
 * @param len length of the buffer
 * @retval true Write success
 * @retval false Write Failed
 */
bool console_uart_write(uint8_t *buf, uint32_t len);

/**
 * @brief console uart exit dlps set
 *
 * @param None
 * @retval None
 */
void console_uart_exit_low_power(POWERMode mode);

/**
 * @brief console uart enter dlps set
 *
 * @param mode current low power mode to enter
 *
 */
void console_uart_enter_low_power(POWERMode mode);

/**
 * @brief Init console uart with a callback
 *
 * @param p_callback Callback used in console uart module, used to notify register some events
 * @retval true
 * @retval false
 */
bool console_uart_init(P_CONSOLE_CALLBACK p_callback);

/**
 * @brief  APP console uart configure set.
 * @param  cfg the pointer for console uart parameter.
 * @return void
 */
void console_uart_config_init(T_CONSOLE_UART_CONFIG *cfg);

/**
 * @brief  APP console uart driver init.
 * @param  void.
 * @return void
 */
void console_uart_driver_init(void);

/** End of APP_RWS_CONSOLE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CONSOLE_UART_H_ */
