#ifndef OVERLAY_MANAGER_H_
#define OVERLAY_MANAGER_H_

void select_overlay_nodes(int min_number_overlay_nodes, 
						  char **all_nodes, 
						  char **selected_nodes);

int initiate_overlay_node_comn_socket(char* hop_ip, 
									  int hop_port);

void close_overlay_node_comn(int socket_fd);

char* build_hop_forward_rules_spec(char *source_ip, 
								   char *destination_ip, 
								   int destination_port, 
								   int type);

void setup_overlay_node(char* hop_ip, 
						int hop_port, 
						char* forwarding_rule, 
						char* darshana_rule);


void validate_number_of_hops(int num_responsive_hops, 
							 int min_number_of_nodes);

#endif /* OVERLAY_MANAGER_H_ */