#ifndef SERVER_H
#define SERVER_H
#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT_ADDRESS 10001

void remote_server(void);
#endif
