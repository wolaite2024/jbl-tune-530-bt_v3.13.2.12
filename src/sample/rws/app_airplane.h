#ifndef _APP_AIRPLANE_H_
#define _APP_AIRPLANE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup
  * @brief
  * @{
  */

bool app_airplane_all_link_idle(void);
bool app_airplane_mode_mmi_support(uint8_t action);
void app_airplane_power_on_handle(void);
void app_airplane_power_off_handle(void);
void app_airplane_mmi_handle(void);
void app_airplane_bt_shutdown_end_check(void);
uint8_t app_airplane_mode_get(void);
uint8_t app_airplane_combine_key_power_on_get(void);
void app_airplane_combine_key_power_on_set(uint8_t enable);
void app_airplane_in_box_handle(void);
void app_airplane_enter_airplane_mode_handle(void);
void app_airplane_init(void);

/** End of APP_AIRPLANE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AIRPLANE_H_ */
