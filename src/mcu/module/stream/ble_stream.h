/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#ifndef _BLE_STREAM_
#define _BLE_STREAM_

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include "profile_server.h"
#include "common_stream.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup BLE_STREAM Bluetooth Low Energy API Sets
  * @brief API sets for user application to use BLE stream
  * @{
  */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup BLE_Stream_Exported_Types BLE Stream Sets Types
 * @{
 */

/**
 * @brief Parameters used in notify the events of BLE stream to user application
 */
typedef enum
{
    BLE_STREAM_WRITE_CCCD_EVENT,  //!< Remote end write CCCD
    BLE_STREAM_DISCONNECT_EVENT,      //!< Disconnect with remote end
    BLE_STREAM_RECEIVE_DATA_EVENT,    //!< Receive data from remote end
    BLE_STREAM_UPDATE_CREDIT_EVENT,   //!< Update credit
    BLE_STREAM_UPDATE_MTU_EVENT       //!< Update MTU(Maximum Transmission Unit)
} T_BLE_STREAM_EVENT;

/**
 * @brief Event parameters that need to be passed along with @ref BLE_STREAM_WRITE_CCCD_EVENT event
 */
typedef struct
{
    uint8_t service_id;   //!< BLE service id
    uint8_t conn_id;      //!< BLE connect id
    uint8_t attr_idx;
} T_BLE_STREAM_WRITE_CCCD_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref BLE_STREAM_DISCONNECT_EVENT event
 */
typedef struct
{
    uint8_t conn_id;     //!< BLE connect id
    uint8_t service_id;
} T_BLE_STREAM_DISCONNECT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref BLE_STREAM_RECEIVE_DATA_EVENT event
 */
typedef struct
{
    uint8_t conn_id;
    uint8_t  *data;      //!< Ponter of the received data
    uint8_t  service_id; //!< BLE service id
    uint16_t length;     //!< Length of received data
    uint8_t  index;      //!< ATT rx channel index
} T_BLE_STREAM_RECEIVE_DATA_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref BLE_STREAM_UPDATE_CREDIT_EVENT event
 */
typedef struct
{
    uint8_t conn_id;
    uint8_t service_id;  //!< BLE service id
    uint8_t credits;     //!< BLE credits
} T_BLE_STREAM_UPDATE_CREDIT_EVENT_PARAM;

/**
 * @brief Event parameters that need to be passed along with @ref BLE_STREAM_UPDATE_CREDIT_EVENT event
 */
typedef struct
{
    uint8_t  conn_id;   //!< BLE connect id
    uint16_t mtu;       //!< BLE MTU size
} T_BLE_STREAM_UPDATE_EVENT_MTU;

/**
 * @brief Union of event parameters for BLE stream
 * @note  This structure make the processing function of the BLE stream to unify the parameters
 *        into one type when dealing with different events.
 */
typedef union
{
    T_BLE_STREAM_WRITE_CCCD_EVENT_PARAM     write_cccd_param;
    T_BLE_STREAM_DISCONNECT_EVENT_PARAM     disconnect_param;
    T_BLE_STREAM_RECEIVE_DATA_EVENT_PARAM   receive_data_param;
    T_BLE_STREAM_UPDATE_CREDIT_EVENT_PARAM  update_credit_param;
    T_BLE_STREAM_UPDATE_EVENT_MTU           update_mut_param;
} T_BLE_STREAM_EVENT_PARAM;


typedef struct _BLE_STREAM_STRUCT *BLE_STREAM;


typedef void (*BLE_STREAM_CB)(COMMON_STREAM stream, COMMON_STREAM_EVENT event);


typedef struct _BLE_STREAM_STRUCT
{
    COMMON_STREAM_STRUCT    common;
    uint8_t                 conn_id;
    uint8_t                 service_id;
    bool                    authenticated;
    uint32_t                read_attr_idx;
    uint32_t                write_attr_idx;
    T_GATT_PDU_TYPE         read_pdu_type;
    T_GATT_PDU_TYPE         write_pdu_type;
} BLE_STREAM_STRUCT;


typedef struct
{
    uint32_t            service_id;
    uint8_t             conn_id;
    uint32_t            read_attr_idx;
    uint32_t            write_attr_idx;
    T_GATT_PDU_TYPE     read_pdu_type;
    T_GATT_PDU_TYPE     write_pdu_type;
    COMMON_STREAM_CB    stream_cb;
} BLE_INIT_SETTINGS;



/** End of BLE_Stream_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup BLE_Stream_Exported_Functions BLE Stream API Functions
 * @{
 */


bool ble_stream_init(uint32_t max_stream_num);


bool ble_stream_write(BLE_STREAM ble_stream, uint8_t *data, uint32_t length);

/**
 * @brief      Initialize the BLE stream
 * @note       BLE stream is a sub-module of transmission stream. It implements the abstract interface
 *             of transmission stream definition.
 * @param[in]  ble_setting  BLE initialization setting
 * @return     transmission stream, if initialize fail, NULL will be returned
 */
BLE_STREAM ble_stream_create(BLE_INIT_SETTINGS *p_settings);

/**
* @brief      Process the event of the BLE
* @note       This API connects BLE stream module to BLE protocal by receiving and processing the BLE
*             event. This API should be called where these events exist, with the required parameters,
*             so that BLE stream can receive these events, and every event defined in @ref T_BLE_STREAM_EVENT
*             is requied
* @param[in]  event        event number
* @param[in]  event_param  event parameter
* @return     void
*
* <b>Example Usage</b>
* @code
*
* // Examples of updating MTU events
* // Suppose this location is where the BLE protocol updates the MTU
* T_BLE_STREAM_EVENT_PARAM ble_stream_param;
* ble_stream_param.update_mut_param.conn_id = conn_id;
* ble_stream_param.update_mut_param.mtu     = mtu;
* ble_stream_event_process(BLE_STREAM_UPDATE_MTU_EVENT, &ble_stream_param);
*
* @endcode
*/
void ble_stream_event_process(T_BLE_STREAM_EVENT event,
                              T_BLE_STREAM_EVENT_PARAM *event_param);


BLE_STREAM ble_stream_search_by_id(uint32_t service_id, uint8_t conn_id,
                                   uint8_t read_attr_idx, uint8_t write_attr_idx);
BLE_STREAM ble_stream_search_by_addr(uint32_t service_id, uint8_t *bd_addr,
                                     uint8_t read_attr_idx, uint8_t write_attr_idx);

bool ble_stream_copy_pub_addr(uint8_t conn_id, uint8_t *pub_addr);

/** @} */ /* End of group BLE_Stream_Exported_Functions */

/** @} */ /* End of group BLE_STREAM */

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _BLE_STREAM_ */
