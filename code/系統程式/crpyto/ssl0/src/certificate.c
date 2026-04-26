#include "../include/certificate.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_decode(const char *src, size_t src_len, uint8_t *out, size_t *out_len) {
    size_t i, j = 0;
    int val;
    int bits = 0;
    int buffer = 0;
    
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

static int get_asn1_length(const uint8_t *data, size_t data_len, size_t *len_out, size_t *header_len_out) {
    if (data_len < 2) return -1;
    
    if (data[1] < 0x80) {
        *len_out = data[1];
        *header_len_out = 2;
        return 0;
    }
    
    size_t num_bytes = data[1] & 0x7f;
    if (num_bytes == 0 || num_bytes > sizeof(size_t)) return -1;
    if (data_len < 2 + num_bytes) return -1;
    
    *len_out = 0;
    for (size_t i = 0; i < num_bytes; i++) {
        *len_out = (*len_out << 8) | data[2 + i];
    }
    *header_len_out = 2 + num_bytes;
    return 0;
}

int x509_parse_from_pem(const char *pem, x509_cert *cert) {
    memset(cert, 0, sizeof(x509_cert));
    
    const char *begin = strstr(pem, "-----BEGIN");
    if (!begin) return -1;
    const char *begin_nl = strchr(begin, '\n');
    if (!begin_nl) return -1;
    begin_nl++;
    
    const char *end = strstr(begin_nl, "-----END");
    if (!end || end <= begin_nl) return -1;
    
    size_t b64_len = end - begin_nl;
    char *b64_clean = (char *)malloc(b64_len + 1);
    if (!b64_clean) return -1;
    
    size_t clean_len = 0;
    for (size_t i = 0; i < b64_len; i++) {
        if (begin_nl[i] != '\n' && begin_nl[i] != '\r' && begin_nl[i] != ' ') {
            b64_clean[clean_len++] = begin_nl[i];
        }
    }
    b64_clean[clean_len] = '\0';
    
    uint8_t *der = (uint8_t *)malloc(clean_len * 3 / 4 + 3);
    if (!der) {
        free(b64_clean);
        return -1;
    }
    
    size_t der_len;
    if (base64_decode(b64_clean, clean_len, der, &der_len) != 0) {
        free(b64_clean);
        free(der);
        return -1;
    }
    free(b64_clean);
    
    if (der_len < 4 || der[0] != 0x30) {
        free(der);
        return -1;
    }
    
    size_t cert_len, cert_header;
    if (get_asn1_length(der, der_len, &cert_len, &cert_header) != 0 || der_len < cert_header + cert_len) {
        free(der);
        return -1;
    }
    
    const uint8_t *tbs = der + cert_header;
    size_t tbs_len, tbs_header;
    if (get_asn1_length(tbs, cert_len, &tbs_len, &tbs_header) != 0 || cert_len < tbs_header + tbs_len) {
        free(der);
        return -1;
    }
    
    const uint8_t *tbs_content = tbs + tbs_header;
    size_t tbs_content_len = tbs_len;
    
    size_t pos = 0;
    int found_key = 0;
    
    while (pos + 4 < tbs_content_len && !found_key) {
        uint8_t tag = tbs_content[pos];
        size_t len, header_len;
        
        if (get_asn1_length(tbs_content + pos, tbs_content_len - pos, &len, &header_len) != 0) {
            break;
        }
        
        if (pos + header_len + len > tbs_content_len) {
            break;
        }
        
        if (tag == 0x30) {
            do {
                const uint8_t *spki = tbs_content + pos + header_len;
                size_t spki_len = len;
                
                size_t alg_len, alg_header;
                if (get_asn1_length(spki, spki_len, &alg_len, &alg_header) != 0) break;
                if (spki[0] != 0x30) break;
                if (spki_len < alg_header + alg_len) break;
                
                const uint8_t *after_alg = spki + alg_header + alg_len;
                size_t remaining = spki_len - alg_header - alg_len;
                
                if (remaining < 2 || after_alg[0] != 0x03) break;
                
                size_t bs_len, bs_header;
                if (get_asn1_length(after_alg, remaining, &bs_len, &bs_header) != 0) break;
                if (remaining < bs_header + bs_len) break;
                
                const uint8_t *bs_content = after_alg + bs_header;
                
                if (bs_len == 0) break;
                
                if (bs_content[0] == 0x00) {
                    bs_content++;
                    bs_len--;
                }
                
                if (bs_len == 0 || bs_content[0] != 0x30) break;
                
                size_t rsa_len, rsa_header;
                if (get_asn1_length(bs_content, bs_len, &rsa_len, &rsa_header) != 0) break;
                if (bs_len < rsa_header + rsa_len) break;
                
                const uint8_t *rsa_content = bs_content + rsa_header;
                
                if (rsa_len < 2 || rsa_content[0] != 0x02) break;
                size_t n_len, n_header;
                if (get_asn1_length(rsa_content, rsa_len, &n_len, &n_header) != 0) break;
                if (rsa_len < n_header + n_len) break;
                
                const uint8_t *n_data = rsa_content + n_header;
                size_t n_copy_len = n_len;
                if (n_copy_len > 0 && n_data[0] == 0x00) {
                    n_data++;
                    n_copy_len--;
                }
                
                if (n_copy_len > sizeof(cert->public_key.n)) break;
                
                const uint8_t *e_data = rsa_content + n_header + n_len;
                size_t e_remaining = rsa_len - n_header - n_len;
                
                if (e_remaining < 2 || e_data[0] != 0x02) break;
                size_t e_len, e_header;
                if (get_asn1_length(e_data, e_remaining, &e_len, &e_header) != 0) break;
                if (e_remaining < e_header + e_len) break;
                
                const uint8_t *e_value = e_data + e_header;
                if (e_len > sizeof(cert->public_key.e)) break;
                
                memcpy(cert->public_key.n, n_data, n_copy_len);
                cert->public_key.n_len = n_copy_len;
                
                memcpy(cert->public_key.e, e_value, e_len);
                cert->public_key.e_len = e_len;
                
                found_key = 1;
            } while (0);
        }
        
        pos += header_len + len;
    }
    
    free(der);
    return found_key ? 0 : -1;
}

int x509_get_public_key(const x509_cert *cert, uint8_t *n, size_t *n_len, uint8_t *e, size_t *e_len) {
    if (cert->public_key.n_len == 0) return -1;
    memcpy(n, cert->public_key.n, cert->public_key.n_len);
    *n_len = cert->public_key.n_len;
    memcpy(e, cert->public_key.e, cert->public_key.e_len);
    *e_len = cert->public_key.e_len;
    return 0;
}

void x509_free(x509_cert *cert) {
    memset(cert, 0, sizeof(x509_cert));
}