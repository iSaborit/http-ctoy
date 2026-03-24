#include "http.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int parse_request_line(const char *buffer, http_request *request);
static int send_response(int client_fd, const http_response *r);
static void ok_response(http_response *r, const char *body);
static void method_not_allowed(http_response *r);
static http_method str_to_method(const char *buffer);
static void not_found(http_response *r);
// static void send_bad_request();

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

    if (req.method == HTTP_GET) {
        http_response r;
        ok_response(&r, "tysmtysm");
        send_response(client_fd, &r);
    } else if (req.method == HTTP_UNKNOWN) {
        http_response r;
        method_not_allowed(&r);
        send_response(client_fd, &r);
    } else if (req.method == HTTP_PUT) {
        http_response r;
        not_found(&r);
        send_response(client_fd, &r);
    }

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
    if (strcmp(buffer, "GET")) {
        return HTTP_GET;
    }
    return HTTP_UNKNOWN;
}

static int send_response(int client_fd, const http_response *r) {
    char header[1024];
    size_t len = 0;

    len += snprintf(header + len, sizeof(header) - len, "HTTP/1.1 %d %s\r\n",
                    r->status_code, r->reason);

    if (r->content_type) {
        len += snprintf(header + len, sizeof(header) - len,
                        "Content-Type: %s\r\n", r->content_type);
    }

    if (r->body && r->body_length > 0) {
        len += snprintf(header + len, sizeof(header) - len,
                        "Content-Length: %zu\r\n", r->body_length);
    }

    len +=
        snprintf(header + len, sizeof(header) - len, "Connection: close\r\n");

    len += snprintf(header + len, sizeof(header) - len, "\r\n");

    if (len >= sizeof(header)) {
        return -1;
    }

    if (write(client_fd, header, len) < 0) {
        perror("write");
        return -1;
    }

    if (r->body && r->body_length > 0) {
        if (write(client_fd, r->body, r->body_length) < 0) {
            perror("write");
            return -1;
        }
    }

    return 0;
}

static void ok_response(http_response *r, const char *body) {
    r->status_code = 200;
    r->reason = "OK";
    r->content_type = "text/plain";
    r->body = body;
    r->body_length = strlen(r->body);
}

static void not_found(http_response *r) {
    r->status_code = 404;
    r->reason = "Not Found";
    r->content_type = NULL;

    r->body = NULL;
    r->body_length = 0;
}

static void method_not_allowed(http_response *r) {
    r->status_code = 405;
    r->reason = "Method Not Allowed";
    r->content_type = NULL;

    r->body = NULL;
    r->body_length = 0;
}
