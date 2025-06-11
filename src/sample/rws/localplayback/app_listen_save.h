/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_LISTEN_SAVE_H_
#define _APP_LISTEN_SAVE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "string.h"
#include "rtl876x.h"
#include "remote.h"
#include "listen_save_buffer.h"

#define TEMP_FILE_NAME_STRING   _T("temp_save_file.rtk0")

#define LISTEN_SAVE_BUF_SIZE        3 * 1024
#define LISTEN_SAVE_WRITE_LEVEL     (LISTEN_SAVE_BUF_SIZE - 1024)

#define SOME_PKTS_SYNC_EN           1
#define SYNC_PKT_NUM                3


/** @defgroup APP_LISTEN_SAVE App listen save
  * @brief App listen save
  * @{
  */

typedef enum
{
    LISTEN_SAVE_SUCCESS             = 0x00,
    LISTEN_SAVE_CREATE_ERROR        = 0x01,
    LISTEN_SAVE_READ_ERROR          = 0x02,
    LISTEN_SAVE_OPEN_FILE_ERROR     = 0x03,
    LISTEN_SAVE_CRC_ERROR           = 0x04,
    LISTEN_SAVE_CLOSE_FILE_ERROR    = 0x05,
    LISTEN_SAVE_WRITE_FILE_ERROR    = 0x06,
    LISTEN_SAVE_PKT_NUM_ERROR       = 0x07,
    LISTEN_SAVE_SYSTEM_ERROR        = 0x08,
} T_LISTEN_SAVE_ERROR;

/**  @brief app remote msg*/
typedef enum
{
    LISTEN_SAVE_REMOTE_MSG_START_SYNC                       = 0x00,
    LISTEN_SAVE_REMOTE_MSG_CREATE_FILE_SYNC                 = 0x01,
    LISTEN_SAVE_REMOTE_MSG_SEC_CREATE_RESULT_SYNC           = 0x02,
    LISTEN_SAVE_REMOTE_MSG_SAVING_SYNC                      = 0x03,
    LISTEN_SAVE_REMOTE_MSG_FRAME_INFO_SYNC                  = 0x04,
    LISTEN_SAVE_REMOTE_MSG_SAVE_END_SYNC                    = 0x05,
    LISTEN_SAVE_REMOTE_MSG_FILE_INFO_SYNC                   = 0x06,

    LISTEN_SAVE_REMOTE_MSG_SEC_RESULT_SYNC                  = 0x20,

    LISTEN_SAVE_REMOTE_MSG_MAX
} T_LISTEN_SAVE_REMOTE_MSG;

typedef enum
{
    LISTEN_SAVE_IDLE                        = 0x00,
    LISTEN_SAVE_START                       = 0x01,
    LISTEN_SAVE_DOING                       = 0x02,
    LISTEN_SAVE_DONE                        = 0x03,
    LISTEN_SAVE_END                         = 0x04,

} T_LISTEN_SAVE_STATE;

//======================remote==================//
typedef struct _LISTEN_SAVE_FILE_HEADER_STRUCT
{
    uint8_t     header[10];
    uint8_t     attr0[10];
    uint8_t     attr0_type_data[4];
    uint8_t     role;
    uint8_t     attr1[10];
    uint32_t    file_total_len;
    uint32_t    toal_frame_num;
} __attribute__((packed)) LISTEN_SAVE_FILE_HEADER_STRUCT;

typedef union
{
    struct
    {
        uint8_t sampling_frequency;
        uint8_t channel_mode;
        uint8_t block_length;
        uint8_t subbands;
        uint8_t allocation_method;
        uint8_t min_bitpool;
        uint8_t max_bitpool;
    } sbc;
    struct
    {
        uint8_t  object_type;
        uint16_t sampling_frequency;
        uint8_t  channel_number;
        bool     vbr_supported;
        uint8_t  allocation_method;
        uint32_t bit_rate;
    } aac;
    struct
    {
        uint8_t  info[12];
    } vendor;
} LISTEN_SAVE_CODEC_INFO;

typedef struct _LISTEN_SAVE_FILE_STRUCT
{
    uint16_t                    character_id;
    uint8_t                     name_len;
    uint8_t                     name[0];
} LISTEN_SAVE_FILE_STRUCT;

typedef struct
{
    uint8_t                     pkt_num;
    T_LISTEN_SAVE_SYNC_INFO     pkt_info[4];
} T_LISTEN_SAVE_WAIT_PKT_INFO;

typedef struct _LISTEN_SAVE_FUNCTION_STRUCT
{
    uint8_t                     file_type;
    bool                        pri_flag;
    bool                        sec_flag;
    uint16_t                    cur_seq_num;
    uint32_t                    cur_offset;
    uint8_t                     *p_temp_buf;
    uint16_t                    buf_used_size;
    uint32_t                    file_total_len;
    uint32_t                    toal_frame_num;
} LISTEN_SAVE_FUNCTION_STRUCT;

//*******************************************//

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LISTEN_SAVE_Exported_Functions App listen save Functions
    * @{
    */


void app_listen_save_mode_enter(void);
void app_listen_save_mode_quit(void);

/**
    * @brief  listen save module init
    * @param  void
    * @return void
    */
void app_listen_save_init(void);

/** @} */ /* End of group APP_LISTEN_SAVE_Exported_Functions */

/** End of APP_LISTEN_SAVE
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_LISTEN_SAVE_H_ */
