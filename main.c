/*
 * main.c
 *
 *  Created on: Mar 25, 2024
 *      Author: ubuntu
 */

#include "main.h"
#include "color.h"
#include "acc.h"

int bucle = 1;
int term_acc, term_color;

data_acc acc_data;
data_color color_data;
pthread_t th_acc, th_color;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t lock_color = PTHREAD_MUTEX_INITIALIZER;

void close_all(int signal);

int main(int argc, char *argv[]) {
	uso(argv, argc);

    signal(SIGINT, close_all);

    void *arguments[2] = {&argc, argv};

    term_acc = 1;
    term_color = 1;

    if(pthread_create(&th_color, NULL, color, NULL) != 0) {
        perror("Error creating color thread");
        return 1;
    }

    if(pthread_create(&th_acc, NULL, acc, arguments) != 0) {
        perror("Error creating acc thread");
        return 1;
    }

    while (bucle) {

    	printf("\033[H");

    	    // Mostrar el menú

        //pthread_mutex_lock(&lock);
        printf("=========== Datos de la Aplicación ===========\n");
        printf("   Aceleración (x, y, z): %.2f, %.2f, %.2f\n", acc_data.acc_x, acc_data.acc_y, acc_data.acc_z);
        printf("   Giroscopio (x, y, z): %.2f, %.2f, %.2f\n", acc_data.gyro_x, acc_data.gyro_y, acc_data.gyro_z);
        printf("   Temperatura: %.2f\n", acc_data.temp);
        printf("   Color RGB: (%d, %d, %d)\n", color_data.red, color_data.green, color_data.blue);
        printf("==============================================\n");
        //pthread_mutex_unlock(&lock);

    }

    pthread_join(th_acc, NULL);
    pthread_join(th_color, NULL);

    return 0;
}

void close_all(int signal) {
    bucle = 0;
    term_acc = 0;
    term_color = 0;
    pthread_mutex_destroy(&lock);
    exit(0);
    printf("\n");
}

void uso(char *argv[], int argc){
	if(argc < 4){
			fprintf(stderr,"Este programa mide la respuesta de un acelerometro y un giroscopio integrado, ademas de controlar la temperatura interna del mismo y proprorcionar medidas RGB. Necesita 3 argumentos:\n");
			fprintf(stderr,"\t- Tiempo entre medidas en milisegundos.\n");
			fprintf(stderr,"\t- Fondo de escala del acelerometro como 2, 4, 8 o 16 g (+/-).\n");
			fprintf(stderr,"\t- Fondo de escala del giroscopio como 250, 500, 1000 o 2000 º/s.\n");
			fprintf(stderr,"Por ejemplo escriba en la ventana de comandos: ./sensores 1000 2 250 como ejemplo de medidas cada segundo con fondo de escala de +-2g y 250 º/s\n\n");
			exit(0);
		}
		if(atoi(argv[1]) <= 0){
			fprintf(stderr, "Introduzca un periodo de muestreo mayor que 0\n ");
			exit(0);
		}else{
			int rangoAcc = atoi(argv[2]);
			if(rangoAcc<=0 || rangoAcc >16) {
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



