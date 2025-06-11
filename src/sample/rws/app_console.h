/*
 * Copyright (c) 2022, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CONSOLE_H_
#define _APP_CONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_CONSOLE_UART APP CONSOLE UART.
  * @brief app console uart event handle and implementation
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_CONSOLE_UART_Exported_Macros App Console UART Macros
    * @{
    */

/** End of APP_CONSOLE_UART_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_CONSOLE_UART_Exported_Types App Console UART Types
    * @{
    */

/**  @brief  App define global app data structure */

/** End of APP_CONSOLE_UART_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup APP_CONSOLE_UART_Exported_Variables App Console UART Variables
    * @{
    */


/** End of APP_CONSOLE_UART_Exported_Variables
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_CONSOLE_UART_Exported_Variables App Console UART Functions
    * @{
    */

void app_console_init(void);
void app_console_uart_driver_init(void);

/** End of APP_CONSOLE_UART_Exported_Variables
    * @}
    */

/** End of APP_CONSOLE_UART
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_CONSOLE_H_ */
