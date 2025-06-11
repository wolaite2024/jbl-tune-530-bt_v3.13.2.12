#ifndef _APP_DONGLE_DATA_CTRL_H_
#define _APP_DONGLE_DATA_CTRL_H_

#define DONGLE_FORMAT_START_BIT                0x52
#define DONGLE_FORMAT_STOP_BIT                 0x54

#define DONGLE_TYPE_DATA                       0x00
#define DONGLE_TYPE_CMD                        0x01

typedef enum
{
    DONGLE_CMD_SET_GAMING_MOE           = 0x01,
    DONGLE_CMD_REQUEST_GAMING_MOE       = 0x02,
    DONGLE_CMD_REQ_OPEN_MIC             = 0x03,
    DONGLE_CMD_SET_VOL_BALANCE          = 0x06,
    DONGLE_CMD_CTRL_RAW_DATA            = 0x08,
    DONGLE_CMD_PASS_THROUGH_DATA        = 0x10,
    DONGLE_CMD_CFU_DATA                 = 0x11,
    DONGLE_CMD_SYNC_STATUS              = 0xFE,
} T_APP_DONGLE_CMD;

#if F_APP_COMMON_DONGLE_SUPPORT
bool app_dongle_send_cmd(T_APP_DONGLE_CMD cmd, uint8_t *data, uint16_t len);
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
void app_dongle_data_ctrl_init(void);
#if F_APP_LEA_SUPPORT
void app_dongle_handle_le_data(uint8_t *data, uint16_t len);
#endif
#endif

#endif
