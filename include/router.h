#ifndef HTTPSV_ROUTER_H
#define HTTPSV_ROUTER_H

#include "handler.h"
#include "http.h"

typedef struct {
    http_method method;
    char path[1024];
    handler_fn handler;
} route;

int router_dispatch(const http_request *req, const int client_fd);

#endif // !HTTPSV_ROUTER_H
