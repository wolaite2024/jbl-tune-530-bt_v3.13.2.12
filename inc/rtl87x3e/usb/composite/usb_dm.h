#ifndef __USB_DM__
#define __USB_DM__
#include <stdbool.h>
#include <stdint.h>

/** @defgroup 87x3e_USB_DTM USB DTM
  * @brief app usb module.
  * @{
  */


/**
 * usb_dm.h
 *
 * \brief   usb device manager event used in \ref USB_DM_CB
 *
 */
typedef enum
{
    USB_DM_EVT_STATUS_IND                   = 0,
    USB_DM_EVT_RESET_INTR                   = 1,
    USB_DM_EVT_GIP_MICRO_OS_DESC            = 2,
    USB_DM_EVT_GET_DEVICE_QUALIFIER_DESC    = 3,
    USB_DM_EVT_CLEAR_FEATURE_ENDPOINT_HALT  = 4,
} T_USB_DM_EVT;

/**
 * usb_dm.h
 *
 * \brief   usb power state obtained from event \ref T_USB_DM_EVT
 *
 */
typedef enum  {USB_PDN = 0,
               USB_RTK_PWRON_SEQ_DONE,
               USB_SW_INIT,
               USB_ATTACHED,
               USB_ADDRESSED,
               USB_CONFIGURATED,
               USB_SUSPEND_STATE,
               USB_ACTIVE
              } T_USB_POWER_STATE;

/**
 * usb_dm.h
 *
 * \brief   USB device manager callback
 *
 * \param[in] T_USB_DM_EVT USB DM EVT defined in \ref T_USB_DM_EVT
 *
 * \param[in] uint32_t Optional parameter depending on different event
 *
 */
typedef bool (*USB_DM_CB)(T_USB_DM_EVT, uint32_t);

/**
 * usb_dm.h
 *
 * \brief   USB settings such as speed \ref USB_SPEED /interface .etc
 *
 */
typedef struct _t_usb_core_config
{
    uint8_t speed;
    struct
    {
        uint8_t uac_enable: 1;
        uint8_t hid_enable: 1;
        uint8_t dual_uac_enable: 1;
        uint8_t rsv: 6;
    } class_set;
} T_USB_CORE_CONFIG;

/**
 * usb_dm.h
 *
 * \brief   USB speed definition
 *
 */
#define USB_SPEED_FULL  0
#define USB_SPEED_HIGH  1
#define USB_SPEED       USB_SPEED_HIGH

/**
 * usb_dm.h
 *
 * \brief   USB core init
 *
 * \param[in] config USB core settings in \ref T_USB_CORE_CONFIG
 *
 */
void usb_dm_core_init(T_USB_CORE_CONFIG config);

/**
 * usb_dm.h
 *
 * \brief   USB core start, this api will start USB task
 *
 */
void usb_dm_start(void);

/**
 * usb_dm.h
 *
 * \brief   USB core start, this api will stop USB task
 *
 */
void usb_dm_stop(void);

/**
 * usb_dm.h
 *
 * \brief   register USB dm callback
 *
 *
 * \param[in] cb USB dm callback \ref USB_DM_CB
 *
 */
void usb_dm_cb_register(USB_DM_CB cb);

/**
 * usb_dm.h
 *
 * \brief   unregister USB dm callback
 *
 */
void usb_dm_cb_unregister(void);


/** @}*/
/** End of 87x3e_USB_DTM
*/
#endif
