#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// specific header files
#include "hijack_alert_handler.h"

// global variables

node_link alerts_history_head;

/********************************************************
			Functions - hijack alerts specifics
*********************************************************/

// alert = Hijack,10.0.2.34,10.0.2.45,lat
hijack_alert parse_hijack_alert(char* data_received) 
{
	const char s[2] = ",";
	char *tmpc;
	char *alert_type, *source_ip, *destination_ip, *metric_name;
	hijack_alert result;
	
	tmpc = strtok(data_received, s);
	alert_type = duplicate_string(tmpc);

	tmpc = strtok(NULL, s);
	source_ip = duplicate_string(tmpc);

	tmpc = strtok(NULL, s);
	destination_ip = duplicate_string(tmpc);
	
	tmpc = strtok(NULL, s);
	metric_name = duplicate_string(tmpc);

	result = new_hijack_alert(alert_type, source_ip, destination_ip, metric_name);

	return result;
}

hijack_alert new_hijack_alert(char* alert_type, char* source_ip, char* destination_ip, char* metric_name) 
{
	hijack_alert *x = (hijack_alert*) malloc(sizeof(hijack_alert));

	x->time_info = get_current_time();

	x->alert_type = (char*) malloc(sizeof(char)*(strlen(alert_type)+1));
	strcpy(x->alert_type, alert_type);
	
	x->source_ip = (char*) malloc(sizeof(char)*(strlen(source_ip)+1));
	strcpy(x->source_ip, source_ip);
	
	x->destination_ip = (char*) malloc(sizeof(char)*(strlen(destination_ip)+1));
	strcpy(x->destination_ip, destination_ip);

	x->metric_name = (char*) malloc(sizeof(char)*(strlen(metric_name)+1));
	strcpy(x->metric_name, metric_name);

	return *x;
}

void save_hijack_alert(hijack_alert alert_parsed)
{
	alerts_history_head = insert_end_h(alerts_history_head, alert_parsed);
}

void show_all_hijack_alerts() 
{
	show_all_nodes_h(alerts_history_head);
}

void show_hijack_alert_structured(hijack_alert alert) 
{
	printf("Action: %s\n", alert.alert_type);
//	printf("TimeStamp: %s\n", alert.timestamp);
	printf("TimeStamp: %s\n", get_time_custom_string(alert.time_info));
	printf("Source IP: %s\n", alert.source_ip);
	printf("Destination IP: %s\n", alert.destination_ip);
	printf("Metric Name: %s\n", alert.metric_name);
}

void show_hijack_alert_simple(hijack_alert alert) 
{
	//printf("%s,%s,%s,%s,%s\n", alert.timestamp, alert.alert_type, alert.source_ip, alert.destination_ip, alert.metric_name);
	printf("%s,%s,%s,%s,%s\n", get_time_custom_string(alert.time_info), alert.alert_type, alert.source_ip, alert.destination_ip, alert.metric_name);
	//printf("-> %s--> %s,%s,%s,%s\n", get_time_asctime_string(alert.time_info), alert.alert_type, alert.source_ip, alert.destination_ip, alert.metric_name);
}

void init_alerts_history_head() 
{
	alerts_history_head = NULL;
}

/********************************************************
			Functions - linked lists specifics
*********************************************************/

node_link new_node_h(hijack_alert alert)
{
	node_link x = (node_link) malloc(sizeof(struct node));
	x->item = alert;
	x->next = NULL;
	return x;
}

node_link insert_begin_h(node_link head, hijack_alert alert)
{
	node_link x = new_node_h(alert);
	x->next = head;
	return x;
}

node_link insert_end_h(node_link head, hijack_alert alert)
{
	node_link x;
	
	if(head == NULL) {
		return new_node_h(alert);	
	}

	for(x = head; x->next != NULL; x = x->next);
	
	x->next = new_node_h(alert);
	return head;
}

void show_all_nodes_h(node_link node)
{
	if (node == NULL) return;
	
	show_hijack_alert_simple(node->item);

	show_all_nodes_h(node->next);
}