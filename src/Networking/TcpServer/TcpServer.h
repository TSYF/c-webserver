#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <netinet/in.h>

#define TCP_S_DEF_PROTOCOL 0
#define TCP_S_DEF_PORT 8080
#define TCP_S_DEF_MAX_CLIENTS 10
#define TCP_S_DEF_BUFFER_SIZE (int)3e4
#define TCP_S_MTU 65536

typedef struct TcpServer TcpServer;

typedef int (*TcpReceiverFunc)(TcpServer* server, size_t buffer_size, char buffer[]);
typedef void (*TcpHandleFunc)(TcpServer* server, const char inData[], uint8_t argc, va_list args);
typedef char* (*TcpOutputFunc)(TcpServer* server, const char[]);
typedef int (*TcpSendFunc)(const TcpServer* server, const int new_socket);
typedef void (*TcpServerLaunchFunc)(TcpServer*, void (*init)(TcpServer*), uint8_t argc, ...);


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

    uint32_t outDataSize;
    char outData[TCP_S_MTU];
};

TcpServer base_server_constructor(const TcpHandleFunc handle);
TcpServer server_constructor(
    const int domain,
    const int service,
    const int protocol,
    const u_long interface,
    const int port,
    const int backlog,
    const TcpHandleFunc handle
);

#endif /* SERVER_H */