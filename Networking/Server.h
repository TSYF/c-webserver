#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct Server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;
    
    void (*launch)(struct Server *server);
} Server;

Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void (*launch)(Server *server));

#endif /* SERVER_H */