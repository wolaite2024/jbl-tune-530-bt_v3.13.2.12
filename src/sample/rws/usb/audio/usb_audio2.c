#if F_APP_USB_AUDIO_SUPPORT
#include "usb_audio_config.h"
#if(USB_AUDIO_VERSION == USB_AUDIO_VERSION_2)
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "hal_adp.h"
#include "usb_spec20.h"
#include "uac2.h"
#include "usb_audio_driver.h"
#include "usb_audio.h"
#include "usb_dm.h"
#include "app_cfg.h"

static T_USB_AUDIO_PIPES *usb_audio2_pipe = NULL;
static T_USB_AUDIO_DRIVER_CTRL_ATTR drv_ctrl_attr = {.data = NULL, .len = 0};
static void *uac2_inst = NULL;

#define USB_AUDIO2_PIPE_CALLBACK(cb, pipe, param1, param2) \
    if(pipe && cb)                                                     \
    {                                                          \
        cb(pipe, param1, param2);                              \
    }

#define T_CTRL_ATTR_FREQ_SPK    typeof(freq_attr_spk)
#define T_CTRL_ATTR_FREQ_MIC    typeof(freq_attr_mic)

static const T_UAC2_STD_IA_DESC uac2_ia_desc =
{
    .bLength                = sizeof(T_UAC2_STD_IA_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE_ASSOCIATION,
    .bFirstInterface        = 0,
#if UAC_MIC_SUPPORT
    .bInterfaceCount        = 3,
#else
    .bInterfaceCount        = 2,
#endif
    .bFunctionClass         = UAC2_CLASS_CODE_AUDIO,
    .bFunctionSubClass      = UAC2_FUNCTION_SUBCLASS_UNDEFINED,
    .bFunctionProtocol      = UAC2_AF_VERSION_02_00,
    .iFunction              = 0,
};

static T_UAC2_STD_IF_DESC uac2_std_ac_if_desc =
{
    .bLength                = sizeof(T_UAC2_STD_IF_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE,
    .bInterfaceNumber       = 0,
    .bAlternateSetting      = 0,
    .bNumEndpoints          = 0,
    .bInterfaceClass        = UAC2_CLASS_CODE_AUDIO,
    .bInterfaceSubClass     = UAC2_SUBCLASS_AUDIOCONTROL,
    .bInterfaceProtocol     = IP_VERSION_02_00,
    .iInterface             = 0,
};

#if UAC_SPK_SUPPORT
const T_UAC2_CLK_SRC_DESC uac2_cs_desc_spk =
{
    .bLength                = sizeof(T_UAC2_CLK_SRC_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_CLOCK_SOURCE,
    .bClockID               = UAC2_ID_CLOCK_SOURCE1,
    .bmAttributes           = 0x03,
    .bmControls             = 0x03,
    .bAssocTerminal         = 0x00,
    .iClockSource           = 0,
};

const T_UAC2_FU_DESC uac2_fu_desc_spk =
{
    .bLength                = sizeof(T_UAC2_FU_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_FEATURE_UNIT,
    .bUnitID                = UAC2_ID_FEATURE_UNIT,
    .bSourceID              = UAC2_ID_INPUT_TERMINAL1,
    .bmaControls            =
    {[0] = 0x00000003, [1] = 0x0000000C, [2] = 0x0000000C,},
    .iFeature               = 0,
};

static const T_UAC2_IT_DESC uac2_it_desc_spk =
{
    .bLength                = sizeof(T_UAC2_IT_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_INPUT_TERMINAL,
    .bTerminalID            = UAC2_ID_INPUT_TERMINAL1,
    .wTerminalType          = UAC2_TERMINAL_STREAMING,
    .bAssocTerminal         = 0x00,
    .bCSourceID             = UAC2_ID_CLOCK_SOURCE1,
    .bNrChannels            = UAC2_SPK_CHAN_NUM,
    .bmChannelConfig        = 0x03,
    .iChannelNames          = 0x00,
    .bmControls             = 0x00,
    .iTerminal              = 0x00,
};

static const T_UAC2_OT_DESC uac2_ot_desc_spk =
{
    .bLength                = sizeof(T_UAC2_OT_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_OUTPUT_TERMINAL,
    .bTerminalID            = UAC2_ID_OUTPUT_TERMINAL1,
    .wTerminalType          = UAC2_OUTPUT_TERMINAL_SPEAKER,
    .bAssocTerminal         = 0x00,
    .bSourceID              = UAC2_ID_FEATURE_UNIT,
    .bCSourceID             = UAC2_ID_CLOCK_SOURCE1,
    .bmControls             = 0x00,
    .iTerminal              = 0x00,
};
#endif

#if UAC_MIC_SUPPORT
const T_UAC2_CLK_SRC_DESC uac2_cs_desc_mic =
{
    .bLength                = sizeof(T_UAC2_CLK_SRC_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_CLOCK_SOURCE,
    .bClockID               = UAC2_ID_CLOCK_SOURCE2,
    .bmAttributes           = 0x03,
    .bmControls             = 0x03,
    .bAssocTerminal         = 0x00,
    .iClockSource           = 0,
};

static const T_UAC2_IT_DESC uac2_it_desc_mic =
{
    .bLength                = sizeof(T_UAC2_IT_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_INPUT_TERMINAL,
    .bTerminalID            = UAC2_ID_INPUT_TERMINAL2,
    .wTerminalType          = UAC2_INPUT_TERMINAL_MICROPHONE,
    .bAssocTerminal         = 0x00,
    .bCSourceID             = UAC2_ID_CLOCK_SOURCE2,
    .bNrChannels            = UAC2_MIC_CHAN_NUM,
    .bmChannelConfig        = 0x04,
    .iChannelNames          = 0x00,
    .bmControls             = 0x00,
    .iTerminal              = 0x00,
};

static const T_UAC2_OT_DESC uac2_ot_desc_mic =
{
    .bLength                = sizeof(T_UAC2_OT_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_OUTPUT_TERMINAL,
    .bTerminalID            = UAC2_ID_OUTPUT_TERMINAL2,
    .wTerminalType          = UAC2_TERMINAL_STREAMING,
    .bAssocTerminal         = 0x00,
    .bSourceID              = UAC2_ID_FEATURE_UNIT2,
    .bCSourceID             = UAC2_ID_CLOCK_SOURCE2,
    .bmControls             = 0x00,
    .iTerminal              = 0x00,
};

static const T_UAC2_FU_DESC_MIC uac2_fu_desc_mic =
{
    .bLength                = sizeof(T_UAC2_FU_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_FEATURE_UNIT,
    .bUnitID                = UAC2_ID_FEATURE_UNIT2,
    .bSourceID              = UAC2_ID_INPUT_TERMINAL2,
    .bmaControls            =
    {[0] = 0x0000000F, [1] = 0x00000000},
    .iFeature               = 0,
};

#endif

static const T_UAC2_AC_IF_HDR_DESC uac2_ac_if_hdr_desc =
{
    .bLength                = sizeof(T_UAC2_AC_IF_HDR_DESC),
    .bDescriptorType        = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype     = UAC2_HEADER,
    .bcdADC                 = 0x0200,
    .bCategory              = UAC2_CATEGORY_HEADSET,
#if UAC_MIC_SUPPORT
    .wTotalLength           = sizeof(uac2_ac_if_hdr_desc) + sizeof(uac2_cs_desc_spk)
    + sizeof(uac2_fu_desc_spk) + sizeof(uac2_it_desc_spk) + sizeof(uac2_cs_desc_spk)
    + sizeof(uac2_fu_desc_mic) + sizeof(uac2_ot_desc_spk) + sizeof(uac2_it_desc_mic)
    + sizeof(uac2_ot_desc_mic),
#else
    .wTotalLength           = sizeof(uac2_ac_if_hdr_desc) + sizeof(uac2_cs_desc1_spk)
    + sizeof(uac2_fu_desc_spk) + sizeof(uac2_it_desc_spk)
    + sizeof(uac2_ot_desc_spk),
#endif
    .bmControls             = 0x00,
};

#if UAC_SPK_SUPPORT
static T_UAC2_STD_IF_DESC uac2_std_as_if_desc_alt0_spk =
{
    .bLength                = sizeof(T_UAC2_STD_IF_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE,
    .bInterfaceNumber       = 1,
    .bAlternateSetting      = 0,
    .bNumEndpoints          = 0,
    .bInterfaceClass        = UAC2_CLASS_CODE_AUDIO,
    .bInterfaceSubClass     = UAC2_SUBCLASS_AUDIOSTREAMING,
    .bInterfaceProtocol     = IP_VERSION_02_00,
    .iInterface             = 0x00,
};

static T_UAC2_STD_IF_DESC uac2_std_as_if_desc_alt1_spk =
{
    .bLength                = sizeof(T_UAC2_STD_IF_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE,
    .bInterfaceNumber       = 1,
    .bAlternateSetting      = 1,
    .bNumEndpoints          = 1,
    .bInterfaceClass        = UAC2_CLASS_CODE_AUDIO,
    .bInterfaceSubClass     = UAC2_SUBCLASS_AUDIOSTREAMING,
    .bInterfaceProtocol     = IP_VERSION_02_00,
    .iInterface             = 0x00,
};


static const T_UAC2_AS_IF_DESC uac2_as_if_desc_spk =
{
    .bLength               = sizeof(T_UAC2_AS_IF_DESC),
    .bDescriptorType       = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype    = UAC2_AS_GENERAL,
    .bTerminalLink         = UAC2_ID_INPUT_TERMINAL1,
    .bmControls            = 0x03,
    .bFormatType           = UAC2_FORMAT_TYPE_I_PCM,
    .bmFormats             = 0x01,//PCM
    .bNrChannels           = UAC2_SPK_CHAN_NUM,
    .bmChannelConfig       = 0x03,
    .iChannelNames         = 0x00,
};

static const T_UAC2_AS_TYPE_1_FMT_DESC uac2_as_type_1_fmt_desc_spk =
{
    .bLength               = sizeof(T_UAC2_AS_TYPE_1_FMT_DESC),
    .bDescriptorType       = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype    = UAC2_FORMAT_TYPE,
    .bFormatType           = UAC2_FORMAT_TYPE_I_PCM,
    .bSubslotSize          = 3,
    .bBitResolution        = UAC2_SPK_BIT_RES,
};

static T_UAC2_STD_EP_DESC uac2_std_ep_desc_spk_hs =
{
    .bLength                = sizeof(T_UAC2_STD_EP_DESC),
    .bDescriptorType        = UAC2_DT_ENDPOINT,
    .bEndpointAddress       = UAC2_ISO_OUT_ENDPOINT_ADDRESS,
    .bmAttributes           = UAC2_EP_XFER_ISOC | UAC2_EP_SYNC_ADAPTIVE,
    .wMaxPacketSize         = 576,
    .bInterval              = UAC2_DS_INTERVAL,
};

static T_UAC2_STD_EP_DESC uac2_std_ep_desc_spk_fs =
{
    .bLength                = sizeof(T_UAC2_STD_EP_DESC),
    .bDescriptorType        = UAC2_DT_ENDPOINT,
    .bEndpointAddress       = UAC2_ISO_OUT_ENDPOINT_ADDRESS,
    .bmAttributes           = UAC2_EP_XFER_ISOC | UAC2_EP_SYNC_ADAPTIVE,
    .wMaxPacketSize         = 576,
    .bInterval              = 1,
};

static const T_UAC2_AS_ISO_EP_DESC uac2_iso_ep_desc_spk =
{
    .bLength                = sizeof(T_UAC2_AS_ISO_EP_DESC),
    .bDescriptorType        = UAC2_DT_CS_ENDPOINT,
    .bDescriptorSubtype     = UAC2_EP_GENERAL,
    .bmAttributes           = 0x00,
    .bmControls             = 0x00,
    .bLockDelayUnits        = 0x00,
    .wLockDelay             = 0x0000,
};
#endif

#if UAC_MIC_SUPPORT
static T_UAC2_STD_IF_DESC uac2_std_as_if_desc_alt0_mic =
{
    .bLength                = sizeof(T_UAC2_STD_IF_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE,
    .bInterfaceNumber       = 2,
    .bAlternateSetting      = 0,
    .bNumEndpoints          = 0,
    .bInterfaceClass        = UAC2_CLASS_CODE_AUDIO,
    .bInterfaceSubClass     = UAC2_SUBCLASS_AUDIOSTREAMING,
    .bInterfaceProtocol     = IP_VERSION_02_00,
    .iInterface             = 0x00,
};

static T_UAC2_STD_IF_DESC uac2_std_as_if_desc_alt1_mic =
{
    .bLength                = sizeof(T_UAC2_STD_IF_DESC),
    .bDescriptorType        = UAC2_DT_INTERFACE,
    .bInterfaceNumber       = 2,
    .bAlternateSetting      = 1,
    .bNumEndpoints          = 1,
    .bInterfaceClass        = UAC2_CLASS_CODE_AUDIO,
    .bInterfaceSubClass     = UAC2_SUBCLASS_AUDIOSTREAMING,
    .bInterfaceProtocol     = IP_VERSION_02_00,
    .iInterface             = 0x00,
};


static const T_UAC2_AS_IF_DESC uac2_as_if_desc_mic =
{
    .bLength               = sizeof(T_UAC2_AS_IF_DESC),
    .bDescriptorType       = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype    = UAC2_AS_GENERAL,
    .bTerminalLink         = UAC2_ID_OUTPUT_TERMINAL2,
    .bmControls            = 0x05,
    .bFormatType           = UAC2_FORMAT_TYPE_I_PCM,
    .bmFormats             = 0x01,//PCM
    .bNrChannels           = UAC2_MIC_CHAN_NUM,
    .bmChannelConfig       = 0x04,
    .iChannelNames         = 0x00,
};

static const T_UAC2_AS_TYPE_1_FMT_DESC uac2_as_type_1_fmt_desc_mic =
{
    .bLength               = sizeof(T_UAC2_AS_TYPE_1_FMT_DESC),
    .bDescriptorType       = UAC2_DT_CS_INTERFACE,
    .bDescriptorSubtype    = UAC2_FORMAT_TYPE,
    .bFormatType           = UAC2_FORMAT_TYPE_I_PCM,
    .bSubslotSize          = 2,
    .bBitResolution        = UAC2_MIC_BIT_RES,
};

static T_UAC2_STD_EP_DESC uac2_std_ep_desc_mic_hs =
{
    .bLength                = sizeof(T_UAC2_STD_EP_DESC),
    .bDescriptorType        = UAC2_DT_ENDPOINT,
    .bEndpointAddress       = UAC2_ISO_IN_ENDPOINT_ADDRESS,
    .bmAttributes           = UAC2_EP_XFER_ISOC | UAC2_EP_SYNC_SYNC,
    .wMaxPacketSize         = 32 * 2 * 1,
    .bInterval              = 4,
};

static T_UAC2_STD_EP_DESC uac2_std_ep_desc_mic_fs =
{
    .bLength                = sizeof(T_UAC2_STD_EP_DESC),
    .bDescriptorType        = UAC2_DT_ENDPOINT,
    .bEndpointAddress       = UAC2_ISO_IN_ENDPOINT_ADDRESS,
    .bmAttributes           = UAC2_EP_XFER_ISOC | UAC2_EP_SYNC_SYNC,
    .wMaxPacketSize         = 32 * 2 * 1,
    .bInterval              = 4,
};

static const T_UAC2_AS_ISO_EP_DESC uac2_iso_ep_desc_mic =
{
    .bLength                = sizeof(T_UAC2_AS_ISO_EP_DESC),
    .bDescriptorType        = UAC2_DT_CS_ENDPOINT,
    .bDescriptorSubtype     = UAC2_EP_GENERAL,
    .bmAttributes           = 0x00,
    .bmControls             = 0x01,
    .bLockDelayUnits        = 0x00,
    .wLockDelay             = 0x0000,
};
#endif

static const T_USB_AUDIO_DRIVER_DESC_HDR *uac2_descs_ctrl[] =
{
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_ia_desc,

    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_ac_if_desc,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_ac_if_hdr_desc,
#if UAC_SPK_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_cs_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_fu_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_it_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_ot_desc_spk,
#endif
#if UAC_MIC_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_cs_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_fu_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_it_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_ot_desc_mic,
#endif

    NULL
};

static const T_USB_AUDIO_DRIVER_DESC_HDR *uac2_descs_spk_hs[] =
{
#if UAC_SPK_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt0_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt1_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_if_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_type_1_fmt_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_ep_desc_spk_hs,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_iso_ep_desc_spk,
#endif

    NULL
};

static const T_USB_AUDIO_DRIVER_DESC_HDR *uac2_descs_spk_fs[] =
{
#if UAC_SPK_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt0_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt1_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_if_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_type_1_fmt_desc_spk,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_ep_desc_spk_fs,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_iso_ep_desc_spk,
#endif

    NULL
};

static const T_USB_AUDIO_DRIVER_DESC_HDR *uac2_descs_mic_hs[] =
{
#if UAC_MIC_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt0_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt1_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_if_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_type_1_fmt_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_ep_desc_mic_hs,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_iso_ep_desc_mic,
#endif

    NULL
};

static const T_USB_AUDIO_DRIVER_DESC_HDR *uac2_descs_mic_fs[] =
{
#if UAC_MIC_SUPPORT
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt0_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_as_if_desc_alt1_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_if_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_as_type_1_fmt_desc_mic,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_std_ep_desc_mic_fs,
    (T_USB_AUDIO_DRIVER_DESC_HDR *) &uac2_iso_ep_desc_mic,
#endif

    NULL
};

#if UAC_SPK_SUPPORT
static T_CTRL_ATTR_FREQ(spk, UAC2_SPK_SAM_RATE_NUM) freq_attr_spk =
{
    .cur =  {.dCUR = UAC_SAM_RATE_48000},
    .range =
    {
        .wNumSubRanges = UAC2_SPK_SAM_RATE_NUM,
        .ranges =
        {
            [0] = {.dMIN = UAC_SAM_RATE_32000, .dMAX = UAC_SAM_RATE_32000, .dRES = 0},
            [1] = {.dMIN = UAC_SAM_RATE_44100, .dMAX = UAC_SAM_RATE_44100, .dRES = 0},
            [2] = {.dMIN = UAC_SAM_RATE_48000, .dMAX = UAC_SAM_RATE_48000, .dRES = 0},
            [3] = {.dMIN = UAC_SAM_RATE_96000, .dMAX = UAC_SAM_RATE_96000, .dRES = 0},
            [4] = {.dMIN = UAC_SAM_RATE_192000, .dMAX = UAC_SAM_RATE_192000, .dRES = 0},

        },
    },
};

static T_CTRL_ATTR_VOL vol_attr_spk =
{
    .cur =  {.wCUR = UAC2_SPK_VOL_CUR},
    .range =
    {
        .wNumSubRanges = 1,
        .ranges =
        {
            [0] = {.wMIN = UAC2_SPK_VOL_MIN, .wMAX = UAC2_SPK_VOL_MAX, .wRES = UAC2_SPK_VOL_RES}
        },
    },
};

static T_CTRL_ATTR_MUTE mute_attr_spk =
{
    .cur =  {.bCUR = 0},
};

static T_CTRL_ATTR_FREQ(mic, UAC2_MIC_SAM_RATE_NUM) freq_attr_mic =
{
    .cur =  {.dCUR = UAC_SAM_RATE_32000},
    .range =
    {
        .wNumSubRanges = UAC2_MIC_SAM_RATE_NUM,
        .ranges =
        {
            [0] = {.dMIN = UAC_SAM_RATE_8000, .dMAX = UAC_SAM_RATE_8000, .dRES = 0},
            [1] = {.dMIN = UAC_SAM_RATE_16000, .dMAX = UAC_SAM_RATE_16000, .dRES = 0},
            [2] = {.dMIN = UAC_SAM_RATE_32000, .dMAX = UAC_SAM_RATE_32000, .dRES = 0},

        },
    },
};
#endif

#if UAC_MIC_SUPPORT
static T_CTRL_ATTR_VOL vol_attr_mic =
{
    .cur =  {.wCUR = UAC2_MIC_VOL_CUR},
    .range =
    {
        .wNumSubRanges = 1,
        .ranges =
        {
            [0] = {.wMIN = UAC2_MIC_VOL_MIN, .wMAX = UAC2_MIC_VOL_MAX, .wRES = UAC2_MIC_VOL_RES}
        },
    },
};

static T_CTRL_ATTR_MUTE mute_attr_mic =
{
    .cur =  {.bCUR = 0},
};
#endif

#if UAC_SPK_SUPPORT
static int32_t vol_attr_set_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr, uint8_t cmd, int value)
{
    T_CTRL_ATTR_VOL *vol_attr = (T_CTRL_ATTR_VOL *)drv_attr->data;
    uint16_t vol = value - vol_attr->range.ranges[0].wMIN;
    T_USB_AUDIO_PIPE_ATTR ctrl_attr = {.dir = USB_AUDIO_DIR_OUT, .content.vol.value = vol};

    vol_attr->cur.wCUR = value;
    APP_PRINT_INFO2("vol_attr_set_spk:0x%x-0x%x", cmd, vol);
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_VOL_SET,
                             ctrl_attr);
    return 0;

}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *vol_attr_get_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr,
                                                      uint8_t cmd)
{
    T_CTRL_ATTR_VOL *vol_attr = (T_CTRL_ATTR_VOL *)drv_attr->data;

    T_USB_AUDIO_PIPE_ATTR ctrl_attr = {.dir = USB_AUDIO_DIR_OUT, .content.vol.type = USB_AUDIO_VOL_TYPE_INVALID};
    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(vol_attr->cur.wCUR);
        drv_ctrl_attr.data = &(vol_attr->cur.wCUR);
        ctrl_attr.content.vol.type = USB_AUDIO_VOL_TYPE_CUR;
        ctrl_attr.content.vol.value = vol_attr->cur.wCUR - vol_attr->range.ranges[0].wMIN;
    }
    else if (cmd == UAC2_RANGE)
    {
        drv_ctrl_attr.len = sizeof(vol_attr->range);
        drv_ctrl_attr.data = &(vol_attr->range);
        ctrl_attr.content.vol.type = USB_AUDIO_VOL_TYPE_RANGE;
        ctrl_attr.content.vol.value = vol_attr->range.ranges[0].wMAX - vol_attr->range.ranges[0].wMIN;
    }
    if (ctrl_attr.content.vol.type != USB_AUDIO_VOL_TYPE_INVALID)
    {
        USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_VOL_GET,
                                 ctrl_attr);
    }

    APP_PRINT_INFO4("vol_attr_get_spk:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len,
                    drv_ctrl_attr.data, vol_attr);

    return &drv_ctrl_attr;
}

static int32_t mute_attr_set_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr, uint8_t cmd, int value)
{
    uint8_t mute = value;
    T_CTRL_ATTR_MUTE *mute_attr = (T_CTRL_ATTR_MUTE *)drv_attr->data;
    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_OUT, .content.mute.value = mute};
    mute_attr->cur.bCUR = mute;
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_MUTE_SET, attr);
    APP_PRINT_INFO2("mute_attr_set_spk:0x%x-0x%x", cmd, mute);
    return 0;

}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *mute_attr_get_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_ctrl,
                                                       uint8_t cmd)
{

    T_CTRL_ATTR_MUTE *mute_attr = (T_CTRL_ATTR_MUTE *)(drv_ctrl->data);

    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(mute_attr->cur.bCUR);
        drv_ctrl_attr.data = &(mute_attr->cur.bCUR);
    }

    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_OUT, .content.mute.value = mute_attr->cur.bCUR};
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_MUTE_GET, attr);
    APP_PRINT_INFO4("mute_attr_get_spk:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len,
                    drv_ctrl_attr.data, mute_attr);

    return &drv_ctrl_attr;
}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *sam_freq_get_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl,
                                                      uint8_t cmd)
{
    T_CTRL_ATTR_FREQ_SPK *freq_attr = (T_CTRL_ATTR_FREQ_SPK *)(ctrl->data);

    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(freq_attr->cur.dCUR);
        drv_ctrl_attr.data = &(freq_attr->cur.dCUR);
        usb_audio_driver_freq_change(uac2_inst, USB_AUDIO_DIR_OUT, freq_attr->cur.dCUR);
    }
    else if (cmd == UAC2_RANGE)
    {
        drv_ctrl_attr.len = sizeof(freq_attr->range.wNumSubRanges)
                            + freq_attr->range.wNumSubRanges * sizeof(freq_attr->range.ranges[0]);
        drv_ctrl_attr.data = &(freq_attr->range);
    }
    APP_PRINT_INFO4("sam_freq_get_spk:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len, drv_ctrl_attr.data,
                    freq_attr);

    return &drv_ctrl_attr;
}

static int32_t sam_freq_set_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl, uint8_t cmd, int value)
{
    T_CTRL_ATTR_FREQ_SPK *freq_attr = (T_CTRL_ATTR_FREQ_SPK *)(ctrl->data);
    freq_attr->cur.dCUR = value;
    usb_audio_driver_freq_change(uac2_inst, USB_AUDIO_DIR_OUT, value);
    APP_PRINT_INFO2("sam_freq_set_spk:0x%x-0x%x-0x%x-0x%x", cmd, value);
    return 0;

}

static const T_USB_AUDIO_DRIVER_CTRL  freq_ctrl_spk =
{
    .type = UAC2_CS_SAM_FREQ_CONTROL,
    .attr = {.data = (void *) &freq_attr_spk, .len = sizeof(vol_attr_spk)},

    .get = (T_USB_AUDIO_CTRL_GET_FUNC)sam_freq_get_spk,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)sam_freq_set_spk,


};

static const T_USB_AUDIO_DRIVER_CTRL vol_ctrl_spk =
{
    .type = UAC2_FU_VOLUME_CONTROL,
    .attr = {.data = (void *) &vol_attr_spk, .len = sizeof(vol_attr_spk)},

    .get = (T_USB_AUDIO_CTRL_GET_FUNC)vol_attr_get_spk,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)vol_attr_set_spk,


};

static const T_USB_AUDIO_DRIVER_CTRL  mute_ctrl_spk =
{
    .type = UAC2_FU_MUTE_CONTROL,
    .attr = {.data = (void *) &mute_attr_spk, .len = sizeof(vol_attr_spk)},

    .get = (T_USB_AUDIO_CTRL_GET_FUNC)mute_attr_get_spk,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)mute_attr_set_spk,
};
#endif

#if UAC_MIC_SUPPORT
static int32_t vol_attr_set_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl, uint8_t cmd, int value)
{
    T_CTRL_ATTR_VOL *vol_attr = (T_CTRL_ATTR_VOL *)(ctrl->data);
    uint16_t vol = value - vol_attr->range.ranges[0].wMIN;
    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_IN, .content.vol.value = vol};

    vol_attr->cur.wCUR = value;
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_VOL_SET, attr);
    APP_PRINT_INFO2("vol_attr_set_mic:0x%x-0x%x", cmd, vol);
    return 0;

}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *vol_attr_get_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl,
                                                      uint8_t cmd)
{
    T_CTRL_ATTR_VOL *vol_attr = (T_CTRL_ATTR_VOL *)(ctrl->data);
    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_IN, .content.vol.type = USB_AUDIO_VOL_TYPE_INVALID};

    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(vol_attr->cur.wCUR);
        drv_ctrl_attr.data = &(vol_attr->cur.wCUR);
        attr.content.vol.type = USB_AUDIO_VOL_TYPE_CUR;
        attr.content.vol.value = vol_attr->cur.wCUR - vol_attr->range.ranges[0].wMIN;
    }
    else if (cmd == UAC2_RANGE)
    {
        drv_ctrl_attr.len = sizeof(vol_attr->range);
        drv_ctrl_attr.data = &(vol_attr->range);
        attr.content.vol.type = USB_AUDIO_VOL_TYPE_RANGE;
        attr.content.vol.value = vol_attr->range.ranges[0].wMAX - vol_attr->range.ranges[0].wMIN;
    }
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_VOL_GET, attr);
    APP_PRINT_INFO4("vol_attr_get_mic:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len,
                    drv_ctrl_attr.data, vol_attr);

    return &drv_ctrl_attr;
}

static int32_t mute_attr_set_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl, uint8_t cmd, int value)
{
    uint8_t mute = value;
    T_CTRL_ATTR_MUTE *mute_attr = (T_CTRL_ATTR_MUTE *)(ctrl->data);
    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_IN, .content.mute.value = mute};
    mute_attr->cur.bCUR = mute;
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_MUTE_SET, attr);
    APP_PRINT_INFO2("mute_attr_set_mic:0x%x-0x%x", cmd, mute);
    return 0;

}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *mute_attr_get_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl,
                                                       uint8_t cmd)
{

    T_CTRL_ATTR_MUTE *mute_attr = (T_CTRL_ATTR_MUTE *)(ctrl->data);

    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(mute_attr->cur.bCUR);
        drv_ctrl_attr.data = &(mute_attr->cur.bCUR);
    }

    T_USB_AUDIO_PIPE_ATTR attr = {.dir = USB_AUDIO_DIR_IN, .content.mute.value = mute_attr->cur.bCUR};
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, USB_AUDIO_CTRL_EVT_MUTE_GET, attr);
    APP_PRINT_INFO4("mute_attr_get_mic:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len,
                    drv_ctrl_attr.data, mute_attr);

    return &drv_ctrl_attr;
}

static T_USB_AUDIO_DRIVER_CTRL_ATTR *sam_freq_get_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl,
                                                      uint8_t cmd)
{
    T_CTRL_ATTR_FREQ_MIC *freq_attr = (T_CTRL_ATTR_FREQ_MIC *)(ctrl->data);

    if (cmd == UAC2_CUR)
    {
        drv_ctrl_attr.len = sizeof(freq_attr->cur.dCUR);
        drv_ctrl_attr.data = &(freq_attr->cur.dCUR);
        usb_audio_driver_freq_change(uac2_inst, USB_AUDIO_DIR_IN, freq_attr->cur.dCUR);
    }
    else if (cmd == UAC2_RANGE)
    {
        drv_ctrl_attr.len = sizeof(freq_attr->range.wNumSubRanges)
                            + freq_attr->range.wNumSubRanges * sizeof(freq_attr->range.ranges[0]);
        drv_ctrl_attr.data = &(freq_attr->range);
    }
    APP_PRINT_INFO4("sam_freq_get_mic:0x%x-0x%x-0x%x-0x%x", cmd, drv_ctrl_attr.len,
                    drv_ctrl_attr.data, freq_attr);

    return &drv_ctrl_attr;
}

static int32_t sam_freq_set_mic(T_USB_AUDIO_DRIVER_CTRL_ATTR *ctrl, uint8_t cmd, int value)
{
    T_CTRL_ATTR_FREQ_MIC *freq_attr = (T_CTRL_ATTR_FREQ_MIC *)(*(uint32_t *)ctrl);
    freq_attr->cur.dCUR = value;
    usb_audio_driver_freq_change(uac2_inst, USB_AUDIO_DIR_IN, value);
    APP_PRINT_INFO2("sam_freq_set_mic:0x%x-0x%x", cmd, value);
    return 0;

}



static const T_USB_AUDIO_DRIVER_CTRL  freq_ctrl_mic =
{
    .type = UAC2_CS_SAM_FREQ_CONTROL,
    .attr = {.data = (void *) &freq_attr_mic, .len = sizeof(vol_attr_spk)},

    .get = (T_USB_AUDIO_CTRL_GET_FUNC)sam_freq_get_mic,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)sam_freq_set_mic,

};

static const T_USB_AUDIO_DRIVER_CTRL vol_ctrl_mic =
{
    .type = UAC2_FU_VOLUME_CONTROL,
    .attr = {.data = (void *) &vol_attr_mic, .len = sizeof(vol_attr_spk)},
    .get = (T_USB_AUDIO_CTRL_GET_FUNC)vol_attr_get_mic,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)vol_attr_set_mic,

};

static const T_USB_AUDIO_DRIVER_CTRL  mute_ctrl_mic =
{
    .type = UAC2_FU_MUTE_CONTROL,
    .attr = {.data = (void *) &mute_attr_mic, .len = sizeof(vol_attr_spk)},

    .get = (T_USB_AUDIO_CTRL_GET_FUNC)mute_attr_get_mic,
    .set = (T_USB_AUDIO_CTRL_SET_FUNC)mute_attr_set_mic,
};

#endif

static const T_USB_AUDIO_DRIVER_CTRL_ENTITY(UAC2_ID_FEATURE_UNIT, 2) uac2_ctrl_entity0 =
{
    .entity_id = UAC2_ID_FEATURE_UNIT,
    .ctrl_num = 2,
    .ctrls[0] = (T_USB_AUDIO_DRIVER_CTRL *) &vol_ctrl_spk,
    .ctrls[1] = (T_USB_AUDIO_DRIVER_CTRL *) &mute_ctrl_spk,
};

static const T_USB_AUDIO_DRIVER_CTRL_ENTITY(UAC2_ID_CLOCK_SOURCE1, 1) uac2_ctrl_entity1 =
{
    .entity_id = UAC2_ID_CLOCK_SOURCE1,
    .ctrl_num = 1,
    .ctrls[0] = (T_USB_AUDIO_DRIVER_CTRL *) &freq_ctrl_spk,
};

static const T_USB_AUDIO_DRIVER_CTRL_ENTITY(UAC2_ID_FEATURE_UNIT2, 2) uac2_ctrl_entity2 =
{
    .entity_id = UAC2_ID_FEATURE_UNIT2,
    .ctrl_num = 2,
    .ctrls[0] = (T_USB_AUDIO_DRIVER_CTRL *) &vol_ctrl_mic,
    .ctrls[1] = (T_USB_AUDIO_DRIVER_CTRL *) &mute_ctrl_mic,
};

static const T_USB_AUDIO_DRIVER_CTRL_ENTITY(UAC2_ID_CLOCK_SOURCE2, 1) uac2_ctrl_entity3 =
{
    .entity_id = UAC2_ID_CLOCK_SOURCE2,
    .ctrl_num = 1,
    .ctrls[0] = (T_USB_AUDIO_DRIVER_CTRL *) &freq_ctrl_mic,
};

static const T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *ctrls[] =
{
    (const T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *) &uac2_ctrl_entity0,
    (const T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *) &uac2_ctrl_entity1,
    (const T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *) &uac2_ctrl_entity2,
    (const T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *) &uac2_ctrl_entity3,
    NULL
};

static int usb_audio_upstream(uint8_t *buf, uint16_t len)
{
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->upstream, usb_audio2_pipe, buf, len);
    return 0;
}

static int usb_audio_downstream(uint8_t *buf, uint16_t len)
{
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->downstream, usb_audio2_pipe, buf, len);
    return 0;
}

static int usb_audio_activate(uint8_t dir, uint8_t bit_res, uint32_t sample_rate, uint8_t chan_num)
{
    T_USB_AUDIO_CTRL_EVT evt = USB_AUDIO_CTRL_EVT_ACTIVATE;
    T_USB_AUDIO_PIPE_ATTR attr =
    {
        .dir = dir,
        .content.audio =
        {
            .bit_width = bit_res,
            .channels = chan_num,
            .sample_rate = sample_rate,
        },
    };
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, evt, attr);
    return 0;
}

static int usb_audio_deactivate(uint8_t dir)
{
    T_USB_AUDIO_CTRL_EVT evt = USB_AUDIO_CTRL_EVT_DEACTIVATE;
    T_USB_AUDIO_PIPE_ATTR attr = {.dir = dir, .content.d32 = 0};
    USB_AUDIO2_PIPE_CALLBACK(usb_audio2_pipe->ctrl, usb_audio2_pipe, evt, attr);
    return 0;
}

static void usb_audio_adp_state_change_cb(T_ADP_PLUG_EVENT event, void *user_data)
{
    if (event == ADP_EVENT_PLUG_OUT)
    {
#if UAC_SPK_SUPPORT
        freq_attr_spk.cur.dCUR = UAC_SAM_RATE_48000;
        vol_attr_spk.cur.wCUR = UAC2_SPK_VOL_CUR;
        mute_attr_spk.cur.bCUR = 0;
#endif
#if UAC_MIC_SUPPORT
        freq_attr_mic.cur.dCUR = UAC_SAM_RATE_32000;
        vol_attr_mic.cur.wCUR = UAC2_MIC_VOL_CUR;
        mute_attr_mic.cur.bCUR = 0;
#endif
    }
}

void usb_audio_init(T_USB_AUDIO_PIPES *pipe)
{
    usb_audio2_pipe = pipe;

    uac2_inst = usb_audio_driver_inst_alloc(USB_AUDIO_VERSION_2, 4, 4);
    usb_audio_driver_desc_register(uac2_inst, (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_ctrl,
                                   (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_ctrl);
    usb_audio_driver_desc_register(uac2_inst, (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_spk_fs,
                                   (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_spk_hs);
    usb_audio_driver_desc_register(uac2_inst, (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_mic_fs,
                                   (T_USB_AUDIO_DRIVER_DESC_HDR **)uac2_descs_mic_hs);
    usb_audio_driver_ctrl_register(uac2_inst, (T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR **)ctrls);
    T_USB_AUDIO_DRIVER_ATTR attr;
#if UAC_SPK_SUPPORT
    attr.dir = USB_AUDIO_DIR_OUT;
    attr.chann_num = 0;
    attr.bit_width = 0;
    attr.cur_sample_rate = 0;
    attr.max_sample_rate = 0;
    usb_audio_driver_attr_init(uac2_inst, 0, attr);

    attr.dir = USB_AUDIO_DIR_OUT;
    attr.chann_num = UAC2_SPK_CHAN_NUM;
    attr.bit_width = UAC2_SPK_BIT_RES;
    attr.cur_sample_rate = 48000;
    attr.max_sample_rate = UAC2_SPK_SAM_RATE_MAX;
    usb_audio_driver_attr_init(uac2_inst, 1, attr);
#endif
#if UAC_MIC_SUPPORT
    attr.dir = USB_AUDIO_DIR_IN;
    attr.chann_num = 0;
    attr.bit_width = 0;
    attr.cur_sample_rate = 0;
    attr.max_sample_rate = 0;
    usb_audio_driver_attr_init(uac2_inst, 0, attr);

    attr.dir = USB_AUDIO_DIR_IN;
    attr.chann_num = UAC2_MIC_CHAN_NUM;
    attr.bit_width = UAC2_MIC_BIT_RES;
    attr.cur_sample_rate = 32000;
    attr.max_sample_rate = UAC2_MIC_SAM_RATE_MAX;
    usb_audio_driver_attr_init(uac2_inst, 1, attr);
#endif

    T_USB_AUDIO_DRIVER_CBS cbs;
    cbs.activate = (USB_AUDIO_DRV_CB_ACTIVATE)usb_audio_activate;
    cbs.deactivate = (USB_AUDIO_DRV_CB_DEACTIVATE)usb_audio_deactivate;
    cbs.upstream = usb_audio_upstream;
    cbs.downstream = usb_audio_downstream;
    cbs.feedback_d = NULL;
    usb_audio_driver_cb_register(uac2_inst, &cbs);

    extern int usb_audio_driver_init(void);
    usb_audio_driver_init();

    adp_register_state_change_cb(ADP_DETECT_5V, (P_ADP_PLUG_CBACK)usb_audio_adp_state_change_cb, NULL);
}

#endif
#endif
