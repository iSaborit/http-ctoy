#ifndef HTTPSV_HTTP_H
#define HTTPSV_HTTP_H

#include <stddef.h>

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_UNKNOWN
} http_method;

typedef struct {
    http_method method;
    char path[1024];
    char version[16];
} http_request;

typedef struct {
    int status_code;
    const char *reason;
    const char *content_type;
    const char *body;
    size_t body_length;
} http_response;

int http_handle_client(int client_fd);

#endif // !HTTPSV_HTTP_H
