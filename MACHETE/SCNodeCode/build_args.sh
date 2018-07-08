#!/bin/sh
#ARG1=MPM IP Address
#ARG2=MPM Port
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <MPM IP> <MPM Port>"
  exit 1
fi

#enable packet forwarding
bash -c 'echo 1 > /proc/sys/net/ipv4/ip_forward'

MPM_IP=$1
MPM_PORT=$2
CURRENT_DIR=`pwd`
LOG_FILE="$CURRENT_DIR/build.log"

#deleting previous build.log file
rm -f "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "Configuring hosts.config..." | tee -a "$LOG_FILE"
echo "Installing SCNodeCode..." | tee -a "$LOG_FILE"
echo "MPM IP Address: $MPM_IP" | tee -a "$LOG_FILE"
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
echo "----------------------- DepSpacito -----------------------" | tee -a "$LOG_FILE"
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
echo "----------------------- MPM Adapter -----------------------" | tee -a "$LOG_FILE"
echo "----------------------- mvn compile ----------------------- " >> $LOG_FILE
mvn --quiet clean compile | tee -a "$LOG_FILE"

cd ..
cd SCNodeCode
echo "" | tee -a "$LOG_FILE"
echo "----------------------- SCNodeCode -----------------------" | tee -a "$LOG_FILE"

echo "Compiling SCNodeCode..." | tee -a "$LOG_FILE"
echo "Cleaning old installation files..." | tee -a "$LOG_FILE"
make clean >> $LOG_FILE

echo "Compiling SCNodeCode..." | tee -a "$LOG_FILE"
make >> "$LOG_FILE"

echo "Done. Log file -> $LOG_FILE" | tee -a "$LOG_FILE"

