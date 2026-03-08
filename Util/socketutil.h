#ifndef UTIL_SOCKETUTIL_H
#define UTIL_SOCKETUTIL_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

int createTCPIpv4Socket ();

struct sockaddr_in* createIPv4Address (char *ip, int port);

#endif