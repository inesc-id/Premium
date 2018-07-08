/*
 * This module defines 
 * the MACHETE receiver's component API
 */

#ifndef MACHETE_RECEIVER_H_
#define MACHETE_RECEIVER_H_

#include "receiving_handler.h"

/*
 * Specific API
 */

int machete_setup_receiver_spec(int receiver_port, 
								char* depspace_proxy_ip, 
								int depspace_proxy_port, 
								int use_stun_public_ip, 
								void *(*receive_data_function)(void*));

void machete_teardown_receiver_spec(int receiver_sock_fd);

/*
 * Simple API
 */

int machete_setup_receiver(int receiver_port, 
						   void *(*receive_data_function)(void*));

void machete_teardown_receiver(int receiver_sock_fd);

#endif /* MACHETE_RECEIVER_H_ */