#if GFPS_FEATURE_SUPPORT
#include "app_timer.h"
#include "trace.h"
#include "string.h"
#include "app_gfps.h"
#include "app_gfps_timer.h"
#include "app_cfg.h"

uint8_t app_gfps_timer_id;
uint8_t timer_idx_update_rpa;
uint32_t app_gfps_timer_timeout_ms = 0xDBBA0;//15min

typedef enum
{
    APP_GFPS_TIMER_UPDATE_RPA = 0x00,
    APP_GFPS_TIMER_TOTAL,
} T_APP_GFPS_TIMER;

void app_gfps_timer_start_update_rpa(void)
{
    APP_PRINT_TRACE0("app_gfps_timer_start_update_rpa");
    app_start_timer(&timer_idx_update_rpa, "app_gfps_timer_update_rpa",
                    app_gfps_timer_id, APP_GFPS_TIMER_UPDATE_RPA, 0,
                    false, app_gfps_timer_timeout_ms);
}

/**
 * @brief stop update adv ei timer
 *
 */
void app_gfps_timer_stop_update_rpa(void)
{
    APP_PRINT_TRACE0("app_gfps_timer_stop_update_rpa");
    app_stop_timer(&timer_idx_update_rpa);
}

void app_gfps_timer_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("app_gfps_timer_timeout_cb: timer_id %d, timer_chann %d", timer_id, timer_chann);

    switch (timer_id)
    {
    case APP_GFPS_TIMER_UPDATE_RPA:
        {
            app_gfps_update_rpa();
        }
        break;

    default:
        break;
    }
}

void app_gfps_timer_init(void)
{
    app_timer_reg_cb(app_gfps_timer_timeout_cb, &app_gfps_timer_id);
}
#endif
