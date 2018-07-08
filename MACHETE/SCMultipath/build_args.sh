#!/bin/sh

#ARG1=MPM IP Address
#ARG2=MPM Port

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <MPM IP Address> <MPM Port>" >&2
  exit 1
fi

MPM_IP=$1
MPM_PORT=$2
CURRENT_DIR=`pwd`
LOG_FILE="$CURRENT_DIR/build.log"

#deleting previous build.log file
rm -f $LOG_FILE

echo "" | tee -a "$LOG_FILE"
echo "Configuring hosts.config..." | tee -a "$LOG_FILE"
echo "Installing SCMultipath..." | tee -a "$LOG_FILE"
echo "MPM IP: $MPM_IP" | tee -a "$LOG_FILE"
echo "MPM Port: $MPM_PORT" | tee -a "$LOG_FILE"
echo "Configuring MPM Adapter..." | tee -a "$LOG_FILE"
rm ../MPMAdapter/config/hosts.config
echo "0 $MPM_IP $MPM_PORT" > ../MPMAdapter/config/hosts.config

echo "" | tee -a "$LOG_FILE"
echo "Installing MPM Adapter dependencies..." | tee -a "$LOG_FILE"
cd ..
cd ..
cd DepSpacito
echo "" >> $LOG_FILE
echo "" >> $LOG_FILE
echo "----------------------- DepSpacito -----------------------" >> $LOG_FILE
echo "----------------------- mvn clean ----------------------- " >> $LOG_FILE
mvn --quiet clean | tee -a "$LOG_FILE"
echo "" >> $LOG_FILE
echo "----------------------- mvn install -----------------------" >> $LOG_FILE
mvn --quiet install -DskipTests | tee -a "$LOG_FILE"
echo "" >> $LOG_FILE
echo "" >> $LOG_FILE

echo "Installing MPM Adapter..." | tee -a "$LOG_FILE"
cd ..
cd MACHETE
cd MPMAdapter
echo "" >> $LOG_FILE
echo "" >> $LOG_FILE
echo "----------------------- MPM Adapter -----------------------" >> $LOG_FILE
echo "----------------------- mvn compile ----------------------- " >> $LOG_FILE
mvn --quiet clean compile | tee -a "$LOG_FILE"

cd ..
cd SCMultipath
echo "" | tee -a "$LOG_FILE"
echo "Compiling SCMultipath..." | tee -a "$LOG_FILE"
echo "Cleaning old installation files..." | tee -a "$LOG_FILE"
make clean >> "$LOG_FILE"

echo "Compiling SCMultipath..." | tee -a "$LOG_FILE"
make >> "$LOG_FILE"
make file_client >> "$LOG_FILE"
make file_server >> "$LOG_FILE"
make hi_client >> "$LOG_FILE"
make hi_server >> "$LOG_FILE"

echo "Done. Log file -> $LOG_FILE" | tee -a "$LOG_FILE"


