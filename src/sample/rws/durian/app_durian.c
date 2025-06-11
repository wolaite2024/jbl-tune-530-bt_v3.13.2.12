/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "bt_avp.h"
#include "durian.h"
#include "app_durian_link.h"
#include "app_durian.h"

T_DURIAN_DB durian_db = {0};

void app_durian_init(void)
{
    app_durian_cfg_init();

#if (F_APP_ERWS_SUPPORT == 1)
    app_durian_sync_init();
#endif
    app_durian_avp_init();
    app_durian_adv_init();
    app_durian_loc_init();
    app_durian_link_init();
}
#endif
