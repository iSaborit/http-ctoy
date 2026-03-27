#include "handler.h"
#include "file.h"
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
static void internal_server_error(http_response *r);
static void html_response(http_response *r, const char *body, const size_t len);
static void css_response(http_response *r, const char *body, const size_t len);

void get_default_handler(const http_request *request, const int client_fd) {
    UNUSED(request);

    http_response r;
    ok_response(&r, "ok buddy\n");
    send_response(client_fd, &r);
}

void get_home(const http_request *request, const int client_fd) {
    UNUSED(request);

    http_response r;
    file_content file = file_read("static/index.html");
    if (file.size == 0) {
        fputs("Could not send index.html", stderr);

        http_response r;
        internal_server_error(&r);
        send_response(client_fd, &r);

        return;
    }

    html_response(&r, file.data, file.size);
    send_response(client_fd, &r);
    file_delete(&file);
}
void get_about(const http_request *request, const int client_fd) {
    UNUSED(request);

    http_response r;
    file_content file = file_read("static/about.html");
    if (file.size == 0) {
        fputs("Could not send about.html", stderr);

        http_response r;
        internal_server_error(&r);
        send_response(client_fd, &r);

        return;
    }

    html_response(&r, file.data, file.size);
    send_response(client_fd, &r);
    file_delete(&file);
}

void get_style(const http_request *request, const int client_fd) {
    UNUSED(request);

    http_response r;
    file_content file = file_read("static/style.css");
    if (file.size == 0) {
        fputs("Could not send style_css", stderr);

        http_response r;
        internal_server_error(&r);
        send_response(client_fd, &r);

        return;
    }

    css_response(&r, file.data, file.size);
    send_response(client_fd, &r);
    file_delete(&file);
}

static void html_response(http_response *r, const char *body,
                          const size_t len) {
    r->status_code = 200;
    r->reason = "OK";
    r->content_type = "text/html";
    r->body = body;
    r->body_length = len;
}

static void css_response(http_response *r, const char *body, const size_t len) {
    r->status_code = 200;
    r->reason = "OK";
    r->content_type = "text/css";
    r->body = body;
    r->body_length = len;
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

static void internal_server_error(http_response *r) {
    r->status_code = 500;
    r->reason = "Internal Server Error";
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
