/***************************************************************************
 Copyright (C) Realtek
 ***************************************************************************/
#ifndef _INDIRECT_ACCESS_H_
#define _INDIRECT_ACCESS_H_

#include <stdint.h>

#define PON_F00_REG                        0xF00  // struct: PON_F00_REG_S
#define PON_F08_REG                        0xF08  // struct: PON_F00_REG_S

typedef union PON_F00_REG_S  /* Vendor 0xF00 */
{
    uint32_t d32;
    struct
    {
        uint32_t ind_32k_wdata: 16;     //[15:0]  R/W   (32k register write date. Read data)
        uint32_t ind_32k_addr: 6;       //[21:16] R/W   (32k register address)
        uint32_t ind_32k_rw: 1;         //[22]    W     (1: write 32k register)
        uint32_t rsvd: 9;               //[31:23] R/W
    };
} PON_F00_REG_S_TYPE;

uint16_t indirect_access_read_32k_reg(uint16_t offset);
void indirect_access_write_32k_reg(uint16_t offset, uint16_t wdata);

#define RD_16BIT_32K_SYSON_IO(offset)          indirect_access_read_32k_reg(offset)
#define WR_16BIT_32K_SYSON_IO(offset, wdata)   indirect_access_write_32k_reg(offset, wdata)

/* Define AON register structure in source file while it is not published */
#define REG_AON_FW_ONREG    0x48
typedef union REG_AON_FW_ONREG_S
{
    uint32_t d32;
    struct
    {
        uint32_t FW_ONREG_WSTROBE: 1; // [0]     RW FW set "0" to "1" to strobe write data
        uint32_t FW_ONREG_DATA  : 16; // [16:1]   RW
        //uint32_t FW_ONREG_RDATA  : 8; // [16:9]  RW
        uint32_t FW_ONREG_ADR    : 13; // [29:17] RW
        uint32_t RVD0             : 1; // [30] RW
        uint32_t ECC_TO          : 1; // [31] RW
    } b;
} REG_AON_FW_ONREG_S_TYPE;
uint16_t btaon_fast_read(uint16_t offset);
uint16_t btaon_fast_read_safe(uint16_t offset);
uint8_t btaon_fast_read_8b(uint16_t offset);
uint8_t btaon_fast_read_safe_8b(uint16_t offset);
void btaon_fast_write(uint16_t offset, uint16_t data);
void btaon_fast_write_safe(uint16_t offset, uint16_t data);
void btaon_fast_update(uint16_t offset, uint16_t mask, uint16_t data);
void btaon_fast_update_safe(uint16_t offset, uint16_t mask, uint16_t data);
void btaon_fast_write_8b(uint16_t offset, uint8_t data);
void btaon_fast_write_safe_8b(uint16_t offset, uint8_t data);
void btaon_fast_update_8b(uint16_t offset, uint8_t mask, uint8_t data);
void btaon_fast_update_safe_8b(uint16_t offset, uint8_t mask, uint8_t data);
void aon_indirect_write_reg_safe(uint32_t offset, uint32_t data);
uint32_t aon_indirect_read_reg_safe(uint32_t offset);

#endif /* _INDIRECT_ACCESS_H_ */

