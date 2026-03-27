#ifndef HTTPSV_HANDLER_H
#define HTTPSV_HANDLER_H

#include "http.h"
typedef void (*handler_fn)(const http_request *request, const int client_fd);

void get_default_handler(const http_request *request, const int client_fd);
void get_home(const http_request *request, const int client_fd);
void get_about(const http_request *request, const int client_fd);
void get_style(const http_request *request, const int client_fd);

#endif // !HTTPSV_HANDLER_H
