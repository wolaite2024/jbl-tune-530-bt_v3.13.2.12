#include "image_header.h"
#include <gap.h>
#include <string.h>
#include "trace.h"


void get_combination_header_by_stream(uint8_t const *stream,
                                      COMBINATION_HEADER *p_combination_header)
{
    LE_STREAM_TO_UINT32(p_combination_header->signature, stream);
    LE_STREAM_TO_UINT32(p_combination_header->version, stream);
    LE_STREAM_TO_UINT32(p_combination_header->num, stream);
    LE_STREAM_TO_UINT32(p_combination_header->bin_infos[0].length, stream);
    LE_STREAM_TO_UINT32(p_combination_header->bin_infos[0].default_bud_role, stream);
    LE_STREAM_TO_UINT32(p_combination_header->bin_infos[1].length, stream);
    LE_STREAM_TO_UINT32(p_combination_header->bin_infos[1].default_bud_role, stream);

    APP_PRINT_TRACE7("get_combination_header_by_stream: sig 0x%08x, version 0x%08x, num %d,"
                     "[0].length %d, [0].default_bud_role %d, [1].length %d, [1].default_bud_role %d",
                     p_combination_header->signature, p_combination_header->version, p_combination_header->num,
                     p_combination_header->bin_infos[0].length, p_combination_header->bin_infos[0].default_bud_role,
                     p_combination_header->bin_infos[1].length, p_combination_header->bin_infos[1].default_bud_role);
}


void get_merged_header_by_stream(uint8_t const *stream, MERGED_HEADER *p_mp_header)
{
    LE_STREAM_TO_UINT16(p_mp_header->signature, stream);
    LE_STREAM_TO_UINT32(p_mp_header->size_of_merged_file, stream);
    memcpy(p_mp_header->checksum, stream,
           sizeof(p_mp_header->checksum));
    stream += sizeof(p_mp_header->checksum);

    LE_STREAM_TO_UINT16(p_mp_header->extension, stream);
    LE_STREAM_TO_UINT32(p_mp_header->sub_file_indicator, stream);

    APP_PRINT_TRACE5("get_merged_header_by_stream: sig 0x%08x, size_of_merged_file %d, checksum 0x%08x,"
                     "extension 0x%08x, sub_file_indicator 0x%08x",
                     p_mp_header->signature, p_mp_header->size_of_merged_file, p_mp_header->checksum,
                     p_mp_header->extension, p_mp_header->sub_file_indicator);
}


void get_sub_img_header_by_stream(uint8_t const *stream, SUB_IMG_HEADER *p_sub_img_header)
{
    LE_STREAM_TO_UINT32(p_sub_img_header->download_addr,
                        stream);
    LE_STREAM_TO_UINT32(p_sub_img_header->size, stream);
    LE_STREAM_TO_UINT32(p_sub_img_header->reserved, stream);

    APP_PRINT_TRACE2("get_sub_img_header_by_stream: download_addr 0x%08x, size %d",
                     p_sub_img_header->download_addr, p_sub_img_header->size);
}


static uint32_t bit_count(uint32_t binary_num)
{
    uint32_t i = 0 ;

    for (i = 0; binary_num; i++)
    {
        binary_num &= (binary_num - 1); // cancel out the last bit
    }

    return i;
}


uint32_t get_file_num_by_file_indicator(uint32_t sub_file_indicator)
{
    uint32_t sub_file_num = 0;

    sub_file_num = bit_count(sub_file_indicator);

    APP_PRINT_TRACE1("get_file_num_by_file_indicator: sub_file_num %d", sub_file_num);

    return sub_file_num;
}

