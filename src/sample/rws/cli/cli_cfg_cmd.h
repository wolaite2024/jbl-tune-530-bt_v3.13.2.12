/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _CLI_CFG_CMD_H_
#define _CLI_CFG_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_CLI App Cli
  * @brief App Cli
  * @{
  */

/** @defgroup APP_RWS_CLI_CFG App Cli Cfg
  * @brief App Cli Cfg
  * @{
  */

/**
  * @brief The callback function that is executed when "cfg" is entered.
  * This is a cfg subcommand that should be registered. It is used to dump
  * some app config fields
  *
  * @param cmd_str command string from console input
  * @param buf     buffer used to output
  * @param buf_len buffer length
  * @retval true   Success
  * @retval false  Failure
  */
bool cfg_dump(const char *cmd_str, char *buf, size_t buf_len);

/** End of APP_RWS_CLI_CFG
* @}
*/

/** End of APP_RWS_CLI
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLI_BUD_CMD_H_ */
