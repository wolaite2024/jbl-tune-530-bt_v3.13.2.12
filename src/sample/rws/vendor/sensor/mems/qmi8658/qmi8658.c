#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
#include "os_sched.h"
#include "trace.h"
#include "platform_utils.h"

#include "qmi8658.h"

#define QMI8658_SLAVE_ADDR_L            0x6a
#define QMI8658_SLAVE_ADDR_H            0x6b
#define qmi8658_printf                  printf

//#define QMI8658_UINT_MG_DPS

enum
{
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,

    AXIS_TOTAL
};

typedef struct
{
    short               sign[AXIS_TOTAL];
    unsigned short      map[AXIS_TOTAL];
} qst_imu_layout;

static unsigned short acc_lsb_div = 0;
static unsigned short gyro_lsb_div = 0;
static unsigned short ae_q_lsb_div = (1 << 14);
static unsigned short ae_v_lsb_div = (1 << 10);
static unsigned int imu_timestamp = 0;
struct Qmi8658Config qmi8658_config;
static uint8_t qmi8658_slave_addr = QMI8658_SLAVE_ADDR_L;
static T_QMI8658_IF qmi8658_if;

static unsigned char qmi8658_odr_sel = 4;
const enum Qmi8658_AccOdr acc_odr_tbl[] =
{
    Qmi8658AccOdr_31_25Hz,
    Qmi8658AccOdr_62_5Hz,
    Qmi8658AccOdr_125Hz,
    Qmi8658AccOdr_250Hz,
    Qmi8658AccOdr_500Hz,
    Qmi8658AccOdr_1000Hz
};

const enum Qmi8658_GyrOdr gyr_odr_tbl[] =
{
    Qmi8658GyrOdr_31_25Hz,
    Qmi8658GyrOdr_62_5Hz,
    Qmi8658GyrOdr_125Hz,
    Qmi8658GyrOdr_250Hz,
    Qmi8658GyrOdr_500Hz,
    Qmi8658GyrOdr_1000Hz
};

void Qmi8658_delay(int ms)
{
    platform_delay_us(ms * 1000);
#if 0
    int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 1000; j++)
        {
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();

            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();

            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
        }
    }
#endif
}

void qmi8658_delay_ms(uint8_t ms)
{
    platform_delay_us(ms * 1000);
#if 0
    int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 2000; j++)
        {
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();

            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();

            __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP();
        }
    }
#endif
}

unsigned char Qmi8658_write_reg(unsigned char reg, unsigned char value)
{
    unsigned char ret = 0;
    unsigned int retry = 0;

    while ((!ret) && (retry++ < 5))
    {
#if defined(QST_USE_SPI)
        ret = qst_imu_spi_write(reg, value);
#elif defined(QST_USE_SW_I2C)
        // ret = qst_sw_write_reg(qmi8658_slave_addr<<1, reg, value);
        ret = qmi8658_if.qst_sw_write_reg(reg, value);
#else
        ret = mx_i2c1_write(qmi8658_slave_addr << 1, reg, value);
#endif
    }
    return ret;
}

#if 0
unsigned char Qmi8658_write_regs(unsigned char reg, unsigned char *value, unsigned char len)
{
    unsigned char ret = 0;
    unsigned int retry = 0;

    while ((!ret) && (retry++ < 5))
    {
#if defined(QST_USE_SPI)
        ret = qst_imu_spi_write_bytes(reg, value, len);
#elif defined(QST_USE_SW_I2C)
        // ret = qst_sw_writeregs(qmi8658_slave_addr<<1, reg, value, len);
        ret = qmi8658_if.qst_sw_write_regs(reg, value, len);
#else
        ret = I2C_BufferRead(qmi8658_slave_addr << 1, reg, value, len);
#endif
    }
    return ret;
}
#endif

unsigned char Qmi8658_read_reg(unsigned char reg, unsigned char *buf, unsigned short len)
{
    unsigned char ret = 0;
    unsigned int retry = 0;

    while ((!ret) && (retry++ < 5))
    {
#if defined(QST_USE_SPI)
        ret = qst_8658_spi_read(reg, buf, len);
#elif defined(QST_USE_SW_I2C)
        ret = qmi8658_if.qst_sw_read_reg(reg, buf, len);
#else
        ret = mx_i2c1_read(qmi8658_slave_addr << 1, reg, buf, len);
#endif
    }
    return ret;
}

#if 0
static qst_imu_layout imu_map;

void Qmi8658_set_layout(short layout)
{
    if (layout == 0)
    {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 1)
    {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 2)
    {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 3)
    {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 4)
    {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 5)
    {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 6)
    {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else if (layout == 7)
    {
        imu_map.sign[AXIS_X] = -1;
        imu_map.sign[AXIS_Y] = -1;
        imu_map.sign[AXIS_Z] = -1;
        imu_map.map[AXIS_X] = AXIS_Y;
        imu_map.map[AXIS_Y] = AXIS_X;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
    else
    {
        imu_map.sign[AXIS_X] = 1;
        imu_map.sign[AXIS_Y] = 1;
        imu_map.sign[AXIS_Z] = 1;
        imu_map.map[AXIS_X] = AXIS_X;
        imu_map.map[AXIS_Y] = AXIS_Y;
        imu_map.map[AXIS_Z] = AXIS_Z;
    }
}
#endif

void Qmi8658_config_acc(enum Qmi8658_AccRange range, enum Qmi8658_AccOdr odr,
                        enum Qmi8658_LpfConfig lpfEnable, enum Qmi8658_StConfig stEnable)
{
    unsigned char ctl_dada;

    switch (range)
    {
    case Qmi8658AccRange_2g:
        acc_lsb_div = (1 << 14);
        break;
    case Qmi8658AccRange_4g:
        acc_lsb_div = (1 << 13);
        break;
    case Qmi8658AccRange_8g:
        acc_lsb_div = (1 << 12);
        break;
    case Qmi8658AccRange_16g:
        acc_lsb_div = (1 << 11);
        break;
    default:
        range = Qmi8658AccRange_8g;
        acc_lsb_div = (1 << 12);
    }
    if (stEnable == Qmi8658St_Enable)
    {
        ctl_dada = (unsigned char)range | (unsigned char)odr | 0x80;
    }
    else
    {
        ctl_dada = (unsigned char)range | (unsigned char)odr;
    }

    Qmi8658_write_reg(Qmi8658Register_Ctrl2, ctl_dada);
// set LPF & HPF
    Qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada, 1);
    ctl_dada &= 0xf0;
    if (lpfEnable == Qmi8658Lpf_Enable)
    {
        ctl_dada |= A_LSP_MODE_3;
        ctl_dada |= 0x01;
    }
    else
    {
        ctl_dada &= ~0x01;
    }
    ctl_dada = 0x00;
    Qmi8658_write_reg(Qmi8658Register_Ctrl5, ctl_dada);
// set LPF & HPF
}

void Qmi8658_config_gyro(enum Qmi8658_GyrRange range, enum Qmi8658_GyrOdr odr,
                         enum Qmi8658_LpfConfig lpfEnable, enum Qmi8658_StConfig stEnable)
{
    // Set the CTRL3 register to configure dynamic range and ODR
    unsigned char ctl_dada;

    // Store the scale factor for use when processing raw data
    switch (range)
    {
    case Qmi8658GyrRange_16dps:
        gyro_lsb_div = 2048;
        break;
    case Qmi8658GyrRange_32dps:
        gyro_lsb_div = 1024;
        break;
    case Qmi8658GyrRange_64dps:
        gyro_lsb_div = 512;
        break;
    case Qmi8658GyrRange_128dps:
        gyro_lsb_div = 256;
        break;
    case Qmi8658GyrRange_256dps:
        gyro_lsb_div = 128;
        break;
    case Qmi8658GyrRange_512dps:
        gyro_lsb_div = 64;
        break;
    case Qmi8658GyrRange_1024dps:
        gyro_lsb_div = 32;
        break;
    case Qmi8658GyrRange_2048dps:
        gyro_lsb_div = 16;
        break;
//      case Qmi8658GyrRange_4096dps:
//          gyro_lsb_div = 8;
//          break;
    default:
        range = Qmi8658GyrRange_512dps;
        gyro_lsb_div = 64;
        break;
    }

    if (stEnable == Qmi8658St_Enable)
    {
        ctl_dada = (unsigned char)range | (unsigned char)odr | 0x80;
    }
    else
    {
        ctl_dada = (unsigned char)range | (unsigned char)odr;
    }
    Qmi8658_write_reg(Qmi8658Register_Ctrl3, ctl_dada);

// Conversion from degrees/s to rad/s if necessary
// set LPF & HPF
    Qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada, 1);
    ctl_dada &= 0x0f;
    if (lpfEnable == Qmi8658Lpf_Enable)
    {
        ctl_dada |= G_LSP_MODE_3;
        ctl_dada |= 0x10;
    }
    else
    {
        ctl_dada &= ~0x10;
    }
    ctl_dada = 0x00;
    Qmi8658_write_reg(Qmi8658Register_Ctrl5, ctl_dada);
// set LPF & HPF
}

void Qmi8658_config_mag(enum Qmi8658_MagDev device, enum Qmi8658_MagOdr odr)
{
    Qmi8658_write_reg(Qmi8658Register_Ctrl4, device | odr);
}

void Qmi8658_config_ae(enum Qmi8658_AeOdr odr)
{
    //Qmi8658_config_acc(Qmi8658AccRange_8g, AccOdr_1000Hz, Lpf_Enable, St_Enable);
    //Qmi8658_config_gyro(Qmi8658GyrRange_2048dps, GyrOdr_1000Hz, Lpf_Enable, St_Enable);
    Qmi8658_config_acc(qmi8658_config.accRange, qmi8658_config.accOdr, Qmi8658Lpf_Enable,
                       Qmi8658St_Disable);
    Qmi8658_config_gyro(qmi8658_config.gyrRange, qmi8658_config.gyrOdr, Qmi8658Lpf_Enable,
                        Qmi8658St_Disable);
    Qmi8658_config_mag(qmi8658_config.magDev, qmi8658_config.magOdr);
    Qmi8658_write_reg(Qmi8658Register_Ctrl6, odr);
}


void Qmi8658_send_ctl9cmd(enum Qmi8658_Ctrl9Command cmd)
{
    unsigned char   status1 = 0x00;
    unsigned short count = 0;

    Qmi8658_write_reg(Qmi8658Register_Ctrl9, (unsigned char)cmd);   // write commond to ctrl9
#if defined(QMI8658_HANDSHAKE_NEW)
#if defined(QMI8658_HANDSHAKE_TO_STATUS)
    unsigned char status_reg = Qmi8658Register_StatusInt;
    unsigned char cmd_done = 0x80;
#else
    unsigned char status_reg = Qmi8658Register_Status1;
    unsigned char cmd_done = 0x01;
#endif

    Qmi8658_read_reg(status_reg, &status1, 1);
    while (((status1 & cmd_done) != cmd_done) && (count++ < 200)) // read statusINT until bit7 is 1
    {
        qmi8658_delay_ms(1);
        Qmi8658_read_reg(status_reg, &status1, 1);
        //qmi8658_printf("status_reg: 0x%x\n", status1);
    }
    //qmi8658_printf("ctrl9 done-1: %d\n", count);
    Qmi8658_write_reg(Qmi8658Register_Ctrl9, Qmi8658_Ctrl9_Cmd_NOP);    // write commond  0x00 to ctrl9
    count = 0;
    Qmi8658_read_reg(status_reg, &status1, 1);
    while (((status1 & cmd_done) == cmd_done) && (count++ < 200)) // read statusINT until bit7 is 0
    {
        qmi8658_delay_ms(1);
        //qmi8658_printf("status_reg: 0x%x\n", status1);
        Qmi8658_read_reg(status_reg, &status1, 1);
    }
    //qmi8658_printf("ctrl9 done-2: %d\n", count);
#else
    while (((status1 & QMI8658_STATUS1_CMD_DONE) == 0) && (count++ < 200))
    {
        Qmi8658_delay(2);
        Qmi8658_read_reg(Qmi8658Register_Status1, &status1, sizeof(status1));
    }
    //qmi8658_printf("ctrl9 done: %d\n", count);
#endif

}


#if defined(QMI8658_USE_FIFO)
void Qmi8658_config_fifo(unsigned char watermark, enum Qmi8658_FifoSize size,
                         enum Qmi8658_FifoMode mode, enum Qmi8658_fifo_format format)
{
    qmi8658_config.fifo_format = format;        //QMI8658_FORMAT_12_BYTES;
    qmi8658_config.fifo_ctrl = (uint8_t)size | (uint8_t)mode;

    Qmi8658_write_reg(Qmi8658Register_FifoCtrl, qmi8658_config.fifo_ctrl);
    Qmi8658_write_reg(Qmi8658Register_FifoWmkTh, watermark);

    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_Rst_Fifo);
}

unsigned short Qmi8658_read_fifo(unsigned char *data)
{
    unsigned char fifo_status[2] = {0, 0};
    unsigned short fifo_bytes = 0;
    unsigned short fifo_level = 0;
    //unsigned short i;

    if ((qmi8658_config.fifo_ctrl & 0x03) != Qmi8658_Fifo_Bypass)
    {
        Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_Req_Fifo);

        Qmi8658_read_reg(Qmi8658Register_FifoCount, fifo_status, 2);
        fifo_bytes = (unsigned short)(((fifo_status[1] & 0x03) << 8) | fifo_status[0]);
        if ((qmi8658_config.fifo_format == QMI8658_FORMAT_ACCEL_6_BYTES) ||
            (qmi8658_config.fifo_format == QMI8658_FORMAT_GYRO_6_BYTES))
        {
            fifo_level = fifo_bytes / 3; // one sensor
            fifo_bytes = fifo_level * 6;
        }
        else if (qmi8658_config.fifo_format == QMI8658_FORMAT_12_BYTES)
        {
            fifo_level = fifo_bytes / 6; // two sensor
            fifo_bytes = fifo_level * 12;
        }
//      qmi8658_printf("fifo byte=%d level=%d\n", fifo_bytes, fifo_level);
        if (fifo_level > 0)
        {
#if 1
            for (int i = 0; i < fifo_level; i++)
            {
                Qmi8658_read_reg(Qmi8658Register_FifoData, &data[i * 12], 12);
            }
#else
            Qmi8658_read_reg(Qmi8658Register_FifoData, data, fifo_bytes);
#endif
        }
        Qmi8658_write_reg(Qmi8658Register_FifoCtrl, qmi8658_config.fifo_ctrl);
    }

    return fifo_level;
}


void Qmi8658_get_fifo_format(enum Qmi8658_fifo_format *format)
{
    if (format)
    {
        *format = qmi8658_config.fifo_format;
    }
}

#endif

unsigned char Qmi8658_readStatusInt(void)
{
    unsigned char status_int;

    Qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);
    //printf("status[0x%x   0x%x]\n",status[0],status[1]);

    return status_int;
}

unsigned char Qmi8658_readStatus0(void)
{
    unsigned char status[2];

    Qmi8658_read_reg(Qmi8658Register_Status0, status, 1);
    //printf("status[0x%x   0x%x]\n",status[0],status[1]);

    return status[0];
}
/*!
 * \brief Blocking read of data status register 1 (::Qmi8658Register_Status1).
 * \returns Status byte \see STATUS1 for flag definitions.
 */
unsigned char Qmi8658_readStatus1(void)
{
    unsigned char status;

    Qmi8658_read_reg(Qmi8658Register_Status1, &status, 1);

    return status;
}

float Qmi8658_readTemp(void)
{
    unsigned char buf[2];
    short temp = 0;
    float temp_f = 0;

    Qmi8658_read_reg(Qmi8658Register_Tempearture_L, buf, 2);
    temp = ((short)buf[1] << 8) | buf[0];
    temp_f = (float)temp / 256.0f;

    return temp_f;
}

void Qmi8658_read_acc_xyz(float acc_xyz[3])
{
    unsigned char   buf_reg[6];
    short           raw_acc_xyz[3];

    Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 6);     // 0x19, 25
    raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
    raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
    raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));

    acc_xyz[0] = (raw_acc_xyz[0] * ONE_G) / acc_lsb_div;
    acc_xyz[1] = (raw_acc_xyz[1] * ONE_G) / acc_lsb_div;
    acc_xyz[2] = (raw_acc_xyz[2] * ONE_G) / acc_lsb_div;
}

void Qmi8658_read_gyro_xyz(float gyro_xyz[3])
{
    unsigned char   buf_reg[6];
    short           raw_gyro_xyz[3];

    Qmi8658_read_reg(Qmi8658Register_Gx_L, buf_reg, 6);     // 0x1f, 31
    raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
    raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
    raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));

    gyro_xyz[0] = (raw_gyro_xyz[0] * 1.0f) / gyro_lsb_div;
    gyro_xyz[1] = (raw_gyro_xyz[1] * 1.0f) / gyro_lsb_div;
    gyro_xyz[2] = (raw_gyro_xyz[2] * 1.0f) / gyro_lsb_div;
}

void Qmi8658_read_xyz(qmi_imu_sensor_data *dat)
{
    unsigned char   buf_reg[12];
    short           raw_acc_xyz[3];
    short           raw_gyro_xyz[3];
//  float acc_t[3];
//  float gyro_t[3];

    unsigned char   buf[3];
    unsigned int timestamp;
    Qmi8658_read_reg(Qmi8658Register_Timestamp_L, buf, 3);  // 0x18 24
    timestamp = (unsigned int)(((unsigned int)buf[2] << 16) | ((unsigned int)buf[1] << 8) | buf[0]);
    if (timestamp > imu_timestamp)
    {
        imu_timestamp = timestamp;
    }
    else
    {
        imu_timestamp = (timestamp + 0x1000000 - imu_timestamp);
    }

    dat->timestamp = imu_timestamp;

    Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 12);    // 0x19, 25
    raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
    raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
    raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));

    raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[7] << 8) | (buf_reg[6]));
    raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[9] << 8) | (buf_reg[8]));
    raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[11] << 8) | (buf_reg[10]));


    APP_PRINT_INFO7("Qmi8658_read_xyz: acc_raw %d %d %d gyro_raw %d %d %d timestamp %d",
                    raw_acc_xyz[0],
                    raw_acc_xyz[1],
                    raw_acc_xyz[2],
                    raw_gyro_xyz[0],
                    raw_gyro_xyz[1],
                    raw_gyro_xyz[2],
                    dat->timestamp);

#if defined(QMI8658_UINT_MG_DPS)
    // mg
    dat->acc[AXIS_X] = (float)(raw_acc_xyz[AXIS_X] * 1000.0f) / acc_lsb_div;
    dat->acc[AXIS_Y] = (float)(raw_acc_xyz[AXIS_Y] * 1000.0f) / acc_lsb_div;
    dat->acc[AXIS_Z] = (float)(raw_acc_xyz[AXIS_Z] * 1000.0f) / acc_lsb_div;
#else
    // m/s2
    dat->acc[AXIS_X] = (float)(raw_acc_xyz[AXIS_X] * ONE_G) / acc_lsb_div;
    dat->acc[AXIS_Y] = (float)(raw_acc_xyz[AXIS_Y] * ONE_G) / acc_lsb_div;
    dat->acc[AXIS_Z] = (float)(raw_acc_xyz[AXIS_Z] * ONE_G) / acc_lsb_div;
#endif
//  dat->acc[AXIS_X] = imu_map.sign[AXIS_X]*acc_t[imu_map.map[AXIS_X]];
//  dat->acc[AXIS_Y] = imu_map.sign[AXIS_Y]*acc_t[imu_map.map[AXIS_Y]];
//  dat->acc[AXIS_Z] = imu_map.sign[AXIS_Z]*acc_t[imu_map.map[AXIS_Z]];

#if defined(QMI8658_UINT_MG_DPS)
    // dps
    dat->gyro[0] = (float)(raw_gyro_xyz[0] * 1.0f) / gyro_lsb_div;
    dat->gyro[1] = (float)(raw_gyro_xyz[1] * 1.0f) / gyro_lsb_div;
    dat->gyro[2] = (float)(raw_gyro_xyz[2] * 1.0f) / gyro_lsb_div;
#else
    // rad/s
    dat->gyro[AXIS_X] = (float)(raw_gyro_xyz[AXIS_X] * 0.01745f) / gyro_lsb_div; // *pi/180
    dat->gyro[AXIS_Y] = (float)(raw_gyro_xyz[AXIS_Y] * 0.01745f) / gyro_lsb_div;
    dat->gyro[AXIS_Z] = (float)(raw_gyro_xyz[AXIS_Z] * 0.01745f) / gyro_lsb_div;
#endif
//  dat->gyro[AXIS_X] = imu_map.sign[AXIS_X]*gyro_t[imu_map.map[AXIS_X]];
//  dat->gyro[AXIS_Y] = imu_map.sign[AXIS_Y]*gyro_t[imu_map.map[AXIS_Y]];
//  dat->gyro[AXIS_Z] = imu_map.sign[AXIS_Z]*gyro_t[imu_map.map[AXIS_Z]];
}


void Qmi8658_read_xyz_raw(short raw_acc_xyz[3], short raw_gyro_xyz[3], unsigned int *tim_count)
{
    unsigned char   buf_reg[12];

    if (tim_count)
    {
        unsigned char   buf[3];
        unsigned int timestamp;
        Qmi8658_read_reg(Qmi8658Register_Timestamp_L, buf, 3);  // 0x18 24
        timestamp = (unsigned int)(((unsigned int)buf[2] << 16) | ((unsigned int)buf[1] << 8) | buf[0]);
        if (timestamp > imu_timestamp)
        {
            imu_timestamp = timestamp;
        }
        else
        {
            imu_timestamp = (timestamp + 0x1000000 - imu_timestamp);
        }

        *tim_count = imu_timestamp;
    }
    if (raw_acc_xyz && raw_gyro_xyz)
    {
        Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 12);    // 0x19, 25
        raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
        raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
        raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));
        raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[7] << 8) | (buf_reg[6]));
        raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[9] << 8) | (buf_reg[8]));
        raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[11] << 8) | (buf_reg[10]));
    }
    else if (raw_acc_xyz)
    {
        Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 6);     // 0x19, 25
        raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
        raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
        raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));
    }
    else if (raw_gyro_xyz)
    {
        Qmi8658_read_reg(Qmi8658Register_Gx_L, buf_reg, 6);     // 0x19, 25
        raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
        raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
        raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));
    }
}


void Qmi8658_read_ae(float quat[4], float velocity[3])
{
    unsigned char   buf_reg[14];
    short           raw_q_xyz[4];
    short           raw_v_xyz[3];

    Qmi8658_read_reg(Qmi8658Register_Q1_L, buf_reg, 14);
    raw_q_xyz[0] = (short)((unsigned short)(buf_reg[1] << 8) | (buf_reg[0]));
    raw_q_xyz[1] = (short)((unsigned short)(buf_reg[3] << 8) | (buf_reg[2]));
    raw_q_xyz[2] = (short)((unsigned short)(buf_reg[5] << 8) | (buf_reg[4]));
    raw_q_xyz[3] = (short)((unsigned short)(buf_reg[7] << 8) | (buf_reg[6]));

    raw_v_xyz[1] = (short)((unsigned short)(buf_reg[9] << 8) | (buf_reg[8]));
    raw_v_xyz[2] = (short)((unsigned short)(buf_reg[11] << 8) | (buf_reg[10]));
    raw_v_xyz[2] = (short)((unsigned short)(buf_reg[13] << 8) | (buf_reg[12]));

    quat[0] = (float)(raw_q_xyz[0] * 1.0f) / ae_q_lsb_div;
    quat[1] = (float)(raw_q_xyz[1] * 1.0f) / ae_q_lsb_div;
    quat[2] = (float)(raw_q_xyz[2] * 1.0f) / ae_q_lsb_div;
    quat[3] = (float)(raw_q_xyz[3] * 1.0f) / ae_q_lsb_div;

    velocity[0] = (float)(raw_v_xyz[0] * 1.0f) / ae_v_lsb_div;
    velocity[1] = (float)(raw_v_xyz[1] * 1.0f) / ae_v_lsb_div;
    velocity[2] = (float)(raw_v_xyz[2] * 1.0f) / ae_v_lsb_div;
}

void Qmi8658_enableWakeOnMotion(enum Qmi8658_Interrupt int_set,
                                enum Qmi8658_WakeOnMotionThreshold threshold, unsigned char blankingTime)
{
    unsigned char cal1_1_reg = (unsigned char)threshold;
    unsigned char cal1_2_reg  = (unsigned char)int_set | (blankingTime & 0x3F);
    // unsigned char status1 = 0;
    // int count = 0;

    Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_DISABLE_ALL);
    Qmi8658_config_acc(Qmi8658AccRange_8g, Qmi8658AccOdr_LowPower_21Hz, Qmi8658Lpf_Disable,
                       Qmi8658St_Disable);

    Qmi8658_write_reg(Qmi8658Register_Cal1_L, cal1_1_reg);
    Qmi8658_write_reg(Qmi8658Register_Cal1_H, cal1_2_reg);
    // ctrl9 wom setting
    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_WoM_Setting);
    // ctrl9 wom setting
    Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_ACC_ENABLE);
}

void Qmi8658_disableWakeOnMotion(void)
{
    // unsigned char status1 = 0;
    // int count = 0;

    Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_DISABLE_ALL);
    Qmi8658_write_reg(Qmi8658Register_Cal1_L, 0);
    Qmi8658_write_reg(Qmi8658Register_Cal1_H, 0);

    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_WoM_Setting);
}

void Qmi8658_enableSensors(unsigned char enableFlags)
{
    if (enableFlags & QMI8658_CONFIG_AE_ENABLE)
    {
        enableFlags |= QMI8658_CTRL7_ACC_ENABLE | QMI8658_CTRL7_GYR_ENABLE;
    }
#if defined(QMI8658_SYNC_SAMPLE_MODE)
    Qmi8658_write_reg(Qmi8658Register_Ctrl7, enableFlags | 0x80);
#else
    Qmi8658_write_reg(Qmi8658Register_Ctrl7, enableFlags & QMI8658_CTRL7_ENABLE_MASK);
#endif
}

void Qmi8658_config_am(void)
{
    Qmi8658_write_reg(Qmi8658Register_Cal1_L,
                      0x03);        // any motion X threshold U 3.5 first three bit(uint 1g)  last five bit (uint 1/32 g)
    Qmi8658_write_reg(Qmi8658Register_Cal1_H,
                      0x03);        // any motion Y threshold U 3.5 first three bit(uint 1g)  last five bit (uint 1/32 g)
    Qmi8658_write_reg(Qmi8658Register_Cal2_L,
                      0x03);        // any motion Z threshold U 3.5 first three bit(uint 1g)  last five bit (uint 1/32 g)
    Qmi8658_write_reg(Qmi8658Register_Cal2_H,
                      0x02);        // no motion X threshold U 3.5 first three bit(uint 1g)  last five bit (uint 1/32 g)
    Qmi8658_write_reg(Qmi8658Register_Cal3_L, 0x02);
    Qmi8658_write_reg(Qmi8658Register_Cal3_H, 0x02);
    Qmi8658_write_reg(Qmi8658Register_Cal4_L, 0xf7);        // MOTION_MODE_CTRL
    Qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x01);        // value 0x01

    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_Motion);

    Qmi8658_write_reg(Qmi8658Register_Cal1_L, 0x05);        // AnyMotionWindow.
    Qmi8658_write_reg(Qmi8658Register_Cal1_H, 0x01);        // NoMotionWindow
    Qmi8658_write_reg(Qmi8658Register_Cal2_L, 0x2c);        // SigMotionWaitWindow[7:0]
    Qmi8658_write_reg(Qmi8658Register_Cal2_H, 0x01);        // SigMotionWaitWindow [15:8]
    Qmi8658_write_reg(Qmi8658Register_Cal3_L, 0x64);        // SigMotionConfirmWindow[7:0]
    Qmi8658_write_reg(Qmi8658Register_Cal3_H, 0x00);        // SigMotionConfirmWindow[15:8]
    //Qmi8658_write_reg(Qmi8658Register_Cal4_L, 0xf7);
    Qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x02);        // value 0x02

    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_Motion);
}

void Qmi8658_Config_apply(struct Qmi8658Config const *config)
{
    unsigned char fisSensors = config->inputSelection;

    if (fisSensors & QMI8658_CONFIG_AE_ENABLE)
    {
        Qmi8658_config_ae(config->aeOdr);
    }
    else
    {
        if (config->inputSelection & QMI8658_CONFIG_ACC_ENABLE)
        {
            Qmi8658_config_acc(config->accRange, config->accOdr, Qmi8658Lpf_Disable, Qmi8658St_Disable);
        }
        if (config->inputSelection & QMI8658_CONFIG_GYR_ENABLE)
        {
            Qmi8658_config_gyro(config->gyrRange, config->gyrOdr, Qmi8658Lpf_Disable, Qmi8658St_Disable);
        }
    }

    if (config->inputSelection & QMI8658_CONFIG_MAG_ENABLE)
    {
        Qmi8658_config_mag(config->magDev, config->magOdr);
    }
#if defined(QMI8658_USE_FIFO)
    //Qmi8658_config_fifo(Qmi8658_Fifo_WmkHalf,Qmi8658_Fifo_64,Qmi8658_Fifo_Stream,QMI8658_FORMAT_12_BYTES);
#endif
    Qmi8658_enableSensors(fisSensors);
}


void Qmi8658_do_selftest(void)
{
    APP_PRINT_INFO0("Qmi8658_do_selftest\n");

    Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
    Qmi8658_delay(1000);    // delay 500ms above
    Qmi8658_send_ctl9cmd(Qmi8658_Ctrl9_Cmd_On_Demand_Cali);
    Qmi8658_delay(3000);    // delay 2000ms above
}

void Qmi8658_select_odr(int index)
{
    if (index == 0)
    {
        APP_PRINT_INFO0("qmi8658 ODR=31Hz\n");
    }
    else if (index == 1)
    {
        APP_PRINT_INFO0("qmi8658 ODR=62Hz\n");
    }
    else if (index == 2)
    {
        APP_PRINT_INFO0("qmi8658 ODR=125Hz\n");
    }
    else if (index == 3)
    {
        APP_PRINT_INFO0("qmi8658 ODR=250Hz\n");
    }
    else if (index == 4)
    {
        APP_PRINT_INFO0("qmi8658 ODR=500Hz\n");
    }
    else if (index == 5)
    {
        APP_PRINT_INFO0("qmi8658 ODR=1000Hz\n");
    }

    qmi8658_odr_sel = (unsigned char)index;
}

unsigned char Qmi8658_init(void)
{
    unsigned char qmi8658_chip_id = 0x00;
    unsigned char qmi8658_revision_id = 0x00;
    unsigned char qmi8658_slave[2] = {QMI8658_SLAVE_ADDR_L, QMI8658_SLAVE_ADDR_H};
    unsigned char iCount = 0;

    while ((qmi8658_chip_id == 0x00) && (iCount < 2))
    {
        qmi8658_slave_addr = qmi8658_slave[iCount];
        Qmi8658_read_reg(Qmi8658Register_WhoAmI, &qmi8658_chip_id, 1);
        if (qmi8658_chip_id == 0x05)
        {
            break;
        }
        iCount++;
    }
    if (qmi8658_chip_id == 0x05)
    {
        unsigned char firmware_id[3];

        Qmi8658_write_reg(Qmi8658Register_Ctrl1, 0x60);
        Qmi8658_read_reg(Qmi8658Register_Revision, &qmi8658_revision_id, 1);
        APP_PRINT_INFO3("Qmi8658_init slave=0x%x  Qmi8658Register_WhoAmI=0x%x 0x%x\n", qmi8658_slave_addr,
                        qmi8658_chip_id, qmi8658_revision_id);
        Qmi8658_read_reg(0x49, firmware_id, 3);
        APP_PRINT_INFO3("Old Firmware ID[0x%x 0x%x 0x%x]\n", firmware_id[0], firmware_id[1],
                        firmware_id[2]);
        qmi8658_config.inputSelection = QMI8658_CONFIG_ACCGYR_ENABLE;//QMI8658_CONFIG_ACCGYR_ENABLE;
        qmi8658_config.accRange = Qmi8658AccRange_8g;
        qmi8658_config.accOdr = acc_odr_tbl[qmi8658_odr_sel];       //Qmi8658AccOdr_500Hz;
        qmi8658_config.gyrRange =
            Qmi8658GyrRange_1024dps;      //Qmi8658GyrRange_2048dps   Qmi8658GyrRange_1024dps
        qmi8658_config.gyrOdr = gyr_odr_tbl[qmi8658_odr_sel];       //Qmi8658GyrOdr_500Hz;
        qmi8658_config.magOdr = Qmi8658MagOdr_125Hz;
        qmi8658_config.magDev = MagDev_AKM09918;
        qmi8658_config.aeOdr = Qmi8658AeOdr_128Hz;
        Qmi8658_write_reg(Qmi8658Register_Ctrl2, 0x00);
        Qmi8658_write_reg(Qmi8658Register_Ctrl3, 0x00);
        Qmi8658_write_reg(Qmi8658Register_Ctrl5, 0x00);
        Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
        qmi8658_delay_ms(10);
        Qmi8658_Config_apply(&qmi8658_config);
        if (0)
        {
            unsigned char read_data = 0x00;
            Qmi8658_read_reg(Qmi8658Register_Ctrl1, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl1=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl2, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl2=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl3, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl3=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl4, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl4=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl5, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl5=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl6, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl6=0x%x \n", read_data);
            Qmi8658_read_reg(Qmi8658Register_Ctrl7, &read_data, 1);
            APP_PRINT_INFO1("Qmi8658Register_Ctrl7=0x%x \n", read_data);
        }
//      Qmi8658_set_layout(2);

        return 1;

    }
    else
    {
        APP_PRINT_INFO0("Qmi8658_init fail\n");
        qmi8658_chip_id = 0;
        return 0;
    }
    //return qmi8658_chip_id;
}


void Qmi8658_enable_fifo(void)
{
    APP_PRINT_INFO0("Qmi8658_enable_fifo\n");

    Qmi8658_write_reg(Qmi8658Register_Ctrl1, 0x60);
    Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);                 // disable sensors
#if defined(QMI8658_HANDSHAKE_NEW)
    Qmi8658_write_reg(Qmi8658Register_Ctrl8, 0xc0); // set control 8 = 0xC0
#endif
    qmi8658_delay_ms(2);                                            // delay 2 mill-second

    Qmi8658_config_fifo(32, Qmi8658_Fifo_64, Qmi8658_Fifo_Stream,
                        QMI8658_FORMAT_12_BYTES); // config fifo
//  Qmi8658_config_motion();                                        // config motion

//  Qmi8658_write_reg(Qmi8658Register_Ctrl2, 0x24);     // write ctrl2  3:1000Hz  4:500Hz  5:250Hz  6 :125Hz 7: 62.5Hz
//  Qmi8658_write_reg(Qmi8658Register_Ctrl3, 0x64);     // write ctrl3
//  Qmi8658_write_reg(Qmi8658Register_Ctrl5, 0x00);     // write ctrl5
//  acc_lsb_div = (1<<12);
//  gyro_lsb_div = 32;

    Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x3);      // write ctrl7
//  Qmi8658_write_reg(Qmi8658Register_Ctrl8, qmi8658_ctrl8_reg|0x02);   // enable any motion
    qmi8658_delay_ms(2);
}


void Qmi8658_config_lowpower(void)
{
    qmi8658_config.inputSelection = QMI8658_CONFIG_ACC_ENABLE;
    qmi8658_config.accRange = Qmi8658AccRange_8g;
    qmi8658_config.accOdr = Qmi8658AccOdr_LowPower_21Hz;        //Qmi8658AccOdr_500Hz;
    //qmi8658_config.gyrRange = Qmi8658GyrRange_1024dps;        //Qmi8658GyrRange_2048dps   Qmi8658GyrRange_1024dps
    //qmi8658_config.gyrOdr = Qmi8658GyrOdr_500Hz;      //Qmi8658GyrOdr_500Hz;
    //qmi8658_config.magOdr = Qmi8658MagOdr_125Hz;
    //qmi8658_config.magDev = MagDev_AKM09918;
    //qmi8658_config.aeOdr = Qmi8658AeOdr_128Hz;

    //qmi8658_delay_ms(10);
    Qmi8658_Config_apply(&qmi8658_config);
}

void Qmi8658_enable_am(void)
{
    Qmi8658_write_reg(Qmi8658Register_Ctrl1, 0x60);
    Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);     // disable sensors
#if defined(QMI8658_HANDSHAKE_NEW)
    Qmi8658_write_reg(Qmi8658Register_Ctrl8, 0xc0);     // set control 8 = 0xC0
#endif
    qmi8658_delay_ms(2);                                            // delay 2 mill-second

    Qmi8658_config_am();

    //Qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x3);        // write ctrl7
    Qmi8658_config_lowpower();
#if defined(QMI8658_HANDSHAKE_NEW)
    Qmi8658_write_reg(Qmi8658Register_Ctrl8, 0xc0 | 0x02);  // enable any motion
#endif
    qmi8658_delay_ms(2);
}

void Qmi8658_cb_init(P_QMI8658_I2C_READ_CB read_cb, P_QMI8658_I2C_WRITE_CB write_cb)
{
    qmi8658_if.qst_sw_read_reg = read_cb;
    qmi8658_if.qst_sw_write_reg = write_cb;
}
#endif
