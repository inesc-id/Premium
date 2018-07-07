#!/bin/sh

# Argument 1 = Receiver IP
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <receiver-ip>" >&2
  exit 1
fi

cd ../SCMultipath

# for the first argument of mpd: 0 - receiver, 1 - sender
./hi_client 1 127.0.0.1 7892 $1 9898 9899 2 0 5 linux-image.jpg
