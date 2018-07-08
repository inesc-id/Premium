# install need packages
aptitude update
aptitude safe-upgrade
aptitude install autoconf build-essential git libtool

# get sources
mkdir ~/git
cd ~/git
git clone https://github.com/libparistraceroute/libparistraceroute.git
cd ~/git/libparistraceroute

# Compile the source
mkdir m4
./autogen.sh
./configure
make