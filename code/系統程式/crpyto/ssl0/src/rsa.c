#include "../include/rsa.h"
#include "../include/bignum.h"
#include <string.h>

int rsa_encrypt(const uint8_t *n, size_t n_len,
                const uint8_t *e, size_t e_len,
                const uint8_t *plaintext, size_t plain_len,
                uint8_t *ciphertext, size_t *cipher_len) {
    bignum N, E, M, C;
    bn_from_bytes(&N, n, n_len);
    bn_from_bytes(&E, e, e_len);
    bn_from_bytes(&M, plaintext, plain_len);
    bn_mod_exp(&C, &M, &E, &N);
    bn_to_bytes(&C, ciphertext, cipher_len);
    return 0;
}

int rsa_decrypt(const uint8_t *n, size_t n_len,
                const uint8_t *d, size_t d_len,
                const uint8_t *ciphertext, size_t cipher_len,
                uint8_t *plaintext, size_t *plain_len) {
    bignum N, D, C, M;
    bn_from_bytes(&N, n, n_len);
    bn_from_bytes(&D, d, d_len);
    bn_from_bytes(&C, ciphertext, cipher_len);
    bn_mod_exp(&M, &C, &D, &N);
    bn_to_bytes(&M, plaintext, plain_len);
    return 0;
}