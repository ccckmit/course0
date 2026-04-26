// ssl_socket.c - TLS Server Socket Implementation

#include "../include/ssl_socket.h"
#include "../include/rand.h"
#include "../include/rsa.h"
#include "../include/bignum.h"
#include "../include/sha.h"
#include "../include/crypto.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TLS_RECORD_HEADER_LEN 5
#define TLS_HANDSHAKE_HEADER_LEN 4
#define TLS_MAX_MESSAGE_LEN 16384

static int send_all(int fd, const uint8_t *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

static int read_until(int fd, uint8_t *buf, size_t *pos, size_t target) {
    while (*pos < target) {
        ssize_t n = recv(fd, buf + *pos, target - *pos, 0);
        if (n <= 0) return -1;
        *pos += n;
    }
    return 0;
}

static int base64_decode_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static size_t pem_to_der(const char *pem, uint8_t *der) {
    const char *begin = strstr(pem, "-----BEGIN");
    if (!begin) return 0;
    const char *data_start = strchr(begin, '\n');
    if (!data_start) return 0;
    
    const char *end = strstr(data_start, "-----END");
    if (!end) return 0;

    size_t out_len = 0;
    uint32_t accumulator = 0;
    int bits = 0;

    for (const char *p = data_start; p < end; p++) {
        int val = base64_decode_char(*p);
        if (val >= 0) {
            accumulator = (accumulator << 6) | val;
            bits += 6;
            if (bits >= 8) {
                bits -= 8;
                der[out_len++] = (accumulator >> bits) & 0xFF;
            }
        }
    }
    return out_len;
}

static int parse_private_key_pem(const char *pem, uint8_t *n, size_t *n_len, uint8_t *d, size_t *d_len) {
    uint8_t der[4096];
    size_t der_len = pem_to_der(pem, der);
    if (der_len == 0) { return -1; }
    
    size_t pos = 0;
    if (der[pos++] != 0x30) { return -1; }
    
    size_t total_len;
    if (der[pos] < 0x80) {
        total_len = der[pos++];
    } else {
        size_t num_bytes = der[pos++] & 0x7f;
        total_len = 0;
        for (size_t i = 0; i < num_bytes; i++) {
            total_len = (total_len << 8) | der[pos++];
        }
    }
    
    // Skip outer SEQUENCE, read version
    if (der[pos++] != 0x02) { return -1; }
    size_t version_len = der[pos++];
    uint8_t version = der[pos];
    pos += version_len;
    
    // Check for PKCS#8 structure:
    // After version INTEGER, PKCS#8 has AlgorithmIdentifier SEQUENCE (0x30)
    // followed by OCTET STRING (0x04) containing PKCS#1 RSAPrivateKey
    
    size_t pkcs1_start = pos;
    if (version == 0 && der[pos] == 0x30) {
        // PKCS#8 - skip AlgorithmIdentifier
        pos++;
        size_t alg_len;
        if (der[pos] < 0x80) {
            alg_len = der[pos++];
        } else if ((der[pos] & 0x7f) == 1) {
            pos++;
            alg_len = der[pos++];
        } else if ((der[pos] & 0x7f) == 2) {
            pos++;
            alg_len = (der[pos] << 8) | der[pos+1];
            pos += 2;
        } else {
            return -1;
        }
        pos += alg_len;
        
        // Next should be OCTET STRING containing PKCS#1
        if (der[pos++] != 0x04) { return -1; }
        
        size_t octet_len;
        if (der[pos] < 0x80) {
            octet_len = der[pos++];
        } else if ((der[pos] & 0x7f) == 1) {
            pos++;
            octet_len = der[pos++];
        } else if ((der[pos] & 0x7f) == 2) {
            pos++;
            octet_len = (der[pos] << 8) | der[pos+1];
            pos += 2;
        } else {
            return -1;
        }
        
        pkcs1_start = pos;
    }
    
    // Parse RSAPrivateKey from pkcs1_start
    pos = pkcs1_start;
    if (der[pos++] != 0x30) { return -1; }
    
    size_t pkcs1_total_len;
    if (der[pos] < 0x80) {
        pkcs1_total_len = der[pos++];
    } else {
        size_t num_bytes = der[pos++] & 0x7f;
        pkcs1_total_len = 0;
        for (size_t i = 0; i < num_bytes; i++) {
            pkcs1_total_len = (pkcs1_total_len << 8) | der[pos++];
        }
    }
    
    // Skip version INTEGER
    if (der[pos++] != 0x02) { return -1; }
    size_t vlen = der[pos++];
    pos += vlen;
    
    *n_len = 0;
    *d_len = 0;
    
    int idx = 0;
    size_t pkcs1_end = pkcs1_start + pkcs1_total_len + 2;
    while (pos < der_len && pos < pkcs1_end) {
        if (der[pos] != 0x02) { pos++; continue; }
        
        pos++;
        size_t len;
        if (der[pos] < 0x80) {
            len = der[pos++];
        } else if ((der[pos] & 0x7f) == 1) {
            pos++;
            len = der[pos++];
        } else if ((der[pos] & 0x7f) == 2) {
            pos++;
            len = (der[pos] << 8) | der[pos+1];
            pos += 2;
        } else {
            pos++;
            continue;
        }
        
        size_t orig_len = len;
        size_t copy_len = len;
        const uint8_t *data_ptr = der + pos;
        
        if (copy_len > 0 && data_ptr[0] == 0x00) {
            data_ptr++;
            copy_len--;
        }
        
        if (copy_len > 256) copy_len = 256;
        
        if (idx == 0 && *n_len == 0) {
            *n_len = copy_len;
            memcpy(n, data_ptr, copy_len);
        } else if (idx == 2 && *d_len == 0) {
            *d_len = copy_len;
            memcpy(d, data_ptr, copy_len);
        }
        
        pos += orig_len;
        idx++;
    }
    
    if (*n_len == 0 || *d_len == 0) {
        return -1;
    }
    
    return 0;
}

int ssl_socket_init(ssl_socket *sock) {
    memset(sock, 0, sizeof(ssl_socket));
    ssl_context_init(&sock->ctx);
    sock->fd = -1;
    sock->connected = 0;
    return 0;
}

int ssl_socket_bind(ssl_socket *sock, int port) {
    sock->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->fd < 0) return -1;
    
    int opt = 1;
    setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    if (bind(sock->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock->fd);
        sock->fd = -1;
        return -1;
    }
    
    if (listen(sock->fd, 128) < 0) {
        close(sock->fd);
        sock->fd = -1;
        return -1;
    }
    
    return 0;
}

static int build_server_hello(uint8_t *buf, size_t *len,
                              const uint8_t *server_random) {
    size_t pos = 0;
    
    buf[pos++] = 0x03;
    buf[pos++] = 0x03;
    memcpy(buf + pos, server_random, 32);
    pos += 32;
    
    buf[pos++] = 0x00;
    
    buf[pos++] = 0x00;
    buf[pos++] = 0x2f;
    
    buf[pos++] = 0x00;
    
    *len = pos;
    return 0;
}

static int build_certificate(uint8_t *buf, size_t *len, const uint8_t *cert_der, size_t cert_der_len) {
    size_t pos = 0;
    
    uint32_t total_len = cert_der_len + 3;
    buf[pos++] = (total_len >> 16) & 0xff;
    buf[pos++] = (total_len >> 8) & 0xff;
    buf[pos++] = total_len & 0xff;
    
    buf[pos++] = (cert_der_len >> 16) & 0xff;
    buf[pos++] = (cert_der_len >> 8) & 0xff;
    buf[pos++] = cert_der_len & 0xff;
    memcpy(buf + pos, cert_der, cert_der_len);
    pos += cert_der_len;
    
    *len = pos;
    return 0;
}

static int build_server_hello_done(uint8_t *buf, size_t *len) {
    *len = 0;
    return 0;
}

static int build_handshake_message(uint8_t *buf, size_t *len, uint8_t type, const uint8_t *data, size_t data_len) {
    size_t pos = 0;
    buf[pos++] = type;
    buf[pos++] = (data_len >> 16) & 0xff;
    buf[pos++] = (data_len >> 8) & 0xff;
    buf[pos++] = data_len & 0xff;
    memcpy(buf + pos, data, data_len);
    pos += data_len;
    *len = pos;
    return 0;
}

static int build_tls_record(uint8_t *buf, size_t *len, uint8_t content_type, const uint8_t *handshake_data, size_t handshake_len) {
    size_t pos = 0;
    buf[pos++] = content_type;
    buf[pos++] = 0x03;
    buf[pos++] = 0x03;
    buf[pos++] = (handshake_len >> 8) & 0xff;
    buf[pos++] = handshake_len & 0xff;
    memcpy(buf + pos, handshake_data, handshake_len);
    pos += handshake_len;
    *len = pos;
    return 0;
}

static int remove_pkcs1_padding(const uint8_t *input, size_t input_len, uint8_t *output, size_t *output_len) {
    if (input_len < 2) return -1;
    if (input[0] != 0x00) return -1;
    
    if (input[1] == 0x02) {
        size_t i = 2;
        while (i < input_len && input[i] != 0x00) i++;
        if (i >= input_len) return -1;
        
        *output_len = input_len - i - 1;
        memcpy(output, input + i + 1, *output_len);
        return 0;
    }
    
    return -1;
}

int ssl_socket_accept(ssl_socket *sock, ssl_socket *client, const char *cert_pem, const char *key_pem) {
    memset(client, 0, sizeof(ssl_socket));
    ssl_context_init(&client->ctx);
    client->ctx.is_server = 1;
    client->fd = -1;
    client->connected = 0;
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(sock->fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) return -1;
    
    client->fd = client_fd;
    
    FILE *fp = fopen(key_pem, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open key file: %s\n", key_pem);
        close(client->fd);
        return -1;
    }
    
    char key_pem_data[4096];
    size_t key_len = fread(key_pem_data, 1, sizeof(key_pem_data) - 1, fp);
    key_pem_data[key_len] = '\0';
    fclose(fp);
    
    uint8_t private_n[256], private_d[256];
    size_t private_n_len, private_d_len;
    if (parse_private_key_pem(key_pem_data, private_n, &private_n_len, private_d, &private_d_len) != 0) {
        fprintf(stderr, "Failed to parse private key\n");
        close(client->fd);
        return -1;
    }
    
    x509_cert cert;
    FILE *cf = fopen(cert_pem, "r");
    if (!cf) {
        fprintf(stderr, "Cannot open cert file: %s\n", cert_pem);
        close(client->fd);
        return -1;
    }
    char cert_pem_data[4096];
    size_t cert_len = fread(cert_pem_data, 1, sizeof(cert_pem_data) - 1, cf);
    cert_pem_data[cert_len] = '\0';
    fclose(cf);
    
    if (x509_parse_from_pem(cert_pem_data, &cert) != 0) {
        fprintf(stderr, "Failed to parse certificate\n");
        close(client->fd);
        return -1;
    }
    
    uint8_t recv_buf[16384];
    size_t recv_pos = 0;
    
    if (read_until(client->fd, recv_buf, &recv_pos, 5) != 0) {
        printf("DEBUG: Failed to read Record Header\n");
        goto handshake_err;
    }
    recv_pos = 5;
    
    if (recv_buf[0] != 0x16 || recv_buf[1] != 0x03) {
        printf("DEBUG: Not a TLS Handshake Record\n");
        goto handshake_err;
    }
    
    uint16_t incoming_record_len = (recv_buf[3] << 8) | recv_buf[4];
    if (incoming_record_len > sizeof(recv_buf) - 5) {
        printf("DEBUG: Record too large\n");
        goto handshake_err;
    }
    
    if (read_until(client->fd, recv_buf, &recv_pos, 5 + incoming_record_len) != 0) {
        printf("DEBUG: Failed to read Record Body\n");
        goto handshake_err;
    }
    
    if (recv_buf[5] != 0x01) {
        printf("DEBUG: Not a ClientHello\n");
        goto handshake_err;
    }
    
    // session ID length is at recv_buf + 43
    uint8_t session_id_len = recv_buf[43];
    size_t cipher_suites_offset = 44 + session_id_len;
    
    uint8_t client_random[32];
    memcpy(client_random, recv_buf + 11, 32);
    
    uint8_t handshake_log[8192];
    size_t hl_len = 0;
    
    memcpy(handshake_log + hl_len, recv_buf + 5, incoming_record_len);
    hl_len += incoming_record_len;
    
    uint8_t server_random[32];
    rand_bytes(server_random, 32);
    
    uint8_t server_hello[128];
    size_t server_hello_len = 0;
    build_server_hello(server_hello, &server_hello_len, server_random);
    
    uint8_t handshake_server_hello[128];
    size_t hs_len = 0;
    build_handshake_message(handshake_server_hello, &hs_len, 0x02, server_hello, server_hello_len);
    
    uint8_t cert_der[2048];
    size_t cert_der_len = pem_to_der(cert_pem_data, cert_der);
    if (cert_der_len == 0) {
        fprintf(stderr, "Failed to decode PEM to DER\n");
        goto handshake_err;
    }
    
    uint8_t cert_msg[2048];
    size_t cert_msg_len = 0;
    build_certificate(cert_msg, &cert_msg_len, cert_der, cert_der_len);
    
    uint8_t handshake_cert[2048];
    size_t hs_cert_len = 0;
    build_handshake_message(handshake_cert, &hs_cert_len, 0x0b, cert_msg, cert_msg_len);
    
    uint8_t server_hello_done[4];
    size_t shd_len = 0;
    build_server_hello_done(server_hello_done, &shd_len);
    
    uint8_t handshake_shd[4];
    size_t hs_shd_len = 0;
    build_handshake_message(handshake_shd, &hs_shd_len, 0x0e, server_hello_done, shd_len);
    
    memcpy(handshake_log + hl_len, handshake_server_hello, hs_len);
    hl_len += hs_len;
    memcpy(handshake_log + hl_len, handshake_cert, hs_cert_len);
    hl_len += hs_cert_len;
    memcpy(handshake_log + hl_len, handshake_shd, hs_shd_len);
    hl_len += hs_shd_len;
    
    uint8_t record_server_hello[2048];
    size_t record_len = 0;
    build_tls_record(record_server_hello, &record_len, 0x16, handshake_server_hello, hs_len);
    send_all(client->fd, record_server_hello, record_len);
    
    uint8_t record_cert[2048];
    record_len = 0;
    build_tls_record(record_cert, &record_len, 0x16, handshake_cert, hs_cert_len);
    send_all(client->fd, record_cert, record_len);
    
    uint8_t record_shd[256];
    record_len = 0;
    build_tls_record(record_shd, &record_len, 0x16, handshake_shd, hs_shd_len);
    send_all(client->fd, record_shd, record_len);
    
    recv_pos = 0;
    if (read_until(client->fd, recv_buf, &recv_pos, 5) != 0) goto handshake_err;
    
    uint16_t record_len2 = (recv_buf[3] << 8) | recv_buf[4];
    if (record_len2 > sizeof(recv_buf) - 5) goto handshake_err;
    if (read_until(client->fd, recv_buf, &recv_pos, 5 + record_len2) != 0) goto handshake_err;
    
    uint8_t *pRecordStart = recv_buf;
    size_t record_offset = 0;
    
    while (record_offset < recv_pos) {
        uint8_t content_type = pRecordStart[record_offset];
        uint16_t rec_len = (pRecordStart[record_offset + 3] << 8) | pRecordStart[record_offset + 4];
        
        if (content_type == 0x14) {
            record_offset += 5 + rec_len;
            continue;
        }
        
        if (content_type != 0x16) goto handshake_err;
        
        size_t pos = record_offset + 5;
        while (pos < record_offset + 5 + rec_len) {
            uint8_t msg_type = pRecordStart[pos];
            uint32_t msg_len = (pRecordStart[pos+1] << 16) | (pRecordStart[pos+2] << 8) | pRecordStart[pos+3];
            
            if (msg_type == 0x10) {
                uint16_t cipher_len = (pRecordStart[pos+4] << 8) | pRecordStart[pos+5];
                
                if (cipher_len > 256) cipher_len = 256;
                
                const uint8_t *cipher_data = pRecordStart + pos + 6;
                
                uint8_t decrypted[256];
                size_t decrypted_len;
                
                rsa_decrypt(private_n, private_n_len, private_d, private_d_len,
                           cipher_data, cipher_len, decrypted, &decrypted_len);
                
                if (decrypted_len < private_n_len) {
                    size_t pad_size = private_n_len - decrypted_len;
                    memmove(decrypted + pad_size, decrypted, decrypted_len);
                    memset(decrypted, 0, pad_size);
                    decrypted_len = private_n_len;
                }
                
                fprintf(stderr, "DEBUG: decrypted[0]=0x%02x, decrypted[1]=0x%02x, decrypted[2]=0x%02x, decrypted[3]=0x%02x\n",
                        decrypted[0], decrypted[1], decrypted[2], decrypted[3]);
                fprintf(stderr, "DEBUG: decrypted_len = %zu\n", decrypted_len);
                
                // Print first few bytes of decrypted
                fprintf(stderr, "DEBUG: decrypted first 20 bytes: ");
                for (size_t i = 0; i < 20 && i < decrypted_len; i++) {
                    fprintf(stderr, "%02x ", decrypted[i]);
                }
                fprintf(stderr, "\n");
                
                uint8_t pre_master_secret[48];
                size_t pms_len = 0;
                if (remove_pkcs1_padding(decrypted, decrypted_len, pre_master_secret, &pms_len) != 0) {
                    fprintf(stderr, "Failed to remove PKCS#1 padding\n");
                    goto handshake_err;
                }
                
                if (pms_len != 48) {
                    fprintf(stderr, "Invalid PMS length: %zu\n", pms_len);
                    goto handshake_err;
                }
                
                ssl_handshake_client(&client->ctx, cert_der, cert_der_len,
                                     client_random, 32, server_random, 32,
                                     pre_master_secret, 48);
                client->connected = 1;
                
                memcpy(handshake_log + hl_len, pRecordStart + pos, 4 + msg_len);
                hl_len += 4 + msg_len;
                
                break;
            }
            
            pos += 4 + msg_len;
        }
        break;
    }
    
    while(1) {
        uint8_t hdr[5];
        size_t rpos = 0;
        if (read_until(client->fd, hdr, &rpos, 5) != 0) goto handshake_err;
        uint16_t rlen = (hdr[3] << 8) | hdr[4];
        uint8_t *rec = (uint8_t *)malloc(rlen + 5);
        memcpy(rec, hdr, 5);
        rpos = 5;
        if (read_until(client->fd, rec, &rpos, 5 + rlen) != 0) { free(rec); goto handshake_err; }
        
        if (hdr[0] == 0x14) { 
            free(rec);
            continue;
        }
        if (hdr[0] == 0x16) { 
            uint8_t ctype;
            uint8_t ptext[TLS_MAX_MESSAGE_LEN];
            size_t plen;
            int dec_ret = ssl_decrypt_record(&client->ctx, rec, rpos, &ctype, ptext, &plen);
            if (dec_ret == 0) {
                if (ctype == 0x16 && ptext[0] == 0x14) {
                    memcpy(handshake_log + hl_len, ptext, plen);
                    hl_len += plen;
                    free(rec);
                    break;
                }
            }
        }
        free(rec);
    }
    
    uint8_t handshake_hash[32];
    sha256(handshake_log, hl_len, handshake_hash);
    uint8_t verify_data[12];
    tls_prf(client->ctx.master_secret, 48, "server finished", handshake_hash, 32, verify_data, 12);
    
    uint8_t server_finished_msg[16];
    server_finished_msg[0] = 0x14;
    server_finished_msg[1] = 0x00;
    server_finished_msg[2] = 0x00;
    server_finished_msg[3] = 0x0c;
    memcpy(server_finished_msg + 4, verify_data, 12);
    
    uint8_t change_cipher_spec[6] = {0x14, 0x03, 0x03, 0x00, 0x01, 0x01};
    send_all(client->fd, change_cipher_spec, 6);
    
    uint8_t finished_record[256];
    size_t finished_record_len;
    ssl_encrypt_record(&client->ctx, 0x16, server_finished_msg, 16, finished_record, &finished_record_len);
    send_all(client->fd, finished_record, finished_record_len);
    
    x509_free(&cert);
    return 0;

handshake_err:
    x509_free(&cert);
    close(client->fd);
    client->fd = -1;
    return -1;
}

int ssl_socket_read(ssl_socket *sock, uint8_t *buf, size_t len) {
    if (!sock->connected) {
        return recv(sock->fd, buf, len, 0);
    }

again:;
    uint8_t header[5];
    size_t pos = 0;
    if (read_until(sock->fd, header, &pos, 5) != 0) return -1;
    
    uint16_t record_len = (header[3] << 8) | header[4];
    if (record_len > TLS_MAX_MESSAGE_LEN) return -1;
    
    uint8_t *record = (uint8_t *)malloc(record_len + 5);
    memcpy(record, header, 5);
    size_t recv_pos = 5;
    if (read_until(sock->fd, record, &recv_pos, 5 + record_len) != 0) {
        free(record);
        return -1;
    }
    
    if (header[0] == 0x14) { // ChangeCipherSpec
        free(record);
        goto again;
    }
    
    uint8_t content_type;
    uint8_t plaintext[TLS_MAX_MESSAGE_LEN];
    size_t plain_len;
    
    int ret = ssl_decrypt_record(&sock->ctx, record, recv_pos, &content_type, plaintext, &plain_len);
    free(record);
    
    if (ret != 0) return -1;
    
    if (content_type == 0x16) { // Handshake (e.g. Finished)
        goto again;
    }
    
    if (content_type != 0x17) {
        return -1;
    }
    
    if (plain_len > len) plain_len = len;
    memcpy(buf, plaintext, plain_len);
    
    return plain_len;
}

int ssl_socket_write(ssl_socket *sock, const uint8_t *buf, size_t len) {
    if (!sock->connected) {
        return send(sock->fd, buf, len, 0);
    }
    
    uint8_t ciphertext[TLS_MAX_MESSAGE_LEN + 32];
    size_t cipher_len;
    
    if (ssl_encrypt_record(&sock->ctx, 0x17, buf, len, ciphertext, &cipher_len) != 0) {
        return -1;
    }
    
    return send_all(sock->fd, ciphertext, cipher_len);
}

void ssl_socket_close(ssl_socket *sock) {
    if (sock->fd >= 0) {
        close(sock->fd);
    }
    ssl_free(&sock->ctx);
    memset(sock, 0, sizeof(ssl_socket));
    sock->fd = -1;
}
