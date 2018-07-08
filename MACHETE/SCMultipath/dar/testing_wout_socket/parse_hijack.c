#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include "parse_hijack.h"
#include "../hijack_alert_handler.h"
#include "../monitored_paths_handler.h"

// Global variables

//node_link alerts_history_head;


int main(int argc, char *argv[]) {

	int i;

	init();

	//hijack_alert alert_parsed;

	for(i = 1; i < argc; i++) {
		hijack_alert alert_parsed = parse_hijack_alert(argv[i]);
		save_hijack_alert(alert_parsed);
		analyse_hijack_alert(alert_parsed);
		
		//show_all_paths();
		sleep(1);
	}

	printf("-> Show hijacking alerts\n");
	show_all_hijack_alerts();

	printf("-> Show all path status\n");
	show_all_paths();


	printf("-----------------------------------------------\n");
	printf("Showing paths that are wer not hijacked yet!!!!\n");
	printf("-----------------------------------------------\n");

	show_hijacked_paths(TRUE);

	return 0;
}

void init()
{
	init_alerts_history_head();
	init_monitored_paths_head();
	init_test_path_values();
}
