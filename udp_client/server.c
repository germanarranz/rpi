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
	serv_addr.sin_addr.s_addr = inet_addr("192.168.56.231");

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

		   t_data med = media(data_recv);

		   t_data devi = dev(data_recv, media(data_recv));

		   t_data maximum = max(data_recv);

		   t_data minimum = min(data_recv);

		   printf("=========== Estadísticas de los Datos =========\n");
		   printf("med:\n");
		   printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g\n", med.acc_data.acc_x, med.acc_data.acc_y, med.acc_data.acc_z);
		   printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº\n", med.acc_data.gyro_x, med.acc_data.gyro_y, med.acc_data.gyro_z);
		   printf("   Temperatura: %.2f ºC\n", med.acc_data.temp);
		   printf("   Color RGB: (%d R, %d G, %d B)\n", med.color_data.red, med.color_data.green, med.color_data.blue);
		   printf("----------------------------------------------\n");

		   printf("Standard devi:\n");
		   printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g\n", devi.acc_data.acc_x, devi.acc_data.acc_y, devi.acc_data.acc_z);
		   printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº\n", devi.acc_data.gyro_x, devi.acc_data.gyro_y, devi.acc_data.gyro_z);
		   printf("   Temperatura: %.2f ºC\n", devi.acc_data.temp);
		   printf("   Color RGB: (%d R, %d G, %d B)\n", devi.color_data.red, devi.color_data.green, devi.color_data.blue);
		   printf("----------------------------------------------\n");

		   printf("Maximum:\n");
		   printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g\n", maximum.acc_data.acc_x, maximum.acc_data.acc_y, maximum.acc_data.acc_z);
		   printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº\n", maximum.acc_data.gyro_x, maximum.acc_data.gyro_y, maximum.acc_data.gyro_z);
		   printf("   Temperatura: %.2f ºC\n", maximum.acc_data.temp);
		   printf("   Color RGB: (%d R, %d G, %d B)\n", maximum.color_data.red, maximum.color_data.green, maximum.color_data.blue);
		   printf("----------------------------------------------\n");

		   printf("Minimum:\n");
		   printf("   Aceleración (x, y, z): %.2f g, %.2f g, %.2f g\n", minimum.acc_data.acc_x, minimum.acc_data.acc_y, minimum.acc_data.acc_z);
		   printf("   Giroscopio (x, y, z): %.2fº, %.2fº, %.2fº\n", minimum.acc_data.gyro_x, minimum.acc_data.gyro_y, minimum.acc_data.gyro_z);
		   printf("   Temperatura: %.2f ºC\n", minimum.acc_data.temp);
		   printf("   Color RGB: (%d R, %d G, %d B)\n", minimum.color_data.red, minimum.color_data.green, minimum.color_data.blue);
		   printf("----------------------------------------------\n");

		   fflush(stdout);
		   char ack[40];
		   sprintf(ack, "ACK: %d packets sent and recived", cnt_recived);
		   sendto(sockfd, ack, sizeof(ack), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr));
	}

	return 0;

}

t_data media(t_data data_recv[]){

	t_data med = {0};

	for(int i = 0; i < 10; i++){

		med.acc_data.acc_x += data_recv[i].acc_data.acc_x;
		med.acc_data.acc_y += data_recv[i].acc_data.acc_y;
        med.acc_data.acc_z += data_recv[i].acc_data.acc_z;
		med.acc_data.gyro_x += data_recv[i].acc_data.gyro_x;
		med.acc_data.gyro_y += data_recv[i].acc_data.gyro_y;
		med.acc_data.gyro_z += data_recv[i].acc_data.gyro_z;
		med.acc_data.temp += data_recv[i].acc_data.temp;
		med.color_data.red += data_recv[i].color_data.red;
		med.color_data.green += data_recv[i].color_data.green;
		med.color_data.blue += data_recv[i].color_data.blue;

	}

    med.acc_data.acc_x /= 10;
    med.acc_data.acc_y /= 10;
    med.acc_data.acc_z /= 10;
    med.acc_data.gyro_x /= 10;
    med.acc_data.gyro_y /= 10;
    med.acc_data.gyro_z /= 10;
    med.acc_data.temp /= 10;
    med.color_data.red /= 10;
    med.color_data.green /= 10;
    med.color_data.blue /= 10;

    return med;

}

t_data dev(t_data data_recv[], t_data med){

	t_data dev = {0};

	for(int i = 0; i < 10; i++){

		dev.acc_data.acc_x += pow(data_recv[i].acc_data.acc_x - med.acc_data.acc_x, 2);
		dev.acc_data.acc_y += pow(data_recv[i].acc_data.acc_y - med.acc_data.acc_y, 2);
		dev.acc_data.acc_z += pow(data_recv[i].acc_data.acc_z - med.acc_data.acc_z, 2);
		dev.acc_data.gyro_x += pow(data_recv[i].acc_data.gyro_x - med.acc_data.gyro_x, 2);
		dev.acc_data.gyro_y += pow(data_recv[i].acc_data.gyro_y - med.acc_data.gyro_y, 2);
		dev.acc_data.gyro_z += pow(data_recv[i].acc_data.gyro_z - med.acc_data.gyro_z, 2);
		dev.acc_data.temp += pow(data_recv[i].acc_data.temp - med.acc_data.temp, 2);
		dev.color_data.red += pow(data_recv[i].color_data.red - med.color_data.red, 2);
		dev.color_data.green += pow(data_recv[i].color_data.green - med.color_data.green, 2);
		dev.color_data.blue += pow(data_recv[i].color_data.blue - med.color_data.blue, 2);

	}

	dev.acc_data.acc_x = sqrt(dev.acc_data.acc_x / 10);
	dev.acc_data.acc_y = sqrt(dev.acc_data.acc_y / 10);
	dev.acc_data.acc_z = sqrt(dev.acc_data.acc_z / 10);
	dev.acc_data.gyro_x = sqrt(dev.acc_data.gyro_x / 10);
	dev.acc_data.gyro_y = sqrt(dev.acc_data.gyro_y / 10);
	dev.acc_data.gyro_z = sqrt(dev.acc_data.gyro_z / 10);
	dev.acc_data.temp = sqrt(dev.acc_data.temp / 10);
	dev.color_data.red = sqrt(dev.color_data.red / 10);
	dev.color_data.green = sqrt(dev.color_data.green / 10);
	dev.color_data.blue = sqrt(dev.color_data.blue / 10);

	return dev;

}

t_data max(t_data data_recv[]){

	t_data max = data_recv[0];

	for(int i = 0; i < 10; i++){

		if (data_recv[i].acc_data.acc_x > max.acc_data.acc_x)		{max.acc_data.acc_x = data_recv[i].acc_data.acc_x;}
		if (data_recv[i].acc_data.acc_y > max.acc_data.acc_y)		{max.acc_data.acc_y = data_recv[i].acc_data.acc_y;}
		if (data_recv[i].acc_data.acc_z > max.acc_data.acc_z)		{max.acc_data.acc_z = data_recv[i].acc_data.acc_z;}
		if (data_recv[i].acc_data.gyro_x > max.acc_data.gyro_x) 	{max.acc_data.gyro_x = data_recv[i].acc_data.gyro_x;}
		if (data_recv[i].acc_data.gyro_y > max.acc_data.gyro_y)		{max.acc_data.gyro_y = data_recv[i].acc_data.gyro_y;}
		if (data_recv[i].acc_data.gyro_z > max.acc_data.gyro_z)		{max.acc_data.gyro_z = data_recv[i].acc_data.gyro_z;}
		if (data_recv[i].acc_data.temp > max.acc_data.temp)			{max.acc_data.temp = data_recv[i].acc_data.temp;}
		if (data_recv[i].color_data.red > max.color_data.red)		{max.color_data.red = data_recv[i].color_data.red;}
		if (data_recv[i].color_data.green > max.color_data.green)	{max.color_data.green = data_recv[i].color_data.green;}
		if (data_recv[i].color_data.blue > max.color_data.blue)		{max.color_data.blue = data_recv[i].color_data.blue;}

	}

	return max;

}

t_data min(t_data data_recv[]){

	t_data min = data_recv[0];

	for(int i = 0; i < 10; i++){

		if (data_recv[i].acc_data.acc_x < min.acc_data.acc_x)		{min.acc_data.acc_x = data_recv[i].acc_data.acc_x;}
		if (data_recv[i].acc_data.acc_y < min.acc_data.acc_y)		{min.acc_data.acc_y = data_recv[i].acc_data.acc_y;}
		if (data_recv[i].acc_data.acc_z < min.acc_data.acc_z)		{min.acc_data.acc_z = data_recv[i].acc_data.acc_z;}
		if (data_recv[i].acc_data.gyro_x < min.acc_data.gyro_x) 	{min.acc_data.gyro_x = data_recv[i].acc_data.gyro_x;}
		if (data_recv[i].acc_data.gyro_y < min.acc_data.gyro_y)		{min.acc_data.gyro_y = data_recv[i].acc_data.gyro_y;}
		if (data_recv[i].acc_data.gyro_z < min.acc_data.gyro_z)		{min.acc_data.gyro_z = data_recv[i].acc_data.gyro_z;}
		if (data_recv[i].acc_data.temp < min.acc_data.temp)			{min.acc_data.temp = data_recv[i].acc_data.temp;}
		if (data_recv[i].color_data.red < min.color_data.red)		{min.color_data.red = data_recv[i].color_data.red;}
		if (data_recv[i].color_data.green < min.color_data.green)	{min.color_data.green = data_recv[i].color_data.green;}
		if (data_recv[i].color_data.blue < min.color_data.blue)		{min.color_data.blue = data_recv[i].color_data.blue;}

	}

	return min;

}

