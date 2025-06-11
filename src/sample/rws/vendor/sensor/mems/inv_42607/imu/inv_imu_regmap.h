#ifndef _INV_IMU_REGMAP_H_
#define _INV_IMU_REGMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file inv_imu_regmap.h
 * File exposing the device register map
 */

#include <stdint.h>

/* forward declaration */
struct inv_imu_device;



/* ----------------------------------------------------------------------------
 * Device Register map
 *
 * Next macros defines address for all registers as listed by device
 * datasheet.
 * Macros name is <REGISTER_NAME> with REGISTER_NAME being the name of
 * the corresponding register in datasheet.
 * Note that macro name is <REGISTER_NAME>_Bx with x being the bank
 * number for registers that are in bank 1 and further (suffix is ommitted for
 * bank 0 registers)
 * ---------------------------------------------------------------------------- */

/* BANK0 */
#define MISC_1                                                         0x10000
#define CHIP_CONFIG_REG                                                0x10001
#define SIGNAL_PATH_RESET                                              0x10002
#define DRIVE_CONFIG_REG1                                              0x10003
#define DRIVE_CONFIG_REG2                                              0x10004
#define DRIVE_CONFIG_REG3                                              0x10005
#define INT_CONFIG_REG                                                 0x10006
#define ODRGRID0                                                       0x10007
#define ODRGRID1                                                       0x10008
#define TEMP_DATA0_UI                                                  0x10009
#define TEMP_DATA1_UI                                                  0x1000a
#define ACCEL_DATA_X0_UI                                               0x1000b
#define ACCEL_DATA_X1_UI                                               0x1000c
#define ACCEL_DATA_Y0_UI                                               0x1000d
#define ACCEL_DATA_Y1_UI                                               0x1000e
#define ACCEL_DATA_Z0_UI                                               0x1000f
#define ACCEL_DATA_Z1_UI                                               0x10010
#define GYRO_DATA_X0_UI                                                0x10011
#define GYRO_DATA_X1_UI                                                0x10012
#define GYRO_DATA_Y0_UI                                                0x10013
#define GYRO_DATA_Y1_UI                                                0x10014
#define GYRO_DATA_Z0_UI                                                0x10015
#define GYRO_DATA_Z1_UI                                                0x10016
#define TMST_FSYNC1                                                    0x10017
#define TMST_FSYNC2                                                    0x10018
#define ODR_LP_STATUS                                                  0x10019
#define APEX_DATA4                                                     0x1001d
#define APEX_DATA5                                                     0x1001e
#define PWR_MGMT_0                                                     0x1001f
#define GYRO_CONFIG0                                                   0x10020
#define ACCEL_CONFIG0                                                  0x10021
#define TEMP_CONFIG0                                                   0x10022
#define GYRO_CONFIG1                                                   0x10023
#define ACCEL_CONFIG1                                                  0x10024
#define APEX_CONFIG0                                                   0x10025
#define APEX_CONFIG1                                                   0x10026
#define WOM_CONFIG                                                     0x10027
#define FIFO_CONFIG1                                                   0x10028
#define FIFO_CONFIG2                                                   0x10029
#define FIFO_CONFIG3                                                   0x1002a
#define INT_SOURCE0                                                    0x1002b
#define INT_SOURCE1                                                    0x1002c
#define INT_SOURCE3                                                    0x1002d
#define INT_SOURCE4                                                    0x1002e
#define FIFO_LOST_PKT0                                                 0x1002f
#define FIFO_LOST_PKT1                                                 0x10030
#define APEX_DATA0                                                     0x10031
#define APEX_DATA1                                                     0x10032
#define APEX_DATA2                                                     0x10033
#define APEX_DATA3                                                     0x10034
#define INTF_CONFIG0                                                   0x10035
#define INTF_CONFIG1                                                   0x10036
#define INT_STATUS_DRDY                                                0x10039
#define INT_STATUS                                                     0x1003a
#define INT_STATUS2                                                    0x1003b
#define INT_STATUS3                                                    0x1003c
#define FIFO_BYTE_COUNT1                                               0x1003d
#define FIFO_BYTE_COUNT2                                               0x1003e
#define FIFO_DATA_REG                                                  0x1003f
#define INTF_CONFIG3                                                   0x1004c
#define WHO_AM_I                                                       0x10075
#define BLK_SEL_W                                                      0x10079
#define MADDR_W                                                        0x1007a
#define M_W                                                            0x1007b
#define BLK_SEL_R                                                      0x1007c
#define MADDR_R                                                        0x1007d
#define M_R                                                            0x1007e

/* MREG_BAR */

/* MMEM_SIGP */

/* MREG_TOP1 */
#define TMST_CONFIG1_MREG_TOP1                                         0x00
#define FIFO_CONFIG5_MREG_TOP1                                         0x01
#define FIFO_CONFIG6_MREG_TOP1                                         0x02
#define FSYNC_CONFIG_MREG_TOP1                                         0x03
#define INT_CONFIG0_MREG_TOP1                                          0x04
#define INT_CONFIG1_MREG_TOP1                                          0x05
#define AFSR_CONFIG0_MREG_TOP1                                         0x07
#define AFSR_CONFIG1_MREG_TOP1                                         0x08
#define ST_CONFIG_MREG_TOP1                                            0x13
#define SELFTEST_MREG_TOP1                                             0x14
#define PADS_CONFIG3_MREG_TOP1                                         0x17
#define TEMP_CONFIG1_MREG_TOP1                                         0x1c
#define TEMP_CONFIG3_MREG_TOP1                                         0x1e
#define INTF_CONFIG6_MREG_TOP1                                         0x23
#define INTF_CONFIG10_MREG_TOP1                                        0x25
#define INTF_CONFIG7_MREG_TOP1                                         0x28
#define OTP_CONFIG_MREG_TOP1                                           0x2b
#define INT_SOURCE6_MREG_TOP1                                          0x2f
#define INT_SOURCE7_MREG_TOP1                                          0x30
#define INT_SOURCE8_MREG_TOP1                                          0x31
#define INT_SOURCE9_MREG_TOP1                                          0x32
#define INT_SOURCE10_MREG_TOP1                                         0x33
#define APEX_CONFIG2_MREG_TOP1                                         0x44
#define APEX_CONFIG3_MREG_TOP1                                         0x45
#define APEX_CONFIG4_MREG_TOP1                                         0x46
#define APEX_CONFIG5_MREG_TOP1                                         0x47
#define APEX_CONFIG9_MREG_TOP1                                         0x48
#define APEX_CONFIG10_MREG_TOP1                                        0x49
#define APEX_CONFIG11_MREG_TOP1                                        0x4a
#define ACCEL_WOM_X_THR_MREG_TOP1                                      0x4b
#define ACCEL_WOM_Y_THR_MREG_TOP1                                      0x4c
#define ACCEL_WOM_Z_THR_MREG_TOP1                                      0x4d
#define GOS_USER0_MREG_TOP1                                            0x4e
#define GOS_USER1_MREG_TOP1                                            0x4f
#define GOS_USER2_MREG_TOP1                                            0x50
#define GOS_USER3_MREG_TOP1                                            0x51
#define GOS_USER4_MREG_TOP1                                            0x52
#define GOS_USER5_MREG_TOP1                                            0x53
#define GOS_USER6_MREG_TOP1                                            0x54
#define GOS_USER7_MREG_TOP1                                            0x55
#define GOS_USER8_MREG_TOP1                                            0x56
#define ST_STATUS1_MREG_TOP1                                           0x63
#define ST_STATUS2_MREG_TOP1                                           0x64
#define PSEQ_SPARE_TRIM_MREG_TOP1                                      0x66
#define APEX_CONFIG12_MREG_TOP1                                        0x67

/* MMEM_TOP */
#define XA_ST_DATA_MMEM_TOP                                            0x5000
#define YA_ST_DATA_MMEM_TOP                                            0x5001
#define ZA_ST_DATA_MMEM_TOP                                            0x5002
#define XG_ST_DATA_MMEM_TOP                                            0x5003
#define YG_ST_DATA_MMEM_TOP                                            0x5004
#define ZG_ST_DATA_MMEM_TOP                                            0x5005

/* MREG_OTP */
#define OTP_CTRL7_MREG_OTP                                             0x2806

/* MREG_FPGA */

/* ROM */


/* ---------------------------------------------------------------------------
 * register BANK0
 * ---------------------------------------------------------------------------*/

/*
 * MISC_1
 * Register Name : MISC_1
 */

/*
 * auto_inc_dis
 * This bit controls whether slave will auto increment the address used for accessing registers in mclk domain (or the MREGs).
 *
 * 1: slave does not auto increment the address. For each read or write access to MREGs, host need to specify a new address by programming maddr_r or maddr_w register. Or otherwise, the previous value in maddr_r or maddr_w is used for MREG accessing.
 *
 * 0: After host completes a write operation to MREGs, slave auto increments the internal address pointing to next MREG location. After host completes a read operation to MREGs, slave auto increments the internal address pointing to next MREG location. Slave then performs a read-prefetch based on the auto-incremented address.
 */
#define MISC_1_AUTO_INC_DIS_POS      0x04
#define MISC_1_AUTO_INC_DIS_MASK     (0x01 << MISC_1_AUTO_INC_DIS_POS)

/*
 * mclk_rdy
 * 1 to indicate internal clock is currently running.
 * 0 to indicate internal clock is not currently running.
 */
#define MISC_1_MCLK_RDY_POS      0x03
#define MISC_1_MCLK_RDY_MASK     (0x01 << MISC_1_MCLK_RDY_POS)

/*
 * otp_done
 * 1 to indicate OTP_copy operation is done.
 */
#define MISC_1_OTP_DONE_POS      0x00
#define MISC_1_OTP_DONE_MASK     0x01



/*
 * CHIP_CONFIG_REG
 * Register Name : CHIP_CONFIG_REG
 */

/*
 * spi_ap_4wires
 * When set to 1, SPI uses 4 wire interface. When set to 0, SPI uses 3 wire I/F
 */
#define CHIP_CONFIG_REG_SPI_AP_4WIRES_POS      0x02
#define CHIP_CONFIG_REG_SPI_AP_4WIRES_MASK     (0x01 << CHIP_CONFIG_REG_SPI_AP_4WIRES_POS)

/*
 * spi_mode
 * SPI mode selection. The default value should store in OTP memory.
 * 1: Mode1 and Mode2
 * 0: Mode0 and Mode3
 * If chip is operating in non-SPI mode, user is not allowed to change the power-on default setting of this register. Change of this register won't take effect till CSB=1.
 */
#define CHIP_CONFIG_REG_SPI_MODE_POS      0x00
#define CHIP_CONFIG_REG_SPI_MODE_MASK     0x01



/*
 * SIGNAL_PATH_RESET
 * Register Name : SIGNAL_PATH_RESET
 */

/*
 * soft_reset_chip_config
 * Software reset (Auto Clear bit)
 * 1: Enable reset
 * 0: Normal
 */
#define SIGNAL_PATH_RESET_SOFT_RESET_CHIP_CONFIG_POS      0x04
#define SIGNAL_PATH_RESET_SOFT_RESET_CHIP_CONFIG_MASK     (0x01 << SIGNAL_PATH_RESET_SOFT_RESET_CHIP_CONFIG_POS)

/*
 * fifo_flush
 * When set to 1, FIFO will get flushed. FIFO flush requires special sequence of programming the registers. Please see use notes for details.
 * fifo_flush procedures: (1) Write to fifo_flush =1, (2) Wait for 1.5us, (3) read fifo_flush, it should be 0 now.
 * Host can only program this register bit to 1.
 */
#define SIGNAL_PATH_RESET_FIFO_FLUSH_POS      0x02
#define SIGNAL_PATH_RESET_FIFO_FLUSH_MASK     (0x01 << SIGNAL_PATH_RESET_FIFO_FLUSH_POS)



/*
 * DRIVE_CONFIG_REG1
 * Register Name : DRIVE_CONFIG_REG1
 */

/*
 * pads_ddr_slew_trim
 * Configure drive strength of AP SDA pin when chip is in I3C DDR protocol. While in I3C operation, chip automatically switches to use pads_ddr_slew_trim_d2a after receiving ENTHDR0 ccc command from host. And chip automatically switches back to pads_sdr_slew_trim_d2a after host issues HDR_EXIT pattern.
 * [2:0]: Rise/Fall time spec
 * 3'b000: 20ns(min)~60ns(max)
 * 3'b001: 12ns(min)~36ns(max)
 * 3'b010: 6ns(min)~18ns(max)
 * 3'b011: 4ns(min)~12ns(max)
 * 3'b100: 2ns(min)~6ns(max)
 * 3'b101: less than 2ns.
 * This register should be programmed in I2C or I3C/SDR, but not in I3C/DDR. Changes is taking effect after CSB (of SPI mode) or after STOP (of I2C mode or I3C/SDR mode).
 */
#define DRIVE_CONFIG_REG1_PADS_DDR_SLEW_TRIM_POS      0x03
#define DRIVE_CONFIG_REG1_PADS_DDR_SLEW_TRIM_MASK     (0x07 << DRIVE_CONFIG_REG1_PADS_DDR_SLEW_TRIM_POS)

/*
 * pads_sdr_slew_trim
 * Configure drive strength of AP SDA pin when chip is in I3C SDR protocol. After chip reset, the pads_i2c_slew_trim_d2a is used by default. If I3C feature is enabled, chip automatically switches to use pads_sdr_slew_trim_d2a after receiving 0x7E+W message (an I3C broadcast message).
 * [2:0]: Rise/Fall time spec
 * 3'b000: 20ns(min)~60ns(max)
 * 3'b001: 12ns(min)~36ns(max)
 * 3'b010: 6ns(min)~18ns(max)
 * 3'b011: 4ns(min)~12ns(max)
 * 3'b100: 2ns(min)~6ns(max)
 * 3'b101: less than 2ns.
 * This register should be programmed in I2C or I3C/SDR, but not in I3C/DDR. Changes is taking effect after CSB (of SPI mode) or after STOP (of I2C mode or I3C/SDR mode).
 */
#define DRIVE_CONFIG_REG1_PADS_SDR_SLEW_TRIM_POS      0x00
#define DRIVE_CONFIG_REG1_PADS_SDR_SLEW_TRIM_MASK     0x07



/*
 * DRIVE_CONFIG_REG2
 * Register Name : DRIVE_CONFIG_REG2
 */

/*
 * pads_i2c_slew_trim
 * Configure drive strength of AP SDA pin when chip is in I2C protocol. After chip reset, the pads_i2c_slew_trim_d2a is used by default. If the 1st write operation from host is a SPI transaction, then chip automatically switches to pads_slew_trim_d2a automatically. If I3C feature is enabled, chip automatically switches to use pads_sdr_slew_trim_d2a after receiving 0x7E+W message (an I3C broadcast message).
 * [2:0]: Rise/Fall time spec
 * 3'b000: 20ns(min)~60ns(max)
 * 3'b001: 12ns(min)~36ns(max)
 * 3'b010: 6ns(min)~18ns(max)
 * 3'b011: 4ns(min)~12ns(max)
 * 3'b100: 2ns(min)~6ns(max)
 * 3'b101: less than 2ns.
 * This register should be programmed in I2C or I3C/SDR, but not in I3C/DDR. Changes is taking effect after CSB (of SPI mode) or after STOP (of I2C mode or I3C/SDR mode).
 */
#define DRIVE_CONFIG_REG2_PADS_I2C_SLEW_TRIM_POS      0x03
#define DRIVE_CONFIG_REG2_PADS_I2C_SLEW_TRIM_MASK     (0x07 << DRIVE_CONFIG_REG2_PADS_I2C_SLEW_TRIM_POS)

/*
 * pads_slew_trim_d2a
 * Configure drive strength for all output pins in all modes (SPI3, SPI4, I2C, I3C). This register also configures the driving strength of AP SDA pin when chip is in SPI mode. After chip reset, the pads_i2c_slew_trim_d2a is used by default for AP SDA pin. If the 1st write operation from host is a SPI transaction, then chip switches to pads_slew_trim_d2a automatically.
 * [2:0]: Rise/Fall time spec
 * 3'b000: 20ns(min)~60ns(max)
 * 3'b001: 12ns(min)~36ns(max)
 * 3'b010: 6ns(min)~18ns(max)
 * 3'b011: 4ns(min)~12ns(max)
 * 3'b100: 2ns(min)~6ns(max)
 * 3'b101: less than 2ns.
 * This register should be programmed in I2C or I3C/SDR, but not in I3C/DDR. Changes is taking effect after CSB (of SPI mode) or after STOP (of I2C mode or I3C/SDR mode).
 */
#define DRIVE_CONFIG_REG2_PADS_SLEW_TRIM_D2A_POS      0x00
#define DRIVE_CONFIG_REG2_PADS_SLEW_TRIM_D2A_MASK     0x07



/*
 * DRIVE_CONFIG_REG3
 * Register Name : DRIVE_CONFIG_REG3
 */

/*
 * pads_spi_slew_trim
 * Configure drive strength of SDA pad using SPI3/4 modes. After chip reset, the pads_i2c_slew_trim_d2a is used by default for SDA pins of all ports. If the 1st write operation from a specific host is a SPI3/4 transaction, chip automatically switches to pads_spi_slew_trim_d2a for the SDA port running in SPI3/4 mode.
 * [2:0]: Rise/Fall time spec,
 * 3'b000: 20ns(min)~60ns(max),
 * 3'b001: 12ns(min)~36ns(max),
 * 3'b010: 6ns(min)~18ns(max),
 * 3'b011: 4ns(min)~12ns(max),
 * 3'b100: 2ns(min)~6ns(max),
 * 3'b101: less than 2ns.
 * This register should be programmed in I2C or I3C/SDR, but not in I3C/DDR. Changes is taking effect after CSB (of SPI mode) or after STOP (of I2C mode or I3C/SDR mode).
 */
#define DRIVE_CONFIG_REG3_PADS_SPI_SLEW_TRIM_POS      0x00
#define DRIVE_CONFIG_REG3_PADS_SPI_SLEW_TRIM_MASK     0x07



/*
 * INT_CONFIG_REG
 * Register Name : INT_CONFIG_REG
 */

/*
 * int2_mode
 * Interrupt mode and drive circuit shall be configurable by register.
 * Interrupt Mode
 * 1: Latched Mode
 * 0: Pulsed Mode
 */
#define INT_CONFIG_REG_INT2_MODE_POS      0x05
#define INT_CONFIG_REG_INT2_MODE_MASK     (0x01 << INT_CONFIG_REG_INT2_MODE_POS)

/*
 * int2_drive_circuit
 * Interrupt mode and drive circuit shall be configurable by register.
 * Drive Circuit
 * 1: Push-Pull
 * 0: Open drain
 */
#define INT_CONFIG_REG_INT2_DRIVE_CIRCUIT_POS      0x04
#define INT_CONFIG_REG_INT2_DRIVE_CIRCUIT_MASK     (0x01 << INT_CONFIG_REG_INT2_DRIVE_CIRCUIT_POS)

/*
 * int2_polarity
 * Interrupt mode and drive circuit shall be configurable by register.
 * Interrupt Polarity
 * 1: Active High
 * 0: Active Low
 */
#define INT_CONFIG_REG_INT2_POLARITY_POS      0x03
#define INT_CONFIG_REG_INT2_POLARITY_MASK     (0x01 << INT_CONFIG_REG_INT2_POLARITY_POS)

/*
 * int1_mode
 * Interrupt mode and drive circuit shall be configurable by register.
 * Interrupt Mode
 * 1: Latched Mode
 * 0: Pulsed Mode
 */
#define INT_CONFIG_REG_INT1_MODE_POS      0x02
#define INT_CONFIG_REG_INT1_MODE_MASK     (0x01 << INT_CONFIG_REG_INT1_MODE_POS)

/*
 * int1_drive_circuit
 * Interrupt mode and drive circuit shall be configurable by register.
 * Drive Circuit
 * 1: Push-Pull
 * 0: Open drain
 */
#define INT_CONFIG_REG_INT1_DRIVE_CIRCUIT_POS      0x01
#define INT_CONFIG_REG_INT1_DRIVE_CIRCUIT_MASK     (0x01 << INT_CONFIG_REG_INT1_DRIVE_CIRCUIT_POS)

/*
 * int1_polarity
 * Interrupt mode and drive circuit shall be configurable by register.
 * Interrupt Polarity
 * 1: Active High
 * 0: Active Low
 */
#define INT_CONFIG_REG_INT1_POLARITY_POS      0x00
#define INT_CONFIG_REG_INT1_POLARITY_MASK     0x01



/*
 * ODRGRID0
 * Register Name : ODRGRID0
 */

/*
 * odrgrid
 * Shows the grid alignment for the ODR samples produced by sensors. The following bit(s) will be set to 1 when the corresponding ODR samples are produced
 * bit15 .. bit 11 -:  Always high
 * bit10 -:  1.6 kHz
 * bit9 -:  800 Hz
 * bit8 -:  400 Hz
 * bit7 -:  200 Hz
 * bit6 -:  100 Hz
 * bit5 -:  50 Hz
 * bit4 -:  25 Hz
 * bit3 -:  12.5 Hz
 * bit2 -:  6.25 Hz
 * bit1 -:  3.125 Hz
 * bit0 -:  1.5625 Hz
 */
#define ODRGRID0_ODRGRID_POS      0x00
#define ODRGRID0_ODRGRID_MASK     0xff



/*
 * ODRGRID1
 * Register Name : ODRGRID1
 */

/*
 * odrgrid
 * Shows the grid alignment for the ODR samples produced by sensors. The following bit(s) will be set to 1 when the corresponding ODR samples are produced
 * bit15 .. bit 11 -:  Always high
 * bit10 -:  1.6 kHz
 * bit9 -:  800 Hz
 * bit8 -:  400 Hz
 * bit7 -:  200 Hz
 * bit6 -:  100 Hz
 * bit5 -:  50 Hz
 * bit4 -:  25 Hz
 * bit3 -:  12.5 Hz
 * bit2 -:  6.25 Hz
 * bit1 -:  3.125 Hz
 * bit0 -:  1.5625 Hz
 */
#define ODRGRID1_ODRGRID_POS      0x00
#define ODRGRID1_ODRGRID_MASK     0xff



/*
 * TEMP_DATA0_UI
 * Register Name : TEMP_DATA0_UI
 */

/*
 * temp_data_ui
 * Temperature data for UI  path
 */
#define TEMP_DATA0_UI_TEMP_DATA_UI_POS      0x00
#define TEMP_DATA0_UI_TEMP_DATA_UI_MASK     0xff



/*
 * TEMP_DATA1_UI
 * Register Name : TEMP_DATA1_UI
 */

/*
 * temp_data_ui
 * Temperature data for UI  path
 */
#define TEMP_DATA1_UI_TEMP_DATA_UI_POS      0x00
#define TEMP_DATA1_UI_TEMP_DATA_UI_MASK     0xff



/*
 * ACCEL_DATA_X0_UI
 * Register Name : ACCEL_DATA_X0_UI
 */

/*
 * accel_data_x_ui
 * Accel X axis data for UI  path
 */
#define ACCEL_DATA_X0_UI_ACCEL_DATA_X_UI_POS      0x00
#define ACCEL_DATA_X0_UI_ACCEL_DATA_X_UI_MASK     0xff



/*
 * ACCEL_DATA_X1_UI
 * Register Name : ACCEL_DATA_X1_UI
 */

/*
 * accel_data_x_ui
 * Accel X axis data for UI  path
 */
#define ACCEL_DATA_X1_UI_ACCEL_DATA_X_UI_POS      0x00
#define ACCEL_DATA_X1_UI_ACCEL_DATA_X_UI_MASK     0xff



/*
 * ACCEL_DATA_Y0_UI
 * Register Name : ACCEL_DATA_Y0_UI
 */

/*
 * accel_data_y_ui
 * Accel Y axis data for UI  path
 */
#define ACCEL_DATA_Y0_UI_ACCEL_DATA_Y_UI_POS      0x00
#define ACCEL_DATA_Y0_UI_ACCEL_DATA_Y_UI_MASK     0xff



/*
 * ACCEL_DATA_Y1_UI
 * Register Name : ACCEL_DATA_Y1_UI
 */

/*
 * accel_data_y_ui
 * Accel Y axis data for UI  path
 */
#define ACCEL_DATA_Y1_UI_ACCEL_DATA_Y_UI_POS      0x00
#define ACCEL_DATA_Y1_UI_ACCEL_DATA_Y_UI_MASK     0xff



/*
 * ACCEL_DATA_Z0_UI
 * Register Name : ACCEL_DATA_Z0_UI
 */

/*
 * accel_data_z_ui
 * Accel Z axis data for UI  path
 */
#define ACCEL_DATA_Z0_UI_ACCEL_DATA_Z_UI_POS      0x00
#define ACCEL_DATA_Z0_UI_ACCEL_DATA_Z_UI_MASK     0xff



/*
 * ACCEL_DATA_Z1_UI
 * Register Name : ACCEL_DATA_Z1_UI
 */

/*
 * accel_data_z_ui
 * Accel Z axis data for UI  path
 */
#define ACCEL_DATA_Z1_UI_ACCEL_DATA_Z_UI_POS      0x00
#define ACCEL_DATA_Z1_UI_ACCEL_DATA_Z_UI_MASK     0xff



/*
 * GYRO_DATA_X0_UI
 * Register Name : GYRO_DATA_X0_UI
 */

/*
 * gyro_data_x_ui
 * Gyro X axis data for UI  path
 */
#define GYRO_DATA_X0_UI_GYRO_DATA_X_UI_POS      0x00
#define GYRO_DATA_X0_UI_GYRO_DATA_X_UI_MASK     0xff



/*
 * GYRO_DATA_X1_UI
 * Register Name : GYRO_DATA_X1_UI
 */

/*
 * gyro_data_x_ui
 * Gyro X axis data for UI  path
 */
#define GYRO_DATA_X1_UI_GYRO_DATA_X_UI_POS      0x00
#define GYRO_DATA_X1_UI_GYRO_DATA_X_UI_MASK     0xff



/*
 * GYRO_DATA_Y0_UI
 * Register Name : GYRO_DATA_Y0_UI
 */

/*
 * gyro_data_y_ui
 * Gyro Y axis data for UI  path
 */
#define GYRO_DATA_Y0_UI_GYRO_DATA_Y_UI_POS      0x00
#define GYRO_DATA_Y0_UI_GYRO_DATA_Y_UI_MASK     0xff



/*
 * GYRO_DATA_Y1_UI
 * Register Name : GYRO_DATA_Y1_UI
 */

/*
 * gyro_data_y_ui
 * Gyro Y axis data for UI  path
 */
#define GYRO_DATA_Y1_UI_GYRO_DATA_Y_UI_POS      0x00
#define GYRO_DATA_Y1_UI_GYRO_DATA_Y_UI_MASK     0xff



/*
 * GYRO_DATA_Z0_UI
 * Register Name : GYRO_DATA_Z0_UI
 */

/*
 * gyro_data_z_ui
 * Gyro Z axis data for UI  path
 */
#define GYRO_DATA_Z0_UI_GYRO_DATA_Z_UI_POS      0x00
#define GYRO_DATA_Z0_UI_GYRO_DATA_Z_UI_MASK     0xff



/*
 * GYRO_DATA_Z1_UI
 * Register Name : GYRO_DATA_Z1_UI
 */

/*
 * gyro_data_z_ui
 * Gyro Z axis data for UI  path
 */
#define GYRO_DATA_Z1_UI_GYRO_DATA_Z_UI_POS      0x00
#define GYRO_DATA_Z1_UI_GYRO_DATA_Z_UI_MASK     0xff



/*
 * TMST_FSYNC1
 * Register Name : TMST_FSYNC1
 */

/*
 * tmst_fsync_data_ui
 * Stores the time delta from the  rising edge of FSYNC to the latest ODR until the UI Interface reads the FSYNC tag in the status register
 */
#define TMST_FSYNC1_TMST_FSYNC_DATA_UI_POS      0x00
#define TMST_FSYNC1_TMST_FSYNC_DATA_UI_MASK     0xff



/*
 * TMST_FSYNC2
 * Register Name : TMST_FSYNC2
 */

/*
 * tmst_fsync_data_ui
 * Stores the time delta from the  rising edge of FSYNC to the latest ODR until the UI Interface reads the FSYNC tag in the status register
 */
#define TMST_FSYNC2_TMST_FSYNC_DATA_UI_POS      0x00
#define TMST_FSYNC2_TMST_FSYNC_DATA_UI_MASK     0xff



/*
 * ODR_LP_STATUS
 * Register Name : ODR_LP_STATUS
 */

/*
 * odr_accel_lp_invalid
 * 1 to indicate ODR Accel Low power invalid case.
 * This bit could return a false "1" if read during a power mode/ODR change. Be sure that the mode/configuration is changed before reading it.
 */
#define ODR_LP_STATUS_ODR_ACCEL_LP_INVALID_POS      0x00
#define ODR_LP_STATUS_ODR_ACCEL_LP_INVALID_MASK     0x01

/*
 * odr_accel_wuosc_lp_invalid
 * 1 to indicate ODR Accel WUOSC Low power invalid case.
 * This bit could return a false "1" if read during a power mode/ODR change. Be sure that the mode/configuration is changed before reading it.
 */
#define ODR_LP_STATUS_ODR_ACCEL_WUOSC_LP_INVALID_POS      0x01
#define ODR_LP_STATUS_ODR_ACCEL_WUOSC_LP_INVALID_MASK     (0x01 << ODR_LP_STATUS_ODR_ACCEL_WUOSC_LP_INVALID_POS)

/*
 * odr_gyro_lp_invalid
 * 1 to indicate ODR Gyro Low power invalid case.
 * This bit could return a false "1" if read during a power mode/ODR change. Be sure that the mode/configuration is changed before reading it.
 */
#define ODR_LP_STATUS_ODR_GYRO_LP_INVALID_POS      0x02
#define ODR_LP_STATUS_ODR_GYRO_LP_INVALID_MASK     (0x01 << ODR_LP_STATUS_ODR_GYRO_LP_INVALID_POS)



/*
 * APEX_DATA4
 * Register Name : APEX_DATA4
 */

/*
 * ff_duration
 * Free Fall duration. The duration is given in number of samples and it can be converted to freefall distance by applying the following formula:
 * ff_distance = 0.5*9.81*(ff_duration*dmp_odr_s)^2)
 * Note: dmp_odr_s in the duration of DMP_ODR expressed in seconds.
 */
#define APEX_DATA4_FF_DURATION_POS      0x00
#define APEX_DATA4_FF_DURATION_MASK     0xff



/*
 * APEX_DATA5
 * Register Name : APEX_DATA5
 */

/*
 * ff_duration
 * Free Fall duration. The duration is given in number of samples and it can be converted to freefall distance by applying the following formula:
 * ff_distance = 0.5*9.81*(ff_duration*dmp_odr_s)^2)
 * Note: dmp_odr_s in the duration of DMP_ODR expressed in seconds.
 */
#define APEX_DATA5_FF_DURATION_POS      0x00
#define APEX_DATA5_FF_DURATION_MASK     0xff



/*
 * PWR_MGMT_0
 * Register Name : PWR_MGMT_0
 */

/*
 * accel_lp_clk_sel
 * When set to 0, the Accel LP mode uses WU oscillator clock. When set to 1, the Accel LP mode uses RC oscillator clock.
 *
 * This field can be changed on-the-fly even if accel sensor is already on
 */
#define PWR_MGMT_0_ACCEL_LP_CLK_SEL_POS      0x07
#define PWR_MGMT_0_ACCEL_LP_CLK_SEL_MASK     (0x01 << PWR_MGMT_0_ACCEL_LP_CLK_SEL_POS)

/*
 * idle
 * If this bit is set to 1, the  RC oscillator is powered on even if Accel and Gyro are powered off.
 * Nominally this bit is set to 0, so when Accel and Gyro are powered off,
 * the  chip will go to OFF state , since the RC oscillator will also be powered off.
 *
 * This field can be changed on-the-fly even if a sensor is already on
 */
#define PWR_MGMT_0_IDLE_POS      0x04
#define PWR_MGMT_0_IDLE_MASK     (0x01 << PWR_MGMT_0_IDLE_POS)

/*
 * gyro_mode
 * Gyro_mode[1:0]
 * 00: OFF
 * 01: standby (for gyro)
 * 10: LPM
 * 11: LNM
 * Gyro needs to be  kept on for a minimum of 45 ms. When transitioning from OFF to STBY/LPM or LNM, do not issue any register writes for 200 us. For details please see use notes.
 *
 * This field can be changed on-the-fly even if gyro sensor is already on
 */
#define PWR_MGMT_0_GYRO_MODE_POS      0x02
#define PWR_MGMT_0_GYRO_MODE_MASK     (0x03 << PWR_MGMT_0_GYRO_MODE_POS)

/*
 * accel_mode
 * Accel_mode[1:0]
 * 00: OFF
 * 01: OFF
 * 10: LPM
 * 11: LNM
 * When transitioning from OFF to LPM/LNM, do not issue any register writes for 200 us. For details please see use notes.
 *
 * This field can be changed on-the-fly even if accel sensor is already on
 */
#define PWR_MGMT_0_ACCEL_MODE_POS      0x00
#define PWR_MGMT_0_ACCEL_MODE_MASK     0x03



/*
 * GYRO_CONFIG0
 * Register Name : GYRO_CONFIG0
 */

/*
 * gyro_ui_fs_sel
 * Full scale select for Gyro Primary output
 * 0 - :  2000 dps
 * 1 - :  1000 dps
 * 2 - :  500 dps
 * 3- :  250 dps
 *
 * This field can be changed on-the-fly even if gyro sensor is already on
 */
#define GYRO_CONFIG0_GYRO_UI_FS_SEL_POS      0x05
#define GYRO_CONFIG0_GYRO_UI_FS_SEL_MASK     (0x03 << GYRO_CONFIG0_GYRO_UI_FS_SEL_POS)

/*
 * gyro_odr
 * ODR frequency
 * 0 -:  N/A
 * 1 -:  N/A
 * 2 -:  N/A
 * 3 -:  N/A
 * 4 -:  N/A
 * 5 -:  1.6k (LN only)
 * 6 -:  800 (default, LN only))
 * 7 -:  400
 * 8 -:  200
 * 9 -:  100
 * 10 -:  50
 * 11 -:  25
 * 12 -:  12.5
 * 13 -:  6.25      (LP only)
 * 14 -:  3.125    (LP only)
 * 15 -:  1.5625 (LP only)
 *
 * This field can be changed on-the-fly even if gyro sensor is already on
 */
#define GYRO_CONFIG0_GYRO_ODR_POS      0x00
#define GYRO_CONFIG0_GYRO_ODR_MASK     0x0f



/*
 * ACCEL_CONFIG0
 * Register Name : ACCEL_CONFIG0
 */

/*
 * accel_ui_fs_sel
 * Full scale select for Accel Primary output
 * 0 - :  16 gee
 * 1 - :  8 gee
 * 2 - :  4 gee
 * 3 - :  2 gee
 *
 * This field can be changed on-the-fly even if accel sensor is already on
 */
#define ACCEL_CONFIG0_ACCEL_UI_FS_SEL_POS      0x05
#define ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK     (0x03 << ACCEL_CONFIG0_ACCEL_UI_FS_SEL_POS)

/*
 * accel_odr
 * 0 -:  N/A
 * 1 -:  N/A
 * 2 -:  N/A
 * 3 -:  N/A
 * 4 -:  N/A
 * 5 -:  1.6k (LN only)
 * 6 -:  800 (default, LN only)
 * 7 -:  400
 * 8 -:  200
 * 9 -:  100
 * 10 -:  50
 * 11 -:  25
 * 12 -:  12.5
 * 13 -:  6.25     (LP only)
 * 14 -:  3.125   (LP only)
 * 15 -:  1.5625 (LP only)
 *
 * This field can be changed on-the-fly when accel sensor is already on
 */
#define ACCEL_CONFIG0_ACCEL_ODR_POS      0x00
#define ACCEL_CONFIG0_ACCEL_ODR_MASK     0x0f



/*
 * TEMP_CONFIG0
 * Register Name : TEMP_CONFIG0
 */

/*
 * temp_filt_bw
 * Sets the BW of the  Temp Signal Path
 * 0 -:  Temp DLPF bypassed
 * 1 -:  180
 * 2 -:  72
 * 3 -:  34
 * 4 -:  16
 * 5 -:  8
 * 6 -:  4
 * 7 -:  4
 *
 * This field can be changed on-the-fly even if a sensor is already on
 */
#define TEMP_CONFIG0_TEMP_FILT_BW_POS      0x04
#define TEMP_CONFIG0_TEMP_FILT_BW_MASK     (0x07 << TEMP_CONFIG0_TEMP_FILT_BW_POS)



/*
 * GYRO_CONFIG1
 * Register Name : GYRO_CONFIG1
 */

/*
 * gyro_ui_avg_ind
 * LPM number of averaged ADC samples to generate output sample
 * 0 -:  2 averaged samples
 * 1 -:  4 averaged samples
 * 2 -:  8 averaged samples
 * 3 -:  16 averaged samples
 * 4 -:  32 averaged samples
 * 5 -:  64 averaged samples
 * 6 -:  64 averaged samples
 * 7 -:  64 averaged samples
 *
 * This field cannot be changed when the gyro sensor is in LPM
 */
#define GYRO_CONFIG1_GYRO_UI_AVG_IND_POS      0x04
#define GYRO_CONFIG1_GYRO_UI_AVG_IND_MASK     (0x07 << GYRO_CONFIG1_GYRO_UI_AVG_IND_POS)

/*
 * gyro_ui_filt_bw_ind
 * low pass filter BW selection for LNM
 * 0 low pass filter bypassed
 * 1 -:  BW = 180Hz
 * 2 -:   BW = 121Hz
 * 3 -:   BW = 73Hz
 * 4 -:   BW = 53Hz
 * 5 -:   BW =   34Hz
 * 6 -:   BW =   25Hz
 * 7 -:   BW =   16Hz
 *
 * This field can be changed on-the-fly even if gyro sensor is already on
 */
#define GYRO_CONFIG1_GYRO_UI_FILT_BW_IND_POS      0x00
#define GYRO_CONFIG1_GYRO_UI_FILT_BW_IND_MASK     0x07



/*
 * ACCEL_CONFIG1
 * Register Name : ACCEL_CONFIG1
 */

/*
 * accel_ui_avg_ind
 * LPM number of averaged ADC samples to generate output sample
 * 0 -:  2 averaged samples
 * 1 -:  4 averaged samples
 * 2 -:  8 averaged samples
 * 3 -:  16 averaged samples
 * 4 -:  32 averaged samples
 * 5 -:  64 averaged samples
 * 6 -:  64 averaged samples
 * 7 -:  64 averaged samples
 *
 * This field cannot be changed when the accel sensor is in LPM
 */
#define ACCEL_CONFIG1_ACCEL_UI_AVG_IND_POS      0x04
#define ACCEL_CONFIG1_ACCEL_UI_AVG_IND_MASK     (0x07 << ACCEL_CONFIG1_ACCEL_UI_AVG_IND_POS)

/*
 * accel_ui_filt_bw_ind
 * low pass filter BW selection for LNM
 * 0 low pass filter bypassed
 * 1 -:  BW = 180Hz
 * 2 -:   BW = 121Hz
 * 3 -:   BW = 73Hz
 * 4 -:   BW = 53Hz
 * 5 -:   BW =   34Hz
 * 6 -:   BW =   25Hz
 * 7 -:   BW =   16Hz
 *
 * This field can be changed on-the-fly even if accel sensor is already on
 */
#define ACCEL_CONFIG1_ACCEL_UI_FILT_BW_IND_POS      0x00
#define ACCEL_CONFIG1_ACCEL_UI_FILT_BW_IND_MASK     0x07



/*
 * APEX_CONFIG0
 * Register Name : APEX_CONFIG0
 */

/*
 * dmp_power_save_en
 * Enable Power Saving for DMP algorithms.  DMP block have 16-bit power_save_en. This control bit will be connected to LSB bit of dmp_power_save_en which dm_power_save_en[0]
 */
#define APEX_CONFIG0_DMP_POWER_SAVE_EN_POS      0x03
#define APEX_CONFIG0_DMP_POWER_SAVE_EN_MASK     (0x01 << APEX_CONFIG0_DMP_POWER_SAVE_EN_POS)

/*
 * dmp_init_en
 * Enable Init algorithm. It can only be set to 1. Once this bit is set, the algorithm must be executed in the next ODR with highest priority than the other features. The readback value is the internal signal resynchronized bit that once falling back to zero indicates that the algorithm has finished.
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG0_DMP_INIT_EN_POS      0x02
#define APEX_CONFIG0_DMP_INIT_EN_MASK     (0x01 << APEX_CONFIG0_DMP_INIT_EN_POS)

/*
 * dmp_sram_reset_en
 * enable DMP SRAM to be cleared to 0.
 * [0]: 1 to clear DMP SRAM for APEX operation or Self-test operation.
 * [1]: 1 to clear DMP SRAM for secure Authentication operation. Secure authetication is not supported in Xian so this bit will be un-used.
 * This register field is clearred to 0 after SRAM contents are cleared.
 */
#define APEX_CONFIG0_DMP_SRAM_RESET_EN_POS      0x00
#define APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK     0x03



/*
 * APEX_CONFIG1
 * Register Name : APEX_CONFIG1
 */

/*
 * dmp_smd_en
 * DMP, APEX op config.
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG1_DMP_SMD_EN_POS      0x06
#define APEX_CONFIG1_DMP_SMD_EN_MASK     (0x01 << APEX_CONFIG1_DMP_SMD_EN_POS)

/*
 * dmp_ff_en
 * DMP, APEX op config.
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG1_DMP_FF_EN_POS      0x05
#define APEX_CONFIG1_DMP_FF_EN_MASK     (0x01 << APEX_CONFIG1_DMP_FF_EN_POS)

/*
 * dmp_tilt_en
 * This bit is set by the host to enable the Tilt algorithm
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG1_DMP_TILT_EN_POS      0x04
#define APEX_CONFIG1_DMP_TILT_EN_MASK     (0x01 << APEX_CONFIG1_DMP_TILT_EN_POS)

/*
 * dmp_pedo_en
 * This bit is set by the host to enable the Pedometer algorithm
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG1_DMP_PEDO_EN_POS      0x03
#define APEX_CONFIG1_DMP_PEDO_EN_MASK     (0x01 << APEX_CONFIG1_DMP_PEDO_EN_POS)

/*
 * dmp_odr
 * DMP_ODR = 00 -:  25 Hz ODR
 * DMP_ODR = 01 -:  400 Hz ODR
 * DMP_ODR = 10 -:  50 Hz ODR
 * DMP_ODR = 11 -: 100 Hz ODR
 *
 * The ACCEL ODR (accel_odr) field must be configured to an ODR equal or greater to the DMP ODR (dmp_odr) field, or else the chip's behavior is undefined.
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define APEX_CONFIG1_DMP_ODR_POS      0x00
#define APEX_CONFIG1_DMP_ODR_MASK     0x03



/*
 * WOM_CONFIG
 * Register Name : WOM_CONFIG
 */

/*
 * wom_int_dur
 * 0: legacy
 * 1: WoM interrupt asserted at second overthreshold event
 * 2: WoM interrupt asserted at third overthreshold event
 * 3: WoM interrupt asserted at forth overthreshold event
 *
 * This field can be changed on-the-fly even if accel sensor is already on, but it cannot be changed if wom_en is already enabled.
 */
#define WOM_CONFIG_WOM_INT_DUR_POS      0x03
#define WOM_CONFIG_WOM_INT_DUR_MASK     (0x03 << WOM_CONFIG_WOM_INT_DUR_POS)

/*
 * wom_int_mode
 * When 0 , the  WOM from 3 axis are ORed to produce WOM signal
 * When 1, the  WOM from 3 axis are ANDed to produce WOM signal
 * When ANDed mode is selected all 3 WoM x/y/z interrupts will be issued as equal only if x&y&z are asserted. ANY axes not asserted will kill the other 2.
 *
 * This field can be changed on-the-fly even if accel sensor is already on, but it cannot be changed if wom_en is already enabled.
 */
#define WOM_CONFIG_WOM_INT_MODE_POS      0x02
#define WOM_CONFIG_WOM_INT_MODE_MASK     (0x01 << WOM_CONFIG_WOM_INT_MODE_POS)

/*
 * wom_mode
 * 0 - Initial sample is stored. Future samples are compared to initial sample
 * 1 - Compare current sample to previous sample
 *
 * This field can be changed on-the-fly even if accel sensor is already on, but it cannot be changed if wom_en is already enabled.
 */
#define WOM_CONFIG_WOM_MODE_POS      0x01
#define WOM_CONFIG_WOM_MODE_MASK     (0x01 << WOM_CONFIG_WOM_MODE_POS)

/*
 * wom_en
 * 1: enable wake-on-motion detection.
 * 0: disable wake-on-motion detection.
 *
 * This field can be changed on-the-fly even if accel sensor is already on.
 */
#define WOM_CONFIG_WOM_EN_POS      0x00
#define WOM_CONFIG_WOM_EN_MASK     0x01



/*
 * FIFO_CONFIG1
 * Register Name : FIFO_CONFIG1
 */

/*
 * fifo_mode
 * FIFO mode ctrl:
 * 0: stream-to-FIFO mode
 * 1: STOP-no-FULL mode
 */
#define FIFO_CONFIG1_FIFO_MODE_POS      0x01
#define FIFO_CONFIG1_FIFO_MODE_MASK     (0x01 << FIFO_CONFIG1_FIFO_MODE_POS)

/*
 * fifo_bypass
 * FIFO mode ctrl:
 * 1: fifo is bypassed.
 * 0: fifo is not bypassed.
 */
#define FIFO_CONFIG1_FIFO_BYPASS_POS      0x00
#define FIFO_CONFIG1_FIFO_BYPASS_MASK     0x01



/*
 * FIFO_CONFIG2
 * Register Name : FIFO_CONFIG2
 */

/*
 * fifo_wm
 * Watermark threshold for FIFO level in bytes or records according to fifo_count_format setting.
 * In number of bytes when fifo_count_format=0.
 * In number of records when fifo_count_format=1.
 *
 * Generate interrupt when the FIFO reaches or exceeds fifo_wm size. Interrupt only fires once. This register should be set to non zero value, before choosing this interrupt source. Please see use notes for behavior if this register is set to 0.
 *
 * This field should be changed when FIFO is empty to avoid spurious interrupts.
 */
#define FIFO_CONFIG2_FIFO_WM_POS      0x00
#define FIFO_CONFIG2_FIFO_WM_MASK     0xff



/*
 * FIFO_CONFIG3
 * Register Name : FIFO_CONFIG3
 */

/*
 * fifo_wm
 * Watermark threshold for FIFO level in bytes or records according to fifo_count_format setting.
 * In number of bytes when fifo_count_format=0.
 * In number of records when fifo_count_format=1.
 *
 * Generate interrupt when the FIFO reaches or exceeds fifo_wm size. Interrupt only fires once. This register should be set to non zero value, before choosing this interrupt source. Please see use notes for behavior if this register is set to 0.
 *
 * This field should be changed when FIFO is empty to avoid spurious interrupts.
 */
#define FIFO_CONFIG3_FIFO_WM_POS      0x00
#define FIFO_CONFIG3_FIFO_WM_MASK     0x0f



/*
 * INT_SOURCE0
 * Register Name : INT_SOURCE0
 */

/*
 * int_st_done_int1_en
 * Self Test Done interrupt source enable - Interrupt output 1
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE0_INT_ST_DONE_INT1_EN_POS      0x07
#define INT_SOURCE0_INT_ST_DONE_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_ST_DONE_INT1_EN_POS)

/*
 * int_fsync_int1_en
 * UI FSYNC Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_FSYNC_INT1_EN_POS      0x06
#define INT_SOURCE0_INT_FSYNC_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_FSYNC_INT1_EN_POS)

/*
 * int_pll_rdy_int1_en
 * PLL RDY Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_PLL_RDY_INT1_EN_POS      0x05
#define INT_SOURCE0_INT_PLL_RDY_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_PLL_RDY_INT1_EN_POS)

/*
 * int_reset_done_int1_en
 * RESET DONE Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_RESET_DONE_INT1_EN_POS      0x04
#define INT_SOURCE0_INT_RESET_DONE_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_RESET_DONE_INT1_EN_POS)

/*
 * int_drdy_int1_en
 * UI DRDY Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_DRDY_INT1_EN_POS      0x03
#define INT_SOURCE0_INT_DRDY_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_DRDY_INT1_EN_POS)

/*
 * int_fifo_ths_int1_en
 * FIFO THS Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_FIFO_THS_INT1_EN_POS      0x02
#define INT_SOURCE0_INT_FIFO_THS_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_FIFO_THS_INT1_EN_POS)

/*
 * int_fifo_full_int1_en
 * FIFO FULL Interrupt source enable for int1
 * To avoid  fifo full interrupts while reading FIFO, this bit needs to be  disabled while reading FIFO. Please see use notes for details
 */
#define INT_SOURCE0_INT_FIFO_FULL_INT1_EN_POS      0x01
#define INT_SOURCE0_INT_FIFO_FULL_INT1_EN_MASK     (0x01 << INT_SOURCE0_INT_FIFO_FULL_INT1_EN_POS)

/*
 * int_agc_rdy_int1_en
 * AGC RDY Interrupt source enable for int1
 */
#define INT_SOURCE0_INT_AGC_RDY_INT1_EN_POS      0x00
#define INT_SOURCE0_INT_AGC_RDY_INT1_EN_MASK     0x01



/*
 * INT_SOURCE1
 * Register Name : INT_SOURCE1
 */

/*
 * int_i3c_protocol_err_int1_en
 * Interrupt  for i3c protocol error for int1
 */
#define INT_SOURCE1_INT_I3C_PROTOCOL_ERR_INT1_EN_POS      0x06
#define INT_SOURCE1_INT_I3C_PROTOCOL_ERR_INT1_EN_MASK     (0x01 << INT_SOURCE1_INT_I3C_PROTOCOL_ERR_INT1_EN_POS)

/*
 * int_smd_int1_en
 * SMD Interrupt source enable for int1
 */
#define INT_SOURCE1_INT_SMD_INT1_EN_POS      0x03
#define INT_SOURCE1_INT_SMD_INT1_EN_MASK     (0x01 << INT_SOURCE1_INT_SMD_INT1_EN_POS)

/*
 * int_wom_z_int1_en
 * WOM-Z Interrupt source enable for int1
 */
#define INT_SOURCE1_INT_WOM_Z_INT1_EN_POS      0x02
#define INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK     (0x01 << INT_SOURCE1_INT_WOM_Z_INT1_EN_POS)

/*
 * int_wom_y_int1_en
 * WOM-Y Interrupt source enable for int1
 */
#define INT_SOURCE1_INT_WOM_Y_INT1_EN_POS      0x01
#define INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK     (0x01 << INT_SOURCE1_INT_WOM_Y_INT1_EN_POS)

/*
 * int_wom_x_int1_en
 * WOM-X Interrupt source enable for int1
 */
#define INT_SOURCE1_INT_WOM_X_INT1_EN_POS      0x00
#define INT_SOURCE1_INT_WOM_X_INT1_EN_MASK     0x01



/*
 * INT_SOURCE3
 * Register Name : INT_SOURCE3
 */

/*
 * int_st_done_int2_en
 * UI Self-Test Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_ST_DONE_INT2_EN_POS      0x07
#define INT_SOURCE3_INT_ST_DONE_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_ST_DONE_INT2_EN_POS)

/*
 * int_fsync_int2_en
 * UI FSYNC Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_FSYNC_INT2_EN_POS      0x06
#define INT_SOURCE3_INT_FSYNC_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_FSYNC_INT2_EN_POS)

/*
 * int_pll_rdy_int2_en
 * PLL RDY Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_PLL_RDY_INT2_EN_POS      0x05
#define INT_SOURCE3_INT_PLL_RDY_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_PLL_RDY_INT2_EN_POS)

/*
 * int_reset_done_int2_en
 * Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_RESET_DONE_INT2_EN_POS      0x04
#define INT_SOURCE3_INT_RESET_DONE_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_RESET_DONE_INT2_EN_POS)

/*
 * int_drdy_int2_en
 * UI DRDY Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_DRDY_INT2_EN_POS      0x03
#define INT_SOURCE3_INT_DRDY_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_DRDY_INT2_EN_POS)

/*
 * int_fifo_ths_int2_en
 * FIFO THS Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_FIFO_THS_INT2_EN_POS      0x02
#define INT_SOURCE3_INT_FIFO_THS_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_FIFO_THS_INT2_EN_POS)

/*
 * int_fifo_full_int2_en
 * FIFO FULL Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_FIFO_FULL_INT2_EN_POS      0x01
#define INT_SOURCE3_INT_FIFO_FULL_INT2_EN_MASK     (0x01 << INT_SOURCE3_INT_FIFO_FULL_INT2_EN_POS)

/*
 * int_agc_rdy_int2_en
 * Interrupt source enable for int2
 */
#define INT_SOURCE3_INT_AGC_RDY_INT2_EN_POS      0x00
#define INT_SOURCE3_INT_AGC_RDY_INT2_EN_MASK     0x01



/*
 * INT_SOURCE4
 * Register Name : INT_SOURCE4
 */

/*
 * int_i3c_protocol_err_int2_en
 * Interrupt  for i3c protocol error
 */
#define INT_SOURCE4_INT_I3C_PROTOCOL_ERR_INT2_EN_POS      0x06
#define INT_SOURCE4_INT_I3C_PROTOCOL_ERR_INT2_EN_MASK     (0x01 << INT_SOURCE4_INT_I3C_PROTOCOL_ERR_INT2_EN_POS)

/*
 * int_smd_int2_en
 * SMD Interrupt source enable for int2
 */
#define INT_SOURCE4_INT_SMD_INT2_EN_POS      0x03
#define INT_SOURCE4_INT_SMD_INT2_EN_MASK     (0x01 << INT_SOURCE4_INT_SMD_INT2_EN_POS)

/*
 * int_wom_z_int2_en
 * WOM-Z Interrupt source enable for int2
 */
#define INT_SOURCE4_INT_WOM_Z_INT2_EN_POS      0x02
#define INT_SOURCE4_INT_WOM_Z_INT2_EN_MASK     (0x01 << INT_SOURCE4_INT_WOM_Z_INT2_EN_POS)

/*
 * int_wom_y_int2_en
 * WOM-Y Interrupt source enable for int2
 */
#define INT_SOURCE4_INT_WOM_Y_INT2_EN_POS      0x01
#define INT_SOURCE4_INT_WOM_Y_INT2_EN_MASK     (0x01 << INT_SOURCE4_INT_WOM_Y_INT2_EN_POS)

/*
 * int_wom_x_int2_en
 * WOM-X Interrupt source enable for int2
 */
#define INT_SOURCE4_INT_WOM_X_INT2_EN_POS      0x00
#define INT_SOURCE4_INT_WOM_X_INT2_EN_MASK     0x01



/*
 * FIFO_LOST_PKT0
 * Register Name : FIFO_LOST_PKT0
 */

/*
 * fifo_lost_pkt_cnt
 * Stores the  number of packets lost in the FIFO
 */
#define FIFO_LOST_PKT0_FIFO_LOST_PKT_CNT_POS      0x00
#define FIFO_LOST_PKT0_FIFO_LOST_PKT_CNT_MASK     0xff



/*
 * FIFO_LOST_PKT1
 * Register Name : FIFO_LOST_PKT1
 */

/*
 * fifo_lost_pkt_cnt
 * Stores the  number of packets lost in the FIFO
 */
#define FIFO_LOST_PKT1_FIFO_LOST_PKT_CNT_POS      0x00
#define FIFO_LOST_PKT1_FIFO_LOST_PKT_CNT_MASK     0xff



/*
 * APEX_DATA0
 * Register Name : APEX_DATA0
 */

/*
 * step_cnt
 * This status register indicates numebr of step taken.
 */
#define APEX_DATA0_STEP_CNT_POS      0x00
#define APEX_DATA0_STEP_CNT_MASK     0xff



/*
 * APEX_DATA1
 * Register Name : APEX_DATA1
 */

/*
 * step_cnt
 * This status register indicates numebr of step taken.
 */
#define APEX_DATA1_STEP_CNT_POS      0x00
#define APEX_DATA1_STEP_CNT_MASK     0xff



/*
 * APEX_DATA2
 * Register Name : APEX_DATA2
 */

/*
 * step_cadence
 * Pedometer step cadence.Walk/run cadency in number of samples. Format is u6.2.
 * E.g, At 50Hz and 2Hz walk frequency, the cadency is 25 samples. The register will output 100.
 */
#define APEX_DATA2_STEP_CADENCE_POS      0x00
#define APEX_DATA2_STEP_CADENCE_MASK     0xff



/*
 * APEX_DATA3
 * Register Name : APEX_DATA3
 */

/*
 * dmp_idle
 * DMP in Idle state. DMP block is not performing anu operation.
 */
#define APEX_DATA3_DMP_IDLE_POS      0x02
#define APEX_DATA3_DMP_IDLE_MASK     (0x01 << APEX_DATA3_DMP_IDLE_POS)

/*
 * activity_class
 * The walk/run activity classification is also an output in pedometer. This 2-bit register report the current user activity as following:
 * 0: unknown activity as default output;
 * 1: walk
 * 2: run (the cadence is faster than a predefined threshold).
 */
#define APEX_DATA3_ACTIVITY_CLASS_POS      0x00
#define APEX_DATA3_ACTIVITY_CLASS_MASK     0x03



/*
 * INTF_CONFIG0
 * Register Name : INTF_CONFIG0
 */

/*
 * fifo_sreg_invalid_ind_dis
 * if INVALID_IND_DIS == 1
 * In FIFO: the last Valid and New sample is held and written into the FIFO upon reception of cnt_sreg_update.
 * If no Valid and New sample had been received since the reset, 0x0000 will be written in FIFO
 * In Sensor Data Registers: the last Valid and New sample is held
 * If no Valid and New sample had been received since the reset, the register default value is read back (that is 0x8000)
 */
#define INTF_CONFIG0_FIFO_SREG_INVALID_IND_DIS_POS      0x07
#define INTF_CONFIG0_FIFO_SREG_INVALID_IND_DIS_MASK     (0x01 << INTF_CONFIG0_FIFO_SREG_INVALID_IND_DIS_POS)

/*
 * fifo_count_format
 * Selects Byte mode or Record mode for the FIFO_COUNT and FIFO_WM
 * 0 : FIFO_COUNT1,FIFO_COUNT2,FIFO_WM in Byte mode
 * 1 : FIFO_COUNT1,FIFO_COUNT2,FIFO_WM in Record mode
 */
#define INTF_CONFIG0_FIFO_COUNT_FORMAT_POS      0x06
#define INTF_CONFIG0_FIFO_COUNT_FORMAT_MASK     (0x01 << INTF_CONFIG0_FIFO_COUNT_FORMAT_POS)

/*
 * fifo_count_endian
 * This bit applies to both fifo_count and lost_pkt_count
 * 0 : Little Endian (The LSByte data is read first, followed by MSByte data).
 * 1 : Big Endian (The MSByte data is read first, followed by LSByte data).
 */
#define INTF_CONFIG0_FIFO_COUNT_ENDIAN_POS      0x05
#define INTF_CONFIG0_FIFO_COUNT_ENDIAN_MASK     (0x01 << INTF_CONFIG0_FIFO_COUNT_ENDIAN_POS)

/*
 * sensor_data_endian
 * This bit applies to sensor data to AP, and fifo data.
 * 0 : Little Endian (The LSByte data is read first, followed by MSByte data).
 * 1 : Big Endian (The MSByte data is read first, followed by LSByte data).
 */
#define INTF_CONFIG0_SENSOR_DATA_ENDIAN_POS      0x04
#define INTF_CONFIG0_SENSOR_DATA_ENDIAN_MASK     (0x01 << INTF_CONFIG0_SENSOR_DATA_ENDIAN_POS)

/*
 * sifs_ap_cfg
 * 0x: automatic protocol
 * 10: I2C only (1)
 * 11: SPI only (2)
 *  (1)  10 setting will be effectively disabling SPI protocol only in case ap_i2c_only and ap_spi_only are both 0's (concurrent protocol). In I2C only pad scenarios this setting would be useless, in SPI only pad scenarios it would be harmful (the only available protocol would be killed)
 *   (2)  11 setting will be effectively disabling I2C protocol only in case ap_i2c_only and ap_spi_only are both 0's (concurrent protocol). In SPI only pad scenarios this setting would be useless, in I2C only pad scenarios it would be harmful (the only available protocol would be killed)
 */
#define INTF_CONFIG0_SIFS_AP_CFG_POS      0x00
#define INTF_CONFIG0_SIFS_AP_CFG_MASK     0x03



/*
 * INTF_CONFIG1
 * Register Name : INTF_CONFIG1
 */

/*
 * gyro_afsr_mode
 * 00 : AFSR is disabled -System defaults to LFS , auto selection is disabled
 * 01 : AFSR is disabled -System defaults to HFS , auto selection is disabled
 * 1x : AFSR is enabled - Sytem defaults to Dynamic selection
 * NOTE: this register is only active if feature is eanbled --:  see afsr_feature_disable
 */
#define INTF_CONFIG1_GYRO_AFSR_MODE_POS      0x05
#define INTF_CONFIG1_GYRO_AFSR_MODE_MASK     (0x03 << INTF_CONFIG1_GYRO_AFSR_MODE_POS)

/*
 * i3c_sdr_en
 * Enable I3C SDR mode. Chip will be in pure I2C mode if {i3c_sdr_en, i3c_ddr_en} = 00.
 */
#define INTF_CONFIG1_I3C_SDR_EN_POS      0x03
#define INTF_CONFIG1_I3C_SDR_EN_MASK     (0x01 << INTF_CONFIG1_I3C_SDR_EN_POS)

/*
 * i3c_ddr_en
 * Enable I3C DDR mode. This bit won't take effect unless i3c_sdr_en = 1.
 */
#define INTF_CONFIG1_I3C_DDR_EN_POS      0x02
#define INTF_CONFIG1_I3C_DDR_EN_MASK     (0x01 << INTF_CONFIG1_I3C_DDR_EN_POS)

/*
 * clksel
 * 00 Alway select internal RC oscillator
 * 01 Select PLL when available, else select RC oscillator (default)
 * 10 (Reserved)
 * 11 Disable all clocks
 */
#define INTF_CONFIG1_CLKSEL_POS      0x00
#define INTF_CONFIG1_CLKSEL_MASK     0x03



/*
 * INT_STATUS_DRDY
 * Register Name : INT_STATUS_DRDY
 */

/*
 * int_status_drdy
 * This is a clear on read register.
 * DRDY_DATA_REG (Accel / Gyro)
 */
#define INT_STATUS_DRDY_INT_STATUS_DRDY_POS      0x00
#define INT_STATUS_DRDY_INT_STATUS_DRDY_MASK     0x01



/*
 * INT_STATUS
 * Register Name : INT_STATUS
 */

/*
 * int_status_st_done
 * Self-Test Done
 */
#define INT_STATUS_INT_STATUS_ST_DONE_POS      0x07
#define INT_STATUS_INT_STATUS_ST_DONE_MASK     (0x01 << INT_STATUS_INT_STATUS_ST_DONE_POS)

/*
 * int_status_fsync
 * UI_FSYNC
 */
#define INT_STATUS_INT_STATUS_FSYNC_POS      0x06
#define INT_STATUS_INT_STATUS_FSYNC_MASK     (0x01 << INT_STATUS_INT_STATUS_FSYNC_POS)

/*
 * int_status_pll_rdy
 * PLL_RDY
 */
#define INT_STATUS_INT_STATUS_PLL_RDY_POS      0x05
#define INT_STATUS_INT_STATUS_PLL_RDY_MASK     (0x01 << INT_STATUS_INT_STATUS_PLL_RDY_POS)

/*
 * int_status_reset_done
 * This is a clear on read register.
 * POR complete and Software reset complete
 */
#define INT_STATUS_INT_STATUS_RESET_DONE_POS      0x04
#define INT_STATUS_INT_STATUS_RESET_DONE_MASK     (0x01 << INT_STATUS_INT_STATUS_RESET_DONE_POS)

/*
 * int_status_fifo_ths
 * This is a clear on read register.
 * FIFO Threshold
 */
#define INT_STATUS_INT_STATUS_FIFO_THS_POS      0x02
#define INT_STATUS_INT_STATUS_FIFO_THS_MASK     (0x01 << INT_STATUS_INT_STATUS_FIFO_THS_POS)

/*
 * int_status_fifo_full
 * This is a clear on read register.
 * FIFO Full
 */
#define INT_STATUS_INT_STATUS_FIFO_FULL_POS      0x01
#define INT_STATUS_INT_STATUS_FIFO_FULL_MASK     (0x01 << INT_STATUS_INT_STATUS_FIFO_FULL_POS)

/*
 * int_status_agc_rdy
 * This is a clear on read register.
 * Gyro drive is ready
 */
#define INT_STATUS_INT_STATUS_AGC_RDY_POS      0x00
#define INT_STATUS_INT_STATUS_AGC_RDY_MASK     0x01



/*
 * INT_STATUS2
 * Register Name : INT_STATUS2
 */

/*
 * int_status_smd
 * SMD Interrupt clears on read
 */
#define INT_STATUS2_INT_STATUS_SMD_POS      0x03
#define INT_STATUS2_INT_STATUS_SMD_MASK     (0x01 << INT_STATUS2_INT_STATUS_SMD_POS)

/*
 * int_status_wom_x
 * WOM Interrupt on X axis, clears on read
 */
#define INT_STATUS2_INT_STATUS_WOM_X_POS      0x02
#define INT_STATUS2_INT_STATUS_WOM_X_MASK     (0x01 << INT_STATUS2_INT_STATUS_WOM_X_POS)

/*
 * int_status_wom_y
 * WOM Interrupt on Y axis, clears on read
 */
#define INT_STATUS2_INT_STATUS_WOM_Y_POS      0x01
#define INT_STATUS2_INT_STATUS_WOM_Y_MASK     (0x01 << INT_STATUS2_INT_STATUS_WOM_Y_POS)

/*
 * int_status_wom_z
 * WOM interrupt on Z axis , clears on read
 */
#define INT_STATUS2_INT_STATUS_WOM_Z_POS      0x00
#define INT_STATUS2_INT_STATUS_WOM_Z_MASK     0x01



/*
 * INT_STATUS3
 * Register Name : INT_STATUS3
 */

/*
 * int_status_step_det
 * DMP Step Detect
 * 0 : No event
 * 1 : DMP Step Detect event
 */
#define INT_STATUS3_INT_STATUS_STEP_DET_POS      0x05
#define INT_STATUS3_INT_STATUS_STEP_DET_MASK     (0x01 << INT_STATUS3_INT_STATUS_STEP_DET_POS)

/*
 * int_status_step_cnt_ovfl
 * DMP Step Counter Overflow
 * 0 : No event
 * 1 : DMP Step Counter Overflow event
 */
#define INT_STATUS3_INT_STATUS_STEP_CNT_OVFL_POS      0x04
#define INT_STATUS3_INT_STATUS_STEP_CNT_OVFL_MASK     (0x01 << INT_STATUS3_INT_STATUS_STEP_CNT_OVFL_POS)

/*
 * int_status_tilt_det
 * Tilt Detection
 * 0 : No event
 * 1 : Tilt detection event
 */
#define INT_STATUS3_INT_STATUS_TILT_DET_POS      0x03
#define INT_STATUS3_INT_STATUS_TILT_DET_MASK     (0x01 << INT_STATUS3_INT_STATUS_TILT_DET_POS)

/*
 * int_status_ff
 * free Fall Detect
 * 0 : No event
 * 1 : Free Fall  event
 */
#define INT_STATUS3_INT_STATUS_FF_POS      0x02
#define INT_STATUS3_INT_STATUS_FF_MASK     (0x01 << INT_STATUS3_INT_STATUS_FF_POS)

/*
 * int_status_lowg
 * LowG Detect
 * 0 : No event
 * 1 : LowG event
 */
#define INT_STATUS3_INT_STATUS_LOWG_POS      0x01
#define INT_STATUS3_INT_STATUS_LOWG_MASK     (0x01 << INT_STATUS3_INT_STATUS_LOWG_POS)



/*
 * FIFO_BYTE_COUNT1
 * Register Name : FIFO_BYTE_COUNT1
 */

/*
 * fifo_count_byte
 * Number of bytes in FIFO when fifo_count_format=0.
 * Number of records in FIFO when fifo_count_format=1.
 */
#define FIFO_BYTE_COUNT1_FIFO_COUNT_BYTE_POS      0x00
#define FIFO_BYTE_COUNT1_FIFO_COUNT_BYTE_MASK     0xff



/*
 * FIFO_BYTE_COUNT2
 * Register Name : FIFO_BYTE_COUNT2
 */

/*
 * fifo_count_byte
 * Number of bytes in FIFO when fifo_count_format=0.
 * Number of records in FIFO when fifo_count_format=1.
 */
#define FIFO_BYTE_COUNT2_FIFO_COUNT_BYTE_POS      0x00
#define FIFO_BYTE_COUNT2_FIFO_COUNT_BYTE_MASK     0xff



/*
 * FIFO_DATA_REG
 * Register Name : FIFO_DATA_REG
 */

/*
 * fifo_data_reg
 * FIFO output port
 */
#define FIFO_DATA_REG_FIFO_DATA_REG_POS      0x00
#define FIFO_DATA_REG_FIFO_DATA_REG_MASK     0xff



/*
 * INTF_CONFIG3
 * Register Name : INTF_CONFIG3
 */

/*
 * pads_ap_sdo_from_pad_disable_d2a
 * Receiver Enable, Low Assertion: 1= RX Path Disabled, 0 = RX Path Enabled.
 * Set to 1 when SPI configuration is selected
 */
#define INTF_CONFIG3_PADS_AP_SDO_FROM_PAD_DISABLE_D2A_POS      0x03
#define INTF_CONFIG3_PADS_AP_SDO_FROM_PAD_DISABLE_D2A_MASK     (0x01 << INTF_CONFIG3_PADS_AP_SDO_FROM_PAD_DISABLE_D2A_POS)

/*
 * WHO_AM_I
 * Register Name : WHO_AM_I
 */

/*
 * who_am_i
 * TDK chip id.
 */
#define WHO_AM_I_WHO_AM_I_POS      0x00
#define WHO_AM_I_WHO_AM_I_MASK     0xff



/*
 * BLK_SEL_W
 * Register Name : BLK_SEL_W
 */

/*
 * blk_sel_w
 * For write operation, select a 256-byte MCLK space, or 128-byte SCLK space.
 * Automatically reset when OTP copy operation is triggered.
 */
#define BLK_SEL_W_BLK_SEL_W_POS      0x00
#define BLK_SEL_W_BLK_SEL_W_MASK     0xff



/*
 * MADDR_W
 * Register Name : MADDR_W
 */

/*
 * maddr_w
 * For MREG write operation, the lower 8-bit address for accessing MCLK domain registers.
 */
#define MADDR_W_MADDR_W_POS      0x00
#define MADDR_W_MADDR_W_MASK     0xff



/*
 * M_W
 * Register Name : M_W
 */

/*
 * m_w
 * For MREG write operation, the write port for accessing MCLK domain registers.
 */
#define M_W_M_W_POS      0x00
#define M_W_M_W_MASK     0xff



/*
 * BLK_SEL_R
 * Register Name : BLK_SEL_R
 */

/*
 * blk_sel_r
 * For read operation, select a 256-byte MCLK space, or 128-byte SCLK space.
 * Automatically reset when OTP copy operation is triggered.
 */
#define BLK_SEL_R_BLK_SEL_R_POS      0x00
#define BLK_SEL_R_BLK_SEL_R_MASK     0xff



/*
 * MADDR_R
 * Register Name : MADDR_R
 */

/*
 * maddr_r
 * For MREG read operation, the lower 8-bit address for accessing MCLK domain registers.
 */
#define MADDR_R_MADDR_R_POS      0x00
#define MADDR_R_MADDR_R_MASK     0xff



/*
 * M_R
 * Register Name : M_R
 */

/*
 * m_r
 * For MREG read operation, the read port for accessing MCLK domain registers.
 */
#define M_R_M_R_POS      0x00
#define M_R_M_R_MASK     0xff


/* ---------------------------------------------------------------------------
 * register MREG_BAR
 * ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
 * register MMEM_SIGP
 * ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
 * register MREG_TOP1
 * ---------------------------------------------------------------------------*/

/*
 * TMST_CONFIG1
 * Register Name : TMST_CONFIG1
 */

/*
 * tmst_on_sreg_en
 * TMST operation:
 * 0: Backward compatible mode.
 * 1: Timestamp will be reported in the sense regsiters when FSYNC is not present.
 */
#define TMST_CONFIG1_TMST_ON_SREG_EN_POS      0x04
#define TMST_CONFIG1_TMST_ON_SREG_EN_MASK     (0x01 << TMST_CONFIG1_TMST_ON_SREG_EN_POS)

/*
 * tmst_resol
 * Time Stamp resolution; When set to 0 (default), time stamp resolution is 1 us. When set to 1, resolution is 16us
 */
#define TMST_CONFIG1_TMST_RESOL_POS      0x03
#define TMST_CONFIG1_TMST_RESOL_MASK     (0x01 << TMST_CONFIG1_TMST_RESOL_POS)

/*
 * tmst_delta_en
 * Time Stamp delta Enable : When set to 1, the Time stamp field contains the  measurement of time since  the last occurrence of ODR.
 */
#define TMST_CONFIG1_TMST_DELTA_EN_POS      0x02
#define TMST_CONFIG1_TMST_DELTA_EN_MASK     (0x01 << TMST_CONFIG1_TMST_DELTA_EN_POS)

/*
 * tmst_fsync_en
 * Time Stamp register Fsync Enable . When set to 1, the contents of the Timestamp feature of  FSYNC  is enabled. The  user also needs to select  fifo_tmst_fsync_en in order to propagate the timestamp value to the  FIFO
 */
#define TMST_CONFIG1_TMST_FSYNC_EN_POS      0x01
#define TMST_CONFIG1_TMST_FSYNC_EN_MASK     (0x01 << TMST_CONFIG1_TMST_FSYNC_EN_POS)

/*
 * tmst_en
 * Time Stamp register Enable
 */
#define TMST_CONFIG1_TMST_EN_POS      0x00
#define TMST_CONFIG1_TMST_EN_MASK     0x01



/*
 * FIFO_CONFIG5
 * Register Name : FIFO_CONFIG5
 */

/*
 * fifo_wm_gt_th
 * 1: trigger FIFO-Watermark interrupt on every ODR(DMA Write)  if FIFO_COUNT: =FIFO_WM
 *
 * 0: Trigger FIFO-Watermark interrupt when FIFO_COUNT == FIFO_WM
 */
#define FIFO_CONFIG5_FIFO_WM_GT_TH_POS      0x05
#define FIFO_CONFIG5_FIFO_WM_GT_TH_MASK     (0x01 << FIFO_CONFIG5_FIFO_WM_GT_TH_POS)

/*
 * fifo_resume_partial_rd
 * 0: FIFO is read in packets. If a partial packet is read, then the subsequent read will start from the beginning of the un-read packet.
 * 1: FIFO can be read partially. When read is resumed, FIFO bytes will  continue  from last read point. The  SW driver is responsible for cascading previous read and present read and maintain frame boundaries.
 */
#define FIFO_CONFIG5_FIFO_RESUME_PARTIAL_RD_POS      0x04
#define FIFO_CONFIG5_FIFO_RESUME_PARTIAL_RD_MASK     (0x01 << FIFO_CONFIG5_FIFO_RESUME_PARTIAL_RD_POS)

/*
 * fifo_hires_en
 * Allows 20 bit resolution in the  FIFO packet readout
 */
#define FIFO_CONFIG5_FIFO_HIRES_EN_POS      0x03
#define FIFO_CONFIG5_FIFO_HIRES_EN_MASK     (0x01 << FIFO_CONFIG5_FIFO_HIRES_EN_POS)

/*
 * fifo_tmst_fsync_en
 * Allows the TMST in the FIFO to be replaced by the FSYNC timestamp
 */
#define FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_POS      0x02
#define FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_MASK     (0x01 << FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_POS)

/*
 * fifo_gyro_en
 * Enables Gyro Packets to go to FIFO
 */
#define FIFO_CONFIG5_FIFO_GYRO_EN_POS      0x01
#define FIFO_CONFIG5_FIFO_GYRO_EN_MASK     (0x01 << FIFO_CONFIG5_FIFO_GYRO_EN_POS)

/*
 * fifo_accel_en
 * Enable Accel Packets to go to FIFO
 */
#define FIFO_CONFIG5_FIFO_ACCEL_EN_POS      0x00
#define FIFO_CONFIG5_FIFO_ACCEL_EN_MASK     0x01



/*
 * FIFO_CONFIG6
 * Register Name : FIFO_CONFIG6
 */

/*
 * fifo_empty_indicator_dis
 * 0: xFF is sent out as FIFO data when FIFO is empty.
 * 1: The last FIFO data is sent out when FIFO is empty.
 */
#define FIFO_CONFIG6_FIFO_EMPTY_INDICATOR_DIS_POS      0x04
#define FIFO_CONFIG6_FIFO_EMPTY_INDICATOR_DIS_MASK     (0x01 << FIFO_CONFIG6_FIFO_EMPTY_INDICATOR_DIS_POS)

/*
 * rcosc_req_on_fifo_ths_dis
 * 0: when FIFO is operating in ALP mode and the watermark interrupt is enabled, FIFO wakes up the system oscillator as soon as the watermark level is reached. The oscillator remains enabled until a Host FIFO read operation happens. As side effect un extra power consumption could be seen in LP mode when the RCOSC is expected to be off.
 * 1: RCOSC is not automatically requested by the FIFO/INT circuit when the WM interrupt is triggered. As side effect the host can receive invalid packets until RCOSC is off.
 */
#define FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_POS      0x00
#define FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK     0x01



/*
 * FSYNC_CONFIG
 * Register Name : FSYNC_CONFIG
 */

/*
 * fsync_ui_sel
 * this register was called (ext_sync_sel)
 * 0 Do not tag Fsync flag
 * 1 Tag Fsync flag to TEMP_OUT's LSB
 * 2 Tag Fsync flag to GYRO_XOUT's LSB
 * 3 Tag Fsync flag to GYRO_YOUT's LSB
 * 4 Tag Fsync flag to GYRO_ZOUT's LSB
 * 5 Tag Fsync flag to ACCEL_XOUT's LSB
 * 6 Tag Fsync flag to ACCEL_YOUT's LSB
 * 7 Tag Fsync flag to ACCEL_ZOUT's LSB
 */
#define FSYNC_CONFIG_FSYNC_UI_SEL_POS      0x04
#define FSYNC_CONFIG_FSYNC_UI_SEL_MASK     (0x07 << FSYNC_CONFIG_FSYNC_UI_SEL_POS)

/*
 * fsync_ui_flag_clear_sel
 * 0 means the FSYNC flag is cleared when UI sensor reg is updated
 * 1 means the FSYNC flag is cleared when UI interface  reads the sensor register LSB of FSYNC tagged axis
 */
#define FSYNC_CONFIG_FSYNC_UI_FLAG_CLEAR_SEL_POS      0x01
#define FSYNC_CONFIG_FSYNC_UI_FLAG_CLEAR_SEL_MASK     (0x01 << FSYNC_CONFIG_FSYNC_UI_FLAG_CLEAR_SEL_POS)

/*
 * fsync_pol
 * 0 is  rising edge,
 * 1 is falling edge
 */
#define FSYNC_CONFIG_FSYNC_POL_POS      0x00
#define FSYNC_CONFIG_FSYNC_POL_MASK     0x01



/*
 * INT_CONFIG0
 * Register Name : INT_CONFIG0
 */

/*
 * int_drdy_clear_sel
 * Interrupt Line Clear Option (latched mode)
 * 0 Clear on Status Bit Read
 * 1 Clear on Status Bit Read
 * 2 Clear on any SREG data byte read
 * 3 Clear on Status Bit Read OR on any SREG data byte read
 */
#define INT_CONFIG0_INT_DRDY_CLEAR_SEL_POS      0x04
#define INT_CONFIG0_INT_DRDY_CLEAR_SEL_MASK     (0x03 << INT_CONFIG0_INT_DRDY_CLEAR_SEL_POS)

/*
 * int_fifo_ths_clear_sel
 * Interrupt Line Clear Option (latched mode)
 * 0 Clear on Status Bit Read
 * 1 Clear on Status Bit Read
 * 2 Clear on FIFO data 1Byte Read
 * 3 Clear on Status Bit Read OR on FIFO data 1B read
 */
#define INT_CONFIG0_INT_FIFO_THS_CLEAR_SEL_POS      0x02
#define INT_CONFIG0_INT_FIFO_THS_CLEAR_SEL_MASK     (0x03 << INT_CONFIG0_INT_FIFO_THS_CLEAR_SEL_POS)

/*
 * int_fifo_full_clear_sel
 * Interrupt Line Clear Option (latched mode)
 * 0 Clear on Status Bit Read
 * 1 Clear on Status Bit Read
 * 2 Clear on FIFO data 1Byte Read
 * 3 Clear on Status Bit Read OR on FIFO data 1B read
 */
#define INT_CONFIG0_INT_FIFO_FULL_CLEAR_SEL_POS      0x00
#define INT_CONFIG0_INT_FIFO_FULL_CLEAR_SEL_MASK     0x03



/*
 * INT_CONFIG1
 * Register Name : INT_CONFIG1
 */

/*
 * int_tpulse_duration
 * 0 - (Default) Interrupt pulse duration is 100us
 * 1- Interrupt pulse duration is  8 us
 */
#define INT_CONFIG1_INT_TPULSE_DURATION_POS      0x06
#define INT_CONFIG1_INT_TPULSE_DURATION_MASK     (0x01 << INT_CONFIG1_INT_TPULSE_DURATION_POS)

/*
 * int_asy_rst_disable
 * Removes the asynchronous clear of the interrupt output when disabling or cleaning all the active interrupt sources, action which may causes a cut on the interrupt pulse length in pulsed mode.)
 */
#define INT_CONFIG1_INT_ASY_RST_DISABLE_POS      0x04
#define INT_CONFIG1_INT_ASY_RST_DISABLE_MASK     (0x01 << INT_CONFIG1_INT_ASY_RST_DISABLE_POS)



/*
 * AFSR_CONFIG0
 * Register Name : AFSR_CONFIG0
 */

/*
 * gyro_afsr_th_high
 * Values for th_high
 * [256 384 512(default) 640 768 896 1024]
 */
#define AFSR_CONFIG0_GYRO_AFSR_TH_HIGH_POS      0x03
#define AFSR_CONFIG0_GYRO_AFSR_TH_HIGH_MASK     (0x07 << AFSR_CONFIG0_GYRO_AFSR_TH_HIGH_POS)

/*
 * gyro_afsr_th_low
 * Values for th_low
 * [256 384(default) 512 640 768 896 1024]
 */
#define AFSR_CONFIG0_GYRO_AFSR_TH_LOW_POS      0x00
#define AFSR_CONFIG0_GYRO_AFSR_TH_LOW_MASK     0x07



/*
 * AFSR_CONFIG1
 * Register Name : AFSR_CONFIG1
 */

/*
 * gyro_afsr_pls_ext_ind
 * configuration for the delay used to transtition from high full scale to low full scale
 * ind         count     delay[ms]
 * 0          bypass            0
 * 1              16              10
 * 2              32              20    (default)
 * 3              64              40
 * 4             128             80
 * 5             256            160
 * 6             512            320
 * 7            1024           640
 */
#define AFSR_CONFIG1_GYRO_AFSR_PLS_EXT_IND_POS      0x00
#define AFSR_CONFIG1_GYRO_AFSR_PLS_EXT_IND_MASK     0x07



/*
 * ST_CONFIG
 * Register Name : ST_CONFIG
 */

/*
 * st_num_sample
 * This bit selects the number of sensor samples that should be used to process self-test.
 * 0: 16 samples.
 * 1: 200 samples.
 */
#define ST_CONFIG_ST_NUM_SAMPLE_POS      0x06
#define ST_CONFIG_ST_NUM_SAMPLE_MASK     (0x01 << ST_CONFIG_ST_NUM_SAMPLE_POS)

/*
 * accel_st_lim
 * These bits control the tolerated ratio between self-test processed values and reference (fused) ones for accelerometer.
 * 0 : 5%
 * 1: 10%
 * 2: 15%
 * 3: 20%
 * 4: 25%
 * 5: 30%
 * 6: 40%
 * 7: 50%
 */
#define ST_CONFIG_ACCEL_ST_LIM_POS      0x03
#define ST_CONFIG_ACCEL_ST_LIM_MASK     (0x07 << ST_CONFIG_ACCEL_ST_LIM_POS)

/*
 * gyro_st_lim
 * These bits control the tolerated ratio between self-test processed values and reference (fused) ones for gyro.
 * 0 : 5%
 * 1: 10%
 * 2: 15%
 * 3: 20%
 * 4: 25%
 * 5: 30%
 * 6: 40%
 * 7: 50%
 */
#define ST_CONFIG_GYRO_ST_LIM_POS      0x00
#define ST_CONFIG_GYRO_ST_LIM_MASK     0x07



/*
 * SELFTEST
 * Register Name : SELFTEST
 */

/*
 * gyro_st_en
 * 1: enably gyro self test operation. Host needs to program this bit to 0 to move chip out of self test mode. If host programs this bit to 0 while st_busy = 1 and st_done =0, the current running self-test operation is terminated by host.
 */
#define SELFTEST_GYRO_ST_EN_POS      0x07
#define SELFTEST_GYRO_ST_EN_MASK     (0x01 << SELFTEST_GYRO_ST_EN_POS)

/*
 * accel_st_en
 * 1: enably accel self test operation. Host needs to program this bit to 0 to move chip out of self test mode. If host programs this bit to 0 while st_busy = 1 and st_done =0, the current running self-test operation is terminated by host.
 */
#define SELFTEST_ACCEL_ST_EN_POS      0x06
#define SELFTEST_ACCEL_ST_EN_MASK     (0x01 << SELFTEST_ACCEL_ST_EN_POS)

/*
 * en_gz_st
 * Enable Gyro Z-axis self test
 */
#define SELFTEST_EN_GZ_ST_POS      0x05
#define SELFTEST_EN_GZ_ST_MASK     (0x01 << SELFTEST_EN_GZ_ST_POS)

/*
 * en_gy_st
 * Enable Gyro Y-axis self test
 */
#define SELFTEST_EN_GY_ST_POS      0x04
#define SELFTEST_EN_GY_ST_MASK     (0x01 << SELFTEST_EN_GY_ST_POS)

/*
 * en_gx_st
 * Enable Gyro X-axis self test
 */
#define SELFTEST_EN_GX_ST_POS      0x03
#define SELFTEST_EN_GX_ST_MASK     (0x01 << SELFTEST_EN_GX_ST_POS)

/*
 * en_az_st
 * Enable Accel Z-axis self test
 */
#define SELFTEST_EN_AZ_ST_POS      0x02
#define SELFTEST_EN_AZ_ST_MASK     (0x01 << SELFTEST_EN_AZ_ST_POS)

/*
 * en_ay_st
 * Enable Accel Y-axis self test
 */
#define SELFTEST_EN_AY_ST_POS      0x01
#define SELFTEST_EN_AY_ST_MASK     (0x01 << SELFTEST_EN_AY_ST_POS)

/*
 * en_ax_st
 * Enable Accel X-axis self test
 */
#define SELFTEST_EN_AX_ST_POS      0x00
#define SELFTEST_EN_AX_ST_MASK     0x01



/*
 * PADS_CONFIG3
 * Register Name : PADS_CONFIG3
 */

/*
 * pads_avdd_mon_d2a
 * Pin function: when high, connects io_reg_avdd inside pads_top to one of the atp busses
 * Requirement at system level: when high, should not allow any other IP to connect to atp busses
 */
#define PADS_CONFIG3_PADS_AVDD_MON_D2A_POS      0x07
#define PADS_CONFIG3_PADS_AVDD_MON_D2A_MASK     (0x01 << PADS_CONFIG3_PADS_AVDD_MON_D2A_POS)

/*
 * pads_tp_lpbk_d2a
 * Pin function: when high, shorts two of the atp busses
 * Requirement at system level: when high, should not allow any other IP to connect to atp busses
 */
#define PADS_CONFIG3_PADS_TP_LPBK_D2A_POS      0x06
#define PADS_CONFIG3_PADS_TP_LPBK_D2A_MASK     (0x01 << PADS_CONFIG3_PADS_TP_LPBK_D2A_POS)

/*
 * TEMP_CONFIG1
 * Register Name : TEMP_CONFIG1
 */

/*
 * temp_offset
 * Register offset to set the register zero to 25degC
 */
#define TEMP_CONFIG1_TEMP_OFFSET_POS      0x00
#define TEMP_CONFIG1_TEMP_OFFSET_MASK     0x7f



/*
 * TEMP_CONFIG3
 * Register Name : TEMP_CONFIG3
 */

/*
 * temp_dem
 * Enable Dynamic Element Selection (DEM) mode
 */
#define TEMP_CONFIG3_TEMP_DEM_POS      0x02
#define TEMP_CONFIG3_TEMP_DEM_MASK     (0x01 << TEMP_CONFIG3_TEMP_DEM_POS)

/*
 * INTF_CONFIG6
 * Register Name : INTF_CONFIG6
 */

/*
 * i3c_timeout_en
 * Value of 1 to enable i2c/i3c timeout function
 */
#define INTF_CONFIG6_I3C_TIMEOUT_EN_POS      0x04
#define INTF_CONFIG6_I3C_TIMEOUT_EN_MASK     (0x01 << INTF_CONFIG6_I3C_TIMEOUT_EN_POS)

/*
 * i3c_ibi_byte_en
 * I3C Enable IBI-payload function.
 */
#define INTF_CONFIG6_I3C_IBI_BYTE_EN_POS      0x03
#define INTF_CONFIG6_I3C_IBI_BYTE_EN_MASK     (0x01 << INTF_CONFIG6_I3C_IBI_BYTE_EN_POS)

/*
 * i3c_ibi_en
 * I3C Enable IBI function.
 */
#define INTF_CONFIG6_I3C_IBI_EN_POS      0x02
#define INTF_CONFIG6_I3C_IBI_EN_MASK     (0x01 << INTF_CONFIG6_I3C_IBI_EN_POS)



/*
 * INTF_CONFIG10
 * Register Name : INTF_CONFIG10
 */

/*
 * asynctime0_dis
 * 1: Disable asynchronouse timing control mode 0 operation.
 */
#define INTF_CONFIG10_ASYNCTIME0_DIS_POS      0x07
#define INTF_CONFIG10_ASYNCTIME0_DIS_MASK     (0x01 << INTF_CONFIG10_ASYNCTIME0_DIS_POS)

/*
 * INTF_CONFIG7
 * Register Name : INTF_CONFIG7
 */

/*
 * i3c_ddr_wr_mode
 * This bit controls how I3C slave treats the 1st 2-byte data from
 *     host in a DDR write operation.
 *
 *     0: (a) The 1st-byte in DDR-WR configures the starting register
 *            address where the write operation should occur.
 *        (b) The 2nd-byte in DDR-WR is ignored and dropped.
 *        (c) The 3rd-byte in DDR-WR will be written into the register
 *            with address specified by the 1st-byte.
 *            Or, the next DDR-RD will be starting from the address
 *            specified by the 1st-byte of previous DDR-WR.
 *
 *     1:  (a) The 1st-byte in DDR-WR configures the starting register
 *             address where the write operation should occur.
 *         (b) The 2nd-byte in DDR-WR will be written into the register
 *             with address specified by the 1st-byte.
 */
#define INTF_CONFIG7_I3C_DDR_WR_MODE_POS      0x03
#define INTF_CONFIG7_I3C_DDR_WR_MODE_MASK     (0x01 << INTF_CONFIG7_I3C_DDR_WR_MODE_POS)

/*
 * OTP_CONFIG
 * Register Name : OTP_CONFIG
 */

/*
 * otp_copy_mode
 * Specify OTP copy mode.
 * 0: Reserved
 * 1: copy OTP blk 1-n to SRAM, except the self-test related data.
 * 2: copy security authentication keys. (Reserved for customer).
 * 3: copy self-test data from OTP blk 1-n to SRAM.
 */
#define OTP_CONFIG_OTP_COPY_MODE_POS      0x02
#define OTP_CONFIG_OTP_COPY_MODE_MASK     (0x03 << OTP_CONFIG_OTP_COPY_MODE_POS)

/*
 * INT_SOURCE6
 * Register Name : INT_SOURCE6
 */

/*
 * int_ff_int1_en
 * Free Fall interrupt source enable for int1
 */
#define INT_SOURCE6_INT_FF_INT1_EN_POS      0x07
#define INT_SOURCE6_INT_FF_INT1_EN_MASK     (0x01 << INT_SOURCE6_INT_FF_INT1_EN_POS)

/*
 * int_lowg_int1_en
 * LowG interrupt source enable for int1
 */
#define INT_SOURCE6_INT_LOWG_INT1_EN_POS      0x06
#define INT_SOURCE6_INT_LOWG_INT1_EN_MASK     (0x01 << INT_SOURCE6_INT_LOWG_INT1_EN_POS)

/*
 * int_step_det_int1_en
 * Step Detect Interrupt routed to INT1
 */
#define INT_SOURCE6_INT_STEP_DET_INT1_EN_POS      0x05
#define INT_SOURCE6_INT_STEP_DET_INT1_EN_MASK     (0x01 << INT_SOURCE6_INT_STEP_DET_INT1_EN_POS)

/*
 * int_step_cnt_ovfl_int1_en
 * Overflow of Step Counter routed to INT1
 */
#define INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_POS      0x04
#define INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_MASK     (0x01 << INT_SOURCE6_INT_STEP_CNT_OVFL_INT1_EN_POS)

/*
 * int_tilt_det_int1_en
 * Tilt detector Interrupt routed to INT1
 */
#define INT_SOURCE6_INT_TILT_DET_INT1_EN_POS      0x03
#define INT_SOURCE6_INT_TILT_DET_INT1_EN_MASK     (0x01 << INT_SOURCE6_INT_TILT_DET_INT1_EN_POS)



/*
 * INT_SOURCE7
 * Register Name : INT_SOURCE7
 */

/*
 * int_ff_int2_en
 * Free Fall interrupt source enable for int2
 */
#define INT_SOURCE7_INT_FF_INT2_EN_POS      0x07
#define INT_SOURCE7_INT_FF_INT2_EN_MASK     (0x01 << INT_SOURCE7_INT_FF_INT2_EN_POS)

/*
 * int_lowg_int2_en
 * LowG interrupt source enable for int2
 */
#define INT_SOURCE7_INT_LOWG_INT2_EN_POS      0x06
#define INT_SOURCE7_INT_LOWG_INT2_EN_MASK     (0x01 << INT_SOURCE7_INT_LOWG_INT2_EN_POS)

/*
 * int_step_det_int2_en
 * Step Detect Interrupt routed to INT2
 */
#define INT_SOURCE7_INT_STEP_DET_INT2_EN_POS      0x05
#define INT_SOURCE7_INT_STEP_DET_INT2_EN_MASK     (0x01 << INT_SOURCE7_INT_STEP_DET_INT2_EN_POS)

/*
 * int_step_cnt_ovfl_int2_en
 * Overflow of Step Counter routed to INT2
 */
#define INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_POS      0x04
#define INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_MASK     (0x01 << INT_SOURCE7_INT_STEP_CNT_OVFL_INT2_EN_POS)

/*
 * int_tilt_det_int2_en
 * Tilt detector Interrupt routed to INT2
 */
#define INT_SOURCE7_INT_TILT_DET_INT2_EN_POS      0x03
#define INT_SOURCE7_INT_TILT_DET_INT2_EN_MASK     (0x01 << INT_SOURCE7_INT_TILT_DET_INT2_EN_POS)



/*
 * INT_SOURCE8
 * Register Name : INT_SOURCE8
 */

/*
 * int_fsync_ibi_en
 * Enables the fsync event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_FSYNC_IBI_EN_POS      0x05
#define INT_SOURCE8_INT_FSYNC_IBI_EN_MASK     (0x01 << INT_SOURCE8_INT_FSYNC_IBI_EN_POS)

/*
 * int_pll_rdy_ibi_en
 * Enables the PLL ready event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_PLL_RDY_IBI_EN_POS      0x04
#define INT_SOURCE8_INT_PLL_RDY_IBI_EN_MASK     (0x01 << INT_SOURCE8_INT_PLL_RDY_IBI_EN_POS)

/*
 * int_drdy_ibi_en
 * Enables the DRDY event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_DRDY_IBI_EN_POS      0x03
#define INT_SOURCE8_INT_DRDY_IBI_EN_MASK     (0x01 << INT_SOURCE8_INT_DRDY_IBI_EN_POS)

/*
 * int_fifo_ths_ibi_en
 * Enables the FIFO Threshold event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_FIFO_THS_IBI_EN_POS      0x02
#define INT_SOURCE8_INT_FIFO_THS_IBI_EN_MASK     (0x01 << INT_SOURCE8_INT_FIFO_THS_IBI_EN_POS)

/*
 * int_fifo_full_ibi_en
 * Enables the FIFO FULL event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_FIFO_FULL_IBI_EN_POS      0x01
#define INT_SOURCE8_INT_FIFO_FULL_IBI_EN_MASK     (0x01 << INT_SOURCE8_INT_FIFO_FULL_IBI_EN_POS)

/*
 * int_agc_rdy_ibi_en
 * Enables the AGC Ready event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE8_INT_AGC_RDY_IBI_EN_POS      0x00
#define INT_SOURCE8_INT_AGC_RDY_IBI_EN_MASK     0x01



/*
 * INT_SOURCE9
 * Register Name : INT_SOURCE9
 */

/*
 * int_i3c_protocol_err_ibi_en
 * Enables the I3C Protocol Error event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_I3C_PROTOCOL_ERR_IBI_EN_POS      0x07
#define INT_SOURCE9_INT_I3C_PROTOCOL_ERR_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_I3C_PROTOCOL_ERR_IBI_EN_POS)

/*
 * int_ff_ibi_en
 * Enables the Free Fall event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_FF_IBI_EN_POS      0x06
#define INT_SOURCE9_INT_FF_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_FF_IBI_EN_POS)

/*
 * int_lowg_ibi_en
 * Enables the LowG event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_LOWG_IBI_EN_POS      0x05
#define INT_SOURCE9_INT_LOWG_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_LOWG_IBI_EN_POS)

/*
 * int_smd_ibi_en
 * Enables the Significant Motion Detector event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_SMD_IBI_EN_POS      0x04
#define INT_SOURCE9_INT_SMD_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_SMD_IBI_EN_POS)

/*
 * int_wom_z_ibi_en
 * Enables the Wake-on-motion in Z direction event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_WOM_Z_IBI_EN_POS      0x03
#define INT_SOURCE9_INT_WOM_Z_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_WOM_Z_IBI_EN_POS)

/*
 * int_wom_y_ibi_en
 * Enables the Wake-on-motion in Y direction event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_WOM_Y_IBI_EN_POS      0x02
#define INT_SOURCE9_INT_WOM_Y_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_WOM_Y_IBI_EN_POS)

/*
 * int_wom_x_ibi_en
 * Enables the Wake-on-motion in X direction event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_WOM_X_IBI_EN_POS      0x01
#define INT_SOURCE9_INT_WOM_X_IBI_EN_MASK     (0x01 << INT_SOURCE9_INT_WOM_X_IBI_EN_POS)

/*
 * int_st_done_ibi_en
 * Enables the Self test done event to trigger an IBI.
 * 0 : Disabled
 * 1 : Enabled
 */
#define INT_SOURCE9_INT_ST_DONE_IBI_EN_POS      0x00
#define INT_SOURCE9_INT_ST_DONE_IBI_EN_MASK     0x01



/*
 * INT_SOURCE10
 * Register Name : INT_SOURCE10
 */

/*
 * int_step_det_ibi_en
 * Step Detect Interrupt routed to INT2
 */
#define INT_SOURCE10_INT_STEP_DET_IBI_EN_POS      0x05
#define INT_SOURCE10_INT_STEP_DET_IBI_EN_MASK     (0x01 << INT_SOURCE10_INT_STEP_DET_IBI_EN_POS)

/*
 * int_step_cnt_ovfl_ibi_en
 * Overflow of Step Counter routed to INT2
 */
#define INT_SOURCE10_INT_STEP_CNT_OVFL_IBI_EN_POS      0x04
#define INT_SOURCE10_INT_STEP_CNT_OVFL_IBI_EN_MASK     (0x01 << INT_SOURCE10_INT_STEP_CNT_OVFL_IBI_EN_POS)

/*
 * int_tilt_det_ibi_en
 * Tilt detector Interrupt routed to INT2
 */
#define INT_SOURCE10_INT_TILT_DET_IBI_EN_POS      0x03
#define INT_SOURCE10_INT_TILT_DET_IBI_EN_MASK     (0x01 << INT_SOURCE10_INT_TILT_DET_IBI_EN_POS)



/*
 * APEX_CONFIG2
 * Register Name : APEX_CONFIG2
 */

/*
 * low_energy_amp_th_sel
 * Use to select low energy amplitude threshold.
 * Decoded by HW as ((30 + low_energy_amp_th_sel [3:0]*5 mg) * 2^25/1000).
 */
#define APEX_CONFIG2_LOW_ENERGY_AMP_TH_SEL_POS      0x04
#define APEX_CONFIG2_LOW_ENERGY_AMP_TH_SEL_MASK     (0x0f << APEX_CONFIG2_LOW_ENERGY_AMP_TH_SEL_POS)

/*
 * dmp_power_save_time_sel
 * This register field value is used for power_save_time_odrXX calculation. HW will calculate odr_xx as below.
 * power_save_time_odr25     = dmp_power_save_time_sel *100.
 * power_save_time_odr50     = dmp_power_save_time_sel *200.
 * power_save_time_odr100   = dmp_power_save_time_sel *400.
 */
#define APEX_CONFIG2_DMP_POWER_SAVE_TIME_SEL_POS      0x00
#define APEX_CONFIG2_DMP_POWER_SAVE_TIME_SEL_MASK     0x0f



/*
 * APEX_CONFIG3
 * Register Name : APEX_CONFIG3
 */

/*
 * pedo_amp_th_sel
 * Decoded by HW as ((30 +  pedo_amp_th_sel [3:0]*4 mg) * 2^25/1000)
 * {1006632, 1140850, 1275068, 1409286, 1543503, 1677721, 1811939, 1946157, 2080374, 2214592, 2348810, 2483027, 2617245, 2751463, 2885681, 3019898}
 */
#define APEX_CONFIG3_PEDO_AMP_TH_SEL_POS      0x04
#define APEX_CONFIG3_PEDO_AMP_TH_SEL_MASK     (0x0f << APEX_CONFIG3_PEDO_AMP_TH_SEL_POS)

/*
 * pedo_step_cnt_th_sel
 * Direct mapping right aligned with 12 leading 0's
 */
#define APEX_CONFIG3_PEDO_STEP_CNT_TH_SEL_POS      0x00
#define APEX_CONFIG3_PEDO_STEP_CNT_TH_SEL_MASK     0x0f



/*
 * APEX_CONFIG4
 * Register Name : APEX_CONFIG4
 */

/*
 * pedo_step_det_th_sel
 * Direct mapping right aligned with 13 leading 0's
 */
#define APEX_CONFIG4_PEDO_STEP_DET_TH_SEL_POS      0x05
#define APEX_CONFIG4_PEDO_STEP_DET_TH_SEL_MASK     (0x07 << APEX_CONFIG4_PEDO_STEP_DET_TH_SEL_POS)

/*
 * pedo_sb_timer_th_sel
 * Decoded by HW as (50 + pedo_sb_timer_th_sel*25) {50, 75, 100, 125, 150, 175, 200, 225}
 */
#define APEX_CONFIG4_PEDO_SB_TIMER_TH_SEL_POS      0x02
#define APEX_CONFIG4_PEDO_SB_TIMER_TH_SEL_MASK     (0x07 << APEX_CONFIG4_PEDO_SB_TIMER_TH_SEL_POS)

/*
 * pedo_hi_enrgy_th_sel
 * Decoded by HW on table {90, 107, 136, 159}
 */
#define APEX_CONFIG4_PEDO_HI_ENRGY_TH_SEL_POS      0x00
#define APEX_CONFIG4_PEDO_HI_ENRGY_TH_SEL_MASK     0x03



/*
 * APEX_CONFIG5
 * Register Name : APEX_CONFIG5
 */

/*
 * tilt_wait_time_sel
 * Decoded by HW on table {0, 100, 200, 300}
 */
#define APEX_CONFIG5_TILT_WAIT_TIME_SEL_POS      0x06
#define APEX_CONFIG5_TILT_WAIT_TIME_SEL_MASK     (0x03 << APEX_CONFIG5_TILT_WAIT_TIME_SEL_POS)

/*
 * lowg_peak_th_hyst_sel
 * This threshold is added to the lowg peak threshold after the initial threshold is met.
 * Bitfield is coded as follow:{31,63,94,125,156,188,219,250}
 */
#define APEX_CONFIG5_LOWG_PEAK_TH_HYST_SEL_POS      0x03
#define APEX_CONFIG5_LOWG_PEAK_TH_HYST_SEL_MASK     (0x07 << APEX_CONFIG5_LOWG_PEAK_TH_HYST_SEL_POS)

/*
 * highg_peak_th_hyst_sel
 * This threshold is added to the highg peak threshold after the initial threshold is met.
 * Bitfield is coded as follow: {31, 63, 94, 125, 156, 188, 219, 250}
 */
#define APEX_CONFIG5_HIGHG_PEAK_TH_HYST_SEL_POS      0x00
#define APEX_CONFIG5_HIGHG_PEAK_TH_HYST_SEL_MASK     0x07



/*
 * APEX_CONFIG9
 * Register Name : APEX_CONFIG9
 */

/*
 * ff_debounce_duration_sel
 * This parameter defines time during which LowG and HighG events are not considered after an HighG event. The goal is to avoid detecting bounces as free falls.
 * Bitfield is coded as follow:{0: 0ms, 1-15: 1125ms + ff_debounce_duration_sel * 125ms
 */
#define APEX_CONFIG9_FF_DEBOUNCE_DURATION_SEL_POS      0x04
#define APEX_CONFIG9_FF_DEBOUNCE_DURATION_SEL_MASK     (0x0f << APEX_CONFIG9_FF_DEBOUNCE_DURATION_SEL_POS)

/*
 * smd_sensitivity_sel
 * DMP_LUT[0x836-0x837] = (316 << smd_sensitivity_sel[2-0]) - 1
 * // If -1 appears to be an issue, DMP sw could handle it.
 */
#define APEX_CONFIG9_SMD_SENSITIVITY_SEL_POS      0x01
#define APEX_CONFIG9_SMD_SENSITIVITY_SEL_MASK     (0x07 << APEX_CONFIG9_SMD_SENSITIVITY_SEL_POS)

/*
 * sensitivity_mode
 * Direct mapping right aligned with 15 leading 0's
 */
#define APEX_CONFIG9_SENSITIVITY_MODE_POS      0x00
#define APEX_CONFIG9_SENSITIVITY_MODE_MASK     0x01



/*
 * APEX_CONFIG10
 * Register Name : APEX_CONFIG10
 */

/*
 * lowg_peak_th_sel
 * This parameter defines the threshold for accel values below which algo considers it enters low-g.
 * Bitfield is coded as follow: {31, 63, 94, 125, 156, 188, 219, 250, 281, 313, 344, 375, 406, 438, 469, 500, 594, 625, 656, 688, 719, 750, 844, 875, 906, 938, 969, 1000}
 */
#define APEX_CONFIG10_LOWG_PEAK_TH_SEL_POS      0x03
#define APEX_CONFIG10_LOWG_PEAK_TH_SEL_MASK     (0x1f << APEX_CONFIG10_LOWG_PEAK_TH_SEL_POS)

/*
 * lowg_time_th_sel
 * This parameter defines the number of samples device should stay in low-g before triggering interrupt.
 * It is coded as: number_of_samples = lowg_time_th_sel + 1
 */
#define APEX_CONFIG10_LOWG_TIME_TH_SEL_POS      0x00
#define APEX_CONFIG10_LOWG_TIME_TH_SEL_MASK     0x07



/*
 * APEX_CONFIG11
 * Register Name : APEX_CONFIG11
 */

/*
 * highg_peak_th_sel
 * This parameter defines the threshold for accel values above which algo considers it enters high-g.
 * Threshold = (highg_peak_th_sel+1) * 250 mGee
 */
#define APEX_CONFIG11_HIGHG_PEAK_TH_SEL_POS      0x03
#define APEX_CONFIG11_HIGHG_PEAK_TH_SEL_MASK     (0x1f << APEX_CONFIG11_HIGHG_PEAK_TH_SEL_POS)

/*
 * highg_time_th_sel
 * This parameter defines the number of samples device should stay in high-g before triggering interrupt.
 * It is coded as: number_of_samples = highg_time_th_sel + 1
 */
#define APEX_CONFIG11_HIGHG_TIME_TH_SEL_POS      0x00
#define APEX_CONFIG11_HIGHG_TIME_TH_SEL_MASK     0x07



/*
 * ACCEL_WOM_X_THR
 * Register Name : ACCEL_WOM_X_THR
 */

/*
 * wom_x_th
 * WoM thresholds are expressed in fixed "mg"independently of the selected full-scale (format <U,8,0: , range [0g : 1g], resolution 1g/256=~4mg).
 */
#define ACCEL_WOM_X_THR_WOM_X_TH_POS      0x00
#define ACCEL_WOM_X_THR_WOM_X_TH_MASK     0xff



/*
 * ACCEL_WOM_Y_THR
 * Register Name : ACCEL_WOM_Y_THR
 */

/*
 * wom_y_th
 * WoM thresholds are expressed in fixed "mg" independently of the selected full-scale (format <U,8,0: , range [0g : 1g], resolution 1g/256=~4mg).
 */
#define ACCEL_WOM_Y_THR_WOM_Y_TH_POS      0x00
#define ACCEL_WOM_Y_THR_WOM_Y_TH_MASK     0xff



/*
 * ACCEL_WOM_Z_THR
 * Register Name : ACCEL_WOM_Z_THR
 */

/*
 * wom_z_th
 * WoM thresholds are expressed in fixed "mg" independently of the selected full-scale (format <U,8,0: , range [0g : 1g], resolution 1g/256=~4mg).
 */
#define ACCEL_WOM_Z_THR_WOM_Z_TH_POS      0x00
#define ACCEL_WOM_Z_THR_WOM_Z_TH_MASK     0xff



/*
 * GOS_USER0
 * Register Name : GOS_USER0
 */

/*
 * gyro_x_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER0_GYRO_X_OFFUSER_POS      0x00
#define GOS_USER0_GYRO_X_OFFUSER_MASK     0xff



/*
 * GOS_USER1
 * Register Name : GOS_USER1
 */

/*
 * gyro_x_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER1_GYRO_X_OFFUSER_POS      0x00
#define GOS_USER1_GYRO_X_OFFUSER_MASK     0x0f

/*
 * gyro_y_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER1_GYRO_Y_OFFUSER_POS      0x04
#define GOS_USER1_GYRO_Y_OFFUSER_MASK     (0x0f << GOS_USER1_GYRO_Y_OFFUSER_POS)



/*
 * GOS_USER2
 * Register Name : GOS_USER2
 */

/*
 * gyro_y_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER2_GYRO_Y_OFFUSER_POS      0x00
#define GOS_USER2_GYRO_Y_OFFUSER_MASK     0xff



/*
 * GOS_USER3
 * Register Name : GOS_USER3
 */

/*
 * gyro_z_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER3_GYRO_Z_OFFUSER_POS      0x00
#define GOS_USER3_GYRO_Z_OFFUSER_MASK     0xff



/*
 * GOS_USER4
 * Register Name : GOS_USER4
 */

/*
 * gyro_z_offuser
 * Gyro offset programmed by user. Max value is +/-64 dps, resolution is 1/32 dps
 */
#define GOS_USER4_GYRO_Z_OFFUSER_POS      0x00
#define GOS_USER4_GYRO_Z_OFFUSER_MASK     0x0f

/*
 * accel_x_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER4_ACCEL_X_OFFUSER_POS      0x04
#define GOS_USER4_ACCEL_X_OFFUSER_MASK     (0x0f << GOS_USER4_ACCEL_X_OFFUSER_POS)



/*
 * GOS_USER5
 * Register Name : GOS_USER5
 */

/*
 * accel_x_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER5_ACCEL_X_OFFUSER_POS      0x00
#define GOS_USER5_ACCEL_X_OFFUSER_MASK     0xff



/*
 * GOS_USER6
 * Register Name : GOS_USER6
 */

/*
 * accel_y_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER6_ACCEL_Y_OFFUSER_POS      0x00
#define GOS_USER6_ACCEL_Y_OFFUSER_MASK     0xff



/*
 * GOS_USER7
 * Register Name : GOS_USER7
 */

/*
 * accel_y_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER7_ACCEL_Y_OFFUSER_POS      0x00
#define GOS_USER7_ACCEL_Y_OFFUSER_MASK     0x0f

/*
 * accel_z_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER7_ACCEL_Z_OFFUSER_POS      0x04
#define GOS_USER7_ACCEL_Z_OFFUSER_MASK     (0x0f << GOS_USER7_ACCEL_Z_OFFUSER_POS)



/*
 * GOS_USER8
 * Register Name : GOS_USER8
 */

/*
 * accel_z_offuser
 * Accel offset programmed by user. Max value is +/-1 gee, resolution is 0.5 mgee
 */
#define GOS_USER8_ACCEL_Z_OFFUSER_POS      0x00
#define GOS_USER8_ACCEL_Z_OFFUSER_MASK     0xff



/*
 * ST_STATUS1
 * Register Name : ST_STATUS1
 */

/*
 * dmp_accel_st_pass
 * 1: Accel self test passed for all the 3 axes
 */
#define ST_STATUS1_DMP_ACCEL_ST_PASS_POS      0x05
#define ST_STATUS1_DMP_ACCEL_ST_PASS_MASK     (0x01 << ST_STATUS1_DMP_ACCEL_ST_PASS_POS)

/*
 * dmp_accel_st_done
 * 1: Accel self test done for all the 3 axes
 */
#define ST_STATUS1_DMP_ACCEL_ST_DONE_POS      0x04
#define ST_STATUS1_DMP_ACCEL_ST_DONE_MASK     (0x01 << ST_STATUS1_DMP_ACCEL_ST_DONE_POS)

/*
 * dmp_az_st_pass
 * 1: Accel Z-axis self test passed
 */
#define ST_STATUS1_DMP_AZ_ST_PASS_POS      0x03
#define ST_STATUS1_DMP_AZ_ST_PASS_MASK     (0x01 << ST_STATUS1_DMP_AZ_ST_PASS_POS)

/*
 * dmp_ay_st_pass
 * 1: Accel Y-axis self test passed
 */
#define ST_STATUS1_DMP_AY_ST_PASS_POS      0x02
#define ST_STATUS1_DMP_AY_ST_PASS_MASK     (0x01 << ST_STATUS1_DMP_AY_ST_PASS_POS)

/*
 * dmp_ax_st_pass
 * 1: Accel X-axis self test passed
 */
#define ST_STATUS1_DMP_AX_ST_PASS_POS      0x01
#define ST_STATUS1_DMP_AX_ST_PASS_MASK     (0x01 << ST_STATUS1_DMP_AX_ST_PASS_POS)



/*
 * ST_STATUS2
 * Register Name : ST_STATUS2
 */

/*
 * dmp_st_incomplete
 * 1: self test is incomplete.
 */
#define ST_STATUS2_DMP_ST_INCOMPLETE_POS      0x06
#define ST_STATUS2_DMP_ST_INCOMPLETE_MASK     (0x01 << ST_STATUS2_DMP_ST_INCOMPLETE_POS)

/*
 * dmp_gyro_st_pass
 * 1: Gyro self test passed for all the 3 axes
 */
#define ST_STATUS2_DMP_GYRO_ST_PASS_POS      0x05
#define ST_STATUS2_DMP_GYRO_ST_PASS_MASK     (0x01 << ST_STATUS2_DMP_GYRO_ST_PASS_POS)

/*
 * dmp_gyro_st_done
 * 1: Gyro self test done for all the 3 axes
 */
#define ST_STATUS2_DMP_GYRO_ST_DONE_POS      0x04
#define ST_STATUS2_DMP_GYRO_ST_DONE_MASK     (0x01 << ST_STATUS2_DMP_GYRO_ST_DONE_POS)

/*
 * dmp_gz_st_pass
 * 1: Gyro Z-axis self test passed
 */
#define ST_STATUS2_DMP_GZ_ST_PASS_POS      0x03
#define ST_STATUS2_DMP_GZ_ST_PASS_MASK     (0x01 << ST_STATUS2_DMP_GZ_ST_PASS_POS)

/*
 * dmp_gy_st_pass
 * 1: Gyro Y-axis self test passed
 */
#define ST_STATUS2_DMP_GY_ST_PASS_POS      0x02
#define ST_STATUS2_DMP_GY_ST_PASS_MASK     (0x01 << ST_STATUS2_DMP_GY_ST_PASS_POS)

/*
 * dmp_gx_st_pass
 * 1: Gyro X-axis self test passed
 */
#define ST_STATUS2_DMP_GX_ST_PASS_POS      0x01
#define ST_STATUS2_DMP_GX_ST_PASS_MASK     (0x01 << ST_STATUS2_DMP_GX_ST_PASS_POS)



/*
 * PSEQ_SPARE_TRIM
 * Register Name : PSEQ_SPARE_TRIM
 */

/*
 * pseq_spare_trim
 * [7:4] Spare OTP bits
 * [3:0]: used to control and configure the FIFO decimation feature. If bit 3 is 0 the feature is disabled and all the generated samples are pushed in the FIFO (legacy behavior). If bit 3 is 1 the feature is enabled and only 1 sampe every 2^N is pushed in the FIFO a per table below.
 * 0--- : disabled
 * 1000 : decimation by 2;  1 sample every 2
 * 1001 : decimation by 4;  1 sample every 4
 * 1010 : decimation by 8;  1 sample every 8
 * 1011 : decimation by 16; 1 sample every 16
 * 1100 : decimation by 32; 1 sample every 32
 * 1101 : decimation by 64; 1 sample every 64
 * 1110 : decimation by 128;1 sample every 128
 * 1111 : decimation by 256;1 sample every 256
 */
#define PSEQ_SPARE_TRIM_PSEQ_SPARE_TRIM_POS      0x00
#define PSEQ_SPARE_TRIM_PSEQ_SPARE_TRIM_MASK     0xff



/*
 * APEX_CONFIG12
 * Register Name : APEX_CONFIG12
 */

/*
 * ff_max_duration_sel
 * This parameter defines the maximum freefall length [cm] that this algo should report. Longer freefalls are ignored.
 * Bitfield is coded as follow: {102, 120, 139, 159, 181, 204, 228, 254, 281, 310, 339, 371, 403, 438, 473, 510}
 */
#define APEX_CONFIG12_FF_MAX_DURATION_SEL_POS      0x04
#define APEX_CONFIG12_FF_MAX_DURATION_SEL_MASK     (0x0f << APEX_CONFIG12_FF_MAX_DURATION_SEL_POS)

/*
 * ff_min_duration_sel
 * This parameter defines the minimum freefall length [cm] that this algo should report. Smaller freefalls are ignored.
 * Bitfield is coded as follow: {10, 12, 13, 16, 18, 20, 23, 25, 28, 31, 34, 38, 41, 45, 48, 52}
 */
#define APEX_CONFIG12_FF_MIN_DURATION_SEL_POS      0x00
#define APEX_CONFIG12_FF_MIN_DURATION_SEL_MASK     0x0f


/* ---------------------------------------------------------------------------
 * register MMEM_TOP
 * ---------------------------------------------------------------------------*/

/*
 * XA_ST_DATA
 * Register Name : XA_ST_DATA
 */

/*
 * xa_st_data
 * Accel X-axis self test data converted to 8 bit code.
 */
#define XA_ST_DATA_XA_ST_DATA_POS      0x00
#define XA_ST_DATA_XA_ST_DATA_MASK     0xff



/*
 * YA_ST_DATA
 * Register Name : YA_ST_DATA
 */

/*
 * ya_st_data
 * Accel Y-axis self test data converted to 8 bit code.
 */
#define YA_ST_DATA_YA_ST_DATA_POS      0x00
#define YA_ST_DATA_YA_ST_DATA_MASK     0xff



/*
 * ZA_ST_DATA
 * Register Name : ZA_ST_DATA
 */

/*
 * za_st_data
 * Accel Z-axis self test data converted to 8 bit code.
 */
#define ZA_ST_DATA_ZA_ST_DATA_POS      0x00
#define ZA_ST_DATA_ZA_ST_DATA_MASK     0xff



/*
 * XG_ST_DATA
 * Register Name : XG_ST_DATA
 */

/*
 * xg_st_data
 * Gyro X-axis self test data converted to 8 bit code.
 */
#define XG_ST_DATA_XG_ST_DATA_POS      0x00
#define XG_ST_DATA_XG_ST_DATA_MASK     0xff



/*
 * YG_ST_DATA
 * Register Name : YG_ST_DATA
 */

/*
 * yg_st_data
 * Gyro Y-axis self test data converted to 8 bit code.
 */
#define YG_ST_DATA_YG_ST_DATA_POS      0x00
#define YG_ST_DATA_YG_ST_DATA_MASK     0xff



/*
 * ZG_ST_DATA
 * Register Name : ZG_ST_DATA
 */

/*
 * zg_st_data
 * Gyro Z-axis self test data converted to 8 bit code.
 */
#define ZG_ST_DATA_ZG_ST_DATA_POS      0x00
#define ZG_ST_DATA_ZG_ST_DATA_MASK     0xff


/* ---------------------------------------------------------------------------
 * register MREG_OTP
 * ---------------------------------------------------------------------------*/

/*
 * OTP_CTRL7
 * Register Name : OTP_CTRL7
 */

/*
 * otp_reload
 * 1: to trigger OTP copy operation. This bit is cleared to 0 after OTP copy is done.
 *
 * With otp_copy_mode[1:0] = 2'b01, it takes 280us to complete the OTP reloading operatioin.
 * With otp_copy_mode[1:0] = 2'b11, it takes 20us to complete the OTP reloading operation.
 */
#define OTP_CTRL7_OTP_RELOAD_POS      0x03
#define OTP_CTRL7_OTP_RELOAD_MASK     (0x01 << OTP_CTRL7_OTP_RELOAD_POS)

/*
 * otp_pwr_down
 * 0: to power up OTP for read/write operation.
 * 1: to power down OTP to save power.
 * This bit is automatically set to 1 when OTP copy operation is complete.
 */
#define OTP_CTRL7_OTP_PWR_DOWN_POS      0x01
#define OTP_CTRL7_OTP_PWR_DOWN_MASK     (0x01 << OTP_CTRL7_OTP_PWR_DOWN_POS)
/* ---------------------------------------------------------------------------
 * register MREG_FPGA
 * ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
 * register ROM
 * ---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /*#ifndef _INV_IMU_REGMAP_H_*/
