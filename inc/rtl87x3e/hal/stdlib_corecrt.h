#ifndef _STDLIB_CORECRT_H
#define _STDLIB_CORECRT_H
#include <stdlib.h>
/** @defgroup  HAL_87x3e_STDLIB_CORECRT    stdlib corecrt api
    * @brief stdlib corecrt api
    * @{
    */

/** @defgroup HAL_87x3e_Stdlib_Corecrt_Exported_Functions stdlib corecrt Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  memcpy_s with security check mechanism
    * @param  dest destination buffer pointer
    * @param  destsz the size of destination buffer
    * @param  src   source buffer pointer
    * @param  count source data size to memcpy
    * @return the error code
    * @retval 0      success
    * @retval != 0   error code
    */
extern int memcpy_s(void *restrict dest, size_t destsz, const void *restrict src, size_t count);
/**
    * @brief  memset_s with security check mechanism
    * @param  dest destination buffer pointer
    * @param  destsz the size of destination buffer
    * @param  ch   memory set value
    * @param  count data size to memset with ch
    * @return the error code
    * @retval 0      success
    * @retval != 0   error code
    */
int memset_s(void *restrict dest, size_t destsz, int ch, size_t count);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_87x3e_Stdlib_Corecrt_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_STDLIB_CORECRT */
#endif /* stdlib_corecrt.h */
