/*
 * api.c
 *
 *  Created on: Apr 13, 2016
 *      Author: feline4
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "debug.h"
#include "receiving_handler.h"
#include "sending_handler.h"


int main(int argc, char *argv[]) {
	pthread_t inc_x_thread;
	int err;
//	char filename[20];
//	char destination[20];
//	char port[5];

	int k;
//	err = pthread_create(&inc_x_thread, NULL, recv_handler, (void *)(intptr_t)atoi(argv[1]));
//	printf("Please insert filename and destination:port\n");
//	while(1{
//		printf("> ");
//		scanf("%s %s %s", filename, destination, port);
		//if( access(filename, F_OK) != -1){
		for(k = 0 ; k < 31 ; k++){
			send_handler("52.17.166.244", 9898, "1B");
			sleep(2);
			send_handler("52.17.166.244", 9898, "10B");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "100B");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "1k");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "10k");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "100k");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "1mb");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "10mb");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "100mb");
                        sleep(2);
			send_handler("52.17.166.244", 9898, "1gb");
                        sleep(2);	
		}
		//} else {
		//	printf("That file doesn't exist");
		//}
	}

