
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CLI_CFG_SUPPORT
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "os_mem.h"
#include "os_msg.h"
#include "cli.h"


bool cfg_dump(const char *cmd_str, char *buf, size_t buf_len)
{
    char       *p_param;
    uint32_t    param_len;

    p_param = (char *)cli_param_get(cmd_str, 1, &param_len);
    p_param[param_len] = '\0';

    if (!strcmp(p_param, "app"))
    {
        snprintf(buf, buf_len, "cfg_dump: %s from cfg mode.\r\n", p_param);
    }
    else if (!strcmp(p_param, "appnv"))
    {
        snprintf(buf, buf_len, "cfg_dump: %s from cfg mode.\r\n", p_param);
    }
    else if (!strcmp(p_param, "sys"))
    {
        snprintf(buf, buf_len, "cfg_dump: %s from cfg mode.\r\n", p_param);
    }
    else if (!strcmp(p_param, "sysnv"))
    {
        snprintf(buf, buf_len, "cfg_dump: %s from cfg mode.\r\n", p_param);
    }
    else if (!strcmp(p_param, "mem"))
    {
        snprintf(buf, buf_len, "cfg_dump: Heap %d.\r\n", mem_peek());
    }
    else
    {
        goto err;
    }


    return false;

err:
    snprintf(buf, buf_len, "Invalid param %s (app/appnv/sys/sysnv).\r\n", p_param);
    return false;
}
#endif

