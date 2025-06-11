/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#ifndef _SPP_STREAM_
#define _SPP_STREAM_

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include "common_stream.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup SPP_STREAM SPP Stream API Sets
  * @brief API sets for user application to use SPP stream
  * @{
  */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup SPP_Stream_Exported_Types SPP Stream Sets Types
 * @{
 */

/**
 * @brief Parameters used in notify the events of SPP stream to user application
 */
typedef enum
{
    SPP_STREAM_CONNECT_EVENT,   //!< SPP stream is connected with the remote end
    SPP_STREAM_DISCONNECT_EVENT,    //!< SPP stream is disconnected with the remote end
    SPP_STREAM_RECEIVE_DATA_EVENT,  //!< SPP stream receive the data from the remote end
    SPP_STREAM_UPDATE_CREDIT_EVENT  //!< SPP stream update the credit
} T_SPP_STREAM_EVENT;

/**
 * @brief Event parameters that need to be passed along with @ref SPP_STREAM_CONNECT_EVENT event
 */
typedef struct
{
    uint8_t  remote_credits;
    uint16_t mtu;
    uint8_t  bd_addr[BLUETOOTH_ADDR_LENGTH];
} T_SPP_STREAM_CONNECT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref SPP_STREAM_DISCONNECT_EVENT event
 */
typedef struct
{
    uint8_t bd_addr[6];
    uint32_t cause;
} T_SPP_STREAM_DISCONNECT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref SPP_STREAM_RECEIVE_DATA_EVENT event
 */
typedef struct
{
    uint8_t  *data;
    uint8_t  credits;
    uint16_t length;
    uint8_t  bd_addr[6];
} T_SPP_STREAM_RECEIVE_DATA_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref SPP_STREAM_UPDATE_CREDIT_EVENT event
 */
typedef struct
{
    uint8_t bd_addr[6];
    uint8_t remote_credits;
} T_SPP_STREAM_UPDATE_CREDIT_EVENT_PARAM;

/**
 * @brief Union of event parameters for SPP stream
 * @note  This structure make the processing function of the SPP stream to unify the parameters
 *        into one type when dealing with different events.
 */
typedef struct
{
    uint8_t server_chann;
    union
    {
        T_SPP_STREAM_CONNECT_EVENT_PARAM       connect_param;
        T_SPP_STREAM_DISCONNECT_EVENT_PARAM    disconnect_param;
        T_SPP_STREAM_RECEIVE_DATA_EVENT_PARAM  receive_data_param;
        T_SPP_STREAM_UPDATE_CREDIT_EVENT_PARAM update_credit_param;
    };
} T_SPP_STREAM_EVENT_PARAM;




typedef struct _SPP_STREAM_STRUCT *SPP_STREAM;



typedef struct _SPP_STREAM_STRUCT
{
    COMMON_STREAM_STRUCT    common;
    uint8_t                 server_chann;
} SPP_STREAM_STRUCT;


typedef struct
{
    uint8_t             server_chann;
    uint8_t            *bd_addr;
    COMMON_STREAM_CB    stream_cb;
} SPP_INIT_SETTINGS;



/** End of SPP_Stream_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup SPP_Stream_Exported_Functions SPP Stream API Functions
 * @{
 */


bool spp_stream_init(uint32_t max_stream_num);


/**
 * @brief      Initialize the SPP stream
 * @note       SPP stream is a sub-module of transmission stream. It implements the abstract interface
 *             of transmission stream definition.
 * @return     transmission stream, if initialize fail, NULL will be returned
 */
SPP_STREAM spp_stream_create(SPP_INIT_SETTINGS *p_settings);

/**
* @brief      Process the event of the SPP
* @note       This API connects SPP stream module to SPP protocal by receiving and processing the SPP
*             event. This API should be called where these events exist, with the required parameters,
*             so that SPP stream can receive these events, and every event defined in @ref T_SPP_STREAM_EVENT
*             is requied
* @param[in]  event        event number
* @param[in]  event_param  event parameter
* @return     void
*
* <b>Example Usage</b>
* @code
*
* // Examples of SPP connect events
* // Suppose this location is where the SPP protocol is connected
* T_SPP_STREAM_EVENT_PARAM spp_stream_param;
* spp_stream_param.connect_param.mtu            = mtu;
* spp_stream_param.connect_param.remote_credits = credits;
* spp_stream_param.connect_param.spp_index      = spp_index;
* spp_stream_event_process(SPP_STREAM_CONNECT_EVENT, &spp_stream_param);
*
* @endcode
*/
void spp_stream_event_process(T_SPP_STREAM_EVENT event, T_SPP_STREAM_EVENT_PARAM *event_param);


SPP_STREAM spp_stream_find_by_server_chann(uint32_t server_chann,
                                           uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]);


void spp_stream_send_all_info_to_sec(void);


/** @} */ /* End of group SPP_Stream_Exported_Functions */

/** @} */ /* End of group SPP_STREAM */

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _SPP_STREAM_ */
