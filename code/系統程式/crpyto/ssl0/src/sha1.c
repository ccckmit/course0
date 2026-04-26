#include "../include/sha1.h"
#include <string.h>
#include <stdlib.h>

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

static void sha1_transform(uint32_t state[5], const uint8_t data[64]) {
    uint32_t W[80];
    for (int i = 0; i < 16; i++) {
        W[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16) | (data[i * 4 + 2] << 8) | data[i * 4 + 3];
    }
    for (int i = 16; i < 80; i++) {
        W[i] = ROTLEFT(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
    }
    
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | (~b & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        
        uint32_t temp = ROTLEFT(a, 5) + f + e + k + W[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = temp;
    }
    
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void sha1(const uint8_t *data, size_t len, uint8_t *digest) {
    uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    
    size_t padded_len = len + 1 + 8;
    if (padded_len % 64 != 0) padded_len += 64 - (padded_len % 64);
    
    uint8_t *msg = (uint8_t *)calloc(padded_len, 1);
    memcpy(msg, data, len);
    msg[len] = 0x80;
    
    uint64_t bits = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++) {
        msg[padded_len - 1 - i] = (bits >> (i * 8)) & 0xff;
    }
    
    for (size_t i = 0; i < padded_len; i += 64) {
        sha1_transform(state, msg + i);
    }
    
    for (int i = 0; i < 5; i++) {
        digest[i * 4] = (state[i] >> 24) & 0xff;
        digest[i * 4 + 1] = (state[i] >> 16) & 0xff;
        digest[i * 4 + 2] = (state[i] >> 8) & 0xff;
        digest[i * 4 + 3] = state[i] & 0xff;
    }
    free(msg);
}

void hmac_sha1(const uint8_t *key, size_t key_len,
               const uint8_t *data, size_t data_len,
               uint8_t *mac) {
    uint8_t k_ipad[64], k_opad[64];
    memset(k_ipad, 0x36, 64);
    memset(k_opad, 0x5c, 64);
    
    uint8_t keybuf[20];
    if (key_len > 64) {
        sha1(key, key_len, keybuf);
        key = keybuf;
        key_len = 20;
    }
    
    for (size_t i = 0; i < key_len; i++) {
        k_ipad[i] ^= key[i];
        k_opad[i] ^= key[i];
    }
    
    uint8_t *inner_msg = (uint8_t *)malloc(64 + data_len);
    memcpy(inner_msg, k_ipad, 64);
    memcpy(inner_msg + 64, data, data_len);
    uint8_t inner_hash[20];
    sha1(inner_msg, 64 + data_len, inner_hash);
    free(inner_msg);
    
    uint8_t *outer_msg = (uint8_t *)malloc(64 + 20);
    memcpy(outer_msg, k_opad, 64);
    memcpy(outer_msg + 64, inner_hash, 20);
    sha1(outer_msg, 64 + 20, mac);
    free(outer_msg);
}
