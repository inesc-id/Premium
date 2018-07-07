#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> // for close
#include <sys/time.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <errno.h>
#include "utils/debug.h"
#include "utils/log_utils.h"

struct timeval *begin_connection_time;
int connection_socket_descriptor;

int init_socket_communication_with_receiver(char* receiver_ip, int receiver_port)
{
	int receiver_socket_fd;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	// start registering time for with server connection
	begin_connection_time = begin_clock();

	receiver_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(receiver_port);
	serverAddr.sin_addr.s_addr = inet_addr(receiver_ip);
	memset(serverAddr.sin_zero, 0, sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;

	if(DEBUG) printf("[Init Socket] Trying to connect with Receiver at %s:%d\n", receiver_ip, receiver_port);

	int con_result = connect(receiver_socket_fd, (struct sockaddr *) &serverAddr, addr_size);
	
	if (con_result < 0)
	{
		perror("[Error] Connecting with receiver");
		exit(-1);
	}

	if(DEBUG) printf("[Init Socket] Connecting to receiving handler result: %d\n", con_result);

	return receiver_socket_fd;
}

void close_socket_communication_with_receiver(int receiver_sock_fd)
{
	if(DEBUG) printf("[End Socket] Closing socket communication with the Receiver...\n");
	close(receiver_sock_fd);

	// log connection duration time
	end_clock(begin_connection_time, "Connection duration time");
}
