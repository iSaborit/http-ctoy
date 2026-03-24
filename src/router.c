#include "router.h"
#include "handler.h"

#include <string.h>

static const route routes[] = {
    {HTTP_GET, "/", get_default_handler},
};

static const size_t routes_count = sizeof(routes) / sizeof(routes[0]);

int router_dispatch(const http_request *req, const int client_fd) {
    for (size_t i = 0; i < routes_count; i++) {
        if (routes[i].method == req->method &&
            strcmp(routes[i].path, req->path) == 0) {
            routes[i].handler(req, client_fd);
            return 0;
        }
    }
    return -1;
}
