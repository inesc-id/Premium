#!/bin/sh
# Arg 1: monitoring mode
# Arg 2: client ip
# Arg 3: destination ip
# Arg 4: destination port
# Arg 5: lat threshold
# Arg 6: hop threshold
# Arg 7: path threshold
# Arg 8: prop threshold
# Arg 9: path nones threshold
# Arg 10: prob period in seconds
# Example of args: "hop 10.0.2.26 10.0.2.28 10008 1 1 0.8 1.5 0.5 1"

if [ "$#" -lt 10 ]; then
  echo "Usage: $0 <monitoring-mode> <client-ip> <destination-ip> <destination-port> <lat> <hop> <path> <prop> <path-nones> <probe-period>" >&2
  exit 1
fi

mvn exec:java -Dexec.mainClass="pt.ulisboa.tecnico.darshana.RouteMon" -Dexec.args="$1 $2 $3 $4 $5 $6 $7 $8 $9 ${10}" -q