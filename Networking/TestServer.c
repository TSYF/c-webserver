#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Server.h"

void test(Server *server) {
    printf("WAITING...\n");

    int buffer_size;
    int address_length;
    int new_socket;
    char* hello;

    while (1) {

        // accept();
        buffer_size = (int)30e3;
        char buffer[buffer_size];
        address_length = sizeof(server->address); //? server. or server->?
        new_socket = accept(
            server->socket,
            (struct sockaddr*)&server->address,
            (socklen_t*)&address_length
        );
        read(new_socket, buffer, buffer_size);

        // handle();
        printf("%s\n", buffer);

        // respond();
        hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 34\n\n<body><h1>Hello World!</h1></body>";
        write(new_socket, hello, strlen(hello));
        close(new_socket);

    }
    printf("DONE\n");
}

int main() {
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8080, 10, test);

    server.launch(&server);
}