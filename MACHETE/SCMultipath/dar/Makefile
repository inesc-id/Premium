FLAGS:="-pthread"

dar: alerts_receiver.h alerts_receiver.o utils.o hijack_alert_handler.o monitored_paths_handler.o
	gcc -o dar alerts_receiver.o alerts_receiver.h utils.o utils.h hijack_alert_handler.o hijack_alert_handler.h monitored_paths_handler.o monitored_paths_handler.h ${FLAGS}

alerts_receiver.o: alerts_receiver.h
	gcc -c alerts_receiver.c ${FLAGS}

utils.o: utils.h
	gcc -c utils.c ${FLAGS}

hijack_alert_handler.o: hijack_alert_handler.h
	gcc -c hijack_alert_handler.c ${FLAGS}

monitored_paths_handler.o: monitored_paths_handler.h
	gcc -c monitored_paths_handler.c ${FLAGS}

clean: 
	rm -f dar alerts_receiver.o utils.o hijack_alert_handler.o monitored_paths_handler.o
