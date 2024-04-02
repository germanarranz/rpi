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


        pthread_mutex_lock(&lock);
        printf("\rAccel: %.2f, %.2f, %.2f", acc_data.acc_x, acc_data.acc_y, acc_data.acc_z);
        fflush(stdout);
        printf("Color: %d, %d, %d   ", color_data.red, color_data.green, color_data.blue);
        pthread_mutex_unlock(&lock);
        fflush(stdout);
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
}



