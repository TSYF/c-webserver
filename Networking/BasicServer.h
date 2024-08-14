#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define S_DEF_PROTOCOL 0
#define S_DEF_PORT 8080
#define S_DEF_MAX_CLIENTS 10
#define S_DEF_BUFFER_SIZE 30e3

typedef struct BasicServer {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;
    
    void (*launch)(struct BasicServer*, void (*init)(struct BasicServer*));
    
    int (*_in)(struct BasicServer*, const unsigned int, char[]);
    void (*_handle)(struct BasicServer* server, const int new_socket, const char inData[]); 
    char* (*out)(struct BasicServer*, const int, const char[]);
    int (*send)(struct BasicServer*, const int);

    char* outData;
} BasicServer;

BasicServer base_server_constructor(void (*handle)(struct BasicServer* server, const int new_socket, const char inData[]));
BasicServer server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, int buffer_size, void (*handle)(struct BasicServer* server, const int new_socket, const char inData[]));

#endif /* SERVER_H */