#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "on_rules_utils.h"

#define FLUSH_NAT_TABLE_COMMAND "iptables -t nat -F"
#define COMMAND_SIZE 500

int forwardPaths(char *overlay_addr, char *source_addr, char *dest_addr, char *dest_port)
{
	char command[COMMAND_SIZE];
	//iptables -t nat -A PREROUTING -p tcp -s 1.1.1.1 -d 2.2.2.2 -j DNAT --to-destination 3.3.3.3
	//iptables -t nat -A POSTROUTING -p tcp -s 1.1.1.1 -d 3.3.3.3 -j SNAT --to-source 2.2.2.2
	//iptables -t nat -A PREROUTING -p tcp -s 3.3.3.3 -d 2.2.2.2 -j DNAT --to-destination 1.1.1.1
	//iptables -t nat -A POSTROUTING -p tcp -s 3.3.3.3 -d 1.1.1.1 -j SNAT --to-source 2.2.2.2
	strcpy(command, "iptables -t nat -A PREROUTING -p tcp --dport ");
	strcat(command, dest_port);
	strcat(command, " -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, overlay_addr);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, dest_addr);
	printf("[Forward paths] Command 1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A POSTROUTING -p tcp -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, dest_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, overlay_addr);
	printf("[Forward paths] Command 2: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A PREROUTING -p tcp --dport ");
	strcat(command, dest_port);
	strcat(command, " -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, overlay_addr);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, source_addr);
	printf("[Forward paths] Command 3: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -A POSTROUTING -p tcp -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, source_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, overlay_addr);
	printf("[Forward paths] Command 4: %s\n", command);
	system(command);
	return 1;
}

int removeRules(char *overlay_addr, char *source_addr, char *dest_addr, char *dest_port)
{
	char command[COMMAND_SIZE];
	strcpy(command, "iptables -t nat -D PREROUTING -p tcp --dport ");
	strcat(command, dest_port);
	strcat(command, " -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, overlay_addr);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, dest_addr);
	printf("[Remove rules] Command 1: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D POSTROUTING -p tcp -s ");
	strcat(command, source_addr);
	strcat(command, " -d ");
	strcat(command, dest_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, overlay_addr);
	printf("[Remove rules] Command 2: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D PREROUTING -p tcp --dport ");
	strcat(command, dest_port);
	strcat(command, " -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, overlay_addr);
	strcat(command, " -j DNAT --to-destination ");
	strcat(command, source_addr);
	printf("[Remove rules] Command 3: %s\n", command);
	system(command);

	strcpy(command, "iptables -t nat -D POSTROUTING -p tcp -s ");
	strcat(command, dest_addr);
	strcat(command, " -d ");
	strcat(command, source_addr);
	strcat(command, " -j SNAT --to-source ");
	strcat(command, overlay_addr);
	printf("[Remove rules] Command 4: %s\n", command);
	system(command);
	return 1;
}

void flush_nat_iptables()
{
	if(DEBUG) printf("[Rules] Flush: %s\n", FLUSH_NAT_TABLE_COMMAND);
	system(FLUSH_NAT_TABLE_COMMAND);
}