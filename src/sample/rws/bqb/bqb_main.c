/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_BQB_CLI_SUPPORT

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "cli.h"
#include "bqb_cmd.h"

static bool bqb_help(const char *cmd_str, char *buf, size_t buf_len);

/*
 * The callback function that is executed when "bqb" is entered.
 * This is the default command that is always present.
 */
static bool bqb_mode(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Enter into BQB mode...\r\n");

    return false;
}

/*
 * The callback function that is executed when "exit" is entered.
 * This is the default subcommand that is always present.
 */
static bool bqb_exit(const char *cmd_str, char *buf, size_t buf_len)
{
    (void)cmd_str;

    /* Ensure a null terminator after each character written. */
    memset(buf, 0x00, buf_len);

    snprintf(buf, buf_len, "Exit from BQB mode...\r\n");

    return false;
}

#if F_APP_BQB_CLI_HFP_AG_SUPPORT
/*
 * The definition of the "bqb_hfhs_ag" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_hfhs_ag =
{
    NULL,           /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "hfhs_ag",        /* The command string to type. */
    "\r\nhfhs_ag:\r\n HFHS AG start from BQB mode\r\n\r\n",
    bqb_hfhs_ag,      /* The function to run. */
    -1,              /* Zero or more parameters are expected. */
    NULL             /* Default command match pattern. */
};
#endif

/*
 * The definition of the "pbap" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_pbap =
{
#if F_APP_BQB_CLI_HFP_AG_SUPPORT
    &bqb_cmd_hfhs_ag,           /* Next command pointed to NULL. */
#else
    NULL,
#endif
    NULL,           /* Next subcommand. */
    "pbap",        /* The command string to type. */
    "\r\npbap:\r\n PBAP start from BQB mode\r\n\r\n",
    bqb_pbap,      /* The function to run. */
    -1,            /* Zero or more parameters are expected. */
    NULL           /* Default command match pattern. */
};

/*
 * The definition of the "map" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_map =
{
    &bqb_cmd_pbap,   /* Next command pointed to bqb_cmd_pbap. */
    NULL,           /* Next subcommand. */
    "map",        /* The command string to type. */
    "\r\nmap:\r\n MAP start from BQB mode\r\n\r\n",
    bqb_map,      /* The function to run. */
    -1,           /* Zero or more parameters are expected. */
    NULL          /* Default command match pattern. */
};

/*
 * The definition of the "hfhs" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_hfhs =
{
    &bqb_cmd_map,   /* Next command pointed to bqb_cmd_map. */
    NULL,           /* Next subcommand. */
    "hfhs",        /* The command string to type. */
    "\r\nhfhs:\r\n HFHS start from BQB mode\r\n\r\n",
    bqb_hfhs,      /* The function to run. */
    -1,            /* Zero or more parameters are expected. */
    NULL           /* Default command match pattern. */
};

/*
 * The definition of the "rfcomm" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_rfcomm =
{
    &bqb_cmd_hfhs,           /* Next command pointed to bqb_cmd_spp. */
    NULL,           /* Next subcommand. */
    "rfcomm",        /* The command string to type. */
    "\r\nsrfcomm:\r\n RFCOMM start from BQB mode\r\n\r\n",
    bqb_rfcomm,      /* The function to run. */
    -1,              /* Zero or more parameters are expected. */
    NULL             /* Default command match pattern. */
};


/*
 * The definition of the "avrcp" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_avrcp =
{
    &bqb_cmd_rfcomm,           /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "avrcp",        /* The command string to type. */
    "\r\navrcp:\r\n AVRCP start from BQB mode\r\n\r\n",
    bqb_avrcp,      /* The function to run. */
    -1,             /* Zero or more parameters are expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "avdtp" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_avdtp =
{
    &bqb_cmd_avrcp,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "avdtp",        /* The command string to type. */
    "\r\navdtp:\r\n AVDTP start from BQB mode\r\n\r\n",
    bqb_avdtp,      /* The function to run. */
    -1,             /* Zero or more parameters are expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "pbp" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_pbp =
{
    &bqb_cmd_avdtp,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "pbp",        /* The command string to type. */
    "\r\npbp:\r\n pbp start from BQB mode\r\n\r\n",
    bqb_pbp,      /* The function to run. */
    -1,             /* Zero or more parameters are expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "tmap" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_tmap =
{
    &bqb_cmd_pbp,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "tmap",        /* The command string to type. */
    "\r\ntmap:\r\n tmap start from BQB mode\r\n\r\n",
    bqb_tmap,      /* The function to run. */
    -1,             /* Zero or more parameters are expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "hap" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_hap =
{
    &bqb_cmd_tmap,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "hap",        /* The command string to type. */
    "\r\nhap:\r\n hap start from BQB mode\r\n\r\n",
    bqb_hap,      /* The function to run. */
    -1,             /* Zero or more parameters are expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "sdp" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_sdp =
{
    &bqb_cmd_hap, /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "sdp",          /* The command string to type. */
    "\r\nsdp:\r\n SDP search from BQB mode\r\n\r\n",
    bqb_sdp,        /* The function to run. */
    7,              /* Seven parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "pair" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_pair =
{
    &bqb_cmd_sdp,   /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "pair",         /* The command string to type. */
    "\r\npair:\r\n Pair start/stop from BQB mode\r\n\r\n",
    bqb_pair,       /* The function to run. */
    1,              /* One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "power" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_power =
{
    &bqb_cmd_pair,  /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "power",        /* The command string to type. */
    "\r\npower:\r\n Power on/off from BQB mode\r\n\r\n",
    bqb_power,      /* The function to run. */
    1,              /* One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "reset" subcommand of bqb.
 */
const static T_CLI_CMD bqb_cmd_reset =
{
    &bqb_cmd_power, /* Next command pointed to NULL. */
    NULL,           /* Next subcommand. */
    "reset",        /* The command string to type. */
    "\r\nreset:\r\n Resets from BQB mode\r\n\r\n",
    bqb_reset,      /* The function to run. */
    0,              /* Zero parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "exit" subcommand of bqb.
 * This command is always registered next to "help" command.
 */
const static T_CLI_CMD bqb_cmd_exit =
{
    &bqb_cmd_reset, /* Next command pointed to "reset". */
    NULL,           /* Next subcommand. */
    "exit",         /* The command string to type. */
    "\r\nexit:\r\n Exits from BQB mode\r\n",
    bqb_exit,       /* The function to run. */
    0,              /* Zero parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "help" subcommand of bqb.
 * This command is always at the front of the list of registered commands.
 */
const static T_CLI_CMD bqb_cmd_help =
{
    &bqb_cmd_exit,  /* Next command pointed to "exit" command. */
    NULL,           /* Next subcommand. */
    "help",         /* The command string to type. */
    "\r\nhelp:\r\n Lists all the registered commands\r\n\r\n",
    bqb_help,       /* The function to run. */
    -1,             /* Zero or One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The definition of the "bqb" command.
 * This command should be registered into file system command list.
 */
static T_CLI_CMD bqb_cmd_entry =
{
    NULL,           /* Next command will be determined when registered into file system. */
    &bqb_cmd_help,  /* Next subcommand pointer to "help" subcommand. */
    "bqb",          /* The command string to type. */
    "\r\nbqb:\r\n Enters BQB mode\r\n\r\n",
    bqb_mode,       /* The function to run. */
    0,              /* Zero or One parameter is expected. */
    NULL            /* Default command match pattern. */
};

/*
 * The callback function that is executed when "help" is entered.
 * This is the default subcommand that is always present.
 */
static bool bqb_help(const char *cmd_str, char *buf, size_t buf_len)
{
    return cli_help_set(&bqb_cmd_help, cmd_str, buf, buf_len);
}

bool bqb_cmd_register(void)
{
    return cli_cmd_register(&bqb_cmd_entry);
}
#endif
