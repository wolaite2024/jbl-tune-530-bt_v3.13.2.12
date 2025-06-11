#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_app_dp.h"
#include "tuya_ble_api.h"
#include "os_mem.h"
#include "trace.h"

static uint32_t test_sn = 0;

void app_tuya_dp_volume_value_set(uint8_t volume)
{
    uint8_t dpvalue[8] = {0x05, DT_VALUE, 0x00, 0x04, 0x0, 0x0, 0x0, 0x04};
    dpvalue[7] = volume;
    tuya_ble_status_t ret = tuya_ble_dp_data_send(test_sn++, DP_SEND_TYPE_ACTIVE,
                                                  DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dpvalue, sizeof(dpvalue));
    APP_PRINT_INFO2("app_tuya_dp_volume_value_set: result %d, dpvalue %b", ret,
                    TRACE_BINARY(sizeof(dpvalue), dpvalue));
}

void app_tuya_dp_change_control_set(T_TUYA_DP_CHANGE_CONTROL change_control)
{
    uint8_t dpvalue[5] = {0x06, DT_ENUM, 0x00, 0x01, 0x0};
    dpvalue[4] = change_control;
    tuya_ble_status_t ret = tuya_ble_dp_data_send(test_sn++, DP_SEND_TYPE_ACTIVE,
                                                  DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dpvalue, sizeof(dpvalue));
    APP_PRINT_INFO2("app_tuya_dp_change_control_set: result %d, dpvalue %b", ret,
                    TRACE_BINARY(sizeof(dpvalue), dpvalue));
}

void app_tuya_dp_data_receive(uint8_t *dp_data, uint16_t dp_data_len)
{
    if ((dp_data == NULL) || (dp_data_len == 0))
    {
        return ;
    }

    uint8_t *p_dp_data = os_mem_zalloc(RAM_TYPE_DATA_ON, dp_data_len);

    if (p_dp_data)
    {
        memcpy(p_dp_data, dp_data, dp_data_len);
        uint8_t dp_id = p_dp_data[0];
        uint16_t dp_len = (p_dp_data[2] << 8) | p_dp_data[3];
        APP_PRINT_INFO3("app_tuya_dp_data_receive: dp_id %d,dp_len %d,dp_data %b", dp_id, dp_len,
                        TRACE_BINARY(dp_data_len, p_dp_data));

        switch (dp_id)
        {
        case TUYA_DP_ID_VOLUME_SET:
            app_tuya_dp_volume_value_set(50);//just for test
            break;

        case TUYA_DP_ID_CHANGE_CONTROL:
            app_tuya_dp_change_control_set(TUYA_DP_CHANGE_CONTROL_NEXT);//just for test
            break;

        default:
            break;
        }

        os_mem_free(p_dp_data);
        p_dp_data = NULL;
    }
    else
    {
        APP_PRINT_ERROR0("app_tuya_dp_data_receive: err");
    }
}
#endif
