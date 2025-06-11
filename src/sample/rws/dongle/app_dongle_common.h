#ifndef _APP_DONGLE_COMMON_H_
#define _APP_DONGLE_COMMON_H_

#include "app_link_util.h"

void app_dongle_common_init(void);

/**
    * @brief  dongle is streaming or not
    * @param  void
    * @return true/false
    */
bool app_dongle_is_streaming(void);

/**
    * @brief  Get connected dongle's link
    * @param  void
    * @return link info
    */
T_APP_BR_LINK *app_dongle_get_connected_dongle_link(void);

/**
    * @brief  Get connected phone link
    * @param  void
    * @return link info
    */
T_APP_BR_LINK *app_dongle_get_connected_phone_link(void);

/**
    * @brief  Get connected dongle's addr
    * @param  bd_addr bluetooth address
    * @return true/false
    */
bool app_dongle_get_connected_dongle_addr(uint8_t *addr);

/**
    * @brief  Check the input addr is dongle addr or not
    * @param  check_addr : addr wants to check
    * @return true if it is dongle addr
    */
bool app_dongle_is_dongle_addr(uint8_t *check_addr);

#if F_APP_LEA_SUPPORT
T_APP_LE_LINK *app_dongle_get_le_audio_link(void);
#if F_APP_GAMING_DONGLE_SUPPORT
void app_dongle_check_exit_pairing_state(uint8_t link_state, uint8_t *bd_addr);
#endif
#endif

#endif
