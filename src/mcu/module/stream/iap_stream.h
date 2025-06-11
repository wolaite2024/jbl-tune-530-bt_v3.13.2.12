/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#ifndef _IAP_STREAM_
#define _IAP_STREAM_

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include "common_stream.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup IAP_STREAM IAP Stream API Sets
  * @brief API sets for user application to use IAP stream
  * @{
  */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup IAP_Stream_Exported_Types IAP Stream Sets Types
 * @{
 */

/**
 * @brief Parameters used in notify the events of IAP stream to user application
 */
typedef enum
{
    IAP_STREAM_CONNECT_EVENT,   //!< IAP stream is connected with the remote end
    IAP_STREAM_DISCONNECT_EVENT,    //!< IAP stream is disconnected with the remote end
    IAP_STREAM_RECEIVE_DATA_EVENT,  //!< IAP stream receive the data from the remote end
    IAP_STREAM_UPDATE_CREDIT_EVENT  //!< IAP stream update the credit
} T_IAP_STREAM_EVENT;

/**
 * @brief Event parameters that need to be passed along with @ref IAP_STREAM_CONNECT_EVENT event
 */
typedef struct
{
    uint8_t  remote_credits;
    uint8_t  ea_protocol_id;
    uint16_t mtu;
} T_IAP_STREAM_CONNECT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref IAP_STREAM_DISCONNECT_EVENT event
 */
typedef struct
{
    bool        disc_all;
    uint16_t    cause;
} T_IAP_STREAM_DISCONNECT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref IAP_STREAM_RECEIVE_DATA_EVENT event
 */
typedef struct
{
    uint8_t  *data;
    uint8_t  credits;
    uint16_t length;
} T_IAP_STREAM_RECEIVE_DATA_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref IAP_STREAM_UPDATE_CREDIT_EVENT event
 */
typedef struct
{
    uint8_t remote_credits;
} T_IAP_STREAM_UPDATE_CREDIT_EVENT_PARAM;

/**
 * @brief Union of event parameters for IAP stream
 * @note  This structure make the processing function of the IAP stream to unify the parameters
 *        into one type when dealing with different events.
 */
typedef struct
{
    uint8_t bd_addr[6];
    uint16_t eap_session_id;
    union
    {
        T_IAP_STREAM_CONNECT_EVENT_PARAM       connect_param;
        T_IAP_STREAM_DISCONNECT_EVENT_PARAM    disconnect_param;
        T_IAP_STREAM_RECEIVE_DATA_EVENT_PARAM  receive_data_param;
        T_IAP_STREAM_UPDATE_CREDIT_EVENT_PARAM update_credit_param;
    };

} T_IAP_STREAM_EVENT_PARAM;


typedef struct _IAP_STREAM_STRUCT *IAP_STREAM;


typedef void (*IAP_STREAM_CB)(COMMON_STREAM iap_stream, COMMON_STREAM_EVENT common_event);


typedef struct _IAP_STREAM_STRUCT
{
    COMMON_STREAM_STRUCT    common;
    uint8_t                 ea_protocol_id;
    uint16_t                eap_session_id;
} IAP_STREAM_STRUCT;


typedef struct
{
    uint8_t             ea_protocol_id;
    uint8_t            *bd_addr;
    COMMON_STREAM_CB    stream_cb;
} IAP_INIT_SETTINGS;

/** End of IAP_Stream_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup IAP_Stream_Exported_Functions IAP Stream API Functions
 * @{
 */


bool iap_stream_init(uint32_t max_stream_num);


/**
 * @brief      Initialize the IAP stream
 * @note       IAP stream is a sub-module of transmission stream. It implements the abstract interface
 *             of transmission stream definition.
 * @return     transmission stream, if initialize fail, NULL will be returned
 */
IAP_STREAM iap_stream_create(IAP_INIT_SETTINGS *p_settings);

/**
* @brief      Process the event of the IAP
* @note       This API connects IAP stream module to IAP protocal by receiving and processing the IAP
*             event. This API should be called where these events exist, with the required parameters,
*             so that IAP stream can receive these events, and every event defined in @ref T_IAP_STREAM_EVENT
*             is requied
* @param[in]  event        event number
* @param[in]  event_param  event parameter
* @return     void
*
* <b>Example Usage</b>
* @code
*
* // Examples of IAP connect events
* // Suppose this location is where the IAP protocol is connected
* T_IAP_STREAM_EVENT_PARAM iap_stream_param;
* iap_stream_param.connect_param.mtu            = mtu;
* iap_stream_param.connect_param.remote_credits = credits;
* iap_stream_param.connect_param.iap_index      = iap_index;
* iap_stream_event_process(IAP_STREAM_CONNECT_EVENT, &iap_stream_param);
*
* @endcode
*/
void iap_stream_event_process(T_IAP_STREAM_EVENT event, T_IAP_STREAM_EVENT_PARAM *event_param);


IAP_STREAM iap_stream_find_by_ea_protocol_id(uint8_t ea_protocol_id,
                                             uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]);


void iap_stream_send_all_info_to_sec(void);


/** @} */ /* End of group IAP_Stream_Exported_Functions */

/** @} */ /* End of group IAP_STREAM */

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _IAP_STREAM_ */
