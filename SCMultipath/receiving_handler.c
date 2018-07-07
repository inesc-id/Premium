/*
 * ReceivingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // for close
#include <string.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>

#include "utils/debug.h"
#include "utils/stun_public_ip.h"
#include "utils/log_utils.h"
#include "common.h"
#include "register_dep.h"
#include "receiving_handler.h"

#define COMMAND_SIZE 100

char *srcs[IP_ADDR_LENGTH];
int numSrcs;
char primary_ip[IP_ADDR_LENGTH];

pthread_t darshana_server_pthread;
pthread_t receiving_data_pthread;

/*
 * Looksup for an IP to use when creating a socket to receive data
 * It picks the first IP v4 from the first interface it finds
 */
void getPrimary()
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[IP_ADDR_LENGTH];
	char addr[IP_ADDR_LENGTH];

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			strcpy(primary_ip, inet_ntoa(sa->sin_addr));
			break;
		}
	}
	if(DEBUG) printf("[Getting Primary IP] got IP = %s\n", primary_ip);
	freeifaddrs(ifap);
}

/*
 * Looksup for interfaces and its IP addresses
 */
void getLocalInterfaces2()
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[IP_ADDR_LENGTH];
	char addr[IP_ADDR_LENGTH];

	if(DEBUG) printf("[Local Interfaces] Receiving Handler: getting local interfaces\n");
	int i = 0;
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
	if(DEBUG) printf("[Local Interfaces] Receiving Handler: ----------------------\n");
	if(DEBUG) printf("[Local Interfaces] Receiving Handler: local interfaces check :) \n");
	if(DEBUG) {
		printf("[Local Interfaces] Receiving Handler: Num srcs registered: %d\n", numSrcs);
		for(i=0; i<numSrcs; i++){
			printf("[Local Interfaces] IP #%d = %s\n", i + 1, srcs[i]);
		}
		printf("\n");
	}
	if(DEBUG) printf("[Local Interfaces] Receiving Handler: ----------------------\n");
	freeifaddrs(ifap);
}

/*
 * Darshana is a hijacking monitor
 * This will run the server side of Darshana (for cryptographic ping)
 * $ sh run-server_args.sh <destination-ip> <destination-port>
 */
void run_darshana_server(char receiver_ip[], char *receiver_port)
{
	int thread_err;
	char* command = (char*) malloc(sizeof(char) * COMMAND_SIZE);

	sprintf(command, "cd %s; sh run-server_args.sh %s %s >> darshana-server.out &", DARSHANA_RELATIVE_PATH, receiver_ip, receiver_port);

	if(DEBUG) printf("[Run DarshanaS Instance] Running Darshana server side ...\n");

	if(DEBUG) printf("[Run Darshana Instance] Creating a thread to run Darshana ... \n");
 	
 	// Prepare to receive data from the Overlay Nodes
	thread_err = pthread_create(&darshana_server_pthread, NULL, run_command_in_thread, command);

	if (thread_err != 0) {
		printf("\n[Run Darshana Instance] Can't create thread :[%s]", strerror(thread_err));
	} else {
		printf("\n[Run Darshana Instance] Thread created successfully\n");
	}
}

void *run_command_in_thread(void *args)
{
	char *command = (char *) args;

	if(DEBUG) printf("[Run command pthread] %s\n", command);
	system(command);

	free(command);
}

int setup_receiver(int receiver_port, char* depspace_proxy_ip, int depspace_proxy_port, 
				   int use_stun_public_ip, void *(*receive_data_function)(void*))
{
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	int i;

	// Get primary IP aka the first IP available at the machine
	getPrimary();

	// Get all available interfaces and sets srcs[] with local IPs
	getLocalInterfaces2();

	if (use_stun_public_ip) {
		// Find public IP for each interface
		for(i = 0; i < numSrcs; i++) {
			stunGetPublicIP(srcs[i], srcs[i]);
		}
	}

	if(DEBUG) {
		printf("[Setup Receiver] Receiving Handler - Num srcs: %d\n", numSrcs);
		for(i = 0; i < numSrcs; i++){
			printf("%s\n", srcs[i]);
		}
		printf("\n");
	}

	// Registers its primary IP and other available IPs (can be local or from Stun Server) into DepSpace
	register_to_manager(depspace_proxy_ip, depspace_proxy_port, numSrcs, srcs, 1);
	if(DEBUG) printf("[Setup Receiver] Receiving Handler: Registered on dep\n");

	run_darshana_server(primary_ip, DARSHANA_SERVER_PORT);

	int sock_fd = init_socket_communication_with_clients(receiver_port, receive_data_function);
	return sock_fd;
}

void teardown_receiver(int client_sock_fd)
{
	// kill the pthread running Darshana server
	pthread_cancel(darshana_server_pthread);
	
	// kill the pthread running receiving data
	pthread_cancel(receiving_data_pthread);

	close_socket_communication_with_clients(client_sock_fd);
}

int init_socket_communication_with_clients(int receiver_port, void *(*receive_data_function)(void*))
{
	int welcome_socket_fd, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	// Creates socket to receive data from the sender
	welcome_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(receiver_port);
	serverAddr.sin_addr.s_addr = inet_addr(primary_ip);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);
	bind(welcome_socket_fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	if (listen(welcome_socket_fd, 5) == 0) {
		if(DEBUG) printf("[Init Socket] Listening on %s:%d\n", inet_ntoa(serverAddr.sin_addr), receiver_port);
	} else {
		if(DEBUG) printf("[Init Socket] Error while listening on receiving handler\n");
	}

	addr_size = sizeof serverStorage;
	int err;
	
	if(DEBUG) printf("[Init Socket] RECEIVING HANDLER UP AND RUNNING\n");
 	
 	// Prepare to receive data from the overlay Nodes
 	while (newSocket = accept(welcome_socket_fd, (struct sockaddr *) &serverStorage, &addr_size)){
		err = pthread_create(&receiving_data_pthread, NULL, receive_data_function, (void *)(intptr_t)newSocket);
		if (err != 0) {
			printf("\n[Init Socket] Can't create thread :[%s]", strerror(err));
		} else {
			printf("\n[Init Socket] Thread created successfully\n");
		}
	}
	return welcome_socket_fd;
}

void close_socket_communication_with_clients(int client_sock_fd)
{
	if(DEBUG) printf("[End Socket] Closing receiving socket ...\n");
	close(client_sock_fd);
}
