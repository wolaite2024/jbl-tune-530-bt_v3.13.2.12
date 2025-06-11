/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_LE_AUDIO_CLI_TEST
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os_msg.h"
#include "cli.h"
#include "gap.h"
#include "app_msg.h"
#include "app_main.h"
#include "cli_le_audio.h"
/*============================================================================*
 *                              Internal Test Files
 *============================================================================*/
bool test_send_msg(T_IO_CONSOLE subtype, void *param_buf)
{
    uint8_t  event;
    T_IO_MSG msg;

    event = EVENT_IO_TO_APP;

    msg.type    = IO_MSG_TYPE_CONSOLE;
    msg.subtype = subtype;
    msg.u.buf   = param_buf;

    if (os_msg_send(audio_evt_queue_handle, &event, 0) == true)
    {
        return os_msg_send(audio_io_queue_handle, &msg, 0);
    }

    return false;
}

static bool audio_bigspath(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     bis_conn_handle_low;
    uint8_t     bis_conn_handle_high;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    bis_conn_handle_low = (uint8_t)strtol(p_param, NULL, 0);
    p_param += param_len + 1;

    p_param = (char *)cli_param_get(p_param, 0, &param_len);
    bis_conn_handle_high = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(4);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BIGSPATH);
        LE_UINT8_TO_STREAM(p, bis_conn_handle_low);
        LE_UINT8_TO_STREAM(p, bis_conn_handle_high);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, " bis_conn_handle_low 0x%x, bis_conn_handle_high 0x%x\r\n",
             bis_conn_handle_low, bis_conn_handle_high);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}
static bool audio_bigrpath(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     bis_conn_handle_low;
    uint8_t     bis_conn_handle_high;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    bis_conn_handle_low = (uint8_t)strtol(p_param, NULL, 0);
    p_param += param_len + 1;

    p_param = (char *)cli_param_get(p_param, 0, &param_len);
    bis_conn_handle_high = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(4);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BIGRPATH);
        LE_UINT8_TO_STREAM(p, bis_conn_handle_low);
        LE_UINT8_TO_STREAM(p, bis_conn_handle_high);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_paterminate(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     dev_idx;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    dev_idx = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_PATERMINATE);
        LE_UINT8_TO_STREAM(p, dev_idx);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "dev_idx %d \r\n", dev_idx);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;
}

static bool audio_bigterminate(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     dev_idx;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    dev_idx = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BIGTERMINATE);
        LE_UINT8_TO_STREAM(p, dev_idx);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "dev_idx: %d \r\n", dev_idx);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_bisoterminate(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     source_id;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    source_id = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BISOTERMINATE);
        LE_UINT8_TO_STREAM(p, source_id);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "source_id: %d \r\n", source_id);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_bpaterminate(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     source_id;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    source_id = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BPATERMINATE);
        LE_UINT8_TO_STREAM(p, source_id);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "source_id: %d \r\n", source_id);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_a2dp_pause(const char *cmd_str, char *buf, size_t buf_len)
{
    void       *param_buf;
    (void)cmd_str;

    param_buf = malloc(2);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_A2DP_PAUSE);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "a2dp pause \r\n");
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;
}

static bool audio_startescan(const char *cmd_str, char *buf, size_t buf_len)
{
    void       *param_buf;
    char       *p_param;
    uint32_t    param_len;
    uint8_t     bis_idx;
    (void)cmd_str;
    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    bis_idx = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_STARTESCAN);
        LE_UINT8_TO_STREAM(p, bis_idx);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }

    snprintf(buf, buf_len, "start escan bis %d\r\n", bis_idx);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;
}

static bool audio_stopescan(const char *cmd_str, char *buf, size_t buf_len)
{
    void       *param_buf;
    (void)cmd_str;
    param_buf = malloc(2);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_STOPESCAN);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "stop escan \r\n");
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;
}

static bool audio_cis_adv(const char *cmd_str, char *buf, size_t buf_len)
{
    void       *param_buf;
    (void)cmd_str;
    param_buf = malloc(2);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_CIS_ADV);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "cis adv \r\n");
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;
}

static bool audio_bigsync(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     dev_idx;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    dev_idx = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_BIGSYNC);
        LE_UINT8_TO_STREAM(p, dev_idx);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "dev_idx: %d \r\n", dev_idx);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_pasync(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;
    void       *param_buf;
    uint8_t     dev_idx;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    dev_idx = (uint8_t)strtol(p_param, NULL, 0);

    param_buf = malloc(3);
    if (param_buf != NULL)
    {
        uint8_t *p;

        p = param_buf;

        LE_UINT16_TO_STREAM(p, AUDIO_CMD_PASYNC);
        LE_UINT8_TO_STREAM(p, dev_idx);

        if (test_send_msg(IO_MSG_CONSOLE_STRING_RX, param_buf) == false)
        {
            free(param_buf);
            goto err;
        }
    }
    snprintf(buf, buf_len, "dev_idx %d \r\n", dev_idx);
    return false;
err:
    snprintf(buf, buf_len, "Failed \r\n");
    return false;

}

static bool audio_mode(const char *cmd_str, char *buf, size_t buf_len)
{

    return false;
}


const static T_CLI_CMD audio_cmd_bigspath  =
{
    NULL,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bigspath",         /* The command string to type. */
    "\r\nbig set path:\r\n example:bigspath [bis_conn_handle_low] [bis_conn_handle_high] \r\n",
    audio_bigspath,       /* The function to run. */
    2              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_bigrpath  =
{
    &audio_cmd_bigspath,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bigrpath",         /* The command string to type. */
    "\r\nbig remove path:\r\n example:bigrpath [bis_conn_handle_low] [bis_conn_handle_high] \r\n",
    audio_bigrpath,       /* The function to run. */
    2              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_bpaterminate  =
{
    &audio_cmd_bigrpath,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bpaterminate",         /* The command string to type. */
    "\r\nbass pa terminate:\r\n example:bpaterminate [source_id] \r\n",
    audio_bpaterminate,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_bisoterminate  =
{
    &audio_cmd_bpaterminate,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bisoterminate",         /* The command string to type. */
    "\r\nbass biso terminate:\r\n example:bisoterminate [source_id] \r\n",
    audio_bisoterminate,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_bigterminate  =
{
    &audio_cmd_bisoterminate,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bigterminate",         /* The command string to type. */
    "\r\nbroadcast source big terminate:\r\n example:bigterminate [dev_idx] \r\n",
    audio_bigterminate,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_paterminate  =
{
    &audio_cmd_bigterminate,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "paterminate",         /* The command string to type. */
    "\r\nbroadcast source pa terminate:\r\n example:paterminate [dev_idx] \r\n",
    audio_paterminate,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_bigsync  =
{
    &audio_cmd_paterminate,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "bigsync",         /* The command string to type. */
    "\r\nbigsync:\r\n example:bigsync [dev_idx] \r\n",
    audio_bigsync,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_pasync  =
{
    &audio_cmd_bigsync,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "pasync",         /* The command string to type. */
    "\r\nstart pasync:\r\n example:pasync [dev_idx] \r\n",
    audio_pasync,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_a2dp_pause  =
{
    &audio_cmd_pasync,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "a2dp_pause",         /* The command string to type. */
    "\r\nstart a2dp_pause:\r\n example:a2dp_pause \r\n",
    audio_a2dp_pause,       /* The function to run. */
    0              /* One parameter is expected. */
};


const static T_CLI_CMD audio_cmd_startescan  =
{
    &audio_cmd_a2dp_pause,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "startescan",         /* The command string to type. */
    "\r\nstart escan:\r\n example:startescan \r\n",
    audio_startescan,       /* The function to run. */
    1              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_stopescan  =
{
    &audio_cmd_startescan,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "stopescan",         /* The command string to type. */
    "\r\nstopescan:\r\n example:stopescan \r\n",
    audio_stopescan,       /* The function to run. */
    0              /* One parameter is expected. */
};

const static T_CLI_CMD audio_cmd_cis_adv  =
{
    &audio_cmd_stopescan,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "cisadv",         /* The command string to type. */
    "\r\ncisadv:\r\n example:cisadv \r\n",
    audio_cis_adv,       /* The function to run. */
    0              /* One parameter is expected. */
};


static T_CLI_CMD audio_cmd_entry  =
{
    NULL,           /* Next command will be determined when registered into file system. */
    &audio_cmd_cis_adv,  /* Next subcommand pointer to "help" subcommand. */
    "leaudio",          /* The command string to type. */
    "\r\naudio:\r\n start audio test cmd\r\n",
    audio_mode,       /* The function to run. */
    0               /* Zero or One parameter is expected. */
};

bool le_audio_cmd_register(void)
{
    return cli_cmd_register(&audio_cmd_entry);
}
#endif

