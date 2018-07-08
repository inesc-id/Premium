#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for close
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "alerts_receiver.h"
#include "hijack_alert_handler.h"
#include "monitored_paths_handler.h"

// Global variables
#define MAX_ALERTS_UNTIL_ALERT_PREMIUM 5

//int alert_receiver_socket_fd;

int max_dar_alerts; // maximum number of alerts to trigger PREMIUM to act accordigly

/*
 * Initialize DAR structures:
 * - alerts_history list
 * - monitored_paths list
 */
void setup_dar(int max_number_of_alerts_to_trigger)
{
	init_alerts_history_head();
	init_monitored_paths_head();

	max_dar_alerts = max_number_of_alerts_to_trigger;
	
	//init_test_path_values();
}

int init_dar_socket(char *alert_receiver_ip, int alert_receiver_port)
{
	int sockfd, newsockfd, portno, clientlen;
	char buffer[DEFAULT_BUFFER_SIZE + 1];
	struct sockaddr_in server_address, client_address;
	int n, optval;

	bzero((char *) &server_address, sizeof(server_address));


	printf("|INFO| Creating UDP socket ... \n");
	// Creating socket to receive
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
		perror("|ERROR| Could not open socket");
		return -1;
		//exit(1);
	}

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(alert_receiver_ip);
	server_address.sin_port = htons(alert_receiver_port);

   	printf("|INFO| Binding socket to [IP => %s, Port => %d] \n", alert_receiver_ip, alert_receiver_port);
	if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("|ERROR| Binding server socket! \n");
		exit(1);
	}

	clientlen = sizeof(client_address);

	printf("|INFO| Ready to receive alerts ... \n");

   	return sockfd;
}

void end_dar_socket(int alert_receiver_socket_fd)
{
	printf("|INFO| Close alert receiver socket ... \n");
	close(alert_receiver_socket_fd);
}

/*
 * Focus on a single alert
 * 1. Parses an alert from a string format into a hijack_alert structure
 * 2. Saves the hijack alert on the structures
 * 3. Analyses the hijack alert and updates the monitored paths' status
 */
void handle_client_alert(char *buffer)
{
	hijack_alert alert_parsed = parse_hijack_alert(buffer);
	save_hijack_alert(alert_parsed);
	analyse_hijack_alert(alert_parsed);

	//printf("|INFO| Showing all hijack alerts ... \n");
	//show_all_hijack_alerts();

	//printf("|INFO| Showing all monitored paths ... \n");
	//show_all_paths();
	//fflush(stdout);
	//printf("|INFO| Showing all hijacked paths ... \n");
	//show_hijacked_paths(TRUE);
}

void handle_darshana_client_loop(int client_sock_fd) 
{
	int bytes_read;
	char buffer[DEFAULT_BUFFER_SIZE + 1];
	
	int number_of_alerts_received = 0;

	while(1) {
		
		bzero(buffer,DEFAULT_BUFFER_SIZE + 1);

		bytes_read = read(client_sock_fd, buffer, DEFAULT_BUFFER_SIZE);

		if (bytes_read < 0) {
			perror("|ERROR| Reading from DAR socket");
			printf("|ERROR| DAR have been shutdown, exiting loop");
			break;
			//exit(1);
		}

		printf("|!DAR ALERT!| -> %s\n", buffer);

		char *alert_string = duplicate_string(buffer);

		handle_client_alert(alert_string);

		// [TEST] to test SIGNALing when receiving a decent amount of alerts
		number_of_alerts_received++;
		if(number_of_alerts_received >=  max_dar_alerts) {
			alert_machete_of_compromised_path();
		}
	}
}

void *handle_darshana_client_thread_function(void *args)
{
	int socket_fd = (intptr_t) args;

	printf("|INFO| Starting alerts receiver loop ... \n");
	handle_darshana_client_loop(socket_fd);
}

void add_monitored_path(char *sourceIp, char *relayIp, char *destinationIp)
{
	save_unstructured_path(sourceIp, relayIp, destinationIp);
}

void alert_machete_of_compromised_path()
{
	// SIGUSR1 and SIGUSR2 is reserved to use within an application, 
	// without interferring with other signals' function
	printf("|INFO| Raising SIGNAL to alert MACHETE of compromised paths ... \n");
	raise(SIGUSR1);
}

// Args: <alert-receiver-ip> <alert-receiver-port>
/*
int main(int argc, char *argv[])
{
	setup_dar();
	
	char* my_ip = malloc(20 * sizeof(char));
	int my_port;

	if (argc < 3) {
		printf("|ERROR| Check the given arguments... Follow this structure:\n");
		printf("|ERROR| ./alerts_receiver <alert-receiver-ip> <alert-receiver-port>\n");
		exit(1);
	} else {
		printf("|INFO| Received arguments: ip -> %s and port -> %s \n", argv[1], argv[2]);
	}

	// Setting initial arguments
	strcpy(my_ip, argv[1]);
	my_port = atoi(argv[2]);

	alert_receiver_socket_fd = initiate_alert_receiver_socket(my_ip, my_port);

	handle_darshana_client_loop(alert_receiver_socket_fd);

	return 0;
}
*/
