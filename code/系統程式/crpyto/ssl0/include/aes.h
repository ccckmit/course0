#ifndef AES_H
#define AES_H

#include <stddef.h>
#include <stdint.h>

void aes_cbc_encrypt(const uint8_t *key, const uint8_t *iv,
                      const uint8_t *plaintext, size_t len,
                      uint8_t *ciphertext);

void aes_cbc_decrypt(const uint8_t *key, const uint8_t *iv,
                      const uint8_t *ciphertext, size_t len,
                      uint8_t *plaintext);

#endif