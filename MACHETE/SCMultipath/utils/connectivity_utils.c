#include <stdio.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h> // for close
#include "debug.h"
#include "connectivity_utils.h"

#define PACKETSIZE 64

// Global variables

struct protoent *proto = NULL;
int pid = -1;

struct packet
{
	struct icmphdr hdr;
	char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

unsigned short checksum (void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int ping_overlay_node (char *addr_str)
{
	if(DEBUG) printf("[Pinging hops] going to ping %s\n", addr_str);
	const int val=255;
	int i, j, sd, cnt=1;
	struct packet pckt;
	struct sockaddr_in r_addr;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = inet_addr(addr_str);
	memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

	proto = getprotobyname("ICMP");

	sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
	if (sd < 0) {
		if(DEBUG) printf("[Pinging hops] Socket failed to create\n");
		perror("socket");
		return -1;
	} else {
		if(DEBUG) printf("[Pinging hops] Socket created, continuing\n");
	}
	if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
		perror("Set TTL option");
	if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
		perror("Request nonblocking I/O");
	for (j = 0 ; j < 10 ; j++ )
	{
	usleep(250000);
	int len = sizeof(r_addr);

	if(DEBUG) printf("[Pinging hops] Msg #%d\n", cnt);
	if (recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0){
		if(DEBUG) printf("[Pinging hops] ***Got message!***\n");
		close(sd);
		return 1;
	}
	bzero(&pckt, sizeof(pckt));
	pckt.hdr.type = ICMP_ECHO;
	pckt.hdr.un.echo.id = pid;
	for (i = 0; i < sizeof(pckt.msg)-1; i++) {
		pckt.msg[i] = i+'0';
	}

	pckt.msg[i] = 0;
	pckt.hdr.un.echo.sequence = cnt++;
	pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
	if (sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0)
		perror("sendto");
	}

	if(DEBUG) printf("[Pinging hops] Returning with no message\n");
	return -1;
}

int probe_overlay_nodes(int num_hops, char **nodes)
{
	if (nodes[0] == NULL) {
		return 0;
	}

	if (DEBUG) printf("[Probing Nodes] Malloc new nodes\n");
	int i = 0, j = 0;
	
	while (i < num_hops) {
		if (DEBUG) printf("[Probing Nodes] Pinging %s\n", nodes[i]);

		if (ping_overlay_node(nodes[i]) == 1) {
			i++;
			if (DEBUG) printf("[Probing Nodes] Adding %d\n", i);
		
		} else {
			nodes[i] = nodes[i+1];
			if (DEBUG) printf("[Probing Nodes] Passing in front of %d\n", i);
		}
	}
	return i;
}