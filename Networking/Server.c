#include "Server.h"
#include <stdio.h>

Server server_constructor(
    int domain,
    int service,
    int protocol,
    u_long interface,
    int port,
    int backlog,
    void (*launch)(Server *server)
) {
    Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;
    server.launch = launch;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface); 

    server.socket = socket(domain, service, protocol);
    // test_connection(server.socket);
    
    if (server.socket == -1) {
        perror("Failed to connect to socket...");
        exit(1);
    }


    if (
        bind(
            server.socket,
            (struct sockaddr*)&server.address,
            (socklen_t)sizeof(server.address)
        ) < 0
    ) {
        perror("Failed to bind socket...");
        exit(1);
    }
    
    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to listen on socket...");
        exit(1);
    }

    server.launch = launch;

    return server;
}