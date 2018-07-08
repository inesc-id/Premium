#!/bin/bash

echo "-> Installing mptcp from apt-repository ..."
# Checkout here the guide: https://multipath-tcp.org/pmwiki.php/Users/AptRepository

apt-key adv --keyserver hkp://keys.gnupg.net --recv-keys 379CE192D401AB61
sh -c "echo 'deb https://dl.bintray.com/cpaasch/deb stretch main' > /etc/apt/sources.list.d/mptcp.list"
apt-get update
apt-get install apt-transport-https
apt-get update
apt-get install linux-mptcp

echo "-> Getting configuration scripts - mptcp_up and mptcp_down - for Debian-based system"

echo "-> Placing mptcp_up in /etc/network/if-up.d/..."

wget https://github.com/multipath-tcp/mptcp-scripts/blob/master/scripts/rt_table/mptcp_up
mv ./mptcp_up /etc/network/if-up.d/

echo "-> Placing mptcp_down in /etc/network/if-post-down.d/ ..."

wget https://github.com/multipath-tcp/mptcp-scripts/blob/master/scripts/rt_table/mptcp_down
mv ./mptcp_down /etc/network/if-post-down.d/

echo "-> Check if MPTCP was successfully installed ..."
dmesg | grep MPTCP

echo "-> If no result, please reboot the machine ..."
