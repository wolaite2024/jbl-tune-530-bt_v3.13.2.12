#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#ifndef IMU_ALGO_AXIS9_H_
#define IMU_ALGO_AXIS9_H_

typedef struct
{
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;
} QST_Filter;

typedef struct
{
    float _delay_element_1;
    float _delay_element_2;
} QST_Filter_Buffer;


#ifdef __cplusplus
extern "C" {
#endif

void set_cutoff_frequency(float sample_freq, float cutoff_freq, QST_Filter *filter);

void qst_fusion_update(float fusion_accel[3], float fusion_gyro[3], float *fusion_dt, float *rpy,
                       float *quaternion, float *line_accel);



extern unsigned char Qmi8658_read_reg(unsigned char reg, unsigned char *buf, unsigned short len);

#ifdef __cplusplus
}
#endif

#endif /* GYRO_ALGO_H_ */
