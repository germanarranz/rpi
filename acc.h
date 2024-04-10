/*
 * acc.h
 *
 *  Created on: Mar 25, 2024
 *      Author: ubuntu
 */

#ifndef ACC_H_
#define ACC_H_

#include "main.h"

#define BASE_ACCEL_SCALE 32768.0
#define BASE_GYRO_SCALE 32750.0
#define MAX_READ 31
#define MAX_WRITE 31
#define ON 0
#define OFF 1
#define ACCEL_REG 0x3B
#define TEMP_REG 0x41
#define GYRO_REG 0x43
#define ACCEL_CONF_REG 0x1C
#define GYRO_CONF_REG 0x1B


typedef struct{
	float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, temp;
}data_acc;

int esPotencia2 (int num);
void power_acc(int on);
void init_I2C_acc();
void readAccel(float accel_scale);
void readGyro(float gyro_scale);
float readTemp();
void conf_accel(int accel_range);
void conf_gyro(int gyro_range);
void *acc (void *arg);

extern data_acc acc_data;
extern int term_acc;


#endif /* ACC_H_ */
