/*
 * ForwardingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // for close
#include <string.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include "debug.h"
#include "register_dep.h"
#include "forwarding_handler.h"
#include "stun_public_ip.h"
#include "on_rules_utils.h"

#define LENGTH 512
#define PORT_SIZE 10
#define RULE_SIZE 200
#define COMMAND_SIZE 500
#define SMALL_COMMAND_SIZE 50
#define IP_ADDR_SIZE 20
#define DARSHANA_SERVER_PORT "11130" // HARDCODED values
#define DARSHANA_RELATIVE_PATH "../../darshana/darshana/"
#define ENABLE_IPV4_FORWARDING_COMMAND "echo 1 > /proc/sys/net/ipv4/ip_forward"


pthread_t darshana_client_pthread;
pthread_t darshana_server_pthread;

char* getPrimaryIP()
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[5];
	char * addr = malloc(IP_ADDR_SIZE * sizeof(char));

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			strcpy(interface, ifa->ifa_name);
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			strcpy(addr, inet_ntoa(sa->sin_addr));
			break;
		}
	}
	freeifaddrs(ifap);
	if(DEBUG) printf("Primary addr %s\n", addr);
	return addr;
}

int start_overlay(char* overlay_addr, int overlay_port)
{
	int fwdSocket, newSocket;
	char buffer[LENGTH];
	struct sockaddr_in fwdAddr;
	struct sockaddr_storage fwdStorage;
	socklen_t addr_size;
	//char *tmp_addr, *src_addr, *dst_addr, *dst_port, *type; //type add or remove rules

	if (DEBUG) printf("[ON start] Creating Forward socket...\n");

	fwdSocket = socket(PF_INET, SOCK_STREAM, 0);
	fwdAddr.sin_family = AF_INET;
	fwdAddr.sin_port = htons(overlay_port);
	fwdAddr.sin_addr.s_addr = inet_addr(overlay_addr);
	memset(fwdAddr.sin_zero, 0, sizeof fwdAddr.sin_zero);
	bind(fwdSocket, (struct sockaddr *) &fwdAddr, sizeof(fwdAddr));

	if (listen(fwdSocket, 10) == 0) {
		printf("[ON start] Listening\n");
	} else {
		printf("[ON start] Error\n");
	}

	addr_size = sizeof fwdStorage;

	while(1) {

		if (DEBUG) printf("[ON start] Ready to accept a client ...\n");

		newSocket = accept(fwdSocket, (struct sockaddr *) &fwdStorage, &addr_size);
		
		receive_forwarding_rule_specification(overlay_addr, newSocket, buffer);

		bzero(buffer, LENGTH);

		receive_darshana_rule_specification(overlay_addr, newSocket, buffer);
	}

	if (DEBUG) printf("[ON start] Closing receiving socket...\n");
	close(newSocket);

	if (DEBUG) printf("[ON start] Closing forwarding socket...\n");
	close(fwdSocket);

	return 0;	
}

/*
 * 
 * Expression: {A,D}-SenderIP-ReceiverIP-ReceiverPort
 */
void receive_forwarding_rule_specification(char* overlay_addr, int sender_socket_fd, char buffer[])
{
	char *tmp_addr, *src_addr, *dst_addr, *dst_port, *type; //type add or remove rules
	const char s[2] = "-";
	
	recv(sender_socket_fd, buffer, RULE_SIZE, 0);

	if (DEBUG) printf("[Recv rule 1] Received this rule specification: %s\n", buffer);

	type = strtok(buffer, s);
	src_addr = strtok(NULL, s);
	dst_addr = strtok(NULL, s);
	dst_port = strtok(NULL, s);

	if (DEBUG) printf("[Recv rule 1] TYPE IS: %s\n", type);
	
	if (DEBUG) printf("[Recv rule 1] Sending back to Sender the rule type that was set...\n");
	send(sender_socket_fd, type, RULE_SIZE, 0);

	if(!strcmp(type, "A")){

		if (DEBUG) printf("[Recv rule 1] Setting NAT forwarding rules...\n");		
		forwardPaths(overlay_addr, src_addr, dst_addr, dst_port);

	} else { // "D"
	
		if (DEBUG) printf("[Recv rule 1] Removing NAT rules...\n");
		removeRules(overlay_addr, src_addr, dst_addr, dst_port);
	}
}

/*
 * 
 * Expression: {A,D}-DarIP-DarPort-ReceiverDarshanaIP-ReceiverDarshanaPort-MonitoringMode-MetricsThresholds
 */
void receive_darshana_rule_specification(char* overlay_addr, int sender_socket_fd, char buffer[])
{
	char *dar_addr, *dar_port, *dst_darshana_addr, *dst_darshana_port, 
		 *monitoring_mode, *metrics_thresholds, *type; //type add or remove rules

	const char s[2] = "-";
	
	recv(sender_socket_fd, buffer, RULE_SIZE, 0);

	if (DEBUG) printf("[Recv rule 2] Received this rule specification: %s\n", buffer);

	type = strtok(buffer, s);
	dar_addr = strtok(NULL, s);
	dar_port = strtok(NULL, s);
	dst_darshana_addr = strtok(NULL, s);
	dst_darshana_port = strtok(NULL, s);
	monitoring_mode = strtok(NULL, s);
	metrics_thresholds = strtok(NULL, s);

	if (DEBUG) printf("[Recv rule 2] TYPE IS: %s\n", type);

	if (DEBUG) printf("[Recv rule 2] Sending back to Sender the rule type that was set...\n");
	send(sender_socket_fd, type, RULE_SIZE, 0);

	if(!strcmp(type, "A")) {

		if (DEBUG) printf("[Recv rule 2] Running Darshana client ...\n");		
		run_darshana_client(overlay_addr, dar_addr, dar_port, dst_darshana_addr, 
							dst_darshana_port, monitoring_mode, metrics_thresholds);
	
	} else { // "D"

		if (DEBUG) printf("[Recv rule 2] Killing Darshana client ...\n");
		//system("killall python");
		//shutdown_darshana_client();
		pthread_cancel(darshana_client_pthread);
	}
}

/*
 * Darshana is a hijacking monitor
 * $ sh run-client-broadcaster_args.sh <monitoring-mode> <client-ip> <destination-ip> <destination-port> <lat> <hop> <path> <prop> <path-nones> <probe-period> <dar-ip,dar-port>
 * $ sh run-client-broadcaster.sh <monitoring-mode> <destination-ip> <dar-ip> <dar-port>
 *
 * Running short script version
 */
void run_darshana_client(char *node_addr, char *sender_dar_ip, char *sender_dar_port, char *dst_darshana_addr, char *dst_darshana_port, char *monitoring_mode, char *metrics_thresholds)
{
	int thread_err;
	char* command = (char*) malloc(sizeof(char) * COMMAND_SIZE);

	sprintf(command, "cd %s; sh run-client-broadcaster_args.sh %s %s %s %s %s %s,%s >> darshana-node-client.out &", 
		DARSHANA_RELATIVE_PATH, 
		monitoring_mode, node_addr, 
		dst_darshana_addr, 
		dst_darshana_port, 
		metrics_thresholds, 
		sender_dar_ip, 
		sender_dar_port);

	if(DEBUG) printf("[Run DarshanaC Instance] Run darshana for node %s\n", dst_darshana_addr);
	
	if(DEBUG) printf("[Run DarshanaC Instance] Creating a thread to run Darshana client... \n");
 	
 	// Prepare to receive data from the Overlay Nodes
	thread_err = pthread_create(&darshana_client_pthread, NULL, run_command_in_thread, command);

	if (thread_err != 0) {
		printf("[Run DarshanaC Instance] Can't create thread :[%s]\n", strerror(thread_err));
	} else {
		printf("[Run DarshanaC Instance] Thread created successfully\n");
	}
}

/*
 * Darshana is a hijacking monitor
 * This will run the server side of Darshana (for cryptographic ping)
 * $ sh run-server_args.sh <destination-ip> <destination-port>
 */
void run_darshana_server(char* receiver_ip)
{
	int thread_err;
	char* command = (char*) malloc(sizeof(char) * COMMAND_SIZE);

	sprintf(command, "cd %s; sh run-server_args.sh %s %s >> darshana-server.out &", 
		DARSHANA_RELATIVE_PATH, 
		receiver_ip, 
		DARSHANA_SERVER_PORT);

	if(DEBUG) printf("[Run DarshanaS Instance] Running Darshana server side ...\n");

	if(DEBUG) printf("[Run DarshanaS Instance] Creating a thread to run Darshana server ... \n");
 	
 	// Prepare to receive data from the Overlay Nodes
	thread_err = pthread_create(&darshana_server_pthread, NULL, run_command_in_thread, command);

	if (thread_err != 0) {
		printf("\n[Run DarshanaS Instance] Can't create thread :[%s]", strerror(thread_err));
	} else {
		printf("\n[Run DarshanaS Instance] Thread created successfully\n");
	}
}

void shutdown_darshana_client(int darshana_client_port)
{
	char* command = (char*) malloc(sizeof(char) * SMALL_COMMAND_SIZE);
	
	// kill program running on specific port
	// kill $(sudo lsof -t -i:PORT);
	sprintf(command, "kill $(sudo lsof -t -i:%d)", darshana_client_port);
	system(command);

	free(command);
}

void *run_command_in_thread(void *args)
{
	char *command = (char *) args;

	if(DEBUG) printf("[Run command pthread] %s\n", command);
	system(command);

	free(command);
}

// Arg0: executable
// Arg1: depspace-local-ip
// Arg2: depspace-local-port
// Arg3: overlay-port
// Arg4: use-stun-server (1 - Use Stun Server, 0 - Use local IPs)
int main(int argc, char *argv[])
{
	char *node_addr;
	node_addr = malloc(sizeof(char) * IP_ADDR_SIZE);

	char *manager_proxy_addr;
	manager_proxy_addr = malloc(sizeof(char) * IP_ADDR_SIZE);
	strcpy(manager_proxy_addr, argv[1]);

	int manager_proxy_port = atoi(argv[2]);
	int node_port = atoi(argv[3]);

	int use_stun_server = atoi(argv[4]);

	// If arg5 is given then assume that IP
	if(use_stun_server) {

		if(DEBUG) printf("[ON fwd main] Using public IP from Stun server...\n");
		stunGetPublicIP(node_addr, NULL);
	
	} else {
		if(DEBUG) printf("[ON fwd main] Getting primary addr...\n");
		strcpy(node_addr, getPrimaryIP());
	}

	if(DEBUG) printf("[ON fwd main] Registering Public IP %s into Depspace client at %s:%d\n", node_addr, manager_proxy_addr, manager_proxy_port);
	registerDep(manager_proxy_addr, manager_proxy_port, node_addr);

	if(DEBUG) printf("[ON fwd main] Done registering IP\n");

	// Enable IPv4 forwarding
	if(DEBUG) printf("[ON fwd main] Enable IPv4 forwarding ...\n");
	system(ENABLE_IPV4_FORWARDING_COMMAND);

	run_darshana_server(node_addr);

	if(DEBUG) printf("[ON fwd main] Overlay Node running...\n");
	start_overlay(node_addr, node_port);
}
