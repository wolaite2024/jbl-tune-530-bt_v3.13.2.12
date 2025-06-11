/**
  ******************************************************************************
  * @file    patch_header_check.h
  * @author
  * @version V0.0.1
  * @date    2017-09-01
  * @brief   This file contains all the functions regarding patch header check.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _PATCH_HEADER_CHECK_H_
#define _PATCH_HEADER_CHECK_H_
#include <stdbool.h>    //for bool
#include <stdint.h>   //for uint* type

/** @addtogroup  87x3e_FLASH_DEVICE    Flash Device
    * @{
    */
/*============================================================================*
  *                                   Constants
  *============================================================================*/
/** @defgroup 87x3e_FLASH_DEVICE_Exported_Constants Flash Device Exported Constants
  * @brief
  * @{
  */

/** @defgroup 87x3e_FLASH_IMAGE_SIGNATURE Flash Image Signature
  * @brief patch signature definition for each image
  * @{
  */


/** End of FLASH_IMAGE_SIGNATURE
  * @}
  */
#define DEFAULT_HEADER_SIZE     0x400
#define EFUSE_HEADER_SIZE_UNIT  0x400

#define UUID_SIZE               16
#define SYS_CFG_SIGNATURE       0x12345bb3
#define IC_TYPE                 0xB

#define BOOT_PATCH_IGNORE_COMMON_IMG   1
#define CLR_BITMAP(val, mask)          ((val)  &= ((uint32_t)0xFFFFFFFF) << (mask))

#define UPPERSTACK_PARTITION_NAME "UPPER_STACK"
#define VP_PARTITION_NAME "VP"

#define IMG_HDR_RSVD_SIZE        (348)
#define GOLDEN_PATTERN_WORD_LEN  (20)
#define GOLDEN_PATTERN_SIZE      (sizeof(uint32_t) * GOLDEN_PATTERN_WORD_LEN)
#define IMG_FEATURE_STR_LEN      (16)
#define IMG_INFO_SIZE            (sizeof(uint32_t) * (IMAGE_MAX - IMG_SBL) * 2)
#define VER_VAL_SIZE             (sizeof(uint32_t))
#define IMG_RELEASE_VER_SIZE     (sizeof(uint64_t))
#define IMG_RSVD0_SIZE           (IMG_HDR_RSVD_SIZE - VER_VAL_SIZE - IMG_INFO_SIZE - GOLDEN_PATTERN_SIZE - \
                                  IMG_FEATURE_STR_LEN - IMG_RELEASE_VER_SIZE)

#define TOOL_RELEASE_VER_SIZE (6)
#define IMG_RSVD1_SIZE     (IMG_HDR_RSVD_SIZE - TOOL_RELEASE_VER_SIZE - IMG_FEATURE_STR_LEN - IMG_RELEASE_VER_SIZE)

/** End of FLASH_DEVICE_Exported_Constants
  * @}
  */
/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup 87x3e_FLASH_DEVICE_Exported_Types Flash Device Exported Types
  * @brief
  * @{
  */
typedef enum _IMG_ID
{
    IMG_OCCD        = 0x278E, /* contained ota header address, not part of OTA bank */
    IMG_BOOTPATCH   = 0x278F,
    IMG_OTA         = 0x2790, /* OTA header */
    IMG_SBL         = 0x2791,
    IMG_MCUPATCH    = 0x2792,
    IMG_MCUAPP      = 0x2793,
    IMG_DSPSYSTEM   = 0x2794,
    IMG_DSPAPP      = 0x2795,
    IMG_MCUCONFIG   = 0x2796,
    IMG_DSPCONFIG   = 0x2797,
    IMG_ANC         = 0x2798,
    IMG_EXT1        = 0x2799,
    IMG_EXT2        = 0x279A,
    IMG_ADSP        = 0x279B,
    IMG_SYSPATCH    = 0x279C,
    IMG_STACKPATCH  = 0x279D,
    IMG_UPPERSTACK  = 0x279E,
    IMG_FRAMEWORK   = 0x279F,
    IMG_SYSDATA     = 0x27A0,
    IMAGE_MAX
} IMG_ID;

typedef enum _PRE_IMG_ID
{
    PRE_IMG_PLATFORMEXT      = IMAGE_MAX,
    PRE_IMG_LOWSTACKEXT      = 0x27A2,
    PRE_IMG_UPPERSTACK       = 0x27A3,
    PRE_IMG_VP               = 0x27A4,
    PRE_IMAGE_MAX
} PRE_IMG_ID;

typedef enum _USER_IMG_ID
{
    IMG_USER_DATA8      = 0xFFF7,
    IMG_USER_DATA_FIRST = IMG_USER_DATA8,
    IMG_USER_DATA7      = 0xFFF8,
    IMG_USER_DATA6      = 0xFFF9,
    IMG_USER_DATA5      = 0xFFFA,
    IMG_USER_DATA4      = 0xFFFB,
    IMG_USER_DATA3      = 0xFFFC,
    IMG_USER_DATA2      = 0xFFFD,
    IMG_USER_DATA1      = 0xFFFE,
    IMG_USER_DATA_MAX   = 0xFFFF,
} USER_IMG_ID;

typedef enum
{
    IMG_CHECK_PASS,
    IMG_CHECK_ERR_HEADER_ADDR,
    IMG_CHECK_ERR_NOT_READY,
    IMG_CHECK_ERR_ROM_UUID,
    IMG_CHECK_ERR_MAGIC_PATTERN,
    IMG_CHECK_ERR_SIZE,
    IMG_CHECK_ERR_ID,
    IMG_CHECK_ERR_ENTRY_RETURN,
    IMG_CHECK_ERR_VERIFY,
    IMG_CHECK_ERR_MAX,
} IMG_CHECK_ERR_TYPE;

typedef enum _ENC_KEY_SELECT
{
    ENC_KEY_SCEK = 0,
    ENC_KEY_SCEK_WITH_RTKCONST,
    ENC_KEY_OCEK,
    ENC_KEY_OCEK_WITH_OEMCONST,
    ENC_KEY_ON_FLASH,
    ENC_KEY_MAX,
} ENC_KEY_SELECT;

typedef struct _RSA_PUBLIC_KEY
{
    uint8_t N[256];
    uint8_t E[4];
} RSA_PUBLIC_KEY;

typedef struct _IMG_CTRL_HEADER_FORMAT
{
    uint16_t crc16;
    uint8_t ic_type;
    uint8_t secure_version;
    union
    {
        uint16_t value;
        struct
        {
            uint16_t xip: 1; // payload is executed on flash
            uint16_t enc: 1; // all the payload is encrypted
            uint16_t load_when_boot: 1; // load image when boot
            uint16_t enc_load: 1; // encrypt load part or not
            uint16_t enc_key_select: 3; // referenced to ENC_KEY_SELECT
            uint16_t not_ready: 1; //for copy image in ota
            uint16_t not_obsolete: 1; //for copy image in ota
            uint16_t integrity_check_en_in_boot: 1; // enable image integrity check in boot flow
            uint16_t header_size: 4; // unit:KB, set for tool
            uint16_t rsvd: 2;
        };
    } ctrl_flag;
    uint16_t image_id;
    uint32_t payload_len;
} T_IMG_CTRL_HEADER_FORMAT;

typedef struct
{
    union
    {
        uint32_t version;
        struct
        {
            uint32_t _version_major: 4;     //!< major version
            uint32_t _version_minor: 8;     //!< minor version
            uint32_t _version_revision: 9; //!< revision version
            uint32_t _version_reserve: 11;   //!< reserved
        } sub_version;
    };
    uint32_t _version_commitid;     //!< git commit id
    uint8_t _customer_name[8];      //!< branch name for customer patch
} T_VERSION_FORMAT;

typedef union
{
    uint8_t bytes[24];
    struct
    {
        uint32_t itcm_heap_addr;
        uint32_t app_ram_data_size;
        uint32_t app_heap_data_on_size;
    } app_ram_info;
} T_EXTRA_INFO_FORMAT;


typedef struct _AUTH_HEADER_FORMAT
{
    uint8_t image_mac[16];
    uint8_t image_signature[256];
    uint8_t image_hash[32];
} T_AUTH_HEADER_FORMAT;

/* image_info[] is used to record image address and size in OTA bank
 * ex. fsbl's addr = image_info[(IMG_SBL - IMG_SBL)*2 + 0] *     fsbl's size = image_info[(IMG_SBL - IMG_SBL)*2 + 1]
 *     mcu patch's addr = image_info[(IMG_MCUPATCH - IMG_SBL)*2 + 0]
 *     mcu patch's size = image_info[(IMG_MCUPATCH - IMG_SBL)*2 + 1]
 *     ...
 *     sys data's addr = image_info[(IMG_SYSDATA - IMG_SBL)*2 + 0]
 *     sys data's size = image_info[(IMG_SYSDATA - IMG_SBL)*2 + 1]
 *  => image_id's addr = image_info[(image_id - IMG_SBL)*2 + 0]
 *     image_id's size = image_info[(image_id - IMG_SBL)*2 + 1]
 */
typedef union _IMG_HEADER_FORMAT
{
    uint8_t bytes[DEFAULT_HEADER_SIZE];
    struct
    {
        T_AUTH_HEADER_FORMAT auth;
        T_IMG_CTRL_HEADER_FORMAT ctrl_header;
        uint8_t uuid[16];
        uint32_t exe_base;
        uint32_t load_src;
        uint32_t load_len;
        uint32_t image_base;
        uint16_t dev_id; // for AMZN
        uint16_t flash_layout_size_4k; // ex. 1MB flash used size => flash_layout_size_4k = 1MB/4KB = 256
        uint32_t magic_pattern;
        uint8_t dec_key[16];
        uint32_t load_dst;
        T_EXTRA_INFO_FORMAT ex_info;
        T_VERSION_FORMAT git_ver;
        RSA_PUBLIC_KEY rsaPubKey;
        union
        {
            uint8_t reserved_data[IMG_HDR_RSVD_SIZE];
            struct __attribute__((packed))
            {
                uint32_t ver_val;
                uint32_t image_info[(IMAGE_MAX - IMG_SBL) * 2];
                uint32_t resv_golden_pattern[GOLDEN_PATTERN_WORD_LEN]; //resrved, not used
                uint8_t  reserved0[IMG_RSVD0_SIZE];
                uint64_t image_release_version;                   //offset: 1000, used for image release version
                uint8_t  image_feature[IMG_FEATURE_STR_LEN];     // offset: 1008,used for image feature string
            };
            struct __attribute__((packed))
            {
                uint16_t tool_version;
                uint32_t timestamp;
                uint8_t  reserved1[IMG_RSVD1_SIZE];
                uint64_t data_image_release_version;             //offset: 1000, used for image release version
                uint8_t  data_image_feature[IMG_FEATURE_STR_LEN];//offset: 1008, used for image feature string
            };
        };
    };
} T_IMG_HEADER_FORMAT;

typedef struct _ROM_HEADER_FORMAT
{
    uint8_t uuid[16];
    void *init_ptr;
    void *entry_ptr;
} T_ROM_HEADER_FORMAT;

typedef struct ConfigHeaderSection
{
    uint32_t Signature_Field;
    uint16_t Data_Length;
    uint16_t Module_ID;
} __attribute__((packed)) ConfigHeaderSection_T;

typedef struct ConfigEntry
{
    uint16_t Offset;
    uint8_t  Length;
    uint8_t  *pData;
    uint8_t  *pMask;
} __attribute__((packed)) ConfigEntry_T;

typedef struct
{
    uint32_t *base;
    uint32_t size;
} MODULE_INFO;

typedef struct
{
    uint32_t Signature;
    uint32_t max_size;
    void *sys_cfg;
} PARSING_PARAMETER_FORMAT;

enum
{
    // Boot ROM
    MODULE_ID_BOOT_CFG = 0,
    MODULE_ID_CLOCK_TREE,
    MODULE_ID_BOOT_ROM_MAX,
    // Platform
    MODULE_ID_OS = MODULE_ID_BOOT_ROM_MAX,
    MODULE_ID_SYS_INIT,
    MODULE_ID_PHY_INIT,
    MODULE_ID_ADAPTER,
    MODULE_ID_CHARGER,
    MODULE_ID_USB,
#if (_IS_BB_SERIES_ == 1)
    MODULE_ID_PSRAM,
#endif
    MODULE_ID_PMU,
    MODULE_ID_CAP_TOUCH,
    MODULE_ID_SYS_ROM_MAX,
    // lowerstack
    MODULE_ID_LOWERSTACK = MODULE_ID_SYS_ROM_MAX,
    MODULE_ID_STACK_ROM_MAX,
    // upperstack
    MODULE_ID_UPPERSTACK = MODULE_ID_STACK_ROM_MAX,

    MODULE_ID_MAX,
};

enum
{
    PARSE_SUCCESS = 0,
    PARSE_ERROR_ID_MISMATCH,
    PARSE_ERROR_NO_CONFIG,               /*did not do the parsing operation*/
    PARSE_ERROR_ENTRY_SIZE_MISMATCH,     /*entry size do not match header legnth*/
    PARSE_ERROR_HEADER_LENGTH_MISMATCH,  /*header length > occd max size */
    PARSE_ERROR_MEM_OFFSET_EXCEED,       /*memory offset > memory size*/
};

__forceinline uint32_t *get_image_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[(image_id - IMG_SBL) * 2];
}

__forceinline uint32_t *get_image_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_info[(image_id - IMG_SBL) * 2 + 1];
}

__forceinline uint32_t *get_image_addr_in_bootpatch(uint32_t bootpatch_addr, PRE_IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->image_info[(image_id - PRE_IMG_PLATFORMEXT) * 2];
}

__forceinline uint32_t *get_image_size_in_bootpatch(uint32_t bootpatch_addr, PRE_IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->image_info[(image_id - PRE_IMG_PLATFORMEXT) * 2 + 1];
}

typedef union
{
    uint32_t d32[16];
    struct
    {
        uint32_t  magic_pattern;
        uint32_t  rsvd[11];

        // Patch version and customer_name
        uint32_t _version_major: 4;     //!< major version
        uint32_t _version_minor: 8;     //!< minor version
        uint32_t _version_revision: 15; //!< revision version
        uint32_t _version_reserve: 5;   //!< reserved
        uint32_t _version_commitid;     //!< git commit id
        uint8_t _customer_name[8];      //!< branch name for customer patch
    } patch_header_ext;
    struct
    {
        uint16_t  app_update_otp_signature;
        uint16_t  app_early_entry_signature;
        uint32_t  app_update_otp_address;
        uint32_t  app_early_entry_address;
        uint32_t  rsvd[9];

        // app version
        uint32_t _version_major: 4;     //!< major version
        uint32_t _version_minor: 8;     //!< minor version
        uint32_t _version_revision: 15; //!< revision version
        uint32_t _version_reserve: 5;   //!< reserved
        uint32_t _version_commitid;     //!< git commit id
        uint8_t _customer_name[8];      //!< reserved
    } app_header_ext;

} T_IMG_HEADER_FORMAT_EXT;

/** End of FLASH_DEVICE_Exported_Types
  * @}
  */
/*************************************************************************************************
*                          Constants
*************************************************************************************************/


/*************************************************************************************************
*                          functions
*************************************************************************************************/
/** @defgroup 87x3e_FLASH_DEVICE_Exported_Functions Flash Device Exported Functions
    * @brief
    * @{
    */
/**
 * @brief check specified signature image start address
 * @param header image header info of the given image.
 * @param signature signature to specify which image
 * @return true or false
*/
bool check_image_header(T_IMG_HEADER_FORMAT *header, uint16_t image_id);

IMG_CHECK_ERR_TYPE check_header_valid(uint32_t header_addr, IMG_ID image_id);
bool check_image_crc16(T_IMG_HEADER_FORMAT *p_header);
IMG_CHECK_ERR_TYPE image_entry(uint16_t image_id, uint32_t image_addr);
bool image_integrity_check(uint32_t img_addr, uint16_t image_id);
bool image_loading(uint32_t img_addr);

uint32_t get_header_addr_by_img_id(IMG_ID image_id);
uint32_t get_temp_ota_bank_addr_by_img_id(IMG_ID image_id);
uint32_t get_img_header_format_size(void);
T_EXTRA_INFO_FORMAT *get_extra_info(void);
extern bool (*is_ota_support_bank_switch)(void);

extern uint32_t ota_header_addr_rom;
static __forceinline uint32_t get_active_ota_bank_addr(void)
{
    return ota_header_addr_rom;
}
/** @} */ /* End of group 87x3e_FLASH_DEVICE_Exported_Functions */
/** @} */ /* End of group 87x3e_FLASH_DEVICE */

#endif // _PATCH_HEADER_CHECK_H_
