INT=$1
EXT=$2
PORT=$3

sudo iptables -t nat -A PREROUTING -d $EXT -p tcp --dport $PORT -j DNAT --to-destination $1
