#ifndef _APP_LEA_VCS_H_
#define _APP_LEA_VCS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/**
 * @brief  Check volume controller exist or not
 * @param  No parameter.
 * @return The link of volume controller
 */
T_APP_LE_LINK *app_lea_vcs_find_vc_link(void);

/**
 * @brief  Initialize parameter of device topology using VCS
 *         and included instances of VOCS and AICS. Register
 *         ble audio callback to handle VCS message.
 * @param  No parameter.
 * @return void
 */
void app_lea_vcs_init(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
