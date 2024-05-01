/*
 * server.c
 *
 *  Created on: Apr 19, 2024
 *      Author: German Arranz y Gonzalo Castillo
 */

#include "server.h"

int main(int argc, char *argv[]){

	if(argc < 2){
			fprintf(stderr, "ERROR, no port provided");
			exit(1);
	}

	int sockfd, portno, n;
	socklen_t addr_size;
	struct sockaddr_in serv_addr, cli_addr;


	struct t_data * data = malloc(16384+sizeof(struct t_data));
	if (data == NULL) {
	    perror("Failed to allocate memory");
	    exit(1);
	}


	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("ERROR opening socket");
		exit(1);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = inet_addr("192.168.182.231");

	n = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if(n < 0){
			perror("ERROR binding socket");
			exit(1);
		}


	for(;;){

		addr_size = sizeof(cli_addr);

		recvfrom(sockfd, data, sizeof(*data), 0, (struct sockaddr*)&cli_addr, &addr_size);
		system("clear");
		        printf("=========== Datos de la Aplicación ===========\n");
		        printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g SENSIBILIDAD: %d g\n", data->acc_data.acc_x, data->acc_data.acc_y, data->acc_data.acc_z, atoi(argv[2]));
		        printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº     SENSIBILIDAD: %d º\n", data->acc_data.gyro_x, data->acc_data.gyro_y, data->acc_data.gyro_z, atoi(argv[3]));
		        printf("   Temperatura: %.2f ºC\n", data->acc_data.temp);
		        printf("   Color RGB: (%d R, %d G, %d B)\n", data->color_data.red, data->color_data.green, data->color_data.blue);
		        printf("==============================================\n");
		        fflush(stdout);
		 sleep(1);

	}


	return 0;

}
