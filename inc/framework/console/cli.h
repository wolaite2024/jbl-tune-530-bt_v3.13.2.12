/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _CLI_H_
#define _CLI_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    CLI Command Line Interface
 *
 * \brief   Provide the functionalities of command line interface.
 * \details Command line interface provides a prompt where the user types a command, which is expressed
 *          as a sequence of characters - typically a command name followed by some parameters, and
 *          presses the \b Return key to execute that command.
 */

/**
 * cli.h
 *
 * \brief Define the prototype of command line callback function.
 *
 * \param[in] cmd_str   The entire string (including params) as input by the user.
 * \param[in] p_buf     The buffer into which the output from executing the command can be written.
 * \param[in] buf_len   The length in bytes of the p_buf buffer.
 *
 * \return          Return true if the callback function should be invoked continuously, or false
 *                  if the callback function is completed.
 * \retval true     The callback function should be invoked continuously.
 * \retval false    The callback function was completed.
 *
 * \ingroup CLI
 */
typedef bool (*P_CLI_CALLBACK)(const char *cmd_str, char *p_buf, size_t buf_len);

/**
 * cli.h
 *
 * \brief Define the prototype of command line pattern matching.
 *
 * \param[in] cmd_str   The entire string (including params) as input by the user.
 * \param[in] p_cmd     The command pattern matching buffer.
 * \param[in] cmd_len   The length in bytes of the p_cmd buffer.
 *
 * \return          The status of matching the command pattern.
 * \retval true     The command pattern was matched.
 * \retval false    The command pattern was failed to match.
 *
 * \ingroup CLI
 */
typedef bool (*P_CLI_MATCH)(const char *cmd_str, const char *p_cmd, size_t cmd_len);

/**
 * cli.h
 *
 * \brief  Define the structure of command line interface.
 *
 * \ingroup CLI
 */
typedef struct t_cli_cmd
{
    const struct t_cli_cmd *p_next;       /**< The next command to be linked. */
    const struct t_cli_cmd *p_next_sub;   /**< The next subcommand to be linked. */
    const char *const       p_cmd;        /**< The command that causes p_callback to be executed. */
    const char *const       p_help;       /**< String that describes how to use the command. */
    const P_CLI_CALLBACK    p_callback;   /**< The command callback that will generate output. */
    int32_t                 param_num;    /**< The command expects a fixed number of parameters. */
    const P_CLI_MATCH       p_match;      /**< The command pattern matching function;
                                               NULL if using default match pattern. */
} T_CLI_CMD;

/**
 * cli.h
 *
 * \brief   Register the command passed in using the cli_cmd parameter.
 * \details Registering a command adds the command to the list of commands that are
 *          handled by the command interpreter. Once a command has been registered it
 *          can be executed from the command line.
 *
 * \param[in] cli_cmd   The registered command \ref T_CLI_CMD.
 *
 * \return          The status of registering the command.
 * \retval true     The command was registered successfully.
 * \retval false    The command was failed to register.
 *
 * \ingroup CLI
 */
bool cli_cmd_register(T_CLI_CMD *const cli_cmd);

/**
 * cli.h
 *
 * \brief  Get the pointer to the param_idx'th word in the command string.
 *
 * \param[in]  cmd_str      The start address of the command string.
 * \param[in]  param_idx    The parameter index in the command string.
 * \param[out] param_len    The length of the parameter word.
 *
 * \return  A pointer to the param_idx'th word in the command string.
 *
 * \ingroup CLI
 */
const char *cli_param_get(const char *cmd_str, uint32_t param_idx, uint32_t *param_len);

/**
 * cli.h
 *
 * \brief  Get the number of parameters that follow the command name.
 *
 * \param[in] cmd_str   The start address of the command string.
 *
 * \return  The number of parameters that follow the command name.
 *
 * \ingroup CLI
 */
int32_t cli_param_num_get(const char *cmd_str);

/**
 * cli.h
 *
 * \brief   Traverse and echo the help prompts of the specified command list.
 *
 * \param[in] cmd_list   The command of which the subcommand list will be traversed.
 * \param[in] cmd_str    The entire string (including parameters) as input by the user.
 * \param[in] buf        The buffer into which the output from executing the command can be written.
 * \param[in] buf_len    The length in bytes of the buffer.
 *
 * \return          The status of traversing and echoing the help prompts of the specified command list.
 * \retval true     The help prompts were traversed and echoed successfully.
 * \retval false    The help prompts were failed to traverse and echo.
 *
 * \ingroup CLI
 */
bool cli_help_set(const T_CLI_CMD *cmd_list, const char *cmd_str, char *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif /* _CLI_H_ */
