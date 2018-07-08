/*
 * SendingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <resolv.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h> // for close
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#include "common.h"
#include "premium_specs.h"
#include "utils/debug.h"
#include "utils/stun_public_ip.h"
#include "utils/log_utils.h"
#include "utils/rules_utils.h"
#include "utils/virtual_int_utils.h"
#include "utils/connectivity_utils.h"
#include "sending_handler.h"
#include "overlay_manager.h"
#include "path_monitoring.h"
#include "connection_manager.h"
#include "register_dep.h"

// Global variables

char **hops, **srcs, **dsts;
char *primary_interface;

int numSrcs, numHops, numDsts;

struct setup_sender_arg *setup_arguments;
struct darshana_alert_receiver_info *dar_info;

// socket descriptor of connection from Sender to Receiver
int connection_sock_fd;

void reserve_memory_for_structures()
{
	int i;
	hops = malloc(MAX_FLOWS * sizeof(char*));
	srcs = malloc(MAX_FLOWS * sizeof(char*)); 
	dsts = malloc(MAX_FLOWS * sizeof(char*));
	
	for (i = 0; i < MAX_FLOWS; i++) {
		hops[i] = malloc(sizeof(char) * IP_ADDR_LENGTH);
		srcs[i] = malloc(sizeof(char) * IP_ADDR_LENGTH);
		dsts[i] = malloc(sizeof(char) * IP_ADDR_LENGTH);
	}
	primary_interface = malloc(sizeof(char)*10);

	dar_info = (darshana_alert_receiver_info*) malloc(sizeof(darshana_alert_receiver_info));
	dar_info->dar_ip = (char*) malloc(sizeof(char) * (IP_ADDR_LENGTH));
}

/*
 * Setup virtual interfaces, ifconfig information, source ips, totalFlows
 * 1. Checks local available interfaces 
 * 2. Checks if there is enough interfaces to establish desired number of paths
 * 3. Creates virtual interfaces if needed
 * 4. Checks the existing interfaces, which may be different from the initial
 */
void setup_machete_info()
{
	const char s = 'e';
	
	get_local_interfaces(primary_interface, &numSrcs, &numDsts, &numHops, srcs, dsts);

	// numSrcs = number of available interfaces of the source
	// numDsts = number of available interfaces at the destination
	int totalFlows = numSrcs*numDsts;

	if(DEBUG) printf("[Setup info] Sending Handler: numsrcs:%d numdsts:%d Total flows:%d numhops:%d Primary interface:%s\n", numSrcs, numDsts, totalFlows, numHops, primary_interface);
	
	// Only creates virtual interfaces if the number of flows is 
	// smaller than the number of available paths or overlay nodes
	if(totalFlows < numHops){
		while(numSrcs*numDsts < numHops){

			// if interface is of type contains char "e" ... if it not localhost
			if (strchr(primary_interface, s) != NULL) {
				printf("[Setup info] Before createvifcfg %d\n", numHops);
				create_virtual_ifconfig(numSrcs, primary_interface);
				if(DEBUG)printf("[Setup info] Done creating new ifconfig iface\n");
			} else {
				create_virtual_interface(numSrcs, primary_interface);
			}
			if(DEBUG)printf("[Setup info] Adding numSrcs\n");
			numSrcs++;

			if(DEBUG)printf("[Setup info] Number of sources is now %d and hops %d\n", numSrcs, numHops);
		}
	}
	if(DEBUG) printf("[Setup info] #Hops before get local interfaces: %d\n", numHops);

	get_local_interfaces(primary_interface, &numSrcs, &numDsts, &numHops, srcs, dsts);

	if(DEBUG) printf("[Setup info] #Hops after getting local interfaces: %d\n", numHops);
}

/*
 * Sends NAT rules to the overlay nodes
 * 1. Checks local available interfaces 
 * 2. Checks if there is enough interfaces to establish desired number of paths
 * 3. Creates virtual interfaces if needed
 * 4. Checks the existing interfaces, which may be different from the initial
 * 5. Creates iptable rules and sends them to the overlay nodes
 */
int setup_each_flow(int overlay_port, int receiver_port, int use_stun_server)
{
	struct timeval *begin_sender_send_rules_time, *begin_on_send_rules_time, *setup_sender_monitor_time;
	int add_rule_type = 1;
	int i, j, hop_index;

	if(DEBUG) printf("[Setup each flow] numsrcs = %d | numdsts = %d | numhops = %d\n", numSrcs, numDsts, numHops);

	// For each available interface at the source (same number as overlay nodes)
	// 1. Sets its own NAT rules
	// 2. Sends the NAT rules to overlay nodes
	for(i = 0; i < numSrcs; i++) {
		for(j = 0; j < numDsts; j++) {

			hop_index = ((i*numDsts+j) % numHops);

			log_message_with_str_value("### Setup for node", hops[hop_index]);

			// Sets its own rules
			
			begin_sender_send_rules_time = begin_clock();

			if(DEBUG) printf("[Setup each flow] Creating rules to forward traffic to hop: %d\n", hop_index);

			set_sender_own_rules_iptables(srcs[i], dsts[j], hops[hop_index]);

			end_clock(begin_sender_send_rules_time, "Setup sender forward rules time");

			// Sends rules to overlay nodes
			
			begin_on_send_rules_time = begin_clock();

			if(DEBUG) printf("[Setup each flow] Sending rule to hop %s\n", hops[hop_index]);
			
			// Create rules to send ******
			if (use_stun_server) {
				
				// Find public IP for each interface
				stunGetPublicIP(srcs[i], srcs[i]);
			}

			// Setup DAR info
			strcpy(dar_info->dar_ip, srcs[0]);
			dar_info->dar_port = DAR_PORT; // HARDCODED string

			// build rule spec to setup forwarding and dar rules
			char* forwarding_rule_spec = build_hop_forward_rules_spec(srcs[i], 
				                                                      dsts[j], 
				                                                      receiver_port, 
				                                                      add_rule_type);

			char* darshana_rule_spec = build_hop_run_darshana_spec(dar_info->dar_ip, 
				                                                   dar_info->dar_port, 
				                                                   dsts[j], 
				                                                   atoi(DARSHANA_SERVER_PORT), 
				                                                   add_rule_type);

			// Send rules spec to Overlay Nodes
			setup_overlay_node(hops[hop_index], overlay_port, forwarding_rule_spec, darshana_rule_spec);

			end_clock(begin_on_send_rules_time, "Setup nodes time");

			setup_sender_monitor_time = begin_clock();

			// Run Darshana instances on Sender
			// hop_index represents the number of overlay nodes, consequently the number of paths
			run_sender_darshana(dar_info->dar_ip, dar_info->dar_port, srcs[i], 
								hops[hop_index], DARSHANA_SERVER_PORT, hop_index);

			// Adds this path to the list of monitored paths
			monitor_path(srcs[i], hops[hop_index], dsts[j]);

			end_clock(setup_sender_monitor_time, "Setup sender path monitoring time");
		}
	}
	if(DEBUG) printf("[Setup each flow] Sending Handler: done setting rules\n");
}

/*
 * Cleans NAT rules at the Sender and the overlay nodes
 * 1. Clean Ovelay Nodes rules
 * 2. Clean Sender rules
 */
void clean_rules(int overlay_port, int receiver_port, int use_stun_server)
{
	struct timeval *begin_clean_on_rules_time, *begin_clean_sender_rules_time;
	int remove_rule_type = 0;	
	int i, j, hop_index;

	// Cleans NAT rules at each overlay node

	begin_clean_on_rules_time = begin_clock();

	for (i = 0; i < numSrcs; i++) {
		for (j = 0; j < numDsts; j++) {

			hop_index = ((i*numDsts+j) % numHops);

			// Send rules to hops (overlay nodes)
			if(DEBUG) printf("[Clean rules] Sending rule to hop %s\n", hops[hop_index]);
	/*
			// Create rules to send ******
			if (use_stun_server) {
				
				// Find public IP for each interface
				stunGetPublicIP(srcs[i], srcs[i]);
			}
	*/
			// build rule spec to setup forwarding and dar rules
			char* forwarding_rule_spec = build_hop_forward_rules_spec(srcs[i], 
				                                                      dsts[j], 
				                                                      receiver_port, 
				                                                      remove_rule_type);

			char* darshana_rule_spec = build_hop_run_darshana_spec(dar_info->dar_ip, 
				                                                   dar_info->dar_port, 
				                                                   dsts[j], 
				                                                   atoi(DARSHANA_SERVER_PORT), 
				                                                   remove_rule_type);

			// Send rules spec to Overlay Nodes
			setup_overlay_node(hops[hop_index], overlay_port, forwarding_rule_spec, darshana_rule_spec);

			kill_sender_darshana_pthreads(hop_index);
		}
	}

	end_clock(begin_clean_on_rules_time, "Teardown nodes time");

    // For each path cleans Sender's NAT rules

	begin_clean_sender_rules_time = begin_clock();

	for(i = 0; i < numSrcs; i++) {
		for(j = 0; j < numDsts; j++) {

			hop_index = ((i*numDsts+j) % numHops);

			if(DEBUG) printf("[Clean rules] Creating rules to forward traffic to hop: %d\n", hop_index);

			remove_sender_own_rules_iptables(srcs[i], dsts[j], hops[hop_index]);
		}
    }
    if(DEBUG) printf("[Clean rules] Sending Handler: done cleaning rules\n");

	end_clock(begin_clean_sender_rules_time, "Teardown device time");

	//clean local
	//	system("iptables -t nat -F");
}

void handle_dar_signal(int signal)
{
	if(DEBUG) printf("[Catch DAR signal] !! Warning !! Communication is COMPROMISED!!\n");

	if (setup_arguments->reaction_spec == SHUTDOWN_COMMUNICATION) {
		if(DEBUG) {
			printf("[Catch DAR signal] Shutting down communication with receiver ...\n");
		}

		teardown_sender();
		exit(1);

	} else if (setup_arguments->reaction_spec == SHUTDOWN_COMMUNICATION_AND_STARTS_AGAIN_WOUT_BAD_PATH) {
		if(DEBUG) {
			printf("[Catch DAR signal] Shutting down communication with receiver ...\n");
			printf("[Catch DAR signal] Starting all over without hijacked path ...\n");
			printf("[Catch DAR signal] NOT IMPLEMENTED !! ...\n");
		}
		
		teardown_sender();

		// remove bad overlay node aka path from reusable overlay nodes

		// setup partially without getting the overlay nodes again
		// setup_sender_new_paths();

	
	} else if (setup_arguments->reaction_spec == REUSE_PATHS_LEFT) {
		if(DEBUG) {
			printf("[Catch DAR signal] Reusing the uncompromised paths ...\n");
			printf("[Catch DAR signal] NOT IMPLEMENTED !! ...\n");
		}

	} else if (setup_arguments->reaction_spec == CREATE_NEW_PATH) {
		if(DEBUG) {
			printf("[Catch DAR signal] Creating new path to replace comprimised one ...\n");
			printf("[Catch DAR signal] NOT IMPLEMENTED !! ...\n");
		}

	} else if (setup_arguments->reaction_spec == DO_NOTHING_AND_IGNORE_ALERTS) {
		if(DEBUG) {
			printf("[Catch DAR signal] Catched signal but PREMIUM option is to DO NOTHING ...\n");
		}

	} else {
		if(DEBUG) {
			printf("[Catch DAR signal] The reaction you chose for hijack alerts does not exist!!\n");
			printf("[Catch DAR signal] The default option is to end communication ...\n");	
			printf("[Catch DAR signal] Shuting down communication with receiver...\n");
		}
		teardown_sender();
		exit(1);
	}
}

int setup_sender(setup_sender_arg *argument)
{
	struct timeval *setup_time, *get_nodes_from_manager_time;
	struct timeval *probe_overlay_nodes_time, *get_receiver_info_time;
	struct timeval *setup_all_nodes_time;
	int number_of_selected_nodes;

	setup_arguments = argument;

	reserve_memory_for_structures();

	log_message("<--------- Setup Phase --------");
   	
   	setup_time = begin_clock();

   	// Getting all of the overlay nodes from the Multipath Manager
	if(DEBUG) printf("[Setup Sender] Looking for hops...\n");
   	get_nodes_from_manager_time = begin_clock();
	numHops = get_overlay_nodes_from_manager(setup_arguments->depspace_proxy_addr, 
                                             setup_arguments->depspace_proxy_port, 
                                             hops);
	end_clock(get_nodes_from_manager_time, "Getting Overlay Nodes from Manager time");
	if(DEBUG) printf("[Setup Sender] Found hops { %d } probing nodes now...\n", numHops);
	log_message_with_int_value("### Number of hops returned from Manager =", numHops);

	// Probe overlaynodes to know which ones are available
   	probe_overlay_nodes_time = begin_clock();
	numHops = probe_overlay_nodes(numHops, hops);
	end_clock(probe_overlay_nodes_time, "Probing Overlay Nodes time");
	if(DEBUG) printf("[Setup Sender] Probed and responsive hops { %d }\n", numHops);
	log_message_with_int_value("### Number of responsive hops =", numHops);

	validate_number_of_hops(numHops, setup_arguments->min_number_of_nodes);

	// Select overlay nodes from all of those returned by the Multipath Manager
	// number_of_selected_nodes = select_overlay_nodes();

	// Get the IPs of the Receiver from the Multipath Manager
	get_receiver_info_time = begin_clock();
	numDsts = get_receiver_info_from_manager(setup_arguments->depspace_proxy_addr, 
                                             setup_arguments->depspace_proxy_port, 
                                             dsts, setup_arguments->receiver_addr);
	end_clock(get_receiver_info_time, "Getting Receiver's info time");

	if(DEBUG) printf("[Setup Sender] Found # dsts { %d }\n", numDsts);

	// Setup virtual interfaces, total number of flows
	setup_machete_info();

	// Set Sender and Overlay Nodes' rules
	// DAR info is initializated here!
	if(DEBUG) printf("[Setup Sender] Set sending rules\n");
	setup_all_nodes_time = begin_clock();
	setup_each_flow(setup_arguments->overlay_port, 
                    setup_arguments->receiver_port, 
                    setup_arguments->use_stun_server);
	end_clock(setup_all_nodes_time, "Setup all flows time");

	// Start path monitor module, i.e., Darshana instances
	if(DEBUG) printf("[Setup Sender] Start Path Monitoring module ...\n");
	start_path_monitoring_module(dar_info->dar_ip, dar_info->dar_port, setup_arguments->max_dar_alerts);

	// Register to get notice of a compromised path
	if (signal(SIGUSR1, handle_dar_signal) == SIG_ERR) {
		if(DEBUG) printf("\n[Setup Sender] Can't register to catch SIGUSR1!\n");
	}

	end_clock(setup_time, "Setup time");


	log_message("<--------- Connection Phase --------");
	// Initiate connection with Destination/Receiver
	if(DEBUG) printf("[Setup Sender] Initiate communication with Receiver ...\n");
	connection_sock_fd = init_socket_communication_with_receiver(setup_arguments->receiver_addr, 
		                                                         setup_arguments->receiver_port);

	return connection_sock_fd;
}

void teardown_sender()
{
	struct timeval *teardown_time;

	// terminate connection with receiver
	close_socket_communication_with_receiver(connection_sock_fd);

	log_message("<-------- Teardown Phase --------");

	// tearing down overlay network setup and log duration time
	teardown_time = begin_clock();

	stop_path_monitoring_module();

	clean_rules(setup_arguments->overlay_port, setup_arguments->receiver_port, setup_arguments->use_stun_server);
	clean_virtual_interfaces();

	end_clock(teardown_time, "Teardown time");
	log_message("<------------------------------");
	
	// close log file
	close_log_file();
}
