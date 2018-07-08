#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/time.h>
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
#include "utils/debug.h"
#include "utils/stun_public_ip.h"
#include "utils/connectivity_utils.h"
#include "utils/log_utils.h"
#include "common.h"

int initiate_overlay_node_comn_socket(char* hop_ip, int hop_port)
{
	if(DEBUG) printf("[Sending rule] Going to send rule to hop %s:%d\n", hop_ip, hop_port);

	int fwdSocket;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	fwdSocket = socket(AF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(hop_port);
	serverAddr.sin_addr.s_addr = inet_addr(hop_ip);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(fwdSocket, (struct sockaddr *) &serverAddr, addr_size);

	if(DEBUG) printf("[Sending rule] Result of connection: %d\n", result);

	return fwdSocket;
}

void close_overlay_node_comn(int socket_fd)
{
	close(socket_fd);
}

/* 
 * Sets up the Overlay Nodes
 * 1. Sends rule spec to set up iptables forwarding rule
 * 2. Sends rule spec to run Darshana to the destination
 */
void setup_overlay_node(char* hop_ip, int hop_port, char* forwarding_rule, char* darshana_rule)
{

	int overlay_node_socket_fd;
	struct timeval *setup_forward_rules_time, *setup_darshana_node_time;

	if(DEBUG) printf("[Setup Hop] Setting up Overlay node %s\n", hop_ip);

	overlay_node_socket_fd = initiate_overlay_node_comn_socket(hop_ip, hop_port);

	// send forwarding rules spec

	setup_forward_rules_time = begin_clock();

	send(overlay_node_socket_fd, forwarding_rule, RULE_SIZE, 0);
	bzero(forwarding_rule, RULE_SIZE);	
	
	while(recv(overlay_node_socket_fd, forwarding_rule, RULE_SIZE, 0) > 0) {
		printf("[Sending rule] Strlen of forwarding rule buffer: %d\n", (int)strlen(forwarding_rule));
		if (strlen(forwarding_rule) > 0) break;
	}

	end_clock(setup_forward_rules_time, "Setup node's forward rules time");

	// send darshana rules spec

	setup_darshana_node_time = begin_clock();

	send(overlay_node_socket_fd, darshana_rule, RULE_SIZE, 0);
	bzero(darshana_rule, RULE_SIZE);	
	
	while(recv(overlay_node_socket_fd, darshana_rule, RULE_SIZE, 0) > 0) {
		printf("[Sending rule] Strlen of darshana rule buffer: %d\n", (int)strlen(darshana_rule));
		if (strlen(darshana_rule) > 0) break;
	}

	end_clock(setup_darshana_node_time, "Setup node's darshana time");

	// reset rules buffer

	bzero(forwarding_rule, RULE_SIZE);
	bzero(darshana_rule, RULE_SIZE);

	close_overlay_node_comn(overlay_node_socket_fd);
}

/*
 * Builds rule expression to send to an Overlay Node
 * After receiving rule expression Overlay Nodes creates its version of the iptables rules 
 * Expression: {A,D}-SenderIP-ReceiverIP-ReceiverPort
 * A = append rule
 * D = deletes rule
 * type argument: 1 to append rule; 0 to delete rule
 */
char* build_hop_forward_rules_spec(char *source_ip, char *destination_ip, int destination_port, int type)
{
	char* rule = (char*) malloc(sizeof(char) * RULE_SIZE);

	// convert port from int to char
	char dst_port_str[PORT_SIZE];
	sprintf(dst_port_str, "%d", destination_port);

	if (type == 1) {
		strcpy(rule, "A-");
	} else {
		strcpy(rule, "D-");
	}
	strcat(rule, source_ip);
	strcat(rule, "-");
	strcat(rule, destination_ip);
	strcat(rule, "-");
	strcat(rule, dst_port_str);

	if(DEBUG) printf("[Sending rule] Preparing to send FORWARD rule spec: %s\n", rule);

	return rule;
}

/*
 * Select X overlay nodes from N overlay nodes returned by DepSpacito
 * - min_number_overlay_nodes -> minimum number of overlay nodes required (without extras)
 * - all_nodes -> all the overlay nodes in the Overlay Network returned by the DepSpace
 * - selected_nodes -> nodes selected from $all_nodes including extras maybe?
 */
void select_overlay_nodes(int min_number_overlay_nodes, char **all_nodes, char **selected_nodes)
{

}

void validate_number_of_hops(int num_responsive_hops, int min_number_of_nodes)
{
	if (num_responsive_hops < min_number_of_nodes)
	{
		printf("[Setup Sender] !! Error !! Not enough hops to work with! \n");
		printf("[Setup Sender] !! Error !! Responsive hops (%d) < Minimum number of hops required (%d) \n", num_responsive_hops, min_number_of_nodes);
		exit(-1);
	
	} else if (num_responsive_hops == min_number_of_nodes) {
		printf("[Setup Sender] !! Warning !! Responsive hops = Minimum number of hops required = %d \n", num_responsive_hops);
		printf("[Setup Sender] !! Warning !! Won't be able to use the reactive feature! \n");
	
	} else {
		printf("[Setup Sender] OK! Enough hops to use the reactive feature! \n");
	
	}
}
