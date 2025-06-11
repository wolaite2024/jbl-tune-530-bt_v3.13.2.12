
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include "os_mem.h"
#include "os_sched.h"
#include "os_msg.h"
#include "os_task.h"
#include "os_ext.h"
#include "system_status_api.h"
#include "rtl876x_pinmux.h"
#include "patch_header_check.h"
#include "img_ctrl_ext.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "dlps_util.h"
#include "trace.h"
#include "audio.h"
#include "audio_probe.h"
#include "sysm.h"
#include "pm.h"
#include "gap_br.h"
#include "gap.h"
#include "test_mode.h"
#include "single_tone.h"
#include "engage.h"
#include "fmc_api.h"
#include "spp_stream.h"
#include "ble_stream.h"
#include "iap_stream.h"
#include "app_console.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "app_gap.h"
#include "app_main.h"
#include "app_led.h"
#include "app_io_msg.h"
#include "app_ble_gap.h"
#include "app_ble_client.h"
#include "app_ble_service.h"
#include "app_key_process.h"
#include "app_dlps.h"
#include "app_key_gpio.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_sdp.h"
#include "app_transfer.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "app_avrcp.h"
#include "app_iap.h"
#include "app_spp.h"
#include "app_cmd.h"
#include "app_bud_loc.h"
#include "app_ble_bond.h"
#include "app_multilink.h"
#include "app_ble_device.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "app_in_out_box.h"
#include "app_mmi.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif
#include "app_tts.h"
#include "app_third_party_srv.h"
#include "app_bond.h"
#include "app_adp.h"
#include "app_adp_cmd.h"
#include "app_io_output.h"
#include "app_amp.h"

#if F_APP_LOG2FLASH_SUPPORT
#include "log_api.h"
#include "task_log_api.h"
#endif

#if F_APP_ADC_SUPPORT
#include "app_adc.h"
#endif

#if (F_APP_PERIODIC_WAKEUP == 1)
#include "rtl876x_rtc.h"
#endif

#if F_APP_ERWS_SUPPORT
#include "app_relay.h"
#include "app_roleswap.h"
#include "app_rdtp.h"
#endif

#if F_APP_BT_PROFILE_PBAP_SUPPORT
#include "app_pbap.h"
#endif

#if F_APP_HFP_AG_SUPPORT
#include "app_hfp_ag.h"
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#include "app_io_output.h"
#endif

#if F_APP_TEST_SUPPORT
#include "app_test.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_io_output.h"
#endif

#if F_APP_HID_SUPPORT
#include "app_hid.h"
#endif

#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif

#if BISTO_FEATURE_SUPPORT
#include "app_bisto_bt.h"
#endif

#if F_APP_CFU_SUPPORT
#include "app_cfu.h"
#endif
#if F_APP_CFU_FEATURE_SUPPORT
#include "app_common_cfu.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
#include "app_usb.h"
#endif

#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
#include "app_rtk_fast_pair_adv.h"
#endif


#if F_APP_LEA_SUPPORT
#include "app_lea_profile.h"
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "app_ble_swift_pair.h"
#endif

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif

#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif

#if F_APP_QDECODE_SUPPORT
#include "app_qdec.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_RWS_MULTI_SPK_SUPPORT
#include "msbc_sync.h"
#endif

#if F_APP_IAP_RTK_SUPPORT
#include "app_iap_rtk.h"
#endif

#if F_APP_BOND_MGR_SUPPORT
#include "bt_bond_api.h"
#if F_APP_LEA_SUPPORT
#include "ble_audio_bond.h"
#endif
#endif

#if F_APP_BOND_MGR_BLE_SYNC
#include "ble_bond_sync.h"
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_1
#include "app_spp_capture_dsp_data.h"
#elif F_APP_SPP_CAPTURE_DSP_DATA_2
#include "app_data_capture.h"
#endif

#if F_APP_POWER_TEST
#include "power_debug.h"
#include "hal_debug.h"
#endif

#if F_APP_AUTO_POWER_TEST_LOG
#include "app_power_test.h"
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "app_findmy_task.h"
#endif

#if F_APP_USB_SUSPEND_SUPPORT
#include "app_usb_suspend.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_behaviors.h"
#include "app_harman_vendor_cmd.h"
#include "app_harman_license.h"
#include "app_ota.h"
#include "fmc_api.h"
#include "flash_map.h"
#include "version.h"
#endif

#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
#include "app_ext_charger.h"
#endif

#if HARMAN_SECURITY_CHECK
#include "hm_security.h"
#endif

#if HARMAN_NTC_DETECT_PROTECT
#include "app_harman_adc.h"
#endif

#include "app_bt_point.h"

#define MAX_NUMBER_OF_GAP_MESSAGE       0x20    //!< indicate BT stack message queue size
#define MAX_NUMBER_OF_IO_MESSAGE        0x20    //!< indicate io queue size
#define MAX_NUMBER_OF_DSP_MSG           0x20    //!< number of dsp message reserved for DSP message handling.
#define MAX_NUMBER_OF_CODEC_MSG         0x10    //!< number of codec message reserved for CODEC message handling.
#define MAX_NUMBER_OF_ANC_MSG           0x10    //!< number of anc message reserved for ANC message handling.
#define MAX_NUMBER_OF_SYS_MSG           0x20    //!< indicate SYS timer queue size
#define MAX_NUMBER_OF_LOADER_MSG        0x20    //!< indicate Bin Loader queue size
#define MAX_NUMBER_OF_APP_TIMER_MODULE  0x30    //!< indicate app timer module size
/** indicate rx event queue size*/
#define MAX_NUMBER_OF_RX_EVENT \
    (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE + MAX_NUMBER_OF_APP_TIMER_MODULE + \
     MAX_NUMBER_OF_DSP_MSG + MAX_NUMBER_OF_CODEC_MSG + MAX_NUMBER_OF_ANC_MSG + \
     MAX_NUMBER_OF_SYS_MSG + MAX_NUMBER_OF_LOADER_MSG)

#define DEFAULT_PAGESCAN_WINDOW         0x12
#define DEFAULT_PAGESCAN_INTERVAL       0x800
#define DEFAULT_PAGE_TIMEOUT            0x2000
#define DEFAULT_SUPVISIONTIMEOUT        0x1f40
#define DEFAULT_INQUIRYSCAN_WINDOW      0x12
#define DEFAULT_INQUIRYSCAN_INTERVAL    0x1000

typedef union t_release_ver
{
    uint64_t  release_ver;
    struct __attribute__((packed))
    {
        uint8_t revision;
        uint16_t tool_ver;
        uint8_t chip_ver;
        uint16_t crb_ver;
        uint8_t rsv[2];
    };
} T_RELEASE_VER;

void *audio_evt_queue_handle;
void *audio_io_queue_handle;

T_APP_DB app_db;

#if HARMAN_SECURITY_CHECK

#define HARMAN_SECURITY_DEBUG       0
#define HARMAN_PUBLICK_KEY_SIZE     260

#if HARMAN_SECURITY_DEBUG  // for test
uint8_t harman_public_key[HARMAN_PUBLICK_KEY_SIZE] =
// test chipID: [0x43, 0x2B, 0x15, 0xA4, 0x0F, 0xF4, 0xED, 0xAE, 0xA6, 0xFB, 0x48, 0xE3, 0x8D, 0x58, 0x51, 0xAC]
{
    0xdf, 0xd0, 0x30, 0xa5, 0x64, 0xbf, 0xf3, 0x1c, 0x5d, 0xab, 0x4f, 0x85, 0xd2, 0xdc, 0x48, 0x29, 0x08, 0x3f, 0x12, 0x21, 0x5d, 0xf6, 0x0c, 0x06, 0x39, 0x86, 0xaf, 0xca, 0x35, 0x48, 0x2e, 0xac,
    0xde, 0x12, 0xab, 0xa6, 0xb3, 0xe1, 0xf6, 0xd4, 0xa9, 0x66, 0xcd, 0x32, 0xd8, 0x85, 0xd1, 0xbc, 0x76, 0xe0, 0x67, 0x7d, 0x41, 0xfb, 0xd4, 0x0f, 0x65, 0xda, 0x8e, 0xdb, 0x66, 0xb0, 0xa0, 0x1a,
    0x59, 0xab, 0x39, 0x91, 0xf4, 0x33, 0x3f, 0x2d, 0x2d, 0x32, 0x16, 0x70, 0x86, 0xa7, 0x47, 0x80, 0xf3, 0xdf, 0x93, 0x80, 0x8e, 0xc4, 0x4e, 0xb3, 0xa7, 0x39, 0x40, 0x26, 0xba, 0x2a, 0xca, 0xac,
    0xaf, 0x20, 0x5b, 0xba, 0xf1, 0x3a, 0x67, 0x9b, 0xe3, 0x2d, 0x0f, 0xb3, 0x3d, 0xb2, 0xa8, 0x85, 0x36, 0xa2, 0xd2, 0x9a, 0x0f, 0xc2, 0x8f, 0x53, 0xeb, 0xcb, 0xa6, 0xa0, 0x01, 0x81, 0xba, 0x2b,
    0xcb, 0x47, 0xe8, 0xea, 0x0d, 0x27, 0x5e, 0x7f, 0x47, 0x61, 0xbc, 0xa1, 0x7f, 0x96, 0x27, 0xa4, 0xa2, 0xe2, 0x89, 0xd7, 0xcf, 0x29, 0xe3, 0x88, 0x73, 0x27, 0x6b, 0x01, 0x86, 0xd9, 0x03, 0x65,
    0x49, 0x39, 0xcf, 0x65, 0x6e, 0xe0, 0x46, 0x3b, 0xeb, 0x1a, 0xb7, 0x02, 0x79, 0xe9, 0x0b, 0x61, 0xd9, 0x67, 0xc4, 0x23, 0x9b, 0x20, 0xc9, 0xca, 0x13, 0x3d, 0x61, 0x11, 0xae, 0x50, 0x08, 0x2e,
    0x07, 0x6d, 0x77, 0xd6, 0x16, 0xac, 0x85, 0xd4, 0x59, 0xbe, 0x3d, 0x12, 0x93, 0x2e, 0x63, 0x63, 0xb9, 0x13, 0x75, 0x79, 0x98, 0xea, 0x8a, 0x3e, 0xb2, 0xee, 0x7e, 0x6f, 0x82, 0xb8, 0x41, 0x38,
    0xb7, 0xac, 0x15, 0xf1, 0x81, 0x32, 0x2a, 0x6e, 0xfc, 0xcf, 0x48, 0x0b, 0xfc, 0x8e, 0x28, 0xe0, 0xf3, 0xbb, 0x4d, 0xa6, 0xea, 0xad, 0xdd, 0xc7, 0x3f, 0x26, 0xcc, 0x0b, 0x19, 0x81, 0x0e, 0xcf,
    0x01, 0x0, 0x01, 0x0
};
#else // HARMAN_SECURITY_DEBUG

// HARMAN_T530_SUPPORT
uint8_t harman_public_key[HARMAN_PUBLICK_KEY_SIZE] =
{
    0x1B, 0xB6, 0xF1, 0xE7, 0xA7, 0x3C, 0x08, 0xC4, 0x01, 0xBA, 0x2E, 0x09, 0xDC, 0xD0, 0xF6, 0x09,
    0xBA, 0x4A, 0xAE, 0x7D, 0xA8, 0x8E, 0xBF, 0x92, 0x3F, 0x90, 0xD7, 0x4B, 0xA1, 0x14, 0xBB, 0xFD,
    0x80, 0x6B, 0x4B, 0x3F, 0xC0, 0xB1, 0x86, 0xDD, 0xD5, 0x31, 0x82, 0xD0, 0xEE, 0xC5, 0xCB, 0x69,
    0x67, 0x7A, 0x68, 0x43, 0xA3, 0x23, 0x74, 0x98, 0x01, 0x75, 0x13, 0x64, 0x40, 0xAC, 0x44, 0x93,
    0x03, 0x00, 0x55, 0xDD, 0xDD, 0x0E, 0xE5, 0xD0, 0x6B, 0xE7, 0xC8, 0xF3, 0xDD, 0x3F, 0xC1, 0xF6,
    0xDA, 0x8E, 0xB0, 0x59, 0x08, 0x4C, 0x94, 0x28, 0xA4, 0xFD, 0xBD, 0x05, 0xE3, 0xC0, 0x60, 0xE3,
    0x17, 0xDE, 0x20, 0x6D, 0xAE, 0xE3, 0x67, 0x7C, 0xCA, 0x3E, 0x46, 0x6F, 0xDD, 0xEC, 0xA6, 0x1C,
    0x9C, 0x69, 0x04, 0x84, 0xEF, 0xEA, 0x30, 0x5A, 0x85, 0xB1, 0x3E, 0xE7, 0xC7, 0xA3, 0x4F, 0x60,
    0x35, 0xD0, 0xB4, 0x11, 0x42, 0xD0, 0x64, 0x7A, 0x5B, 0x9A, 0x0D, 0x08, 0xD9, 0xD4, 0x1A, 0x67,
    0x6B, 0xF9, 0x02, 0x0B, 0xEC, 0x46, 0x0A, 0x80, 0x0D, 0xAD, 0xB3, 0x79, 0x67, 0x1B, 0xEF, 0x63,
    0x9D, 0x52, 0x29, 0x72, 0x1F, 0x96, 0x0F, 0x03, 0x34, 0xED, 0xE7, 0xEB, 0xAB, 0xE3, 0x17, 0x63,
    0x12, 0x62, 0x83, 0xC7, 0x30, 0x88, 0x13, 0x8C, 0x7D, 0x91, 0xD6, 0xEA, 0x36, 0x94, 0xF7, 0x52,
    0xF0, 0xD0, 0x87, 0xB7, 0xAB, 0xDF, 0xB6, 0x3C, 0xFA, 0x11, 0x87, 0xE0, 0xD9, 0x4E, 0x01, 0x22,
    0xD1, 0x5D, 0x02, 0x90, 0xD8, 0xC1, 0x80, 0xA1, 0xF3, 0x11, 0x1A, 0x0B, 0x9F, 0x65, 0xD2, 0x47,
    0x90, 0xF2, 0x60, 0x37, 0xA0, 0xCC, 0x0F, 0x9D, 0x18, 0x2F, 0xCF, 0xF1, 0x35, 0x2F, 0x34, 0xC1,
    0x90, 0x6B, 0xA7, 0xF9, 0xEB, 0xE1, 0x3F, 0xA1, 0x16, 0x2A, 0x8B, 0x5E, 0x1E, 0xA3, 0x28, 0xF4,
    0x01, 0x00, 0x01, 0x00
};
// HARMAN_T530_SUPPORT

#endif // HARMAN_SECURITY_DEBUG

int32_t harmain_public_key_read(uint8_t *public_key_buf, uint16_t buf_size)
{
    if (public_key_buf == NULL || buf_size < HARMAN_PUBLICK_KEY_SIZE)
    {
        return 0;
    }
    else
    {
        memcpy(public_key_buf, harman_public_key, HARMAN_PUBLICK_KEY_SIZE);
        return HARMAN_PUBLICK_KEY_SIZE;
    }
}
#endif // HARMAN_SECURITY_CHECK


/**
* @brief board_init() contains the initialization of pinmux settings and pad settings.
*
*   All the pinmux settings and pad settings shall be initiated in this function.
*   But if legacy driver is used, the initialization of pinmux setting and pad setting
*   should be peformed with the IO initializing.
*
* @return void
*/
static void board_init(void)
{
    PAD_Pull_Mode pull_mode;

#if (F_APP_PERIODIC_WAKEUP == 1)
    app_dlps_system_wakeup_clear_rtc_int();
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
    app_harman_set_cpu_clk(true);
    app_harman_cpu_clk_improve();
#endif


    if (app_cfg_const.enable_data_uart)
    {
        //when select 4pogo download and uart tx shares the same pin with 3pin gpio, don't config uart tx pinmux in here
        if ((app_cfg_const.enable_4pogo_pin == 0) ||
            (app_cfg_const.gpio_box_detect_pinmux != app_cfg_const.data_uart_tx_pinmux))
        {
            Pinmux_Config(app_cfg_const.data_uart_tx_pinmux, UART0_TX);
            Pad_Config(app_cfg_const.data_uart_tx_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        Pinmux_Config(app_cfg_const.data_uart_rx_pinmux, UART0_RX);
        Pad_Config(app_cfg_const.data_uart_rx_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        if (app_cfg_const.enable_rx_wake_up)
        {
            Pinmux_Config(app_cfg_const.rx_wake_up_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.rx_wake_up_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        if (app_cfg_const.enable_tx_wake_up)
        {
            Pinmux_Config(app_cfg_const.tx_wake_up_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.tx_wake_up_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        }
    }

#if F_APP_ONE_WIRE_UART_SUPPORT
    if (app_cfg_const.one_wire_uart_support)
    {
        if (app_cfg_const.one_wire_uart_gpio_support)
        {
            Pad_Config(app_cfg_const.one_wire_uart_gpio_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, ONE_WIRE_GPIO_5V_PULL, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_PullConfigValue(app_cfg_const.one_wire_uart_gpio_pinmux, PAD_STRONG_PULL);
        }

        Pinmux_Config(app_cfg_const.one_wire_uart_data_pinmux, UART0_TX);
        Pad_Config(app_cfg_const.one_wire_uart_data_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
#endif

    if (app_cfg_const.dsp_log_output_select == DSP_OUTPUT_LOG_BY_UART)
    {
        Pinmux_Config(app_cfg_const.dsp_log_pin, UART2_TX);
        Pad_Config(app_cfg_const.dsp_log_pin,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }

#if 0
    //Config I2C0 pinmux. For CP control
    if ((app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK) ||
        (app_cfg_const.gsensor_support) ||
#if F_APP_SENSOR_MEMS_SUPPORT
        (app_cfg_const.mems_support) ||
#endif
        (app_cfg_const.psensor_support && (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)) ||
        (app_cfg_const.psensor_support && (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_SC7A20)) ||
        (app_cfg_const.ext_buck_support))
    {
        Pinmux_Config(app_cfg_const.i2c_0_dat_pinmux, I2C0_DAT);
        Pinmux_Config(app_cfg_const.i2c_0_clk_pinmux, I2C0_CLK);

        if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
        {
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
        else
        {
#if (TARGET_RTL8773DO || TARGET_RTL8773DFL )
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
#else
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
            Pad_PullConfigValue(app_cfg_const.i2c_0_dat_pinmux, PAD_STRONG_PULL);
            Pad_PullConfigValue(app_cfg_const.i2c_0_clk_pinmux, PAD_STRONG_PULL);
        }
    }

#if F_APP_SENSOR_MEMS_SUPPORT
    if (app_cfg_const.mems_support)
    {
        app_sensor_mems_cfg_pad();
    }
#endif

    //Config sensor pinmux
    if (app_cfg_const.sensor_support)
    {
        if (0)
        {
            /* for feature define; do nothing */
        }
#if F_APP_SENSOR_HX3001_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_HX)
        {
            Pad_SetPinDrivingCurrent(app_cfg_const.sensor_detect_pinmux, LEVEL3);
        }
#endif
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225 ||
                 app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
        {
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        }
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_IO)
        {
            pull_mode = (app_cfg_const.iosensor_active) ? PAD_PULL_DOWN : PAD_PULL_UP;
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, pull_mode, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)
        {
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        }
    }

    //Config gsensor pinmux
    if (app_cfg_const.gsensor_support || app_cfg_const.psensor_support)
    {
        //INT pad config
        Pinmux_Config(app_cfg_const.gsensor_int_pinmux, DWGPIO);
        Pad_Config(app_cfg_const.gsensor_int_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    }

    //Config output 10hz pinmux
    if (app_cfg_const.enable_external_mcu_reset && (app_cfg_const.external_mcu_input_pinmux != 0xFF))
    {
        Pad_Config(app_cfg_const.external_mcu_input_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

    //Config amp pinmux
    if (app_cfg_const.enable_ctrl_ext_amp)
    {
        pull_mode = (app_cfg_const.enable_ext_amp_high_active) ? PAD_PULL_DOWN : PAD_PULL_UP;
        Pinmux_Config(app_cfg_const.ext_amp_pinmux, DWGPIO);
        Pad_Config(app_cfg_const.ext_amp_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, pull_mode, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }

    //Config thermistor power pinmux
    if (app_cfg_const.thermistor_power_gpio_support && (app_cfg_const.thermistor_power_pinmux != 0xFF))
    {
        Pad_Config(app_cfg_const.thermistor_power_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

    //Config pcba shipping mode pinmux
    if (app_cfg_const.pcba_shipping_mode_pinmux != 0xFF)
    {
        Pad_Config(app_cfg_const.pcba_shipping_mode_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

#if F_APP_IO_OUTPUT_SUPPORT
    //Config out indication pinmux
    app_io_output_out_indication_pinmux_init();
#endif

#if F_APP_QDECODE_SUPPORT
    if (app_cfg_const.wheel_support)
    {
        app_qdec_pad_config();
    }
#endif

#if F_APP_GPIO_MICBIAS_SUPPORT
    //For no micbias pin IC , do micbias control gpio pad init in app
    if (app_cfg_const.micbias_pinmux != 0xFF && app_cfg_const.micbias_pinmux != MICBIAS)
    {
        Pad_Config(app_cfg_const.micbias_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
#endif

    if (app_cfg2_const.dsp_gpio_num)
    {
        for (uint8_t i = 0; i < app_cfg2_const.dsp_gpio_num; i++)
        {
            Pinmux_Config(app_cfg2_const.dsp_gpio_pinmux[i], DSP_GPIO_OUT);
            Pad_Config(app_cfg2_const.dsp_gpio_pinmux[i],
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
    }

    if (app_cfg2_const.dsp_jtag_enable)
    {
        Pinmux_Config(app_cfg2_const.dsp_jtck_pinmux, DSP_JTCK);
        Pad_Config(app_cfg2_const.dsp_jtck_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtdi_pinmux, DSP_JTDI);
        Pad_Config(app_cfg2_const.dsp_jtdi_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtdo_pinmux, DSP_JTDO);
        Pad_Config(app_cfg2_const.dsp_jtdo_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtms_pinmux, DSP_JTMS);
        Pad_Config(app_cfg2_const.dsp_jtms_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtrst_pinmux, DSP_JTRST);
        Pad_Config(app_cfg2_const.dsp_jtrst_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
#endif
}

#if F_APP_EXT_FLASH_SUPPORT
static bool ext_flash_init(void)
{
    return fmc_flash_nor_init(FMC_FLASH_NOR_IDX3);
}
#endif

static void driver_init(void)
{
    hal_gpio_init();
    hal_gpio_int_init();
    hal_gpio_set_debounce_time(30);
    app_adp_init();

    if (app_cfg_const.key_gpio_support)
    {
        key_gpio_initial();
        app_key_init();

#if 0
        if (app_cfg_const.enable_hall_switch_function)
        {
            // Check if there are hall switch definitions
            app_key_hybird_parse_hall_switch_setting();

            // Read the hall switch status
            app_db.hall_switch_status = app_key_read_hall_switch_status();
        }
#endif
    }

    if (app_cfg_const.mfb_replace_key0)
    {
        key_gpio_mfb_init();
    }

#if F_APP_IAP_SUPPORT
    if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
    {
        app_iap_cp_hw_init(I2C0);
    }
#endif

#if F_APP_CONSOLE_SUPPORT
    if (app_cfg_const.enable_data_uart || app_cfg_const.one_wire_uart_support)
    {
        app_console_init();
    }
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
    //app_case_detect_gpio_driver_init();
    if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        app_gpio_on_off_driver_init(app_cfg_const.gpio_box_detect_pinmux);
    }
#endif

#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
    if (app_cfg_const.charger_support)
    {
        app_ext_charger_init();
    }
#endif

    if (app_cfg_const.charger_support || app_cfg_const.discharger_support)
    {
        app_charger_init();
    }

    if (app_cfg_const.led_support)
    {
        app_led_init();
#if F_APP_TEAMS_FEATURE_SUPPORT
        teams_extend_led_init();
        teams_extend_led_breath_init();
#endif
    }

#if F_APP_USB_AUDIO_SUPPORT
    if (app_cfg_const.usb_audio_support)
    {
        //app_usb_audio_init();
    }
#endif

#if F_APP_NFC_SUPPORT
    if (app_cfg_const.nfc_support)
    {
        app_nfc_init();
    }
#endif

#if F_APP_LINEIN_SUPPORT
    if (app_cfg_const.line_in_support)
    {
        app_line_in_driver_init();
        app_dlps_pad_wake_up_polarity_invert(app_cfg_const.line_in_pinmux);
    }
#endif

#if F_APP_AMP_SUPPORT
    if (app_cfg_const.enable_ctrl_ext_amp)
    {
        app_amp_init();
    }
#endif

#if F_APP_SENSOR_SUPPORT
    if (app_cfg_const.sensor_support)
    {
        if (0)
        {
            /* for feature define; do nothing */
        }
#if F_APP_SENSOR_HX3001_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_HX)
        {
            app_sensor_ld_init();
        }
#endif
#if F_APP_SENSOR_JSA1225_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init JSA1225 after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(SENSOR_ADDR_JSA, DEF_I2C_CLK_SPD, false);
                app_sensor_jsa1225_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux, app_sensor_jsa_int_gpio_intr_cb);
            }
        }
#endif
#if F_APP_SENSOR_JSA1227_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init JSA1227 after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(SENSOR_ADDR_JSA1227, JSA1227_I2C_CLK_SPD, false);
                app_sensor_jsa1227_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux, app_sensor_jsa_int_gpio_intr_cb);
            }
        }
#endif
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_IO)
        {
            app_sensor_ld_io_init();
            app_dlps_pad_wake_up_polarity_invert(app_cfg_const.sensor_detect_pinmux);
        }
#if F_APP_SENSOR_PX318J_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init PX318J after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(PX318J_ID, PX318J_I2C_CLK_SPD, false);
                app_sensor_px318j_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux,
                                         app_sensor_px318j_int_gpio_intr_cb);
            }
        }
#endif
#if F_APP_SENSOR_SC7A20_AS_LS_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_SC7A20)
        {
            app_sensor_i2c_init(GSENSOR_I2C_SLAVE_ADDR_SILAN, DEF_I2C_CLK_SPD, false);
            app_sensor_sc7a20_as_ls_init();
        }
#endif
    }

#if F_APP_SENSOR_SL7A20_SUPPORT
    if (app_cfg_const.gsensor_support)
    {
        //P sensor and G sensor will not coexist
        //Re-use gsensor INT pinmux
        app_sensor_gsensor_init();
        app_dlps_pad_wake_up_polarity_invert(app_cfg_const.gsensor_int_pinmux);
    }
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
    if (app_cfg_const.mems_support)
    {
        app_sensor_mems_init();
    }
#endif

    if (app_cfg_const.psensor_support)
    {
        //P sensor and G sensor will not coexist
        //Re-use gsensor INT pinmux

        if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IO)
        {
            app_sensor_psensor_init();
            app_dlps_pad_wake_up_polarity_invert(app_cfg_const.gsensor_int_pinmux);
        }
#if F_APP_SENSOR_IQS773_873_SUPPORT
        else if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)
        {
            app_sensor_i2c_init(I2C_AZQ_ADDR, DEF_I2C_CLK_SPD, false);
            if (i2c_iqs_check_device() != IQS773_MODULE)
            {
                APP_PRINT_ERROR0("No IQS773 module detected");
            }
            else
            {
                i2c_iqs773_setup();
                app_sensor_int_gpio_init(app_cfg_const.gsensor_int_pinmux, app_psensor_iqs773_873_intr_cb);
                app_dlps_pad_wake_up_polarity_invert(app_cfg_const.gsensor_int_pinmux);
                i2c_iqs_initial();
            }
        }
        else if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)
        {
            app_sensor_i2c_init(I2C_AZQ_ADDR, DEF_I2C_CLK_SPD, false);
            if (i2c_iqs_check_device() != IQS873_MODULE)
            {
                APP_PRINT_ERROR0("No IQS873 module detected");
            }
            else
            {
                i2c_iqs873_setup();
                app_sensor_int_gpio_init(app_cfg_const.gsensor_int_pinmux, app_psensor_iqs773_873_intr_cb);
                app_dlps_pad_wake_up_polarity_invert(app_cfg_const.gsensor_int_pinmux);
                i2c_iqs_initial();
            }
        }
#endif
    }

    if ((app_cfg_const.sensor_support) || (app_cfg_const.gsensor_support) ||
        (app_cfg_const.psensor_support))
    {
        app_sensor_init();
    }
#endif

#if F_APP_CAP_TOUCH_SUPPORT
    if (CapTouch_IsSystemEnable())
    {
        app_cap_touch_driver_init(true);
    }
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
    app_slide_switch_driver_init();
#endif

#if F_APP_QDECODE_SUPPORT
    if (app_cfg_const.wheel_support)
    {
        app_qdec_init_status_read();
        app_qdec_driver_init();
    }
#endif

#if F_APP_HIFI4_SUPPORT
    if (app_cfg_const.ext_buck_support)
    {
        app_buck_tps62860_init();
    }
#endif

#if F_APP_ALC1017_SUPPORT
    app_alc1017_init();
#endif

#if F_APP_EXT_FLASH_SUPPORT
    if (app_cfg_const.external_flash_support)
    {
        ext_flash_init();
    }
#endif
}

static void app_bt_gap_init(void)
{
    uint32_t class_of_device = (uint32_t)((app_cfg_const.class_of_device[0]) |
                                          (app_cfg_const.class_of_device[1] << 8) |
                                          (app_cfg_const.class_of_device[2] << 16));

    uint16_t supervision_timeout = DEFAULT_SUPVISIONTIMEOUT;
    uint16_t link_policy = GAP_LINK_POLICY_ROLE_SWITCH | GAP_LINK_POLICY_SNIFF_MODE;
    uint8_t radio_mode = GAP_RADIO_MODE_NONE_DISCOVERABLE;
    bool limited_discoverable = false;
    bool auto_accept_acl = false;

    uint8_t  pagescan_type = GAP_PAGE_SCAN_TYPE_INTERLACED;
    uint16_t pagescan_interval = DEFAULT_PAGESCAN_INTERVAL;
    uint16_t pagescan_window = DEFAULT_PAGESCAN_WINDOW;
    uint16_t page_timeout = DEFAULT_PAGE_TIMEOUT;
    uint8_t  page_scan_repetition_mode = GAP_PAGE_SCAN_REPETITION_R1;

    uint8_t inquiryscan_type = GAP_INQUIRY_SCAN_TYPE_INTERLACED;
    uint16_t inquiryscan_window = DEFAULT_INQUIRYSCAN_WINDOW;
    uint16_t inquiryscan_interval = DEFAULT_INQUIRYSCAN_INTERVAL;
    uint8_t inquiry_mode = GAP_INQUIRY_MODE_EXTENDED_RESULT;

    uint8_t pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint8_t io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oob_enable = false;
    uint8_t bt_mode = GAP_BT_MODE_21ENABLED;

    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG  |
                          GAP_AUTHEN_BIT_SC_FLAG |
                          GAP_AUTHEN_BIT_SC_BR_FLAG;

#if F_APP_BOND_MGR_SUPPORT
    bt_bond_init();
#if F_APP_LEA_SUPPORT
    ble_audio_bond_init();
#endif
#endif

    if ((app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
        (app_cfg_nv.factory_reset_done == 0))
    {
        /* change the scan interval to 100ms(0xA0 * 0.625) before factory reset */
        pagescan_interval = 0xA0;
        inquiryscan_interval = 0xA0;
    }
#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        pagescan_interval = SASS_PAGESCAN_INTERVAL;
        pagescan_window = SASS_PAGESCAN_WINDOW;
    }
#endif

    gap_lib_init();

    //0: to be master
    gap_br_cfg_accept_role(1);

    gap_br_set_param(GAP_BR_PARAM_NAME, GAP_DEVICE_NAME_LEN, app_cfg_nv.device_name_legacy);

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_bt_set_name();
    }
#endif

    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(uint8_t), &pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);

    gap_br_set_param(GAP_BR_PARAM_BT_MODE, sizeof(uint8_t), &bt_mode);

#if F_APP_LEA_SUPPORT
    if (app_cfg_nv.lea_mode_enable == 0)
    {
        class_of_device &= ~(BIT14); // lea bit
        extend_app_cfg_const.gfps_le_device_support = 0;
    }

#endif

    gap_br_set_param(GAP_BR_PARAM_COD, sizeof(uint32_t), &class_of_device);
    gap_br_set_param(GAP_BR_PARAM_LINK_POLICY, sizeof(uint16_t), &link_policy);
    gap_br_set_param(GAP_BR_PARAM_SUPV_TOUT, sizeof(uint16_t), &supervision_timeout);
    gap_br_set_param(GAP_BR_PARAM_AUTO_ACCEPT_ACL, sizeof(bool), &auto_accept_acl);

    gap_br_set_param(GAP_BR_PARAM_RADIO_MODE, sizeof(uint8_t), &radio_mode);
    gap_br_set_param(GAP_BR_PARAM_LIMIT_DISCOV, sizeof(bool), &limited_discoverable);

    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_TYPE, sizeof(uint8_t), &pagescan_type);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_INTERVAL, sizeof(uint16_t), &pagescan_interval);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_WINDOW, sizeof(uint16_t), &pagescan_window);
    gap_br_set_param(GAP_BR_PARAM_PAGE_TIMEOUT, sizeof(uint16_t), &page_timeout);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_REPETITION_MODE, sizeof(uint8_t),
                     &page_scan_repetition_mode);

    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_TYPE, sizeof(uint8_t), &inquiryscan_type);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_INTERVAL, sizeof(uint16_t), &inquiryscan_interval);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_WINDOW, sizeof(uint16_t), &inquiryscan_window);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_MODE, sizeof(uint8_t), &inquiry_mode);

    bt_pairing_tx_power_set(-2);

    app_ble_gap_param_init();
}

static void framework_init(void)
{
    /* System Manager */
    sys_mgr_init(audio_evt_queue_handle);
#if F_APP_ERWS_SUPPORT
    /* RemoteController Manager */
    remote_mgr_init((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
    remote_local_addr_set(app_cfg_nv.bud_local_addr);
    remote_peer_addr_set(app_cfg_nv.bud_peer_addr);
#endif
    /* Bluetooth Manager */
    bt_mgr_init(MAX_BR_LINK_NUM);

    /* Audio Manager */
    if (app_cfg_nv.audio_latency == 0)
    {
        app_cfg_nv.audio_latency = 280;
    }

#if F_APP_HARMAN_FEATURE_SUPPORT && (F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT)
    app_cfg_nv.audio_latency = 150;
#endif

    audio_mgr_init(app_cfg_nv.audio_latency * POOL_SIZE_FACTOR, VOICE_POOL_SIZE, RECORD_POOL_SIZE,
                   NOTIFICATION_POOL_SIZE);

#if F_APP_HARMAN_FEATURE_SUPPORT
    //DBG_DIRECT("[SD_CHECK] MAX_PLC_CNT: %d", app_cfg_const.maximum_packet_lost_compensation_count);
    app_cfg_const.maximum_packet_lost_compensation_count = 0;
#endif

    audio_mgr_set_max_plc_count(app_cfg_const.maximum_packet_lost_compensation_count);

    if ((app_cfg2_const.dsp_gpio_num) || (app_cfg2_const.dsp_jtag_enable) ||
        (app_cfg2_const.dsp2_jtag_enable))
    {
        audio_probe_disable_dsp_powerdown();
    }

#if F_APP_DUAL_AUDIO_EFFECT
    audio_remote_join_set(false, false);
#endif
}

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
static void app_ble_audio_init(void)
{
    app_lea_profile_init();
}
#endif


static void app_task(void *pvParameters)
{
    uint8_t event;

    APP_PRINT_WARN2("app_task: unused memory %d, current cpu freq %d MHz", mem_peek(),
                    pm_cpu_freq_get());
#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
    if (app_ext_charger_check_support())
    {
        app_adp_detect();
    }
#else
    app_adp_detect();
#endif

    gap_start_bt_stack(audio_evt_queue_handle, audio_io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

    while (true)
    {
        if (os_msg_recv(audio_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (EVENT_GROUP(event) == EVENT_GROUP_IO)
            {
                T_IO_MSG io_msg;

                if (os_msg_recv(audio_io_queue_handle, &io_msg, 0) == true)
                {
                    if (event == EVENT_IO_TO_APP)
                    {
                        app_io_msg_handler(io_msg);
                    }
                }
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_STACK)
            {
                gap_handle_msg(event);
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_FRAMEWORK)
            {
                sys_mgr_event_handle(event);
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_APP)
            {
                app_timer_handle_msg(event);
            }

            if (app_cfg_const.led_support)
            {
                app_led_check_charging_mode(0); //LED mode has higher priority
                app_led_check_repeat_mode();
#if F_APP_TEAMS_GLOBAL_MUTE_SUPPORT
                teams_extend_led_check_mode();
#endif
            }
        }
    }
}

static void app_wakeup_reason_check(void)
{
    uint8_t pm_wakeup_reason = 0;

    pm_wakeup_reason = power_down_check_wake_up_reason();

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_RTC)
    {
        app_db.wake_up_reason |= WAKE_UP_RTC;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_MFB)
    {
        app_db.wake_up_reason |= WAKE_UP_MFB;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_ADP)
    {
        app_db.wake_up_reason |= WAKE_UP_ADP;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_CTC)
    {
        app_db.wake_up_reason |= WAKE_UP_CTC;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_PAD)
    {
        uint8_t i = 0;

        app_db.wake_up_reason |= WAKE_UP_PAD;

        if (System_WakeUpInterruptValue(app_cfg_const.gpio_box_detect_pinmux))
        {
            app_db.wake_up_reason |= WAKE_UP_3PIN;
        }

        if (System_WakeUpInterruptValue(app_cfg_const.key_pinmux[0]))
        {
            app_db.wake_up_reason |= WAKE_UP_KEY0;
        }

        for (i = 1; i < MAX_KEY_NUM; i++)
        {
            if (System_WakeUpInterruptValue(app_cfg_const.key_pinmux[i]))
            {
                if (app_key_is_combinekey_power_on_off(i))
                {
                    app_db.wake_up_reason |= WAKE_UP_COMBINE_KEY_POWER_ONOFF;
                }
            }
        }
    }

    DBG_DIRECT("app_wakeup_reason_check: pm_wakeup_reason: 0x%x, app_wake_up_reason: 0x%x",
               pm_wakeup_reason, app_db.wake_up_reason);
}

int main(void)
{
    uint32_t time_entry_app;
    void *app_task_handle;
    uint16_t stack_size = 1024 * 3 - 256;
#if F_APP_HIFI4_SUPPORT
    void *ipc_task_handle = NULL;
#endif
    T_RELEASE_VER mcu_cfg;
    T_RELEASE_VER dsp_cfg;

    mcu_cfg.release_ver = get_image_release_version(IMG_MCUCONFIG);
    dsp_cfg.release_ver = get_image_release_version(IMG_DSPCONFIG);

    APP_PRINT_INFO8("mcu cfg version: %d.%d.%d.%d,dsp cfg version: %d.%d.%d.%d",
                    mcu_cfg.chip_ver, mcu_cfg.crb_ver, mcu_cfg.tool_ver, mcu_cfg.revision,
                    dsp_cfg.chip_ver, dsp_cfg.crb_ver, dsp_cfg.tool_ver, dsp_cfg.revision);

    log_module_trace_set(MODULE_HCI, LEVEL_TRACE, false);
    log_module_trace_set(MODULE_BTIF, LEVEL_TRACE, false);
    log_module_trace_set(MODULE_GAP, LEVEL_TRACE, false);

    time_entry_app = sys_timestamp_get();
    APP_PRINT_INFO1("TIME FROM PATCH TO APP: %d ms", time_entry_app);
    APP_PRINT_INFO2("APP COMPILE TIME: [%s - %s]", TRACE_STRING(__DATE__), TRACE_STRING(__TIME__));

// for harman chipid signature check
#if HARMAN_SECURITY_CHECK
    // Check Secure Boot
    if (sys_hall_get_secure_state() == true)
    {
        APP_PRINT_INFO0("APP START FROM SECURE BOOT");
    }
    else
    {
        DBG_DIRECT("APP START FROM Non SECURE");
        chip_reset(RESET_ALL);
    }
    register_chip_id_pub_key_read_func(harmain_public_key_read);
#endif

    // Check dsp image feature string
    {
        uint8_t image_feature_info[16] = {0};
        if (get_image_feature_info(IMG_DSPSYSTEM, image_feature_info, 16) != 0)
        {
            image_feature_info[15] = 0;
        }
        APP_PRINT_INFO2("IMG_DSPSYSTEM(%x) Feature String: %s", IMG_DSPSYSTEM,
                        TRACE_STRING(image_feature_info));
        memset(image_feature_info, 0, 16);
        if (get_image_feature_info(IMG_DSPAPP, image_feature_info, 16) != 0)
        {
            image_feature_info[15] = 0;
        }
        APP_PRINT_INFO2("IMG_DSPAPP(%x) Feature String: %s", IMG_DSPAPP, TRACE_STRING(image_feature_info));
    }

    {
        uint32_t cpu_freq;
        pm_cpu_freq_set(APP_BASE_CPU_FREQ, &cpu_freq);
    }

    if (sys_hall_get_reset_status())
    {
        APP_PRINT_INFO0("APP RESTART FROM WDT_SOFTWARE_RESET");
    }
    else
    {
        //APP power off reboot also in this case
        APP_PRINT_INFO0("APP START FROM HW_RESET");
    }

    os_msg_queue_create(&audio_io_queue_handle, "ioQ", MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&audio_evt_queue_handle, "evtQ", MAX_NUMBER_OF_RX_EVENT, sizeof(unsigned char));
#if F_APP_FINDMY_FEATURE_SUPPORT
    os_msg_queue_create(&app_findmy_queue_handle, "findmy_msg", MAX_NUMBER_OF_IO_MESSAGE,
                        sizeof(T_FINDMY_BLE_INDICATION));
#endif

    app_init_timer(audio_evt_queue_handle, MAX_NUMBER_OF_APP_TIMER_MODULE);
    pm_cpu_freq_init();
    app_ipc_init();

    app_cfg_init();

    app_wakeup_reason_check();

    board_init();
    app_bt_gap_init();
    framework_init();

#if F_APP_OTA_SUPPORT
    app_ota_init();
#endif

    //driver init MUST after callback provider init
    driver_init();

#if F_APP_HARMAN_FEATURE_SUPPORT
    // app_cfg_const.disallow_adjust_volume_when_idle = true;
    DBG_DIRECT("version: %d.%d.%d.%d, lea_mode_enable: %d, sass_switching_preference: 0x%x, finder_support: %d",
               VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILDNUM,
               app_cfg_nv.lea_mode_enable,
               app_cfg_nv.sass_switching_preference,
               extend_app_cfg_const.gfps_finder_support);
    app_harman_license_get();

    app_cfg_const.enter_pairing_while_only_one_device_connected = 0;
    app_cfg_const.play_max_min_tone_when_adjust_vol_from_phone = 1;

    app_harman_license_init();
#endif

#if F_APP_LOG2FLASH_SUPPORT
    app_log2flash_init(LOG2FLASH_ADDR_OFFSET, LOG2FLASH_SIZE);
#endif

    app_dlps_init();
    app_auto_power_off_init();

#if F_APP_ADP_CMD_SUPPORT
    app_adp_cmd_init();
#endif

    app_mmi_init();

    if (is_single_tone_test_mode()) //DUT test mode
    {
        reset_single_tone_test_mode();
        mp_hci_test_init(MP_HCI_TEST_DUT_MODE);
    }
    else //Normal mode
    {
        app_gap_init();
        app_ble_gap_init();
        app_ble_client_init();

        //DO NOT change order: app_cfg_nv.bud_role will be affected
        app_bt_policy_init();

        app_bt_point_init();

        //DO NOT change order: app_db.remote_session_state will be affected
#if F_APP_ERWS_SUPPORT
        app_rdtp_init();
        app_bt_sniffing_init();
        app_roleswap_init();
        //DO NOT change order: roleswap ctrl init must be placed before audio init
        app_roleswap_ctrl_init();
#endif

        //Other app module MUST init after here
#if F_APP_IO_OUTPUT_SUPPORT
        app_io_output_init();
#endif

        app_ble_service_init();

        app_hfp_init();
        app_avrcp_init();
        app_a2dp_init();
        app_sdp_init();
        app_spp_init();

#if F_APP_BT_PROFILE_PBAP_SUPPORT
        app_pbap_init();
#endif

#if F_APP_HID_SUPPORT
        app_hid_init();
#endif

#if F_APP_IAP_SUPPORT
        app_iap_init();
#endif

#if F_APP_HFP_AG_SUPPORT
        app_hfp_ag_init();
#endif

        app_audio_init();
        app_multi_init();

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        app_teams_multilink_init();
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
        app_multilink_customer_init();
#endif

#if F_APP_BUD_LOCATION_SUPPORT
        app_bud_loc_init();
        app_loc_mgr_init();
        app_in_out_box_init();
#endif

#if F_APP_TEST_SUPPORT
        app_test_init();
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
        app_rtk_fast_pair_adv_init();
#endif

#if F_APP_ADC_SUPPORT
        app_adc_init();
#endif

#if HARMAN_NTC_DETECT_PROTECT
        app_harman_adc_init();
#endif
        app_device_init();
        app_ble_device_init();
        app_transfer_init();

#if F_APP_LISTENING_MODE_SUPPORT
        app_listening_mode_init();
        app_anc_init();
        app_apt_init();
#endif

#if F_APP_LINEIN_SUPPORT
        app_line_in_init();
#endif

#if F_APP_AIRPLANE_SUPPORT
        app_airplane_init();
#endif

#if F_APP_VENDOR_CMD_SUPPORT
        app_cmd_init();
#endif
        app_bond_init();

#if F_APP_SPP_CAPTURE_DSP_DATA_1
        app_spp_capture_init();
#elif F_APP_SPP_CAPTURE_DSP_DATA_2
        app_data_capture_init();
#endif

#if F_APP_BLE_BOND_SYNC_SUPPORT
        app_ble_bond_init();
#endif

#if F_APP_BOND_MGR_BLE_SYNC
        ble_bond_sync_init();
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
        if (app_cfg_nv.lea_mode_enable)
        {
            app_ble_audio_init();
        }
#endif

#if AMA_FEATURE_SUPPORT || BISTO_FEATURE_SUPPORT || F_APP_IAP_RTK_SUPPORT
        spp_stream_init(0xff);
        ble_stream_init(0xff);
        iap_stream_init(0xff);
#endif
        app_third_party_srv_init();

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
        app_swift_pair_init();
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
#if F_APP_HARMAN_FEATURE_SUPPORT
        app_cfg2_const.usb_config.func_en = 1;
        app_cfg2_const.usb_config.speed = 1;
        app_cfg2_const.usb_config.audio_class_en = 1;
        app_cfg2_const.usb_config.hid_class_en = 1;
#endif
        if (app_cfg2_const.usb_config.func_en)
        {
            app_usb_init();
        }
#endif

#if F_APP_USB_SUSPEND_SUPPORT
        app_usb_suspend_init();
#endif

#if F_APP_IAP_RTK_SUPPORT
        app_iap_rtk_init();
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
        app_slide_switch_init();
#endif

#if F_APP_QDECODE_SUPPORT
        if (app_cfg_const.wheel_support)
        {
            app_qdec_init();
        }
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
        app_harman_vendor_cmd_init();
        app_harman_behaviors_init();
        app_harman_ble_ota_init();
#endif

#if F_APP_AUTO_POWER_TEST_LOG
        app_power_test_init();
#endif

#if GFPS_SASS_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_sass_policy_init();
        }
#endif

        //increase app task priority to 2 so that user could implement some background daemon task
        os_task_create(&app_task_handle, "app_task", app_task, NULL, stack_size, 2);

#if F_APP_POWER_TEST
        hal_debug_init();
        hal_debug_task_time_proportion_init(10 * 1000); //include cache hit
        dlps_util_debug_init(10 * 1000); //ms
        dlps_util_cpu_sleep_monitor_start();
        power_test_dump_register(POWER_DEBUG_STAGE_BOOT);
#endif

#if F_APP_DEBUG_HIT_RATE_PRINT
        cache_hit_count_init(10000);
#endif
    }

    monitor_memory_and_timer(app_cfg_const.timer_monitor_heap_and_timer_timeout);

    APP_PRINT_INFO1("TIME FROM APP TO OS SCHED: %d ms", sys_timestamp_get() - time_entry_app);

    os_sched_start();

    return 0;
}

