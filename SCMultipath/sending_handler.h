/*
 * sending_handler.h
 *
 *  Created on: Apr 13, 2016
 *      Author: feline4
 */

#ifndef SENDING_HANDLER_H_
#define SENDING_HANDLER_H_

void getLocalInterfaces();

void sendRuleToHop(char *src, char *dst, char* hop, int port, int type);

void createVirtualInterface(int addition);

void cleanVirtualInterfaces();

int setSendingRules(int port);

void cleanRules(int port);

int sendFile(char* address, int port, char* filename);

int send_handler(char* address, int port, char* filename);

#endif /* SENDING_HANDLER_H_ */
