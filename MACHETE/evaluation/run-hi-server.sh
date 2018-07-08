#!/bin/sh

cd ../SCMultipath

# for the first argument of mpd: 0 - receiver, 1 - sender
./hi_server 0 127.0.0.1 7892 9898 0
