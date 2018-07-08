echo "Install necessary libs ..."
apt-get install libffi-dev libssl-dev
echo "Getting get-pip.py file ..."
curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
echo "Executing get-pip.py ..."
python get-pip.py
echo "Installing PyCrypto with pip ..."
pip install pycrypto
