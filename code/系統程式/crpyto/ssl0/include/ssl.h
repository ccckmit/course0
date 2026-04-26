#ifndef SSL_H
#define SSL_H

#include <stddef.h>
#include <stdint.h>

#define SSL_MAX_RECORD_LEN 16384
#define SSL_MAX_PLAINTEXT_LEN 16384

#define SSL_CONTENT_TYPE_CHANGE_CIPHER_SPEC 20
#define SSL_CONTENT_TYPE_ALERT 21
#define SSL_CONTENT_TYPE_HANDSHAKE 22
#define SSL_CONTENT_TYPE_APPLICATION_DATA 23

#define SSL_HANDSHAKE_TYPE_CLIENT_HELLO 1
#define SSL_HANDSHAKE_TYPE_SERVER_HELLO 2
#define SSL_HANDSHAKE_TYPE_CERTIFICATE 11
#define SSL_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE 12
#define SSL_HANDSHAKE_TYPE_CERTIFICATE_REQUEST 13
#define SSL_HANDSHAKE_TYPE_SERVER_HELLO_DONE 14
#define SSL_HANDSHAKE_TYPE_CERTIFICATE_VERIFY 15
#define SSL_HANDSHAKE_TYPE_CLIENT_KEY_EXCHANGE 16
#define SSL_HANDSHAKE_TYPE_FINISHED 20

typedef struct {
    uint8_t type;
    uint16_t version;
    uint16_t length;
    uint8_t fragment[SSL_MAX_RECORD_LEN];
} ssl_record;

typedef struct {
    uint8_t handshake_type;
    uint32_t length;
    uint8_t data[SSL_MAX_RECORD_LEN];
} ssl_handshake;

typedef struct {
    uint8_t major;
    uint8_t minor;
} ssl_version;

typedef struct {
    uint8_t master_secret[48];
    uint8_t client_write_mac_key[32];
    uint8_t server_write_mac_key[32];
    uint8_t client_write_key[16];
    uint8_t server_write_key[16];
    uint64_t client_seq_num;
    uint64_t server_seq_num;
    int has_keys;
    int is_server;
} ssl_context;

void ssl_context_init(ssl_context *ctx);
void ssl_free(ssl_context *ctx);

int ssl_handshake_client(ssl_context *ctx,
                         const uint8_t *server_cert, size_t server_cert_len,
                         const uint8_t *client_random, size_t client_random_len,
                         const uint8_t *server_random, size_t server_random_len,
                         const uint8_t *pre_master_secret, size_t pms_len);

int ssl_encrypt_record(ssl_context *ctx,
                       uint8_t content_type,
                       const uint8_t *plaintext, size_t plain_len,
                       uint8_t *ciphertext, size_t *cipher_len);

int ssl_decrypt_record(ssl_context *ctx,
                       const uint8_t *ciphertext, size_t cipher_len,
                       uint8_t *content_type,
                       uint8_t *plaintext, size_t *plain_len);

int ssl_compute_master_secret(const uint8_t *pre_master_secret, size_t pms_len,
                               const uint8_t *client_random, size_t cr_len,
                               const uint8_t *server_random, size_t sr_len,
                               uint8_t *master_secret);

int ssl_derive_keys(ssl_context *ctx, const uint8_t *master_secret,
                    const uint8_t *client_random, size_t cr_len,
                    const uint8_t *server_random, size_t sr_len);

#endif