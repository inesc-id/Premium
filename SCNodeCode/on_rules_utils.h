#ifndef ON_RULES_UTILS_H_
#define ON_RULES_UTILS_H_

int forwardPaths(char *overlay_addr, char *source_addr, char *dest_addr, char *dest_port);

int removeRules(char *overlay_addr, char *source_addr, char *dest_addr, char *dest_port);

void flush_nat_iptables();

#endif /* ON_RULES_UTILS_H_ */