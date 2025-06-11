/**
*****************************************************************************************
*     Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_playback_trans_file.h
   * @brief     Head file for using SPP_PLAYBACK updata playback file
   * @author    lemon
   * @date      2020-05-01
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2019 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _APP_SPP_PLAYBACK_H_
#define _APP_SPP_PLAYBACK_H_

//#include "patch_header_check.h"
#include "app_relay.h"
#include "stdint.h"
#include "gap.h"
#include "app_gpio.h"
/** @defgroup  APP_PLAYBACK_SERVICE APP SPP_PLAYBACK handle
    * @brief APP PLAYBACK Service to implement SPP_PLAYBACK feature
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_PLAYBACK_SERVICE_Exported_Macros App SPP_PLAYBACK service Exported Macros
    * @brief
    * @{
    */

#define APP_PLAYBACK_TRANS_COUPLE_EN          1

/*ota version num*/
#define BLE_PLAYBACK_VERSION     0x2
#define SPP_PLAYBACK_VERSION     0x5
/*ic type*/
#define PLAYBACK_BBPRO              0x04
#define PLAYBACK_BBPRO2             0x07
/*query info*/
//#define PLAYBACK_MAX_BUFFER_SIZE     PLAYBACK_BUF_CHECK_SIZE

//rx and write
#define PLAYBACK_PKT_SIZE           0x300 //0x100
#define PLAYBACK_BUF_CHECK_SIZE     (PLAYBACK_PKT_SIZE * 3) //0x800
#define PLAYBACK_PROTOCOL_VERSION   1 // 2020-10-20 version:1
#define PLAYBACK_MODE_SINGLE        0x00
#define PLAYBACK_MODE_COUPLE        0x01
#define PLAYBACK_MODE_COUPLE_PRI    (0x01 << 1)
#define PLAYBACK_MODE_COUPLE_SEC    0x00

#define PLAYBACK_LIST_HEAD_LEN          1
#define PLAYBACK_LIST_READ_LENTH_LEN    2
//read and tx
#define PLAYBACK_READ_FRAME_SIZE    0x100
#define PLAYBACK_READ_PKT_SIZE      (PLAYBACK_READ_FRAME_SIZE + \
                                     PLAYBACK_LIST_HEAD_LEN + \
                                     PLAYBACK_LIST_READ_LENTH_LEN)

#define PLAYBACK_BUF_A_EVENT      0x01
#define PLAYBACK_BUF_B_EVENT      0x02

/** @brief  spp playback command length. */
#define PLAYBACK_LENGTH_GET_INFO            0
#define PLAYBACK_LENGTH_GET_LIST_DATA       1
#define PLAYBACK_LENGTH_BUFFER_CHECK_EN     4
#define PLAYBACK_LENGTH_VALID_SONG          6
#define PLAYBACK_LRNGTH_TRIGGER_ROLE_SWAP   0
#define PLAYBACK_LENGTH_TRANS_CANCEL        0
#define PLAYBACK_LRNGTH_DELETE_SONG         2
#define PLAYBACK_LRNGTH_DELETE_ALL_SONG     0

//byte wide
#define PLAYBACK_LEN_PLAYLIST_IDX           2 // ~ 2020-10-19: 1
#define PLAYBACK_LEN_FILE_IDX               2
#define PLAYBACK_LEN_NAME_LENTH             2
/*bit set of device info data3*/
#define PB_DEVICE_FEATURE_SUPPORT_BUFFER_CHECK     (1 << 0)
#define PB_DEVICE_FEATURE_RWS_SINGLE               (1 << 5)
#define PB_DEVICE_FEATURE_RWS_LEFT                 (1 << 6)
#define PB_DEVICE_FEATURE_RWS_UPDATE_SUCCESS       (1 << 7)


/** End of APP_PLAYBACK_SERVICE_Exported_Macros
    * @}
    */


/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_PLAYBACK_SERVICE_Exported_Types APP SPP_PLAYBACK Service Exported Types
    * @brief
    * @{
    */
/** @brief  PLAYBACK support format tye */
typedef enum
{
    PB_FORMAT_TYPE_DEFAULT          = 0x00,
    PB_FORMAT_TYPE_AAC              = 0x01,
    PB_FORMAT_TYPE_MP3              = 0x02,
    PB_FORMAT_TYPE_FLAC             = 0x04,
    PB_FORMAT_TYPE_WAV              = 0x08,
    PB_FORMAT_TYPE_RSVD
} T_PLAYBACK_FORMAT_TYPE;
typedef union
{
    uint8_t value[32];
    struct
    {
        uint16_t pkt_size;          //byte0-byte1
        uint16_t buf_check_size;    //byte2-byte3
        uint8_t  protocol_ver;      //byte4
        uint8_t  mode;              //byte5
        uint8_t  ic_type;           //byte6
        uint8_t  song_format_type;  //byte7
        uint8_t  rsvd[25];          //byte8-byte31
    };
} PLAYBACK_DEVICE_INFO;

typedef struct _PLAYLIST_UPDATE_STRUCT
{
    uint16_t playlist_idx;
    uint16_t file_idx;
    uint16_t name_len;
} PB_PLAYLIST_UPDATE_STRUCT;

typedef struct _DELETE_FILE_STRUCT
{
    uint16_t file_idx;
    uint16_t name_len;
} PB_DELETE_FILE_STRUCT;

/** @brief  PLAYBACK transfer mode */
typedef enum
{
    PB_TRANS_RET_SUCCESS                      = 0x01,
    PB_TRANS_RET_CRC_ERROR                    = 0x05,
    PB_TRANS_RET_LENTH_ERROR                  = 0x06,
    PB_TRANS_RET_WRITE_ERROR                  = 0x07,
    PB_TRANS_RET_READ_ERROR                   = 0x10,
    PB_TRANS_RET_CREAT_ERROR                  = 0x11,
    PB_TRANS_RET_RX_DATA_ERROR                = 0x12,
    PB_TRANS_RET_OPERATION_ERROR              = 0x13,
    PB_TRANS_RET_OFFSET_ERROR                 = 0x14,
    PB_TRANS_RET_ROLE_SWAP_ERROR              = 0x15,
    PB_TRANS_RET_COUPLE_DISCON_ERROR          = 0x16,
    PB_TRANS_RET_CLOSE_ERROR                  = 0x17,
    PB_TRANS_RET_BUF_FULL_ERROR               = 0x18,
    PB_TRANS_RET_SEQUENCE_ERROR               = 0x19,
    PB_TRANS_RET_DELETE_ERROR                 = 0x1A,
    PB_TRANS_RET_DELETE_OPENED_ERROR          = 0x1B,

    PB_TRANS_RET_BUF_A_SUCCESS                = 0xF1,
    PB_TRANS_RET_BUF_B_SUCCESS                = 0xF2,

} T_SPP_PB_RET;

/** @brief  playback file type */
typedef enum
{
    PLAYBACK_HEAD_BIN           = 0x00,
    PLAYBACK_NAME_BIN           = 0x01,
} T_SPP_PB_FILE_TYPE;
/** @brief  playback read file state */
typedef enum
{
    PLAYBACK_TRANS_START          = 0x00,
    PLAYBACK_TRANS_CONTINUE       = 0x01,
    PLAYBACK_TRANS_END            = 0x02,
} T_SPP_PB_READ_STATE;

/** @brief  rws ota sync cmd */
typedef enum
{
    RWS_SPP_PLAYBACK_PB_ACK,
    RWS_SPP_PLAYBACK_PB_VALID,
    RWS_SPP_PLAYBACK_PB_HANDSHAKE,
} T_RWS_SPP_PLAYBACK_PB_CMD;

/** @brief  PLAYBACK mode */
typedef enum
{
    PLAYBACK_TRANS_SPP_MODE,
    PLAYBACK_TRANS_BLE_MODE,
} T_PB_TRANS_MODE;
typedef union _SPP_IDX_BLE_CONN_ID
{
    uint8_t spp_idx;
    uint8_t ble_conn_id;
} SPP_IDX_BLE_CONN_ID;

/** @brief  Table used to store Extended Device Information */
typedef struct _SPP_PB_FUNCTION_STRUCT
{

    uint32_t file_total_length;
    uint32_t cur_offset;
    uint32_t send_cnt;
    uint8_t file_list_type;
    uint16_t file_id;
    uint8_t bd_addr[6];
    uint8_t trans_mode;
    SPP_IDX_BLE_CONN_ID id;

    uint8_t *p_temp_buf_A_head;
    uint16_t temp_buf_A_used_size;
    uint16_t local_seq;
    bool buf_A_is_full;
    uint8_t *p_temp_buf_B_head;
    uint16_t temp_buf_B_used_size;
    bool buf_B_is_full;
    bool buf_pingpang_flag; //0 :A 1:B

    bool is_trans_pb_process;
} PB_TRANS_FUNCTION_STRUCT;

typedef enum
{
    PB_REMOTE_MSG_START_SYNC                        = 0x07,
    PB_REMOTE_MSG_PLAYLIST_ADD_SONG_SYNC            = 0x00,
    PB_REMOTE_MSG_PLAYLIST_DELETE_SONG_SYNC         = 0x01,
    PB_REMOTE_MSG_DELETE_FILE_SYNC                  = 0x02,
    PB_REMOTE_MSG_CANCEL_SYNC                       = 0x03,
    PB_REMOTE_MSG_EXIT_SYNC                         = 0x04,
    PB_REMOTE_MSG_SEC_RESULT_SYNC                   = 0x05,
    PB_REMOTE_MSG_RESUME_SYNC                       = 0x06,
    PB_REMOTE_MSG_PLAYLIST_UPDATA_SYNC              = 0x08,
    PB_REMOTE_MSG_DELETE_ALL_FILE_SYNC              = 0x09,
    PB_REMOTE_MSG_DELETE_OPENED_FILE_SYNC           = 0x0A,

    PB_REMOTE_MSG_SEC_INIT_RESULT_SYNC              = 0x20,

    PB_REMOTE_MSG_MAX
} T_SPP_PLAYBACK_REMOTE_MSG;

//-------------------------------ble------------------------------//


/** End of APP_PLAYBACK_SERVICE_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_PLAYBACK_SERVICE_Exported_Functions APP PLAYBACK service Functions
    * @brief
    * @{
    */
/**
    * @brief  The main function to handle all the spp command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  app_idx received rx command device index
    * @return void
    */
void app_playback_trans_cmd_handle(uint16_t length, uint8_t *p_value, uint8_t app_idx);
/**
    * @brief  handle the get list info event ack
    * @param  event_id  the ack is which event's
    * @param  status  the status of the ack
    * @return void
    */
void app_playback_trans_list_data_handle(void);

/**
    * @brief  app spp update file init
    * @param  void
    * @return void
    */
void app_playback_trans_file_init(void);

/**
    * @brief  get playback transfer status
    * @return True:is doing ota; False: is not doing ota
    */
bool app_playback_trans_is_busy(void);

/**
    * @brief  The main function to handle all the ble command
    * @param  length length of command id and data
    * @param  p_value data addr
    * @param  conn_id received rx command conn_id
    * @return void
    */
T_APP_RESULT app_playback_trans_ble_handle_cp_req(uint8_t conn_id, uint16_t length,
                                                  uint8_t *p_value);

void app_playback_trans_write_file_ack_handle_msg(T_IO_MSG *io_driver_msg_recv);

/** @} */ /* End of group APP_PLAYBACK_SERVICE_Exported_Functions */

/** @} */ /* End of group APP_PLAYBACK_SERVICE */
#endif
