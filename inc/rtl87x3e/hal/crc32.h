#ifndef _CRC32_H_
#define _CRC32_H_

/** @defgroup  HAL_87x3e_CRC32    CRC32
    * @brief hal debug api
    * @{
    */

/** @defgroup HAL_87x3e_CRC32_Exported_Functions CRC32 Exported Functions
    * @brief
    * @{
    */
#include <stdint.h>

/**
    * @brief  calculate crc32
    * @note   Poly: 0xedb88320
    * @param  crc: the checksum to pack
    * @param  data: the pointer of input data
    * @param  data_len: the size of data
    * @return The packed checksum
    */
uint32_t crc32_calu(uint32_t crc, const void *data, uint32_t data_len);

/** @} */ /* End of group HAL_87x3e_CRC32_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_CRC32_DEBUG */

#endif
