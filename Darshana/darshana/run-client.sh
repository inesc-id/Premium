#!/bin/sh
# Arg 1: monitoring mode
# Arg 2: destination ip

sh run-client_args.sh $1 `hostname -I` $2 10008 0.95 1 0.8 1.5 0.3 1