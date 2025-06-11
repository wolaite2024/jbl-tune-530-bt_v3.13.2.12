#ifndef LCD_SH8601Z_454_QSPI_H
#define LCD_SH8601Z_454_QSPI_H

#include <stdint.h>


#define LCD_RST P4_4


void sh8601a_init(void);
void SH8601A_qspi_power_off(void);
void SH8601A_qspi_power_on(void);
void lcd_SH8601A_qspi_454_set_window(uint16_t xStart, uint16_t yStart,
                                     uint16_t xEnd, uint16_t yEnd);


#endif // LCD_SH8601Z_454_QSPI_H
