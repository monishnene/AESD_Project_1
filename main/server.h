/******************************************
* server.h
* Remote socket task
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

#ifndef SERVER_H
#define SERVER_H


/*******************************************
* Includes
*******************************************/

#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>

/*******************************************
* Macro
*******************************************/

#define PORT_ADDRESS 10001

void remote_server(void);
#endif
