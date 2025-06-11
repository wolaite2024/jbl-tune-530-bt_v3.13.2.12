#ifndef _APP_LEA_VOL_POLICY_H_
#define _APP_LEA_VOL_POLICY_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "app_link_util.h"

typedef enum
{
    LEA_SPK_UP,
    LEA_SPK_DOWN,
    LEA_SPK_MUTE,
    LEA_SPK_UNMUTE,
} T_LEA_VOL_CHG_TYPE;

typedef enum
{
    LEA_VOL_NONE,
    LEA_VOL_LEVEL_CHANGE,
    LEA_VOL_LEVEL_LIMIT,
    LEA_VOL_MUTE_CHANGE,
    LEA_VOL_MUTE_UNCHANGE,
} T_LEA_VOL_CHG_RESULT;

/**
 * @brief  transfer the vcs volume to volume gain
 * @param  No parameter.
 * @return The volume gain
 */
uint8_t app_lea_vol_gain_get(void);

/**
 * @brief  Update volume and set vcs paramaters if vcs support
 * @param  type  change type of volume
 * @return The result of update volume
 */
T_LEA_VOL_CHG_RESULT app_lea_vol_local_volume_change(T_LEA_VOL_CHG_TYPE type);

/**
 * @brief  Update the volume if streaming
 * @param  No parameter.
 * @return void
 */
void app_lea_vol_update_track_volume(void);

/**
 * @brief  Sync remote volume info
 * @param  No parameter.
 * @return void
 */
void app_lea_vol_sync_info(void);

/**
 * @brief  Init le audio volume policy
 * @param  No parameter.
 * @return void
 */
void app_lea_vol_init(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
