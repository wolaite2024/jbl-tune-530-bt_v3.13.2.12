/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_HID_SUPPORT
#include "stdlib.h"
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "bt_hid.h"
#include "app_cfg.h"
#include "app_bt_policy_api.h"
#include "app_roleswap.h"
#include "app_link_util.h"
#include "app_hid.h"
#include "app_bt_sniffing.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_hid.h"
#endif
#if F_APP_CFU_FEATURE_SUPPORT
#include "app_cfu_transfer.h"
#endif
#define REPORT_ID_USB_UTILITY_1 (42)
#define REPORT_ID_USB_UTILITY_3 (44)
#define REPORT_ID_USB_UTILITY_4 (45)

#define REPORT_ID_USB_UTILITY_5 (0x24)
#define REPORT_ID_USB_UTILITY_6 (0x27)

#define REPORT_ID_USB_UTILITY_7 (0x39)

#if F_APP_TEAMS_FEATURE_SUPPORT
#if F_APP_TEAMS_HID_SUPPORT
const uint8_t hid_report_map1[] =
{
    0x06, 0x0b, 0xff,        /* USAGE_PAGE       Vendor          */
    0x0a, 0x04, 0x01,        /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,              /* COLLECTION        (Application)  */
    0x15, 0x00,              /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,              /* LOGICAL_MAXIMUM   (255)          */
    0x75, 0x08,              /* REPORT_SIZE       (8)            */
    0x95, 0x3c,              /* REPORT_COUNT      (60)           */
    0x85, REPORT_ID_USB_UTILITY_1,              /* REPORT_ID         (42)           */
    0x09, 0x60,              /* USAGE             (0x60)         */
    0x82, 0x02, 0x01,        /* INPUT             (Data,Var,Abs) */
    0x09, 0x61,              /* USAGE             (0x61)         */
    0x92, 0x02, 0x01,        /* OUTPUT            (Data,Var,Abs) */
    0x09, 0x62,              /* USAGE             (0x62)         */
    0xb2, 0x02, 0x01,        /* FEATURE           (Data,Var,Abs) */
    /* 32-bit data */
    0x17, 0x00, 0x00, 0x00, 0x80,   /* LOGICAL_MINIMUM                 */
    0x27, 0xff, 0xff, 0xff, 0x7e,   /* LOGICAL_MAXIMUM                 */
    0x75, 0x20,                     /* REPORT_SIZE      (32)           */
    0x95, 0x04,                     /* REPORT_COUNT     (4)            */
    0x85, REPORT_ID_USB_UTILITY_3,                     /* REPORT_ID        (44)           */
    0x19, 0x66,                     /* USAGE_MINIMUM    (0x66)         */
    0x29, 0x69,                     /* USAGE_MAXIMUM    (0x69)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x85, REPORT_ID_USB_UTILITY_4,                     /* REPORT_ID        (45)           */
    0x19, 0x8a,                     /* USAGE_MINIMUM    (0x8a)         */
    0x29, 0x8d,                     /* USAGE_MAXIMUM    (0x8d)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x19, 0x8e,                     /* USAGE_MINIMUM    (0x8e)         */
    0x29, 0x91,                     /* USAGE_MAXIMUM    (0x91)         */
    0x91, 0x02,                     /* OUTPUT           (Data,Var,Abs) */
    0xc0,                           /* END_COLLECTION                  */

    0x06, 0x07, 0xff,               /* USAGE_PAGE       Vendor          */
    0x0a, 0x22, 0x02,               /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,                     /* COLLECTION        (Application)  */
    0x85, REPORT_ID_USB_UTILITY_5,  /* REPORT_ID         (24)           */
    0x75, 0x08,                     /* REPORT_SIZE      (8)           */
    0x95, 0x3e,                     /* REPORT_COUNT     (62)            */
    0x15, 0x00,                     /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,                     /* LOGICAL_MAXIMUM   (255)          */
    0x09, 0x01,                     /* USAGE             (0x01)         */
    0xb2, 0x02, 0x01,               /* FEATURE           (Data,Var,Abs) */

    0x85, REPORT_ID_USB_UTILITY_6,  /* REPORT_ID         (27)           */
    0x95, 0x3e,                     /* REPORT_COUNT     (62)            */
    0x09, 0x02,                     /* USAGE             (0x02)         */
    0x82, 0x02, 0x01,               /* INPUT             (Data,Var,Abs) */
    0xc0,                           /* END_COLLECTION                  */

    0x06, 0x00, 0xff,               /* USAGE_PAGE       Vendor          */
    0x0a, 0x39, 0xff,               /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,                     /* COLLECTION        (Application)  */
    0x85, REPORT_ID_USB_UTILITY_7,  /* REPORT_ID         (0x39)           */
    0x75, 0x08,                     /* REPORT_SIZE      (8)           */
    0x95, 0x07,                     /* REPORT_COUNT     (7)            */
    0x09, 0x04,                     /* USAGE             (0x04)         */
    0x82, 0x02, 0x01,               /* INPUT             (Data,Var,Abs) */
    0xc0                            /* END_COLLECTION                   */
};

#endif
#endif

#if F_APP_CFU_HID_SUPPORT
const uint8_t hid_report_map2[] =
{
    0x06, 0x0b, 0xff,        /* USAGE_PAGE       Vendor          */
    0x0a, 0x04, 0x01,        /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,              /* COLLECTION        (Application)  */
    0x15, 0x00,              /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,              /* LOGICAL_MAXIMUM   (255)          */
    0x75, 0x08,              /* REPORT_SIZE       (8)            */
    0x95, 0x3c,              /* REPORT_COUNT      (60)           */
    0x85, REPORT_ID_USB_UTILITY_1,              /* REPORT_ID         (42)           */
    0x09, 0x60,              /* USAGE             (0x60)         */
    0x82, 0x02, 0x01,        /* INPUT             (Data,Var,Abs) */
    0x09, 0x61,              /* USAGE             (0x61)         */
    0x92, 0x02, 0x01,        /* OUTPUT            (Data,Var,Abs) */
    0x09, 0x62,              /* USAGE             (0x62)         */
    0xb2, 0x02, 0x01,        /* FEATURE           (Data,Var,Abs) */
    /* 32-bit data */
    0x17, 0x00, 0x00, 0x00, 0x80,   /* LOGICAL_MINIMUM                 */
    0x27, 0xff, 0xff, 0xff, 0x7e,   /* LOGICAL_MAXIMUM                 */
    0x75, 0x20,                     /* REPORT_SIZE      (32)           */
    0x95, 0x04,                     /* REPORT_COUNT     (4)            */
    0x85, REPORT_ID_USB_UTILITY_3,                     /* REPORT_ID        (44)           */
    0x19, 0x66,                     /* USAGE_MINIMUM    (0x66)         */
    0x29, 0x69,                     /* USAGE_MAXIMUM    (0x69)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x85, REPORT_ID_USB_UTILITY_4,                     /* REPORT_ID        (45)           */
    0x19, 0x8a,                     /* USAGE_MINIMUM    (0x8a)         */
    0x29, 0x8d,                     /* USAGE_MAXIMUM    (0x8d)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x19, 0x8e,                     /* USAGE_MINIMUM    (0x8e)         */
    0x29, 0x91,                     /* USAGE_MAXIMUM    (0x91)         */
    0x91, 0x02,                     /* OUTPUT           (Data,Var,Abs) */
    0xc0,                           /* END_COLLECTION                  */
};
#endif

#if F_APP_HID_MOUSE_SUPPORT
const uint8_t hid_descriptor_mouse_boot_mode[] =
{
    0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x02,             /* USAGE (Mouse) */
    0xA1, 0x01,             /* COLLECTION (Application) */

    0x85, 0x02,             /* REPORT_ID         (2)           */
    0x09, 0x01,             /*   USAGE (Pointer) */
    0xA1, 0x00,             /*   COLLECTION (Physical) */

    0x05, 0x09,             /*     USAGE_PAGE (Button) */
    0x19, 0x01,             /*     USAGE_MINIMUM (Button 1) */
    0x29, 0x03,             /*     USAGE_MAXIMUM (Button 3) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x95, 0x03,             /*     REPORT_COUNT (3) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x05,             /*     REPORT_SIZE (5) */
    0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */

    0x05, 0x01,             /*     USAGE_PAGE (Generic Desktop) */
    0x09, 0x30,             /*     USAGE (X) */
    0x09, 0x31,             /*     USAGE (Y) */
    0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x95, 0x02,             /*     REPORT_COUNT (2) */
    0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
    0x09, 0x38,             /*     USAGE (Wheel) */
    0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x81, 0x06,             /*     INPUT (Data,Var,Rel) */

    0xC0,                   /*   END_COLLECTION */
    0xC0                    /* END_COLLECTION */
};
#endif

#if F_APP_HID_KEYBOARD_SUPPORT
const uint8_t hid_descriptor_keyboard_boot_mode[] =
{
    0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x06,             /* USAGE (Keyboard) */
    0xA1, 0x01,             /* COLLECTION (Application) */
    0x85, 0x01,             /*     REPORT_ID         (1)           */
    0x95, 0x08,             /*     REPORT_COUNT (8) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x05, 0x07,             /*     USAGE (Keyboard/Keypad) */
    0x19, 0xe0,             /*     USAGE_MINIMUM (Keyboard Left Control) */
    0x29, 0xe7,             /*     USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */

    0x95, 0x05,             /*     REPORT_COUNT (5) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x05, 0x08,             /*     USAGE_PAGE (LEDs) */
    0x19, 0x01,             /*     USAGE_MINIMUM (Num Lock) */
    0x29, 0x05,             /*     USAGE_MAXIMUM (Kana) */
    0x91, 0x02,             /*     OUTPUT (Data,Var,Abs) */
    0x95, 0x01,             /*     REPORT_COUNT (1) */
    0x75, 0x03,             /*     REPORT_SIZE (3) */
    0x91, 0x03,             /*     OUTPUT (Cnst,Var,Abs) */

    0x95, 0x06,             /*     REPORT_COUNT (6) */
    0x75, 0x08,             /*     REPORT_SIZE (8) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0xff,             /*     LOGICAL_MAXIMUM (255) */
    0x05, 0x07,             /*     USAGE_PAGE (Keyboard/Keypad) */
    0x19, 0x00,             /*     USAGE_MINIMUM (Keyboard Return (ENTER)) */
    0x29, 0xff,             /*     USAGE_MAXIMUM (Keyboard ESCAPE) */
    0x81, 0x00,             /*     INPUT (Data,Arr,Abs) */
    0xC0,                   /* END_COLLECTION */

    0x05, 0x0c,             /* USAGE_PAGE (Consumer) */
    0x09, 0x01,             /* USAGE (Consumer Control) */
    0xA1, 0x01,             /* COLLECTION (Application) */
    0x85, 0x02,             /*     REPORT_ID         (2)           */
    0x95, 0x18,             /*     REPORT_COUNT (24) */
    0x75, 0x01,             /*     REPORT_SIZE (1) */
    0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
    0x0a, 0x23, 0x02,       /*     USAGE (AL Home) */
    0x0a, 0x21, 0x02,       /*     USAGE (AC Search) */
    0x0a, 0x8a, 0x01,       /*     USAGE (AL Email Reader) */
    0x0a, 0x96, 0x01,       /*     USAGE (AL Internet Browser) */
    0x0a, 0x9e, 0x01,       /*     USAGE (AL Terminal Lock/Screensaver) */
    0x0a, 0x01, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x02, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x05, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x06, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x07, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x08, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x0a, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0x0b, 0x03,       /*     USAGE (Reserved) */
    0x0a, 0xae, 0x01,       /*     USAGE (AL Keyboard Layboard) */
    0x0a, 0xb1, 0x01,       /*     USAGE (AL Screen Saver) */
    0x09, 0x40,             /*     USAGE (Menu) */
    0x09, 0x30,             /*     USAGE (Power) */
    0x09, 0xb7,             /*     USAGE (Stop) */
    0x09, 0xb6,             /*     USAGE (Scan Previous Track) */
    0x09, 0xcd,             /*     USAGE (Play/Pause) */
    0x09, 0xb5,             /*     USAGE (Scan Next Track) */
    0x09, 0xe2,             /*     USAGE (Mute) */
    0x09, 0xea,             /*     USAGE (Volume Down) */
    0x09, 0xe9,             /*     USAGE (Volume Up) */
    0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
    0xC0,                   /* END_COLLECTION */
};
#endif


static void app_hid_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;
#if F_APP_TEAMS_FEATURE_SUPPORT
    T_APP_TEAMS_HID_DATA hid_data = {0};
    memcpy(hid_data.teams_classic_hid_data.bd_addr, param->hid_get_report_ind.bd_addr, 6);
    hid_data.teams_classic_hid_data.report_id =  param->hid_get_report_ind.report_id;
    hid_data.teams_classic_hid_data.report_type =  param->hid_get_report_ind.report_type;
    hid_data.teams_classic_hid_data.data_len = param->hid_get_report_ind.report_size;
#endif
#if F_APP_CFU_FEATURE_SUPPORT
    T_APP_CFU_DATA cfu_hid_data = {0};
    memcpy(cfu_hid_data.classic_hid_data.bd_addr, param->hid_get_report_ind.bd_addr, 6);
    cfu_hid_data.classic_hid_data.report_id =  param->hid_get_report_ind.report_id;
    cfu_hid_data.classic_hid_data.report_type =  param->hid_get_report_ind.report_type;
    cfu_hid_data.classic_hid_data.data_len = param->hid_get_report_ind.report_size;
#endif
    switch (event_type)
    {
    case BT_EVENT_HID_CONN_IND:
        {
            p_link = app_link_find_br_link(param->hid_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_hid_connect_cfm(p_link->bd_addr, true);
            }
        }
        break;

    case BT_EVENT_HID_CONN_CMPL:
        {
            app_bt_policy_qos_param_update(param->hid_conn_cmpl.bd_addr, BP_TPOLL_HID_CONN_EVENT);
        }
        break;

    case BT_EVENT_HID_CONTROL_DATA_IND:
        {

        }
        break;

    case BT_EVENT_HID_GET_REPORT_IND:
        {
            if ((app_link_conn_profiles() & HID_PROFILE_MASK) != 0)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                hid_data.teams_classic_hid_data.report_action = APP_TEAMS_HID_REPORT_ACTION_GET;
                hid_data.teams_classic_hid_data.report_size = param->hid_get_report_ind.report_size;
#endif
#if F_APP_CFU_FEATURE_SUPPORT
                cfu_hid_data.classic_hid_data.report_action = APP_CFU_REPORT_ACTION_GET;
                cfu_hid_data.classic_hid_data.report_size = param->hid_get_report_ind.report_size;
#endif
                switch (param->hid_get_report_ind.report_id)
                {
#if F_APP_CFU_FEATURE_SUPPORT
                case REPORT_ID_USB_UTILITY_1:
                case REPORT_ID_USB_UTILITY_3:
                case REPORT_ID_USB_UTILITY_4:
                    app_cfu_handle_report(param->hid_get_report_ind.report_id, APP_CFU_REPORT_SOURCE_CLASSIC_HID,
                                          &cfu_hid_data);
                    break;
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
                case REPORT_ID_USB_UTILITY_5:
                case REPORT_ID_USB_UTILITY_6:
                    app_teams_hid_handle_config_report(param->hid_get_report_ind.report_id,
                                                       APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID, &hid_data);
                    break;

                case REPORT_ID_USB_UTILITY_7:
                    app_teams_hid_handle_wl_report(param->hid_get_report_ind.report_id,
                                                   APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID, &hid_data);
                    break;
#endif

                default:
                    break;
                }
            }
        }
        break;

    case BT_EVENT_HID_SET_REPORT_IND:
        {
            if ((app_link_conn_profiles() & HID_PROFILE_MASK) != 0)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                hid_data.teams_classic_hid_data.p_data = param->hid_set_report_ind.p_data;
                hid_data.teams_classic_hid_data.report_action = APP_TEAMS_HID_REPORT_ACTION_SET;
#endif
#if F_APP_CFU_FEATURE_SUPPORT
                cfu_hid_data.classic_hid_data.p_data = param->hid_set_report_ind.p_data;
                cfu_hid_data.classic_hid_data.report_action = APP_CFU_REPORT_ACTION_SET;
#endif
                switch (param->hid_set_report_ind.report_id)
                {
#if F_APP_CFU_FEATURE_SUPPORT
                case REPORT_ID_USB_UTILITY_1:
                case REPORT_ID_USB_UTILITY_3:
                case REPORT_ID_USB_UTILITY_4:
                    app_cfu_handle_report(param->hid_set_report_ind.report_id, APP_CFU_REPORT_SOURCE_CLASSIC_HID,
                                          &cfu_hid_data);
                    break;
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
                case REPORT_ID_USB_UTILITY_5:
                case REPORT_ID_USB_UTILITY_6:
                    app_teams_hid_handle_config_report(param->hid_set_report_ind.report_id,
                                                       APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID, &hid_data);
                    break;

                case REPORT_ID_USB_UTILITY_7:
                    app_teams_hid_handle_wl_report(param->hid_set_report_ind.report_id,
                                                   APP_TEAMS_HID_REPORT_SOURCE_CLASSIC_HID, &hid_data);
                    break;
#endif

                default:
                    break;
                }
            }
        }
        break;

    case BT_EVENT_HID_GET_PROTOCOL_IND:
        {

        }
        break;

    case BT_EVENT_HID_SET_PROTOCOL_IND:
        {

        }
        break;

    case BT_EVENT_HID_SET_IDLE_IND:
        {
        }
        break;

    case BT_EVENT_HID_INTERRUPT_DATA_IND:
        {

        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_hid_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_hid_init(void)
{
    if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
    {
        bt_hid_init(app_cfg_const.hid_link_number, true);
#if F_APP_HID_MOUSE_SUPPORT
        bt_hid_descriptor_set(hid_descriptor_mouse_boot_mode, sizeof(hid_descriptor_mouse_boot_mode));
#endif
#if F_APP_HID_KEYBOARD_SUPPORT
        bt_hid_descriptor_set(hid_descriptor_keyboard_boot_mode, sizeof(hid_descriptor_keyboard_boot_mode));
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#if F_APP_TEAMS_HID_SUPPORT
        bt_hid_descriptor_set(hid_report_map1, sizeof(hid_report_map1));
#endif
#endif
#if F_APP_CFU_HID_SUPPORT
        bt_hid_descriptor_set(hid_report_map2, sizeof(hid_report_map2));
#endif
        bt_mgr_cback_register(app_hid_bt_cback);
    }
}
#endif
