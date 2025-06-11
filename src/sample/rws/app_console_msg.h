/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CONSOLE_MSG_H_
#define _APP_CONSOLE_MSG_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_CONSOLE_MSG App Console Msg
  * @brief App Console Msg
  * @{
  */
/** @defgroup APP_CONSOLE_MSG_Exported_Functions App Console Msg Functions
    * @{
    */
/**
    * @brief  handle bqb/bud with case/uart message
    * @param  console_msg @ref T_IO_MSG
    * @return none
    */
void app_console_handle_msg(T_IO_MSG console_msg);

/** @} */ /* End of group APP_CONSOLE_MSG_Exported_Functions */
/** End of APP_CONSOLE_MSG
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_CONSOLE_MSG_H_ */
