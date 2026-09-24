#include <string.h>
#include <stdio.h>
#include <err.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int handle_client(int client_socket) {
    char buffer[1000];
    ssize_t bytes = 0;

    // response with HTTP/1.0
    const char* res = "HTTP/1.0 200 OK\r\n\r\n<p>Hello world!</p>";

    printf("\n----\n");
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        bytes = read(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes < 0) {
            err(1, "read");
            return -1;
        }
        if (bytes == 0) {
            printf("Connection closed.\n");
            break;
        }

        printf("REQUEST BODY:\n----\n%s", buffer);

        // sending the reponse by writing to the file descriptor client_socket
        write(client_socket, res, strlen(res));
        // closing the socket
        close(client_socket);
        break;
    }
    printf("\n----\n");
    return 0;
}

int main() {
    // creating the socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        close(server_socket);
        err(1, "socket");
    }
    printf("Socket created.\n");

    int on = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // creating the socket address struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // binding the socket to that address
    int return_value = bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr));

    if (return_value == -1) {
        close(server_socket);
        err(1, "bind");
    }
    printf("Bound address to socket.\n");

    return_value = listen(server_socket, SOMAXCONN);

    if (return_value == -1) {
        close(server_socket);
        err(1, "listen");
    }
    printf("Listen sucessful.\n");

    while (1) {
        printf("Waiting for a connection...\n");
        int client_socket = accept(server_socket, NULL, NULL);

        printf("Connection!!!\n");

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
