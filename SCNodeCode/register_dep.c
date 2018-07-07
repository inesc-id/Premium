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

	if(DEBUG)printf("Connecting to receiving handler: %d\n", result);
	strcpy(buffer, "out ");
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
	if(DEBUG)printf("Registering with command %s\n", buffer);

	if(DEBUG)printf("Sent first command\n");

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

	/*if(DEBUG) while(recv(depSocket, buffer, LENGTH, 0)){
		if(DEBUG) printf("%s\n", buffer);
	}*/

	//shutdown(depSocket, SHUT_RDWR);
	close(depSocket);

}



