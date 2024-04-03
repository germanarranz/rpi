/*
 * color.c
 *
 *  Created on: Mar 25, 2024
 *      Author: ubuntu
 */

#include "color.h"

int fd_color = 0;

void* color(void* arg){

	int sleep_time = 1000000;
	int rgbc_cycles = 10;
	int rgbc_gain = 1;
	float rgbc_scale = 1.0;

	init_I2C_color();
	power_color(1);
	conf_rgbc(rgbc_cycles, rgbc_gain);

	while(term_color && bucle){
		pthread_mutex_lock(&lock);
		readRGBC();
		pthread_mutex_unlock(&lock);
		usleep(sleep_time);
	}
	pthread_exit(NULL);


}

void terminar_color(int signal){
	term_color = 0;
	close(fd_color);
}


void init_I2C_color(){
	char i2cFile[15]; //Donde se almacenara el nombre del dispositivo
	int device = 1;   //En caso de que tengamos mas de un dispositivo controlado por i2c
	int addr = 0x29;  //La direccion del MPU6050 por defecto

	/*
	Ponemos la direccion del dispositivo en el array de caracteres (es C) y abrimos un descriptor mediante la fucnion open(),
	esta nos devuelve un int que nos servira para referenciarlo durante todo el programa.
	*/

	sprintf(i2cFile, "/dev/i2c-%d", device);
	fd_color = open(i2cFile, O_RDWR);   //Se puede escribir y leer

	ioctl(fd_color, I2C_SLAVE, addr);   //Le pasamos la direccion del esclavo, y le decimos que actue como tal
}

void power_color(int on){
	char write_bytes [2];
	write_bytes[0] = COMMAND;
	write_bytes[1] = on;

	write(fd_color, write_bytes, 2);	//Escribimos en el registro de POWER
}


void readRGBC(){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	//Empezamos la medida
	write_bytes[0] = COMMAND | ENABLE_REG;
	write_bytes[1] = 0x03;
	write(fd_color, write_bytes, 2);

	//Esperamoos 700ms
	usleep(700000);

	//Ponemos el puntero en el primer registro
	write_bytes[0] = COMMAND | CLEAR_REG;
	write(fd_color, write_bytes, 1);

	read(fd_color, read_bytes, 8);

	color_data.clearence= (read_bytes[1] << 8) | read_bytes[0]; //Concatenar
	int16_t red_raw = (read_bytes[3] << 8) | read_bytes[2];   //Concatenar
	int16_t green_raw = (read_bytes[5] << 8) | read_bytes[4]; //Concatenar
	int16_t blue_raw = (read_bytes[7] << 8) | read_bytes[6];  //Concatenar

	color_data.red = (int) (((float)red_raw / color_data.clearence)*255); //Sensibilidad pasada como parametro
	color_data.green = (int) (((float)green_raw / color_data.clearence)*255); //Sensibilidad pasada como parametro
	color_data.blue = (int) (((float)blue_raw / color_data.clearence)*255); //Sensibilidad pasada como parametro
}

void conf_rgbc(int rgbc_cycles, int rgbc_gain){
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	//Configuramos los ciclos de integracion
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

	//Configuramos la ganancia
	write_bytes[0] = COMMAND | GAIN_REG;
	write_bytes[1] = 0x00;
	switch(rgbc_cycles){
		case 1: write_bytes[1] = 0x00;
		break;
		case 4: write_bytes[1] = 0x01;
		break;
		case 16: write_bytes[1] = 0x02;
		break;
		case 60: write_bytes[1] = 0x03;
		break;
		default: //fprintf(stderr, "Ganancia no soportada usando 1 por defecto");
	}
	write(fd_color, write_bytes, 2);
}


