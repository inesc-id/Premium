/*
/*
 * RegisterDep.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: feline4
 */

#include "register_dep.h"
#include <unistd.h> // for close
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include "debug.h"

#define LENGTH 512

//Type 1=device, Type 2=node
int registerDep(char *manager_proxy_addr, int manager_proxy_port, char *primary_addr)
{
	int depSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	if(DEBUG) printf("[Reg DepSpace] Connectiong to %s:%d\n", manager_proxy_addr, manager_proxy_port);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(manager_proxy_port);
	serverAddr.sin_addr.s_addr = inet_addr(manager_proxy_addr);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);

	// result = 0 if connection is successful
	if(DEBUG)printf("[Reg DepSpace] Connecting to receiving handler: %d\n", result);
	
	strcpy(buffer, "out ");
	strcat(buffer, "node,");
	strcat(buffer, primary_addr);
	strcat(buffer, "\n");

	if(DEBUG)printf("[Reg DepSpace] Registering with command \"%s\"...\n", buffer);

	// Send registration command
	if(DEBUG)printf("[Reg DepSpace] BUFFER SIZE: %d\n", (int)strlen(buffer));		
	int send_res = send(depSocket, buffer, strlen(buffer), 0);
	bzero(buffer, strlen(buffer));
	
	if(DEBUG) {
		if (send_res == -1) {
			printf("[Reg DepSpace] Failed to send first command...\n");

		} else {
			printf("[Reg DepSpace] Sent first command...\n");
		}
	}
	
	// Checking if insertion was OK pr ERROR
	int recv_res;
	while (recv_res = recv(depSocket, buffer, LENGTH, 0)) {
		if(DEBUG) printf("[Reg DepSpace] Adapter response: %s\n", buffer);
		if(strstr(buffer, "OK") != NULL) {
			if(DEBUG) printf("[Reg DepSpace] Registration was successful!\n");
		} else {
			if(DEBUG) printf("[Reg DepSpace] Registration failed!\n");
		}
		break;
	}

	printf("[Reg DepSpace] Closing DepSpace communication socket!\n");
	close(depSocket);

	return 1;
}

