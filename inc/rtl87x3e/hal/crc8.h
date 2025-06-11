#ifndef _CRC8_H_
#define _CRC8_H_
/** @defgroup  HAL_87x3e_CRC8    CRC8
    * @brief hal debug api
    * @{
    */

/** @defgroup HAL_87x3e_CRC8_Exported_Functions CRC8 Exported Functions
    * @brief
    * @{
    */
#include <stdint.h>

/**
    * @brief  calculate crc8
    * @note   Poly: 0xedb88320
    * @param  crc: the checksum to pack
    * @param  data: the pointer of input data
    * @param  data_len: the size of data
    * @return The packed checksum
    */
uint8_t crc8_calu(uint8_t crc, const void *data, uint32_t data_len);

/** @} */ /* End of group HAL_87x3e_CRC8_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_CRC8_DEBUG */

#endif
