#ifndef SSL_SOCKET_H
#define SSL_SOCKET_H

#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>

#include "ssl.h"
#include "certificate.h"

typedef struct {
    int fd;
    ssl_context ctx;
    int connected;
} ssl_socket;

int ssl_socket_init(ssl_socket *sock);
int ssl_socket_bind(ssl_socket *sock, int port);
int ssl_socket_accept(ssl_socket *sock, ssl_socket *client, const char *cert_pem, const char *key_pem);
int ssl_socket_read(ssl_socket *sock, uint8_t *buf, size_t len);
int ssl_socket_write(ssl_socket *sock, const uint8_t *buf, size_t len);
void ssl_socket_close(ssl_socket *sock);

#endif