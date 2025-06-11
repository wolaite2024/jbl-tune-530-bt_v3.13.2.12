/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap.h"
#include "gap_br.h"
#include "app_gap.h"
#include "app_main.h"
#include "app_cfg.h"
#include "btm.h"

#if GFPS_FEATURE_SUPPORT
#include "app_gfps.h"
#endif

static void app_gap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_READY:
        {
            static const uint8_t null_addr[6] = {0};

            memcpy(app_db.factory_addr, param->ready.bd_addr, 6);

            if (!memcmp(app_cfg_nv.bud_local_addr, null_addr, 6))
            {
                memcpy(app_cfg_nv.bud_local_addr, app_db.factory_addr, 6);
                // remote_local_addr_set(app_cfg_nv.bud_local_addr);
            }

            gap_set_bd_addr(app_cfg_nv.bud_local_addr);
        }
        break;

    case BT_EVENT_LINK_USER_CONFIRMATION_REQ:
        {
#if GFPS_FEATURE_SUPPORT
            if (extend_app_cfg_const.gfps_support)
            {
                uint8_t io_cap;

                gap_get_param(GAP_PARAM_BOND_IO_CAPABILITIES, &io_cap);
                if ((io_cap == GAP_IO_CAP_DISPLAY_ONLY ||
                     io_cap == GAP_IO_CAP_KEYBOARD_ONLY ||
                     io_cap == GAP_IO_CAP_NO_INPUT_NO_OUTPUT) &&
                    param->link_user_confirmation_req.just_works == true)
                {
                    gap_br_user_cfm_req_cfm(param->link_user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
                }
                else
                {
                    app_gfps_handle_bt_user_confirm(param->link_user_confirmation_req);
                }
            }
            else
#endif
            {
                gap_br_user_cfm_req_cfm(param->link_user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
            }
        }
        break;

    default:
        break;
    }
}

void app_gap_init(void)
{
    bt_mgr_cback_register(app_gap_bt_cback);
}
