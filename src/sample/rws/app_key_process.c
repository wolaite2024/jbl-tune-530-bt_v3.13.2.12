/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "trace.h"
#include "board.h"
#include "ftl.h"
#include "ringtone.h"
#include "gap_vendor.h"
#include "remote.h"
#include "single_tone.h"
#include "app_key_process.h"
#include "app_adp.h"
#include "app_charger.h"
#include "app_led.h"
#include "app_mmi.h"
#include "app_main.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_cfg.h"
#include "app_dlps.h"
#include "app_ipc.h"
#include "app_timer.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "app_audio_policy.h"
#include "app_in_out_box.h"
#include "app_multilink.h"
#include "app_auto_power_off.h"
#include "app_cmd.h"
#include "app_bt_policy_api.h"
#include "app_sensor.h"
#include "hal_gpio.h"

//ysc start
#include "app_harman_vendor_cmd.h"
//ysc end
#if F_APP_CLI_BINARY_MP_SUPPORT
#include "mp_test_vendor.h"
#include "mp_test.h"
#endif

#if F_APP_ERWS_SUPPORT
#include "app_relay.h"
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#endif

#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_asp_device.h"
#include "asp_device_link.h"
#include "app_teams_cmd.h"
#include "app_teams_extend_led.h"
#include "app_teams_audio_policy.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif

#if F_APP_USB_HID_SUPPORT
//#include "app_usb_audio.h"
#include "app_usb_audio_hid.h"
#include "app_usb_mmi.h"
#include "errno.h"
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_KEY_EXTEND_FEATURE
#include "app_ota.h"
#endif

#if F_APP_CCP_SUPPORT
#include "app_lea_ccp.h"
#endif

#if F_APP_CAP_TOUCH_SUPPORT
#include "app_cap_touch.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
#include "app_adc.h"
#endif

#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
#include "app_harman_adc.h"
#endif

#define LONG_PRESS                          1
#define LONG_PRESS_POWER_ON                 2
#define LONG_PRESS_POWER_OFF                3
#define LONG_PRESS_ENTER_PAIRING            4
#define LONG_PRESS_FACTORY_RESET            5
#define LONG_PRESS_REPEAT                   6
#define BETWEEN_LONG_AND_ULTRA_LONG_PRESS   7
#define ULTRA_LONG_PRESS                    8
#define LONG_PRESS_FACTORY_RESET_PHONE      9
#define CLICK_AND_PRESS                     10
#define ULTRA_LONG_PRESS_REPEAT             11

#define KEY_TIMER_UNIT_MS                   100
#define KEY_IN_OUT_EAR_IMITATE              0
#define KEY_REMAP_DEBUG                     0

#define MAPPING_DIRECTION_PHONE_TO_MCU      0
#define MAPPING_DIRECTION_MCU_TO_PHONE      1

#define BUD_PHYSICAL_CHANNEL_MAXIMUM        3
#define KEY_REMAPPING_ARRAY_SIZE            ((BUD_PHYSICAL_CHANNEL_MAXIMUM-1) * (KEY_CLICK_MAX-1) * KEY_CALL_STATUS_MAX)

#define SEPEARTE_MFB_GSENSOR (app_cfg_const.gsensor_support && !app_cfg_const.enable_mfb_pin_as_gsensor_interrupt_pin)

//ysc start
#if HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
#define SPECAIL_ULTRA_LONG_KEY_ID           (KEY0_MASK | KEY3_MASK)
#define SPECAIL_ULTRA_LONG_KEY_TIME         100
#define SPECAIL_LONG_KEY_TIME               12
#endif
//ysc end

typedef enum t_key_click
{
    KEY_CLICK_SINGLE            = 0x01,
    KEY_CLICK_MULTI_2           = 0x02,
    KEY_CLICK_MULTI_3           = 0x03,
    KEY_CLICK_LONG_PRESS        = 0x04,
    KEY_CLICK_ULTRA_LONG_PRESS  = 0x05,
    KEY_CLICK_MULTI_4           = 0x06,
    KEY_CLICK_MULTI_5           = 0x07,
    KEY_CLICK_MULTI_6           = 0x08,
    KEY_CLICK_MULTI_7           = 0x09,

    KEY_CLICK_MAX
} T_KEY_CLICK;

typedef enum t_key_call_status
{
    KEY_CALL_STATUS_CALL_IDLE       = 0x00,
    KEY_CALL_STATUS_NOT_CALL_IDLE   = 0x01,

    KEY_CALL_STATUS_MAX
} T_KEY_CALL_STATUS;

typedef enum t_key_bud_side
{
    KEY_BUD_SIDE_STEREO                 = 0x00,
    KEY_BUD_SIDE_LEFT_BUD               = 0x01,
    KEY_BUD_SIDE_RIGHT_BUD              = 0x02,
    KEY_BUD_SIDE_LEFT_AND_RIGHT_BUDS    = 0x03,

    KEY_BUD_SIDE_MAX
} T_KEY_BUD_SIDE;

typedef enum t_app_key_remote_msg
{
    APP_KEY_MAP                     = 0x00,
    APP_KEY_SEC_MMI                 = 0x01,
    APP_KEY_RESET                   = 0x02,
    APP_KEY_SEC_DEFAULT_MMI         = 0x03,
    APP_KEY_RWS_KEY_MAP             = 0x04,
    APP_KEY_RWS_KEY_SEC_MMI         = 0x05,
    APP_KEY_RWS_KEY_RESET           = 0x06,
    APP_KEY_RWS_KEY_SEC_DEFAULT_MMI = 0x07,
    APP_KEY_RWS_KEY_TONE_RELAY      = 0x08,

    APP_KEY_MSG_MAX
} T_APP_KEY_REMOTE_MSG;

/* App define IO timer type, such as key, uart etc. */
typedef enum t_app_key_timer
{
    APP_TIMER_KEY_MULTI_CLICK,
    APP_TIMER_KEY_LONG_PRESS,
    APP_TIMER_KEY_LONG_PRESS_REPEAT,
    APP_TIMER_KEY_POWER_ON_LONG_PRESS,
    APP_TIMER_KEY_POWER_OFF_LONG_PRESS,
    APP_TIMER_KEY_ENTER_PAIRING,
    APP_TIMER_KEY_FACTORY_RESET,
    APP_TIMER_KEY_ULTRA_LONG_PRESS,
    APP_TIMER_KEY_MUTE_LONG_PRESS,
    APP_TIMER_KEY_HALL_SWITCH_STABLE,
    APP_TIMER_KEY_FACTORY_RESET_PHONE,
    APP_TIMER_KEY_CLICK_AND_PRESS,
    APP_TIMER_KEY_ULTRA_LONG_PRESS_REPEAT,
    APP_TIMER_KEY_CAP_TOUCH_CHECK,
} T_APP_KEY_TIMER;

/* Key data for record key various status */
typedef struct t_key_data
{
    uint8_t         mfb_key;                /* MFB key (power on)*/
	//ysc start
	uint8_t         mfb_key1;
	//ysc end
    uint8_t         pre_key;                /* previous key value */
    uint8_t         combine_key_bits;       /* record current hybrid press key value*/
    uint8_t         pre_combine_key_bits;   /* record previous hybrid press key value*/
    uint8_t         key_bit_mask;           /* key mask */
    uint8_t         key_long_pressed;       /* key long press category*/
    uint8_t         key_detected_num;       /* detecte key pressed num*/
    uint8_t         key_click;              /* click key num */
    uint8_t         key_action;             /* action of key value corresponding*/
    uint8_t         key_enter_pairing;      /* long press enter pairing*/
    uint8_t         key_bit_mask_airplane;  /* key mask of airplane mode combine key*/
} T_KEY_DATA;

/* Key check parameters */
typedef struct t_key_check
{
    uint8_t         key;                /* key Mask */
    uint8_t         key_pressed;        /* key action KEY_RELEASE or  KEY_PRESS*/
} T_KEY_CHECK;

typedef struct t_key_remap_info
{
    uint8_t rsv : 1;
    uint8_t is_modified : 1;
    uint8_t bud_channel : 2;
    uint8_t click_type : 4;
    uint8_t call_status : 4;
    uint8_t key_cmd : 8;
} T_KEY_REMAP_INFO;

typedef struct t_app_key_map
{
    uint8_t bud_channel;
    uint8_t call_state;
    uint8_t click_type;
    uint8_t key_mmi;
} T_APP_KEY_MAP;

typedef struct t_customer_key_info
{
    uint8_t app_test_rsv1;
    uint8_t app_test_rsv2;
    uint8_t app_test_rsv3;
    uint8_t app_test_rsv4;
    T_KEY_REMAP_INFO app_key_remap_info[KEY_REMAPPING_ARRAY_SIZE];
} T_CUSTOMER_KEY_INFO;

typedef struct t_customer_rws_key_info
{
    uint8_t app_test_rsv1;
    uint8_t app_test_rsv2;
    uint8_t app_test_rsv3;
    uint8_t app_test_rsv4 : 6;
    uint8_t is_setting_rws_key: 1;
    uint8_t already_sync: 1;
    T_KEY_REMAP_INFO app_key_remap_info[KEY_REMAPPING_ARRAY_SIZE];
} T_CUSTOMER_RWS_KEY_INFO;

#if F_APP_KEY_EXTEND_FEATURE
const uint8_t supported_key_mmi_list[] =
{
    MMI_NULL,
    MMI_HF_INITIATE_VOICE_DIAL,             //0x09
    MMI_DEV_SPK_VOL_UP,                     //0x30
    MMI_DEV_SPK_VOL_DOWN,                   //0x31
    MMI_AV_PLAY_PAUSE,                      //0x32
    MMI_AV_FWD,                             //0x34
    MMI_AV_BWD,                             //0x35
#if F_APP_APT_SUPPORT
    MMI_AUDIO_APT,                          //0x65
#endif
    MMI_AUDIO_EQ_SWITCH,                    //0x6B
#if F_APP_ANC_SUPPORT
    MMI_AUDIO_ANC_ON,                       //0x81
    MMI_LISTENING_MODE_CYCLE,               //0xD1
    MMI_ANC_CYCLE,                          //0xD2
#endif
    0xFF,
};

const T_KEY_CLICK supported_click_type[] =
{
    KEY_CLICK_SINGLE,
    KEY_CLICK_MULTI_2,
    KEY_CLICK_MULTI_3,
    KEY_CLICK_LONG_PRESS,
    KEY_CLICK_ULTRA_LONG_PRESS,
};

const T_KEY_CALL_STATUS supported_call_state[] =
{
    KEY_CALL_STATUS_CALL_IDLE,
};

#define SUPPORTED_KEY_MMI_NUM (sizeof(supported_key_mmi_list)/sizeof(uint8_t))
#define SUPPORTED_CLICK_TYPE_NUM (sizeof(supported_click_type)/sizeof(uint8_t))
#define SUPPORTED_CALL_STATE_NUM (sizeof(supported_call_state)/sizeof(uint8_t))

T_CUSTOMER_KEY_INFO *app_key_stored_key_info;

#if F_APP_RWS_KEY_SUPPORT
T_CUSTOMER_RWS_KEY_INFO *app_key_stored_rws_key_info;
#endif
#endif

static bool is_key_volume_set;
static bool disallow_sync_play_vol_changed_tone = false;

static uint8_t key_timer_id = 0;
static uint8_t timer_idx_key_multi_click = 0;
static uint8_t timer_idx_key_long_press = 0;
static uint8_t timer_idx_key_long_press_repeat = 0;
static uint8_t timer_idx_key_power_on_long_press = 0;
static uint8_t timer_idx_key_power_off_long_press = 0;
static uint8_t timer_idx_key_enter_pairing = 0;
static uint8_t timer_idx_key_factory_reset = 0;
static uint8_t timer_idx_key_ultra_long_press = 0;
static uint8_t timer_idx_key_hall_switch_stable = 0;
static uint8_t timer_idx_key_factory_reset_phone = 0;
static uint8_t timer_idx_key_click_and_press = 0;
#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
static uint8_t timer_idx_key_ultra_long_press_repeat = 0;
#endif

#if F_APP_CAP_TOUCH_SUPPORT
static uint8_t timer_idx_key_cap_touch_check = 0;
#endif

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
static uint8_t timer_idx_key_mute_long_press = 0;
static bool mute_key_long_press;
#endif

static T_KEY_DATA key_data = {0}; /**<record key variable */
static uint8_t power_onoff_combinekey_found = 0;
static bool long_key_power_off_pressed = false;

//ysc start 
#if HARMAN_CUSTOMIZED_BUTTON_CONTROL
    static uint8_t app_harman_gesture_func(uint8_t func_id)
    {
        uint8_t action = 0;
        switch (func_id)
        {
            case FUNC_PLAY_PAUSE:
                action = MMI_AV_PLAY_PAUSE;
                //app_key_execute_action(MMI_AV_PLAY_PAUSE);
            break;
            case FUNC_VOICE_AWARE:
                if(app_cfg_nv.sidetone_switch)
                {
                    app_cfg_nv.harman_sidetone = app_cfg_nv.harman_sidetone & 0x7F;
                    APP_PRINT_TRACE2("FEATURE_SIDETONE: switch: %d, level: %d",
                            app_cfg_nv.sidetone_switch,app_cfg_nv.sidetone_level);
						app_harman_set_sidetone(app_cfg_nv.harman_sidetone);
                }
                else
                {
                    app_cfg_nv.harman_sidetone = app_cfg_nv.harman_sidetone | 0x80;
                    APP_PRINT_TRACE2("FEATURE_SIDETONE: switch: %d, level: %d",
                            app_cfg_nv.sidetone_switch,app_cfg_nv.sidetone_level);
                    app_harman_set_sidetone(app_cfg_nv.harman_sidetone);
                }
                
            break;
            case FUNC_EQ_STATUS:
                app_eq_status();
                    //app_cfg_nv.app_mfb_eq_state_local !=app_cfg_nv.app_mfb_eq_state_local
            break;
            case FUNC_VOICE_ASSISTANT:
                action = MMI_HF_INITIATE_VOICE_DIAL;
            break;
            default:
                break;
        }
        APP_PRINT_TRACE2("app_harman_gesture_func func_id %x  action %x",func_id,action);
        return action;
    }
#endif
//ysc end
#if F_APP_KEY_EXTEND_FEATURE
uint8_t app_key_click_type_mapping(uint8_t key_type, uint8_t direction)
{
    uint8_t value;

    if (direction == MAPPING_DIRECTION_PHONE_TO_MCU)
    {
        uint8_t key_table[] =
        {
            HYBRID_KEY_TOTAL,
            HYBRID_KEY_SHORT_PRESS,
            HYBRID_KEY_2_CLICK,
            HYBRID_KEY_3_CLICK,
            HYBRID_KEY_LONG_PRESS,
            HYBRID_KEY_ULTRA_LONG_PRESS,
            HYBRID_KEY_4_CLICK,
            HYBRID_KEY_5_CLICK,
            HYBRID_KEY_6_CLICK,
            HYBRID_KEY_7_CLICK
        };

        if (key_type < sizeof(key_table) / sizeof(uint8_t))
        {
            value = key_table[key_type];
        }
        else
        {
            value = HYBRID_KEY_TOTAL;
        }
    }
    else if (MAPPING_DIRECTION_MCU_TO_PHONE)
    {
        uint8_t key_table[] =
        {
            KEY_CLICK_SINGLE,
            KEY_CLICK_LONG_PRESS,
            KEY_CLICK_ULTRA_LONG_PRESS,
            KEY_CLICK_MULTI_2,
            KEY_CLICK_MULTI_3,
            KEY_CLICK_MULTI_4,
            KEY_CLICK_MULTI_5,
            KEY_CLICK_MAX,
            KEY_CLICK_MAX,
            KEY_CLICK_MAX,
            KEY_CLICK_MULTI_6,
            KEY_CLICK_MULTI_7
        };

        if (key_type < sizeof(key_table) / sizeof(uint8_t))
        {
            value = key_table[key_type];
        }
        else
        {
            value = KEY_CLICK_MAX;
        }
    }

#if KEY_REMAP_DEBUG
    APP_PRINT_TRACE3("app_key_click_type_mapping: key_type %d, direction %d, value %d", key_type,
                     direction, value);
#endif
    return value;
}

uint8_t app_key_call_state_mapping(uint8_t call_state, uint8_t direction)
{
    if (direction == MAPPING_DIRECTION_PHONE_TO_MCU)
    {
        if (call_state == KEY_CALL_STATUS_CALL_IDLE)
        {
            return APP_CALL_IDLE;
        }
        else
        {
            return APP_CALL_ACTIVE;
        }
    }
    else if (direction == MAPPING_DIRECTION_MCU_TO_PHONE)
    {
        if (call_state == APP_CALL_IDLE)
        {
            return KEY_CALL_STATUS_CALL_IDLE;
        }
        else
        {
            return KEY_CALL_STATUS_NOT_CALL_IDLE;
        }
    }

    return 0;
}

uint32_t app_key_save_user_param(void)
{
    return ftl_save_to_storage(app_key_stored_key_info, APP_RW_KEY_REMAP_INFO_ADDR,
                               APP_RW_KEY_REMAP_INFO_SIZE);
}

uint32_t app_key_load_user_param(void)
{
    return ftl_load_from_storage(app_key_stored_key_info, APP_RW_KEY_REMAP_INFO_ADDR,
                                 APP_RW_KEY_REMAP_INFO_SIZE);
}

bool app_key_is_key_remap_support_ultra_long_press(void)
{
    bool ret = false;

    if (app_cfg_const.key_ultra_long_press_interval != 0)
    {
        uint8_t idx = 0;
        uint8_t hybrid_key_num = sizeof(app_cfg_const.hybrid_key_table[0]) /
                                 sizeof(app_cfg_const.hybrid_key_table[0][0]);

        for (idx = 0; idx < hybrid_key_num; idx++)
        {
            /* support ultra long press */
            if ((app_cfg_const.hybrid_key_mapping[idx][0] == KEY0_MASK) &&
                (app_cfg_const.hybrid_key_mapping[idx][1] == HYBRID_KEY_ULTRA_LONG_PRESS))
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}

static uint8_t app_key_get_key_remap_default_mmi(uint8_t call_state, uint8_t key_type,
                                                 uint8_t click_type)
{
    uint8_t default_cmd = MMI_NULL;

    if (click_type == KEY_CLICK_SINGLE || click_type == KEY_CLICK_LONG_PRESS)
    {
        default_cmd =
            app_cfg_const.key_table[key_type][call_state][0];
    }
    else
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if ((app_cfg_const.hybrid_key_mapping[i][0] == KEY0_MASK) &&
                (app_cfg_const.hybrid_key_mapping[i][1] == key_type))
            {
                default_cmd =
                    app_cfg_const.hybrid_key_table[call_state][i];
            }
        }
    }

    return default_cmd;
}

static uint8_t app_key_get_key_remap_idx(uint8_t call_state, uint8_t click_type, uint8_t channel)
{
    /* see as three dimensional array [call_state][click_type][channel] to get index */
    uint8_t idx = (call_state * (KEY_CLICK_MAX - 1) * (BUD_PHYSICAL_CHANNEL_MAXIMUM - 1))
                  + ((click_type - 1) * (BUD_PHYSICAL_CHANNEL_MAXIMUM - 1))
                  + (channel - 1);

    return idx;
}

static bool app_key_check_is_need_reset_cfg(void)
{
    uint8_t call_state_num = sizeof(supported_call_state) / sizeof(uint8_t);
    uint8_t click_type_num = sizeof(supported_click_type) / sizeof(uint8_t);
    uint8_t local_channel = app_device_get_bud_channel();
    uint8_t call_state, click_type, idx;
    uint8_t key_type_mapping;
    uint8_t call_state_mapping;
    uint8_t default_cmd = MMI_NULL;

    for (call_state = 0; call_state < call_state_num; call_state++)
    {
        for (click_type = 0; click_type < click_type_num; click_type++)
        {
            idx = app_key_get_key_remap_idx(supported_call_state[call_state],
                                            supported_click_type[click_type], local_channel);

            T_KEY_REMAP_INFO *current_key_remap_info = &app_key_stored_key_info->app_key_remap_info[idx];

            key_type_mapping = app_key_click_type_mapping((uint8_t) supported_click_type[click_type],
                                                          MAPPING_DIRECTION_PHONE_TO_MCU);
            call_state_mapping = app_key_call_state_mapping((uint8_t)supported_call_state[call_state],
                                                            MAPPING_DIRECTION_PHONE_TO_MCU);

            if (key_type_mapping != HYBRID_KEY_TOTAL)
            {
                default_cmd = app_key_get_key_remap_default_mmi(call_state_mapping, key_type_mapping,
                                                                supported_click_type[click_type]);

                if (!current_key_remap_info->is_modified && current_key_remap_info->key_cmd != default_cmd)
                {
                    return true;
                }
            }
        }
    }

#if F_APP_RWS_KEY_SUPPORT
    for (uint8_t i = 0; i < RWS_KEY_NUM; i++)
    {
        if (app_cfg_const.rws_key_mapping[i].mmi != MMI_NULL)
        {
            uint8_t app_click_type = app_key_click_type_mapping(app_cfg_const.rws_key_mapping[i].click_type,
                                                                MAPPING_DIRECTION_MCU_TO_PHONE);
            uint8_t app_call_status = app_key_call_state_mapping(app_cfg_const.rws_key_mapping[i].call_status,
                                                                 MAPPING_DIRECTION_MCU_TO_PHONE);
            uint8_t local_channel = app_device_get_bud_channel();

            if (app_click_type != KEY_CLICK_MAX)
            {
                uint8_t idx = app_key_get_key_remap_idx(app_call_status, app_click_type, local_channel);

                T_KEY_REMAP_INFO *current_key_remap_info = &app_key_stored_rws_key_info->app_key_remap_info[idx];

                if (!current_key_remap_info->is_modified &&
                    current_key_remap_info->key_cmd != app_cfg_const.rws_key_mapping[i].mmi)
                {
                    return true;
                }
            }
        }
    }
#endif

    return false;
}

static void app_key_set_default_key_setting(T_KEY_REMAP_INFO *key_remap_info,
                                            bool overwrite_current_setting, bool *setting_changed)
{
    uint8_t call_state_num = sizeof(supported_call_state) / sizeof(uint8_t);
    uint8_t click_type_num = sizeof(supported_click_type) / sizeof(uint8_t);
    uint8_t local_channel = app_device_get_bud_channel();
    uint8_t call_state, click_type, idx;
    uint8_t key_type_mapping;
    uint8_t call_state_mapping;
    uint8_t default_cmd = MMI_NULL;

    if (setting_changed)
    {
        *setting_changed = false;
    }

    /* not support ultra long press */
    if (!app_key_is_key_remap_support_ultra_long_press())
    {
        click_type_num--;
    }

    for (call_state = 0; call_state < call_state_num; call_state++)
    {
        for (click_type = 0; click_type < click_type_num; click_type++)
        {
            idx = app_key_get_key_remap_idx(supported_call_state[call_state],
                                            supported_click_type[click_type], local_channel);

            T_KEY_REMAP_INFO *current_key_remap_info = &key_remap_info[idx];

            /* set the default mmi if overwrite_current_setting is true or the key remap is not been setting */
            if (overwrite_current_setting ||
                (current_key_remap_info->bud_channel == 0))
            {
                if (setting_changed)
                {
                    *setting_changed = true;
                }

                key_type_mapping = app_key_click_type_mapping((uint8_t) supported_click_type[click_type],
                                                              MAPPING_DIRECTION_PHONE_TO_MCU);
                call_state_mapping = app_key_call_state_mapping((uint8_t)supported_call_state[call_state],
                                                                MAPPING_DIRECTION_PHONE_TO_MCU);

                if (key_type_mapping != HYBRID_KEY_TOTAL)
                {
                    default_cmd = app_key_get_key_remap_default_mmi(call_state_mapping, key_type_mapping,
                                                                    supported_click_type[click_type]);

                    current_key_remap_info->bud_channel = local_channel;
                    current_key_remap_info->click_type = supported_click_type[click_type];
                    current_key_remap_info->call_status = supported_call_state[call_state];
                    current_key_remap_info->key_cmd = default_cmd;
                    current_key_remap_info->is_modified = 0;
                }
            }
        }
    }
}

uint32_t app_key_reset_user_param(void)
{
    memset((void *)app_key_stored_key_info, 0x00, APP_RW_KEY_REMAP_INFO_SIZE);
    app_key_set_default_key_setting(app_key_stored_key_info->app_key_remap_info, true, NULL);

    return ftl_save_to_storage(app_key_stored_key_info, APP_RW_KEY_REMAP_INFO_ADDR,
                               APP_RW_KEY_REMAP_INFO_SIZE);
}

#if F_APP_RWS_KEY_SUPPORT
bool app_key_is_rws_key_setting(void)
{
    uint8_t i;
    bool ret = false;

    if (app_key_stored_rws_key_info->is_setting_rws_key)
    {
        ret = true;
    }
    else
    {
        for (i = 0; i < RWS_KEY_NUM; i++)
        {
            if (app_cfg_const.rws_key_mapping[i].mmi != MMI_NULL)
            {
                ret = true;
            }
        }
    }

    return ret;
}

static void app_key_set_default_rws_key_setting(T_KEY_REMAP_INFO *key_remap_info,
                                                bool overwrite_current_setting, bool *setting_changed)
{
    uint8_t i;

    if (setting_changed)
    {
        *setting_changed = false;
    }

    for (i = 0; i < RWS_KEY_NUM; i++)
    {
        if (app_cfg_const.rws_key_mapping[i].mmi != MMI_NULL)
        {
            uint8_t app_click_type = app_key_click_type_mapping(app_cfg_const.rws_key_mapping[i].click_type,
                                                                MAPPING_DIRECTION_MCU_TO_PHONE);
            uint8_t app_call_status = app_key_call_state_mapping(app_cfg_const.rws_key_mapping[i].call_status,
                                                                 MAPPING_DIRECTION_MCU_TO_PHONE);
            uint8_t local_channel = app_device_get_bud_channel();
            uint8_t idx = app_key_get_key_remap_idx(app_call_status, app_click_type, local_channel);

            if (app_click_type != KEY_CLICK_MAX)
            {
                if ((key_remap_info[idx].bud_channel == 0) || overwrite_current_setting)
                {
                    if (setting_changed)
                    {
                        *setting_changed = true;
                    }

                    key_remap_info[idx].bud_channel = local_channel;
                    key_remap_info[idx].click_type = app_click_type;
                    key_remap_info[idx].call_status = app_call_status;
                    key_remap_info[idx].key_cmd = app_cfg_const.rws_key_mapping[i].mmi;
                }
            }
        }
    }
}

uint32_t app_key_load_rws_key_user_param(void)
{
    return ftl_load_from_storage(app_key_stored_rws_key_info, APP_RW_RWS_KEY_REMAP_INFO_ADDR,
                                 APP_RW_RWS_KEY_REMAP_INFO_SIZE);
}

uint32_t app_key_save_rws_key_user_param(void)
{
    return ftl_save_to_storage(app_key_stored_rws_key_info, APP_RW_RWS_KEY_REMAP_INFO_ADDR,
                               APP_RW_RWS_KEY_REMAP_INFO_SIZE);
}

uint32_t app_key_reset_rws_key_user_param(bool reset_all_setting)
{
    if (reset_all_setting)
    {
        memset((void *)app_key_stored_rws_key_info, 0x00, APP_RW_RWS_KEY_REMAP_INFO_SIZE);
    }

    app_key_set_default_key_setting(app_key_stored_rws_key_info->app_key_remap_info, true, NULL);
    app_key_set_default_rws_key_setting(app_key_stored_rws_key_info->app_key_remap_info, true, NULL);
    app_key_stored_rws_key_info->is_setting_rws_key = app_key_is_rws_key_setting();

    return app_key_save_rws_key_user_param();
}
#endif

static void app_key_exit_ota_cback(void)
{
    app_cfg_nv.reboot_after_ota = 1;
}

static void app_key_remap_init(void)
{
    app_ota_cback_register(app_key_exit_ota_cback, OTA_EXIT);

    if (app_key_stored_key_info == NULL)
    {
        app_key_stored_key_info = malloc(sizeof(T_CUSTOMER_KEY_INFO));
    }

    app_key_load_user_param();

#if F_APP_RWS_KEY_SUPPORT
    if (app_key_stored_rws_key_info == NULL)
    {
        app_key_stored_rws_key_info = malloc(sizeof(T_CUSTOMER_RWS_KEY_INFO));
    }

    app_key_load_rws_key_user_param();
#endif

    if (app_key_check_is_need_reset_cfg())
    {
        app_key_reset_user_param();

#if F_APP_RWS_KEY_SUPPORT
        app_key_reset_rws_key_user_param(true);
#endif
    }
}

#if F_APP_ERWS_SUPPORT
void app_key_set_remote_key_setting(T_KEY_REMAP_INFO *key_remap_info, uint8_t *key_setting)
{
    uint8_t remote_channel = (app_device_get_bud_channel() == CHANNEL_L) ? CHANNEL_R : CHANNEL_L;
    uint8_t click_type, call_state, idx;
    bool need_save = false;

    for (call_state = 0; call_state < SUPPORTED_CALL_STATE_NUM; call_state++)
    {
        for (click_type = 0; click_type < SUPPORTED_CLICK_TYPE_NUM; click_type++)
        {
            uint8_t tmp_key_cmd = key_setting[call_state * SUPPORTED_CLICK_TYPE_NUM + click_type];

            idx = app_key_get_key_remap_idx(supported_call_state[call_state],
                                            supported_click_type[click_type], remote_channel);

            if (key_remap_info[idx].key_cmd != tmp_key_cmd || key_remap_info[idx].bud_channel == 0)
            {
                key_remap_info[idx].bud_channel = remote_channel;
                key_remap_info[idx].click_type = supported_click_type[click_type];
                key_remap_info[idx].call_status = supported_call_state[call_state];
                key_remap_info[idx].key_cmd = tmp_key_cmd;
                need_save = true;
            }
        }
    }

    if (need_save)
    {
#if F_APP_RWS_KEY_SUPPORT
        if (key_remap_info == app_key_stored_rws_key_info->app_key_remap_info)
        {
            app_key_save_rws_key_user_param();
        }
        else
#endif
        {
            app_key_save_user_param();
        }
    }
}

void app_key_get_key_setting_for_relay(T_KEY_REMAP_INFO *app_key_remap_info, uint8_t *mmi_evt,
                                       uint8_t call_state_num, uint8_t click_type_num)
{
    uint8_t local_channel = app_device_get_bud_channel();
    uint8_t offset = 0;

    for (uint8_t call_state = 0; call_state < call_state_num; call_state++)
    {
        for (uint8_t click_type = 0; click_type < click_type_num; click_type++)
        {
            uint8_t idx = app_key_get_key_remap_idx(supported_call_state[call_state],
                                                    supported_click_type[click_type], local_channel);

            offset = call_state * click_type_num + click_type;

            mmi_evt[offset] = app_key_remap_info[idx].key_cmd;
        }
    }
}
#endif

static uint8_t app_key_get_key_remapping_cmd(T_KEY_REMAP_INFO *key_remap_info, uint8_t input_bud_ch,
                                             uint8_t input_click_type, uint8_t input_call_stat)
{
    uint8_t key_index = 0;
    uint8_t ret = 0xFF;

    //Find the corresponding key_remapping array index
    for (key_index = 0; key_index < KEY_REMAPPING_ARRAY_SIZE; key_index++)
    {
        if ((key_remap_info[key_index].bud_channel == input_bud_ch)
            && (key_remap_info[key_index].click_type == input_click_type)
            && (key_remap_info[key_index].call_status == input_call_stat))
        {
            ret = key_remap_info[key_index].key_cmd;
            break;
        }
    }

#if KEY_REMAP_DEBUG
    APP_PRINT_TRACE5("[KEY_DEBUG] app_key_get_key_remapping_cmd input_bud_ch %d,input_click_type%d ,input_call_stat %d,index %d, ret 0x%x ",
                     input_bud_ch, input_click_type, input_call_stat, key_index,
                     ret);
#endif

    return ret;
}

void app_key_set_key_remapping_info(T_KEY_REMAP_INFO *key_remap_info, T_KEY_REMAP_INFO *key_info)
{
    uint8_t idx = 0;
    T_APP_KEY_REMOTE_MSG msg_type;

    if (key_info != NULL)
    {
        // if the bud channel input was single, store the data at left channel
        if (key_info->bud_channel == 0x0)
        {
            idx = 0;
            key_info->bud_channel = 0x01; //remapping the bud channel 0 to bud channel 1
        }
        else
        {
            idx = key_info->bud_channel - 1;
        }

        idx = idx + ((key_info->click_type - 1) * (BUD_PHYSICAL_CHANNEL_MAXIMUM - 1))
              + (key_info->call_status * ((KEY_CLICK_MAX - 1) * (BUD_PHYSICAL_CHANNEL_MAXIMUM - 1)));

        key_remap_info[idx].bud_channel = key_info->bud_channel;
        key_remap_info[idx].click_type = key_info->click_type;
        key_remap_info[idx].call_status = key_info->call_status;
        key_remap_info[idx].key_cmd = key_info->key_cmd;
        key_remap_info[idx].is_modified = true;

#if KEY_REMAP_DEBUG
        APP_PRINT_TRACE5("[KEY_DEBUG] key_info = 0x%X, 0x%X, 0x%X, 0x%X, idx = %d", key_info->bud_channel,
                         key_info->click_type, key_info->call_status, key_info->key_cmd, idx);
#endif

#if F_APP_RWS_KEY_SUPPORT
        if (key_remap_info == app_key_stored_rws_key_info->app_key_remap_info)
        {
            app_key_save_rws_key_user_param();
            msg_type = APP_KEY_RWS_KEY_MAP;
        }
        else
#endif
        {
            app_key_save_user_param();
            msg_type = APP_KEY_MAP;
        }

        if (app_cfg_const.bud_role != REMOTE_SESSION_ROLE_SINGLE)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, msg_type);
            }
        }
    }
}

void app_key_get_key_mmi_map(T_KEY_REMAP_INFO *key_remap_info, uint8_t *evt_param,
                             uint8_t state_num,
                             const uint8_t *supported_call_state,
                             uint8_t type_num, const uint8_t *supported_click_type)
{
    uint8_t offest = 1; //start from 1 , 0 is total key set number
    uint8_t bud_channel_num = 2;
    T_APP_KEY_MAP tmp_setting;

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        bud_channel_num = 1;
    }

    for (uint8_t i = 0; i < bud_channel_num; i++) //bud channel
    {
        for (uint8_t j = 0; j < state_num; j++) //call state
        {
            for (uint8_t k = 0; k < type_num; k++) //click type
            {
                memset(&tmp_setting, 0x00, sizeof(tmp_setting));
                //the single bud role key remap data was stored at left channel
                tmp_setting.bud_channel = i + 1; //ref. BUD_PHYSICAL_CHANNEL
                tmp_setting.call_state = supported_call_state[j];     //ref. KEY_CALL_STATUS_TYPE
                tmp_setting.click_type = supported_click_type[k];       //ref. KEY_CLICK_TYPE
                tmp_setting.key_mmi = app_key_get_key_remapping_cmd(key_remap_info, tmp_setting.bud_channel,
                                                                    tmp_setting.click_type, tmp_setting.call_state);

                if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                {
                    tmp_setting.bud_channel = 0; //restore bud_channel = 0 after get key mapping data;
                }

                memcpy(&evt_param[offest], &tmp_setting, sizeof(tmp_setting));
                offest += sizeof(tmp_setting);
            }
        }
    }
}

void app_key_report_key_mmi_map(T_EVENT_ID event_id)
{
    uint8_t *evt_param = NULL;
    uint8_t state_num = sizeof(supported_call_state) / (sizeof(T_KEY_CALL_STATUS));
    uint8_t type_num = sizeof(supported_click_type) / (sizeof(T_KEY_CLICK));
    uint8_t key_set_num = 0;

    /* not support ultra long press */
    if (!app_key_is_key_remap_support_ultra_long_press())
    {
        type_num--;
    }

    /* if bud role was single */
    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        key_set_num = state_num * type_num;
    }
    else
    {
        key_set_num = 2 * state_num * type_num;
    }

    evt_param = calloc(1, key_set_num * 4 + 1);
    if (evt_param != NULL)
    {
        T_KEY_REMAP_INFO *key_remap_info = app_key_stored_key_info->app_key_remap_info;

        memset(evt_param, 0x00, key_set_num * 4 + 1);
        evt_param[0] = key_set_num;

#if F_APP_RWS_KEY_SUPPORT
        key_remap_info = (event_id == EVENT_REPORT_KEY_MMI_MAP) ?
                         key_remap_info : app_key_stored_rws_key_info->app_key_remap_info;
#endif

        app_key_get_key_mmi_map(key_remap_info, evt_param, state_num, supported_call_state, type_num,
                                supported_click_type);

        app_report_event_broadcast(event_id, evt_param, key_set_num * 4 + 1);

        free(evt_param);
    }
}

uint8_t app_key_handle_key_remapping_cmd(uint16_t length, uint8_t *p_value, uint8_t app_idx,
                                         uint8_t cmd_path)
{
    uint8_t ret = CMD_SET_STATUS_COMPLETE;
    uint16_t cmd_id;
    uint8_t ack_pkt[3];

    cmd_id = (uint16_t)(p_value[0] | (p_value[1] << 8));
    ack_pkt[0] = p_value[0];
    ack_pkt[1] = p_value[1];
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    switch (cmd_id)
    {
    case CMD_GET_SUPPORTED_MMI_LIST:
        {
            uint8_t *evt_param = NULL;
            uint8_t mmi_num = sizeof(supported_key_mmi_list) / (sizeof(uint8_t));

            evt_param = calloc(1, mmi_num + 1);
            if (evt_param != NULL)
            {
                memset(evt_param, 0x00, mmi_num + 1);
                evt_param[0] = mmi_num;
                memcpy(&evt_param[1], supported_key_mmi_list, mmi_num);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(cmd_path, EVENT_REPORT_SUPPORTED_MMI_LIST, app_idx, evt_param, mmi_num + 1);
                free(evt_param);
            }
            else
            {
                ret = CMD_SET_STATUS_PROCESS_FAIL;
            }
        }
        break;

    case CMD_GET_SUPPORTED_CLICK_TYPE:
        {
            uint8_t *evt_param = NULL;
            uint8_t type_num = sizeof(supported_click_type) / (sizeof(uint8_t));

            /* not support ultra long press */
            if (app_cfg_const.key_ultra_long_press_interval == 0)
            {
                type_num--;
            }
            else
            {
                uint8_t idx = 0;
                uint8_t hybrid_key_num = sizeof(app_cfg_const.hybrid_key_table[0]) /
                                         sizeof(app_cfg_const.hybrid_key_table[0][0]);

                type_num--;
                for (idx = 0; idx < hybrid_key_num; idx++)
                {
                    /* support ultra long press */
                    if ((app_cfg_const.hybrid_key_mapping[idx][0] == KEY0_MASK) &&
                        (app_cfg_const.hybrid_key_mapping[idx][1] == HYBRID_KEY_ULTRA_LONG_PRESS))
                    {
                        type_num++;
                        break;
                    }
                }
            }

            evt_param = calloc(1, type_num + 1);
            if (evt_param != NULL)
            {
                memset(evt_param, 0x00, type_num + 1);
                evt_param[0] = type_num;
                memcpy(&evt_param[1], supported_click_type, type_num);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(cmd_path, EVENT_REPORT_SUPPORTED_CLICK_TYPE, app_idx, evt_param, type_num + 1);
                free(evt_param);
            }
            else
            {
                ret = CMD_SET_STATUS_PROCESS_FAIL;
            }
        }
        break;

    case CMD_GET_SUPPORTED_CALL_STATUS:
        {
            uint8_t *evt_param = NULL;
            uint8_t type_num = sizeof(supported_call_state) / (sizeof(uint8_t));

            evt_param = calloc(1, type_num + 1);
            if (evt_param != NULL)
            {
                memset(evt_param, 0x00, type_num + 1);
                evt_param[0] = type_num;
                memcpy(&evt_param[1], supported_call_state, type_num);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_report_event(cmd_path, EVENT_REPORT_SUPPORTED_CALL_STATE, app_idx, evt_param, type_num + 1);
                free(evt_param);
            }
            else
            {
                ret = CMD_SET_STATUS_PROCESS_FAIL;
            }
        }
        break;

    case CMD_GET_KEY_MMI_MAP:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_key_report_key_mmi_map(EVENT_REPORT_KEY_MMI_MAP);
        }
        break;

    case CMD_SET_KEY_MMI_MAP:
        {
            T_KEY_REMAP_INFO key_info;

            key_info.bud_channel = p_value[2];
            key_info.call_status = p_value[3];
            key_info.click_type = p_value[4];
            key_info.key_cmd = p_value[5];

            if ((key_info.call_status >= KEY_CALL_STATUS_MAX) ||
                (key_info.click_type >= KEY_CLICK_MAX) ||
                (key_info.bud_channel >= BUD_PHYSICAL_CHANNEL_MAXIMUM))
            {
                ret = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                app_key_set_key_remapping_info(app_key_stored_key_info->app_key_remap_info, &key_info);
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_RESET_KEY_MMI_MAP:
        {
            uint8_t side;
            uint8_t local_side = app_device_get_bud_channel();

            if (length == 2)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    side = local_side;
                }
                else
                {
                    side = KEY_BUD_SIDE_LEFT_AND_RIGHT_BUDS;
                }
            }
            else
            {
                // if the bud channel input was single, store the data at left channel
                side = (p_value[2] == KEY_BUD_SIDE_STEREO) ? KEY_BUD_SIDE_LEFT_BUD : p_value[2];

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    if (side != local_side)
                    {
                        ret = CMD_SET_STATUS_PARAMETER_ERROR;
                    }
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ret == CMD_SET_STATUS_COMPLETE)
            {
                if ((side == local_side) || (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    app_key_set_default_key_setting(app_key_stored_key_info->app_key_remap_info, true, NULL);
                    app_key_save_user_param();
                    app_key_report_key_mmi_map(EVENT_REPORT_KEY_MMI_MAP);

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_MAP);
                    }
                }
                else
                {
                    if (side == KEY_BUD_SIDE_LEFT_AND_RIGHT_BUDS)
                    {
                        app_key_reset_user_param();
                    }

                    /* pri would report key mmi after receive sec's key setting */
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RESET);
                }
            }
        }
        break;

#if F_APP_RWS_KEY_SUPPORT
    case CMD_GET_RWS_KEY_MMI_MAP:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_key_report_key_mmi_map(EVENT_REPORT_RWS_KEY_MMI_MAP);
        }
        break;

    case CMD_SET_RWS_KEY_MMI_MAP:
        {
            T_KEY_REMAP_INFO key_info;

            key_info.bud_channel = p_value[2];
            key_info.call_status = p_value[3];
            key_info.click_type = p_value[4];
            key_info.key_cmd = p_value[5];

            if ((key_info.call_status >= KEY_CALL_STATUS_MAX) ||
                (key_info.click_type >= KEY_CLICK_MAX) ||
                (key_info.bud_channel >= BUD_PHYSICAL_CHANNEL_MAXIMUM))
            {
                ret = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                app_key_set_key_remapping_info(app_key_stored_rws_key_info->app_key_remap_info, &key_info);
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    case CMD_RESET_RWS_KEY_MMI_MAP:
        {
            uint8_t side;
            uint8_t local_side = app_device_get_bud_channel();

            if (length == 2)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    side = local_side;
                }
                else
                {
                    side = KEY_BUD_SIDE_LEFT_AND_RIGHT_BUDS;
                }
            }
            else
            {
                // if the bud channel input was single, store the data at left channel
                side = (p_value[2] == KEY_BUD_SIDE_STEREO) ? KEY_BUD_SIDE_LEFT_BUD : p_value[2];

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    if (side != local_side)
                    {
                        ret = CMD_SET_STATUS_PARAMETER_ERROR;
                    }
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ret == CMD_SET_STATUS_COMPLETE)
            {
                if ((side == local_side) || (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    app_key_reset_rws_key_user_param(false);
                    app_key_report_key_mmi_map(EVENT_REPORT_RWS_KEY_MMI_MAP);
                }
                else
                {
                    if (side == KEY_BUD_SIDE_LEFT_AND_RIGHT_BUDS)
                    {
                        app_key_reset_rws_key_user_param(true);
                    }

                    /* pri would report key mmi after receive sec's key setting */
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RWS_KEY_RESET);
                }
            }
        }
        break;
#endif

    default:
        break;
    }
    return ret;
}

static bool app_key_customer_key_remapping(T_KEY_REMAP_INFO *key_remap_info,
                                           uint8_t input_key_click_type,
                                           uint8_t input_call_status, uint8_t *ret_cmd)
{
    bool ret = false;
    uint8_t current_bud_channel = app_device_get_bud_channel();

    //Find the corresponding key_remapping command
    *ret_cmd = app_key_get_key_remapping_cmd(key_remap_info, current_bud_channel, input_key_click_type,
                                             input_call_status);
    if (*ret_cmd != 0xFF)
    {
        ret = true;
    }
    return ret;
}

static bool app_key_check_key_remap(uint8_t type, uint8_t *key_action)
{
    bool ret = false;
    uint8_t key_type = app_key_click_type_mapping(type, MAPPING_DIRECTION_MCU_TO_PHONE);
#if F_APP_LEA_SUPPORT
    uint8_t call_status = app_key_call_state_mapping((uint8_t)app_bt_policy_get_call_status(),
                                                     MAPPING_DIRECTION_MCU_TO_PHONE);
#else
    uint8_t call_status = app_key_call_state_mapping(app_hfp_get_call_status(),
                                                     MAPPING_DIRECTION_MCU_TO_PHONE);
#endif
    uint8_t tmp_key_action = 0xFF;
    T_KEY_REMAP_INFO *key_remap_info = app_key_stored_key_info->app_key_remap_info;

#if F_APP_RWS_KEY_SUPPORT
    if (app_key_stored_rws_key_info->is_setting_rws_key &&
        app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        key_remap_info = app_key_stored_rws_key_info->app_key_remap_info;
    }
#endif

#if KEY_REMAP_DEBUG
    APP_PRINT_TRACE1("[KEY_DEBUG] app_key_check_key_remap: type %d", type);
#endif

    if (key_type != KEY_CLICK_MAX)
    {
        ret = app_key_customer_key_remapping(key_remap_info, key_type, call_status, &tmp_key_action);

        if (app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING)
        {
            uint8_t tmp_cmd = 0;

            app_key_customer_key_remapping(key_remap_info, key_type, KEY_CALL_STATUS_CALL_IDLE, &tmp_cmd);
            if (tmp_cmd == MMI_HF_INITIATE_VOICE_DIAL)
            {
                ret = true;
                tmp_key_action = MMI_HF_CANCEL_VOICE_DIAL;

#if KEY_REMAP_DEBUG
                APP_PRINT_TRACE0("[KEY_DEBUG] force remap mmi to cancel voice trigger");
#endif
            }
        }
    }

    if (ret)
    {
        *key_action = tmp_key_action;
    }

#if KEY_REMAP_DEBUG
    APP_PRINT_TRACE2("[KEY_DEBUG] key_remap_res = %d, get_cmd = 0x%X", ret, *key_action);
#endif

    return ret;
}
#endif

static uint8_t app_key_get_click_and_press_click_number(uint8_t key)
{
    uint8_t config_click_num = 0;

    switch (key)
    {
    case 0:
        {
            config_click_num = app_cfg_const.hybrid_key0_click_number;
        }
        break;

    case 1:
        {
            config_click_num = app_cfg_const.hybrid_key1_click_number;
        }
        break;

    case 2:
        {
            config_click_num = app_cfg_const.hybrid_key2_click_number;
        }
        break;

    case 3:
        {
            config_click_num = app_cfg_const.hybrid_key3_click_number;
        }
        break;

    default:
        break;
    }

    return config_click_num;
}

#if F_APP_RWS_KEY_SUPPORT
static void app_key_rws_key_check(T_HYBRID_KEY click_type, uint8_t click_num, uint8_t *key_action)
{
    uint8_t i;

    for (i = 0; i < RWS_KEY_NUM; i++)
    {
        if (app_cfg_const.rws_key_mapping[i].click_type == click_type)
        {
            if ((click_type == HYBRID_KEY_CLICK_AND_PRESS) &&
                (click_num != app_key_get_click_and_press_click_number(i)))
            {
                /* the click number does not match */
                continue;
            }

#if F_APP_LEA_SUPPORT
            if (app_cfg_const.rws_key_mapping[i].call_status == app_bt_policy_get_call_status())
#else
            if (app_cfg_const.rws_key_mapping[i].call_status == app_hfp_get_call_status())
#endif
            {
                if (app_cfg_const.rws_key_mapping[i].mmi != MMI_NULL)
                {
                    *key_action = app_cfg_const.rws_key_mapping[i].mmi;
                    break;
                }
            }
        }
    }
}
#endif

/*  Clear key parameters.
 * After perform the corresponding action,app will call this function to clear key parameters.
 */
static void app_key_clear(void)
{
    key_data.pre_key = KEY_NULL;
    key_data.key_click = 0;
    key_data.pre_combine_key_bits = 0;
}

static bool app_key_is_out_ear_disallow_mmi(uint8_t action)
{
    bool ret = false;

    if (/* hfp */
        (action >= MMI_HF_END_OUTGOING_CALL &&
         action <= MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL) ||
        /* a2dp */
        (action >= MMI_DEV_SPK_VOL_UP && action <= MMI_DEV_SPK_UNMUTE) ||
        /* listening mode related */
        (action == MMI_AUDIO_APT || action == MMI_ANC_ON_OFF ||
         action == MMI_ANC_CYCLE || action == MMI_LISTENING_MODE_CYCLE ||
         action == MMI_DEFAULT_LISTENING_MODE_CYCLE || action == MMI_LLAPT_CYCLE ||
         action == MMI_ANC_APT_ON_OFF) ||
        /* others */
        (action == MMI_DEV_GAMING_MODE_SWITCH || action == MMI_AUDIO_APT_EQ_SWITCH ||
         action == MMI_AUDIO_EQ_SWITCH)
#if F_APP_LEA_SUPPORT
        || (action == MMI_BIG_START || action == MMI_BIG_STOP)
#endif
    )
    {
        ret = true;
    }

    if (LIGHT_SENSOR_ENABLED &&
        app_cfg_const.listening_mode_does_not_depend_on_in_ear_status)
    {
        if (action == MMI_LISTENING_MODE_CYCLE)
        {
            ret = false;
        }
    }

    return ret;
}

/*
 * Find out the key index by key mask.
 * For example: The key index 0 correspond to key0, such as KEY0_MASK, KEY1_MASK etc.
 */
uint8_t app_key_search_index(uint8_t key)
{
    uint8_t index;

    switch (key)
    {
    case KEY0_MASK:
        index = 0;
        break;

    case KEY1_MASK:
        index = 1;
        break;

    case KEY2_MASK:
        index = 2;
        break;

    case KEY3_MASK:
        index = 3;
        break;

    case KEY4_MASK:
        index = 4;
        break;

    case KEY5_MASK:
        index = 5;
        break;

    case KEY6_MASK:
        index = 6;
        break;

    case KEY7_MASK:
        index = 7;
        break;

    default:
        index = 0;
        break;
    }

    return index;
}

/**
    * @brief  Depending on the current connection state, single or multi-spk mode,
    *         app hanldes the corresponding procedures.
    * @param  action The MMI action to be executed.
    * @return void
    */
void app_key_execute_action(uint8_t action)
{
    APP_PRINT_INFO4("app_key_execute_action: action = %x, local_loc = %d remote_loc = %d roleswap_status %d",
                    action, app_db.local_loc, app_db.remote_loc, app_roleswap_ctrl_get_status());

    if (app_roleswap_ctrl_get_status() != APP_ROLESWAP_STATUS_IDLE)
    {
        /* execute after roleswap finished */
        app_db.pending_mmi_action_in_roleswap = action;
        return;
    }

    if (app_cfg_const.fixed_inband_tone_gain &&
        (action == MMI_DEV_SPK_VOL_UP || action == MMI_DEV_SPK_VOL_DOWN))
    {
        if (app_hfp_get_call_status() == APP_CALL_INCOMING ||
            app_hfp_get_call_status() == APP_CALL_OUTGOING)
        {
            /* disallow adjust vol when incoming/outgoing call */
            app_key_clear();
            return;
        }
    }

    if ((app_db.local_loc == BUD_LOC_IN_AIR) && LIGHT_SENSOR_ENABLED &&
        (!app_cfg_const.allow_trigger_mmi_when_bud_not_in_ear))
    {
        if (app_key_is_out_ear_disallow_mmi(action))
        {
            app_key_clear();
            return;
        }
    }

    if ((app_db.local_loc == BUD_LOC_IN_CASE) && (app_cfg_const.enable_rtk_charging_box) &&
        (!app_cfg_const.allow_trigger_mmi_when_bud_not_in_ear))
    {
        if (app_key_is_out_ear_disallow_mmi(action))
        {
            app_key_clear();
            return;
        }
    }

    if (app_cfg_const.disallow_sync_play_vol_changed_tone_when_vol_adjust)
    {
        if ((action == MMI_DEV_SPK_VOL_UP) || (action == MMI_DEV_SPK_VOL_DOWN))
        {
            app_key_set_sync_play_vol_changed_tone_disallow(true);
        }
    }

#if F_APP_LISTENING_MODE_SUPPORT
    if (app_listening_mode_mmi(action))
    {
        if (app_cfg_const.disallow_listening_mode_before_bud_connected)
        {
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                /* Not handle listening mmi before b2b connected. */
                return;
            }
        }

        if (app_cfg_const.disallow_listening_mode_before_phone_connected)
        {
            if (app_link_get_b2s_link_num() == 0)
            {
                /* Not handle listening mmi before b2s connected. */
                return;
            }
        }
    }
#endif

#if F_APP_AIRPLANE_SUPPORT
    if (app_airplane_mode_get() && !app_airplane_mode_mmi_support(action))
    {
        return;
    }
#endif

#if F_APP_USB_SUSPEND_SUPPORT
    if (app_db.is_usb_suspend)
    {
        return;
    }
#endif

    if ((action == MMI_DEV_FACTORY_RESET) || (action == MMI_DEV_PHONE_RECORD_RESET))
    {
        if (app_mmi_is_allow_factory_reset() == false)
        {
            return;
        }
    }

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED) //single mode
    {
        if (mp_hci_test_mode_is_running())
        {
            mp_hci_test_mmi_handle_action(action);
        }
        else
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            APP_PRINT_INFO2("MULTILINK MMI: action = %x, active device = %x", action,
                            app_teams_multilink_get_active_index());
            if (app_teams_multilink_get_active_index() == 0xff)
            {
#if F_APP_USB_HID_SUPPORT
                if (app_usb_mmi_handle_action(action) == -ESRCH)
                {
                    app_mmi_handle_action(action);
                }
#endif
            }
            else
            {
                app_mmi_handle_action(action);
            }
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            uint8_t multilink_active_link = app_multilink_customer_get_active_index();

            APP_PRINT_INFO2("CUSTOMER_MULTILINK_SCENERIO_2_MMI: action = %x, active_link = %x", action,
                            multilink_active_link);

            if (multilink_active_link == multilink_usb_idx)
            {
#if F_APP_USB_HID_SUPPORT
                if (app_usb_mmi_handle_action(action) == -ESRCH)
                {
                    app_mmi_handle_action(action);
                }
#endif
            }
            else
            {
                app_mmi_handle_action(action);
            }
#else
            app_mmi_handle_action(action);
#endif
        }
    }
#if F_APP_ERWS_SUPPORT
    else
    {
        if (app_mmi_is_local_execute(action))
        {
            app_mmi_handle_action(action);
        }
        else if ((action == MMI_DEV_SPK_MUTE) || (action == MMI_DEV_SPK_UNMUTE) ||
                 (action == MMI_AUDIO_APT) || (action == MMI_AUDIO_APT_EQ_SWITCH) ||
                 (action == MMI_AUDIO_APT_VOL_UP) || (action == MMI_AUDIO_APT_VOL_DOWN) ||
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                 (action == MMI_24G_BT_AUDIO_SOURCE_SWITCH) ||
#endif
#if F_APP_LOCAL_PLAYBACK_SUPPORT
                 (action == MMI_SD_PLAYBACK_SWITCH) || (action == MMI_SD_PLAYPACK_PAUSE) ||
                 (action == MMI_SD_PLAYBACK_FWD) || (action == MMI_SD_PLAYBACK_FWD_PLAYLIST) ||
                 (action == MMI_SD_PLAYBACK_BWD) || (action == MMI_SD_PLAYBACK_BWD_PLAYLIST) ||
#endif
                 (action == MMI_AIRPLANE_MODE) ||
#if F_APP_LEA_SUPPORT
                 ((action == MMI_BIG_START) || (action == MMI_BIG_STOP)) ||
                 ((mtc_get_btmode() == MULTI_PRO_BT_CIS &&
                   ((action == MMI_AV_PLAY_PAUSE) || (action == MMI_DEV_MIC_UNMUTE) ||
                    (action == MMI_DEV_MIC_MUTE) || (action == MMI_DEV_MIC_MUTE_UNMUTE))) ||
                  (mtc_get_btmode() == MULTI_PRO_BT_BIS && ((action == MMI_DEV_SPK_VOL_UP) ||
                                                            (action == MMI_DEV_SPK_VOL_DOWN)))) ||
#endif
                 (action == MMI_AUDIO_APT_DISALLOW_SYNC_VOLUME_TOGGLE))
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                                      0, false);
            }
            else
            {
                app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                                      0, true);
            }
        }
        else if (action == MMI_DEV_FORCE_ENTER_PAIRING_MODE || action == MMI_DEV_ENTER_PAIRING_MODE ||
                 action == MMI_DEV_EXIT_PAIRING_MODE ||
                 action == MMI_START_ROLESWAP)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_mmi_handle_action(action);
            }
            else
            {
                app_relay_async_single(APP_MODULE_TYPE_MMI, action);
            }
        }
        else
        {
            app_relay_async_single(APP_MODULE_TYPE_MMI, action);
            app_mmi_handle_action(action);
        }
    }
#endif
}

#if 0
bool app_key_hybrid_hall_switch_match(uint8_t key, uint8_t hall_switch_action)
{
    bool ret = false;
    uint8_t i = 0;

    for (i = 0; i < HYBRID_KEY_NUM; i++)
    {
        // Check if pressed key(s) equal hybrid key mapping and hybrid key behavior equal to hall switch actions
        if ((app_cfg_const.hybrid_key_mapping[i][0] == key) &&
            (app_cfg_const.hybrid_key_mapping[i][1] == hall_switch_action))
        {
            if (app_cfg_const.key_hall_switch_stable_interval != 0)
            {
                uint8_t key_status = app_key_read_hall_switch_status();

                app_stop_timer(&timer_idx_key_hall_switch_stable);

                if (key_status != app_db.hall_switch_status)
                {
                    // Initiate stable timer. Execute action after the switch is stable.
                    if (app_cfg_const.key_hall_switch_stable_interval != 0)
                    {
                        app_start_timer(&timer_idx_key_hall_switch_stable, "key_hall_switch_stable",
                                        key_timer_id, APP_TIMER_KEY_HALL_SWITCH_STABLE, hall_switch_action, false,
                                        app_cfg_const.key_hall_switch_stable_interval * KEY_TIMER_UNIT_MS);
                    }
                }
            }
            else
            {
                // No stable time defined. Execute action directly
                app_key_execute_hall_switch_action(hall_switch_action);
            }
            ret = true;
            break;
        }
    }
    return ret;
}
#endif

/**
    * @brief  Check if current pressed key has multi-click behavior.
    * @param  key  Current pressed key
    * @return True if there's multi-click behavior definition for this key. False otherwise.
    */
static bool app_key_multi_click_match(uint8_t key)
{
    bool ret = false;
    uint8_t i;

    for (i = 0; i < MAX_KEY_NUM; i++)
    {
        if (app_cfg_const.hybrid_key_mapping[i][0] == key)
        {
            if ((app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_2_CLICK) ||
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_3_CLICK) ||
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_4_CLICK) ||
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_5_CLICK) ||
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_6_CLICK) ||
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_7_CLICK)
               )
            {
                ret = true;
                break;
            }
        }
    }

#if F_APP_KEY_EXTEND_FEATURE
    if (!ret && (key == KEY0_MASK))
    {
        /* handle the click type mcu config file not set,
        ex: ui did not set 2-click, and KEY_EXTEND set 2-click */
        uint8_t tmp = 0;

        if (app_key_check_key_remap(HYBRID_KEY_2_CLICK, &tmp) ||
            app_key_check_key_remap(HYBRID_KEY_3_CLICK, &tmp) ||
            app_key_check_key_remap(HYBRID_KEY_4_CLICK, &tmp) ||
            app_key_check_key_remap(HYBRID_KEY_5_CLICK, &tmp) ||
            app_key_check_key_remap(HYBRID_KEY_6_CLICK, &tmp) ||
            app_key_check_key_remap(HYBRID_KEY_7_CLICK, &tmp)
           )
        {
            ret = true;
        }
    }
#endif

    APP_PRINT_INFO1("app_key_multi_click_match: ret %d", ret);
    return ret;
}

static void app_key_handle_key_tone_play(T_APP_AUDIO_TONE_TYPE tone_type)
{
    bool relay_key_tone = true;

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED &&
        app_cfg_const.only_play_key_tone_on_trigger_bud_side)
    {
        relay_key_tone = false;
    }
#endif

    APP_PRINT_TRACE3("app_key_handle_key_tone_play: bud_role %d, tone_type 0x%04x, relay_key_tone %d",
                     app_cfg_nv.bud_role, tone_type, relay_key_tone);

    // no need to play tone if bud is in case
    if (app_db.local_loc != BUD_LOC_IN_CASE)
    {
#if F_APP_ERWS_SUPPORT
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) && relay_key_tone)
        {
            uint8_t buf[2] = {tone_type, relay_key_tone};
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RWS_KEY_TONE_RELAY,
                                                buf, sizeof(buf));
        }
        else
#endif
        {
            app_audio_tone_type_play(tone_type, false, relay_key_tone);
        }
    }
}

/*
 * App process single key action include key long press and short press.
 * When app process the specific key that bbpro assign those key,
 * app will call ringtone_play() to push specific ringtone.
 * After app read key action from key table and perform the corresponding action,
 * then will call key_clear().
 * key mask, such as KEY0_MASK, KEY1_MASK etc.
 */
void app_key_single_click(uint8_t key)
{
    uint8_t long_press = 0;

    if (app_db.device_state == APP_DEVICE_STATE_ON) //APP in non-off state
    {
        if (key_data.key_long_pressed != 0)
        {
            long_press = 1;
        }

        if (long_press == 0)
        {
            if (app_cfg_const.key_short_press_tone_mask & key)
            {
                app_key_handle_key_tone_play(TONE_KEY_SHORT_PRESS);
            }
        }
        //Long press MFB key tone is triggered when long press timeout
        else if (key != key_data.mfb_key)
        {
            if (app_cfg_const.key_long_press_tone_mask & key)
            {
                if (timer_idx_key_long_press_repeat)
                {
                    // no need to play long_press tone again
                }
                else
                {
                    app_key_handle_key_tone_play(TONE_KEY_LONG_PRESS);
                }
            }
        }

#if F_APP_DURIAN_SUPPORT
        if (app_durian_key_press(key, 0, long_press))
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            key_data.key_action = app_cfg_const.key_table[long_press][app_teams_multilink_get_voice_status()]
                                  [app_key_search_index(key)];
#else
            key_data.key_action = app_cfg_const.key_table[long_press][app_hfp_get_call_status()]
                                  [app_key_search_index(key)];
#endif
            app_key_execute_action(key_data.key_action);

        }
#else
#if F_APP_LEA_SUPPORT
        if (mtc_get_btmode())
        {
            key_data.key_action = app_cfg_const.key_table[long_press][app_lea_ccp_get_call_status()]
                                  [app_key_search_index(key)];

            //app_key_execute_action(key_data.key_action);
        }
        else
#endif
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            key_data.key_action = app_cfg_const.key_table[long_press][app_teams_multilink_get_voice_status()]
                                  [app_key_search_index(key)];
#else
            key_data.key_action = app_cfg_const.key_table[long_press][app_hfp_get_call_status()]
                                  [app_key_search_index(key)];
#endif

#if F_APP_RWS_KEY_SUPPORT
            if ((key == KEY0_MASK) && (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
            {
                T_HYBRID_KEY click_type = (long_press == 0) ? HYBRID_KEY_SHORT_PRESS : HYBRID_KEY_LONG_PRESS;

                app_key_rws_key_check(click_type, key_data.key_click, &key_data.key_action);
            }
#endif

#if F_APP_KEY_EXTEND_FEATURE
            T_HYBRID_KEY click_type = (long_press == 0) ? HYBRID_KEY_SHORT_PRESS : HYBRID_KEY_LONG_PRESS;

            if (key == KEY0_MASK)
            {
                uint8_t key_action = 0;

                if (app_key_check_key_remap(click_type, &key_action))
                {
                    key_data.key_action = key_action;
                }
            }
#endif
        }
//ysc start
#if HARMAN_CUSTOMIZED_BUTTON_CONTROL
#if F_APP_LEA_SUPPORT
    if((app_bt_policy_get_call_status() == BT_HFP_CALL_IDLE) && (key == KEY1_MASK))
#else
    if((app_hfp_get_call_status() == BT_HFP_CALL_IDLE) && (key == KEY1_MASK))
#endif
    {
        if (long_press == 0)
        {
            key_data.key_action = app_harman_gesture_func(app_harman_get_short_function());
        }
        else
        {
            key_data.key_action = app_harman_gesture_func(app_harman_get_long_function());
        }
    }
#endif
//ysc end
        app_key_execute_action(key_data.key_action);
#endif

//ysc start
/////////////////////////////
#if HARMAN_DETECT_BATTERY_STATUS_LED_SUPPORT
    if ((app_db.device_state == APP_DEVICE_STATE_ON) &&
        (app_link_get_connected_src_num() != 0) && (long_press == 0))
    {
        APP_PRINT_INFO0("[SD_CHECK] key_check_press blink");
        app_led_change_mode(LED_MODE_GAMING_MODE, true, false);
    }
#endif
//ysc end
}
    app_key_clear();
}

/**
    * @brief  App process hybrid key action include key long press and short press.
    *         When app process the specific key that bbpro assign those key,
    *         app will call au_push_ringtone() to push specific ringtone.
    *         After app read key action from key table and perform the corresponding action,
    *         then will call key_clear().
    * @param  key key mask, such as KEY0_MASK, KEY1_MASK etc.
    * @return void
    */
static void app_key_hybrid_click(uint8_t key)
{
    uint8_t i;
//ysc start	
    uint8_t harman_action = 0;
//ysc end	
    uint8_t hybrid_type = HYBRID_KEY_SHORT_PRESS;
    bool is_only_allow_factory_reset = false;

    APP_PRINT_INFO3("app_key_hybrid_click: device_state = %d, enable_factory_reset_when_in_the_box = %d  in box = %d",
                    app_db.device_state, app_cfg_const.enable_factory_reset_when_in_the_box == 1,
                    app_device_is_in_the_box());

    if ((app_db.device_state == APP_DEVICE_STATE_OFF) &&
        (app_cfg_const.enable_factory_reset_when_in_the_box == 1) &&
        app_device_is_in_the_box())
    {
        /* power off, only allow factory reset MMI */
        is_only_allow_factory_reset = true;
    }

    if ((app_db.device_state == APP_DEVICE_STATE_ON) || is_only_allow_factory_reset)
    {
        if ((key_data.key_long_pressed == ULTRA_LONG_PRESS)
#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
            || (key_data.key_long_pressed == ULTRA_LONG_PRESS_REPEAT)
#endif
           )
        {
            hybrid_type = HYBRID_KEY_ULTRA_LONG_PRESS;
        }
        else if (key_data.key_long_pressed == CLICK_AND_PRESS)
        {
            hybrid_type = HYBRID_KEY_CLICK_AND_PRESS;
        }
        else if (key_data.key_long_pressed)
        {
            hybrid_type = HYBRID_KEY_LONG_PRESS;
        }
        else if (key_data.key_click >= 2)
        {
            hybrid_type = key_data.key_click + 1;
            if (key_data.key_click >= 6)
            {
                hybrid_type = key_data.key_click + 4;
            }
        }

        if (app_cfg_const.enable_double_click_power_off_under_dut_test_mode
            && mp_hci_test_mode_is_running())
        {
            if (key == KEY0_MASK && hybrid_type == HYBRID_KEY_2_CLICK)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_HYBRID_KEY;
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
                app_key_clear();
                return;
            }
        }
//ysc start		
#if HARMAN_CUSTOMIZED_BUTTON_CONTROL
#if F_APP_LEA_SUPPORT
    if((app_bt_policy_get_call_status() == BT_HFP_CALL_IDLE) && (key == KEY1_MASK))
#else
    if((app_hfp_get_call_status() == BT_HFP_CALL_IDLE) && (key == KEY1_MASK))
#endif
    {
        if (hybrid_type == HYBRID_KEY_2_CLICK)
        {
            app_db.power_off_cause = POWER_OFF_CAUSE_HYBRID_KEY;
            harman_action = app_harman_gesture_func(app_harman_get_double_function());
            app_mmi_handle_action(harman_action);
            app_key_clear();
            return;
        }
        if (hybrid_type == HYBRID_KEY_3_CLICK)
        {
            app_db.power_off_cause = POWER_OFF_CAUSE_HYBRID_KEY;
            harman_action = app_harman_gesture_func(app_harman_get_triple_function());
            app_mmi_handle_action(harman_action);
            app_key_clear();
            return;
        }
    }
#endif
//ysc end
        for (i = 0; i < HYBRID_KEY_NUM; i++)
        {
            if (((app_cfg_const.hybrid_key_mapping[i][0] == key) &&
                 (app_cfg_const.hybrid_key_mapping[i][1] == hybrid_type)))
            {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                uint8_t action = app_cfg_const.hybrid_key_table[app_teams_multilink_get_voice_status()][i];
#else
#if F_APP_LEA_SUPPORT
                uint8_t action = app_cfg_const.hybrid_key_table[app_bt_policy_get_call_status()][i];
#else
                uint8_t action = app_cfg_const.hybrid_key_table[app_hfp_get_call_status()][i];
#endif
#endif
                if (hybrid_type == HYBRID_KEY_CLICK_AND_PRESS)
                {
                    /* the click number does not match */
                    if (key_data.key_click != app_key_get_click_and_press_click_number(i))
                    {
                        continue;
                    }
                }

                if (action == MMI_DEV_FACTORY_RESET || (app_db.device_state == APP_DEVICE_STATE_ON))
                {
#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
                    if (key_data.key_long_pressed != ULTRA_LONG_PRESS_REPEAT)
#endif
                    {
                        if (app_cfg_const.hybrid_key_press_tone_mask & (1 << i))
                        {
                            app_key_handle_key_tone_play(TONE_KEY_HYBRID);
                        }
                    }

#if F_APP_DURIAN_SUPPORT
                    if (app_durian_key_press(key, 1, hybrid_type))
                    {
                        key_data.key_action = action;
                        app_key_execute_action(key_data.key_action);
                    }
#else
                    {
                        key_data.key_action = action;

#if F_APP_RWS_KEY_SUPPORT
                        if ((key == KEY0_MASK) &&
                            (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                        {
                            app_key_rws_key_check((T_HYBRID_KEY)hybrid_type, key_data.key_click, &key_data.key_action);
                        }
#endif

#if F_APP_KEY_EXTEND_FEATURE
                        if (key == KEY0_MASK)
                        {
                            uint8_t key_action = 0;

                            if (app_key_check_key_remap(hybrid_type, &key_action))
                            {
                                key_data.key_action = key_action;
                            }
                        }
#endif

                        app_key_execute_action(key_data.key_action);
                    }
#endif
                }
                break;
            }
        }

#if F_APP_KEY_EXTEND_FEATURE
        if (i == HYBRID_KEY_NUM && key == KEY0_MASK)
        {
            /* handle the click type mcu config file not set,
                ex: ui did not set 2-click, and KEY_EXTEND set 2-click */
            uint8_t tmp = 0;

            if (app_key_check_key_remap(hybrid_type, &tmp))
            {
                key_data.key_action = tmp;

                app_key_execute_action(key_data.key_action);
            }
        }
#endif
    }

    app_key_clear();
}

void app_key_long_key_power_off_pressed_set(bool value)
{
    long_key_power_off_pressed = value;
}

bool app_key_long_key_power_off_pressed_get(void)
{
    return long_key_power_off_pressed;
}

/**
    * @brief  Register timer callback to gap layer. Called by app when gap timer is timeout.
    * @note
    * @param  timer_id distinguish io timer type
    * @param  timer_chann indicate which key
    * @return void
    */
static void app_key_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_key_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_KEY_MULTI_CLICK:
        {
            /* not support "click and press" or release all key */
            if (app_cfg_const.key_click_and_press_interval == 0 ||
                key_data.key_detected_num == 0)
            {
                if ((key_data.key_click == 1) && (key_data.pre_combine_key_bits == 0))
                {
                    app_key_single_click(param);
                }
                else
                {
                    app_key_hybrid_click(param);
                }
            }

            app_stop_timer(&timer_idx_key_multi_click);
        }
        break;

    case APP_TIMER_KEY_LONG_PRESS:
        {
            uint8_t index = 0;

            app_stop_timer(&timer_idx_key_long_press);
            key_data.key_long_pressed = LONG_PRESS;

            // Check if the current pressed key(s) exist ultra long press definition if long press timeout exec disable
            if (app_cfg_const.key_trigger_long_press != 1)
            {
                for (index = 0; index < 8; index++)
                {
                    if ((app_cfg_const.hybrid_key_mapping[index][0] == param) &&
                        (app_cfg_const.hybrid_key_mapping[index][1] == HYBRID_KEY_ULTRA_LONG_PRESS))
                    {
                        key_data.key_long_pressed = BETWEEN_LONG_AND_ULTRA_LONG_PRESS;
                        if ((param == key_data.mfb_key) && (app_cfg_const.key_power_off_interval != 0))
                        {
                            if (app_cfg_const.key_long_press_tone_mask & param)
                            {
                                app_key_handle_key_tone_play(TONE_KEY_LONG_PRESS);
                            }
                        }
                        break;
                    }
                }
            }

            // Execute the long press action if no ultra long press definition
            if ((key_data.key_long_pressed != BETWEEN_LONG_AND_ULTRA_LONG_PRESS) ||
                (app_cfg_const.key_trigger_long_press == 1))
            {
                if (app_cfg_const.key_trigger_long_press != 1)
                {
                    // Timer starts when key pressed. Stop ultra long key press timer if no ultra long key definition.
                    app_stop_timer(&timer_idx_key_ultra_long_press);
                }

                if ((param == key_data.mfb_key) && (app_cfg_const.key_power_off_interval != 0))
                {
                    //Long Press Power Off will also trigger Long Press
                    //Handle Long Press when key release
                    if (app_cfg_const.key_long_press_tone_mask & param)
                    {
                        app_key_handle_key_tone_play(TONE_KEY_LONG_PRESS);
                    }

                    if (app_cfg_const.key0_tigger_long_press_option == 0) //key0_tigger_press_option
                    {
                        break;
                    }
                }

                if (key_data.combine_key_bits == 0)
                {
                    app_key_single_click(param);
                }
                else
                {
                    app_key_hybrid_click(param);
                }

                if ((key_data.combine_key_bits == 0) &&
                    (app_cfg_const.key_long_press_repeat_interval != 0) &&
                    ((app_cfg_const.key_long_press_repeat_mask & param) == param))
                {
                    //Check long pressed key repeat
                    app_start_timer(&timer_idx_key_long_press_repeat, "key_long_press_repeat",
                                    key_timer_id, APP_TIMER_KEY_LONG_PRESS_REPEAT, param,  true,
                                    app_cfg_const.key_long_press_repeat_interval * KEY_TIMER_UNIT_MS);
                }
            }
        }
        break;

    case APP_TIMER_KEY_LONG_PRESS_REPEAT:
        {
            key_data.key_long_pressed = LONG_PRESS_REPEAT;

            if (key_data.combine_key_bits == 0)
            {
                app_key_single_click(param);
            }
            else
            {
                app_key_hybrid_click(param);
            }
        }
        break;

#if 0
    //a new situation has been added
    case APP_TIMER_KEY_HALL_SWITCH_STABLE:
        {
            uint8_t key_status = app_key_read_hall_switch_status();

            app_stop_timer(&timer_idx_key_hall_switch_stable);

            // Execute action if status has changed for more than stable time
            if (app_db.hall_switch_status != key_status)
            {
                app_db.hall_switch_status = key_status;
                app_key_execute_hall_switch_action(param);
            }
        }
        break;
#endif

    case APP_TIMER_KEY_POWER_ON_LONG_PRESS:
        {
            app_stop_timer(&timer_idx_key_power_on_long_press);

            if (app_db.device_state == APP_DEVICE_STATE_OFF)
            {
                if ((app_cfg_const.charging_disallow_power_on) &&
                    (app_adp_get_plug_state() == ADAPTOR_PLUG))
                {
                    //Disable blink power on led or play power on vp when bud inbox
                    break;
                }

                app_dlps_stop_power_down_wdg_timer();
                app_led_change_mode(LED_MODE_POWER_ON, true, false);
                app_db.is_long_press_power_on_play_vp = true;
                app_db.ignore_led_mode = true;
                key_data.key_long_pressed = LONG_PRESS_POWER_ON;

#if F_APP_AIRPLANE_SUPPORT
                if (key_data.key_bit_mask_airplane)
                {
                    app_audio_tone_type_play(TONE_ENTER_AIRPLANE, false, false);
                }
                else
#endif
                {
#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_cfg_const.enable_dongle_dual_mode && app_cfg_nv.is_dual_mode)
                    {
                        app_audio_tone_type_play(TONE_POWER_ON_BT_OR_BT_24G_MODE, false, false);
                    }
                    else
#endif
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        APP_PRINT_TRACE1("language_status_power_on_check %d", app_cfg_nv.language_status);
#if HARMAN_DISCHARGER_NTC_DETECT_PROTECT
                        if (!app_harman_discharger_ntc_check_valid())
                        {
                            app_db.is_long_press_power_on_play_vp = false;
                            app_device_unlock_vbat_disallow_power_on();
                            break;
                        }
#endif
                        if (app_cfg_nv.language_status == 0)
                        {
                            app_audio_tone_type_play(TONE_HARMAN_POWER_ON, false, false);
                        }
                        else
#endif
                        {
                            app_audio_tone_type_play(TONE_POWER_ON, false, false);
                        }
                    }
                }
            }
            else
            {
                app_stop_timer(&timer_idx_key_enter_pairing);
                app_stop_timer(&timer_idx_key_factory_reset);
                app_stop_timer(&timer_idx_key_factory_reset_phone);
            }

            app_device_unlock_vbat_disallow_power_on();
        }
        break;

    case APP_TIMER_KEY_POWER_OFF_LONG_PRESS:
        {
            app_stop_timer(&timer_idx_key_power_off_long_press);

            if (app_cfg_const.disable_power_off_by_mfb)
            {
                break;
            }

            key_data.key_long_pressed = LONG_PRESS_POWER_OFF;
            app_db.power_off_cause = POWER_OFF_CAUSE_LONG_KEY;

            app_key_long_key_power_off_pressed_set(true);

#if F_APP_ERWS_SUPPORT
            app_roleswap_poweroff_handle(false);
#else
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
#endif
        }
        break;

    case APP_TIMER_KEY_ENTER_PAIRING:
        {
            key_data.key_long_pressed = LONG_PRESS_ENTER_PAIRING;
            key_data.key_enter_pairing = 1;
            app_stop_timer(&timer_idx_key_enter_pairing);

#if F_APP_LEA_SUPPORT
            uint8_t is_legacy = 0;

            is_legacy = app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_LEGACY, NULL);
            if (is_legacy)
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
                if (app_cfg_nv.language_status == 0)
                {
                    app_audio_tone_type_play(TONE_HARMAN_PAIRING, false, false);
                }
                else
#endif
                {
                    app_audio_tone_type_play(TONE_PAIRING, false, false);
                }

#if GFPS_FEATURE_SUPPORT
                if (extend_app_cfg_const.gfps_support)
                {
                    //power_on->pairing mode
                    APP_PRINT_INFO0("app_gfps_force_enter_pairing_mode: APP_TIMER_KEY_ENTER_PAIRING");
                    app_gfps_force_enter_pairing_mode(GFPS_KEY_FORCE_ENTER_PAIR_MODE);
                }
#endif
            }
#else
#if F_APP_HARMAN_FEATURE_SUPPORT
            if (app_cfg_nv.language_status == 0)
            {
                app_audio_tone_type_play(TONE_HARMAN_PAIRING, false, false);
            }
            else
#endif
            {
                app_audio_tone_type_play(TONE_PAIRING, false, false);
            }
#endif
        }
        break;

    case APP_TIMER_KEY_FACTORY_RESET:
        {
            app_stop_timer(&timer_idx_key_factory_reset);
            app_key_reset_enter_pairing();

            app_led_change_mode(LED_MODE_FACTORY_RESET, true, false);
            app_db.ignore_led_mode = true;
            key_data.key_long_pressed = LONG_PRESS_FACTORY_RESET;

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (app_cfg_nv.language_status == 0)
            {
                app_audio_tone_type_play(TONE_HARMAN_FACTORY_RESET, false, false);
            }
            else
#endif
            {
                app_audio_tone_type_play(TONE_FACTORY_RESET, false, false);
            }
        }
        break;

    case APP_TIMER_KEY_ULTRA_LONG_PRESS:
        {
            // Ultra long press detected, execute the key action
            app_stop_timer(&timer_idx_key_ultra_long_press);
            key_data.key_long_pressed = ULTRA_LONG_PRESS;

            /*  ultra long press triggered by timeout */
            if (app_cfg_const.timeout_trigger_ultra_long_press == 1)
            {
                app_key_hybrid_click(param);
            }

            app_key_handle_key_tone_play(TONE_KEY_ULTRA_LONG_PRESS);

#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
            if (app_cfg_const.key_ultra_long_press_repeat_interval != 0)
            {
                //Check ultra long pressed key repeat
                app_start_timer(&timer_idx_key_ultra_long_press_repeat, "key_ultra_long_press_repeat",
                                key_timer_id, APP_TIMER_KEY_ULTRA_LONG_PRESS_REPEAT, param, true,
                                app_cfg_const.key_ultra_long_press_repeat_interval * KEY_TIMER_UNIT_MS);
            }
#endif
        }
        break;

#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
    case APP_TIMER_KEY_ULTRA_LONG_PRESS_REPEAT:
        {
            key_data.key_long_pressed = ULTRA_LONG_PRESS_REPEAT;

            app_key_hybrid_click(param);
        }
        break;
#endif

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
    case APP_TIMER_KEY_MUTE_LONG_PRESS:
        {
            app_stop_timer(&timer_idx_key_mute_long_press);
            mute_key_long_press = true;
        }
        break;
#endif

    case APP_TIMER_KEY_FACTORY_RESET_PHONE:
        {
            app_stop_timer(&timer_idx_key_factory_reset_phone);
            app_key_reset_enter_pairing();

            app_led_change_mode(LED_MODE_FACTORY_RESET, true, false);
            app_db.ignore_led_mode = true;
            key_data.key_long_pressed = LONG_PRESS_FACTORY_RESET_PHONE;

#if F_APP_HARMAN_FEATURE_SUPPORT
            if (app_cfg_nv.language_status == 0)
            {
                app_audio_tone_type_play(TONE_HARMAN_FACTORY_RESET, false, false);
            }
            else
#endif
            {
                app_audio_tone_type_play(TONE_FACTORY_RESET, false, false);
            }
        }
        break;

    case APP_TIMER_KEY_CLICK_AND_PRESS:
        {
            app_stop_timer(&timer_idx_key_click_and_press);
            key_data.key_long_pressed = CLICK_AND_PRESS;
            app_key_hybrid_click(param);
        }
        break;

#if F_APP_CAP_TOUCH_SUPPORT
    case APP_TIMER_KEY_CAP_TOUCH_CHECK:
        {
            app_stop_timer(&timer_idx_key_cap_touch_check);
            app_cap_touch_send_event_check();
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_CAP_TOUCH_SUPPORT
void app_key_start_cap_touch_check_timer(void)
{
    app_start_timer(&timer_idx_key_cap_touch_check, "key_cap_touch_check",
                    key_timer_id, APP_TIMER_KEY_CAP_TOUCH_CHECK, 0, false,
                    app_cfg_const.ctc_slide_time  * 100);
}
#endif

static void app_key_start_factory_reset_timer(void)
{
    if (app_cfg_const.key_factory_reset_rws_interval != 0)
    {
        app_start_timer(&timer_idx_key_factory_reset_phone, "key_factory_reset_phone",
                        key_timer_id, APP_TIMER_KEY_FACTORY_RESET_PHONE, 0, false,
                        app_cfg_const.key_factory_reset_rws_interval * KEY_TIMER_UNIT_MS);
    }

    if (app_cfg_const.key_factory_reset_interval != 0)
    {
        app_start_timer(&timer_idx_key_factory_reset, "key_factory_reset",
                        key_timer_id, APP_TIMER_KEY_FACTORY_RESET, 0, false,
                        app_cfg_const.key_factory_reset_interval * KEY_TIMER_UNIT_MS);
    }
}

void app_key_stop_timer(void)
{
    app_stop_timer(&timer_idx_key_power_on_long_press);
    app_stop_timer(&timer_idx_key_power_off_long_press);
    app_stop_timer(&timer_idx_key_enter_pairing);
    app_stop_timer(&timer_idx_key_factory_reset);
    app_stop_timer(&timer_idx_key_factory_reset_phone);
    app_stop_timer(&timer_idx_key_long_press);
    app_stop_timer(&timer_idx_key_long_press_repeat);
    app_stop_timer(&timer_idx_key_ultra_long_press);
    app_stop_timer(&timer_idx_key_click_and_press);
#if F_APP_KEY_ULTRA_LONG_PRESS_REPEAT_SUPPORT
    app_stop_timer(&timer_idx_key_ultra_long_press_repeat);
#endif
}

#if F_APP_CAP_TOUCH_SUPPORT
void app_key_cap_touch_stop_timer(void)
{
    app_stop_timer(&timer_idx_key_multi_click);
}
#endif

/**
    * @brief  App check status of key press or release in this function.
    *         The major content is divided into two parts to deal with, including  key press and key release.
    *         And app start gap timer to judge key action status, such as LONG_PRESS,
    *         LONG_PRESS_POWER_ON, LONG_PRESS_POWER_OFF, LONG_PRESS_ENTER_PAIRING etc.
    * @param  key_check this struct include key press status and key mask.
    * @return void
    */
static void app_key_check_press(T_KEY_CHECK key_check)
{
    uint8_t key = key_check.key;
    uint8_t i;

    APP_PRINT_TRACE4("app_key_check_press: key mask %02x, key_pressed %d, pre_clicks %d, long %d",
                     key, key_check.key_pressed, key_data.key_click, key_data.key_long_pressed);

    if ((app_db.device_state == APP_DEVICE_STATE_OFF)
        && (key != 0x01))
    {
        APP_PRINT_INFO1("Only allow key 0 in power off state: 0x%x", key);
        return;
    }

#if F_APP_DURIAN_SUPPORT
    app_durian_key_release_event(key, key_check.key_pressed);
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
    if (!app_adc_ntc_voltage_valid_check())
    {
        return;
    }
#endif

    if (key_check.key_pressed == KEY_PRESS)
    {
        if (app_cfg_const.only_play_min_max_vol_once)
        {
            app_db.play_min_max_vol_tone = true;

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_PLAY_MIN_MAX_VOL_TONE);
            }
#endif
        }

        if (app_db.device_state == APP_DEVICE_STATE_OFF_ING) //power off ing, not response key press
        {
            APP_PRINT_INFO0("app_key_check_press: device is power off ing!");
            return;
        }

//ysc start
//delete #if HARMAN_DETECT_BATTERY_STATUS_LED_SUPPORT
//ysc end
        if ((app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off &&
             (app_link_get_b2s_link_num() != 0)))
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_KEY);
        }

        power_onoff_combinekey_found = 0;
        key_data.key_bit_mask |= key;
        key_data.key_detected_num++;

#if F_APP_AIRPLANE_SUPPORT
        key_data.key_bit_mask_airplane = 0;
#endif

        for (i = 0; i < HYBRID_KEY_NUM; i++)
        {
            if ((app_cfg_const.hybrid_key_mapping[i][0] == key_data.key_bit_mask) &&
                (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_COMBINEKEY_POWER_ONOFF))
            {
                power_onoff_combinekey_found = 1;
                key_data.combine_key_bits = key_data.key_bit_mask;

#if F_APP_AIRPLANE_SUPPORT
                if (app_cfg_const.hybrid_key_table[0][i] == MMI_AIRPLANE_MODE)
                {
                    key_data.key_bit_mask_airplane = key_data.key_bit_mask;
                }
#endif
                break;
            }
        }

        if (((key == key_data.mfb_key) && (key_data.key_detected_num == 1)) ||
            ((power_onoff_combinekey_found == 1) && (app_cfg_const.enable_combinekey_power_onoff)))
        {
            app_dlps_disable(APP_DLPS_ENTER_CHECK_MFB_KEY);

            if (app_db.device_state == APP_DEVICE_STATE_OFF) //Power on press
            {
                if (app_cfg_const.discharger_support)
                {
                    T_APP_CHARGER_STATE app_charger_state;
                    uint8_t state_of_charge;

                    app_charger_state = app_charger_get_charge_state();
                    state_of_charge = app_charger_get_soc();
                    if ((app_charger_state == APP_CHARGER_STATE_NO_CHARGE) && (state_of_charge == BAT_CAPACITY_0))
                    {
                        return;
                    }
                }

                if (app_cfg_const.charging_disallow_power_on)
                {
                    //Disallow power on if in the box, ensure safety when some product do not allow power on when charging
                    if (app_adp_get_plug_state() == ADAPTOR_PLUG)
                    {
                        if (app_cfg_const.enable_factory_reset_when_in_the_box)
                        {
                            app_key_start_factory_reset_timer();
                        }
                        return;
                    }
                }

#if F_APP_GPIO_ONOFF_SUPPORT
                if ((app_cfg_const.box_detect_method == GPIO_DETECT) &&
                    (app_gpio_on_off_get_location() == IN_CASE))
                {
                    if (app_cfg_const.enable_factory_reset_when_in_the_box &&
                        (app_adp_get_plug_state() == ADAPTOR_PLUG))
                    {
                        app_key_start_factory_reset_timer();
                    }
                    //Disallow power on action if detect pin in low state
                    return;
                }
#endif

#if F_APP_SLIDE_SWITCH_POWER_ON_OFF
                if (!app_slide_switch_power_on_valid_check())
                {
                    //Disallow power on action in slide switch module
                    return;
                }
#endif

                if ((app_cfg_const.key_power_on_interval != 0) &&
                    ((app_cfg_const.key_disable_power_on_off == 0) ||
                     (app_cfg_nv.factory_reset_done == 0))) //Long press power on
                {
					//ysc start
                    app_start_timer(&timer_idx_key_power_on_long_press, "key_power_on_long_press",
                                    key_timer_id, APP_TIMER_KEY_POWER_ON_LONG_PRESS, key, false,
                                    SPECAIL_LONG_KEY_TIME * KEY_TIMER_UNIT_MS);
					//ysc end				
                }

                if ((app_cfg_const.key_enter_pairing_interval != 0) &&
                    ((app_cfg_const.key_disable_power_on_off == 0) ||
                     (app_cfg_nv.factory_reset_done == 0))) //Long press power enter pairing
                {
                    app_start_timer(&timer_idx_key_enter_pairing, "key_enter_pairing",
                                    key_timer_id, APP_TIMER_KEY_ENTER_PAIRING, key, false,
                                    app_cfg_const.key_enter_pairing_interval * KEY_TIMER_UNIT_MS);
                }

                if ((app_cfg_const.enable_factory_reset_when_in_the_box == 0) ||
                    app_device_is_in_the_box())
                {
                    app_key_start_factory_reset_timer();
                }

            }
            else if (app_db.device_state == APP_DEVICE_STATE_ON) //Power off press
            {
                /* if support click and press, and not first click */
                if (app_cfg_const.key_click_and_press_interval != 0 && key_data.key_click != 0)
                {
                    app_start_timer(&timer_idx_key_click_and_press, "key_click_and_press",
                                    key_timer_id, APP_TIMER_KEY_CLICK_AND_PRESS, key, false,
                                    app_cfg_const.key_click_and_press_interval * KEY_TIMER_UNIT_MS);
                }
                else
                {
#if F_APP_LEA_SUPPORT
                    if ((app_cfg_const.enable_power_off_only_when_call_idle == 0)
                        || (app_bt_policy_get_call_status() == APP_CALL_IDLE)
#if HARMAN_ALLOW_POWER_OFF_ON_CALL_ACTIVE_SUPPORT
                        || (app_bt_policy_get_call_status() == BT_HFP_CALL_ACTIVE)
#endif
                       )

#else
                    if ((app_cfg_const.enable_power_off_only_when_call_idle == 0)
                        || (app_hfp_get_call_status() == APP_CALL_IDLE)
#if HARMAN_ALLOW_POWER_OFF_ON_CALL_ACTIVE_SUPPORT
                        || (app_hfp_get_call_status() == BT_HFP_CALL_ACTIVE)
#endif
                       )

#endif
                    {
                        if ((app_cfg_const.key_power_off_interval != 0) &&
                            ((app_cfg_const.key_disable_power_on_off == 0) ||
                             (app_cfg_nv.factory_reset_done == 0))) //Long press power off
                        {
                            app_start_timer(&timer_idx_key_power_off_long_press, "key_power_off_long_press",
                                            key_timer_id, APP_TIMER_KEY_POWER_OFF_LONG_PRESS, key, false,
                                            app_cfg_const.key_power_off_interval * KEY_TIMER_UNIT_MS);
                        }
                    }

                    if (app_cfg_const.key_long_press_interval != 0)
                    {
//ysc start
#if HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
                        if (key == key_data.mfb_key1 &&
                                app_harman_get_long_function() == FUNC_VOICE_ASSISTANT && !app_bt_policy_get_call_status())
                        {
                            APP_PRINT_TRACE0("2861 APP_TIMER_KEY_LONG_PRESS_INITIATE~~~~~~~");
                            app_start_timer(&timer_idx_key_long_press, "key_long_press",
                                key_timer_id, APP_TIMER_KEY_LONG_PRESS, key, false,
                                SPECAIL_LONG_KEY_TIME * KEY_TIMER_UNIT_MS);
                        }
                        else
#endif
                        {
							APP_PRINT_TRACE0("2804 APP_TIMER_KEY_LONG_PRESS~~~~~~~");
                            app_start_timer(&timer_idx_key_long_press, "key_long_press",
                                        key_timer_id, APP_TIMER_KEY_LONG_PRESS, key, false,
                                        app_cfg_const.key_long_press_interval * KEY_TIMER_UNIT_MS);
                        }
                    }

                    if (app_cfg_const.key_ultra_long_press_interval != 0)
                    {
#if HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
                        if (key == SPECAIL_ULTRA_LONG_KEY_ID)
                        {
                            APP_PRINT_TRACE0("2843 APP_TIMER_KEY_ULTRA_LONG_PRESS~~~~~~~");
                            app_start_timer(&timer_idx_key_ultra_long_press, "key_ultra_long_press",
                                        key_timer_id,APP_TIMER_KEY_ULTRA_LONG_PRESS, key,false,
                                        SPECAIL_ULTRA_LONG_KEY_TIME * KEY_TIMER_UNIT_MS);
                        }
                        else
#endif
                        {
                            app_start_timer(&timer_idx_key_ultra_long_press, "key_ultra_long_press",
                                        key_timer_id, APP_TIMER_KEY_ULTRA_LONG_PRESS, key, false,
                                        app_cfg_const.key_ultra_long_press_interval * KEY_TIMER_UNIT_MS);
                        }
//ysc end						
                    }
                }
            }
        }
        else
        {
#if 0
            if ((app_cfg_const.enable_hall_switch_function == 1) && (app_cfg_const.key_hall_switch_mask == key))
            {
                if (app_key_hybrid_hall_switch_match(key, HYBRID_KEY_HALL_SWITCH_LOW))
                {
                    key_data.key_bit_mask = 0;
                    key_data.key_detected_num = 0;
                    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                    return;
                }
            }
#endif

            //Combine Key pressed
            if (key_data.key_detected_num >= 2)
            {
                key = key_data.key_bit_mask;
                key_data.combine_key_bits = key_data.key_bit_mask;
                app_key_stop_timer();
            }

            /* if support click and press, and not first click */
            if (app_cfg_const.key_click_and_press_interval != 0 && key_data.key_click != 0)
            {
                app_start_timer(&timer_idx_key_click_and_press, "key_click_and_press",
                                key_timer_id, APP_TIMER_KEY_CLICK_AND_PRESS, key, false,
                                app_cfg_const.key_click_and_press_interval * KEY_TIMER_UNIT_MS);
            }
            else
            {
                if (app_cfg_const.key_long_press_interval != 0)
                {
//ysc start				
#if HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
                    if (key == key_data.mfb_key1 &&
                        app_harman_get_long_function() == FUNC_VOICE_ASSISTANT && !app_bt_policy_get_call_status())
                    {
                        APP_PRINT_TRACE0("2861 APP_TIMER_KEY_LONG_PRESS_INITIATE~~~~~~~");
                        app_start_timer(&timer_idx_key_long_press, "key_long_press",
                            key_timer_id, APP_TIMER_KEY_LONG_PRESS, key, false,
                            SPECAIL_LONG_KEY_TIME * KEY_TIMER_UNIT_MS);
                    }
                    else
#endif
                    {
                        APP_PRINT_TRACE0("2855 APP_TIMER_KEY_LONG_PRESS~~~~~~~");
                        app_start_timer(&timer_idx_key_long_press, "key_long_press",
                                        key_timer_id, APP_TIMER_KEY_LONG_PRESS, key, false,
                                        app_cfg_const.key_long_press_interval * KEY_TIMER_UNIT_MS);
                    }
                }
				
                if (app_cfg_const.key_ultra_long_press_interval != 0)
                {
#if HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
                    if (key == SPECAIL_ULTRA_LONG_KEY_ID)
                    {
                        APP_PRINT_TRACE0("2916 APP_TIMER_KEY_ULTRA_LONG_PRESS~~~~~~~");
                        app_start_timer(&timer_idx_key_ultra_long_press, "key_ultra_long_press",
                                        key_timer_id,APP_TIMER_KEY_ULTRA_LONG_PRESS, key, false,
                                        SPECAIL_ULTRA_LONG_KEY_TIME * KEY_TIMER_UNIT_MS);
                    }
                    else
#endif
                    {
                        app_start_timer(&timer_idx_key_ultra_long_press, "key_ultra_long_press",
                                    key_timer_id, APP_TIMER_KEY_ULTRA_LONG_PRESS, key, false,
                                    app_cfg_const.key_ultra_long_press_interval * KEY_TIMER_UNIT_MS);
                    }
//ysc end					
                }

#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                APP_PRINT_TRACE2("app_key_check_press: multilink_call_status %d, active_index %d",
                                 app_teams_multilink_get_voice_status(), app_teams_multilink_get_active_index());
                if (app_teams_multilink_get_voice_status() != APP_CALL_INCOMING)
#else
                if ((app_hfp_get_call_status() >= APP_CALL_ACTIVE) ||
                    (app_hfp_get_call_status() == APP_VOICE_ACTIVATION_ONGOING) ||
                    (app_hfp_get_call_status() == APP_CALL_IDLE))
#endif
                {
                    if (key == MIC_MUTE_KEY_MASK)
                    {
                        app_start_timer(&timer_idx_key_mute_long_press, "key_mute_long_press",
                                        key_timer_id, APP_TIMER_KEY_MUTE_LONG_PRESS, key, false,
                                        MIC_KEY_LONG_PRESS_INTERVAL * KEY_TIMER_UNIT_MS);
                        mute_key_long_press = false;

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                        uint8_t active_index = app_teams_multilink_get_active_index();

                        if (active_index == 0x03)
                        {
                            bool global_target_mute_status = !app_teams_audio_get_global_mute_status();

                            app_teams_audio_set_global_mute_status(global_target_mute_status);

                            if (global_target_mute_status)
                            {
                                app_audio_tone_type_play(TONE_MIC_MUTE_ON, true, true);
                            }
                            else
                            {
                                app_audio_tone_type_play(TONE_MIC_MUTE_OFF, true, true);
                            }
                        }
                        else
#endif
                        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                            if (active_index == 0xFF)
                            {
#if F_APP_USB_HID_SUPPORT
                                app_usb_mmi_handle_action(MMI_USB_TEAMS_MIC_MUTE_UNMUTE);
#endif
                            }
                            else
#endif
                            {
                                app_mmi_handle_action(MMI_DEV_MIC_MUTE_UNMUTE);
                            }
                        }
                    }
                }
#endif
            }
        }
    }
    else //Key release
    {
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
        if ((key == MIC_MUTE_KEY_MASK) && mute_key_long_press &&
            (!app_teams_audio_get_global_mute_status()) &&
            (app_teams_cmd_data.teams_ptt_status == 1))
        {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            uint8_t active_index = app_teams_multilink_get_active_index();

            if (active_index == 0x03)
            {
                bool global_target_mute_status = !app_teams_audio_get_global_mute_status();

                app_teams_audio_set_global_mute_status(global_target_mute_status);

                if (global_target_mute_status)
                {
                    app_audio_tone_type_play(TONE_MIC_MUTE_ON, true, true);
                }
                else
                {
                    app_audio_tone_type_play(TONE_MIC_MUTE_OFF, true, true);
                }
            }
            else
#endif
            {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                if (active_index == 0xFF)
                {
#if F_APP_USB_HID_SUPPORT
                    app_usb_mmi_handle_action(MMI_USB_TEAMS_MIC_MUTE_UNMUTE);
#endif
                }
                else
#endif
                {
                    app_mmi_handle_action(MMI_DEV_MIC_MUTE_UNMUTE);
                }
            }
        }

        if (key == MIC_MUTE_KEY_MASK)
        {
            app_stop_timer(&timer_idx_key_mute_long_press);
            mute_key_long_press = false;
        }
#endif

        key_data.key_bit_mask &= ~key;

        //fix hall switch not power off issue
        if (key_data.key_detected_num == 0)
        {
            key_data.key_detected_num = 0;
        }
        else
        {
            key_data.key_detected_num--;
        }

        if (key_data.combine_key_bits)
        {
            if (key_data.key_detected_num != 0)
            {
                //once combine key was pressed, detect key/action after all key release
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                return;
            }
            else
            {
                key = key_data.combine_key_bits;
            }
        }

        app_key_stop_timer();

#if 0
        if ((app_cfg_const.enable_hall_switch_function == 1) && (app_cfg_const.key_hall_switch_mask == key))
        {
            if (app_key_hybrid_hall_switch_match(key, HYBRID_KEY_HALL_SWITCH_HIGH))
            {
                key_data.key_detected_num = 0;
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                return;
            }
        }
#endif

        if ((key == key_data.mfb_key) ||
            ((power_onoff_combinekey_found == 1) && (app_cfg_const.enable_combinekey_power_onoff)))
        {
            app_dlps_enable(APP_DLPS_ENTER_CHECK_MFB_KEY);

            if (app_db.device_state == APP_DEVICE_STATE_OFF) //Power on release
            {
                if (app_cfg_const.discharger_support)
                {
                    T_APP_CHARGER_STATE app_charger_state;
                    uint8_t state_of_charge;

                    app_charger_state = app_charger_get_charge_state();
                    state_of_charge = app_charger_get_soc();
                    if ((app_charger_state == APP_CHARGER_STATE_NO_CHARGE) && (state_of_charge == BAT_CAPACITY_0))
                    {
                        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                        return;
                    }
                }

                if ((app_cfg_const.charging_disallow_power_on) &&
                    (app_adp_get_plug_state() == ADAPTOR_PLUG))
                {
                    if (!app_cfg_const.enable_factory_reset_when_in_the_box)
                    {
                        app_key_reset_enter_pairing();
                        key_data.key_long_pressed = 0;
                        app_db.is_long_press_power_on_play_vp = 0;
                        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                        return;
                    }
                }
                else
                {
                    if (app_cfg_const.key_power_on_interval != 0) //Long press power on
                    {
                        if (key_data.key_long_pressed == LONG_PRESS_POWER_ON)
                        {
#if F_APP_AIRPLANE_SUPPORT
                            if (power_onoff_combinekey_found && key_data.key_bit_mask_airplane)
                            {
                                app_airplane_combine_key_power_on_set(1);
                            }
#endif
                            app_mmi_handle_action(MMI_DEV_POWER_ON);
                        }
                    }
                    else
                    {
                        if (key_data.key_long_pressed == 0) //Short press power on
                        {
                            // Before factory reset, allow long press MFB to power on and enter pairing when key_disable_power_on_off
                            if ((app_cfg_const.key_disable_power_on_off == 0) ||
                                (app_cfg_nv.factory_reset_done == 0))
                            {
                                app_mmi_handle_action(MMI_DEV_POWER_ON);

                                app_device_unlock_vbat_disallow_power_on();
                            }
                        }
                    }
                }

                if (key_data.key_long_pressed == LONG_PRESS_ENTER_PAIRING)
                {
                    app_mmi_handle_action(MMI_DEV_ENTER_PAIRING_MODE);
                }

                if (key_data.key_long_pressed == LONG_PRESS_FACTORY_RESET)
                {
                    app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
                }

                if (key_data.key_long_pressed == LONG_PRESS_FACTORY_RESET_PHONE)
                {
                    app_mmi_handle_action(MMI_DEV_PHONE_RECORD_RESET);
                }

                app_key_reset_enter_pairing();
                key_data.key_long_pressed = 0;
                app_db.is_long_press_power_on_play_vp = 0;
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

                if ((app_cfg_const.enable_factory_reset_when_in_the_box == 1) &&
                    app_device_is_in_the_box())
                {
                    goto hybrid;
                }

                if (key_data.combine_key_bits)
                {
                    goto exit;
                }
                return;

            }
            else if ((app_db.device_state == APP_DEVICE_STATE_OFF_ING) ||
                     (app_db.device_state == APP_DEVICE_STATE_ON))   //Power off release
            {
                if (app_cfg_const.key_power_off_interval != 0) //Long press power off
                {
                    app_key_long_key_power_off_pressed_set(false);

                    if (key_data.key_long_pressed == LONG_PRESS_POWER_OFF)
                    {
                        key_data.key_long_pressed = 0;
                        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                        return;
                    }
                    else if (key_data.key_long_pressed == LONG_PRESS ||
                             key_data.key_long_pressed == BETWEEN_LONG_AND_ULTRA_LONG_PRESS)
                    {
                        if ((app_cfg_const.key0_tigger_long_press_option == 0) ||
                            ((app_cfg_const.key_trigger_long_press != 1) &&
                             (key_data.key_long_pressed == BETWEEN_LONG_AND_ULTRA_LONG_PRESS)))
                        {
                            app_key_single_click(key);
                        }
                        key_data.key_long_pressed = 0;
                        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

                        return;
                    }
                }
                else if ((app_cfg_const.key_disable_power_on_off == 0) ||
                         (app_cfg_nv.factory_reset_done == 0)) //Short press power off
                {
#if F_APP_LEA_SUPPORT
                    if ((app_cfg_const.enable_power_off_only_when_call_idle == 0) ||
                        (app_bt_policy_get_call_status() == APP_CALL_IDLE))
#else
                    if ((app_cfg_const.enable_power_off_only_when_call_idle == 0) ||
                        (app_hfp_get_call_status() == APP_CALL_IDLE))
#endif
                    {
                        app_db.power_off_cause = POWER_OFF_CAUSE_SHORT_KEY;

#if F_APP_ERWS_SUPPORT
                        app_roleswap_poweroff_handle(false);
#else
                        app_mmi_handle_action(MMI_DEV_POWER_OFF);
#endif
                    }

                    key_data.key_long_pressed = 0;
                    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                    return;
                }
            }
        }

        if ((key_data.key_long_pressed == LONG_PRESS) ||
            (key_data.key_long_pressed == LONG_PRESS_REPEAT))
        {
            if (key_data.key_action == MMI_AV_FASTFORWARD)
            {
                // Special handling Fast-forward release MMI, maybe triggered from SPK2
                app_key_execute_action(MMI_AV_FASTFORWARD_STOP);
            }
            else if (key_data.key_action == MMI_AV_REWIND)
            {
                // Special handling Rewind release MMI, maybe triggered from SPK2
                app_key_execute_action(MMI_AV_REWIND_STOP);
            }
        }

hybrid:
        if (key_data.key_long_pressed == 0) //Short press
        {
            if (key_data.key_click == 0) //First key
            {
                key_data.key_click++;

                if ((app_cfg_const.key_multi_click_interval != 0) && app_key_multi_click_match(key))
                {
                    if ((key == key_data.mfb_key) && SEPEARTE_MFB_GSENSOR)
                    {
                        // if GSensor enabled, disable key click triggered by MFB key
                        app_key_clear();
                        goto exit;
                    }
                    else
                    {
                        app_start_timer(&timer_idx_key_multi_click, "key_multi_click",
                                        key_timer_id, APP_TIMER_KEY_MULTI_CLICK, key, false,
                                        app_cfg_const.key_multi_click_interval * KEY_TIMER_UNIT_MS);
                    }
                }
                else
                {
                    // if GSensor enabled, disable key click triggered by MFB key
                    if ((key == key_data.mfb_key) && SEPEARTE_MFB_GSENSOR)
                    {
                        app_key_clear();
                        goto exit;
                    }

                    if (key_data.combine_key_bits == 0) //Single Key press
                    {
                        app_key_single_click(key);
                    }
                    else
                    {
                        app_key_hybrid_click(key_data.combine_key_bits);
                    }
                }
            }
            else
            {
                if (key_data.pre_key == key) //Same key repeat click
                {
                    key_data.key_click++;

                    if ((app_cfg_const.key_multi_click_interval != 0) && app_key_multi_click_match(key))
                    {
                        app_stop_timer(&timer_idx_key_multi_click);

                        if ((key == key_data.mfb_key) && SEPEARTE_MFB_GSENSOR)
                        {
                            // if GSensor enabled, disable key click triggered by MFB key
                            app_key_clear();
                            goto exit;
                        }
                        else
                        {
                            app_start_timer(&timer_idx_key_multi_click, "key_multi_click",
                                            key_timer_id, APP_TIMER_KEY_MULTI_CLICK, key, false,
                                            app_cfg_const.key_multi_click_interval * KEY_TIMER_UNIT_MS);
                        }
                    }
                }
                else //Different keys, process directly
                {
                    // if GSensor enabled, disable key click triggered by MFB key
                    if ((key == key_data.mfb_key) && SEPEARTE_MFB_GSENSOR)
                    {
                        app_key_clear();
                        goto exit;
                    }
                    else
                    {
                        //Previous key
                        if ((key_data.key_click == 1) && (key_data.pre_combine_key_bits == 0))
                        {
                            app_key_single_click(key_data.pre_key);
                        }
                        else
                        {
                            app_key_hybrid_click(key_data.pre_key);
                        }

                        //Current key
                        if (key_data.combine_key_bits == 0)
                        {
                            app_key_single_click(key);
                        }
                        else
                        {
                            app_key_hybrid_click(key_data.combine_key_bits);
                        }
                    }
                    app_stop_timer(&timer_idx_key_multi_click);
                }
            }
        }
        else if (key_data.key_long_pressed == BETWEEN_LONG_AND_ULTRA_LONG_PRESS)
        {
            if (key_data.combine_key_bits == 0)
            {
                app_key_single_click(key);
            }
            else
            {
                app_key_hybrid_click(key);
            }
        }
        else if (key_data.key_long_pressed == ULTRA_LONG_PRESS)
        {
            /* ultra long press triggered by key release */
            if (app_cfg_const.timeout_trigger_ultra_long_press == 0)
            {
                app_key_hybrid_click(key);
            }
        }

exit:
        if (key_data.key_bit_mask == 0) //All key release
        {
            key_data.pre_key = key;
            key_data.pre_combine_key_bits = key_data.combine_key_bits;
            key_data.key_long_pressed = 0;
            key_data.combine_key_bits = 0;
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_MFB_KEY);

            if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off &&
                (app_link_get_b2s_link_num() != 0))
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_KEY,
                                          app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
            }
        }
    }
}
static void app_key_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
#if F_APP_KEY_EXTEND_FEATURE
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                /* sec sync default mmi to pri,
                    pri would merge and sync final setting to sec later */
                app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_SEC_MMI);

#if F_APP_RWS_KEY_SUPPORT
                app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RWS_KEY_SEC_MMI);
#endif
            }
#endif
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
        APP_PRINT_TRACE1("app_key_bt_cback: event 0x%04x, status %d", event_type);

    }
}

#if F_APP_ERWS_SUPPORT
static void app_key_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    bool ret = true;

    switch (msg_type)
    {
#if F_APP_KEY_EXTEND_FEATURE
    case APP_KEY_MAP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                memcpy((void *)app_key_stored_key_info, buf, sizeof(T_CUSTOMER_KEY_INFO));
                app_key_save_user_param();
            }
        }
        break;

    case APP_KEY_SEC_MMI:
    case APP_KEY_SEC_DEFAULT_MMI:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_key_set_remote_key_setting(app_key_stored_key_info->app_key_remap_info, buf);
                app_key_report_key_mmi_map(EVENT_REPORT_KEY_MMI_MAP);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_MAP);
                }
            }
        }
        break;

    case APP_KEY_RESET:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_key_set_default_key_setting(app_key_stored_key_info->app_key_remap_info, true, NULL);
                app_key_save_user_param();

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_SEC_DEFAULT_MMI);
                }
            }
        }
        break;

#if F_APP_RWS_KEY_SUPPORT
    case APP_KEY_RWS_KEY_MAP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                memcpy((void *)app_key_stored_rws_key_info, buf, sizeof(T_CUSTOMER_RWS_KEY_INFO));
                app_key_save_rws_key_user_param();
            }
        }
        break;

    case APP_KEY_RWS_KEY_SEC_MMI:
    case APP_KEY_RWS_KEY_SEC_DEFAULT_MMI:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (buf[0])
                {
                    app_key_stored_rws_key_info->is_setting_rws_key = true;
                }

                app_key_stored_rws_key_info->already_sync = true;
                app_key_set_remote_key_setting(app_key_stored_rws_key_info->app_key_remap_info, &buf[1]);
                app_key_report_key_mmi_map(EVENT_REPORT_RWS_KEY_MMI_MAP);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RWS_KEY_MAP);
                }
            }
        }
        break;

    case APP_KEY_RWS_KEY_RESET:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_key_reset_rws_key_user_param(false);

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_relay_async_single(APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_RWS_KEY_SEC_DEFAULT_MMI);
                }
            }
        }
        break;

    case APP_KEY_RWS_KEY_TONE_RELAY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_APP_AUDIO_TONE_TYPE tone_type = (T_APP_AUDIO_TONE_TYPE)buf[0];
                bool relay_key_tone = (bool)buf[1];

                APP_PRINT_TRACE2("app_key_parse_cback: APP_KEY_RWS_KEY_TONE_RELAY tone_type 0x%04x, relay_key_tone %d",
                                 tone_type, relay_key_tone);
                app_audio_tone_type_play(tone_type, false, relay_key_tone);
            }
        }
        break;

#endif
#endif

    default:
        {
            ret = false;
        }
        break;
    }

    if (ret)
    {
        APP_PRINT_TRACE2("app_key_parse_cback: event 0x%04x, status %d", msg_type, status);
    }
}

static uint16_t app_key_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    uint8_t *spk2_mmi_evt = NULL;
    uint16_t msg_len;

    switch (msg_type)
    {
#if F_APP_KEY_EXTEND_FEATURE
    case APP_KEY_MAP:
        {
            payload_len = sizeof(T_CUSTOMER_KEY_INFO);
            msg_ptr = (uint8_t *)app_key_stored_key_info;
        }
        break;

    case APP_KEY_SEC_DEFAULT_MMI:
    case APP_KEY_SEC_MMI:
        {
            uint8_t click_type_num = SUPPORTED_CLICK_TYPE_NUM;
            uint8_t call_state_num = SUPPORTED_CALL_STATE_NUM;

            payload_len = click_type_num * call_state_num;
            spk2_mmi_evt = malloc(click_type_num * call_state_num);

            if (spk2_mmi_evt != NULL)
            {
                memset(spk2_mmi_evt, 0x00, click_type_num * call_state_num);
                app_key_get_key_setting_for_relay(app_key_stored_key_info->app_key_remap_info,
                                                  spk2_mmi_evt, call_state_num, click_type_num);
                msg_ptr = (uint8_t *)spk2_mmi_evt;
            }
        }
        break;

#if F_APP_RWS_KEY_SUPPORT
    case APP_KEY_RWS_KEY_MAP:
        {
            payload_len = sizeof(T_CUSTOMER_RWS_KEY_INFO);
            msg_ptr = (uint8_t *)app_key_stored_rws_key_info;
        }
        break;

    case APP_KEY_RWS_KEY_SEC_MMI:
    case APP_KEY_RWS_KEY_SEC_DEFAULT_MMI:
        {
            uint8_t click_type_num = SUPPORTED_CLICK_TYPE_NUM;
            uint8_t call_state_num = SUPPORTED_CALL_STATE_NUM;

            payload_len = click_type_num * call_state_num + 1;
            spk2_mmi_evt = malloc(payload_len);

            if (spk2_mmi_evt != NULL)
            {
                memset(spk2_mmi_evt, 0x00, click_type_num * call_state_num);
                spk2_mmi_evt[0] = app_key_is_rws_key_setting();

                app_key_get_key_setting_for_relay(app_key_stored_rws_key_info->app_key_remap_info,
                                                  &spk2_mmi_evt[1], call_state_num, click_type_num);
                msg_ptr = (uint8_t *)spk2_mmi_evt;
            }
        }
        break;
#endif
#endif

    default:
        break;
    }

    msg_len = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_KEY_PROCESS, payload_len, msg_ptr, skip,
                                 total);

    if (spk2_mmi_evt != NULL)
    {
        free(spk2_mmi_evt);
    }

    return msg_len;
}
#endif

static void app_key_factory_reset_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_FACTORY_RESET:
        {
#if F_APP_KEY_EXTEND_FEATURE
            app_key_reset_user_param();
#if F_APP_RWS_KEY_SUPPORT
            app_key_reset_rws_key_user_param(true);
#endif
#endif
        }
        break;

    default:
        break;
    }
}

void app_key_init(void)
{
    key_data.mfb_key = KEY0_MASK;
	//ysc start
	key_data.mfb_key1 = KEY1_MASK;
	//ysc end
    bt_mgr_cback_register(app_key_bt_cback);

#if F_APP_KEY_EXTEND_FEATURE
    app_key_remap_init();
#endif

    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_key_factory_reset_cback);

#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_key_relay_cback, app_key_parse_cback,
                             APP_MODULE_TYPE_KEY_PROCESS, APP_KEY_MSG_MAX);
#endif

    app_timer_reg_cb(app_key_timeout_cb, &key_timer_id);
}

bool app_key_is_enter_pairing(void)
{
    return (key_data.key_enter_pairing == 1);
}

void app_key_reset_enter_pairing(void)
{
    key_data.key_enter_pairing = 0;
}

void app_key_set_volume_status(bool volume_status)
{
    is_key_volume_set = volume_status;
}

bool app_key_is_set_volume(void)
{
    return is_key_volume_set;
}

void app_key_set_sync_play_vol_changed_tone_disallow(bool disallow_sync)
{
    disallow_sync_play_vol_changed_tone = disallow_sync;
}

bool app_key_is_sync_play_vol_changed_tone_disallow(void)
{
    return disallow_sync_play_vol_changed_tone;
}

void app_key_hybrid_multi_clicks(uint8_t key, uint8_t clicks)
{
    key_data.key_click = clicks;
    app_key_hybrid_click(key);
}

void app_key_power_onoff_combinekey_dlps_process(void)
{
    PAD_WAKEUP_POL_VAL wake_up_val;
    uint8_t i, j, tmp_combinekey;

    for (i = 0; i < HYBRID_KEY_NUM; i++)
    {
        if (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_COMBINEKEY_POWER_ONOFF)
        {
            tmp_combinekey = app_cfg_const.hybrid_key_mapping[i][0];
            for (j = 0; j < MAX_KEY_NUM; j++)
            {
                if (tmp_combinekey % 2 == 1)
                {
                    wake_up_val = (app_cfg_const.key_high_active_mask & BIT(j)) ? PAD_WAKEUP_POL_HIGH :
                                  PAD_WAKEUP_POL_LOW;
                    Pad_ControlSelectValue(app_cfg_const.key_pinmux[j], PAD_SW_MODE);
                    System_WakeUpPinEnable(app_cfg_const.key_pinmux[j], wake_up_val);
                    System_WakeUpInterruptEnable(app_cfg_const.key_pinmux[j]);
                }
                tmp_combinekey = tmp_combinekey >> 1;
            }
            break;
        }
    }
}

ISR_TEXT_SECTION bool app_key_is_combinekey_power_on_off(uint8_t key)
{
    uint8_t i;
    bool ret = false;

    for (i = 0; i < HYBRID_KEY_NUM; i++)
    {
        if (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_COMBINEKEY_POWER_ONOFF)
        {
            if (app_cfg_const.hybrid_key_mapping[i][0] & BIT(key))
            {
                ret = true;
            }
        }
    }
    return ret;
}

ISR_TEXT_SECTION bool app_key_is_gpio_combinekey_power_on_off(uint8_t gpio_index)
{
    uint8_t i;
    bool ret = false;

    for (i = 1; i < MAX_KEY_NUM; i++)
    {
        if (gpio_index == app_cfg_const.key_pinmux[i])
        {
            if (app_key_is_combinekey_power_on_off(i))
            {
                ret = true;
            }
        }
    }
    return ret;
}

void app_key_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    T_KEY_CHECK key_check = {0};

    key_check.key = io_driver_msg_recv->u.param >> 8;
    key_check.key_pressed = io_driver_msg_recv->u.param & 0xFF;

    app_key_check_press(key_check);
#if F_APP_CLI_BINARY_MP_SUPPORT
    if (mp_test_get_mode_flag())
    {
        mp_test_vendor_send_key_event(key_check.key);
    }
#endif
}

#if 0
/**
    * @brief  App parses if there are hall switch function definitions in the hybrid key behaviors.
    *         Hall switch function is defined as one type of hybrid key.
    *         Execute this function to check if there's hall switch function used.
    *         Hall switch function actions are different from general hybrid key actions.
    * @param  void
    * @return void
    */
void app_key_hybird_parse_hall_switch_setting(void)
{
    uint8_t i = 0;

    for (i = 0; i < HYBRID_KEY_NUM; i++)
    {
        if ((app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_HALL_SWITCH_HIGH) ||
            (app_cfg_const.hybrid_key_mapping[i][1] == HYBRID_KEY_HALL_SWITCH_LOW))
        {
            // Record in app_cfg.key_hall_switch_stable_interval (app_cfg.key_hall_switch_mask) if hall switch actions are defined
            // key used for HYBRID_KEY_HALL_SWITCH_HIGH & HYBRID_KEY_HALL_SWITCH_LOW should be the same
            app_cfg_const.key_hall_switch_mask = app_cfg_const.hybrid_key_mapping[i][0];
            break;
        }
    }
}

/**
    * @brief  Read current status of Hall Switch sensor.
    * @param  void
    * @return status
    */
uint8_t app_key_read_hall_switch_status(void)
{
    // app_cfg.key_hall_switch_stable_interval: app_cfg.key_hall_switch_mask
    uint8_t i, key_num = 0;

    for (i = 0; i < MAX_KEY_NUM; i++)
    {
        if (app_cfg_const.key_hall_switch_mask == BIT(i))
        {
            // Find the key number of hall switch function
            key_num = i;
            break;
        }
    }
    return hal_gpio_get_input_level(app_cfg_const.key_pinmux[key_num]);
}

/**
    * @brief  Execute the corresponding action with Hall Switch status.
    *         Once Hall Switch status changed. It required a stable timer for debouncing.
    *         After the debouncing process, related action will be executed.
    *         Note that action only be executed in the correct status.
    * @param  hall_switch_action Hall Switch high or Hall Switch Low.
    * @return void
    */

void app_key_execute_hall_switch_action(uint8_t hall_switch_action)
{
    uint8_t i;
    uint8_t app_idx = app_a2dp_get_active_idx();
    bool mmi_play_pause_flag = false;

    // MMI is designed as play-pause toggle. Only initiate play or pause if status matched.
    if (hall_switch_action == HYBRID_KEY_HALL_SWITCH_LOW)
    {
        if (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
        {
            mmi_play_pause_flag = true;
        }
    }
    else
    {
        if ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED) ||
            (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED))
        {
            mmi_play_pause_flag = true;
        }
    }

    // Find hybrid key with hall switch definition
    for (i = 0; i < MAX_KEY_NUM; i++)
    {
        if ((app_cfg_const.hybrid_key_mapping[i][0] == app_cfg_const.key_hall_switch_mask) &&
            (app_cfg_const.hybrid_key_mapping[i][1] == hall_switch_action))
        {
#if F_APP_LEA_SUPPORT
            key_data.key_action = app_cfg_const.hybrid_key_table[app_bt_policy_get_call_status()][i];
#else
            key_data.key_action = app_cfg_const.hybrid_key_table[app_hfp_get_call_status()][i];
#endif
            if ((key_data.key_action != MMI_AV_PLAY_PAUSE) || (mmi_play_pause_flag))
            {
                app_key_execute_action(key_data.key_action);
            }
            break;
        }
    }
}
#endif

void app_key_check_vol_mmi(void)
{
    uint8_t key_idx = 0;
    uint8_t hybrid_key_enable_mask = 0;
    bool check_hybrid_click_flag = true;
    bool check_single_click_flag = true;
    bool has_vol_ctrl_key = false;

    for (key_idx = 0; key_idx < MAX_KEY_NUM; key_idx++)
    {
        if ((app_cfg_const.hybrid_key_mapping[key_idx][0] != 0) &&
            (app_cfg_const.hybrid_key_mapping[key_idx][1] != 0xff))//Hybrid key enable
        {
            hybrid_key_enable_mask |= (0x01 << key_idx);
        }
    }

    for (key_idx = 0; key_idx < MAX_KEY_NUM; key_idx++)
    {
        check_hybrid_click_flag = true;
        check_single_click_flag = true;

        if ((BIT(key_idx) & hybrid_key_enable_mask) == 0)
        {
            check_hybrid_click_flag = false;
        }

        if (((BIT(key_idx) & app_cfg_const.key_enable_mask) == 0) && (key_idx != 0))
        {
            check_single_click_flag = false;
        }

        if (check_single_click_flag &&
            (((app_cfg_const.key_table[0][APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_UP) ||
              (app_cfg_const.key_table[0][APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_DOWN)) ||
             ((app_cfg_const.key_table[1][APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_UP) ||
              (app_cfg_const.key_table[1][APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_DOWN))))
        {
            has_vol_ctrl_key = true;
        }

        if (check_hybrid_click_flag &&
            ((app_cfg_const.hybrid_key_table[APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_UP) ||
             (app_cfg_const.hybrid_key_table[APP_CALL_IDLE][key_idx] == MMI_DEV_SPK_VOL_DOWN)))
        {
            has_vol_ctrl_key = true;
        }

        if (has_vol_ctrl_key)
        {
            break;
        }
    }

    APP_PRINT_TRACE1("app_key_check_vol_mmi: ret %d", has_vol_ctrl_key);

    if (has_vol_ctrl_key != app_cfg_nv.either_bud_has_vol_ctrl_key)
    {
        app_cfg_nv.either_bud_has_vol_ctrl_key = has_vol_ctrl_key;
        app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);
    }
}

#if F_APP_CAP_TOUCH_SUPPORT
void app_key_cap_touch_key_process(uint8_t key_chek_press)
{
    T_KEY_CHECK key_check = {0};

    key_check.key = KEY0_MASK;
    key_check.key_pressed = key_chek_press;
    app_key_check_press(key_check);
}
#endif

