#!/bin/sh

cd ..

# for the first argument of mpd: 0 - receiver, 1 - sender
./file_client 1 127.0.0.1 7892 10.0.2.19 9895 9899 2 0 5 linux-image.jpg
