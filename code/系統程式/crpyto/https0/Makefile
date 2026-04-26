CC = gcc
CFLAGS = -I ../ssl0/include
SRCS = ../ssl0/src/ssl_socket.c ../ssl0/src/ssl.c ../ssl0/src/crypto.c ../ssl0/src/sha.c ../ssl0/src/sha1.c ../ssl0/src/aes.c ../ssl0/src/bignum.c ../ssl0/src/rsa.c ../ssl0/src/certificate.c ../ssl0/src/rand.c

httpd_ssl0: httpd_ssl0.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

all: httpd_ssl0

clean:
	rm -f httpd_ssl0

.PHONY: all clean