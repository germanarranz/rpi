/*
 * main.c
 *
 *  Created on: Mar 25, 2024
 *      Author: Germán Arranz y Gonzalo Castillo
 */

#include "main.h"
#include "color.h"
#include "acc.h"

int bucle = 1;
int term_acc, term_color;

t_data data;

pthread_t th_acc, th_color;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void close_all(int signal);

int main(int argc, char *argv[]) {

	uso(argv, argc);

    signal(SIGINT, close_all);

    void *arguments[2] = {&argc, argv};

    term_acc = 1;
    term_color = 1;

    pthread_create(&th_color, NULL, color, arguments);
    pthread_create(&th_acc, NULL, acc, arguments);
    char buffer[300];
    char command[400];

    while (bucle){

    	data.acc_sens = atoi(argv[1]);
    	data.gyro_sens = atoi(argv[2]);

    	signal(SIGINT, close_all);
    	system("clear");
        printf("=========== Datos de la Aplicación ===========\n");
        printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g SENSIBILIDAD: %d g\n", data.acc_data.acc_x, data.acc_data.acc_y, data.acc_data.acc_z, data.acc_sens);
        printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº     SENSIBILIDAD: %d º\n", data.acc_data.gyro_x, data.acc_data.gyro_y, data.acc_data.gyro_z, data.gyro_sens);
        printf("   Temperatura: %.2f ºC\n", data.acc_data.temp);
        printf("   Color RGB: (%d R, %d G, %d B)\n", data.color_data.red, data.color_data.green, data.color_data.blue);
        printf("==============================================\n");
        fflush(stdout);

        int j = sprintf(buffer,"{temperature:%.2f,",data.acc_data.temp);
        j += sprintf(buffer+j,"AccelerometerX:%.2f,",data.acc_data.acc_x);
        j += sprintf(buffer+j,"AccelerometerY:%.2f,",data.acc_data.acc_y);
        j += sprintf(buffer+j,"AccelerometerZ:%.2f,",data.acc_data.acc_z);
        j += sprintf(buffer+j,"GyroscopeX:%.2f,",data.acc_data.gyro_x);
        j += sprintf(buffer+j,"GyroscopeY:%.2f,",data.acc_data.gyro_y);
        j += sprintf(buffer+j,"GyroscopeZ:%.2f,",data.acc_data.gyro_z);
        j += sprintf(buffer+j,"Red:%d,",data.color_data.red);
        j += sprintf(buffer+j,"Green:%d,",data.color_data.green);
        j += sprintf(buffer+j,"Blue:%d,",data.color_data.blue);
        j += sprintf(buffer+j,"Clearance:%d}",data.color_data.clearence);

        sprintf(command, "mosquitto_pub -d -q 1 -h %s -p 1883 -t v1/devices/me/telemetry -u \"YJJmywXOiWAb6B9mK5aI\" -m \"%s\"", argv[5], buffer);

        system(command);

        sleep(1);

    }

    pthread_join(th_acc, NULL);
    pthread_join(th_color, NULL);

    printf("Adios");

    return 0;
}

/*
 * @function close_all
 * @abstarct Closes all the threads and the main function
 *
 * @param SIGINT
 *
 * @result None
 *
 * */


void close_all(int signal) {
    bucle = 0;
    term_acc = 0;
    term_color = 0;
    pthread_mutex_destroy(&lock);
    printf("\n");

}

void uso(char *argv[], int argc){
	if(argc < 4){
			fprintf(stderr,"Este programa mide la respuesta de un acelerometro y un giroscopio integrado, ademas de controlar la temperatura interna del mismo y proprorcionar medidas RGB. Necesita 5 argumentos:\n");
			fprintf(stderr,"\t- Fondo de escala del acelerometro como 2, 4, 8 o 16 g (+/-).\n");
			fprintf(stderr,"\t- Fondo de escala del giroscopio como 250, 500, 1000 o 2000 º/s.\n");
			fprintf(stderr,"\t- Ciclos de integración del sensor de color como 1, 10, 42, 64 o 256.\n");
			fprintf(stderr,"\t- Ganancia del sensor de color como 1, 4, 16 o 60.\n");
			fprintf(stderr,"\t- Dirección IP del servidor de ThingsBoard.\n");
			fprintf(stderr,"Por ejemplo escriba en la ventana de comandos: ./sensores 2 250 1 1 127.0.0.1 como ejemplo de medidas cada segundo con fondo de escala de +-2g y 250 º/s, ciclos de integración 1 y ganancia 1\n\n");
			exit(0);
		}
	else{
			int rangoAcc = atoi(argv[1]);
			if(rangoAcc<=0 || rangoAcc >16) {
				fprintf(stderr, "El rango del acelerometro debe tener un valor de 2, 4, 8 o 16 g\n");
				exit(0);
			}
			int rangoGyro = atoi(argv[2]);
			if(rangoGyro != 250 && rangoGyro != 500 && rangoGyro != 1000 && rangoGyro != 2000){
				fprintf(stderr, "El rango del giroscopio debe tener un valor de 250, 500, 1000 o 2000 º/s\n");
				exit(0);
			}
			int rgcb_cycles = atoi(argv[3]);
			if(rgcb_cycles != 1 && rgcb_cycles != 10 && rgcb_cycles != 42 && rgcb_cycles != 64 && rgcb_cycles != 256){
				fprintf(stderr, "Los ciclos de integracion han de tener un valor de 1, 10, 42, 64, 256\n");
				exit(0);
			}
			int rgbc_gain = atoi(argv[4]);
			if(rgbc_gain != 1 && rgbc_gain != 4 && rgbc_gain != 16 && rgbc_gain != 60){
				fprintf(stderr, "La ganancia ha de tener un valor de 1, 4, 16, 60\n");
				exit(0);
			}
		}

}
