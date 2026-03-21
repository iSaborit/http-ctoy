#include "http.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int parse_request_line(const char *buffer, http_request *request);
// static void send_response();
// static void send_bad_request();

int http_handle_client(int client_fd) {
    http_request req;
    char buffer[4096];

    ssize_t n = read(client_fd, buffer, 4096);
    if (n <= 0) {
        return -1;
    }

    buffer[n] = '\0';

    if (parse_request_line(buffer, &req)) {
        perror("parse");
        return -1;
    }

    printf("%s\n%s\n%s\n", req.method, req.path, req.version);
    fflush(stdout);

    close(client_fd);
    return 0;
}

static int parse_request_line(const char *buffer, http_request *request) {
    if (sscanf(buffer, "%7s %1023s %15s", request->method, request->path,
               request->version) != 3) {
        perror("parsing header");
        return -1;
    }
    if (strcmp(request->version, "HTTP/1.1") != 0 && strcmp(request->version, "HTTP/1.0") != 0) {
        perror("getting version");
    }

    return 0;
}
