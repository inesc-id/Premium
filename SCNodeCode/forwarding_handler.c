/*
 * ForwardingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/tcp.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "debug.h"
#include "register_dep.h"
#include "stun_public_ip.h"

#define LENGTH 512

char* getPrimaryIP(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[5];
	char addr[20];

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
	if(DEBUG) printf("addr %s\n", addr);
	return addr;
}

int start(char* addr, char* port, char* pub_addr){
	int fwdSocket, newSocket;
	char buffer[1024];
	struct sockaddr_in fwdAddr;
	struct sockaddr_storage fwdStorage;
	socklen_t addr_size;
	char *tmp_addr, *src_addr, *dst_addr, *type; //type add or remove rules
	const char s[2] = "-";

	fwdSocket = socket(PF_INET, SOCK_STREAM, 0);
	fwdAddr.sin_family = AF_INET;
	fwdAddr.sin_port = htons(atoi(port));
	fwdAddr.sin_addr.s_addr = inet_addr(addr);
	memset(fwdAddr.sin_zero, '\0', sizeof fwdAddr.sin_zero);
	bind(fwdSocket, (struct sockaddr *) &fwdAddr, sizeof(fwdAddr));

	if (listen(fwdSocket, 10) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	addr_size = sizeof fwdStorage;

	while(1){
		newSocket = accept(fwdSocket, (struct sockaddr *) &fwdStorage,&addr_size);
		recv(newSocket, buffer, LENGTH, 0);
		type = strtok(buffer, s);
		src_addr = strtok(NULL, s);
		dst_addr = strtok(NULL, s);
		printf("TYPE IS: %s\n", type);
		if(!strcmp(type, "A"))
			forwardPaths(addr, src_addr,  dst_addr);
		else removeRules(addr, src_addr, dst_addr);
		send(newSocket, type, LENGTH, 0);
	}
	close(newSocket);
	close(fwdSocket);
	return 0;	
}


int forwardPaths(char *address, char *source_addr, char *dest_addr) {
	char command[500];
	//iptables -t nat -A PREROUTING -p tcp -s 1.1.1.1 -d 2.2.2.2 -j DNAT --to-destination 3.3.3.3
	//iptables -t nat -A POSTROUTING -p tcp -s 1.1.1.1 -d 3.3.3.3 -j SNAT --to-source 2.2.2.2
	//iptables -t nat -A PREROUTING -p tcp -s 3.3.3.3 -d 2.2.2.2 -j DNAT --to-destination 1.1.1.1
	//iptables -t nat -A POSTROUTING -p tcp -s 3.3.3.3 -d 1.1.1.1 -j SNAT --to-source 2.2.2.2
	strcpy(command, "iptables -t nat -A PREROUTING -p tcp --dport 9898 -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, address);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, dest_addr);
	printf("Command 1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A POSTROUTING -p tcp -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, dest_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, address);
	printf("Command 2: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A PREROUTING -p tcp --dport 9898 -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, address);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, source_addr);
	printf("Command 3: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A POSTROUTING -p tcp -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, source_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, address);
	printf("Command 4: %s\n", command);
	system(command);
	return 1;
}

int removeRules(char *address, char *source_addr, char *dest_addr){
	char command[500];
	strcpy(command, "iptables -t nat -D PREROUTING -p tcp --dport 9898 -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, address);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, dest_addr);
	printf("Command 1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D POSTROUTING -p tcp -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, dest_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, address);
	printf("Command 2: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D PREROUTING -p tcp --dport 9898 -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, address);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, source_addr);
	printf("Command 3: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D POSTROUTING -p tcp -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, source_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, address);
	printf("Command 4: %s\n", command);
	system(command);
	return 1;
}

//Arg0:depspace address, Arg1:depspace port
int main(int argc, char *argv[]){
	char **primIP;
	char *myPubIP;
	primIP = malloc(sizeof(char *));
	primIP[0] = malloc(sizeof(char)*20);
	primIP[1] = malloc(sizeof(char)*20);
	strcpy(primIP[1], getPrimaryIP());
	stunGetPublicIP(primIP[0], NULL);
	if(DEBUG) printf("Registering primary IP %s\n", primIP[0]);
	registerDep(argv[1], atoi(argv[2]), 1, primIP, 2);
	if(DEBUG) printf("Done registering IP\n");
	start(primIP[1], "9899", primIP[0]);
}
