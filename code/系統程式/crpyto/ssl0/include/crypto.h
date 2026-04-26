#ifndef CRYPTO_H
#define CRYPTO_H

#include "sha.h"
#include "aes.h"
#include "rsa.h"

void tls_prf(const uint8_t *secret, size_t secret_len,
            const char *label,
            const uint8_t *seed, size_t seed_len,
            uint8_t *output, size_t out_len);

#endif