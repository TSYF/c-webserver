#include "TcpServer.h"

void handle(TcpServer* server, const int socket, const char inData[]) {
    printf("======== HANDLING ========\n");
    printf(inData);

    // char out[100];
    // sprintf(out, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello World</h1>\r\n<p>This is a test.</p>\r\n<pre>This is\na\ntest 2.</pre>");

    // server->out(server, socket, out);
    
    server->out(server, socket, "HTTP/1.1 200 OK");
    server->out(server, socket, "\r\nContent-Type: text/html");
    server->out(server, socket, "\r\n\r\n<h1>Hello World</h1>");
    server->out(server, socket, "\r\n<p>This is a test.</p>");
    server->out(server, socket, "\r\n<pre>This is\na\ntest 2.</pre>");
    printf("======== HANDLED =========\n");
}

void init(TcpServer* server) {
    printf("LISTENING...\n");
}

int main() {
    TcpServer server = server_constructor(AF_INET, SOCK_STREAM, S_DEF_PROTOCOL, INADDR_ANY, S_DEF_PORT, S_DEF_MAX_CLIENTS, S_DEF_BUFFER_SIZE, handle);

    server.launch(&server, init);
}