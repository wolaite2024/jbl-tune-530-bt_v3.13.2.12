
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CLI_CFG_SUPPORT
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "cli.h"
#include "cli_cfg_cmd.h"

static bool cfg_help(const char *cmd_str, char *p_buf, size_t buf_len);


/*
 * The callback function that is executed when "exit" is entered.
 * This is the default subcommand that is always present.
 */
static bool cfg_exit(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Exit from cfg mode...\r\n");

    return false;
}

static bool cfg_mode(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Enter cfg mode...\r\n");

    return false;
}

const static T_CLI_CMD cfg_cmd_dump  =
{
    NULL,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "dump",         /* The command string to type. */
    "\r\ndump:\r\n example:dump app/appnv/sys/sysnv\r\n",
    cfg_dump,       /* The function to run. */
    1,              /* One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "exit" subcommand of bud.
 * This command is always registered next to "help" command.
 */
const static T_CLI_CMD cfg_cmd_exit  =
{
    &cfg_cmd_dump, /* Next command pointed to "reset". */
    NULL,           /* Next subcommand. */
    "exit",         /* The command string to type. */
    "\r\nexit:\r\n Exits from cfg mode\r\n",
    cfg_exit,       /* The function to run. */
    0,              /* Zero parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "help" subcommand of bud.
 * This command is always at the front of the list of registered commands.
 */
const static T_CLI_CMD cfg_cmd_help  =
{
    &cfg_cmd_exit,  /* Next command pointed to "exit" command. */
    NULL,           /* Next subcommand. */
    "help",         /* The command string to type. */
    "\r\nhelp:\r\n Lists all the registered commands\r\n",
    cfg_help,       /* The function to run. */
    -1,             /* Zero or One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "bud" command.
 * This command should be registered into file system command list.
 */
static T_CLI_CMD cfg_cmd_entry  =
{
    NULL,           /* Next command will be determined when registered into file system. */
    &cfg_cmd_help,  /* Next subcommand pointer to "help" subcommand. */
    "cfg",          /* The command string to type. */
    "\r\ncfg:\r\n Enters cfg mode\r\n",
    cfg_mode,       /* The function to run. */
    0,              /* Zero or One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The callback function that is executed when "help" is entered.
 * This is the default subcommand that is always present.
 */
static bool cfg_help(const char *cmd_str, char *p_buf, size_t buf_len)
{
    return cli_help_set(&cfg_cmd_help, cmd_str, p_buf, buf_len);
}

bool cfg_cmd_register(void)
{
    return cli_cmd_register(&cfg_cmd_entry);
}
#endif

