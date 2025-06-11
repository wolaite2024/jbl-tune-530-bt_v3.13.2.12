#ifndef __USB_AUDIO_H__
#define __USB_AUDIO_H__
#include <stdint.h>

#include "usb_audio_driver.h"

/** @defgroup APP_USB APP USB
  * @brief app usb module.
  * @{
  */

/** @defgroup USB_AUDIO_IF USB Audio Interface
  * @brief USB Audio interface
  * @{
  */

/**
 * usb_audio.h
 *
 * \brief   USB Audio stream volume type
 *
 * \ingroup USB_AUDIO_IF
 */
typedef enum
{
    USB_AUDIO_VOL_TYPE_CUR,
    USB_AUDIO_VOL_TYPE_RANGE,
    USB_AUDIO_VOL_TYPE_INVALID,
} T_USB_AUDIO_VOL_TYPE;

typedef enum
{
    USB_AUDIO_CTRL_EVT_ACTIVATE,
    USB_AUDIO_CTRL_EVT_DEACTIVATE,
    USB_AUDIO_CTRL_EVT_VOL_SET,
    USB_AUDIO_CTRL_EVT_VOL_GET,
    USB_AUDIO_CTRL_EVT_MUTE_SET,
    USB_AUDIO_CTRL_EVT_MUTE_GET,
    USB_AUDIO_CTRL_EVT_MAX,
} T_USB_AUDIO_CTRL_EVT;

typedef struct _usb_audio_ctrl_attr
{
    uint8_t dir;
    union
    {
        uint32_t d32;
        struct
        {
            uint32_t      type: 16;
            uint32_t      value: 16;
        } vol;

        struct
        {
            uint32_t value;
        } mute;

        struct
        {
            uint32_t bit_width: 6;
            uint32_t channels: 4;
            uint32_t sample_rate: 22;
        } audio;
    } content;

} T_USB_AUDIO_PIPE_ATTR;

struct _usb_audio_pipes;
typedef int (*USB_AUDIO_PIPE_CB_STREAM)(struct _usb_audio_pipes *pipe, void *buf, uint32_t len);
typedef int (*USB_AUDIO_PIPE_CB_CTRL)(struct _usb_audio_pipes *pipe, T_USB_AUDIO_CTRL_EVT evt,
                                      T_USB_AUDIO_PIPE_ATTR ctrl);
typedef struct _usb_audio_pipes
{
    uint32_t label;
    USB_AUDIO_PIPE_CB_CTRL ctrl;
    USB_AUDIO_PIPE_CB_STREAM upstream;
    USB_AUDIO_PIPE_CB_STREAM downstream;
} T_USB_AUDIO_PIPES;

/**
 * usb_audio.h
 *
 * \brief   usb audio init
 *
 * \ingroup USB_AUDIO_IF
 */
void usb_audio_init(T_USB_AUDIO_PIPES *pipe);

/** @}*/
/** End of USB_AUDIO_IF
*/

/** @}*/
/** End of APP_USB
*/
#endif
