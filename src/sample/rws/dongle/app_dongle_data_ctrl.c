#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "bt_spp.h"
#include "app_sdp.h"
#include "app_dongle_common.h"
#include "app_dongle_data_ctrl.h"
#include "app_dongle_ctrl.h"
#include "app_transfer.h"
#include "app_link_util.h"
#include "app_dongle_dual_mode.h"

#if F_APP_COMMON_DONGLE_SUPPORT
bool app_dongle_send_cmd(T_APP_DONGLE_CMD cmd, uint8_t *data, uint16_t len)
{
    bool ret = false;

#if F_APP_LEA_SUPPORT
    T_APP_LE_LINK *p_lea_link = app_dongle_get_le_audio_link();
#else
    uint8_t addr[6] = {0};
#endif

    if (
#if F_APP_LEA_SUPPORT
        p_lea_link
#else
        (app_dongle_get_connected_dongle_addr(addr) == true)
#endif
    )
    {
        uint16_t total_len = len + 5;
        uint8_t *buf = malloc(total_len);

        /* cmd + payload len, total 12 bits, buf[1] first 4 bits + buf[2] */
        uint16_t payload_len = len + 1;

        if (buf != NULL)
        {
            buf[0] = DONGLE_FORMAT_START_BIT;
            buf[1] = ((DONGLE_TYPE_CMD & 0x0F) | (((payload_len >> 8) & 0x0F) << 4));
            buf[2] = (payload_len & 0xFF);
            buf[3] = cmd;
            memcpy(buf + 4, data, len);
            buf[len + 4] = DONGLE_FORMAT_STOP_BIT;

#if F_APP_LEA_SUPPORT
            ret = app_transfer_start_for_le(p_lea_link->id, total_len, buf);
#else
            ret = bt_spp_data_send(addr, RFC_SPP_DONGLE_CHANN_NUM, buf, total_len, false);
#endif
            if (ret == false)
            {
                APP_PRINT_ERROR0("app_dongle_send_cmd: failed");
            }

            free(buf);
        }
    }

    return ret;
}
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_LEA_SUPPORT
void app_dongle_handle_le_data(uint8_t *data, uint16_t len)
{
    if ((data[0] != DONGLE_FORMAT_START_BIT) || (data[len - 1] != DONGLE_FORMAT_STOP_BIT))
    {
        APP_PRINT_ERROR0("app_dongle_handle_le_data: Data fromat is not correct!");
    }

    bool handle = true;
    uint8_t msg_type = data[1];
    uint8_t payload_len = data[2];
    uint8_t payload_id = data[3];

    switch (msg_type)
    {
    case DONGLE_TYPE_CMD:
        {
            if (payload_id == DONGLE_CMD_CTRL_RAW_DATA)
            {
                app_dongle_rcv_ctrl_data(data + 4, len - 4);
            }
            else if (payload_id == DONGLE_CMD_REQ_OPEN_MIC)
            {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
                app_dongle_le_audio_handle_mic(data[4]);
#endif
            }
            else if (payload_id == DONGLE_CMD_SYNC_STATUS)
            {
                memcpy(&dongle_status, &data[4], sizeof(dongle_status));

                app_dongle_lea_handle_dongle_status();
            }
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
        APP_PRINT_TRACE2("app_dongle_handle_le_data: msg_type: 0x%02x, id: 0x%02x", msg_type, payload_id);
    }
}
#endif

void app_dongle_data_ctrl_init(void)
{
}
#endif
