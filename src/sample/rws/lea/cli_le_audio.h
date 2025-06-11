/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _CLI_LE_AUDIO_H_
#define _CLI_LE_AUDIO_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef enum
{
    AUDIO_CMD_SET               = 0x1000,
    AUDIO_CMD_STARTESCAN        = 0x1001,
    AUDIO_CMD_STOPESCAN         = 0x1002,
    AUDIO_CMD_PASYNC            = 0x1003,
    AUDIO_CMD_BIGSYNC           = 0x1004,
    AUDIO_CMD_PATERMINATE       = 0x1005,
    AUDIO_CMD_BIGTERMINATE      = 0x1006,
    AUDIO_CMD_BISOTERMINATE     = 0x1007,
    AUDIO_CMD_BPATERMINATE      = 0x1008,
    AUDIO_CMD_BIGSPATH          = 0x1009,
    AUDIO_CMD_BIGRPATH          = 0x100A,
    AUDIO_CMD_A2DP_PAUSE        = 0x100B,
    AUDIO_CMD_CIS_ADV             = 0x100C,
} T_AUDIO_CMD;

bool le_audio_cmd_register(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLI_ISOC_H_ */
