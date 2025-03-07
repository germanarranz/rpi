/*
 * client.h
 *
 *  Created on: Apr 19, 2024
 *      Author: ubuntu
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

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

t_data media(t_data data_recv[]);
t_data dev(t_data data_recv[], t_data mean);
t_data max(t_data data_recv[]);
t_data min(t_data data_recv[]);


#endif /* SERVER_H_ */
