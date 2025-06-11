#ifndef _APP_HARMAN_BEHAVIORS_H_
#define _APP_HARMAN_BEHAVIORS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HARMAN_NOTIFY_DEVICE_INFO_TIME      500

#define HARMAN_HEARTBEAT_CHECK_TIME         2500
#define HARMAN_HEARTBEAT_CHECK_MAX_TIME     5000

typedef enum
{
    CONNECT_IDLE_POWER_OFF_START    = 0,
    DISC_RESET_TO_COUNT             = 1,
    ACTIVE_NEED_STOP_COUNT          = 2,
} T_CONNECT_IDLE_POWER_OFF;

void app_harman_set_cpu_clk(bool evt);
void app_harman_cpu_clk_improve(void);
void app_harman_behaviors_init(void);

uint8_t app_harman_power_off_option_timer_get(void);
void app_harman_notify_device_info_timer_start(uint32_t time);
void app_harman_notify_device_info_timer_stop(void);

void app_harman_aling_active_hfp_idx_set(uint8_t active_idx);
uint8_t app_harman_aling_active_hfp_idx_get(void);
void app_harman_silent_check(uint8_t set_idx, uint8_t silent_check);
void app_harman_is_record_a2dp_active_ever_set(bool res);
bool app_harman_is_record_a2dp_active_ever_get(void);
void app_harman_power_on_link_back_flag_set(bool res);
bool app_harman_power_on_link_back_flag_get(void);
void app_harman_connect_idle_to_power_off(T_CONNECT_IDLE_POWER_OFF action, uint8_t index);
void app_harman_dump_link_information(void);
uint8_t app_harman_ever_link_information(void);
void app_harman_pairable_mode_set(uint8_t enable);
void app_harman_is_already_connect_one_set(bool res);
bool app_harman_is_already_connect_one_get(void);

void app_harman_acl_disconn_handle(uint8_t *bd_addr);
void app_harman_sco_handle(uint8_t app_idx, bool is_sco_conn);
void app_harman_a2dp_stop_handle(uint8_t app_idx);

void app_harman_heartbeat_check_timer_start(uint32_t time);
void app_harman_heartbeat_check_timer_stop(void);
void app_harman_heartbeat_check_times_set(uint8_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
