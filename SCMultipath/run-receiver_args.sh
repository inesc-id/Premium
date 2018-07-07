#!/bin/sh

# Receiver

# Arg $1 is MPM Adapter Client ID
# Arg $2 is MPM Adapter IP
# Arg $3 is MPM Adapter Port
# Arg $4 is Receiver port
# Arg $5 indicates if the Receiver's IPs must come from Stun Server
#        (1 - Use Stun Server, 0 - Use local IPs)

if [ "$#" -lt 5 ]; then
  echo "Usage: $0 <AdapterClientID> <MPMAdapterIP> <MPMAdapterPort> <ReceiverPort> <UseSTUN>" >&2
  exit 1
fi

# MPM Adapter
echo "Starting MPM Adapter..."
cd ..
cd MPMAdapter
mvn exec:java -Dclient.id=$1&
ant -f waitforserver.xml -Dhost=$2 -Dport=$3 >/dev/null 2>&1
echo "MPM Adapter Running"

cd ..
cd SCMultipath

# for the first argument of mpd: 0 - receiver, 1 - sender
./hi_server 0 $2 $3 $4 $5
