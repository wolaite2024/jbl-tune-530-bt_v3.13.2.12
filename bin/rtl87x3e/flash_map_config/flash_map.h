/** Copyright(c) 2018, Realtek Semiconductor Corporation.All rights reserved.*/

#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

#define IMG_HDR_SIZE                    0x00000400  //Changable, default 4K, modify in eFuse if needed

#define CFG_FILE_PAYLOAD_LEN            0x00001000  //Fixed


/* ========== High Level Flash Layout Configuration ========== */
#define OEM_CFG_ADDR                    0x02002000
#define OEM_CFG_SIZE                    0x00001400  //5K Bytes
#define BOOT_PATCH_ADDR                 0x02004000
#define BOOT_PATCH_SIZE                 0x00002000  //8K Bytes
#define PLATFORM_EXT_ADDR               0x02006000
#define PLATFORM_EXT_SIZE               0x00008000  //32K Bytes
#define LOWERSTACK_EXT_ADDR             0x0200E000
#define LOWERSTACK_EXT_SIZE             0x00007000  //28K Bytes
#define UPPERSTACK_ADDR                 0x02015000
#define UPPERSTACK_SIZE                 0x00036000  //216K Bytes
#define OTA_BANK0_ADDR                  0x0204B000
#define OTA_BANK0_SIZE                  0x000E3000  //908K Bytes
#define OTA_BANK1_ADDR                  0x0212E000
#define OTA_BANK1_SIZE                  0x000A2000  //648K Bytes
#define VP_DATA_ADDR                    0x021D0000
#define VP_DATA_SIZE                    0x00028000  //160K Bytes
#define FTL_ADDR                        0x021F8000
#define FTL_SIZE                        0x00006000  //24K Bytes
#define BKP_DATA1_ADDR                  0x021FE000
#define BKP_DATA1_SIZE                  0x00002000  //8K Bytes
#define BKP_DATA2_ADDR                  0x02200000
#define BKP_DATA2_SIZE                  0x00000000  //20K Bytes

/* ========== OTA Bank0 Flash Layout Configuration ========== */
#define BANK0_OTA_HDR_ADDR              0x0204B000
#define BANK0_OTA_HDR_SIZE              0x00000400  //1K Bytes
#define BANK0_FSBL_ADDR                 0x0204B400
#define BANK0_FSBL_SIZE                 0x00001C00  //7K Bytes
#define BANK0_STACK_PATCH_ADDR          0x0204D000
#define BANK0_STACK_PATCH_SIZE          0x00032000  //200K Bytes
#define BANK0_SYS_PATCH_ADDR            0x0207F000
#define BANK0_SYS_PATCH_SIZE            0x00011000  //68K Bytes
#define BANK0_APP_ADDR                  0x02090000
#define BANK0_APP_SIZE                  0x00075000  //468K Bytes
#define BANK0_DSP_SYS_ADDR              0x02105000
#define BANK0_DSP_SYS_SIZE              0x0000E000  //56K Bytes
#define BANK0_DSP_APP_ADDR              0x02113000
#define BANK0_DSP_APP_SIZE              0x00012000  //72K Bytes
#define BANK0_DSP_CFG_ADDR              0x02125000
#define BANK0_DSP_CFG_SIZE              0x00002000  //8K Bytes
#define BANK0_APP_CFG_ADDR              0x02127000
#define BANK0_APP_CFG_SIZE              0x00002000  //8K Bytes
#define BANK0_EXT_IMG0_ADDR             0x00000000
#define BANK0_EXT_IMG0_SIZE             0x00000000  //0K Bytes
#define BANK0_EXT_IMG1_ADDR             0x00000000
#define BANK0_EXT_IMG1_SIZE             0x00000000  //0K Bytes
#define BANK0_EXT_IMG2_ADDR             0x02129000
#define BANK0_EXT_IMG2_SIZE             0x00005000  //20K Bytes
#define BANK0_EXT_IMG3_ADDR             0x00000000
#define BANK0_EXT_IMG3_SIZE             0x00000000  //0K Bytes

/* ========== OTA Bank1 Flash Layout Configuration ========== */
#define BANK1_OTA_HDR_ADDR              0x02110000
#define BANK1_OTA_HDR_SIZE              0x00000400  //1K Bytes
#define BANK1_FSBL_ADDR                 0x02110400
#define BANK1_FSBL_SIZE                 0x00001C00  //7K Bytes
#define BANK1_STACK_PATCH_ADDR          0x02112000
#define BANK1_STACK_PATCH_SIZE          0x00013000  //76K Bytes
#define BANK1_SYS_PATCH_ADDR            0x02125000
#define BANK1_SYS_PATCH_SIZE            0x0000D000  //52K Bytes
#define BANK1_APP_ADDR                  0x02132000
#define BANK1_APP_SIZE                  0x00072000  //456K Bytes
#define BANK1_DSP_SYS_ADDR              0x021A4000
#define BANK1_DSP_SYS_SIZE              0x0000D000  //52K Bytes
#define BANK1_DSP_APP_ADDR              0x021B1000
#define BANK1_DSP_APP_SIZE              0x00024000  //144K Bytes
#define BANK1_DSP_CFG_ADDR              0x021D5000
#define BANK1_DSP_CFG_SIZE              0x00003000  //12K Bytes
#define BANK1_APP_CFG_ADDR              0x021D8000
#define BANK1_APP_CFG_SIZE              0x00002000  //8K Bytes
#define BANK1_EXT_IMG0_ADDR             0x00000000
#define BANK1_EXT_IMG0_SIZE             0x00000000  //0K Bytes
#define BANK1_EXT_IMG1_ADDR             0x00000000
#define BANK1_EXT_IMG1_SIZE             0x00000000  //0K Bytes
#define BANK1_EXT_IMG2_ADDR             0x00000000
#define BANK1_EXT_IMG2_SIZE             0x00000000  //0K Bytes
#define BANK1_EXT_IMG3_ADDR             0x00000000
#define BANK1_EXT_IMG3_SIZE             0x00000000  //0K Bytes


#endif /* _FLASH_MAP_H_ */
