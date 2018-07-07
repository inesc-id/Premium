FLAGS:="-pthread"
SENDER_DEPENDENCIES:=         \
	utils/debug.h             \
	common.h                  \
	premium_specs.h           \
	connection_manager.o      \
	path_monitoring.o         \
	alerts_receiver.o         \
	stun_public_ip.o          \
	machete_sender.o          \
	connectivity_utils.o      \
	log_utils.o               \
	virtual_int_utils.o       \
	rules_utils.o             \
	overlay_manager.o         \
	register_dep.o            \
	sending_handler.o utils.o \
	hijack_alert_handler.o    \
	monitored_paths_handler.o

RECEIVER_DEPENDENCIES:= \
	utils/debug.h       \
	common.h            \
	stun_public_ip.o    \
	machete_receiver.o  \
	log_utils.o         \
	register_dep.o      \
	receiving_handler.o
	
# actions

all: file_client file_server hi_client hi_server hi_client_eval hi_server_eval 

clean: 
	rm -f file_client file_server hi_client hi_server hi_client_eval hi_server_eval 
	rm -f run.o file_client.o file_server.o hi_client.o hi_server.o
	rm -f connection_manager.o overlay_manager.o path_monitoring.o machete_sender.o sending_handler.o
	rm -f machete_receiver.o receiving_handler.o
	# Clean utils
	rm -f virtual_int_utils.o connectivity_utils.o log_utils.o rules_utils.o stun_public_ip.o register_dep.o
	# Clean DAR
	rm -f dar_exec alerts_receiver.o utils.o hijack_alert_handler.o monitored_paths_handler.o

hi: hi_client hi_server

file: file_client file_server

# Executables

#mpd: run.o utils/debug.h stun_public_ip.o path_monitoring.o connection_manager.o machete_receiver.o machete_sender.o connectivity_utils.o log_utils.o rules_utils.o virtual_int_utils.o overlay_manager.o register_dep.o receiving_handler.o sending_handler.o
#	gcc -o mpd run.o utils/debug.h stun_public_ip.o path_monitoring.o connection_manager.o machete_receiver.o machete_sender.o connectivity_utils.o log_utils.o rules_utils.o virtual_int_utils.o overlay_manager.o register_dep.o receiving_handler.o sending_handler.o ${FLAGS}

file_client: file_client.o ${SENDER_DEPENDENCIES}
	gcc -o file_client file_client.o ${SENDER_DEPENDENCIES} ${FLAGS}

file_server: file_server.o ${RECEIVER_DEPENDENCIES}
	gcc -o file_server file_server.o ${RECEIVER_DEPENDENCIES} ${FLAGS}

hi_client: hi_client.o ${SENDER_DEPENDENCIES}
	gcc -o hi_client hi_client.o ${SENDER_DEPENDENCIES} ${FLAGS}

hi_server: hi_server.o ${RECEIVER_DEPENDENCIES}
	gcc -o hi_server hi_server.o ${RECEIVER_DEPENDENCIES} ${FLAGS}

# for evaluations

hi_client_eval: hi_client_eval.o ${SENDER_DEPENDENCIES}
	gcc -o hi_client_eval hi_client_eval.o ${SENDER_DEPENDENCIES} ${FLAGS}

hi_server_eval: hi_server_eval.o ${RECEIVER_DEPENDENCIES}
	gcc -o hi_server_eval hi_server_eval.o ${RECEIVER_DEPENDENCIES} ${FLAGS}

# Applications

file_client.o: machete_sender.o
	gcc -c file_client.c ${FLAGS}

file_server.o: machete_receiver.o
	gcc -c file_server.c ${FLAGS}

hi_client.o: machete_sender.o
	gcc -c hi_client.c ${FLAGS}

hi_server.o: machete_receiver.o
	gcc -c hi_server.c ${FLAGS}

hi_client_eval.o: machete_sender.o
	gcc -c hi_client_eval.c ${FLAGS}

hi_server_eval.o: machete_receiver.o
	gcc -c hi_server_eval.c ${FLAGS}

# Main Modules

machete_sender.o: machete_sender.c machete_sender.h
	gcc -c machete_sender.c ${FLAGS}

machete_receiver.o: machete_receiver.c machete_receiver.h
	gcc -c machete_receiver.c ${FLAGS}

sending_handler.o: utils/debug.h common.h premium_specs.h register_dep.o path_monitoring.o connection_manager.o log_utils.o rules_utils.o stun_public_ip.o virtual_int_utils.o connectivity_utils.o overlay_manager.o sending_handler.c sending_handler.h 
	gcc -c sending_handler.c ${FLAGS}

receiving_handler.o: utils/debug.h common.h register_dep.o log_utils.o stun_public_ip.o receiving_handler.c receiving_handler.h
	gcc -c receiving_handler.c ${FLAGS}

register_dep.o: utils/debug.h utils/config_ips.h register_dep.c register_dep.h
	gcc -c register_dep.c ${FLAGS}

overlay_manager.o: overlay_manager.c overlay_manager.h
	gcc -c overlay_manager.c ${FLAGS}

connection_manager.o: connection_manager.c connection_manager.h
	gcc -c connection_manager.c ${FLAGS}

path_monitoring.o: alerts_receiver.o path_monitoring.c path_monitoring.h
	gcc -c path_monitoring.c ${FLAGS}

# Utils

log_utils.o: utils/log_utils.c utils/log_utils.h
	gcc -c utils/log_utils.c ${FLAGS}

rules_utils.o: utils/rules_utils.c utils/rules_utils.h
	gcc -c utils/rules_utils.c ${FLAGS}

virtual_int_utils.o: utils/virtual_int_utils.c utils/virtual_int_utils.h
	gcc -c utils/virtual_int_utils.c ${FLAGS}

connectivity_utils.o: utils/connectivity_utils.c utils/connectivity_utils.h
	gcc -c utils/connectivity_utils.c ${FLAGS}

stun_public_ip.o: utils/debug.h utils/config_ips.h utils/stun_public_ip.c utils/stun_public_ip.h
	gcc -c utils/stun_public_ip.c ${FLAGS}

# Darshana Alert Receiver (DAR)

dar_exec: dar/alerts_receiver.h alerts_receiver.o utils.o hijack_alert_handler.o monitored_paths_handler.o
	gcc -o dar_exec alerts_receiver.o utils.o hijack_alert_handler.o monitored_paths_handler.o ${FLAGS}

alerts_receiver.o: dar/alerts_receiver.h dar/alerts_receiver.c utils.o hijack_alert_handler.o monitored_paths_handler.o
	gcc -c dar/alerts_receiver.c ${FLAGS}

utils.o: dar/utils.h
	gcc -c dar/utils.c ${FLAGS}

hijack_alert_handler.o: dar/hijack_alert_handler.h
	gcc -c dar/hijack_alert_handler.c ${FLAGS}

monitored_paths_handler.o: dar/monitored_paths_handler.h
	gcc -c dar/monitored_paths_handler.c ${FLAGS}
	