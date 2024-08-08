#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct Server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;
    
    void (*launch)(struct Server*, void (*init)(struct Server*));
    
    int (*_in)(struct Server*, const unsigned int, char[]);
    void (*_handle)(struct Server* server, const int new_socket, const char inData[]); 
    int (*out)(const int, const char[]);
} Server;

Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void (*handle)(struct Server* server, const int new_socket, const char inData[]));

#endif /* SERVER_H */