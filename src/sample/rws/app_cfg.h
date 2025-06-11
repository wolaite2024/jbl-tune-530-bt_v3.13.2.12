/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include <stdint.h>
#include <stdbool.h>

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_ble_ota.h"
#include "app_harman_eq.h"
#endif


#ifdef __cplusplus
extern "C" { /* __cplusplus */
#endif

// App base cpu freq
#if F_APP_POWER_TEST && TARGET_RTL8773DO
#define APP_BASE_CPU_FREQ                   40
#else
#define APP_BASE_CPU_FREQ                   40
#endif

//App config data
#define APP_DATA_SYNC_WORD                  0xAA55
#define APP_DATA_SYNC_WORD_LEN              2
#define APP_DATA_TONE_NUM_LEN               2

#define DATA_SYNC_WORD                      0xAA55AA55
#define DATA_SYNC_WORD_LEN                  4

#define APP_CONFIG_OFFSET                   0
#define APP_CONFIG_SIZE                     1024

#define APP_LED_OFFSET                      (APP_CONFIG_OFFSET + APP_CONFIG_SIZE)
#define APP_LED_SIZE                        512

#define SYS_CONFIG_OFFSET                   (APP_LED_OFFSET + APP_LED_SIZE)
#define SYS_CONFIG_SIZE                     512

#define APP_CONFIG2_OFFSET                  (SYS_CONFIG_OFFSET + SYS_CONFIG_SIZE)
#define APP_CONFIG2_SIZE                    512

#define EXTEND_APP_CONFIG_OFFSET            (APP_CONFIG2_OFFSET + APP_CONFIG2_SIZE)
#define EXTEND_APP_CONFIG_SIZE              128

#define TONE_DATA_OFFSET                    4096 //Rsv 4K for APP parameter for better flash control
#define TONE_DATA_SIZE                      3072
#define VOICE_PROMPT_OFFSET                 (TONE_DATA_OFFSET + TONE_DATA_SIZE)
#define VOICE_PROMPT_SIZE                   137 * 1024

//FTL start
#define APP_RW_DATA_ADDR                    3072
#define APP_RW_DATA_SIZE                    360

#define FACTORY_RESET_OFFSET                140

#if F_APP_HARMAN_FEATURE_SUPPORT
#define APP_HARMAN_RW_PAIR_LIST                 (APP_RW_DATA_ADDR + APP_RW_DATA_SIZE)
#define APP_HARMAN_RW_LIST_SIZE                 (OTA_FILE_FW_TABLE_ITEM_SIZE * 20)

#define HARMAN_SERIALS_NUM_OFFSET               (APP_HARMAN_RW_PAIR_LIST + APP_HARMAN_RW_LIST_SIZE)
#define HARMAN_SERIALS_NUM_LEN                  16
#define HARMAN_PRODUCT_ID_OFFSET                (HARMAN_SERIALS_NUM_OFFSET + HARMAN_SERIALS_NUM_LEN)
#define HARMAN_PRODUCT_ID_LEN                   2
#define HARMAN_COLOR_ID_OFFSET                  (HARMAN_PRODUCT_ID_OFFSET + HARMAN_PRODUCT_ID_LEN)
#define HARMAN_COLOR_ID_LEN                     1
#define HARMAN_RESVED_OFFSET                    (HARMAN_COLOR_ID_OFFSET + HARMAN_COLOR_ID_LEN)
#define HARMAN_RESVED_LEN                       1

// EQ DATA to DSP, max stage num is 17
#define APP_HARMAN_EQ_DATA                      (HARMAN_RESVED_OFFSET + HARMAN_RESVED_LEN)
#define APP_HARMAN_EQ_DATA_SIZE                 sizeof(T_APP_HARMAN_EQ_DATA) // 176

// customer EQ or preset EQ INFO from JBL APP, max stage num is 10
#define APP_HARMAN_CUSTOMER_EQ_INFO             (APP_HARMAN_EQ_DATA + APP_HARMAN_EQ_DATA_SIZE)
#define APP_HARMAN_CUSTOMER_EQ_INFO_SIZE        sizeof(T_APP_HARMAN_EQ_INFO) // 152

// design EQ INFO from DSP CFG, max stage num is 7
#define APP_HARMAN_DEFAULT_EQ_INFO              (APP_HARMAN_CUSTOMER_EQ_INFO + APP_HARMAN_CUSTOMER_EQ_INFO_SIZE)
#define APP_HARMAN_DEFAULT_EQ_INFO_SIZE         sizeof(T_APP_HARMAN_DEFAULT_EQ_INFO) // 112

#else
#define APP_RW_RESERVE0                     (APP_RW_DATA_ADDR + APP_RW_DATA_SIZE)
#define APP_RW_RESERVE0_SIZE                4

#define APP_RW_PX318J_ADDR                  (APP_RW_RESERVE0 + APP_RW_RESERVE0_SIZE)
#define APP_RW_PX318J_SIZE                  12

#define APP_RW_JSA_ADDR                     (APP_RW_PX318J_ADDR + APP_RW_PX318J_SIZE)
#define APP_RW_JSA_SIZE                     12

#define APP_RW_CAPTOUCH_CALIBRATION_ADDR    (APP_RW_JSA_ADDR + APP_RW_JSA_SIZE)
#define APP_RW_CAPTOUCH_CALIBRATION_SIZE    32

#define APP_RW_SAIYAN_ADDR                  (APP_RW_CAPTOUCH_CALIBRATION_ADDR + APP_RW_CAPTOUCH_CALIBRATION_SIZE)
#define APP_RW_SAIYAN_SIZE                  4

#define APP_FINDMY_INFO_ADDR                (APP_RW_SAIYAN_ADDR + APP_RW_SAIYAN_SIZE)
#define APP_FINDMY_INFO_SIZE                320

#define APP_RW_RESERVE1                     (APP_FINDMY_INFO_ADDR + APP_FINDMY_INFO_SIZE)
#define APP_RW_RESERVE1_SIZE                872

#define APP_RW_AVP_ID_DATA_ADDR             (APP_RW_RESERVE1 + APP_RW_RESERVE1_SIZE)
#define APP_RW_AVP_ID_DATA_SIZE             36

#define TILE_APPDATA_BANK_A_ADDR            (APP_RW_AVP_ID_DATA_ADDR + APP_RW_AVP_ID_DATA_SIZE)
#define TILE_STORAGE_SIZE                   28

#define TILE_LINK_BUD_ADDR                  (TILE_APPDATA_BANK_A_ADDR + TILE_STORAGE_SIZE)
#define TILE_ADDR_SIZE                      8

#define TILE_LINK_BUD_LE_ADDR               (TILE_LINK_BUD_ADDR + TILE_ADDR_SIZE)
#define TILE_LE_ADDR_SIZE                   8

#define APP_RW_KEY_REMAP_INFO_ADDR          (TILE_LINK_BUD_LE_ADDR + TILE_LE_ADDR_SIZE)
#define APP_RW_KEY_REMAP_INFO_SIZE          112 /* note: this size must be sync with sizeof(T_CUSTOMER_KEY_INFO) */

#define APP_RW_RESERVE2                     (APP_RW_KEY_REMAP_INFO_ADDR + APP_RW_KEY_REMAP_INFO_SIZE)
#define APP_RW_RESERVE2_SIZE                64

#define APP_RW_DUAL_AUDIO_KEY_VAL_ADDR      (APP_RW_RESERVE2 + APP_RW_RESERVE2_SIZE)
#define APP_RW_DUAL_AUDIO_KEY_VAL_SIZE      32

#define APP_RW_RESERVE3                     (APP_RW_DUAL_AUDIO_KEY_VAL_ADDR + APP_RW_DUAL_AUDIO_KEY_VAL_SIZE)
#define APP_RW_RESERVE3_SIZE                128

#define APP_RW_VENDOR_DRIVER_DATA_ADDR      (APP_RW_RESERVE3 + APP_RW_RESERVE3_SIZE)
#define APP_RW_VENDOR_DRIVER_DATA_SIZE      128

#define APP_RW_RWS_KEY_REMAP_INFO_ADDR      (APP_RW_VENDOR_DRIVER_DATA_ADDR + APP_RW_VENDOR_DRIVER_DATA_SIZE)
#define APP_RW_RWS_KEY_REMAP_INFO_SIZE      112 /* note: this size must be sync with sizeof(T_CUSTOMER_RWS_KEY_INFO) */
#endif
//FTL end

#define TRANSPORT_BIT_LE                    0x01
#define TRANSPORT_BIT_SPP                   0x02
#define TRANSPORT_BIT_IAP                   0x04

#define VOICE_PROMPT_INDEX                  0x80
#define TONE_INVALID_INDEX                  0xFF

#define AAC_LOW_LATENCY_MS                  60
#define SBC_LOW_LATENCY_MS                  60
#define TWS_ULTRA_LOW_LATENCY_MS            20
#define ULTRA_LOW_LATENCY_MS                5
#define ULTRA_LOW_LATENCY_MS_WITH_MULTI_SRC 10
#define ULTRA_LOW_LATENCY_MS_WITH_LINKBACK  25

#if F_APP_SUPPORT_SPATIAL_AUDIO && TARGET_RTL8753GFE
#define A2DP_LATENCY_MS                     185
#elif F_APP_HARMAN_FEATURE_SUPPORT
#define A2DP_LATENCY_MS                     124
#else
#define A2DP_LATENCY_MS                     280
#endif

#define GAMING_MODE_DYNAMIC_LATENCY_FIX     false
#define NORMAL_MODE_DYNAMIC_LATENCY_FIX     false

#if F_APP_SUPPORT_SPATIAL_AUDIO && TARGET_RTL8753GFE
#define PLAYBACK_POOL_SIZE                  (10*1024)
#else
#define PLAYBACK_POOL_SIZE                  (15*1024)
#endif
#define VOICE_POOL_SIZE                     (2*1024)
#define RECORD_POOL_SIZE                    (0*1024)
#define NOTIFICATION_POOL_SIZE              (2*1024)

#define POOL_SIZE_FACTOR                    (15*1024/280.0f)

#define RSSI_FLASH_ADDR_OFFSET              0x2000   // offset 8K, start from syspatch  // 0x02130000
#define RSSI_FLASH_SIZE                     0x14000  // 80K
#define LOG2FLASH_ADDR_OFFSET               (RSSI_FLASH_ADDR_OFFSET + RSSI_FLASH_SIZE)  // 0x02124000
#define LOG2FLASH_SIZE                      (flash_partition_size_get(PARTITION_FLASH_OTA_BANK_1) - LOG2FLASH_ADDR_OFFSET)

typedef enum t_dsp_output_log
{
    DSP_OUTPUT_LOG_NONE = 0x0,          //!< no DSP log.
    DSP_OUTPUT_LOG_BY_UART = 0x1,       //!< DSP log by uart directly.
    DSP_OUTPUT_LOG_BY_MCU = 0x2,        //!< DSP log by MCU.
} T_DSP_OUTPUT_LOG;

typedef enum t_channel_type
{
    CHANNEL_L = 0x01,
    CHANNEL_R = 0x02,
    CHANNEL_LR_HELF = 0x03,
} T_CHANNEL_TYPE;

typedef enum t_device_bud_side
{
    DEVICE_BUD_SIDE_LEFT = 0,
    DEVICE_BUD_SIDE_RIGHT = 1,
} T_DEVICE_BUD_SIDE;

/** @brief in/out chargebox detect method for power on off function*/
typedef enum t_detect_type
{
    DISABLE_DETECT = 0,
    ADAPTOR_DETECT = 1,
    GPIO_DETECT = 2,
} T_DETECT_TYPE;

/** @defgroup APP_CFG App Cfg
  * @brief App Cfg
  * @{
  */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_CFG_Exported_Types App Cfg Types
    * @{
    */
/** @brief  Read/Write configurations for inbox audio application  */
typedef struct
{
    uint32_t sync_word;
    uint32_t length;
} T_APP_CFG_NV_HDR;

/*T_APP_CFG_NV usage:
The offsets of existing items MUST NOT be changed:
1. If deleting existing items, positions of these items shall be reserved
2. If adding new items, the new items shall be appended in the end
*/
typedef struct
{
    T_APP_CFG_NV_HDR hdr;

    //offset: 0x08
    uint8_t device_name_legacy[40];
    uint8_t device_name_le[40];

    //offset: 0x58
    uint8_t le_single_random_addr[6];
    uint8_t first_engaged;
    uint8_t bud_role;
    uint8_t last_btmode[8];
    uint8_t bud_sn[8];

    //offset: 0x70
    uint8_t bud_local_addr[6];
    uint8_t bud_peer_addr[6];
    uint8_t bud_peer_factory_addr[6];
    uint16_t peer_valid_magic;

    //offset: 0x84
    uint32_t single_tone_timeout_val;

    //offset: 0x88
    uint8_t single_tone_tx_gain;
    uint8_t single_tone_channel_num;
    union
    {
        uint8_t offset_xtal_k_result;
        struct
        {
            uint8_t xtal_k_result : 3;
            uint8_t time_delay_to_open_apt_when_power_on : 5;
        };
    };
    uint8_t apt_volume_out_level;

    //offset: 0x8c
    uint8_t eq_idx_anc_mode_record;
    uint8_t anc_apt_state;

    union
    {
        uint8_t offset_misc_config;
        struct
        {
            uint8_t id_is_display : 1;
            uint8_t is_app_reboot : 1;
            uint8_t is_clockwise : 1;
            uint8_t ear_detect_en : 1;
            uint8_t single_cfg_rsv0 : 1;
            uint8_t enable_multi_link : 1;
            uint8_t mic_setting : 2;
        };
    };

    union
    {
        uint8_t offset_app_is_power_on;
        struct
        {
            uint8_t app_is_power_on : 1;
            uint8_t pre_3pin_status_unplug : 1;
            uint8_t factory_reset_done : 1;
            uint8_t adp_factory_reset_power_on : 1;
            uint8_t ota_tooling_start : 1;
            uint8_t need_set_lps_mode : 1;
            uint8_t gpio_out_detected : 1;
            uint8_t adaptor_is_plugged : 1;
        };
    };

    uint8_t eq_idx;
    uint8_t audio_effect_normal_type;
    uint8_t audio_effect_gaming_type;

    uint8_t anc_cur_setting;
    uint8_t anc_one_setting;
    uint8_t anc_both_setting;

    struct
    {
        uint8_t anc_cycle_setting : 3;
        uint8_t anc_one_bud_enabled : 1;
        uint8_t dual_audio_space : 2;
        uint8_t disable_report_avp_id : 1;
        uint8_t single_cfg_rsv1 : 1;
    };

    uint8_t local_remote_long_action;

    uint8_t case_battery;
    uint8_t local_level;
    uint8_t remote_level;
    uint8_t local_loc;

    //offset: 0x9c
    uint8_t remote_loc;
    union
    {
        uint8_t offset_smart_chargerbox;
        struct
        {
            uint8_t power_off_cause_cmd : 1;
            uint8_t power_on_cause_cmd: 1;
            uint8_t ori_bud_role : 2;
            uint8_t slide_switch_power_on_off_gpio_status: 1;
            uint8_t disable_charger_by_box_battery : 1;
            uint8_t is_bt_pairing : 1;
            uint8_t cfg_rsv_bit0 : 1;
        };
    };

    uint8_t audio_gain_level[8];
    uint8_t voice_gain_level[8];

    uint8_t voice_prompt_language;

    uint8_t spk_channel;
    uint8_t mic_channel;
    uint8_t pin_code_size;
    uint8_t pin_code[8];

    uint8_t line_in_gain_level;
    uint8_t usb_need_start;

    //offset: 0xbc
    union
    {
        uint8_t offset_notification_vol;
        struct
        {
            uint8_t voice_prompt_volume_out_level : 4;
            uint8_t ringtone_volume_out_level : 4;
        };
    };

    uint8_t light_sensor_enable;
    uint8_t eq_idx_gaming_mode_record;
    uint8_t eq_idx_normal_mode_record;

    //offset: 0xc0
    union
    {
        uint8_t offset_is_dut_test_mode;
        struct
        {
            uint8_t is_dut_test_mode : 1;
            uint8_t adaptor_changed : 1;
            uint8_t ota_parking_lps_mode: 2;
            uint8_t report_box_bat_lv_again_after_sw_reset : 1;
            uint8_t auto_power_on_after_factory_reset : 1;
            uint8_t trigger_dut_mode_from_power_off : 1;
            uint8_t rws_disallow_sync_apt_volume : 1;
        };
    };

    uint8_t local_remote_double_action;

    union
    {
        uint8_t offset_jig_dongle_id;
        struct
        {
            uint8_t jig_dongle_id : 5;
            uint8_t jig_subcmd : 3;
        };
    };

    uint8_t apt_eq_idx;

    union
    {
        uint8_t offset_listening_mode_cycle;
        struct
        {
            uint8_t listening_mode_cycle : 2;
            uint8_t rws_low_latency_level_record : 3;
            uint8_t disallow_power_on_when_vbat_in: 1;
            uint8_t is_notification_vol_set_from_phone : 1;
            uint8_t allowed_source : 1;
        };
    };

    // rtk fast pair advertisement count
    uint8_t adv_count;

    uint8_t abs_vol[8];

    uint8_t le_rws_random_addr[6];

    uint8_t main_brightness_level;
    uint16_t apt_sub_volume_out_level;
    uint8_t eq_idx_line_in_mode_record;

    uint8_t app_pair_idx_mapping[8];

    uint16_t sub_brightness_level;

    union
    {
        uint8_t offset_rws_sync_notification_vol;
        struct
        {
            uint8_t rws_sync_notification_vol : 1;
            uint8_t rws_disallow_sync_llapt_brightness : 1;
            uint8_t either_bud_has_vol_ctrl_key : 1;
            uint8_t reboot_after_ota : 1;
            uint8_t is_dual_mode : 1; /* true: support bt only or bt and 2.4g; false: support 2.4g only */
            uint8_t listening_mode_cycle_ext : 2;
            uint8_t apt_support_type : 1;
        };
    };

    union
    {
        uint8_t offset_one_wire_aging_test_done;
        struct
        {
            uint8_t one_wire_aging_test_done : 1;
            uint8_t one_wire_disable_power_on : 1;
            uint8_t one_wire_start_force_engage : 1;
            uint8_t one_wire_reset_to_uninitial_state : 1;
            uint8_t le_audio_dongle_connected_record : 1;
            uint8_t one_wire_rsv : 3;
        };
    };

    uint16_t anc_selected_list;
    uint16_t llapt_selected_list;

    int32_t gyro_bias[3];
    int32_t acc_bias[3];
    uint8_t lea_sirk[16];
    uint8_t lea_mode_enable;
    uint8_t lea_broadcast_id[3];
    uint8_t lea_rsv[3];
    uint8_t lea_srcaddr[6];
    uint8_t lea_srctype;
    uint8_t lea_srcsid;
    uint8_t lea_srcch;
    uint8_t lea_valid;
    uint8_t lea_vol_setting;
    uint8_t lea_vol_out_mute;
    uint8_t lea_vcs_change_cnt;
    uint8_t lea_vcs_vol_flag;
    uint8_t lea_step_size;

    uint8_t iqs773_sensitivity_ati_target;
    uint8_t iqs773_trigger_level;
    uint8_t spatial_auto_cal;
    uint8_t lea_encryp;
    uint8_t lea_code[16];
    /* audio latency : 2 bytes (offset 0x3AE) */
    uint16_t audio_latency;

    uint8_t primary_irk[16];
    uint8_t secondary_irk[16];
    uint16_t sass_switching_preference;
    uint8_t sass_multipoint_state;
    uint8_t sass_advanced_switching_setting;
    uint8_t sass_bitmap;
    uint8_t sass_recently_used_account_key_index;
    uint8_t rsvd2;

    // TODO:  T520 items, need to remove
    uint8_t language_type;
    uint16_t cmd_latency_value;
    uint16_t rsv5;
    uint8_t rsv1[2];
    uint8_t rsv2[4];
    uint8_t rsv3[4];

    uint8_t rsv4[5];
    uint8_t harman_volume_step;
    uint8_t harman_LR_option;
    uint8_t harman_LR_vol_level;
    // todo end

    union
    {
        uint16_t harman_auto_power_off;
        struct
        {
            uint16_t auto_power_off_time : 15;
            uint16_t auto_power_off_status : 1;
        };
    };
    union
    {
        uint16_t harman_auto_standby;
        struct
        {
            uint16_t auto_standby_time : 15;
            uint16_t auto_standby_status : 1;
        };
    };

    union
    {
        uint8_t harman_sidetone;
        struct
        {
            uint8_t sidetone_level : 7;
            uint8_t sidetone_switch : 1;
        };
    };
    union
    {
        uint8_t harman_max_volume_limit;
        struct
        {
            uint8_t max_volume_limit_level : 7;
            uint8_t max_volume_limit_status : 1;
        };
    };
    uint8_t host_type; // 0x01: Android, 0x02: IOS, 0x03: Dongle, 0x04: SoudBar
    union
    {
        uint8_t harman_language_status;
        struct
        {
            uint8_t rsv7 : 7;
            uint8_t language_status : 1;
        };
    };

    uint8_t language_version[3];
    uint8_t usb_connector_protection_enabled;


    uint8_t vp_ota_status;
    uint8_t rsv12;
    union
    {
        uint8_t find_my_buds_status;
        struct
        {
            uint8_t rsv9 : 5;
            uint8_t left_bud_status : 1; // 0:stop beeping, 1: start beeping
            uint8_t right_bud_status : 1; // 0:stop beeping, 1: start beeping
            uint8_t case_status : 1; // 0:stop beeping, 1: start beeping
        };
    };
    uint8_t rsv10;

    /* Default audio mode:  256, 53, 150, 230
               normal mode: 197, 53, 150, 230
               video mode:  197, 53, 120, 230
    1. Default mode is Smart Audio & Video on and focus on audio mode.
    2. When Smart Audio & Video mode is set to off, use normal mode parameter.
    3. The headset latency strictly use APP preset values, shall pass EE
        performance test criteria.
    */
    uint16_t cmd_aac_bit_rate;
    uint16_t cmd_sbc_max_bitpool;
    uint16_t cmd_latency_in_ms;
    uint16_t cmd_second_latency_in_ms;

    uint16_t ota_mtu;
    uint16_t ota_max_size_in_one_pack;

    uint8_t ota_continuous_packets_max_count;
    uint8_t harman_ota_complete;
    uint8_t cur_sub_image_index;
    uint8_t vbat_detect_status;
    //offset: 0x114
    uint32_t harman_record_ota_version;
    uint32_t harman_breakpoint_offset;
    uint32_t cur_sub_image_relative_offset;

    uint8_t harman_category_id;
    uint8_t vbat_detect_normal;   // VBAT_DETECT_STATUS
    uint8_t harman_default_EQ_stage_num;
    uint8_t harman_customer_stage;

    uint16_t harman_EQmaxdB;
    union
    {
        uint8_t harman_LR_balance;
        struct
        {
            uint8_t LR_vol_level : 7;
            uint8_t LR_option : 1;
        };
    };
    uint8_t xtal_k_times;
    uint32_t harman_record_crc_for_breakpoint;
    uint32_t total_playback_time;
    uint32_t total_power_on_time;
    uint32_t rtc_wakeup_count;
    uint32_t vbat_ntc_value;

    uint32_t algo_status;

    uint8_t  mic_switch_seq;
    uint8_t  mic_status_seq[5];
    uint8_t  mic_status[5];
    uint8_t  mic_status_rsvd;

    uint8_t  remote_device_vendor_id[8];
} T_APP_CFG_NV;

/** @brief  Read only configurations for inbox audio application */
typedef struct
{
    uint32_t sync_word;

    //Timer: 32 bytes(offset 0x04)
    uint8_t timer_hfp_ring_period;
    uint8_t timer_no_service_warning;
    uint16_t timer_linkback_timeout;
    uint16_t timer_auto_power_off;
    uint16_t timer_pairing_timeout;
    uint16_t timer_link_avrcp;
    uint8_t timer_low_bat_warning;
    uint8_t timer_mic_mute_alarm;
    uint16_t timer_link_spp;
    uint16_t timer_ota_adv_timeout;
    uint16_t timer_tts_adv_timeout;
    uint8_t timer_rws_creation;
    uint8_t timer_ext_amp_boost;
    uint16_t timer_link_back_loss;
    uint8_t timer_low_bat_led;
    uint8_t timer_hfp_auto_answer_call;
    uint16_t timer_auto_power_off_while_phone_connected_and_anc_apt_off;
    uint16_t timer_swift_pair_adv_timeout;
    uint16_t timer_monitor_heap_and_timer_timeout;
    uint16_t timer_pairing_while_one_conn_timeout;

    //Key: 32 bytes (offset 0x24)
    uint8_t key_multi_click_interval;
    uint8_t key_long_press_interval;
    uint8_t key_long_press_repeat_interval;
    uint8_t key_power_on_interval;
    uint8_t key_power_off_interval;
    uint8_t key_enter_pairing_interval;
    uint8_t key_factory_reset_interval;
    uint8_t nfc_stable_interval;
    uint8_t key_pinmux[8];
    uint8_t key_enable_mask;
    uint8_t key_long_press_repeat_mask;
    uint8_t key_short_press_tone_mask;
    uint8_t key_long_press_tone_mask;
    uint8_t hybrid_key_press_tone_mask;
    uint8_t key_ultra_long_press_interval;
    uint8_t key_high_active_mask;
    uint8_t key_hall_switch_mask;
    uint8_t key_hall_switch_stable_interval;
    uint8_t key0_tigger_long_press_option;
    uint8_t key_disable_power_on_off;
    uint8_t key_trigger_long_press;
    uint8_t timeout_trigger_ultra_long_press : 1;
    uint8_t mfb_replace_key0 : 1;
    uint8_t reserved_key_rsv1 : 6;
    uint8_t key_factory_reset_rws_interval;
    uint8_t key_click_and_press_interval;
    uint8_t key_ultra_long_press_repeat_interval;

    //Key table: 232 bytes (offset 0x44)
    uint8_t key_table[2][9][8]; //Table 0: key short click, Table 1: key long press
    uint8_t hybrid_key_table[9][8];
    uint8_t hybrid_key_mapping[8][2]; //Mapping[0]: Key Index. Mapping[1]: Hybrid Key click type

    /* NOTICE: Make sure T_APP_AUDIO_TONE_TYPE is align to app_cfg_const.tone_xxxx offset */
    //Tone1: 128 bytes (offset 0x12c)
    uint8_t tone_power_on;
    uint8_t tone_power_off;
    uint8_t tone_pairing;
    uint8_t tone_pairing_complete;
    uint8_t tone_pairing_fail;
    uint8_t tone_key_short_press;
    uint8_t tone_key_long_press;
    uint8_t tone_vol_changed;
    uint8_t tone_key_hybrid;
    uint8_t tone_hf_no_service;
    uint8_t tone_hf_call_in;
    uint8_t tone_first_hf_call_in;
    uint8_t tone_last_hf_call_in;
    uint8_t tone_hf_call_active;
    uint8_t tone_hf_call_end;
    uint8_t tone_hf_call_reject;
    uint8_t tone_hf_call_voice_dial;
    uint8_t tone_hf_call_redial;
    uint8_t tone_in_ear_detection;
    uint8_t tone_gfps_findme;
    uint8_t tone_link_connected;
    uint8_t tone_vol_max;
    uint8_t tone_vol_min;
    uint8_t tone_alarm; // also used for tone_exit_dongle_mode
    uint8_t tone_link_loss;
    uint8_t tone_link_disconnect;
    uint8_t tone_remote_connected;
    uint8_t tone_remote_role_primary;
    uint8_t tone_remote_role_secondary;
    uint8_t tone_light_sensor_on;
    uint8_t tone_light_sensor_off;
    uint8_t tone_exit_gaming_mode;
    uint8_t tone_enter_airplane;
    uint8_t tone_ota_over_ble_start;
    uint8_t tone_le_pair_complete;
    uint8_t tone_pairing_timeout;
    uint8_t tone_mic_mute_alarm;
    uint8_t tone_key_ultra_long_press;
    uint8_t tone_dev_multilink_on;
    uint8_t tone_remote_disconnect;
    uint8_t tone_factory_reset;
    uint8_t tone_remote_loss;
    uint8_t tone_mic_mute_on;
    uint8_t tone_mic_mute_off;
    uint8_t tone_link_loss2;
    uint8_t tone_link_disconnect2;
    uint8_t tone_battery_percentage_10;
    uint8_t tone_battery_percentage_20;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint8_t tone_harman_power_on;
    uint8_t tone_harman_power_off;
    uint8_t tone_harman_factory_rst;
    uint8_t tone_harman_pair;
    uint8_t tone_harman_call_in;
    uint8_t tone_harman_connect;
    uint8_t tone_harman_vol_max;
    uint8_t tone_harman_mic_mute_on;
#else
    uint8_t tone_battery_percentage_30;
    uint8_t tone_battery_percentage_40;
    uint8_t tone_battery_percentage_50;
    uint8_t tone_battery_percentage_60;
    uint8_t tone_battery_percentage_70;
    uint8_t tone_battery_percentage_80;
    uint8_t tone_battery_percentage_90;
    uint8_t tone_battery_percentage_100;
#endif
    uint8_t tone_audio_playing;
    uint8_t tone_audio_paused;
    uint8_t tone_audio_forward;
    uint8_t tone_audio_backward;
    uint8_t tone_apt_on;
    uint8_t tone_apt_off;
    uint8_t tone_enter_gaming_mode;
    uint8_t tone_output_indication_1;

    uint8_t tone_switch_vp_language;
    uint8_t tone_dut_test;
    uint8_t tone_hf_transfer_to_phone;
    uint8_t tone_hf_outgoing_call;
    uint8_t tone_audio_effect_0;
    uint8_t tone_audio_effect_1;
    uint8_t tone_audio_effect_2;
    uint8_t tone_sound_press_calibration;

    uint8_t tone_exit_airplane;
    uint8_t tone_anc_apt_off;
#if F_APP_HARMAN_FEATURE_SUPPORT
    uint8_t tone_harman_mic_mute_off;
    uint8_t tone_harman_battery_percentage_10;
    uint8_t tone_harman_battery_percentage_20;
    uint8_t tone_harman_gfps_find_me;
    uint8_t tone_harman_dult_tone;
    uint8_t tone_harman_dult_vp;
#else
    uint8_t tone_anc_scenario_1;
    uint8_t tone_anc_scenario_2;
    uint8_t tone_anc_scenario_3;
    uint8_t tone_anc_scenario_4;
    uint8_t tone_anc_scenario_5;
    uint8_t tone_enter_dongle_mode;
#endif

    uint8_t tone_apt_vol_0;
    uint8_t tone_apt_vol_1;
    uint8_t tone_apt_vol_2;
    uint8_t tone_apt_volume_up;
    uint8_t tone_apt_volume_down;
#if F_APP_FINDMY_FEATURE_SUPPORT
    uint8_t tone_find_my;
#else
    uint8_t tone_audio_eq_0;
#endif
    uint8_t tone_audio_eq_1;
    uint8_t tone_audio_eq_2;
    uint8_t tone_audio_eq_3;
    uint8_t tone_audio_eq_4;
    uint8_t tone_audio_eq_5;
    uint8_t tone_audio_eq_6;
    uint8_t tone_audio_eq_7;
    uint8_t tone_audio_eq_8;
    uint8_t tone_audio_eq_9;
    uint8_t tone_apt_vol_max;
    uint8_t tone_apt_vol_min;
    uint8_t tone_apt_eq_0;
    uint8_t tone_apt_eq_1;
    uint8_t tone_apt_eq_2;
    uint8_t tone_apt_eq_3;
    uint8_t tone_apt_eq_4;
    uint8_t tone_apt_eq_5;
    uint8_t tone_apt_eq_6;
    uint8_t tone_apt_eq_7;
    uint8_t tone_apt_eq_8;
    uint8_t tone_apt_eq_9;
    uint8_t tone_llapt_scenario_1;
    uint8_t tone_llapt_scenario_2;
    uint8_t tone_llapt_scenario_3;
    uint8_t tone_llapt_scenario_4;
    uint8_t tone_llapt_scenario_5;
    uint8_t tone_ama_start_speech;
    uint8_t tone_ama_stop_speech;
    uint8_t tone_ama_error_indication;
    uint8_t tone_apt_vol_3;
    uint8_t tone_apt_vol_4;
    uint8_t tone_apt_vol_5;
    uint8_t tone_apt_vol_6;
    uint8_t tone_apt_vol_7;
    uint8_t tone_apt_vol_8;
#if GFPS_LE_DEVICE_SUPPORT
    uint8_t gfps_pairing_tone;
#else
    uint8_t tone_apt_vol_9;
#endif
    uint8_t tone_bis_start;
    uint8_t tone_bis_stop;
    uint8_t tone_cis_start;
    uint8_t tone_cis_timeout;
    uint8_t tone_cis_simu ;
    uint8_t tone_bis_loss;

    //Buzzer: 24 bytes (offset 0x1ac)
    uint8_t buzzer_pwm_freq;
    uint8_t buzzer_mode_power_on;
    uint8_t buzzer_mode_power_off;
    uint8_t buzzer_mode_enter_pairing;
    uint8_t buzzer_mode_pairing_complete;
    uint8_t buzzer_mode_incoming_call;
    uint8_t buzzer_mode_battery_low;
    uint8_t buzzer_mode_link_weak;
    uint8_t buzzer_mode_link_loss;
    uint8_t buzzer_mode_alarm;
#if F_APP_DURIAN_SUPPORT
    uint8_t avp_single_id[12];
    uint8_t display_bat_status_for_android: 1;
    uint8_t buzzer_rsv: 7;
    uint8_t buzzer_rsv0;
#else
    uint8_t buzzer_rsv[14];
#endif

    //Peripheral: 48 bytes (offset 0x1c4)
    uint8_t data_uart_tx_pinmux;
    uint8_t data_uart_rx_pinmux;
    uint8_t tx_wake_up_pinmux;
    uint8_t rx_wake_up_pinmux;
    uint8_t i2c_0_dat_pinmux;
    uint8_t i2c_0_clk_pinmux;
    uint8_t nfc_pinmux;
    uint8_t line_in_pinmux;
    uint8_t pwm_pinmux;
    uint8_t led_0_pinmux;
    uint8_t led_1_pinmux;
    uint8_t led_2_pinmux;

    uint8_t ext_amp_pinmux;
    uint8_t thermistor_power_pinmux;
    uint8_t micbias_pinmux;
    uint8_t ext_amp_boost_pinmux;

    uint8_t gpio_box_detect_pinmux;
    uint8_t output_indication1_pinmux;
    uint8_t output_indication2_pinmux;
    uint8_t output_indication3_pinmux;
    uint8_t sensor_detect_pinmux;
    uint8_t sensor_result_pinmux;
    uint8_t gpio_rtk_box_detect_pinmux;//remove fake 3 pin
    uint8_t gsensor_int_pinmux;
    uint8_t external_mcu_input_pinmux;
    uint8_t sd_pwr_ctrl_pinmux;
    uint8_t slide_switch_0_pinmux;
    uint8_t slide_switch_1_pinmux;
    uint8_t one_wire_uart_data_pinmux;
    uint8_t one_wire_uart_gpio_pinmux;
    uint8_t pcba_shipping_mode_pinmux;
    uint8_t qdec_y_pha_pinmux;
    uint8_t qdec_y_phb_pinmux;
    uint8_t buck_enable_pinmux;
    uint8_t pinumx_rsv1[2];

    uint8_t data_uart_baud_rate;
    uint8_t dt_resend_num;

    uint8_t integration_time;       /* SENSOR_VENDOR_JSA1225 */
    uint8_t waiting_time;           /* SENSOR_VENDOR_JSA1225 */
    uint8_t interrupt_persistence;  /* SENSOR_VENDOR_JSA1225 */

    uint8_t iosensor_active : 1;    /* 0: low active, 1: high active */
    uint8_t px_mean : 2;            /* SENSOR_VENDOR_PX318J */
    uint8_t px_output_precise : 2;  /* SENSOR_VENDOR_PX318J */
    uint8_t px_pulse_time : 3;      /* SENSOR_VENDOR_PX318J */
    uint8_t px_waiting_time;        /* SENSOR_VENDOR_PX318J */
    uint8_t px_pulse_duration : 6;  /* SENSOR_VENDOR_PX318J */
    uint8_t jsa_gain : 2;           /* SENSOR_VENDOR_JSA1225 */
    uint8_t jsa_pulse;              /* SENSOR_VENDOR_JSA1225 */

    uint8_t iqs773_bias : 2;                    /* PSENSOR_VENDOR_IQS773 */
    uint8_t iqs773_hall_effect_support : 1;     /* PSENSOR_VENDOR_IQS773 */
    uint8_t psensor_vendor : 4;
    uint8_t one_wire_uart_support : 1;

    uint8_t one_wire_uart_gpio_support : 1;
    uint8_t wheel_support : 1;
    uint8_t report_uart_event_only_once : 1;
    uint8_t mems_support : 1;
    uint8_t ntc_power_by_pad_direct : 1;
    uint8_t peripheral_rsv1 : 3;

    uint8_t enable_external_mcu_reset : 1;
    uint8_t iqs_release_debounce : 4;
    uint8_t mems_vendor : 2;
    uint8_t peripheral_rsv3 : 1;

    //RWS: 32 bytes (offset 0x1f4)
    uint16_t rws_custom_uuid;
    uint8_t rws_disconnect_enter_pairing : 1;
    uint8_t bud_role : 2; // 0x0: Single, 0x1: primary, 0x2: Secondary
    uint8_t enable_outbox_power_on : 1;
    uint8_t enable_inbox_power_off : 1;
    uint8_t bud_side : 1;
    uint8_t rws_circular_vol_up_when_solo_mode : 1;
    uint8_t rws_enable_seamless_join: 1;

    int8_t rws_pairing_required_rssi;

    uint8_t rws_rsv1[6];

    uint16_t recovery_link_a2dp_interval;
    uint16_t recovery_link_a2dp_interval_gaming_mode;//for temp use like seamless handover
    uint16_t recovery_link_a2dp_flush_timeout;
    uint16_t recovery_link_a2dp_flush_timeout_gaming_mode;
    uint8_t recovery_link_a2dp_rsvd_slot;
    uint8_t recovery_link_a2dp_rsvd_slot_gaming_mode;

    uint8_t rws_linkloss_linkback_timeout;
    uint8_t rws_gaming_mode;
    uint8_t rws_remote_link_encryption_off; // disable b2b link encryption

    uint8_t rws_connected_show_channel_vp : 1;
    uint8_t rws_sniff_negotiation : 1;
    uint8_t rws_disallow_sync_apt_volume : 1;
    uint8_t rssi_roleswap_judge_timeout: 5;
    uint8_t disable_link_monitor_roleswap : 1;
    uint8_t roleswap_rssi_threshold : 4;
    uint8_t rws_disable_codec_mute_when_linkback : 1;
    uint8_t rws_apt_eq_adjust_separate: 1;
    uint8_t rws_disallow_sync_llapt_brightness: 1;
    uint8_t rws_rsv[7];

    //App option: 8 bytes (offset 0x214)
    //app_option0
    uint8_t enable_direct_power_on : 1;
    uint8_t enable_power_off_to_dlps_mode : 1;
    uint8_t enable_power_on_linkback : 1;
    uint8_t enable_power_on_linkback_fail_enter_pairing : 1;
    uint8_t enable_link_back_acl_in_interlaced : 1;
    uint8_t enable_pairing_timeout_to_power_off : 1;
    uint8_t enable_discoverable_in_standby_mode : 1;
    uint8_t enable_always_discoverable : 1; //used in multi-link app
    //app_option1
    uint8_t enable_auto_answer_incoming_call : 1;
    uint8_t aux_in_channel : 1; //(0: Mono, 1: Stereo)
    uint8_t disable_bt_when_aux_plugged_in : 1;
    uint8_t enable_last_num_redial_always_by_first_phone: 1;
    uint8_t enable_last_num_redial_always_by_last_phone: 1;
    uint8_t enable_repeat_dut_test_tone : 1;
    uint8_t enable_vad : 1;
    uint8_t enable_rtk_vendor_cmd : 1;
    //app_option2
    uint8_t enable_nfc_multi_link_switch : 1;
    uint8_t enable_data_uart : 1;
    uint8_t enable_report_lower_battery_volume : 1;
    uint8_t enable_tx_wake_up : 1;
    uint8_t enable_rx_wake_up : 1;
    uint8_t always_play_hf_incoming_tone_when_incoming_call : 1;
    uint8_t auto_power_on_and_enter_pairing_mode_before_factory_reset : 1;
    uint8_t enable_dlps : 1;
    //app_option3
    uint8_t led_support : 1;
    uint8_t key_gpio_support : 1;
    uint8_t llapt_support : 1;
    uint8_t line_in_support : 1;
    uint8_t nfc_support : 1;
    uint8_t buzzer_support : 1;
    uint8_t pwm_support : 1;
    uint8_t charger_support : 1;
    //app_option4
    uint8_t normal_apt_support : 1;
    uint8_t enable_auto_power_off_when_listening_mode_is_not_off : 1;
    uint8_t disable_multilink_preemptive : 1;
    uint8_t enable_multi_sco_disc_resume : 1;
    uint8_t disable_fake_earfit_verification : 1;
    uint8_t restore_a2dp_when_bud_in_ear_in_15s : 1;
    uint8_t app_option4_rsv6 : 1;
    uint8_t enable_slide_switch_detect : 1;
    //app_option5 (Options that can only be used for "support")
    uint8_t discharger_support : 1;
    uint8_t usb_audio_support : 1;
    uint8_t local_playback_support : 1;
    uint8_t swift_pair_support : 1;
    uint8_t rtk_app_adv_support : 1;
    uint8_t tts_support : 1;
    uint8_t output_indication_support : 1;
    uint8_t sensor_support : 1;

    //app_option6
    uint8_t enable_led0_low_active : 1;
    uint8_t enable_led1_low_active : 1;
    uint8_t enable_led2_low_active : 1;
    uint8_t enable_power_off_only_when_call_idle : 1;
    uint8_t enable_dsp_capture_data_by_spp : 1;
    uint8_t enable_power_off_immediately_when_close_case : 1;
    uint8_t enable_multi_link : 1;
    uint8_t charging_disallow_power_on : 1;
    //app_option7
    uint8_t enable_hall_switch_function : 1;
    uint8_t enable_double_click_power_off_under_dut_test_mode : 1;
    uint8_t enable_factory_reset_when_in_the_box : 1;
    uint8_t enable_circular_vol_up : 1;
    uint8_t enable_output_ind1_high_active : 1;
    uint8_t enable_output_ind2_high_active : 1;
    uint8_t enable_output_ind3_high_active : 1;
    uint8_t enable_combinekey_power_onoff : 1;

    //Miscellaneous: 124 bytes (offset 0x21c)
    uint8_t device_name_legacy_default[40];
    uint8_t device_name_le_default[40];
    uint8_t class_of_device[3];
    uint8_t max_legacy_multilink_devices;
    uint16_t hfp_brsf_capability;
    uint16_t a2dp_codec_type_sbc : 1;
    uint16_t a2dp_codec_type_aac : 1;
    uint16_t a2dp_codec_type_ldac : 1;
    uint16_t a2dp_codec_type_lhdc : 1;
    uint16_t a2dp_codec_type_rsv : 12;
    uint8_t battery_warning_percent;
    uint8_t sensor_vendor;
    union
    {
        uint8_t gsensor_click_sensitivity;
        uint8_t sc7a20_z_min;
    };
    union
    {
        uint8_t gsensor_click_threshold;
        uint8_t sc7a20_z_max;
    };
    uint32_t supported_profile_mask;
    uint8_t link_scenario;
    uint8_t pin_code_size;
    uint8_t pin_code[8];
    uint8_t gsensor_noise_threshold;
    uint8_t gsensor_click_pp_num;
    uint8_t gsensor_click_max_num;
    uint8_t voice_prompt_language;
#if F_APP_DURIAN_SUPPORT
    uint8_t avp_id[12];
#else
    uint8_t company_id[2];
    uint8_t uuid[2];
    uint8_t rsv[8];
#endif
    uint8_t mic_channel;
    uint8_t couple_speaker_channel;
    uint8_t solo_speaker_channel;

    //dsp_option0
    uint8_t enable_ctrl_ext_amp : 1;
    uint8_t enable_ext_amp_high_active : 1;
    uint8_t listening_mode_power_on_status : 2;
    uint8_t enable_ringtone_mixing : 1;
    uint8_t enable_vp_mixing : 1;
    uint8_t enable_anc_when_sco : 1;
    uint8_t enable_aux_in_supress_a2dp : 1;

    //dsp_option1
    uint8_t dsp_log_pin : 6;
    uint8_t dsp_log_output_select : 2;

    uint8_t calibration_spl_offset;

    uint8_t time_delay_to_open_anc : 2; // 0~3 sec
    uint8_t time_delay_to_close_anc : 2;
    uint8_t time_delay_to_open_llapt : 2;
    uint8_t time_delay_to_close_llapt : 2;

    uint8_t enable_llapt_when_sco : 1;
    uint8_t disallow_trigger_listening_mode_again_time : 2;
    uint8_t only_play_key_tone_on_trigger_bud_side : 1;
    uint8_t apt_support_type : 1;
    uint8_t dsp_option1_rsv_bits : 3;

    uint8_t smart_charger_enable_threshold;
    uint8_t smart_charger_disable_threshold;

    //App option_2: 8 bytes (offset 0x2a0)
    uint8_t enable_not_save_apt_status : 1;
    uint8_t enable_not_discoverable_when_linkback : 1;
    uint8_t disallow_listening_mode_before_bud_connected : 1;
    uint8_t smart_charger_control : 1;
    uint8_t enable_linkback_fail_try_next_stored_device : 1;
    uint8_t enable_dut_test_when_adp_in : 1;
    uint8_t enable_not_reset_device_name : 1;
    uint8_t enable_tts_only_report_number : 1;

    //app_option9
    uint8_t first_time_engagement_only_by_5v_command : 1;
    uint8_t cpu_freq_option : 4;
    uint8_t reserved_app_option9_rsv5 : 1;
    uint8_t enable_dongle_dual_mode : 1;
    uint8_t disable_key_when_dut_mode : 1;

    //app_option10
    uint8_t enable_disconneted_enter_pairing : 1;
    uint8_t enable_output_power_supply : 1;
    uint8_t enable_av_fwd_bwd_only_when_playing : 1;
    uint8_t enable_specific_service_uuid : 1;
    uint8_t box_detect_method : 2; //0:disable, 1:5v charger, 2:gpio
    uint8_t sdio_group_select : 2; //0:Disable, 1:SDIO_PinGroup_0, 2:SDIO_PinGroup_1

    //app_option11 (Options that can only be used for "support")
    uint8_t gsensor_support : 1;
    uint8_t sw_led_support  : 1;
    uint8_t psensor_support : 1;
    uint8_t slide_switch_0_support : 1;
    uint8_t slide_switch_1_support : 1;
    uint8_t app_option11_rsv5 : 1;
    uint8_t app_option11_rsv6 : 1;
    uint8_t thermistor_power_gpio_support : 1;

    //app_option12
    uint8_t timer_first_engage : 4;
    uint8_t enable_mfb_pin_as_gsensor_interrupt_pin : 1;
    uint8_t enable_rtk_fast_pair_adv : 1;
    uint8_t enable_align_default_volume_after_factory_reset: 1;
    uint8_t do_not_auto_power_off_when_case_not_close: 1;

    //app_option13
    uint8_t allow_trigger_mmi_when_bud_not_in_ear : 1;
    uint8_t enable_new_low_bat_time_unit : 1; //0:sec, 1:min
    uint8_t enable_bat_report_when_level_drop : 1;
    uint8_t reserved_app_option13_rsv3 : 1;
    uint8_t play_max_min_tone_when_adjust_vol_from_phone : 1;
    uint8_t maximum_linkback_number : 3;

    //app_option14
    uint8_t disable_power_off_by_mfb : 1;
    uint8_t disable_bat_report_when_power_on : 1;
    uint8_t enable_bat_report_when_phone_conn : 1;
    uint8_t enable_rtk_charging_box : 1;
    uint8_t disallow_charging_led_before_power_off : 1;
    uint8_t smart_charger_box_cmd_set : 2;
    uint8_t smart_charger_box_bit_length : 1;

    //app_option15
    uint8_t disable_low_bat_role_swap : 1;
    uint8_t rws_disallow_sync_power_off : 1;
    uint8_t enable_disallow_sync_power_off_under_voltage_0_percent : 1;
    uint8_t listening_mode_cycle : 2;
    uint8_t enable_4pogo_pin : 1;//reserved for mcu tool
    uint8_t enable_high_batt_primary : 1;
    uint8_t light_sensor_default_disabled : 1;

    //Profile A2DP codec settings + profile link number: Total 36 bytes(offset = 680)
    //SBC settings
    uint8_t sbc_sampling_frequency;
    uint8_t sbc_channel_mode;
    uint8_t sbc_block_length;
    uint8_t sbc_subbands;
    uint8_t sbc_allocation_method;
    uint8_t sbc_min_bitpool;
    uint8_t sbc_max_bitpool;
    uint8_t sbc_rsv;

    //AAC settings
    uint16_t aac_sampling_frequency;
    uint8_t  aac_object_type;
    uint8_t  aac_channel_number;
    uint8_t  aac_vbr_supported;
    uint8_t  aac_reserved[3];
    uint32_t aac_bit_rate;

    //LDAC settings
    uint8_t ldac_sampling_frequency;
    uint8_t ldac_channel_mode;
    uint8_t ldac_reserved[6];

    //Profile link
    uint8_t a2dp_link_number;
    uint8_t hfp_link_number;
    uint8_t spp_link_number;
    uint8_t pbap_link_number;
    uint8_t avrcp_link_number;
    uint8_t hid_link_number;
    uint8_t map_link_number;
    uint8_t profile_rsv;

    //Audio 64 bytes (offset = 0x2cc)
    uint8_t audio_rsv2[21];
    uint8_t dual_audio_normal : 4;
    uint8_t dual_audio_gaming : 4;
    uint8_t audio_rsv3[15];
    uint8_t low_bat_warning_count;

    uint8_t fixed_inband_tone_gain : 1;
    uint8_t inband_tone_gain_lv : 4;
    uint8_t maximum_packet_lost_compensation_count : 3;

    uint8_t user_eq_spk_eq_num : 4;
    uint8_t user_eq_mic_eq_num : 4;

    uint8_t audio_rsv[24];

    //psensor: iqs773 8 bytes (offset = 0x30c)
    union
    {
        uint8_t iqs773_sample_rate;
        struct
        {
            uint8_t base: 2;
            uint8_t ati_target: 6;
        } iqs873_wear_sensitivity;
    } iqs_p1;

    union
    {
        uint8_t iqs773_intr_persistence;
        uint8_t iqs873_wear_trigger_level;
    } iqs_p2;

    union
    {
        uint8_t sensitivity;
        struct
        {
            uint8_t ati_target: 6;
            uint8_t base: 2;
        } iqs773_sensitivity;
        struct
        {
            uint8_t base: 2;
            uint8_t ati_target: 6;
        } iqs873_sensitivity;
    } iqs_p3;

    uint8_t iqs773_trigger_level;
    uint8_t iqs773_ulp_rate;
    uint8_t iqs773_halt_timeout;

    union
    {
        uint8_t iqs773_hall_effect_sensitivity;
        struct
        {
            uint8_t base: 2;
            uint8_t ati_target: 6;
        } iqs873_hall_sensitivity;
    } iqs_p4;

    uint8_t iqs773_hall_effect_trigger_level;

    //App option_3: 10 bytes (offset 0x314)
    uint8_t disallow_listening_mode_off_when_airplane_mode : 1;
    uint8_t exit_airplane_when_into_charger_box : 1;
    uint8_t disallow_auto_transfer_to_phone_when_out_of_ear : 1;
    uint8_t disallow_auto_power_off_when_airplane_mode : 1;
    uint8_t thermistor_power_vpa_support : 1;
    uint8_t rws_low_latency_level : 3;

    //app_option17
    uint8_t apt_on_when_enter_airplane_mode : 1;
    uint8_t app_option17_rsv_1 : 3;
    uint8_t app_option17_rsv_2: 1;
    uint8_t app_option17_rsv_3 : 1;
    uint8_t enable_apt_circular_vol_up : 1;
    uint8_t only_primary_bud_play_connected_tone : 1;

    //app_option18
    uint8_t play_in_ear_tone_regardless_of_phone_connection : 1;
    uint8_t enable_auto_a2dp_sco_control_for_android: 1;
    uint8_t line_in_plug_enter_airplane_mode : 1;
    uint8_t enable_power_supply_adp_in: 1;
    uint8_t enable_repeat_gaming_mode_led : 1;
    uint8_t disable_report_avp_id: 1;
    uint8_t enable_enter_gaming_mode_after_power_on : 1;
    uint8_t only_play_min_max_vol_once : 1;

    //app_option19
    uint8_t play_in_ear_tone_when_any_bud_in_ear : 1;
    uint8_t sd_pwr_ctrl_en: 1;
    uint8_t sd_pwr_ctrl_active_level: 1;
    uint8_t reset_eq_when_power_on : 1;
    uint8_t enable_factory_reset_whitout_limit : 1;
    uint8_t enable_align_default_volume_from_bud_to_phone: 1;
    uint8_t enter_pairing_while_only_one_device_connected: 1;
    uint8_t disallow_sync_play_vol_changed_tone_when_vol_adjust: 1;

    //app_option20
    uint8_t enable_multi_outband_call_tone: 1;
    uint8_t external_flash_support: 1;
    uint8_t listening_mode_cycle_ext: 2;
    uint8_t listening_mode_power_on_status_ext: 1;
    uint8_t maximum_linkback_number_high_bit : 1;
    uint8_t disallow_adjust_volume_when_idle : 1;
    uint8_t app_option20_rsv_7: 1;

    //app_option21
    uint8_t listening_mode_does_not_depend_on_in_ear_status: 1;
    uint8_t ext_buck_support: 1;
    uint8_t ext_buck_vendor: 3;
    uint8_t set_listening_mode_status_anc_apt_on_when_enter_airplane_mode: 1;
    uint8_t set_listening_mode_status_all_off_when_enter_airplane_mode: 1;
    uint8_t keep_listening_mode_status_when_enter_airplane_mode: 1;

    //app_option22
    uint8_t disallow_avp_advertising : 1;
    uint8_t gsensor_clock : 2;
    uint8_t disallow_listening_mode_before_phone_connected : 1;
    uint8_t auto_power_on_after_factory_reset : 1;
    uint8_t use_key_mmi_setting_on_android : 1;
    uint8_t enable_qcy_app_advertising : 1;
    uint8_t in_ear_auto_playing : 1;

    //app_option23
    uint8_t bud_to_phone_sco_tx_power_control : 4;
    uint8_t chip_supply_power_to_light_sensor_digital_VDD : 1;
    uint8_t disallow_listening_mode_on_when_gaming_mode : 1;
    uint8_t avp_display_spatial_audio : 1;
    uint8_t avp_enable_one_key_trigger : 1;

    /* click and press setting */
    uint8_t hybrid_key0_click_number : 4;
    uint8_t hybrid_key1_click_number : 4;
    uint8_t hybrid_key2_click_number : 4;
    uint8_t hybrid_key3_click_number : 4;

    struct
    {
        uint8_t call_status: 4;
        uint8_t click_type: 4;
        uint8_t mmi;
    } rws_key_mapping[8];

    uint16_t battery_capacity;

    //app_option24
    uint8_t led0_function : 4;
    uint8_t led1_function : 4;

    //app_option25
    uint8_t led2_function : 4;
    uint8_t normal_bat_led0_low_bat_led1 : 1;
    uint8_t disable_power_off_wdt_reset : 1;
    uint8_t enable_connected_led_over_role_led : 1;
    uint8_t apt_auto_on_off_while_music_playing : 1;

    //Timer2: 8 bytes (offset 0x332)
    uint8_t timer_rws_couple_led : 3;
    uint8_t time_delay_to_open_apt_when_power_on : 5;
    uint8_t amp_post_on_guard_time;
    uint8_t amp_pre_on_guard_time;
    uint8_t amp_off_guard_time;
    uint8_t timer_rsv1[4];

    //app_option26 (offset 0x33A)
    uint8_t enable_power_on_adv_with_timeout : 1;
    uint8_t output_ind3_link_mic_toggle : 1;
    uint8_t led_on_when_mic_mute : 1;
    uint8_t still_linkback_if_first_engage_fail : 1;
    uint8_t disallow_linkback_led_when_b2s_connected : 1;
    uint8_t airplane_mode_when_power_on : 1;
    uint8_t enter_shipping_mode_if_outcase_power_off : 1;
    uint8_t enable_direct_apt_on_off : 1;

    //LE audio:32 byte(offset 0x33B)
    uint8_t iso_mode: 2;
    uint8_t subgroup: 4;
    uint8_t lea_rsv_bit0: 1;
    uint8_t legacy_enable: 1;
    uint8_t bstsrc[6];
    uint8_t bis_policy: 4;
    uint8_t lea_rsv_bit1: 4;
    uint8_t cis_profile;
    uint8_t bis_mode: 2;
    uint8_t active_prio_connected_device: 4;
    uint8_t lea_rsv_bit2: 1;
    uint8_t power_off_bis_to: 1;
    uint8_t active_prio_connected_device_after_bis: 4;
    uint8_t lea_rsv_bit3: 3;
    uint8_t focus_bis_ch: 1;
    uint8_t lea_rsv0[4];
    uint16_t cis_pd: 10;
    uint16_t lea_rsv_bit4: 2;
    uint16_t upstresm_mode: 2;
    uint16_t upstresm_algo: 1;
    uint16_t lea_rsv_bit5: 1;
    uint16_t src_sample_rate;
    uint16_t scan_to;
    uint16_t bis_resync_to;
    uint16_t sink_sample_rate;
    uint8_t lea_rsv1;
    uint8_t callend_resume_media: 1;
    uint8_t lea_rsv_bit6 : 7;
    uint8_t lea_rsv2[5];

    //app_option27
    uint8_t enable_5_mins_auto_power_off_under_dut_test_mode: 1;
    uint8_t mems_pos: 3;
    uint8_t enable_ota_enc: 1;
    uint8_t console_mode: 1;
    uint8_t app_option27_rsv_1: 1;
    uint8_t le_name_sync_to_legacy_name: 1;

    //SPP UUID: 16 bytes (offset 0x35C)
    uint8_t specific_service_uuid[16];

    //offset 0x36C
    uint8_t slide_switch_0_low_action : 4;
    uint8_t slide_switch_0_high_action : 4;

    //offset 0x36D
    uint8_t slide_switch_1_low_action : 4;
    uint8_t slide_switch_1_high_action : 4;

    //app_option28 (offset 0x36E)
    uint8_t iqs773_charge_freq: 2;
    uint8_t iqs773_rsv: 6;

    uint8_t iqs773_prox_thr;

    //Cap_Touch: 1 bytes (offset 0x370)
    uint8_t ctc_rsv3 : 1;
    uint8_t ctc_light_det_pin_num : 2;
    uint8_t ctc_slide_time : 4;
    uint8_t ctc_rsv : 1;

    // pinmux: 7 bytes(offset 0x371)
    uint8_t mems_int_pinmux;
    uint8_t pinmux_rsv[6];

    //app_option29 (offset 0x378)
    uint8_t fast_pair_id : 5;
    uint8_t linkback_to_dongle_first : 1;
    uint8_t enable_ha : 1;
    uint8_t app_option29_bit_rsv : 1;

    //offset 0x379
    uint8_t spp_voice_smaple_rate : 2;
    uint8_t app_option_bit_rsv : 4;
    uint8_t enable_24g_bt_audio_source_switch : 1;
    /* enable_dongle_dual_mode && enable_multi_link must also select */
    uint8_t enable_dongle_multi_pairing : 1;

    uint8_t time_delay_to_open_apt_when_in_ear : 6;
    uint8_t apt_sample_rate : 2;
    uint8_t app_option29_rsv;

    //Cap_Touch2: 18 bytes (offset 0x37C)
#if F_APP_CAP_TOUCH_SUPPORT
    uint32_t ctc_chan_0_type : 4;
    uint32_t ctc_chan_0_default_baseline : 12;
    uint32_t ctc_chan_1_type : 4;
    uint32_t ctc_chan_1_default_baseline : 12;
    uint32_t ctc_chan_2_type : 4;
    uint32_t ctc_chan_2_default_baseline : 12;
    uint32_t ctc_chan_3_type : 4;
    uint32_t ctc_chan_3_default_baseline : 12;
    uint8_t  ctc_chan_0_mbias;
    uint8_t  ctc_chan_1_mbias;
    uint8_t  ctc_chan_2_mbias;
    uint8_t  ctc_chan_3_mbias;
    uint32_t ctc_chan_0_threshold: 12;
    uint32_t ctc_chan_1_threshold: 12;
    uint32_t ctc_chan_2_threshold_lower: 8;
    uint16_t ctc_chan_2_threshold_upper: 4;
    uint16_t ctc_chan_3_threshold: 12;
#else
    uint8_t ctc_rsv2[18];
#endif

    // NTC_PROTECT: 5 bytes (offset 0x38E)
    uint16_t high_temperature_protect_value;
    uint16_t low_temperature_protect_value;
    uint8_t  discharge_ntc_protection_thermistor_option: 1;
    uint8_t  enable_rtk_multilink: 1;
    uint8_t  app_rsv1: 6;

    //(offset 0x393)
    uint8_t supported_rtk_link_number;
    uint8_t rsv01[8];

    //LEA KEY: 16 byte (offset 0x39C)
    uint8_t lea_sirk[16];

    /* AVRCP feature for snk : 2 bytes (offset 0x3AC) */
    union
    {
        uint8_t features_value;
        struct
        {
            uint8_t category1: 1;
            uint8_t category2: 1;
            uint8_t category3: 1;
            uint8_t category4: 1;
            uint8_t rsv: 4;
        } features;
    } avrcp_features_ct;

    union
    {
        uint8_t features_value;
        struct
        {
            uint8_t category1: 1;
            uint8_t category2: 1;
            uint8_t category3: 1;
            uint8_t category4: 1;
            uint8_t rsv: 4;
        } features;
    } avrcp_features_tg;

    /* audio latency : 2 bytes (offset 0x3AE) */
    uint16_t audio_latency;

    //Tone2: 32 bytes (offset 0x3B0)
    uint8_t tone_cis_loss;
    uint8_t tone_cis_connected;
    uint8_t tone_gfps_dult_find;
    uint8_t tone_reserved[29];
} T_APP_CFG_CONST;

/** @brief  Read only configurations for app  configurations, max size:512 bytes*/
typedef struct
{
    uint32_t sync_word;//0xAA55AA55

    uint8_t dsp_gpio_num;
    uint8_t dsp_gpio_pinmux[7];
    uint8_t dsp_jtag_enable : 1;
    uint8_t dsp2_jtag_enable : 1;
    uint8_t dsp2_log_output_select : 2;
    uint8_t dsp_rsv0 : 4;
    uint8_t dsp_jtck_pinmux;
    uint8_t dsp_jtdi_pinmux;
    uint8_t dsp_jtdo_pinmux;
    uint8_t dsp_jtms_pinmux;
    uint8_t dsp_jtrst_pinmux;

    uint8_t dsp2_jtck_pinmux;
    uint8_t dsp2_jtdi_pinmux;
    uint8_t dsp2_jtdo_pinmux;
    uint8_t dsp2_jtms_pinmux;
    uint8_t dsp2_jtrst_pinmux;

    uint8_t rsv[9];

    union
    {
        uint8_t d8;
        struct
        {
            uint8_t func_en: 1;
            uint8_t speed: 2;
            uint8_t hid_class_en: 1;
            uint8_t audio_class_en: 1;
            uint8_t rsv: 3;
        };
    } usb_config;

} T_APP_CFG2_CONST;

/** @brief  Read only extend configurations for inbox audio application, max size is 512 bytes */

typedef struct
{
    uint32_t sync_word;//0xAA55AA55

    //GFPS: 108 bytes(offset 4)
    uint8_t gfps_model_id[3];

    uint8_t gfps_support : 1;
    uint8_t gfps_enable_tx_power : 1;
    uint8_t gfps_battery_info_enable : 1;
    uint8_t gfps_battery_remain_time_enable : 1;
    uint8_t gfps_battery_show_ui : 1;
    uint8_t gfps_left_ear_batetry_support : 1;
    uint8_t gfps_right_ear_batetry_support : 1;
    uint8_t gfps_case_battery_support : 1;

    int8_t  gfps_tx_power;
    uint8_t gfps_account_key_num;

    uint8_t gfps_sass_support : 1;
    uint8_t gfps_sass_support_dyamic_multilink_switch : 1;
    uint8_t gfps_sass_rsv : 5;
    uint8_t gfps_finder_support : 1;

    uint8_t gfps_le_device_support : 1;
    uint8_t gfps_le_disconn_force_enter_pairing_mode: 1;
    uint8_t gfps_le_device_mode: 2;
    uint8_t disable_finder_adv_when_power_off : 1;
    uint8_t gfps_rsv : 3;

    uint16_t gfps_discov_adv_interval;
    uint16_t gfps_not_discov_adv_interval;
    uint8_t  gfps_public_key[64];
    uint8_t  gfps_private_key[32];

    //20 bytes(offset 112)
    uint8_t ama_support : 1;
    uint8_t xiaoai_support : 1;
    uint8_t bisto_support : 1;
    uint8_t xiaowei_support : 1;
    uint8_t dma_support : 1;
    uint8_t teams_support : 1;
    uint8_t tuya_support : 1;
    uint8_t tile_support : 1;

    uint8_t xiaoai_tranport;
    uint8_t bisto_tranport;
    uint8_t ama_tranport;

    uint8_t ama_id[14];
    uint16_t multi_adv_interval;

    //reserved for other purpose
    uint16_t xiaoai_vid;
    uint16_t xiaoai_pid;
    uint16_t xiaoai_adv_timeout_val;
    uint16_t ama_adv_timeout_val;
    uint8_t xiaoai_major_id;
    uint8_t xiaoai_minor_id;

    //xiaowei
    uint16_t xiaowei_adv_timeout;
    uint32_t xiaowei_product_id;
    uint8_t xiaowei_model_name[64];
    uint8_t xiaowei_tranport;

    uint8_t ama_mcu_ringtone_enable : 1;
    uint8_t ama_ext_option_rsv : 7;

    uint8_t tile_device_name : 4;
    uint8_t tile_rsv : 4;

    uint16_t tuya_adv_timeout;

    uint8_t gfps_company_name[64];
    uint8_t gfps_device_name[64];
    uint8_t gfps_device_type;
    uint8_t gfps_version[10];
    //gfps finder adv interval in power on state, unit 0.625ms, range (32,3200), default value 800, time = 800*0.625 = 500ms;
    uint32_t gfps_power_on_finder_adv_interval;
    //gfps finder adv interval in power off state, unit 0.625ms, range (32,3200), default value 1600, time = 1600*0.625 = 1000ms;
    uint32_t gfps_power_off_finder_adv_interval;
    //timeout value to start timer for finder adv in power off state, unit 1s, Range(60,3600), default value 600s
    uint16_t gfps_power_off_start_finder_adv_timer_timeout_value;
    //finder adv duration in power off state, unit 1s, range(10s,600s), default value 10s
    uint16_t gfps_power_off_finder_adv_duration;

    // unit 1s, Range(0,3600),   0: Disable
    uint16_t power_off_rtc_wakeup_timeout;
    // Range(0, 100)
    uint16_t gfps_finder_adv_skip_count_when_wakeup;
} T_EXTEND_APP_CFG_CONST;

/** End of APP_CFG_Exported_Types
    * @}
    */

extern T_APP_CFG_CONST app_cfg_const;
extern T_APP_CFG2_CONST app_cfg2_const;
extern T_EXTEND_APP_CFG_CONST extend_app_cfg_const;
extern T_APP_CFG_NV app_cfg_nv;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_CFG_Exported_Functions App Cfg Functions
    * @{
    */
/**
    * @brief  store config parameters to ftl
    * @param  void
    * @return ftl result
    */
uint32_t app_cfg_store_all(void);

/**
    * @brief    Save FTL value defined in @ref T_APP_CFG_NV
    * @note
    * @param    pdata  specify data buffer
    * @param    size   size to store
    * @return   status
    * @return   0  status successful
    * @return   otherwise fail
    */
uint32_t app_cfg_store(void *pdata, uint16_t size);

/**
    * @brief  reset config parameters
    * @param  void
    * @return ftl result
    */
uint32_t app_cfg_reset(void);

/**
    * @brief  config module init
    * @param  void
    * @return void
    */
void app_cfg_init(void);

/**
  * @brief  load led table from config.
  * @param  p_data  led table buffer
  * @param  mode    led mode
  * @param  led_table_size led table size for each led mode table
  * @return true  load cfg success.
  * @return false  load cfg fail.
  */
bool app_cfg_load_led_table(void *p_data, uint8_t mode, uint16_t led_table_size);

/** @} */ /* End of group APP_CFG_Exported_Functions */

/** End of APP_CFG
* @}
*/


#ifdef __cplusplus
} /* __cplusplus */
#endif

#endif /*_AU_CFG_H_*/
