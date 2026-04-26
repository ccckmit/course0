# ssl0

A minimalist SSL framework in C. Implements the minimum required to establish HTTPS connections without external SSL/crypto libraries.

## Build

```bash
make
```

## Test

```bash
make test
```

Or run individual test scripts:

```bash
./ssl_test.sh
./ssl_socket_test.sh
./aes_test.sh
# ...
```

## HTTPS Server

```bash
make https/httpd_ssl0
./https/httpd_ssl0 8443
```

Then visit `https://localhost:8443` in your browser.

## Structure

```
include/      - Header files
src/          - Source files
test/         - Test source files
https/        - HTTPS server
```

## Components

- **ssl** - SSL record encryption/decryption
- **ssl_socket** - TCP socket wrapper with SSL support
- **crypto** - SHA256, HMAC, TLS-PRF
- **aes** - AES-CBC encryption
- **bignum** - Big integer arithmetic (for RSA)
- **rsa** - RSA encryption/decryption
- **certificate** - X.509 certificate parsing
- **sha / sha1** - SHA256 / HMAC-SHA1
- **rand** - Random bytes generation

## Design

Minimal code principle: only implement what's needed for HTTPS.
Certificate generation uses OpenSSL; ssl0 handles the SSL protocol.