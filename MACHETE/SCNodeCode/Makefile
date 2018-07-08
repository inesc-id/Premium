FLAGS:="-pthread"

fwd: register_dep.o stun_public_ip.o forwarding_handler.o on_rules_utils.o debug.h
	gcc -o fwd register_dep.o stun_public_ip.o forwarding_handler.o on_rules_utils.o debug.h ${FLAGS}

forwardind_handler.o: forwarding_handler.c stun_public_ip.o register_dep.o on_rules_utils.o debug.h forwarding_handler.h
	gcc -c forwarding_handler.c ${FLAGS}

stun_public_ip.o: stun_public_ip.c stun_public_ip.h debug.h
	gcc -c stun_public_ip.c ${FLAGS}

register_dep.o: register_dep.h register_dep.c debug.h
	gcc -c register_dep.c ${FLAGS}

on_rules_utils.o: on_rules_utils.c on_rules_utils.h
	gcc -c on_rules_utils.c ${FLAGS}

clean:
	rm -f fwd register_dep.o stun_public_ip.o forwarding_handler.o on_rules_utils.o
