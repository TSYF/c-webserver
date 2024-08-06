#include "Server.h"
#include <stdio.h>
#include <unistd.h>

/** This one's pretty much responsible for reading the TCP input.
* @param server Server Pointer to the server struct that owns this method.
* @param buffer_size Size of allowed input
* @param buffer String pointer into which to write the input.
*/
static int _read(Server* server, const unsigned int buffer_size, char buffer[]);

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

    server._read = _read;
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

static int _read(
    Server* server,
    const unsigned int buffer_size,
    char* buffer
) {
    // accept();
    unsigned int address_length = sizeof(server->address); //? server. or server->?
    int new_socket = accept(
        server->socket,
        (struct sockaddr*)&server->address,
        (socklen_t*)&address_length
    );
    read(new_socket, buffer, buffer_size);
    return new_socket;
}