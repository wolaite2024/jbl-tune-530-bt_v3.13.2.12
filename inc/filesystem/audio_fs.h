/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author    carol_shen
   * @date      2020-4-20
   * @version   1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/
#ifndef _AUDIO_FS_H_
#define _AUDIO_FS_H_


/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include "audio_fs_decode.h"

/*============================================================================*
  *                                  Variables
  *============================================================================*/
typedef enum
{
    AUDIO_FS_OK                     = 0,
    AUDIO_FS_NO_INIT                = 0x0001,
    AUDIO_FS_ERR_FILE_OPENED        = 0x0007,
    AUDIO_FS_ERR_ERROR              = 0xFFFF,

    AUDIO_FS_ERR_MOUNT              = 0x1000,
    AUDIO_FS_ERR_OPEN               = 0x1001,
    AUDIO_FS_ERR_READ               = 0x1002,
    AUDIO_FS_ERR_WRITE              = 0x1003,
    AUDIO_FS_ERR_CLOSE              = 0x1004,
    AUDIO_FS_ERR_FILE_NOT_EXIST     = 0x1005,
    AUDIO_FS_ERR_DELETE             = 0x1006,
    AUDIO_FS_ERR_ADD                = 0x1007,
    AUDIO_FS_ERR_COMPARE            = 0x1008,
    AUDIO_FS_ERR_SET_PLAYLIST       = 0x1009,
    AUDIO_FS_ERR_CREATE             = 0x100A,
    AUDIO_FS_ERR_CREATE_DIR         = 0x100B,
    AUDIO_FS_ERR_CHDIR              = 0x100C,
    AUDIO_FS_ERR_OPEN_DIR           = 0x100D,
    AUDIO_FS_ERR_READ_DIR           = 0x100E,

    AUDIO_FS_ERR_ID3V2_HEADER_SYNC  = 0x3000,
    AUDIO_FS_ERR_ID3V1_HEADER_SYNC  = 0x3001,
    AUDIO_FS_ERR_MP3_HEADER_SYNC    = 0x3002,
    AUDIO_FS_ERR_AAC_HEADER_SYNC    = 0x3003,
    AUDIO_FS_ERR_BIG_FRAME_LEN      = 0x3004,
    AUDIO_FS_ERR_ZERO_LEN           = 0x3005,
    AUDIO_FS_ERR_MP3_BITRATE_ZERO   = 0x3006,

    AUDIO_FS_ERR_MALLOC             = 0x4000,
    AUDIO_FS_ERR_PARAM              = 0x4001,
    AUDIO_FS_ERR_SCAN               = 0x4002,
    AUDIO_FS_ERR_CHECK_FS           = 0x4003,
    AUDIO_FS_NO_SPACE               = 0x4005,
} T_AUDIO_FS_ERR;

#define FS_END_OF_FILE              0x2222
#define PATH_LEN                    FF_MAX_LFN /* byte */

typedef enum
{
    UTF_8            = 106,
    UTF_16           = 1015,
    UTF_32           = 1017,
} T_AUDIO_FS_CHARATER_FORMAT;

typedef struct
{
    uint32_t    offset;    //Start offset of the song name
    uint16_t    length;       //Length of the song name
    uint16_t    plIndex;    /*Play List Index, indicate which playlist the song belongs to.
                             0x0: belong to no playlist, 0x1: belong to playlist1.*/
    uint8_t     isDeleted : 1;  /* flag of if song is deleted.
                                    1: deleted,
                                    0: not deleted, */
    uint8_t     needToUnlink : 1;  /* flag of if song need to unlink.
                                      1: need to call audio_fs_unlink to delete the file
                                      2: not need to unlink*/
    uint8_t     extension : 6;
    uint16_t     rsv;            /* Reserve for future usage, should set to "0" */
} __attribute__((packed)) T_SONG_NAME_INFO;

typedef struct
{
    uint16_t num;           // Total song number of the lists
    uint32_t rsv;           // Reserve for future usage, should set to "0"
    // the first T_SONG_NAME_INFO content, Should include (num) _SONG_NAME_INFO_
    T_SONG_NAME_INFO info[0];
} __attribute__((packed))T_SONG_LIST;

typedef struct
{
    uint32_t    offset;    //Start offset of the song name
    uint16_t    length;       //Length of the song name
} __attribute__((packed))T_PLAY_LIST_INFO;

typedef struct
{
    uint16_t     plIndex;
    uint16_t     count;
    T_PLAY_LIST_INFO info[0];   /* the first T_SONG_NAME_INFO content,
                                Should include (num) _SONG_NAME_INFO_*/
} __attribute__((packed))T_PLAY_LIST;


#define AUDIO_FS_HEADER_COUNT           0
#define AUDIO_FS_HEADER_COUNT_SIZE      sizeof(uint16_t)
#define AUDIO_FS_HEADER_RSV_SIZE        sizeof(uint32_t)
#define AUDIO_FS_HEADER_INFO_START      (AUDIO_FS_HEADER_COUNT_SIZE + AUDIO_FS_HEADER_RSV_SIZE)

#define AUDIO_FS_PLAYLIST_PLINDEX       0
#define AUDIO_FS_PLAYLIST_PLINDEX_SIZE  sizeof(uint16_t)
#define AUDIO_FS_PLAYLIST_COUNT         (AUDIO_FS_PLAYLIST_PLINDEX_SIZE)
#define AUDIO_FS_PLAYLIST_COUNT_SIZE    sizeof(uint16_t)
#define AUDIO_FS_PLAYLIST_INFO_START    (AUDIO_FS_PLAYLIST_PLINDEX_SIZE + AUDIO_FS_PLAYLIST_COUNT_SIZE)

typedef struct t_audio_file
{
    struct t_audio_file *p_next; /**< Pointer to next list queue element. */
    FIL               fil;
    TCHAR             filename[PATH_LEN];
    uint32_t          fileOffset;
    uint16_t          namelen;
    uint16_t          playlistIndex;
    uint16_t          fileIndex;
    uint8_t           flag; /* the handle status: 0: unused handle;
                             1: read file handle; 2: write file handle*/
} __attribute__((packed))T_AUDIO_FILE;

typedef void *T_AUDIO_FS_HANDLE;

typedef struct
{
    TCHAR    *relative_path;
    FATFS    *fs;
    FIL      *header_fil;
    FIL      *name_fil;
    union
    {
        FIL      *playlist_fil;
        uint16_t *playlistInfo;
    } playlist;
    uint16_t  playlistSize;
    bool      initiativeUpdate;
} __attribute__((packed))T_AUDIO_FS_DB;

/*============================================================================*
  *                                Functions
  *============================================================================*/
/* FOR LOCAL PLAYBACK */
uint16_t audio_fs_mkfs(void);
void audio_fs_sd_power_off(void);
void audio_fs_sd_power_on(void);
uint16_t audio_fs_sd_status_check_and_init(void);

uint16_t audio_fs_get_fileinfo(uint16_t fileIdx, uint8_t *pFileName, uint16_t *pNameLen);
bool audio_fs_end_of_file(T_AUDIO_FS_HANDLE handle);

T_AUDIO_FS_HANDLE audio_fs_open(uint8_t *pFilename, uint16_t namelen, uint8_t open_mode);
uint16_t audio_fs_read(T_AUDIO_FS_HANDLE handle, uint8_t *readBuf, uint32_t readLen, uint32_t *len);
uint16_t audio_fs_write(T_AUDIO_FS_HANDLE handle, uint8_t *writeBuf,
                        uint32_t writeLen, uint32_t *len);
uint32_t audio_fs_size(T_AUDIO_FS_HANDLE handle);
uint16_t audio_fs_close(T_AUDIO_FS_HANDLE handle);

uint32_t audio_fs_get_file_offset(T_AUDIO_FS_HANDLE handle);
void audio_fs_set_file_offset(T_AUDIO_FS_HANDLE handle, uint32_t offset);

uint8_t *audio_fs_get_filename(T_AUDIO_FS_HANDLE handle);
uint16_t audio_fs_get_filenameLen(T_AUDIO_FS_HANDLE handle);
uint16_t audio_fs_get_fileIndex(T_AUDIO_FS_HANDLE handle);

uint16_t audio_fs_set_playlist(uint16_t playlistIndex);
uint16_t audio_fs_get_filecount_from_playlist(void);

uint16_t audio_fs_get_frame(T_AUDIO_FS_HANDLE handle, FRAME_CONTENT *p_framContent);
uint16_t audio_fs_get_frame_para(T_AUDIO_FS_HANDLE handle, FRAME_INFO *p_frameInfo);

uint16_t audio_fs_init(TCHAR *path, uint32_t playlistSize, bool initiativeUpdate,
                       bool *needUpdatePlaylist);
void audio_fs_deinit(void);
uint16_t audio_fs_update(bool *needUpdatePlaylist);

uint16_t audio_fs_rename(T_AUDIO_FS_HANDLE handle, uint8_t *pFileName, uint16_t namelen);
uint16_t utf8_to_unicode(uint8_t *pInput, uint16_t inputBytes, uint8_t *pUnicode);
uint16_t get_unicode_bytes(uint8_t *pInput, uint16_t inputBytes);

uint16_t audio_fs_read_header_list(uint32_t offset, uint8_t *readBuf,
                                   uint32_t readLen, uint32_t *len);
uint16_t audio_fs_read_name_list(uint32_t offset, uint8_t *readBuf,
                                 uint32_t readLen, uint32_t *len);
uint32_t audio_fs_get_header_list_size(void);
uint32_t audio_fs_get_name_list_size(void);
uint16_t audio_fs_update_playlist_index(uint8_t *pFileName, uint16_t namelen,
                                        uint16_t playlistIndex,
                                        uint16_t fileIndex);
uint16_t audio_fs_delete_file(uint8_t *pFileName, uint16_t namelen, uint16_t fileIndex);
uint16_t audio_fs_delete_all_files(void);
void audio_fs_unlink_file(void);
uint16_t audio_fs_unlink_all_files(void);
uint16_t audio_fs_add_file(T_AUDIO_FS_HANDLE handle, uint16_t playlistIndex);
uint16_t audio_fs_spp_delete_file(uint8_t *pFileName, uint16_t namelen);

uint16_t audio_fs_get_id3v2_InfoLen(uint16_t tagIdx);
uint8_t *audio_fs_get_id3v2_Info(uint16_t tagIdx);
uint16_t audio_fs_id3v2Info(T_AUDIO_FS_HANDLE handle, uint16_t tagIdx);

uint16_t audio_fs_free_space(uint32_t *pfreeSpace);
uint16_t audio_fs_get_space_info(uint32_t *ptotalSpace, uint32_t *pfreeSpace);
void audio_fs_close_all_list(void);

#endif //_AUDIO_FS_H_
