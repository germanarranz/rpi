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

typedef struct{
	int red, green, blue, clearence;
}data_color;

typedef struct{
	float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, temp;
}data_acc;

struct t_data{
	data_acc acc_data;
	data_color color_data;
	int acc_sens;
	int gyro_sens;
};


#endif /* SERVER_H_ */
