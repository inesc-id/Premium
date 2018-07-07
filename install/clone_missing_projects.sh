#!/bin/sh

cd ../../

echo "Cloning DepSpacito ..."
git clone https://github.com/inesc-id/DepSpacito

echo "Cloning Darshana ..."
git clone https://github.com/inesc-id/darshana

cd darshana

echo "Checkingout alerts-receiver branch ..."
git checkout alerts-receiver
