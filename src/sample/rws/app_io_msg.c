/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "os_sync.h"
#include "os_msg.h"
#include "trace.h"
#include "app_main.h"
#include "app_charger.h"
#include "app_key_gpio.h"
#include "app_key_process.h"
#include "app_ble_gap.h"
#include "app_cfg.h"
#include "app_loc_mgr.h"
#include "app_transfer.h"
#include "app_console_msg.h"
#include "app_in_out_box.h"
#include "app_sensor.h"
#include "app_adp.h"
#if F_APP_ADP_CMD_SUPPORT
#include "app_adp_cmd.h"
#include "app_adp_cmd_parse.h"
#endif
#include "app_line_in.h"

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#endif

#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#if GFPS_FEATURE_SUPPORT
#include "app_ecc.h"
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "app_playback_trans.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
#include "app_usb.h"
#endif

#if BISTO_FEATURE_SUPPORT
#include "app_bisto_msg.h"
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_QDECODE_SUPPORT
#include "app_qdec.h"
#endif

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_device.h"
#include "rtk_tuya_ble_ota.h"
#endif

#if F_APP_CAP_TOUCH_SUPPORT
#include "app_cap_touch.h"
#endif

#if F_APP_SS_SUPPORT
#include "app_ss_cmd.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "ancs.h"
#include "app_findmy_task.h"
#include "fmna_connection.h"
#include "fmna_state_machine.h"
#include "os_mem.h"
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
#include "app_adc.h"
#endif

#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
#include "app_ext_charger.h"
#endif

#if HARMAN_USB_CONNECTOR_PROTECT
#include "app_harman_usb_connector_protect.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_unicast_audio.h"
#endif

#include "app_harman_adc.h"

RAM_TEXT_SECTION bool app_io_msg_send(T_IO_MSG *io_msg)
{
    T_EVENT_TYPE event = EVENT_IO_TO_APP;
    bool ret = false;

    if (os_msg_send(audio_io_queue_handle, io_msg, 0) == true)
    {
        ret = os_msg_send(audio_evt_queue_handle, &event, 0);
    }

    if (ret == false)
    {
        APP_PRINT_ERROR3("app_io_msg_send failed, type = %x, subtype = %x, param = %x",
                         io_msg->type, io_msg->subtype, io_msg->u.param);
    }

    return ret;
}

void app_io_msg_handler(T_IO_MSG io_driver_msg_recv)
{
    uint16_t msgtype = io_driver_msg_recv.type;

    switch (msgtype)
    {
    case IO_MSG_TYPE_GPIO:
        {
            switch (io_driver_msg_recv.subtype)
            {
            case IO_MSG_GPIO_KEY:
                {
                    app_key_handle_msg(&io_driver_msg_recv);
                }
                break;

#if F_APP_CONSOLE_SUPPORT
            case IO_MSG_GPIO_UART_WAKE_UP:
                {
                    app_transfer_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

            case IO_MSG_GPIO_CHARGER:
                {
                    app_charger_handle_msg(&io_driver_msg_recv);
                }
                break;

#if F_APP_LOCAL_PLAYBACK_SUPPORT
            case IO_MSG_GPIO_PLAYBACK_TRANS_FILE_ACK:
                {
                    app_playback_trans_write_file_ack_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
            case IO_MSG_GPIO_CHARGERBOX_DETECT:
                {
                    if (app_cfg_const.box_detect_method == GPIO_DETECT)
                    {
                        app_gpio_on_off_handle_msg(&io_driver_msg_recv);
                    }
                }
                break;
#endif

#if F_APP_NFC_SUPPORT
            case IO_MSG_GPIO_NFC:
                {
                    app_nfc_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_SENSOR_SUPPORT
            case IO_MSG_GPIO_SENSOR_LD:
                {
                    app_sensor_ld_handle_msg(&io_driver_msg_recv);
                }
                break;

            case IO_MSG_GPIO_SENSOR_LD_IO_DETECT:
                {
                    app_sensor_ld_io_handle_msg(&io_driver_msg_recv);
                }
                break;

#if F_APP_SENSOR_SL7A20_SUPPORT
            case IO_MSG_GPIO_GSENSOR:
                {
                    app_sensor_gsensor_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_SENSOR_IQS773_873_SUPPORT
            case IO_MSG_GPIO_PSENSOR:
                {
                    app_sensor_psensor_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
            // case IO_MSG_MEMS_ACC_DATA_INT:
            //     {
            //         if (app_cfg_const.mems_support)
            //         {
            //             app_sensor_mems_dat_int();
            //         }
            //     }
            //     break;

            case IO_MSG_MEMS_ACC_DATA:
                {
                    if (app_cfg_const.mems_support)
                    {
                        app_sensor_mems_handle_data(io_driver_msg_recv.u.buf);
                    }
                }
                break;
#endif
#endif

#if F_APP_ADP_CMD_SUPPORT
            case IO_MSG_GPIO_ADAPTOR_DAT:
                {
                    uint32_t cmd_data = io_driver_msg_recv.u.param;

                    app_adp_cmd_delay_charger_enable();
                    app_adp_cmd_parse_handle_msg(cmd_data);
                }
                break;
#endif

#if F_APP_LINEIN_SUPPORT
            case IO_MSG_GPIO_LINE_IN:
                {
                    app_line_in_detect_msg_handler(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
            case IO_MSG_GPIO_SLIDE_SWITCH_0:
            case IO_MSG_GPIO_SLIDE_SWITCH_1:
                {
                    app_slide_switch_handle_msg(&io_driver_msg_recv);
                }
                break;
#endif

            case IO_MSG_GPIO_KEY0:
                {
                    key_gpio_key0_debounce_start();
                }
                break;

#if F_APP_QDECODE_SUPPORT
            case IO_MSG_GPIO_QDEC:
                {
                    app_qdec_msg_handler(&io_driver_msg_recv);
                }
                break;
#endif

#if F_APP_ADP_CMD_SUPPORT
            case IO_MSG_GPIO_SMARTBOX_COMMAND_PROTECT:
                {
                    app_adp_cmd_parse_protect();
                }
                break;

            case IO_MSG_GPIO_ADP_INT:
                {
                    app_adp_cmd_parse_int_handle(&io_driver_msg_recv);
                }
                break;

            case IO_MSG_GPIO_ADP_HW_TIMER_HANDLER:
                {
                    app_adp_cmd_parse_hw_timer_handler();
                }
                break;
#endif

            case IO_MSG_GPIO_ADAPTOR_PLUG:
            case IO_MSG_GPIO_ADAPTOR_UNPLUG:
                {
                    app_adp_msg_handler(io_driver_msg_recv.subtype);
                }
                break;

#if F_APP_BUD_LOCATION_SUPPORT
            case IO_MSG_GPIO_BUD_LOC_CHANGE:
                {
                    T_BUD_LOCATION_EVENT evt = (T_BUD_LOCATION_EVENT)(io_driver_msg_recv.u.param & 0x00FF);
                    T_BUD_LOCATION_CAUSE_ACTION cause_action =
                        (T_BUD_LOCATION_CAUSE_ACTION)((io_driver_msg_recv.u.param & 0xFF00) >> 8);

                    app_loc_mgr_state_machine(evt, 0, cause_action);
                }
                break;
#endif

#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
            case IO_MSG_GPIO_EXT_CHARGER_STATE:
                {
                    app_ext_charger_handle_io_msg(io_driver_msg_recv);
                }
                break;
#endif

#if HARMAN_NTC_DETECT_PROTECT || HARMAN_DISCHARGER_NTC_DETECT_PROTECT
            case IO_MSG_GPIO_EXT_CHARGER_ADC_VALUE:
                {
                    app_harman_adc_msg_handle();
                }
                break;
#endif

#if HARMAN_USB_CONNECTOR_PROTECT
            case IO_MSG_GPIO_USB_CONNECTOR_ADC_VALUE:
                {
                    app_harman_usb_connector_adc_update();
                }
                break;
#endif
            default:
                break;
            }
        }
        break;

    case IO_MSG_TYPE_BT_STATUS:
        {
            app_ble_gap_handle_gap_msg(&io_driver_msg_recv);
        }
        break;

#if F_APP_ONE_WIRE_UART_SUPPORT
    case IO_MSG_TYPE_UART_LINE_STATUS:
        {
            if (app_cfg_const.one_wire_uart_support)
            {
                app_adp_io_plug_out();
            }
        }
        break;
#endif

#if F_APP_CONSOLE_SUPPORT
    case IO_MSG_TYPE_CONSOLE:
        {
            app_console_handle_msg(io_driver_msg_recv);
        }
        break;
#endif

#if GFPS_FEATURE_SUPPORT
    case IO_MSG_TYPE_ECC:
        {
            app_ecc_handle_msg();
        }
        break;
#endif

#if BISTO_FEATURE_SUPPORT
    case IO_MSG_TYPE_BISTO:
        {
            app_bisto_msg_handle(io_driver_msg_recv);
        }
        break;
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
    case IO_MSG_TYPE_USB_UAC:
        {
            app_usb_msg_handle(&io_driver_msg_recv);
        }
        break;

    case IO_MSG_TYPE_USB_HID:
        {
//            extern void app_usb_hid_handle_msg(T_IO_MSG * msg);
//            app_usb_hid_handle_msg(&io_driver_msg_recv);
        }
        break;
#endif

#if F_APP_CAP_TOUCH_SUPPORT
    case IO_MSG_TYPE_CAP_TOUCH:
        {
            app_cap_touch_msg_handle((T_IO_MSG_CAP_TOUCH)io_driver_msg_recv.subtype);
        }
        break;
#endif

#if F_APP_TUYA_SUPPORT
    case IO_MSG_TYPE_TUYA:
        {
            rtk_tuya_handle_event_msg(io_driver_msg_recv);
        }
        break;
    case IO_MSG_TYPE_TUYA_OTA:
        {
            tuya_ota_handle_msg(io_driver_msg_recv);
        }
        break;
#endif

#if F_APP_SS_SUPPORT
    case IO_MSG_TYPE_SS:
        {
            app_ss_cmd_handle_msg(&io_driver_msg_recv);
        }
        break;
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#if F_BT_ANCS_CLIENT_SUPPORT
    case IO_MSG_TYPE_ANCS:
        {
            ancs_handle_msg(&io_driver_msg_recv);
        }
        break;
#endif

    case IO_MSG_TYPE_FINDMY:
        {
            switch (io_driver_msg_recv.subtype)
            {
            case IO_MSG_FINDMY_KEYROLL:
                {
                    fmna_state_machine_handle_msg(io_driver_msg_recv.subtype);
                }
                break;

            case IO_MSG_FINDMY_EVENT:
                {
                    T_APP_SCHED_EVT *evt = (T_APP_SCHED_EVT *)io_driver_msg_recv.u.param;

                    if (evt->handler != NULL)
                    {
                        evt->handler(evt->p_event_data, evt->event_size);
                    }

                    if (evt->p_event_data != NULL)
                    {
                        os_mem_free(evt->p_event_data);
                    }

                    os_mem_free(evt);
                }
                break;

            default:
                break;
            }
        }
        break;
#endif

#if F_APP_DISCHARGER_NTC_DETECT_PROTECT
    case IO_MSG_TYPE_ADC:
        {
            app_adc_ntc_voltage_power_off_check();
        }
        break;
#endif

#if F_APP_LEA_SUPPORT
    case IO_MSG_TYPE_LEA_SNK:
        {
            app_lea_uca_msg_handle(&io_driver_msg_recv);
        }
        break;
#endif

    default:
        break;
    }
}

