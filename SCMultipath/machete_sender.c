#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machete_sender.h"
#include "utils/log_utils.h"

/*
 * Specific API, more explicit
 */

int experiment_run = 1; // !!TEST!! Used for test purposes, and express on the log file the experiment number

/*
 * Setup Sender with appropriate arguments:
 *     dst_address - receiver's IP address
 *     dst_port - receiver's port
 *     depspace_proxy_ip - IP of the proxy that communicates with Multipath Manager (DepSpacito)
 *     depspace_proxy_port - port of the proxy that communicates with Multipath Manager (DepSpacito)
 *     overlay_port - Overlay node listening port to receive commands
 *     min_number_of_nodes  - minimum number of overlay nodes to use in the communication
 *     use_stun_server - indicates MACHETE should use IPs discovered by a Stun server
 */
int machete_setup_sender_spec(char* dst_address, 
	                          int dst_port, 
	                          char* depspace_proxy_ip, 
	                          int depspace_proxy_port, 
	                          int overlay_port, 
	                          int min_number_of_nodes, 
	                          int use_stun_server,
	                          int max_dar_alerts)
{

	open_log_file();

	// !!TEST!! Used for test purposes ---------------------------------
	log_message("<---------------------------");
	log_message_with_int_value("<-------- EXPERIMENT =", experiment_run);
	log_message("<---------------------------");
	experiment_run = experiment_run + 1;
	// !!TEST!! Used for test purposes ---------------------------------

	setup_sender_arg *argument = (setup_sender_arg*) malloc(sizeof(setup_sender_arg));

	argument->receiver_addr = (char*) malloc(sizeof(char)*(strlen(dst_address)+1));
	strcpy(argument->receiver_addr, dst_address);
	
	argument->receiver_port = dst_port;

	argument->depspace_proxy_addr = (char*) malloc(sizeof(char)*(strlen(depspace_proxy_ip)+1));
	strcpy(argument->depspace_proxy_addr, depspace_proxy_ip);
	
	argument->depspace_proxy_port = depspace_proxy_port;
	argument->overlay_port = overlay_port;
	argument->min_number_of_nodes = min_number_of_nodes;
	argument->use_stun_server = use_stun_server;
	argument->reaction_spec = SHUTDOWN_COMMUNICATION;
	argument->max_dar_alerts = max_dar_alerts;

	int sock_fd = setup_sender(argument);

	return sock_fd;
}

void machete_teardown_sender_spec()
{
	teardown_sender();
}

/*
 * Simple API, has some argments implicit
 */

int machete_setup_sender(char* receiver_ip, int receiver_port)
{
	return -1;
}

void machete_teardown_sender(int receiver_sock_fd)
{

}

