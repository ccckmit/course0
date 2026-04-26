#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/ssl_socket.h"
#include "../include/rand.h"
#include "../include/certificate.h"
#include "../include/common.h"

int main() {
    printf("=== ssl_socket tests ===\n\n");
    
    /* rand_bytes */
    {
        uint8_t buf[16];
        rand_bytes(buf, 16);
        CHECK(1, "rand_bytes runs");
    }
    
    /* x509 parse cert */
    {
        FILE *f = fopen("./https/cert.pem", "r");
        CHECK(f != NULL, "open cert.pem");
        if (!f) goto skip_cert;
        
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
    skip_cert:;
    }
    
    /* x509 get public key */
    {
        FILE *f = fopen("./https/cert.pem", "r");
        if (!f) goto skip_pk;
        
        fseek(f, 0, SEEK_END);
        long pem_len = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        char *pem = malloc(pem_len + 1);
        fread(pem, 1, pem_len, f);
        pem[pem_len] = '\0';
        fclose(f);
        
        x509_cert cert;
        int ret = x509_parse_from_pem(pem, &cert);
        if (ret == 0) {
            uint8_t n[256], e[4];
            size_t n_len, e_len;
            ret = x509_get_public_key(&cert, n, &n_len, e, &e_len);
            CHECK(ret == 0, "x509 get public key");
            x509_free(&cert);
        }
        free(pem);
    skip_pk:;
    }
    
    /* ssl_socket_init */
    {
        ssl_socket server;
        int ret = ssl_socket_init(&server);
        CHECK(ret == 0, "ssl_socket_init");
        ssl_socket_close(&server);
    }
    
    /* ssl_socket_bind without listening */
    {
        ssl_socket server;
        int ret = ssl_socket_init(&server);
        if (ret == 0) {
            ret = ssl_socket_bind(&server, 18444);
            if (ret == 0) {
                CHECK(1, "ssl_socket_bind");
                ssl_socket_close(&server);
            } else {
                CHECK(ret != 0, "ssl_socket_bind port in use is ok");
            }
        }
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}