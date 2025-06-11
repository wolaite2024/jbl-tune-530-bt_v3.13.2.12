/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_HARMAN_USB_CONNECTOR_PROTECT_H_
#define _APP_HARMAN_USB_CONNECTOR_PROTECT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_HARMAN_USB_CONNECTOR_PROTECT_H
  * @brief
  * @{
  */
#define HARMAN_USB_CONNECTOR_HICCUP_PIN       P0_0
#define HARMAN_USB_CONNECTOR_PROTECT_PIN      P0_1
#define HARMAN_USB_CONNECTOR_MOSFET_PIN       P2_3

#define USB_CONNECTOR_NTC_CHECK_PERIOD        15

#define USB_CONNECTOR_HICCUP_VALUE            20 // unit: mv
#if HARMAN_PACE_SUPPORT
#define USB_CONNECTOR_HICCUP_CHECK_TIME       5000 // 5s
#else
#define USB_CONNECTOR_HICCUP_CHECK_TIME       1300 // 1.3s
#endif

typedef enum _t_app_harman_usb_connector_status_
{
    STATUS_NULL                           = 0x00,
    STATUS_DISABLE_CHARGER_CLOSE_MOSFET   = 0x01,
    STATUS_DISABLE_CHARGER_OPEN_MOSFET    = 0x02,
    STATUS_ENABLE_CHARGER_CLOSE_MOSFET    = 0x03,
    STATUS_ENABLE_CHARGER_OPEN_MOSFET     = 0x04,
} T_APP_HARMAN_USB_CONNECTOR_STATUS;

typedef enum _t_app_harman_usb_connector_protect_ntc_threshold_
{
    USB_PROTECT_NTC_THRESHOLD_HIGH            = 151,  // unit: mv
    USB_PROTECT_NTC_THRESHOLD_MID             = 206,  // unit: mv
    USB_PROTECT_NTC_THRESHOLD_LOW             = 409,  // unit: mv
} T_APP_HARMAN_USB_CONNECTOR_PROTECT_NTC_THRESHOLD;

void app_harman_usb_connector_protect_init(void);
void app_harman_usb_connector_adc_update(void);
bool app_harman_usb_connector_detect_is_normal(void);
void app_harman_usb_connector_adp_in_handle(void);
void app_harman_usb_connector_adp_out_handle(void);
void app_harman_usb_connector_protect_nv_clear(void);
bool app_harman_usb_hiccup_check_timer_started(void);
void app_harman_usb_adc_ntc_check_timer_stop(void);

/** End of APP_HARMAN_USB_CONNECTOR_PROTECT_H
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HARMAN_USB_CONNECTOR_PROTECT_H_ */


