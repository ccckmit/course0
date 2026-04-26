/*
 * test_bignum.c – unit tests for bignum.c
 *
 * Compile:
 *   gcc -Wall -Wextra -g -fsanitize=address,undefined \
 *       test_bignum.c bignum.c -o test_bignum && ./test_bignum
 */

#define _POSIX_C_SOURCE 200809L  /* expose strdup */

#include "bignum.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Convert decimal string to bignum via repeated multiply-add. */
static void bn_from_dec(bignum *a, const char *s)
{
    bn_zero(a);
    bignum ten, tmp, digit;
    bn_zero(&ten);  ten.words[0]   = 10; ten.len   = 1;
    bn_zero(&digit);
    for (; *s; s++) {
        bn_mul(&tmp, a, &ten);
        bn_copy(a, &tmp);
        bn_zero(&digit); digit.words[0] = (uint32_t)(*s - '0'); digit.len = 1;
        bn_add(&tmp, a, &digit);
        bn_copy(a, &tmp);
    }
}

/* Convert bignum to decimal string (heap-allocated; caller frees). */
static char *bn_to_dec(const bignum *a)
{
    if (a->len == 0) { char *s = malloc(2); s[0]='0'; s[1]='\0'; return s; }

    /* Worst case: 32 * MAX_BIGNUM_WORDS bits ≈ 617 decimal digits */
    char buf[2048];
    int  pos = 0;

    bignum tmp, q, rem;
    bignum ten; bn_zero(&ten); ten.words[0] = 10; ten.len = 1;
    bn_copy(&tmp, a);

    while (!( tmp.len == 0 )) {
        bn_div_mod(&tmp, &ten, &q, &rem);
        buf[pos++] = (char)('0' + (rem.len ? rem.words[0] : 0));
        bn_copy(&tmp, &q);
    }
    buf[pos] = '\0';
    /* reverse */
    for (int i = 0, j = pos - 1; i < j; i++, j--) {
        char c = buf[i]; buf[i] = buf[j]; buf[j] = c;
    }
    return strdup(buf);
}

/* Check that a equals the decimal string s. */
static int bn_eq_dec(const bignum *a, const char *s)
{
    char *got = bn_to_dec(a);
    int   ok  = strcmp(got, s) == 0;
    if (!ok) printf("    expected %s, got %s\n", s, got);
    free(got);
    return ok;
}

/* ------------------------------------------------------------------ */
/*  Tests                                                              */
/* ------------------------------------------------------------------ */

static void test_zero_and_copy(void)
{
    printf("\n[zero / copy]\n");
    bignum a, b;
    bn_zero(&a);
    CHECK(a.len == 0, "bn_zero sets len=0");

    bn_from_dec(&a, "12345678901234567890");
    bn_copy(&b, &a);
    CHECK(bn_cmp(&a, &b) == 0, "bn_copy == original");

    bn_zero(&a);
    CHECK(a.len == 0, "bn_zero after copy");
    char *s = bn_to_dec(&b);
    CHECK(strcmp(s,"12345678901234567890")==0, "copy not affected by zeroing src");
    free(s);
}

static void test_from_to_bytes(void)
{
    printf("\n[from_bytes / to_bytes]\n");
    /* 0x0102030405060708090a0b0c */
    uint8_t raw[] = {0x01,0x02,0x03,0x04,0x05,0x06,
                     0x07,0x08,0x09,0x0a,0x0b,0x0c};
    bignum a;
    bn_from_bytes(&a, raw, sizeof(raw));

    uint8_t out[64];
    size_t  outlen;
    bn_to_bytes(&a, out, &outlen);

    CHECK(outlen == sizeof(raw), "round-trip length matches");
    CHECK(memcmp(raw, out, outlen) == 0, "round-trip bytes match");

    /* zero */
    uint8_t zero_byte = 0;
    bignum z;
    bn_from_bytes(&z, &zero_byte, 1);
    CHECK(z.len == 0, "from_bytes of 0x00 gives len=0");
}

static void test_cmp(void)
{
    printf("\n[cmp]\n");
    bignum a, b;
    bn_from_dec(&a, "100");
    bn_from_dec(&b, "200");
    CHECK(bn_cmp(&a, &b) < 0,  "100 < 200");
    CHECK(bn_cmp(&b, &a) > 0,  "200 > 100");
    bn_copy(&b, &a);
    CHECK(bn_cmp(&a, &b) == 0, "100 == 100");

    bignum z; bn_zero(&z);
    CHECK(bn_cmp(&z, &a) < 0,  "0 < 100");
    CHECK(bn_cmp(&a, &z) > 0,  "100 > 0");
    CHECK(bn_cmp(&z, &z) == 0, "0 == 0");
}

static void test_add(void)
{
    printf("\n[add]\n");
    bignum a, b, r;

    bn_from_dec(&a, "999999999999999999");
    bn_from_dec(&b, "1");
    bn_add(&r, &a, &b);
    CHECK(bn_eq_dec(&r, "1000000000000000000"), "999...9 + 1");

    bn_from_dec(&a, "18446744073709551615"); /* 2^64 - 1 */
    bn_from_dec(&b, "18446744073709551615");
    bn_add(&r, &a, &b);
    CHECK(bn_eq_dec(&r, "36893488147419103230"), "2*(2^64-1)");

    /* aliasing: r == a */
    bn_from_dec(&a, "500");
    bn_from_dec(&b, "300");
    bn_add(&a, &a, &b);
    CHECK(bn_eq_dec(&a, "800"), "add aliased result == a");
}

static void test_sub(void)
{
    printf("\n[sub]\n");
    bignum a, b, r;

    bn_from_dec(&a, "1000000000000000000");
    bn_from_dec(&b, "1");
    bn_sub(&r, &a, &b);
    CHECK(bn_eq_dec(&r, "999999999999999999"), "10^18 - 1");

    bn_from_dec(&a, "12345678901234567890");
    bn_copy(&b, &a);
    bn_sub(&r, &a, &b);
    CHECK(r.len == 0, "a - a == 0");
}

static void test_mul(void)
{
    printf("\n[mul]\n");
    bignum a, b, r;

    bn_from_dec(&a, "123456789");
    bn_from_dec(&b, "987654321");
    bn_mul(&r, &a, &b);
    CHECK(bn_eq_dec(&r, "121932631112635269"), "123456789 * 987654321");

    /* 2^64 * 2^64 = 2^128 */
    bn_from_dec(&a, "18446744073709551616"); /* 2^64 */
    bn_copy(&b, &a);
    bn_mul(&r, &a, &b);
    CHECK(bn_eq_dec(&r, "340282366920938463463374607431768211456"), "2^64 * 2^64 = 2^128");

    bignum z; bn_zero(&z);
    bn_mul(&r, &a, &z);
    CHECK(r.len == 0, "n * 0 == 0");
}

static void test_div_mod(void)
{
    printf("\n[div_mod]\n");
    bignum a, b, q, rem;

    bn_from_dec(&a, "1000000000000000000");
    bn_from_dec(&b, "7");
    bn_div_mod(&a, &b, &q, &rem);
    CHECK(bn_eq_dec(&q,   "142857142857142857"), "1e18 / 7 quotient");
    CHECK(bn_eq_dec(&rem, "1"),                  "1e18 % 7 remainder");

    /* a < b → q=0, rem=a */
    bn_from_dec(&a, "5");
    bn_from_dec(&b, "100");
    bn_div_mod(&a, &b, &q, &rem);
    CHECK(q.len == 0,             "5 / 100 q == 0");
    CHECK(bn_eq_dec(&rem, "5"),   "5 % 100 rem == 5");

    /* divisor == 1 */
    bn_from_dec(&a, "9999999999");
    bn_from_dec(&b, "1");
    bn_div_mod(&a, &b, &q, &rem);
    CHECK(bn_eq_dec(&q, "9999999999"), "n / 1 == n");
    CHECK(rem.len == 0,                "n % 1 == 0");
}

static void test_ext_gcd(void)
{
    printf("\n[ext_gcd]\n");
    bignum a, b, gcd, x, y;

    /* gcd(35, 15) = 5 */
    bn_from_dec(&a, "35");
    bn_from_dec(&b, "15");
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd, "5"), "gcd(35,15) == 5");

    /* gcd(1071, 462) = 21 */
    bn_from_dec(&a, "1071");
    bn_from_dec(&b, "462");
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd, "21"), "gcd(1071,462) == 21");

    /* gcd(a, 0) = a */
    bn_from_dec(&a, "42");
    bn_zero(&b);
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd, "42"), "gcd(42,0) == 42");

    /* coprime: gcd(17, 31) = 1 */
    bn_from_dec(&a, "17");
    bn_from_dec(&b, "31");
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd, "1"), "gcd(17,31) == 1 (coprime)");
}

static void test_mod_mul(void)
{
    printf("\n[mod_mul]\n");
    bignum a, b, mod, r;

    bn_from_dec(&a,   "123456789");
    bn_from_dec(&b,   "987654321");
    bn_from_dec(&mod, "1000000007");
    bn_mod_mul(&r, &a, &b, &mod);
    /* 121932631112635269 mod 1000000007 */
    bignum expected; bn_from_dec(&expected, "121932631112635269");
    bignum rem; bn_div_mod(&expected, &mod, NULL, &rem);
    CHECK(bn_cmp(&r, &rem) == 0, "(123456789 * 987654321) mod 1e9+7");
}

static void test_mod_exp(void)
{
    printf("\n[mod_exp]\n");
    bignum base, exp, mod, r;

    /* 2^10 mod 1000 = 24 */
    bn_from_dec(&base, "2");
    bn_from_dec(&exp,  "10");
    bn_from_dec(&mod,  "1000");
    bn_mod_exp(&r, &base, &exp, &mod);
    CHECK(bn_eq_dec(&r, "24"), "2^10 mod 1000 = 24");

    /* Fermat: 2^(p-1) ≡ 1 (mod p) for prime p=1000000007 */
    bn_from_dec(&base, "2");
    bn_from_dec(&exp,  "1000000006");
    bn_from_dec(&mod,  "1000000007");
    bn_mod_exp(&r, &base, &exp, &mod);
    CHECK(bn_eq_dec(&r, "1"), "Fermat: 2^(p-1) mod p == 1");

    /* base^0 = 1 */
    bn_from_dec(&base, "99999");
    bn_zero(&exp);
    bn_from_dec(&mod, "7");
    bn_mod_exp(&r, &base, &exp, &mod);
    CHECK(bn_eq_dec(&r, "1"), "n^0 mod m == 1");

    /* 0^n = 0 */
    bn_zero(&base);
    bn_from_dec(&exp, "5");
    bn_from_dec(&mod, "7");
    bn_mod_exp(&r, &base, &exp, &mod);
    CHECK(bn_eq_dec(&r, "0"), "0^n mod m == 0");

    /* RSA-like: encrypt then decrypt
     * p=61, q=53, n=p*q=3233, phi=(p-1)*(q-1)=3120
     * e=17, d=2753  (17*2753 mod 3120 = 1)
     * plaintext m=65, c = 65^17 mod 3233, then c^2753 mod 3233 = 65
     */
    bignum n, e, d, c, pt;
    bn_from_dec(&n,  "3233");
    bn_from_dec(&e,  "17");
    bn_from_dec(&d,  "2753");
    bn_from_dec(&pt, "65");
    bn_mod_exp(&c, &pt, &e, &n);  /* encrypt */
    bn_mod_exp(&r, &c,  &d, &n);  /* decrypt */
    CHECK(bn_eq_dec(&r, "65"), "RSA round-trip (p=61,q=53,e=17,d=2753,m=65)");
}

/* ------------------------------------------------------------------ */
/*  Large-number tests (vectors generated with Python)                */
/* ------------------------------------------------------------------ */

static void test_large_add(void)
{
    printf("\n[large add]\n");
    bignum a, b, r;

    /* (2^256-1) + (2^256-1) = 2^257 - 2 */
    bn_from_dec(&a,
        "115792089237316195423570985008687907853269984665640564039457584007913129639935");
    bn_copy(&b, &a);
    bn_add(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "231584178474632390847141970017375815706539969331281128078915168015826259279870"),
        "(2^256-1)*2");

    /* 2^511 + 2^511 = 2^512 */
    bn_from_dec(&a,
        "6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042048");
    bn_copy(&b, &a);
    bn_add(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096"),
        "2^511 + 2^511 = 2^512");
}

static void test_large_sub(void)
{
    printf("\n[large sub]\n");
    bignum a, b, r;

    /* 2^512 - 1 */
    bn_from_dec(&a,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096");
    bn_from_dec(&b, "1");
    bn_sub(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095"),
        "2^512 - 1");

    /* 2^512 - (2^256-1) */
    bn_from_dec(&b,
        "115792089237316195423570985008687907853269984665640564039457584007913129639935");
    bn_sub(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073431184712636981971479856705023170278632780869088242247907112362425735876444161"),
        "2^512 - (2^256-1)");
}

static void test_large_mul(void)
{
    printf("\n[large mul]\n");
    bignum a, b, r;

    /* 10^38 * 10^38 = 10^76 */
    bn_from_dec(&a, "100000000000000000000000000000000000000");  /* 10^38 */
    bn_copy(&b, &a);
    bn_mul(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "10000000000000000000000000000000000000000000000000000000000000000000000000000"),
        "10^38 * 10^38 = 10^76");

    /* (2^256-1)^2  */
    bn_from_dec(&a,
        "115792089237316195423570985008687907853269984665640564039457584007913129639935");
    bn_copy(&b, &a);
    bn_mul(&r, &a, &b);
    CHECK(bn_eq_dec(&r,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073315392623399665776056285720014482370779510884422601683867654778417822746804225"),
        "(2^256-1)^2");
}

static void test_large_div_mod(void)
{
    printf("\n[large div_mod]\n");
    bignum a, b, q, rem;

    /* 2^512 / (10^30 + 37) */
    bn_from_dec(&a,
        "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096");
    bn_from_dec(&b, "1000000000000000000000000000037");
    bn_div_mod(&a, &b, &q, &rem);
    CHECK(bn_eq_dec(&q,
        "13407807929942597099574024997709757234071489727908154452808300426061118910140945262047967182402642028014816883490711265104856"),
        "2^512 / (10^30+37) quotient");
    CHECK(bn_eq_dec(&rem,
        "127775398345257277332197204424"),
        "2^512 % (10^30+37) remainder");

    /* Verify: q*b + rem == a */
    bignum check, tmp;
    bn_mul(&check, &q, &b);
    bn_add(&tmp, &check, &rem);
    CHECK(bn_cmp(&tmp, &a) == 0, "verify: q*b + rem == a");
}

static void test_large_gcd(void)
{
    printf("\n[large gcd]\n");
    bignum a, b, gcd, x, y;

    /* gcd(42*2^256, 70*2^256) = 14*2^256 */
    bn_from_dec(&a,
        "4863267747967280207789981370364892129837339355956903689657218528332351444877312");
    bn_from_dec(&b,
        "8105446246612133679649968950608153549728898926594839482762030880553919074795520");
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd,
        "1621089249322426735929993790121630709945779785318967896552406176110783814959104"),
        "gcd(42*2^256, 70*2^256) = 14*2^256");

    /* gcd(2^256-1, 2^128-1) = 2^128-1  (since 128|256) */
    bn_from_dec(&a,
        "115792089237316195423570985008687907853269984665640564039457584007913129639935");
    bn_from_dec(&b,
        "340282366920938463463374607431768211455");  /* 2^128-1 */
    bn_ext_gcd(&a, &b, &gcd, &x, &y);
    CHECK(bn_eq_dec(&gcd,
        "340282366920938463463374607431768211455"),
        "gcd(2^256-1, 2^128-1) = 2^128-1");
}

static void test_large_mod_exp(void)
{
    printf("\n[large mod_exp]\n");
    bignum base, exp_v, mod, r;

    /* 3^(2^127-1) mod (10^30+37) -- very large exponent */
    bn_from_dec(&base,  "3");
    bn_from_dec(&exp_v, "170141183460469231731687303715884105727"); /* 2^127-1 */
    bn_from_dec(&mod,   "1000000000000000000000000000037");
    bn_mod_exp(&r, &base, &exp_v, &mod);
    CHECK(bn_eq_dec(&r, "976726003180054558614199858433"),
        "3^(2^127-1) mod (10^30+37)");

    /* RSA with Mersenne primes: p=2^61-1, q=2^31-1
     * n = 4951760154835678088235319297
     * e = 65537, d = 4181459486416926817284415673
     * plain = 123456789
     */
    bignum n, e, d, c, pt;
    bn_from_dec(&n,  "4951760154835678088235319297");
    bn_from_dec(&e,  "65537");
    bn_from_dec(&d,  "4181459486416926817284415673");
    bn_from_dec(&pt, "123456789");
    bn_mod_exp(&c, &pt, &e, &n);
    CHECK(bn_eq_dec(&c, "2011207219826418609349770464"),
        "RSA Mersenne: encrypt");
    bn_mod_exp(&r, &c, &d, &n);
    CHECK(bn_eq_dec(&r, "123456789"),
        "RSA Mersenne: decrypt round-trip");

    /* Fermat: a^(p-1) ≡ 1 (mod p), p = 2^61-1 (Mersenne prime) */
    bn_from_dec(&base,  "31415926535897932384");
    bn_from_dec(&mod,   "2305843009213693951");   /* 2^61-1 */
    bn_from_dec(&exp_v, "2305843009213693950");   /* p-1 */
    bn_mod_exp(&r, &base, &exp_v, &mod);
    CHECK(bn_eq_dec(&r, "1"),
        "Fermat: a^(p-1) mod p == 1  (p=2^61-1)");
}

/* ------------------------------------------------------------------ */
/*  Main                                                               */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("=== bignum tests ===\n");

    printf("\n--- basic ---\n");
    test_zero_and_copy();
    test_from_to_bytes();
    test_cmp();
    test_add();
    test_sub();
    test_mul();
    test_div_mod();
    test_ext_gcd();
    test_mod_mul();
    test_mod_exp();

    printf("\n--- large numbers ---\n");
    test_large_add();
    test_large_sub();
    test_large_mul();
    test_large_div_mod();
    test_large_gcd();
    test_large_mod_exp();

    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
