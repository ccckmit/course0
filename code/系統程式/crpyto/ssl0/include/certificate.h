#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t n[256];
    size_t n_len;
    uint8_t e[4];
    size_t e_len;
} rsa_public_key;

typedef struct {
    rsa_public_key public_key;
    uint8_t serial[16];
    size_t serial_len;
    uint8_t subject[256];
    size_t subject_len;
    uint8_t issuer[256];
    size_t issuer_len;
} x509_cert;

int x509_parse_from_pem(const char *pem, x509_cert *cert);
int x509_get_public_key(const x509_cert *cert, uint8_t *n, size_t *n_len, uint8_t *e, size_t *e_len);
void x509_free(x509_cert *cert);

#endif