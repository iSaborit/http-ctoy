#ifndef HTTPSV_SERVER_H
#define HTTPSV_SERVER_H

int server_start(int *server_fd, int port);
int server_accept(int server_fd);

#endif // !HTTPSV_SERVER_H
