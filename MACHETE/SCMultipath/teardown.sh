#!/bin/sh

echo "-> Flushing NAT table ..."
iptables -t nat -F

echo "-> Killing all related running processes java, python and dar..."
#pkill java
pkill python
#pkill dar

echo "-> Killing all hi programs ..."
pkill hi_server
pkill hi_client

#echo "-> Killing all file programs ..."
#pkill file_server
#pkill file_client

echo "-> Wait 2 secs to give time to the processes ..."
sleep 2

echo "-> Listing NAT table ..."
iptables -t nat -L

echo "-> Listing processes and used ports ..."
lsof -i -P -n