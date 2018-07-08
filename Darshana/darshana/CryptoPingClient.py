    
import os, re, socket, threading, struct, sys, random, string
from ctypes import *
import os.path
import time
from Crypto.Hash import SHA256
from Crypto.PublicKey import RSA

def read_file(filename): 
	
	f = open(filename,'r')

	data = f.read()
	f.close()
	return data

def create_file(filename, data): 
	f = open(filename,'w')

	f.write(data)
	f.close()
	return

def recv_data(sock): 
	
	data_len = struct.unpack('!I',sock.recv(4))
	data = sock.recv(data_len[0])

	return data
    
def send_data(sock,data): 

	data_len = struct.pack('!I',len(data))
	sock.send(data_len)
	sock.send(data)

	return
    
def send_file_contents(file_name,usersock,rsa_key):

	data = read_file(file_name)
	cipher = rsa_key.public_encrypt(data, RSA.pkcs1_padding)
	send_data(usersock,cipher)

	return

def receive_file_contents(file_name,usersock):

	data = recv_data(usersock)

	create_file(file_name,data)

	return 

def getMyIP():
    
    data = str(urlopen('http://checkip.dyndns.com/').read())

    return re.compile('\d+\.\d+\.\d+\.\d+').search(data).group(0)


# Arg 1 : destination IP -> IP to connect with the destination
# Arg 2 : destination port -> port to connect with the destination
def main():

	destination_ip = sys.argv[1]
	destination_port = int(sys.argv[2])

	print "Starting Cryptographic ping to destination at " + destination_ip + ":" + str(destination_port)

	pub_name = destination_ip + "pub.key"
	order = "continue"
	begin_time = 0
	end_time = 0
	rtt = 0
	nonce = 0
	ciphNonce = 0

	sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	sock.connect((destination_ip,destination_port))

	if(not(os.path.isfile(pub_name))):
		order = "GETPUB"					
	
	send_data(sock,order)

	if(order == "GETPUB"):
		send_data(sock,"pub.key")
		receive_file_contents(pub_name,sock)

	pub = open(pub_name,"r")
	keyPub = RSA.importKey(pub)
	pub.close()

	nonce = random.expovariate(0.5)
	begin_time = time.time()

	print "Sending nonce", nonce
	send_data(sock,str(nonce))
	signature = recv_data(sock)

	end_time = time.time()

	hash = SHA256.new(str(nonce)).digest()
	s = ''.join(x for x in signature.split("-")[0] if x.isdigit())

	rtt = end_time - begin_time
	rtt = rtt * 1000 #milisegundos

	if(keyPub.verify(hash, (long(s),))):
		print "Server verified!"
		print "RTT =", "%0.18f" % rtt
	else:
		print "Server NOT verified!"
		print "RTT =", 0
			
	return

main()
        
            
            
