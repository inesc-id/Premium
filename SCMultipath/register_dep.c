/*
/*
 * RegisterDep.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: feline4
 */

#include "register_dep.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"


#define LENGTH 512


//Type 1=device, Type 2=node
int registerDep(char *server, int serverport, int num_interfaces, char **interfaces, int type){

	int depSocket;
	char buffer[1024], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	if(DEBUG) printf("Connectiong to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);

	//Clean previous entries
	strcpy(buffer, "inp ");
	if(type==1)
		strcat(buffer, "device,");
	else strcat(buffer, "node,");
	sprintf(num_int, "%d", num_interfaces);
	strcat(buffer, interfaces[0]);
	if(type==1){
		strcat(buffer, ",");
		strcat(buffer, num_int);
	}
	strcat(buffer, "\n");

	if(type==1){
		strcat(buffer, "inp ");
		int i = 0;
		while(interfaces[i]!=NULL){
			strcat(buffer, interfaces[i]);
			if(interfaces[i+1]!=NULL)
				strcat(buffer, ",");
			else {
				break;
			}
			i++;
		}
		strcat(buffer, "\n");
		if(DEBUG)printf("Registering interfaces %s\n", buffer);
	}

	//Insert
	strcat(buffer, "out ");
	if(type==1)
		strcat(buffer, "device,");
	else strcat(buffer, "node,");
	sprintf(num_int, "%d", num_interfaces);
	strcat(buffer, interfaces[0]);
	if(type==1){
		strcat(buffer, ",");
		strcat(buffer, num_int);
	}
	strcat(buffer, "\n");

	if(type==1){
		strcat(buffer, "out ");
		int i = 0;
		while(interfaces[i]!=NULL){
			strcat(buffer, interfaces[i]);
			if(interfaces[i+1]!=NULL)
				strcat(buffer, ",");
			else {
				break;
			}
			i++;
		}
		strcat(buffer, "\n");
		if(DEBUG)printf("Registering interfaces %s\n", buffer);
	}
	send(depSocket, buffer, LENGTH, 0);
	close(depSocket);

	return 1;
}

int findHops(char *server, int serverport, char **hops){
	int depSocket;
	char buffer[1024], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	const char s[2] = ",";
        const char s2[2] = "<";
        const char s3[2] = ">";
	const char s4[2] = ";";

	if(DEBUG) printf("Connecting to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);
	strcpy(buffer, "rdall node,*\n");

	send(depSocket, buffer, LENGTH, 0);
	bzero(buffer, LENGTH);

	char *temp = (char*)malloc(sizeof(char)*50);
	int i, size;
	while (recv(depSocket, buffer, LENGTH, 0)) {
		if(DEBUG) printf("Receiving on REGISTERDEP %s\n", buffer);
	//	temp = strtok(buffer,s);
		sscanf(strtok(buffer,s), "%d", &size);
		if(DEBUG) printf("REGISTERDEP size: %d\n", size);
		
		for(i = 0 ; i < size ; i ++) {
			//temp = strtok(temp, s3);
	        	strcpy(hops[i], strtok(NULL, s));
			if(DEBUG) printf("REGISTERDEP hop: %d->%s\n", i, hops[i]);
		}
		bzero(buffer, LENGTH);
	}

	return i;
}

int findDsts(char *server, int serverport, char **dsts, char *prim_dest){
	int depSocket;
	char buffer[1024], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	const char s[2] = ",";
	const char s2[2] = "<";
	const char s3[2] = ">";

	if(DEBUG) printf("Connecting to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);
	printf("Prim %s\n", prim_dest);
	printf("Connected? %d\n", result);
	strcpy(buffer, "rdp device,");
	strcat(buffer, prim_dest);
	strcat(buffer, ",*\n");
	send(depSocket, buffer, strlen(buffer), 0);

	
	bzero(buffer, LENGTH);
	int size = 0;
	char *size_str;
	while (recv(depSocket, buffer, LENGTH, 0)) {
		break;
	}
	strtok(buffer, s);
	strtok(NULL, s);
	size_str = strtok(NULL, s);
	size = atoi(strtok(size_str, s3));

	int i;
	bzero(buffer, LENGTH);
	strcpy(buffer, "rdp ");
	strcat(buffer, prim_dest);
	for(i=0; i<size-1;i++){
		strcat(buffer, ",*");
	}
	strcat(buffer, "\n");
	//send(depSocket, "\n", 2, 0);
	send(depSocket, buffer, strlen(buffer), 0);
	if(DEBUG) printf("Sent second request %s\n", buffer);
	bzero(buffer, LENGTH);
	while (recv(depSocket, buffer, LENGTH, 0)) {
		break;
	}

	char *temp = (char*)malloc(sizeof(char)*50);
	if(DEBUG) printf("Buffer initial form %s\n", buffer);
	strtok(buffer, s2);

	temp = strtok(NULL, s2);

	temp = strtok(temp, s3);

	strcpy(dsts[0], strtok(temp, s));
	for(i=1;i<size;i++){
		strcpy(dsts[i], strtok(NULL, s));
	}
	return size;
}



