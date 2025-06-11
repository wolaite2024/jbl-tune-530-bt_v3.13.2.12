/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_TYPE_H_
#define _AUDIO_TYPE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_TYPE Audio Type
 *
 * \brief   Define basic and common types for Audio subsystem.
 */


/**
 * audio_type.h
 *
 * \brief Define Audio supported stream types.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_stream_type
{
    AUDIO_STREAM_TYPE_PLAYBACK  = 0x00, /**< Audio stream for audio playback */
    AUDIO_STREAM_TYPE_VOICE     = 0x01, /**< Audio stream for voice communication */
    AUDIO_STREAM_TYPE_RECORD    = 0x02, /**< Audio stream for recognition or capture */
} T_AUDIO_STREAM_TYPE;

/**
 * audio_type.h
 *
 * \brief Define Audio stream modes.
 *
 * \details Each Audio stream type can set its stream performance mode.
 *          \ref AUDIO_STREAM_MODE_NORMAL is the default mode that balances its latency with
 *          its stability.
 *          \ref AUDIO_STREAM_MODE_LOW_LATENCY is the mode that focuses on the latency performance,
 *          and has a lower internal buffer water-level, with a tradeoff of lower stability.
 *          \ref AUDIO_STREAM_MODE_HIGH_STABILITY is the mode that focuses on the stability
 *          performance, and has a deeper internal buffer water-level and a better underrun
 *          resistance, with a tradeoff of higher latency.
 *          \ref AUDIO_STREAM_MODE_DIRECT is the mode that bypasses the buffer management
 *          procedure, and then fills data into or drains data from the low level hardware.
 *          \ref AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY is the mode that derives from \ref
 *          AUDIO_STREAM_MODE_LOW_LATENCY, but has an extreme low internal buffer water-level,
 *          and thus ignores any performance degradations.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_stream_mode
{
    AUDIO_STREAM_MODE_NORMAL            = 0x00, /**< Audio stream normal mode */
    AUDIO_STREAM_MODE_LOW_LATENCY       = 0x01, /**< Audio stream low-latency mode */
    AUDIO_STREAM_MODE_HIGH_STABILITY    = 0x02, /**< Audio stream high-stability mode */
    AUDIO_STREAM_MODE_DIRECT            = 0x03, /**< Audio stream direct mode */
    AUDIO_STREAM_MODE_ULTRA_LOW_LATENCY = 0x04, /**< Audio stream ultra low-latency mode */
} T_AUDIO_STREAM_MODE;

/**
 * audio_type.h
 *
 * \brief Define Audio stream usages.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_stream_usage
{
    AUDIO_STREAM_USAGE_LOCAL    = 0x00, /**< Audio stream local usage */
    AUDIO_STREAM_USAGE_RELAY    = 0x01, /**< Audio stream remote relay usage */
    AUDIO_STREAM_USAGE_SNOOP    = 0x02, /**< Audio stream remote snoop usage */
} T_AUDIO_STREAM_USAGE;

/**
 * audio_type.h
 *
 * \brief Define Audio stream status.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_stream_status
{
    AUDIO_STREAM_STATUS_CORRECT = 0x00, /**< The whole payload data are marked as "good data". */
    AUDIO_STREAM_STATUS_ERROR   = 0x01, /**< The whole or partial payload data are marked as "wrong data". */
    AUDIO_STREAM_STATUS_LOST    = 0x02, /**< The payload data are marked as "empty data" in zero length. */
    AUDIO_STREAM_STATUS_DUMMY   = 0x03, /**< The payload data are marked as "default data". */
} T_AUDIO_STREAM_STATUS;

/**
 * audio_type.h
 *
 * \brief Define Audio category.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_category
{
    AUDIO_CATEGORY_AUDIO        = 0x00,
    AUDIO_CATEGORY_VOICE        = 0x01,
    AUDIO_CATEGORY_RECORD       = 0x02,
    AUDIO_CATEGORY_ANALOG       = 0x03,
    AUDIO_CATEGORY_TONE         = 0x04,
    AUDIO_CATEGORY_VP           = 0x05,
    AUDIO_CATEGORY_APT          = 0x06,
    AUDIO_CATEGORY_LLAPT        = 0x07,
    AUDIO_CATEGORY_ANC          = 0x08,
    AUDIO_CATEGORY_VAD          = 0x09,
    AUDIO_CATEGORY_NUMBER,
} T_AUDIO_CATEGORY;

/**
 * audio_type.h
 *
 * \defgroup AUDIO_DEVICE_BITMASK Audio Device Bitmask
 *
 * \brief Define the bitmask of Audio out/sink and in/source devices.
 *
 * \details The low 16 bits represent audio out devices, and high 16 bits represent audio in devices.
 *
 * \note If the audio stream chooses \ref AUDIO_DEVICE_OUT_DEFAULT or \ref AUDIO_DEVICE_IN_DEFAULT,
 *       it will be dynamically routed to the most relevant hardware device in different audio scenarios.
 *
 * \ingroup AUDIO_TYPE
 * @{
 */
#define AUDIO_DEVICE_OUT_DEFAULT        0x0000FFFF /**< Audio default out device */
#define AUDIO_DEVICE_OUT_SPK            0x00000001 /**< Speaker for A2DP, SCO or local playback */
#define AUDIO_DEVICE_OUT_USB            0x00000002 /**< USB audio out */
#define AUDIO_DEVICE_OUT_AUX            0x00000004 /**< Digital SPDIF/HDMI output */
#define AUDIO_DEVICE_IN_DEFAULT         0xFFFF0000 /**< Audio default in device */
#define AUDIO_DEVICE_IN_MIC             0x00010000 /**< Microphone for SCO, voice recognition, or record */
#define AUDIO_DEVICE_IN_USB             0x00020000 /**< USB audio in */
#define AUDIO_DEVICE_IN_AUX             0x00040000 /**< Analog line-in input */
/**
 * @}
 */

/**
 * audio_type.h
 *
 * \defgroup AUDIO_CHANNEL_LOCATION Audio Channel Location Bitmap
 *
 * \brief Define the 4-octet bitmap of Audio channel location.
 *
 * \details A media packet shall contain encoded codec frames for the Audio Channels, ordered from the
 *          lowest to highest Audio Location bit indices present in the bitmap value.
 *
 * \note    An Audio Channel Location bitmap value of all zeros that refers to Mono audio, or a single
 *          channel of no specified location.
 *
 * \ingroup AUDIO_TYPE
 * @{
 */
#define AUDIO_CHANNEL_LOCATION_MONO     0x00000000  /**< Audio mono or no specified channel */
#define AUDIO_CHANNEL_LOCATION_FL       0x00000001  /**< Audio front left channel */
#define AUDIO_CHANNEL_LOCATION_FR       0x00000002  /**< Audio front right channel */
#define AUDIO_CHANNEL_LOCATION_FC       0x00000004  /**< Audio front center channel */
#define AUDIO_CHANNEL_LOCATION_LFE1     0x00000008  /**< Audio low frequency effects 1 channel */
#define AUDIO_CHANNEL_LOCATION_BL       0x00000010  /**< Audio back left channel */
#define AUDIO_CHANNEL_LOCATION_BR       0x00000020  /**< Audio back right channel */
#define AUDIO_CHANNEL_LOCATION_FLC      0x00000040  /**< Audio front left of center channel */
#define AUDIO_CHANNEL_LOCATION_FRC      0x00000080  /**< Audio front right of center channel */
#define AUDIO_CHANNEL_LOCATION_BC       0x00000100  /**< Audio back center channel */
#define AUDIO_CHANNEL_LOCATION_LFE2     0x00000200  /**< Audio low frequency effects 2 channel */
#define AUDIO_CHANNEL_LOCATION_SL       0x00000400  /**< Audio side left channel */
#define AUDIO_CHANNEL_LOCATION_SR       0x00000800  /**< Audio side right channel */
#define AUDIO_CHANNEL_LOCATION_TFL      0x00001000  /**< Audio top front left channel */
#define AUDIO_CHANNEL_LOCATION_TFR      0x00002000  /**< Audio top front right channel */
#define AUDIO_CHANNEL_LOCATION_TFC      0x00004000  /**< Audio top front center channel */
#define AUDIO_CHANNEL_LOCATION_TC       0x00008000  /**< Audio top center channel */
#define AUDIO_CHANNEL_LOCATION_TBL      0x00010000  /**< Audio top back left channel */
#define AUDIO_CHANNEL_LOCATION_TBR      0x00020000  /**< Audio top back right channel */
#define AUDIO_CHANNEL_LOCATION_TSL      0x00040000  /**< Audio top side left channel */
#define AUDIO_CHANNEL_LOCATION_TSR      0x00080000  /**< Audio top side right channel */
#define AUDIO_CHANNEL_LOCATION_TBC      0x00100000  /**< Audio top back center channel */
#define AUDIO_CHANNEL_LOCATION_BFC      0x00200000  /**< Audio bottom front center channel */
#define AUDIO_CHANNEL_LOCATION_BFL      0x00400000  /**< Audio bottom front left channel */
#define AUDIO_CHANNEL_LOCATION_BFR      0x00800000  /**< Audio bottom front right channel */
#define AUDIO_CHANNEL_LOCATION_FLW      0x01000000  /**< Audio front left wide channel */
#define AUDIO_CHANNEL_LOCATION_FRW      0x02000000  /**< Audio front right wide channel */
#define AUDIO_CHANNEL_LOCATION_LS       0x04000000  /**< Audio left surround channel */
#define AUDIO_CHANNEL_LOCATION_RS       0x08000000  /**< Audio right surround channel */
#define AUDIO_CHANNEL_LOCATION_RFU1     0x10000000  /**< Audio reserved 1 channel */
#define AUDIO_CHANNEL_LOCATION_RFU2     0x20000000  /**< Audio reserved 2 channel */
#define AUDIO_CHANNEL_LOCATION_RFU3     0x40000000  /**< Audio reserved 3 channel */
#define AUDIO_CHANNEL_LOCATION_RFU4     0x80000000  /**< Audio reserved 4 channel */
/**
 * @}
 */

/**
 * audio_type.h
 *
 * \brief Define Audio supported encoding/decoding format types.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_format_type
{
    AUDIO_FORMAT_TYPE_PCM       = 0x00, /**< Audio PCM format type, see \ref T_AUDIO_PCM_ATTR */
    AUDIO_FORMAT_TYPE_CVSD      = 0x01, /**< Audio CVSD format type, see \ref T_AUDIO_CVSD_ATTR */
    AUDIO_FORMAT_TYPE_MSBC      = 0x02, /**< Audio mSBC format type, see \ref T_AUDIO_MSBC_ATTR */
    AUDIO_FORMAT_TYPE_SBC       = 0x03, /**< Audio SBC format type, see \ref T_AUDIO_SBC_ATTR */
    AUDIO_FORMAT_TYPE_AAC       = 0x04, /**< Audio AAC format type, see \ref T_AUDIO_AAC_ATTR */
    AUDIO_FORMAT_TYPE_OPUS      = 0x05, /**< Audio OPUS format type, see \ref T_AUDIO_OPUS_ATTR */
    AUDIO_FORMAT_TYPE_FLAC      = 0x06, /**< Audio FLAC format type, see \ref T_AUDIO_FLAC_ATTR */
    AUDIO_FORMAT_TYPE_MP3       = 0x07, /**< Audio MP3 format type, see \ref T_AUDIO_MP3_ATTR */
    AUDIO_FORMAT_TYPE_LC3       = 0x08, /**< Audio LC3 format type, see \ref T_AUDIO_LC3_ATTR */
    AUDIO_FORMAT_TYPE_LDAC      = 0x09, /**< Audio LDAC format type, see \ref T_AUDIO_LDAC_ATTR */
} T_AUDIO_FORMAT_TYPE;

/**
 * audio_type.h
 *
 * \brief Define Audio PCM format attributes for \ref AUDIO_FORMAT_TYPE_PCM.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_pcm_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 8000, 16000, 44100, 48000 and 96000 in HZ.
     */
    uint32_t            sample_rate;

    /**
     * Frame length in octets for encoding or decoding.
     */
    uint16_t            frame_length;

    /**
     * Number of channels for encoding or decoding.
     * The supported numbers are 1 (Mono) and 2 (Stereo).
     */
    uint8_t             chann_num;

    /**
     * Bits per sample for encoding or decoding.
     * The supported numbers are 16 and 24.
     */
    uint8_t             bit_width;
} T_AUDIO_PCM_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio CVSD frame duration for \ref T_AUDIO_CVSD_ATTR.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum
{
    AUDIO_CVSD_FRAME_DURATION_3_75_MS     = 0x00,
    AUDIO_CVSD_FRAME_DURATION_7_5_MS      = 0x01,
} T_AUDIO_CVSD_FRAME_DURATION;

/**
 * audio_type.h
 *
 * \brief Define Audio CVSD format attributes for \ref AUDIO_FORMAT_TYPE_CVSD.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_cvsd_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported value is fixed at 8000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Number of channels for encoding or decoding.
     * The supported numbers are 1 (Mono).
     */
    uint8_t                     chann_num;

    /**
     * Frame duration for encoding.
     * The supported frame durations are defined in \ref T_AUDIO_CVSD_FRAME_DURATION.
     */
    T_AUDIO_CVSD_FRAME_DURATION frame_duration;
} T_AUDIO_CVSD_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio SBC channel modes.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_sbc_channel_mode
{
    AUDIO_SBC_CHANNEL_MODE_MONO         = 0x00, /**< SBC mono channel mode */
    AUDIO_SBC_CHANNEL_MODE_DUAL         = 0x01, /**< SBC dual channel mode */
    AUDIO_SBC_CHANNEL_MODE_STEREO       = 0x02, /**< SBC stereo channel mode */
    AUDIO_SBC_CHANNEL_MODE_JOINT_STEREO = 0x03, /**< SBC joint stereo channel mode */
} T_AUDIO_SBC_CHANNEL_MODE;

/**
 * audio_type.h
 *
 * \brief Define Audio SBC format attributes for \ref AUDIO_FORMAT_TYPE_SBC.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_sbc_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 16000, 32000, 44100 and 48000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Channel mode for encoding or decoding.
     * The supported values are defined in \ref T_AUDIO_SBC_CHANNEL_MODE.
     */
    T_AUDIO_SBC_CHANNEL_MODE    chann_mode;

    /**
     * Block length for encoding or decoding.
     * The supported values are 4, 8, 12 and 16.
     */
    uint8_t                     block_length;

    /**
     * Number of subbands for encoding or decoding.
     * The supported values are 4 and 8.
     */
    uint8_t                     subband_num;

    /**
     * Allocation method for encoding or decoding.
     * The supported methods are 0 for Loudness, and 1 for SNR.
     */
    uint8_t                     allocation_method;

    /**
     * Bitpool value for encoding or decoding.
     * The supported ranges are from 2 to 250.
     */
    uint8_t                     bitpool;
} T_AUDIO_SBC_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio mSBC format attributes for \ref AUDIO_FORMAT_TYPE_MSBC.
 *
 * \details mSBC is a modified version of the SBC codec that is used for voice communication,
 * for example, Bluetooth HFP SCO; therefore all format attribute parameters of mSBC are fixed
 * and not negotiable.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_msbc_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported value is fixed at 16000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Channel mode for encoding or decoding.
     * The supported value is fixed at \ref AUDIO_SBC_CHANNEL_MODE_MONO
     * defined in \ref T_AUDIO_SBC_CHANNEL_MODE.
     */
    T_AUDIO_SBC_CHANNEL_MODE    chann_mode;

    /**
     * Block length for encoding or decoding.
     * The supported value is fixed at 15.
     */
    uint8_t                     block_length;

    /**
     * Number of subbands for encoding or decoding.
     * The supported value is fixed at 8.
     */
    uint8_t                     subband_num;

    /**
     * Allocation method for encoding or decoding.
     * The supported method is fixed at 0 for Loudness.
     */
    uint8_t                     allocation_method;

    /**
     * Bitpool value for encoding or decoding.
     * The supported value is fixed at 26.
     */
    uint8_t                     bitpool;
} T_AUDIO_MSBC_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio MPEG-4 AAC transport formats.
 *
 * \details There is no standard transport mechanism of the MPEG-4 elementary streams over a channel.
 *          This is because the broad range of applications that can make use of MPEG-4 technology
 *          have delivery requirements that are too wide to easily characterize with a single solution.
 *          Rather, what is standardized is an interface (the Delivery Multimedia Interface Format,
 *          or DMIF, specified in ISO/IEC 14496-6) that describes the capabilities of a transport layer
 *          and the communication between transport, multiplex, and demultiplex functions in encoders
 *          and decoders. The use of DMIF and the MPEG-4 Systems specification allows transmission
 *          functions that are much more sophisticated than are possible with previous MPEG standards.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_aac_transport_format
{
    /**
     * Multiplex format: MPEG-4 Multiplex scheme, defined in ISO/IEC 14496-1.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_M4MUX    = 0x00,

    /**
     * Multiplex format: Low Overhead Audio Transport Multiplex, defined in ISO/IEC 14496-3.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_LATM     = 0x01,

    /**
     * Storage format: Audio Data Interchange Format (AAC only), defined in ISO/IEC 14496-3.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_ADIF     = 0x02,

    /**
     * Storage format: MPEG-4 File Format, defined in ISO/IEC 14496-12.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_MP4FF    = 0x03,

    /**
     * Transmission format: Audio Data Transport Stream (AAC only), defined in ISO/IEC 14496-3.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_ADTS     = 0x04,

    /**
     * Transmission format: Low Overhead Audio Stream based on LATM, defined in ISO/IEC 14496-3.
     */
    AUDIO_AAC_TRANSPORT_FORMAT_LOAS     = 0x05,
} T_AUDIO_AAC_TRANSPORT_FORMAT;

/**
 * audio_type.h
 *
 * \brief Define Audio MPEG-4 AAC object types.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_aac_object_type
{
    /**
     * The NULL object provides the possibility to feed raw PCM data directly to the audio compositor.
     * No decoding is involved, However, an audio object descriptor is used to specify the sampling
     * rate and the audio channel configuration.
     */
    AUDIO_AAC_OBJECT_TYPE_NULL          = 0x00,

    /**
     * The AAC Main object is very similar to the AAC Main Profile that is defined in ISO/IEC 13818-7.
     * However, additionally the PNS tool is available. The AAC Main object type bitstream syntax is
     * compatible with the syntax defined in ISO/IEC 13818-7. All the MPEG-2 AAC multi-channel capabilities
     * are available. A decoder capable to decode a MPEG-4 main object stream can also parse and decode
     * a MPEG-2 AAC raw data stream. On the other hand, although a MPEG-2 AAC coder can parse an MPEG-4
     * AAC Main bitstream, decoding may fail, since PNS might have been used.
     */
    AUDIO_AAC_OBJECT_TYPE_MAIN          = 0x01,

    /**
     * The MPEG-4 AAC Low Complexity object type is the counterpart to the MPEG-2 AAC Low Complexity
     * Profile, with exactly the same restrictions as mentioned above for the AAC Main object type
     * \ref AUDIO_AAC_OBJECT_TYPE_MAIN.
     */
    AUDIO_AAC_OBJECT_TYPE_LC            = 0x02,

    /**
     * The MPEG-4 AAC Scalable Sampling Rate object type is the counterpart to the MPEG-2 AAC Scalable
     * Sampling Rate Profile, with exactly the same restrictions as mentioned above for the AAC Main
     * object type \ref AUDIO_AAC_OBJECT_TYPE_MAIN.
     */
    AUDIO_AAC_OBJECT_TYPE_SSR           = 0x03,

    /**
     * The MPEG-4 AAC Long Term Prediction object type is similar to the AAC Main object type
     * \ref AUDIO_AAC_OBJECT_TYPE_MAIN. However, a Long Term Predictor replaces the MPEG-2 AAC predictor.
     * The LTP achieves a similar coding gain, but requires significantly lower implementation complexity.
     * The bitstream syntax for this object type is very similar to the syntax defined in ISO/IEC 13818-7.
     * An MPEG-2 AAC LC profile bitstream can be decoded without restrictions using an MPEG-4 AAC LTP
     * object decoder.
     */
    AUDIO_AAC_OBJECT_TYPE_LTP           = 0x04,

    /**
     * The MPEG-4 AAC Reserved object.
     * \note Do not use this object type.
     */
    AUDIO_AAC_OBJECT_TYPE_RESERVED      = 0x05,

    /**
     * The MPEG-4 AAC Scalable object uses a different bitstream syntax to support bitrate and bandwidth
     * scalability. A large number of scalable combinations are available, including combinations with
     * TwinVQ and CELP coder tools. However, only mono, or 2-channel stereo objects are supported.
     */
    AUDIO_AAC_OBJECT_TYPE_SCALABLE      = 0x06,
} T_AUDIO_AAC_OBJECT_TYPE;

/**
 * audio_type.h
 *
 * \brief Define Audio AAC format attributes for \ref AUDIO_FORMAT_TYPE_AAC.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_aac_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100,
     * 48000, 64000, 88200 and 96000 in HZ.
     */
    uint32_t                        sample_rate;

    /**
     * Number of channels for encoding or decoding.
     * The supported numbers are 1 (Mono) and 2 (Stereo).
     */
    uint8_t                         chann_num;

    /**
     * Transport formats for encoding or decoding.
     * The supported values are defined in \ref T_AUDIO_AAC_TRANSPORT_FORMAT.
     */
    T_AUDIO_AAC_TRANSPORT_FORMAT    transport_format;

    /**
     * Object type for encoding or decoding.
     * The supported values are defined in \ref T_AUDIO_AAC_OBJECT_TYPE.
     */
    T_AUDIO_AAC_OBJECT_TYPE         object_type;

    /**
     * Variable bitrate (VBR) or constant bitrate (CBR) for encoding.
     * The supported values are 0 for CBR and 1 for VBR.
     */
    uint8_t                         vbr;

    /**
     * Bitrate for encoding.
     * The supported ranges are from 0 bit/s to 0x7FFFFF bit/s.
     * \note A value of 0 indicates that the bitrate is not known.
     */
    uint32_t                        bitrate;
} T_AUDIO_AAC_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio OPUS application for \ref T_AUDIO_OPUS_ATTR.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_opus_application
{
    OPUS_APPLICATION_VOIP                 = 0x01,
    OPUS_APPLICATION_AUDIO                = 0x02,
    OPUS_APPLICATION_RESTRICTED_LOWDELAY  = 0x03,
} T_AUDIO_OPUS_APPLICATION;

/**
 * audio_type.h
 *
 * \brief Define Audio OPUS frame duration for \ref T_AUDIO_OPUS_ATTR.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_opus_frame_duration
{
    AUDIO_OPUS_FRAME_DURATION_2_5_MS    = 0x00,
    AUDIO_OPUS_FRAME_DURATION_5_MS      = 0x01,
    AUDIO_OPUS_FRAME_DURATION_10_MS     = 0x02,
    AUDIO_OPUS_FRAME_DURATION_20_MS     = 0x03,
    AUDIO_OPUS_FRAME_DURATION_40_MS     = 0x04,
    AUDIO_OPUS_FRAME_DURATION_60_MS     = 0x05,
    AUDIO_OPUS_FRAME_DURATION_80_MS     = 0x06,
    AUDIO_OPUS_FRAME_DURATION_100_MS    = 0x07,
    AUDIO_OPUS_FRAME_DURATION_120_MS    = 0x08,
} T_AUDIO_OPUS_FRAME_DURATION;

/**
 * audio_type.h
 *
 * \brief Define Audio OPUS format attributes for \ref AUDIO_FORMAT_TYPE_OPUS.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_opus_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 8000, 12000, 16000, 24000 and 48000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Number of channels for encoding or decoding.
     * The supported numbers are 1 (Mono) and 2 (Stereo).
     */
    uint8_t                     chann_num;

    /**
     * Constant bitrate (CBR) or variable bitrate (VBR) for encoding.
     * The supported values are 0 for VBR and 1 for CBR.
     */
    uint8_t                     cbr;

    /**
     * Constrained variable bitrate (CVBR) or unconstrained variable bitrate (VBR) for encoding.
     * The supported values are 0 for VBR and 1 for CVBR.
     * \note CVBR shall be enabled only when CBR disabled.
     */
    uint8_t                     cvbr;

    /**
     * Operating mode for encoding or decoding.
     * The supported modes are 1 for SILK-only, 2 for Hybrid (SILK+CELT), and 3 for CELT-only.
     */
    uint8_t                     mode;

    /**
     * Computational complexity for encoding.
     * The supported values are from 0 to 10, where 0 is the lowest complexity
     * and 10 is the highest complexity.
     */
    uint8_t                     complexity;

    /**
     * Frame duration for encoding.
     * The supported frame durations are defined in \ref T_AUDIO_OPUS_FRAME_DURATION.
     * \note Select 20ms frame duration by default.
     */
    T_AUDIO_OPUS_FRAME_DURATION frame_duration;

    /**
     * Bitrate for encoding.
     * The supported ranges are from 6000 bit/s to 510000 bit/s.
     */
    uint32_t                    bitrate;
} T_AUDIO_OPUS_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio FLAC format attributes for \ref AUDIO_FORMAT_TYPE_FLAC.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_flac_attr
{
    uint32_t            sample_rate; /**< Support 8K, 12K, 16K, 24K, 44.1K and 48K in HZ */
    uint8_t             chann_mode; /**< Support mono and stereo channel modes */
} T_AUDIO_FLAC_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio MP3 channel modes.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_mp3_channel_mode
{
    AUDIO_MP3_CHANNEL_MODE_STEREO       = 0x00, /**< MP3 stereo channel mode */
    AUDIO_MP3_CHANNEL_MODE_JOINT_STEREO = 0x01, /**< MP3 joint stereo channel mode */
    AUDIO_MP3_CHANNEL_MODE_DUAL         = 0x02, /**< MP3 dual channel mode */
    AUDIO_MP3_CHANNEL_MODE_MONO         = 0x03, /**< MP3 mono channel mode */
} T_AUDIO_MP3_CHANNEL_MODE;

/**
 * audio_type.h
 *
 * \brief Define Audio MP3 format attributes for \ref AUDIO_FORMAT_TYPE_MP3.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_mp3_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 32000, 44100 and 48000 in HZ for MPEG-1 Layer 3;
     * Additional 16000, 22050 and 24000 in HZ for MPEG-2 Layer 3, and
     * additional 8000, 11025 and 12000 in HZ for MPEG-2.5 Layer 3.
     */
    uint32_t                    sample_rate;

    /**
     * Channel mode for encoding or decoding.
     * The supported values are defined in \ref T_AUDIO_MP3_CHANNEL_MODE.
     */
    T_AUDIO_MP3_CHANNEL_MODE    chann_mode;

    /**
     * MPEG Audio version ID for encoding or decoding.
     * The supported values are 0 for MPEG Version 2.5 (unofficial), 1 for reserved,
     * 2 for MPEG Version 2 (ISO/IEC 13818-3), and 3 for MPEG Version 1 (ISO/IEC 11172-3).
     */
    uint8_t                     version;

    /**
     * MPEG Audio Layer description for encoding or decoding.
     * The supported values are 0 for reserved, 1 for Layer 3, 2 for Layer 2, and 3 for Layer 1.
     */
    uint8_t                     layer;

    /**
     * Bitrate for encoding or decoding.
     * The supported ranges are from 8000 bit/s to 448000 bit/s.
     */
    uint32_t                    bitrate;
} T_AUDIO_MP3_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio LC3 frame duration for \ref T_AUDIO_LC3_ATTR.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_lc3_frame_duration
{
    AUDIO_LC3_FRAME_DURATION_7_5_MS     = 0x00,
    AUDIO_LC3_FRAME_DURATION_10_MS      = 0x01,
} T_AUDIO_LC3_FRAME_DURATION;

/**
 * audio_type.h
 *
 * \brief Define Audio LC3 format attributes for \ref AUDIO_FORMAT_TYPE_LC3.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_lc3_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 8000, 16000, 24000, 32000, 44100 and 48000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Channel location for encoding or decoding.
     * The supported values are 4-octet bitmap defined in \ref AUDIO_CHANNEL_LOCATION.
     */
    uint32_t                    chann_location;

    /**
     * Frame length per channel in octets for encoding or decoding.
     * The supported values are from 20 to 400.
     */
    uint16_t                    frame_length;

    /**
     * Frame duration for encoding.
     * The supported frame durations are defined in \ref T_AUDIO_LC3_FRAME_DURATION.
     */
    T_AUDIO_LC3_FRAME_DURATION  frame_duration;

    /**
     * Presentation delay for encoding or decoding.
     * The supported values are from 0 to 0xFFFFFFFF in microsecond units.
     */
    uint32_t                    presentation_delay;
} T_AUDIO_LC3_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio LDAC channel modes.
 *
 * \ingroup AUDIO_TYPE
 */
typedef enum t_audio_ldac_channel_mode
{
    AUDIO_LDAC_CHANNEL_MODE_MONO         = 0x00, /**< LDAC mono channel mode */
    AUDIO_LDAC_CHANNEL_MODE_DUAL         = 0x01, /**< LDAC dual channel mode */
    AUDIO_LDAC_CHANNEL_MODE_STEREO       = 0x02, /**< LDAC stereo channel mode */
} T_AUDIO_LDAC_CHANNEL_MODE;

/**
 * audio_type.h
 *
 * \brief Define Audio LDAC format attributes for \ref AUDIO_FORMAT_TYPE_LDAC.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_ldac_attr
{
    /**
     * Sample rate for encoding or decoding.
     * The supported values are 44100, 48000, 88200, 96000, 176400 and 192000 in HZ.
     */
    uint32_t                    sample_rate;

    /**
     * Channel mode for encoding or decoding.
     * The supported values are defined in \ref T_AUDIO_LDAC_CHANNEL_MODE.
     */
    T_AUDIO_LDAC_CHANNEL_MODE   chann_mode;
} T_AUDIO_LDAC_ATTR;

/**
 * audio_type.h
 *
 * \brief Define Audio format informations.
 *
 * \ingroup AUDIO_TYPE
 */
typedef struct t_audio_format_info
{
    T_AUDIO_FORMAT_TYPE type;       /**< Audio format type defined in \ref T_AUDIO_FORMAT_TYPE */
    uint8_t             frame_num;  /**< Preset frame number */
    union
    {
        T_AUDIO_PCM_ATTR    pcm;    /**< Audio PCM format attribute */
        T_AUDIO_CVSD_ATTR   cvsd;   /**< Audio CVSD format attribute */
        T_AUDIO_MSBC_ATTR   msbc;   /**< Audio mSBC format attribute */
        T_AUDIO_SBC_ATTR    sbc;    /**< Audio SBC format attribute */
        T_AUDIO_AAC_ATTR    aac;    /**< Audio AAC format attribute */
        T_AUDIO_OPUS_ATTR   opus;   /**< Audio OPUS format attribute */
        T_AUDIO_FLAC_ATTR   flac;   /**< Audio FLAC format attribute */
        T_AUDIO_MP3_ATTR    mp3;    /**< Audio MP3 format attribute */
        T_AUDIO_LC3_ATTR    lc3;    /**< Audio LC3 format attribute */
        T_AUDIO_LDAC_ATTR   ldac;   /**< Audio LDAC format attritute */
    } attr;                         /**< Audio format attribute union */
} T_AUDIO_FORMAT_INFO;

/**
 * audio_type.h
 *
 * \brief Define Audio Effect instance.
 *
 * \ingroup AUDIO_TYPE
 */
typedef void *T_AUDIO_EFFECT_INSTANCE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_TYPE_H_ */
