#include "Server.h"

void test(Server *server) {
    printf("WAITING...\n");

    unsigned int buffer_size = (int)30e3;
    char buffer[buffer_size];
    char* hello;
    int new_socket;

    while (1) {

        new_socket = server->_read(
            server,
            buffer_size,
            buffer
        );

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