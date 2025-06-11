/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author    carol_shen
   * @date      2020-4-20
   * @version   1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/
#ifndef _AUDIO_FS_DECODE_H_
#define _AUDIO_FS_DECODE_H_


/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "os_mem.h"
#include "os_queue.h"
#include "ff.h"
#include "diskio.h"
#include "id3v2_tag.h"
#include "save_listen_tag.h"

#define AUDIO_FS_DECODE_DEBUG   0

#define AAC_FORMAT_SUPPORT      1
#define MP3_FORMAT_SUPPORT      1
#define MP4_FORMAT_SUPPORT      0

#define ADTS_SYNC_WORD_LEN    7
#define SBC_SYNC_WORD_LEN     8
#define LATM_SYNC_WORD_LEN    16
#define MP3_SYNC_WORD_LEN     4

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/* MP4:
boxType:
    ftyp(file type)
    moov(movie)
        mvhd(movie header)
        trak(track)
            media(media)
                mdhd(media header)
                stbl(sample table)
                    stsd(sample descriptions)
                        enca(encryped audio)
                            esds(elementary stream)
                            sinf(encryption info)
                            frma(original format)
                            schm(scheme type)
                            schi(scheme info)
                            DRM specifics
                    stts(timestamp table)
                    stsc(sample to chunk table)
                    stsz(sample size table)
                    stco(chunk offsets table)
                        sample0, sample1, sample2, sample3, sample4, sample5
        trak(track)
            media(media)
                mdhd(media header)
                stbl(sample table)
                    stsd(sample descriptions)
                        enca(encryped audio)
                            esds(elementary stream )
                            sinf(encryption info)
                            frma(original format)
                            schm(scheme type)
                            schi(scheme info)
                            DRM specifics
                    stts(timestamp table)
                    stsc(sample to chunk table)
                    stsz(sample size table)
                    stco(chunk offsets table)
                        sample6, sample7, sample8, sample9, sample10, sample11
        udta(user data)
            meta(meta data)
        mdat(media data)
        Access Unit(Sample0)
        Access Unit(Sample1)
        Access Unit(Sample2)
        Access Unit(Sample3)
        Access Unit(Sample4)
        Access Unit(Sample5)
*/
typedef enum
{
    ftyp = 0,
    moov,
    mvhd,
    meta,
    trak,
    tkhd,
    tref,
    trgr,
    edts,
    mdia,
    mdhd,
    mdlr,
    hdlr,
    elng,
    minf,
    vmhd,
    smhd,
    hmhd,
    sthd,
    nmhd,
    dinf,
    dref,
    stbl,
    stsd,
    stts,
    ctts,
    cslg,
    stsc,
    stsz,
    stco,
    co64,
    stss,
    stsh,
    padb,
    stdp,
    sdtp,
    sbgp,
    sgpd,
    subs,
    saiz,
    saio,
    udta,
    mvex,
    mehd,
    trex,
    leva,
    moof,
    mfhd,
    traf,
    tfhd,
    trun,
    iods,
    stz2,
    mdat,
} MP4_BOX_TYPE;

typedef struct
{
    uint8_t size[4];
    uint8_t type[4]; /* 32bit */
} MP4_BOX_HEADER;

typedef struct
{
    uint8_t     header[8];
    uint8_t     resved0[6];
    uint16_t    dataRefInx;
    uint8_t     resved1[8];
    uint16_t    channelCount;
    uint16_t    sampleCount;
    uint16_t    preDefined;
    uint8_t     resved2[2];
    uint32_t    sampleRate;
    uint8_t     ESDescriptor[54];
} MP4_AUDIO_DESCRIPT;

typedef struct
{
    uint16_t           version;
    uint8_t            flags;
    uint32_t           count;
    MP4_AUDIO_DESCRIPT mp4AudioDescript;
} MP4_STSD_BOX;

typedef struct
{
    uint64_t totalFrameCnt;
    uint32_t frameTableStartOffset;
    uint32_t frameTableLength;
    uint32_t chunkTableOffset;
    uint32_t chunkTableLength;
    uint32_t mdatOffset;
    uint32_t mdatLength;
    uint32_t curFrameNum;
} MP4_HEADER_INFO;

typedef enum
{
    CHANNEL_STEREO       = 0,
    CHANNEL_JOINT_STEREO = 1,
    CHANNEL_DOUBLE       = 2,
    CHANNEL_SINGLE       = 3,
} MP3_CHANNEL_MODE, EMp3ChannelMode;

typedef enum
{
    MPEG_2P5            = 0,
    VERSION_NONE        = 1,
    MPEG_2              = 2,
    MPEG_1              = 3,
} MP3_VERSION, EMp3Ver;

typedef enum
{
    LAYER_NONE          = 0,
    LAYER_3             = 1,
    LAYER_2             = 2,
    LAYER_1             = 3,
} MP3_LAYER, EMp3Layer;

typedef struct
{
    uint8_t sync1: 8;
    uint8_t error_protection: 1;
    uint8_t layer: 2;
    uint8_t version: 2;
    uint8_t sync2: 3;
    uint8_t extension: 1;
    uint8_t padding: 1;
    uint8_t sample_rate_index: 2;
    uint8_t bit_rate_index: 4;
    uint8_t emphasis: 2;
    uint8_t original: 1;
    uint8_t copyright: 1;
    uint8_t mode_extension: 2;
    uint8_t channel_mode: 2;
} MP3_FHEADER, SMp3FrmHdr;

typedef struct
{
    uint8_t  header[4];
    uint8_t  xingFlag[4];
    uint8_t  totalFrameLen[4];
    uint8_t  fileSize[4];
} XING_HEADER_INFO;

typedef enum
{
    AOT_SPECIFIC_CONFIG     = 0, /* Defined in AOT Specifc Config */
    CHANNEL_1PCS            = 1, /* front-center */
    CHANNEL_2PCS            = 2, /* front-left,front-right */
    CHANNEL_3PCS            = 3, /* front-center,front-left, front-right */
    CHANNEL_4PCS            = 4, /* front-center, front-left, front-right, back-center */
    CHANNEL_5PCS            = 5, /* front-center, front-left, front-right, back-left, back-right */
    CHANNEL_6PCS            = 6, /* front-center, front-left, front-right, back-left, back-right, LFE-channel */
    CHANNEL_7PCS            = 7, /* front-center, front-left, front-right,
                                    side-left, side-right, back-left, back-right, LFE-channel */
} AAC_CHANNEL_MODE;

typedef struct
{
    uint16_t sync1: 12; /* sync word, ADTS  always 0xFFF; LATM: */
    uint8_t ID: 1;      /* 0 MPEG-4   1 MPEG-2*/
    uint8_t layer: 2;   /* always '00' */
    uint8_t protection_absent: 1; /* 0 has CRC   1 no CRC */
    uint8_t profile: 2; /* 0 main profile
                          1 low complexity profile(LC)
                          2 scalable sampling rate profile(SSR)
                          3 reserved*/
    uint8_t sample_rate_index: 4;
    uint8_t private_bit: 1;
    uint8_t channel_configuration: 3;
    uint8_t original_copy: 1;
    uint8_t home: 1;
} ADTS_FIXED_FHEADER;

typedef struct
{
    uint8_t copyright_identification_bit: 1;
    uint8_t copyright_identification_start: 1;
    uint16_t aac_frame_length: 13; /* size(AACFrame)=(ADTS header+AAC stream);
                                     aac_frame_length=(protection_absent == 1 ? 7 : 9) + size(AACFrame)*/
    uint16_t adts_buffer_fullness: 11; /* 0x7FF variable code rate */
    uint8_t number_of_raw_data_blocks_in_frame: 2;
} ADTS_VARIABLE_FHEADER;

/*============================================================================*
  *                                  Variables
  *============================================================================*/
typedef enum
{
    TIT2 = 0,
    TALB,
    TCOM,
    TPE1,
    TPE2,
    TCON,
    TXXX,
    TOFN,
    TIME,

    ALL_TAGS,
} ID3V2_TAG;

typedef struct
{
    uint8_t *info;
    uint16_t length;
} ID3V2_TAG_INFO;

typedef struct
{
    uint32_t startOffset;
    uint32_t length;
} ID3V2_INFO;

typedef enum
{
    ID3V2_HEADER            = 0x10,
    ID3V1_HEADER            = 0x20,
    MP3_HEADER              = 0x30,
    XING_HEADER             = 0x40,
    AAC_HEADER              = 0x50,
    XING_OR_MP3_HEADER      = 0x60,
    RTK_ADTS_HEADER         = 0x70,
    RTK_LATM_HEADER         = 0x80,
    RTK_SBC_HEADER          = 0x90,
    MP4_HEADER              = 0xA0,
    HEADER_TYPE_NONE        = 0xFF,
} HEADER_TYPE;

typedef struct
{
    uint8_t                 frameNum;
    uint16_t                length;   /* frame length: before decode */
    uint8_t                 *content;
} FRAME_CONTENT;

typedef enum
{
    UNKNOWN     = 0,
    RTK         = 0x10,
    AAC         = 0x20,
    MP3         = 0x30,
    MP4         = 0x40,
    FLAC        = 0x50,
} AUDIO_FILE_FORMAT;

typedef enum
{
    MP3_CBR     = 0x31,
    MP3_VBR     = 0x32,
    MP3_VBRI    = 0x33,
} MP3_SUB_FORMAT;

typedef enum
{
    RTK_SBC      = 0x11,
    RTK_ADTS     = 0x12,
    RTK_LATM     = 0x13,
} RTK_TRANSPORT_FORMAT;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint8_t     block_length;
    uint8_t     subbands;
    uint8_t     allocation_method;
    uint8_t     bitpool;
} SBC_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint16_t    frame_duration;
    uint32_t    bit_rate;
} ADTS_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
} LATM_FRAME_INFO;

typedef struct
{
    RTK_TRANSPORT_FORMAT rtkTransFormat;
    union
    {
        SBC_FRAME_INFO sbc;
        ADTS_FRAME_INFO adts;
        LATM_FRAME_INFO latm;
    } rtk_trans_info;
} RTK_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint16_t    frame_duration;
} AAC_FRAME_INFO;

typedef struct
{
    MP3_SUB_FORMAT    mp3_sub_format;
    uint32_t          sampling_frequency;
    uint8_t           channel_mode;
    uint8_t           version;
    uint8_t           layer;
    uint32_t          bit_rate;
    uint32_t          total_frames;
    uint32_t          total_file_size;
    uint16_t          frame_size_after_decode; /* frame size: after decode */
    uint16_t          sample_counts;
    uint16_t          frame_duration;
} MP3_FRAME_INFO;

typedef struct
{
    uint32_t          sampling_frequency;
    uint8_t           channel_mode;
    uint32_t          bit_rate;
    uint32_t          total_frames;
    uint32_t          total_file_size;
    uint16_t          sample_counts;
} MP4_FRAME_INFO;

typedef struct
{
    AUDIO_FILE_FORMAT format;
    union
    {
        RTK_FRAME_INFO  rtk;
        AAC_FRAME_INFO  aac;
        MP3_FRAME_INFO  mp3;
        MP4_FRAME_INFO  mp4;
    } format_info;
} FRAME_INFO;


/*============================================================================*
 *                                Functions
 *============================================================================*/
uint16_t audio_fs_decode_init(void);
uint16_t audio_fs_decode_before_get_frame(void *handle);
uint16_t audio_fs_decode_get_frame(void *handle, FRAME_CONTENT *p_frameContent);
void audio_fs_decode_set_frame_format(TCHAR *fname, uint16_t nameLen);
void audio_fs_decode_get_frame_info(FRAME_INFO *p_frameInfo);
void audio_fs_decode_deinit(void);

uint32_t audio_fs_decode_id3v2_info(void *handle, uint16_t tagIdx);
uint16_t audio_fs_decode_get_id3v2_InfoLen(uint16_t tagIdx);
uint8_t *audio_fs_decode_get_id3v2_Info(uint16_t tagIdx);
void audio_fs_decode_get_mp3_playtime(void);

#endif //_AUDIO_FS_DECODE_H_void
