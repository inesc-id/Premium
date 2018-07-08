#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "utils/debug.h"
#include "machete_sender.h"

#define MAX_EXPERIMENTS_THRESHOLD 31
#define MAX_HELLOS_THRESHOLD 3

void execute_hello_message_exchange(int socket_fd)
{
	char *hi_message = "Hello";
	int len = 0;

	long *hi_size = malloc(sizeof(long));
	*hi_size = sizeof(char) * strlen(hi_message);

	long *echo_message_size = malloc(sizeof(long));

	int max_hellos = 0;

	while(1) {		

		if (max_hellos >= MAX_HELLOS_THRESHOLD) {
			return;
		}

		if(DEBUG) printf("[ECHO] Send message with size: %ld\n", *hi_size);
		send(socket_fd, hi_size, sizeof(long), 0);

		if(DEBUG) printf("[ECHO] Send message: %s\n", hi_message);
		send(socket_fd, hi_message, *hi_size, 0);

		if(DEBUG) printf("[ECHO] Preparing to receive echoed message\n");

		*echo_message_size = 0;

		// 1 - First, let's read the size of the message
		if((len = recv(socket_fd, echo_message_size, sizeof(long), 0)) <= 0) break;
		//recv(socket_fd, echo_message_size, sizeof(long), 0);
		if(DEBUG) printf("[ECHO] Will receive a message with: %ld bytes\n", *echo_message_size);

		// 2 -  Now let's read the hello in upper case 
		char *recv_message = malloc(*echo_message_size);

		if((len = recv(socket_fd, recv_message, *echo_message_size, 0)) <= 0) break;
		//recv(socket_fd, recv_message, *echo_message_size, 0);
		if(DEBUG) printf("[ECHO] Recv Message: %s\n", recv_message);
		
		//memset(recv_message, 0, strlen(recv_message));
		free(recv_message);

		if(DEBUG) printf("[ECHO] End echo protocol ----------- !\n");

		max_hellos = max_hellos + 1;
		sleep(2);
	}

}

/* Sender args: 
 * <MODE> 
 * <DEPSPACE_PROXY_IP> 
 * <DEPSAPCE_PROXY_PORT> 
 * <RECEIVER_IP> 
 * <RECEIVER_PORT> 
 * <OVERLAY_PORT> 
 * <MIN_NUMBER_OF_NODES> 
 * <USE_STUN_SERVER> 
 * <MAX_DAR_ALERTS> 
 * <FILE_NAME>
 */
int main(int argc, char *argv[])
{
	// 0 if receiver; 
	// 1 if sender
	int mode = atoi(argv[1]);

	char* depspace_proxy_ip = malloc(20 * sizeof(char));
	strcpy(depspace_proxy_ip, argv[2]);
	int depspace_proxy_port = atoi(argv[3]);

	char* receiver_ip = malloc(20 * sizeof(char));
	strcpy(receiver_ip, argv[4]);
	int receiver_port = atoi(argv[5]);
	
	int overlay_port = atoi(argv[6]);
	int min_number_of_nodes = atoi(argv[7]);
	int use_stun_server = atoi(argv[8]);
	int max_dar_alerts = atoi(argv[9]);
	char* filename = malloc(20 * sizeof(char));
	strcpy(filename, argv[10]);

	if (DEBUG) {
		printf("\n[Args] Role id is %d \n", mode);
		printf("[Args] DepSpace proxy IP is %s \n", depspace_proxy_ip);
		printf("[Args] DepSpace proxy port is %d \n", depspace_proxy_port);
		printf("[Args] Receiver IP is %s \n", receiver_ip);
		printf("[Args] Receiver port is %d \n", receiver_port);
		printf("[Args] Overlay Node port is %d \n", overlay_port);
		printf("[Args] Minimum Number of Overlay Nodes is %d \n", min_number_of_nodes);
		printf("[Args] Use Stun server is %d \n", use_stun_server);
		printf("[Args] Maximum Number of DAR Alerts is %d \n", max_dar_alerts);
		printf("[Args] File to send is %s \n", filename);
		printf("[mpd main] Starting Sender process...\n");
	}

	// application code

	int number_of_experiments = 0;

	for (number_of_experiments = 0 ; number_of_experiments < MAX_EXPERIMENTS_THRESHOLD ; number_of_experiments++) {

		printf("-------------------------------------------\n");
		printf("----- Running experiment #%d ! -----\n", number_of_experiments);
		printf("-------------------------------------------\n");

		int sock_fd = machete_setup_sender_spec(receiver_ip, 
			                                    receiver_port, 
			                                    depspace_proxy_ip, 
			                                    depspace_proxy_port, 
			                                    overlay_port, 
			                                    min_number_of_nodes, 
			                                    use_stun_server,
			                                    max_dar_alerts);

		if (DEBUG) printf("[MPD Sender main] Sending HELLO and hello\n");
		execute_hello_message_exchange(sock_fd);

		machete_teardown_sender_spec();

		// Force teardown sender: adapter, java programs including Darshana
		sleep(1);
		system("iptables -t nat -F");
		//system("killall java");
		system("pkill java");
		sleep(3);
		system("cd ../evaluation; sh run-adapter.sh 1005; cd ../SCMultipath/;");
		sleep(10);
	}
}
