/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_ERWS_SUPPORT
#include "app_rdtp.h"
#include "app_link_util.h"
#include "app_cfg.h"
#include "bt_rdtp.h"

void app_rdtp_init(void)
{
    if (app_cfg_const.supported_profile_mask & RDTP_PROFILE_MASK)
    {
        bt_rdtp_init();
    }
}

#endif
