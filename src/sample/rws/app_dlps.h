/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DLPS_H_
#define _APP_DLPS_H_

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x_pinmux.h"
#include "app_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_DLPS App Dlps
  * @brief this file handle device dlps mode related process
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_DLPS_Exported_Macros App Dlps Macros
   * @{
   */
#define APP_DLPS_ENTER_CHECK_UART_RX            0x00000001
#define APP_DLPS_ENTER_CHECK_UART_TX            0x00000002
#define APP_DLPS_ENTER_CHECK_GPIO               0x00000004
#define APP_DLPS_ENTER_CHECK_APP                0x00000008
#define APP_DLPS_ENTER_CHECK_MFB_KEY            0x00000010
#define APP_DLPS_ENTER_CHECK_DISCHARGER_NTC     0x00000020
#define APP_DLPS_ENTER_CHECK_RSV2               0x00000040
#define APP_DLPS_ENTER_CHECK_LED                0x00000080
#define APP_DLPS_ENTER_CHECK_ADAPTOR            0x00000100
#define APP_DLPS_ENTER_CHECK_QDEC               0x00000200
#define APP_DLPS_ENTER_CHECK_RSV3               0x00000400
#define APP_DLPS_ENTER_CHECK_PLAYBACK           0x00000800
#define APP_DLPS_ENTER_CHECK_USB                0x00001000
#define APP_DLPS_ENTER_CHECK_LINEIN             0x00002000
#define APP_DLPS_ENTER_CHECK_ADP_VOLTAGE        0x00004000
#define APP_DLPS_ENTER_CHECK_WAIT_RESET         0x00008000
#define APP_DLPS_ENTER_CHECK_OTA_TOOLING_PARK   0x00010000
#define APP_DLPS_ENTER_CHECK_BOX_BAT            0x00020000
#define APP_DLPS_ENTER_CHECK_RF_XTAL_K          0x00040000
#define APP_DLPS_ENTER_CHECK_SENSOR_CALIB       0x00080000
#define APP_DLPS_ENTER_CHECK_SPP_CAPTURE        0x00100000
#define APP_DLPS_ENTER_CHECK_ANC_TEST           0x00200000
#define APP_DLPS_ENTER_CHECK_CMD_PROTECT        0x00400000
#define APP_DLPS_ENTER_CHECK_CMD                0x00800000
#define APP_DLPS_ENTER_VBAT_VALUE_UPDATE        0x01000000

/** End of APP_DLPS_Exported_Macros
    * @}
    */

typedef enum
{
    OTA_TOOLING_POWER_DONE                  = 0,
    OTA_TOOLING_SHIPPING_BOTH               = 1, /*Wake up by both 5V or MFB*/
    OTA_TOOLING_SHIPPING_5V_WAKEUP_ONLY     = 2, /*Wake up by both 5V */
    OTA_TOOLING_SHIPPING_MFB_WAKEUP_ONLY    = 3, /*Wake up by both MFB*/
} T_OTA_DLPS_SETTING;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_DLPS_Exported_Functions App Dlps Functions
    * @{
    */
/* @brief  enable dlps by clear specific bit mask
*
* @param  bit one bit that show whether dlps is allowed or not by specific function uint
* @return none
*/
void app_dlps_enable(uint32_t bit);

/* @brief  disable dlps by set specific bit mask
*
* @param  bit one bit that show whether dlps is allowed or not by specific function uint
* @return none
*/
void app_dlps_disable(uint32_t bit);

/**
    * @brief  When app is about to enter dlps mode, you need to check via this callback first.
    * @param  void
    * @return bool
    */
bool app_dlps_check_callback(void);

/**
    * @brief   Need to handle message in this callback function,when App enter dlps mode
    * @param  void
    * @return void
    */
void app_dlps_enter_callback(void);

/**
    * @brief  Need to handle message in this callback function,when App exit dlps mode.
    * @param  void
    * @return void
    */
void app_dlps_exit_callback(void);

/**
    * @brief  dlps related process when need to detect adaptor status.
    * @param  T_IO_MSG
    * @return void
    */
void app_dlps_adaptor_detect_handle_msg(T_IO_MSG *io_driver_msg_recv);

/**
    * @brief  dlps related process when power off.
    * @param  void
    * @return void
    */
void app_dlps_power_off(void);

/**
    * @brief  stop power down timer and enable already poweroff mask.
    * @param  void
    * @return void
    */
void app_dlps_enable_auto_poweroff_stop_wdg_timer(void);

/**
    * @brief  stop power down timer.
    * @param  void
    * @return void
    */
void app_dlps_stop_power_down_wdg_timer(void);

/**
    * @brief  start power down timer.
    * @param  void
    * @return void
    */
void app_dlps_start_power_down_wdg_timer(void);

/**
    * @brief  check whether do direct power on or not when wakeup from power down mode.
    * @param  void
    * @return bool
    */

bool app_dlps_check_short_press_power_on(void);

void app_dlps_set_pad_wake_up(uint8_t pinmux, PAD_WAKEUP_POL_VAL wake_up_val);

void app_dlps_restore_pad(uint8_t pinmux);

/**
    * @brief  pad wake up enable before enter dlps
    * @param  pinmux
    * @return void
    */
void app_dlps_pad_wake_up_enable(uint8_t pinmux);

/**
    * @brief  invert pad wake up polarity
    * @param  pinmux
    * @return void
    */
void app_dlps_pad_wake_up_polarity_invert(uint8_t pinmux);

/**
    * @brief  dlps module init.
    * @param  void
    * @return void
    */
void app_dlps_init(void);

/**
    * @brief  get dlps bitmap.
    * @param  void
    * @return dlps bitmap
    */
uint32_t app_dlps_get_dlps_bitmap(void);

/**
    * @brief  set power mode.
    * @param  void
    * @return void
    */
void app_dlps_power_mode_set(void);

#if (F_APP_PERIODIC_WAKEUP == 1)
#define RTC_PRESCALER_VALUE             4095
/* 1 counter : (prescaler_value + 1)/32000  sec */
/* second: counter / (RTC_CLOCK_SOURCE_FREQ / (RTC_PRESCALER_VALUE + 1)) */
#define RTC_COUNTER_TO_SECOND(counter)  (counter / (32000.0 / (RTC_PRESCALER_VALUE + 1)))
/**
    * @brief  clear rtc interrupt bit when system wakeup
    * @param  void
    * @return void
    */
void app_dlps_system_wakeup_clear_rtc_int(void);
#endif

/** @} */ /* End of group APP_DLPS_Exported_Functions */
/** End of APP_DLPS
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_DLPS_H_ */
