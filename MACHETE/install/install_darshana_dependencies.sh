#!/bin/bash

# Darshana build commands for Debian ...

# git (if needed)
# apt-get install git -y

# python 2.7.9 -----------------------------

apt-get install python -y

# pycrypto v2.6.1 -----------------------------

apt-get install python-pip -y
apt-get install python-crypto -y

# list python installed modules
# pip freeze 

# java v8

apt-get install software-properties-common -y
add-apt-repository "deb http://ppa.launchpad.net/webupd8team/java/ubuntu xenial main"
apt-get update -y
apt-get install oracle-java8-installer -y

# maven v3.3.9 --------------------------------

apt-get install maven -y

# paris-traceroute ----------------------------

aptitude update -y
aptitude safe-upgrade -y
aptitude install autoconf build-essential git libtool -y

mkdir ~/git
cd ~/git
git clone https://github.com/libparistraceroute/libparistraceroute.git
cd ~/git/libparistraceroute

mkdir m4
./autogen.sh
./configure
make

make install

ldconfig

cd ~/git/libparistraceroute
make all install

# whois --------------------------------------

apt-get install whois -y

# curl (optional) --------------------------------------

apt-get install curl -y

# Checking versions

echo "END -> Checking versions..."
git --version
java -version
javac -version
python --version
pip --version
pip show pycrypto
mvn -version
whois --version
paris-traceroute --version
curl -V
