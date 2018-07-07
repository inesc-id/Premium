#!/bin/sh

# Sender

# Arg $1 is MPM Adapter Client ID
# Arg $2 is MPM Adapter IP
# Arg $3 is MPM Adapter Port
# Arg $4 is Receiver IP
# Arg $5 is Receiver Port
# Arg $6 is Overlay Port
# Arg $7 is Minimum number of overlay nodes
# Arg $8 indicates if the Receiver's IPs must come from Stun Server
#        (1 - Use Stun Server, 0 - Use local IPs)
# Arg $9 is maximun number of DAR alerts
# Arg $10 is Filename

if [ "$#" -lt 8 ]; then
  echo "Usage: $0 <MPMAdapterClientID> <MPMAdapterIP> <MPMAdapterPort> <ReceiverIP> <ReceiverPort> <OverLayNodePort> <MinNumberOfOverlayNodes> <UseSTUN> <MaxDarAlerts> <Filename>" >&2
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
./hi_client 1 $2 $3 $4 $5 $6 $7 $8 $9
