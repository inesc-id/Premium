cd MultipathManager/
make
./scripts/client-single /config/ &
sleep 5
cd ..
gcc forwarding_handler.c register_dep.c stun_public_ip.c -o fwd
./fwd 127.0.0.2 7892 &
