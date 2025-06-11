/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _CLI_MP_H_
#define _CLI_MP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_MP App mp
  * @brief App mp
  * @{
  */

/**
 * @brief   Register mp string command
 * @retval  true   Success
 * @retval  false  Failed
 */
bool mp_cmd_str_register(void);


/** End of RWS_MP
* @}
*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLI_MP_H_ */
