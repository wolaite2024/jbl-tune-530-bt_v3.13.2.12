#ifndef _APP_ONE_WIRE_UART_H_
#define _APP_ONE_WIRE_UART_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_ONE_WIRE_UART App One-Wire UART
  * @brief Provides one-wire UART commutation interfaces.
  * @{
  */

#define ONE_WIRE_GPIO_5V_PULL               PAD_PULL_UP
#define ONE_WIRE_GPIO_COM_PULL              PAD_PULL_DOWN

/**
* Packet format of one wire uart
*   |index 0 |index 1 |index 2 |index 3  |index 4   |index 5    |index 6     |index 7 ~ N-1 |index N  |
*   |0xAA    |rx_seqn |len_low |len_high |device id |cmd_id_low |cmd_id_high |    payload   |check_sum|
*/

/** @brief  One-wire UART state */
typedef enum
{
    ONE_WIRE_STATE_STOPPED        = 0,
    ONE_WIRE_STATE_STOPPING       = 1,
    ONE_WIRE_STATE_IN_ONE_WIRE    = 2,
} T_ONE_WIRE_UART_STATE;

/** @brief  One-wire UART data pinmux mode */
typedef enum
{
    ONE_WIRE_UART_TX = 0,
    ONE_WIRE_UART_RX = 1,
} T_ONE_WIRE_UART_TYPE;

typedef enum
{
    ONE_WIRE_AGING_TEST_STATE_STOPED    = 0x00,
    ONE_WIRE_AGING_TEST_STATE_TESTING   = 0x01,
} T_ONE_WIRE_AGING_TEST_STATE;

typedef enum
{
    CMD_AGING_TEST_ACTION_CHECK_STATE   = 0x00,
    CMD_AGING_TEST_ACTION_START         = 0x01,
    CMD_AGING_TEST_ACTION_STOP          = 0x02,
    CMD_AGING_TEST_ACTION_DONE          = 0x03,
} T_CMD_AGING_TEST_ACTION;

extern T_ONE_WIRE_UART_STATE one_wire_state;

void app_one_wire_data_uart_handler(void);

/**
    * @brief  One-wire UART deinitialization.
    * @return void
    */
void app_one_wire_uart_deinit(void);

/**
    * @brief  One-wire UART initialization.
    * @return void
    */
void app_one_wire_uart_init(void);

/**
    * @brief  open One-wire UART.
    * @return void
    */
void app_one_wire_uart_open(void);

/**
    * @brief  close One-wire UART.
    * @return void
    */
void app_one_wire_uart_close(void);

void app_one_wire_start_force_engage(uint8_t *target_bud_addr);
void app_one_wire_reset_to_uninitial_state(void);
void app_one_wire_enter_shipping_mode(void);
void app_one_wire_start_aging_test(void);
void app_one_wire_stop_aging_test(bool aging_test_done);
T_ONE_WIRE_AGING_TEST_STATE app_one_wire_get_aging_test_state(void);

/**
    * @brief  App process one-wire uart command.
    * @param  cmd_ptr command pointer
    * @param  cmd_len command length
    * @param  cmd_path command path use for distinguish uart,or le,or spp channel.
    * @param  app_idx received rx command device index
    * @param  ack_pkt command ack
    * @return void
    */
void app_one_wire_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                             uint8_t *ack_pkt);

/** @} End of APP_ONE_WIRE_UART */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
