/*
 * main.h
 *
 *  Created on: Mar 25, 2024
 *      Author: ubuntu
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

extern pthread_mutex_t lock;
extern int bucle;


#endif /* MAIN_H_ */
