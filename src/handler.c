#include "handler.h"

#include <unistd.h>

void get_default_handler(const http_request *request, const int client_fd) {
    (void)request;
    write(client_fd, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\nConnection: close\r\n\r\nok\n",
          60);
}
