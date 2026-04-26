#include <stdio.h>
#include <string.h>
#include "../include/sha.h"
#include "../include/common.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) printf("%02x", data[i]);
    printf("\n");
}

int main() {
    printf("=== SHA tests ===\n\n");
    
    /* SHA-256 known answer test */
    {
        const char *msg = "hello";
        uint8_t digest[SHA256_DIGEST_SIZE];
        uint8_t expected[SHA256_DIGEST_SIZE] = {
            0x2c, 0xf2, 0x4d, 0xba, 0x5f, 0xb0, 0xa3, 0x0e,
            0x26, 0xe8, 0x3b, 0x2a, 0xc5, 0xb9, 0xe2, 0x9e,
            0x1b, 0x16, 0x1e, 0x5c, 0x1f, 0xa7, 0x42, 0x5e,
            0x73, 0x04, 0x33, 0x62, 0x93, 0x8b, 0x98, 0x24
        };
        
        sha256((uint8_t *)msg, strlen(msg), digest);
        
        CHECK(memcmp(digest, expected, SHA256_DIGEST_SIZE) == 0, "SHA256(hello) KAT");
    }
    
    /* HMAC-SHA256 known answer test */
    {
        const char *key = "key";
        const char *msg = "hello";
        uint8_t mac[SHA256_DIGEST_SIZE];
        uint8_t expected[SHA256_DIGEST_SIZE] = {
            0x93, 0x07, 0xb3, 0xb9, 0x15, 0xef, 0xb5, 0x17,
            0x1f, 0xf1, 0x4d, 0x8c, 0xb5, 0x5f, 0xbc, 0xc7,
            0x98, 0xc6, 0xc0, 0xef, 0x14, 0x56, 0xd6, 0x6d,
            0xed, 0x1a, 0x6a, 0xa7, 0x23, 0xa5, 0x8b, 0x7b
        };
        
        hmac_sha256((uint8_t *)key, strlen(key), (uint8_t *)msg, strlen(msg), mac);
        
        CHECK(memcmp(mac, expected, SHA256_DIGEST_SIZE) == 0, "HMAC(key, hello) KAT");
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}