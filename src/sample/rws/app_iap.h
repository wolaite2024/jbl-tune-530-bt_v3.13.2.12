/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_IAP_H_
#define _APP_IAP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

/** @defgroup APP_IAP App Iap
  * @brief This file provide api for iap function.
  * @{
  */

#define EA_PROTOCOL_ID_ALEXA    1
#define EA_PROTOCOL_ID_RTK      10

typedef struct t_app_iap T_APP_IAP, *T_APP_IAP_HDL;

struct t_app_iap
{
    T_APP_IAP   *p_next;
    T_BT_EVENT  bt_event;
    bool        authen_flag;
    uint8_t     credit;
    uint16_t    frame_size;
    uint8_t     bd_addr[6];
};

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_IAP_Exported_Functions App Iap Functions
    * @{
    */
/**
    * @brief  hardware init for i2c communication with external chip.
    * @param  p_i2c pointer to struct @ref I2C_TypeDef object
    * @return void
    */
void app_iap_cp_hw_init(void *p_i2c);

/**
    * @brief  Iap module init.
    * @param  void
    * @return void
    */
void app_iap_init(void);


void app_iap_handle_remote_conn_cmpl(void);

T_APP_IAP_HDL app_iap_search_by_addr(uint8_t *bd_addr);

bool app_iap_set_roleswap_info(T_APP_IAP_HDL hdl, uint8_t credit, uint16_t frame_size);

uint8_t app_iap_get_credit(T_APP_IAP_HDL hdl);

uint16_t app_iap_get_frame_size(T_APP_IAP_HDL hdl);

bool app_iap_is_authened(T_APP_IAP_HDL hdl);
/** @} */ /* End of group APP_IAP_Exported_Functions */
/** End of APP_IAP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_IAP_H_ */
