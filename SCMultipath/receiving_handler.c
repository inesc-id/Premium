/*
 * ReceivingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "debug.h"
#include "register_dep.h"
#include "stun_public_ip.h"
#include "receiving_handler.h"

#define LENGTH 512

char *srcs[20];
int numSrcs;
char primary_ip[20];


void getPrimary(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];
	char addr[20];

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			strcpy(primary_ip, inet_ntoa(sa->sin_addr));
			break;
		}
	}
	if(DEBUG) printf("API: got primary\n");
	freeifaddrs(ifap);
}


void getLocalInterfaces2(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];
	char addr[20];

	if(DEBUG) printf("Receiving Handler: getting local interfaces\n");
	int i=0;
	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			srcs[i] = (char*)malloc(sizeof(char)*50);
			strcpy(srcs[i], inet_ntoa(sa->sin_addr));
			i++;
		}
	}
	numSrcs = i;
	if(DEBUG) printf("Receiving Handler: ----------------------\n");
	if(DEBUG) printf("Receiving Handler: local interfaces check :) \n");
	if(DEBUG){
			printf("Receiving Handler: Num srcs registered: %d\n", numSrcs);
			for(i=0; i<numSrcs; i++){
				printf("%s\n", srcs[i]);
			}
			printf("\n");
	}
	if(DEBUG) printf("Receiving Handler: ----------------------\n");
	freeifaddrs(ifap);
}

void *receiveData(void *args){
	char buffer[1024];
	int block = 0;
	int newSocket = (intptr_t)args;
	while(recv(newSocket, buffer, LENGTH, 0)){
		//Open file to send
		printf("File name: %s\n", buffer);
		break;
	}
	FILE *fp;
	fp = fopen(buffer, "w");
	bzero(buffer, LENGTH);
	while (block = recv(newSocket, buffer, LENGTH, 0)) {
		//printf("%s", buffer);
		fwrite(buffer, sizeof(char), block, fp);
		bzero(buffer, LENGTH);
	}
	fclose(fp);
	return NULL;
}

void *recv_handler(void *port_ptr){

	int welcomeSocket, newSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	char* filename;
	int i;
	int port = (intptr_t)port_ptr;

	getPrimary();
	getLocalInterfaces2();
	for(i = 0 ; i < numSrcs ; i++){
		stunGetPublicIP(srcs[i], srcs[i]);
	}
	if(DEBUG){
			printf("Receiving Handler: Num srcs: %d\n", numSrcs);
			for(i=0; i<numSrcs; i++){
				printf("%s\n", srcs[i]);
			}
			printf("\n");
	}
	registerDep("127.0.0.2", 7892, numSrcs, srcs, 1);
	if(DEBUG) printf("Receiving Handler: Registered on dep\n");

	welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(primary_ip);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	if (listen(welcomeSocket, 5) == 0)
		if(DEBUG) printf("Listening on %s:%d\n", inet_ntoa(serverAddr.sin_addr), port);
	else
		printf("Error while listening on receiving handler\n");

	addr_size = sizeof serverStorage;
	int err;
	pthread_t inc_x_thread;
	if(DEBUG) printf("RECEIVING HANDLER UP AND RUNNING\n");
 	while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size)){
		err = pthread_create(&inc_x_thread, NULL, receiveData, (void *)(intptr_t)newSocket);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n Thread created successfully\n");
	}
	close(welcomeSocket);
	return 0;
}

//Only for testing below
/*
//Arg0: address, Arg1: port
int main(int argc, char *argv[]) {
	printf("Args: %s\n", argv[1]);
	recv_handler(argv[1]);

}*/


