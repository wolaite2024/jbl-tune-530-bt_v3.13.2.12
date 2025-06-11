#if F_APP_TUYA_SUPPORT
#include <os_msg.h>
#include <os_task.h>
#include "os_timer.h"
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "rtk_tuya_ble_app_demo.h"
#include "tuya_ble_log.h"
#include "rtk_tuya_ble_ota.h"
#include "rtk_tuya_ble_ota_flash.h"
#include "rtk_tuya_ble_demo_version.h"
#include "app_cfg.h"
#include "rtk_tuya_app_relay.h"
#include "rtk_tuya_app_dp.h"
#include "app_device.h"
#include "app_ipc.h"
#include "rtk_tuya_app_ext_ftl.h"

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TUYA_TASK_PRIORITY             2         //!< Task priorities
#define APP_TUYA_TASK_STACK_SIZE           256 * 12   //!<  Task stack size
#define MAX_NUMBER_OF_TUYA_TUYA_MESSAGE    0x20      //!<  GAP message queue size
/*============================================================================*
 *                              Variables
 *============================================================================*/
static void *app_tuya_task_handle;
static void *app_tuya_queue_handle;


static tuya_ble_device_param_t device_param = {0};
/*license info*/
static const char auth_key_test[] = "tsCPAPyTxG1BBzm8qn0gGfl721DzANLF";
static const char device_id_test[] = {0x74, 0x75, 0x79, 0x61, 0x37, 0x33, 0x38, 0x38, 0x33, 0x32, 0x38, 0x36, 0x33, 0x38, 0x36, 0x31};
static const uint8_t mac_test[6] = {0x33, 0x22, 0x77, 0x56, 0x22, 0xaa}; //The actual MAC address is : 66:55:44:33:22:11


#define APP_TUYA_EVENT_1  1
#define APP_TUYA_EVENT_2  2
#define APP_TUYA_EVENT_3  3
#define APP_TUYA_EVENT_4  4
#define APP_TUYA_EVENT_5  5

static uint8_t dp_data_array[255 + 3];
static uint16_t dp_data_len = 0;

typedef struct
{
    uint8_t data[50];
} tuya_data_type_t;

//void tuya_data_process(int32_t evt_id, void *data)
//{
//    tuya_data_type_t *event_1_data;
//    TUYA_APP_LOG_DEBUG("tuya_data_process: evt_id %d", evt_id);
//    switch (evt_id)
//    {
//    case APP_TUYA_EVENT_1:
//        event_1_data = (tuya_data_type_t *)data;
//        TUYA_APP_LOG_HEXDUMP_DEBUG("received APP_TUYA_EVENT_1 data:", event_1_data->data, 50);
//        break;
//    case APP_TUYA_EVENT_2:
//        break;
//    case APP_TUYA_EVENT_3:
//        break;
//    case APP_TUYA_EVENT_4:
//        break;
//    case APP_TUYA_EVENT_5:
//        break;
//    default:
//        break;

//    }
//}

tuya_data_type_t tuya_data;

//void tuya_evt_1_send_test(uint8_t data)
//{
//    tuya_ble_tuya_evt_t event;

//    for(uint8_t i=0; i<50; i++)
//    {
//        tuya_data.data[i] = data;
//    }
//    event.evt_id = APP_TUYA_EVENT_1;
//    event.tuya_event_handler = (void *)tuya_data_process;
//    event.data = &tuya_data;
//    tuya_ble_tuya_event_send(event);
//}


//static uint16_t sn = 0;
//static uint32_t time_stamp = 1587795793;

void app_tuya_task(void *p_param)
{
    tuya_ble_cb_evt_param_t event;
    /*this task is used to handle callabck event tuya_ble_cb_evt_param_t*/
    while (true)
    {
        if (os_msg_recv(app_tuya_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            TUYA_APP_LOG_INFO("app_tuya_task: event %d", event.evt);
            int16_t result = 0;
            switch (event.evt)
            {
            case TUYA_BLE_CB_EVT_CONNECTE_STATUS:
                TUYA_APP_LOG_INFO("app_tuya_task: current connect status = %d",
                                  event.connect_status);
                break;
            case TUYA_BLE_CB_EVT_DP_WRITE:
                dp_data_len = event.dp_write_data.data_len;
                memset(dp_data_array, 0, sizeof(dp_data_array));
                memcpy(dp_data_array, event.dp_write_data.p_data, dp_data_len);
                TUYA_APP_LOG_HEXDUMP_DEBUG("app_tuya_task: received dp write data :", dp_data_array, dp_data_len);
                //tuya_ble_dp_data_report(dp_data_array,dp_data_len);//danni
                break;
            case TUYA_BLE_CB_EVT_DP_DATA_RECEIVED:
                app_tuya_dp_data_receive(event.dp_received_data.p_data, event.dp_received_data.data_len);
                break;
            case TUYA_BLE_CB_EVT_DP_DATA_REPORT_RESPONSE:
                TUYA_APP_LOG_INFO("app_tuya_task:received dp data report response result code =%d",
                                  event.dp_response_data.status);
                break;
            case TUYA_BLE_CB_EVT_DP_DATA_WTTH_TIME_REPORT_RESPONSE:
                TUYA_APP_LOG_INFO("app_tuya_task:received dp data report response result code =%d",
                                  event.dp_response_data.status);
                break;
            case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_REPORT_RESPONSE:
                TUYA_APP_LOG_INFO("app_tuya_task: received dp data with flag report response sn = %d , flag = %d , result code =%d",
                                  event.dp_with_flag_response_data.sn, event.dp_with_flag_response_data.mode
                                  , event.dp_with_flag_response_data.status);
                break;
            case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_AND_TIME_REPORT_RESPONSE:
                TUYA_APP_LOG_INFO("app_tuya_task: received dp data with flag and time report response sn = %d , flag = %d , result code =%d",
                                  event.dp_with_flag_and_time_response_data.sn,
                                  event.dp_with_flag_and_time_response_data.mode, event.dp_with_flag_and_time_response_data.status);

                break;
            case TUYA_BLE_CB_EVT_UNBOUND:

                TUYA_APP_LOG_INFO("app_tuya_task: received unbound req");

                break;
            case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND:

                TUYA_APP_LOG_INFO("app_tuya_task: received anomaly unbound req");

                break;
            case TUYA_BLE_CB_EVT_DEVICE_RESET:

                TUYA_APP_LOG_INFO("app_tuya_task: received device reset req");

                break;
            case TUYA_BLE_CB_EVT_DP_QUERY:
                TUYA_APP_LOG_INFO("app_tuya_task: received TUYA_BLE_CB_EVT_DP_QUERY event");
                if (dp_data_len > 0)
                {
                    //tuya_ble_dp_data_report(dp_data_array,dp_data_len);//danni
                }
                break;
            case TUYA_BLE_CB_EVT_OTA_DATA:
                tuya_ota_send_event_msg(event.ota_data.type, event.ota_data.p_data, event.ota_data.data_len);
                //tuya_ota_proc(event.ota_data.type, event.ota_data.p_data, event.ota_data.data_len);
                break;
            case TUYA_BLE_CB_EVT_NETWORK_INFO:
                TUYA_APP_LOG_INFO("app_tuya_task: received net info : %s", event.network_data.p_data);
                tuya_ble_net_config_response(result);
                break;
            case TUYA_BLE_CB_EVT_WIFI_SSID:

                break;
            case TUYA_BLE_CB_EVT_TIME_STAMP:
                TUYA_APP_LOG_INFO("app_tuya_task: received unix timestamp : %s ,time_zone : %d",
                                  event.timestamp_data.timestamp_string, event.timestamp_data.time_zone);
                break;
            case TUYA_BLE_CB_EVT_TIME_NORMAL:

                break;
            case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH:
                TUYA_APP_LOG_HEXDUMP_DEBUG("app_tuya_task: received ble passthrough data :",
                                           event.ble_passthrough_data.p_data,
                                           event.ble_passthrough_data.data_len);
                tuya_ble_data_passthrough(event.ble_passthrough_data.p_data, event.ble_passthrough_data.data_len);
                break;
            default:
                TUYA_APP_LOG_WARNING("app_tuya_task: app_tuya_cb_queue msg: unknown event type 0x%04x", event.evt);
                break;
            }

            tuya_ble_event_response(&event);
        }
    }
}

static void tuya_ble_init(void)
{
    device_param.use_ext_license_key = 1;
    //If use the license stored by the SDK,initialized to 0, Otherwise 16 or 20.
    device_param.device_id_len = 16;

    /*TUYA_BLE_ADV_LOCAL_NAME_MAX_LEN = 5*/
    if (sizeof(app_cfg_const.device_name_le_default) < 5)
    {
        device_param.adv_local_name_len = sizeof(app_cfg_const.device_name_le_default);
        memcpy(device_param.adv_local_name, app_cfg_const.device_name_le_default,
               sizeof(app_cfg_const.device_name_le_default));
    }

    if (device_param.use_ext_license_key == 1)
    {
        memcpy(device_param.auth_key, (void *)auth_key_test, AUTH_KEY_LEN);
        memcpy(device_param.device_id, (void *)device_id_test, DEVICE_ID_LEN);
        memcpy(device_param.mac_addr.addr, mac_test, 6);
        device_param.mac_addr.addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;
    }

    device_param.p_type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    device_param.product_id_len = 8;
    memcpy(device_param.product_id, APP_PRODUCT_ID, 8);

    device_param.firmware_version = tuya_ota_get_app_ui_version(); //generated in app ui image

    device_param.hardware_version = TY_HARD_VER_NUM;

    tuya_ble_sdk_init(&device_param);
    tuya_ble_callback_queue_register(app_tuya_queue_handle);
    tuya_ble_device_delay_ms(200);

    TUYA_APP_LOG_INFO("tuya ble demo version : "TUYA_BLE_DEMO_VERSION_STR);
    TUYA_APP_LOG_INFO("app version: 0x%08x", device_param.firmware_version);
}

static void app_tuya_ble_device_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_STACK_READY:
        {
            tuya_ble_init();
        }
        break;

    default:
        break;
    }
}

void tuya_ble_app_init(void)
{
    os_task_create(&app_tuya_task_handle, "app_tuya", app_tuya_task, 0,
                   APP_TUYA_TASK_STACK_SIZE, APP_TUYA_TASK_PRIORITY);
    os_msg_queue_create(&app_tuya_queue_handle, "tuya_msg", MAX_NUMBER_OF_TUYA_TUYA_MESSAGE,
                        sizeof(tuya_ble_cb_evt_param_t));
    tuya_ble_ota_init();
    app_tuya_relay_init();
    app_tuya_ext_ftl_init();

    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_tuya_ble_device_event_cback);
}
#endif
