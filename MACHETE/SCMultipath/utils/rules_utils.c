#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "rules_utils.h"

#define FLUSH_NAT_TABLE_COMMAND "iptables -t nat -F"
#define COMMAND_SIZE 500

void set_sender_own_rules_iptables(char *sender_ip, char *receiver_ip, char *overlay_node_ip)
{	
	char* command = (char*) malloc(sizeof(char)*COMMAND_SIZE);

	strcpy(command, "iptables -t nat -A PREROUTING -p tcp -s ");
	strcat(command, sender_ip);
	strcat(command, " -d ");
	strcat(command, receiver_ip);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, overlay_node_ip);
	
	if(DEBUG) printf("[Setting rules] Command #1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A OUTPUT -p tcp -s ");
	strcat(command, sender_ip);
	strcat(command, " -d ");
	strcat(command, receiver_ip);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, overlay_node_ip);
	
	if(DEBUG) printf("[Setting rules] Command #2: %s\n", command);
	system(command);

	free(command);
}

void remove_sender_own_rules_iptables(char *sender_ip, char *receiver_ip, char *overlay_node_ip)
{
	char* command = (char*) malloc(sizeof(char)*COMMAND_SIZE);

	strcpy(command, "iptables -t nat -D PREROUTING -p tcp -s ");
	strcat(command, sender_ip);
	strcat(command, " -d ");
	strcat(command, receiver_ip);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, overlay_node_ip);
	
	if(DEBUG) printf("[Removing rules] Command #1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D OUTPUT -p tcp -s ");
	strcat(command, sender_ip);
	strcat(command, " -d ");
	strcat(command, receiver_ip);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, overlay_node_ip);
	
	if(DEBUG) printf("[Removing rules] Command #2: %s\n", command);
	system(command);

	free(command);
}

void flush_nat_iptables()
{
	if(DEBUG) printf("[Rules] Flush: %s\n", FLUSH_NAT_TABLE_COMMAND);
	system(FLUSH_NAT_TABLE_COMMAND);
}