#ifndef RSA_H
#define RSA_H

#include <stddef.h>
#include <stdint.h>

int rsa_encrypt(const uint8_t *n, size_t n_len,
                const uint8_t *e, size_t e_len,
                const uint8_t *plaintext, size_t plain_len,
                uint8_t *ciphertext, size_t *cipher_len);

int rsa_decrypt(const uint8_t *n, size_t n_len,
                const uint8_t *d, size_t d_len,
                const uint8_t *ciphertext, size_t cipher_len,
                uint8_t *plaintext, size_t *plain_len);

#endif