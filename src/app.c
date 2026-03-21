#include "app.h"
#include "http.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int app_run() {
    int port = 4000;
    int server_fd;
    if (server_start(&server_fd, port)) {
        perror("server start");
        exit(-1);
    }

    while (1) {
        int client_fd = server_accept(server_fd);
        if (client_fd < 0)
            continue;
        http_handle_client(client_fd);
        close(client_fd);
    }

    return 0;
}
