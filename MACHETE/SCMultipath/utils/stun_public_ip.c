/*
 * stun_public_ip.c
 *
 *  Created on: Apr 12, 2016
 *      Author: feline4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "config_ips.h"
#include "stun_public_ip.h"
#include "debug.h"

#define MAXLINE 200

int stunGetPublicIP(char *return_ip_port, char *localaddr2)
{
	struct sockaddr_in servaddr;
	struct sockaddr_in localaddr;
	unsigned char buf[MAXLINE];
	int sockfd, i;
	unsigned char bindingReq[20];

	int stun_method,msg_length;
	short attr_type;
	short attr_length;
	short port;
	short n;

	//Default connecting server is stun.l.google.com 1
	char * stun_server_ip = STUN_SERVER_IP;
	short stun_server_port = STUN_SERVER_PORT;
	short local_port = STUN_SERVER_SOCKET_LOCAL_PORT;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, stun_server_ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(stun_server_port);
	bzero(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(local_port);
	if(localaddr2!=NULL){
		localaddr.sin_addr.s_addr = inet_addr(localaddr2);
	}
	n = bind(sockfd,(struct sockaddr *)&localaddr,sizeof(localaddr));

	* (short *)(&bindingReq[0]) = htons(0x0001);    // stun_method
	* (short *)(&bindingReq[2]) = htons(0x0000);    // msg_length
	* (int *)(&bindingReq[4])   = htonl(0x2112A442);// magic cookie

	*(int *)(&bindingReq[8]) = htonl(0x63c7117e);   // transacation ID
	*(int *)(&bindingReq[12])= htonl(0x0714278f);
	*(int *)(&bindingReq[16])= htonl(0x5ded3221);

	n = sendto(sockfd, bindingReq, sizeof(bindingReq),0,(struct sockaddr *)&servaddr, sizeof(servaddr)); // send UDP
	if (n == -1)
	{
		printf("sendto error\n");
		return -1;
	}
	sleep(1);
	n = recvfrom(sockfd, buf, MAXLINE, 0, NULL,0); // recv UDP
	if (n == -1)
	{
		printf("recvfrom error\n");
		return -2;
	}
	if (*(short *)(&buf[0]) == htons(0x0101))
	{
		n = htons(*(short *)(&buf[2]));
		i = 20;
		while(i<sizeof(buf))
		{
			attr_type = htons(*(short *)(&buf[i]));
			attr_length = htons(*(short *)(&buf[i+2]));
			if (attr_type == 0x0020)
			{
				port = ntohs(*(short *)(&buf[i+6]));
				port ^= 0x2112;
				sprintf(return_ip_port,"%d.%d.%d.%d",buf[i+8]^0x21,buf[i+9]^0x12,buf[i+10]^0xA4,buf[i+11]^0x42);
				break;
			}
			i += (4  + attr_length);
		}
	}
	close(sockfd);
	if(DEBUG) printf("Public ip successfully translated!\n");
	return 0;
}


