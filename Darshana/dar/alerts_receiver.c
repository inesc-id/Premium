#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // for close

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <time.h>

#include "alerts_receiver.h"
#include "hijack_alert_handler.h"
#include "monitored_paths_handler.h"

// Global variables
#define NUMBER_OF_SURVEILLED_NODES 20

// Args: <alert-receiver-ip> <alert-receiver-port>

int main(int argc, char *argv[]) {

	init();
	
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
		exit(1);
	}

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(my_ip);
	server_address.sin_port = htons(my_port);

   	printf("|INFO| Binding socket to [IP => %s, Port => %d] \n", my_ip, my_port);
	if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("|ERROR| Binding server socket! \n");
		exit(1);
	}

	clientlen = sizeof(client_address);

   	printf("|INFO| Ready to receive alerts ... \n");
	while(1) {

		//handle_darshana_client(sockfd);

		bzero(buffer,DEFAULT_BUFFER_SIZE + 1);

		n = read(sockfd, buffer, DEFAULT_BUFFER_SIZE);
		if (n < 0) {
			perror("|ERROR| Reading from the socket");
			exit(1);
		}

		printf("|!ALERT!| -> %s\n", buffer);
		char *alert_string = duplicate_string(buffer);
		handle_client_alert(alert_string);
	
	}

	return 0;
}

void init()
{
	init_alerts_history_head();
	init_monitored_paths_head();
	init_test_path_values();
}

void handle_darshana_client (int client_sock_fd) {
	int n;
	char buffer[DEFAULT_BUFFER_SIZE + 1];
	bzero(buffer,DEFAULT_BUFFER_SIZE + 1);

	n = read(client_sock_fd, buffer, DEFAULT_BUFFER_SIZE);

	if (n < 0) {
		perror("|ERROR| Reading from the socket");
		exit(1);
	}

	printf("|!ALERT!| -> %s\n", buffer);

	char *alert_string = duplicate_string(buffer);

	handle_client_alert(alert_string);
}

void handle_client_alert(char *buffer)
{
	hijack_alert alert_parsed = parse_hijack_alert(buffer);
	save_hijack_alert(alert_parsed);
	analyse_hijack_alert(alert_parsed);


	// show_all_hijack_alerts();
	show_all_paths();

}
