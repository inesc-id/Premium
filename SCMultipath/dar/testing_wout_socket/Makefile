FLAGS:="-pthread"

parse: parse_hijack.h parse_hijack.o utils.o hijack_alert_handler.o monitored_paths_handler.o
	gcc -o parse_hijack parse_hijack.o parse_hijack.h utils.o ../utils.h hijack_alert_handler.o ../hijack_alert_handler.h monitored_paths_handler.o ../monitored_paths_handler.h ${FLAGS}

parse_hijack.o: parse_hijack.h
	gcc -c parse_hijack.c ${FLAGS}

utils.o: ../utils.h
	gcc -c ../utils.c ${FLAGS}

hijack_alert_handler.o: ../hijack_alert_handler.h
	gcc -c ../hijack_alert_handler.c ${FLAGS}

monitored_paths_handler.o: ../monitored_paths_handler.h
	gcc -c ../monitored_paths_handler.c ${FLAGS}

clean: 
	rm -f parse_hijack utils.o hijack_alert_handler.o parse_hijack.o monitored_paths_handler.o