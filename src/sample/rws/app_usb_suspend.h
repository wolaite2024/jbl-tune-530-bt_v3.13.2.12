#ifndef __APP_USB_SUSPEND_H__
#define __APP_USB_SUSPEND_H__

/** @defgroup APP_USB_SUSPEND
  * @brief app usb suspend module.
  * @{
  */

/* @brief  enable usb suspend, APP suspend and stop charging
*
* @param  void
* @return none
*/
void app_usb_suspend_enable(void);

/* @brief  disable usb suspend, reset to exit suspend mode and resume charging
*
* @param  void
* @return none
*/
void app_usb_suspend_disable(void);

/* @brief  app usb suspend module init.
*
* @param  void
* @return none
*/
void app_usb_suspend_init(void);

/** @}*/
/** End of APP_USB_SUSPEND
*/

#endif
