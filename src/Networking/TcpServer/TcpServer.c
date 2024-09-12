#include "TcpServer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

/** This one's pretty much responsible for reading the TCP input.
* @param server BasicServer Pointer to the server struct that owns this method.
* @param buffer_size Size of allowed input
* @param buffer String pointer into which to write the input.
*/
static int _in(TcpServer* server, size_t buffer_size, char buffer[buffer_size]);
static char* _out(TcpServer* server, const char response[]);
static int _send(const TcpServer* server, const int new_socket);
static void _launch(TcpServer* server, void (*init)(TcpServer*), uint8_t argc, ...);
// static void getMtu(TcpServer* server);

TcpServer base_server_constructor(const TcpHandleFunc handle)
{
    TcpServer server;

    server.domain = AF_INET;
    server.service = SOCK_STREAM;
    server.protocol = TCP_S_DEF_PROTOCOL;
    server.interface = INADDR_ANY;
    server.port = TCP_S_DEF_PORT;
    server.backlog = TCP_S_DEF_MAX_CLIENTS;

    server.address.sin_family = server.domain;
    server.address.sin_port = htons(server.port);
    server.address.sin_addr.s_addr = htonl(server.interface); 

    server.socket = socket(server.domain, server.service, server.protocol);

    server._in = _in;
    server._handle = handle;
    server.out = _out;
    server.send = _send;
    server.outDataSize = TCP_S_MTU;
    // test_connection(server.socket);
    
    if (server.socket == -1)
    {
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
    
    if (listen(server.socket, server.backlog) < 0)
    {
        perror("Failed to listen on socket...");
        exit(1);
    }

    server.launch = _launch;

    return server;
}

TcpServer server_constructor(
    const int domain,
    const int service,
    const int protocol,
    const u_long interface,
    const int port,
    const int backlog,
    const TcpHandleFunc handle
)
{
    TcpServer server;

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
    server.outDataSize = TCP_S_MTU;
    // test_connection(server.socket);
    
    if (server.socket == -1)
    {
        perror("Failed to connect to socket...");
        exit(1);
    }


    int reuse = 0;
    setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
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
    
    if (listen(server.socket, server.backlog) < 0)
    {
        perror("Failed to listen on socket...");
        exit(1);
    }

    server.launch = _launch;

    return server;
}

static int _in(
    TcpServer* server,
    size_t buffer_size,
    char buffer[buffer_size]
)
{
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

static char* _out(TcpServer* server, const char response[])
{
    uint64_t responseLength = strlen(response);
    uint64_t outDataLength = strlen(server->outData);

    while (responseLength + outDataLength > server->outDataSize)
    {
        server->outDataSize += TCP_S_MTU;
        // if (realloc(server->outData, server->outDataSize)) {
        //     printf("REALLOC FAILED\n");
        //     exit(1);
        // }
    }
    return strcat(server->outData, response);
}

static int _send(const TcpServer* server, const int new_socket)
{
    printf("SENDING\n");
    printf(server->outData);
    printf("\n");
    int out = write(new_socket, server->outData, strlen(server->outData));
    printf("SENT\n");
    return out;
}

static void _launch(TcpServer* server, void (*init)(TcpServer*), uint8_t argc, ...)
{
    char buffer[TCP_S_DEF_BUFFER_SIZE];
    int new_socket;
    
    if (init != NULL)
    {
        init(server);
    }

    while (1)
    {

        new_socket = server->_in(
            server,
            TCP_S_DEF_BUFFER_SIZE,
            buffer
        );

        // handle();

        if (argc < 1)
        {
            server->_handle(server, buffer, 0);
        } else {
            va_list args;
            va_start(args, argc);
            server->_handle(server, buffer, argc, args);
            va_end(args);
        }

        // respond();
        server->send(server, new_socket);
        close(new_socket);
        memset(server->outData, 0, sizeof(server->outData));
    }
    free(server->outData);
    free(buffer);
}

/* static uint64_t getMtu(char *name)
{
    FILE *f;
    char buf[128];
    char *line = NULL;
    ssize_t count;
    size_t len = 0;
    uint64_t mtu;

    snprintf(buf, sizeof(buf), "/sys/class/net/%s/mtu", name);
    f = fopen(buf, "r");
    if(!f)
    {
        perror("Error opening:");
        exit(EXIT_FAILURE);
    }
    count = getline(&line, &len, f);

    if (count == -1)
    {
        perror("Error opening:");
        exit(EXIT_FAILURE);
    }
    sscanf(line, "%d\n", &mtu);
    fclose(f);

    return mtu;
} */