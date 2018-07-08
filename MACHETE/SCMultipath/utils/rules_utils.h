#ifndef RULES_UTILS_H_
#define RULES_UTILS_H_

void set_sender_own_rules_iptables(char *sender_ip, char *receiver_ip, char *overlay_node_ip);

void remove_sender_own_rules_iptables(char *sender_ip, char *receiver_ip, char *overlay_node_ip);

void flush_nat_iptables();

#endif /* RULES_UTILS_H_ */