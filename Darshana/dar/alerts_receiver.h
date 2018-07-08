#ifndef ALERTS_RECEIVER_H_
#define ALERTS_RECEIVER_H_

// definitions

#define TRUE 1
#define FALSE 0

#define DEFAULT_BUFFER_SIZE 255

// structures

// functions

void init();

void handle_darshana_client(int client_sock_fd);

void handle_client_alert(char *buffer);

#endif /* ALERTS_RECEIVER */