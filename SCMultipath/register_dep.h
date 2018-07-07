/*
 * RegisterDep.h
 *
 *  Created on: Apr 8, 2016
 *      Author: feline4
 */

#ifndef REGISTER_DEP_H_
#define REGISTER_DEP_H_

int registerDep(char *server, int serverport, int num_interfaces, char **interfaces, int type);

int findHops(char *server, int serverport, char *hops[20]);

int findDsts(char *server, int serverport, char *dsts[20], char *prim_dest);

#endif /* REGISTER_DEP_H_ */
