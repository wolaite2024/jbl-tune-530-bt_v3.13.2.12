#ifndef _APP_HARMAN_REPORT_H_
#define _APP_HARMAN_REPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_link_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void app_harman_report_le_event(T_APP_LE_LINK *p_link, uint16_t event_id, uint8_t *data,
                                uint16_t len);
void app_harman_wait_ota_report(uint8_t conn_id);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
