/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_RFC_H_
#define _APP_GFPS_RFC_H_

#include "bt_rfc.h"
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */

void app_gfps_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf);
/** End of APP_GFPS_RFC
* @}
*/

/** End of APP_RWS_GFPS
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_GFPS_RFC_H_ */


