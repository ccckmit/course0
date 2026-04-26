#include <stdio.h>
#include <string.h>
#include "../include/aes.h"
#include "../include/common.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) printf("%02x", data[i]);
    printf("\n");
}

int main() {
    printf("=== AES tests ===\n\n");
    
    /* AES-CBC encrypt/decrypt round-trip */
    {
        uint8_t key[16] = {0};
        uint8_t iv[16] = {0};
        uint8_t plaintext[] = "Hello TLS!";
        uint8_t ciphertext[32];
        
        aes_cbc_encrypt(key, iv, plaintext, 11, ciphertext);
        
        uint8_t decrypted[32];
        aes_cbc_decrypt(key, iv, ciphertext, 16, decrypted);
        decrypted[11] = '\0';
        
        CHECK(strcmp((char *)decrypted, "Hello TLS!") == 0, "AES-CBC decrypt round-trip");
    }
    
    /* AES-CBC known answer test */
    {
        uint8_t key[16] = {0};
        uint8_t iv[16] = {0};
        uint8_t plaintext[] = "Hello TLS!";
        uint8_t ciphertext[32];
        uint8_t expected[] = {0xdd, 0x89, 0x46, 0xe2, 0x86, 0xcf, 0x4c, 0x51,
                           0x6e, 0x5f, 0x32, 0xe3, 0x70, 0xf5, 0xce, 0x9b};
        
        aes_cbc_encrypt(key, iv, plaintext, 11, ciphertext);
        
        CHECK(memcmp(ciphertext, expected, 16) == 0, "AES-CBC encrypt KAT");
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}