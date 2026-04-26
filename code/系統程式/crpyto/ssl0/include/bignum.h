#ifndef BIGNUM_H
#define BIGNUM_H

#include <stddef.h>
#include <stdint.h>

#define MAX_BIGNUM_WORDS 130

typedef struct {
    uint32_t words[MAX_BIGNUM_WORDS]; /* little-endian: words[0] is least significant */
    size_t len;                        /* number of significant words */
} bignum;

void bn_zero(bignum *a);
void bn_copy(bignum *dst, const bignum *src);
void bn_from_bytes(bignum *a, const uint8_t *bytes, size_t len);
void bn_to_bytes(const bignum *a, uint8_t *bytes, size_t *len);
int  bn_cmp(const bignum *a, const bignum *b);
void bn_add(bignum *r, const bignum *a, const bignum *b);
void bn_sub(bignum *r, const bignum *a, const bignum *b);
void bn_mul(bignum *r, const bignum *a, const bignum *b);
void bn_div_mod(const bignum *a, const bignum *b, bignum *q, bignum *rem);
void bn_ext_gcd(const bignum *a, const bignum *b, bignum *gcd, bignum *x, bignum *y);
void bn_mod_mul(bignum *r, const bignum *a, const bignum *b, const bignum *mod);
void bn_mod_exp(bignum *r, const bignum *base, const bignum *exp, const bignum *mod);

#endif /* BIGNUM_H */
