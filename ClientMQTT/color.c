/*
 * color.c
 *
 *  Created on: Mar 25, 2024
 *      Author: Germán Arranz y Gonzalo Castillo
 */

#include "color.h"

//File descriptor
int fd_color;
extern t_data data;

/*
 * @function color
 * @abstarct Obtains the mesurements from the color sensor
 *
 * @param A void pointer to an array of parameters (number of arguments and the arguments thmeselves)
 *
 * @result None
 *
 * */

void* color(void* arg){

	//Importing arguments from the command line
    char **arguments = (char **)arg;
    char **argv = (char **)arguments[1];

    //Powering and configurating the accelerometer
    pthread_mutex_lock(&lock);
	init_I2C_color();
	power_color(1);
	conf_rgbc(atoi(argv[3]), atoi(argv[4]));
	pthread_mutex_unlock(&lock);

	//Mesurements
	while(term_color && bucle){
		pthread_mutex_lock(&lock);
		readRGBC();
		pthread_mutex_unlock(&lock);
		sleep(0.5);
	}
	close(fd_color);
	pthread_exit(NULL);


}

/*
 * @function power_color
 * @abstarct Turns on/off the color sensor
 *
 * @param An integer signaling on/off
 *
 * @result None
 *
 * */

void power_color(int on){
	char write_bytes [2];
	write_bytes[0] = COMMAND;
	write_bytes[1] = on;

	write(fd_color, write_bytes, 2);
}

/*
 * @function init_I2C_acc
 * @abstarct Starts the I2C communication with the color sensor
 *
 * @param None
 *
 * @result None
 *
 * */

void init_I2C_color(){
	char i2cFile[15];
	int device = 1;
	int addr = 0x29;

	sprintf(i2cFile, "/dev/i2c-%d", device);
	fd_color = open(i2cFile, O_RDWR);

	ioctl(fd_color, I2C_SLAVE, addr);
}

/*
 * @function readAccel
 * @abstarct Reads the acceleration data from the accelerometer and translates it to a decimal number
 *
 * @param None
 *
 * @result None
 *
 * */


void readRGBC(){
	char read_bytes[MAX_READ];
	char write_bytes[MAX_WRITE];

	write_bytes[0] = COMMAND | ENABLE_REG;
	write_bytes[1] = 0x03;
	write(fd_color, write_bytes, 2);

	write_bytes[0] = COMMAND | CLEAR_REG;
	write(fd_color, write_bytes, 1);

	read(fd_color, read_bytes, 8);

	data.color_data.clearence= (read_bytes[1] << 8) | read_bytes[0];
	int16_t red_raw = (read_bytes[3] << 8) | read_bytes[2];
	int16_t green_raw = (read_bytes[5] << 8) | read_bytes[4];
	int16_t blue_raw = (read_bytes[7] << 8) | read_bytes[6];

	data.color_data.red = (int) (((float)red_raw / data.color_data.clearence)*255);
	data.color_data.green = (int) (((float)green_raw / data.color_data.clearence)*255);
	data.color_data.blue = (int) (((float)blue_raw / data.color_data.clearence)*255);
}


/*
 * @function conf_rgbc
 * @abstarct Configures the integration cycles and the gain of the accelerometer
 *
 * @param rgbc_cycles, rgbc_gain
 *
 * @result None
 *
 * */

void conf_rgbc(int rgbc_cycles, int rgbc_gain){
	char write_bytes[MAX_WRITE];

	write_bytes[0] = COMMAND | TIMING_REG;
	write_bytes[1] = 0x00;
	switch(rgbc_cycles){
		case 1: write_bytes[1] = 0xFF;
		break;
		case 10: write_bytes[1] = 0xF6;
		break;
		case 42: write_bytes[1] = 0xD5;
		break;
		case 64: write_bytes[1] = 0xC0;
		break;
		case 256: write_bytes[1] = 0x00;
		break;
		default: fprintf(stderr, "Ciclos no soportados usando 256 por defecto");
	}
	write(fd_color, write_bytes, 2);

	write_bytes[0] = COMMAND | GAIN_REG;
	write_bytes[1] = 0x00;
	switch(rgbc_gain){
		case 1: write_bytes[1] = 0x00;
		break;
		case 4: write_bytes[1] = 0x01;
		break;
		case 16: write_bytes[1] = 0x02;
		break;
		case 60: write_bytes[1] = 0x03;
		break;
		default: fprintf(stderr, "Ganancia no soportada usando 1 por defecto");
	}
	write(fd_color, write_bytes, 2);
}


