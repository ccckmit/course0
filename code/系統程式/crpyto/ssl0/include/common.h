#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  Test infrastructure                                                */
/* ------------------------------------------------------------------ */

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg)                                          \
    do {                                                          \
        if (cond) { g_pass++; printf("  PASS  %s\n", msg); }    \
        else      { g_fail++; printf("  FAIL  %s  (line %d)\n", \
                                     msg, __LINE__); }           \
    } while (0)

#define PASS(msg)              CHECK(1, msg)
#define FAIL(msg)             CHECK(0, msg)

#endif /* COMMON_H */