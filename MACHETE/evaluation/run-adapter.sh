#!/bin/sh

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <MPMAdapterClientID>" >&2
  exit 1
fi

# MPM Adapter
echo "Starting MPM Adapter..."
cd ../MPMAdapter
mvn exec:java -Dclient.id=$1&
ant -f waitforserver.xml -Dhost=127.0.0.1 -Dport=7892 >/dev/null 2>&1
echo "MPM Adapter Running"