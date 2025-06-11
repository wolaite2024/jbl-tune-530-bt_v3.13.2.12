/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_BQB_CLI_SUPPORT

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "os_mem.h"
#include "os_msg.h"
#include "bt_types.h"
#include "cli.h"
#include "bqb.h"

#include "app_io_msg.h"
#include "app_main.h"

bool bqb_send_msg(T_IO_CONSOLE subtype, void *param_buf)
{
    T_IO_MSG msg;

    msg.type    = IO_MSG_TYPE_CONSOLE;
    msg.subtype = subtype;
    msg.u.buf   = param_buf;

    return app_io_msg_send(&msg);
}

bool bqb_reset(const char *cmd_str, char *buf, size_t buf_len)
{
    void *param_buf;

    (void)cmd_str;

    param_buf = malloc(2);
    if (param_buf != NULL)
    {
        LE_UINT16_TO_ARRAY(param_buf, BQB_CMD_RESET);
        (void)bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf);
    }

    snprintf(buf, buf_len, "Reset from BQB mode.\r\n");

    return false;
}

bool bqb_power(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    uint8_t     action;
    void       *param_buf;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    p_param[param_len] = '\0';

    if (!strcmp(p_param, "on"))
    {
        action = BQB_ACTION_POWER_ON;
    }
    else if (!strcmp(p_param, "off"))
    {
        action = BQB_ACTION_POWER_OFF;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, BQB_CMD_POWER);
        LE_UINT8_TO_STREAM(p, action);

        (void)bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf);
    }

    snprintf(buf, buf_len, "Power %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (on/off).\r\n", p_param);
    return false;
}

bool bqb_pair(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    uint8_t     action;
    void       *param_buf;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    p_param[param_len] = '\0';

    if (!strcmp(p_param, "start"))
    {
        action = BQB_ACTION_PAIR_START;
    }
    else if (!strcmp(p_param, "stop"))
    {
        action = BQB_ACTION_PAIR_STOP;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, BQB_CMD_PAIR);
        LE_UINT8_TO_STREAM(p, action);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "Pair %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (start/stop).\r\n", p_param);
    return false;
}

bool bqb_sdp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    uint8_t     action;
    uint16_t    uuid16;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    p_param[param_len] = '\0';

    action = BQB_ACTION_SDP_SEARCH;

    if (!strcmp(p_param, "uuid_l2cap"))
    {
        uuid16 = UUID_L2CAP;
    }
    else if (!strcmp(p_param, "uuid_avdtp"))
    {
        uuid16 = UUID_AVDTP;
    }
    else if (!strcmp(p_param, "uuid_a2dp"))
    {
        uuid16 = UUID_ADVANCED_AUDIO_DISTRIBUTION;
    }
    else if (!strcmp(p_param, "uuid_audio_source"))
    {
        uuid16 = UUID_AUDIO_SOURCE;
    }
    else
    {
        goto err;
    }

    p_param += param_len + 1;

    for (i = 0; i < 6; i++)
    {
        addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
    }

    param_buf = malloc(20);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, BQB_CMD_SDP);
        LE_UINT8_TO_STREAM(p, action);
        LE_UINT16_TO_STREAM(p, uuid16);
        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "SDP search %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (uuid).\r\n", p_param);
    return false;
}

bool bqb_avdtp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "open"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVDTP_OPEN;
    }
    else if (!strcmp(subcmd, "start"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVDTP_START;
    }
    else if (!strcmp(subcmd, "close"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVDTP_CLOSE;
    }
    else if (!strcmp(subcmd, "abort"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVDTP_ABORT;
    }
    else if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        subcmd += param_len + 1;

        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "signal"))
        {
            action = BQB_ACTION_AVDTP_CONNECT_SIGNAL;
        }
        else if (!strcmp(subcmd, "stream"))
        {
            action = BQB_ACTION_AVDTP_CONNECT_STREAM;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVDTP_DISCONNECT;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_AVDTP);
        LE_UINT8_TO_STREAM(p, action);
        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "AVDTP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (discover/connect signal|stream).\r\n", subcmd);
    return false;
}

bool bqb_avrcp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     vol;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_CONNECT;
    }
    else if (!strcmp(subcmd, "connect_controller"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_CONNECT_CONTROLLER;
    }
    else if (!strcmp(subcmd, "connect_target"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_CONNECT_TARGET;
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_DISCONNECT;
    }
    else if (!strcmp(subcmd, "get_play_status"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_GET_PLAY_STATUS;
    }
    else if (!strcmp(subcmd, "get_element_attr"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_GET_ELEMENT_ATTR;
    }
    else if (!strcmp(subcmd, "play"))
    {
        if (param_num != 7)
        {
            goto err;
        }
        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_PLAY;
    }
    else if (!strcmp(subcmd, "pause"))
    {
        if (param_num != 7)
        {
            goto err;
        }
        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_PAUSE;
    }
    else if (!strcmp(subcmd, "stop"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_STOP;
    }
    else if (!strcmp(subcmd, "rewind"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_REWIND;
    }
    else if (!strcmp(subcmd, "firstforward"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_FASTFORWARD;
    }
    else if (!strcmp(subcmd, "forward"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_FORWARD;
    }
    else if (!strcmp(subcmd, "backward"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_BACKWARD;
    }
    else if (!strcmp(subcmd, "notify_volume"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        vol = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_AVRCP_NOTIFY_VOLUME;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_AVRCP);
        LE_UINT8_TO_STREAM(p, action);

        if (action == BQB_ACTION_AVRCP_NOTIFY_VOLUME)
        {
            LE_UINT8_TO_STREAM(p, vol);
        }

        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "AVRCP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (connect/get_play_status/backward).\r\n", subcmd);
    return false;
}

bool bqb_rfcomm(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "spp"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_SPP;
        }
        else if (!strcmp(subcmd, "hfp"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_HFP;
        }
        else if (!strcmp(subcmd, "hsp"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_HSP;
        }
#if F_APP_BQB_CLI_HFP_AG_SUPPORT
        else if (!strcmp(subcmd, "hfp_ag"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_HFP_AG;
        }
        else if (!strcmp(subcmd, "hsp_ag"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_HSP_AG;
        }
#endif
        else if (!strcmp(subcmd, "pbap"))
        {
            action = BQB_ACTION_RFCOMM_CONNECT_PBAP;
        }
        else
        {
            goto err;
        }
        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "spp"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_SPP;
        }
        else if (!strcmp(subcmd, "hfp"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_HFP;
        }
        else if (!strcmp(subcmd, "hsp"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_HSP;
        }
#if F_APP_BQB_CLI_HFP_AG_SUPPORT
        else if (!strcmp(subcmd, "hfp_ag"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_HFP_AG;
        }
        else if (!strcmp(subcmd, "hsp_ag"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_HSP_AG;
        }
#endif
        else if (!strcmp(subcmd, "pbap"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_PBAP;
        }
        else if (!strcmp(subcmd, "all"))
        {
            action = BQB_ACTION_RFCOMM_DISCONNECT_ALL;
        }
        else
        {
            goto err;
        }
        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_RFCOMM);
        LE_UINT8_TO_STREAM(p, action);
        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "RFCOMM %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (RFCOMM connect/disconnect spp).\r\n", subcmd);
    return false;
}

bool bqb_hfhs(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    uint8_t     level;
    uint16_t    voice_setting;
    uint8_t     retrans_effort;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 8)
        {
            goto err;
        }
        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "sco"))
        {
            action = BQB_ACTION_HFHS_CONNECT_SCO;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 8)
        {
            goto err;
        }
        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "sco"))
        {
            action = BQB_ACTION_HFHS_DISCONNECT_SCO;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "call"))
    {
        if (param_num != 8)
        {
            goto err;
        }
        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "active"))
        {
            action = BQB_ACTION_HFHS_CALL_ACTIVE;
        }
        else if (!strcmp(subcmd, "end"))
        {
            action = BQB_ACTION_HFHS_CALL_END;
        }
        else if (!strcmp(subcmd, "reject"))
        {
            action = BQB_ACTION_HFHS_CALL_REJECT;
        }
        else if (!strcmp(subcmd, "answer"))
        {
            action = BQB_ACTION_HFHS_CALL_ANSWER;
        }
        else if (!strcmp(subcmd, "redial"))
        {
            action = BQB_ACTION_HFHS_CALL_REDIAL;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "voice_recognition"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "activate"))
        {
            action = BQB_ACTION_HFHS_VOICE_RECOGNITION_ACTIVATE;
        }
        else if (!strcmp(subcmd, "deactivate"))
        {
            action = BQB_ACTION_HFHS_VOICE_RECOGNITION_DEACTIVATE;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "spk_gain_level_report"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        level = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_SPK_GAIN_LEVEL_REPORT;
    }
    else if (!strcmp(subcmd, "mic_gain_level_report"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        level = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_MIC_GAIN_LEVEL_REPORT;
    }
    else if (!strcmp(subcmd, "sco_conn"))
    {
        if (param_num != 9)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        voice_setting = (uint16_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        retrans_effort = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_SCO_CONN_REQ;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_HFHS);
        LE_UINT8_TO_STREAM(p, action);

        if ((action == BQB_ACTION_HFHS_SPK_GAIN_LEVEL_REPORT) ||
            (action == BQB_ACTION_HFHS_MIC_GAIN_LEVEL_REPORT))
        {
            LE_UINT8_TO_STREAM(p, level);
        }
        else if (action == BQB_ACTION_HFHS_SCO_CONN_REQ)
        {
            LE_UINT16_TO_STREAM(p, voice_setting);
            LE_UINT8_TO_STREAM(p, retrans_effort);
        }

        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "HFHS %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (hfhs connect/disconnect).\r\n", subcmd);
    return false;
}

#if F_APP_BQB_CLI_HFP_AG_SUPPORT
bool bqb_hfhs_ag(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    uint8_t     level;
    uint16_t    ring_interval;
    uint8_t     ring_enable;
    uint8_t     clcc_call_idx;
    uint8_t     clcc_call_dir;
    uint8_t     clcc_call_status;
    uint8_t     clcc_call_mode;
    uint8_t     clcc_call_mpty;
    char        call_num[20] = {""};
    char        operator_name[17] = {""};
    uint8_t     call_type;
    uint8_t     service;
    uint8_t     indicator_value;
    uint16_t    max_latency;
    uint16_t    voice_setting;
    uint8_t     retrans_effort;
    uint8_t     service_status;
    uint8_t     call_status;
    uint8_t     call_setup_status;
    uint8_t     call_held_status;
    uint8_t     signal_status;
    uint8_t     roam_status;
    uint8_t     batt_chg_status;
    char        vnd_cmd[50] = {""};
    uint8_t     cme_error_code;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 8)
        {
            goto err;
        }
        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "sco"))
        {
            action = BQB_ACTION_HFHS_AG_CONNECT_SCO;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 8)
        {
            goto err;
        }
        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "sco"))
        {
            action = BQB_ACTION_HFHS_AG_DISCONNECT_SCO;
        }
        else
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else if (!strcmp(subcmd, "call_incoming"))
    {
        if (param_num != 9)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 19)
        {
            param_len = 19;
        }
        memcpy(call_num, p_param, param_len);
        p_param += param_len + 1;
        call_type = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_INCOMING;
    }
    else if (!strcmp(subcmd, "call_answer"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_ANSWER;
    }
    else if (!strcmp(subcmd, "call_terminate"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_TERMINATE;
    }
    else if (!strcmp(subcmd, "call_dial"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_DIAL;
    }
    else if (!strcmp(subcmd, "call_alerting"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_ALERTING;
    }
    else if (!strcmp(subcmd, "call_waiting"))
    {
        if (param_num != 9)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 19)
        {
            param_len = 19;
        }
        memcpy(call_num, p_param, param_len);
        p_param += param_len + 1;
        call_type = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_WAITING;
    }
    else if (!strcmp(subcmd, "mic_gain_level_report"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        level = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_MIC_GAIN_LEVEL_REPORT ;
    }
    else if (!strcmp(subcmd, "spk_gain_level_report"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        level = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_SPEAKER_GAIN_LEVEL_REPORT;
    }
    else if (!strcmp(subcmd, "ring_interval_set"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        ring_interval = (uint16_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_RING_INTERVAL_SET;
    }
    else if (!strcmp(subcmd, "inband_ringing_set"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        ring_enable = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_INBAND_RINGING_SET;
    }
    else if (!strcmp(subcmd, "clcc_send"))
    {
        if (param_num != 14)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        clcc_call_idx = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        clcc_call_dir = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        clcc_call_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        clcc_call_mode = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        clcc_call_mpty = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 19)
        {
            param_len = 19;
        }
        memcpy(call_num, p_param, param_len);
        p_param += param_len + 1;
        call_type = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CURRENT_CALLS_LIST_SEND;
    }
    else if (!strcmp(subcmd, "service_status_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_SEVICE_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "call_status_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_CALL_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "call_setup_status_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_CALL_SETUP_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "call_held_status_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_CALL_HELD_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "signal_strength_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_SIGNAL_STRENGTH_SEND;
    }
    else if (!strcmp(subcmd, "roaming_status_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_ROAMING_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "battery_change_indicator_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        indicator_value = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_BATTERY_CHANGE_SEND;
    }
    else if (!strcmp(subcmd, "ok_send"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_OK_SEND;
    }
    else if (!strcmp(subcmd, "error_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        cme_error_code = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_ERROR_SEND;
    }
    else if (!strcmp(subcmd, "operator_name_send"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 16)
        {
            param_len = 16;
        }
        memcpy(operator_name, p_param, param_len);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_NETWORK_OPERATOR_SEND;
    }
    else if (!strcmp(subcmd, "subscriber_number_send"))
    {
        if (param_num != 10)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 19)
        {
            param_len = 19;
        }
        memcpy(call_num, p_param, param_len);
        p_param += param_len + 1;
        call_type = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        service = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_SUBSCRIBER_NUMBER_SEND;
    }
    else if (!strcmp(subcmd, "indicators_status_send"))
    {
        if (param_num != 14)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        service_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        call_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        call_setup_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        call_held_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        signal_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        roam_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        batt_chg_status = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_INDICATORS_STATUS_SEND;
    }
    else if (!strcmp(subcmd, "vendor_cmd_send"))
    {
        int res;
        int offset;

        p_param = subcmd + param_len + 1;
        res = sscanf(p_param, "\"%49[^\"]\"%n", vnd_cmd, &offset);
        if ((res < 1) || (offset <= 0))
        {
            goto err;
        }
        p_param += offset;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action =  BQB_ACTION_HFHS_AG_VENDOR_CMD_SEND;
    }
    else if (!strcmp(subcmd, "sco_conn"))
    {
        if (param_num != 10)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        max_latency = (uint16_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        voice_setting = (uint16_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        retrans_effort = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_HFHS_AG_SCO_CONN_REQ;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(80);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_HFHS_AG);
        LE_UINT8_TO_STREAM(p, action);

        if ((action == BQB_ACTION_HFHS_AG_MIC_GAIN_LEVEL_REPORT) ||
            (action == BQB_ACTION_HFHS_AG_SPEAKER_GAIN_LEVEL_REPORT))
        {
            LE_UINT8_TO_STREAM(p, level);
        }
        else if (action == BQB_ACTION_HFHS_AG_CALL_INCOMING)
        {
            memcpy(p, call_num, strlen(call_num) + 1);
            p += strlen(call_num) + 1;
            LE_UINT8_TO_STREAM(p, call_type);
        }
        else if (action == BQB_ACTION_HFHS_AG_CALL_WAITING)
        {
            memcpy(p, call_num, strlen(call_num) + 1);
            p += strlen(call_num) + 1;
            LE_UINT8_TO_STREAM(p, call_type);
        }
        else if (action == BQB_ACTION_HFHS_AG_NETWORK_OPERATOR_SEND)
        {
            memcpy(p, operator_name, strlen(operator_name) + 1);
            p += strlen(operator_name) + 1;
        }
        else if (action == BQB_ACTION_HFHS_AG_RING_INTERVAL_SET)
        {
            LE_UINT16_TO_STREAM(p, ring_interval);
        }
        else if (action == BQB_ACTION_HFHS_AG_INBAND_RINGING_SET)
        {
            LE_UINT8_TO_STREAM(p, ring_enable);
        }
        else if (action == BQB_ACTION_HFHS_AG_CURRENT_CALLS_LIST_SEND)
        {
            LE_UINT8_TO_STREAM(p, clcc_call_idx);
            LE_UINT8_TO_STREAM(p, clcc_call_dir);
            LE_UINT8_TO_STREAM(p, clcc_call_status);
            LE_UINT8_TO_STREAM(p, clcc_call_mode);
            LE_UINT8_TO_STREAM(p, clcc_call_mpty);
            memcpy(p, call_num, strlen(call_num) + 1);
            p += strlen(call_num) + 1;
            LE_UINT8_TO_STREAM(p, call_type);
        }
        else if ((action == BQB_ACTION_HFHS_AG_SEVICE_STATUS_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_CALL_STATUS_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_CALL_SETUP_STATUS_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_CALL_HELD_STATUS_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_SIGNAL_STRENGTH_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_ROAMING_STATUS_SEND) ||
                 (action == BQB_ACTION_HFHS_AG_BATTERY_CHANGE_SEND))
        {
            LE_UINT8_TO_STREAM(p, indicator_value);
        }
        else if (action == BQB_ACTION_HFHS_AG_SUBSCRIBER_NUMBER_SEND)
        {
            memcpy(p, call_num, strlen(call_num) + 1);
            p += strlen(call_num) + 1;
            LE_UINT8_TO_STREAM(p, call_type);
            LE_UINT8_TO_STREAM(p, service);
        }
        else if (action == BQB_ACTION_HFHS_AG_INDICATORS_STATUS_SEND)
        {
            LE_UINT8_TO_STREAM(p, service_status);
            LE_UINT8_TO_STREAM(p, call_status);
            LE_UINT8_TO_STREAM(p, call_setup_status);
            LE_UINT8_TO_STREAM(p, call_held_status);
            LE_UINT8_TO_STREAM(p, signal_status);
            LE_UINT8_TO_STREAM(p, roam_status);
            LE_UINT8_TO_STREAM(p, batt_chg_status);
        }
        else if (action == BQB_ACTION_HFHS_AG_VENDOR_CMD_SEND)
        {
            memcpy(p, vnd_cmd, strlen(vnd_cmd) + 1);
            p += strlen(vnd_cmd) + 1;
        }
        else if (action == BQB_ACTION_HFHS_AG_ERROR_SEND)
        {
            LE_UINT8_TO_STREAM(p, cme_error_code);
        }
        else if (action == BQB_ACTION_HFHS_AG_SCO_CONN_REQ)
        {
            LE_UINT16_TO_STREAM(p, max_latency);
            LE_UINT16_TO_STREAM(p, voice_setting);
            LE_UINT8_TO_STREAM(p, retrans_effort);
        }

        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "HFHS AG %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (hfhs ag connect/disconnect).\r\n", subcmd);
    return false;
}
#endif

bool bqb_pbap(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param = NULL;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "vcard"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        subcmd += param_len + 1;
        subcmd = (char *)cli_param_get(subcmd, 0, &param_len);
        subcmd[param_len] = '\0';

        if (!strcmp(subcmd, "srm"))
        {
            action = BQB_ACTION_PBAP_VCARD_SRM;
        }
        else if (!strcmp(subcmd, "nosrm"))
        {
            action = BQB_ACTION_PBAP_VCARD_NOSRM;
        }
        else if (!strcmp(subcmd, "entry"))
        {
            action = BQB_ACTION_PBAP_VCARD_ENTRY;
        }
        else
        {
            goto err;
        }
        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_PBAP);
        LE_UINT8_TO_STREAM(p, action);
        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    if (param_buf != NULL)
    {
        snprintf(buf, buf_len, "PBAP %s from BQB mode.\r\n", p_param);
    }
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (PBAP connect/disconnect rfcomm).\r\n", subcmd);
    return false;
}

#if (F_APP_BQB_CLI_MAP_SUPPORT == 1)
bool bqb_map(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     addr[6];
    uint8_t     i;
    void       *param_buf;
    uint16_t    upload_repeat = 0;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "connect"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_CONNECT_MAS;
    }
    else if (!strcmp(subcmd, "disconnect"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_DISCONNECT_MAS;
    }
    else if (!strcmp(subcmd, "mns_on"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_MNS_ON;
    }
    else if (!strcmp(subcmd, "mns_off"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_MNS_OFF;
    }
    else if (!strcmp(subcmd, "folder_listing"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_FOLDER_LISTING;
    }
    else if (!strcmp(subcmd, "set_folder_inbox"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_SET_FOLDER_INBOX;
    }
    else if (!strcmp(subcmd, "set_folder_outbox"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_SET_FOLDER_OUTBOX;
    }
    else if (!strcmp(subcmd, "msg_listing"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_MSG_LISTING;
    }
    else if (!strcmp(subcmd, "get_msg"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_GET_MSG;
    }
    else if (!strcmp(subcmd, "upload"))
    {
        if (param_num != 8)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;
        upload_repeat = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param = p_param + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_UPLOAD;
    }
    else if (!strcmp(subcmd, "upload_srm"))
    {
        if (param_num != 7)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 6; i++)
        {
            addr[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 0);
        }

        action = BQB_ACTION_MAP_UPLOAD_SRM;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_MAP);
        LE_UINT8_TO_STREAM(p, action);
        if (action == BQB_ACTION_MAP_UPLOAD)
        {
            upload_repeat = upload_repeat * 100;
            LE_UINT16_TO_STREAM(p, upload_repeat);
        }
        ARRAY_TO_STREAM(p, addr, 6);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "MAP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (MAP connect/disconnect rfcomm).\r\n", subcmd);
    return false;
}
#else
bool bqb_map(const char *cmd_str, char *buf, size_t buf_len)
{
    return false;
}
#endif

bool bqb_mcp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     i = 0;
    void       *param_buf;
    uint8_t     conn_id;
    uint8_t     srv_idx;
    uint8_t     general;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "play"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_PLAY;
    }
    else if (!strcmp(subcmd, "pause"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_PAUSE;
    }
    else if (!strcmp(subcmd, "stop"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_STOP;
    }
    else if (!strcmp(subcmd, "fastforward"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_FASTFORWARD;
    }
    else if (!strcmp(subcmd, "rewind"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_REWIND;
    }
    else if (!strcmp(subcmd, "fastforward_stop"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_FASTFORWARD_STOP;
    }
    else if (!strcmp(subcmd, "rewind_stop"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_REWIND_STOP;
    }
    else if (!strcmp(subcmd, "fwd"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_FORWARD;
    }
    else if (!strcmp(subcmd, "bwd"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_MCP_BACKWARD;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_MCP);
        LE_UINT8_TO_STREAM(p, action);
        LE_UINT8_TO_STREAM(p, conn_id);
        LE_UINT8_TO_STREAM(p, srv_idx);
        LE_UINT8_TO_STREAM(p, general);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "MAP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (play/pause).\r\n", subcmd);
    return false;
}

bool bqb_ccp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     i = 0;
    void       *param_buf;
    uint8_t     conn_id;
    uint8_t     srv_idx;
    uint8_t     call_idx;
    bool        is_cmd;
    uint8_t     general;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "accept"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        call_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        is_cmd = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_CAP_ACCEPT;
    }
    else if (!strcmp(subcmd, "terminate"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        call_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        is_cmd = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_CAP_TERMINATE;
    }
    else if (!strcmp(subcmd, "local_hold"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        call_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        is_cmd = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_CAP_LOCAL_HOLD;
    }
    else if (!strcmp(subcmd, "local_retrieve"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        conn_id = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        srv_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        call_idx = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        is_cmd = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        general = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_CAP_LOCAL_RETRIEVE;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_CCP);
        LE_UINT8_TO_STREAM(p, action);
        LE_UINT8_TO_STREAM(p, conn_id);
        LE_UINT8_TO_STREAM(p, srv_idx);
        LE_UINT8_TO_STREAM(p, call_idx);
        LE_UINT8_TO_STREAM(p, is_cmd);
        LE_UINT8_TO_STREAM(p, general);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "CCP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (accept/terminate).\r\n", subcmd);
    return false;
}

bool bqb_vcs(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     i = 0;
    void       *param_buf;
    uint8_t     volume_setting;
    uint8_t     mute;
    uint8_t     change_counter;
    uint8_t     volume_flags;
    uint8_t     step_size;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "up"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        volume_setting = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        mute = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        change_counter = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        volume_flags = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        step_size = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_VCS_UP;
    }
    else if (!strcmp(subcmd, "down"))
    {
        if (param_num != 6)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        volume_setting = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        mute = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        change_counter = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        volume_flags = (uint8_t)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);
        step_size = (bool)strtol(cli_param_get(p_param, i++, &param_len), NULL, 0);

        action = BQB_ACTION_VCS_DOWN;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_VCS);
        LE_UINT8_TO_STREAM(p, action);
        LE_UINT8_TO_STREAM(p, volume_setting);
        LE_UINT8_TO_STREAM(p, mute);
        LE_UINT8_TO_STREAM(p, change_counter);
        LE_UINT8_TO_STREAM(p, volume_flags);
        LE_UINT8_TO_STREAM(p, step_size);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "VCS %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (up/down).\r\n", subcmd);
    return false;
}

bool bqb_pbp(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     broadcast_code[16];
    uint8_t     action;
    uint8_t     i;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "bmr_start"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_PBP_BMR_START;
    }
    else if (!strcmp(subcmd, "bmr_stop"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_PBP_BMR_STOP;
    }
    else if (!strcmp(subcmd, "broadcast_code"))
    {
        if (param_num != 17)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        for (i = 0; i < 16; i++)
        {
            broadcast_code[i] = (uint8_t)strtol(cli_param_get(p_param, i, &param_len), NULL, 16);
        }

        action = BQB_ACTION_PBP_BROADCAST_CODE;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_PBP);
        LE_UINT8_TO_STREAM(p, action);
        ARRAY_TO_STREAM(p, broadcast_code, 16);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "PBP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (broadcast code).\r\n", subcmd);
    return false;
}

bool bqb_tmap(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     gaming_mode;
    void       *param_buf;

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "adv_start"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_TMAP_ADV_START;
    }
    else if (!strcmp(subcmd, "adv_stop"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_TMAP_ADV_STOP;
    }
    else if (!strcmp(subcmd, "gaming_mode"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        gaming_mode = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_TMAP_GAMING_MODE;
    }
    else if (!strcmp(subcmd, "originate"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_TMAP_ORIGINATE_CALL;
    }
    else if (!strcmp(subcmd, "terminate"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_TMAP_TERMINATE_CALL;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_TMAP);
        LE_UINT8_TO_STREAM(p, action);
        LE_UINT8_TO_STREAM(p, gaming_mode);

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "TMAP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (gaming mode).\r\n", subcmd);
    return false;
}

bool bqb_hap(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *subcmd;
    char       *p_param;
    int32_t     param_num;
    uint32_t    param_len;
    uint8_t     action;
    uint8_t     index;
    void       *param_buf;
    uint8_t     feature;
    uint8_t     preset_prop;
    char        name[20] = {""};

    param_num = cli_param_num_get(cmd_str);
    if (param_num < 1)
    {
        subcmd = (char *)cmd_str;
        goto err;
    }

    subcmd = (char *)cli_param_get(cmd_str, 1, &param_len);
    subcmd[param_len] = '\0';

    if (!strcmp(subcmd, "has_feature"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        feature = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_FEATURE;
    }
    else if (!strcmp(subcmd, "active_index"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_ACTIVE_INDEX;
    }
    else if (!strcmp(subcmd, "preset_name"))
    {
        if (param_num != 3)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);
        p_param += param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 20)
        {
            memcpy(name, p_param, 20);
        }
        else
        {
            memcpy(name, p_param, param_len);
        }

        action = BQB_ACTION_HAP_PRESET_NAME;
    }
    else if (!strcmp(subcmd, "preset_available"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_PRESET_AVAILABLE;
    }
    else if (!strcmp(subcmd, "preset_unavailable"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_PRESET_UNAVAILABLE;
    }
    else if (!strcmp(subcmd, "preset_add"))
    {
        if (param_num != 4)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        p_param += param_len + 1;
        cli_param_get(p_param, 0, &param_len);
        if (param_len > 20)
        {
            memcpy(name, p_param, 20);
        }
        else
        {
            memcpy(name, p_param, param_len);
        }

        p_param += param_len + 1;
        preset_prop = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_PRESET_ADD;
    }
    else if (!strcmp(subcmd, "preset_delete"))
    {
        if (param_num != 2)
        {
            goto err;
        }

        p_param = subcmd + param_len + 1;

        index = (uint8_t)strtol(cli_param_get(p_param, 0, &param_len), NULL, 0);

        action = BQB_ACTION_HAP_PRESET_DELETE;
    }
    else if (!strcmp(subcmd, "rtk_adv_start"))
    {
        p_param = subcmd + param_len + 1;

        action = BQB_ACTION_HAP_RTK_ADV_START;
    }
    else
    {
        goto err;
    }

    param_buf = malloc(40);
    if (param_buf != NULL)
    {
        uint8_t    *p;

        p = param_buf;
        LE_UINT16_TO_STREAM(p, BQB_CMD_HAP);
        LE_UINT8_TO_STREAM(p, action);
        if (!strcmp(subcmd, "has_feature"))
        {
            LE_UINT8_TO_STREAM(p, feature);
        }
        else
        {
            LE_UINT8_TO_STREAM(p, index);
        }

        if (!strcmp(subcmd, "preset_name") || !strcmp(subcmd, "preset_add"))
        {
            LE_UINT8_TO_STREAM(p, strlen(name));
            ARRAY_TO_STREAM(p, name, strlen(name));
            LE_UINT8_TO_STREAM(p, preset_prop);
        }

        if (bqb_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "HAP %s from BQB mode.\r\n", p_param);
    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (gaming mode).\r\n", subcmd);
    return false;
}

#endif
