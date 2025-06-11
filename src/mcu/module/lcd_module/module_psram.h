#ifndef _RTL876X_MODULE_PSRAM_H_
#define _RTL876X_MODULE_PSRAM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

void app_apm_qspi_psram_init(void);
void app_apm_opi_psram_init(void);
void app_apm_psram_read_write_test(void);
void app_wb_opi_psram_init(void);
void app_wb_opi_psram_read_write_test(void);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_MODULE_LCD_TE_H_ */
