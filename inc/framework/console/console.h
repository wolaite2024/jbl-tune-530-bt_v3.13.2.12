/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup CONSOLE    Console
 *
 * \brief   Provide console interfaces.
 */
/**
 * console.h
 *
 * \brief Define console parameter.
 *
 * \ingroup CONSOLE
 */
typedef struct t_console_param
{
    uint32_t tx_buf_size;
    uint32_t rx_buf_size;
    uint8_t  tx_wakeup_pin;
    uint8_t  rx_wakeup_pin;
} T_CONSOLE_PARAM;

/**
 * console.h
 *
 * \brief Define console mode.
 *
 * \ingroup CONSOLE
 */
typedef enum t_console_mode
{
    CONSOLE_MODE_STRING     = 0,
    CONSOLE_MODE_BINARY     = 1,
} T_CONSOLE_MODE;

/**
 * console.h
 *
 * \brief  Define console parser prototype in binary mode.
 *
 * \param[in]  buf     The start address of the data.
 * \param[in]  len     The length of the data.
 *
 * \return          The status of parsing data.
 * \retval true     Data was parsed successfully.
 * \retval false    Data was failed to parse.
 *
 * \ingroup CONSOLE
 */
typedef bool (*P_CONSOLE_PARSER)(uint8_t *buf, uint32_t len);

/**
 * console.h
 *
 * \brief Define console event.
 *
 * \ingroup CONSOLE
 */
typedef enum t_console_evt
{
    CONSOLE_EVT_IDLE        = 0,
    CONSOLE_EVT_OPENED      = 1,
    CONSOLE_EVT_CLOSED      = 2,
    CONSOLE_EVT_DATA_IND    = 3,
    CONSOLE_EVT_DATA_XMIT   = 4,
    CONSOLE_EVT_ERROR       = 5,
} T_CONSOLE_EVT;

/**
 * console.h
 *
 * \brief  Define console event callback prototype.
 *
 * \param[in]  evt     Console callback event \ref T_CONSOLE_EVT.
 * \param[in]  buf     The start address of the data.
 * \param[in]  len     The length of the data.
 *
 * \return          The status of processing console callback event.
 * \retval true     Console callback event was processed successfully.
 * \retval false    Console callback event was failed to process.
 *
 * \ingroup CONSOLE
 */
typedef bool (*P_CONSOLE_CALLBACK)(T_CONSOLE_EVT evt, uint8_t *buf, uint32_t len);

/**
 * console.h
 *
 * \brief Define console IO operations.
 *
 * \ingroup CONSOLE
 */
typedef struct t_console_op
{
    bool (*init)(P_CONSOLE_CALLBACK p_callback);
    bool (*tx_wakeup_enable)(uint8_t pin);
    bool (*rx_wakeup_enable)(uint8_t pin);
    bool (*write)(uint8_t *buf, uint32_t len);
    bool (*wakeup)(void);
    bool (*tx_empty)(void);
} T_CONSOLE_OP;

/**
 * \brief  Initiate the console.
 *
 * \param[in] console_param   console parameter \ref T_CONSOLE_PARAM.
 * \param[in] console_op      console operation \ref T_CONSOLE_OP.
 *
 * \return          The status of Initiating the console.
 * \retval true     Console was initiated successfully.
 * \retval false    Console was failed to initiate.
 *
 * \ingroup CONSOLE
 */
bool console_init(T_CONSOLE_PARAM *console_param, T_CONSOLE_OP *console_op);

/**
 * \brief  Set the console mode.
 *
 * \param[in] mode  console mode \ref T_CONSOLE_MODE.
 *
 * \return          The status of setting the console mode.
 * \retval true     Console mode was set successfully.
 * \retval false    Console mode was failed to set.
 *
 * \ingroup CONSOLE
 */
bool console_set_mode(T_CONSOLE_MODE mode);

/**
 * \brief  Get the console mode.
 *
 * \return  The console mode \ref T_CONSOLE_MODE.
 *
 * \ingroup CONSOLE
 */
T_CONSOLE_MODE console_get_mode(void);

/**
 * \brief  Register the console parser in binary mode.
 *
 * \param[in] parser    console parser \ref P_CONSOLE_PARSER.
 *
 * \return          The status of registering parser.
 * \retval true     Parser was registered successfully.
 * \retval false    Parser was failed to register.
 *
 * \ingroup CONSOLE
 */
bool console_register_parser(P_CONSOLE_PARSER parser);

/**
 * \brief  Unregister the console parser in binary mode.
 *
 *
 * \return          The status of unregistering parser.
 * \retval true     Parser was unregistered successfully.
 * \retval false    Parser was failed to unregister.
 *
 * \ingroup CONSOLE
 */
bool console_unregister_parser(void);

/**
 * \brief  Write data to the console.
 *
 * \param[in] buf    The start address of the data.
 * \param[in] len    The length of the data.
 *
 * \return          The status of writing data to the console.
 * \retval true     Data was written to the console successfully.
 * \retval false    Data was failed to written to the console.
 *
 * \ingroup CONSOLE
 */
bool console_write(uint8_t *buf, uint32_t len);

/**
 * \brief  Wake up the console.
 *
 * \return          The status of waking up the console.
 * \retval true     Console was woken up successfully.
 * \retval false    Console was failed to wake up.
 *
 * \ingroup CONSOLE
 */
bool console_wakeup(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CONSOLE_H_ */
