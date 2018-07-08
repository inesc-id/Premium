#!/bin/sh
# Arg 1: alerts receiver ip
# Arg 2: alerts receiver port

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <alerts-receiver-ip> <alerts-receiver-port>" >&2
  exit 1
fi

./dar $1 $2