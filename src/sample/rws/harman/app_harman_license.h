#ifndef _APP_HARMAN_SPP_H_
#define _APP_HARMAN_SPP_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _t_app_harman_product_id
{
    PRODUCT_ID_T135             = 0x2135,
    PRODUCT_ID_T235             = 0x2136,

    PRODUCT_ID_RUN3_WIRELESS    = 0x214F,

    PRODUCT_ID_ENDURANCE_PACE   = 0x2150,

    PRODUCT_ID_T530             = 0x2162,
} T_APP_HARMAN_PRODUCT_ID;

typedef enum _t_app_harman_color_id
{
    COLOR_ID_BLACK             = 0x00,
    COLOR_ID_WHITE             = 0x01,
    COLOR_ID_BLUE              = 0x02,

    COLOR_ID_PURPLE            = 0x05,

    COLOR_ID_BLACK_GRAY        = 0x1d,
    COLOR_ID_BLACK_LIME        = 0x1e,
} T_APP_HARMAN_COLOR_ID;

#define HARMAN_PRODUCT_TOTAL_NUM        5
#define HARMAN_COLOR_TOTAL_NUM          6

#define HARMAN_MODEL_ID_LEN             3
#define HARMAN_PUBLIC_KEY_LEN           64
#define HARMAN_PRIVATE_KEY_LEN          32

typedef struct _t_app_harman_gfps_id
{
    uint8_t color_id;
    uint8_t model_id[HARMAN_MODEL_ID_LEN];
    uint8_t public_key[HARMAN_PUBLIC_KEY_LEN];
    uint8_t private_key[HARMAN_PRIVATE_KEY_LEN];
} T_APP_HARMAN_GFPS_INFO;

typedef struct _t_app_harman_product_info
{
    uint8_t  device_name[40];
    uint16_t id;
} T_APP_HARMAN_PRODUCT_INFO;

typedef struct _t_app_harman_device_id
{
    T_APP_HARMAN_PRODUCT_INFO product_info;
    T_APP_HARMAN_GFPS_INFO gfps_id[HARMAN_COLOR_TOTAL_NUM];
} T_APP_HARMAN_DEVICE_ID;

#define APP_HARMAN_LISENCE_OFFSET               0x021FE000
#define APP_HARMAN_PRODUCT_INFO_LEN             sizeof(T_APP_HARMAN_PRODUCT_INFO)
#define APP_HARMAN_GFPS_INFO_LEN                sizeof(T_APP_HARMAN_GFPS_INFO)
#define APP_HARMAN_LICENSE_ONE_PRODUCT_LEN      (APP_HARMAN_PRODUCT_INFO_LEN + HARMAN_COLOR_TOTAL_NUM * APP_HARMAN_GFPS_INFO_LEN)
#define APP_HARMAN_LICENSE_ALL_PRODUCTS_LEN     (HARMAN_PRODUCT_TOTAL_NUM * APP_HARMAN_LICENSE_ONE_PRODUCT_LEN)

#define HARMAN_LICENSE_SIZE_SAVE_TO_FTL     (HARMAN_SERIALS_NUM_LEN + HARMAN_PRODUCT_ID_LEN + HARMAN_COLOR_ID_LEN + HARMAN_RESVED_LEN)

typedef enum _t_app_harman_spp_sub_cmd_id
{
    HARMAN_SPP_SUB_CMD_NONE                     = 0x00,
    HARMAN_SPP_SUB_CMD_PID_SET                  = 0x01,
    HARMAN_SPP_SUB_CMD_CID_SET                  = 0x02,
    HARMAN_SPP_SUB_CMD_SERIALS_NUM_SET          = 0x03,

    HARMAN_SPP_SUB_CMD_PID_GET                  = 0x11,
    HARMAN_SPP_SUB_CMD_CID_GET                  = 0x12,
    HARMAN_SPP_SUB_CMD_SERIALS_NUM_GET          = 0x13,

    HARMAN_SPP_SUB_CMD_TOTAL_TIME_GET           = 0x21,
    HARMAN_SPP_SUB_CMD_TOTAL_TIME_CLEAR         = 0x22,
    HARMAN_SPP_SUB_CMD_CHIP_ID_GET              = 0x23,
    HARMAN_SPP_SUB_CMD_ALGO_STATUS_GET          = 0x24,
} T_APP_HARMAN_SPP_SUB_CMD_ID;

void app_harman_license_get(void);
void app_harman_license_init(void);
void app_harman_license_product_info_dump(void);
uint16_t app_harman_license_pid_get(void);
uint8_t app_harman_license_cid_get(void);
uint8_t *app_harman_license_serials_num_get(void);
bool app_harman_get_flag_need_clear_total_time(void);
void app_harman_spp_cmd_set_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                   uint8_t app_idx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

