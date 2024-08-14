//
// Created by tomas on 14-08-24.
//

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "../TcpServer/TcpServer.h"

typedef struct HttpServer {
    TcpServer* tcpServer;
} HttpServer;

#endif //HTTPSERVER_H
