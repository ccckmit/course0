/*
 * bignum.c – arbitrary-precision unsigned integer arithmetic
 *
 * Representation
 * --------------
 * words[] is little-endian: words[0] holds the 32 least-significant bits.
 * len is the number of *significant* words (len==0 means the value is zero).
 *
 * Signed values (needed internally for bn_ext_gcd) are represented as a
 * sign flag + magnitude bignum stored in a helper struct (see below).
 */

#include "bignum.h"

#include <assert.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                   */
/* ------------------------------------------------------------------ */

/* Remove leading zero words and keep len accurate. */
static void bn_trim(bignum *a)
{
    while (a->len > 0 && a->words[a->len - 1] == 0)
        a->len--;
}

/* Return 1 if a == 0. */
static int bn_is_zero(const bignum *a)
{
    return a->len == 0;
}

/* Left-shift a by one bit in place.  Returns the bit shifted out of MSB. */
static uint32_t bn_shl1(bignum *a)
{
    uint32_t carry = 0;
    for (size_t i = 0; i < a->len; i++) {
        uint32_t next = a->words[i] >> 31;
        a->words[i]   = (a->words[i] << 1) | carry;
        carry         = next;
    }
    if (carry && a->len < MAX_BIGNUM_WORDS) {
        a->words[a->len++] = carry;
        carry = 0;
    }
    return carry; /* non-zero on overflow */
}

/* Right-shift a by one bit in place (utility; used by callers). */
static void bn_shr1(bignum *a) __attribute__((unused));
static void bn_shr1(bignum *a)
{
    if (a->len == 0) return;
    for (size_t i = 0; i + 1 < a->len; i++)
        a->words[i] = (a->words[i] >> 1) | (a->words[i + 1] << 31);
    a->words[a->len - 1] >>= 1;
    bn_trim(a);
}

/* Test bit k (0 = LSB). */
static int bn_bit(const bignum *a, size_t k)
{
    size_t word = k / 32, bit = k % 32;
    if (word >= a->len) return 0;
    return (a->words[word] >> bit) & 1;
}

/* Number of bits needed to represent a (0 → 0). */
static size_t bn_bits(const bignum *a)
{
    if (a->len == 0) return 0;
    uint32_t top = a->words[a->len - 1];
    size_t   b   = (a->len - 1) * 32;
    while (top) { b++; top >>= 1; }
    return b;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

void bn_zero(bignum *a)
{
    memset(a->words, 0, sizeof(a->words));
    a->len = 0;
}

void bn_copy(bignum *dst, const bignum *src)
{
    memcpy(dst->words, src->words, src->len * sizeof(uint32_t));
    /* zero the rest so stale data can't sneak in */
    if (src->len < MAX_BIGNUM_WORDS)
        memset(dst->words + src->len, 0,
               (MAX_BIGNUM_WORDS - src->len) * sizeof(uint32_t));
    dst->len = src->len;
}

/*
 * bn_from_bytes – big-endian byte array → bignum
 * Leading zero bytes are accepted and ignored.
 */
void bn_from_bytes(bignum *a, const uint8_t *bytes, size_t len)
{
    bn_zero(a);
    /* skip leading zeros */
    while (len > 0 && *bytes == 0) { bytes++; len--; }
    if (len == 0) return;

    /* How many 32-bit words do we need? */
    size_t nwords = (len + 3) / 4;
    assert(nwords <= MAX_BIGNUM_WORDS);
    a->len = nwords;

    /* Fill words from the least-significant end. */
    for (size_t i = 0; i < len; i++) {
        size_t   byte_pos  = len - 1 - i;   /* distance from LSB */
        size_t   word_idx  = byte_pos / 4;
        size_t   shift     = (byte_pos % 4) * 8;
        a->words[word_idx] |= (uint32_t)bytes[i] << shift;
    }
}

/*
 * bn_to_bytes – bignum → big-endian byte array
 * *len is set to the number of bytes written (0 for zero).
 * The caller must ensure bytes[] is large enough (MAX_BIGNUM_WORDS*4 suffices).
 */
void bn_to_bytes(const bignum *a, uint8_t *bytes, size_t *len)
{
    if (a->len == 0) { *len = 0; return; }

    size_t total = a->len * 4;
    /* Big-endian: most-significant word first. */
    for (size_t i = 0; i < a->len; i++) {
        uint32_t w = a->words[a->len - 1 - i];
        bytes[i * 4 + 0] = (uint8_t)(w >> 24);
        bytes[i * 4 + 1] = (uint8_t)(w >> 16);
        bytes[i * 4 + 2] = (uint8_t)(w >>  8);
        bytes[i * 4 + 3] = (uint8_t)(w >>  0);
    }
    /* Strip leading zeros from the byte output. */
    size_t start = 0;
    while (start + 1 < total && bytes[start] == 0) start++;
    if (start > 0) memmove(bytes, bytes + start, total - start);
    *len = total - start;
}

/* -1 if a < b, 0 if equal, +1 if a > b */
int bn_cmp(const bignum *a, const bignum *b)
{
    if (a->len != b->len) return a->len < b->len ? -1 : 1;
    for (size_t i = a->len; i-- > 0;) {
        if (a->words[i] != b->words[i])
            return a->words[i] < b->words[i] ? -1 : 1;
    }
    return 0;
}

/* r = a + b  (r may alias a or b) */
void bn_add(bignum *r, const bignum *a, const bignum *b)
{
    bignum tmp;
    bn_zero(&tmp);
    uint64_t carry = 0;
    size_t   n     = a->len > b->len ? a->len : b->len;

    for (size_t i = 0; i < n || carry; i++) {
        assert(i < MAX_BIGNUM_WORDS);
        uint64_t sum = carry;
        if (i < a->len) sum += a->words[i];
        if (i < b->len) sum += b->words[i];
        tmp.words[i] = (uint32_t)(sum & 0xFFFFFFFF);
        carry = sum >> 32;
        if (i >= tmp.len) tmp.len = i + 1;
    }
    bn_trim(&tmp);
    bn_copy(r, &tmp);
}

/* r = a - b  (assumes a >= b; result is undefined otherwise) */
void bn_sub(bignum *r, const bignum *a, const bignum *b)
{
    bignum tmp;
    bn_zero(&tmp);
    int64_t borrow = 0;
    size_t  n      = a->len;

    for (size_t i = 0; i < n; i++) {
        int64_t diff = (int64_t)a->words[i] - borrow;
        if (i < b->len) diff -= b->words[i];
        if (diff < 0) { tmp.words[i] = (uint32_t)(diff + (int64_t)0x100000000LL); borrow = 1; }
        else          { tmp.words[i] = (uint32_t)diff; borrow = 0; }
    }
    tmp.len = n;
    bn_trim(&tmp);
    bn_copy(r, &tmp);
}

/* r = a * b */
void bn_mul(bignum *r, const bignum *a, const bignum *b)
{
    bignum tmp;
    bn_zero(&tmp);
    size_t n = a->len, m = b->len;

    for (size_t i = 0; i < n; i++) {
        uint64_t carry = 0;
        for (size_t j = 0; j < m || carry; j++) {
            assert(i + j < MAX_BIGNUM_WORDS);
            uint64_t cur = (uint64_t)tmp.words[i + j] + carry;
            if (j < m) cur += (uint64_t)a->words[i] * b->words[j];
            tmp.words[i + j] = (uint32_t)(cur & 0xFFFFFFFF);
            carry = cur >> 32;
            if (i + j >= tmp.len) tmp.len = i + j + 1;
        }
    }
    bn_trim(&tmp);
    bn_copy(r, &tmp);
}

/* (q, rem) = a / b   –– long division via binary shift-subtract */
void bn_div_mod(const bignum *a, const bignum *b, bignum *q, bignum *rem)
{
    assert(!bn_is_zero(b));

    bignum quotient, remainder;
    bn_zero(&quotient);
    bn_zero(&remainder);

    if (bn_cmp(a, b) < 0) {
        if (q)   bn_copy(q,   &quotient);
        if (rem) bn_copy(rem, a);
        return;
    }

    size_t bits = bn_bits(a);

    for (size_t i = bits; i-- > 0;) {
        /* remainder = remainder << 1 | bit i of a */
        bn_shl1(&remainder);
        if (bn_bit(a, i)) {
            remainder.words[0] |= 1;
            if (remainder.len == 0) remainder.len = 1;
        }
        if (bn_cmp(&remainder, b) >= 0) {
            bn_sub(&remainder, &remainder, b);
            /* set bit i of quotient */
            size_t wi = i / 32, bi2 = i % 32;
            quotient.words[wi] |= (1u << bi2);
            if (wi >= quotient.len) quotient.len = wi + 1;
        }
    }
    bn_trim(&quotient);
    if (q)   bn_copy(q,   &quotient);
    if (rem) bn_copy(rem, &remainder);
}

/* ------------------------------------------------------------------ */
/*  Signed bignum (for extended GCD)                                  */
/* ------------------------------------------------------------------ */

typedef struct {
    bignum mag; /* magnitude */
    int    neg; /* 0 = non-negative, 1 = negative */
} sbignum;

static void sbn_zero(sbignum *a) { bn_zero(&a->mag); a->neg = 0; }

static void sbn_from_bn(sbignum *a, const bignum *src, int neg)
{
    bn_copy(&a->mag, src);
    a->neg = bn_is_zero(src) ? 0 : neg;
}

/* dst = src */
static void sbn_copy(sbignum *dst, const sbignum *src)
{
    bn_copy(&dst->mag, &src->mag);
    dst->neg = src->neg;
}

/* r = a + b  (signed) */
static void sbn_add(sbignum *r, const sbignum *a, const sbignum *b)
{
    if (a->neg == b->neg) {
        bn_add(&r->mag, &a->mag, &b->mag);
        r->neg = a->neg;
    } else {
        int cmp = bn_cmp(&a->mag, &b->mag);
        if (cmp >= 0) { bn_sub(&r->mag, &a->mag, &b->mag); r->neg = a->neg; }
        else          { bn_sub(&r->mag, &b->mag, &a->mag); r->neg = b->neg; }
        if (bn_is_zero(&r->mag)) r->neg = 0;
    }
}

/* r = a - b  (signed) */
static void sbn_sub(sbignum *r, const sbignum *a, const sbignum *b)
{
    sbignum neg_b;
    sbn_copy(&neg_b, b);
    neg_b.neg = bn_is_zero(&b->mag) ? 0 : !b->neg;
    sbn_add(r, a, &neg_b);
}

/* r = a * b  (signed, magnitude only – used in ext_gcd quotient scaling) */
static void sbn_mul_bn(sbignum *r, const sbignum *a, const bignum *b)
{
    bn_mul(&r->mag, &a->mag, b);
    r->neg = (a->neg && !bn_is_zero(b)) ? 1 : 0;
    if (bn_is_zero(&r->mag)) r->neg = 0;
}

/*
 * bn_ext_gcd – Extended Euclidean Algorithm
 *
 * Computes gcd, x, y such that  a*x + b*y == gcd(a,b).
 * x and y are returned as magnitudes only (the function does not support
 * negative outputs via the bignum interface; callers that need signs should
 * use the internal sbignum layer directly).
 *
 * Note: for RSA key generation you typically only need gcd and one of x/y.
 */
void bn_ext_gcd(const bignum *a, const bignum *b,
                bignum *gcd, bignum *x, bignum *y)
{
    /* We use the iterative version to avoid stack issues with large inputs.
     *
     * Invariants:
     *   old_r = a * old_s + b * old_t
     *   r     = a * s     + b * t
     */
    sbignum old_s, s, old_t, t, tmp, q_signed;
    bignum  old_r, r, q, rem;

    bn_copy(&old_r, a);
    bn_copy(&r, b);

    sbn_zero(&old_s); old_s.mag.words[0] = 1; old_s.mag.len = 1; /* old_s = 1 */
    sbn_zero(&s);                                                   /* s     = 0 */
    sbn_zero(&old_t);                                               /* old_t = 0 */
    sbn_zero(&t); t.mag.words[0] = 1; t.mag.len = 1;               /* t     = 1 */

    while (!bn_is_zero(&r)) {
        bn_div_mod(&old_r, &r, &q, &rem);

        /* (old_r, r) = (r, old_r - q*r) */
        bn_copy(&old_r, &r);
        bn_copy(&r, &rem);

        /* s update: (old_s, s) = (s, old_s - q*s) */
        sbn_from_bn(&q_signed, &q, 0);
        sbn_mul_bn(&tmp, &s, &q);
        sbn_sub(&tmp, &old_s, &tmp); /* tmp = old_s - q*s */
        sbn_copy(&old_s, &s);
        sbn_copy(&s, &tmp);

        /* t update */
        sbn_mul_bn(&tmp, &t, &q);
        sbn_sub(&tmp, &old_t, &tmp);
        sbn_copy(&old_t, &t);
        sbn_copy(&t, &tmp);
    }

    if (gcd) bn_copy(gcd, &old_r);
    if (x)   bn_copy(x,   &old_s.mag);
    if (y)   bn_copy(y,   &old_t.mag);

    (void)q_signed; /* suppress unused warning */
}

/* r = (a * b) mod m */
void bn_mod_mul(bignum *r, const bignum *a, const bignum *b, const bignum *mod)
{
    bignum tmp;
    bn_mul(&tmp, a, b);
    bn_div_mod(&tmp, mod, NULL, r);
}

/* r = base^exp mod m  (square-and-multiply) */
void bn_mod_exp(bignum *r, const bignum *base, const bignum *exp,
                const bignum *mod)
{
    bignum result, b, tmp;
    bn_zero(&result);
    result.words[0] = 1; result.len = 1; /* result = 1 */

    bn_div_mod(base, mod, NULL, &b);     /* b = base mod m */

    size_t bits = bn_bits(exp);
    for (size_t i = 0; i < bits; i++) {
        if (bn_bit(exp, i)) {
            bn_mod_mul(&tmp, &result, &b, mod);
            bn_copy(&result, &tmp);
        }
        bn_mod_mul(&tmp, &b, &b, mod);
        bn_copy(&b, &tmp);
    }
    bn_copy(r, &result);
}
