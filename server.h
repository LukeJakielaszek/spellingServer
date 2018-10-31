#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

int getlistenfd(char *port);

#endif /** SERVER_H */
