/*
 * This module defines 
 * the MACHETE sender's component API
 */

#ifndef MACHETE_SENDER_H_
#define MACHETE_SENDER_H_

#include "sending_handler.h"

/*
 * Specific API
 */

int machete_setup_sender_spec(char* dst_address, 
							  int dst_port, 
							  char* depspace_proxy_ip, 
							  int depspace_proxy_port, 
							  int overlay_port, 
							  int min_number_of_nodes, 
							  int use_stun_server,
							  int max_dar_alerts);

void machete_teardown_sender_spec();

/*
 * Simple API
 */

int machete_setup_sender(char* receiver_ip, int receiver_port);

void machete_teardown_sender(int receiver_sock_fd);

#endif /* MACHETE_SENDER_H_ */