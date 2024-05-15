/*
 * acc.c
 *
 *  Created on: Mar 25, 2024
 *      Author: Germán Arranz y Gonzalo Castillo
 */

#include "acc.h"

//File Descriptor
int fd_acc;
extern t_data data;

/*
 * @function acc
 * @abstarct Obtains the mesuremnets from the accelorometer
 *
 * @param A void pointer to an array of parameters (number of arguments and the arguments thmeselves)
 *
 * @result None
 *
 * */

void* acc(void* arg) {

	//Importing arguments from the command line
    char **arguments = (char **)arg;
    char **argv = (char **)arguments[1];

    //Powering and configurating the accelerometer
    pthread_mutex_lock(&lock);
    init_I2C_acc();
    power_acc(ON);
    conf_accel(atoi(argv[1]));
    conf_gyro(atoi(argv[2]));
    pthread_mutex_unlock(&lock);

    //Mesurements
    while (term_acc && bucle) {
        pthread_mutex_lock(&lock);
        readAccel(BASE_ACCEL_SCALE / (float)atoi(argv[1]));
        data.acc_data.temp = readTemp();
        readGyro(BASE_GYRO_SCALE / (float)atoi(argv[2]));
        pthread_mutex_unlock(&lock);
        sleep(0.1);
    }
    close(fd_acc);
    pthread_exit(NULL);
}

/*
 * @function power_acc
 * @abstarct Turns on/off the accelerometer
 *
 * @param An integer signaling on/off
 *
 * @result None
 *
 * */

void power_acc(int on) {
    char write_bytes[2];
    write_bytes[0] = 0x6B;
    write_bytes[1] = on;

    write(fd_acc, write_bytes, 2);
}

/*
 * @function init_I2C_acc
 * @abstarct Starts the I2C communication with the accelerometer
 *
 * @param None
 *
 * @result None
 *
 * */

void init_I2C_acc() {
    char i2cFile[15];
    int device = 1;
    int addr = 0x68;

    sprintf(i2cFile, "/dev/i2c-%d", device);
    fd_acc = open(i2cFile, O_RDWR);

    ioctl(fd_acc, I2C_SLAVE, addr);
}

/*
 * @function readAccel
 * @abstarct Reads the acceleration data from the accelerometer and translates it to a decimal number
 *
 * @param Accelerometer scale
 *
 * @result None
 *
 * */

void readAccel(float accel_scale){
	char read_bytes[MAX_READ];
	char write_bytes[MAX_WRITE];

	write_bytes[0] = ACCEL_REG;
	write(fd_acc, write_bytes, 1);
	read(fd_acc, read_bytes, 6);

	int16_t x_accel = (read_bytes[0] << 8) | read_bytes[1];
	int16_t y_accel = (read_bytes[2] << 8) | read_bytes[3];
	int16_t z_accel = (read_bytes[4] << 8) | read_bytes[5];

	data.acc_data.acc_x = x_accel / accel_scale;
	data.acc_data.acc_y = y_accel / accel_scale;
	data.acc_data.acc_z = z_accel / accel_scale;
}

/*
 * @function readTemp
 * @abstarct Reads the inside temperature from the chip and translates it to a decimal number
 *
 * @param None
 *
 * @result None
 *
 * */

float readTemp(){
	char read_bytes[MAX_READ];
	char write_bytes[MAX_WRITE];
	float temp;

	write_bytes[0] = TEMP_REG;
	write(fd_acc, write_bytes, 1);
	read(fd_acc, read_bytes, 2);
	int16_t temper = (read_bytes[0] << 8) | read_bytes[1];

	temp = (temper/340.0) + 36.53;

	return temp;
}

/*
 * @function readGyro
 * @abstarct Reads the angular acceleration data from the chip and translates it to a decimal number
 *
 * @param None
 *
 * @result None
 *
 * */

void readGyro(float gyro_scale){
	char read_bytes[MAX_READ];
	char write_bytes[MAX_WRITE];

	write_bytes[0] = GYRO_REG;
	write(fd_acc, write_bytes, 1);
	read(fd_acc, read_bytes, 6);

	int16_t x_gyro = (read_bytes[0] << 8) | read_bytes[1];
	int16_t y_gyro = (read_bytes[2] << 8) | read_bytes[3];
	int16_t z_gyro = (read_bytes[4] << 8) | read_bytes[5];

	data.acc_data.gyro_x = x_gyro / gyro_scale;
	data.acc_data.gyro_y = y_gyro / gyro_scale;
	data.acc_data.gyro_z = z_gyro / gyro_scale;
}

/*
 * @function conf_accel
 * @abstarct Configures the acceleration scale of the accelerometer based on the user input
 *
 * @param accel_range
 *
 * @result None
 *
 * */

void conf_accel(int accel_range) {
	char write_bytes[MAX_WRITE];
	write_bytes[0] = ACCEL_CONF_REG;
	write_bytes[1] = 0x00;

	switch(accel_range){
		case 2: write_bytes[1] = 0x00;
		break;
		case 4: write_bytes[1] = 0x08;
		break;
		case 8: write_bytes[1] = 0xF0;
		break;
		case 16: write_bytes[1] = 0xF8;
		break;
		default: fprintf(stderr, "Rango del acelerometro no puesto correctamente, usando rango por defecto +- 2g");
	}
	write(fd_acc, write_bytes, 2);
}

/*
 * @function conf_accel
 * @abstarct Configures the angular scale of the gyroscope based on the user input
 *
 * @param gyro_range
 *
 * @result None
 *
 * */

void conf_gyro(int gyro_range){
	char write_bytes[MAX_WRITE];
	write_bytes[0] = GYRO_CONF_REG;
	write_bytes[1] = 0x00;

	switch(gyro_range){
		case 250: write_bytes[1] = 0x00;
		break;
		case 500: write_bytes[1] = 0x08;
		break;
		case 1000: write_bytes[1] = 0xF0;
		break;
		case 2000: write_bytes[1] = 0xF8;
		break;
		default: fprintf(stderr, "Rango del giroscopio no puesto correctamente, usando rango por defecto 250 º/s");
	}
	write(fd_acc, write_bytes, 2);
}


