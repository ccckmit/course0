#include <stdio.h>
#include <string.h>
#include "../include/ssl.h"
#include "../include/crypto.h"
#include "../include/common.h"

int main() {
    printf("=== ssl tests ===\n\n");
    
    /* ssl_context_init/free */
    {
        ssl_context ctx;
        ssl_context_init(&ctx);
        CHECK(ctx.has_keys == 0, "ssl_context_init");
        
        ssl_free(&ctx);
        CHECK(ctx.has_keys == 0, "ssl_free");
    }
    
    /* ssl_compute_master_secret */
    {
        uint8_t pms[] = {0x01, 0x02, 0x03};
        uint8_t client_random[] = {0x01, 0x02, 0x03};
        uint8_t server_random[] = {0x04, 0x05, 0x06};
        uint8_t master_secret[48];
        
        int ret = ssl_compute_master_secret(pms, 3, client_random, 3, server_random, 3, master_secret);
        CHECK(ret == 0, "ssl_compute_master_secret");
        CHECK(master_secret[0] != 0 || master_secret[47] != 0, "master_secret non-zero");
    }
    
/* ssl_derive_keys */
  {
    ssl_context ctx;
    ssl_context_init(&ctx);
    uint8_t master_secret[48] = {0};
    uint8_t client_random[] = {0x01, 0x02, 0x03};
    uint8_t server_random[] = {0x04, 0x05, 0x06};

    int ret = ssl_derive_keys(&ctx, master_secret, client_random, 3, server_random, 3);
    CHECK(ret == 0, "ssl_derive_keys");
    CHECK(ctx.client_write_key[0] != ctx.server_write_key[0], "client_key != server_key");
  }
    
    /* ssl_handshake_client (full key derivation) */
    {
        ssl_context ctx;
        ssl_context_init(&ctx);
        
        uint8_t pms[] = {0x01, 0x02, 0x03};
        uint8_t client_random[32], server_random[32];
        for (int i = 0; i < 32; i++) {
            client_random[i] = i;
            server_random[i] = 0x20 - i;
        }
        
        int ret = ssl_handshake_client(&ctx, NULL, 0, client_random, 32, server_random, 32, pms, 3);
        CHECK(ret == 0, "ssl_handshake_client");
        CHECK(ctx.has_keys == 1, "has_keys set");
        
        ssl_free(&ctx);
    }
    
    /* ssl_encrypt_record / ssl_decrypt_record - use same context for symmetric test */
    {
        ssl_context ctx;
        ssl_context_init(&ctx);
        
        /* Setup keys manually for symmetric test */
        uint8_t pms[] = {0x01, 0x02, 0x03};
        uint8_t client_random[32] = {0}, server_random[32] = {0};
        ssl_handshake_client(&ctx, NULL, 0, client_random, 32, server_random, 32, pms, 3);
        
/* Copy keys for symmetric encryption/decryption (client->server uses server keys) */
  memcpy(ctx.server_write_key, ctx.client_write_key, 16);
        
        /* Encrypt */
        uint8_t plaintext[] = "Hello TLS!";
        uint8_t ciphertext[128];
        size_t cipher_len;
        
        int ret = ssl_encrypt_record(&ctx, 0x17, plaintext, 11, ciphertext, &cipher_len);
        CHECK(ret == 0, "ssl_encrypt_record");
        
        /* Decrypt */
        uint8_t content_type;
        uint8_t decrypted[128];
        size_t plain_len;
        
        ret = ssl_decrypt_record(&ctx, ciphertext, cipher_len, &content_type, decrypted, &plain_len);
        CHECK(ret == 0, "ssl_decrypt_record");
        CHECK(plain_len == 11, "decrypted length");
        CHECK(memcmp(decrypted, plaintext, 11) == 0, "decrypted content");
        
        ssl_free(&ctx);
    }
    
    /* ssl_encrypt_record without keys fails */
    {
        ssl_context ctx;
        ssl_context_init(&ctx);
        
        uint8_t plaintext[] = "Test";
        uint8_t ciphertext[64];
        size_t cipher_len;
        
        int ret = ssl_encrypt_record(&ctx, 0x17, plaintext, 4, ciphertext, &cipher_len);
        CHECK(ret != 0, "encrypt without keys fails");
        
        ssl_free(&ctx);
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}