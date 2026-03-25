#include "handler.h"
#include "http.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define UNUSED(x) (void)x
#define FUNUSED __attribute__((unused))

static int send_response(int client_fd, const http_response *r) FUNUSED;
static void ok_response(http_response *r, const char *body);
static void method_not_allowed(http_response *r) FUNUSED;
static void not_found(http_response *r) FUNUSED;

void get_default_handler(const http_request *request, const int client_fd) {
    UNUSED(request);

    http_response r;
    ok_response(&r, "ok buddy\n");
    send_response(client_fd, &r);
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

