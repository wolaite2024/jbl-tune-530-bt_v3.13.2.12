/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _APP_GFPS_BATTERY_H_
#define _APP_GFPS_BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdbool.h"
#include "gfps.h"

/** @defgroup APP_RWS_GFPS App Gfps
  * @brief App Gfps
  * @{
  */

typedef enum
{
    GFPS_BATTERY_REPORT_STATE_OF_CHARGER,// = 0 left or right battery level change
    GFPS_BATTERY_REPORT_CHARGER_STATE,// = 1 left or right charger state change
    GFPS_BATTERY_REPORT_b2b_DISCONNECT,// = 2 b2b disconnect, remote 0x7F
    GFPS_BATTERY_REPORT_b2b_CONNECT_SUCCESS,//= 3 b2b connected
    GFPS_BATTERY_REPORT_ROLESWAP_SUCCESS,//= 4 roleswap success
    GFPS_BATTERY_REPORT_ENGAGE_SUCCESS,//= 5 engage success
    GFPS_BATTERY_REPORT_ENGAGE_FAIL,//= 6 set remote 0x7F
    GFPS_BATTERY_REPORT_BUD_IN_CASE,//= 7 left in case, right in case or both in case
    GFPS_BATTERY_REPORT_BUDS_OUT_CASE,//= 8 both out case,case battery level 0x7F
    GFPS_BATTERY_REPORT_CASE_BATTERY_CHANGE,//= 9 case battery level change
    GFPS_BATTERY_REPORT_RFCOMM,//= 10 report battery by rfcomm
    GFPS_BATTERY_REPORT_ADV_START,//= 11 report battery by adv
    GFPS_BATTERY_REPORT_LE_L2CAP,//=12 report battery by le l2cap channel
} T_GFPS_BATTERY_REPORT_EVENT;

void app_gfps_battery_show_ui(bool show_ui);

void app_gfps_battery_info_init(void);

void app_gfps_battery_info_report(T_GFPS_BATTERY_REPORT_EVENT event);

/** End of APP_GFPS_BATTERY
* @}
*/

/** End of APP_RWS_GFPS
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_GFPS_BATTERY_H_ */
