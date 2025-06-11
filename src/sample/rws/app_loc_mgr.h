/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BUD_LOC_MGR_H_
#define _APP_BUD_LOC_MGR_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BUD_LOCATION_IPC_TOPIC "BUD_LOC"

/** @brief Bud location */
typedef enum
{
    BUD_LOC_UNKNOWN     = 0x00,
    BUD_LOC_IN_CASE     = 0x01,
    BUD_LOC_IN_AIR      = 0x02,
    BUD_LOC_IN_EAR      = 0x03,
} T_BUD_LOCATION;

typedef enum
{
    EVENT_NON           = 0x00,
    EVENT_IN_CASE       = 0x01,
    EVENT_OUT_CASE      = 0x02,
    EVENT_IN_EAR        = 0x03,
    EVENT_OUT_EAR       = 0x04,
    EVENT_OPEN_CASE     = 0x05,
    EVENT_CLOSE_CASE    = 0x06,
    EVENT_RMEOTE_SYNCED = 0x07
} T_BUD_LOCATION_EVENT;

typedef enum
{
    CAUSE_ACTION_NON                = 0x00,
    CAUSE_ACTION_IO                 = 0x01,
    CAUSE_ACTION_ADP                = 0x02,
    CAUSE_ACTION_SENSOR_LD          = 0x03,
    CAUSE_ACTION_SENSOR_LD_IO       = 0x04,
    CAUSE_ACTION_PLAY_STATUS_CHANGE = 0x05,
} T_BUD_LOCATION_CAUSE_ACTION;

typedef struct
{
    uint8_t local_pre;
    uint8_t remote_pre;
    uint8_t local_cur;
    uint8_t remote_cur;
} T_BUD_LOCATIOIN_EVENT_MSG;

typedef enum
{
    BUD_MSG_LOC_EVT = 0,
    BUD_MSG_LOC = 1,

    BUD_MSG_MAX
} T_BUD_LOCATION_MSG;

void app_loc_mgr_state_machine(T_BUD_LOCATION_EVENT evt, bool from_remote,
                               T_BUD_LOCATION_CAUSE_ACTION cause_action);
void app_loc_mgr_init(void);
T_BUD_LOCATION app_loc_mgr_local_detect(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BUD_LOC_MGR_H_ */
