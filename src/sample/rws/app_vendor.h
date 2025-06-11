#ifndef _APP_VENDOR_H_
#define _APP_VENDOR_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Set Advertising Extended Misc Command
 *
 * @param fix_channel  Aux ADV Fix Channel:
 * 0 Extended Advertising auxiliary packet channel unfixed.
 * 1 Extended Advertising auxiliary packet channel fixed to 1 (2406MHz).
 * @param offset  Aux ADV offset
 * 0~0xFF (Unit: slot)
 * The minimum value of the offset from a Extended Advertising packet to its auxiliary packet
 * The actual offset value might larger than Aux ADV offset because of collision with other protocol.
 * @return true  success
 * @return false fail
 */
bool app_vendor_set_adv_extend_misc(uint8_t fix_channel, uint8_t offset);

/**
 * @brief Set Afh Policy Priority Command
 *
 * @param lea_conn_handle(2 byte)  CIS Connect Handle
 * @param afh policy priority (1 byte)
 * remote first = 0, local first = 1
 * @return true  success
 * @return false fail
 */
bool app_vendor_send_psd_policy(uint16_t lea_conn_handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _APP_VENDOR_H_ */
