#include "Server.h"

// void test(Server *server) {
//     printf("WAITING...\n");

    
//     printf("DONE\n");
// }

void handle(Server* server, char inData[]) {
    printf("======== HANDLING ========\n");
    printf(inData);
    printf("======== HANDLED =========\n");
}

void init(Server* server) {
    printf("LISTENING...\n");
}

int main() {
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8080, 10, handle);

    server.launch(&server, init);
}