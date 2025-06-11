/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#ifndef _TRANSMISSION_STREAM_
#define _TRANSMISSION_STREAM_

/*============================================================================*
 *                               Header Files
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup TRANSMISSION_STREAM Transmission Stream API Sets
  * @brief API sets for user application to use transmission stream
  * @{
  */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup Transmission_Stream_Exported_Types Transmission Stream Sets Types
 * @{
 */

/**
 * @brief Parameters used in notify the events of transmission stream
 */
typedef enum
{
    TRANSMISSION_STREAM_CONNECTED_EVENT,      //!< Stream has established a connection with the remote end
    TRANSMISSION_STREAM_DISCONNECTED_EVENT,   //!< Stream is disconnected with the remote end
    TRANSMISSION_STREAM_READY_TO_READ_EVENT,  //!< Stream has data that is ready be read
    TRANSMISSION_STREAM_READY_TO_WRITE_EVENT, //!< Stream's state has changed from being unable to write to being able to write
} T_TRANSMISSION_STREAM_EVENT;

/**
 * @brief Parameters used in indicate supported transmission type
 */
typedef enum
{
    SPP_TRANSMISSION_STREAM, //!< Support SPP to transmit data through stream
    BLE_TRANSMISSION_STREAM  //!< Support BLE to transmit data through stream
} T_TRANSMISSION_STREAM_TYPE;

/**
 * @brief Pointer type of the transmission type
 * @note  T_TRANSMISSION_STREAM_STRUCT is the specific definition of the type, but it is placed
 *        in the source file and can only be used by the user through the provided API
 */
typedef struct T_TRANSMISSION_STREAM_STRUCT *T_TRANSMISSION_STREAM;

/**
 * @brief      Transmission stream callback function prototype
 * @note       Stream callback function is used to receive and process events from stream
 * @param[in]  event  callback event
 * @return     void
 *
 * <b>Example Usage</b>
 * @code
 *
 * void stream_connected_event_process(void)
 * {
 *     // ...
 * }
 *
 * void stream_disconnected_event_process(void)
 * {
 *     // ...
 * }
 *
 * void stream_ready_to_read_event_process(void)
 * {
 *     // ...
 * }
 *
 * void stream_ready_to_write_event_process(void)
 * {
 *     // ...
 * }

 * void stream_callback(T_TRANSMISSION_STREAM_EVENT event)
 * {
 *      switch (event)
 *      {
 *          case TRANSMISSION_STREAM_CONNECTED_EVENT:
 *              {
 *                  stream_connected_event_process();
 *              }
 *              break;
 *
 *          case TRANSMISSION_STREAM_DISCONNECTED_EVENT:
 *              {
 *                  stream_disconnected_event_process();
 *              }
 *              break;
 *
 *          case TRANSMISSION_STREAM_READY_TO_READ_EVENT:
 *              {
 *                  stream_ready_to_read_event_process();
 *              }
 *              break;
 *
 *          case TRANSMISSION_STREAM_READY_TO_WRITE_EVENT:
 *              {
 *                  stream_ready_to_write_event_process();
 *              }
 *              break;
 *
 *          default:
 *              break;
 *      }
 * }
 *
 * @endcode
 */
typedef void (* T_TRANSMISSION_STREAM_CB)(T_TRANSMISSION_STREAM_EVENT event);

/**
 * @brief      Transmission stream register callback function prototype
 */
typedef bool (* P_STREAM_REGISTER_CALLBACK)(T_TRANSMISSION_STREAM stream,
                                            T_TRANSMISSION_STREAM_CB callback);

/**
 * @brief      Transmission stream get bytes avalilable function prototype
 */
typedef uint32_t (* P_STREAM_GET_BYTES_AVAILABLE)(T_TRANSMISSION_STREAM stream);

/**
 * @brief      Transmission stream get MTU function prototype
 */
typedef uint32_t (* P_STREAM_GET_MTU)(T_TRANSMISSION_STREAM stream);

/**
 * @brief      Transmission stream write function prototype
 */
typedef bool (* P_STREAM_WRITE)(T_TRANSMISSION_STREAM stream, const uint8_t *data, uint32_t length);

/**
 * @brief      Transmission stream read function prototype
 */
typedef bool (* P_STREAM_READ)(T_TRANSMISSION_STREAM stream, uint32_t length, uint8_t *buffer);

/**
 * @brief      Transmission stream de-initialize function prototype
 */
typedef void (* P_STREAM_DEINIT)(T_TRANSMISSION_STREAM stream);

/** End of Transmission_Stream_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup Transmission_Stream_Exported_Functions Transmission Stream API Functions
 * @{
 */

/**
 * @brief      Register a callback function to the stream
 * @note       User can call this function to register a callback function to the stream,
 *             after that, user can receive the event from the stream.Event is defined in
 *             @ref T_TRANSMISSION_STREAM_EVENT . After the streaming is initialized, you should
 *             register the callback of the stream.
 * @param[in]  stream   transmission stream pointer
 * @param[in]  callback the callback user register to the stream
 * @return     true when read success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * T_TRANSMISSION_STREAM stream;
 *
 * void stream_callback(T_TRANSMISSION_STREAM_EVENT event);
 *
 * int main(void)
 * {
 *
 *     // ...
 *     // initialize stream
 *     // ...
 *
 *     if (transmission_stream_register_callback(stream, stream_callback) == true)
 *     {
 *         // ...
 *     }
 *     else
 *     {
 *         // ...
 *     }
 *
 * }
 *
 * @endcode
 */
bool transmission_stream_register_callback(T_TRANSMISSION_STREAM stream,
                                           T_TRANSMISSION_STREAM_CB callback);
/**
 * @brief      Get the length of the data in the stream
 * @note       When you need to read the data, call this function to get the
 *             length of the data in the stream
 * @param[in]  stream transmission stream pointer
 * @return     length of data in the stream
 */
uint32_t transmission_stream_get_bytes_available(T_TRANSMISSION_STREAM stream);

/**
 * @brief      Get MTU(Maximum Transmission Unit) of the stream
 * @note       User can call this funtion to get MTU,The length of data sent
 *             each time should not exceed MTU, otherwise
 *             transmission will fail
 * @param[in]  stream transmission stream pointer
 * @return     Maximum Transmission Unit
 */
uint32_t transmission_stream_get_mtu(T_TRANSMISSION_STREAM stream);

/**
 * @brief      Write data to the stream
 * @note       It means that user will send data by the stream, maximum length of data
 *             that can be sent at a time is limited, user can get it by the API
 *             @ref transmission_stream_get_mtu.
 *             User can try to use stream write function in two cases:
 *             1. When user have data to send, user can actively call this function.
 *                But when write fail, suspending of actively write is recommended.
 *             2. When the callback function registerd by the user receives the
 *                @ref TRANSMISSION_STREAM_READY_TO_WRITE_EVENT, it means that the state
 *                of the stream has changed from being unable to send data to being able
 *                to send data, user can call this function to continue sending data
 * @param[in]  stream  transmission stream pointer
 * @param[in]  data    the data you want write to the stream
 * @param[in]  length  length of the data you want to write to the stream
 * @return     true when write success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * bool send_data(T_TRANSMISSION_STREAM stream, uint8_t *data, uint32_t length)
 * {
 *     uint32_t remaining_length = length;
 *     uint32_t mtu = transmission_stream_get_mtu(stream);
 *     uint32_t transmission_length = 0;
 *
 *     while(remaining_length != 0)
 *     {
 *         // Calculate the length of data that can be sent at a time
 *         if(remaining_length >= mtu)
 *         {
 *             transmission_length = mtu;
 *         }
 *         else
 *         {
 *             transmission_length = remaining_length;
 *         }
 *
 *         if(transmission_stream_write(stream, data, transmission_length) == true)
 *         {
 *             remaining_length -= transmission_length;
 *         }
 *         else
 *         {
 *             return false;
 *         }
 *     }
 *
 *     return true;
 * }
 *
 * @endcode
 */
bool transmission_stream_write(T_TRANSMISSION_STREAM stream, const uint8_t *data, uint32_t length);

/**
 * @brief      Read data in the stream
 * @note       When you receive the event @ref TRANSMISSION_STREAM_READY_TO_READ_EVENT,
 *             it means that stream have reviced data, user can call this function to
 *             read the data from stream.User can call the API @ref transmission_stream_get_bytes_available
 *             to get the length of data in the stream
 * @param[in]  stream  transmission stream pointer
 * @param[in]  length  the length you want to read from the stream
 * @param[out] buffer  the data that read from the stream.
 * @return     true when read success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * T_TRANSMISSION_STREAM stream;
 *
 * // ...
 * // Initialize stream and wait to stream is connected
 * // ...
 *
 * void stream_ready_to_read_event_process(void)
 * {
 *     uint32_t length = transmission_stream_get_bytes_available(stream);
 *     uint8_t *buffer = os_mem_alloc(RAM_TYPE_BUFFER_OFF, length);
 *     if(buffer == NULL)
 *     {
 *         return;
 *     }
 *
 *     if(transmission_stream_read(stream, length, buffer) == false)
 *     {
 *         os_mem_free(buffer);
 *         return;
 *     }
 *
 *     // ...
 *     // Processing of received data
 *     // ...
 * }
 *
 * void stream_callback(T_TRANSMISSION_STREAM_EVENT event)
 * {
 *      switch (event)
 *      {
 *          case TRANSMISSION_STREAM_READY_TO_READ_EVENT:
 *              {
 *                  stream_ready_to_read_event_process();
 *              }
 *              break;
 *
 *          // ...
 *
 *          default:
 *              break;
 *      }
 * }
 *
 * @endcode
 */
bool transmission_stream_read(T_TRANSMISSION_STREAM stream, uint32_t length, uint8_t *buffer);

/**
 * @brief      Transmission stream de-initialize function prototype
 * @note       When you no longer need to use stream, you can call this function to
 *             de-initialize stream and release the resources that stream occupies
 * @param[in]  stream transmission stream pointer
 * @return     void
 */
void transmission_stream_deinit(T_TRANSMISSION_STREAM *stream);

/**
* @brief      Create the transmission stream
* @note       This API is for the sub-module that instancing abstract transmission stream,
*             not for the user to operate the stream. It is recommended for use in sub-module
*             initialization functions.
* @return     the transmission stream pointer, please check if it is NULL.
*/
T_TRANSMISSION_STREAM transmission_stream_create(void);

/**
* @brief      Destroy the transmission stream
* @note       Transmission stream can be destroyed by this API. This API is for the sub-module
*             that instancing abstract transmission stream, not for the user to operate the stream.
*             It is recommended for use in sub-module de-initialization functions.
* @param      stream  pointer of type @ref T_TRANSMISSION_STREAM
* @return     void
*/
void transmission_stream_destroy(T_TRANSMISSION_STREAM *stream);

/**
 * @brief      Set instance for stream register callback function
 * @note       Register callback is a function pointer in the transmission stream, sub-module that
 *             instancing abstract transmission stream should implement register callback function.
 *             With this API, sub-module can assign the implemented function to the funtion
 *             pointer to instance abstract interface. This API is for the sub-module,
 *             not for the user to operate the stream. it is recommended for use in sub-module
 *             initialization functions. The return value of the initialization function need to be
 *             @ref T_TRANSMISSION_STREAM type
 * @param[in]  stream             transmission stream pointer
 * @param[in]  register_callback  implemented register callback function
 * @return     true when read success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * static bool xxx_stream_register_callback(T_TRANSMISSION_STREAM stream, T_TRANSMISSION_STREAM_CB callback)
 * {
 *     // ...
 * }
 *
 * T_TRANSMISSION_STREAM xxx_stream_init(void)
 * {
 *     T_TRANSMISSION_STREAM stream = transmission_stream_create();
 *     if(stream)
 *     {
 *         if(transmission_stream_set_instance_for_register_callback(stream, xxx_stream_register_callback) == false)
 *         {
 *             return NULL;
 *         }
 *     }
 * }
 *
 * @endcode
 */
bool transmission_stream_set_instance_for_register_callback(T_TRANSMISSION_STREAM stream,
                                                            P_STREAM_REGISTER_CALLBACK register_callback);
/**
 * @brief      Set instance for stream get bytes available function
 * @note       Get bytes available is a function pointer in the transmission stream, sub-module that
 *             instancing abstract transmission stream should implement get bytes available function.
 *             With this API, sub-module can assign the implemented function to the funtion
 *             pointer to instance abstract interface. This API is for the sub-module,
 *             not for the user to operate the stream. it is recommended for use in sub-module
 *             initialization functions. The return value of the initialization function need to be
 *             @ref T_TRANSMISSION_STREAM type
 * @param[in]  stream               transmission stream pointer
 * @param[in]  get_bytes_available  implemented get bytes available function
 * @return     true when read success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * static uint32_t xxx_stream_get_bytes_available(T_TRANSMISSION_STREAM stream)
 * {
 *     // ...
 * }
 *
 * T_TRANSMISSION_STREAM xxx_stream_init(void)
 * {
 *     T_TRANSMISSION_STREAM stream = transmission_stream_create();
 *     if(stream)
 *     {
 *         if(transmission_stream_set_instance_for_get_bytes_available(stream, xxx_stream_get_bytes_available) == false)
 *         {
 *             return NULL;
 *         }
 *     }
 * }
 *
 * @endcode
 */
bool transmission_stream_set_instance_for_get_bytes_available(T_TRANSMISSION_STREAM stream,
                                                              P_STREAM_GET_BYTES_AVAILABLE get_bytes_available);

/**
* @brief      Set instance for stream get MTU available function
* @note       Get MTU is a function pointer in the transmission stream, sub-module that
*             instancing abstract transmission stream should implement get MTU function.
*             With this API, sub-module can assign the implemented function to the funtion
*             pointer to instance abstract interface. This API is for the sub-module,
*             not for the user to operate the stream. it is recommended for use in sub-module
*             initialization functions. The return value of the initialization function need to be
*             @ref T_TRANSMISSION_STREAM type
* @param[in]  stream   transmission stream pointer
* @param[in]  get_mtu  implemented get MTU function
* @return     true when read success, false otherwise
*
* <b>Example Usage</b>
* @code
*
* static uint32_t xxx_stream_get_mtu(T_TRANSMISSION_STREAM stream)
* {
*     // ...
* }
*
* T_TRANSMISSION_STREAM xxx_stream_init(void)
* {
*     T_TRANSMISSION_STREAM stream = transmission_stream_create();
*     if(stream)
*     {
*         if(transmission_stream_set_instance_for_get_mtu(stream, xxx_stream_get_mtu) == false)
*         {
*             return NULL;
*         }
*     }
* }
*
* @endcode
*/
bool transmission_stream_set_instance_for_get_mtu(T_TRANSMISSION_STREAM stream,
                                                  P_STREAM_GET_MTU get_mtu);

/**
 * @brief      Set instance for stream write function
 * @note       Write is a function pointer in the transmission stream, sub-module that
 *             instancing abstract transmission stream should implement write function.
 *             With this API, sub-module can assign the implemented function to the funtion
 *             pointer to instance abstract interface. This API is for the sub-module,
 *             not for the user to operate the stream. it is recommended for use in sub-module
 *             initialization functions. The return value of the initialization function need
 *             to be @ref T_TRANSMISSION_STREAM type
 * @param[in]  stream  transmission stream pointer
 * @param[in]  write   implemented write function
 * @return     true when read success, false otherwise
 *
 * <b>Example Usage</b>
 * @code
 *
 * static bool xxx_stream_write(T_TRANSMISSION_STREAM stream, const uint8_t *data, uint32_t length)
 * {
 *     // ...
 * }
 *
 * T_TRANSMISSION_STREAM xxx_stream_init(void)
 * {
 *     T_TRANSMISSION_STREAM stream = transmission_stream_create();
 *     if(stream)
 *     {
 *         if(transmission_stream_set_instance_for_write(stream, xxx_stream_write) == false)
 *         {
 *             return NULL;
 *         }
 *     }
 * }
 *
 * @endcode
 */
bool transmission_stream_set_instance_for_write(T_TRANSMISSION_STREAM stream, P_STREAM_WRITE write);

/**
* @brief      Set instance for stream read function
* @note       Read is a function pointer in the transmission stream, sub-module that
*             instancing abstract transmission stream should implement read function.
*             With this API, sub-module can assign the implemented function to the funtion
*             pointer to instance abstract interface. This API is for the sub-module,
*             not for the user to operate the stream. it is recommended for use in sub-module
*             initialization functions. The return value of the initialization function need to be
*             @ref T_TRANSMISSION_STREAM type
* @param[in]  stream   transmission stream pointer
* @param[in]  read     implemented read function
* @return     true when read success, false otherwise
*
* <b>Example Usage</b>
* @code
*
* static bool xxx_stream_read(T_TRANSMISSION_STREAM stream, uint32_t length, uint8_t *buffer)
* {
*     // ...
* }
*
* T_TRANSMISSION_STREAM xxx_stream_init(void)
* {
*     T_TRANSMISSION_STREAM stream = transmission_stream_create();
*     if(stream)
*     {
*         if(transmission_stream_set_instance_for_read(stream, xxx_stream_read) == false)
*         {
*             return NULL;
*         }
*     }
* }
*
* @endcode
*/
bool transmission_stream_set_instance_for_read(T_TRANSMISSION_STREAM stream, P_STREAM_READ read);

/**
* @brief      Set instance for de-initialize function
* @note       De-initialize is a function pointer in the transmission stream, sub-module that
*             instancing abstract transmission stream should implement de-initialize function.
*             With this API, sub-module can assign the implemented function to the funtion
*             pointer to instance abstract interface. This API is for the sub-module,
*             not for the user to operate the stream. it is recommended for use in sub-module
*             initialization functions. The return value of the initialization function need to be
*             @ref T_TRANSMISSION_STREAM type
* @param[in]  stream   transmission stream pointer
* @param[in]  deinit   implemented deinit function
* @return     true when read success, false otherwise
*
* <b>Example Usage</b>
* @code
*
* static void xxx_stream_deinit(T_TRANSMISSION_STREAM stream)
* {
*     // ...
* }
*
* T_TRANSMISSION_STREAM xxx_stream_init(void)
* {
*     T_TRANSMISSION_STREAM stream = transmission_stream_create();
*     if(stream)
*     {
*         if(transmission_stream_set_instance_for_deinit(stream, xxx_stream_deinit) == false)
*         {
*             return NULL;
*         }
*     }
* }
*
* @endcode
*/
bool transmission_stream_set_instance_for_deinit(T_TRANSMISSION_STREAM stream,
                                                 P_STREAM_DEINIT deinit);

/**
* @brief      Assigne stream implemented by sub-module to transmission stream
* @note       The stream pointer implemented by the sub-module can be stored in the transmission stream,
*             by this API, so when the transmission stream call the API to control the sub-module(for
*             example @ref transmission_stream_get_mtu), the function in sub-module can get the required
*             information from the saved stream pointer. This API is for the sub-module, not for the user
*             to operate the stream.it is recommended for use in sub-module initialization functions. The
*             return value of the initialization function need to be @ref T_TRANSMISSION_STREAM type
* @param[in]  stream           transmission stream pointer
* @param[in]  stream_instance  stream instance of the sub-module
* @return     true when read success, false otherwise
*
* <b>Example Usage</b>
* @code
*
* T_TRANSMISSION_STREAM xxx_stream_init(void)
* {
*     T_XXX_STREAM xxx_stream_instance = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(T_XXX_STREAM_STRUCT));
*     if (xxx_stream_instance)
*     {
*         T_TRANSMISSION_STREAM stream = transmission_stream_create();
*         if (stream)
*         {
*             transmission_stream_set_instance(stream, (void *)xxx_stream_instance);
*         }
*     }
* }
*
* @endcode
*/
bool transmission_stream_set_instance(T_TRANSMISSION_STREAM stream, void *stream_instance);

/**
* @brief      Get the stream pointer of the sub-module stored in the transmission stream
* @note       If sub-module have used @ref transmission_stream_set_instance to save the stream pointer
*             of the sub-module in the transmission stream, sub-module can use this API to get saved
*             stream pointer form the transmission stream. This API is for the sub-module, not for the
*             user to operate the stream
* @param[in]  stream  transmission stream pointer
* @return     the stream pointer of the sub-module
*/
void *transmission_stream_get_instance(T_TRANSMISSION_STREAM stream);

/** @} */ /* End of group Transmission_Stream_Exported_Functions */

/** @} */ /* End of group TRANSMISSION_STREAM */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _TRANSMISSION_STREAM_ */
