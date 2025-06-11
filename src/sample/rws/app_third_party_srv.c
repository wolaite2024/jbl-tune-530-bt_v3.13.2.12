/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "app_main.h"
#include "app_cfg.h"
#if BISTO_FEATURE_SUPPORT
#include "app_bisto.h"
#include "app_bisto_ble.h"
#include "app_bisto_bt.h"
#endif

#if GFPS_FEATURE_SUPPORT
#include "app_gfps.h"
#include "app_gfps_msg.h"
#if GFPS_FINDER_SUPPORT
#include "app_dult.h"
#include "app_dult_device.h"
#endif
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xm_xiaoai.h"
#include "app_xm_xiaoai_rfc.h"
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "xiaowei_protocol.h"
#include "app_xiaowei_rfc.h"
#endif
#if AMA_FEATURE_SUPPORT
#include "app_ama.h"
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_asp_device_rfc.h"
#include "app_hid_rfc.h"
#endif

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_app_demo.h"
#include "rtk_tuya_ble_adv.h"
#endif

#if DMA_FEATURE_SUPPORT
#include "app_dma.h"
#endif


void app_third_party_srv_init(void)
{

#if GFPS_FEATURE_SUPPORT
    app_gfps_msg_rfc_init();
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_init();
    }
#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_dult_device_init();
        app_dult_svc_init();
    }
#endif
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xm_xiaoai_init();
        app_xiaoai_rfc_init();
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_rfc_init();
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_init();
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_init();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        tuya_ble_app_init();
        le_tuya_adv_init();
    }
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
    app_asp_device_rfc_init();
    app_hid_rfc_init();
#endif

#if DMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.dma_support)
    {
        app_dma_init();
    }
#endif
}

