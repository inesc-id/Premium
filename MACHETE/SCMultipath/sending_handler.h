/*
 * sending_handler.h
 *
 *  Created on: Apr 13, 2016
 *      Author: feline4
 */

#ifndef SENDING_HANDLER_H_
#define SENDING_HANDLER_H_

#include "premium_specs.h"

/*
 * Darshana Alert Receiver structure
 */
typedef struct darshana_alert_receiver_info {
	char *dar_ip;
	int dar_port;

} darshana_alert_receiver_info;

/*
 * setup MACHETE Sender arguments structure
 */
typedef struct setup_sender_arg {
    char* receiver_addr;
    int receiver_port;
    char* depspace_proxy_addr; 
    int depspace_proxy_port; 
    int overlay_port;
    int min_number_of_nodes; 
    int use_stun_server;
    machete_reaction reaction_spec;
    int max_dar_alerts;

} setup_sender_arg;


void reserve_memory_for_structures();

void setup_machete_info();

int setup_each_flow(int overlay_port, 
                      int receiver_port, 
                      int use_stun_server);

void clean_rules(int overlay_port, 
                 int receiver_port, 
                 int use_stun_server);

int setup_sender(setup_sender_arg *argument);

void teardown_sender();

void handle_dar_signal(int signal);

#endif /* SENDING_HANDLER_H_ */
