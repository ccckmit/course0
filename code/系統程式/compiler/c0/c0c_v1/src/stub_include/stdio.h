/* stub stdio.h for self-parse */
#ifndef _STUB_STDIO_H
#define _STUB_STDIO_H
#include <stdarg.h>
typedef struct _FILE FILE;
extern FILE *stdout;
extern FILE *stderr;
extern FILE *stdin;
int printf(const char *fmt, ...);
int fprintf(FILE *f, const char *fmt, ...);
int sprintf(char *s, const char *fmt, ...);
int snprintf(char *s, int n, const char *fmt, ...);
int vfprintf(FILE *f, const char *fmt, va_list ap);
int vprintf(const char *fmt, va_list ap);
int vsnprintf(char *s, int n, const char *fmt, va_list ap);
int puts(const char *s);
int fputs(const char *s, FILE *f);
int fputc(int c, FILE *f);
int fgetc(FILE *f);
char *fgets(char *s, int n, FILE *f);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *f);
int fseek(FILE *f, long off, int whence);
long ftell(FILE *f);
void rewind(FILE *f);
int ferror(FILE *f);
void perror(const char *s);
int sscanf(const char *s, const char *fmt, ...);
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
