#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machete_receiver.h"

/*
 * Specific API
 */

int machete_setup_receiver_spec(int receiver_port, char* depspace_proxy_ip, int depspace_proxy_port, int use_stun_public_ip, void *(*receive_data_function)(void*))
{
	int sock_fd = setup_receiver(receiver_port, depspace_proxy_ip, depspace_proxy_port, use_stun_public_ip, receive_data_function);

	return sock_fd;
}

void machete_teardown_receiver_spec(int receiver_sock_fd)
{
	teardown_receiver(receiver_sock_fd);
}

/*
 * Simple API
 */

int machete_setup_receiver(int receiver_port, void *(*receive_data_function)(void*))
{
	return -1;
}

void machete_teardown_receiver(int receiver_sock_fd)
{
	teardown_receiver(receiver_sock_fd);
}