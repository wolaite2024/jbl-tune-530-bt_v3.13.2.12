/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BIN_LOADER_H_
#define _BIN_LOADER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * bin_loader.h
 *
 * \brief Define the Bin Loader events.
 *
 * \ingroup BIN_LOADER
 */
typedef enum t_bin_loader_event
{
    BIN_LOADER_EVENT_SUCCESS       = 0x00,
    BIN_LOADER_EVENT_EXEC_FAIL    = 0x01,
    BIN_LOADER_EVENT_VERIFY_FAIL = 0x02,
} T_BIN_LOADER_EVENT;

/**
 * bin_loader.h
 *
 * \brief Define the Bin Loader session handle.
 *
 * \ingroup BIN_LOADER
 */
typedef void *T_BIN_LOADER_SESSION_HANDLE;

/**
 * bin_loader.h
 *
 * \brief Define the Bin Loader execution callback.
 *
 * \details Each Bin Loader session shall register the execution callback when the Bin Loader
 *          session is created \ref bin_loader_session_create. When the Bin Loader Token issued
 *          is scheduled to execute, Bin Loader Manager shall invoke the corresponding execution
 *          callback.
 *
 * \param[in] session   The Bin Loader session handle \ref T_BIN_LOADER_SESSION_HANDLE.
 * \param[in] id        The Bin Loader token id.
 * \param[in] context   The Bin Loader token context.
 *
 * \ingroup BIN_LOADER
 */
typedef bool (*P_BIN_LOADER_EXEC)(T_BIN_LOADER_SESSION_HANDLE  session,
                                  uint32_t                     id,
                                  void                        *context);

/**
 * bin_loader.h
 *
 * \brief Define the Bin Loader verification callback.
 *
 * \details Each Bin Loader session shall register the execution callback when the Bin Loader
 *          session is created \ref bin_loader_session_create. When the Bin Loader Token issued
 *          is executed \ref P_BIN_LOADER_EXEC, Bin Loader Manager shall invoke the corresponding
 *          verification callback.
 *
 * \param[in] session   The Bin Loader session handle \ref T_BIN_LOADER_SESSION_HANDLE.
 * \param[in] id        The Bin Loader token id.
 * \param[in] context   The Bin Loader token context.
 *
 * \ingroup BIN_LOADER
 */
typedef bool (*P_BIN_LOADER_VERIFY)(T_BIN_LOADER_SESSION_HANDLE  session,
                                    uint32_t                     id,
                                    void                        *context);

/**
 * bin_loader.h
 *
 * \brief Define the Bin Loader event callback.
 *
 * \details Each Bin Loader session shall register the execution callback when the Bin Loader
 *          session is created \ref bin_loader_session_create. When the state of the Bin Loader
 *          session is changed, Bin Loader Manager shall invoke the corresponding
 *          event callback \ref T_BIN_LOADER_EVENT.
 *
 * \param[in] session   The Bin Loader session handle \ref T_BIN_LOADER_SESSION_HANDLE.
 * \param[in] event     The Bin Loader event \ref T_BIN_LOADER_EVENT.
 * \param[in] context   The Bin Loader token context.
 *
 * \ingroup BIN_LOADER
 */
typedef bool (*P_BIN_LOADER_CBACK)(T_BIN_LOADER_SESSION_HANDLE  session,
                                   T_BIN_LOADER_EVENT           event,
                                   void                        *context);

/**
 * bin_loader.h
 *
 * \brief   Initialize Bin Loader Manager.
 *
 * \return          The status of initializing Bin Loader Manager.
 * \retval true     Bin Loader Manager was initialized successfully.
 * \retval false    Bin Loader Manager was failed to initialize.
 *
 * \ingroup BIN_LOADER
 */
bool bin_loader_init(void);

/**
 * bin_loader.h
 *
 * \brief   De-initialize Bin Loader Manager.
 *
 * \return          The status of de-initializing Bin Loader Manager.
 * \retval true     Bin Loader Manager was de-initialized successfully.
 * \retval false    Bin Loader Manager was failed to de-initialize.
 *
 * \ingroup BIN_LOADER
 */
void bin_loader_deinit(void);


/**
 * bin_loader.h
 *
 * \brief   Create a Bin Loader session instance.
 *
 * \details Bin Loader Manager supports maintain multiple coexisting Bin Loader sessions. The
 *          clients can create their own Bin Loader sessions, and interact with Bin Loader
 *          Manager independently.
 *
 * \param[in] exec      The Bin Loader execution callback \ref P_BIN_LOADER_EXEC.
 * \param[in] verify    The Bin Loader verification callback \ref P_BIN_LOADER_VERIFY.
 * \param[in] cback     The Bin Loader event callback \ref P_BIN_LOADER_CBACK.
 *
 * \return  The instance handle of Bin Loader session. If returned handle is NULL, the Bin Loader
 *          session instance was failed to create.
 *
 * \ingroup BIN_LOADER
 */
T_BIN_LOADER_SESSION_HANDLE bin_loader_session_create(P_BIN_LOADER_EXEC    exec,
                                                      P_BIN_LOADER_VERIFY verify,
                                                      P_BIN_LOADER_CBACK  cback);

/**
 * bin_loader.h
 *
 * \brief   Release the Bin Loader session instance.
 *
 * \details All resources and configurations associated with the Bin Loader session will be
 *          released.
 *
 * \param[in] handle    The Bin Loader session handle \ref T_BIN_LOADER_SESSION_HANDLE.
 *
 * \return          The status of releasing the Bin Loader session.
 * \retval true     Bin Loader session was released successfully.
 * \retval false    Bin Loader session was failed to release.
 *
 * \ingroup BIN_LOADER
 */
bool bin_loader_session_destory(T_BIN_LOADER_SESSION_HANDLE handle);

/**
 * bin_loader.h
 *
 * \brief   Issue the Bin Loader token of the specific session.
 *
 * \details The Clients can issue any tokens independently whenever the corresponding Bin
 *          Loader sessions are created.
 *
 * \param[in] handle    The Bin Loader session handle \ref T_BIN_LOADER_SESSION_HANDLE.
 * \param[in] id        The Bin Loader token id.
 * \param[in] context   The Bin Loader token context.
 *
 * \return          The status of issuing the Bin Loader token.
 * \retval true     Bin Loader token was issued successfully.
 * \retval false    Bin Loader token was failed to issue.
 *
 * \ingroup BIN_LOADER
 */
bool bin_loader_token_issue(T_BIN_LOADER_SESSION_HANDLE  handle,
                            uint32_t                     id,
                            void                        *context);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _BIN_LOADER_H_ */
