#include "http.h"
#include "router.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int parse_request_line(const char *buffer, http_request *request);
static http_method str_to_method(const char *buffer);

int http_handle_client(int client_fd) {
    http_request req;
    char buffer[4096];

    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        return -1;
    }

    buffer[n] = '\0';

    if (parse_request_line(buffer, &req)) {
        fputs("parse request failed\n", stderr);
        return -1;
    }

    router_dispatch(&req, client_fd);

    return 0;
}

static int parse_request_line(const char *buffer, http_request *request) {
    char method[32];
    if (sscanf(buffer, "%7s %1023s %15s", method, request->path,
               request->version) != 3) {
        fputs("parsing header\n", stderr);
        return -1;
    }

    request->method = str_to_method(method);

    if (strcmp(request->version, "HTTP/1.1") != 0 &&
        strcmp(request->version, "HTTP/1.0") != 0) {
        fputs("getting version\n", stderr);
        return -1;
    }

    return 0;
}

static http_method str_to_method(const char *buffer) {
    if (!buffer) {
        return HTTP_UNKNOWN;
    }
    if (!strcmp(buffer, "GET")) {
        return HTTP_GET;
    }
    return HTTP_UNKNOWN;
}
