#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "utils/debug.h"
#include "machete_receiver.h"

#define LENGTH_SPEC 512

struct receiver_args_struct_z {
	int receiver_port;
	char* depspace_proxy_ip;
	int depspace_proxy_port;
	int use_stun_public_ip;
};

/*
	Function responsible to recieve data and write it to new file
	with the filename provided by the Sender
*/
void *machete_receive_data(void *args)
{
	char buffer[LENGTH_SPEC];
	int block = 0;
	int newSocket = (intptr_t)args;
	int filename_size = 0;
	long filesize = 0;
	int remain_data = 0;
	int len = 0;

	// 1 - First, let's read the size of the filename
	recv(newSocket, &filename_size, sizeof(int), 0);
	if(DEBUG)printf("Will receive a filename with: %d chars\n", filename_size);

	// 2 -  Now let's read the filename 
	char filename[filename_size];
	recv(newSocket, filename, filename_size, 0);
	if(DEBUG)printf("Filename: %s\n", filename);

	// 3 - Now let's read the size of the file itself
	if(DEBUG)printf("Will receive a filesize\n");
	recv(newSocket, &filesize, sizeof(long), 0);
	if(DEBUG)printf("FileSize: %ld\n", filesize);

	// 3.5 - Creating a file
	FILE *received_file = fopen(filename, "w");
	if (received_file == NULL){
			printf("Error creating file: %s\n", filename);
			exit(-1);
	}

	remain_data = filesize;

	// 4 - Finally, let's read the file itself 
	while ( ((len = recv(newSocket, buffer, LENGTH_SPEC, 0)) > 0) && (remain_data > 0))
	{
		remain_data -= len;	
		if(DEBUG)printf("Received %d bytes and we hope: %d bytes\n", len, remain_data);
		fwrite(buffer, sizeof(char), len, received_file);
	}
	
	if(DEBUG) printf("[Receive data] Closing file created to receive data...\n");
	fclose(received_file);
	if(DEBUG) printf("[Receive data] Closed file created to receive data.\n");

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
		machete_receive_data
	);

	machete_teardown_receiver_spec(sock_fd);
}
