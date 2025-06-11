#ifndef __COMBINE_HEADER_H
#define __COMBINE_HEADER_H



#include <stdint.h>
#include "patch_header_check.h"


#define COMBINATION_HEADER_SIZE         28
#define MERGED_HEADER_SIZE              44
#define MP_HEADER_SIZE                  512
#define DEVICE_BIN_NUM                  2
#define SUB_IMG_HEADER_SIZE             12
#define IMG_CTRL_HDR_SIZE               4096


#define BANK_NUM                    2


typedef struct
{
    uint32_t length; // length of this bin file
    uint8_t default_bud_role : 1; // if 1, for primary ear; if 0, for secondary ear
    uint8_t rsv : 7;
    uint8_t rsv2[3];
} COMBNATION_BIN_INFO;



typedef struct
{
    uint32_t signature;// const signature: 0x9697b5c8
    uint32_t version;//version of combine spec
    uint32_t num; //number of bin
    COMBNATION_BIN_INFO bin_infos[DEVICE_BIN_NUM]; // actually has num of "struct BIN_INFO"
} COMBINATION_HEADER;


typedef struct
{
    uint32_t download_addr;
    uint32_t size;
    uint32_t reserved;
} SUB_IMG_HEADER;


typedef struct
{
    uint16_t    signature;
    uint32_t    size_of_merged_file;
    uint8_t     checksum[32];
    uint16_t    extension;
    uint32_t    sub_file_indicator;
} MERGED_HEADER;


void get_combination_header_by_stream(uint8_t const *stream,
                                      COMBINATION_HEADER *p_combination_header);


void get_merged_header_by_stream(uint8_t const *stream, MERGED_HEADER *p_mp_header);


void get_sub_img_header_by_stream(uint8_t const *stream, SUB_IMG_HEADER *p_sub_img_header);


uint32_t get_file_num_by_file_indicator(uint32_t sub_file_indicator);
#endif

