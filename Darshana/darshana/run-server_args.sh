#!/bin/sh
# Arg 1: destination ip
# Arg 2: destination port

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <destination-ip> <destination-port>" >&2
  exit 1
fi

python CryptoPingServer.py $1 $2