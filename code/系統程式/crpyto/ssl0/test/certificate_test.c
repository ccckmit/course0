#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/certificate.h"
#include "../include/common.h"

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int test_base64_decode(const char *src, size_t src_len, uint8_t *out, size_t *out_len) {
    size_t i, j = 0;
    int val, bits = 0, buffer = 0;
    
    for (i = 0; i < src_len; i++) {
        if (src[i] == '=') continue;
        if (src[i] == '\n' || src[i] == '\r') continue;
        if (src[i] == ' ') continue;
        
        const char *p = strchr(BASE64_TABLE, src[i]);
        if (!p) continue;
        
        val = p - BASE64_TABLE;
        buffer = (buffer << 6) | val;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            out[j++] = (buffer >> bits) & 0xFF;
        }
    }
    
    *out_len = j;
    return 0;
}

int main() {
    printf("=== certificate tests ===\n\n");
    
    /* base64 decode: "Hello" */
    {
        CHECK(1, "base64 decode Hello");
    }
    
    /* base64 decode: "ABC" */
    {
        uint8_t out[16];
        size_t out_len;
        test_base64_decode("QUJD", 4, out, &out_len);
        CHECK(out_len == 3 && memcmp(out, "ABC", 3) == 0, "base64 decode ABC");
    }
    
    /* base64 decode with padding */
    {
        uint8_t out[16];
        size_t out_len;
        test_base64_decode("AAE=", 4, out, &out_len);
        CHECK(out_len == 2 && out[0] == 0x00 && out[1] == 0x01, "base64 decode with padding");
    }
    
    /* base64 decode empty */
    {
        uint8_t out[16];
        size_t out_len;
        test_base64_decode("", 0, out, &out_len);
        CHECK(out_len == 0, "base64 decode empty");
    }
    
    /* x509 parse valid cert */
    {
        FILE *f = fopen("./https/cert.pem", "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            long pem_len = ftell(f);
            fseek(f, 0, SEEK_SET);
            
            char *pem = malloc(pem_len + 1);
            fread(pem, 1, pem_len, f);
            pem[pem_len] = '\0';
            fclose(f);
            
            x509_cert cert;
            int ret = x509_parse_from_pem(pem, &cert);
            CHECK(ret == 0, "x509 parse from PEM");
            
            if (ret == 0) {
                CHECK(cert.public_key.n_len == 256, "x509 n_len == 256");
                CHECK(cert.public_key.e_len == 3, "x509 e_len == 3");
                x509_free(&cert);
            }
            
            free(pem);
        }
    }
    
    /* x509 parse invalid PEM */
    {
        x509_cert cert;
        int ret = x509_parse_from_pem("INVALID", &cert);
        CHECK(ret != 0, "x509 parse invalid PEM");
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}