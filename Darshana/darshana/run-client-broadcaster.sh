#!/bin/sh
# Arg 1: monitoring mode
# Arg 2: destination ip
# Arg 3: darshana alert receiver ip
# Arg 4: darshana alert receiver port

sh run-client-broadcaster_args.sh $1 `hostname -I` $2 10008 1 1 0.8 1.5 0.3 1 $3,$4
