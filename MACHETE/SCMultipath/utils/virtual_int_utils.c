#include <stdio.h>
#include <resolv.h>
#include <netdb.h>
#include <unistd.h> // for close
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "debug.h"
#include "connectivity_utils.h"

void get_local_interfaces(char *primary_interface, int *number_of_sources, int *number_of_destinations, int *number_of_hops, char **srcs, char **dsts)
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];

	if(DEBUG) printf("[Local Interfaces] Sending Handler: getting local interfaces\n");
	int i=0;
	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			strcpy(primary_interface, ifa->ifa_name);
			sa = (struct sockaddr_in *) ifa->ifa_addr;
		//	srcs[i] = (char*)malloc(sizeof(char)*20);
			strcpy(srcs[i], inet_ntoa(sa->sin_addr));
			i++;
		}
	}

	*number_of_sources = i;

	if(DEBUG) printf("[Local Interfaces] Sending Handler: local interfaces check :) \n");
	if(DEBUG){
		printf("[Local Interfaces] Sending Handler: Num srcs: %d\n", *number_of_sources);
		for(i = 0; i < *number_of_sources; i++){
			printf("[Local Interfaces] IP #%d = %s\n", i + 1, srcs[i]);
		}
		printf("\n");

		printf("[Local Interfaces] Sending Handler: Num dsts: %d\n", *number_of_destinations);
		for(i = 0; i < *number_of_destinations; i++){
			printf("%s\n", dsts[i]);
		}
		printf("\n");
	}
	freeifaddrs(ifap);
	printf("[Local Interfaces] last part %d\n", *number_of_hops);
    if(DEBUG) printf("[Local Interfaces] Sending Handler: ----------------------\n");
}

/*
 * Creates alias for available interfaces
 * Assigns new addresses for alias
 * Ex: ifconfig <interface>:<IP4> <IP1,IP2,IP3,IP4>
 */
void create_virtual_ifconfig(int addition, char *primary_interface)
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char pt1[4], pt2[4], pt3[4], pt4[4], *tmpc;
	char addr[20];
	const char s[2] = ".";

	if(DEBUG) printf("[Create Virtual Ifconfig] Creating alias for available interfaces\n");

	getifaddrs(&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if ((strcmp(primary_interface, ifa->ifa_name) == 0) && (ifa->ifa_addr->sa_family==AF_INET)) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			strcpy(addr, inet_ntoa(sa->sin_addr));
			break;
		}
	}
	freeifaddrs(ifap);
	if(DEBUG) printf("[Create Virtual Ifconfig] addr %s\n", addr);

	// divide address
	tmpc = strtok(addr, s);
	strcpy(pt1, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt2, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt3, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt4, tmpc);

	// to int and back to char
	int tmp;

	while(1) {
		tmp = atoi(pt4);
		tmp = tmp + addition;
		sprintf(pt4, "%d", tmp);
		if(DEBUG) printf("[Create Virtual Ifconfig] new pt4 %s\n", pt4);

		//rebuild ip
		strcpy(addr, pt1);
		strcat(addr, ".");
		strcat(addr, pt2);
		strcat(addr, ".");
		strcat(addr, pt3);
		strcat(addr, ".");
		strcat(addr, pt4);

		if (ping_overlay_node(addr) == -1) {
			if(DEBUG) printf("[Create Virtual Ifconfig] Ping failed, address is free\n");
			break;
		}
	}

	if(DEBUG) printf("[Create Virtual Ifconfig] new addr %s\n", addr);

	//create command
	char command[100];
	strcpy(command, "ifconfig ");
	strcat(command, primary_interface);
	strcat(command, ":");
	strcat(command, pt4);
	strcat(command, " ");
	strcat(command, addr);
	if(DEBUG) printf("[Create Virtual Ifconfig] System command -> %s\n", command);
	system(command);
}

/*
 * Creates a temporary interface to use while sending data to overlay nodes
 */
void create_virtual_interface(int addition, char *primary_interface)
{
	char interface[4] = "mpc";
	char *number = malloc(sizeof(char));
	sprintf(number, "%d", time(NULL));
	strcat(interface, number);

	char command[100];
	strcpy(command, "ip link add name ");
	strcat(command, interface);
	strcat(command, " link ");
	strcat(command, primary_interface);
	strcat(command, " type macvlan");
	if(DEBUG) printf("[Create Virtual Interface] Creating new interface: %s\n", command);
	system(command);

	strcpy(command, "ip link set dev ");
	strcat(command, interface);
	strcat(command, " up");
	if(DEBUG) printf("[Create Virtual Interface] Set interface up: %s\n", command);
	system(command);

	strcpy(command, "dhclient ");
	strcat(command, interface);
	if(DEBUG) printf("[Create Virtual Interface] Requesting dhcp: %s\n", command);
	system(command);
}

/*
 * Deletes virtual interfaces previously created
 */
void clean_virtual_interfaces()
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];
	const char s = 'm';
	char command[100];

	if(DEBUG)printf("[Clean Virtual Interfaces] Cleaning interfaces\n");
	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if(DEBUG)printf("checking: %s\n", ifa->ifa_name);
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name) != 0)) && (ifa->ifa_addr->sa_family == AF_INET)) {
			strcpy(interface, ifa->ifa_name);
			if(strchr(interface, s) != NULL){
				if(DEBUG) printf("Deleting %s\n", interface);
				strcpy(command, "ifconfig ");
				strcat(command, interface);
				strcat(command, " down");
				if(DEBUG) printf("[Clean Virtual Interfaces] Command1 %s\n", command);
				system(command);

				strcpy(command, "ip link delete dev ");
				strcat(command, interface);
				if(DEBUG) printf("[Clean Virtual Interfaces] Command2 %s\n", command);
				system(command);
			}
		}
	}
	freeifaddrs(ifap);
}