#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "platform_utils.h"
#include "csis_mgr.h"
#include "remote.h"
#include "app_cfg.h"
#include "app_lea_csis.h"

#if F_APP_CSIS_SUPPORT
void app_lea_csis_update_sirk(void)
{
    uint8_t csis_srv_id;
    T_ATTR_UUID srv_uuid = {0};

    srv_uuid.is_uuid16 = true;
    srv_uuid.p.uuid16 = GATT_UUID_CAS;

    if (csis_get_srv_id(&csis_srv_id, &srv_uuid))
    {
        csis_update_sirk(csis_srv_id, app_cfg_nv.lea_sirk, CSIS_SIRK_ENC);
    }
}

void app_lea_csis_gen_sirk(void)
{
    uint8_t null_sirk[CSIS_SIRK_LEN] = {0};

    if (memcmp(app_cfg_const.lea_sirk, null_sirk, CSIS_SIRK_LEN) == 0)
    {
        if (!app_cfg_nv.first_engaged)
        {
            uint8_t *p_sirk = &app_cfg_nv.lea_sirk[0];

            BE_UINT32_TO_STREAM(p_sirk, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p_sirk, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p_sirk, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p_sirk, platform_random(0xFFFFFFFF));
        }
    }
    else
    {
        memcpy(app_cfg_nv.lea_sirk, app_cfg_const.lea_sirk, CSIS_SIRK_LEN);
    }
}

void app_lea_csis_init(T_CAP_INIT_PARAMS *p_param)
{
    uint8_t size = 1;
    uint8_t rank = 1;

    if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
    {
        if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
        {
            rank = 2;
        }
        size = 2;
    }

    p_param->csis_num = 1;
    p_param->cas.enable = true;
    p_param->cas.csis_sirk_type = CSIS_SIRK_ENC;
    p_param->cas.csis_size = size;
    p_param->cas.csis_rank = rank;
    p_param->cas.csis_feature = (SET_MEMBER_LOCK_EXIST | SET_MEMBER_SIZE_EXIST | SET_MEMBER_RANK_EXIST |
                                 SET_MEMBER_SIRK_NOTIFY_SUPPORT);
    p_param->cas.csis_sirk = app_cfg_nv.lea_sirk;
}
#endif
