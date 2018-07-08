#ifndef PATH_MONITORING_H_
#define PATH_MONITORING_H_

//void run_sender_path_monitors(char *dar_ip, int dar_port, char **overlay_nodes_list, int number_overlay_nodes);

void run_sender_darshana(char *sender_dar_ip, 
						 int sender_dar_port, 
						 char *sender_ip_assign_to_node, 
						 char *node_darshana_addr, 
						 char *node_darshana_port,
						 int hop_index);

int start_dar_handler(void *(*handle_darshana_alerts_function)(void*));

void start_path_monitoring_module(char *sender_ip, int sender_port, int max_dar_alerts);

void stop_path_monitoring_module();

void send_commnad_to_run_overlay_nodes_darshana(int overlay_node_socket_fd);

void monitor_path(char *sourceIp, char *relayIp, char *destinationIp);

char* build_hop_run_darshana_spec(char *sender_dar_ip, 
								  int sender_dar_port, 
								  char *darshana_target_dst_ip, 
								  int darshana_target_dst_port, 
								  int type);

void *run_command_in_thread(void *args);

void kill_sender_darshana_pthreads(int hop_index);

#endif /* PATH_MONITORING_H_ */