#ifndef FORWARDING_HANDLER_H_
#define FORWARDING_HANDLER_H_

int start_overlay(char* overlay_addr, int overlay_port);

void receive_forwarding_rule_specification(char* overlay_addr, int sender_socket_fd, char buffer[]);

void receive_darshana_rule_specification(char* overlay_addr, int sender_socket_fd, char buffer[]);

void run_darshana_client(char *node_addr, char *sender_dar_ip, char *sender_dar_port, char *dst_darshana_addr, char *dst_darshana_port, char *monitoring_mode, char *metrics_thresholds);

void run_darshana_server(char* receiver_ip);

void *run_command_in_thread(void *command);

#endif /* FORWARDING_HANDLER_H_ */