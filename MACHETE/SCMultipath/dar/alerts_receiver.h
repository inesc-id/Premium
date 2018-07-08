#ifndef ALERTS_RECEIVER_H_
#define ALERTS_RECEIVER_H_

// definitions

#define TRUE 1
#define FALSE 0

#define DEFAULT_BUFFER_SIZE 255

// structures

// functions

void setup_dar(int max_number_of_alerts_to_trigger);

int init_dar_socket(char *alert_receiver_ip, int alert_receiver_port);

void end_dar_socket();

void *handle_darshana_client_thread_function(void *args);

void handle_darshana_client_loop(int client_sock_fd);

void handle_client_alert(char *buffer);

void add_monitored_paths_to_list_structure(char **sourceIps, char **relayIps, char **destinationIps, int num_paths);

void add_monitored_path(char *sourceIp, char *relayIp, char *destinationIp);

void alert_machete_of_compromised_path();

#endif /* ALERTS_RECEIVER */