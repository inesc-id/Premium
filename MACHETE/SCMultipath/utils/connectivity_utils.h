#ifndef CONNECTIVITY_UTILS_H_
#define CONNECTIVITY_UTILS_H_

unsigned short checksum(void *b, int len);

/*
 * Pings a specific Overlay Node to know if it is responsive
 */
int ping_overlay_node(char *addr_str);

/*
 * Probes Overlay Nodes to know if these are available
 */
int probe_overlay_nodes(int num_hops, char **nodes);

#endif /* CONNECTIVITY_UTILS_H_ */