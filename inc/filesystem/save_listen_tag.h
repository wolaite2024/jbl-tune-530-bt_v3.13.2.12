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
#ifndef _SAVE_LISTEN_TAG_H_
#define _SAVE_LISTEN_TAG_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include "ff.h"
#include "stdint.h"
#include "id3v2_tag.h"

typedef struct
{
    uint8_t fileHeader[3];/* 'ID3' */
    uint8_t version; /* '3' */
    uint8_t revision; /* '0' */
    uint8_t flag; /* '0' */
    uint8_t headerSize[4];
} RTK_FILE_HEADER; /* same as ID3V2H */

/*  attr0: ("TFLT", 2, 0)    fileType(1Byte): LATM or SBC+fileSource(1Byte): master or slave
    attr1: ('"TSIZ", 8, 0)    File totalLen(4Byte)+Total FrameNum:(4Byte)
*/
typedef struct
{
    uint8_t attrHeader[4];
    uint8_t flag[2];            /* '0' */
    uint8_t size[4];
} RTK_FILE_ATTR_HEADER; /* same as ID3V2EH */

/* SBC_file = RTK_FILE_HEADER +
              RTK_PKT_HEADER + SBC_FHEADER + rawData + SBC_FHEADER + rawData + SBC_FHEADER + rawData + ... +
              RTK_PKT_HEADER + SBC_FHEADER + rawData + SBC_FHEADER + rawData + SBC_FHEADER + rawData + ...
   LATM_file = RTK_FILE_HEADER +
               RTK_PKT_HEADER + LATM_FHEADER + rawData +
               RTK_PKT_HEADER + LATM_FHEADER + rawData + ...
*/

typedef struct
{
    uint16_t    syncword: 8;        /* 0xFF */
    uint16_t    reserved: 3;        /* '000' */
    uint16_t    pktIsInValid: 1;    /* 1: pkt is invalid,
                                       0: pkt is valid */
    uint16_t    pktFrameNum: 4;     /* sbc: according to phone
                                       LATM: always 1 */
    uint16_t    pktLength;
} RTK_PKT_HEADER;

typedef struct
{
    uint8_t syncWord: 8;
    uint8_t samplingFrequency: 2;
    uint8_t blocks: 2;
    uint8_t channelMode: 2;
    uint8_t allocationMethod: 1;
    uint8_t subbands: 1;
    uint8_t bitpool: 8;
    uint8_t crc: 8;
} SBC_FHEADER;
/*  IOS LATM_FHEADER: fixed header(9Byte)--(0x47, 0xFC, 0x00, 0x00, 0xB0, 0x90, 0x80, 0x03, 0x00)
    the 3 Byte after LATM header, is frame lenth. adding together is the frame length
*/

extern const uint8_t g_file_header[10];
extern const uint8_t g_attr0_header[10];
extern const uint8_t g_attr1_header[10];
extern const uint8_t g_latm_fheader[9];
//extern uint8_t g_sbc_attr0_data[5] = {0x53, 0x42, 0x43, 0x00, 0x00};
//extern uint8_t g_latm_attr0_data[5] = {0x4C, 0x41, 0x54, 0x4D, 0x00};
//extern uint32_t g_attr1_data[2] = {0x00000000, 0x00000000};
//extern uint8_t g_rtk_pkt_header[4] = {0xFF, 0x00, 0x00, 0x00};
//extern uint8_t g_sbc_fheader[4] = {0x9C, 0x00, 0x00, 0x00};

#define SBC_ATTR0_LEN           5
#define SBC_ATTR0_CONTENT       0x53, 0x42, 0x43, 0x00, 0x00
#define LATM_ATTR0_LEN          5
#define LATM_ATTR0_CONTENT      0x4C, 0x41, 0x54, 0x4D, 0x00

#define SBC_HEADER_LEN          4
#define SBC_HEADER_CONTENT      0x9C, 0x00, 0x00, 0x00
#define AAC_LATM_HEADER_LEN     9
#define AAC_LATM_HEADER_CONTENT 0x47, 0xFC, 0x00, 0x00, 0xB0, 0x90, 0x80, 0x03, 0x00

#define RTK_PKT_HEAD_SYNCWORD        0xFF

#endif /*_SAVE_LISTEN_TAG_H_*/
