/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file uac1.h
 * @version 1.0
 * @brief uac 1.0 related definitions
 *
 * @note:
 */
#ifndef __UAC1_H__
#define __UAC1_H__
#include <stdint.h>
#include "usb_spec20.h"
#include "uac_common.h"

#define UAC1_CLASS_CODE_AUDIO           0x01

#define UAC1_SUBCLASS_UNDEFINED          (UAC_SUBCLASS_UNDEFINED)
#define UAC1_SUBCLASS_AUDIOCONTROL       (UAC_SUBCLASS_AUDIOCONTROL)
#define UAC1_SUBCLASS_AUDIOSTREAMING     (UAC_SUBCLASS_AUDIOSTREAMING)
#define UAC1_SUBCLASS_MIDISTREAMING      (UAC_SUBCLASS_MIDISTREAMING)

#define UAC1_DT_CS_UNDEFINE                (UAC_DT_CS_UNDEFINE)
#define UAC1_DT_CS_DEVICE                  (UAC_DT_CS_DEVICE)
#define UAC1_DT_CS_CONFIGURATION           (UAC_DT_CS_CONFIGURATION)
#define UAC1_DT_CS_STRING                  (UAC_DT_CS_STRING)
#define UAC1_DT_CS_INTERFACE               (UAC_DT_CS_INTERFACE)
#define UAC1_DT_CS_ENDPOINT                (UAC_DT_CS_ENDPOINT)

#define UAC1_DT_DEVICE                  USB_DESC_TYPE_DEVICE
#define UAC1_DT_CONFIG                  USB_DESC_TYPE_CONFIG
#define UAC1_DT_STRING                  USB_DESC_TYPE_STRING
#define UAC1_DT_INTERFACE               USB_DESC_TYPE_INTERFACE
#define UAC1_DT_ENDPOINT                USB_DESC_TYPE_ENDPOINT
#define UAC1_DT_DEVICE_QUALIFIER        USB_DESC_TYPE_DEVICE_QUALIFIER
#define UAC1_DT_OTHER_SPEED_CONFIG      USB_DESC_TYPE_OTHER_SPEED_CONFIG
#define UAC1_DT_INTERFACE_POWER         USB_DT_INTERFACE_POWER
#define UAC1_DT_OTG                     USB_DT_OTG
#define UAC1_DT_DEBUG                   USB_DT_DEBUG
#define UAC1_DT_INTERFACE_ASSOCIATION   USB_DESC_TYPE_IAD

#define UAC1_AC_DESC_UNDEFINED          0x00
#define UAC1_HEADER                     0x01
#define UAC1_INPUT_TERMINAL             0x02
#define UAC1_OUTPUT_TERMINAL            0x03
#define UAC1_MIXER_UNIT                 0x04
#define UAC1_SELECTOR_UNIT              0x05
#define UAC1_FEATURE_UNIT               0x06
#define UAC1_EFFECT_UNIT                0x07
#define UAC1_PROCESSING_UNIT            0x08
#define UAC1_EXTENSION_UNIT             0x09
#define UAC1_CLOCK_SOURCE               0x0A
#define UAC1_CLOCK_SELECTOR             0x0B
#define UAC1_CLOCK_MULTIPLIER           0x0C
#define UAC1_SAM_RATE_CONVERTER         0x0D

#define UAC1_FU_CONTROL_UNDEFINED         0x00
#define UAC1_FU_MUTE_CONTROL              0x01
#define UAC1_FU_VOLUME_CONTROL            0x02
#define UAC1_FU_BASS_CONTROL              0x03
#define UAC1_FU_MID_CONTROL               0x04
#define UAC1_FU_TREBLE_CONTROL            0x05
#define UAC1_FU_GRAPHIC_EQUALIZER_CONTROL 0x06
#define UAC1_FU_AUTOMATIC_GAIN_CONTROL    0x07
#define UAC1_FU_DELAY_CONTROL             0x08
#define UAC1_FU_BASS_BOOST_CONTROL        0x09
#define UAC1_FU_LOUDNESS_CONTROL          0x0A

#define UAC1_TERMINAL_UNDEFINED          0x100
#define UAC1_TERMINAL_STREAMING          0x101
#define UAC1_TERMINAL_VENDOR_SPEC        0x1FF

#define UAC1_INPUT_TERMINAL_UNDEFINED                    0x200
#define UAC1_INPUT_TERMINAL_MICROPHONE                   0x201
#define UAC1_INPUT_TERMINAL_DESKTOP_MICROPHONE           0x202
#define UAC1_INPUT_TERMINAL_PERSONAL_MICROPHONE          0x203
#define UAC1_INPUT_TERMINAL_OMNI_DIR_MICROPHONE          0x204
#define UAC1_INPUT_TERMINAL_MICROPHONE_ARRAY             0x205
#define UAC1_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY        0x206

#define UAC1_OUTPUT_TERMINAL_UNDEFINED                   0x300
#define UAC1_OUTPUT_TERMINAL_SPEAKER                     0x301
#define UAC1_OUTPUT_TERMINAL_HEADPHONES                  0x302
#define UAC1_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO  0x303
#define UAC1_OUTPUT_TERMINAL_DESKTOP_SPEAKER             0x304
#define UAC1_OUTPUT_TERMINAL_ROOM_SPEAKER                0x305
#define UAC1_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER       0x306
#define UAC1_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER    0x307

/* Terminals - 2.4 Bi-directional Terminal Types */
#define UAC1_BI_DIRECTIONAL_TERMINAL_UNDEFINED                       0x400
#define UAC1_BI_DIRECTIONAL_TERMINAL_HANDSE                          0x401
#define UAC1_BI_DIRECTIONAL_TERMINAL_HEADSET                         0x402
#define UAC1_BI_DIRECTIONAL_TERMINAL_SPEAKERPHONE                    0x403
#define UAC1_BI_DIRECTIONAL_TERMINAL_SPEAKERPHONE_ECOS               0x404
#define UAC1_BI_DIRECTIONAL_TERMINAL_SPEAKERPHONE_ECOC               0x405

#define UAC1_AS_DESC_UNDEFINED           0x00
#define UAC1_AS_GENERAL                  0x01
#define UAC1_FORMAT_TYPE                 0x02

#define UAC1_FORMAT_TYPE_I_PCM           0x01

#define UAC1_DIR_OUT                     EP_DIR_OUT
#define UAC1_DIR_IN                      EP_DIR_IN

#define UAC1_EP_XFER_CONTROL            USB_EP_TYPE_CONTROL
#define UAC1_EP_XFER_ISOC               USB_EP_TYPE_ISOC
#define UAC1_EP_XFER_BULK               USB_EP_TYPE_BULK
#define UAC1_EP_XFER_INT                USB_EP_TYPE_INT

#define UAC1_EP_SYNCTYPE                USB_EP_SYNCTYPE
#define UAC1_EP_SYNC_NONE               USB_EP_SYNC_NONE
#define UAC1_EP_SYNC_ASYNC              USB_EP_SYNC_ASYNC
#define UAC1_EP_SYNC_ADAPTIVE           USB_EP_SYNC_ADAPTIVE
#define UAC1_EP_SYNC_SYNC               USB_EP_SYNC_SYNC

#define UAC1_EP_GENERAL                 0x01

#define UAC1_ISO_OUT_ENDPOINT_ADDRESS    0x03
#define UAC1_ISO_IN_ENDPOINT_ADDRESS     0x83

#define UAC1_ID_FEATURE_UNIT1                 0x22
#define UAC1_ID_INPUT_TERMINAL1               0x23
#define UAC1_ID_OUTPUT_TERMINAL1              0x24


#define UAC1_ID_INPUT_TERMINAL2               0x26
#define UAC1_ID_OUTPUT_TERMINAL2              0x27
#define UAC1_ID_FEATURE_UNIT2                 0x28

#define UAC_SET_                        0x00
#define UAC_GET_                        0x80

#define UAC1_CUR                        0x1
#define UAC1_MIN                        0x2
#define UAC1_MAX                        0x3
#define UAC1_RES                        0x4
#define UAC1_MEM                        0x5
#define UAC1_SET_CUR                     (UAC_SET_ | UAC1_CUR)
#define UAC1_GET_CUR                     (UAC_GET_ | UAC1_CUR)
#define UAC1_SET_MIN                     (UAC_SET_ | UAC1_MIN)
#define UAC1_GET_MIN                     (UAC_GET_ | UAC1_MIN)
#define UAC1_SET_MAX                     (UAC_SET_ | UAC1_MAX)
#define UAC1_GET_MAX                     (UAC_GET_ | UAC1_MAX)
#define UAC1_SET_RES                     (UAC_SET_ | UAC1_RES)
#define UAC1_GET_RES                     (UAC_GET_ | UAC1_RES)
#define UAC1_SET_MEM                     (UAC_SET_ | UAC1_MEM)
#define UAC1_GET_MEM                     (UAC_GET_ | UAC1_MEM)

#define UAC_CONTROL_BIT(CS)     (1 << ((CS) - 1))

#pragma pack(push,1)


#define T_UAC1_AC_HDR_DESC(id, as_if_num)                          \
    struct _uac1_ac_header_descriptor_##id##_as_if_num              \
    {                                                               \
        uint8_t  bLength;                                           \
        uint8_t  bDescriptorType;                                   \
        uint8_t  bDescriptorSubtype;                                \
        uint16_t bcdADC;                                            \
        uint16_t wTotalLength;                                      \
        uint8_t  bInCollection;                                     \
        uint8_t  baInterfaceNr[as_if_num];                          \
    }__attribute__((packed))

#define T_UAC1_SELECTOR_UINT_DESC(id, src_num)                        \
    struct _uac1_selector_unit_descriptor_##id##_src_num               \
    {                                                                 \
        uint8_t bLength;                                              \
        uint8_t bDescriptorType;                                      \
        uint8_t bDescriptorSubtype;                                   \
        uint8_t bUnitID;                                              \
        uint8_t bNrInPins;                                            \
        uint8_t baSourceID[src_num];                                  \
        uint8_t iSelector;                                            \
    }__attribute__((packed))

#define T_UAC1_FEATURE_UNIT_DESC(id, chann_num)                        \
    struct _uac1_feature_unit_descriptor_##id##_chann_num               \
    {                                                                 \
        uint8_t bLength;                                              \
        uint8_t bDescriptorType;                                      \
        uint8_t bDescriptorSubtype;                                   \
        uint8_t bUnitID;                                              \
        uint8_t bSourceID;                                            \
        uint8_t bControlSize;                                         \
        uint8_t bmaControls[chann_num];                               \
        uint8_t iFeature;                                             \
    }__attribute__((packed))

typedef struct _uac1_input_terminal_descriptor
{
    uint8_t  bLength;                  /* in bytes: 12 */
    uint8_t  bDescriptorType;          /* CS_INTERFACE descriptor type */
    uint8_t  bDescriptorSubtype;       /* INPUT_TERMINAL descriptor subtype */
    uint8_t  bTerminalID;              /* Constant uniquely terminal ID */
    uint16_t wTerminalType;           /* USB Audio Terminal Types */
    uint8_t  bAssocTerminal;           /* ID of the Output Terminal associated */
    uint8_t  bNrChannels;              /* Number of logical output channels */
    uint16_t wChannelConfig;
    uint8_t  iChannelNames;
    uint8_t  iTerminal;
} T_UAC1_IT_DESC;

typedef struct _uac1_output_terminal_descriptor
{
    uint8_t  bLength;                  /* in bytes: 9 */
    uint8_t  bDescriptorType;          /* CS_INTERFACE descriptor type */
    uint8_t  bDescriptorSubtype;       /* OUTPUT_TERMINAL descriptor subtype */
    uint8_t  bTerminalID;              /* Constant uniquely terminal ID */
    uint16_t wTerminalType;           /* USB Audio Terminal Types */
    uint8_t  bAssocTerminal;           /* ID of the Input Terminal associated */
    uint8_t  bSourceID;                /* ID of the connected Unit or Terminal*/
    uint8_t  iTerminal;
} T_UAC1_OT_DESC;

typedef struct _uac1_as_header_descriptor
{
    uint8_t  bLength;                  /* in bytes: 7 */
    uint8_t  bDescriptorType;          /* USB_DT_CS_INTERFACE */
    uint8_t  bDescriptorSubtype;       /* AS_GENERAL */
    uint8_t  bTerminalLink;            /* Terminal ID of connected Terminal */
    uint8_t  bDelay;                   /* Delay introduced by the data path */
    uint16_t wFormatTag;              /* The Audio Data Format */
} T_UAC1_AS_HDR_DESC;

#define T_UAC1_FMT_TYPE_I_DESC(dir, sample_freq_num)                        \
    struct _uac1_format_type_i_discrete_descriptor_##dir##sample_freq_num       \
    {                                                                       \
        uint8_t  bLength;                  /* in bytes: 8 + (ns * 3) */     \
        uint8_t  bDescriptorType;          /* USB_DT_CS_INTERFACE */        \
        uint8_t  bDescriptorSubtype;       /* FORMAT_TYPE */                \
        uint8_t  bFormatType;              /* FORMAT_TYPE_1 */              \
        uint8_t  bNrChannels;              /* physical channels in the stream */\
        uint8_t  bSubframeSize;            /* */                            \
        uint8_t  bBitResolution;                                            \
        uint8_t  bSamFreqType;                                              \
        uint8_t  tSamFreq[sample_freq_num][3];                              \
    }__attribute__((packed))

typedef struct _uac1_iso_endpoint_descriptor
{
    uint8_t  bLength;                  /* in bytes: 7 */
    uint8_t  bDescriptorType;          /* USB_DT_CS_ENDPOINT */
    uint8_t  bDescriptorSubtype;       /* EP_GENERAL */
    uint8_t  bmAttributes;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} T_UAC1_ISO_EP_DESC;

typedef struct _uac1_std_iso_endpoint_descriptor
{
    uint8_t       bLength;
    uint8_t       bDescriptorType;
    uint8_t       bEndpointAddress;
    uint8_t       bmAttributes;
    uint16_t      wMaxPacketSize;
    uint8_t       bInterval;
    uint8_t       bRefresh;
    uint8_t       bSynchAddress;
} T_UAC1_STD_ISO_EP_DESC;

#define T_UAC1_STD_IA_DESC      T_USB_INTERFACE_ASSOC_DESC
#define T_UAC1_STD_IF_DESC      T_USB_INTERFACE_DESC
#define T_UAC1_STD_EP_DESC      T_USB_ENDPOINT_DESC
#pragma pack(pop)

typedef int32_t (*UAC_KITS_FUNC_INT)();

typedef union ctrl_attr
{
    int32_t    data[5];
    struct
    {
        int32_t    cur;
        int32_t    min;
        int32_t    max;
        int32_t    res;
        int32_t    mem;
    } attr;
} T_CTRL_ATTR;

#endif

