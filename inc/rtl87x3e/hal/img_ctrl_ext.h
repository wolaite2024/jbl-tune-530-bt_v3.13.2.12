/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     img_ctrl_ext.h
* @brief    This file provides image control related api.
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/
#ifndef __IMG_CTRL_EXT_H__
#define __IMG_CTRL_EXT_H__

#include <stddef.h>
#include "patch_header_check.h"

/** @defgroup  HAL_87x3e_IMG_CTRL_EXT    image control extention
    * @brief image control api
    * @{
    */

/** @defgroup HAL_87x3e_Img_Ctrl_Ext_Exported_Functions image control extention  Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  get the image feature information
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @note   get the image feature field(16 bytes) from the image header
    * @param  img_id  specify the image id to get the image feature
    * @param  p_feature_buf specify the buffer to save the image feature
    * @param  feature_buf_len specify the buffer length of p_feature_buf
    * @return the result of getting image feature information
    * @retval 0      success
    * @retval other  fail
    */
int get_image_feature_info(IMG_ID img_id, uint8_t *p_feature_buf, uint8_t feature_buf_len);
/**
    * @brief  get the image release version
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @note   get the image release version field(8 bytes) from the image header
    * @param  img_id  specify the image id to get the image release version
    * @return the image release version, data type: unsigned long long
    */
uint64_t get_image_release_version(IMG_ID img_id);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_87x3e_Img_Ctrl_Ext_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_IMG_CTRL_EXT */
#endif
