#!/bin/sh

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <node-number>" >&2
  exit 1
fi

cd ../SCNodeCode

# Arg $1 is the number of the node, the id
sh run_args.sh 1005$1 127.0.0.1 7892 9899 0
