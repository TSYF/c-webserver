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
#define S_DEF_BUFFER_SIZE (int)30e3

typedef struct TcpServer TcpServer;

typedef int (*TcpReceiverFunc)(TcpServer*, const unsigned int, char[]);
typedef void (*TcpHandleFunc)(TcpServer* server, const int new_socket, const char inData[]);
typedef char* (*TcpOutputFunc)(const TcpServer* server, const int, const char[]);
typedef int (*TcpSendFunc)(const TcpServer*, const int);
typedef void (*TcpServerLaunchFunc)(TcpServer*, void (*init)(struct TcpServer*));

struct TcpServer {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;

    TcpServerLaunchFunc launch;

    TcpReceiverFunc _in;
    TcpHandleFunc _handle;
    TcpOutputFunc out;
    TcpSendFunc send;

    char* outData;
};

TcpServer base_server_constructor(const TcpHandleFunc handle);
TcpServer server_constructor(
    const int domain,
    const int service,
    const int protocol,
    const u_long interface,
    const int port,
    const int backlog,
    const int buffer_size,
    const TcpHandleFunc handle
);

#endif /* SERVER_H */