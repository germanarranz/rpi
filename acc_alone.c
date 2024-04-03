/*
 * acc_alone.c
 *
 *  Created on: Apr 1, 2024
 *      Author: ubuntu
 */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <stdlib.h>


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

void terminar(int cmd);
void uso(char *argv[], int argc);
int esPotencia2 (int num);
void power (int on);
void init_I2C();
void readAccel(float *accel, float accel_scale);
void readGyro(float* gyro, float gyro_scale);
float readTemp();
void conf_accel(int accel_range);
void conf_gyro(int gyro_range);

int term = 1;
int fd = 0;

int main (int argc, char *argv[]){

	uso(argv, argc);	//Comprobamos que el numero de argumentos sea correcto y que sus valores sean correctos

	int sleep_time = atoi(argv[1])*1000;
	int accel_range = atoi(argv[2]);
	float accel_scale = BASE_ACCEL_SCALE/(float)accel_range;
	int gyro_range = atoi(argv[3]);
	float gyro_scale = BASE_GYRO_SCALE/(float)gyro_range;

	signal(SIGINT, terminar);

	init_I2C();		//Inicializamos el I2C
	power(ON); 		//Encendemos el sensor

	conf_accel(accel_range); //Configuramos la sensibilidad elegida por el cliente
	conf_gyro(gyro_range); //Configuramos la sensibilidad elegida por el cliente

	while(term){

		float accel[3];
		float temp;
		float gyro[3];

		readAccel(accel, accel_scale);
		temp = readTemp();
		readGyro(gyro, gyro_scale);

		system("clear");
		printf("Acelerometro:\n");
		printf("   -Sensibilidad y fondo de escala del acelerometro: %.2f LSB/g, (+-)%d g\n", accel_scale, accel_range);
		printf("   -EJE-x: %.2f g  EJE-y: %.2f g  EJE-z: %.2f g\n", accel[0], accel[1], accel[2]);
		printf("Giroscopio:\n");
		printf("   -Sensibilidad y fondo de escala del giroscopio: %.2f LSB/º/s, (+-)%d º/s\n", gyro_scale, gyro_range);
		printf("   -EJE-x: %.2fº/s  EJE-y: %.2fº/s  EJE-z: %.2fº/s\n", gyro[0], gyro[1], gyro[2]);
		printf("Temperatura interna:\n");
		printf("   -TEMP: %.2f ºC\n", temp);
		fflush(stdout);

		usleep(sleep_time);

		/*
		write_bytes[0] = 0x3B;      //Este es el registro de los 8 primeros bits del registro X del acelerometro.
		write(fd, write_bytes, 1);     //Se lo pasamos al 6050 para que lo sepa

		read(fd, read_bytes, 14);       //Leeemos los dos bytes del eje x

		int16_t x_accel = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar
		int16_t y_accel = (read_bytes[2] << 8) | read_bytes[3]; //Concatenar
		int16_t z_accel = (read_bytes[4] << 8) | read_bytes[5]; //Concatenar
		int16_t temp = (read_bytes[6] << 8) | read_bytes[7]; //Concatenar
		int16_t x_gyro = (read_bytes[8] << 8) | read_bytes[9]; //Concatenar
		int16_t y_gyro = (read_bytes[10] << 8) | read_bytes[11]; //Concatenar
		int16_t z_gyro = (read_bytes[12] << 8) | read_bytes[13]; //Concatenar

		float accel_x = x_accel / accel_scale; //Sensibilidad especificada por el fabricante con 2g
		float accel_y = y_accel / accel_scale; //Sensibilidad especificada por el fabricante con 2g
		float accel_z = z_accel / accel_scale; //Sensibilidad especificada por el fabricante con 2g
		float temp_def = (temp/340.0) + 36.53;
		float gyro_x = x_gyro / 131.0; //Sensibilidad especificada por el fabricante con 2g
		float gyro_y = y_gyro / 131.0; //Sensibilidad especificada por el fabricante con 2g
		float gyro_z = z_gyro / 131.0; //Sensibilidad especificada por el fabricante con 2g

		system("clear");
		printf("EJE-x: %.2f g  EJE-y: %.2f g  EJE-y: %.2f g\n", accel_x, accel_y, accel_z);
		printf("Sensibilidad y rango del acelerometro: %.2f LSB, %d g\n", accel_scale, accel_range);
		printf("TEMP: %.2f ºC\n", temp_def);
		printf("EJE-x: %.2fº/s  EJE-y: %.2fº/s  EJE-y: %.2fº/s\n", gyro_x, gyro_y, gyro_z);
		fflush(stdout);
		usleep(atoi(argv[1]));
		*/
	}

	power(OFF);
	close(fd);
	printf("\nSaliendo \n");
	exit(0);
}

void terminar(int cmd){
	term = 0;
}

void uso(char *argv[], int argc){
	if(argc < 4){
		fprintf(stderr,"Este programa mide la respuesta de un acelerometro y un giroscopio integrado, ademas de controlar la temperatura interna del mismo. Necesita 3 argumentos:\n");
		fprintf(stderr,"\t- Tiempo entre medidas en milisegundos.\n");
		fprintf(stderr,"\t- Fondo de escala del acelerometro como 2, 4, 8 o 16 g (+/-).\n");
		fprintf(stderr,"\t- Fondo de escala del giroscopio como 250, 500, 1000 o 2000 º/s.\n");
		fprintf(stderr,"Por ejemplo escriba en la ventana de comandos: ./acc 1000 2 250 como ejemplo de medidas cada segundo con fondo de escala de +-2g y 250 º/s\n\n");
		exit(0);
	}
	if(atoi(argv[1]) <= 0){
		fprintf(stderr, "Introduzca un periodo de muestreo mayor que 0\n ");
		exit(0);
	}else{
		int rangoAcc = atoi(argv[2]);
		if(rangoAcc<=0 || rangoAcc >16 || esPotencia2(rangoAcc) == 1) {
			fprintf(stderr, "El rango del acelerometro debe tener un valor de 2, 4, 8 o 16 g\n");
			exit(0);
		}
		int rangoGyro = atoi(argv[3]);
		if(rangoGyro != 250 && rangoGyro != 500 && rangoGyro != 1000 && rangoGyro != 2000){
			fprintf(stderr, "El rango del giroscopio debe tener un valor de 250, 500, 1000 o 2000 º/s\n");
			exit(0);
		}
	}
}

int esPotencia2 (int num){
	while(num != 1){
		if(num%2 != 0){
			return 1;
		}
		num = num/2;
	}
	return 0;
}

void power (int on){
	char write_bytes [2];
	write_bytes[0] = 0x6B;
	write_bytes[1] = on;

	write(fd, write_bytes, 2);	//Escribimos en el registro de POWER
}

void init_I2C(){

	char i2cFile[15]; //Donde se almacenara el nombre del dispositivo
	int device = 1;   //En caso de que tengamos mas de un dispositivo controlado por i2c
	int addr = 0x68;  //La direccion del MPU6050 por defecto

	/*
	Ponemos la direccion del dispositivo en el array de caracteres (es C) y abrimos un descriptor mediante la fucnion open(),
	esta nos devuelve un int que nos servira para referenciarlo durante todo el programa.
	*/

	sprintf(i2cFile, "/dev/i2c-%d", device);
	fd = open(i2cFile, O_RDWR);   //Se puede escribir y leer

	ioctl(fd, I2C_SLAVE, addr);   //Le pasamos la direccion del esclavo, y le decimos que actue como tal
}

void readAccel(float* accel, float accel_scale){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	write_bytes[0] = ACCEL_REG;     //Este es el registro de los 8 primeros bits del registro X del acelerometro.
	write(fd, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd, read_bytes, 6);        //Los 6 registros de aceleracion

	int16_t x_accel = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar
	int16_t y_accel = (read_bytes[2] << 8) | read_bytes[3]; //Concatenar
	int16_t z_accel = (read_bytes[4] << 8) | read_bytes[5]; //Concatenar

	accel[0] = x_accel / accel_scale; //Sensibilidad pasada como parametro
	accel[1] = y_accel / accel_scale; //Sensibilidad pasada como parametro
	accel[2] = z_accel / accel_scale; //Sensibilidad pasada como parametro
}

float readTemp(){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.
	float temp;

	write_bytes[0] = TEMP_REG;      //Este es el registro de la temperatura
	write(fd, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd, read_bytes, 2);       	//Los 2 registros de temperatura
	int16_t temper = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar

	temp = (temper/340.0) + 36.53;

	return temp;
}

void readGyro(float* gyro, float gyro_scale){
	char read_bytes[MAX_READ];  	//Buffer donde almacenaremos los bits de lectura
	char write_bytes[MAX_WRITE];  	//Buffer para los bits de escritura.

	write_bytes[0] = GYRO_REG;      //Este es el registro de inicio del giroscopio.
	write(fd, write_bytes, 1);     	//Se lo pasamos al 6050 para que lo sepa
	read(fd, read_bytes, 6);       	//Los 6 registros del giroscopio

	int16_t x_gyro = (read_bytes[0] << 8) | read_bytes[1]; //Concatenar
	int16_t y_gyro = (read_bytes[2] << 8) | read_bytes[3]; //Concatenar
	int16_t z_gyro = (read_bytes[4] << 8) | read_bytes[5]; //Concatenar

	gyro[0] = x_gyro / gyro_scale; //Sensibilidad pasada como parametro
	gyro[1] = y_gyro / gyro_scale; //Sensibilidad pasada como parametro
	gyro[2] = z_gyro / gyro_scale; //Sensibilidad pasada como parametro
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
	write(fd, write_bytes, 2);
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
	write(fd, write_bytes, 2);
}



