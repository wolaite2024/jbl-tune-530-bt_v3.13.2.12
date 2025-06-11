#ifndef _APP_GFPS_LINK_H_
#define _APP_GFPS_LINK_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GFPS_LE_MAX_LINK_NUMBER 2

/**
 * @brief gfps priority queue init
 *
 */
void app_gfps_link_priority_queue_init(void);

/**
 * @brief add gfps link connection id into priority queue
 *
 * @param conn_id gfps link connection id
 */
void app_gfps_link_add_link_into_priority_queue(uint8_t conn_id);

/**
 * @brief delete the oldest link in queue
 *
 * @return uint8_t  connection id which will be delete
 */
uint8_t app_gfps_link_delete_oldest_link_from_priority_queue(void);

/**
 * @brief delete link by connection id
 *
 * @param conn_id connection id
 */
void app_gfps_link_delete_link_from_priority_queue(uint8_t conn_id);

/**
 * @brief disconnect all gfps link
 *
 */
void app_gfps_link_disconnect_all_gfps_link(void);

/**
 * @brief find connected gfps ble link number
 *
 * @return uint8_t ble link number
 */
uint8_t app_gfps_link_find_gfps_link(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
