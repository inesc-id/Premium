#!/bin/sh
# Arg 1: hijacker ip
# Arg 2: client ip

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <hijacker-ip> <client-ip>" >&2
  exit 1
fi

mvn exec:java -Dexec.mainClass="pt.ulisboa.tecnico.darshana.BeginHijack" -Dexec.args="$1 $2" -q
