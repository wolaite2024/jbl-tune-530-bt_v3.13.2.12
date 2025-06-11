
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "trace.h"
#include "app_bud_loc.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_relay.h"
#include "app_sensor.h"

#if F_APP_BUD_LOCATION_SUPPORT

#if (F_APP_SENSOR_SUPPORT == 1)
bool ld_sensor_cause_action = false;
#endif

#if (F_APP_SENSOR_SUPPORT == 1)
void app_bud_loc_cause_action_flag_set(bool sensor_cause_action)
{
    ld_sensor_cause_action = sensor_cause_action;
}

bool app_bud_loc_cause_action_flag_get(void)
{
    return ld_sensor_cause_action;
}
#endif

#if F_APP_ERWS_SUPPORT
void app_bud_loc_evt_handle(uint8_t event, bool from_remote, uint8_t para)
{
    switch (event)
    {
    case APP_BUD_LOC_EVENT_SENSOR_LD_CONFIG:
        {
#if (F_APP_SENSOR_SUPPORT == 1)
            if (from_remote)
            {
                app_sensor_control(para, false, false);
            }
#endif
        }
        break;

    default:
        break;
    }

    APP_PRINT_TRACE4("app_bud_loc_evt_handle: local_loc %d remote_loc %d local_case_closed %d remote_case_closed %d",
                     app_db.local_loc, app_db.remote_loc, app_db.local_case_closed, app_db.remote_case_closed);

    if (!from_remote)
    {
        app_relay_async_single(APP_MODULE_TYPE_BUD_LOC, event);
    }
}

uint16_t app_bud_loc_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;


    switch (msg_type)
    {
    case APP_BUD_LOC_EVENT_SENSOR_LD_CONFIG:
        {
            payload_len = 1;
            msg_ptr = &app_cfg_nv.light_sensor_enable;
        }
        break;

    default:
        break;

    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_BUD_LOC, payload_len, msg_ptr, skip,
                              total);
}

static void app_bud_loc_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        uint8_t *p_info = buf;
        uint8_t event;
        uint8_t para;

        event = msg_type;
        if (buf != NULL)
        {
            para = p_info[0];
        }
        else
        {
            para = 0;
        }

        app_bud_loc_evt_handle(event, 1, para);
    }
}
#endif

void app_bud_loc_init(void)
{
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_bud_loc_relay_cback, app_bud_loc_parse_cback,
                             APP_MODULE_TYPE_BUD_LOC, APP_BUD_LOC_EVENT_MAX);
#endif
}

#endif
