#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "utils/debug.h"
#include "machete_sender.h"


off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

//Arg0: address, arg1: port, arg2: filename
int send_file_protocol(int clientSocket, char* filename)
{
	char buffer[1024];
	int sent_bytes = 0;
	int offset = 0;
	int fd = 0; //file descriptor
	int block = 0;
	int remain_data = 0;
	long file_size = 0;
	int filename_size = 0;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("Error reading file: %s\n", filename);
		perror("[Error] Reading file");
		exit(-1);
	}

	// 1 - Let's send the size of the file name
	filename_size = strlen(filename);
	if(DEBUG) printf("[Sending file] Will send the filename size: %d\n", filename_size);
	send(clientSocket, &filename_size, sizeof(int), 0);

	// 2 - Now, let's send the filename
	if(DEBUG) printf("[Sending file] Will send the filename %s\n", filename);
	send(clientSocket, filename, strlen(filename), 0);
	
	// 3 - Let's send the size of the file itself
	file_size = fsize(filename);
	if(DEBUG) printf("[Sending file] Will send the file_size: %ld\n", file_size);
	send(clientSocket, &file_size, sizeof(long), 0);

	if(DEBUG) printf("[Sending file] Begin file transmission...\n");
	
	//4 - Finally, let's send the payload in blocks of LENGTH size
	if (sendfile(clientSocket, fd, NULL, file_size) < 0) {
		perror("[Error] Transmitting file");
		exit(-1);
	}
	
	if(DEBUG) printf("[Sending file] End file transmission...\n");

	return 0;
}

/* Sender args: 
 * <MODE> 
 * <DEPSPACE_PROXY_IP> 
 * <DEPSAPCE_PROXY_PORT> 
 * <RECEIVER_IP> 
 * <RECEIVER_PORT> 
 * <OVERLAY_PORT> 
 * <MIN_NUMBER_OF_NODES> 
 * <USE_STUN_SERVER> 
 * <MAX_DAR_ALERTS> 
 * <FILE_NAME>
 */
int main(int argc, char *argv[])
{
	// 0 if receiver; 
	// 1 if sender
	int mode = atoi(argv[1]);

	char* depspace_proxy_ip = malloc(20 * sizeof(char));
	strcpy(depspace_proxy_ip, argv[2]);
	int depspace_proxy_port = atoi(argv[3]);

	char* receiver_ip = malloc(20 * sizeof(char));
	strcpy(receiver_ip, argv[4]);
	int receiver_port = atoi(argv[5]);
	
	int overlay_port = atoi(argv[6]);
	int min_number_of_nodes = atoi(argv[7]);
	int use_stun_server = atoi(argv[8]);
	int max_dar_alerts = atoi(argv[9]);
	char* filename = malloc(20 * sizeof(char));
	strcpy(filename, argv[10]);

	if (DEBUG) {
		printf("\n[Args] Role id is %d \n", mode);
		printf("[Args] DepSpace proxy IP is %s \n", depspace_proxy_ip);
		printf("[Args] DepSpace proxy port is %d \n", depspace_proxy_port);
		printf("[Args] Receiver IP is %s \n", receiver_ip);
		printf("[Args] Receiver port is %d \n", receiver_port);
		printf("[Args] Overlay Node port is %d \n", overlay_port);
		printf("[Args] Minimum Number of Overlay Nodes is %d \n", min_number_of_nodes);
		printf("[Args] Use Stun server is %d \n", use_stun_server);
		printf("[Args] Maximum Number of DAR Alerts is %d \n", max_dar_alerts);
		printf("[Args] File to send is %s \n", filename);
		printf("[mpd main] Starting Sender process...\n");
	}

	int sock_fd = machete_setup_sender_spec(receiver_ip, 
		                                    receiver_port, 
		                                    depspace_proxy_ip, 
		                                    depspace_proxy_port, 
		                                    overlay_port, 
		                                    min_number_of_nodes, 
		                                    use_stun_server,
		                                    max_dar_alerts);

	if (DEBUG) printf("[MPD Sender main] Sending file \"%s\" to Receiver at %s:%d\n", filename, receiver_ip, receiver_port);
	send_file_protocol(sock_fd, filename);

	machete_teardown_sender_spec();
}
