/*
 * SendingHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "stun_public_ip.h"
#include "debug.h"
#include "sending_handler.h"

#define LENGTH 128
#define PACKETSIZE 64

#define ROW 20

//Global variables for now
char **hops;
char **srcs;
char **dsts;
char *primary_interface;
int pid=-1;
struct protoent *proto=NULL;
char *buffer;

int numSrcs;
int numHops;
int numDsts;

struct timeval begin, end;
long int end_ms;
long int begin_ms;
long int result_ms;
FILE *log;

void setup(){
	int i;
	hops = malloc(ROW * sizeof(char*));
	srcs = malloc(ROW * sizeof(char*)); 
	dsts = malloc(ROW * sizeof(char*));
	for(i = 0;i<ROW;i++){
		hops[i] = malloc(sizeof(char)*20);
		srcs[i] = malloc(sizeof(char)*20);
		dsts[i] = malloc(sizeof(char)*20);
	}
	primary_interface = malloc(sizeof(char)*10);
}

struct packet
{
	struct icmphdr hdr;
	char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

unsigned short checksum(void *b, int len){
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int ping(char *addr_str){
	if(DEBUG) printf("Inside ping: going to ping %s\n", addr_str);
	const int val=255;
	int i, j, sd, cnt=1;
	struct packet pckt;
	struct sockaddr_in r_addr;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = inet_addr(addr_str);
	memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

	proto = getprotobyname("ICMP");

	sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
	if ( sd < 0 )
	{
		if(DEBUG) printf("Socket failed to create\n");
		perror("socket");
		return -1;
	} else {
		if(DEBUG) printf("Socket created, continuing\n");
	}
	if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
		perror("Set TTL option");
	if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
		perror("Request nonblocking I/O");
	for (j = 0 ; j < 10 ; j++ )
	{
	usleep(250000);
	int len=sizeof(r_addr);

	if(DEBUG) printf("Msg #%d\n", cnt);
	if ( recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0 ){
		if(DEBUG) printf("***Got message!***\n");
		close(sd);
		return 1;
	}
	bzero(&pckt, sizeof(pckt));
	pckt.hdr.type = ICMP_ECHO;
	pckt.hdr.un.echo.id = pid;
	for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
		pckt.msg[i] = i+'0';
	pckt.msg[i] = 0;
	pckt.hdr.un.echo.sequence = cnt++;
	pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
	if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0 )
		perror("sendto");
	}
	if(DEBUG) printf("Returning with no message\n");
	return -1;
}

int probeNodes(char **nodes){

	if(nodes[0]==NULL){
		return 0;
	}

	if(DEBUG) printf("Malloc new nodes\n");
	int i = 0, j = 0;
	while(i<numHops){
		if(DEBUG) printf("Pinging %s\n", nodes[i]);
		if(ping(nodes[i])==1){
			i++;
			if(DEBUG) printf("Adding %d\n", i);
		} else {
			nodes[i]=nodes[i+1];
			if(DEBUG) printf("Passing in front of %d\n", i);
		}
	}
	return i;
}

void getLocalInterfaces(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];
	char addr[20];

	if(DEBUG) printf("Sending Handler: getting local interfaces\n");
	int i=0;
	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name))) && (ifa->ifa_addr->sa_family==AF_INET)) {
			strcpy(primary_interface, ifa->ifa_name);
			sa = (struct sockaddr_in *) ifa->ifa_addr;
		//	srcs[i] = (char*)malloc(sizeof(char)*20);
			strcpy(srcs[i], inet_ntoa(sa->sin_addr));
			i++;
		}
	}
	numSrcs = i;
	if(DEBUG) printf("Sending Handler: local interfaces check :) \n");
	if(DEBUG){
		printf("Sending Handler: Num srcs: %d\n", numSrcs);
		for(i=0; i<numSrcs; i++){
			printf("%s\n", srcs[i]);
		}
		printf("\n");

		printf("Sending Handler: Num dsts: %d\n", numDsts);
		for(i=0; i<numDsts; i++){
			printf("%s\n", dsts[i]);
		}
		printf("\n");
	}
	freeifaddrs(ifap);
	printf("last part %d\n", numHops);
    if(DEBUG) printf("Sending Handler: ----------------------\n");
}

void sendRuleToHop(char *src, char *dst, char* hop, int port, int type){

	stunGetPublicIP(src, src);

	if(DEBUG)printf("Going to send rule to hop %s on port %d\n", hop, port);

	int fwdSocket;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	char rule[100];
	if(type==1){
		strcpy(rule, "A-");
	} else {
		strcpy(rule, "D-");
	}
	strcat(rule,src);
	strcat(rule,"-");
	strcat(rule,dst);

	if(DEBUG)printf("Preparing to send rule:%s\n", rule);


	fwdSocket = socket(AF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(hop);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(fwdSocket, (struct sockaddr *) &serverAddr, addr_size);

	if(DEBUG)printf("Result of connection: %d\n", result);

	if(DEBUG)printf("Connecting to socket\n");

	send(fwdSocket, rule, LENGTH, 0);
	bzero(rule, LENGTH);	
	while(recv(fwdSocket,rule,LENGTH,0)){
		printf("Strlen of buffer:%d\n", (int)strlen(rule));
		if(strlen(rule)>0) break;
	}

	close(fwdSocket);
}

void createVirtualIfconfig(int addition){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char pt1[4], pt2[4], pt3[4], pt4[4], *tmpc;
	char addr[20];
	const char s[2] = ".";

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if ((strcmp(primary_interface, ifa->ifa_name) == 0) && (ifa->ifa_addr->sa_family==AF_INET)) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			strcpy(addr, inet_ntoa(sa->sin_addr));
			break;
		}
	}
	freeifaddrs(ifap);
	if(DEBUG) printf("addr %s\n", addr);

	//divide address
	tmpc = strtok(addr, s);
	strcpy(pt1, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt2, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt3, tmpc);
	tmpc = strtok(NULL, s);
	strcpy(pt4, tmpc);


	//to int and back to char
	int tmp;

	while(1){
		tmp=atoi(pt4);
		tmp = tmp + addition;
		sprintf(pt4, "%d", tmp);
		if(DEBUG) printf("new pt4 %s\n", pt4);


		//rebuild ip
		strcpy(addr, pt1);
		strcat(addr, ".");
		strcat(addr, pt2);
		strcat(addr, ".");
		strcat(addr, pt3);
		strcat(addr, ".");
		strcat(addr, pt4);

		if(ping(addr)==-1){
			if(DEBUG) printf("Sending handler: ping failed, address is free\n");
			break;
		}

	}

	if(DEBUG) printf("new addr %s\n", addr);

	//create command
	char command[100];
	strcpy(command, "ifconfig ");
	strcat(command, primary_interface);
	strcat(command, ":");
	strcat(command, pt4);
	strcat(command, " ");
	strcat(command, addr);
	if(DEBUG) printf("%s\n", command);
	system(command);
}

void createVirtualInterface(int addition){
	char interface[4] = "mpc";
	char *number = malloc(sizeof(char));
	sprintf(number, "%d", time(NULL));
	strcat(interface, number);

	char command[100];
	strcpy(command, "ip link add name ");
	strcat(command, interface);
	strcat(command, " link ");
	strcat(command, primary_interface);
	strcat(command, " type macvlan");
	if(DEBUG) printf("Sending Handler: creating new interface %s\n", command);
	system(command);

	strcpy(command, "ip link set dev ");
	strcat(command, interface);
	strcat(command, " up");
	if(DEBUG) printf("Sending Handler: set interface up %s\n", command);
	system(command);

	strcpy(command, "dhclient ");
	strcat(command, interface);
	if(DEBUG) printf("Sending Handler: requesting dhcp %s\n", command);
	system(command);

}


void cleanVirtualInterfaces(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char interface[20];
	const char s = 'm';
	char command[100];

	if(DEBUG)printf("Cleaning interfaces\n");
	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if(DEBUG)printf("checking: %s\n", ifa->ifa_name);
		if (((strcmp("lo", ifa->ifa_name) != 0) && (strcmp("lo0", ifa->ifa_name) != 0)) && (ifa->ifa_addr->sa_family==AF_INET)) {
			strcpy(interface, ifa->ifa_name);
			if(strchr(interface, s)!=NULL){
				if(DEBUG) printf("Deleting %s\n", interface);
				strcpy(command, "ifconfig ");
				strcat(command, interface);
				strcat(command, " down");
				if(DEBUG) printf("Command1 %s\n", command);
				system(command);

				strcpy(command, "ip link delete dev ");
				strcat(command, interface);
				if(DEBUG) printf("Command2 %s\n", command);
				system(command);
			}
		}
	}
	freeifaddrs(ifap);
}

int setSendingRules(int port){
	char command[500];
	const char s='e';
	getLocalInterfaces();

	int totalFlows = numSrcs*numDsts;

	if(DEBUG) printf("Sending Handler: numsrcs:%d numdsts:%d Total flows:%d numhops:%d Primary interface:%s\n", numSrcs, numDsts, totalFlows, numHops, primary_interface);
	if(totalFlows < numHops){
		while(numSrcs*numDsts<numHops){
			if(strchr(primary_interface, s)!=NULL){
				printf("Before createvifcfg %d\n", numHops);
				createVirtualIfconfig(numSrcs);
				if(DEBUG)printf("Done creating new ifconfig iface\n");
			} else {
				createVirtualInterface(numSrcs);
			}
			if(DEBUG)printf("Adding numSrcs\n");
			numSrcs++;
			if(DEBUG)printf("Number of sources is now %d and hops %d\n", numSrcs, numHops);
		}
	}
	printf("before get local interfaces%d\n", numHops);
	getLocalInterfaces();

	printf("before i declaration%d\n", numHops);
	int i, j;
	
	printf("numsrcs %d   numdsts %d   numhops%d\n", numSrcs, numDsts, numHops);
	for(i=0; i<numSrcs; i++){
		for(j=0; j<numDsts;j++){
			        gettimeofday(&begin, NULL);

			strcpy(command, "iptables -t nat -A PREROUTING -p tcp -s ");
			strcat(command, srcs[i]);
			strcat(command, " -d ");
			//if(i==0 && j==0)
				strcat(command, dsts[j]);
			//else
			//	strcat(command, hops[0]);
			strcat(command, " -j DNAT --to-destination ");
			if(DEBUG)printf("Set first rule\n");
			if(DEBUG)printf("Sending Handler: Forwarding to hop:%d\n", ((i*numDsts+j) % numHops));
			strcat(command, hops[((i*numDsts+j) % numHops)]);
			if(DEBUG)printf("Sending Handler: Command1 %s\n", command);
			system(command);

			strcpy(command, "iptables -t nat -A OUTPUT -p tcp -s ");
			strcat(command, srcs[i]);
			strcat(command, " -d ");
			//if(i==0 && j==0)
                                strcat(command, dsts[j]);
                        //else
                        //        strcat(command, hops[0]);
			strcat(command, " -j DNAT --to-destination ");
			strcat(command, hops[((i*numDsts+j) % numHops)]);
			if(DEBUG)printf("Sending Handler: Command2 %s\n", command);

			system(command);

			gettimeofday(&end, NULL);
        		end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
        		begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
        		result_ms = end_ms - begin_ms;
			fprintf(log,"->Setup a rule time %ld\n", result_ms);
        		printf("::::::::Setup a rule time %ld\n", result_ms);
			        gettimeofday(&begin, NULL);

			printf("Sending Handler: Sending rule to hop %s\n", hops[((i*numDsts+j) % numHops)]);
			sendRuleToHop(srcs[i], dsts[j], hops[((i*numDsts+j) % numHops)], port, 1);

			gettimeofday(&end, NULL);
        		end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
        		begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
        		result_ms = end_ms - begin_ms;
        		printf("::::::::Setup a node time %ld\n", result_ms);
		        fprintf(log,"->Setup a node time %ld\n", result_ms);

		}
	}
	if(DEBUG) printf("Sending Handler: done setting rules\n");
}

void cleanRules(int port){
	gettimeofday(&begin, NULL);
	char command[500];
	int i,j;
	for(i=0; i<numSrcs; i++){
		for(j=0; j<numDsts;j++){
			//send rules to hops
			printf("Sending rule to hop %s\n", hops[((i*numDsts+j) % numHops)]);
			sendRuleToHop(srcs[i], dsts[j], hops[((i*numDsts+j) % numHops)], port, 0);
		}
	}

	gettimeofday(&end, NULL);
        end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
        begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
        result_ms = end_ms - begin_ms;
	fprintf(log,"->Tear down nodes time %ld\n", result_ms);
        printf("::::::::Tead down nodes time %ld\n", result_ms);
        gettimeofday(&begin, NULL);	
	for(i=0; i<numSrcs; i++){
                for(j=0; j<numDsts;j++){
                        strcpy(command, "iptables -t nat -D PREROUTING -p tcp -s ");
                        strcat(command, srcs[i]);
                        strcat(command, " -d ");
                        strcat(command, dsts[j]);
                        strcat(command, " -j DNAT --to-destination ");
                        if(DEBUG)printf("Sending Handler: Forwarding to hop:%d\n", ((i*numDsts+j) % numHops));
                        strcat(command, hops[((i*numDsts+j) % numHops)]);
                        if(DEBUG)printf("Sending Handler: Command1 %s\n", command);
                        system(command);

                        strcpy(command, "iptables -t nat -D OUTPUT -p tcp -s ");
                        strcat(command, srcs[i]);
                        strcat(command, " -d ");
                        strcat(command, dsts[j]);
                        strcat(command, " -j DNAT --to-destination ");
                        strcat(command, hops[((i*numDsts+j) % numHops)]);
                        if(DEBUG)printf("Sending Handler: Command2 %s\n", command);
	system(command);
                }
        }
        if(DEBUG) printf("Sending Handler: done cleaning rules\n");

	gettimeofday(&end, NULL);
        end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
        begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
        result_ms = end_ms - begin_ms;
        printf("::::::::Tear down device time %ld\n", result_ms);
	fprintf(log,"->Tear down device time %ld\n", result_ms);
	//clean local
//	system("iptables -t nat -F");

}

//Arg0: address, arg1: port, arg2: filename
int sendFile(char* address, int port, char* filename) {
	int clientSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	FILE *fp;
	fp = fopen(filename, "r");

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(address);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	int result = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

	if(DEBUG)printf("Connecting to receiving handler: %d\n", result);
	send(clientSocket, filename, strlen(filename)+1, 0);
	int block = 0;
	while ((block = fread(buffer, sizeof(char), LENGTH, fp)) > 0) {
		send(clientSocket, buffer, block, 0);
		bzero(buffer, LENGTH);
	}

	close(clientSocket);
	fclose(fp);
	return 0;
}

int send_handler(char* address, int port, char* filename){
	setup();
	log = fopen("log.txt", "a");
	fprintf(log, "File name:%s\n", filename);
	struct timeval begin, end;
    	gettimeofday(&begin, NULL);
	numHops = findHops("127.0.0.2", 7892, hops);
	if(DEBUG) printf("MAAAAAIN:Found hops\n");
	numHops = probeNodes(hops);
	fprintf(log, "Number of hops: %d\n", numHops);
	if(DEBUG) printf("MAAAAAIN:Probed hops %d\n", numHops);
	numDsts = findDsts("127.0.0.2", 7892, dsts, address);
	if(DEBUG) printf("MAAAAAIN:Found dsts\n");
	setSendingRules(9899);
	gettimeofday(&end, NULL);
    	end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
    	begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
	result_ms = end_ms - begin_ms;
	printf("::::::::Setup nodes time %ld\n", result_ms);
	if(DEBUG) printf("MAAAAAIN:Set sending rules\n");
	gettimeofday(&begin, NULL);
	sendFile(address, port, filename);
	gettimeofday(&end, NULL);
    	end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
    	begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
    	result_ms = end_ms - begin_ms;
	fprintf(log,"->Send time %ld\n", result_ms);
    	printf("::::::::Send time %ld\n", result_ms);
	gettimeofday(&begin, NULL);
	cleanRules(9899);
	cleanVirtualInterfaces();
	gettimeofday(&end, NULL);
    	end_ms = end.tv_sec * 1000 + end.tv_usec/1000;
    	begin_ms = begin.tv_sec * 1000 + begin.tv_usec/1000;
    	result_ms = end_ms - begin_ms;
    	printf("::::::::Tear down nodes time %ld\n", result_ms);
	fprintf(log, "<------------------------------\n");
	fclose(log);	
	return 1;
}
