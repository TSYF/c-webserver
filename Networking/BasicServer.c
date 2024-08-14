#include "BasicServer.h"
#include <stdio.h>
#include <unistd.h>

/** This one's pretty much responsible for reading the TCP input.
* @param server BasicServer Pointer to the server struct that owns this method.
* @param buffer_size Size of allowed input
* @param buffer String pointer into which to write the input.
*/
static int _in(BasicServer* server, const unsigned int buffer_size, char buffer[]);
static char* _out(BasicServer* server, const int new_socket, const char response[]);
static int _send(BasicServer* server, const int new_socket);
static void _launch(BasicServer* server, void (*init)(BasicServer*));

BasicServer base_server_constructor(void (*handle)(BasicServer* server, const int socket, const char inData[])) {
    BasicServer server;

    server.domain = AF_INET;
    server.service = SOCK_STREAM;
    server.protocol = S_DEF_PROTOCOL;
    server.interface = INADDR_ANY;
    server.port = S_DEF_PORT;
    server.backlog = S_DEF_MAX_CLIENTS;

    server.address.sin_family = server.domain;
    server.address.sin_port = htons(server.port);
    server.address.sin_addr.s_addr = htonl(server.interface); 

    server.socket = socket(server.domain, server.service, server.protocol);

    server._in = _in;
    server._handle = handle;
    server.out = _out;
    server.send = _send;
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
    server.outData = malloc(S_DEF_BUFFER_SIZE * sizeof(char));

    return server;
}

BasicServer server_constructor(
    int domain,
    int service,
    int protocol,
    u_long interface,
    int port,
    int backlog,
    int buffer_size,
    void (*handle)(BasicServer* server, const int socket, const char inData[])
) {
    BasicServer server;

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
    server.send = _send;
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
    server.outData = malloc(buffer_size * sizeof(char));

    return server;
}

static int _in(
    BasicServer* server,
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

static char* _out(BasicServer* server, const int new_socket, const char response[]) {
    return strcat(server->outData, response);
}

static int _send(BasicServer* server, const int new_socket) {
    printf("SENDING\n");
    printf(server->outData);
    printf("\n");
    int out = write(new_socket, server->outData, strlen(server->outData));
    printf("SENT\n");
    return out;
}

static void _launch(BasicServer* server, void (*init)(BasicServer*)) {
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
        server->_handle(server, new_socket, buffer);

        // respond();
        server->send(server, new_socket);
        close(new_socket);
    }
}