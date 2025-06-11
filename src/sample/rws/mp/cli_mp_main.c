
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CLI_STRING_MP_SUPPORT
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "cli.h"
#include "test_mode.h"



static bool mp_help(const char *cmd_str, char *p_buf, size_t buf_len);

/*
 * The callback function that is executed when "exit" is entered.
 * This is the default subcommand that is always present.
 */
static bool mp_exit(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Exit from mp mode...\r\n");

    return false;
}

/*
 * The callback function that is executed when "mp" is entered.
 */
static bool mp_mode(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Enter mp mode...\r\n");

    return false;
}

/*
 * The callback function that is executed when "hci_mode" is entered.
 */
static bool mp_hci(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Enter hci mode...\r\n");

    switch_into_hci_mode();
    return false;
}

/*
 * The definition of the "hci_mode" subcommand of mp.
 */
const static T_CLI_CMD mp_cmd_hci  =
{
    NULL,           /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "hci_mode",     /* The command string to type. */
    "\r\nhci_mode:\r\n Enter hci mode\r\n",
    mp_hci,         /* The function to run. */
    0,              /* One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "exit" subcommand of mp.
 * This command is always registered next to "help" command.
 */
const static T_CLI_CMD mp_cmd_exit  =
{
    &mp_cmd_hci,    /* Next command pointed to "reset". */
    NULL,           /* Next subcommand. */
    "exit",         /* The command string to type. */
    "\r\nexit:\r\n Exits from mp mode\r\n",
    mp_exit,        /* The function to run. */
    0,              /* Zero parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "help" subcommand of mp.
 * This command is always at the front of the list of registered commands.
 */
const static T_CLI_CMD mp_cmd_help  =
{
    &mp_cmd_exit,   /* Next command pointed to "exit" command. */
    NULL,           /* Next subcommand. */
    "help",         /* The command string to type. */
    "\r\nhelp:\r\n Lists all the registered commands\r\n",
    mp_help,        /* The function to run. */
    -1,             /* Zero or One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "mp" command.
 * This command should be registered into file system command list.
 */
static T_CLI_CMD mp_cmd_entry  =
{
    NULL,          /* Next command will be determined when registered into file system. */
    &mp_cmd_help,  /* Next subcommand pointer to "help" subcommand. */
    "mp",          /* The command string to type. */
    "\r\nmp:\r\n Enters mp mode\r\n",
    mp_mode,       /* The function to run. */
    0,             /* Zero or One parameter is expected. */
    NULL           /* Default command match pattern. */
};

/*
 * The callback function that is executed when "help" is entered.
 * This is the default subcommand that is always present.
 */
static bool mp_help(const char *cmd_str, char *p_buf, size_t buf_len)
{
    return cli_help_set(&mp_cmd_help, cmd_str, p_buf, buf_len);
}

bool mp_cmd_str_register(void)
{
    return cli_cmd_register(&mp_cmd_entry);
}
#endif
