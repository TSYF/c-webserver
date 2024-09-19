#include "HttpServer.h"
#include <stdio.h>
#include <stdlib.h>

void init(TcpServer* server) {
    printf("LISTENING...\n");
}

void handle(HttpServer* httpServer, HttpRequest* request, HttpResponse* res) {
    if (request == NULL) {
        printf("NULL REQUEST :D\n");
        return;
    }
    char* ret = malloc(100 * sizeof(char));
    printf("REQUEST: %s\n", request->_requestLine);
    sprintf(ret, "<h1>Hello World From C!</h1><h2>Path: %s</h2>", request->_requestLine);
    // printf("%s\n", ret);
    res->stat(res, 200)->send(res, ret);
}

int main(void) { 
    HttpServer* sv = base_httpserver_constructor(handle);

    sv->tcpServer->launch(sv->tcpServer, init, 1, sv);
}