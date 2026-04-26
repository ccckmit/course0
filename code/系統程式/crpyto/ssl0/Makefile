CC = gcc
CFLAGS = -I include
SRCS = src/ssl.c src/crypto.c src/sha.c src/sha1.c src/aes.c src/bignum.c src/rsa.c src/certificate.c src/rand.c src/ssl_socket.c
TESTS = test/ssl_test test/ssl_socket_test test/aes_test test/bignum_test \
	test/certificate_test test/crypto_test test/rand_test test/rsa_test test/sha_test

all: $(TESTS)

test/ssl_test: test/ssl_test.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

test/ssl_socket_test: test/ssl_socket_test.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

test/aes_test: test/aes_test.c src/aes.c
	$(CC) $(CFLAGS) -o $@ $< src/aes.c

test/bignum_test: test/bignum_test.c src/bignum.c
	$(CC) $(CFLAGS) -o $@ $< src/bignum.c

test/certificate_test: test/certificate_test.c src/certificate.c
	$(CC) $(CFLAGS) -o $@ $< src/certificate.c

test/crypto_test: test/crypto_test.c src/crypto.c src/sha.c src/aes.c
	$(CC) $(CFLAGS) -o $@ $< src/crypto.c src/sha.c src/aes.c

test/rand_test: test/rand_test.c src/rand.c
	$(CC) $(CFLAGS) -o $@ $< src/rand.c

test/rsa_test: test/rsa_test.c src/rsa.c src/bignum.c
	$(CC) $(CFLAGS) -o $@ $< src/rsa.c src/bignum.c

test/sha_test: test/sha_test.c src/sha.c
	$(CC) $(CFLAGS) -o $@ $< src/sha.c

https/httpd_ssl0: https/httpd_ssl0.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

test: $(TESTS)
	set -x
	./test/ssl_test
	./test/ssl_socket_test
	./test/aes_test
	./test/bignum_test
	./test/certificate_test
	./test/crypto_test
	./test/rand_test
	./test/rsa_test
	./test/sha_test

clean:
	rm -f test/ssl_test test/ssl_socket_test test/aes_test test/bignum_test
	rm -f test/certificate_test test/crypto_test test/rand_test test/rsa_test test/sha_test
	rm -f https/httpd_ssl0