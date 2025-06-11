#if GFPS_FINDER_SUPPORT
#include "ftl.h"
#include "btm.h"
#include "trace.h"
#include "string.h"
#include "stdlib.h"
#include "app_cfg.h"
#include "gfps_find_my_device.h"
#include "app_gfps_finder.h"
#include "app_gfps_finder_adv.h"
#include "app_bt_policy_api.h"
#include "app_gfps_msg.h"
#include "bt_gfps.h"
#include "app_main.h"
#include "app_timer.h"
#include "platform_secp160r1.h"
#include "sha256.h"
#include "app_adv_stop_cause.h"
#include "app_ble_gap.h"
#include "platform_utils.h"
#include "app_gfps.h"
#include "app_gfps_account_key.h"
#include "pm.h"
#include "app_dult.h"
#include "app_dlps.h"
#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_behaviors.h"
#endif

#define GFPS_FINDER_EIK_FLASH_OFFSET    0xA5C
#define GFPS_FINDER_EIK_LEN             sizeof(T_GFPS_EIK)
#define GFPS_FINDER_CLOCK_FLASH_OFFSET  (GFPS_FINDER_EIK_FLASH_OFFSET + GFPS_FINDER_EIK_LEN)
#define GFPS_FINDER_CLOCK_LEN           0x04

typedef enum
{
    GFPS_FINDER_UPDATE_ADV_EI,
    GFPS_FINDER_UPDATE_RANDOM_WINDOW,
    GFPS_FINDER_UTP_ENABLE_UPDATE_RPA,

    GFPS_FINDER_TOTAL
} T_GFPS_FINDER_TIMER;

typedef struct
{
    T_GFPS_FINDER *p_finder;
    uint8_t timer_idx_finder_update_ei;
    uint8_t timer_idx_finder_update_random_window;
    uint8_t timer_idx_finder_utp_enable_update_rpa;
    uint8_t gfps_finder_timer_id;
    uint32_t gfps_finder_timeout_ms;
    /*gfps finder current BLE link connection id: maybe GFPS ble link or finder beacon BLE link*/
    uint8_t gfps_finder_conn_id;
    uint8_t gfps_finder_service_id;
    uint8_t gfps_finder_ring_volume_level;
    /*gfps finder beacon BLE link connection id*/
    uint8_t gfps_finder_beacon_conn_id;
} T_APP_GFPS_FINDER;

typedef enum
{
    EDDYSTONE_PROVISION_STATE_UNPROVISIONED = 0x00,
    EDDYSTONE_PROVISION_STATE_PROVISIONED = 0x01,
} T_EDDYSTONE_PROVISION_STATE;

T_APP_GFPS_FINDER *p_app_gfps_finder = NULL;

void app_gfps_finder_set_default_value(void)
{
    p_app_gfps_finder->p_finder = NULL;
    p_app_gfps_finder->timer_idx_finder_update_ei = 0;
    p_app_gfps_finder->timer_idx_finder_update_random_window = 0;
    p_app_gfps_finder->timer_idx_finder_utp_enable_update_rpa = 0;
    p_app_gfps_finder->gfps_finder_timer_id = 0;
    p_app_gfps_finder->gfps_finder_timeout_ms = 1000;//1s
    p_app_gfps_finder->gfps_finder_conn_id = 0xFF;
    p_app_gfps_finder->gfps_finder_beacon_conn_id = 0xFF;
    p_app_gfps_finder->gfps_finder_service_id = 0xFF;
}

void app_gfps_finder_generate_hash_flag(uint8_t *r, uint8_t len, uint8_t *hash)
{
    uint8_t local_batt_level = app_db.local_batt_level;

    if (local_batt_level < 5)
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_CRITICALLY_LOW;
    }
    else if (local_batt_level < 10)
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_LOW;
    }
    else
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_NORMAL;
    }

    APP_PRINT_INFO2("app_gfps_finder_generate_hash_flag: app_db.local_batt_level %d%, hash_flag.battery_level %d",
                    app_db.local_batt_level, p_app_gfps_finder->p_finder->hash_flag.battery_level);

    uint8_t input = 0;
    uint8_t output[32] = {0};

    SHA256(r + 1, 20, output);
    memcpy(&input, &p_app_gfps_finder->p_finder->hash_flag, 1);

    /*To produce the final value of this byte, it should be xor-ed with the least significant byte of
     SHA256(r).
     the least significant byte is output[0] or output[31] ????????????*/
    *hash = input ^ output[31];
    APP_PRINT_INFO4("app_gfps_finder_generate_hash_flag: sha256 input %b, sha256 output %b, hash input 0x%x, hash result 0x%x",
                    TRACE_BINARY(20, r + 1), TRACE_BINARY(32, output), input, *hash);
}

/**
 * @brief app_gfps_finder_generate_adv_ei_and_hash
 *
 * @param p_adv_ei[out]    20 bytes EI
 * @param p_eik[in]        32 bytes EIK
 * @param counter_value[in]  counter_value = (clock_value / 1024) * 1024
 */
void app_gfps_finder_generate_adv_ei_and_hash(uint8_t *p_adv_ei, uint8_t *p_eik,
                                              uint32_t counter_value, uint8_t *hash)
{
#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_set_cpu_clk(true);
#endif

    /*beacon_data[0] - beacon_data[10] Padding, Value = 0xFF*/
    uint8_t padding_len = 11;
    uint8_t K = 10;
    uint8_t *p = p_app_gfps_finder->p_finder->beacon_data;
    uint8_t r1[32] = {0};// r' in google spec
    uint8_t r[21] = {0};
    uint8_t r_len = 21;
    uint8_t adv_ei[20] = {0};

    memset(p, 0xFF, padding_len);
    p += padding_len;

    BE_UINT8_TO_STREAM(p, K);
    BE_UINT32_TO_STREAM(p, counter_value);

    /*beacon_data[16] - beacon_data[26] Padding, Value = 0x00*/
    memset(p, 0x00, padding_len);
    p += padding_len;

    BE_UINT8_TO_STREAM(p, K);
    BE_UINT32_TO_STREAM(p, counter_value);

    gfps_finder_encrypted_beacon_data(p_eik, p_app_gfps_finder->p_finder->beacon_data, r1);

    /*r1 equal to r' in google spec, r = r' mod n*/
    platform_uecc_compute_public_key(r1, adv_ei, r, SECP160R1);
    memcpy(p_adv_ei, adv_ei, 20);
    APP_PRINT_TRACE2("gfps_finder: adv_ei %b, r %b", TRACE_BINARY(20, adv_ei), TRACE_BINARY(21, r));

    //r will be get from platform_uecc_compute_public_key()
    app_gfps_finder_generate_hash_flag(r, r_len, hash);

#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_set_cpu_clk(false);
#endif
}

/**
 * @brief start timer and set timeout value to 1000ms.
 * every 1024 seconds update adv EI and update RPA in deactive UTP mode.
 *
 * Rotation should happen every 1024 seconds on average. It is required that the
 * precise point at which the beacon starts advertising the new identifier is randomized within the
 * window.
 */
void app_gfps_finder_start_update_adv_ei_timer()
{
    APP_PRINT_TRACE0("app_gfps_finder_start_update_adv_ei_timer");
    app_start_timer(&p_app_gfps_finder->timer_idx_finder_update_ei, "gfps_finder_update_ei",
                    p_app_gfps_finder->gfps_finder_timer_id, GFPS_FINDER_UPDATE_ADV_EI, 0,
                    false, p_app_gfps_finder->gfps_finder_timeout_ms);
}

/**
 * @brief stop update adv ei timer
 *
 */
void app_gfps_finder_stop_update_adv_ei_timer()
{
    APP_PRINT_TRACE0("app_gfps_finder_stop_update_adv_ei_timer");
    app_stop_timer(&p_app_gfps_finder->timer_idx_finder_update_ei);
}

/**
 * @brief start timer and set timeout value to platform_random(204).
 * Add random window, and when timeout provider should update adv EI.
 *
 * The recommended size of the randomization window is 20% of the interval, i.e. 204 seconds.
 * So when the device needs to compute the next time point to rotate EID/MAC,
 * it's going to be floor((last_rotation_time_seconds + 1024) / 1024) * 1024 + random(0, 204).
 * Rotation should happen every 1024 seconds on average. It is required that the
 * precise point at which the beacon starts advertising the new identifier is randomized within the
 * window.
 */
void app_gfps_finder_start_update_random_window_timer()
{
    uint8_t random_value = platform_random(204);
    APP_PRINT_TRACE1("app_gfps_finder_start_update_random_window_timer: random_value %d", random_value);
    app_start_timer(&p_app_gfps_finder->timer_idx_finder_update_random_window,
                    "gfps_finder_update_random_window",
                    p_app_gfps_finder->gfps_finder_timer_id,
                    GFPS_FINDER_UPDATE_RANDOM_WINDOW, 0,
                    false, random_value * 1000);
}

/**
 * @brief stop update random window timer
 *
 */
void app_gfps_finder_stop_update_random_window_timer()
{
    APP_PRINT_TRACE0("app_gfps_finder_stop_update_random_window_timer");
    app_stop_timer(&p_app_gfps_finder->timer_idx_finder_update_random_window);
}

/**
 * @brief start timer and set timeout value to 24h.
 * when timeout update RPA.
 */
void app_gfps_finder_upt_enable_start_update_rpa_timer()
{
    uint32_t timeout = 0x05265C00;//24 hours
    APP_PRINT_TRACE0("app_gfps_finder_upt_enable_start_update_rpa_timer");
    app_start_timer(&p_app_gfps_finder->timer_idx_finder_utp_enable_update_rpa,
                    "gfps_finder_update_rpa",
                    p_app_gfps_finder->gfps_finder_timer_id, GFPS_FINDER_UTP_ENABLE_UPDATE_RPA, 0,
                    false, timeout);
}

/**
 * @brief stop update rpa timer
 *
 */
void app_gfps_finder_upt_enable_stop_rpa_timer()
{
    APP_PRINT_TRACE0("app_gfps_finder_upt_enable_stop_rpa_timer");
    app_stop_timer(&p_app_gfps_finder->timer_idx_finder_utp_enable_update_rpa);
}

void app_gfps_finder_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    uint32_t old_counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;
    uint32_t new_counter = old_counter;

    switch (timer_id)
    {
    case GFPS_FINDER_UPDATE_ADV_EI:
        {
            {
                if (p_app_gfps_finder->p_finder->clock_value > 0xFFFFFC00)
                {
                    p_app_gfps_finder->p_finder->clock_value = 0;
                }
                else
                {
                    p_app_gfps_finder->p_finder->clock_value++;
                }

                new_counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;

                if (old_counter != new_counter)
                {
                    app_gfps_finder_start_update_random_window_timer();
                }

                /*Phone will caucalate a current_clock_value
                if headset clock value in range of [current_clock_value - 17*1024, current_clock_value + 17*1024],
                phone can successfully detect the headset and caucalate EID.*/
                APP_PRINT_TRACE3("app_gfps_finder_timeout_cb: timer_id %d, current_clock_value %d, new_counter %d",
                                 timer_id, p_app_gfps_finder->p_finder->clock_value, new_counter);

                app_gfps_finder_start_update_adv_ei_timer();
            }
        }
        break;

    case GFPS_FINDER_UPDATE_RANDOM_WINDOW:
        {
            if (p_app_gfps_finder->timer_idx_finder_update_random_window != NULL)
            {
                APP_PRINT_TRACE0("app_gfps_finder_timeout_cb: GFPS_FINDER_UPDATE_RANDOM_WINDOW update adv ei");
                uint8_t hash;
                uint8_t old_adv_ei[20];//old EID
                memcpy(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20);

                app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                                         p_app_gfps_finder->p_finder->eik.key, new_counter, &hash);

                if (memcmp(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20) != 0)
                {
                    //old EID not equal to new EID
                    gfps_finder_adv_update_adv_ei_hash(p_app_gfps_finder->p_finder->adv_ei, hash);
                    /*if device not in active UTP mode, update RPA at the same time when update adv EI*/
                    if (app_gfps_finder_get_utp_mode() == false)
                    {
                        gfps_finder_adv_update_rpa();
                    }
                };

                if ((app_db.wake_up_reason & WAKE_UP_RTC) &&
                    (!extend_app_cfg_const.disable_finder_adv_when_power_off))
                {
                    uint16_t adv_duration = extend_app_cfg_const.gfps_power_off_finder_adv_duration * 100; //uint 10ms
                    uint32_t adv_interval = extend_app_cfg_const.gfps_power_off_finder_adv_interval;
                    gfps_finder_adv_update_adv_interval(adv_interval);
                    gfps_finder_adv_start(adv_duration);
                }
                else
                {
                    uint32_t adv_interval = extend_app_cfg_const.gfps_power_on_finder_adv_interval;
                    gfps_finder_adv_update_adv_interval(adv_interval);
                    gfps_finder_adv_start(0);
                }
            }
        }
        break;

    case GFPS_FINDER_UTP_ENABLE_UPDATE_RPA:
        {
            {
                APP_PRINT_TRACE0("app_gfps_finder_timeout_cb: GFPS_FINDER_UTP_ENABLE_UPDATE_RPA");

                if (p_app_gfps_finder->p_finder->hash_flag.utp_mode == 1)
                {
                    gfps_finder_adv_update_rpa();
                }

                app_gfps_finder_upt_enable_start_update_rpa_timer();
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief app_gfps_finder_handle_factory_reset
 * If a factory reset is performed,
 * beaconing should be disabled and the Ephemeral Identity Key
 * as well as the owner Account Key used to provision it should be wiped out.
 *
 */
void app_gfps_finder_handle_factory_reset(void)
{
    uint8_t ret_eik   = 0;
    uint8_t ret_clock = 0;
    uint8_t ret_value = 0;

    memset(&p_app_gfps_finder->p_finder->eik, 0, sizeof(T_GFPS_EIK));
    ret_eik = ftl_save_to_storage(&p_app_gfps_finder->p_finder->eik, GFPS_FINDER_EIK_FLASH_OFFSET,
                                  sizeof(T_GFPS_EIK));

    memset(&p_app_gfps_finder->p_finder->clock_value, 0, 4);
    ret_clock =  ftl_save_to_storage(&p_app_gfps_finder->p_finder->clock_value,
                                     GFPS_FINDER_CLOCK_FLASH_OFFSET, 4);

    if (ret_eik || ret_clock)
    {
        ret_value = 1;
    }
    APP_PRINT_TRACE1("app_gfps_finder_handle_factory_reset: ret %s",
                     ((ret_value == 0) ? TRACE_STRING("success") : TRACE_STRING("fail")));
}

/**
 * @brief app_gfps_finder_handle_power_off
 * 1.it is important that the Provider is able to recover its clock value in the event of power loss.
 * We recommend that it writes its current clock value to non-volatile memory.
 * 2.disable finder beacon adv and disconnect finder beacon ble link
 */
void app_gfps_finder_handle_power_off(void)
{
    uint8_t ret_clock = 0;
    ret_clock = ftl_save_to_storage(&p_app_gfps_finder->p_finder->clock_value,
                                    GFPS_FINDER_CLOCK_FLASH_OFFSET, 4);
    APP_PRINT_TRACE1("app_gfps_finder_handle_power_off: ret %s",
                     ((ret_clock == 0) ? TRACE_STRING("success") : TRACE_STRING("fail")));
    if (extend_app_cfg_const.disable_finder_adv_when_power_off == false)
    {
        uint32_t adv_interval = extend_app_cfg_const.gfps_power_off_finder_adv_interval;
        gfps_finder_adv_update_adv_interval(adv_interval);
    }

    T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_OFF;
    app_gfps_finder_enter_beacon_state(beacon_state);

    if (p_app_gfps_finder->gfps_finder_beacon_conn_id != 0xFF)
    {
        T_APP_LE_LINK *p_link;
        p_link = app_link_find_le_link_by_conn_id(p_app_gfps_finder->gfps_finder_beacon_conn_id);

        if (p_link != NULL)
        {
            app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_GFPS_FINDER);
        }
    }

    //app_gfps_finder_upt_enable_stop_rpa_timer();
    //app_gfps_finder_stop_update_adv_ei_timer();
    //app_gfps_finder_stop_update_random_window_timer();
}

void app_gfps_finder_rtc_wakeup_save_info(void)
{
    uint8_t ret_clock = 0;
    ret_clock = ftl_save_to_storage(&p_app_gfps_finder->p_finder->clock_value,
                                    GFPS_FINDER_CLOCK_FLASH_OFFSET, 4);
    APP_PRINT_TRACE1("app_gfps_finder_rtc_wakeup_save_info: ret %s",
                     ((ret_clock == 0) ? TRACE_STRING("success") : TRACE_STRING("fail")));
}

/**
 * @brief app_gfps_finder_beacon_set_conn_id
 * set gfps finder beacon ble link connection id
 * @param conn_id
 */
void app_gfps_finder_beacon_set_conn_id(uint8_t conn_id)
{
    APP_PRINT_TRACE1("app_gfps_finder_beacon_set_conn_id: conn_id %d", conn_id);
    p_app_gfps_finder->gfps_finder_beacon_conn_id = conn_id;
}

/**
 * @brief app_gfps_finder_beacon_get_conn_id
 * get gfps finder beacon ble link connection id
 * @return uint8_t
 */
uint8_t app_gfps_finder_beacon_get_conn_id(void)
{
    return p_app_gfps_finder->gfps_finder_beacon_conn_id;
}

/**
 * @brief app_gfps_finder_beacon_le_disconnect_cb
 * gfps finder beacon BLE link disconnect callback
 * @param conn_id
 * @param local_disc_cause
 * @param disc_cause
 */
void app_gfps_finder_beacon_le_disconnect_cb(uint8_t conn_id,
                                             uint8_t local_disc_cause,
                                             uint16_t disc_cause)
{
    APP_PRINT_TRACE4("app_gfps_finder_beacon_le_disconnect_cb: conn_id %d, beacon_conn_id %d,local_disc_cause %d, disc_cause 0x%x",
                     conn_id, p_app_gfps_finder->gfps_finder_beacon_conn_id, local_disc_cause, disc_cause);

    if (conn_id == p_app_gfps_finder->gfps_finder_beacon_conn_id)
    {
        p_app_gfps_finder->gfps_finder_beacon_conn_id = 0xFF;
        app_gfps_finder_reset_conn();
        app_dlps_start_power_down_wdg_timer();
    }
}

/**
 * @brief app_gfps_finder_enter_beacon_state
 * used to start or stop beacon adv(gfps_finder_adv)
 *
 * @param beacon_state  @ref T_GFPS_FINDER_BEACON_STATE
 */
void app_gfps_finder_enter_beacon_state(T_GFPS_FINDER_BEACON_STATE beacon_state)
{
    T_BLE_EXT_ADV_MGR_STATE adv_state = gfps_finder_adv_get_adv_state();
    APP_PRINT_INFO3("app_gfps_finder_enter_beacon_state %d, eik.valid %d, adv_state %d",
                    beacon_state, p_app_gfps_finder->p_finder->eik.valid, adv_state);

    if (beacon_state == GFPS_FINDER_BEACON_STATE_ON)
    {
        if (p_app_gfps_finder->p_finder->eik.valid)
        {
            uint8_t hash;
            uint32_t counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;

            uint8_t old_adv_ei[20];//old EID
            memcpy(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20);

            app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                                     p_app_gfps_finder->p_finder->eik.key, counter, &hash);

            if (memcmp(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20) != 0)
            {
                //old EID not equal to new EID
                gfps_finder_adv_update_adv_ei_hash(p_app_gfps_finder->p_finder->adv_ei, hash);
                /*if device not in active UTP mode, update RPA at the same time when update adv EI*/
                if (app_gfps_finder_get_utp_mode() == false)
                {
                    gfps_finder_adv_update_rpa();
                }
            };

            gfps_finder_adv_start(0);
            app_gfps_finder_start_update_adv_ei_timer();
        }
    }
    else if (beacon_state == GFPS_FINDER_BEACON_STATE_OFF)
    {
        if (adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
        {
            gfps_finder_adv_stop(APP_STOP_ADV_CAUSE_GFPS_FINDER);
        }
    }
}

/**
 * @brief app_gfps_finder_send_ring_rsp
 * When ringing starts or terminates a notification is sent to seeker.
 * @param ring_state @ref T_GFPS_FINDER_RING_STATE
 * @return true
 * @return false
 */
bool app_gfps_finder_send_ring_rsp(uint8_t ring_state)
{
    bool ret = false;

    if ((p_app_gfps_finder->gfps_finder_conn_id == 0xFF) ||
        (p_app_gfps_finder->gfps_finder_service_id == 0xFF))
    {
        APP_PRINT_ERROR0("app_gfps_finder_send_ring_rsp: invalid conn");
        return false;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        T_GFPS_MSG_RING_STATE ring_type = app_gfps_msg_get_ring_state();
        uint16_t ring_time = app_gfps_msg_get_ring_state();

        ret = gfps_finder_rsp_ring_request(p_app_gfps_finder->gfps_finder_conn_id,
                                           p_app_gfps_finder->gfps_finder_service_id,
                                           ring_state, ring_type, ring_time);
    }
    return ret;
}

/**
 * @brief app_gfps_finder_reset_conn
 * reset conn_id when ble link disconnected.
 * this ble link maybe gfps ble link or gfps finder beacon ble link.
 * if seeker modify EIK, The new Ephemeral Identity Key
 * should take effect immediately after the BLE connection is terminated.
 */
void app_gfps_finder_reset_conn(void)
{
    APP_PRINT_INFO3("app_gfps_finder_reset_conn: temp_modified_eik.valid %d, device_state %d, provisioned %d",
                    p_app_gfps_finder->p_finder->temp_modified_eik.valid,
                    app_db.device_state, app_gfps_finder_provisioned());

    if (p_app_gfps_finder->p_finder->temp_modified_eik.valid == true)
    {
        memcpy(p_app_gfps_finder->p_finder->eik.key, p_app_gfps_finder->p_finder->temp_modified_eik.key,
               32);
        p_app_gfps_finder->p_finder->temp_modified_eik.valid = false;
        ftl_save_to_storage(&p_app_gfps_finder->p_finder->eik, GFPS_FINDER_EIK_FLASH_OFFSET,
                            sizeof(T_GFPS_EIK));
        //if EIK update, we shall stop finder adv and reopen it again to make sure using the correct EIK
        gfps_finder_adv_stop(APP_STOP_ADV_CAUSE_GFPS_FINDER_UPDATE_EIK);
    }

    //in power off state, when ble link disconnected, need enter power down mode
    if (app_db.device_state != APP_DEVICE_STATE_ON &&
        !extend_app_cfg_const.disable_finder_adv_when_power_off
        && app_gfps_finder_provisioned())
    {
        power_mode_set(POWER_POWERDOWN_MODE);
    }
    else
    {
        //in power on state, when ble link disconnected, need restart finder adv
        T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_ON;
        app_gfps_finder_enter_beacon_state(beacon_state);
    }
};

void app_gfps_finder_set_ring_param(void)
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        p_app_gfps_finder->p_finder->ring_components_num = GFPS_FINDER_RING_RWS;
    }
    else
    {
        p_app_gfps_finder->p_finder->ring_components_num = GFPS_FINDER_RING_SINGLE;
    }

    p_app_gfps_finder->p_finder->ring_volume_modify = GFPS_FINDER_RING_VOLUME_ENABLE;
}

T_GFPS_FINDER_RING_VOLUME_LEVEL app_gfps_finder_get_ring_volume_level(void)
{
    return (T_GFPS_FINDER_RING_VOLUME_LEVEL)p_app_gfps_finder->gfps_finder_ring_volume_level;
}

void app_gfps_finder_send_eddystone_capability(uint8_t *bd_addr, uint8_t local_server_chann)
{
    bool ret = true;
    uint8_t ble_addr[6] = {0};
    uint8_t additional_data[7] = {0};
    uint8_t additional_data_len = 7;
    uint8_t *p = additional_data;

    if (p_app_gfps_finder->p_finder->eik.valid)
    {
        BE_UINT8_TO_STREAM(p, EDDYSTONE_PROVISION_STATE_PROVISIONED);
    }
    else
    {
        BE_UINT8_TO_STREAM(p, EDDYSTONE_PROVISION_STATE_UNPROVISIONED);
    }

    app_gfps_get_random_addr(ble_addr);
    app_gfps_msg_reverse_data(ble_addr, 6);
    memcpy(p, ble_addr, 6);

    ret = bt_gfps_send_eddystone_capability(0xFF, 0, bd_addr, local_server_chann,
                                            additional_data, additional_data_len);
    APP_PRINT_INFO1("app_gfps_finder_send_eddystone_capability: ret %d", ret);
}

void app_gfps_finder_update_clock_value(uint32_t clock_value_delayed)
{
    if (p_app_gfps_finder->p_finder->eik.valid)
    {
        uint32_t old_clock_value = p_app_gfps_finder->p_finder->clock_value;

        p_app_gfps_finder->p_finder->clock_value += clock_value_delayed;//unit 1s

        if (p_app_gfps_finder->p_finder->clock_value > 0xFFFFFC00)
        {
            p_app_gfps_finder->p_finder->clock_value = 0;
        }

        APP_PRINT_INFO2("app_gfps_finder_update_clock_value: start finder adv, old clock %d, new clock %d",
                        old_clock_value, p_app_gfps_finder->p_finder->clock_value);

    }
}

bool app_gfps_finder_provisioned(void)
{
    return p_app_gfps_finder->p_finder->eik.valid;
}

void app_gfps_finder_handle_event_wakeup_by_rtc(void)
{
    if (p_app_gfps_finder->p_finder->eik.valid)
    {
        T_BLE_EXT_ADV_MGR_STATE adv_state = gfps_finder_adv_get_adv_state();

        if (adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
        {
            uint8_t hash;
            uint32_t counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;
            uint8_t old_adv_ei[20];//old EID
            memcpy(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20);

            app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                                     p_app_gfps_finder->p_finder->eik.key, counter, &hash);

            if (memcmp(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20) != 0)
            {
                //old EID not equal to new EID
                gfps_finder_adv_update_adv_ei_hash(p_app_gfps_finder->p_finder->adv_ei, hash);
                /*if device not in active UTP mode, update RPA at the same time when update adv EI*/
                if (app_gfps_finder_get_utp_mode() == false)
                {
                    gfps_finder_adv_update_rpa();
                }
            };

            uint16_t adv_duration = extend_app_cfg_const.gfps_power_off_finder_adv_duration * 100; //uint 10ms
            uint32_t adv_interval = extend_app_cfg_const.gfps_power_off_finder_adv_interval;
            gfps_finder_adv_update_adv_interval(adv_interval);
            gfps_finder_adv_start(adv_duration);
            app_gfps_finder_start_update_adv_ei_timer();
        }
    }
}

void app_gfps_finder_update_hash(void)
{
    uint8_t hash;//encrypted hash value
    T_GFPS_FINDER_HASH_FLAG hash_flag;

    uint32_t counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;

    app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                             p_app_gfps_finder->p_finder->eik.key,
                                             counter, &hash);
    gfps_finder_adv_update_hash(hash);

    hash_flag = p_app_gfps_finder->p_finder->hash_flag;

    APP_PRINT_INFO2("app_gfps_finder_update_hash: hash 0x%x, hash_flag 0x%x", hash, hash_flag);
}

void app_gfps_finder_update_battery(uint8_t local_battery)
{
    uint8_t new_battery_level = 0;
    uint8_t old_battery_level = (uint8_t)p_app_gfps_finder->p_finder->hash_flag.battery_level;

    if (local_battery < 5)
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_CRITICALLY_LOW;
    }
    else if (local_battery < 10)
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_LOW;
    }
    else
    {
        p_app_gfps_finder->p_finder->hash_flag.battery_level = GFPS_FINDER_BATTERY_NORMAL;
    }

    new_battery_level = p_app_gfps_finder->p_finder->hash_flag.battery_level;

    if (old_battery_level != new_battery_level)
    {
        app_gfps_finder_update_hash();
    }

    APP_PRINT_INFO3("app_gfps_finder_update_battery: local_battery %d%, old_battery_level %d, new_battery_level %d",
                    local_battery, old_battery_level, new_battery_level);
}

/**
 * @brief app_gfps_finder_cb
 * receive event from gfps_lib then handle it.
 * @param p_data
 * @return T_GFPS_FINDER_CAUSE
 */
T_GFPS_FINDER_CAUSE app_gfps_finder_cb(T_GFPS_FINDER_CB_DATA *p_data)
{
    T_GFPS_FINDER_CAUSE cause = GFPS_FINDER_CAUSE_SUCCESS;

    T_GFPS_FINDER_CB_DATA data;
    memcpy(&data, p_data, sizeof(T_GFPS_FINDER_CB_DATA));

    uint8_t evt = data.evt;
    uint8_t ret_err = 0;
    APP_PRINT_INFO1("app_gfps_finder_cb: evt %d", evt);

    switch (evt)
    {
    case GFPS_FINDER_EVT_SET_EIK:
        {
            memcpy(&p_app_gfps_finder->p_finder->eik, &data.msg_data.eik, sizeof(T_GFPS_EIK));

            int8_t save_ret = ftl_save_to_storage(&p_app_gfps_finder->p_finder->eik,
                                                  GFPS_FINDER_EIK_FLASH_OFFSET,
                                                  sizeof(T_GFPS_EIK));
            if (save_ret)
            {
                ret_err = 1;
                goto err;
            };

            T_BLE_EXT_ADV_MGR_STATE adv_state = gfps_finder_adv_get_adv_state();
//            if (adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                uint8_t hash;
                uint32_t counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;

                uint8_t old_adv_ei[20];//old EID
                memcpy(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20);

                app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                                         p_app_gfps_finder->p_finder->eik.key, counter, &hash);

                if (memcmp(old_adv_ei, p_app_gfps_finder->p_finder->adv_ei, 20) != 0)
                {
                    //old EID not equal to new EID
                    gfps_finder_adv_update_adv_ei_hash(p_app_gfps_finder->p_finder->adv_ei, hash);
                    /*if device not in active UTP mode, update RPA at the same time when update adv EI*/
                    if (app_gfps_finder_get_utp_mode() == false)
                    {
                        gfps_finder_adv_update_rpa();
                    }
                };

                gfps_finder_adv_start(0);
                app_gfps_finder_start_update_adv_ei_timer();
            }

            T_GFPS_FINDER test;
            memset(&test, 0, sizeof(T_GFPS_FINDER));
            int8_t load_ret = ftl_load_from_storage(&test.eik, GFPS_FINDER_EIK_FLASH_OFFSET,
                                                    sizeof(T_GFPS_EIK));
            if (load_ret)
            {
                APP_PRINT_ERROR1("app_gfps_finder_cb: load eik from ftl fail %d", load_ret);
            };
            APP_PRINT_INFO2("app_gfps_finder_cb: load eik %b, valid %d",
                            TRACE_BINARY(32, test.eik.key), test.eik.valid);

        }
        break;

    case GFPS_FINDER_EVT_CLEAR_EIK:
        {
            memset(&p_app_gfps_finder->p_finder->eik, 0, sizeof(T_GFPS_EIK));
            uint8_t ret = ftl_save_to_storage(&p_app_gfps_finder->p_finder->eik, GFPS_FINDER_EIK_FLASH_OFFSET,
                                              sizeof(T_GFPS_EIK));

            if (ret)
            {
                ret_err = 2;
                goto err;
            }

            T_BLE_EXT_ADV_MGR_STATE adv_state = gfps_finder_adv_get_adv_state();
            if (adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                gfps_finder_adv_stop(APP_STOP_ADV_CAUSE_GFPS_FINDER);
                app_gfps_finder_stop_update_adv_ei_timer();
                app_gfps_finder_stop_update_random_window_timer();
            }

            if (gfps_get_owner_key_valid() == true)
            {
                gfps_set_owner_key_valid(false);
                app_gfps_account_key_save_ownerkey_valid();
            }
        }
        break;

    case GFPS_FINDER_EVT_KEY_RECOVERY:
        {
            T_BT_DEVICE_MODE radio_mode = app_bt_policy_get_radio_mode();

            if (radio_mode != BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
            {
                cause = GFPS_FINDER_CAUSE_NO_USER_CONSENT;
            }
        }
        break;

    case GFPS_FINDER_EVT_RING:
        {
            p_app_gfps_finder->gfps_finder_conn_id = data.msg_data.ring.conn_id;
            p_app_gfps_finder->gfps_finder_service_id = data.msg_data.ring.service_id;

            uint8_t ring_type = data.msg_data.ring.ring_type;
            uint16_t ring_time = data.msg_data.ring.ring_time;
            uint8_t ring_volume_level = data.msg_data.ring.ring_volume_level;

            /*the first byte may hold a special value of 0xFF to ring all components that can ring.*/
            if (ring_type == 0xFF)
            {
                ring_type = GFPS_ALL_RING;
            }

            app_gfps_msg_set_ring_timeout(ring_time);
            app_gfps_msg_handle_ring_event(ring_type);

            if ((ring_volume_level != GFPS_FINDER_RING_VOLUME_DEFAULT) &&
                (p_app_gfps_finder->p_finder->ring_volume_modify == GFPS_FINDER_RING_VOLUME_ENABLE))
            {
                p_app_gfps_finder->gfps_finder_ring_volume_level = ring_volume_level;
            }

            /*A byte indicating the new ringing state*/
            uint8_t ring_state;
            if (ring_type == GFPS_ALL_STOP)
            {
                ring_state = GFPS_FINDER_RING_GATT_STOP;
            }
            else
            {
                ring_state = GFPS_FINDER_RING_STARTED;
            }

            gfps_finder_rsp_ring_request(p_app_gfps_finder->gfps_finder_conn_id,
                                         p_app_gfps_finder->gfps_finder_service_id,
                                         ring_state, ring_type, ring_time);
        }
        break;

    case GFPS_FINDER_EVT_RING_STATE:
        {
            p_app_gfps_finder->gfps_finder_conn_id = data.msg_data.ring.conn_id;
            p_app_gfps_finder->gfps_finder_service_id = data.msg_data.ring.service_id;

            T_GFPS_MSG_RING_STATE ring_type = app_gfps_msg_get_ring_state();
            uint16_t ring_time = app_gfps_msg_get_ring_timeout();

            /*Two bytes indicating the remaining time for ringing in deciseconds*/
            gfps_finder_rsp_ring_state(p_app_gfps_finder->gfps_finder_conn_id,
                                       p_app_gfps_finder->gfps_finder_service_id,
                                       ring_type, ring_time);
        }
        break;

    case GFPS_FINDER_EVT_UTP_ACTIVE:
        {
            app_dult_handle_none_owner_state(DULT_SEPERATED);
            gfps_finder_adv_update_frame_type(0x41);
            app_gfps_finder_upt_enable_start_update_rpa_timer();
            app_gfps_finder_update_hash();
        }
        break;

    case GFPS_FINDER_EVT_UTP_DEACTIVE:
        {
            app_dult_handle_none_owner_state(DULT_NEARING_OWNER);
            gfps_finder_adv_update_frame_type(0x40);
            app_gfps_finder_upt_enable_stop_rpa_timer();
            app_gfps_finder_update_hash();
        }
        break;

    case GFPS_FINDER_EVT_SET_OWNERKEY_VALID:
        {
            app_gfps_account_key_save_ownerkey_valid();
        }
        break;

    default:
        {

        }
        break;
    }
    return cause;

err:
    cause = GFPS_FINDER_CAUSE_INVALID_VALUE;
    APP_PRINT_ERROR1("app_gfps_finder_cb: err %d", ret_err);
    return cause;
}

bool app_gfps_finder_provisoned(void)
{
    return p_app_gfps_finder->p_finder->eik.valid;
}

/**
 * @brief get utp mode
 *
 * @return true active utp mode
 * @return false deactive utp mode
 */
bool app_gfps_finder_get_utp_mode(void)
{
    return p_app_gfps_finder->p_finder->hash_flag.utp_mode;
}

void app_gfps_finder_ftl_init(void)
{
    uint32_t eik_ret = ftl_load_from_storage(&p_app_gfps_finder->p_finder->eik,
                                             GFPS_FINDER_EIK_FLASH_OFFSET, sizeof(T_GFPS_EIK));
    uint32_t clock_ret = ftl_load_from_storage(&p_app_gfps_finder->p_finder->clock_value,
                                               GFPS_FINDER_CLOCK_FLASH_OFFSET, 4);

    if (eik_ret == ENOF)
    {
        memset(&p_app_gfps_finder->p_finder->eik, 0, sizeof(T_GFPS_EIK));
        ftl_save_to_storage(&p_app_gfps_finder->p_finder->eik, GFPS_FINDER_EIK_FLASH_OFFSET,
                            sizeof(T_GFPS_EIK));
    }

    if (clock_ret == ENOF)
    {
        memset(&p_app_gfps_finder->p_finder->clock_value, 0, 4);
        ftl_save_to_storage(&p_app_gfps_finder->p_finder->clock_value, GFPS_FINDER_CLOCK_FLASH_OFFSET, 4);
    }

    APP_PRINT_INFO4("app_gfps_finder_ftl_init: eik %b, valid %d, eik size %d, clock %d",
                    TRACE_BINARY(32, p_app_gfps_finder->p_finder->eik.key), p_app_gfps_finder->p_finder->eik.valid,
                    sizeof(T_GFPS_EIK), p_app_gfps_finder->p_finder->clock_value);
}

void app_gfps_get_ei(uint8_t *p_ei)
{
    memcpy(p_ei, p_app_gfps_finder->p_finder->adv_ei, 32);
}

void app_gfps_finder_beacon_init(void)
{
    uint8_t hash = 0;
    uint32_t counter = (p_app_gfps_finder->p_finder->clock_value / 1024) * 1024;

    p_app_gfps_finder->p_finder->secp_mode = GFPS_FINDER_SECP160R1;
    app_gfps_finder_generate_adv_ei_and_hash(p_app_gfps_finder->p_finder->adv_ei,
                                             p_app_gfps_finder->p_finder->eik.key, counter, &hash);

    gfps_finder_adv_init(p_app_gfps_finder->p_finder->adv_ei, hash);
}

bool app_gfps_finder_init()
{
    p_app_gfps_finder = calloc(1, sizeof(T_APP_GFPS_FINDER));

    if (p_app_gfps_finder)
    {
        app_gfps_finder_set_default_value();
        p_app_gfps_finder->p_finder = calloc(1, sizeof(T_GFPS_FINDER));

        if (p_app_gfps_finder->p_finder)
        {
#if GFPS_FINDER_TEST_SUPPORT
            bool app_gfps_finder_test(void);
            app_gfps_finder_test();
#endif
            app_gfps_finder_ftl_init();
            app_gfps_finder_beacon_init();
            gfps_finder_init(p_app_gfps_finder->p_finder, app_gfps_finder_cb);
            app_timer_reg_cb(app_gfps_finder_timeout_cb, &p_app_gfps_finder->gfps_finder_timer_id);
            return true;
        }
        else
        {
            APP_PRINT_ERROR0("app_gfps_finder_init: err");
            return false;
        }
    }
    return false;
}
#endif
