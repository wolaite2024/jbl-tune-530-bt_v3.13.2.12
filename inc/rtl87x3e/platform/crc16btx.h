/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#if ! defined (__CRC16BTX_H)
#define       __CRC16BTX_H

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>

/** @defgroup 87x3e_CRC_FCS CRC Implementation
  * @brief CRC implementation for specified polynomial:X**0 + X**2 + X**15 + X16
  * @{
  */

/*============================================================================*
 *                              Macro
*============================================================================*/
/** @defgroup 87x3e_CRC_FCS_Exported_Macros CRC Implementation Exported Macros
    * @{
    */
#define BTXFCS_INIT      0x0000  //!< Initial FCS value
#define BTXFCS_GOOD      0x0000  //!< Good final FCS value


/**
* @brief  Calculate a new fcs given the current fcs and the new data.
*       Polynomial: X**0 + X**2 + X**15 + X16
*
* @param  fcs: init or good final
* @param  cp: data point
* @param  len: length
*
* @return
*
*/
uint16_t btxfcs(uint16_t fcs,
                uint8_t  *cp,
                uint16_t len);

/**
* @brief  Calculate
*
* @param  buf: data point
* @param  size: data size
* @param  compared_crc16: expected crc16 result
* @return true: the same as compared_crc16, false: has differnet value
*
*/
bool crc16_ibm(uint8_t *buf, uint32_t size, uint16_t compared_crc16);

/**
* @brief  proprietary inteface for Realtek internal handshake API .
* @param  data: input data buffer
* @param  len: input data buffer length
* @return true if handshare pass, fals othewise
*
*/
extern bool btx_vendor_get_response(uint8_t *data, uint8_t len);

/** @} */ /* End of group 87x3e_CRC_FCS_Exported_Macros */


/** @} */ /* End of group 87x3e_CRC_FCS */

#endif  /**< #if ! defined (__CRC16BTX_H) */

/** End of CRC16BTX.H */

