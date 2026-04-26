# ssl0

Minimalist SSL/TLS framework in C. Implements the minimum required for HTTPS without external crypto libraries.

## Build & Test

- Build: `make` (uses Makefile with gcc -I include)
- Run all tests: `make test` or `make clean && make test`
- Build single test: `make test/<name>_test` (e.g., `make test/ssl_test`)
- Run single test binary: `./test/ssl_test`

## Structure

- Source: `src/`, Headers: `include/`, Tests: `test/`
- Entry points: `src/ssl.c`, `src/ssl_socket.c`

## Running HTTPS Server

```bash
make https/httpd_ssl0
./https/httpd_ssl0 8443
```

Then visit `https://localhost:8443` (may need to accept self-signed cert).

## Notes

- Tests in `test/` are git-tracked (not in `.gitignore`)
- Dependencies: OpenSSL (for certificate generation), standard C library
- Components: ssl, ssl_socket, crypto, aes, bignum, rsa, certificate, sha, sha1, rand