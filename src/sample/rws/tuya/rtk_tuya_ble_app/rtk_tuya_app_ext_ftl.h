#ifndef  RTK_TUYA_APP_EXT_FTLH_
#define  RTK_TUYA_APP_EXT_FTLH_

#ifdef __cplusplus
extern "C" {
#endif

#define APP_TUYA_FTL_START_ADDR             (0)
#define APP_TUYA_FTL_SIZE                   (0x1000)

bool app_tuya_ext_ftl_init(void);
uint32_t app_tuya_ext_save_ftl_data(uint8_t *data, uint32_t offset, uint32_t len);
uint32_t app_tuya_ext_load_ftl_data(uint8_t *data, uint32_t offset, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
