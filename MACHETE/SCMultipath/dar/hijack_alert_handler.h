#ifndef _HIJACK_ALERT_HANDLER_
#define _HIJACK_ALERT_HANDLER_

// Includes
#include "utils.h"

// Structures

typedef struct hijack_alert {
	char *timestamp;
	struct tm *time_info;
	char *alert_type;
    char *source_ip;
    char *destination_ip;
    char *metric_name;
} hijack_alert;

typedef struct node {
	hijack_alert item;
	struct node *next;
} Node;

typedef Node *node_link;

/********************************************************
			Functions - linked lists specifics
*********************************************************/

// Creates new node
node_link new_node_h(hijack_alert alert);

// Inserts nodes in the begining of the list (print order: newest -> oldest)
node_link insert_begin_h(node_link head, hijack_alert alert);

// Inserts nodes at the end of the list (print order: oldest -> newest)
node_link insert_end_h(node_link head, hijack_alert alert);

// prints all the items of the nodes
void show_all_nodes_h(node_link node);

/********************************************************
			Functions - hijack alerts specifics
*********************************************************/

// initialized linked list head
void init_alerts_history_head();

hijack_alert parse_hijack_alert(char* data_received);

hijack_alert new_hijack_alert(char* alert_type, char* source_ip, char* destination_ip, char* metric_name);

void save_hijack_alert(hijack_alert alert_parsed);

void show_hijack_alert_structured(hijack_alert alert);

void show_hijack_alert_simple(hijack_alert alert);

void show_all_hijack_alerts();

#endif