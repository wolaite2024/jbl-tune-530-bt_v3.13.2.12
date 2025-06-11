#ifndef _USB_AUDIO_DRIVER_H_
#define _USB_AUDIO_DRIVER_H_
#include <stdbool.h>
#include <stdint.h>

/** @defgroup 87x3e_USB_AUDIO_Driver USB Audio Driver
  * @brief app usb module.
  * @{
  */


/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio class verison
 *
 */
#define USB_AUDIO_VERSION_1         0x01
#define USB_AUDIO_VERSION_2         0x02

/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio callback
 *
 */
typedef bool (*BOOL_USB_AUDIO_CB)();
typedef void (*VOID_USB_AUDIO_CB)();

/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio related event from  USB Core
 *
 */
typedef enum
{
    USB_AUDIO_DRIVER_CB_ACTIVE = 0,
    USB_AUDIO_DRIVER_CB_DEACTIVE,
    USB_AUDIO_DRIVER_CB_DATA_XMIT_OUT,
    USB_AUDIO_DRIVER_CB_DATA_XMIT_IN,
    USB_AUDIO_DRIVER_CB_SPK_VOL_SET,
    USB_AUDIO_DRIVER_CB_SPK_VOL_GET,
    USB_AUDIO_DRIVER_CB_SPK_MUTE_SET,
    USB_AUDIO_DRIVER_CB_MIC_VOL_SET,
    USB_AUDIO_DRIVER_CB_MIC_VOL_GET,
    USB_AUDIO_DRIVER_CB_MIC_MUTE_SET,
    USB_AUDIO_DRIVER_CB_MAX,
} T_AUDIO_CB;

/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio stream direction
 *
 */
typedef enum
{
    USB_AUDIO_DIR_OUT = 1,
    USB_AUDIO_DIR_IN = 2,
} T_USB_AUDIO_DIR;

typedef  int (*USB_AUDIO_DRV_CB_ACTIVATE)(uint8_t dir, uint8_t bit_res, uint32_t sample_rate,
                                          uint8_t chan_num);
typedef  int (*USB_AUDIO_DRV_CB_DEACTIVATE)(uint8_t dir);
typedef  int (*USB_AUDIO_DRV_CB_XMIT)(uint8_t *buf, uint16_t len);

/**
 * @brief USB Audio driver cbs to transport necessary informations or audio data to upper sw \n
 *        \ref activate: usb audio function is ready to transmit audio data, param \dir is \n
 *        defined in \ref T_USB_AUDIO_DIR, \ref bit_res, \ref sample_rate and \ref chan_num are used together \n
 *        to define audio data attribute \n
 *        \ref deactivate: usb audio function is no longer transmit audio data, param \dir is \n
 *        defined in \ref T_USB_AUDIO_DIR \n
 *        \ref upstream: transmit data from device to host, \ref buf is audio data will be sent, \ref len is length of \ref buf \n
 *        \ref downstream: transmit data from host to device, \ref buf is audio data has been received, \ref len is length of \ref buf \n
 * @ingroup USB_CORE
 */
typedef struct _usb_audio_driver_cbs
{
    USB_AUDIO_DRV_CB_ACTIVATE activate;
    USB_AUDIO_DRV_CB_DEACTIVATE deactivate;
    USB_AUDIO_DRV_CB_XMIT upstream;
    USB_AUDIO_DRV_CB_XMIT downstream;
} T_USB_AUDIO_DRIVER_CBS;

/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio control
 *
 */
typedef struct _usb_audio_driver_ctrl
{
    uint8_t     num;
    void        *buf;
} T_USB_AUDIO_DRIVER_CTRL;

/**
 * usb_audio_driver.h
 *
 * \brief   USB Audio stream atrributes
 *
 */
typedef struct _usb_audio_driver_attr
{
    T_USB_AUDIO_DIR dir;
    uint8_t         chann_num;
    uint8_t         bit_width;
    uint32_t        max_sample_rate;
} T_USB_AUDIO_ATTR;

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface descriptor register.
 *
 * \param[in] desc USB audio descriptor
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_desc_register(void **desc, uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface callback register.
 *
 * \param[in] cbs callback  in \ref T_AUDIO_CB
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_cb_register(T_USB_AUDIO_DRIVER_CBS *cbs, uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface control-related register.
 *
 * \param[in] ctrl USB Audio control-related structure used for volume/mute .etc
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_ctrl_register(T_USB_AUDIO_DRIVER_CTRL *ctrl, uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface descriptor unregister.
 *
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_desc_unregister(uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface callback unregister.
 *
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_cb_unregister(uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface control-related unregister.
 *
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_ctrl_unregister(uint8_t uac_ver);

/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface frequency set to USB audio driver.
 *
 * \param[in] dir USB Audio direction \ref T_USB_AUDIO_DIR
 * \param[in] freq USB Audio frequency
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_freq_set(T_USB_AUDIO_DIR dir, uint32_t freq, uint8_t uac_ver);


/**
 * usb_dev_driver.h
 *
 * \brief   USB audio interface attribute init.
 *
 * \param[in] attr attribute defined in \ref  T_USB_AUDIO_ATTR
 * \param[in] uac_ver USB Audio version
 *
 */
void usb_audio_driver_attr_init(T_USB_AUDIO_ATTR attr, uint8_t uac_ver);


/** @}*/
/** End of 87x3e_USB_AUDIO_Driver
*/
#endif
