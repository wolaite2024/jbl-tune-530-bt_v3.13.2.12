
#ifndef _APP_FLAGS_H_
#define _APP_FLAGS_H_

/** @defgroup APP_FLAGS App Flags
  * @brief App Flags
  * @{
  */

//Init value of default features are defined here
//----- [Device related] -----
#define F_APP_POWER_TEST                            0 //Disable RSSI and Lowerbattery roleswap
#define F_APP_AUTO_POWER_TEST_LOG                   0
#define F_APP_TEST_SUPPORT                          0
#define F_APP_SPP_CAPTURE_DSP_DATA_1                0
#define F_APP_SPP_CAPTURE_DSP_DATA_2                0
#define F_APP_SAIYAN_MODE                           0
#define F_APP_SAIYAN_EQ_FITTING                     0
#define F_APP_CONSOLE_SUPPORT                       0
#define F_APP_CLI_STRING_MP_SUPPORT                 0
#define F_APP_CLI_BINARY_MP_SUPPORT                 0
#define F_APP_CLI_CFG_SUPPORT                       0
#define F_APP_BQB_CLI_SUPPORT                       0
#define F_APP_BQB_CLI_HFP_AG_SUPPORT                0
#define F_APP_BQB_CLI_MAP_SUPPORT                   0

#define F_APP_MAX_BT_POINT_NUM                      2
#define F_APP_MULTI_LINK_ENABLE                     1
#define F_APP_RWS_MULTI_SPK_SUPPORT                 0
#define F_APP_SINGLE_MUTLILINK_SCENERIO_1           0 //this macro only enable in single mode, and if it enable, F_APP_MUTILINK_VA_PREEMPTIVE must be disable
#define F_APP_MUTILINK_VA_PREEMPTIVE                1 //ERWS_MULTILINK_SUPPORT
#define F_APP_MUTILINK_TRIGGER_HIGH_PRIORITY        0
#define F_APP_MUTLILINK_SOURCE_PRIORITY_UI          0
#define F_APP_MUTILINK_ALLOW_TWO_SCO                0 //considering bandwidth issue, only can be enabled for single mode application
#define F_APP_CALL_HOLD_SUPPORT                     0
#define F_APP_VOICE_SPK_EQ_SUPPORT                  1
#define F_APP_VOICE_MIC_EQ_SUPPORT                  1
#define F_APP_AUDIO_VOCIE_SPK_EQ_INDEPENDENT_CFG    0
#define F_APP_AUDIO_VOCIE_SPK_EQ_COMPENSATION_CFG   0

#define F_APP_DUT_MODE_AUTO_POWER_OFF               1
#define F_APP_VOICE_NREC_SUPPORT                    1
#define F_APP_TTS_SUPPORT                           0
#define F_APP_LOCAL_PLAYBACK_SUPPORT                0
#define F_APP_USB_AUDIO_SUPPORT                     0
#define F_APP_USB_HID_SUPPORT                       0
#define F_APP_USB_SUSPEND_SUPPORT                   0
#define F_APP_LINEIN_SUPPORT                        0
#define F_APP_ERWS_SUPPORT                          0
#define F_APP_VAD_SUPPORT                           0
#define F_APP_CFU_SUPPORT                           0
#define F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT     0
#define F_APP_IPHONE_ABS_VOL_HANDLE                 1
#define F_APP_SMOOTH_BAT_REPORT                     1
#define F_APP_GOLDEN_RANGE                          1
#define F_APP_CFU_FEATURE_SUPPORT                   0
#define F_APP_CFU_SPP_SUPPORT                       0
#define F_APP_CFU_BLE_CHANNEL_SUPPORT               0

#define F_APP_FLASH_DUMP_SUPPORT                    0
#define F_APP_LOG2FLASH_SUPPORT                     0   // if support log to flash,  F_APP_FLASH_DUMP_SUPPORT should be 1
#define F_APP_RSSI_MONITOR_SUPPORT                  0   // if support rssi to flash,  F_APP_FLASH_DUMP_SUPPORT should be 1

#define F_APP_OTA_SUPPORT                           0
#define F_APP_OTA_TOOLING_SUPPORT                   0
#define F_APP_ADP_CMD_SUPPORT                       0
#define F_APP_BAS_DIS_SUPPORT                       1
#define F_APP_VENDOR_CMD_SUPPORT                    1
#define F_APP_TRANSMIT_SRV_DONGLE_SUPPORT           0
#define F_APP_BUD_LOCATION_SUPPORT                  0
#define F_APP_ENABLE_PAUSE_SECOND_LINK              0

//Dongle related
#define F_APP_COMMON_DONGLE_SUPPORT                 0
#define F_APP_GAMING_DONGLE_SUPPORT                 0
#define F_APP_CONFERENCE_DONGLE_SUPPORT             0
#define F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT      0
#define F_APP_LEA_ALWAYS_CONVERSATION               0
#define F_APP_DONGLE_MULTI_PAIRING                  0
/* this flag depends on F_APP_COMMON_DONGLE_SUPPORT */
#define F_APP_ALLOW_ONE_DONGLE_AND_ONE_PHONE_ONLY   0
/* this flag depends on F_APP_COMMON_DONGLE_SUPPORT */
#define F_APP_KEEP_LEA_BOND                         0
#define F_APP_DONGLE_ALWAYS_GAMING_MODE             0
#define F_APP_DISALLOW_AUTO_PAIRING                 0
#define F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV     0
#define F_APP_LOW_RSSI_DISCONNECT_DONGLE            0

#define F_APP_HFP_CMD_SUPPORT                       0
#define F_APP_DEVICE_CMD_SUPPORT                    0
#define F_APP_AVRCP_CMD_SUPPORT                     0
#define F_APP_PBAP_CMD_SUPPORT                      0


//----- [Legacy related] -----
#define F_APP_A2DP_CODEC_LDAC_SUPPORT               0
#define F_APP_A2DP_CODEC_LC3_SUPPORT                0
#define F_APP_SPECIFIC_UUID_SUPPORT                 0
#define F_APP_HFP_AG_SUPPORT                        0

#define F_APP_IAP_RTK_SUPPORT                       0
#define F_APP_IAP_SUPPORT                           0

#define F_APP_HID_SUPPORT                           0
#define F_APP_HID_MOUSE_SUPPORT                     0
#define F_APP_HID_KEYBOARD_SUPPORT                  0
#define F_APP_CFU_HID_SUPPORT                       0 //conflict with F_APP_TEAMS_HID_SUPPORT

#define F_APP_BT_PROFILE_PBAP_SUPPORT               0
#define F_APP_BT_PROFILE_MAP_SUPPORT                0

//----- [LE related] -----
#define F_APP_GATT_SERVER_EXT_API_SUPPORT           0
#define F_APP_BLE_BOND_SYNC_SUPPORT                 1
#define F_APP_SC_KEY_DERIVE_SUPPORT                 1
#define F_APP_BLE_ANCS_CLIENT_SUPPORT               0
#define F_APP_RWS_BLE_USE_RPA_SUPPORT               0
#define GFPS_SASS_ON_HEAD_DETECT_SUPPORT            0

//----- [Peripheral related] -----
#define F_APP_CAP_TOUCH_SUPPORT                     0

#define F_APP_SENSOR_SUPPORT                        0
#define F_APP_SENSOR_IQS773_873_SUPPORT             0 //Light sensor, P sensor
#define F_APP_SENSOR_JSA1225_SUPPORT                0 //Light sensor
#define F_APP_SENSOR_JSA1227_SUPPORT                0 //Light sensor
#define F_APP_SENSOR_PX318J_SUPPORT                 0 //Light sensor
#define F_APP_SENSOR_HX3001_SUPPORT                 0 //Light sensor
#define F_APP_SENSOR_SL7A20_SUPPORT                 0 //G seneor
#define F_APP_SENSOR_SC7A20_AS_LS_SUPPORT           0 //SC7A20 as light sensor
#define F_APP_SENSOR_CAP_TOUCH_SUPPORT              0

#define F_APP_PWM_SUPPORT                           0
#if F_APP_PWM_SUPPORT
#define F_APP_BUZZER_SUPPORT                        1
#endif

#define F_APP_NFC_SUPPORT                           0
#define F_APP_ADC_SUPPORT                           0
#define F_APP_DISCHARGER_NTC_DETECT_PROTECT         0
#define F_APP_GPIO_ONOFF_SUPPORT                    0
#define F_APP_IO_OUTPUT_SUPPORT                     0
#define F_APP_AMP_SUPPORT                           0
#define F_APP_GPIO_MICBIAS_SUPPORT                  0
#define F_APP_KEY_EXTEND_FEATURE                    0
#define F_APP_RWS_KEY_SUPPORT                       0
#define F_APP_EXT_FLASH_SUPPORT                     0

#define F_APP_SLIDE_SWITCH_SUPPORT                  0
#define F_APP_SLIDE_SWITCH_POWER_ON_OFF             0
#define F_APP_SLIDE_SWITCH_LISTENING_MODE           0
#define F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE          0

#define F_APP_ONE_WIRE_UART_SUPPORT                 0
#if F_APP_ONE_WIRE_UART_SUPPORT
#undef F_APP_CONSOLE_SUPPORT
#define F_APP_CONSOLE_SUPPORT                       1
#undef F_APP_CLI_BINARY_MP_SUPPORT
#define F_APP_CLI_BINARY_MP_SUPPORT                 1
#endif

//----- [Listening related] -----
#define F_APP_HEARABLE_SUPPORT                      0
#define F_APP_ANC_SUPPORT                           0
#define F_APP_APT_SUPPORT                           0
#define F_APP_SIDETONE_SUPPORT                      1
#define F_APP_AIRPLANE_SUPPORT                      0
#define F_APP_LISTENING_MODE_SUPPORT                0
#define F_APP_SUPPORT_ANC_APT_COEXIST               0
#define F_APP_ANC_DEFAULT_ACTIVATE_ALL_GROUP        0
#define F_APP_SUPPORT_ANC_APT_APPLY_BURN            0

//--- [RHE related] ----
#define F_APP_BRIGHTNESS_SUPPORT                    0
#define F_APP_POWER_ON_DELAY_APPLY_APT_SUPPORT      0
#define F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT        0
#define F_APP_LLAPT_SCENARIO_CHOOSE_SUPPORT         0
#define F_APP_SEPARATE_ADJUST_APT_VOLUME_SUPPORT    0
#define F_APP_ADJUST_NOTIFICATION_VOL_SUPPORT       0

//----- [3rd party related] -----
#define GFPS_FEATURE_SUPPORT                        1
#define GFPS_LE_DEVICE_SUPPORT                     (1 && GFPS_FEATURE_SUPPORT)
#define GFPS_SASS_SUPPORT                          (1 && GFPS_FEATURE_SUPPORT)
#define GFPS_FINDER_SUPPORT                        (1 && GFPS_FEATURE_SUPPORT)
#define GFPS_SASS_LEA_SUPPORT                      (1 && GFPS_SASS_SUPPORT && GFPS_FEATURE_SUPPORT)
#define F_APP_BLE_SWIFT_PAIR_SUPPORT                1

//----- [LEA related]-----
#define F_APP_LEA_SUPPORT                           1
#define F_APP_BOND_MGR_SUPPORT                      1

#define F_APP_PERIODIC_WAKEUP                       1

//ysc start
#define HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME                  0
#define HARMAN_CUSTOMIZED_BUTTON_CONTROL                    0
#define  HARMAN_SUPPORT_CONNECT_VP_IN_HFP                   0
//ysc end
#if TARGET_RTL8763EKH
#define IC_NAME                                     "RTL8763EKH"

#define F_APP_HARMAN_FEATURE_SUPPORT                        1

#define HARMAN_LOW_BAT_WARNING_TIME_SET_SUPPORT             0
#define HARMAN_LOW_BAT_WARNING_TIME                         900

#define HARMAN_ENTER_STANDBY_MODE_AFTER_PAIRING_TIMEOUT     1
//ysc start
#define HARMAN_DETECT_BATTERY_STATUS_LED_SUPPORT            1 // 5S continues
//ysc end
#define HARMAN_ONLY_CONN_ONE_DEVICE_WHEN_PAIRING            0
#define HARMAN_LED_BREATH_SUPPORT                           1
#define HARMAN_VP_DATA_HEADER_GET_SUPPORT                   1
//ysc start
#define HARMAN_ADJUST_LINKBACK_RETRY_TIME_SUPPORT           1
//ysc end
#define HARMAN_FIND_MY_BUDS_TONE_SUPPORT                    1
#define HARMAN_SPP_CMD_SUPPORT                              1
#define HARMAN_ADJUST_MAX_MIN_VOLUME_FROM_PHONE_SUPPORT     0

//ysc start
#define HARMAN_DISABLE_STANDBY_LED_FLASH_WHEN_LOW_BATTERY   1
//ysc end
#define HARMAN_ADD_GFPS_FINDER_STOP_ADV_TONE                1

#define HARMAN_VP_LR_BALANCE_SUPPORT                        0
#define HARMAN_DSP_LICENSE_SUPPORT                          0

#define HARMAN_AUTO_ACCEPT_SECOND_CALL_SUPPORT              0

#define HARMAN_ALLOW_POWER_OFF_ON_CALL_ACTIVE_SUPPORT       1
//ysc start
#define HARMAN_NOT_ALLOW_ENTER_PAIR_MODE_ON_CALLING_SUPPORT 1
//ysc end
#define HARMAN_POWER_DISPLAY_ACCURACY_1_PERCENTAGE_SUPPORT  1

#define HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT                1
#define HARMAN_BONDING_LEGACY_AND_BLE_LINK                  0

#define HARMAN_DELAY_CHARGING_FINISH_SUPPORT                0
#define HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT                 0

#define HARMAN_REQ_REMOTE_DEVICE_NAME_TIME                  1

#if F_APP_HARMAN_FEATURE_SUPPORT
#define HARMAN_AUTO_POWER_OFF_DEFAULT_TIME           (30 * 60)  // s

#define HARMAN_OTA_VERSION_CHECK                            0
#define HARMAN_OPEN_LR_FEATURE                              1   // todo: not ready

#define HARMAN_SECURITY_CHECK                               0

#define HARMAN_COMPRESSED_OTA_SUPPORT                       1
#define HARMAN_OTA_COMPLETE_NONEED_POWERON                  1

#define F_APP_EXT_CHARGER_FEATURE_SUPPORT                   1
#define HARMAN_NTC_DETECT_PROTECT                           1
#define HARMAN_DISCHARGER_NTC_DETECT_PROTECT                1
#define HARMAN_BRIGHT_LED_WHEN_ADP_IN                       0
#define HARMAN_SUPPORT_WATER_EJECTION                       0
#define HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA                 0

// set according to differrnt Project(T135/Run3/Pace)
#define HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT               0
#define HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT               0
#define HARMAN_SECOND_NTC_DETECT_PROTECT                    0
//ysc start
#define HARMAN_USB_CONNECTOR_PROTECT                        1
//ysc end

#if HARMAN_T135_SUPPORT

#undef HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#define HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT               1
#undef HARMAN_SECOND_NTC_DETECT_PROTECT
#define HARMAN_SECOND_NTC_DETECT_PROTECT                    0
#define HARMAN_VBAT_ADC_DETECTION                           0

#elif HARMAN_RUN3_SUPPORT

#undef HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#define HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT               1
#undef HARMAN_SECOND_NTC_DETECT_PROTECT
#define HARMAN_SECOND_NTC_DETECT_PROTECT                    0
#undef HARMAN_USB_CONNECTOR_PROTECT
#define HARMAN_USB_CONNECTOR_PROTECT                        1
#undef  HARMAN_SECURITY_CHECK
#define HARMAN_SECURITY_CHECK                               0
#undef  HARMAN_FIND_MY_BUDS_TONE_SUPPORT
#define HARMAN_FIND_MY_BUDS_TONE_SUPPORT                    0
#undef  HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
#define HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT                1
#undef  HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
#define HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME                  1
#elif HARMAN_PACE_SUPPORT

#undef HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
#define HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT               1
#undef HARMAN_SECOND_NTC_DETECT_PROTECT
#define HARMAN_SECOND_NTC_DETECT_PROTECT                    1
#undef HARMAN_USB_CONNECTOR_PROTECT
#define HARMAN_USB_CONNECTOR_PROTECT                        1
#undef  HARMAN_SECURITY_CHECK
#define HARMAN_SECURITY_CHECK                               0
#undef  HARMAN_DSP_LICENSE_SUPPORT
#define HARMAN_DSP_LICENSE_SUPPORT                          1
#undef  HARMAN_ALLOW_POWER_OFF_ON_CALL_ACTIVE_SUPPORT
#define HARMAN_ALLOW_POWER_OFF_ON_CALL_ACTIVE_SUPPORT       0
#undef  HARMAN_SUPPORT_WATER_EJECTION
#define HARMAN_SUPPORT_WATER_EJECTION                       1
#undef  HARMAN_BRIGHT_LED_WHEN_ADP_IN
#define HARMAN_BRIGHT_LED_WHEN_ADP_IN                       1
#undef  HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT
#define HARMAN_BLE_ENCRYPTED_CONNECT_SUPPORT                1#undef  HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
#define HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT                 1

#elif HARMAN_T530_SUPPORT

//ysc start
#define HARMAN_BATTERY_ID_VERIFICATION                      0
//ysc end

//ysc start
#define JBL_APP_LEA_BUTTON                                  1
#undef  HARMAN_SUPPORT_CONNECT_VP_IN_HFP
#define HARMAN_SUPPORT_CONNECT_VP_IN_HFP                    1

//ysc end

#undef HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#define HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT               1
#undef HARMAN_SECOND_NTC_DETECT_PROTECT
#define HARMAN_SECOND_NTC_DETECT_PROTECT                    0
#undef HARMAN_USB_CONNECTOR_PROTECT
#define HARMAN_USB_CONNECTOR_PROTECT                        1
//ysc start
#undef  HARMAN_SECURITY_CHECK
#define HARMAN_SECURITY_CHECK                               1
//ysc end
#undef F_APP_SPP_CAPTURE_DSP_DATA
#define F_APP_SPP_CAPTURE_DSP_DATA                          0
#undef  HARMAN_DSP_LICENSE_SUPPORT
#define HARMAN_DSP_LICENSE_SUPPORT                          1
#undef F_APP_ENABLE_PAUSE_SECOND_LINK
#define F_APP_ENABLE_PAUSE_SECOND_LINK                      1
#undef  HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA
#define HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA                 1
//ysc start 
#undef  HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME
#define HARMAN_SPECAIL_ULTRA_LONG_KEY_TIME                  1
#undef  HARMAN_CUSTOMIZED_BUTTON_CONTROL
#define HARMAN_CUSTOMIZED_BUTTON_CONTROL                    1
//ysc end
#endif

#endif /* F_APP_HARMAN_FEATURE_SUPPORT */

#if GFPS_FEATURE_SUPPORT
#undef F_APP_RWS_BLE_USE_RPA_SUPPORT
#define F_APP_RWS_BLE_USE_RPA_SUPPORT                       1
#endif

#endif

//----- [Target related] -----
#if F_APP_BOND_MGR_SUPPORT
#define F_APP_BOND_MGR_DEBUG                                1
#define F_APP_BOND_MGR_BLE_SYNC                             0
#undef F_APP_BLE_BOND_SYNC_SUPPORT
#define F_APP_BLE_BOND_SYNC_SUPPORT                         0
#endif

#if (F_APP_USER_EQ_SUPPORT == 1)
#undef F_APP_AUDIO_VOCIE_SPK_EQ_INDEPENDENT_CFG
#define F_APP_AUDIO_VOCIE_SPK_EQ_INDEPENDENT_CFG            1
#undef F_APP_AUDIO_VOCIE_SPK_EQ_COMPENSATION_CFG
#define F_APP_AUDIO_VOCIE_SPK_EQ_COMPENSATION_CFG           1
#endif

/** End of APP_CHARGER
* @}
*/

#endif

