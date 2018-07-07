#!/bin/sh

echo "-> Pulling the latest version of MACHETE ..."
git pull;

echo "-> Make Multipath Device code ..."
cd ../SCMultipath
make clean; make;

echo "-> Make Overlay Node code ..."
cd ../SCNodeCode
make clean; make;

echo "-> Returning to evaluation folder ..."
cd ../evaluation
