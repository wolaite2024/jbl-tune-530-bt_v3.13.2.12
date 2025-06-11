#ifndef _APP_PROFILE_INIT_H_
#define _APP_PROFILE_INIT_H_

#if F_APP_TMAS_SUPPORT
#include "tmas_mgr.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define APP_MAX_PA_ADV_SET_NUM 4
#define APP_MAX_SYNC_HANDLE_NUM 4

#define APP_ISOC_CIG_MAX_NUM                      2
#define APP_ISOC_CIS_MAX_NUM                      4

#define APP_ISOC_BROADCASTER_MAX_BIG_HANDLE_NUM  2
#define APP_ISOC_BROADCASTER_MAX_BIS_NUM  4

#define APP_SYNC_RECEIVER_MAX_BIG_HANDLE_NUM  1
#define APP_SYNC_RECEIVER_MAX_BIS_NUM  4

/* cccd info */
#define APP_LEA_PACS_CCCD_ENABLED   0x01
#define APP_LEA_VCS_CCCD_ENABLED    0x02
#define APP_LEA_BASS_CCCD_ENABLED   0x04
/**
 * @brief  Initialize parameter of bap include ROLE,
*          cig/big max number,...etc.
 * @param  void
 * @return void
 */
void app_lea_profile_init(void);

/**
 * @brief  Get TMAP Role.
 * @param  void
 * @return TMAP role.
 */
uint16_t app_lea_profile_get_tmas_role(void);
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
