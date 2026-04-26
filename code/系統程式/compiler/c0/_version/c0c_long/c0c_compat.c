/*
 * c0c_compat.c — compiled with the system C compiler.
 * Provides: __c0c_emit, __c0c_stderr/stdout/stdin, __c0c_get_tbuf.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* The central emit function */
#define EMIT_BUF 8192
static char compat_emit_buf[EMIT_BUF];

void __c0c_emit(FILE *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(compat_emit_buf, EMIT_BUF, fmt, ap);
    va_end(ap);
    if (n > 0)
        fwrite(compat_emit_buf, 1, n < EMIT_BUF ? (size_t)n : EMIT_BUF-1, out);
}

/* stderr/stdout/stdin */
FILE *__c0c_stderr(void) { return stderr; }
FILE *__c0c_stdout(void) { return stdout; }
FILE *__c0c_stdin(void)  { return stdin;  }

/* Type string rotation buffers — 8 slots of 256 bytes each.
   llvm_type() in codegen.c uses these to return temporary type strings. */
static char tbuf_storage[8][256];

char *__c0c_get_tbuf(int i) {
    return tbuf_storage[((unsigned)i) % 8];
}

/* Standard typedef name/kind table for parser_new().
   Static arrays with initializers compile to null in c0c IR,
   so we provide accessor functions from native code. */
static const char *td_names_table[] = {
    "size_t","ssize_t","ptrdiff_t","intptr_t","uintptr_t","off_t",
    "int8_t","uint8_t","int16_t","uint16_t","int32_t","uint32_t",
    "int64_t","uint64_t",
    "int_least8_t","uint_least8_t","int_least16_t","uint_least16_t",
    "int_least32_t","uint_least32_t","int_least64_t","uint_least64_t",
    "int_fast8_t","uint_fast8_t","int_fast16_t","uint_fast16_t",
    "int_fast32_t","uint_fast32_t","int_fast64_t","uint_fast64_t",
    "intmax_t","uintmax_t","wchar_t","wint_t",
    "FILE","va_list","__gnuc_va_list",
    "pid_t","uid_t","gid_t","mode_t","dev_t","ino_t",
    "nlink_t","time_t","clock_t","suseconds_t","socklen_t",
    "bool","_Bool",
    NULL
};
/* TypeKind values matching enum in ast.h — must stay in sync */
enum __c0c_TypeKind {
    __TY_VOID=0,__TY_BOOL,__TY_CHAR,__TY_SCHAR,__TY_UCHAR,
    __TY_SHORT,__TY_USHORT,__TY_INT,__TY_UINT,
    __TY_LONG,__TY_ULONG,__TY_LONGLONG,__TY_ULONGLONG,
    __TY_FLOAT,__TY_DOUBLE,__TY_PTR,__TY_ARRAY,__TY_FUNC,
    __TY_STRUCT,__TY_UNION,__TY_ENUM,__TY_TYPEDEF_REF
};
static int td_kinds_table[] = {
    11/*ULONG*/,10/*LONG*/,10/*LONG*/,10/*LONG*/,11/*ULONG*/,10/*LONG*/,
    3/*SCHAR*/,4/*UCHAR*/,5/*SHORT*/,6/*USHORT*/,7/*INT*/,8/*UINT*/,
    10/*LONG*/,11/*ULONG*/,
    3/*SCHAR*/,4/*UCHAR*/,5/*SHORT*/,6/*USHORT*/,
    7/*INT*/,8/*UINT*/,10/*LONG*/,11/*ULONG*/,
    3/*SCHAR*/,4/*UCHAR*/,10/*LONG*/,11/*ULONG*/,
    10/*LONG*/,11/*ULONG*/,10/*LONG*/,11/*ULONG*/,
    10/*LONG*/,11/*ULONG*/,7/*INT*/,8/*UINT*/,
    18/*STRUCT*/,15/*PTR*/,15/*PTR*/,
    7/*INT*/,8/*UINT*/,8/*UINT*/,8/*UINT*/,11/*ULONG*/,11/*ULONG*/,
    11/*ULONG*/,10/*LONG*/,10/*LONG*/,10/*LONG*/,8/*UINT*/,
    7/*INT*/,7/*INT*/,
    0
};

const char *__c0c_get_td_name(long long i) {
    return td_names_table[i];
}
long long __c0c_get_td_kind(long long i) {
    return td_kinds_table[i];
}

/* Assign operator table for parse_assign() */
static int assign_ops_table[] = {
    61/*TOK_ASSIGN*/,
    43/*TOK_PLUS_ASSIGN*/, 45/*TOK_MINUS_ASSIGN*/,
    42/*TOK_STAR_ASSIGN*/, 47/*TOK_SLASH_ASSIGN*/,
    37/*TOK_PERCENT_ASSIGN*/, 38/*TOK_AMP_ASSIGN*/,
    124/*TOK_PIPE_ASSIGN*/, 94/*TOK_CARET_ASSIGN*/,
    60/*TOK_LSHIFT_ASSIGN*/, 62/*TOK_RSHIFT_ASSIGN*/,
    0/*TOK_EOF*/
};

int __c0c_get_assign_op(int i) {
    return assign_ops_table[i];
}
