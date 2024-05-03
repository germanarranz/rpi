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

	t_data data_recv[10];
	int cnt_recived = 0;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("ERROR opening socket");
		exit(1);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = inet_addr("192.168.55.231");

	n = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if(n < 0){
			perror("ERROR binding socket");
			exit(1);
		}


	for(;;){

		addr_size = sizeof(cli_addr);

		recvfrom(sockfd, data_recv, sizeof(data_recv), 0, (struct sockaddr*)&cli_addr, &addr_size);
		cnt_recived++;
		system("clear");
		 printf("=========== Datos de la Aplicación ===========\n");
		        for(int i = 0; i < 10; i++) {
		            printf("Packet: %d Muestra %d:\n", cnt_recived, i+1);
		            printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g SENSIBILIDAD: %d g\n", data_recv[i].acc_data.acc_x, data_recv[i].acc_data.acc_y, data_recv[i].acc_data.acc_z, data_recv[i].acc_sens);
		            printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº     SENSIBILIDAD: %d º\n", data_recv[i].acc_data.gyro_x, data_recv[i].acc_data.gyro_y, data_recv[i].acc_data.gyro_z, data_recv[i].gyro_sens);
		            printf("   Temperatura: %.2f ºC\n", data_recv[i].acc_data.temp);
		            printf("   Color RGB: (%d R, %d G, %d B)\n", data_recv[i].color_data.red, data_recv[i].color_data.green, data_recv[i].color_data.blue);
		            printf("----------------------------------------------\n");
		        }
		        fflush(stdout);
		        char ack[40];
		        sprintf(ack, "ACK: %d packets sent and recived", cnt_recived);
		        sendto(sockfd, ack, sizeof(ack), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr));


	}


	return 0;

}
