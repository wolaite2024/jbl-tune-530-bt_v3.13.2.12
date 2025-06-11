/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FEATURE_SUPPORT
#include "trace.h"
#include "ecc_enhanced.h"
#include "gfps.h"

void app_ecc_handle_msg()
{
    T_ECC_CAUSE ecc_cause = ecc_sub_proc();

    if (ecc_cause == ECC_CAUSE_SUCCESS)
    {
#if GFPS_FEATURE_SUPPORT
        gfps_handle_pending_char_kbp();
#endif
    }
};
#endif
