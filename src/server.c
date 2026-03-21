#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int server_start(int *server_fd, int port) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd < 0) {
        perror("socket");
        return -1;
    }

    // permitir reutilizar puerto
    int opt = 1;
    setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in sockaddr = {0};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(port);

    socklen_t socket_len = sizeof(sockaddr);
    if (bind(*server_fd, (struct sockaddr *)&sockaddr, socket_len) < 0) {
        perror("bind");
        close(*server_fd);
        return -1;
    }

    if (listen(*server_fd, 10) < 0) {
        perror("listen");
        close(*server_fd);
        return -1;
    }

    return 0;
}

int server_accept(int server_fd) {
    struct sockaddr_in client;
    socklen_t client_t = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_t);
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    return client_fd;
}
