#ifndef _APP_DONGLE_SPP_H_
#define _APP_DONGLE_SPP_H_

#include <stdbool.h>
#include "app_sdp.h"
#include "app_dongle_data_ctrl.h"
#include "app_link_util.h"
#include "legacy_gaming.h"

#define VOL_BALANCE_VALUE_MAX                   100

typedef enum
{
    DONGLE_CMD_ACK          = 0x00,
    DONGLE_CMD_INFORM       = 0x01, // from dongle to earphone
    DONGLE_CMD_UPDATE       = 0x02, // from earphone to dongle
} T_DONGLE_CMD_MSG_TYPE;

typedef enum
{
    APP_IGNORE_DONGLE_SPP_GAMING_CMD_RESET          = 0x00,
    APP_IGNORE_DONGLE_SPP_GAMING_CMD_FIRST_CMD      = 0x01,
    APP_IGNORE_DONGLE_SPP_GAMING_CMD_SRC_SWITCH     = 0x02,
} T_APP_IGNORE_DONGLE_SPP_GAMING_CMD_REASON;

uint8_t app_dongle_get_mic_data_idx(void);
void app_dongle_updata_mic_data_idx(bool is_reset);
void app_dongle_spp_init(void);
void app_dongle_handle_gaming_mode_cmd(uint8_t action);
void app_dongle_update_is_mic_enable(bool mic_enable);
/**
    * @brief        This function can request enter or exit gaming mode.
    * @return       void
    */
void app_dongle_gaming_mode_request(uint8_t status);

/**
    * @brief        This function can report or request volume balance record.
    * @return       void
    */
void app_dongle_volume_balance_report(uint8_t msg_type);

/**
    * @brief        This function can update volume balance value.
    * @param        gaming balance: 0 ~ 100, which stands for 0% ~ 100%
    * @param        chat balance: 0 ~ 100, which stands for 0% ~ 100%
    * @return       void
    */
void app_dongle_update_volume_balance(uint8_t gaming_balance, uint8_t chat_balance);
void app_dongle_report_pass_through_event(uint8_t *data, uint16_t data_len);

/**
 * \brief   Check if legacy gaming is ready
 *
 * \return               true if it is ready
 */
bool app_dongle_legacy_gaming_is_ready(void);

/**
 * \brief   Legacy gaming event callback function
 *
 * \param[in] event       legacy gaming event
 * \param[in] addr       peer addr
 *
 * \return               none
 */
void app_dongle_legacy_gaming_event_cback(T_LEGACY_GAMING_EVENT event, uint8_t *addr);


#endif
