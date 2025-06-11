#ifndef _USB_AUDIO_CONFIG_H_
#define _USB_AUDIO_CONFIG_H_
#include "usb_audio_driver.h"

#define UAC_SPK_SUPPORT             1
#define UAC_MIC_SUPPORT             1

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#define USB_AUDIO_VERSION           USB_AUDIO_VERSION_2
#else
#define USB_AUDIO_VERSION           USB_AUDIO_VERSION_1
#endif

#if RTL8763ESE_PRODUCT_SPEAKER
#undef UAC_MIC_SUPPORT
#define UAC_MIC_SUPPORT             0
#endif

#define UAC_SAM_RATE_8000                  8000
#define UAC_SAM_RATE_16000                 16000
#define UAC_SAM_RATE_32000                 32000
#define UAC_SAM_RATE_44100                 44100
#define UAC_SAM_RATE_48000                 48000
#define UAC_SAM_RATE_96000                 96000
#define UAC_SAM_RATE_192000                192000

#define UAC_BIT_RES_16                      16
#define UAC_BIT_RES_24                      24

#define UAC_CHAN_MONO                       1
#define UAC_CHAN_STEREO                     2

#if(USB_AUDIO_VERSION == USB_AUDIO_VERSION_2)

#define UAC2_SPK_BIT_RES                    UAC_BIT_RES_24
#define UAC2_MIC_BIT_RES                    UAC_BIT_RES_16

#define UAC2_SPK_CHAN_NUM                   UAC_CHAN_STEREO
#define UAC2_MIC_CHAN_NUM                   UAC_CHAN_MONO

#define UAC2_MIC_SAM_RATE_NUM               3
#define UAC2_SPK_SAM_RATE_NUM               5
#define UAC2_SPK_SAM_RATE_MAX               UAC_SAM_RATE_192000
#define UAC2_MIC_SAM_RATE_MAX               UAC_SAM_RATE_32000

#define UAC2_SPK_VOL_RANGE_NUM              1
#define UAC2_SPK_VOL_CUR                    0xF000
#define UAC2_SPK_VOL_MIN                    0xDB00
#define UAC2_SPK_VOL_MAX                    0x0000
#define UAC2_SPK_VOL_RES                    0x0100

#define UAC2_MIC_VOL_RANGE_NUM              1
#define UAC2_MIC_VOL_CUR                    0xF000
#define UAC2_MIC_VOL_MIN                    0xDB00
#define UAC2_MIC_VOL_MAX                    0x0000
#define UAC2_MIC_VOL_RES                    0x0100

#else
#define UAC1_BIT_RES_16                      16
#define UAC1_BIT_RES_24                      24

#if RTL8763ESE_PRODUCT_SPEAKER
#define UAC1_SPK_BIT_RES                    UAC1_BIT_RES_24
#else
#define UAC1_SPK_BIT_RES                    UAC1_BIT_RES_16
#endif
#define UAC1_MIC_BIT_RES                    UAC1_BIT_RES_16

#define UAC1_SPK_CHAN_NUM                   UAC_CHAN_STEREO
#define UAC1_MIC_CHAN_NUM                   UAC_CHAN_MONO

#if TARGET_RTL8763ESE | TARGET_RTL8763EWM
#define UAC1_SPK_SAM_RATE_NUM               1
#else
#define UAC1_SPK_SAM_RATE_NUM               1
#endif
#define UAC1_MIC_SAM_RATE_NUM               1

#if TARGET_RTL8763EWM
#define UAC1_SPK_SAM_RATE_MAX               UAC_SAM_RATE_48000
#define UAC1_MIC_SAM_RATE_MAX               UAC_SAM_RATE_16000
#else
#define UAC1_SPK_SAM_RATE_MAX               UAC_SAM_RATE_48000
#define UAC1_MIC_SAM_RATE_MAX               UAC_SAM_RATE_32000
#endif

#define UAC1_SPK_VOL_CUR                    0xF600
#define UAC1_SPK_VOL_MIN                    0xDB00
#define UAC1_SPK_VOL_MAX                    0x0000
#define UAC1_SPK_VOL_RES                    0x0100

#define UAC1_MIC_VOL_CUR                    0xF000
#define UAC1_MIC_VOL_MIN                    0xDB00
#define UAC1_MIC_VOL_MAX                    0x0000
#define UAC1_MIC_VOL_RES                    0x0100

#if UAC_SPK_SUPPORT & UAC_MIC_SUPPORT
#define UAC1_STREAM_INTF_NUM            2
#else
#define UAC1_STREAM_INTF_NUM            1
#endif
#endif

#endif

