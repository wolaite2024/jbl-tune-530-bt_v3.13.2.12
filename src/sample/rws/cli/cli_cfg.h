/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _CLI_CFG_H_
#define _CLI_CFG_H_

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
 * @brief This api is used to register cfg command
 *
 * @retval true
 * @retval false
 */
bool cfg_cmd_register(void);


/** End of APP_RWS_CLI_CFG
* @}
*/

/** End of APP_RWS_CLI
* @}
*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLI_BUD_H_ */
