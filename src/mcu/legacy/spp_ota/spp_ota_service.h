/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      spp_ota_service.h
   * @brief     Head file for using spp OTA service
   * @author    Michael
   * @date      2018-10-15
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _SPP_OTA_SERVICE_H_
#define _SPP_OTA_SERVICE_H_

/** @defgroup  SPP_OTA_SERVICE SPP OTA Service
    * @brief SPP Service to implement OTA feature
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup SPP_OTA_SERVICE_Exported_Macros Spp OTA service Exported Macros
    * @brief
    * @{
    */

/** @brief  spp ota command length. */
#define OTA_LENGTH_OTA_GET_INFO         0
#define OTA_LENGTH_OTA_GET_IMG_VER      0
#define OTA_LENGTH_START_OTA            16
#define OTA_LENGTH_DATA_PKT             127
#define OTA_LENGTH_VALID_FW             3
#define OTA_LENGTH_IMAGE_INFO           2
#define OTA_LENGTH_BUFFER_CHECK_EN      0
#define OTA_LENGTH_BUFFER_CRC           4
#define OTA_LRNGTH_RWS_INFO             0
#define OTA_LRNGTH_OTHER_INFO           0
#define OTA_LENGTH_SECTION_SIZE         0

#define OTA_DEVICE_FEATURE_SUPPORT_BUFFER_CHECK     (1 << 0)
#define OTA_DEVICE_FEATURE_ENABLE_AES_ENCRYPT       (1 << 1)
#define OTA_DEVICE_FEATURE_SUPPORT_MULTIIMAGE       (1 << 4)

#define SPP_PROTOCOL_INFO      0x0011//0x0012
/** End of SPP_OTA_SERVICE_Exported_Macros
    * @}
    */


/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup SPP_OTA_SERVICE_Exported_Types SPP OTA Service Exported Types
    * @brief
    * @{
    */
/** @brief  OTA timer callback */
typedef enum
{
    OTA_TIMER_DELAY_RESET,

    OTA_TIMER_TOTAL,
} T_OTA_TIMER;

/** End of SPP_OTA_SERVICE_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup SPP_OTA_SERVICE_Exported_Functions Spp OTA service Functions
    * @brief
    * @{
    */
extern void app_report_spp_event(T_APP_BR_LINK *p_link, uint16_t event_id, uint8_t *data,
                                 uint16_t len);

extern void app_cmd_set_event_ack(uint8_t cmd_path, uint8_t app_idx, uint8_t *buf);
/**
    * @brief  The main function to handle all the spp ota command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  app_idx received rx command device index
    * @return void
    */
void spp_ota_service_handle(uint16_t length, uint8_t *p_value, uint8_t app_idx);


/** @} */ /* End of group SPP_OTA_SERVICE_Exported_Functions */

/** @} */ /* End of group SPP_OTA_SERVICE */
#endif
