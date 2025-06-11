#ifndef _APP_GFPS_FINDER_H_
#define _APP_GFPS_FINDER_H_

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "gfps.h"
#include "app_gfps_rfc.h"
#include "gfps_find_my_device.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief beacon state
 * GFPS_FINDER_BEACON_STATE_OFF  stop beacon adv and stop beacon timer
 * GFPS_FINDER_BEACON_STATE_ON   start beacon adv and start beacon timer
 */
typedef enum
{
    GFPS_FINDER_BEACON_STATE_OFF = 0x00,
    GFPS_FINDER_BEACON_STATE_ON  = 0x01,
} T_GFPS_FINDER_BEACON_STATE;

/**
 * @brief app_gfps_finder_init
 * init gfps finder struct.
 * @return true
 * @return false
 */
bool app_gfps_finder_init(void);

/**
 * @brief app_gfps_finder_handle_factory_reset
 * If a factory reset is performed,
 * beaconing should be disabled and the Ephemeral Identity Key
 * as well as the owner Account Key used to provision it (as described above) should be wiped out.
 */
void app_gfps_finder_handle_factory_reset(void);

/**
 * @brief app_gfps_finder_handle_power_off
 * it is important that the Provider is able to recover its clock value in the event of power loss.
 * We recommend that it writes its current clock value to non-volatile memory.
 */
void app_gfps_finder_handle_power_off(void);

/**
 * @brief app_gfps_finder_enter_beacon_state
 * used to start or stop beacon adv(gfps_finder_adv)
 * @param beacon_state  @ref T_GFPS_FINDER_BEACON_STATE
 */
void app_gfps_finder_enter_beacon_state(T_GFPS_FINDER_BEACON_STATE beacon_state);

/**
 * @brief app_gfps_finder_beacon_set_conn_id
 * set gfps finder beacon ble link connection id
 * @param conn_id
 */
void app_gfps_finder_beacon_set_conn_id(uint8_t conn_id);

/**
 * @brief app_gfps_finder_beacon_get_conn_id
 * get gfps finder beacon ble link connection id
 * @return uint8_t
 */
uint8_t app_gfps_finder_beacon_get_conn_id(void);

/**
 * @brief app_gfps_finder_beacon_le_disconnect_cb
 * gfps finder beacon BLE link disconnect callback
 * @param conn_id
 * @param local_disc_cause
 * @param disc_cause
 */
void app_gfps_finder_beacon_le_disconnect_cb(uint8_t conn_id,
                                             uint8_t local_disc_cause,
                                             uint16_t disc_cause);
/**
 * @brief app_gfps_finder_send_ring_rsp
 * When ringing starts or terminates a notification is sent to seeker.
 * @param ring_state @ref T_GFPS_FINDER_RING_STATE
 * @return true
 * @return false
 */
bool app_gfps_finder_send_ring_rsp(uint8_t ring_state);

/**
 * @brief app_gfps_finder_reset_conn
 * reset conn_id when ble link disconnect.
 */
void app_gfps_finder_reset_conn(void);


T_GFPS_FINDER_RING_VOLUME_LEVEL app_gfps_finder_get_ring_volume_level(void);

void app_gfps_finder_set_ring_param(void);

bool app_gfps_finder_provisoned(void);

/**
 * @brief get utp mode
 *
 * @return true active utp mode
 * @return false deactive utp mode
 */
bool app_gfps_finder_get_utp_mode(void);

/**
 * @brief send eddystone capability when RFCOMM connected
 *
 * @param bd_addr bd address
 * @param local_server_chann server channel
 */
void app_gfps_finder_send_eddystone_capability(uint8_t *bd_addr, uint8_t local_server_chann);

/**
 * @brief when system wakeup by RTC, gfps finder need update clock value and start finder adv
 *
 * @param none
 */
void app_gfps_finder_handle_event_wakeup_by_rtc(void);

/**
 * @brief when system wakeup, gfps finder need update clock value
 *
 * @param clock_value_delayed delayed clock value
 */
void app_gfps_finder_update_clock_value(uint32_t clock_value_delayed);

/**
 * @brief get ephemeral identifier data
 *
 * @return ephemeral identifier data
 */
void app_gfps_get_ei(uint8_t *p_ei);

/**
 * @brief finder save information when enter powerdown mode
 *
 */
void app_gfps_finder_rtc_wakeup_save_info(void);

/**
 * @brief get finder provisioned state
 *
 */
bool app_gfps_finder_provisioned(void);

/**
 * @brief update hash flag when battery level changed or UTP mode changed
 *
 */
void app_gfps_finder_update_hash(void);

/**
 * @brief update battery level
 *
 * @param local_battery_level
 */
void app_gfps_finder_update_battery(uint8_t local_battery_level);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
