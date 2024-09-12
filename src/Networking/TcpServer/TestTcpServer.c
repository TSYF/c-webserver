#include "TcpServer.h"
#include <stdio.h>

void handle(TcpServer* server, const char inData[], uint8_t _, ...) {
    printf("======== HANDLING ========\n");
    printf(inData);

    // char out[100];
    // sprintf(out, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello World</h1>\r\n<p>This is a test.</p>\r\n<pre>This is\na\ntest 2.</pre>");

    // server->out(server, socket, out);
    
    server->out(server, "HTTP/1.1 200 OK");
    server->out(server, "\r\nContent-Type: text/html");
    server->out(server, "\r\n\r\n<h1>Hello World</h1>");
    server->out(server, "\r\n<p>This is a test.</p>");
    server->out(server, "\r\n<pre>This is\na\ntest 2.</pre>");
    printf("======== HANDLED =========\n");
}

void init(TcpServer* server) {
    printf("LISTENING...\n");
}

int main() {
    TcpServer server = server_constructor(
        AF_INET,
        SOCK_STREAM,
        TCP_S_DEF_PROTOCOL,
        INADDR_ANY,
        TCP_S_DEF_PORT,
        TCP_S_DEF_MAX_CLIENTS,
        handle
    );

    server.launch(&server, init, 0, NULL);
}