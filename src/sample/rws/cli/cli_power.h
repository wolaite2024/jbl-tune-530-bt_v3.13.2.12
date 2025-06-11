/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _CLI_POWER_H_
#define _CLI_POWER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_CLI App Cli
  * @brief App Cli
  * @{
  */

/** @defgroup APP_RWS_CLI_POWER App Cli Power
  * @brief App Cli Power
  * @{
  */

#define POWER_CMD                           0x01

#define POWER_ACTION_POWER_ON                 0x02
#define POWER_ACTION_POWER_OFF                0x03

/**
 * @brief This api is used to register power command
 *
 * @retval true  Command register successfully
 * @retval false Command register failed
 */
bool power_cmd_register(void);


/** End of APP_RWS_CLI_POWER
* @}
*/

/** End of APP_RWS_CLI
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLI_POWER_H_ */
