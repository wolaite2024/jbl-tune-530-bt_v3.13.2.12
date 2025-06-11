#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include "inv_imu_transport.h"
#include "inv_imu_regmap.h"
#include "inv_imu_driver.h"

#include "InvError.h"
#include "trace.h"


/* Function definition */
static int write_sreg(struct inv_imu_device *s, uint8_t reg, uint32_t len, const uint8_t *buf);
static int read_sreg(struct inv_imu_device *s, uint8_t reg, uint32_t len, uint8_t *buf);
static int write_mclk_reg(struct inv_imu_device *s, uint16_t regaddr, uint8_t wr_cnt,
                          const uint8_t  *buf);
static int read_mclk_reg(struct inv_imu_device *s, uint16_t regaddr, uint8_t rd_cnt, uint8_t *buf);


int inv_imu_init_transport(struct inv_imu_device *s)
{
    int status = 0;
    struct inv_imu_transport *t = (struct inv_imu_transport *)s;

    status |= read_sreg(s, (uint8_t)INTF_CONFIG1,           1, &(t->register_cache.intf_cfg_1_reg));
    status |= read_sreg(s, (uint8_t)PWR_MGMT_0,             1, &(t->register_cache.pwr_mngt_0_reg));
    status |= read_sreg(s, (uint8_t)GYRO_CONFIG0,           1, &(t->register_cache.gyro_cfg_0_reg));
    status |= read_sreg(s, (uint8_t)ACCEL_CONFIG0,          1, &(t->register_cache.accel_cfg_0_reg));
    status |= read_sreg(s, (uint8_t)INT_SOURCE0,            1, &(t->register_cache.int_source_0_reg));
    status |= read_sreg(s, (uint8_t)INT_SOURCE1,            1, &(t->register_cache.int_source_1_reg));
    status |= read_sreg(s, (uint8_t)INT_SOURCE3,            1, &(t->register_cache.int_source_3_reg));
    status |= read_sreg(s, (uint8_t)INT_SOURCE4,            1, &(t->register_cache.int_source_4_reg));

    status |= read_mclk_reg(s, (TMST_CONFIG1_MREG_TOP1 & 0xFFFF),  1,
                            &(t->register_cache.tmst_cfg_1_reg));
    status |= read_mclk_reg(s, (INT_SOURCE6_MREG_TOP1  & 0xFFFF),  1,
                            &(t->register_cache.int_source_6_reg));
    status |= read_mclk_reg(s, (INT_SOURCE7_MREG_TOP1  & 0xFFFF),  1,
                            &(t->register_cache.int_source_7_reg));
    status |= read_mclk_reg(s, (INT_SOURCE8_MREG_TOP1  & 0xFFFF),  1,
                            &(t->register_cache.int_source_8_reg));
    status |= read_mclk_reg(s, (INT_SOURCE9_MREG_TOP1  & 0xFFFF),  1,
                            &(t->register_cache.int_source_9_reg));
    status |= read_mclk_reg(s, (INT_SOURCE10_MREG_TOP1 & 0xFFFF),  1,
                            &(t->register_cache.int_source_10_reg));

    t->need_mclk_cnt = 0;

    return status;
}

int inv_imu_read_reg(struct inv_imu_device *s, uint32_t reg, uint32_t len, uint8_t *buf)
{
    int rc = 0;

    for (uint32_t i = 0; i < len ; i++)
    {
        if (!(reg & 0x10000))
        {
            rc |= read_mclk_reg(s, ((reg + i) & 0xFFFF), 1, &buf[i]);
        }
        else
        {
            rc |= read_sreg(s, (uint8_t)reg + i, len - i, &buf[i]);
            break;
        }
    }

    return rc;
}

int inv_imu_write_reg(struct inv_imu_device *s, uint32_t reg, uint32_t len, const uint8_t *buf)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (!(reg & 0x10000))
        {
            write_mclk_reg(s, ((reg + i) & 0xFFFF), 1, &buf[i]);
        }
    }

    if (reg & 0x10000)
    {
        write_sreg(s, (uint8_t)reg, len, buf);
    }

    return 0;
}

#define ENABLE_ALWAYS_ON_MCLK 1

int inv_imu_switch_on_mclk(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t data;
    struct inv_imu_transport *t = (struct inv_imu_transport *)s;
#if (ENABLE_ALWAYS_ON_MCLK == 1)
    if (t->need_mclk_cnt > 0)
    {
        return 0;
    }
#endif
    /* set IDLE bit only if it is not set yet */
    if (t->need_mclk_cnt == 0)
    {
        status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
        data |= PWR_MGMT_0_IDLE_MASK;
        status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &data);

        if (status)
        {
            return status;
        }
        /* Check if MCLK is ready */
        uint32_t trial_times = 0;
        do
        {
            status = inv_imu_read_reg(s, MISC_1, 1, &data); // |=
            trial_times++;

            //if (trial_times > 0xF)
            //    break;
        }
        while ((status != 0) || !(data & MISC_1_MCLK_RDY_MASK));
    }
    else
    {
        /* Make sure it is already on */
        status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
        if (0 == (data &= PWR_MGMT_0_IDLE_MASK))
        {
            status |= INV_ERROR;
        }
    }

    /* Increment the counter to keep track of number of MCLK requesters */
    t->need_mclk_cnt++;
    APP_PRINT_INFO1("inv_imu_switch_on_mclk, need count: 0x%x", t->need_mclk_cnt);
    return status;
}

int inv_imu_switch_off_mclk(struct inv_imu_device *s)
{
    int status = 0;
    uint8_t data;
    struct inv_imu_transport *t = (struct inv_imu_transport *)s;
#if (ENABLE_ALWAYS_ON_MCLK == 1)
    if (t->need_mclk_cnt > 0)
    {
        return 0;
    }
#endif
    /* Reset the IDLE but only if there is one requester left */
    if (t->need_mclk_cnt == 1)
    {
        status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
        data &= ~PWR_MGMT_0_IDLE_MASK;
        status |= inv_imu_write_reg(s, PWR_MGMT_0, 1, &data);
    }
    else
    {
        /* Make sure it is still on */
        status |= inv_imu_read_reg(s, PWR_MGMT_0, 1, &data);
        if (0 == (data &= PWR_MGMT_0_IDLE_MASK))
        {
            status |= INV_ERROR;
        }
    }

    /* Decrement the counter */
    t->need_mclk_cnt--;

    return status;
}

static int read_sreg(struct inv_imu_device *s, uint8_t reg, uint32_t len, uint8_t *buf)
{
    /*First field of struct inv_imu_device is assumed to be a struct inv_imu_serif object.
    So let's cast s to struct inv_imu_serif and ignore the rest of struct inv_imu_device*/
    struct inv_imu_serif *serif = (struct inv_imu_serif *)s;

    if (len > serif->max_read)
    {
        return INV_ERROR_SIZE;
    }

    if (serif->read_reg(serif, reg, buf, len) != 0)
    {
        return INV_ERROR_TRANSPORT;
    }

    return 0;
}

static int write_sreg(struct inv_imu_device *s, uint8_t reg, uint32_t len, const uint8_t *buf)
{
    // First field of struct inv_imu_device is assumed to be a struct inv_imu_serif object.
    // So let's cast s to struct inv_imu_serif and ignore the rest of struct inv_imu_device.
    struct inv_imu_serif *serif = (struct inv_imu_serif *)s;

    if (len > serif->max_write)
    {
        return INV_ERROR_SIZE;
    }

    if (serif->write_reg(serif, reg, buf, len) != 0)
    {
        return INV_ERROR_TRANSPORT;
    }

    return 0;
}

static int read_mclk_reg(struct inv_imu_device *s, uint16_t regaddr, uint8_t rd_cnt, uint8_t *buf)
{
    uint8_t data;
    uint8_t blk_sel = (regaddr & 0xFF00) >> 8;
    int status = 0;

    APP_PRINT_INFO2("read_mclk_reg, regaddr: 0x%x, rd_cnt: 0x%x", regaddr, rd_cnt);

    // Have IMU not in IDLE mode to access MCLK domain
    status |= inv_imu_switch_on_mclk(s);

    // optimize by changing BLK_SEL only if not NULL
    if (blk_sel)
    {
        status |= write_sreg(s, (uint8_t)BLK_SEL_R & 0xff, 1, &blk_sel);
    }

    data = (regaddr & 0x00FF);
    status |= write_sreg(s, (uint8_t)MADDR_R, 1, &data);
    // wait 16 MCLK (4MHz) clock cycles
    inv_imu_sleep_us(4);
    status |= read_sreg(s, (uint8_t)M_R, rd_cnt, buf);
    // wait 16 MCLK (4MHz) clock cycles
    inv_imu_sleep_us(4);

    if (blk_sel)
    {
        data = 0;
        status |= write_sreg(s, (uint8_t)BLK_SEL_R, 1, &data);
    }

    // switch OFF MCLK if needed
    status |= inv_imu_switch_off_mclk(s);

    return status;
}

static int write_mclk_reg(struct inv_imu_device *s, uint16_t regaddr, uint8_t wr_cnt,
                          const uint8_t  *buf)
{
    uint8_t data;
    uint8_t blk_sel = (regaddr & 0xFF00) >> 8;
    int status = 0;

    // Have IMU not in IDLE mode to access MCLK domain
    status |= inv_imu_switch_on_mclk(s);

    // optimize by changing BLK_SEL only if not NULL
    if (blk_sel)
    {
        status |= write_sreg(s, (uint8_t)BLK_SEL_W, 1, &blk_sel);
    }

    data = (regaddr & 0x00FF);
    status |= write_sreg(s, (uint8_t)MADDR_W, 1, &data);
    for (uint8_t i = 0; i < wr_cnt; i++)
    {
        status |= write_sreg(s, (uint8_t)M_W, 1, &buf[i]);
        // wait 16 MCLK (4MHz) clock cycles
        inv_imu_sleep_us(4);
    }

    if (blk_sel)
    {
        data = 0;
        status = write_sreg(s, (uint8_t)BLK_SEL_W, 1, &data);
    }

    status |= inv_imu_switch_off_mclk(s);

    return status;
}
#endif
