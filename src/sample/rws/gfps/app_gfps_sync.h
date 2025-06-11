/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_SYNC_H_
#define _APP_GFPS_SYNC_H_

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    APP_REMOTE_MSG_GFPS_ACCOUNT_KEY_ADD        = 0x00,
    APP_REMOTE_MSG_GFPS_ACCOUNT_KEY            = 0x01,
    APP_REMOTE_MSG_GFPS_PERSONALIZED_NAME      = 0x02,
    APP_REMOTE_MSG_GFPS_SUBPAIR_SEC_ADV_ENABLE = 0x03,
    APP_REMOTE_MSG_GFPS_ADDITIONAL_BOND_INFO   = 0x04,
    APP_REMOTE_MSG_GFPS_ADDITIONAL_IO_CAP      = 0x05,
    APP_REMOTE_MSG_GFPS_MAX_MSG_NUM,
} T_APP_GFPS_REMOTE_MSG;

typedef enum
{
    GFPS_ADDITIONAL_BOND_TYPE_LE = 0x00,
    GFPS_ADDITIONAL_BOND_TYPE_BR = 0x01,
} T_GFPS_ADDITIONAL_BOND_TYPE;

typedef struct
{
    uint8_t status_code;
    uint32_t bond_passkey;
    uint8_t  bond_bd_addr[6];

    //T_GFPS_ADDITIONAL_BOND_TYPE bond_type;
} T_GFPS_ADDITIONAL_BOND_DATA;

bool app_gfps_sync_relay_init(void);

/** End of APP_RWS_GFPS
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
