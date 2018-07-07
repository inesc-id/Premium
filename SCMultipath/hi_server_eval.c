#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <ctype.h>
#include "utils/debug.h"
#include "machete_receiver.h"

struct receiver_args_struct_z {
	int receiver_port;
	char* depspace_proxy_ip;
	int depspace_proxy_port;
	int use_stun_public_ip;
};

void *echo_hello_message(void *args)
{
	int socket_fd = (intptr_t) args;
	long* message_size= malloc(sizeof(long));
	int len = 0;

	while(1) {

		// 1 - First, let's read the size of the message
		if((len = recv(socket_fd, message_size, sizeof(long), 0)) <= 0) break;
		//recv(socket_fd, message_size, sizeof(long), 0);
		if(DEBUG) printf("[ECHO] Will receive a message with: %ld bytes\n", *message_size);

		// 2 -  Now let's read the message 
		char * message =  malloc(*message_size);
		//recv(socket_fd, message, *message_size, 0);
		if((len = recv(socket_fd, message, *message_size, 0)) <= 0) break;
		if(DEBUG) printf("[ECHO] Message received: %s\n", message);

		char *echo_message =  malloc(*message_size);

		// Convert to upper case
		int i;
		for (i = 0; i < strlen(message); i++) {
			echo_message[i] = toupper(message[i]);
		}

		if(DEBUG) printf("[ECHO] Send echo message with size: %ld\n", *message_size);
		send(socket_fd, message_size, sizeof(long), 0);

		if(DEBUG) printf("[ECHO] Send echo message: %s\n", echo_message);
		send(socket_fd, echo_message, *message_size, 0);

		if(DEBUG) printf("[ECHO] Sent new message!\n");

		*message_size = 0;
		//memset(message, 0, strlen(message));
		//memset(echo_message, 0, strlen(echo_message));

		free(message);
		free(echo_message);

		if(DEBUG) printf("[ECHO] End echo protocol ----------- !\n");
	}

	return NULL;
}

// Receiver args: <MODE> <DEPSPACE_PROXY_IP> <DEPSAPCE_PROXY_PORT> <RECEIVER_PORT> <USE_STUN_SERVER>
int main(int argc, char *argv[]) 
{
	// 0 if receiver; 
	// 1 if sender
	int mode = atoi(argv[1]);

	char* depspace_proxy_ip = malloc(20 * sizeof(char));
	strcpy(depspace_proxy_ip, argv[2]);
	int depspace_proxy_port = atoi(argv[3]);
	int use_stun_server;
	if(DEBUG) {
		printf("\n[Args] Role id is %d \n", mode);
		printf("[Args] DepSpace proxy IP is %s \n", depspace_proxy_ip);
		printf("[Args] DepSpace proxy port is %d \n", depspace_proxy_port);
	}

	char* receiver_port = malloc(20 * sizeof(char));
	strcpy(receiver_port, argv[4]);
	use_stun_server = atoi(argv[5]);

	struct receiver_args_struct_z receiver_args;
	receiver_args.depspace_proxy_ip = malloc(20 * sizeof(char));
	strcpy(receiver_args.depspace_proxy_ip, depspace_proxy_ip);
	receiver_args.depspace_proxy_port = depspace_proxy_port;
	receiver_args.receiver_port = atoi(argv[4]);
	receiver_args.use_stun_public_ip = use_stun_server;

	if(DEBUG) { 
		printf("[Args] Receiver port is %s \n", receiver_port);
		printf("[Args] Use Stun Server mode is %d\n", use_stun_server);
		printf("[mpd main] Starting Receiver process...\n");
	}

	int sock_fd = machete_setup_receiver_spec(
		receiver_args.receiver_port, 
		receiver_args.depspace_proxy_ip, 
		receiver_args.depspace_proxy_port, 
		receiver_args.use_stun_public_ip, 
		echo_hello_message
		);

	machete_teardown_receiver_spec(sock_fd);
	
}
