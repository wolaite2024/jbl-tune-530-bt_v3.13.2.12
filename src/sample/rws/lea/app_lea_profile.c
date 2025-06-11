#include "bass_mgr.h"
#if BT_GATT_CLIENT_SUPPORT
#include "bt_gatt_client.h"
#endif
#include "ccp_client.h"
#include "bap.h"
#include "cap.h"
#include "mcp_client.h"
#include "app_lea_ascs.h"
#include "app_lea_ccp.h"
#include "app_lea_csis.h"
#include "app_lea_mcp.h"
#include "app_lea_mgr.h"
#include "app_lea_profile.h"
#include "app_lea_pacs.h"
#include "app_lea_unicast_audio.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_vcs.h"
#include "app_lea_vol_policy.h"
#include "app_cfg.h"

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
#if F_APP_ERWS_SUPPORT
#define APP_SNK_ASE_NUM     2
#define APP_SRC_ASE_NUM     1
#else
#define APP_SNK_ASE_NUM     4
#define APP_SRC_ASE_NUM     2
#endif
#endif

#if F_APP_CCP_SUPPORT
static uint16_t app_lea_profile_tmas_role = 0;
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
static void app_lea_profile_bap_init(void)
{
    T_BAP_ROLE_INFO role_info = {0};

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    role_info.role_mask |= (BAP_UNICAST_SRV_SRC_ROLE | BAP_UNICAST_SRV_SNK_ROLE);
    role_info.isoc_cig_max_num = APP_ISOC_CIG_MAX_NUM;
    role_info.isoc_cis_max_num = APP_ISOC_CIS_MAX_NUM;
#endif

#if F_APP_TMAP_BMR_SUPPORT
    role_info.pa_adv_num = 0;
    role_info.isoc_big_broadcaster_num = 0;
    role_info.isoc_bis_broadcaster_num = 0;
    role_info.pa_sync_num = APP_MAX_SYNC_HANDLE_NUM;
    role_info.isoc_big_receiver_num = APP_SYNC_RECEIVER_MAX_BIG_HANDLE_NUM;
    role_info.isoc_bis_receiver_num = APP_SYNC_RECEIVER_MAX_BIS_NUM;
    role_info.brs_num = APP_SYNC_RECEIVER_MAX_BIG_HANDLE_NUM;
    role_info.role_mask |= BAP_BROADCAST_SINK_ROLE;
    if (app_cfg_const.bis_mode & LEA_BROADCAST_DELEGATOR)
    {
        role_info.role_mask |= BAP_SCAN_DELEGATOR_ROLE;
    }
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
    role_info.snk_ase_num = APP_SNK_ASE_NUM;
    role_info.src_ase_num = APP_SRC_ASE_NUM;
#endif

    role_info.init_gap = true;
    bap_role_init(&role_info);
}

static bool app_lea_profile_cap_init(T_CAP_INIT_PARAMS *p_cap_param)
{
    if (p_cap_param)
    {
        p_cap_param->cap_role = CAP_ACCEPTOR_ROLE;
#if F_APP_MCP_SUPPORT
        p_cap_param->mcp_media_control_client = true;
#endif
#if F_APP_CCP_SUPPORT
        p_cap_param->ccp_call_control_client = true;
#endif
        return cap_init(p_cap_param);
    }
    return false;
}

#if F_APP_TMAS_SUPPORT
static void app_lea_profile_tmas_init(void)
{
#if F_APP_TMAP_CT_SUPPORT
    app_lea_profile_tmas_role |= TMAS_CT_ROLE;
#endif
#if F_APP_TMAP_UMR_SUPPORT
    app_lea_profile_tmas_role |= TMAS_UMR_ROLE;
#endif
#if F_APP_TMAP_BMR_SUPPORT
    app_lea_profile_tmas_role |= TMAS_BMR_ROLE;
#endif

    if (app_lea_profile_tmas_role > 0)
    {
        tmas_init(app_lea_profile_tmas_role);
    }
}
#endif

void app_lea_profile_init(void)
{
    T_CAP_INIT_PARAMS cap_init_param = {0};

    app_lea_ascs_init();

#if F_APP_MCP_SUPPORT
    app_lea_mcp_init();
#endif

#if F_APP_CCP_SUPPORT
    app_lea_ccp_init();
#endif

    app_lea_pacs_init();

#if F_APP_VCS_SUPPORT
    app_lea_vcs_init();
#endif

    app_lea_uca_init();
    app_lea_bca_init();
    app_lea_profile_bap_init();

#if F_APP_CSIS_SUPPORT
    app_lea_csis_init(&cap_init_param);
#endif

    app_lea_profile_cap_init(&cap_init_param);

#if F_APP_TMAS_SUPPORT
    app_lea_profile_tmas_init();
#endif

    app_lea_vol_init();
    app_lea_mgr_init();
}

#if F_APP_TMAS_SUPPORT
uint16_t app_lea_profile_get_tmas_role(void)
{
    return app_lea_profile_tmas_role;
}
#endif

#endif
