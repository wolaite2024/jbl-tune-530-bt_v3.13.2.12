#if F_APP_HEARABLE_SUPPORT
#ifndef _APP_HEARABLE_H_
#define _APP_HEARABLE_H_

#include <stdbool.h>
#include "app_listening_mode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_HEARABLE param report
  * @brief App HEARABLE param report
  * @{
  */
void app_ha_listening_delay_start(void);
void app_ha_switch_hearable_prog(void);
void app_ha_adjust_volume_level(bool is_vol_up);
void app_ha_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt);
void app_ha_clear_param(void);
void app_ha_init(void);

/** End of APP_HEARABLE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HEARABLE_H_ */
#endif /*F_APP_HEARABLE_SUPPORT */
