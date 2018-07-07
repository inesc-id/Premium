cd config/
rm hosts.config
rm currentView
touch hosts.config
echo "0 $1 11000" > hosts.config
