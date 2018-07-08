#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// specific header files
#include "monitored_paths_handler.h"

// global variables

path_node_link monitored_paths_list_head;

/********************************************************
			Functions - path status specifics
*********************************************************/

path_status new_path_status(char* source_ip, char* relay_ip, char* destination_ip)
{
	path_status *x = (path_status*) malloc(sizeof(path_status));

//	x->previous_time_info = (char*) malloc(sizeof(char)*TIME_MAX_LENGHT);
//	x->last_time_info = (char*) malloc(sizeof(char)*TIME_MAX_LENGHT);
	
	x->source_ip = (char*) malloc(sizeof(char)*(strlen(source_ip)+1));
	strcpy(x->source_ip, source_ip);

	x->relay_ip = (char*) malloc(sizeof(char)*(strlen(relay_ip)+1));
	strcpy(x->relay_ip, relay_ip);

	x->destination_ip = (char*) malloc(sizeof(char)*(strlen(destination_ip)+1));
	strcpy(x->destination_ip, destination_ip);
	x->was_hijacked = FALSE;

	return *x;
}

void save_path_status(path_status path_status_struct)
{
	monitored_paths_list_head = insert_end_p(monitored_paths_list_head, path_status_struct);
}

void show_path_status(path_status path) 
{
	printf("-----------> Path Status <---------- \n");

	printf("Before Last Timestamp: ");
	if (path.previous_time_info == NULL) {
		printf("-----------\n");
	} else {
		printf("%s\n", get_time_custom_string(path.previous_time_info));
	}

	printf("Last Timestamp: ");
	if (path.last_time_info == NULL) {
		printf("-----------\n");
	} else {
		printf("%s\n", get_time_custom_string(path.last_time_info));
	}

	printf("Path = SourceIP -> RelayIP -> DestinationIP:\n");
	printf("= %s -> %s -> %s\n", path.source_ip, path.relay_ip, path.destination_ip);

	printf("Was hijacked? ");
	if (path.was_hijacked == TRUE) {
		printf("YES\n");
	} else {
		printf("NO\n");
	}
}

void show_all_paths()
{
	show_all_nodes_p(monitored_paths_list_head);
}

// *********** Hijack alert and Monitored Path Intersection ***********

void analyse_hijack_alert(hijack_alert alert)
{
	path_node_link path_node = lookup_path_by_relay_ip(alert.source_ip);
	if (path_node != NULL)
	{
		update_path_status(path_node, alert);

	} else {
		path_node = lookup_path_by_relay_ip(alert.destination_ip);
		if (path_node != NULL)
		{
			update_path_status(path_node, alert);
		}	
	}
}

void update_path_status(path_node_link path_link, hijack_alert alert) 
{
	path_link->item.was_hijacked = TRUE;
	if (path_link->item.last_time_info == NULL) {
		path_link->item.last_time_info = duplicate_time_structure(alert.time_info);
	} else {
		path_link->item.previous_time_info = path_link->item.last_time_info;
		path_link->item.last_time_info = duplicate_time_structure(alert.time_info);
	}
}

// ********************** Lookups **********************


path_node_link lookup_path_by_relay_ip(char* relay_ip)
{
	return lookup_path_by_relay_ip_recursive(monitored_paths_list_head, relay_ip);
}

path_node_link lookup_path_by_hijack_value(int was_hijacked)
{
	return lookup_path_by_hijack_value_recursive(monitored_paths_list_head, was_hijacked);
}

path_node_link lookup_path_by_relay_ip_recursive(path_node_link starting_point, char* relay_ip)
{
	path_node_link t;
	for(t = starting_point; t != NULL; t = t->next) {
		if(strcmp(t->item.relay_ip, relay_ip) == 0) {
			return t;
		}	
	}
	return NULL;
}

path_node_link lookup_path_by_hijack_value_recursive(path_node_link starting_point, int was_hijacked)
{
	path_node_link t;
	for(t = starting_point; t != NULL; t = t->next) {
		if(t->item.was_hijacked == was_hijacked) {
			return t;
		}	
	}
	return NULL;
}


/********************************************************
			Functions - testing
*********************************************************/

void init_test_path_values()
{
	char* sourceIPs[] = { "10.23.44.1", "10.23.44.2", "10.23.44.3" };
	char* relayIPs[] = { "101.0.2.1", "10.0.2.30", "101.0.2.3" };
	char* destinationIPs[] = { "30.123.2.1", "30.123.2.2", "30.123.2.3" };

	printf("-> Inserting test paths...\n");
	monitored_paths_list_head = insert_end_p(monitored_paths_list_head, new_path_status(sourceIPs[0], relayIPs[0], destinationIPs[0]));
	monitored_paths_list_head = insert_end_p(monitored_paths_list_head, new_path_status(sourceIPs[1], relayIPs[1], destinationIPs[1]));
	monitored_paths_list_head = insert_end_p(monitored_paths_list_head, new_path_status(sourceIPs[2], relayIPs[2], destinationIPs[2]));

/*
	// Testing lookup
	
	printf("-> Test looking up for path with overlay node %s\n", relayIPs[2] );
	//printf("-> Test looking up for path with overlay node %s\n", "LIXOOOOO" );

	path_node_link foundPathNode = lookup_path_by_relay_ip(relayIPs[2]);
	if (foundPathNode == NULL)
	{
		printf("NOooo Path Found!!! \n");
		
	} else {
		printf("Found Path: \n");
		show_path_status(foundPathNode->item);
	}
*/
}

void show_hijacked_paths(int was_hijacked)
{
	path_node_link path_node = lookup_path_by_hijack_value(was_hijacked);
	while(path_node != NULL){
		show_path_status(path_node->item);
		path_node = lookup_path_by_hijack_value_recursive(path_node->next, was_hijacked);
	}
}

/********************************************************
			Functions - linked lists specifics
*********************************************************/

void init_monitored_paths_head() 
{
	monitored_paths_list_head = NULL;
}

path_node_link new_node_p(path_status path_status_structure)
{
	path_node_link x = (path_node_link) malloc(sizeof(struct node_p));
	x->item = path_status_structure;
	x->next = NULL;
	return x;
}

path_node_link insert_begin_p(path_node_link head, path_status path_status_structure)
{
	path_node_link x = new_node_p(path_status_structure);
	x->next = head;
	return x;
}

path_node_link insert_end_p(path_node_link head, path_status path_status_structure)
{
	path_node_link x;
	
	if(head == NULL) {
		return new_node_p(path_status_structure);	
	}

	for(x = head; x->next != NULL; x = x->next);
	
	x->next = new_node_p(path_status_structure);
	return head;
}

void show_all_nodes_p(path_node_link node)
{
	if (node == NULL) return;
	
	show_path_status(node->item);

	show_all_nodes_p(node->next);
}
