#include "Server.h"
#include <stdio.h>
#include <unistd.h>

/** This one's pretty much responsible for reading the TCP input.
* @param server Server Pointer to the server struct that owns this method.
* @param buffer_size Size of allowed input
* @param buffer String pointer into which to write the input.
*/
static int _in(Server* server, const unsigned int buffer_size, char buffer[]);
static int _out(int new_socket, const char response[]);
static void _launch(Server* server, void (*init)(Server*));

Server server_constructor(
    int domain,
    int service,
    int protocol,
    u_long interface,
    int port,
    int backlog,
    void (*handle)(Server*, char inData[])
) {
    Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface); 

    server.socket = socket(domain, service, protocol);

    server._in = _in;
    server._handle = handle;
    server.out = _out;
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

    server.launch = _launch;

    return server;
}

static int _in(
    Server* server,
    const unsigned int buffer_size,
    char buffer[buffer_size]
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

static int _out(int new_socket, const char response[]) {
    int out = write(new_socket, response, strlen(response));
    return out;
}

static void _launch(Server* server, void (*init)(Server*)) {
    unsigned int buffer_size = (int)30e3;
    char buffer[buffer_size];
    char* hello;
    int new_socket;
    
    if (init != NULL) {
        init(server);
    }

    while (1) {

        new_socket = server->_in(
            server,
            buffer_size,
            buffer
        );

        // handle();
        server->_handle(server, buffer);

        // respond();
        hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 34\n\n<body><h1>Hello World!</h1></body>";
        
        server->out(new_socket, hello);
        close(new_socket);
    }
}