#ifndef VIRTUAL_INT_UTILS_H_
#define VIRTUAL_INT_UTILS_H_

void get_local_interfaces(char *primary_interface, int *number_of_sources, int *number_of_destinations, int *number_of_hops, char **srcs, char **dsts);

void create_virtual_interface(int addition, char *primary_interface);

void create_virtual_ifconfig(int addition, char *primary_interface);

void clean_virtual_interfaces();

#endif /* VIRTUAL_INT_UTILS_H_ */