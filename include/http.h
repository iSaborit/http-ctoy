#ifndef HTTPSV_HTTP_H
#define HTTPSV_HTTP_H

typedef struct {
    char method[8];
    char path[1024];
    char version[16];
} http_request;

int http_handle_client(int client_fd);

#endif // !HTTPSV_HTTP_H
