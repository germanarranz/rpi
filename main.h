/*
 * main.h
 *
 *  Created on: Mar 25, 2024
 *      Author: Germán Arranz y Gonzalo Castillo
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


typedef struct{
	int red, green, blue, clearence;
}data_color;

typedef struct{
	float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, temp;
}data_acc;

typedef struct{
	data_acc acc_data;
	data_color color_data;
	int acc_sens;
	int gyro_sens;
}t_data;

extern pthread_mutex_t lock;
extern int bucle;

void uso(char *argv[], int argc);

#endif /* MAIN_H_ */
