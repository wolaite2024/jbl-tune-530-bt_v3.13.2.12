#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "app_main.h"
#include "app_harman_ble.h"
#include "app_harman_report.h"
#include "app_harman_license.h"
#include "ble_adv_data.h"
#include "ble_ext_adv.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_ble_common_adv.h"
#include "app_ble_rand_addr_mgr.h"
#include "app_multilink.h"
#include "app_ota.h"
#include "app_ble_device.h"
#include "app_ble_common_adv.h"

static uint8_t DEVICE_CRC1[2] = {0x00, 0x00};
static uint8_t DEVICE_CRC2[2] = {0x00, 0x00};
static uint8_t BT_MAC_CRC[2] = {0x00, 0x00};

/* Note:
    1. byte 7 / byte 13-14 / byte15 / byte 16-17 / byte 18-19 / byte 20-21 are all dynamic
    2. no legacy link connected  => no need to advertise
       set bit15/bit12 in byte 13-14,
       byte15      indicate color id,
       byte 16-17  indicate BT MAC Address CRC,
    3. one SRC connected         => need set SrcName1 CRC
       set bit15/bit14/bit12 in byte 13-14,
       byte15      indicate color id,
       byte 16-17  indicate SrcName1 CRC,
       byte 18-19  indicate BT MAC Address CRC,
    4. two SRC connected         => need set SrcName1 CRC and SrcName2 CRC
       set bit15/bit14/bit13/bit12 in byte 13-14,
       byte15      indicate color id,
       byte 16-17  indicate SrcName1 CRC,
       byte 18-19  indicate SrcName2 CRC,
       byte 20-21  indicate BT MAC Address CRC,
*/
static uint8_t harman_adv_data[22] =
{
    /* 0-6 */
    0x02, 0x0a, 0x00, 0x03, 0x16, 0x03, 0xfe,
    /* 7, The length of the Type + Value fields */
    0x0E,
    /* 8, Service Data AD Type */
    0x16,
    /* 9-10, Service Data UUID */
    0xDF, 0xFD,
    /* 11-12, product id */
    0x35, 0x21,
    /* 13-14, supported data status
              15th bit: Color ID support
              14th bit: SrcName1 CRC support
              13th bit: SrcName2 CRC support
              12th bit: BT MAC Address CRC support
    */
    0x00, 0xF0,
    /* 15, color id */
    0x02,
    /* 16-17, SrcName1 CRC: The CRC16 of a connected host name. */
    0x00, 0x00,
    /* 18-19, SrcName2 CRC: The CRC16 of the other connected host name. */
    0x00, 0x00,
    /* 20-21, BT MAC Address CRC: The CRC16 of Earbuds/Headset BT MAC Address. */
    0x00, 0x00
};

static const uint16_t harman_btxfcsTab[256] =
{
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

uint16_t harman_btxfcs(uint16_t fcs, uint8_t *cp, uint16_t len)
{
    while (len--)
    {
        fcs = (fcs >> 8) ^ harman_btxfcsTab[(fcs ^ *cp++) & 0xff];
    }
    //APP_PRINT_TRACE1("harman_btxfcs calced_crc16 %0x%02x", fcs);
    return fcs;
}

uint8_t *app_harman_ble_data_get(void)
{
    return (uint8_t *)&harman_adv_data[0];
}

uint8_t app_harman_ble_data_len_get(void)
{
    return sizeof(harman_adv_data);
}

extern T_LE_COMMON_ADV le_common_adv;
void harman_le_common_disc(void)
{
    APP_PRINT_INFO0("[SD_CHECK] harman_le_common_disc");
    //ble link disconnect
    if (le_common_adv.conn_id != 0xFF)
    {
        T_APP_LE_LINK *p_link;
        p_link = app_link_find_le_link_by_conn_id(le_common_adv.conn_id);

        if (p_link != NULL && (p_link->lea_device == 0))
        {
            app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_HARMAN_DEVICE_DISC);
        }
    }
}

void app_harman_le_common_adv_update(void)
{
    uint8_t adv_data_len = 0;
    uint16_t pid = app_harman_license_pid_get();
    uint8_t cid = app_harman_license_cid_get();

    APP_PRINT_TRACE1("app_harman_le_common_adv_update: before: %b",
                     TRACE_BINARY(sizeof(harman_adv_data), harman_adv_data));

    memcpy(&harman_adv_data[11], (uint8_t *)&pid, 2);
    harman_adv_data[15] = cid;
    memset(&harman_adv_data[16], 0, 4);

    harman_adv_data[13] = 0x00;
    harman_adv_data[14] = 0xF0;

    harman_adv_data[13] = 0x00;
    harman_adv_data[14] = 0xF0;

    memcpy(&harman_adv_data[16], &DEVICE_CRC1[0], sizeof(DEVICE_CRC1));
    memcpy(&harman_adv_data[18], &DEVICE_CRC2[0], sizeof(DEVICE_CRC2));
    memcpy(&harman_adv_data[20], &BT_MAC_CRC[0], sizeof(BT_MAC_CRC));

    harman_adv_data[7] = 0x0E;

    adv_data_len = harman_adv_data[7] + 8;
    app_harman_adv_data_update(harman_adv_data, adv_data_len);

    APP_PRINT_TRACE1("app_harman_le_common_adv_update:  after: %b",
                     TRACE_BINARY(sizeof(harman_adv_data), harman_adv_data));
}

void app_harman_remote_device_name_crc_set(uint8_t conn_id, uint8_t *bd_addr, uint8_t action)
{
    T_APP_BR_LINK *p_link;
    T_APP_LE_LINK *p_le_link;
    uint8_t id = 0;
    uint8_t device_name_crc[2];

    if (conn_id == 0xFF)
    {
        p_link = app_link_find_br_link(bd_addr);
        p_le_link = app_link_find_le_link_by_addr(bd_addr);

        if (p_link == NULL ||
            (p_le_link != NULL && action == false)) // le link exist
        {
            return;
        }

        APP_PRINT_TRACE2("app_harman_remote_device_name_crc_set: link_num: %d, id: %d",
                         app_link_get_b2s_link_num(), p_link->id);
        id = p_link->id;
        memcpy(device_name_crc, p_link->device_name_crc, sizeof(device_name_crc));
    }
    else
    {
        p_le_link = app_link_find_le_link_by_conn_id(conn_id);

        if (p_le_link != NULL)
        {
            p_link = app_link_find_br_link(p_le_link->bd_addr);
            if (p_link != NULL && action == false)   // legacy link exist
            {
                return;
            }
        }
        else
        {
            return;
        }

        APP_PRINT_TRACE1("app_harman_remote_device_name_crc_set: id: %d", p_le_link->id);
        id = p_le_link->id;
        memcpy(device_name_crc, p_le_link->device_name_crc, sizeof(device_name_crc));
    }

    if (action)
    {
        if (id == 0)
        {
            memcpy(&DEVICE_CRC1[0], device_name_crc, sizeof(DEVICE_CRC1));
        }
        else
        {
            memcpy(&DEVICE_CRC2[0], device_name_crc, sizeof(DEVICE_CRC2));
        }
    }
    else
    {
        if (id == 0)
        {
            memset(&DEVICE_CRC1[0], 0, sizeof(DEVICE_CRC1));
        }
        else
        {
            memset(&DEVICE_CRC2[0], 0, sizeof(DEVICE_CRC2));
        }
    }
}

static void harman_bt_mac_crc16_ibm(uint8_t *buf, uint32_t size)
{
    const uint32_t blksize = 32 * 1024;
    uint32_t addr = (uint32_t)buf;
    uint32_t blkcount  = size / blksize;
    uint32_t remainder = size % blksize;
    uint16_t calced_crc16 = 0;

    uint32_t i = 0;
    for (i = 0; i < blkcount; ++i)
    {
        calced_crc16 = harman_btxfcs(calced_crc16, (uint8_t *)(addr + i * blksize), blksize);
    }

    if (remainder)
    {
        calced_crc16 = harman_btxfcs(calced_crc16, (uint8_t *)(addr + i * blksize), remainder);
    }

    BT_MAC_CRC[0] = (uint8_t)(calced_crc16 & 0xff);
    BT_MAC_CRC[1] = (uint8_t)((calced_crc16 >> 8) & 0xff);

    APP_PRINT_TRACE1("harman_bt_mac_crc16_ibm: 0x%02x", calced_crc16);
}

uint16_t harman_crc16_ibm(uint8_t *buf, uint32_t size, uint8_t *bd_addr)
{
    const uint32_t blksize = 32 * 1024;
    uint32_t addr = (uint32_t)buf;
    uint32_t blkcount  = size / blksize;
    uint32_t remainder = size % blksize;
    uint16_t calced_crc16 = 0;

    uint32_t i = 0;
    for (i = 0; i < blkcount; ++i)
    {
        calced_crc16 = harman_btxfcs(calced_crc16, (uint8_t *)(addr + i * blksize), blksize);
    }

    if (remainder)
    {
        calced_crc16 = harman_btxfcs(calced_crc16, (uint8_t *)(addr + i * blksize), remainder);
    }

    return calced_crc16;
}

static void app_harman_ble_generate_bt_mac_crc(void)
{
    char local_bt_addr[6] = {0};
    uint32_t rcv_device_name_len = 0;
    uint8_t *crc_start_addr = NULL;

    gap_get_param(GAP_PARAM_BD_ADDR, local_bt_addr);
    harman_bt_mac_crc16_ibm((uint8_t *)&local_bt_addr[0], strlen(local_bt_addr));

    APP_PRINT_INFO1("app_harman_ble_generate_bt_mac_crc: bt_addr: %s", TRACE_BDADDR(local_bt_addr));
}

uint8_t *app_harman_ble_bt_mac_crc_get(void)
{
    app_harman_ble_generate_bt_mac_crc();

    return &BT_MAC_CRC[0];
}

uint8_t app_harman_ble_process(uint8_t entry_point, void *msg)
{
    uint8_t ret = 0;
    uint8_t *rcv_msg = msg;

    switch (entry_point)
    {
    case HARMAN_BLE_UPDATE_ADV_CRC:
        {
            uint8_t rcv_device_addr[6] = {0};
            char  rcv_device_name[31] = {0};

            memcpy(&rcv_device_addr[0], &rcv_msg[0], 6);
            memcpy(&rcv_device_name[0], &rcv_msg[6], 31);
            APP_PRINT_TRACE3("remote %s length %d bank_version %d", TRACE_STRING(rcv_device_name),
                             sizeof(rcv_device_name), app_ota_get_active_bank());
            uint32_t rcv_device_name_len = strlen(rcv_device_name);
            uint8_t *crc_start_addr = (uint8_t *)&rcv_device_name[0];

            harman_crc16_ibm(crc_start_addr, rcv_device_name_len, rcv_device_addr);
            APP_PRINT_TRACE4("crc_check change DEVICE_CRC1: size %d,data %b,DEVICE_CRC2: size %d,data %b",
                             sizeof(DEVICE_CRC1),
                             TRACE_BINARY(sizeof(DEVICE_CRC1), DEVICE_CRC1),
                             sizeof(DEVICE_CRC2),
                             TRACE_BINARY(sizeof(DEVICE_CRC2), DEVICE_CRC2));

            static const uint8_t CRC_NULL_CHECK[2] = {0};
            if (app_cfg_const.rtk_app_adv_support)
            {
                APP_PRINT_ERROR2("harman_enable_app_tool_adv2 b2s=%d,data %b", app_link_get_b2s_link_num(),
                                 TRACE_BINARY(sizeof(CRC_NULL_CHECK), CRC_NULL_CHECK));

                if ((memcmp(&DEVICE_CRC1[0], &CRC_NULL_CHECK[0], 2) ||
                     memcmp(&DEVICE_CRC2[0], &CRC_NULL_CHECK[0], 2)))
                {
                    app_ble_rtk_adv_start();
                }
            }
        }
        break;

    default:
        break;
    }

    APP_PRINT_TRACE2("app_harman_ble_process 0x%X, ret = %d", entry_point, ret);

    return ret;
}

void app_harman_ble_adv_stop(void)
{
    T_BLE_EXT_ADV_MGR_STATE adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;

    adv_state = app_ble_common_adv_get_state();

    if (adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_CONNECTED);
    }
}
#endif
