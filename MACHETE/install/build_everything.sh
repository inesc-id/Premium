#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <MPM IP Address> <MPM Port>" >&2
  exit 1
fi

# Arguments
# $1 DepSpacito IP
# $2 DepSpacito Port

# Build DepSpacito
cd ../../DepSpacito
sh build_args.sh $1 $2

# Build Sender and Receiver
cd ../MACHETE/SCMultipath
sh build_args.sh $1 $2

# Build Overlay Node
cd ../SCNodeCode
sh build_args.sh $1 $2

# Build Darshana
cd ../../darshana/darshana
sh build.sh
# sh getGeoLiteCity.sh

# Returning to install folder
cd ../../MACHETE/install

echo "Now you can run the system"
