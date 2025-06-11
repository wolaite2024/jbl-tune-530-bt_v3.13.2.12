#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "os_mem.h"
#include "os_timer.h"
#include "os_sync.h"
#include "console.h"
#include "trace.h"
#include "rtl876x_gpio.h"
#include "rtl876x_wdg.h"
#include "rtl876x.h"
#include "bt_types.h"
#include "gap_br.h"
#include "sysm.h"
#if F_APP_APT_SUPPORT
#include "audio_passthrough.h"
#include "app_audio_passthrough.h"
#endif

#include "app_auto_power_off.h"
#include "anc_tuning.h"
#include "anc.h"
#include "audio.h"
#include "app_hfp.h"
#include "app_cfg.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_main.h"
#include "app_anc.h"
#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif
#include "app_bt_policy_api.h"
#include "app_audio_policy.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#include "app_timer.h"
#include "app_mmi.h"
#include "app_dlps.h"
#include "wdg.h"

#include "app_sniff_mode.h"
#include "dfu_api.h"

#include "audio_probe.h"
#include "audio_route.h"
#include "app_audio_route.h"
#include "app_multilink.h"
#include "eq_utils.h"
#include "app_eq.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "system_status_api.h"
#include "fmc_api.h"
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif
#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb_audio.h"
#endif
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "asp_device_link.h"
#endif
#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif
#include "app_sensor.h"
#include "app_a2dp.h"

#if F_APP_IAP_SUPPORT
#include "app_iap.h"
#endif

/*************************************************
APP must use system_status_api to access AON
follow BB2
**************************************************/
#if F_APP_ANC_SUPPORT
#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#define DSP_SRAM_ADDR                       0x00600000
#else
#define DSP_SRAM_ADDR                       0x00500000
#endif

#define MAX_ANC_READ_DATA_LOG_BY_SPP        250
#define MAX_ANC_READ_DATA_LOG_BY_UART       100
#define MAX_RAMP_TOOL_CMD_LENGTH            250    //8773 BFE/CFE difference

#define ANC_INVALID_DATA                    0XFF

#define ANC_IMAGE_SUB_TYPE_ANC_COEF         0
#define APT_IMAGE_SUB_TYPE_IDLE_COEF        1
#define APT_IMAGE_SUB_TYPE_A2DP_COEF        2
#define APT_IMAGE_SUB_TYPE_SCO_COEF         3

//for anc tool
#define HCI_VENDOR_READ_RTK_ROM_VERSION     0xFC6D
#define HCI_VENDOR_READ_RTK_RTK_CHIP_ID     0xFC6F
#define HCI_VENDOR_WRITE_ANC_REGISTER       0XFD93
#define GAP_GET_ROM_VER         0x05
#define GAP_GET_RTK_CHIP_ID     0x06
#define GAP_WRITE_ANC_REG       0x07

#define ANC_MP_GRP_INFO_BITS    12
#define ANC_TOOL_LINKBACK_TIMES 3

#define ANC_CMD_STATUS_SUCCESS               0x00
#define ANC_CMD_STATUS_FAILED                0x01
#define ANC_CMD_STATUS_SPK2_NOT_EXIST        0x02
#define ANC_CMD_STATUS_SPK2_TIMEOUT          0x03
#define ANC_CMD_STATUS_UNKNOW_CMD            0x04
#define ANC_CMD_STATUS_CHANNEL_NOT_MATCH     0x05
#define ANC_CMD_STATUS_WRONG                 0x06
#define ANC_CMD_STATUS_CRC_FAIL              0x07
#define ANC_CMD_STATUS_LENGTH_FAIL           0x08
#define ANC_CMD_STATUS_SEGMENT_FAIL          0x09
#define ANC_CMD_STATUS_MEMORY_LACK           0x0A
#define ANC_CMD_STATUS_FLASH_BURN_FAILED     0x0B
#define ANC_CMD_STATUS_FLASH_COMFIRM_FAILED  0x0C
#define ANC_CMD_STATUS_CMD_LENGTH_INVALID    0x0D
#define ANC_CMD_STATUS_CMD_NOT_READY         0x0E

#define ANC_GROUP_INACTIVATE                 0xFF
#define ANC_GROUP_UNSELECT                   0xFE
#define CEIL_DIV(A, B)                      (((A) + (B) - 1) / (B))

static uint8_t anc_timer_id = 0;
static uint8_t timer_idx_anc_measure_kill_acl = 0;
static uint8_t timer_idx_anc_tool_wait_sec_respond = 0;

static uint8_t anc_tool_linkback_cnt = 0;
static T_ANC_SM_STATE anc_state = ANC_STOPPED;

T_ANC_MP_OPERATE_DATA   anc_gain_data;
T_ANC_MP_TOOL_DATA      anc_mp_tool_data;
T_ANC_RAMP_TOOL_DATA    anc_ramp_tool_data;
T_ANC_GROUP_DATA        anc_group_data;

typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rsvd1: 3;
        uint32_t grp_info: 12;           /* bit[7:0] */
        uint32_t rsvd2: 17;              /* bit[31:8], reserved */
    } data;
} T_ANC_MP_EXT_DATA;

static uint8_t anc_active_app_idx;
static uint8_t anc_active_cmd_path;
static uint8_t resp_meas_mode = 0;

typedef enum
{
    APP_TIMER_ANC_MEASURE_KILL_ACL,
    APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND,
} T_APP_ANC_TIMER;

#if F_APP_ANC_DEFAULT_ACTIVATE_ALL_GROUP
static void app_anc_burn_all_group(uint8_t num);
static void app_anc_activate_all_group(uint8_t num);
#endif

bool app_anc_set_first_anc_sceanrio(T_ANC_APT_STATE *state)
{
    bool ret = true;

    uint8_t first_scenario_index;

    for (first_scenario_index = 0; first_scenario_index < ANC_MP_GRP_INFO_BITS; first_scenario_index++)
    {
        if (app_cfg_nv.anc_selected_list & BIT(first_scenario_index))
        {
            break;
        }
    }

    if (first_scenario_index + ANC_ON_SCENARIO_1_APT_OFF > ANC_ON_SCENARIO_5_APT_OFF)
    {
        ret = false;
    }

    if (ret)
    {
        *state = (T_ANC_APT_STATE)(first_scenario_index + ANC_ON_SCENARIO_1_APT_OFF);
    }

    APP_PRINT_TRACE2("app_anc_set_first_anc_sceanrio ret = %x, state = 0x%x", ret,
                     first_scenario_index + ANC_ON_SCENARIO_1_APT_OFF);

    return ret;
}

static void app_anc_activated_scenario_init(void)
{
    T_ANC_MP_EXT_DATA anc_mp_ext_data;
    anc_tool_read_mp_ext_data(&(anc_mp_ext_data.d32));

    uint8_t dummy[ANC_MP_GRP_INFO_BITS];
    uint8_t anc_image_group_num;

    anc_image_group_num = anc_tool_get_anc_scenario_info(dummy);
    memset(dummy, 0, ANC_MP_GRP_INFO_BITS);

    if (anc_mp_ext_data.data.rsvd2 == 0)  // has been burned group information
    {
        if (anc_mp_ext_data.data.grp_info &&
            (32 - __clz(anc_mp_ext_data.d32) - 3) <= anc_image_group_num) // group information valid
            /*  ex: anc_image_group = 5, invalid case
                          [rsvd2]                 | 0 0 0 0 0 0 1 0 0 0 0 0 |[rsvd1]|
                _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ | _ _ _ _ _ _ _ _ _ _ _ _ | _ _ _ |  */
        {
            anc_group_data.anc_activated_list = (uint16_t)anc_mp_ext_data.data.grp_info;
        }
        else
        {
            if (anc_image_group_num > 0)
            {
#if F_APP_ANC_DEFAULT_ACTIVATE_ALL_GROUP
                app_anc_burn_all_group(anc_image_group_num);
                app_anc_activate_all_group(anc_image_group_num);
#else
                anc_group_data.anc_activated_list = UINT16_SET_LSB_BITS(1);  //default value
#endif
            }
        }
    }
    else  //burn group information never
    {
        if (anc_image_group_num > 0)
        {
#if F_APP_ANC_DEFAULT_ACTIVATE_ALL_GROUP
            app_anc_burn_all_group(anc_image_group_num);
            app_anc_activate_all_group(anc_image_group_num);
#else
            anc_group_data.anc_activated_list = UINT16_SET_LSB_BITS(1);  //default value
            app_cfg_nv.anc_selected_list = UINT16_SET_LSB_BITS(app_anc_get_activated_scenario_cnt());
#endif
        }
    }

    if ((app_anc_get_selected_scenario_cnt() > ANC_MP_GRP_INFO_BITS) ||
        // after factory reset, selected cnt will be 16
        app_anc_get_selected_scenario_cnt() == 0)    // (i) SOC don't accept scenario cnt be 0
        // (ii) OTA from unsupported version to supported version will be 0
    {
        app_cfg_nv.anc_selected_list = UINT16_SET_LSB_BITS(app_anc_get_activated_scenario_cnt());
    }

    APP_PRINT_TRACE3("app_anc_activated_scenario_init activated_list = 0x%x, selected_list = 0x%x, group_number = %d",
                     anc_group_data.anc_activated_list, app_cfg_nv.anc_selected_list, anc_image_group_num);
}

uint8_t app_anc_scenario_select_is_busy(void)
{
    return anc_group_data.enter_anc_select_mode;
}

void app_anc_enter_scenario_select_mode(void)
{
    anc_group_data.enter_anc_select_mode = true;

    app_listening_state_machine(EVENT_ALL_OFF, false, true);
}

void app_anc_exit_scenario_select_mode(void)
{
    app_listening_state_machine(EVENT_ALL_OFF, false, true);

    anc_group_data.enter_anc_select_mode = false;
}

uint8_t app_anc_get_selected_scenario_cnt(void)
{
    return app_listening_count_1bits((uint32_t)app_cfg_nv.anc_selected_list);
}

uint8_t app_anc_get_activated_scenario_cnt(void)
{
    return app_listening_count_1bits((uint32_t)anc_group_data.anc_activated_list);
}

static bool app_anc_check_anc_choose_data_valid(uint32_t l_data, uint32_t r_data)
{
    uint8_t l_scenario_count;
    uint8_t activated_group_number;

    activated_group_number = app_listening_count_1bits(anc_group_data.anc_activated_list);

    l_scenario_count = app_listening_count_1bits(l_data);

    //check bitmap count valid
    if ((l_data == r_data) &&
        (l_scenario_count > 0) &&
        (l_scenario_count <= activated_group_number))
    {
        //check bitmap position valid
        if (32 - __clz(l_data) <= activated_group_number)
        {
            return true;
        }
    }

    return false;
}

uint8_t app_anc_ramp_tool_is_busy(void)
{
    return anc_ramp_tool_data.ramp_burn_start;
}

static uint16_t app_anc_ramp_cal_crc(uint8_t *buf, uint32_t length)
{
    uint16_t checksum16 = 0;
    uint16_t *p16;
    uint32_t i;

    p16 = (uint16_t *)buf;
    for (i = 0; i < length / 2; ++i)
    {
        checksum16 = checksum16 ^ (*p16);
        ++p16;
    }

    //swap_16
    checksum16 = ((checksum16 & 0xff) << 8) | ((checksum16 & 0xff00) >> 8);

    return checksum16;
}

bool app_anc_ramp_tool_channel_match(T_CHANNEL_TYPE channel)
{
    bool ret = false;

    if ((channel == CHANNEL_L) && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
    {
        ret = true;
    }
    else if ((channel == CHANNEL_R) && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
    {
        ret = true;
    }

    return ret;
}

static void app_anc_ramp_tool_data_reset(void)
{
    anc_ramp_tool_data.ramp_burn_start = false;
    anc_ramp_tool_data.buffer_size = 0;
    anc_ramp_tool_data.data_receive_length = 0;
    anc_ramp_tool_data.data_segement_count = 0;
    anc_ramp_tool_data.mode = APP_ANC_RAMP_BURN_MODE;
    anc_ramp_tool_data.encrypted = APP_ANC_RAMP_ENCRYPTED;

    if (anc_ramp_tool_data.buffer_ptr)
    {
        free(anc_ramp_tool_data.buffer_ptr);
        anc_ramp_tool_data.buffer_ptr = NULL;
    }

    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_ANC_APT);
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_ANC_APT_MODE, app_cfg_const.timer_auto_power_off);
}

static bool app_anc_ramp_tool_data_initial(uint8_t *buffer_ptr, uint16_t data_length)
{
    bool ret = false;

    app_anc_ramp_tool_data_reset();

    if (buffer_ptr == NULL)
    {
        anc_ramp_tool_data.buffer_ptr = (uint8_t *)malloc(data_length);
    }
    else
    {
        anc_ramp_tool_data.buffer_ptr = buffer_ptr;
    }

    if (anc_ramp_tool_data.buffer_ptr)
    {
        anc_ramp_tool_data.buffer_size = data_length;
        anc_ramp_tool_data.ramp_burn_start = true;

        app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_ANC_APT);
        app_auto_power_off_disable(AUTO_POWER_OFF_MASK_ANC_APT_MODE);

        ret = true;
    }

    return ret;
}

uint8_t app_anc_ramp_tool_burn_parameter(void)
{
    uint8_t status = ANC_CMD_STATUS_SUCCESS;

    uint32_t flash_tem_addr;
    flash_tem_addr = (uint32_t)audio_probe_media_buffer_malloc(FLASH_SECTION_SIZE);

    if (flash_tem_addr)
    {
        uint8_t media_buffer_param[4];
        media_buffer_param[0] = (uint8_t)(flash_tem_addr);
        media_buffer_param[1] = (uint8_t)((flash_tem_addr) >> 8);
        media_buffer_param[2] = (uint8_t)((flash_tem_addr) >> 16);
        media_buffer_param[3] = (uint8_t)((flash_tem_addr) >> 24);
        anc_tool_anc_mp_bak_buf_config(*(uint32_t *)media_buffer_param, 0);

        uint8_t param[8];

        param[0] = anc_ramp_tool_data.parameter_type;
        param[1] = anc_ramp_tool_data.parameter_group_index;
        param[2] = (uint8_t)anc_ramp_tool_data.data_receive_length;
        param[3] = anc_ramp_tool_data.data_receive_length >> 8;
        param[4] = (uint8_t)(((uint32_t)anc_ramp_tool_data.buffer_ptr));
        param[5] = (uint8_t)(((uint32_t)(anc_ramp_tool_data.buffer_ptr)) >> 8);
        param[6] = (uint8_t)(((uint32_t)(anc_ramp_tool_data.buffer_ptr)) >> 16);
        param[7] = (uint8_t)(((uint32_t)(anc_ramp_tool_data.buffer_ptr)) >> 24);

        uint32_t result;
        anc_tool_ramp_data_write((uint32_t)param);
        result = *(uint32_t *)param;

        audio_probe_media_buffer_free((void *)flash_tem_addr);

        anc_tool_anc_mp_bak_buf_config(NULL, 0);

        app_anc_ramp_tool_data_reset();

        APP_PRINT_TRACE0("audio_app_anc ramp_burn_para_continue done");

        if (result)
        {
            status = ANC_CMD_STATUS_FLASH_BURN_FAILED;
        }
    }
    else
    {
        status = ANC_CMD_STATUS_MEMORY_LACK;
    }

    return status;
}

uint8_t app_anc_ramp_tool_burn_confirm(void)
{
    uint16_t ans_crc, read_back_crc;
    uint32_t anc_data_addr;
    uint8_t param[8];

    ans_crc = app_anc_ramp_cal_crc(anc_ramp_tool_data.buffer_ptr,
                                   anc_ramp_tool_data.data_receive_length);

    param[0] = anc_ramp_tool_data.parameter_type;
    param[1] = anc_ramp_tool_data.parameter_group_index;
    param[2] = 4;   // data length, LSB
    param[3] = 0;   // data length, MSB
    param[4] = (uint8_t)(((uint32_t)(&anc_data_addr)));
    param[5] = (uint8_t)(((uint32_t)(&anc_data_addr)) >> 8);
    param[6] = (uint8_t)(((uint32_t)(&anc_data_addr)) >> 16);
    param[7] = (uint8_t)(((uint32_t)(&anc_data_addr)) >> 24);
    anc_tool_get_scenario_addr((uint32_t)param);

    fmc_flash_nor_read(anc_data_addr, anc_ramp_tool_data.buffer_ptr,
                       anc_ramp_tool_data.data_receive_length); // read anc image data
    read_back_crc = app_anc_ramp_cal_crc(anc_ramp_tool_data.buffer_ptr,
                                         anc_ramp_tool_data.data_receive_length);

    return (ans_crc == read_back_crc) ? ANC_CMD_STATUS_SUCCESS : ANC_CMD_STATUS_FLASH_COMFIRM_FAILED;

}

static uint8_t app_anc_ramp_tool_data_check(uint8_t segement_total, uint8_t payload_length,
                                            uint8_t *crc_ptr, uint16_t crc_len, uint16_t crc_ans)
{
    if ((!anc_ramp_tool_data.ramp_burn_start) && (anc_ramp_tool_data.mode == APP_ANC_RAMP_BURN_MODE))
    {
        return ANC_CMD_STATUS_WRONG;
    }

    if (anc_ramp_tool_data.data_segement_count >= segement_total)
    {
        return ANC_CMD_STATUS_SEGMENT_FAIL;
    }

    if ((anc_ramp_tool_data.data_receive_length + payload_length) > anc_ramp_tool_data.buffer_size)
    {
        return ANC_CMD_STATUS_LENGTH_FAIL;
    }
    if (dfu_checkbufcrc(crc_ptr, crc_len, crc_ans))
    {
        /* crc check fail */
        return  ANC_CMD_STATUS_CRC_FAIL;
    }

    return ANC_CMD_STATUS_SUCCESS;
}

uint8_t app_anc_ramp_tool_apply_parameter(void)
{
    T_ANC_APT_STATE anc_ramp_state = ANC_OFF_APT_OFF;

    app_listening_assign_specific_state(*app_db.final_listening_state,
                                        anc_ramp_state, false, true);

    anc_tool_set_scenario(anc_ramp_tool_data.buffer_ptr);

    if (anc_ramp_tool_data.parameter_type == ANC_IMAGE_SUB_TYPE_ANC_COEF)
    {
        anc_ramp_state = (T_ANC_APT_STATE)(ANC_ON_SCENARIO_1_APT_OFF +
                                           anc_ramp_tool_data.parameter_group_index);

    }
#if F_APP_APT_SUPPORT
    else if (anc_ramp_tool_data.parameter_type == APT_IMAGE_SUB_TYPE_IDLE_COEF ||
             anc_ramp_tool_data.parameter_type == APT_IMAGE_SUB_TYPE_A2DP_COEF ||
             anc_ramp_tool_data.parameter_type == APT_IMAGE_SUB_TYPE_SCO_COEF)
    {
        anc_ramp_state = (T_ANC_APT_STATE)(ANC_OFF_LLAPT_ON_SCENARIO_1 +
                                           anc_ramp_tool_data.parameter_group_index);
    }
#endif
    else
    {
        return ANC_CMD_STATUS_FAILED;
    }

    app_listening_assign_specific_state(*app_db.final_listening_state,
                                        anc_ramp_state, false, true);

    anc_ramp_tool_data.ramp_burn_start = false;
    anc_ramp_tool_data.buffer_size = 0;
    anc_ramp_tool_data.data_receive_length = 0;
    anc_ramp_tool_data.data_segement_count = 0;
    anc_ramp_tool_data.mode = APP_ANC_RAMP_BURN_MODE;
    anc_ramp_tool_data.encrypted = APP_ANC_RAMP_ENCRYPTED;
    anc_ramp_tool_data.buffer_ptr = NULL;

    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_ANC_APT);
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_ANC_APT_MODE, app_cfg_const.timer_auto_power_off);

    return ANC_CMD_STATUS_SUCCESS;
}

static bool app_anc_tool_media_buffer_idle(void)
{
    T_AUDIO_TRACK_STATE state;
    T_APP_BR_LINK *p_link;

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    uint8_t active_index = app_teams_multilink_get_active_music_index();

#if F_APP_USB_AUDIO_SUPPORT
    if (active_index == multilink_usb_idx)
    {
        state = app_usb_audio_get_ds_track_state();
    }
    else
#endif
    {
        p_link = app_link_find_br_link(app_db.br_link[active_index].bd_addr);

        if (p_link == NULL || p_link->a2dp_track_handle == NULL)
        {
            return true;
        }
        audio_track_state_get(p_link->a2dp_track_handle, &state);
    }
#else
    p_link = app_link_find_br_link(app_db.br_link[app_a2dp_get_active_idx()].bd_addr);

    if (p_link == NULL || p_link->a2dp_track_handle == NULL)
    {
        return true;
    }

    audio_track_state_get(p_link->a2dp_track_handle, &state);
#endif

    if (state == AUDIO_TRACK_STATE_STARTED)
    {
        return false;
    }

    return true;
}

uint8_t app_anc_tool_burn_gain_is_busy(void)
{
    return anc_mp_tool_data.burn_start;
}

bool app_anc_tool_burn_gain_prepare(void)
{
    anc_mp_tool_data.burn_start = true;

    if (app_anc_tool_media_buffer_idle())
    {
        return true;
    }

    return false;
}

void app_anc_tool_burn_gain_finish(void)
{
    anc_mp_tool_data.burn_start = false;
}

uint16_t app_anc_cal_crc16(const uint8_t *data_p, uint32_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--)
    {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

uint8_t app_anc_get_coeff_idx(uint8_t scenario_id)
{
    int8_t order = -1;

    for (uint8_t i = 0; i < ANC_MP_GRP_INFO_BITS; i++)
    {
        if (anc_group_data.anc_activated_list & BIT(i))
        {
            order++;
            if (order == scenario_id)
            {
                APP_PRINT_TRACE1("app_anc_get_coeff_idx, coeff idx:%d", i);
                return i;
            }
        }
    }

    APP_PRINT_ERROR0("app_anc_get_coeff_idx, scenario id out of range");

    return 0xFF;
}

T_ANC_SWITCH_SCENARIO app_anc_switch_next_scenario(T_ANC_APT_STATE anc_current_scenario,
                                                   T_ANC_APT_STATE *anc_next_scenario)
{
    if (!app_anc_is_anc_on_state(anc_current_scenario))
    {
        return ANC_SWITCH_SCENARIO_NO_DEFINE;
    }

    T_ANC_APT_STATE scenario_index = ++anc_current_scenario;

    for (; scenario_index <= ANC_ON_SCENARIO_5_APT_OFF; scenario_index++)
    {
        if (app_cfg_nv.anc_selected_list & BIT(scenario_index - ANC_ON_SCENARIO_1_APT_OFF))
        {
            *anc_next_scenario = scenario_index;
            return ANC_SWITCH_SCENARIO_SUCCESS;
        }
    }

    return ANC_SWITCH_SCENARIO_MAX;
}

static uint8_t app_anc_tool_burn_gain_mismatch(uint32_t ext_data)
{
    uint32_t flash_tem_addr = 0;

    //flash burning need 4K for temporary storage, take it from media buffer
    flash_tem_addr = (uint32_t)audio_probe_media_buffer_malloc(FLASH_SECTION_SIZE);

    if (flash_tem_addr == NULL)
    {
        APP_PRINT_WARN0("app_anc_tool_burn_gain_mismatch media_buffer_malloc fail");
        return ANC_CMD_STATUS_FAILED;
    }

    uint8_t media_buffer_param[4];
    media_buffer_param[0] = (uint8_t)(((uint32_t)flash_tem_addr));
    media_buffer_param[1] = (uint8_t)((uint32_t)(flash_tem_addr) >> 8);
    media_buffer_param[2] = (uint8_t)(((uint32_t)(flash_tem_addr)) >> 16);
    media_buffer_param[3] = (uint8_t)(((uint32_t)(flash_tem_addr)) >> 24);

    //set flash section size(4K) address to patch code
    anc_tool_anc_mp_bak_buf_config(*(uint32_t *)media_buffer_param, 0);

    anc_tool_burn_gain_mismatch(ext_data);

    //give flash section size(4K) to media buffer
    audio_probe_media_buffer_free((void *)flash_tem_addr);

    //reset flash section size(4K) address
    anc_tool_anc_mp_bak_buf_config(NULL, 0);

    return ANC_CMD_STATUS_SUCCESS;
}

#if F_APP_ANC_DEFAULT_ACTIVATE_ALL_GROUP
static void app_anc_burn_all_group(uint8_t num)
{
    uint32_t ext_data = 0;

    ext_data = (uint32_t)UINT16_SET_LSB_BITS(num) << 3;
    app_anc_tool_burn_gain_mismatch(ext_data);
}

static void app_anc_activate_all_group(uint8_t num)
{
    if (num > 0)
    {
        uint8_t i = 0;

        for (i = 0; i < num; i++)
        {
            anc_group_data.anc_activated_list |= BIT(i);
        }
    }
}
#endif

uint8_t app_anc_tool_burn_llapt_gain_mismatch(uint32_t llapt_ext_data)
{
    uint32_t flash_tem_addr = 0;

    //flash burning need 4K for temporary storage, take it from media buffer
    flash_tem_addr = (uint32_t)audio_probe_media_buffer_malloc(FLASH_SECTION_SIZE);

    if (flash_tem_addr == NULL)
    {
        APP_PRINT_WARN0("app_anc_tool_burn_gain_mismatch media_buffer_malloc fail");
        return ANC_CMD_STATUS_FAILED;
    }

    uint8_t media_buffer_param[4];
    media_buffer_param[0] = (uint8_t)(((uint32_t)flash_tem_addr));
    media_buffer_param[1] = (uint8_t)((uint32_t)(flash_tem_addr) >> 8);
    media_buffer_param[2] = (uint8_t)(((uint32_t)(flash_tem_addr)) >> 16);
    media_buffer_param[3] = (uint8_t)(((uint32_t)(flash_tem_addr)) >> 24);

    //set flash section size(4K) address to patch code
    anc_tool_anc_mp_bak_buf_config(*(uint32_t *)media_buffer_param, 0);

    anc_tool_burn_llapt_gain_mismatch(llapt_ext_data);

    //give flash section size(4K) to media buffer
    audio_probe_media_buffer_free((void *)flash_tem_addr);

    //reset flash section size(4K) address
    anc_tool_anc_mp_bak_buf_config(NULL, 0);

    return ANC_CMD_STATUS_SUCCESS;
}
extern void anc_mgr_tool_enable(void);
extern void anc_mgr_tool_disable(void);
void app_anc_enter_test_mode(uint8_t mode)
{
    T_ANC_FEATURE_MAP feature_map;

    app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_ANC_APT);
    app_dlps_disable(APP_DLPS_ENTER_CHECK_ANC_TEST);

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_ANC_APT_MODE);

    if (mode == ANC_TEST_MODE_ENTER_ANC)
    {
        anc_enable(0xff);
    }
#if F_APP_APT_SUPPORT
    else if (mode == ANC_TEST_MODE_ENTER_LLAPT)
    {
        audio_passthrough_enable(AUDIO_PASSTHROUGH_MODE_LOW_LATENCY, 0xff);
    }
#endif

    feature_map.d32 = anc_tool_get_feature_map();
    feature_map.mic_path = mode;
    anc_tool_set_feature_map(feature_map.d32);
}

void app_anc_exit_test_mode(uint8_t mode)
{
    T_ANC_FEATURE_MAP feature_map;

    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_ANC_APT_MODE, app_cfg_const.timer_auto_power_off);

    feature_map.d32 = anc_tool_get_feature_map();
    feature_map.user_mode = ENABLE;
    anc_tool_set_feature_map(feature_map.d32);
    if (mode == ANC_TEST_MODE_ENTER_ANC)
    {
        anc_disable();
        anc_tool_read_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, 1, &anc_gain_data.ext_l_gain,
                                    &anc_gain_data.ext_r_gain);
        anc_tool_read_gain_mismatch(ANC_GAIN_SRC_INT_MIC, 1, &anc_gain_data.int_l_gain,
                                    &anc_gain_data.int_r_gain);
        anc_tool_set_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, anc_gain_data.ext_l_gain,
                                   anc_gain_data.ext_r_gain);
        anc_tool_set_gain_mismatch(ANC_GAIN_SRC_INT_MIC, anc_gain_data.int_l_gain,
                                   anc_gain_data.int_r_gain);
    }
#if F_APP_APT_SUPPORT
    else if (mode == ANC_TEST_MODE_ENTER_LLAPT)
    {
        audio_passthrough_disable();
        anc_tool_read_llapt_gain_mismatch(1, &anc_gain_data.llapt_ext_l_gain,
                                          &anc_gain_data.llapt_ext_r_gain);
        anc_tool_set_llapt_gain_mismatch(anc_gain_data.llapt_ext_l_gain, anc_gain_data.llapt_ext_r_gain);
    }
#endif

    // resume feature map to default when exit test mode
    feature_map.d32 = anc_tool_get_feature_map();
    feature_map.set_regs = ENABLE;
    feature_map.enable_fading = ENABLE;
    feature_map.enable_dehowling = ENABLE;
    feature_map.enable_wns = ENABLE;
    anc_tool_set_feature_map(feature_map.d32);
    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_ANC_APT);
    app_dlps_enable(APP_DLPS_ENTER_CHECK_ANC_TEST);
}

static void app_anc_report(uint16_t anc_report_event, uint8_t *event_data, uint16_t event_len)
{
    bool handle = true;

    switch (anc_report_event)
    {
    case EVENT_ANC_QUERY:
    case EVENT_SPECIFY_ANC_QUERY:
        {
            uint8_t report_data[16];
            uint8_t i;
            uint8_t scenario_mode[ANC_MP_GRP_INFO_BITS];
            uint8_t select_list_index = 0;

            report_data[0] = 0; // query_type

            if (anc_report_event == EVENT_ANC_QUERY)
            {
                report_data[1] = app_anc_is_anc_on_state(app_db.current_listening_state);
                report_data[2] = (report_data[1]) ? (uint8_t)(app_db.current_listening_state -
                                                              ANC_ON_SCENARIO_1_APT_OFF) : ANC_INVALID_DATA;
            }
#if F_APP_SUPPORT_ANC_APT_COEXIST
            else if (anc_report_event == EVENT_SPECIFY_ANC_QUERY)
            {
                report_data[1] = app_listening_is_anc_apt_on_state(app_db.current_listening_state);
                report_data[2] = (report_data[1]) ? (uint8_t)(app_db.current_listening_state -
                                                              ANC_ON_SCENARIO_1_APT_ON) : ANC_INVALID_DATA;
            }
#endif

            report_data[3] = anc_tool_get_anc_scenario_info(scenario_mode);

            for (i = 0; i < report_data[3]; i++)
            {
                // User didn't define the anc mode, so SOC use a default value to replace.
                if (scenario_mode[i] == APP_ANC_MODE_TYPE_UNKNOWN)
                {
                    scenario_mode[i] = APP_ANC_MODE_TYPE_NONE;
                }

                // If this ANC index not included when burn gain, return UnKnown_Senario (0xFF)
                if (anc_group_data.anc_activated_list & BIT(i))
                {
                    report_data[4 + i] = (app_cfg_nv.anc_selected_list & BIT(select_list_index)) ? scenario_mode[i] :
                                         ANC_GROUP_UNSELECT;
                    select_list_index++;

                }
                else
                {
                    report_data[4 + i] = ANC_GROUP_INACTIVATE;
                }
            }

            app_report_event_broadcast(anc_report_event, report_data, report_data[3] + 4);
        }
        break;

    case EVENT_ANC_SCENARIO_CHOOSE_INFO:
        {
            uint8_t report_data[ANC_MP_GRP_INFO_BITS + 1];
            uint8_t scenario_mode[ANC_MP_GRP_INFO_BITS];
            uint8_t total_anc_group;

            uint8_t i;
            uint8_t select_list_index = 0;

            total_anc_group = anc_tool_get_anc_scenario_info(scenario_mode);
            report_data[0] = app_anc_get_activated_scenario_cnt();

            for (i = 0; i < total_anc_group; i++)
            {
                // User didn't define the anc mode, so SOC use a default value to replace.
                if (scenario_mode[i] == APP_ANC_MODE_TYPE_UNKNOWN)
                {
                    scenario_mode[i] = APP_ANC_MODE_TYPE_NONE;
                }

                // If this ANC index not included when burn gain, return UnKnown_Senario (0xFF)
                if (anc_group_data.anc_activated_list & BIT(i))
                {
                    report_data[1 + select_list_index] = scenario_mode[i];
                    select_list_index++;
                }
            }

            app_report_event_broadcast(EVENT_ANC_SCENARIO_CHOOSE_INFO, report_data, report_data[0] + 1);
        }
        break;

    case EVENT_ANC_SCENARIO_CHOOSE_TRY:
        {
            uint8_t anc_status = event_data[0];

            app_report_event_broadcast(EVENT_ANC_SCENARIO_CHOOSE_TRY, &anc_status, sizeof(anc_status));
        }
        break;

    case EVENT_ANC_SCENARIO_CHOOSE_RESULT:
        {
            uint8_t anc_status = event_data[0];

            app_report_event_broadcast(EVENT_ANC_SCENARIO_CHOOSE_RESULT, &anc_status, sizeof(anc_status));
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE1("app_anc_report = %x", anc_report_event);
    }
}

void app_anc_cmd_pre_handle(uint16_t anc_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                            uint8_t app_idx, uint8_t *ack_pkt)
{
    /*
    switch (anc_cmd)
    {
    // reserved for some SKIP_OPERATE condition check
    default:
        break;
    }
    */

    app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
    app_anc_cmd_handle(anc_cmd, param_ptr, param_len, path, app_idx);

    /*
    SKIP_OPERATE:
        app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
    */
}

void app_anc_cmd_handle(uint16_t anc_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                        uint8_t app_idx)
{
    bool only_report_status = false;
    uint8_t anc_status = ANC_CMD_STATUS_FAILED;
    anc_active_app_idx = app_idx;
    anc_active_cmd_path = path;
    anc_mp_tool_data.report_event = anc_cmd;
    uint16_t max_payload_len = app_cmd_get_max_payload_len();
    uint16_t max_relay_len = APP_RELAY_MTU - APP_RELAY_HEADER_LEN - APP_CMD_RELAY_HEADER_LEN;

    APP_PRINT_TRACE8("app_anc_cmd_handle: anc_cmd 0x%04X, param_ptr 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                     anc_cmd, param_ptr[1], param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5], param_ptr[6],
                     param_ptr[7]);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) && (max_payload_len >= max_relay_len))
    {
        max_payload_len = max_relay_len;
    }
    else
    {
        max_payload_len -= RTK_HEADER_LEN;
    }

    switch (anc_cmd)
    {
    case CMD_ANC_READ_REGISTER:
        {
            uint32_t reg_addr;
            uint8_t report_value[4];
            uint32_t reg_value = 0;
            reg_addr = param_ptr[1] | (param_ptr[2] << 8) | (param_ptr[3] << 16) | (param_ptr[4] << 24);
            reg_value = anc_tool_read_reg(reg_addr);

            report_value[0] = reg_value;
            report_value[1] = reg_value >> 8;
            report_value[2] = reg_value >> 16;
            report_value[3] = reg_value >> 24;

            APP_PRINT_TRACE2("app_anc_cmd_handle: CMD_ANC_READ_REGISTER, register 0x%08x, value 0x%08x",
                             reg_addr, reg_value);

            app_report_event(path, anc_cmd, app_idx, &report_value[0], 4);
        }
        break;

    case CMD_ANC_WRITE_REGISTER:
        {
            uint8_t data_len = param_ptr[0];
            only_report_status = true;

            APP_PRINT_TRACE8("app_anc_cmd_handle: CMD_ANC_WRITE_REGISTER: data_len 0x%02x, anc_cmd_ptr 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                             data_len, param_ptr[0], param_ptr[1], param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5],
                             param_ptr[6]);

            anc_tool_set_para((void *)param_ptr);

            anc_status = ANC_CMD_STATUS_SUCCESS;
        }
        break;

    case CMD_ANC_TEST_MODE:
        {
            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_READ) ? MP_TYPE_ONE_DEVICE :  MP_TYPE_TWO_DEVICE;

            T_ANC_TEST_MODE test_mode = (T_ANC_TEST_MODE)param_ptr[0];

            if (test_mode == ANC_TEST_MODE_ENTER_ANC || test_mode == ANC_TEST_MODE_ENTER_LLAPT)
            {
                if (app_db.current_listening_state != ANC_OFF_APT_OFF)
                {
                    app_listening_cmd_postpone_stash(anc_cmd, param_ptr, param_len, path, app_idx,
                                                     ANC_APT_CMD_POSTPONE_WAIT_USER_MODE_CLOSE);

                    APP_PRINT_TRACE0("app_anc_cmd_handle: CMD_ANC_TEST_MODE postpone");

                    app_listening_state_machine(EVENT_ALL_OFF, false, true);

                    break;
                }
            }

            APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_ANC_TEST_MODE 0x%02x, mp_type = %d, path = %d",
                             param_ptr[0], mp_type, path);

            // disable set regs, fading, user mode when enter test mode
            T_ANC_FEATURE_MAP feature_map = {.d32 = anc_tool_get_feature_map()};

            feature_map.set_regs = DISABLE;
            feature_map.enable_fading = DISABLE;
            feature_map.user_mode = DISABLE;
            feature_map.enable_dehowling = DISABLE;
            feature_map.enable_wns = DISABLE;
            anc_tool_set_feature_map(feature_map.d32);

            if (test_mode == ANC_TEST_MODE_EXIT)
            {
                app_anc_exit_test_mode(feature_map.mic_path);
            }
            else
            {
                app_anc_enter_test_mode(test_mode);
            }

            anc_status = ANC_CMD_STATUS_SUCCESS;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_READ], MP_LEN_READ, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }

        }
        break;

    case CMD_ANC_WRITE_GAIN:
        {
            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_ANC_BURN_WRITE) ? MP_TYPE_ONE_DEVICE :
                                    MP_TYPE_TWO_DEVICE;

            //take first section data
            anc_gain_data.ext_l_gain = param_ptr[0] | (param_ptr[1] << 8) | (param_ptr[2] << 16) |
                                       (param_ptr[3] << 24);
            anc_gain_data.ext_r_gain = param_ptr[4] | (param_ptr[5] << 8) | (param_ptr[6] << 16) |
                                       (param_ptr[7] << 24);
            anc_gain_data.int_l_gain = param_ptr[8] | (param_ptr[9] << 8) | (param_ptr[10] << 16) |
                                       (param_ptr[11] << 24);
            anc_gain_data.int_r_gain = param_ptr[12] | (param_ptr[13] << 8) | (param_ptr[14] << 16) |
                                       (param_ptr[15] << 24);
            anc_gain_data.grp_index = param_ptr[16] | (param_ptr[17] << 8) | (param_ptr[18] << 16) |
                                      (param_ptr[19] << 24);

            // enable set regs for ANC MP usage
            T_ANC_FEATURE_MAP feature_map;
            feature_map.d32 = anc_tool_get_feature_map();
            feature_map.set_regs = ENABLE;
            anc_tool_set_feature_map(feature_map.d32);

            anc_tool_disable();


            /*APP_PRINT_TRACE5("app_anc_cmd_handle: CMD_ANC_WRITE_GAIN index = %d, EXT_L = 0x%X, EXT_R = 0x%X, INT_L = 0x%X, INT_R = 0x%X",
                             anc_gain_data.grp_index, anc_gain_data.ext_l_gain, anc_gain_data.ext_r_gain,
                             anc_gain_data.int_l_gain, anc_gain_data.int_r_gain);
            */

            anc_tool_set_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, anc_gain_data.ext_l_gain,
                                       anc_gain_data.ext_r_gain);
            anc_tool_set_gain_mismatch(ANC_GAIN_SRC_INT_MIC, anc_gain_data.int_l_gain,
                                       anc_gain_data.int_r_gain);

            anc_tool_load_scenario(ANC_IMAGE_SUB_TYPE_ANC_COEF, anc_gain_data.grp_index);

            anc_status = ANC_CMD_STATUS_SUCCESS;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                //take second section data
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_ANC_BURN_WRITE], MP_LEN_ANC_BURN_WRITE,
                                              APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_ANC_READ_GAIN:
        {
            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_READ) ? MP_TYPE_ONE_DEVICE :  MP_TYPE_TWO_DEVICE;

            /*APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_ANC_READ_GAIN param_len = %d, mp_type = %d, path = %d",
                             param_len, mp_type, path);
            */
            uint32_t d32 = 0;

            anc_tool_read_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, param_ptr[0], &anc_gain_data.ext_l_gain,
                                        &anc_gain_data.ext_r_gain);
            anc_tool_read_gain_mismatch(ANC_GAIN_SRC_INT_MIC, param_ptr[0], &anc_gain_data.int_l_gain,
                                        &anc_gain_data.int_r_gain);
            anc_tool_read_mp_ext_data(&d32);

            anc_mp_tool_data.report_value[0] = anc_gain_data.ext_l_gain;
            anc_mp_tool_data.report_value[1] = anc_gain_data.ext_l_gain >> 8;
            anc_mp_tool_data.report_value[2] = anc_gain_data.ext_l_gain >> 16;
            anc_mp_tool_data.report_value[3] = anc_gain_data.ext_l_gain >> 24;
            anc_mp_tool_data.report_value[4] = anc_gain_data.ext_r_gain;
            anc_mp_tool_data.report_value[5] = anc_gain_data.ext_r_gain >> 8;
            anc_mp_tool_data.report_value[6] = anc_gain_data.ext_r_gain >> 16;
            anc_mp_tool_data.report_value[7] = anc_gain_data.ext_r_gain >> 24;
            anc_mp_tool_data.report_value[8] = anc_gain_data.int_l_gain;
            anc_mp_tool_data.report_value[9] = anc_gain_data.int_l_gain >> 8;
            anc_mp_tool_data.report_value[10] = anc_gain_data.int_l_gain >> 16;
            anc_mp_tool_data.report_value[11] = anc_gain_data.int_l_gain >> 24;
            anc_mp_tool_data.report_value[12] = anc_gain_data.int_r_gain;
            anc_mp_tool_data.report_value[13] = anc_gain_data.int_r_gain >> 8;
            anc_mp_tool_data.report_value[14] = anc_gain_data.int_r_gain >> 16;
            anc_mp_tool_data.report_value[15] = anc_gain_data.int_r_gain >> 24;
            anc_mp_tool_data.report_value[16] = anc_gain_data.grp_index;
            anc_mp_tool_data.report_value[17] = anc_gain_data.grp_index >> 8;
            anc_mp_tool_data.report_value[18] = anc_gain_data.grp_index >> 16;
            anc_mp_tool_data.report_value[19] = anc_gain_data.grp_index >> 24;
            anc_mp_tool_data.report_value[20] = d32;
            anc_mp_tool_data.report_value[21] = d32 >> 8;
            anc_mp_tool_data.report_value[22] = d32 >> 16;
            anc_mp_tool_data.report_value[23] = d32 >> 24;
            anc_mp_tool_data.report_value[24] = app_anc_is_anc_on_state(app_db.current_listening_state) ?
                                                (app_db.current_listening_state - ANC_ON_SCENARIO_1_APT_OFF + 1) : 0xFF;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_READ], MP_LEN_READ, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_mp_tool_data.report_value[0], 25, APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                app_report_event(path, anc_cmd, app_idx, &anc_mp_tool_data.report_value[0], 25);
                only_report_status = false;
            }
        }
        break;

    case CMD_ANC_BURN_GAIN:
        {
            if (!app_anc_tool_burn_gain_prepare())
            {
                app_listening_cmd_postpone_stash(anc_cmd, param_ptr, param_len, path, app_idx,
                                                 ANC_APT_CMD_POSTPONE_ANC_WAIT_MEDIA_BUFFER);
                break;
            }

            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_ANC_BURN_WRITE) ? MP_TYPE_ONE_DEVICE :
                                    MP_TYPE_TWO_DEVICE;

            /*APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_ANC_BURN_GAIN param_len = %d, mp_type = %d, path = %d",
                             param_len, mp_type, path);
            */

            //take first section data
            anc_gain_data.ext_l_gain = param_ptr[0] | (param_ptr[1] << 8) | (param_ptr[2] << 16) |
                                       (param_ptr[3] << 24);
            anc_gain_data.ext_r_gain = param_ptr[4] | (param_ptr[5] << 8) | (param_ptr[6] << 16) |
                                       (param_ptr[7] << 24);
            anc_gain_data.int_l_gain = param_ptr[8] | (param_ptr[9] << 8) | (param_ptr[10] << 16) |
                                       (param_ptr[11] << 24);
            anc_gain_data.int_r_gain = param_ptr[12] | (param_ptr[13] << 8) | (param_ptr[14] << 16) |
                                       (param_ptr[15] << 24);
            uint32_t ext_data = param_ptr[16] | (param_ptr[17] << 8) | (param_ptr[18] << 16) |
                                (param_ptr[19] << 24);

            // enable set regs for ANC MP usage
            T_ANC_FEATURE_MAP feature_map;
            feature_map.d32 = anc_tool_get_feature_map();
            feature_map.set_regs = ENABLE;
            anc_tool_set_feature_map(feature_map.d32);

            anc_tool_disable();

            /*APP_PRINT_TRACE5("app_anc_cmd_handle: CMD_ANC_BURN_GAIN ext_data = %d, EXT_L = 0x%X, EXT_R = 0x%X, INT_L = 0x%X, INT_R = 0x%X",
                             ext_data, anc_gain_data.ext_l_gain, anc_gain_data.ext_r_gain,
                             anc_gain_data.int_l_gain, anc_gain_data.int_r_gain);
            */

            anc_tool_set_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, anc_gain_data.ext_l_gain,
                                       anc_gain_data.ext_r_gain);
            anc_tool_set_gain_mismatch(ANC_GAIN_SRC_INT_MIC, anc_gain_data.int_l_gain,
                                       anc_gain_data.int_r_gain);

            anc_tool_load_scenario(ANC_IMAGE_SUB_TYPE_ANC_COEF, anc_gain_data.grp_index);

            anc_status = app_anc_tool_burn_gain_mismatch(ext_data);

            app_anc_activated_scenario_init();
            app_cfg_nv.anc_selected_list = UINT16_SET_LSB_BITS(app_anc_get_activated_scenario_cnt());

            app_anc_tool_burn_gain_finish();

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                //take second section data
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_ANC_BURN_WRITE], MP_LEN_ANC_BURN_WRITE,
                                              APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_ANC_COMPARE:
        {
            only_report_status = true;
            anc_status = ANC_CMD_STATUS_SUCCESS;
        }
        break;

    case CMD_ANC_GEN_TONE:
        {
            WDG_Disable();

            only_report_status = true;
            uint16_t freq_count = param_len >> 2;    //byte to word
            uint8_t ch_sel = param_ptr[0];
            APP_PRINT_TRACE1("app_anc_cmd_handle: CMD_ANC_GEN_TONE freq_count %d", freq_count);
            uint32_t *tx_freq;
            uint32_t offset;
            tx_freq = (uint32_t *)malloc(param_len);
            if (tx_freq != NULL)
            {
                for (uint16_t i = 0; i < freq_count; i++)
                {
                    offset = i * 4 + 1;
                    uint32_t fq = (param_ptr[offset]) | (param_ptr[offset + 1] << 8) |
                                  (param_ptr[offset + 2] << 16) | (param_ptr[offset + 3] << 24);
                    APP_PRINT_TRACE1("app_anc_cmd_handle: fq %d", fq);
                    tx_freq[i] = fq;
                }
                uint8_t amp = param_ptr[param_len - 1];
                APP_PRINT_TRACE1("app_anc_cmd_handle: amp %x", amp);
                anc_tool_response_measure_enable(ENABLE, ch_sel, tx_freq, (uint8_t)freq_count, amp);

                anc_status = ANC_CMD_STATUS_SUCCESS;
                free(tx_freq);
            }
        }
        break;

    case CMD_ANC_CONFIG_DATA_LOG:
        {
            only_report_status = true;
            APP_PRINT_TRACE0("app_anc_cmd_handle: CMD_ANC_CONFIG_DATA_LOG");
            uint16_t len = (param_ptr[2]) | (param_ptr[3] << 8);
            anc_tool_config_data_log(param_ptr[0], param_ptr[1], len);
            // anc_tool_config_data_log(3, 4, len);
            anc_tool_load_data_log();
            anc_tool_response_measure_enable(DISABLE, 0, NULL, 0, 0x00);
            anc_status = ANC_CMD_STATUS_SUCCESS;
        }
        break;

    case CMD_ANC_READ_DATA_LOG:
        {
            uint32_t anc_data_log_length = anc_tool_get_data_log_length();
            uint32_t max_dsp_ram_address = anc_data_log_length  + DSP_SRAM_ADDR;
            uint32_t rx_address = param_ptr[0] | (param_ptr[1] << 8) | (param_ptr[2] << 16) |
                                  (param_ptr[3] << 24);
            uint8_t *dsp_ram_address;

            anc_tool_convert_data_log_addr(&max_dsp_ram_address);
            anc_tool_convert_data_log_addr(&rx_address);

            dsp_ram_address = (uint8_t *)rx_address;
            // APP_PRINT_TRACE2("Wade_max_dsp_ram_address = %x, rx_address = %x", max_dsp_ram_address, rx_address);

            //check remaining ram data on
            //extern size_t xPortGetFreeHeapSize(RAM_TYPE ramType);
            size_t remaining_ram_size = mem_peek();
            // uint8_t param[2];
            // param[0] = 0xD1;
            // param[1] = RAM_TYPE_DATA_ON;
            // au_universal_api(param, 2);

            if (remaining_ram_size < 68)
            {
                // APP_PRINT_TRACE0("Wade_remaining data on size not enough !!");
                only_report_status = true;
                anc_status = ANC_CMD_STATUS_FAILED;
                break;
            }

            if ((rx_address >= max_dsp_ram_address))
            {
                // APP_PRINT_TRACE0("Wade_over_max_dsp_ram_address");
                only_report_status = true;
                anc_status = ANC_CMD_STATUS_FAILED;
                break;
            }

            size_t read_word_count = (remaining_ram_size - 60) >>
                                     3;    //divide 2 for storage and uart tx, divide 4 for 4 byte an unit
            while (rx_address + (read_word_count * 4) > max_dsp_ram_address)
            {
                read_word_count--;
            }

            if (path == CMD_PATH_SPP)
            {
                if (read_word_count > MAX_ANC_READ_DATA_LOG_BY_SPP)
                {
                    read_word_count = MAX_ANC_READ_DATA_LOG_BY_SPP;
                }
            }
            else if (path == CMD_PATH_UART)
            {
                if (read_word_count > MAX_ANC_READ_DATA_LOG_BY_UART)
                {
                    read_word_count = MAX_ANC_READ_DATA_LOG_BY_UART;
                }
            }

            // APP_PRINT_TRACE1("Wade_expected_read_count = %d", read_word_count);
            // APP_PRINT_TRACE4("Wade_check_8bit = %x, %x, %x, %x", HAL_READ8(rx_address, 0), HAL_READ8(rx_address,
            //                  1), HAL_READ8(rx_address, 2), HAL_READ8(rx_address, 3));

            app_report_event(path, anc_cmd, app_idx, dsp_ram_address, read_word_count * 4);
        }
        break;

    case CMD_ANC_QUERY:
        {
            app_anc_report(EVENT_ANC_QUERY, NULL, 0);
        }
        break;

    case CMD_SPECIFY_ANC_QUERY:
        {
#if F_APP_SUPPORT_ANC_APT_COEXIST
            app_anc_report(EVENT_SPECIFY_ANC_QUERY, NULL, 0);
#endif
        }
        break;

    case CMD_ANC_ENABLE_DISABLE:
        {
            uint8_t enable = param_ptr[0];
            T_ANC_FEATURE_MAP feature_map;

            if (enable == ENABLE)
            {
                // temporarily enable set regs for anc_tool_enable
                T_ANC_FEATURE_MAP feature_map;
                feature_map.d32 = anc_tool_get_feature_map();
                feature_map.set_regs = ENABLE;
                anc_tool_set_feature_map(feature_map.d32);
                anc_tool_enable();
            }
            else if (enable == DISABLE)
            {
                APP_PRINT_TRACE0("anc_tool_disable");
                anc_tool_disable();
            }

            // resume ANC feature map setting
            feature_map.d32 = anc_tool_get_feature_map();
            feature_map.set_regs = DISABLE;
            anc_tool_set_feature_map(feature_map.d32);

            if (only_report_status == false)
            {
                // check ANC enable/disable success if needed
                anc_status = ANC_CMD_STATUS_SUCCESS;

                uint8_t report_value[2];

                report_value[0] = enable;
                report_value[1] = anc_status;

                app_report_event(path, anc_cmd, app_idx, report_value, 2);
            }
        }
        break;

    case CMD_ANC_LLAPT_WRITE_GAIN:
        {
            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_LLAPT_BURN_WRITE) ? MP_TYPE_ONE_DEVICE :
                                    MP_TYPE_TWO_DEVICE;

            //take first section data
            anc_gain_data.ext_l_gain = param_ptr[0] | (param_ptr[1] << 8) | (param_ptr[2] << 16) |
                                       (param_ptr[3] << 24);
            anc_gain_data.ext_r_gain = param_ptr[4] | (param_ptr[5] << 8) | (param_ptr[6] << 16) |
                                       (param_ptr[7] << 24);
            anc_gain_data.int_l_gain = param_ptr[8] | (param_ptr[9] << 8) | (param_ptr[10] << 16) |
                                       (param_ptr[11] << 24);
            anc_gain_data.int_r_gain = param_ptr[12] | (param_ptr[13] << 8) | (param_ptr[14] << 16) |
                                       (param_ptr[15] << 24);
            anc_gain_data.llapt_ext_l_gain = param_ptr[16] | (param_ptr[17] << 8) | (param_ptr[18] << 16) |
                                             (param_ptr[19] << 24);
            anc_gain_data.llapt_ext_r_gain = param_ptr[20] | (param_ptr[21] << 8) | (param_ptr[22] << 16) |
                                             (param_ptr[23] << 24);
            anc_gain_data.grp_index = param_ptr[24] | (param_ptr[25] << 8) | (param_ptr[26] << 16) |
                                      (param_ptr[27] << 24);

            // enable set regs for ANC MP usage
            T_ANC_FEATURE_MAP feature_map;
            feature_map.d32 = anc_tool_get_feature_map();
            feature_map.set_regs = ENABLE;
            anc_tool_set_feature_map(feature_map.d32);

            anc_tool_disable();

            if ((anc_gain_data.llapt_ext_l_gain == 0) || (anc_gain_data.llapt_ext_r_gain == 0))
            {
                anc_tool_set_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, anc_gain_data.ext_l_gain,
                                           anc_gain_data.ext_r_gain);
                anc_tool_set_gain_mismatch(ANC_GAIN_SRC_INT_MIC, anc_gain_data.int_l_gain,
                                           anc_gain_data.int_r_gain);
                anc_tool_load_scenario(ANC_IMAGE_SUB_TYPE_ANC_COEF, anc_gain_data.grp_index);
            }
            else
            {
                anc_tool_set_llapt_gain_mismatch(anc_gain_data.llapt_ext_l_gain, anc_gain_data.llapt_ext_r_gain);

                anc_tool_load_scenario(APT_IMAGE_SUB_TYPE_IDLE_COEF, anc_gain_data.grp_index);
            }

            anc_status = ANC_CMD_STATUS_SUCCESS;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                //take second section data
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_LLAPT_BURN_WRITE],
                                              MP_LEN_LLAPT_BURN_WRITE, APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_ANC_LLAPT_READ_GAIN:
        {
            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_READ) ? MP_TYPE_ONE_DEVICE :  MP_TYPE_TWO_DEVICE;

            /*APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_ANC_READ_GAIN param_len = %d, mp_type = %d, path = %d",
                             param_len, mp_type, path);
            */
            uint32_t d32 = 0;

            anc_tool_read_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, param_ptr[0], &anc_gain_data.ext_l_gain,
                                        &anc_gain_data.ext_r_gain);
            anc_tool_read_gain_mismatch(ANC_GAIN_SRC_INT_MIC, param_ptr[0], &anc_gain_data.int_l_gain,
                                        &anc_gain_data.int_r_gain);
            anc_tool_read_mp_ext_data(&d32);
            anc_tool_read_llapt_gain_mismatch(param_ptr[0], &anc_gain_data.llapt_ext_l_gain,
                                              &anc_gain_data.llapt_ext_r_gain);

            anc_mp_tool_data.report_value[0] = anc_gain_data.ext_l_gain;
            anc_mp_tool_data.report_value[1] = anc_gain_data.ext_l_gain >> 8;
            anc_mp_tool_data.report_value[2] = anc_gain_data.ext_l_gain >> 16;
            anc_mp_tool_data.report_value[3] = anc_gain_data.ext_l_gain >> 24;
            anc_mp_tool_data.report_value[4] = anc_gain_data.ext_r_gain;
            anc_mp_tool_data.report_value[5] = anc_gain_data.ext_r_gain >> 8;
            anc_mp_tool_data.report_value[6] = anc_gain_data.ext_r_gain >> 16;
            anc_mp_tool_data.report_value[7] = anc_gain_data.ext_r_gain >> 24;
            anc_mp_tool_data.report_value[8] = anc_gain_data.int_l_gain;
            anc_mp_tool_data.report_value[9] = anc_gain_data.int_l_gain >> 8;
            anc_mp_tool_data.report_value[10] = anc_gain_data.int_l_gain >> 16;
            anc_mp_tool_data.report_value[11] = anc_gain_data.int_l_gain >> 24;
            anc_mp_tool_data.report_value[12] = anc_gain_data.int_r_gain;
            anc_mp_tool_data.report_value[13] = anc_gain_data.int_r_gain >> 8;
            anc_mp_tool_data.report_value[14] = anc_gain_data.int_r_gain >> 16;
            anc_mp_tool_data.report_value[15] = anc_gain_data.int_r_gain >> 24;
            anc_mp_tool_data.report_value[16] = anc_gain_data.llapt_ext_l_gain;
            anc_mp_tool_data.report_value[17] = anc_gain_data.llapt_ext_l_gain >> 8;
            anc_mp_tool_data.report_value[18] = anc_gain_data.llapt_ext_l_gain >> 16;
            anc_mp_tool_data.report_value[19] = anc_gain_data.llapt_ext_l_gain >> 24;
            anc_mp_tool_data.report_value[20] = anc_gain_data.llapt_ext_r_gain;
            anc_mp_tool_data.report_value[21] = anc_gain_data.llapt_ext_r_gain >> 8;
            anc_mp_tool_data.report_value[22] = anc_gain_data.llapt_ext_r_gain >> 16;
            anc_mp_tool_data.report_value[23] = anc_gain_data.llapt_ext_r_gain >> 24;
            anc_mp_tool_data.report_value[24] = d32;
            anc_mp_tool_data.report_value[25] = d32 >> 8;
            anc_mp_tool_data.report_value[26] = d32 >> 16;
            anc_mp_tool_data.report_value[27] = d32 >> 24;
            anc_mp_tool_data.report_value[28] = anc_gain_data.grp_index;
            anc_mp_tool_data.report_value[29] = anc_gain_data.grp_index >> 8;
            anc_mp_tool_data.report_value[30] = anc_gain_data.grp_index >> 16;
            anc_mp_tool_data.report_value[31] = anc_gain_data.grp_index >> 24;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_READ], MP_LEN_READ, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_mp_tool_data.report_value[0], 32, APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                app_report_event(path, anc_cmd, app_idx, &anc_mp_tool_data.report_value[0], 32);
                only_report_status = false;
            }
        }
        break;

    case CMD_ANC_LLAPT_BURN_GAIN:
        {
            if (!app_anc_tool_burn_gain_prepare())
            {
                app_listening_cmd_postpone_stash(anc_cmd, param_ptr, param_len, path, app_idx,
                                                 ANC_APT_CMD_POSTPONE_ANC_WAIT_MEDIA_BUFFER);
                break;
            }

            T_ANC_MP_TYPE mp_type = (param_len == MP_LEN_LLAPT_BURN_WRITE) ? MP_TYPE_ONE_DEVICE :
                                    MP_TYPE_TWO_DEVICE;

            //take first section data
            anc_gain_data.ext_l_gain = param_ptr[0] | (param_ptr[1] << 8) | (param_ptr[2] << 16) |
                                       (param_ptr[3] << 24);
            anc_gain_data.ext_r_gain = param_ptr[4] | (param_ptr[5] << 8) | (param_ptr[6] << 16) |
                                       (param_ptr[7] << 24);
            anc_gain_data.int_l_gain = param_ptr[8] | (param_ptr[9] << 8) | (param_ptr[10] << 16) |
                                       (param_ptr[11] << 24);
            anc_gain_data.int_r_gain = param_ptr[12] | (param_ptr[13] << 8) | (param_ptr[14] << 16) |
                                       (param_ptr[15] << 24);
            anc_gain_data.llapt_ext_l_gain = param_ptr[16] | (param_ptr[17] << 8) | (param_ptr[18] << 16) |
                                             (param_ptr[19] << 24);
            anc_gain_data.llapt_ext_r_gain = param_ptr[20] | (param_ptr[21] << 8) | (param_ptr[22] << 16) |
                                             (param_ptr[23] << 24);
            anc_gain_data.grp_index = param_ptr[24] | (param_ptr[25] << 8) | (param_ptr[26] << 16) |
                                      (param_ptr[27] << 24);

            // enable set regs for ANC MP usage
            T_ANC_FEATURE_MAP feature_map;
            feature_map.d32 = anc_tool_get_feature_map();
            feature_map.set_regs = ENABLE;
            anc_tool_set_feature_map(feature_map.d32);

            anc_tool_disable();

            if ((anc_gain_data.llapt_ext_l_gain == 0) || (anc_gain_data.llapt_ext_r_gain == 0))
            {
                anc_tool_set_gain_mismatch(ANC_GAIN_SRC_EXT_MIC, anc_gain_data.ext_l_gain,
                                           anc_gain_data.ext_r_gain);
                anc_tool_set_gain_mismatch(ANC_GAIN_SRC_INT_MIC, anc_gain_data.int_l_gain,
                                           anc_gain_data.int_r_gain);

                anc_tool_load_scenario(ANC_IMAGE_SUB_TYPE_ANC_COEF, anc_gain_data.grp_index);

                anc_status = app_anc_tool_burn_gain_mismatch(anc_gain_data.grp_index);
            }
            else
            {
                uint32_t tmp_llapt_ext_data;

                anc_tool_set_llapt_gain_mismatch(anc_gain_data.llapt_ext_l_gain, anc_gain_data.llapt_ext_r_gain);

                anc_tool_load_scenario(APT_IMAGE_SUB_TYPE_IDLE_COEF, anc_gain_data.grp_index);

                anc_tool_read_llapt_ext_data(&tmp_llapt_ext_data);

                anc_status = app_anc_tool_burn_llapt_gain_mismatch(tmp_llapt_ext_data);
            }

            app_anc_tool_burn_gain_finish();

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                //take second section data
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[MP_LEN_LLAPT_BURN_WRITE],
                                              MP_LEN_LLAPT_BURN_WRITE, APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_ANC_LLAPT_FEATURE_CONTROL:
        {
            uint16_t opcode = param_ptr[0] | (param_ptr[1] << 8);
            T_ANC_MP_TYPE mp_type = (T_ANC_MP_TYPE)param_ptr[2];
            uint16_t len = (param_len - MP_LEN_SUBOPCODE) / mp_type;
            uint8_t report_status = ANC_CMD_STATUS_SUCCESS;

            switch (opcode)
            {
            case ANC_LLAPT_FEATURE_WNS_SWITCH:
                {
                    // enable set regs for ANC MP usage
                    T_ANC_FEATURE_MAP feature_map;
                    feature_map.d32 = anc_tool_get_feature_map();

                    if (feature_map.user_mode == DISABLE)
                    {
                        feature_map.enable_wns = param_ptr[3];
                        anc_tool_set_feature_map(feature_map.d32);

                        anc_tool_limiter_wns_switch();
                    }
                }
                break;

            default:
                break;
            }

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC) &&
                (mp_type == MP_TYPE_TWO_DEVICE))
            {
                uint8_t *event_buff = (uint8_t *)malloc(len + MP_LEN_SUBOPCODE + 9);
                event_buff[0] = (uint8_t)anc_cmd;
                event_buff[1] = (uint8_t)(anc_cmd >> 8);
                event_buff[2] = (uint8_t)(len + MP_LEN_SUBOPCODE);
                event_buff[3] = (uint8_t)((len + MP_LEN_SUBOPCODE) >> 8);
                event_buff[4] = CMD_PATH_RWS_ASYNC;
                event_buff[5] = app_idx;
                event_buff[6] = (uint8_t)opcode;
                event_buff[7] = (uint8_t)(opcode >> 8);
                event_buff[8] = mp_type;
                memcpy(&event_buff[9], &param_ptr[len + MP_LEN_SUBOPCODE], len + MP_LEN_SUBOPCODE);

                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL,
                                                    &event_buff[0], len + MP_LEN_SUBOPCODE + 6);
                free(event_buff);
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_relay_async_single(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL_FINISH);
            }
            else
            {
                app_report_event(path, anc_cmd, app_idx, &report_status, 1);
                only_report_status = false;
            }
        }
        break;

    case CMD_ANC_CONFIG_MEASURE_MIC:
        {
            uint8_t audio_category, logical_mic;
            T_AUDIO_ROUTE_PHYSICAL_MIC physical_mic;

            only_report_status = true;

            anc_disable();

            switch ((T_ANC_RESP_MEAS_MIC_SEL)param_ptr[0])
            {
            case ANC_RESP_MEAS_LEFT_FEED_FORWARD_MIC:
                {
                    audio_category = AUDIO_CATEGORY_ANC;
                    logical_mic = AUDIO_ROUTE_LOGIC_EXTERNAL_MIC_LEFT;
                }
                break;

            case ANC_RESP_MEAS_VOICE_PRIMARY_MIC:
                {
                    audio_category = AUDIO_CATEGORY_VOICE;
                    logical_mic = AUDIO_ROUTE_LOGIC_PRIMARY_MIC;
                }
                break;

            case ANC_RESP_MEAS_VOICE_SECONDARY_MIC:
                {
                    audio_category = AUDIO_CATEGORY_VOICE;
                    logical_mic = AUDIO_ROUTE_LOGIC_SECONDARY_MIC;
                }
                break;

            case ANC_RESP_MEAS_APT_PRIMARY_MIC:
                {
                    audio_category = AUDIO_CATEGORY_APT;
                    logical_mic = AUDIO_ROUTE_LOGIC_PRIMARY_MIC;
                }
                break;

            case ANC_RESP_MEAS_APT_SECONDARY_MIC:
                {
                    audio_category = AUDIO_CATEGORY_APT;
                    logical_mic = AUDIO_ROUTE_LOGIC_SECONDARY_MIC;
                }
                break;

            default:
                break;
            }

            app_audio_route_physical_mic_get((T_AUDIO_CATEGORY)audio_category,
                                             (T_AUDIO_ROUTE_LOGIC_IO_TYPE)logical_mic,
                                             &physical_mic);

            app_audio_route_physical_mic_set(AUDIO_CATEGORY_ANC,
                                             AUDIO_ROUTE_LOGIC_EXTERNAL_MIC_LEFT,
                                             physical_mic);

            anc_enable(0xff);

            anc_status = ANC_CMD_STATUS_SUCCESS;
        }
        break;

    case CMD_RAMP_GET_INFO:
        {
            uint8_t report_data[7];

            report_data[0] = 0;    //default version
            report_data[1] = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? 1 : 0;
            report_data[2] = app_cfg_const.bud_side;
            report_data[3] = max_payload_len;
            report_data[4] = max_payload_len >> 8;
            report_data[5] = app_db.local_batt_level;
            report_data[6] = app_db.remote_batt_level;

            APP_PRINT_TRACE5("audio_app_anc ramp_get_info is_engage = %x, channel = %x, supported_size = %d, pri_vol = %d, sec_vol = %d",
                             report_data[1], report_data[2], max_payload_len, app_db.local_batt_level,
                             app_db.remote_batt_level);

            app_report_event(path, EVENT_RAMP_GET_INFO, app_idx, report_data, sizeof(report_data));
        }
        break;

    case CMD_RAMP_BURN_PARA_START:
        {
            /* byte[0]   : channel       *
             * byte[1-4] : total length  *
             * byte [5]  : bit[2:0] = mode, bit[3] encypted [7:4]reserve
             * byte[6-8] : reserve       */

            uint8_t channel = param_ptr[0];
            uint16_t total_length = (uint16_t)((param_ptr[1]) | (param_ptr[2] << 8) | (param_ptr[3] << 16) |
                                               (param_ptr[4] << 24));
            // spk2 not care length check
            bool length_valid = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
                                 app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) ? true
                                : (max_payload_len  >= param_len) ? true : false;

            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            anc_status = (!length_valid)  ? ANC_CMD_STATUS_CMD_LENGTH_INVALID :
                         (!channel_match) ? ANC_CMD_STATUS_CHANNEL_NOT_MATCH : anc_status;

            APP_PRINT_TRACE2("audio_app_anc ramp_burn_para_start channel = %x, total_length = %d",
                             channel, total_length);

            if (channel_match && length_valid)
            {
                if (app_anc_ramp_tool_data_initial(NULL, total_length))
                {
                    anc_ramp_tool_data.mode = (T_ANC_RAMP_MODE)(param_ptr[5] & 0x07);
                    anc_ramp_tool_data.encrypted = (T_ANC_RAMP_ENCRYPTED)((param_ptr[5] & BIT3) >> 3);

                    anc_status = ANC_CMD_STATUS_SUCCESS;
                }
            }
            else
            {
                anc_status = ANC_CMD_STATUS_CHANNEL_NOT_MATCH;
            }

            if ((!channel_match) &&
                (length_valid) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_RAMP_BURN_PARA_CONTINUE:
        {
            /* byte[0,1]: CRC                       *
             * byte[2]  : channel                   *
             * byte[3]  : parameter type(ANC/LLAPT) *
             * byte[4]  : group index               *
             * byte[5]  : segment total             *
             * byte[6]  : segment index             *
             * byte[7]  : payload                   */

            uint16_t crc_check = param_ptr[0] | (param_ptr[1] << 8);
            uint8_t channel = param_ptr[2];
            uint8_t para_type = param_ptr[3];
            uint8_t grp_index = param_ptr[4];

            uint16_t crc_cal_length = param_len - 2;  //start from index 2
            uint16_t payload_length = param_len - 7;
            uint8_t segement_total = param_ptr[5];
            uint8_t segement_index = param_ptr[6];
            uint8_t relay_ptr[3];

            if (segement_index == 0 && anc_ramp_tool_data.mode == APP_ANC_RAMP_APPLY_MODE)
            {
                if (para_type == ANC_IMAGE_SUB_TYPE_ANC_COEF)
                {
                    relay_ptr[0] = ANC_ON_SCENARIO_1_APT_OFF + grp_index;
                }
                else if (para_type == APT_IMAGE_SUB_TYPE_IDLE_COEF ||
                         para_type == APT_IMAGE_SUB_TYPE_A2DP_COEF ||
                         para_type == APT_IMAGE_SUB_TYPE_SCO_COEF)
                {
                    relay_ptr[0] = ANC_OFF_LLAPT_ON_SCENARIO_1 + grp_index;
                }

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    relay_ptr[1] = false;
                    relay_ptr[2] = true;

                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_LISTENING_MODE,
                                                       APP_REMOTE_MSG_LISTENING_SOURCE_CONTROL,
                                                       relay_ptr, 3, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                }
                else
                {
                    app_listening_assign_specific_state(*app_db.final_listening_state,
                                                        (T_ANC_APT_STATE)relay_ptr[0], false, true);
                }
            }

            // spk2 not care length check
            bool length_valid = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
                                 app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) ? true
                                : (max_payload_len >= param_len) ? true : false;

            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            anc_status = (!length_valid)  ? ANC_CMD_STATUS_CMD_LENGTH_INVALID :
                         (!channel_match) ? ANC_CMD_STATUS_CHANNEL_NOT_MATCH : anc_status;

            APP_PRINT_TRACE7("audio_app_anc ramp_burn_para_continue channel = %x, para_type = %x, grp_index = %x, segment index = %x, segment_total = %x, crc = %x, length = %d",
                             channel, para_type, grp_index, segement_index, segement_total, crc_check, payload_length);

            if (channel_match && length_valid)
            {
                APP_PRINT_TRACE3("audio_app_anc ramp_burn_para_continue doing = %x, data_segement_index = %x, crc_result = %x",
                                 anc_ramp_tool_data.ramp_burn_start, anc_ramp_tool_data.data_segement_count,
                                 dfu_checkbufcrc(&param_ptr[2], crc_cal_length, crc_check));

                anc_status = app_anc_ramp_tool_data_check(segement_total, payload_length, &param_ptr[2],
                                                          crc_cal_length, crc_check);

                if (anc_status == ANC_CMD_STATUS_SUCCESS)
                {
                    memcpy_s(&anc_ramp_tool_data.buffer_ptr[anc_ramp_tool_data.data_receive_length],
                             (anc_ramp_tool_data.buffer_size - anc_ramp_tool_data.data_receive_length),
                             &param_ptr[7], payload_length);

                    anc_ramp_tool_data.data_receive_length += payload_length;
                    anc_ramp_tool_data.data_segement_count++;
                    anc_ramp_tool_data.parameter_type = para_type;
                    anc_ramp_tool_data.parameter_group_index = grp_index;

                    //check data segement finish or not
                    if (anc_ramp_tool_data.data_segement_count == segement_total)
                    {
                        if (anc_ramp_tool_data.data_receive_length == anc_ramp_tool_data.buffer_size)
                        {
                            if (anc_ramp_tool_data.mode == APP_ANC_RAMP_BURN_MODE)
                            {
                                if (anc_ramp_tool_data.encrypted == APP_ANC_RAMP_DECRYPTED)
                                {
                                    anc_tool_para_aes_encrypt((uint32_t)anc_ramp_tool_data.buffer_ptr,
                                                              anc_ramp_tool_data.data_receive_length);

                                    anc_status = app_anc_ramp_tool_burn_parameter();

                                    if (anc_status == ANC_CMD_STATUS_SUCCESS)
                                    {
                                        anc_status = app_anc_ramp_tool_burn_confirm();

                                        app_listening_assign_specific_state(*app_db.final_listening_state,
                                                                            ANC_OFF_APT_OFF, false, true);

                                        if (para_type == ANC_IMAGE_SUB_TYPE_ANC_COEF)
                                        {
                                            relay_ptr[0] = ANC_ON_SCENARIO_1_APT_OFF + grp_index;
                                        }
                                        else if (para_type == APT_IMAGE_SUB_TYPE_IDLE_COEF ||
                                                 para_type == APT_IMAGE_SUB_TYPE_A2DP_COEF ||
                                                 para_type == APT_IMAGE_SUB_TYPE_SCO_COEF)
                                        {
                                            relay_ptr[0] = ANC_OFF_LLAPT_ON_SCENARIO_1 + grp_index;
                                        }

                                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                                        {
                                            relay_ptr[1] = false;
                                            relay_ptr[2] = true;

                                            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_LISTENING_MODE,
                                                                               APP_REMOTE_MSG_LISTENING_SOURCE_CONTROL,
                                                                               relay_ptr, 3, REMOTE_TIMER_HIGH_PRECISION, 0, false);
                                        }
                                        else
                                        {
                                            app_listening_assign_specific_state(*app_db.final_listening_state,
                                                                                (T_ANC_APT_STATE)relay_ptr[0], false, true);
                                        }
                                    }
                                }
                                else
                                {
                                    anc_status = app_anc_ramp_tool_burn_parameter();

                                    if (anc_status == ANC_CMD_STATUS_SUCCESS)
                                    {
                                        anc_status = app_anc_ramp_tool_burn_confirm();
                                    }
                                }

                                APP_PRINT_TRACE1("audio_app_anc ramp_burn_para_continue done status = %x", anc_status);
                            }
                            else if (anc_ramp_tool_data.mode == APP_ANC_RAMP_APPLY_MODE)
                            {
                                anc_status = app_anc_ramp_tool_apply_parameter();
                            }
                        }
                        else
                        {
                            anc_status = ANC_CMD_STATUS_LENGTH_FAIL;
                        }
                    }
                }
            }
            else
            {
                anc_status = ANC_CMD_STATUS_CHANNEL_NOT_MATCH;
            }

            if ((anc_status != ANC_CMD_STATUS_SUCCESS) && (anc_ramp_tool_data.ramp_burn_start))
            {
                app_anc_ramp_tool_data_reset();
            }

            if ((!channel_match) &&
                (length_valid) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_RAMP_BURN_GRP_INFO:
        {
            /* byte[0]   : channel  *
             * byte[1~4] : grp_info */

            if (!app_anc_tool_burn_gain_prepare())
            {
                app_listening_cmd_postpone_stash(anc_cmd, param_ptr, param_len, path, app_idx,
                                                 ANC_APT_CMD_POSTPONE_ANC_WAIT_MEDIA_BUFFER);
                break;
            }

            uint8_t channel = param_ptr[0];

            // spk2 not care length check
            bool length_valid = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
                                 app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) ? true
                                : (max_payload_len  >= param_len) ? true : false;

            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            anc_status = (!length_valid)  ? ANC_CMD_STATUS_CMD_LENGTH_INVALID :
                         (!channel_match) ? ANC_CMD_STATUS_CHANNEL_NOT_MATCH : anc_status;

            if (channel_match && length_valid)
            {
                uint32_t ext_data = param_ptr[1] | (param_ptr[2] << 8) | (param_ptr[3] << 16) |
                                    (param_ptr[4] << 24);

                anc_status = app_anc_tool_burn_gain_mismatch(ext_data);
            }

            app_anc_tool_burn_gain_finish();

            if ((!channel_match) &&
                (length_valid) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_RAMP_MULTI_DEVICE_APPLY:
        {
            //BBLite Dcut - CMD_ANC_VENDOR_COMMAND
            //BBPro2      - CMD_ANC_WRITE_REGISTER

            /* CMD_RAMP_MULTI_DEVICE_APPLY         *
             * byte[0]   : channel                 *
             * byte[1~N] : CMD_ANC_WRITE_REGISTER  */

            uint8_t channel = param_ptr[0];
            uint8_t data_len = param_ptr[1];

            // spk2 not care length check
            bool length_valid = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
                                 app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) ? true
                                : (max_payload_len >= param_len) ? true : false;

            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            anc_status = (!length_valid)  ? ANC_CMD_STATUS_CMD_LENGTH_INVALID :
                         (!channel_match) ? ANC_CMD_STATUS_CHANNEL_NOT_MATCH : anc_status;

            if (channel_match && length_valid)
            {
                APP_PRINT_TRACE5("app_anc_cmd_handle: CMD_RAMP_MULTI_DEVICE_APPLY: data_len 0x%02x, anc_cmd_ptr 0x%02X 0x%02X 0x%02X 0x%02X",
                                 data_len, param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5]);

                anc_status = anc_tool_set_para((void *)&param_ptr[1]);    //8773 BFE/CFE difference
            }

            if ((!channel_match) &&
                (length_valid) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                only_report_status = true;
            }
        }
        break;

    case CMD_LISTENING_MODE_CYCLE_SET:
        {
            uint8_t report_value = 0xff;
            uint8_t cycle_mode = param_ptr[0];

            /*0: All off->ANC on->APT on,1: All off->APT on->ANC on,2: APT on<->ANC on, 3: All off<->ANC on*/
            /*4: All off->ANC on->APT on->ANC+APT on*/
            if (cycle_mode < 5)
            {
                app_cfg_nv.listening_mode_cycle = (cycle_mode & 0x03);
                app_cfg_nv.listening_mode_cycle_ext = ((cycle_mode & 0x0C) >> 2);
                report_value = cycle_mode;
                app_cfg_store(&app_cfg_nv.offset_listening_mode_cycle, 4);
                app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);

                app_report_event(path, EVENT_LISTENING_MODE_CYCLE_SET, app_idx, &report_value, 1);
                app_relay_async_single(APP_MODULE_TYPE_LISTENING_MODE, APP_REMOTE_MSG_LISTENING_MODE_CYCLE_SYNC);
            }

            APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_LISTENING_MODE_CYCLE_SET: cycle_mode 0x%02x, report_value 0x%02x, listening_mode_cycle 0x%02x,",
                             cycle_mode, report_value, app_cfg_nv.listening_mode_cycle);

        }
        break;

    case CMD_LISTENING_MODE_CYCLE_GET:
        {
            uint8_t report_value ;

            report_value = ((app_cfg_nv.listening_mode_cycle_ext << 2 & 0x0C) |
                            app_cfg_nv.listening_mode_cycle);
            app_report_event(path, EVENT_LISTENING_MODE_CYCLE_GET, app_idx, &report_value, 1);
        }
        break;
#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
    case CMD_ANC_GET_ADSP_INFO:
        {
            uint8_t adsp_info[3] = {0, 0, 0};
            // call driver to get version and mtu_size
            anc_status = anc_tool_query_para(&adsp_info[0]);
            APP_PRINT_TRACE3("app_anc_cmd_handle: CMD_ANC_GET_ADSP_INFO: adsp_info 0x%02X 0x%02X 0x%02X",
                             adsp_info[0], adsp_info[1], adsp_info[2]);

            app_report_event(path, EVENT_ANC_GET_ADSP_INFO, app_idx, (uint8_t *)&adsp_info[0], 3);
        }
        break;
    case CMD_ANC_SEND_ADSP_PARA_START:
        {
            /* byte[0]   : type       *
             * byte[1-2] : CRC  *
             * byte[3-4] : reserve       *
             * byte[5-8] : encode       *
             * byte[9-12] : ADSP para length       *
             * byte[13]   : seqment total          */
            uint8_t para_type = param_ptr[0];
            uint16_t crc_check = 0;
            uint32_t encode = 0;
            uint32_t adsp_para_len = 0;
            uint8_t segment_total = 0;
            anc_status = ANC_CMD_STATUS_SUCCESS;
            if ((para_type == 1) && (param_len > 1)) // adsp enable with payload
            {
                para_type = param_ptr[0];
                crc_check = param_ptr[1] | (param_ptr[2] << 8);
                encode = param_ptr[5] | (param_ptr[6] << 8) | (param_ptr[7] << 16) | (param_ptr[8] << 24);
                adsp_para_len = param_ptr[9] | (param_ptr[10] << 8) | (param_ptr[11] << 16) | (param_ptr[12] << 24);
                segment_total = param_ptr[13];
                APP_PRINT_TRACE7("app_anc_cmd_handle: CMD_ANC_SEND_ADSP_PARA_START: anc_cmd_ptr 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                                 param_ptr[0], param_ptr[1], param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5], param_ptr[6]);

                APP_PRINT_TRACE5("app_anc_cmd_handle para type = 0x%x, crc = 0x%x, encode = 0x%x, adsp_para_len = 0x%x, segment_total = 0x%x",
                                 para_type, crc_check, encode, adsp_para_len, segment_total);
                anc_status = anc_tool_load_adsp_image(encode);
                anc_status = anc_tool_config_adsp_para(crc_check, adsp_para_len, segment_total);

            }
            else if (para_type == 1) // only turn on ADSP
            {
                anc_status = anc_tool_turn_on_adsp(ENABLE);
            }
            else if (para_type == 0) // only turn off ADSP
            {
                anc_status = anc_tool_turn_on_adsp(DISABLE);
            }
            else if (para_type == 2)
            {
                uint8_t status = 0;
                status = anc_tool_get_adsp_image_load_done();
                anc_status = (status == 0) ? ANC_CMD_STATUS_SUCCESS : ANC_CMD_STATUS_CMD_NOT_READY;
            }


            app_report_event(path, EVENT_ANC_SEND_ADSP_PARA_START, app_idx, &anc_status, 1);
        }
        break;
    case CMD_ANC_SEND_ADSP_PARA_CONTINUE:
        {
            /* byte[0]  : segment index             *
             * byte[1]  : payload                   */
            uint8_t segment_index = param_ptr[0];
            uint8_t *payload = &param_ptr[1];

            APP_PRINT_TRACE7("app_anc_cmd_handle: CMD_ANC_SEND_ADSP_PARA_CONTINUE: segment_index 0x%02X payload 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                             segment_index, param_ptr[1], param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5], param_ptr[6]);

            anc_status = anc_tool_load_adsp_para(payload, segment_index);
            app_report_event(path, EVENT_ANC_SEND_ADSP_PARA_CONTINUE, app_idx, &anc_status, 1);
        }
        break;
    case CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC:
        {
            /* byte[0]   : type       *
             * byte[1]   : grp_index  */

            APP_PRINT_TRACE7("app_anc_cmd_handle: CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC: anc_cmd_ptr 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                             param_ptr[0], param_ptr[1], param_ptr[2], param_ptr[3], param_ptr[4], param_ptr[5], param_ptr[6]);

            anc_status = anc_tool_enable_adaptive_anc(param_ptr[0], param_ptr[1]);
            app_report_event(path, EVENT_ANC_ENABLE_DISABLE_ADAPTIVE_ANC, app_idx, (uint8_t *)&anc_status, 1);
        }
        break;
#endif

    case CMD_ANC_SCENARIO_CHOOSE_INFO:
        {
            app_anc_report(EVENT_ANC_SCENARIO_CHOOSE_INFO, NULL, 0);
        }
        break;

    case CMD_ANC_SCENARIO_CHOOSE_TRY:
        {
            uint8_t anc_enable;
            uint8_t anc_scenario;

            anc_enable = (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? param_ptr[0] : param_ptr[2];
            anc_scenario = (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? param_ptr[1] : param_ptr[3];

            if (anc_group_data.enter_anc_select_mode == false)
            {
                app_anc_enter_scenario_select_mode();
            }

            if (anc_enable == ENABLE)
            {
                if (anc_scenario < app_anc_get_activated_scenario_cnt())
                {
                    app_listening_state_machine(ANC_STATE_TO_EVENT(ANC_ON_SCENARIO_1_APT_OFF + anc_scenario),
                                                true, false);
                }
            }
            else
            {
                app_listening_state_machine(EVENT_ANC_OFF, true, false);
            }

            anc_status = ANC_CMD_STATUS_SUCCESS;

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                //rws primary
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                //rws secondary
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                //single primary
                app_anc_report(EVENT_ANC_SCENARIO_CHOOSE_TRY, &anc_status, 1);
            }
        }
        break;

    case CMD_ANC_SCENARIO_CHOOSE_RESULT:
        {
            if (param_len != 0)
            {
                uint32_t l_ext_data;
                uint32_t r_ext_data;

                LE_ARRAY_TO_UINT16(l_ext_data, &param_ptr[0]);
                LE_ARRAY_TO_UINT16(r_ext_data, &param_ptr[4]);

                anc_status = app_anc_check_anc_choose_data_valid(l_ext_data,
                                                                 r_ext_data) ? ANC_CMD_STATUS_SUCCESS : ANC_CMD_STATUS_FAILED;

                if (anc_status == ANC_CMD_STATUS_SUCCESS)
                {
                    uint32_t used_ext_data;

                    used_ext_data = (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? l_ext_data : r_ext_data;

                    app_cfg_nv.anc_selected_list = (uint16_t)used_ext_data;
                    app_cfg_store(&app_cfg_nv.anc_selected_list, sizeof(uint16_t));

                    app_anc_set_first_anc_sceanrio(&app_db.last_anc_on_state);
#if F_APP_SUPPORT_ANC_APT_COEXIST
                    app_db.last_anc_apt_on_state = ANC_TO_ANC_APT_STATE(app_db.last_anc_on_state);
#endif
                }
            }
            else
            {
                //exit the choose action and do nothing
                anc_status = ANC_CMD_STATUS_SUCCESS;
            }

            app_anc_exit_scenario_select_mode();

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                //rws primary
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    anc_status = ANC_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))

            {
                //rws secondary
                app_cmd_relay_event(anc_cmd, &anc_status, sizeof(anc_status), APP_MODULE_TYPE_ANC,
                                    APP_REMOTE_MSG_RELAY_ANC_EVENT);
            }
            else
            {
                //single primary
                app_anc_report(EVENT_ANC_SCENARIO_CHOOSE_RESULT, &anc_status, 1);
            }
        }
        break;

    case CMD_ANC_GET_SCENARIO_IMAGE:
        {
            uint8_t channel = param_ptr[0];
            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            app_anc_ramp_tool_data_reset();

            if (channel_match)
            {
                uint8_t para_type = param_ptr[1];
                uint8_t grp_index = param_ptr[2];
                uint32_t total_length = 0;
                uint32_t payload_length = max_payload_len - 9;
                uint32_t report_length = 0;
                uint8_t *buf_ptr;
                uint32_t crc = 0;
                uint32_t segment_total = 0;
                uint8_t *report_value = (uint8_t *)malloc(max_payload_len);
                uint8_t param[10];

                buf_ptr = anc_tool_get_scenario_img(para_type, grp_index, &total_length);

                crc = app_anc_cal_crc16(buf_ptr, total_length);
                segment_total = CEIL_DIV(total_length, payload_length);

                report_value[0] = crc;
                report_value[1] = (crc >> 8);
                report_value[2] = channel;
                report_value[3] = para_type;
                report_value[4] = grp_index;
                report_value[5] = total_length;
                report_value[6] = (total_length >> 8);
                report_value[7] = segment_total;

                if (app_anc_ramp_tool_data_initial(buf_ptr, total_length))
                {
                    while (segment_total > anc_ramp_tool_data.data_segement_count)
                    {
                        report_value[8] = anc_ramp_tool_data.data_segement_count;

                        memcpy(&report_value[9], &anc_ramp_tool_data.buffer_ptr[anc_ramp_tool_data.data_receive_length],
                               payload_length);

                        if ((total_length - anc_ramp_tool_data.data_receive_length) < payload_length)
                        {
                            payload_length = total_length - anc_ramp_tool_data.data_receive_length;
                            report_length = payload_length + 9;
                        }
                        else
                        {
                            report_length = max_payload_len;
                        }

                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                        {
                            app_report_event(path, EVENT_ANC_GET_SCENARIO_IMAGE, app_idx, &report_value[0], report_length);
                        }
                        else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                                 (path == CMD_PATH_RWS_ASYNC))
                        {
                            app_cmd_relay_event(EVENT_ANC_GET_SCENARIO_IMAGE, &report_value[0], report_length,
                                                APP_MODULE_TYPE_ANC,
                                                APP_REMOTE_MSG_RELAY_ANC_EVENT);
                        }

                        anc_ramp_tool_data.data_receive_length += payload_length;
                        anc_ramp_tool_data.data_segement_count++;
                    }

                    anc_status = ANC_CMD_STATUS_SUCCESS;
                }

                free(report_value);
                app_anc_ramp_tool_data_reset();
            }
            else
            {
                anc_status = ANC_CMD_STATUS_CHANNEL_NOT_MATCH;
            }

            if ((!channel_match) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
            }
        }
        break;

    case CMD_ANC_GET_FILTER_INFO:
        {
            uint8_t channel = param_ptr[0];
            bool channel_match = app_anc_ramp_tool_channel_match((T_CHANNEL_TYPE)channel);

            app_anc_ramp_tool_data_reset();

            if (channel_match)
            {
                uint32_t total_length = 0;
                uint32_t report_length = 0;
                uint32_t payload_length = max_payload_len - 7;
                uint8_t *buf_ptr;
                uint32_t crc = 0;
                uint32_t segment_total = 0;
                uint8_t *report_value = (uint8_t *)malloc(max_payload_len);
                uint8_t param[10];

                buf_ptr = anc_tool_get_filter_info_img(&total_length);
                crc = app_anc_cal_crc16(buf_ptr, total_length);
                segment_total = CEIL_DIV(total_length, payload_length);

                report_value[0] = crc;
                report_value[1] = (crc >> 8);
                report_value[2] = channel;
                report_value[3] = total_length;
                report_value[4] = (total_length >> 8);
                report_value[5] = segment_total;

                if (app_anc_ramp_tool_data_initial(buf_ptr, total_length))
                {
                    while (segment_total > anc_ramp_tool_data.data_segement_count)
                    {
                        report_value[6] = anc_ramp_tool_data.data_segement_count;

                        memcpy(&report_value[7], &anc_ramp_tool_data.buffer_ptr[anc_ramp_tool_data.data_receive_length],
                               payload_length);

                        if ((total_length - anc_ramp_tool_data.data_receive_length) < payload_length)
                        {
                            payload_length = total_length - anc_ramp_tool_data.data_receive_length;
                            report_length = payload_length + 7;
                        }
                        else
                        {
                            report_length = max_payload_len;
                        }

                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                        {
                            app_report_event(path, EVENT_ANC_GET_FILTER_INFO, app_idx, &report_value[0], report_length);
                        }
                        else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                                 (path == CMD_PATH_RWS_ASYNC))
                        {
                            app_cmd_relay_event(EVENT_ANC_GET_FILTER_INFO, &report_value[0], report_length, APP_MODULE_TYPE_ANC,
                                                APP_REMOTE_MSG_RELAY_ANC_EVENT);
                        }

                        anc_ramp_tool_data.data_receive_length += payload_length;
                        anc_ramp_tool_data.data_segement_count++;
                    }

                    anc_status = ANC_CMD_STATUS_SUCCESS;
                }

                free(report_value);
                app_anc_ramp_tool_data_reset();
            }
            else
            {
                anc_status = ANC_CMD_STATUS_CHANNEL_NOT_MATCH;
            }

            if ((!channel_match) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                if (app_cmd_relay_command_set(anc_cmd, &param_ptr[0], param_len, APP_MODULE_TYPE_ANC,
                                              APP_REMOTE_MSG_RELAY_ANC_CMD, false))
                {
                    app_start_timer(&timer_idx_anc_tool_wait_sec_respond, "anc_tool_wait_sec_respond",
                                    anc_timer_id, APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
            }
        }
        break;

    default:
        {
            only_report_status = true;
            anc_status = ANC_CMD_STATUS_UNKNOW_CMD;
        }
        break;
    }

    if (only_report_status == true)
    {
        app_report_event(path, anc_cmd, app_idx, &anc_status, 1);
    }
}

bool app_anc_is_anc_on_state(T_ANC_APT_STATE state)
{
    bool ret = false;

    if ((state == ANC_ON_SCENARIO_1_APT_OFF) ||
        (state == ANC_ON_SCENARIO_2_APT_OFF) ||
        (state == ANC_ON_SCENARIO_3_APT_OFF) ||
        (state == ANC_ON_SCENARIO_4_APT_OFF) ||
        (state == ANC_ON_SCENARIO_5_APT_OFF))
    {
        ret = true;
    }

    return ret;
}

T_ANC_SM_STATE app_anc_state_get(void)
{
    return anc_state;
}

static void app_anc_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;
    bool anc_tool = false;
    bool anc_status_stable = true;

    T_ANC_FEATURE_MAP feature_map;
    feature_map.d32 = anc_tool_get_feature_map();

    if (event_type == AUDIO_EVENT_ANC_ENABLED)
    {
        /* In user mdoe, APP execute app_anc_enter_test_mode(ANC), driver reports AUDIO_EVENT_ANC_ENABLED */
        if (feature_map.user_mode == DISABLE)
        {
            anc_state = ANC_STARTED;
            handle = false;
            anc_tool = true;
            goto SKIP;
        }
    }

    if (event_type == AUDIO_EVENT_ANC_DISABLED)
    {
        /* In ANC test mdoe, APP execute app_anc_exit_test_mode(ANC), driver reports AUDIO_EVENT_ANC_DISABLED */
        if (feature_map.user_mode == DISABLE)
        {
            anc_state = ANC_STOPPED;
            handle = false;
            anc_tool = true;

            goto SKIP;
        }

        if (feature_map.mic_path != ANC_TEST_MODE_EXIT)
        {
            anc_state = ANC_STOPPED;
            handle = false;
            anc_tool = true;

            feature_map.mic_path = ANC_TEST_MODE_EXIT;
            anc_tool_set_feature_map(feature_map.d32);

            goto SKIP;
        }
    }

    switch (event_type)
    {
    case AUDIO_EVENT_ANC_ENABLED:
        {
            anc_state = ANC_STARTED;

            if (app_anc_is_anc_on_state(app_db.current_listening_state))
            {
                app_db.last_anc_on_state = app_db.current_listening_state;
            }
#if F_APP_SUPPORT_ANC_APT_COEXIST
            else if (app_listening_is_anc_apt_on_state(app_db.current_listening_state))
            {
                app_db.last_anc_apt_on_state = app_db.current_listening_state;
            }
#endif

            if (app_listening_get_blocked_state() != ANC_APT_STATE_TOTAL)
            {
                app_listening_state_machine(EVENT_APPLY_BLOCKED_STATE, false, false);
                anc_status_stable = false;
            }
            else if (app_listening_final_state_valid())
            {
                if (app_db.current_listening_state != *app_db.final_listening_state)
                {
                    app_listening_state_machine(EVENT_APPLY_FINAL_STATE, false, false);
                    anc_status_stable = false;
                }
            }
            APP_PRINT_TRACE1("AUDIO_EVENT_ANC_ENABLED stable = %x", anc_status_stable);

#if F_APP_SUPPORT_ANC_APT_COEXIST
            if ((app_listening_is_anc_apt_on_state(app_listening_get_temp_state())) &&
                (app_apt_is_busy()))
            {
                anc_status_stable = false;
            }
#endif

            if (anc_status_stable)
            {
                app_listening_report_status_change(app_db.current_listening_state);
                app_eq_change_audio_eq_mode(true);
            }

            if (app_cfg_const.rws_sniff_negotiation)
            {
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_ANC_APT);
            }
        }
        break;

    case AUDIO_EVENT_ANC_DISABLED:
        {
            anc_state = ANC_STOPPED;

            if (app_listening_get_temp_state() != app_db.current_listening_state)
            {
                app_listening_state_machine(EVENT_APPLY_PENDING_STATE, false, false);
                anc_status_stable = false;
            }
            else if (app_listening_get_blocked_state() != ANC_APT_STATE_TOTAL)
            {
                app_listening_state_machine(EVENT_APPLY_BLOCKED_STATE, false, false);
                anc_status_stable = false;
            }
            else
            {
                if (app_listening_final_state_valid())
                {
                    if (app_db.current_listening_state != *app_db.final_listening_state)
                    {
                        app_listening_state_machine(EVENT_APPLY_FINAL_STATE, false, false);
                        anc_status_stable = false;
                    }
                }
            }

            APP_PRINT_TRACE1("AUDIO_EVENT_ANC_DISABLED stable = %x", anc_status_stable);

            if (anc_status_stable)
            {
                app_listening_report_status_change(app_db.current_listening_state);

                //need reset eq when set eq off
                if (!app_cfg_const.enable_rtk_charging_box || (app_db.local_loc != BUD_LOC_IN_CASE))
                {
                    if ((app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE) &&
                        (app_db.device_state == APP_DEVICE_STATE_ON))
                    {
                        app_eq_change_audio_eq_mode(true);
                    }
                    else
                    {
                        app_db.spk_eq_mode = app_eq_judge_audio_eq_mode();
                        app_eq_idx_check_accord_mode();
                    }
                }
            }

            if (app_cfg_const.rws_sniff_negotiation)
            {
                app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_ANC_APT);
            }

            if (app_listening_cmd_postpone_reason_get() == ANC_APT_CMD_POSTPONE_WAIT_USER_MODE_CLOSE)
            {
                /* User mode is at all off state, APP ready to execute ANC tool command */
                app_listening_cmd_postpone_pop();
            }
        }
        break;

    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STOPPED)
                {
                    if (app_listening_cmd_postpone_reason_get() == ANC_APT_CMD_POSTPONE_ANC_WAIT_MEDIA_BUFFER ||
                        app_listening_cmd_postpone_reason_get() == ANC_APT_CMD_POSTPONE_LLAPT_WAIT_MEDIA_BUFFER)
                    {
                        app_listening_cmd_postpone_pop();
                    }
                }
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

SKIP:
    if (handle == true)
    {
        APP_PRINT_TRACE1("app_anc_audio_cback: event 0x%04x", event_type);
    }

    if (anc_tool)
    {
        APP_PRINT_TRACE3("app_anc_audio_cback: ANC_TOOL event 0x%04x, user_mode = %x, mic_path = %x",
                         event_type, feature_map.user_mode, feature_map.mic_path);
    }
}

void app_anc_handle_vendor_cmd(uint8_t *cmd_ptr)
{
    uint8_t cmd_path = cmd_ptr[0];
    uint8_t app_idx = cmd_ptr[1];
    uint16_t hci_cmd = (uint16_t)(cmd_ptr[2] | cmd_ptr[3] << 8);
    uint8_t buf[3] = {0};
    uint8_t rom_version, chip_id, anc_register_status;

    APP_PRINT_TRACE3("app_anc_handle_vendor_cmd: HCI_CMD = 0x%04X, app_idx = %d, cmd_path = %d",
                     hci_cmd, app_idx, cmd_path);

    switch (hci_cmd)
    {
    case HCI_VENDOR_READ_RTK_ROM_VERSION:
        //return rom_version to tool
        rom_version = sys_hall_read_rom_version();
        buf[0] = cmd_ptr[2];
        buf[1] = cmd_ptr[3];
        buf[2] = rom_version - 1;
        break;
    case HCI_VENDOR_READ_RTK_RTK_CHIP_ID:
        //return chip_id to tool
        chip_id = sys_hall_read_chip_id();
        buf[0] = cmd_ptr[2];
        buf[1] = cmd_ptr[3];
        buf[2] = chip_id;
        break;
    case HCI_VENDOR_WRITE_ANC_REGISTER:
        anc_register_status = anc_tool_set_para((&cmd_ptr[4]));
        buf[0] = cmd_ptr[2];
        buf[1] = cmd_ptr[3];
        buf[2] = anc_register_status;
        break;
    default:
        break;
    }

    APP_PRINT_TRACE3("app_anc_handle_vendor_cmd: HCI_CMD = 0x%02X %02X, result = %d",
                     buf[1], buf[0], buf[2]);

    app_report_event(cmd_path, EVENT_ANC_VENDOR_REPORT, app_idx, buf, 3);
}

void app_anc_enter_measure_mode(uint8_t resp_meas_mode)
{
    T_WDG_MODE wdg_mode;

    anc_tool_set_resp_meas_mode((T_ANC_RESP_MEAS_MODE)resp_meas_mode);

    wdg_mode = resp_meas_mode ? RESET_ALL_EXCEPT_AON : RESET_ALL;
    chip_reset(wdg_mode);
}

void app_anc_wait_wdg_reset(uint8_t mode)
{
    resp_meas_mode = mode;
    APP_PRINT_TRACE3("app_anc_wait_wdg_reset: %d, %d, link_num = %d", resp_meas_mode, mode,
                     app_link_get_b2s_link_num());

    //save cfg before wdg reset
    app_cfg_store_all();

    if (app_link_get_b2s_link_num())
    {
        uint8_t idx = 0;

        for (idx = 0; idx < MAX_BR_LINK_NUM; idx++)
        {
            if (app_db.br_link[idx].connected_profile != 0)
            {
                if (app_db.br_link[idx].connected_profile & RDTP_PROFILE_MASK)
                {
                    app_relay_async_single(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_ANC_TOOL_MEASURE_MODE);
                }
                else
                {
                    app_bt_policy_disconnect(app_db.br_link[idx].bd_addr, ALL_PROFILE_MASK);
                }
            }
        }
        app_start_timer(&timer_idx_anc_measure_kill_acl, "anc_measure_kill_acl",
                        anc_timer_id, APP_TIMER_ANC_MEASURE_KILL_ACL, 0, false,
                        3000);
    }
}

uint8_t app_anc_scenario_mode_get(uint8_t *mode)
{
    uint8_t i = 0;
    uint8_t scenario_mode[12] = {0};
    uint8_t num = anc_tool_get_anc_scenario_info(scenario_mode);
    int8_t order = -1;

    for (i = 0; i < num; i++)
    {
        if (anc_group_data.anc_activated_list & BIT(i))
        {
            order++;
            if (order < ANC_SCENARIO_MAX_NUM)
            {
                mode[order] = scenario_mode[i];
            }
            else
            {
                return ANC_SCENARIO_MAX_NUM;
            }
        }
    }
    return order + 1;
}

void app_anc_wdg_reset(void)
{
    if (resp_meas_mode)
    {
        DBG_DIRECT("app_anc_wdg_reset mode %d anc_drv mode %d", resp_meas_mode,
                   anc_tool_check_resp_meas_mode());
        /* all link disconnected, wdg reset */
        app_anc_enter_measure_mode(resp_meas_mode);
    }
}

bool app_anc_open_condition_check(void)
{
    /* check SCO condition to open ANC */
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    if ((app_teams_multilink_get_voice_status() != APP_CALL_IDLE) &&
        (!app_cfg_const.enable_anc_when_sco))
#else
    if ((app_bt_policy_get_call_status() != APP_CALL_IDLE) &&
        (!app_cfg_const.enable_anc_when_sco))
#endif
    {
        return false;
    }
    else if (LIGHT_SENSOR_ENABLED &&
             (!app_cfg_const.listening_mode_does_not_depend_on_in_ear_status))
    {
        if (((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
             ((app_db.remote_loc != BUD_LOC_IN_EAR)
#if F_APP_AIRPLANE_SUPPORT
              && (!app_airplane_mode_get())
#endif
             )) ||
            (app_db.local_loc != BUD_LOC_IN_EAR))
        {
            return false;
        }
    }
    if (app_db.gaming_mode)
    {
        if (app_cfg_const.disallow_listening_mode_on_when_gaming_mode)
        {
            return false;
        }
    }

    if (app_anc_get_selected_scenario_cnt() == 0)
    {
        return false;
    }

    return true;
}

uint8_t app_anc_get_measure_mode(void)
{
    return resp_meas_mode;
}



static void app_anc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_anc_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_ANC_MEASURE_KILL_ACL:
        {
            app_stop_timer(&timer_idx_anc_measure_kill_acl);

            uint8_t idx = 0;

            for (idx = 0; idx < MAX_BR_LINK_NUM; idx++)
            {
                if (app_db.br_link[idx].connected_profile != 0)
                {
                    if ((app_db.br_link[idx].connected_profile & RDTP_PROFILE_MASK) == 0)
                    {
                        gap_br_send_acl_disconn_req(app_db.br_link[idx].bd_addr);
                    }
                }
            }
        }
        break;

    case APP_TIMER_ANC_TOOL_WAIT_SEC_RESPOND:
        {
            app_stop_timer(&timer_idx_anc_tool_wait_sec_respond);

            uint8_t report_status;

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                report_status = ANC_CMD_STATUS_SPK2_TIMEOUT;
            }
            else
            {
                report_status = ANC_CMD_STATUS_SPK2_NOT_EXIST;
            }

            app_report_event(anc_active_cmd_path, anc_mp_tool_data.report_event, anc_active_app_idx,
                             &report_status, 1);
        }
        break;

    default:
        break;
    }
}

bool app_anc_related_event(T_ANC_APT_EVENT event)
{
    bool ret = false;

    if ((event == EVENT_ANC_ON_SCENARIO_1) || (event == EVENT_ANC_ON_SCENARIO_2) ||
        (event == EVENT_ANC_ON_SCENARIO_3) || (event == EVENT_ANC_ON_SCENARIO_4) ||
        (event == EVENT_ANC_ON_SCENARIO_5))
    {
        ret = true;
    }

    return ret;
}

static void app_anc_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            if (anc_tool_check_resp_meas_mode() != ANC_RESP_MEAS_MODE_NONE)
            {
                if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED &&
                    (app_link_check_b2s_link(param->acl_conn_success.bd_addr)))
                {
                    anc_tool_linkback_cnt = 0;
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_FAIL:
        {
            if (anc_tool_check_resp_meas_mode() != ANC_RESP_MEAS_MODE_NONE)
            {
                if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED &&
                    (app_link_check_b2s_link(param->acl_conn_fail.bd_addr)) &&
                    (app_link_get_b2s_link_num() == 0))
                {
                    anc_tool_linkback_cnt++;

                    if (anc_tool_linkback_cnt == ANC_TOOL_LINKBACK_TIMES)
                    {
                        app_anc_enter_measure_mode((uint8_t)ANC_RESP_MEAS_MODE_NONE);
                    }
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_anc_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_anc_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
            if (anc_tool_check_resp_meas_mode() != ANC_RESP_MEAS_MODE_NONE)
            {
                anc_tool_set_resp_meas_mode(ANC_RESP_MEAS_MODE_NONE);
            }

            T_ANC_FEATURE_MAP feature_map;
            feature_map.d32 = anc_tool_get_feature_map();

            if (feature_map.user_mode == DISABLE)
            {
                app_anc_exit_test_mode(feature_map.mic_path);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_relay_async_single(APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_EXIT_TEST_MODE);
                }
            }
        }
        break;

    default:
        break;
    }
}

bool app_anc_is_busy(void)
{
    if (anc_state == ANC_STOPPING ||
        anc_state == ANC_STARTING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_anc_enable(uint8_t scenario_id)
{
    if (anc_state != ANC_STARTED)
    {
        anc_state = ANC_STARTING;
        return anc_enable(scenario_id);
    }
    return false;
}

bool app_anc_disable(void)
{
    if (anc_state != ANC_STOPPED)
    {
        anc_state = ANC_STOPPING;
        return anc_disable();
    }
    return false;
}

#if F_APP_ERWS_SUPPORT
uint16_t app_anc_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    // uint16_t payload_len = 0;
    // uint8_t *msg_ptr = NULL;
    // bool skip = true;

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_ANC, 0, NULL, true, total);
}

static void app_anc_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_anc_parse_cback: msg = 0x%x, status = %x", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_EXIT_TEST_MODE:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_ANC_FEATURE_MAP feature_map;
                feature_map.d32 = anc_tool_get_feature_map();

                if (feature_map.user_mode == DISABLE)
                {
                    app_anc_exit_test_mode(feature_map.mic_path);
                }
            }
        }
        break;
    case APP_REMOTE_MSG_ANC_TOOL_MEASURE_MODE:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                break;
            }

            app_db.power_off_cause = POWER_OFF_CAUSE_ANC_TOOL;
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
        break;

    case APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                break;
            }
            uint8_t *p_info = (uint8_t *)buf;
            uint16_t cmd_id;
            uint16_t anc_cmd_len;

            cmd_id = (p_info[0] | p_info[1] << 8);
            anc_cmd_len = (p_info[2] | p_info[3] << 8);
            app_anc_cmd_handle(cmd_id, &p_info[6], anc_cmd_len, p_info[4], p_info[5]);
        }
        break;
    case APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL_FINISH:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                break;
            }
            uint8_t report_status = ANC_CMD_STATUS_SUCCESS;

            app_report_event(anc_active_cmd_path, anc_mp_tool_data.report_event, anc_active_app_idx,
                             &report_status, 1);
        }
        break;
    case APP_REMOTE_MSG_RELAY_ANC_CMD:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                /* bypass_cmd
                 * byte [0,1]  : cmd_id   *
                 * byte [2,3]  : cmd_len  *
                 * byte [4]    : cmd_path *
                 * byte [5-N]  : cmd      */

                uint8_t *p_info = (uint8_t *)buf;
                uint16_t cmd_id;
                uint16_t anc_cmd_len;

                cmd_id = (p_info[0] | p_info[1] << 8);
                anc_cmd_len = (p_info[2] | p_info[3] << 8);

                app_anc_cmd_handle(cmd_id, &p_info[5], anc_cmd_len, p_info[4], 0);
            }
        }
        break;

    case APP_REMOTE_MSG_RELAY_ANC_EVENT:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                /* report
                 * byte [0,1] : event_id    *
                 * byte [2,3] : report_len  *
                 * byte [4-N] : report      */

                uint8_t *p_info = (uint8_t *)buf;
                uint16_t event_id;
                uint16_t event_len;

                event_id = (p_info[0] | p_info[1] << 8);
                event_len = (p_info[2] | p_info[3] << 8);

                app_stop_timer(&timer_idx_anc_tool_wait_sec_respond);

                if (event_id == EVENT_ANC_READ_GAIN || event_id == EVENT_ANC_LLAPT_READ_GAIN)
                {
                    memcpy_s(&anc_mp_tool_data.report_value[event_len], (REPORT_VALUE_MAX_LENGTH - event_len),
                             &p_info[4], event_len);

                    app_report_event(anc_active_cmd_path, event_id, anc_active_app_idx, anc_mp_tool_data.report_value,
                                     event_len * 2);
                }
                else
                {
                    app_report_event(anc_active_cmd_path, event_id, anc_active_app_idx, &p_info[4], event_len);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_EXIT_ANC_CHOOSE_MODE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_anc_scenario_select_is_busy())
                {
                    app_anc_exit_scenario_select_mode();
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

static void app_anc_valid_state_check(T_ANC_APT_STATE *state)
{
    T_ANC_APT_STATE temp_state = *state;

    if (app_anc_is_anc_on_state(temp_state))
    {
        if (!(app_cfg_nv.anc_selected_list & BIT(temp_state - ANC_ON_SCENARIO_1_APT_OFF + 1)))
        {
            app_anc_set_first_anc_sceanrio(state);
        }
    }
#if F_APP_SUPPORT_ANC_APT_COEXIST
    else if (app_listening_is_anc_apt_on_state(temp_state))
    {
        if (!(app_cfg_nv.anc_selected_list & BIT(temp_state - ANC_ON_SCENARIO_1_APT_ON + 1)))
        {
            T_ANC_APT_STATE first_anc = ANC_OFF_APT_OFF;
            app_anc_set_first_anc_sceanrio(&first_anc);
            *state =  ANC_TO_ANC_APT_STATE(first_anc);
        }
    }
#endif
}

void app_anc_init(void)
{
    app_anc_activated_scenario_init();

    app_anc_valid_state_check(&app_cfg_nv.anc_one_setting);
    app_anc_valid_state_check(&app_cfg_nv.anc_both_setting);
    app_anc_valid_state_check(&app_cfg_nv.anc_apt_state);

    app_anc_set_first_anc_sceanrio(&app_db.last_anc_on_state);
#if F_APP_SUPPORT_ANC_APT_COEXIST
    app_db.last_anc_apt_on_state = ANC_TO_ANC_APT_STATE(app_db.last_anc_on_state);
#endif
    audio_mgr_cback_register(app_anc_audio_cback);
    app_timer_reg_cb(app_anc_timeout_cb, &anc_timer_id);
    bt_mgr_cback_register(app_anc_bt_cback);
    sys_mgr_cback_register(app_anc_dm_cback);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_anc_relay_cback, app_anc_parse_cback,
                             APP_MODULE_TYPE_ANC, APP_REMOTE_MSG_ANC_TOTAL);
#endif
}

#endif
