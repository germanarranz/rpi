/*
 * acc.c
 *
 *  Created on: Mar 25, 2024
 *      Author: ubuntu
 */

#include "acc.h"

int fd_acc;

void* acc(void* arg) {
    char **arguments = (char **)arg;

    int argc = *((int *)arguments[0]);
    char **argv = (char **)arguments[1];

    int sleep_time = atoi(argv[1]) * 1000;
    int accel_range = atoi(argv[2]);
    float accel_scale = BASE_ACCEL_SCALE / (float)accel_range;
    int gyro_range = atoi(argv[3]);
    float gyro_scale = BASE_GYRO_SCALE / (float)gyro_range;

    init_I2C_acc();
    power_acc(ON);
    conf_accel(accel_range);
    conf_gyro(gyro_range);

    while (term_acc) {
        pthread_mutex_lock(&lock);
        readAccel(accel_scale);
        acc_data.temp = readTemp();
        readGyro(acc_data, gyro_scale);
        pthread_mutex_unlock(&lock);
        usleep(sleep_time);
    }

    pthread_exit(NULL);
}

void terminar_acc(int cmd) {
    term_acc = 0;
    close(fd_acc);
}

void power_acc(int on) {
    char write_bytes[2];
    write_bytes[0] = 0x6B;
    write_bytes[1] = on;

    write(fd_acc, write_bytes, 2);	//Escribimos en el registro de POWER
}

void init_I2C_acc() {
    char i2cFile[15]; //Donde se almacenara el nombre del dispositivo
    int device = 1;   //En caso de que tengamos mas de un dispositivo controlado por i2c
    int addr = 0x68;  //La direccion del MPU6050 por defecto

    /*
    Ponemos la direccion del dispositivo en el array de caracteres (es C) y abrimos un descriptor mediante la fucnion open(),
    esta nos devuelve un int que nos servira para referenciarlo durante todo el programa.
    */

    sprintf(i2cFile, "/dev/i2c-%d", device);
    fd_acc = open(i2cFile, O_RDWR);   //Se puede escribir y leer

    if (fd_acc < 0) {
        perror("Error opening I2C device");
        exit(1);
    }

    if (ioctl(fd_acc, I2C_SLAVE, addr) < 0) {
        perror("Error setting I2C slave address");
        exit(1);
    }
}

void readAccel(float accel_scale){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	write_bytes[0] = ACCEL_REG;     //Este es el registro de los 8 primeros bits del registro X del acelerometro.
	write(fd_acc, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd_acc, read_bytes, 6);        //Los 6 registros de aceleracion

	int16_t x_accel = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar
	int16_t y_accel = (read_bytes[2] << 8) | read_bytes[3]; //Concatenar
	int16_t z_accel = (read_bytes[4] << 8) | read_bytes[5]; //Concatenar

	acc_data.acc_x = x_accel / accel_scale; //Sensibilidad pasada como parametro
	acc_data.acc_y = y_accel / accel_scale; //Sensibilidad pasada como parametro
	acc_data.acc_z = z_accel / accel_scale; //Sensibilidad pasada como parametro
}

float readTemp(){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.
	float temp;

	write_bytes[0] = TEMP_REG;      //Este es el registro de la temperatura
	write(fd_acc, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd_acc, read_bytes, 2);       	//Los 2 registros de temperatura
	int16_t temper = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar

	temp = (temper/340.0) + 36.53;

	return temp;
}

void readGyro(data_acc arg, float gyro_scale){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	write_bytes[0] = GYRO_REG;      //Este es el registro de inicio del giroscopio.
	write(fd_acc, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd_acc, read_bytes, 6);       	//Los 6 registros del giroscopio

	int16_t x_gyro = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar
	int16_t y_gyro = (read_bytes[2] << 8) | read_bytes[3]; //Concatenar
	int16_t z_gyro = (read_bytes[4] << 8) | read_bytes[5]; //Concatenar

	arg.gyro_x = x_gyro / gyro_scale; //Sensibilidad pasada como parametro
	arg.gyro_y = y_gyro / gyro_scale; //Sensibilidad pasada como parametro
	arg.gyro_z = z_gyro / gyro_scale; //Sensibilidad pasada como parametro
}

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


