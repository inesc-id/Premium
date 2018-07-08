#ifndef _MONITORED_PATHS_HANDLER_
#define _MONITORED_PATHS_HANDLER_

// Includes

#include "hijack_alert_handler.h"
#include "utils.h"

// Structures

typedef struct path_status {
	struct tm *previous_time_info; // before last info
	struct tm *last_time_info;
    char *source_ip;
    char *relay_ip;
    char *destination_ip;
    int was_hijacked;
} path_status;

typedef struct node_p {
	path_status item;
	struct node_p *next;
} Node_p;

typedef Node_p *path_node_link;

/********************************************************
			Functions - linked lists specifics
*********************************************************/

// Creates new node
path_node_link new_node_p(path_status path_status_structure);

// Inserts nodes in the begining of the list (print order: newest -> oldest)
path_node_link insert_begin_p(path_node_link head, path_status path_status_structure);

// Inserts nodes at the end of the list (print order: oldest -> newest)
path_node_link insert_end_p(path_node_link head, path_status path_status_structure);

// prints all the items of the nodes
void show_all_nodes_p(path_node_link node);

/********************************************************
			Functions - monitored paths specifics
*********************************************************/

// initialized linked list head
void init_monitored_paths_head();

path_status new_path_status(char* source_ip, char* relay_ip, char* destination_ip);

void save_path_status(path_status path_status_struct);

void show_path_status(path_status path);

void show_all_paths();

// Initializes path values
void init_test_path_values();

// Show paths with value "was_hijacked" recursively
void show_hijacked_paths(int was_hijacked);

// Checks if either source_ip or destination_ip are a relay node of any of the paths
void analyse_hijack_alert(hijack_alert alert);

// Sets path as hijacked, and updates timestamps (last and before last alert)
void update_path_status(path_node_link path_link, hijack_alert alert);

// Looksup path by relay node ip, !! stops at first occurrence !!
path_node_link lookup_path_by_relay_ip(char* relay_ip);

path_node_link lookup_path_by_relay_ip_recursive(path_node_link starting_point, char* relay_ip);

// Looksup path that was hijack
path_node_link lookup_path_by_hijack_value(int was_hijacked);

path_node_link lookup_path_by_hijack_value_recursive(path_node_link starting_point, int was_hijacked);

#endif