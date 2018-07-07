/*
 * receiving_handler.h
 *
 *  Created on: Apr 13, 2016
 *      Author: feline4
 */

#ifndef RECEIVING_HANDLER_H_
#define RECEIVING_HANDLER_H_

void getPrimary();

void getLocalInterfaces2();

void *receiveData(void *args);

void *recv_handler(void *port_ptr);




#endif /* RECEIVING_HANDLER_H_ */
