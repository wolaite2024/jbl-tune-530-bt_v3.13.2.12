/**
 * @copyright Copyright (C) 2022 Realtek Semiconductor Corporation.
 *
 * @file usb_audio_stream.h
 * @version 1.0
 *
 */

#ifndef __USB_AUDIO_STREAM_H__
#define __USB_AUDIO_STREAM_H__
#include "audio_type.h"

/**
 * @defgroup UAS
 * @brief module for universal usb audio process
 * @{
 *
 * |Terms         |Details                                               |
 * |--------------|------------------------------------------------------|
 * |\b USB        |Universal Serial Bus.                                 |
 * |\b UAS        |USB Audio Streaming                                   |
 *
 *
 *
 * @section UAS_Usage_Chapter How to use this module.
 *
 * - Initialize the USB Audio stream. \n
 *  - Step1: Initialize USB Audio Stream callbacks.
 *  - Step2: Bind callbacks to the stream.
 *  - sample code:
 * @code
*       void *uas_func_create(T_STREAM_ATTR, void *)
        {
            Initialization of USB Audio stream.
        }

        bool *uas_func_release(void *)
        {
            Release USB Audio stream.
        }

        bool *uas_func_start(void *)
        {
            prepare to process USB Audio data.
        }

        bool *uas_func_stop(void *)
        {
            stop processing USB Audio data.
        }

        bool *uas_func_xmit(void *)
        {
            if(len_per_proc < usb_audio_stream_get_data_len())
            {
                usb_audio_stream_data_peek();
                process data
                usb_audio_stream_data_flush();
            }
        }

        bool *uas_func_ctrl(void *handle, uint8_t cmd, void *param)
        {
            switch (cmd)
            {
            case CTRL_CMD_VOL_CHG:
                {
                    Handle the volume of USB Audio.
                }
                break;
            case CTRL_CMD_MUTE:
                {
                    Mute/Unmute USB Audio.
                }
                break;
            case CTRL_CMD_STOP:
                {
                    Stop the USB Audio.
                }
                break;
            }
        }

        T_UAS_FUNC uas_funcs =
        {
            .create = uas_func_create,
            .release = uas_func_release,
            .start = uas_func_start,
            .stop = uas_func_stop,
            .xmit = uas_func_xmit,
            .ctrl = uas_func_ctrl,
        }

        uint32_t id = usb_audio_stream_ual_bind(T_USB_AUDIO_STREAM_TYPE, pipe, &uas_funcs);
 *
 * @endcode
 *
 * - Start the USB Audio stream. \n
 *  - case1: uas_func_start is synchronous, the start process is complete as long as start function returns.
 * @code
        usb_audio_stream_start(id, SYNC)
        {
            uas_func_start();
        }
 *
 * @endcode
 *  - case2: uas_func_start is asynchronous, /ref usb_audio_stream_start_complete must be called when usb audio start process is complete.
 * @code
        usb_audio_stream_start(id, ASYNC)
        {
            uas_func_start();
        }
        ....
        // start process is complete
        usb_audio_stream_start_complete();
 *
 * @endcode
 * - Stop the USB Audio stream. \n
 *  - case1: uas_func_stop is synchronous, the stop process is complete as long as stop function returns.
 * @code
        usb_audio_stream_stop(id, SYNC)
        {
            uas_func_stop();
        }
 *
 * @endcode
 *  - case2: uas_func_stop is asynchronous, /ref usb_audio_stream_stop_complete must be called when usb audio stop process is complete.
 * @code
        usb_audio_stream_stop(id, ASYNC)
        {
            uas_func_stop();
        }
        ....
        // stop process is complete
        usb_audio_stream_stop_complete();
 *
 * @endcode
 *
 */

/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream control command
 *
 * @ingroup UAS
 */
/** @brief  The format type of USB Audio Stream.*/
#define T_STREAM_ATTR   T_AUDIO_PCM_ATTR
/** @brief  The state of start/stop cb in @ref T_UAS_FUNC is synchronous.*/
#define SYNC    true
/** @brief  The state of start/stop cb in @ref T_UAS_FUNC is asynchronous.*/
#define ASYNC   false
/** @brief  The label of USB Audio Stream.*/
#define USB_AUDIO_STREAM_LABEL_1 1


/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream control command
 *
 * @ingroup UAS
 */
typedef enum
{
    CTRL_CMD_VOL_CHG,       /**<  A volume change control command. */
    CTRL_CMD_MUTE,          /**<  A mute set control command. */
    CTRL_CMD_STOP,          /**<  A usb stop control command. */
} T_CTRL_CMD;

/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream direction
 *
 * @ingroup UAS
 */
typedef enum
{
    USB_AUDIO_STREAM_TYPE_OUT,      /**<  PC direction is OUT. */
    USB_AUDIO_STREAM_TYPE_IN,       /**<  PC direction is IN. */
    USB_AUDIO_STREAM_TYPE_MAX,
} T_USB_AUDIO_STREAM_TYPE;

/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream volume
 *
 * @ingroup UAS
 */
typedef struct _uas_vol
{
    uint32_t range: 16;         /**<  The range of the volume. */
    uint32_t cur: 16;           /**<  The current volume value. */
} T_UAS_VOL;

/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream control ,vol/mute .etc
 *
 * @ingroup UAS
 */
typedef struct _uas_ctrl
{
    T_UAS_VOL vol;          /**<  The volume control of USB Audio stream. */
    bool mute;              /**<  The mute control of USB Audio stream. */
} T_UAS_CTRL;

/**
 * usb_audio_stream.h
 *
 * @brief   USB Audio stream callbacks, realized by user application layer
 *
 * @param create the cb will be called when usb audio is activated
 * @param release the cb will be called when usb audio is deactivated
 * @param start the cb will be called when @ref usb_audio_stream_start is called
 * @param stop the cb will be called when @ref usb_audio_stream_stop is called
 * @param xmit the cb will be called only if usb audio stream is already started
 * @param ctrl the cb is used for usb audio control defined in @ref T_CTRL_CMD
 *
 * @ingroup UAS
 */
typedef struct  _uas_func
{
    void *(*create)(T_STREAM_ATTR, void *);
    bool (*release)(void *);
    bool (*start)(void *);
    bool (*stop)(void *);
    bool (*xmit)(void *);
    bool (*ctrl)(void *, uint8_t, void *);
} T_UAS_FUNC;

/**
 * usb_audio_stream.h
 *
 * @brief   bind user application layer cbs to usb audio stream
 *
 * @param[in] stream_type @ref T_USB_AUDIO_STREAM_TYPE
 * @param[in] label the pipe of @ref T_USB_AUDIO_STREAM
 * @param[in] func @ref T_UAS_FUNC
 *
 * @return user application id
 * @par Example
 * Please refer to "Initialize the USB Audio stream" in @ref UAS_Usage_Chapter.
 * @ingroup UAS
 */
uint32_t usb_audio_stream_ual_bind(uint8_t stream_type, uint8_t label, T_UAS_FUNC *func);

/**
 * usb_audio_stream.h
 *
 * @brief   start usb audio stream
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] sync if start cb in @ref T_UAS_FUNC is synchronous, the start process is complete as long as start function returns, start is synchronous,
 *                 otherwise it is asynchronous
 *
 * \return success of fail
 * @par Example
 * Please refer to "Start the USB Audio stream" in @ref UAS_Usage_Chapter.
 * \ingroup UAS
 */
bool usb_audio_stream_start(uint32_t id, bool sync);

/**
 * usb_audio_stream.h
 *
 * @brief   start usb audio stream completely
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @details the function MUST be called if usb audio start process is complete
 * @par Example
 * Please refer to "Start the USB Audio stream" in @ref UAS_Usage_Chapter.
 *
 * @ingroup UAS
 */
void usb_audio_stream_start_complete(uint32_t id);

/**
 * usb_audio_stream.h
 *
 * @brief   stop usb audio stream
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] sync if stop cb in @ref T_UAS_FUNC is synchronous, the stop process is complete as long as stop function returns, start is synchronous,
 *                 otherwise it is asynchronous
 *
 * @return success of fail
 * @par Example
 * Please refer to "Stop the USB Audio stream" in @ref UAS_Usage_Chapter.
 * @ingroup UAS
 */
bool usb_audio_stream_stop(uint32_t id, bool sync);

/**
 * usb_audio_stream.h
 *
 * @brief   stop usb audio stream completely
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @details the function MUST be called if @ref usb_audio_stream_stop returns true and usb audio stop process is complete
 * @par Example
 * Please refer to "Stop the USB Audio stream" in @ref UAS_Usage_Chapter.
 * @ingroup UAS
 */
void usb_audio_stream_stop_complete(uint32_t id);

/**
 * usb_audio_stream.h
 *
 * @brief   usb audio stream remaining data length
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @par Example
 * Please refer to "Initialize the USB Audio stream" in @ref UAS_Usage_Chapter.
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_get_data_len(uint32_t id);

/**
 * usb_audio_stream.h
 *
 * @brief   peek usb audio stream data, note the data is not removed from usb audio stream buffer until @ref usb_audio_stream_data_flush
 *          is called
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] buf data buffer
 * @param[in] len length of data to peek
 *
 * @return actual length of peeked data
 *
 * @par Example
 * Please refer to "Initialize the USB Audio stream" in @ref UAS_Usage_Chapter.
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_data_peek(uint32_t id, void *buf, uint32_t len);

/**
 * usb_audio_stream.h
 *
 * @brief   remove data from usb audio stream buffer
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] len length of data to remove
 *
 * @return actual length of removed data
 * @par Example
 * Please refer to "Initialize the USB Audio stream" in @ref UAS_Usage_Chapter.
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_data_flush(uint32_t id, uint32_t len);

/**
 * usb_audio_stream.h
 *
 * @brief   write usb audio stream data
 *          is called
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] buf data buffer
 * @param[in] len length of data to write
 *
 * @return actual length of written data
 * @par Example
 * Sample code:
 * @code
 *      void mic_read()
 *      {
 *          if (usb_audio_stream_get_remaining_pool_size(id) < required_len)
 *          {
 *              usb_audio_stream_data_flush(id, required_len);
 *          }
 *          usb_audio_stream_data_write(id, buf, required_len);
 *      }
 * @endcode
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_data_write(uint32_t id, void *buf, uint32_t len);

/**
 * usb_audio_stream.h
 *
 * @brief   read usb audio stream data, note the data will be removed from usb audio stream buffer after read
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 * @param[in] buf data buffer
 * @param[in] len length of data to read
 *
 * @return actual length of read data
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_data_read(uint32_t id, void *buf, uint32_t len);

/**
 * usb_audio_stream.h
 *
 * @brief   get usb audio remaining pool size
 *
 * @param[in] id return value of @ref usb_audio_stream_ual_bind
 *
 * @return remaining pool size
 * @par Example
 * Please refer to @ref usb_audio_stream_data_write.
 *
 * @ingroup UAS
 */
uint32_t usb_audio_stream_get_remaining_pool_size(uint32_t id);
/**
 * usb_audio_stream.h
 *
 * @brief   handle usb audio stream event
 *
 * @param[in] evt event
 * @param[in] param optional parameter
 * @par Example
 * Sample code:
 * @code
 *  void app_usb_msg_handle (void)
 *  {
 *      subtype = msg->subtype;
 *      group = USB_MSG_GROUP(subtype);
 *      switch(group)
 *      {
 *      case  USB_EVT:
 *          {
 *              usb_audio_stream_evt_handle();
 *          }
 *          break;
 *      }
 *  }
 * @endcode
 *
 * @ingroup UAS
 */
void usb_audio_stream_evt_handle(uint8_t evt, uint32_t param);

/**
 * usb_audio_stream.h
 *
 * @brief   init usb audio stream
 * @par Example
 * Sample code:
 * @code
 * void usb_init (void)
 * {
 *     usb_audio_stream_init();
 * }
 * @endcode
 *
 * @ingroup UAS
 */
void usb_audio_stream_init(void);

/** @}*/
/** End of UAS
*/

#endif
