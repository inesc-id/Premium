#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

int init_socket_communication_with_receiver(char* receiver_ip, int receiver_port);

void close_socket_communication_with_receiver(int receiver_sock_fd);

void connection_interruption_signal_action();

#endif /* CONNECTION_MANAGER_H_ */