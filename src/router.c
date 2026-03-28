#include "router.h"
#include "handler.h"
#include "http.h"

#include <string.h>

static const route routes[] = {
    {HTTP_GET, "/", get_default_handler},
    {HTTP_GET, "/home", get_home},
    {HTTP_GET, "/index.html", get_home},
    {HTTP_GET, "/about.html", get_about},
    {HTTP_GET, "/contact.html", get_contact},
    {HTTP_GET, "/style.css", get_style},
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

    handler_not_found(client_fd);
    return -1;
}
