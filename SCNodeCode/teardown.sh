#!/bin/sh

echo "-> Flushing NAT table ..."
iptables -t nat -F

echo "-> Killing all related running processes java, python and fwd ..."
pkill java
pkill python
pkill fwd

echo "-> Wait 2 secs to give time to the processes ..."
sleep 2

echo "-> Listing NAT table ..."
iptables -t nat -L

echo "-> Listing processes and used ports ..."
lsof -i -P -n