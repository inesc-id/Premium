#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for close
#include <netdb.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "utils/debug.h"
#include "dar/alerts_receiver.h"
#include "common.h"
#include "path_monitoring.h"

#define COMMAND_SIZE 500

/*
 * Global Variables
 */

int alert_receiver_socket_fd; // socket descriptor used for Darshana Alert Receiver DAR
							  // to receive Darshana alerts

pthread_t dar_pthread;        // thread running Darshana Alert Receiver (DAR) loop

pthread_t sender_darshanas[MAX_FLOWS];

/* "Path monitoring Main"
 * 1. Run alert receiver socket in a thread
 * 2. Run a Darshana instance for each path overlay node
 * 3. Command Overlay nodes to run darshana on Overlay_Manager
 * 4. Register signal interrupt
 */
void start_path_monitoring_module(char *sender_ip, int sender_port, int max_dar_alerts)
{
	// Initialize Darshana Alert Receiver (DAR) structures
	setup_dar(max_dar_alerts);

	// Initialize socket
	alert_receiver_socket_fd = init_dar_socket(sender_ip, sender_port);

	// Start DAR in a separate thread
	start_dar_handler(handle_darshana_client_thread_function);
}

int start_dar_handler(void *(*handle_darshana_alerts_function)(void*))
{
	int err;
	if(DEBUG) printf("[DAR Thread] Creating Darshana Alert Receiver (DAR) thread ...\n");
 	
	err = pthread_create(&dar_pthread, NULL, handle_darshana_alerts_function, (void *)(intptr_t)alert_receiver_socket_fd);
	if (err != 0) {
		printf("\n[DAR Thread] Can't create thread :[%s]", strerror(err));
	} else {
		printf("\n[DAR Thread] Thread created successfully\n");
	}
	return err;
}

/*
 * Gets the information from a path: source IP, overlay nodes IP and destination IP
 * Adds this as a path structure that is inspected whenever a alert is received by DAR
 */
void monitor_path(char *sourceIp, char *relayIp, char *destinationIp)
{
	if(DEBUG) printf("[Add monitored path] %s -> %s -> %s\n", sourceIp, relayIp, destinationIp);
	add_monitored_path(sourceIp, relayIp, destinationIp);
}

/*
 * Darshana is a hijacking monitor
 * Run local path monitors for each path, i.e., overlay nodes
 * Run instances for Darshana for each overlay node
 *
 * $ sh run-client-broadcaster_args.sh <monitoring-mode> <client-ip> <destination-ip> <destination-port>
 * 									   <lat> <hop> <path> <prop> <path-nones> <probe-period> 
 * 									   <dar-ip,dar-port>
 * $ sh run-client-broadcaster.sh <monitoring-mode> <destination-ip> <dar-ip> <dar-port>
 *
 * Running short script version
 */
void run_sender_darshana(char *sender_dar_ip, int sender_dar_port, char *sender_ip_assign_to_node, 
						 char *node_darshana_addr, char *node_darshana_port, int hop_index)
{
	int thread_err;
	pthread_t darshana_client_pthread;
	
	char* command = (char*) malloc(sizeof(char) * COMMAND_SIZE);

	sprintf(command, "cd %s; sh run-client-broadcaster_args.sh %s %s %s %s %s %s,%d >> darshana-client-node.out &", 
		DARSHANA_RELATIVE_PATH, 
		DARSHANA_MONITORING_MODE, 
		sender_ip_assign_to_node, 
		node_darshana_addr, 
		node_darshana_port, 
		DARSHANA_METRICS_THRESHOLDS, 
		sender_dar_ip, 
		sender_dar_port
	);

	if(DEBUG) printf("[Run Darshana Instance] Run darshana for node %s\n", node_darshana_addr);
	
	if(DEBUG) printf("[Run Darshana Instance] This command is NOT RUNNING yet!\n");
 	
 	// Prepare to receive data from the Overlay Nodes
	//thread_err = pthread_create(&darshana_client_pthread, NULL, run_command_in_thread, command);
	thread_err = pthread_create(&sender_darshanas[hop_index], NULL, run_command_in_thread, command);

	if (thread_err != 0) {
		printf("\n[Run Darshana Instance] Can't create thread :[%s]", strerror(thread_err));
	} else {
		printf("\n[Run Darshana Instance] Thread created successfully\n");
	}
}

/*
 * Terminates the thread running DAR loop that reads alerts
 * Ends UDP socket
 */
void stop_path_monitoring_module()
{
	// kill the pthread running DAR
	if(DEBUG) printf("[Stop Path Monitoring] Cancelling DAR loop thread!\n");
	
	//[TEST] This might work best for several iterations
	//pthread_cancel(dar_pthread);
	pthread_kill(dar_pthread, 0);
	
	// close alert receiver socket
	if(DEBUG) printf("[Stop Path Monitoring] Closing DAR socket!\n");
	end_dar_socket(alert_receiver_socket_fd);
}

/*
 * Utils
 */

void *run_command_in_thread(void *args)
{
	char *command = (char *) args;

	if(DEBUG) printf("[Run command pthread] %s\n", command);
	system(command);

	free(command);
}

/*
 * Builds rule expression to send to an Overlay Node
 * After receiving rule expression Overlay Nodes creates its version of the iptables rules 
 * Expression: {A,D}-DarIP-DarPort-ReceiverDarshanaIP-ReceiverDarshanaPort-MonitoringMode-MetricsThresholds
 * A = append rule
 * D = deletes rule
 * type argument: 1 to append rule; 0 to delete rule
 * broadcaster args: <monitoring-mode> <client-ip> <destination-ip> <destination-port> <lat> <hop> <path> <prop> <path-nones> <probe-period> <dar-ip,dar-port>
 */
char* build_hop_run_darshana_spec(char *sender_dar_ip, int sender_dar_port, char *darshana_target_dst_ip, 
								  int darshana_target_dst_port, int type)
{
	char* rule = (char*) malloc(sizeof(char) * RULE_SIZE);

	// convert Darshana ON port from int to char
	char darshana_target_dst_port_str[PORT_SIZE];
	sprintf(darshana_target_dst_port_str, "%d", darshana_target_dst_port);

	// convert DAR port from int to char
	char sender_dar_port_str[PORT_SIZE];
	sprintf(sender_dar_port_str, "%d", sender_dar_port);

	if (type == 1) {
		strcpy(rule, "A-");
	} else {
		strcpy(rule, "D-");
	}
	strcat(rule, sender_dar_ip);
	strcat(rule, "-");
	strcat(rule, sender_dar_port_str);
	strcat(rule, "-");
	strcat(rule, darshana_target_dst_ip);
	strcat(rule, "-");
	strcat(rule, darshana_target_dst_port_str);
	strcat(rule, "-");
	strcat(rule, DARSHANA_MONITORING_MODE);
	strcat(rule, "-");
	strcat(rule, DARSHANA_METRICS_THRESHOLDS);

	if(DEBUG) printf("[Sending rule] Preparing to send DAR rule spec: %s\n", rule);

	return rule;
}

void kill_sender_darshana_pthreads(int hop_index)
{
	pthread_cancel(sender_darshanas[hop_index]);
}
