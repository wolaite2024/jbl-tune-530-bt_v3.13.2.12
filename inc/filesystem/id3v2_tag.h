#ifndef _ID3V2TAG_H
#define _ID3V2TAG_H

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define TAGHSIZE sizeof(ID3V2H)
#define TAGFHSIZE sizeof(ID3V2FH)
#define TAGFLSIZE sizeof(ID3V2FL)

#pragma  pack(1)

//ID3V2 header
typedef struct ID3V2TagH
{
    char header[3];
    char version;
    char revision;
    char flag;
    char size[4];
} ID3V2H;

//ID3v2 extended header
typedef struct ID3V2TagEH
{
    char header[4];
    char flag[2];
    char size[4];
} ID3V2EH;

//ID3v2 frame header
typedef struct ID3V2FrameH
{
    char frameID[4];
    char size[4];
    char flags[2];
} ID3V2FH;

typedef struct ID3V2TagFL
{
    char fName[4 + 1];  /* more save '\0' */
    char *pFBody;
} ID3V2FL;
#pragma  pack()

typedef struct ID3V1TAG
{
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[28];
    char resvered;
    char track;
    char genre;
} ID3V1H;

#endif
