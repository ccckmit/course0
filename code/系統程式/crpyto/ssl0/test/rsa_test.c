#include <stdio.h>
#include <string.h>
#include "../include/bignum.h"
#include "../include/common.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) printf("%02x", data[i]);
    printf("\n");
}

int main() {
    printf("=== RSA tests ===\n\n");
    
    // Test 1: small RSA (n=55, e=3, d=27)
    {
        uint8_t n[] = {0x37};  // 55
        uint8_t e[] = {0x03};
        uint8_t d[] = {0x1b};  // 27
        uint8_t m[] = {0x02};  // 2
        
        bignum N, E, D, M, C, R;
        bn_from_bytes(&N, n, sizeof(n));
        bn_from_bytes(&E, e, sizeof(e));
        bn_from_bytes(&D, d, sizeof(d));
        bn_from_bytes(&M, m, sizeof(m));
        
        bn_mod_exp(&C, &M, &E, &N);
        bn_mod_exp(&R, &C, &D, &N);
        
        uint8_t result[16];
        size_t result_len;
        bn_to_bytes(&R, result, &result_len);
        
        CHECK(result_len == 1 && result[0] == 2, "RSA small: 2^3 mod 55 -> 8, then 8^27 mod 55 -> 2");
    }
    
    // Test 2: medium RSA (n=143, e=7, d=103)
    {
        uint8_t n[] = {0x8f};  // 143
        uint8_t e[] = {0x07};  // 7
        uint8_t d[] = {0x67};  // 103
        uint8_t m[] = {0x05};  // 5
        
        bignum N, E, D, M, C, R;
        bn_from_bytes(&N, n, sizeof(n));
        bn_from_bytes(&E, e, sizeof(e));
        bn_from_bytes(&D, d, sizeof(d));
        bn_from_bytes(&M, m, sizeof(m));
        
        bn_mod_exp(&C, &M, &E, &N);
        bn_mod_exp(&R, &C, &D, &N);
        
        uint8_t result[16];
        size_t result_len;
        bn_to_bytes(&R, result, &result_len);
        
        CHECK(result_len == 1 && result[0] == 5, "RSA medium: 5^7 mod 143 -> 47, then 47^103 mod 143 -> 5");
    }
    
    // Test 3: large RSA (n=3233, e=17, d=2753)
    {
        uint8_t n[] = {0x0c, 0xa1};  // 3233
        uint8_t e[] = {0x11};  // 17
        uint8_t d[] = {0x0a, 0xc1};  // 2753
        uint8_t m[] = {0x41};  // 65
        
        bignum N, E, D, M, C, R;
        bn_from_bytes(&N, n, sizeof(n));
        bn_from_bytes(&E, e, sizeof(e));
        bn_from_bytes(&D, d, sizeof(d));
        bn_from_bytes(&M, m, sizeof(m));
        
        bn_mod_exp(&C, &M, &E, &N);
        bn_mod_exp(&R, &C, &D, &N);
        
        uint8_t result[16];
        size_t result_len;
        bn_to_bytes(&R, result, &result_len);
        
        CHECK(result_len == 1 && result[0] == 65, "RSA large: 65^17 mod 3233 -> C^17, then C^2753 mod 3233 -> 65");
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}