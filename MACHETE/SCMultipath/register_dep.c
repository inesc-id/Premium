/*
 * RegisterDep.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h> // for close
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "register_dep.h"
#include "utils/config_ips.h"
#include "utils/debug.h"

#define LENGTH 512

//Type 1=device, Type 2=node
int register_to_manager(char *server, int serverport, int num_interfaces, char **interfaces, int type){

	int depSocket;
	char buffer[LENGTH], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	if(DEBUG) printf("[Reg DepSpace] Connectiong to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);

	// Clean previous entries
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
		if(DEBUG)printf("[Reg DepSpace] Registering interfaces %s\n", buffer);
	}

	// Register itself as Receiver with its primary IP and the number of available interfaces/IPs 

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

	// Register its available Receiver's IPs 

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
		if(DEBUG)printf("[Reg DepSpace] Registering interfaces %s\n", buffer);
	}

	// Sending previous commands
	int send_res = send(depSocket, buffer, strlen(buffer)+1, 0);
	bzero(buffer, LENGTH);
	
	if(DEBUG) {
		if (send_res == -1) printf("[Reg DepSpace] Failed to send two commands...\n");
		else printf("[Reg DepSpace] Sent two commands...\n");
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

/*
 * Gets all overlay nodes from Multipath Manager (DepSpace)
 */
int get_overlay_nodes_from_manager(char *server, int serverport, char **hops)
{
	int depSocket;
	char buffer[LENGTH], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	const char s[2] = ",";
    const char s2[2] = "<";
    const char s3[2] = ">";
	const char s4[2] = ";";

	if(DEBUG) printf("[Find Hops] Connecting to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);
	strcpy(buffer, "rdall node,*\n");

	if(DEBUG) printf("[Find Hops] Sending depspace cmd: %s to %s:%d\n", buffer, server, serverport);
	int send_res = send(depSocket, buffer, strlen(buffer)+1, 0);
	printf("[Find Hops] Send cmd to DepSpace result: %d\n", send_res);
	bzero(buffer, LENGTH);

	char *temp = (char*)malloc(sizeof(char)*50);
	int i = 0, size;
	int recv_res;
	while (recv_res = recv(depSocket, buffer, LENGTH, 0)) {

		if(DEBUG) printf("[Find Hops] Adapter response: %s", buffer);

		// Check if any error ocurred
		if(strstr(buffer, "ERROR") != NULL) {
			if(DEBUG) printf("[Find Hops] Could not find the overlay nodes\n");
			break;
		}

		// Check if first line indicates success
		if(strstr(buffer, "OK") != NULL) {
			if(DEBUG) printf("[Find Hops] Its all OK, ready to receive the overlay nodes\n");
			continue;
		}

		// Empty line means end of a response
		if(strcmp(buffer, "\n") == 0){
			if(DEBUG) printf("[Find Hops] END of response :)\n");
			break;
		}

		if(DEBUG) printf("[Find Hops] Receiving on REGISTERDEP tuple: %s\n", buffer);
		temp = strtok(buffer,s);
		strcpy(hops[i], strtok(NULL, s3));
		if(DEBUG) printf("[Find Hops] REGISTERDEP hop #%d->%s\n\n", i, hops[i]);
		i++;

		bzero(buffer, LENGTH);
	}
	printf("[Find Hops] Recv from DepSpace result: %d\n", recv_res);

	close(depSocket);
	return i;
}

/*
	Find Receiver's available IPs
*/
int get_receiver_info_from_manager(char *server, int serverport, char **dsts, char *prim_dest)
{
	int depSocket;
	char buffer[LENGTH], num_int[5];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	const char s[2] = ",";
	const char s2[2] = "<";
	const char s3[2] = ">";

	if(DEBUG) printf("[Find Dsts IPs] Connecting to %s:%d\n", server, serverport);

	depSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverport);
	serverAddr.sin_addr.s_addr = inet_addr(server);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(depSocket, (struct sockaddr *) &serverAddr, addr_size);
	printf("[Find Dsts IPs] Prim %s\n", prim_dest);
	printf("[Find Dsts IPs] Connected? %d\n", result);

	// Asking MPM/DepSpace the number of IPs available at the Receiver

	printf("[Find Dsts IPs] Reading Receiver's number of interfaces...\n");
	strcpy(buffer, "rdp device,");
	strcat(buffer, prim_dest);
	strcat(buffer, ",*\n");

	if(DEBUG) printf("[Find Dsts IPs] Sending first command: %s\n", buffer);
	int send_res = send(depSocket, buffer, strlen(buffer)+1, 0);
	bzero(buffer, LENGTH);

	int size = 0;
	char *size_str;

	int recv_res;
	while (recv_res = recv(depSocket, buffer, LENGTH, 0)) {
		if(DEBUG) printf("[Find Dsts IPs] Adapter response: %s\n", buffer);
		if(strstr(buffer, "OK") != NULL) {
			if(DEBUG) printf("[Find Dsts IPs] Getting Receiver's #interfaces OK!\n");
			continue;
		} 
		if(strstr(buffer, "ERROR") != NULL) {
			if(DEBUG) printf("[Find Dsts IPs] Getting Receiver's #interfaces ERROR!\n");
		}
		break;
	}

	if(DEBUG) {
		if (recv_res == -1)
		{
			printf("[Find Dsts IPs] Failed to receive response...\n");

		} else {
			printf("[Find Dsts IPs] Received the response successfully!...\n");
		}
	}

	strtok(buffer, s);
	strtok(NULL, s);
	size_str = strtok(NULL, s);
	size = atoi(strtok(size_str, s3));
	if(DEBUG) printf("[Find Dsts IPs] Getting Receiver's %d IPs!\n", size);

	int i;
	bzero(buffer, LENGTH);

	// Asking MPM/DepSpace the IPs available at the Receiver

	printf("[Find Dsts IPs] Reading { %d } Receiver's available IPs...\n", size);
	strcpy(buffer, "rdp ");
	strcat(buffer, prim_dest);
	for(i=0; i < size-1; i++){
		strcat(buffer, ",*");
	}
	strcat(buffer, "\n");
	//send(depSocket, "\n", 2, 0);
	send(depSocket, buffer, strlen(buffer)+1, 0);
	if(DEBUG) printf("[Find Dsts IPs] Sent second command: %s\n", buffer);
	bzero(buffer, LENGTH);
	while (recv(depSocket, buffer, LENGTH, 0)) {
		if(DEBUG) printf("[Find Dsts IPs] Adapter response: %s\n", buffer);
		if(strstr(buffer, "OK") != NULL) {
			if(DEBUG) printf("[Find Dsts IPs] Getting Receiver's IPs OK!\n");
			continue;
		} 
		if(strstr(buffer, "ERROR") != NULL) {
			if(DEBUG) printf("[Find Dsts IPs] Getting Receiver's IPs ERROR!\n");
		}
		break;
	}

	char *temp = (char*)malloc(sizeof(char)*50);
	if(DEBUG) printf("[Find Dsts IPs] Buffer initial form %s\n", buffer);
	strtok(buffer, s2);

	temp = strtok(NULL, s2);

	temp = strtok(temp, s3);

	// Parsing Receiver's IPs
	strcpy(dsts[0], strtok(temp, s));
	if(DEBUG) printf("[Find Dsts IPs] Receiver IP #%d -> %s\n", 0, dsts[0]);
	for(i=1; i < size; i++){
		strcpy(dsts[i], strtok(NULL, s));
		if(DEBUG) printf("[Find Dsts IPs] Receiver IP #%d -> %s\n", i, dsts[i]);
	}

	close(depSocket);
	return size;
}



