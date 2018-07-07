#!/bin/sh

# MPM Adapter
echo "Starting MPM Adapter..."
cd ..
cd ../MPMAdapter
mvn exec:java -Dclient.id=$1&
ant -f waitforserver.xml -Dhost=127.0.0.1 -Dport=7892 >/dev/null 2>&1
echo "MPM Adapter Running"