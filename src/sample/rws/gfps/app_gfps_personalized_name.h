/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_PERSONALIZED_NAME_H_
#define _APP_GFPS_PERSONALIZED_NAME_H_

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "gfps.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */

typedef enum
{
    APP_GFPS_PERSONALIZED_NAME_SUCCESS,
    APP_GFPS_PERSONALIZED_NAME_POINTER_NULL,
    APP_GFPS_PERSONALIZED_NAME_LOAD_FAIL,
    APP_GFPS_PERSONALIZED_NAME_SAVE_FAIL,
    APP_GFPS_PERSONALIZED_NAME_INVALID,
} T_GFPS_PERSON_NAME_RST;

#define PERSONALIZED_NAME_FLASH_OFFSET            0xA8C   //personalized name store offset

void app_gfps_personalized_name_init(void);
T_GFPS_PERSON_NAME_RST app_gfps_personalized_name_store(uint8_t *stored_personalized_name,
                                                        uint8_t stored_personalized_name_len);
T_GFPS_PERSON_NAME_RST app_gfps_personalized_name_read(uint8_t *stored_personalized_name,
                                                       uint8_t *stored_personalized_name_len);
void app_gfps_personalized_name_send(uint8_t conn_id, uint8_t service_id);
void app_gfps_personalized_name_clear(void);
bool app_gfps_personalized_name_remote_sync(void);

/** End of APP_RWS_GFPS
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
