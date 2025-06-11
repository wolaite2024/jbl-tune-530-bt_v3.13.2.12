#ifndef _LCD_ST7789_240_QSPI_H_
#define _LCD_ST7789_240_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

#define LCD_RST P4_0

void lcd_st7789_qspi_240_power_on(void);
void lcd_st7789_qspi_240_power_off(void);
void lcd_st7789_qspi_240_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd,
                                    uint16_t yEnd);
void lcd_st7789_qspi_240_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_RM69330_454_QSPI_H_ */
