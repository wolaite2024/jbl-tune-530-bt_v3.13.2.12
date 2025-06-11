#ifndef __USB_AUDIO_IF_H
#define __USB_AUDIO_IF_H

#include <stdint.h>
#include <stdbool.h>

#define USB_AUDIO_HID_REPORT              0x01
#define USB_AUDIO_POWER_STATUS_REPORT     0x02

bool usb_audio_interface(void *audio_if);

typedef void (*APP_USB_AUDIO_PLUG_CALLBACK)(uint8_t audio_path, uint8_t bit_res, uint8_t sf,
                                            uint8_t chann_mode);
typedef void (*APP_USB_AUDIO_UNPLUG_CALLBACK)(uint8_t audio_path);
typedef bool (*APP_USB_AUDIO_CHECK_CALLBACK)(uint8_t audio_path);
typedef bool (*AU_PUT_USB_AUDIO_DATA_CALLBACK)(uint8_t *p_data, uint16_t len);
typedef void (*APP_SET_USB_AUDIO_GAIN_CALLBACK)(uint8_t lr_chann, uint8_t gain);
typedef bool (*AU_PULL_USB_AUDIO_DATA_CALLBACK)(uint8_t *p_data, uint16_t len);


void usb_if_set_app_usb_audio_plug_callback(APP_USB_AUDIO_PLUG_CALLBACK callback_func);
void usb_if_set_app_usb_audio_unplug_callback(APP_USB_AUDIO_UNPLUG_CALLBACK callback_func);
void usb_if_set_au_put_audio_data_callback(AU_PUT_USB_AUDIO_DATA_CALLBACK callback_func);
void usb_if_set_app_set_usb_audio_gain_callback(APP_SET_USB_AUDIO_GAIN_CALLBACK callback_func);
void usb_if_set_au_pull_audio_data_callback(AU_PULL_USB_AUDIO_DATA_CALLBACK callback_func);

#endif
