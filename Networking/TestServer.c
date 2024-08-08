#include "Server.h"

#define PROTOCOL 0
#define PORT 8080
#define MAX_CLIENTS 10

void handle(Server* server, const int socket, const char inData[]) {
    printf("======== HANDLING ========\n");
    printf(inData);

    server->out(socket, "HTTP/1.1 200 OK");
    server->out(socket, "\r\nContent-Type: text/html");
    server->out(socket, "\r\n<h1>Hello World</h1>");
    server->out(socket, "\r\n<p>This is a test.</p>");
    server->out(socket, "\r\n<pre>This is\na\ntest 2.</pre>");
    printf("======== HANDLED =========\n");
}

void init(Server* server) {
    printf("LISTENING...\n");
}

int main() {
    Server server = server_constructor(AF_INET, SOCK_STREAM, PROTOCOL, INADDR_ANY, PORT, MAX_CLIENTS, handle);

    server.launch(&server, init);
}