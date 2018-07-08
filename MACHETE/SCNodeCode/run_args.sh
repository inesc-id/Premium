#!/bin/sh

# Arg $1 is MPM Adapter client ID
# Arg $2 is MPM Adapter IP
# Arg $3 is MPM Adapter Port
# Arg $4 is Overlay node port
# Arg $5 is (1 - Use Stun Server, 0 - Use local IPs)

if [ "$#" -lt 5 ]; then
  echo "Usage: $0 <MPMAdapterClientID> <MPMAdapterIP> <MPMAdapterPort> <OverlayNodePort> <UseStunServer>" >&2
  exit 1
fi

# MPM Adapter
echo "Starting MPM Adapter..."
cd ..
cd MPMAdapter
mvn exec:java -Dclient.id=$1 &
ant -f waitforserver.xml -Dhost=$2 -Dport=$3 >/dev/null 2>&1
echo "MPM Adapter Running"

cd ..
cd SCNodeCode

# Run main program
./fwd $2 $3 $4 $5 &
