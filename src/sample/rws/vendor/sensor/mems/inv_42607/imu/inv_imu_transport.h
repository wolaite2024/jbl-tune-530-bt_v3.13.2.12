#ifndef _INV_IMU_TRANSPORT_H_
#define _INV_IMU_TRANSPORT_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* forward declaration */
struct inv_imu_device;


/** @brief enumeration  of serial interfaces available on IMU */
typedef enum
{
    UI_I2C,
    UI_SPI4,
    UI_SPI3
} SERIAL_IF_TYPE_t;

/** @brief basesensor serial interface
 */
struct inv_imu_serif
{
    void *context;
    int (*read_reg)(struct inv_imu_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len);
    int (*write_reg)(struct inv_imu_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len);
    int (*configure)(struct inv_imu_serif *serif);
    uint32_t max_read;
    uint32_t max_write;
    SERIAL_IF_TYPE_t serif_type;
};

/** @brief transport interface
 */
struct inv_imu_transport
{
    /**< Warning : this field MUST be the first one of struct inv_imu_transport */
    struct inv_imu_serif serif;

    /*Contains mirrored values of some IP registers */
    struct register_cache
    {
        uint8_t intf_cfg_1_reg;      /**< INTF_CONFIG1, Bank: 0 */
        uint8_t pwr_mngt_0_reg;      /**< PWR_MGMT_0, Bank: 0 */
        uint8_t gyro_cfg_0_reg;      /**< GYRO_CONFIG0, Bank: 0 */
        uint8_t accel_cfg_0_reg;     /**< ACCEL_CONFIG0, Bank: 0 */
        uint8_t tmst_cfg_1_reg;      /**< TMST_CONFIG1, Bank: MREG_TOP1 */
        uint8_t int_source_0_reg;    /**< INT_SOURCE0, Bank: 0 */
        uint8_t int_source_1_reg;    /**< INT_SOURCE1, Bank: 0 */
        uint8_t int_source_3_reg;    /**< INT_SOURCE3, Bank: 0 */
        uint8_t int_source_4_reg;    /**< INT_SOURCE4, Bank: 0 */
        uint8_t int_source_6_reg;    /**< INT_SOURCE6, Bank: MREG_TOP1 */
        uint8_t int_source_7_reg;    /**< INT_SOURCE7, Bank: MREG_TOP1 */
        uint8_t int_source_8_reg;    /**< INT_SOURCE8, Bank: MREG_TOP1 */
        uint8_t int_source_9_reg;    /**< INT_SOURCE9, Bank: MREG_TOP1 */
        uint8_t int_source_10_reg;   /**< INT_SOURCE10, Bank: MREG_TOP1 */
    } register_cache;

    /**< Store mostly used register values on SRAM.
    *  MISC_1, INT_STATUS and INTF_CONFIG13 registers
    *  are read before the cache has a chance to be initialized.
    *  Therefore, these registers shall never be added to the cache
    */

    uint8_t need_mclk_cnt;           /**< internal counter to keep track of everyone that needs MCLK */

};

/** @brief Init cache variable.
 * @return            0 in case of success, -1 for any error
 */
int inv_imu_init_transport(struct inv_imu_device *s);

/** @brief Reads data from a register on IMU.
 * @param[in] reg    register address to be read
 * @param[in] len    number of byte to be read
 * @param[out] buf   output data from the register
 * @return            0 in case of success, -1 for any error
 */
int inv_imu_read_reg(struct inv_imu_device *s, uint32_t reg, uint32_t len, uint8_t *buf);

/** @brief Writes data to a register on IMU.
 * @param[in] reg    register address to be written
 * @param[in] len    number of byte to be written
 * @param[in] buf    input data to write
 * @return            0 in case of success, -1 for any error
 */
int inv_imu_write_reg(struct inv_imu_device *s, uint32_t reg, uint32_t len, const uint8_t *buf);

/** @brief Enable MCLK so that MREG are clocked and system beyond SOI can be safely accessed
 * @return            0 in case of success, -1 for any error
 */
int inv_imu_switch_on_mclk(struct inv_imu_device *s);

/** @brief Disable MCLK so that MREG are not clocked anymore, hence reducing power consumption
 * @return            0 in case of success, -1 for any error
 */
int inv_imu_switch_off_mclk(struct inv_imu_device *s);

#ifdef __cplusplus
}
#endif

#endif /* _INV_IMU_TRANSPORT_H_ */
