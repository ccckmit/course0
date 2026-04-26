#ifndef SHA_H
#define SHA_H

#include <stddef.h>
#include <stdint.h>

#define SHA256_DIGEST_SIZE 32

void sha256(const uint8_t *data, size_t len, uint8_t *digest);

void hmac_sha256(const uint8_t *key, size_t key_len,
                 const uint8_t *data, size_t data_len,
                 uint8_t *mac);

#endif