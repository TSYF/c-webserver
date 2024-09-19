//
// Created by tomas on 14-08-24.
//

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "../TcpServer/TcpServer.h"
#include "../../Utilities/HashTable/HashTable.h"


#define HTTP_SERVER_DEFAULT_PORT 80

#define HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE 8192
#define HTTP_SERVER_DEFAULT_METHOD_SIZE 8
#define HTTP_SERVER_DEFAULT_PATH_SIZE 8178
#define HTTP_SERVER_DEFAULT_VERSION_SIZE 9
#define HTTP_SERVER_DEFAULT_HEADERS_SIZE 8192
#define HTTP_SERVER_DEFAULT_BODY_SIZE (uint32_t)2147483647
#define HTTP_SERVER_DEFAULT_REQUEST_SIZE (uint32_t)HTTP_SERVER_DEFAULT_REQUEST_LINE_SIZE + (uint32_t)HTTP_SERVER_DEFAULT_HEADERS_SIZE + HTTP_SERVER_DEFAULT_BODY_SIZE

#define HTTP_SERVER_DEFAULT_RESPONSE_STATUS_SIZE 32
#define HTTP_SERVER_DEFAULT_RESPONSE_BODY_SIZE 2147483647
#define HTTP_SERVER_DEFAULT_RESPONSE_CONTENT_TYPE_SIZE 64
#define HTTP_SERVER_DEFAULT_RESPONSE_LOCATION_SIZE HTTP_SERVER_DEFAULT_PATH_SIZE

extern HashTable* http_statuses;

typedef struct HttpRequest HttpRequest;
typedef struct HttpResponse HttpResponse;
typedef struct HttpServer HttpServer;

typedef void (*HttpHandleFunc)(HttpServer* httpServer, HttpRequest* request, HttpResponse* response); 
typedef void (*HttpServerLaunchFunc)(HttpServer*, void (*init)(HttpServer*));

/**
 * Works with TcpServer to provide a better interface for HTTP requests and responses.
 */
struct HttpServer {
    TcpServer* tcpServer;

    HttpHandleFunc onRequest;
    HttpServerLaunchFunc launch;
};

// HttpServer* httpserver_constructor(const int domain, const int service, const int protocol, const u_long interface, const int port, const int backlog, const int buffer_size, const HttpHandleFunc handle);
HttpServer* base_httpserver_constructor(const HttpHandleFunc handle); 

struct HttpResponse {
    uint16_t code;
    char* status;
    char* version;
    char* contentType;
    char* location;
    char* body;

    TcpServer* tcpServer;

    struct HttpResponse* (*stat)(struct HttpResponse* response, const uint16_t status);
    void (*send)(struct HttpResponse* response, const char* body);
};


struct HttpRequest {
    char* _requestLine;
    char* method;
    char* path;
    char* version;
    char* _headers;
    HashTable* headers;
    char* body;
};

#endif //HTTPSERVER_H
