#include "../include/crypto.h"
#include "../include/sha.h"
#include <string.h>
#include <stdlib.h>

void tls_prf(const uint8_t *secret, size_t secret_len,
            const char *label,
            const uint8_t *seed, size_t seed_len,
            uint8_t *output, size_t out_len) {
    size_t label_len = strlen(label);
    size_t A_len = label_len + seed_len;
    uint8_t *label_seed = (uint8_t *)malloc(A_len);
    memcpy(label_seed, label, label_len);
    memcpy(label_seed + label_len, seed, seed_len);
    uint8_t A[SHA256_DIGEST_SIZE];
    hmac_sha256(secret, secret_len, label_seed, A_len, A);
    uint8_t result[SHA256_DIGEST_SIZE];
    size_t generated = 0;
    while (generated < out_len) {
        uint8_t *block = (uint8_t *)malloc(SHA256_DIGEST_SIZE + A_len);
        memcpy(block, A, SHA256_DIGEST_SIZE);
        memcpy(block + SHA256_DIGEST_SIZE, label_seed, A_len);
        hmac_sha256(secret, secret_len, block, SHA256_DIGEST_SIZE + A_len, result);
        free(block);
        size_t copy_len = (out_len - generated < SHA256_DIGEST_SIZE) ? (out_len - generated) : SHA256_DIGEST_SIZE;
        memcpy(output + generated, result, copy_len);
        generated += copy_len;
        hmac_sha256(secret, secret_len, A, SHA256_DIGEST_SIZE, A);
    }
    free(label_seed);
}