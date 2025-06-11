// Abbrevations:
// ofs = offset             ver = version           hdl = handle
// frm = frame              sqn = sequence          curr = current
// pos = position           hdr = header            au = audio


// Prefix:
// g = global variable      j = static variable     k = const
// p = pointer              a = array
// u = unsigned integer     s = signed integer
// f = float                d = double
// c = char                 b = bool
// S/z = struct             E/e = enum              U/n = union
// v = void                 x = unknown/unconcerned type
// h = handle


#ifndef MP3_PARSER_H
#define MP3_PARSER_H


#include <stdint.h>
#include "ff.h"
#include "audio_fs_decode.h"


// (SamplesPerFrame / 32kHz * 40kB/s + padding). Also enough for MPEG2 and MPEG2.5.
#define MAX_POSSIBLE_FRAME_BYTES    (1152/32*40+1)
#define TOC_SIZE                    100
#define MP3_BUF_BYTES (MAX_POSSIBLE_FRAME_BYTES + sizeof(MP3_FHEADER))

#ifndef NDEBUG
#undef ASSERT
#define ASSERT(e) \
    do \
    { \
        if(!(e)) \
        { \
            DBG_DIRECT("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            DBG_DIRECT("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            for(;;); \
        } \
    } while(0)
#else
#define ASSERT(e) ((void)0)
#endif


typedef enum
{
    AUDIO_FRAME = 1, // CBR
    XING_FRAME,     // VBR
    INFO_FRAME,     // CBR or VBR. More likely to be CBR.
} E1stFrmType;

typedef struct
{
    // Buffers:
    FIL xFil;
    // Used to perform interal operations and return frame data.
    uint8_t auBuf[MP3_BUF_BYTES];

    // Id3v1/Id3v2 info:
    bool bIsId3v1Exist;
    uint8_t uId3v2Count; // There may be more than 1 id3v2 tag.

    // Audio parameters:
    uint32_t uSamplingFreq_Hz;
    uint32_t uBitRate_kbps; // Only valid when CBR.

    uint32_t uSamplePerFrm;
    float fBytesPerFrm; // Only valid when CBR.
    float fTimePerFrm_ms;
    uint32_t uTotalFrmNum; // -1 if get failed.
    float fTotalPlayTime_ms; // 0 if get failed.

    uint32_t uMaxPossibleFrmBytes;

    uint32_t u1stAuFrmOfs;
    SMp3FrmHdr z1stAuFrmHdr;
    E1stFrmType e1stFrmType;

    // f_read position:
    uint32_t uReadOfs;
    uint32_t uReadSqnNum;

    // Parameters get from Xing/Info frame,
    // only valid when e1stFrmType == XING_FRAME or INFO_FRAME:
    uint8_t uFlag;
    uint32_t uFileLen;
    uint8_t auTocTable[TOC_SIZE];
} *Mp3Hdl_t;


typedef enum
{
    MP3RES_OK = 0,

    MP3RES_MALLOC_FAILED,
    MP3RES_FOPEN_ERROR, // 2
    MP3RES_FLSEEK_ERROR,
    MP3RES_FREAD_ERROR,
    MP3RES_FLSEEK_OR_FREAD_ERROR, //5
    MP3RES_UNSUPPORTED_FORMAT,
    MP3RES_SEARCH_FRAME_FAILED, // 7
    MP3RES_NO_PLAY_TIME_INFO,
    MP3RES_POS_BEYOND_END,
    MP3RES_VBR_WITHOUT_TOC_INFO,  // 10
    MP3RES_ID3V1_NOT_EXIST,
    MP3RES_ID3V2_NOT_EXIST,
    MP3RES_ID3V2_FRAME_NOT_EXIST, // 13
    MP3RES_FILE_ENDS,
    MP3RES_OTHER_ERR,
} EMp3Res;


/** @param[in]  pxFileName: File name in file system.
  * @param[out]  peRes: Operation result. May be MP3RES_OK, MP3RES_MALLOC_FAILED, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_UNSUPPORTED_FORMAT, MP3RES_OTHER_ERR.
  *
  * @return Handle. NULL if operation failed.
  */
Mp3Hdl_t Mp3_CreateHandle(const TCHAR *pxFileName, EMp3Res *peRes);

/** @note Do not free handle that is alread freed.
  */
EMp3Res Mp3_FreeHandle(Mp3Hdl_t hMp3);

EMp3Ver Mp3_GetVersion(Mp3Hdl_t hMp3);
EMp3Layer Mp3_GetLayer(Mp3Hdl_t hMp3);
uint32_t Mp3_GetSamplingFrequency_Hz(Mp3Hdl_t hMp3);
uint32_t Mp3_GetBitRate_kbps(Mp3Hdl_t hMp3);
EMp3ChannelMode Mp3_GetChannelMode(Mp3Hdl_t hMp3);
uint32_t Mp3_GetSamplePerFrame(Mp3Hdl_t hMp3);
float Mp3_GetTimePerFrame_ms(Mp3Hdl_t hMp3);

/*** @return 0.0f if failed. Others if successes.
  */
float Mp3_GetTotalPlayTime_ms(Mp3Hdl_t hMp3);

/** @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[out]  ppuBuf: Return a buffer that contains 128 bytes id3v1 info if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer. Return 128 if operation successes.
  *
  * @return MP3RES_OK, MP3RES_ID3V1_NOT_EXIST, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_ReadId3v1(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen);

/** @brief Retrieve id3v2 frame by frame ID.
  * @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[in]  acFrmId: id3v2 frame id. Eg. TIT2, TPE1, TABL...
  * @param[out]  ppuBuf: Return a buffer that contains id3v2 frame if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer.
  *
  * @return MP3RES_OK, MP3RES_ID3V2_NOT_EXIST, MP3RES_ID3V2_FRAME_NOT_EXIST, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_RetrieveId3v2Frame(Mp3Hdl_t hMp3, const char acFrmId[4], uint8_t **ppuBuf,
                               uint32_t *puLen);

/** @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[out]  ppuBuf: Return a buffer that contains audio frame if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer.
  * @param[out]  pfPlayPos_ms: Current play position in ms.
  *
  * @return MP3RES_OK, MP3RES_FILE_ENDS, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_ReadNextFrame(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen, float *pfPlayPos_ms);

EMp3Res Mp3_SetPlayPos(Mp3Hdl_t hMp3, float fPos_ms);




#endif // MP3_PARSER_H
