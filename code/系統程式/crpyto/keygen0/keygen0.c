#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "../include/bignum.h"
#include "../include/rand.h"

#define KEY_BITS 2048
#define KEY_BYTES (KEY_BITS / 8)
#define MAX_KEY_BYTES 256
#define PUBLIC_EXPONENT {0x01, 0x00, 0x01, 0x00}

typedef struct {
    bignum n;
    bignum e;
} rsa_public_key;

typedef struct {
    rsa_public_key public_key;
    bignum d;
} rsa_private_key;

int is_prime(const bignum *n) {
    if (n->len == 0 || n->words[0] < 2) return 0;
    if (n->words[0] == 2) return 1;
    if (n->words[0] % 2 == 0) return 0;
    
    bignum three;
    bn_zero(&three);
    three.words[0] = 3;
    three.len = 1;
    
    for (uint32_t i = 3; i < 100; i += 2) {
        bignum divisor, rem;
        uint32_t d = i;
        divisor.words[0] = d;
        divisor.len = 1;
        
        bignum q, r;
        bn_div_mod(n, &divisor, &q, &r);
        
        if (r.len == 1 && r.words[0] == 0) {
            return 0;
        }
    }
    
    return 1;
}

void generate_prime(bignum *p, int bits) {
    while (1) {
        bn_zero(p);
        p->len = (bits + 31) / 32;
        
        for (size_t i = 0; i < p->len; i++) {
            p->words[i] = rand() ^ (rand() << 15);
        }
        
        if (p->len > 0) {
            p->words[0] |= 1;
        }
        
        size_t last_idx = (bits + 31) / 32 - 1;
        p->words[last_idx] |= (1ULL << (bits % 32));
        
        if (is_prime(p)) {
            return;
        }
    }
}

void generate_rsa_keypair(rsa_private_key *priv, rsa_public_key *pub, int bits) {
    bignum p, q, phi, tmp, one;
    
    printf("Generating %d-bit RSA keypair...\n", bits);
    printf("Generating prime p...\n");
    fflush(stdout);
    generate_prime(&p, bits / 2);
    
    printf("Generating prime q...\n");
    fflush(stdout);
    generate_prime(&q, bits / 2);
    
    printf("Computing modulus...\n");
    bn_mul(&priv->public_key.n, &p, &q);
    bn_copy(&pub->n, &priv->public_key.n);
    
    bn_sub(&tmp, &p, &one);
    tmp.words[0] = p.words[0] - 1;
    
    bignum tmp2;
    bn_sub(&tmp2, &q, &one);
    tmp2.words[0] = q.words[0] - 1;
    
    bn_mul(&phi, &tmp, &tmp2);
    
    bn_zero(&one);
    one.words[0] = 1;
    
    uint8_t e_bytes[] = PUBLIC_EXPONENT;
    bn_from_bytes(&pub->e, e_bytes, 4);
    bn_copy(&priv->public_key.e, &pub->e);
    
    printf("Computing private exponent...\n");
    bignum gcd, x, y;
    bn_ext_gcd(&pub->e, &phi, &gcd, &x, &y);
    
    if (x.words[0] & 0x80000000) {
        bn_add(&priv->d, &phi, &x);
    } else {
        bn_copy(&priv->d, &x);
    }
    
    printf("Keypair generated!\n");
}

void write_private_key(const char *filename, const rsa_private_key *priv) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open private key file");
        return;
    }
    
    uint8_t n_bytes[MAX_KEY_BYTES];
    uint8_t d_bytes[MAX_KEY_BYTES];
    size_t n_len, d_len;
    
    bn_to_bytes(&priv->public_key.n, n_bytes, &n_len);
    bn_to_bytes(&priv->d, d_bytes, &d_len);
    
    fprintf(fp, "----BEGIN RSA PRIVATE KEY----\n");
    fprintf(fp, "N:");
    for (size_t i = 0; i < n_len; i++) {
        fprintf(fp, "%02x", n_bytes[i]);
    }
    fprintf(fp, "\n");
    fprintf(fp, "D:");
    for (size_t i = 0; i < d_len; i++) {
        fprintf(fp, "%02x", d_bytes[i]);
    }
    fprintf(fp, "\n");
    fprintf(fp, "E:010001\n");
    fprintf(fp, "----END RSA PRIVATE KEY----\n");
    
    fclose(fp);
    printf("Private key written to %s\n", filename);
}

void write_public_key(const char *filename, const rsa_public_key *pub) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open public key file");
        return;
    }
    
    uint8_t n_bytes[MAX_KEY_BYTES];
    size_t n_len;
    
    bn_to_bytes(&pub->n, n_bytes, &n_len);
    
    fprintf(fp, "ssh-rsa ");
    for (size_t i = 0; i < n_len; i++) {
        fprintf(fp, "%02x", n_bytes[i]);
    }
    fprintf(fp, " user@localhost\n");
    
    fclose(fp);
    printf("Public key written to %s\n", filename);
}

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  -t <type>     Key type (rsa, dsa, ecdsa, ed25519) [default: rsa]\n");
    printf("  -b <bits>     Number of bits [default: %d]\n", KEY_BITS);
    printf("  -f <file>     Key file path [default: id_rsa]\n");
    printf("  -N <pass>     Passphrase\n");
    printf("  -P <pass>     Old passphrase\n");
    printf("  -C <comment>  Comment\n");
    printf("  -y            Read public key only\n");
    printf("  -h            Show this help\n");
}

int main(int argc, char *argv[]) {
    int bits = KEY_BITS;
    char *key_file = "id_rsa";
    char *comment = "user@localhost";
    int show_help = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            bits = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            key_file = argv[++i];
        } else if (strcmp(argv[i], "-C") == 0 && i + 1 < argc) {
            comment = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) {
            show_help = 1;
        }
    }
    
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    srand(time(NULL));
    
    rsa_private_key priv;
    rsa_public_key pub;
    
    bn_zero(&priv.public_key.n);
    bn_zero(&priv.public_key.e);
    bn_zero(&priv.d);
    bn_zero(&pub.n);
    bn_zero(&pub.e);
    
    generate_rsa_keypair(&priv, &pub, bits);
    
    char priv_file[256];
    char pub_file[256];
    
    sprintf(priv_file, "%s", key_file);
    sprintf(pub_file, "%s.pub", key_file);
    
    write_private_key(priv_file, &priv);
    write_public_key(pub_file, &pub);
    
    return 0;
}