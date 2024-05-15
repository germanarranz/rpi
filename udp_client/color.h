/*
 * color.h
 *
 *  Created on: Mar 25, 2024
 *      Author: Germán Arranz y Gonzalo Castillo
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "main.h"

#define MAX_READ 31
#define MAX_WRITE 31

#define COMMAND 0x80
#define ENABLE_REG 0x00
#define TIMING_REG 0x01
#define GAIN_REG 0x0F
#define STATUS_REG 0x13
#define CLEAR_REG 0x14
#define RED_REG 0x16
#define GREEN_REG 0x18
#define BLUE_REG 0x1A
#define INCREMENT 0x20


void init_I2C_color();
void power_color(int on);
void readRGBC();
void conf_rgbc(int rgbc_cycles, int rgbc_gain);
void* color(void* arg);


extern int term_color;
//extern t_data data;


#endif /* COLOR_H_ */
