/*
 * receiving_handler.h
 *
 *  Created on: Apr 13, 2016
 *      Author: feline4
 */

#ifndef RECEIVING_HANDLER_H_
#define RECEIVING_HANDLER_H_

void getPrimary();

void getLocalInterfaces2();

void run_darshana_server(char receiver_ip[], 
						 char *receiver_port);

void *run_command_in_thread(void *args);

void close_socket_communication_with_clients(int client_sock_fd);

int init_socket_communication_with_clients(int receiver_port, 
										   void *(*receive_data_function)(void*));

/*
 * Setup sender component and overlay network rules 
 * and initiates communication with receiver
 */
int setup_receiver(int receiver_port, 
				   char* depspace_proxy_ip, 
				   int depspace_proxy_port, 
				   int use_stun_public_ip, 
				   void *(*receive_data_function)(void*));

/*
 * Teardown sender component and overlay network rules 
 * and closes communication with receiver
 */
void teardown_receiver(int client_sock_fd);

#endif /* RECEIVING_HANDLER_H_ */
