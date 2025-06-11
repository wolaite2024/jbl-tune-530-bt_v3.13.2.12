/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BQB_CMD_H_
#define _BQB_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @defgroup APP_RWS_BQB App bqb
 * @brief Provides BQB interfaces for RWS BQB test.
 * @{
 */


/**
  * @brief  The callback function that is executed when "reset" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_reset(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "power on/off" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_power(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "pair start/stop" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_pair(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "sdp <uuid>" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_sdp(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "avdtp discover/connect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_avdtp(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "avrcp stop/connect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */

bool bqb_avrcp(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "rfcomm connect/disconnect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_rfcomm(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "hfhs connect/disconnect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_hfhs(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "hfhs ag connect/disconnect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_hfhs_ag(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "pbap connect/disconnect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_pbap(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "map connect/disconnect" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_map(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "pbp broadcast_code" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_pbp(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "tmap gaming_mode/originate" is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_tmap(const char *cmd_str, char *buf, size_t buf_len);

/**
  * @brief  The callback function that is executed when "hap " is entered. This is a bqb
            subcommand that should be registered.
  * @param  cmd_str    The start address of the command buffer.
  * @param  buf        The start address of output_buffer.
  * @param  buf_len    The length of output_buffer.
  * @return The result of sending command.
  * @retval true    The command needs to be executed again.
  * @retval false   The command has been executed.
  */
bool bqb_hap(const char *cmd_str, char *buf, size_t buf_len);

/**
 * End of APP_RWS_BQB
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BQB_CMD_H_ */
