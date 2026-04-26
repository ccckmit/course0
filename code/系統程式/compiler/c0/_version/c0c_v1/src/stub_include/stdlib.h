/* stub stdlib.h */
#ifndef _STUB_STDLIB_H
#define _STUB_STDLIB_H
typedef unsigned long size_t;
void *malloc(size_t n);
void *calloc(size_t n, size_t m);
void *realloc(void *p, size_t n);
void  free(void *p);
void  exit(int code);
long  strtol(const char *s, char **e, int base);
long long strtoll(const char *s, char **e, int base);
unsigned long strtoul(const char *s, char **e, int base);
double strtod(const char *s, char **e);
int   atoi(const char *s);
void *memcpy(void *d, const void *s, size_t n);
void *memmove(void *d, const void *s, size_t n);
void *memset(void *d, int c, size_t n);
size_t fread(void *p, size_t sz, size_t n, FILE *f);
size_t fwrite(const void *p, size_t sz, size_t n, FILE *f);
#endif
